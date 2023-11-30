#include <QtCore/qfileinfo.h>
#include "Editor/Widget/WidgetDropUI.h"
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include "Game/Component/CPModel.h"
#include "EngineCore/Service/ServiceLocator.h"
#include <QtWidgets/qmessagebox.h>
#include "Editor/Data/ProjectLocation.h"
#include "QtWidgets/qmenu.h"

using namespace Editor::Widget;

WidgetDropUI::WidgetDropUI(QLabel& pName)
	: mName(pName)
{
	setAcceptDrops(true);
    installEventFilter(this);
	connect(this, &QWidget::customContextMenuRequested, this, &WidgetDropUI::showMenu);
}

void WidgetDropUI::dragEnterEvent(QDragEnterEvent* e)
{
	e->acceptProposedAction();
}

void WidgetDropUI::dropEvent(QDropEvent* e)
{
	
}

void WidgetDropUI::showMenu(const QPoint& pPos)
{
    QMenu Menu("Hello world", this);
    
    QAction Action("Clear");
    Menu.addAction(&Action);

    connect(&Action, &QAction::triggered, this, [this] 
    {
        mName.clear();
        clear();

        Q_EMIT textChanged("");
    });

    Menu.exec(mapToGlobal(pPos));
}

bool WidgetDropUI::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) 
    {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() != Qt::LeftButton)
        {
            showMenu(mouseEvent->pos());
            return true;
        }
    }
        
    return QLineEdit::eventFilter(object, event);
}