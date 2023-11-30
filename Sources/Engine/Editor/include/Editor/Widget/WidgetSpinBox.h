#pragma once
#include <QtWidgets/qspinbox.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlineedit.h>

namespace Editor::Widget
{
	class WidgetSpinBox : public QSpinBox
	{
		public:
			QPoint mDragStartPosition;
			bool mDragging = false;
			bool manuelEvent = false;

			WidgetSpinBox(const int pDefaultValue, const int pMin, const int pMax, const int pStep = 1);
	};
	
	class WidgetSpinBoxLineEdit : public QLineEdit
	{
		public:
			WidgetSpinBox& mSpinBox;

			QPoint mDragStartPosition;
			bool mDragging = false;

			WidgetSpinBoxLineEdit(WidgetSpinBox& pSpinBox);

			void mousePressEvent(QMouseEvent* pEvent) override;
			void mouseMoveEvent(QMouseEvent* pEvent) override;
			void mouseReleaseEvent(QMouseEvent* pEvent) override;
			void mouseDoubleClickEvent(QMouseEvent* pEvent) override;
	};
}