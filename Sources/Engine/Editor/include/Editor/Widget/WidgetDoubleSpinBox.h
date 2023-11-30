#pragma once
#include <QtWidgets/qspinbox.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qlineedit.h>

namespace Editor::Widget
{
	class WidgetDoubleSpinBox : public QDoubleSpinBox
	{
		public:
			QPoint mDragStartPosition;
			bool mDragging = false;
			bool manuelEvent = false;

			WidgetDoubleSpinBox(const double pDefaultValue, const long long pMin, const long long pMax, const float pStep = 0.3);
	};
	
	class WidgetDoubleSpinBoxLineEdit : public QLineEdit
	{
		public:
			WidgetDoubleSpinBox& mSpinBox;

			QPoint mDragStartPosition;
			bool mDragging = false;

			WidgetDoubleSpinBoxLineEdit(WidgetDoubleSpinBox& pSpinBox);

			void mousePressEvent(QMouseEvent* pEvent) override;
			void mouseMoveEvent(QMouseEvent* pEvent) override;
			void mouseReleaseEvent(QMouseEvent* pEvent) override;
			void mouseDoubleClickEvent(QMouseEvent* pEvent) override;
	};
}