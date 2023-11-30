#pragma once

#include <iostream>
#include "Game/SceneSys/SceneManager.h"
#include "Game/Component/CPUI.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <Game/Utils/ComponentType.h>
#include "Tools/Utils/PathParser.h"
#include <filesystem>
#ifdef NSHIPPING
	//#include "EngineCore/Core/EngineApp.h"
#include "Editor/Widget/WidgetEditor.h"
#endif
using namespace Game::Component;

CPUI::CPUI()
{
}

AComponent* CPUI::clone()
{
	return new CPUI(*this);
}

void CPUI::setUI(const std::string& pName, const char* pUI)
{
	ui = service(EngineCore::ResourcesManagement::ResourceManager).get<Rendering::Resources::UI>(pUI);
	if (ui == nullptr)
		ui = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::UI>(pUI, pUI);
	else
		ui->updateSynch(mOwner);
	
	mNameUI = pName;
	mPathUI = pUI;

	if (ui == nullptr)
	{
		mNameUI = "Unknown";
		mPathUI = "";
	}
}

void CPUI::setUIWithPath(const char* pUI)
{
	if (pUI == nullptr)
	{
		ui = nullptr;
		mNameUI = "";
		mPathUI = "";
		return;
	}

	ui = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::UI>(pUI, pUI);
	mPathUI = pUI;

	if (ui == nullptr)
		mPathUI = "";
}

void CPUI::setUIWithPathLua(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";

	ui = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::UI>(currentPath + pPath, currentPath + pPath);
	mPathUI = currentPath + pPath;

	if (ui == nullptr)
		mPathUI = "";
}

void CPUI::updateUI(Maths::FMatrix4& pVP, int width, int height)
{
	if (ui == nullptr)
		return;

	ui->updateProj(pVP, width, height);
}

void CPUI::draw(void* pCmd, Rendering::Data::Material* pPipeLine, Rendering::Data::Material* pPipeLineText)
{
	if (ui == nullptr || !isActive)
		return;

	ui->draw((VkCommandBuffer)pCmd, *pPipeLine, *pPipeLineText);
}

bool CPUI::contain(Maths::FVector2 p)
{
	if (ui == nullptr || !isActive)
		return false;

	return ui->contain(p);
}

Rendering::Resources::UIResource::IUIResource* CPUI::containClick(Maths::FVector2 p)
{
	if (ui == nullptr || !isActive)
		return false;

	return ui->containClick(p);
}

#ifdef NSHIPPING
	#include "Editor/Widget/WidgetEditor.h"
	#include "EngineCore/Service/ServiceLocator.h"
	#include "Editor/Utils/Utils.h"
#endif
void CPUI::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::UI);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Name");
	pWriter.String(mNameUI.c_str());
	
	#ifdef NSHIPPING
		std::string currentPath = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder);
		pWriter.Key("Path");
		pWriter.String(mPathUI.empty() ? "": mPathUI.substr(currentPath.length()).c_str());
	#endif

	pWriter.EndObject();
}

