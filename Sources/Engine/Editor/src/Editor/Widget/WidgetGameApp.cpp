#include <QtWidgets/qboxlayout.h>

#include "Editor/Widget/WidgetGameApp.h"

using namespace Editor::Widget;

WidgetGameApp::WidgetGameApp(EngineCore::Core::EngineApp* mApp, QWidget* pParen) :
	ads::CDockWidget("Game")
{
	provideService(Editor::Widget::WidgetGameApp, *this);
	QWidget* content = new QWidget(pParen);
	setWidget(content);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	layout->setContentsMargins(5, 0, 5, 5);
	layout->setSpacing(0);
	content->setLayout(layout);


	windowId = mApp->mWindow.getWindowId();
	qWindow = QWindow::fromWinId((WId)windowId);
	renderingWindow = QWidget::createWindowContainer(qWindow, nullptr);

	layout->addWidget(renderingWindow);
}

Maths::FVector2 WidgetGameApp::calculateCursorPos()
{
	//QPoint NewCenterQt;
	/*NewCenterQt.setX((renderingWindow->rect().topLeft().x() + renderingWindow->rect().width()));
	NewCenterQt.setY((renderingWindow->rect().topLeft().y() + renderingWindow->rect().height()));*/

	QPoint NewCenterQt = renderingWindow->mapToGlobal(renderingWindow->rect().center());
	return { (float)NewCenterQt.x(), (float)NewCenterQt.y() };
}