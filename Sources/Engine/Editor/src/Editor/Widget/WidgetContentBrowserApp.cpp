#include "Editor/Widget/WidgetContentBrowser.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qstandardpaths.h>
#include <QtGui/qdesktopservices.h>
#include <QtCore/qtimer.h>

#include "Editor/Utils/Utils.h"
#include "Tools/Utils/Define.h"
#include "Tools/Utils/PathParser.h"
#include "Editor/Widget/WidgetMenuSperarator.h"
#include "Editor/Widget/WidgetEditor.h"


using namespace Editor::Widget;

WidgetContentBrowserApp::WidgetContentBrowserApp(QSettings& pSettings, const Data::ProjectLocation& pProject, QWidget* pParent) :
	ads::CDockWidget("Content Browser"), 
	mSettings(pSettings),
	mRootName(pProject.mFolder.split("/").last()),
	mRootDir(pProject.mFolder),
	mProjectName(pProject.mName)
{
	mContent = new WidgetContentBrowser(*this, mCurrentDir, pSettings);

	

	mTree = new WidgetBrowserTree(*this);

	QWidget* content = new QWidget(pParent);
	setWidget(content);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	layout->setContentsMargins(5, 0, 5, 5);
	layout->setSpacing(1);
	content->setLayout(layout);

	initToolBar(pSettings, layout);
	initBottomSide(layout);
	updateContentDir(mRootDir);

	connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &WidgetContentBrowserApp::contentChanged);
}

WidgetContentBrowserApp::~WidgetContentBrowserApp()
{
	delete mContent;
	delete mTree;
}

void WidgetContentBrowserApp::initToolBar(QSettings& pSettings, QBoxLayout* pLayout)
{
	mBar->setObjectName("ToolbarContentBrowser");
	mBar->setIconSize(QSize(pSettings.value("BrowserToolBarWidth").toInt(), pSettings.value("BrowserToolBarHeight").toInt()));

	pLayout->addWidget(mBar);
}

void WidgetContentBrowserApp::initBottomSide(QBoxLayout* pLayout)
{
	QSplitter* splitter = new QSplitter();
	pLayout->addWidget(splitter);

	initLeftSide(splitter);
	initRightSide(splitter);

	splitter->setStretchFactor(0, 0);
	splitter->setStretchFactor(1, 3);
}

void WidgetContentBrowserApp::initLeftSide(QSplitter* pSplitter)
{
	QWidget* widget = new QWidget();
	widget->setObjectName("ContentBrowser");

	QVBoxLayout* box = new QVBoxLayout();
	widget->setLayout(box);
	box->addWidget(mTree);

	pSplitter->addWidget(widget);


	mTree->connect(mTree, &QTreeView::clicked, this, &WidgetContentBrowserApp::treeItemClick);
	updateTree();
}

void WidgetContentBrowserApp::initRightSide(QSplitter* pSplitter)
{
	mContent->installEventFilter(this);
	pSplitter->addWidget(mContent);
}

void WidgetContentBrowserApp::updateContentDir(const QString& pDir)
{
	mCurrentDir = pDir;

	const QList<QFileInfo> pDirFileList = QDir(pDir).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);
	mContent->updateContentDir(pDirFileList);

	updateToolBar(pDir);
}

void WidgetContentBrowserApp::updateContentDir()
{
	updateContentDir(mCurrentDir);
}

void WidgetContentBrowserApp::treeItemClick(const QModelIndex& index)
{
	QString path = ((WidgetBrowserTreeItem*)mTree->mTreeModel->itemFromIndex(index))->mPath;
	updateContentDir(path);
}

void WidgetContentBrowserApp::updateTree()
{
	mTree->mTreeModel->clear();
	mTree->initHeader(mProjectName);

	WidgetBrowserTreeItem* rootItem = new WidgetBrowserTreeItem(QFileInfo(mRootDir).absoluteFilePath(), "All");
	mTree->mTreeModel->appendRow(rootItem);

	QStringList dirList = watcher.directories();
	if (dirList.size() != 0)
		watcher.removePaths(dirList);
	watcher.addPath(mRootDir + mSettings.value("previewFolder").toString());

	addItem(mRootDir, rootItem);

	if (mTree->selectedIdx.isEmpty())
		mTree->setClicked(rootItem->mPath);
}

void WidgetContentBrowserApp::addItem(const QString& pDir, WidgetBrowserTreeItem* pParent)
{
	watcher.addPath(pDir);

	const QList<QFileInfo> dirList = QDir(pDir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (unsigned int i = 0; i < dirList.size(); i++)
	{
		WidgetBrowserTreeItem* item = new WidgetBrowserTreeItem(dirList[i].absoluteFilePath(), dirList[i].baseName());

		if (!mTree->state.contains(dirList[i].absoluteFilePath()))
			mTree->state[dirList[i].absoluteFilePath()] = false;

		if (pParent != nullptr)
			pParent->appendRow(item);
		else
			mTree->mTreeModel->appendRow(item);

		addItem(dirList[i].absoluteFilePath(), item);
	}
}

void WidgetContentBrowserApp::updateToolBar(const QString& pDir)
{

	for (unsigned int i = 0; i < mBar->actions().size(); i++)
	{
		disconnect(mBar->actions()[i], &QAction::triggered, 0, 0);
		delete mBar->actions()[i];
	}
	mBar->clear();

	bool rootFound = false;
	QList<QString> strs = pDir.split("/");
	for (unsigned int i = 0; i < strs.size(); i++)
	{
		QAction* action = nullptr;
		if (!rootFound && strs[i] != mRootName)
			continue;
		else if (strs[i] == mRootName && !rootFound)
		{
			rootFound = true;
			action = new QAction("All");
		}
		else
			action = new QAction(strs[i]);

		mBar->addAction(action);
		mBar->widgetForAction(action)->setObjectName("BrowserTab");
		connect(action, &QAction::triggered, this, [this, strs, i]
			{
				QString path;
				for (unsigned int j = 0; j <= i; j++)
					path += strs[j] + (j <= i - 1 ? "/" : "");

				mTree->setClicked(path);
				updateContentDir(path);
			});

		if (i < strs.size() - 1)
		{
			action = new QAction(QIcon(mSettings.value("BrowserSplitter").toString()), "");
			action->setDisabled(true);
			mBar->addAction(action);
			mBar->widgetForAction(action)->setObjectName("BrowserSplitter");
		}
	}
}

bool WidgetContentBrowserApp::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ProjectSelectionWidget" || obj->objectName() == "ProjectWidgetLabel" || obj == mContent)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			for (unsigned int i = 0; i < mContent->mContents.size(); i++)
			{
				mContent->mContents[i]->setClicked(false);
			}

			QMouseEvent* mouseEvent = (QMouseEvent*)event;
			if (mouseEvent->button() != Qt::LeftButton)
			{
				QObject::eventFilter(obj, event);
				if (obj == mContent)
					mContent->showContextMenu(mouseEvent->pos());

				return true;
			}

			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

void WidgetContentBrowserApp::contentChanged(QString str)
{
	if (!mShouldUpdate)
	{
		mShouldUpdate = true;

		QTimer::singleShot(50, this, [this, str]
			{
				if (str == mCurrentDir || str.contains(mSettings.value("previewFolder").toString(), Qt::CaseInsensitive))
					updateContentDir();


				updateTree();


				for (QString e : mTree->state.keys())
				{
					QModelIndex idx = mTree->forEach(e);
					if (!idx.isValid())
						continue;

					if (!mTree->state[e])
						continue;

					if (!((WidgetBrowserTreeItem*)mTree->mTreeModel->itemFromIndex(idx))->hasChildren())
						continue;

					mTree->expand(idx);
				}

				mTree->setClicked(mTree->selectedIdx);

				mShouldUpdate = false;
			});
	}
}