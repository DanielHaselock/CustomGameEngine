#include <QtDocking/DockWidget.h>
#include "Editor/Widget/WidgetGameObjectTree.h"
#include "Editor/Widget/WidgetInspectorApp.h"

namespace Editor::Widget
{
	class WidgetSceneHierarchyApp : public ads::CDockWidget
	{
		public:
			WidgetGameObjectTree mTree;

			WidgetSceneHierarchyApp(QSettings& pSetting, WidgetInspectorApp& pInspector, QWidget* pParent = nullptr);
			void treeItemClick(const QModelIndex& index);

			void clearTree();
	};
}