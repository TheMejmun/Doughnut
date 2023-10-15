//
// Created by Saman on 11.10.23.
//

#ifndef DOUGHNUT_SCHEDULER_H
#define DOUGHNUT_SCHEDULER_H

#include <cstdint>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>

namespace Doughnut {
    class Scheduler {
    public:
        /**
         * Init with one worker per available core
         */
        Scheduler();

        /**
         * Init with specified number of workers
         * @param workerCount Number of workers to create
         */
        explicit Scheduler(uint32_t workerCount);

        ~Scheduler();

        /**
         * Check whether any jobs are currently waiting
         * @return True, if all jobs are finished
         */
        bool done();

        /**
         * Block thread until all jobs are finished
         */
        void await();

        /**
         * Insert jobs into queue and execute
         * @param functions List of jobs to perform
         */
        void queue(std::initializer_list<std::function<void()>> functions);

        uint32_t activeWorkerCount();

        uint32_t workerCount();

    private:
        std::queue<std::function<void()>> mQueue{};
        std::mutex mQueueMutex{};
        std::atomic<uint32_t> mWaitingJobCount = 0;

        std::vector<std::thread> mThreads{};
        std::atomic<bool> mExitThreads = false;
        std::mutex mRunMutex{};
        std::condition_variable mRunCondition{};
    };

    void testScheduler();
}

#endif //DOUGHNUT_SCHEDULER_H
