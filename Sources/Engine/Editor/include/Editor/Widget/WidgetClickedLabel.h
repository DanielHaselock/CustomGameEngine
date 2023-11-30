#pragma once
#include "QtWidgets/qlabel.h"

namespace Editor::Widget
{
	class WidgetClickedLabel : public QLabel
	{
		public:
			void clicked() {};

			void mousePressEvent(QMouseEvent* event) override { emit setText(""); }
	};
}