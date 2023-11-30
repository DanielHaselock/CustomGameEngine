#pragma once
#include <QtCore/qsettings.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qwidget.h>

namespace Editor::Widget
{
	class WidgetSceneLabel
	{
		public:
			QWidget* mWidget = nullptr;
			QLabel* mMapTitle = nullptr;
			QString mFileName;

			WidgetSceneLabel();
			void init(QWidget* pParent, QSettings& pSetting, int pHeight, const QString& pScenePath, const QString& pSceneName);
	};
}