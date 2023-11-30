#pragma once
#include "Editor/Widget/WidgetSelectableBrowserUI.h"
#include "Editor/Widget/WidgetEditor.h"
#include "EngineCore/Service/ServiceLocator.h"


using namespace Editor::Widget;

WidgetSelectableBrowserUI::WidgetSelectableBrowserUI(WidgetContentBrowser& pApp, const QString& pPath, const bool& pIsDirectory, QSettings& pSetting, const QString& pName, const QString& pImage, bool pIsAnimation, QWidget* pParent) :
	WidgetSelectableBrowser(pApp, pPath, pIsDirectory, pSetting, pName, pImage, pIsAnimation, pParent)
{
}

void WidgetSelectableBrowserUI::doubleClick()
{
	service(Editor::Widget::WidgetEditor).openUI(mPath);
}