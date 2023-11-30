#pragma once
#include "Editor/Widget/WidgetSelectableBrowserPrefab.h"
#include "Editor/Widget/WidgetEditor.h"
#include "EngineCore/Service/ServiceLocator.h"


using namespace Editor::Widget;

WidgetSelectableBrowserPrefab::WidgetSelectableBrowserPrefab(WidgetContentBrowser& pApp, const QString& pPath, const bool& pIsDirectory, QSettings& pSetting, const QString& pName, const QString& pImage, bool pIsAnimation, QWidget* pParent) :
	WidgetSelectableBrowser(pApp, pPath, pIsDirectory, pSetting, pName, pImage, pIsAnimation, pParent)
{
}

void WidgetSelectableBrowserPrefab::doubleClick()
{
	
}