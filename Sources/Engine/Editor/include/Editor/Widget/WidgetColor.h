#pragma once
#include "QtWidgets/qcolordialog.h"

namespace Editor::Widget
{
	class WidgetColor : public QColorDialog
	{
	public:
		WidgetColor(const QColor& pColor);
	};
}