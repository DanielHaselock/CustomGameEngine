#include "EngineCore/Thread/ThreadPool.h"


using namespace EngineCore::Thread;

ThreadPool::ThreadPool()
{
    start();
}

void ThreadPool::start()
{
    mThreads.reserve(std::thread::hardware_concurrency() - 1);
    for (unsigned int i = 0; i < std::thread::hardware_concurrency() - 1; i++)
        mThreads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
}

ThreadPool::~ThreadPool()
{
    if (!mStopped)
        stop();
}

void ThreadPool::queueJob(std::function<void()> pJob)
{
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mJobs.push(pJob);
    }
    mMutexCondition.notify_one();
}

void ThreadPool::stop()
{
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mShouldTerminate = true;
    }
    
    mMutexCondition.notify_all();
    for (unsigned int i = 0; i < mThreads.size(); i++)
        mThreads[i].join();


    mThreads.clear();
    mStopped = true;
}

bool ThreadPool::busy()
{
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        poolbusy = !mJobs.empty();
    }

    return poolbusy;
}

void ThreadPool::threadLoop()
{
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mMutexCondition.wait(lock, [this] 
                {
                    return !mJobs.empty() || mShouldTerminate;
                }
            );

            if (mShouldTerminate) 
                return;

            job = mJobs.front();
            mJobs.pop();
        }
        job();
    }
}