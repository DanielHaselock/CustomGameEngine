#include "Editor/Widget/WidgetDropTexture.h"
#include <QtCore/qfileinfo.h>
#include <QtCore/qmimedata.h>

using namespace Editor::Widget;

WidgetDropTexture::WidgetDropTexture(const QString& pText) : QLabel(pText)
{
	setAcceptDrops(true);
}

void WidgetDropTexture::dragEnterEvent(QDragEnterEvent* pEvent)
{
	pEvent->acceptProposedAction();
}

void WidgetDropTexture::dragMoveEvent(QDragMoveEvent* pEvent)
{
	pEvent->acceptProposedAction();
}