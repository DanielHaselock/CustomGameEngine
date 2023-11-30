#include "Editor/Widget/WidgetDoubleSpinBox.h"
#include <QtWidgets/qapplication.h>
#include <QtGui/qcursor.h>

using namespace Editor::Widget;

WidgetDoubleSpinBox::WidgetDoubleSpinBox(const double pDefaultValue, const long long pMin, const long long pMax, const float pStep) : QDoubleSpinBox()
{	
	setRange(pMin, pMax);
	setValue(pDefaultValue);
	setSingleStep(pStep);
	setDecimals(4);
	setLineEdit(new WidgetDoubleSpinBoxLineEdit(*this));

	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setObjectName("spinBox");
}

WidgetDoubleSpinBoxLineEdit::WidgetDoubleSpinBoxLineEdit(WidgetDoubleSpinBox& pSpinBox) : mSpinBox(pSpinBox)
{
	setCursor(Qt::CursorShape::SizeHorCursor);
}

void WidgetDoubleSpinBoxLineEdit::mousePressEvent(QMouseEvent* pEvent)
{
	if (pEvent->button() == Qt::LeftButton)
		mDragStartPosition = pEvent->pos();
	QLineEdit::mousePressEvent(pEvent);
}

void WidgetDoubleSpinBoxLineEdit::mouseMoveEvent(QMouseEvent* pEvent)
{
	if (!(pEvent->buttons() & Qt::LeftButton))
		return;

	if (!mDragging)
	{
		mDragging = true;
		setReadOnly(mDragging);
		QApplication::setOverrideCursor(Qt::BlankCursor);
	}


	int res = (pEvent->pos().x()) - mDragStartPosition.x();
	QCursor::setPos(QWidget::mapToGlobal(mDragStartPosition));


	mSpinBox.setValue(mSpinBox.value() + (res * mSpinBox.singleStep()));
}

void WidgetDoubleSpinBoxLineEdit::mouseReleaseEvent(QMouseEvent* pEvent)
{
	if (mDragging)
	{
		mDragging = false;
		setReadOnly(mDragging);
		clearFocus();
		
		QApplication::restoreOverrideCursor();
		setCursor(Qt::CursorShape::SizeHorCursor);
	}
}

void WidgetDoubleSpinBoxLineEdit::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
	selectAll();
}