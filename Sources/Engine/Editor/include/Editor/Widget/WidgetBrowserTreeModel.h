#pragma once
#include <QtGui/qstandarditemmodel.h>
#include <QtCore/qmimedata.h>
#include <QtGui/qevent.h>

#include "Editor/Widget/WidgetBrowserTree.h"

namespace Editor::Widget
{
	class WidgetBrowserTree;

	class WidgetBrowserTreeModel : public QStandardItemModel
	{
		public:
			WidgetBrowserTree& mTree;

			WidgetBrowserTreeModel(WidgetBrowserTree& pTree);
			QVariant data(const QModelIndex& index, int role) const override;
			Qt::DropActions supportedDropActions() const override;
			Qt::ItemFlags flags(const QModelIndex& index) const override;
			QStringList mimeTypes() const override;
			bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
			bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
			QMimeData* mimeData(const QModelIndexList& indexes) const override;
	};
}