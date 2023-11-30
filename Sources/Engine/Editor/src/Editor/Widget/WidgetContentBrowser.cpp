#include "Editor/Widget/WidgetContentBrowser.h"
#include "Editor/Widget/WidgetSelectableBrowserModel.h"
#include "Editor/Widget/WidgetSelectableBrowserMaterial.h"
#include "Editor/Widget/WidgetSelectableBrowserScene.h"
#include "Editor/Widget/WidgetSelectableBrowserUI.h"
#include "Editor/Widget/WidgetSelectableBrowserPrefab.h"
#include "Editor/Widget/WidgetSelectableBrowserParticle.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qstandardpaths.h>
#include <QtGui/qdesktopservices.h>

#include "Editor/Utils/Utils.h"
#include "Tools/Utils/Define.h"
#include "Tools/Utils/PathParser.h"
#include "Editor/Widget/WidgetMenuSperarator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Editor/Widget/WidgetEditor.h"
#include <QtWidgets/qscrollarea.h>
#include "Rendering/Resources/Material.h"
#include "EngineCore/Service/ServiceLocator.h"

using namespace Editor::Widget;

WidgetContentBrowser::WidgetContentBrowser(WidgetContentBrowserApp& pApp, QString& pCurrentDir, QSettings& pSettings, QWidget* pParent) : 
	mApp(pApp), 
	mCurrentDir(pCurrentDir),
	mSettings(pSettings)
{
	QWidget* content = new QWidget();
	content->setLayout(mLayout);
	content->setObjectName("ContentBrowser");

	setAcceptDrops(true);
	setObjectName("ContentBrowser");


	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setWidgetResizable(true);
	setWidget(content);
}

void WidgetContentBrowser::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls())
		e->acceptProposedAction();
	else if (e->mimeData()->hasText())
		e->acceptProposedAction();
}

void WidgetContentBrowser::dragLeaveEvent(QDragLeaveEvent* event)
{
	QWidget::dragLeaveEvent(event);
}

void WidgetContentBrowser::dropEvent(QDropEvent* e)
{
	foreach(const QUrl & url, e->mimeData()->urls())
	{
		Utils::copyFileAndDir(url.toLocalFile(), mCurrentDir, this);

		QFileInfo info(url.toLocalFile());
		Tools::Utils::PathParser::EFileType type = Utils::getFileType(info.suffix());
		if (type == Tools::Utils::PathParser::EFileType::MODEL)
			service(Editor::Widget::WidgetEditor).loadFile<Rendering::Resources::Model>(mCurrentDir + "/" + info.fileName(), info.baseName());
	}


	mApp.updateContentDir();
}

void WidgetContentBrowser::showContextMenu(const QPoint& pos)
{
	QMenu contextMenu(tr("Context menu"), this);

	WidgetMenuSeperator sepaImport(mSettings, mSettings.value("CategoryImport").toString(), mSettings.value("SeparatorIcon").toString(), &contextMenu);
	contextMenu.addAction(sepaImport.mAction);

	QAction import(QIcon(mSettings.value("CategoryImportIcon").toString()), mSettings.value("CategoryImportName").toString(), this);
	connect(&import, &QAction::triggered, this, &WidgetContentBrowser::import);
	contextMenu.addAction(&import);

	WidgetMenuSeperator sepaCategory(mSettings, mSettings.value("CategoryFolder").toString(), mSettings.value("SeparatorIcon").toString(), &contextMenu);
	contextMenu.addAction(sepaCategory.mAction);

	QAction showInDir(QIcon(mSettings.value("CategoryFolderIcon").toString()), mSettings.value("CategoryFolderName").toString(), this);
	connect(&showInDir, &QAction::triggered, this, &WidgetContentBrowser::showInExplorer);
	contextMenu.addAction(&showInDir);

	QAction newFolder(QIcon(mSettings.value("CategoryNewFolderIcon").toString()), mSettings.value("CategoryNewFolderName").toString(), this);
	connect(&newFolder, &QAction::triggered, this, &WidgetContentBrowser::createFolder);
	contextMenu.addAction(&newFolder);


	WidgetMenuSeperator sepaAssets(mSettings, mSettings.value("CategoryAsset").toString(), mSettings.value("SeparatorIcon").toString(), &contextMenu);
	contextMenu.addAction(sepaAssets.mAction);

	QAction assetClass(QIcon(mSettings.value("CategoryAssetClassIcon").toString()), mSettings.value("CategoryAssetClassName").toString(), this);
	connect(&assetClass, &QAction::triggered, this, [this] { createFile("/NewClass", Define::CLASS_EXTENSION); });
	contextMenu.addAction(&assetClass);

	QAction assetLevel(QIcon(mSettings.value("LevelIcon").toString()), mSettings.value("CategoryAssetLevelName").toString(), this);
	connect(&assetLevel, &QAction::triggered, this, [this] { createFile("/NewLevel", Define::SCENE_EXTENSION); });
	contextMenu.addAction(&assetLevel);

	QAction assetMaterial(QIcon(mSettings.value("CategoryAssetMaterialIcon").toString()), mSettings.value("CategoryAssetMaterialName").toString(), this);
	connect(&assetMaterial, &QAction::triggered, this, [this] { createFile("/NewMaterial", Define::MATERIAL_EXTENSION); });
	contextMenu.addAction(&assetMaterial);

	QAction assetParticle(QIcon(mSettings.value("CategoryAssetParticleIcon").toString()), mSettings.value("CategoryAssetParticleName").toString(), this);
	connect(&assetParticle, &QAction::triggered, this, [this] { createFile("/NewParticle", Define::PARTICLE_EXTENSION); });
	contextMenu.addAction(&assetParticle);

	QAction assetUI(QIcon(mSettings.value("CategoryAssetUIIcon").toString()), mSettings.value("CategoryAssetUIName").toString(), this);
	connect(&assetUI, &QAction::triggered, this, [this] { createFile("/NewUI", Define::UI_EXTENSION); });
	contextMenu.addAction(&assetUI);

	contextMenu.exec(mapToGlobal(pos));
}

void WidgetContentBrowser::import()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Import File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
	if (!fileName.isEmpty())
	{
		Utils::copyFileAndDir(fileName, mCurrentDir, this);
	
		QFileInfo info(fileName);
		Tools::Utils::PathParser::EFileType type = Utils::getFileType(info.suffix());
		if (type == Tools::Utils::PathParser::EFileType::MODEL)
			service(Editor::Widget::WidgetEditor).loadFile<Rendering::Resources::Model>(mCurrentDir + "/" + info.fileName(), info.baseName());
	}
}

void WidgetContentBrowser::showInExplorer()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(mCurrentDir));
}

void WidgetContentBrowser::createFolder()
{
	QString path = Utils::uniqueString(mCurrentDir + "/New folder");
	mNewDoc = Data::NewDocument(path, true);
	QDir().mkdir(path);
}

QString WidgetContentBrowser::createFile(QString pName, const char* pExtension)
{
	QString path = Utils::uniqueString(mCurrentDir + pName, pExtension);
	mNewDoc = Data::NewDocument(path, true);
	QFile file(path);
	file.open(QIODevice::WriteOnly);

	return path;
}

void WidgetContentBrowser::clear()
{
	for (unsigned int i = 0; i < mContents.size(); i++)
	{
		mLayout->removeWidget(mContents[i]);
		mContents[i]->setParent(nullptr);
		mContents[i]->deleteLater();
	}
	mContents.clear();
}

void WidgetContentBrowser::updateContentDir(const QList<QFileInfo>& pDirFileList)
{
	clear();

	unsigned int idx = 0;
	for (unsigned int i = 0; i < pDirFileList.size(); i++)
	{
		if (pDirFileList[i].suffix().contains(Define::PROJECT_EXTENSION))
			continue;

		WidgetSelectableBrowser* content = nullptr;
		if (pDirFileList[i].isDir())
			content = new WidgetSelectableBrowser(*this, pDirFileList[i].absoluteFilePath(), true, mSettings, pDirFileList[i].fileName(), mSettings.value("BrowserDirIcon").toString());
		else
		{
			Tools::Utils::PathParser::EFileType type = Utils::getFileType(pDirFileList[i].suffix());

			switch (type)
			{
				case Tools::Utils::PathParser::EFileType::UNKNOWN:
					content = new WidgetSelectableBrowser(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserNonDirIcon").toString());
					break;
			
				case Tools::Utils::PathParser::EFileType::MODEL:
					content = new WidgetSelectableBrowserModel(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), tryLoadIcon(pDirFileList[i].absoluteFilePath(), pDirFileList[i].baseName()), true);
					break;

				case Tools::Utils::PathParser::EFileType::TEXTURE:
					content = new WidgetSelectableBrowser(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), pDirFileList[i].absoluteFilePath());
					break;

				case Tools::Utils::PathParser::EFileType::MATERIAL:
					content = new WidgetSelectableBrowserMaterial(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), tryLoadIcon(pDirFileList[i].absoluteFilePath(), pDirFileList[i].baseName()), true);
					break;
			
				case Tools::Utils::PathParser::EFileType::SOUND:
					content = new WidgetSelectableBrowser(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserSoundIcon").toString());
					break;

				case Tools::Utils::PathParser::EFileType::SCENE:
					content = new WidgetSelectableBrowserScene(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserLevelIcon").toString());
					break;

				case Tools::Utils::PathParser::EFileType::SCRIPT:
					content = new WidgetSelectableBrowser(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserClassIcon").toString());
					break;

				case Tools::Utils::PathParser::EFileType::FONT:
					content = new WidgetSelectableBrowser(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserLoadingIcon").toString(), true);
					break;

				case Tools::Utils::PathParser::EFileType::UI:
					content = new WidgetSelectableBrowserUI(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserUIIcon").toString(), true);
					break;

				case Tools::Utils::PathParser::EFileType::PREFAB:
					content = new WidgetSelectableBrowserPrefab(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserPrefabIcon").toString(), false);
					break;

				case Tools::Utils::PathParser::EFileType::PARTICLE:
					content = new WidgetSelectableBrowserParticle(*this, pDirFileList[i].absoluteFilePath(), false, mSettings, pDirFileList[i].baseName(), mSettings.value("BrowserParticleIcon").toString(), false);
					break;
			}
		}

		mLayout->addWidget(content);
		content->installEventFilter(this);
		content->mText.installEventFilter(this);
		mContents.push_back(content);

		idx++;

		if (mNewDoc.mIsNew)
		{
			if (pDirFileList[i].absoluteFilePath() == mNewDoc.mPath)
			{
				content->setClicked(true);
				content->mText.setReadOnly(false);
				content->mText.setFocus();
				content->mText.selectAll();

				mNewDoc.mIsNew = false;
				mNewFolderContent = content;
			}
		}
	}
}

bool WidgetContentBrowser::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ProjectSelectionWidget" || obj->objectName() == "ProjectWidgetLabel" || obj == this)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			for (unsigned int i = 0; i < mContents.size(); i++)
			{
				if (obj == mContents[i] || obj == &mContents[i]->mText)
					mContents[i]->setClicked(true);
				else
					mContents[i]->setClicked(false);
			}

			QMouseEvent* mouseEvent = (QMouseEvent*)event;
			if (mouseEvent->button() != Qt::LeftButton)
			{
				QObject::eventFilter(obj, event);

				if (obj->objectName() == "ProjectWidgetLabel")
					((WidgetSelectableBrowser*)obj->parent())->showContextMenu(((QWidget*)obj)->mapToGlobal(mouseEvent->pos()));
				else
					((WidgetSelectableBrowser*)obj)->showContextMenu(((QWidget*)obj)->mapToGlobal(mouseEvent->pos()));

				return true;
			}

			setFocus();
			if (mNewFolderContent != nullptr)
			{
				mNewFolderContent->mText.setReadOnly(true);
				mNewFolderContent->mText.setProperty("readOnly", true);

				style()->unpolish(&mNewFolderContent->mText);
				style()->polish(&mNewFolderContent->mText);
				mNewFolderContent = nullptr;
			}

			return true;
		}
		else if (event->type() == QEvent::MouseButtonDblClick)
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() != Qt::LeftButton)
				return QObject::eventFilter(obj, event);

			WidgetSelectableBrowser* content = dynamic_cast<WidgetSelectableBrowser*>(obj);
			WidgetSelectableBrowser* content2 = dynamic_cast<WidgetSelectableBrowser*>(obj->parent());
			if (content == nullptr && content2 == nullptr)
				return QObject::eventFilter(obj, event);

			content = content != nullptr ? content : content2;
			content->doubleClick();

			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

const QString WidgetContentBrowser::tryLoadIcon(const QString& pPath, const QString& pFileName)
{
	EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
	if (manager.get<Rendering::Resources::IResource>(Utils::qStringToStdString(pPath)) != nullptr)
	{
		QString preview = mApp.mRootDir + mSettings.value("previewFolder").toString() + "/" + pFileName + ".png";
		if (QFile::exists(preview))
			return preview;
	}
	
	return mSettings.value("BrowserLoadingIcon").toString();
}