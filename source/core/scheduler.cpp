//
// Created by Saman on 11.10.23.
//

#include "core/scheduler.h"
#include "io/printer.h"

#include <iostream>

using namespace Doughnut;

static inline const char *bool_to_string(bool b) {
    return b ? "true" : "false";
}

static void threadBody(
        std::queue<std::function<void()>> *queue,
        std::mutex *queueMutex,
        std::atomic<uint32_t> *waitingJobCount,
        std::atomic<bool> *exit,
        std::mutex *runMutex,
        std::condition_variable *runCondition
) {
    while (!*exit) {
        verbose(std::this_thread::get_id() << " Going to sleep.");
        std::unique_lock<std::mutex> runLock(*runMutex);
        runCondition->wait(runLock);
//        runCondition->wait_for(runLock, std::chrono::milliseconds(250)); // Worst case exit wait after 1/4 second
        runLock.unlock(); // TODO this unlock could theoretically throw an exception if not locked & the condition sporadically unlocks (I think)

        while (true) {
            std::function<void()> job;

            {
                std::lock_guard<std::mutex> queueLock(*queueMutex);
                if (queue->empty()) {
                    break;
                } else {
                    job = queue->front();
                    queue->pop();
                    verbose(std::this_thread::get_id() << " Took a job.");
                }
            }

            job();
            --(*waitingJobCount);
        }
    }
    verbose(std::this_thread::get_id() << " Exiting thread.");
}

Scheduler::Scheduler() {
    const auto processor_count = std::thread::hardware_concurrency();
    mThreads.reserve(processor_count);
    for (size_t i = 0; i < mThreads.capacity(); ++i) {
        mThreads.emplace_back(threadBody,
                              &mQueue, &mQueueMutex, &mWaitingJobCount, &mExitThreads, &mRunMutex, &mRunCondition);
    }
}

Scheduler::Scheduler(uint32_t workerCount) {
    mThreads.reserve(workerCount);
    for (size_t i = 0; i < mThreads.capacity(); ++i) {
        mThreads.emplace_back(threadBody,
                              &mQueue, &mQueueMutex, &mWaitingJobCount, &mExitThreads, &mRunMutex, &mRunCondition);
    }
}

void Scheduler::await() {
    while (true) {
        if (mWaitingJobCount == 0)
            return;
    }
}

bool Scheduler::done() {
    return mWaitingJobCount == 0;
}

void Scheduler::queue(std::initializer_list<std::function<void()>> functions) {
    std::lock_guard<std::mutex> guard(mQueueMutex);
    for (auto &job: functions) {
        ++mWaitingJobCount;
        mQueue.emplace(job);
        mRunCondition.notify_one();
    }
//    mRunCondition.notify_all();
}

uint32_t Scheduler::activeWorkerCount() {
    std::lock_guard<std::mutex> guard(mQueueMutex);
    return mWaitingJobCount - mQueue.size();
}

uint32_t Scheduler::workerCount() {
    return mThreads.size();
}

Scheduler::~Scheduler() {
    await();
    mExitThreads = true;
    // This may clear the scheduler before all jobs are completed.
    mRunCondition.notify_all();
    for (auto &thread: mThreads) {
        if (thread.joinable())
            thread.join();
    }
}

void Doughnut::testScheduler() {
    bool task1Done = false;
    bool task2Done = false;
    bool task3Done = false;

    Doughnut::Scheduler scheduler{2};

    assert(scheduler.done());

    scheduler.queue({
                            [&]() {
                                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                task1Done = true;
                            },
                            [&]() {
                                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                task2Done = true;
                            },
                            [&]() {
                                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                // Do nothing
                            }
                    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(scheduler.activeWorkerCount() == 2);
    assert(!scheduler.done());

    scheduler.await();

    assert(scheduler.done());
    assert(task1Done);
    assert(task2Done);

    scheduler.queue({
                            [&]() {
                                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                task3Done = true;
                            }
                    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(scheduler.activeWorkerCount() == 1);

    scheduler.await();

    assert(scheduler.done());
    assert(task3Done);

    std::cout << "Scheduler test successful." << std::endl;
}