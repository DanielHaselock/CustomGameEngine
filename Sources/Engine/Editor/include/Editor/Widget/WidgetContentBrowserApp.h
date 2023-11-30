#pragma once
#include <QtWidgets/qwidget.h>
#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include <QtCore/qfilesystemwatcher.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtoolbar.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qmimedata.h>

#include "Editor/Widget/WidgetContentBrowser.h"
#include "Editor/Widget/WidgetBrowserTree.h"
#include "Editor/Widget/WidgetBrowserTreeItem.h"
#include "Editor/Data/ProjectLocation.h"

namespace Editor::Widget
{
	class WidgetContentBrowser;
	class WidgetBrowserTree;

	class WidgetContentBrowserApp : public ads::CDockWidget
	{
		public:
			QSettings& mSettings;
			QToolBar* mBar = new QToolBar();

			WidgetContentBrowser* mContent = nullptr;
			WidgetBrowserTree* mTree = nullptr;

			QString mCurrentDir;
			QString mRootDir;
			QString mRootName;
			QString mProjectName;
			QFileSystemWatcher watcher;
			bool mShouldUpdate = false;

			WidgetContentBrowserApp(QSettings& pSettings, const Data::ProjectLocation& pProject, QWidget* pParent = nullptr);
			~WidgetContentBrowserApp();
			void initToolBar(QSettings& pSettings, QBoxLayout* pLayout);
			void initBottomSide(QBoxLayout* pLayout);
			void initLeftSide(QSplitter* pSplitter);
			void initRightSide(QSplitter* pSplitter);

			void updateContentDir(const QString& pDir);
			void updateToolBar(const QString& pDir);
			void updateContentDir();

			void treeItemClick(const QModelIndex& index);
			void updateTree();
			void addItem(const QString& pDir, WidgetBrowserTreeItem* pParent = nullptr);

			bool eventFilter(QObject* obj, QEvent* event) override;
			void contentChanged(QString str);
	};
}