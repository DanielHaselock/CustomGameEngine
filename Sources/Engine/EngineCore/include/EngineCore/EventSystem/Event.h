#pragma once
#include <vector>
#include <functional>
#include "Maths/FMatrix4.h"

namespace EngineCore::EventSystem
{
	template<typename ... Args> class Event
	{
		public:
			std::list<std::function<void(Args&...)>> mActions;

			size_t subscribe(std::function<void(Args&...)> pAction)
			{
				mActions.push_back(pAction);
				return mActions.size() - 1;
			}

			void remove(size_t pPos)
			{
				auto it = mActions.begin();
				std::advance(it, (int)pPos);

				mActions.erase(it);
			}

			void dispatch(Args&... pArgs)
			{
				if (mActions.size() == 0)
					return;

				for (std::function<void(Args&...)>& action : mActions)
					action(pArgs...);
			}
	};
}