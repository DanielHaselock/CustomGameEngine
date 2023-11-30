#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include <QtCore/qsettings.h>
#include <QtGui/qmovie.h>

namespace Editor::Widget
{
	class WidgetSelectable : public QWidget
	{
		public:
			bool mChecked = false;
			QVBoxLayout mLayout;
			QLabel mImage;
			QLineEdit mText;
			QMovie* mMovie = nullptr;

			WidgetSelectable(QSettings& pSetting, const QString& pProjectName, const QString& pIcon, bool pIsAnimation = false, QWidget* pParent = nullptr);
			~WidgetSelectable();
			virtual void setClicked(const bool& pChecked);
	};
}