#pragma once
#include "WidgetInspectorApp.h"
#include <QtWidgets/qlineedit.h>
#include <QtGui/qevent.h>
#include "Game/Component/CPModel.h"
#include "Tools/Utils/PathParser.h"

namespace Editor::Widget
{
	class WidgetDropInputMat : public QLineEdit
	{
		public:
			Game::Component::CPModel& mComponent;

			WidgetDropInputMat(Game::Component::CPModel& pComponent);
			void dragEnterEvent(QDragEnterEvent* e) override;
			void dropEvent(QDropEvent* e) override;

			void showMenu(const QPoint& pPos);
			bool eventFilter(QObject* object, QEvent* event) override;
	};
}