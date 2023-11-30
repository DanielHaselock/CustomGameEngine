#include "Editor/Widget/WidgetInspectorApp.h"
#include <QtCore/qfileinfo.h>
#include "Tools/Utils/PathParser.h"
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"

using namespace Editor::Widget;

WidgetInspectorApp::WidgetInspectorApp(QSettings& pSetting, QWidget* pParent) : ads::CDockWidget("Inspector"), mSetting(pSetting)
{
	QWidget* content = new QWidget(pParent);
	setWidget(content);

	mLayout->setContentsMargins(5, 0, 5, 5);
	mLayout->setSpacing(0);
	content->setLayout(mLayout);

	setAcceptDrops(true);
}

WidgetInspectorApp::~WidgetInspectorApp()
{
	mSelectedItem = nullptr;
	mData = nullptr;
}

void WidgetInspectorApp::dragEnterEvent(QDragEnterEvent* pEvent)
{
	pEvent->acceptProposedAction();
}

void WidgetInspectorApp::dropEvent(QDropEvent* pEvent)
{
	if (mSelectedItem == nullptr)
		return;

	QFileInfo info = QFileInfo(pEvent->mimeData()->text());
	if (Utils::getFileType(info.suffix()) == Tools::Utils::PathParser::EFileType::MODEL)
		mSelectedItem->mDatas->setModel(info);
	else if (Utils::getFileType(info.suffix()) == Tools::Utils::PathParser::EFileType::SCRIPT)
		mSelectedItem->mDatas->setScript(info);
	else if (Utils::getFileType(info.suffix()) == Tools::Utils::PathParser::EFileType::SOUND)
		mSelectedItem->mDatas->setSound(info);
	else if (Utils::getFileType(info.suffix()) == Tools::Utils::PathParser::EFileType::UI)
		mSelectedItem->mDatas->setUI(info);
	else if (Utils::getFileType(info.suffix()) == Tools::Utils::PathParser::EFileType::PARTICLE)
		mSelectedItem->mDatas->setParticle(info);
}

void WidgetInspectorApp::updateInspector(WidgetGameObjectTreeItem* pItem)
{
	WidgetGameObjectTreeItem* previous = mSelectedItem;
	mSelectedItem = pItem;
	if (mData != nullptr)
	{
		mLayout->removeWidget(mData);
		mData->hide();
		mData = nullptr;
	}

	if (pItem == nullptr)
		return;

	mData = pItem->mDatas;
	mLayout->addWidget(mData, 0, Qt::AlignTop);
	mData->show();
}