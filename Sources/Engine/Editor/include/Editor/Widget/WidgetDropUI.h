#pragma once
#include "WidgetInspectorApp.h"
#include <QtWidgets/qlineedit.h>
#include <QtGui/qevent.h>
#include "Game/Component/AComponent.h"
#include "Tools/Utils/PathParser.h"

namespace Editor::Widget
{
	class WidgetDropUI : public QLineEdit
	{
		public:
			QLabel& mName;

			WidgetDropUI(QLabel& pName);
			void dragEnterEvent(QDragEnterEvent* e) override;
			void dropEvent(QDropEvent* e) override;

			void showMenu(const QPoint& pPos);
			bool eventFilter(QObject* object, QEvent* event);
	};
}