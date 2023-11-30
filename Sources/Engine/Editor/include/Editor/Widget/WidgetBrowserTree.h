#pragma once
#include "Editor/Widget/WidgetContentBrowserApp.h"
#include <QtWidgets/qtreeview.h>
#include "Editor/Widget/WidgetBrowserTreeModel.h"

namespace Editor::Widget
{
	class WidgetBrowserTreeModel;
	class WidgetContentBrowserApp;

	class WidgetBrowserTree : public QTreeView
	{
	public:
		WidgetContentBrowserApp& mApp;
		WidgetBrowserTreeModel* mTreeModel = nullptr;
		QMap<QString, bool> state;
		QString selectedIdx;

		WidgetBrowserTree(WidgetContentBrowserApp& pApp);
		~WidgetBrowserTree();
		void initHeader(const QString& pProjectName);
		void setClicked(const QString& pItem);

		QModelIndex forEach(const QString& pItem, QModelIndex parent = QModelIndex());
		void dragEnterEvent(QDragEnterEvent* e) override;
		void dropEvent(QDropEvent* e) override;
		void dragMoveEvent(QDragMoveEvent* e) override;

		void collapse(const QModelIndex& index);
		void expand(const QModelIndex& index);

		void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	};
}