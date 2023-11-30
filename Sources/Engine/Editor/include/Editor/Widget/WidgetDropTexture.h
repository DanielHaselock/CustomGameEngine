#pragma once
#include <QtWidgets/qlabel.h>
#include <QtGui/qevent.h>

namespace Editor::Widget
{
	class WidgetDropTexture : public QLabel
	{
	public:
		WidgetDropTexture(const QString& pText);
		void dragEnterEvent(QDragEnterEvent* pEvent) override;
		void dragMoveEvent(QDragMoveEvent* pEvent) override;
	};
}