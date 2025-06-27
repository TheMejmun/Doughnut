//
// Created by Saman on 11.10.23.
//

#include "core/scheduler.h"
#include "io/logger.h"
#include "util/timer.h"

#include <iostream>
#include <cassert>

using namespace dn;
using namespace std::literals::chrono_literals;

static void threadBody(
        std::queue<std::function<void()>> *queue,
        std::mutex *queueMutex,
        std::atomic<uint32_t> *waitingJobCount,
        const bool *exit,
        std::mutex *runMutex,
        std::condition_variable *runCondition
) {
    while (!(*exit)) {
        while (true) {
            std::function < void() > job;

            {
                std::lock_guard<std::mutex> queueLock(*queueMutex);
                if (queue->empty()) {
                    break;
                } else {
                    job = queue->front();
                    queue->pop();
                }
            }

            job();
            --(*waitingJobCount);
        }

        // Will be unlocked when it goes out of scope
        std::unique_lock<std::mutex> runLock(*runMutex);
        runCondition->wait(runLock,
                           [=]() {
                               return (*exit) || (!queue->empty());
                           }
        );
    }
}

Scheduler::Scheduler() {
    const auto processorCount = std::thread::hardware_concurrency();
    mThreads.reserve(processorCount / 2);
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
        if (mWaitingJobCount == 0) {
            return;
        }
    }
}

bool Scheduler::done() {
    return mWaitingJobCount == 0;
}

void Scheduler::queue(const std::initializer_list<std::function<void()>> &functions) {
    std::lock_guard<std::mutex> runGuard{mRunMutex};
    std::lock_guard<std::mutex> queueGuard(mQueueMutex);
    for (auto &job: functions) {
        ++mWaitingJobCount;
        mQueue.emplace(job);
        mRunCondition.notify_one();
    }
}

void Scheduler::queue(const std::function<void()> &function) {
    std::lock_guard<std::mutex> runGuard{mRunMutex};
    std::lock_guard<std::mutex> queueGuard(mQueueMutex);
    ++mWaitingJobCount;
    mQueue.emplace(function);
    mRunCondition.notify_one();
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
    {
        // https://www.modernescpp.com/index.php/c-core-guidelines-be-aware-of-the-traps-of-condition-variables/
        // Must acquire lock before notifying in order to avoid deadlocks
        std::lock_guard<std::mutex> runGuard{mRunMutex};
        mExitThreads = true;
        mRunCondition.notify_all();
    }
    for (auto &thread: mThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}
