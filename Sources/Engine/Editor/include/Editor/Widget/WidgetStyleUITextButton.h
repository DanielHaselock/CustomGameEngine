#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qsettings.h>
#include <QtCore/qstring.h>
#include <QtGui/qevent.h>
#include "Editor/Widget/WidgetDoubleSpinBox.h"
#include "Editor/Widget/WidgetSelectable.h"
#include "Editor/UI/Button.h"

namespace Editor::Widget
{
	class WidgetStyleUITextButton : public QDialog
	{
		public:
			QSettings mSettings;
			QBoxLayout* mWindow = nullptr;
			Editor::UI::Button& mResource;

			WidgetStyleUITextButton(Editor::UI::Button& pResource, QWidget* pParent = nullptr);

			void initCentralWidget();
			
			WidgetDoubleSpinBox* setSpinBox(std::string pText, int& pAxisVal, QGridLayout* pLayout, int row, int column);
			WidgetDoubleSpinBox* setSpinBoxF(std::string pText, float& pAxisVal, QGridLayout* pLayout, int row, int column);
	
			void setTexture(QBoxLayout& widLayout, Editor::UI::UIStyle& style);
			void setColor(QBoxLayout& widLayout, Editor::UI::UIStyle& style);
	};
}