#pragma once

#include <sol.hpp>
#include "EngineCore/EventSystem/InputManager.h"
namespace Scripting
{
	namespace Binder
	{
		class LuaGlobalsBinder
		{
		public:
			void bind(sol::state& pLuaState);

			void SetKeys();

			static std::map<EngineCore::EventSystem::Key, bool> mKeys;
			static std::map<std::string, EngineCore::EventSystem::Key> mKeyStrings;

			std::vector<std::string> CalledFunctionsInLua;

		};
	}
}