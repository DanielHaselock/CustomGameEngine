#pragma once
#include <QtWidgets/qtreeview.h>
#include "Editor/Widget/WidgetGameObjectTreeModel.h"
#include "Editor/Widget/WidgetGameObjectTreeItem.h"
#include "Editor/Widget/WidgetInspectorApp.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

namespace Editor::Widget
{
	class WidgetGameObjectTreeModel;

	class WidgetGameObjectTree : public QTreeView
	{
	public:
		QSettings& mSetting;
		WidgetGameObjectTreeModel mTreeModel;
		WidgetGameObjectTreeItem* mCopiedActor = nullptr;
		WidgetInspectorApp& mInspector;

		int mIdSelected = 0;
		Editor::Data::Actor* mSelectedActor = nullptr;

		WidgetGameObjectTree(QSettings& pSetting, WidgetInspectorApp& pInspector);
		~WidgetGameObjectTree();
		void initHeader(const QString& pProjectName);

		void append(WidgetGameObjectTreeItem* pItem);
		void append(WidgetGameObjectTreeItem* pParent, WidgetGameObjectTreeItem* pItem);

		void showContextMenu(const QPoint& pos);

		void create();
		void paste();

		virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

		void dragEnterEvent(QDragEnterEvent* e) override;
		void dropEvent(QDropEvent* e) override;
		void dragMoveEvent(QDragMoveEvent* e) override;

		void insertRow(int pRow, QList<QStandardItem*>& pItems);
		void insertRow(int pRow, QStandardItem* pParent, QList<QStandardItem*>& pItems);
		void reorder(int pSide, WidgetGameObjectTreeItem* pSrc, WidgetGameObjectTreeItem* pDist);

		void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
		void onItemChanged(QStandardItem* pItem);

		void onDoubleClicked(const QModelIndex& index);

		void serializeItem(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter);
		void serializeItemChild(QStandardItem* pItem, rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter);
	};
}