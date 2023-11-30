#pragma once
#include "WidgetInspectorApp.h"
#include <QtWidgets/qlineedit.h>
#include <QtGui/qevent.h>
#include "Game/Component/AComponent.h"
#include "Tools/Utils/PathParser.h"

namespace Editor::Widget
{
	class WidgetDropInput : public QLineEdit
	{
		public:
			Game::Component::AComponent& mComponent;
			QLabel& mName;
			Tools::Utils::PathParser::EFileType mExtension;

			WidgetDropInput(QLabel& pName, Game::Component::AComponent& pComponent, Tools::Utils::PathParser::EFileType pExtension);
			void dragEnterEvent(QDragEnterEvent* e) override;
			void dropEvent(QDropEvent* e) override;

			void showMenu(const QPoint& pPos);
			bool eventFilter(QObject* object, QEvent* event) override;
	};
}