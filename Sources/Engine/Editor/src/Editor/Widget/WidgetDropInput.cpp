#include <QtCore/qfileinfo.h>
#include "Editor/Widget/WidgetDropInput.h"
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include "Game/Component/CPModel.h"
#include "EngineCore/Service/ServiceLocator.h"
#include <QtWidgets/qmessagebox.h>
#include "Editor/Data/ProjectLocation.h"
#include "QtWidgets/qmenu.h"

using namespace Editor::Widget;

WidgetDropInput::WidgetDropInput(QLabel& pName, Game::Component::AComponent& pComponent, Tools::Utils::PathParser::EFileType pExtension)
	: mName(pName), mComponent(pComponent), mExtension(pExtension)
{
	setAcceptDrops(true);
	installEventFilter(this);
	connect(this, &QWidget::customContextMenuRequested, this, &WidgetDropInput::showMenu);
}

void WidgetDropInput::dragEnterEvent(QDragEnterEvent* e)
{
	e->acceptProposedAction();
}

void WidgetDropInput::dropEvent(QDropEvent* e)
{
	QFileInfo info = QFileInfo(e->mimeData()->text());

	QString Defaultpath = service(Data::ProjectLocation).mFolder;
	if (!info.absoluteFilePath().contains(Defaultpath))
	{
		QMessageBox::warning(this, tr("Application"), tr("Can\'t use file outside of project"));
		return;
	}

	Tools::Utils::PathParser::EFileType type = Utils::getFileType(info.suffix());
	if (type != mExtension)
	{
		QMessageBox::warning(this, tr("Application"), tr("Wrong type of file"));
		return;
	}

	if (type == Tools::Utils::PathParser::EFileType::MODEL && dynamic_cast<Game::Component::CPModel*>(&mComponent) != nullptr)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPModel*)&mComponent)->setModel(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::MODEL && dynamic_cast<Game::Component::CPAnimator*>(&mComponent) != nullptr)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPAnimator*)&mComponent)->setAnimation(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::MATERIAL)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPModel*)&mComponent)->setMat(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::SCRIPT)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPScript*)&mComponent)->setScript(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::SOUND)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPSound*)&mComponent)->setClip(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::UI)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPUI*)&mComponent)->setUI(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::PARTICLE)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPParticle*)&mComponent)->setParticle(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
	else if (type == Tools::Utils::PathParser::EFileType::TEXTURE)
	{
		mName.setText(info.baseName());
		setText(info.absoluteFilePath());

		((Game::Component::CPDecal*)&mComponent)->setDecal(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetDropInput::showMenu(const QPoint& pPos)
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

bool WidgetDropInput::eventFilter(QObject* object, QEvent* event)
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