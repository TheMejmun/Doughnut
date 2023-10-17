//
// Created by Sam on 2023-10-15.
//

#ifndef DOUGHNUT_THREAD_SAFE_H
#define DOUGHNUT_THREAD_SAFE_H

#include <mutex>

namespace Doughnut {
    template<class T>
    class ThreadSafe {
    public:
        ThreadSafe() {
            mObject = {};
        }

        ThreadSafe(T object) : mObject(object) {}

        void set(T object) {
            if (!mIsLocked) {
                std::lock_guard<std::mutex> guard{mMutex};
                mObject = object;
            } else {
                mObject = object;
            }
        }

        T &get() {
            if (!mIsLocked) {
                std::lock_guard<std::mutex> guard{mMutex};
                return mObject;
            } else {
                return mObject;
            }
        }

        void lock() {
            mMutex.lock();
            mIsLocked = true;
        }

        void unlock() {
            mIsLocked = false;
            mMutex.unlock();
        }

    private:
        T mObject;
        bool mIsLocked = false;
        std::mutex mMutex{};
    };
}

#endif //DOUGHNUT_THREAD_SAFE_H
