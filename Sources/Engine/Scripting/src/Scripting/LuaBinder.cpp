#include "Scripting/LuaBinder.h"


using namespace Scripting::Binder;

void LuaBinder::CallBinders(sol::state& pLuaState)
{
	mGlobalBinder.bind(pLuaState);
	mMathsBinder.bind(pLuaState);
	mComponentBinder.bind(pLuaState);
	mActorBinder.bind(pLuaState);
	mSceneBinder.bind(pLuaState);
}