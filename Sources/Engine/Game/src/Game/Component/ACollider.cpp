#include "Game/Component/ACollider.h"
#include "Scripting/ScriptInterpreter.h"

#ifdef NSHIPPING
#include "Editor/Widget/WidgetConsole.h"
#endif

void Game::Component::ACollider::OnCollisionEnter(void* pOtherActor, ACollider* pOtherCollider)
{
	if (/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnCollisionEnter)
	{
		try
		{
			/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnCollisionEnter(pOtherActor);
		}
		catch (sol::error err)
		{
#ifdef NSHIPPING
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
#endif
		}
	}
}

void Game::Component::ACollider::OnTriggerEnter(void* pOtherActor, ACollider* pOtherCollider)
{
	if (/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnTriggerEnter)
	{
		try
		{
			/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnTriggerEnter(pOtherActor);
		}
		catch (sol::error err)
		{
#ifdef NSHIPPING
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
#endif
		}
	}
}

void Game::Component::ACollider::OnCollisionStay(void* pOtherActor, ACollider* pOtherCollider)
{
	if (/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnCollisionStay)
	{
		try
		{
			/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnCollisionStay(pOtherActor);
		}
		catch (sol::error err)
		{
#ifdef NSHIPPING
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
#endif
		}
	}
}

void Game::Component::ACollider::OnTriggerStay(void* pOtherActor, ACollider* pOtherCollider)
{
	if (/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnTriggerStay)
	{
		try
		{
			/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnTriggerStay(pOtherActor);
		}
		catch (sol::error err)
		{
#ifdef NSHIPPING
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
#endif
		}
	}

}

void Game::Component::ACollider::OnCollisionExit(void* pOtherActor, ACollider* pOtherCollider)
{
	if (/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnCollisionExit)
	{
		try
		{
			/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnCollisionExit(pOtherActor);
		}
		catch (sol::error err)
		{
#ifdef NSHIPPING
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
#endif
		}
	}
}

void Game::Component::ACollider::OnTriggerExit(void* pOtherActor, ACollider* pOtherCollider)
{
	if (/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnTriggerExit)
	{
		try
		{
			/*service(Scripting::ScriptInterpreter).mLuaBinder.mComponentBinder.*/mPhysicsFunctions.OnTriggerExit(pOtherActor);
		}
		catch (sol::error err)
		{
#ifdef NSHIPPING
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
#endif
		}
	}
}
