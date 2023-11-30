#include "Editor/Widget/WidgetBrowserTreeModel.h"
#include "Editor/Widget/WidgetBrowserTreeItem.h"

using namespace Editor::Widget;

WidgetBrowserTreeModel::WidgetBrowserTreeModel(WidgetBrowserTree& pTree) : mTree(pTree)
{ 
}

QVariant WidgetBrowserTreeModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DecorationRole)
	{
		if (mTree.isExpanded(index))
			return QPixmap("Resources/Editor/Icon/OpenedFolder.png").scaled(14, 14);
		else
			return QPixmap("Resources/Editor/Icon/ClosedFolder.png").scaled(14, 14);
	}
	return QStandardItemModel::data(index, role);
}

Qt::DropActions WidgetBrowserTreeModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags WidgetBrowserTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsDropEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
}

QStringList WidgetBrowserTreeModel::mimeTypes() const
{
	QStringList types;
	types << "text/plain";
	return types;
}

bool WidgetBrowserTreeModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
	return true;
}

bool WidgetBrowserTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	return true;
}

QMimeData* WidgetBrowserTreeModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();
	QByteArray encodedData;

	for (const QModelIndex& index : indexes)
	{
		WidgetBrowserTreeItem* item = (WidgetBrowserTreeItem*)itemFromIndex(index);
		mimeData->setData("text/plain", item->mPath.toUtf8());
		mTree.selectedIdx.clear();
		return mimeData;
	}

	return mimeData;
}