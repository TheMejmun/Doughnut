//
// Created by Sam on 2024-03-23.
//

#include "test_util.h"
#include "core/scheduler.h"

#include <atomic>
#include <mutex>

REGISTER(Scheduler) {
    TEST(givenNewScheduler_whenCallingDone_thenTrue) {
        dn::Scheduler scheduler{2};
        expect(scheduler.done(), "New Scheduler should be done");
    };

    TEST(testSchedulerFlow) {
        bool task1Done = false;
        bool task2Done = false;
        bool task3Done = false;
        bool task4Done = false;

        dn::Scheduler scheduler{2};

        std::atomic<uint32_t> runningThreads = 0;
        std::mutex m1{};
        std::mutex m2{};
        m1.lock();
        m2.lock();
        scheduler.queue({
                                [&]() {
                                    ++runningThreads;
                                    std::lock_guard<std::mutex> guard(m1);
                                    task1Done = true;
                                    --runningThreads;
                                },
                                [&]() {
                                    ++runningThreads;
                                    std::lock_guard<std::mutex> guard(m2);
                                    task2Done = true;
                                    --runningThreads;
                                },
                                [&]() {
                                    ++runningThreads;
                                    task3Done = true;
                                    --runningThreads;
                                }
                        });

        expect(!scheduler.done(), "Scheduler with waiting jobs should not be done");

        while (runningThreads != 2) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        expect(scheduler.activeWorkerCount() == 2, "Scheduler with 2 threads and 2 active jobs should have 2 active workers");
        expect(!task1Done, "Task 1 should not be done");
        expect(!task2Done, "Task 2 should not be done");
        expect(!task3Done, "Task 3 should not be done as the scheduler only has 2 workers");

        m1.unlock();
        m2.unlock();
        scheduler.await();

        expect(task1Done, "Task 1 should be done after unlocking mutex");
        expect(task2Done, "Task 2 should be done after unlocking mutex");
        expect(task3Done, "Task 3 should be done");
        expect(scheduler.done(), "Scheduler should be done");

        m1.lock();
        scheduler.queue({
                                [&]() {
                                    ++runningThreads;
                                    std::lock_guard<std::mutex> guard(m1);
                                    task4Done = true;
                                    --runningThreads;
                                }
                        });

        expect(!scheduler.done(), "Scheduler with new jobs should not be done");

        while (runningThreads != 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        expect(scheduler.activeWorkerCount() == 1, "Scheduler should have one worker processing the new job");

        m1.unlock();
        scheduler.await();

        expect(task4Done, "Task 3 should be done after unlocking mutex");
        expect(scheduler.done(), "Scheduler should be done");
    };
}
