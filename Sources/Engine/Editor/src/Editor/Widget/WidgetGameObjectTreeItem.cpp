#include "Editor/Widget/WidgetGameObjectTreeItem.h"
#include <QtWidgets/qmenu.h>
#include <QtGui/qicon.h>
#include <QtCore/qobject.h>
#include <QtGui/qwindow.h>

#include "Editor/Widget/WidgetMenuSperarator.h"
#include "Editor/Widget/WidgetGameObjectTree.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Game/SceneSys/SceneManager.h"
#include "EngineCore/Thread/ThreadPool.h"
#include "Editor/Widget/WidgetConsole.h"
#include "Editor/Utils/Utils.h"
#include <QtWidgets/qlabel.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qpushbutton.h>
#include <Editor/Widget/WidgetSceneApp.h>
#include "Editor/Widget/WidgetContentBrowser.h"
#include "Editor/Data/ProjectLocation.h"
#include "QtWidgets/qfiledialog.h"
#include <QtCore/qsavefile.h>
#include <QtWidgets/qmessagebox.h>


using namespace Editor::Widget;

WidgetGameObjectTreeItem::WidgetGameObjectTreeItem(QSettings& pSetting, const QString& pText) 
	: QStandardItem(pText), mSetting(pSetting)
{
	mActor = new Game::Data::Actor();
	mEditorActor = new Editor::Data::Actor(*mActor, this);
#ifdef NSHIPPING
	mActor->mItemOwner = this;
#endif
	service(Editor::Widget::WidgetSceneApp).addActor(mEditorActor);
	service(Game::SceneSys::SceneManager).mCurrentScene->addActor(mActor);
	service(EngineCore::Thread::ThreadPool).queueJob([pText]
	{
		WidgetConsole::infoPrint("%s Created", Utils::qStringToStdString(pText));
	});

	mDatas = new WidgetInspectorData(pSetting, this);
}

WidgetGameObjectTreeItem::WidgetGameObjectTreeItem(QSettings& pSetting, const QString& pText, Game::Data::Actor* pActor)
	: QStandardItem(pText), mSetting(pSetting)
{
	mActor = pActor;
	mEditorActor = new Editor::Data::Actor(*mActor, this);
#ifdef NSHIPPING
	mActor->mItemOwner = this;
#endif
	service(Editor::Widget::WidgetSceneApp).addActor(mEditorActor);
	service(EngineCore::Thread::ThreadPool).queueJob([pText]
		{
			WidgetConsole::infoPrint("%s Created", Utils::qStringToStdString(pText));
		});

	mDatas = new WidgetInspectorData(pSetting, this);
}

WidgetGameObjectTreeItem::WidgetGameObjectTreeItem(const WidgetGameObjectTreeItem& pOther) 
	: QStandardItem(pOther.text()), mSetting(pOther.mSetting)
{
	QString str = pOther.text();
	mActor = new Game::Data::Actor(*pOther.mActor);
	mEditorActor = new Editor::Data::Actor(*mActor, this);

	service(Editor::Widget::WidgetSceneApp).addActor(mEditorActor);
	service(Game::SceneSys::SceneManager).mCurrentScene->addActor(mActor);
	service(EngineCore::Thread::ThreadPool).queueJob([str]
	{
		WidgetConsole::infoPrint("%s Created", Utils::qStringToStdString(str));
	});

	mDatas = new WidgetInspectorData(pOther.mSetting, this);
}

WidgetGameObjectTreeItem::~WidgetGameObjectTreeItem()
{
	//TODO remove item from scene
	service(Editor::Widget::WidgetSceneApp).removeActor(mEditorActor);
	service(Game::SceneSys::SceneManager).mCurrentScene->removeActor(mActor);
	delete mDatas;
}

void WidgetGameObjectTreeItem::onItemChanged()
{
	if (mModel == nullptr)
		return;

	((WidgetGameObjectTreeModel*)mModel)->mShouldEdit = false;
	mModel = nullptr;

	mDatas->mGameObjectName->setText(text());
}

void WidgetGameObjectTreeItem::showContextMenu(QSettings& pSetting, const QPoint& pPoint, QWidget* pParent)
{
	QMenu contextMenu(pParent);

	WidgetMenuSeperator sepaAssets(pSetting, pSetting.value("CategoryData").toString(), pSetting.value("SeparatorIcon").toString(), &contextMenu);
	contextMenu.addAction(sepaAssets.mAction);

	QAction saveNew(QIcon(pSetting.value("CategorySaveNewIcon").toString()), pSetting.value("CategorySaveNewName").toString());
	pParent->connect(&saveNew, &QAction::triggered, pParent, [pParent, this] { this->saveNew();  });
	contextMenu.addAction(&saveNew);

	QAction saveTo(QIcon(pSetting.value("CategorySaveIcon").toString()), pSetting.value("CategorySaveName").toString());
	pParent->connect(&saveTo, &QAction::triggered, pParent, [pParent, this] { this->saveTo();  });
	contextMenu.addAction(&saveTo);

	QAction create(QIcon(pSetting.value("CategoryCreateIcon").toString()), pSetting.value("CategoryCreateName").toString());
	pParent->connect(&create, &QAction::triggered, pParent, [pParent, this] { this->create(pParent);  });
	contextMenu.addAction(&create);

	QAction rename(QIcon(pSetting.value("CategoryRenameIcon").toString()), pSetting.value("CategoryRenameName").toString());
	contextMenu.addAction(&rename);
	pParent->connect(&rename, &QAction::triggered, pParent, [pParent, this] { this->rename(pParent); });

	QAction copy(QIcon(pSetting.value("CategoryDataCopieIcon").toString()), pSetting.value("CategoryDataCopieName").toString());
	contextMenu.addAction(&copy);
	pParent->connect(&copy, &QAction::triggered, pParent, [pParent, this] { this->copy(pParent); });


	QString strIcon = (((WidgetGameObjectTree*)pParent)->mCopiedActor == nullptr) ? pSetting.value("CategoryDataPasteDisabledIcon").toString() : pSetting.value("CategoryDataPasteIcon").toString();
	QAction paste(QIcon(strIcon), pSetting.value("CategoryDataPasteName").toString());
	contextMenu.addAction(&paste);
	pParent->connect(&paste, &QAction::triggered, pParent, [pParent, this] { this->paste(pParent); });
	if (((WidgetGameObjectTree*)pParent)->mCopiedActor == nullptr)
		paste.setDisabled(true);


	QAction aDelete(QIcon(pSetting.value("CategoryDeleteIcon").toString()), pSetting.value("CategoryDeleteName").toString());
	contextMenu.addAction(&aDelete);
	pParent->connect(&aDelete, &QAction::triggered, pParent, [pParent, this] { this->deleteItem(pParent); });
	
	contextMenu.exec(pParent->mapToGlobal(pPoint));
}

void WidgetGameObjectTreeItem::appendRow(WidgetGameObjectTreeItem* pItem)
{
	Maths::FTransform* transform = ((WidgetGameObjectTreeItem*)pItem)->mActor->getTransform();
	//if (transform->mParent == this->mActor->getTransform())
	//	return;

	transform->setParent(*mActor->getTransform());
	service(EngineCore::Thread::ThreadPool).queueJob([=]
	{
		WidgetConsole::infoPrint("%s became child of %s", Utils::qStringToStdString(pItem->text()), Utils::qStringToStdString(text()));
	});
	QStandardItem::appendRow(pItem);
}

void WidgetGameObjectTreeItem::appendRow(QList<QStandardItem*>& pItems)
{
	for (QStandardItem* item : pItems)
	{
		Maths::FTransform* transform = ((WidgetGameObjectTreeItem*)item)->mActor->getTransform();
		if (transform->mParent == this->mActor->getTransform())
			continue;

		*mActor->mChildActors.emplace_back(((WidgetGameObjectTreeItem*)item)->mActor);
		((WidgetGameObjectTreeItem*)item)->mActor->mParent = mActor;

		transform->setParent(*mActor->getTransform());
		service(EngineCore::Thread::ThreadPool).queueJob([=]
		{
			WidgetConsole::infoPrint("%s became child of %s", Utils::qStringToStdString(item->text()), Utils::qStringToStdString(text()));
		});
	}
	QStandardItem::appendRow(pItems);
}

void WidgetGameObjectTreeItem::create(QWidget* pParent)
{
	WidgetGameObjectTree* tree = ((WidgetGameObjectTree*)pParent);

	WidgetGameObjectTreeItem* item = new WidgetGameObjectTreeItem(mSetting, "GameObject");
	WidgetGameObjectTreeItem::appendRow(item);

	QModelIndex idx = tree->mTreeModel.indexFromItem(item);

	tree->expand(tree->mTreeModel.indexFromItem(this));
	tree->setCurrentIndex(idx);
	
	item->rename(pParent);
}

void WidgetGameObjectTreeItem::rename(QWidget* pParent)
{
	WidgetGameObjectTree* tree = ((WidgetGameObjectTree*)pParent);
	tree->mTreeModel.mShouldEdit = true;

	QModelIndex idx = tree->mTreeModel.indexFromItem(this);
	tree->edit(idx);

	mModel = &tree->mTreeModel;
}

void WidgetGameObjectTreeItem::copy(QWidget* pParent)
{
	((WidgetGameObjectTree*)pParent)->mCopiedActor = this;
}

void WidgetGameObjectTreeItem::paste(QWidget* pParent)
{
	WidgetGameObjectTree* tree = ((WidgetGameObjectTree*)pParent);

	WidgetGameObjectTreeItem* item = new WidgetGameObjectTreeItem(*tree->mCopiedActor);
	appendRow(item);
	QModelIndex idx = tree->mTreeModel.indexFromItem(item);
	tree->setCurrentIndex(idx);
	tree->expand(tree->mTreeModel.indexFromItem(this));
	tree->mCopiedActor = nullptr;
}

void WidgetGameObjectTreeItem::deleteItem(QWidget* pParent)
{
	QString str = text();
	service(EngineCore::Thread::ThreadPool).queueJob([str]
	{
		WidgetConsole::infoPrint("%s is deleted", Utils::qStringToStdString(str));
	});
	
	WidgetGameObjectTree* tree = ((WidgetGameObjectTree*)pParent);
	QModelIndex idx = tree->mTreeModel.indexFromItem(this);
	tree->mTreeModel.removeRow(idx.row(), idx.parent());
	tree->clearSelection();
}

void WidgetGameObjectTreeItem::saveNew()
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;
	QString path = Utils::uniqueString(Defaultpath + "/" + text(), "fab");
	QFile file(path);
	file.open(QIODevice::WriteOnly);
	file.close();
	save(path);
}

void WidgetGameObjectTreeItem::saveTo()
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	QString filter = "Prefab File (*.fab)";

	QFileDialog dialog(nullptr, QString("Open Prefab"), Defaultpath, filter);
	(new QWidget())->connect(&dialog, &QFileDialog::directoryEntered, [&dialog, Defaultpath, this](QString path)
	{
		if (!path.contains(Defaultpath))
			dialog.setDirectory(Defaultpath);
	});

	if (dialog.exec() != QDialog::Accepted)
		return;

	QString fileName = dialog.selectedFiles().first();
	if (!fileName.isEmpty())
		save(fileName);
}

void WidgetGameObjectTreeItem::save(QString path)
{
	QString errorMessage;
	rapidjson::StringBuffer ss;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(ss);

	QSaveFile file(QUrl::fromLocalFile(path).toLocalFile());
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		WidgetGameObjectTreeItem* currItem = this;

		writer.StartObject();

		writer.Key("Name");
		writer.String(Utils::qStringToStdString(text()).c_str());

		currItem->mActor->serialize(writer);

		writer.Key("Childs");
		writer.StartArray();
			serializeItemChild(currItem, writer);
		writer.EndArray();

		writer.EndObject();


		std::string s = ss.GetString();
		file.write(s.c_str());

		if (!file.commit())
		{
			errorMessage = QSaveFile::tr("Cannot write file %1:\n%2.")
				.arg(QDir::toNativeSeparators(path), file.errorString());
		}
	}
	else
	{
		errorMessage = QSaveFile::tr("Cannot open file %1 for writing:\n%2.")
			.arg(QDir::toNativeSeparators(path), file.errorString());
	}

	if (!errorMessage.isEmpty())
		QMessageBox::warning(nullptr, QSaveFile::tr("Application"), errorMessage);
}

void WidgetGameObjectTreeItem::serializeItemChild(QStandardItem* pItem, rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	int numChildren = pItem->rowCount();
	for (int i = 0; i < numChildren; ++i)
	{
		WidgetGameObjectTreeItem* currItem = (WidgetGameObjectTreeItem*)pItem->child(i, 0);

		pWriter.StartObject();

		pWriter.Key("Name");
		pWriter.String(Utils::qStringToStdString(currItem->text()).c_str());

		currItem->mActor->serialize(pWriter);

		pWriter.Key("Childs");
		pWriter.StartArray();
		serializeItemChild(currItem, pWriter);
		pWriter.EndArray();

		pWriter.EndObject();
	}
}