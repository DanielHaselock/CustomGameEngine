#pragma once
#include <QtCore/qdiriterator.h>
#include <QtCore/qfileinfo.h>

#include "Editor/Widget/WidgetBrowserTree.h"
#include "Editor/Widget/WidgetBrowserTreeItem.h"
#include "Editor/Utils/Utils.h"
#include <QtWidgets/qheaderview.h>

using namespace Editor::Widget;


	WidgetBrowserTree::WidgetBrowserTree(WidgetContentBrowserApp& pApp) :
		mApp(pApp), QTreeView()
{
	mTreeModel = new WidgetBrowserTreeModel(*this);

	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectItems);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(this, &QTreeView::collapsed, this, &WidgetBrowserTree::collapse);
	connect(this, &QTreeView::expanded, this, &WidgetBrowserTree::expand);

	setObjectName("Tree");
	setFocusPolicy(Qt::NoFocus);

	setModel(mTreeModel);
	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setDropIndicatorShown(true);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	setDefaultDropAction(Qt::MoveAction);
}

WidgetBrowserTree::~WidgetBrowserTree()
{
	delete mTreeModel;
}

void WidgetBrowserTree::initHeader(const QString& pProjectName)
{
	mTreeModel->setHorizontalHeaderItem(0, new QStandardItem(pProjectName));
	header()->setSectionResizeMode(QHeaderView::Fixed);
}

QModelIndex WidgetBrowserTree::forEach(const QString& pItem, QModelIndex parent)
{
	for (int r = 0; r < mTreeModel->rowCount(parent); ++r)
	{
		QModelIndex index = mTreeModel->index(r, 0, parent);
		WidgetBrowserTreeItem* item = ((WidgetBrowserTreeItem*)mTreeModel->itemFromIndex(index));
		if (item == nullptr)
			continue;

		QString path = item->mPath;

		if (path == pItem)
			return index;

		if (item->hasChildren())
		{
			index = forEach(pItem, index);
			item = ((WidgetBrowserTreeItem*)mTreeModel->itemFromIndex(index));
			if (item == nullptr)
				continue;
			path = item->mPath;
			if (path == pItem)
				return index;
		}
	}

	return QModelIndex();
}

void WidgetBrowserTree::setClicked(const QString& pItem)
{
	QModelIndex index = forEach(pItem);

	if (!index.isValid())
		return;

	if (((WidgetBrowserTreeItem*)mTreeModel->itemFromIndex(index))->hasChildren())
		expand(index);

	selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
}

void WidgetBrowserTree::dragEnterEvent(QDragEnterEvent* e)
{
	e->acceptProposedAction();
}

void WidgetBrowserTree::dropEvent(QDropEvent* e)
{
	QModelIndex idx = indexAt(e->pos());
	if (!idx.isValid())
		return;

	WidgetBrowserTreeItem* item = dynamic_cast<WidgetBrowserTreeItem*>(mTreeModel->itemFromIndex(idx));
	if (item == nullptr)
		return;

	if (e->mimeData()->hasUrls())
	{
		foreach(const QUrl & url, e->mimeData()->urls())
			Utils::copyFileAndDir(url.toLocalFile(), item->mPath, this);
	}
	else if (e->mimeData()->hasText())
	{
		QString str = e->mimeData()->text();
		mApp.watcher.removePath(str);

		const QList<QFileInfo> dirList = QDir(item->mPath).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (unsigned int i = 0; i < dirList.size(); i++)
		{
			if (dirList[i].absoluteFilePath() == str)
				return;
		}

		QDirIterator it(str, QDirIterator::Subdirectories);
		while (it.hasNext())
		{
			it.next();
			mApp.watcher.removePath(it.fileInfo().absoluteFilePath());
		}

		if (selectedIdx.isEmpty())
			selectedIdx = item->mPath;

		Utils::copyFileAndDir(str, item->mPath, this, true);
		mApp.updateContentDir();
	}

	QWidget::dropEvent(e);
}

void WidgetBrowserTree::collapse(const QModelIndex& index)
{
	state[dynamic_cast<WidgetBrowserTreeItem*>(mTreeModel->itemFromIndex(index))->mPath] = false;
	QTreeView::collapse(index);
}

void WidgetBrowserTree::expand(const QModelIndex& index)
{
	state[dynamic_cast<WidgetBrowserTreeItem*>(mTreeModel->itemFromIndex(index))->mPath] = true;
	QTreeView::expand(index);
}

void WidgetBrowserTree::dragMoveEvent(QDragMoveEvent* e)
{
	QTreeView::dragMoveEvent(e);
}

void WidgetBrowserTree::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QModelIndexList indexes = selected.indexes();
	foreach(const QModelIndex & index, indexes)
	{
		WidgetBrowserTreeItem* item = dynamic_cast<WidgetBrowserTreeItem*>(mTreeModel->itemFromIndex(index));
		selectedIdx = item->mPath;
	}

	QTreeView::selectionChanged(selected, deselected);
}