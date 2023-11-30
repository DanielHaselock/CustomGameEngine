#pragma once
#include "Game/Component/CPScript.h"
#include "Scripting/LuaBinder.h"
#include "Scripting/ScriptInterpreter.h"
#ifdef NSHIPPING
#include "Editor/Widget/WidgetConsole.h"
#endif
#include "Game/Data/Actor.h"
using namespace Game::Component;

CPScript::CPScript(const CPScript& pOther)
{
	mPath = pOther.mPath;
	mName = pOther.mName;
}

AComponent* CPScript::clone()
{
	return new CPScript(*this);
}

CPScript::~CPScript()
{
	service(Scripting::ScriptInterpreter).unregisterScript(this);
}

void CPScript::setScript(const std::string& pName, const char* pScript)
{
	mName = pName;
	mPath = pScript;
}

void CPScript::setScript(const char* pPath)
{
	if (pPath == nullptr)
	{
		mName = "";
		mPath = "";
		return;
	}
}

bool CPScript::registerToLua(sol::state& pLuaState)
{
	static std::mutex mtx;
	std::unique_lock lock(mtx);

	auto result = pLuaState.safe_script_file(mPath, &sol::script_pass_on_error);

	if (!result.valid())
	{
		sol::error err = result;
#ifdef NSHIPPING
		service(Editor::Widget::WidgetConsole).errorPrint(err.what());
#endif
		
		return false;
	}

	if (result.return_count() != 1 || !(result[0].is<sol::table>()))
	{

#ifdef NSHIPPING
		service(Editor::Widget::WidgetConsole).errorPrint("Script must return a table");
#endif
		return false;
	}

	mLuaTable = result[0];
	Game::Data::Actor* Actor = (Game::Data::Actor*)mOwner;
	mLuaTable["owner"] = Actor;

	return true;
}

void CPScript::unregisterFromLua()
{
	mLuaTable = sol::nil;
}

void Game::Component::CPScript::OnStart()
{
	if (!isActive)
		return;

	static std::mutex mtx2;
	std::unique_lock lock(mtx2);
	luaCall("OnBegin");
}

#include <Game/Utils/ComponentType.h>
void CPScript::OnUpdate(float pDeltaTime)
{
	if (!isActive)
		return;

	luaCall("OnUpdate", pDeltaTime);
}

#ifdef NSHIPPING
#include "Editor/Widget/WidgetEditor.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Editor/Utils/Utils.h"
#endif
void CPScript::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Script);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Name");
	pWriter.String(mName.c_str());

#ifdef NSHIPPING
	std::string currentPath = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder);
	pWriter.Key("Path");
	pWriter.String(mPath.empty() ? "" : mPath.substr(currentPath.length()).c_str());
#endif

	pWriter.EndObject();
}
