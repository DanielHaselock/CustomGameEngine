#pragma once
#include <functional>
#include <queue>
#include <mutex>

namespace EngineCore::Thread
{
    class ThreadPool {
        public:
            bool mShouldTerminate = false;
            bool mStopped = false;
            std::mutex mQueueMutex;
            std::condition_variable mMutexCondition;
            std::vector<std::thread> mThreads;
            std::queue<std::function<void()>> mJobs;

            ThreadPool();
            ~ThreadPool();
            void start();
            void queueJob(std::function<void()> pJob);
            void stop();
            bool busy();
            void threadLoop();
    };
}