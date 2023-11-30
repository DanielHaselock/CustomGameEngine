#pragma once

#include <vector>

#include <sol.hpp>

#include "Game/Component/CPScript.h"

#include "Scripting/LuaBinder.h"

namespace Scripting
{
	class ScriptInterpreter
	{
	public:
		ScriptInterpreter();
		~ScriptInterpreter();

		void CreateLuaContextAndBindGlobals();
		void DestroyLuaContext();

		void RefreshAll();

		bool IsInitialized();

		void unregisterScript(Game::Component::CPScript* pScript);

		void registerScript(Game::Component::CPScript* pScript);

	public:
		std::unique_ptr<sol::state> mLuaState;
		std::vector<Game::Component::CPScript*> mBehaviours;

		bool mIsOk;

		Binder::LuaBinder mLuaBinder;
	};
}