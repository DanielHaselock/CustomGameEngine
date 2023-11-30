#pragma once
#include <QtGui/qstandarditemmodel.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsettings.h>
#include "Game/Data/Actor.h"
#include "Editor/Widget/WidgetInspectorData.h"
#include "Editor/Data/Actor.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

namespace Editor::Widget
{
	class WidgetGameObjectTreeItem : public QStandardItem
	{
		public:
			QStandardItemModel* mModel = nullptr;
			Game::Data::Actor* mActor = nullptr;
			Editor::Data::Actor* mEditorActor = nullptr;
			WidgetInspectorData* mDatas;
			
			QSettings& mSetting;

			WidgetGameObjectTreeItem(QSettings& pSetting, const QString& pText);
			WidgetGameObjectTreeItem(QSettings& pSetting, const QString& pText, Game::Data::Actor* pActor);
			WidgetGameObjectTreeItem(const WidgetGameObjectTreeItem& pOther);
			~WidgetGameObjectTreeItem();
			void onItemChanged();
			void showContextMenu(QSettings& pSetting, const QPoint& pPoint, QWidget* pParent);

			void create(QWidget* pParent);
			void rename(QWidget* pParent);
			void copy(QWidget* pParent);
			void paste(QWidget* pParent);
			void deleteItem(QWidget* pParent);

			void appendRow(WidgetGameObjectTreeItem* pItem);
			void appendRow(QList<QStandardItem*>& pItems);

			void saveNew();
			void saveTo();

			void save(QString path);

			void serializeItemChild(QStandardItem* pItem, rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter);
	};
}