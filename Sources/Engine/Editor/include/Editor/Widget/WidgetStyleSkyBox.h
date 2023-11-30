#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qsettings.h>
#include <QtCore/qstring.h>
#include <QtGui/qevent.h>
#include "Editor/Widget/WidgetDoubleSpinBox.h"
#include "Editor/Widget/WidgetSelectable.h"
#include "Game/Component/CPCamera.h"

namespace Editor::Widget
{
	class WidgetStyleSkyBox : public QDialog
	{
		public:
			QSettings mSettings;
			QBoxLayout* mWindow = nullptr;
			QVector<QLineEdit*> lineEdit;
			Game::Component::CPCamera& mCamera;

			WidgetStyleSkyBox(Game::Component::CPCamera& pCamera, QWidget* pParent = nullptr);

			void setWindowDecoration();
			void initWindowCloseButton();
			void initCentralWidget();
			void initBottom();
			bool eventFilter(QObject* obj, QEvent* event);
			void quit();

			void setTextureDialogue(QString name, int idx, QVBoxLayout& box);
	};
}