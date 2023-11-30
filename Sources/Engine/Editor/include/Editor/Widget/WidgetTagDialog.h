#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qsettings.h>
#include "QtGui/qevent.h"
#include "string"
#include "vector"


namespace Editor::Widget
{
	class WidgetTagDialog : public QDialog
	{
		public:
			QSettings mSettings;
			QBoxLayout* mWindow = nullptr;
			std::list<std::string>& mTags;
			QWidget* central = nullptr;
			int counter = 0;

			WidgetTagDialog(std::list<std::string>& pTags, QWidget* pParent = nullptr);

			void setWindowDecoration();
			void initWindowCloseButton();
			void initCentralWidget();
			void initBottom();
			bool eventFilter(QObject* obj, QEvent* event);

			void loadTag(QVBoxLayout& pLayout, std::string& tag);
			void addTag(QVBoxLayout& pLayout);

			void quit();
	};
}