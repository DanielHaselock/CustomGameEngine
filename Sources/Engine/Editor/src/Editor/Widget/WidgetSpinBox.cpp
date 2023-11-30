#include "Editor/Widget/WidgetSpinBox.h"
#include <QtWidgets/qapplication.h>
#include <QtGui/qcursor.h>

using namespace Editor::Widget;

WidgetSpinBox::WidgetSpinBox(const int pDefaultValue, const int pMin, const int pMax, const int pStep) : QSpinBox()
{	
	setRange(pMin, pMax);
	setValue(pDefaultValue);
	setSingleStep(pStep);
	setLineEdit(new WidgetSpinBoxLineEdit(*this));

	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setObjectName("spinBox");
}

WidgetSpinBoxLineEdit::WidgetSpinBoxLineEdit(WidgetSpinBox& pSpinBox) : mSpinBox(pSpinBox)
{
	setCursor(Qt::CursorShape::SizeHorCursor);
}

void WidgetSpinBoxLineEdit::mousePressEvent(QMouseEvent* pEvent)
{
	if (pEvent->button() == Qt::LeftButton)
		mDragStartPosition = pEvent->pos();
	QLineEdit::mousePressEvent(pEvent);
}

void WidgetSpinBoxLineEdit::mouseMoveEvent(QMouseEvent* pEvent)
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

	printf("%d %d\n", mSpinBox.value(), (res * mSpinBox.singleStep()));
}

void WidgetSpinBoxLineEdit::mouseReleaseEvent(QMouseEvent* pEvent)
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

void WidgetSpinBoxLineEdit::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
	selectAll();
}