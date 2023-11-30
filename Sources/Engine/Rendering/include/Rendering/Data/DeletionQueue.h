#pragma once
#include <functional>
#include <deque>
#include <mutex>

namespace Rendering::Data
{
	struct DeletionQueue
	{
		std::deque<std::function<void()>> mDeletors;
		std::recursive_mutex mMutex;

		void pushFunction(std::function<void()>&& pFunction);

		void flush();
		void timerFlush();

		void executeLoop();
	};
}
