#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include <QtGui/qwindow.h>
#include "EngineCore/Core/EngineApp.h"

namespace Editor::Widget
{
	class WidgetGameApp : public ads::CDockWidget
	{
	public:
		HWND windowId;
		QWindow* qWindow;
		QWidget* renderingWindow;
		WidgetGameApp() : ads::CDockWidget("Game") {};
		WidgetGameApp(EngineCore::Core::EngineApp* mApp, QWidget* pParent = nullptr);

		Maths::FVector2 calculateCursorPos();
	};
}

