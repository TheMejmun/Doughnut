//
// Created by Saman on 11.10.23.
//

#ifndef DOUGHNUT_SCHEDULER_H
#define DOUGHNUT_SCHEDULER_H

#include <cstdint>
#include <queue>
#include <mutex>
#include <thread>

namespace Doughnut {
    class Scheduler {
    public:
        Scheduler();

        explicit Scheduler(uint32_t workerCount);

        ~Scheduler();

        bool done();

        void await();

        void queue(std::initializer_list<std::function<void()>> functions);

    private:
        std::queue<std::function<void()>> mQueue{};
        std::mutex mQueueMutex{};
        std::vector<std::thread> mThreads{};

        std::atomic<uint32_t> mWaitingJobCount = 0;
        std::atomic<bool> mExitThreads = false;
        std::mutex mRunMutex{};
        std::condition_variable mRunCondition{};
    };
}

#endif //DOUGHNUT_SCHEDULER_H
