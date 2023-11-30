#include <QtDocking/DockWidget.h>
#include <QtCore/qsettings.h>
#include <QtGui/qwindow.h>
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "EngineWindow/Core/Window.h"
#include "Editor/Data/Actor.h"
#include "Rendering/Data/DeletionQueue.h"
#include "Rendering/Renderer/VK/VKOffScreenRenderer.h"
#include "Editor/Widget/WidgetGameObjectTree.h"
#include "Tools/Time/Clock.h"
#include "Editor/Data/PickingArrow.h"
#include <Rendering/Renderer/SkyBox.h>
#include "EngineCore/Light/LightManager.h"
#include "Rendering/Resources/ShadowTexture.h"
#include "Rendering/Resources/Texture.h"
#include <Rendering/Resources/ShadowCubeTexture.h>

namespace Editor::Widget
{
	class WidgetSceneApp : public ads::CDockWidget
	{
		public:
			Editor::Widget::WidgetGameObjectTree* mTreeItem = nullptr;

			EngineWindow::Core::Window mWindow;
			Rendering::Resources::ShadowTexture* mShadowTexture = nullptr;
			Rendering::Resources::ShadowCubeTexture* mShadowCubeTexture = nullptr;
			Rendering::Renderer::VK::VKRenderer* renderer = nullptr;
			Rendering::Renderer::VK::VKOffScreenRenderer* offScreenRenderer = nullptr;

			HWND windowId;
			QWindow* qWindow;
			QWidget* renderingWindow;
			std::list<Editor::Data::Actor*> mActors;
			EngineCore::Light::LightManager* mLightManager = nullptr;
			Rendering::Data::DeletionQueue mainThreadAction;
			Rendering::Data::DeletionQueue clearThreadAction;
			
			Tools::Time::Clock mClock;
			Maths::Frustum camFrustum;
			Editor::Data::Camera mCamera;

			Rendering::Data::Material* mSceneMat = nullptr;
			Rendering::Data::Material* mSceneMatStencil = nullptr;
			Rendering::Data::Material* mOutline = nullptr;
			Rendering::Data::Material* mPickingMat = nullptr;
			Rendering::Data::Material* skyMat = nullptr;
			Rendering::Data::Material* ParticleMat = nullptr;
			Rendering::Data::Material* LightBillMat = nullptr;
			Rendering::Data::Material* mLightBillPickingMat = nullptr;
			Rendering::Data::Material* mShadowMat = nullptr;
			Rendering::Data::Material* mShadowCubeMat = nullptr;
			Rendering::Data::Material* mDecal = nullptr;
			

			EngineCore::EventSystem::InputManager* mInput = nullptr;

			Editor::Data::PickingArrow* pickingArrow = nullptr;

			Rendering::Renderer::SkyBox* skyBox = nullptr;

			std::condition_variable cv;
			std::mutex closeMut;
			bool closed = false;

			WidgetSceneApp();
			WidgetSceneApp(QSettings& pSetting, WidgetGameObjectTree& pTreeItem, QWidget* pParent = nullptr);
			~WidgetSceneApp();
			void run();
			void close();

			void addActor(Editor::Data::Actor* pActor);
			void removeActor(Editor::Data::Actor* pActor);

			void initSceneMat();
			void initSkyBox();
			void initInput();
			void initLight();

			void eventUpdate();
			void checkResizeWindow();
			void checkMousePress(bool& pSelectedActor);

			void pickupPhase();

			void drawScene(bool& pSelectedActor);

			void waitForCleanUp();
			void cleanupScene();

			void drawActors(bool& pSelectedActor);
			void drawActorsShadow();
			void drawActorsShadowCube();
			void drawCollider();
			void drawSky();
			void drawArrow();
			void drawParticle();
			void drawLightBillBoard();
			void drawDecals ();

			bool bindLights();

			Maths::FMatrix4 getLookAt(Maths::FVector3& pPos, int i);
			void lookAtActor(Editor::Data::Actor& pActor);
	};
}