#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtCore/qsettings.h>

namespace Editor::Widget
{
	class WidgetTemplateDescription : public QWidget
	{
	public:
		QVBoxLayout mLayout;
		QLabel mImage;
		QLabel mText;
		QLabel mTitle;

		WidgetTemplateDescription(QSettings& pSetting, const QString& pGroupTag, QWidget* pParent = nullptr);
	};
}