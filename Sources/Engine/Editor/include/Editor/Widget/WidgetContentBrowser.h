#pragma once
#include <QtWidgets/qwidget.h>
#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include <QtCore/qfilesystemwatcher.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtoolbar.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qmimedata.h>

#include "Editor/Data/NewDocument.h"
#include "Editor/Data/FlowLayout.h"
#include "Editor/Widget/WidgetSelectableBrowser.h"
#include "Editor/Widget/WidgetContentBrowserApp.h"
#include <QtWidgets/qscrollarea.h>

namespace Editor::Widget
{
	class WidgetSelectableBrowser;
	class WidgetContentBrowserApp;

	class WidgetContentBrowser : public QScrollArea
	{
		public:
			WidgetContentBrowserApp& mApp;

			QSettings& mSettings;
			QString& mCurrentDir;
			Data::FlowLayout* mLayout = new Data::FlowLayout(nullptr);
			
			Data::NewDocument mNewDoc;
			WidgetSelectableBrowser* mNewFolderContent = nullptr;
			QVector<WidgetSelectableBrowser*> mContents;

			WidgetContentBrowser(WidgetContentBrowserApp& pApp, QString& pCurrentDir, QSettings& pSettings, QWidget* pParent = nullptr);

			void dragEnterEvent(QDragEnterEvent* e) override;
			void dragLeaveEvent(QDragLeaveEvent* event) override;
			void dropEvent(QDropEvent* e) override;
			void showContextMenu(const QPoint& pos);
			void import();
			void showInExplorer();
			void createFolder();
			QString createFile(QString pName, const char* pExtension);
			void clear();
			void updateContentDir(const QList<QFileInfo>& pDirFileList);

			bool eventFilter(QObject* obj, QEvent* event) override;

			 const QString tryLoadIcon(const QString& pPath, const QString& pFileName);
	};
}