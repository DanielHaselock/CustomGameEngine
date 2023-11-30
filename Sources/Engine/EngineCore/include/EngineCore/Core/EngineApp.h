#pragma once
#include "EngineWindow/Core/Window.h"
#include "Rendering/Data/DeletionQueue.h"
#include "EngineCore/EventSystem/InputManager.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Tools/Time/Clock.h"
#include "Game/SceneSys/SceneManager.h"
#include "Game/Data/Actor.h"
#include "EngineCore/Light/LightManager.h"
#include "Rendering/Resources/ShadowTexture.h"
#include "Rendering/Resources/ShadowCubeTexture.h"

namespace EngineCore::Core
{
	class EngineApp
	{
	public:
		bool mPlaying = false;
		bool mStart = false;
		bool mPaused = false;
		bool mStop = false;
		bool mHasPlayed = false;
		bool mMouseLock = false;
		bool mInGameMouseLock = false;
		Maths::FVector2 mScreenCenter;

		bool mMouseClick = false;
		Rendering::Resources::UIResource::IUIResource* mUIPressed = nullptr;

		EngineWindow::Core::Window mWindow;
		Rendering::Resources::ShadowTexture* mShadowTexture = nullptr;
		Rendering::Resources::ShadowCubeTexture* mShadowCubeTexture = nullptr;
		Rendering::Renderer::VK::VKRenderer* rend = nullptr;

		Rendering::Data::DeletionQueue mainThreadAction;
		Rendering::Data::DeletionQueue clearThreadAction;

		EngineCore::EventSystem::InputManager* mInput = nullptr;

		Game::SceneSys::Scene* mScene = nullptr;
		std::list<Game::Data::Actor*>* mActors = nullptr;
		Light::LightManager* mLightManager = nullptr;

		Tools::Time::Clock clock;

		Rendering::Data::Material* mat = nullptr;
		Rendering::Data::Material* mUIMat = nullptr;
		Rendering::Data::Material* mTextMat = nullptr;
		Rendering::Data::Material* sky = nullptr;
		Rendering::Data::Material* particleMat = nullptr;
		Rendering::Data::Material* mShadowMat = nullptr;
		Rendering::Data::Material* mShadowCubeMat = nullptr;
		Rendering::Data::Material* mDecal = nullptr;

		Maths::FMatrix4 uiProj;
		int mWidth;
		int mHeight;

		std::condition_variable cv;
		std::mutex closeMut;
		bool closed = false;

		EngineApp();
		EngineApp(unsigned int pWidth, unsigned int pHeight);

		void run();
		void close();

		void playFrame();

		void initRenderer();
		void initSceneMat();
		void initInput();
		void initScene();
		void initScripting();

		void eventUpdate();
		void checkResizeWindow();
		void wakeActorComponent();
		void updateComponentOnce();
		void updateComponent(float Dt);
		void updatePhysique(float Dt);
		void drawScene();

		void drawActors();
		void drawActorsShadow();
		void drawActorsShadowCube();
		void drawSky();
		void drawUI();
		void drawParticle();
		void drawDecals();

		void waitForClose();
		void cleanupScene();

		bool bindLights();

		Maths::FMatrix4 getLookAt(Maths::FVector3& pPos, int i);
	};
}