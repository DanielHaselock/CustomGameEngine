#pragma once
#include "Editor/Widget/WidgetSelectableBrowser.h"
#include "Editor/Widget/WidgetContentBrowser.h"
#include <QtWidgets/qwidget.h>
#include <QtCore/qstring.h>
#include <QtCore/qsettings.h>

namespace Editor::Widget
{
	class WidgetSelectableBrowserModel : public WidgetSelectableBrowser
	{
		public:
			WidgetSelectableBrowserModel(WidgetContentBrowser& pApp, const QString& pPath, const bool& pIsDirectory, QSettings& pSetting, const QString& pName, const QString& pImage, bool pIsAnimation = false, QWidget* pParent = nullptr);
			
			void doubleClick() override;
	};
}