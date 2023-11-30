#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qsettings.h>
#include "QtGui/qevent.h"
#include "string"
#include "vector"
#include "Game/Component/CPModel.h"


namespace Editor::Widget
{
	class WidgetMeshMatDialog : public QDialog
	{
	public:
		QSettings mSettings;
		QBoxLayout* mWindow = nullptr;
		QWidget* central = nullptr;
		Game::Component::CPModel* mModel;
		std::map<std::string, std::list<Rendering::Resources::VK::VKMesh*>> mMats;

		WidgetMeshMatDialog(Game::Component::CPModel* pModel, QWidget* pParent = nullptr);

		void setWindowDecoration();
		void initWindowCloseButton();
		void initCentralWidget();
		void initBottom();
		bool eventFilter(QObject* obj, QEvent* event);

		void quit();
	};
}