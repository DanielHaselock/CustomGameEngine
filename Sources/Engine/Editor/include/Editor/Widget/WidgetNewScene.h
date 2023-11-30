#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qsettings.h>
#include <QtCore/qstring.h>
#include <QtGui/qevent.h>

#include "Editor/Widget/WidgetSelectable.h"

namespace Editor::Widget
{
	class WidgetNewScene : public QDialog
	{
		public:
			QSettings mSettings;
			QString mProjectDir;
			QString mNewScene;

			QBoxLayout* mWindow = nullptr;
			WidgetSelectable* mCurrent = nullptr;
			QVector<WidgetSelectable*> mChoice;

			WidgetNewScene(const QString& pProjectDir, QWidget* pParent = nullptr);

			void setWindowDecoration();
			void initWindowCloseButton();
			void initCentralWidget();
			void initBottom();
			bool eventFilter(QObject* obj, QEvent* event);
			void createLevel();
			void quit();
	};
}