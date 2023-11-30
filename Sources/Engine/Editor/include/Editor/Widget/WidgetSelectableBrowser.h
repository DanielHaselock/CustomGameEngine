#pragma once
#include "Editor/Widget/WidgetSelectable.h"
#include "Editor/Widget/WidgetContentBrowser.h"
#include <QtGui/qevent.h>
#include <QtCore/qmimedata.h>

namespace Editor::Widget
{
	class WidgetContentBrowser;

	class WidgetSelectableBrowser : public WidgetSelectable
	{
		public:
			WidgetContentBrowser& mApp;
			QSettings& mSetting;
			QString mPath;
			bool mIsDirectory;
			QPoint mDragStartPosition;

			WidgetSelectableBrowser(WidgetContentBrowser& pApp, const QString& pPath, const bool& pIsDirectory, QSettings& pSetting, const QString& pName, const QString& pImage, bool pIsAnimation = false, QWidget* pParent = nullptr);
			void dragEnterEvent(QDragEnterEvent* e) override;
			void dragLeaveEvent(QDragLeaveEvent* event) override;
			void dragMoveEvent(QDragMoveEvent* e) override;
			void dropEvent(QDropEvent* e) override;
			void dropFromExtern(QDropEvent* e);
			void dropFromIntern(QDropEvent* e);

			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;

			void showContextMenu(const QPoint& pos);
			void import();
			void showInExplorer();
			void rename();
			void deleteAction();

			bool eventFilter(QObject* obj, QEvent* event) override;
			void finishRename();

			virtual void doubleClick();
	};
}