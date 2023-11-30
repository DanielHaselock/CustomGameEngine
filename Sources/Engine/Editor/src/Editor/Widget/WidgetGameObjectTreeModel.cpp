#include "Editor/Widget/WidgetGameObjectTreeModel.h"
#include "Editor/Widget/WidgetGameObjectTreeItem.h"
#include "Editor/Utils/Utils.h"

using namespace Editor::Widget;

QStandardItem* WidgetGameObjectTreeModel::dropingItem = nullptr;

WidgetGameObjectTreeModel::WidgetGameObjectTreeModel()
{
}

QVariant WidgetGameObjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DecorationRole)
		return QPixmap("Resources/Editor/Icon/shapes.png").scaled(14, 14);
	return QStandardItemModel::data(index, role);
}

Qt::DropActions WidgetGameObjectTreeModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags WidgetGameObjectTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsDropEnabled;

	if (mShouldEdit)
		return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
}

QStringList WidgetGameObjectTreeModel::mimeTypes() const
{
	QStringList types;
	types << "Actor";
	return types;
}

bool WidgetGameObjectTreeModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
	return true;
}

bool WidgetGameObjectTreeModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	return true;
}

QMimeData* WidgetGameObjectTreeModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = QStandardItemModel::mimeData(indexes);
	mimeData->setData("Data", "Drop");

	for (int i = 0; i < indexes.size(); i++)
		dropingItem = itemFromIndex(indexes[i]);

	return mimeData;
}