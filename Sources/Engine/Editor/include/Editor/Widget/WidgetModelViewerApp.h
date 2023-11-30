
#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include <QtGui/qwindow.h>
#include "EngineCore/Core/EngineApp.h"
#include "EngineWindow/Core/Window.h"
#include "Rendering/Resources/Model.h"
#include "Editor/Data/OrbitalCamera.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Data/GridData.h"
#include "Rendering/LineDrawer.h"

namespace Editor::Widget
{
	class WidgetModelViewerApp : public ads::CDockWidget
	{
		public:
			bool mIsActive = false;

			EngineWindow::Core::Window mWindow;
			Rendering::Renderer::VK::VKRenderer renderer;
			
			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataAssetViewer> mUnibuffer;
			Rendering::Data::Material* mDefaultMat = nullptr;
			Rendering::Resources::Texture mDefaultTexture;

			Rendering::Data::Material* mGridMat = nullptr;
			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::GridData> mGridUnibuffer;
			
			Editor::Data::OrbitalCamera mCamera;
			Rendering::Resources::Model* mModel = nullptr;

			HWND windowId;
			QWindow* qWindow;
			QWidget* renderingWindow;


			WidgetModelViewerApp(QSettings& pSetting, QWidget* pParent = nullptr);
			~WidgetModelViewerApp();
			void open(Rendering::Resources::Model* pModel);
			void run();
			void createBoundingBox(Rendering::LineDrawer& pLinesDrawer);
			void updateModelTransform();
			void close();

			bool eventFilter(QObject* obj, QEvent* event) override;
	};
}