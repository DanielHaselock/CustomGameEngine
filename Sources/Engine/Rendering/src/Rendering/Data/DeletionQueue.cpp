#include "Rendering/Data/DeletionQueue.h"
#include <chrono>
#include <iostream>

using namespace Rendering::Data;

void DeletionQueue::pushFunction(std::function<void()>&& pFunction)
{
	std::unique_lock lock(mMutex);
	mDeletors.push_back(pFunction);
}

void DeletionQueue::flush()
{
	std::unique_lock lock(mMutex);
	for (auto it = mDeletors.rbegin(); it != mDeletors.rend(); it++)
		(*it)();

	mDeletors.clear();
}

void DeletionQueue::timerFlush()
{
	if (mDeletors.size() == 0)
		return;

	auto t1 = std::chrono::high_resolution_clock::now();

	std::unique_lock lock(mMutex);
	
	while (mDeletors.size() != 0)
	{
		std::function<void()>& job = mDeletors.back();
		job();
		mDeletors.pop_back();

		auto t2 = std::chrono::high_resolution_clock::now();
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

		if (ms_int.count() > 2)
			return;
	}
}

void DeletionQueue::executeLoop()
{
	std::unique_lock lock(mMutex);
	for (auto it = mDeletors.rbegin(); it != mDeletors.rend(); it++)
		(*it)();
}