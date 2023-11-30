#pragma once

#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include "Editor/Widget/WidgetGameObjectTreeItem.h"
#include <QtWidgets/qboxlayout.h>

namespace Editor::Widget
{
	class WidgetInspectorApp : public ads::CDockWidget
	{
	public:
		QSettings& mSetting;
		WidgetGameObjectTreeItem* mSelectedItem = nullptr;
		WidgetInspectorData* mData = nullptr;
		QVBoxLayout* mLayout = new QVBoxLayout();

		WidgetInspectorApp(QSettings& pSetting, QWidget* pParent = nullptr);
		~WidgetInspectorApp();

		void dragEnterEvent(QDragEnterEvent* pEvent);
		void dropEvent(QDropEvent* pEvent);

		void updateInspector(WidgetGameObjectTreeItem* pItem);
	};
}