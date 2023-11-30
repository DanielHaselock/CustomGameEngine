#include "Editor/Widget/WidgetBrowserTreeItem.h"

using namespace Editor::Widget;

WidgetBrowserTreeItem::WidgetBrowserTreeItem(const QString& pPath, const QString& pText) : mPath(pPath), QStandardItem(pText)
{
	setEditable(false);
}