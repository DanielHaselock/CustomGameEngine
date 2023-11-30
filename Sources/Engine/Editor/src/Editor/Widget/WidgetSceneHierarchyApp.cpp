#include <QtWidgets/qboxlayout.h>
#include "Editor/Widget/WidgetSceneHierarchyApp.h"
#include "Editor/Widget/WidgetGameObjectTreeItem.h"

using namespace Editor::Widget;

WidgetSceneHierarchyApp::WidgetSceneHierarchyApp(QSettings& pSetting, WidgetInspectorApp& pInspector, QWidget* pParen) :
	ads::CDockWidget("Hierarchy"), mTree(pSetting, pInspector)
{
	QWidget* content = new QWidget(pParen);
	setWidget(content);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	layout->setContentsMargins(5, 0, 5, 5);
	layout->setSpacing(0);
	content->setLayout(layout);

	layout->addWidget(&mTree);
	clearTree();
}

void WidgetSceneHierarchyApp::clearTree()
{
	mTree.mTreeModel.clear();
	mTree.initHeader("Scene");
}