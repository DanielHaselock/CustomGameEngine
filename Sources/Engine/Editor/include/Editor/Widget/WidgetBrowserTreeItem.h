#pragma once
#include <QtGui/qstandarditemmodel.h>

namespace Editor::Widget
{
	class WidgetBrowserTreeItem : public QStandardItem
	{
		public:
			QString mPath;
			WidgetBrowserTreeItem(const QString& pPath, const QString& pText);
	};
}