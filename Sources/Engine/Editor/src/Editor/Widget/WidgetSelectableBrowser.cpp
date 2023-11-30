#pragma once
#include <QtCore/qstandardpaths.h>
#include <QtWidgets/qfiledialog.h>
#include <QtGui/qdesktopservices.h>
#include <QtCore/qdiriterator.h>
#include "Editor/Widget/WidgetSelectableBrowser.h"
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qmessagebox.h>
#include <QtGui/qdrag.h>

#include "Editor/Utils/Utils.h"
#include "Editor/Widget/WidgetMenuSperarator.h"
#include "Tools/Utils/PathParser.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Resources/Model.h"
#include "Editor/Widget/WidgetEditor.h"

using namespace Editor::Widget;

WidgetSelectableBrowser::WidgetSelectableBrowser(WidgetContentBrowser& pApp, const QString& pPath, const bool& pIsDirectory, QSettings& pSetting, const QString& pName, const QString& pImage, bool pIsAnimation, QWidget* pParent) :
	WidgetSelectable(pSetting, pName, pImage, pIsAnimation, pParent), mApp(pApp), mSetting(pSetting), mPath(pPath), mIsDirectory(pIsDirectory)
{
	if (mIsDirectory)
	{
		mImage.setObjectName("ProjectWidgetDirectory");
		setAcceptDrops(true);
	}

	mText.installEventFilter(this);

	connect(&mText, &QLineEdit::editingFinished, this, &WidgetSelectableBrowser::finishRename);
}

void WidgetSelectableBrowser::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls())
		e->acceptProposedAction();
	else if (e->mimeData()->hasText())
		e->acceptProposedAction();

	setProperty("hover", true);
	style()->unpolish(this);
	style()->polish(this);
}

void WidgetSelectableBrowser::dragLeaveEvent(QDragLeaveEvent* event)
{
	setProperty("hoverr", false);
	style()->unpolish(this);
	style()->polish(this);

	QWidget::dragLeaveEvent(event);
}

void WidgetSelectableBrowser::dragMoveEvent(QDragMoveEvent* e)
{
	QWidget::dragMoveEvent(e);
}

void WidgetSelectableBrowser::dropEvent(QDropEvent* e)
{
	if (e->mimeData()->hasUrls())
		dropFromExtern(e);
	else if (e->mimeData()->hasText())
		dropFromIntern(e);

	QWidget::dropEvent(e);
}

void WidgetSelectableBrowser::dropFromExtern(QDropEvent* e)
{
	foreach(const QUrl & url, e->mimeData()->urls())
	{
		Utils::copyFileAndDir(url.toLocalFile(), mPath, this);
	
		QFileInfo info(url.toLocalFile());
		Tools::Utils::PathParser::EFileType type = Utils::getFileType(info.suffix());
		if (type == Tools::Utils::PathParser::EFileType::MODEL)
			service(Editor::Widget::WidgetEditor).loadFile<Rendering::Resources::Model>(mPath + "/" + info.fileName(), info.baseName());
	}
}

void WidgetSelectableBrowser::dropFromIntern(QDropEvent* e)
{

	QString str = e->mimeData()->text();
	mApp.mApp.watcher.removePath(str);

	QDirIterator it(str, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		it.next();
		mApp.mApp.watcher.removePath(it.fileInfo().absoluteFilePath());
	}

	Utils::copyFileAndDir(str, mPath, this, mIsDirectory);
	mApp.mApp.updateContentDir();
}

void WidgetSelectableBrowser::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
		mDragStartPosition = event->pos();
}

void WidgetSelectableBrowser::mouseMoveEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;

	if ((event->pos() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;

	QDrag* drag = new QDrag(&mImage);
	QMimeData* mimeData = new QMimeData;


	mimeData->setText(mPath);

	setClicked(false);
	QPixmap* widgetPixmap = new QPixmap(size());
	render(widgetPixmap);
	setClicked(true);

	drag->setMimeData(mimeData);
	drag->setPixmap(*widgetPixmap);

	drag->exec(Qt::MoveAction);
}

void WidgetSelectableBrowser::showContextMenu(const QPoint& pos)
{
	QMenu contextMenu(tr("Context menu"), this);

	WidgetMenuSeperator sepaImport(mSetting, mSetting.value("CategoryImport").toString(), mSetting.value("SeparatorIcon").toString(), &contextMenu);
	contextMenu.addAction(sepaImport.mAction);

	QAction import(QIcon(mSetting.value("CategoryImportIcon").toString()), mSetting.value("CategoryImportName").toString(), this);
	connect(&import, &QAction::triggered, this, &WidgetSelectableBrowser::import);
	contextMenu.addAction(&import);

	WidgetMenuSeperator sepaFolder(mSetting, mSetting.value("CategoryFolder").toString(), mSetting.value("SeparatorIcon").toString(), &contextMenu);
	contextMenu.addAction(sepaFolder.mAction);

	QAction showInDir(QIcon(mSetting.value("CategoryFolderIcon").toString()), mSetting.value("CategoryFolderName").toString(), this);
	connect(&showInDir, &QAction::triggered, this, &WidgetSelectableBrowser::showInExplorer);
	contextMenu.addAction(&showInDir);

	QAction rename(QIcon(mSetting.value("CategoryRenameIcon").toString()), mSetting.value("CategoryRenameName").toString(), this);
	connect(&rename, &QAction::triggered, this, &WidgetSelectableBrowser::rename);
	contextMenu.addAction(&rename);

	QAction deleteAction(QIcon(mSetting.value("CategoryDeleteIcon").toString()), mSetting.value("CategoryDeleteName").toString(), this);
	connect(&deleteAction, &QAction::triggered, this, &WidgetSelectableBrowser::deleteAction);
	contextMenu.addAction(&deleteAction);

	contextMenu.exec(pos);
}

void WidgetSelectableBrowser::showInExplorer()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(mPath).absolutePath()));
}

void WidgetSelectableBrowser::import()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
	if (!fileName.isEmpty())
	{
		Utils::copyFileAndDir(fileName, mPath, this);
		mApp.mApp.mTree->setClicked(mPath);
		mApp.mApp.updateContentDir(mPath);

		QFileInfo info(fileName);
		Tools::Utils::PathParser::EFileType type = Utils::getFileType(info.suffix());
		if (type == Tools::Utils::PathParser::EFileType::MODEL)
			service(Editor::Widget::WidgetEditor).loadFile<Rendering::Resources::Model>(mPath + "/" + info.fileName(), info.baseName());
	}
}

void WidgetSelectableBrowser::rename()
{
	mApp.mNewDoc = Data::NewDocument(mPath, true);
	mApp.mApp.updateContentDir();
}

void WidgetSelectableBrowser::deleteAction()
{
	if (mIsDirectory)
	{
		mApp.mApp.watcher.removePath(mPath);

		QStringList dirList = mApp.mApp.watcher.directories();
		if (dirList.size() != 0)
			mApp.mApp.watcher.removePaths(dirList);

		QStringList fileList = mApp.mApp.watcher.files();
		if (fileList.size() != 0)
			mApp.mApp.watcher.removePaths(fileList);

		QDir dir(mPath);
		dir.removeRecursively();

		mApp.mApp.contentChanged("");
		mApp.mApp.updateContentDir();
	}
	else
		QFile::remove(mPath);
}

bool WidgetSelectableBrowser::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = (QMouseEvent*)event;
		mousePressEvent(mouseEvent);
	}
	else if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = (QMouseEvent*)event;
		mouseMoveEvent(mouseEvent);
	}

	return QWidget::eventFilter(obj, event);
}

void WidgetSelectableBrowser::finishRename()
{
	if (!mText.isModified())
		return;

	QString src;
	QString dst;
	QString oldName;
	if (mIsDirectory)
	{
		QDir currentDirectory(mPath);
		oldName = mPath.split("/").last();
		src = QDir::cleanPath(currentDirectory.filePath("."));
		dst = QDir::cleanPath(currentDirectory.filePath(QStringLiteral("..%1%2").arg(QDir::separator()).arg(mText.text())));
	}
	else
	{
		QFileInfo currentFile(mPath);
		oldName = currentFile.baseName();
		src = mPath;
		dst = currentFile.absolutePath() + "/" + mText.text() + "." + currentFile.suffix();
	}

	if (!QFile::exists(dst))
		QFile::rename(src, dst);
	else
	{
		QMessageBox msgBox(QMessageBox::Warning,
			"",
			QString("Name already exists. \n"),
			QMessageBox::Ok, this,
			Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

		msgBox.exec();

		mText.setText(oldName);
	}

	mText.setReadOnly(true);
	mText.setProperty("readOnly", true);

	style()->unpolish(&mText);
	style()->polish(&mText);
	mApp.mNewFolderContent = nullptr;
}

void WidgetSelectableBrowser::doubleClick()
{
	if (mIsDirectory)
	{
		mApp.mApp.mTree->setClicked(mPath);
		mApp.mApp.updateContentDir(mPath);
	}
	else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(mPath).absoluteFilePath()));
}