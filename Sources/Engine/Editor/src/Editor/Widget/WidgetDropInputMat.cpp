#include <QtCore/qfileinfo.h>
#include "Editor/Widget/WidgetDropInputMat.h"
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include "Game/Component/CPModel.h"
#include "EngineCore/Service/ServiceLocator.h"
#include <QtWidgets/qmessagebox.h>
#include "Editor/Data/ProjectLocation.h"
#include "QtWidgets/qmenu.h"

using namespace Editor::Widget;

WidgetDropInputMat::WidgetDropInputMat(Game::Component::CPModel& pComponent)
	: mComponent(pComponent)
{
	setAcceptDrops(true);
	installEventFilter(this);
	connect(this, &QWidget::customContextMenuRequested, this, &WidgetDropInputMat::showMenu);
}

void WidgetDropInputMat::dragEnterEvent(QDragEnterEvent* e)
{
	e->acceptProposedAction();
}

void WidgetDropInputMat::dropEvent(QDropEvent* e)
{
	QFileInfo info = QFileInfo(e->mimeData()->text());

	QString Defaultpath = service(Data::ProjectLocation).mFolder;
	if (!info.absoluteFilePath().contains(Defaultpath))
	{
		QMessageBox::warning(this, tr("Application"), tr("Can\'t use file outside of project"));
		return;
	}

	Tools::Utils::PathParser::EFileType type = Utils::getFileType(info.suffix());
	if (type != Tools::Utils::PathParser::EFileType::MATERIAL)
	{
		QMessageBox::warning(this, tr("Application"), tr("Wrong type of file"));
		return;
	}

	
	//mName.setText(baseName());
	//setText(info.absoluteFilePath());
}

void WidgetDropInputMat::showMenu(const QPoint& pPos)
{
	QMenu Menu("Hello world", this);

	QAction Action("Clear");
	Menu.addAction(&Action);

	connect(&Action, &QAction::triggered, this, [this]
	{
		/*mName.clear();
		clear();

		Q_EMIT textChanged("");*/
	});

	Menu.exec(mapToGlobal(pPos));
}

bool WidgetDropInputMat::eventFilter(QObject* object, QEvent* event)
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