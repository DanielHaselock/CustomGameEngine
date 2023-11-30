#pragma once

#include <sol.hpp>
#include "Scripting/Binders/LuaGlobalsBinder.h"
#include "Scripting/Binders/LuaMathsBinder.h"
#include "Scripting/Binders/LuaComponentBinder.h"
#include "Scripting/Binders/LuaActorBinder.h"
#include "Scripting/Binders/LuaSceneBinder.h"
namespace Scripting
{
	namespace Binder
	{
		class LuaBinder
		{
		public:
			void CallBinders(sol::state& pLuaState);

			LuaGlobalsBinder mGlobalBinder;
			LuaMathsBinder mMathsBinder;
			LuaComponentBinder mComponentBinder;
			LuaActorBinder mActorBinder;
			LuaSceneBinder mSceneBinder;
		};
	}
}