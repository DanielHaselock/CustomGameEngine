#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qsettings.h>
#include <QtCore/qstring.h>
#include <QtGui/qevent.h>
#include "Editor/Widget/WidgetDoubleSpinBox.h"
#include "Editor/Widget/WidgetSelectable.h"
#include "Editor/UI/Text.h"

namespace Editor::Widget
{
	class WidgetStyleUIText : public QDialog
	{
		public:
			QSettings mSettings;
			QBoxLayout* mWindow = nullptr;
			Editor::UI::Text& mResource;

			WidgetStyleUIText(Editor::UI::Text& pResource, QWidget* pParent = nullptr);

			void setWindowDecoration();
			void initWindowCloseButton();
			void initCentralWidget();
			void initBottom();
			bool eventFilter(QObject* obj, QEvent* event);
			void quit();

			WidgetDoubleSpinBox* setSpinBox(std::string pText, int& pAxisVal, QGridLayout* pLayout, int row, int column);
			WidgetDoubleSpinBox* setSpinBoxF(std::string pText, float& pAxisVal, QGridLayout* pLayout, int row, int column);
	
			void setTexture(QBoxLayout& widLayout, Editor::UI::UIStyle& style);
			void setColor(QBoxLayout& widLayout, Editor::UI::UIStyle& style);
	};
}