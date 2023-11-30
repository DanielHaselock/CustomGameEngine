#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qwidgetaction.h>
#include <QtCore/qsettings.h>

namespace Editor::Widget
{
	class WidgetMenuSeperator : public QWidget
	{
	public:
		QHBoxLayout* mHLayout = nullptr;
		QLabel* icon = nullptr;
		QLabel* mapTitle = nullptr;

		QWidgetAction* mAction = nullptr;

		WidgetMenuSeperator(const QSettings& pSetting, const QString& pName, const QString& pIcon, QWidget* pWidget = nullptr);
	};
}