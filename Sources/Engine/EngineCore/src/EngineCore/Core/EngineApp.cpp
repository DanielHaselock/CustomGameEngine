#include "EngineCore/Core/EngineApp.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Thread/ThreadPool.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Resources/Texture.h"
#include "EngineCore/Service/ServiceLocator.h"

#include "Game/Component/CPScript.h"
#include "Physics/Core/BulPhysicsEngine.h"

#include "EngineCore/EventSystem/InputManager.h"
#include "EngineCore/EventSystem/Key.h"
#include "Maths/FVector2.h"
#include "Scripting/ScriptInterpreter.h"
#include <Rendering/Resources/Font.h>
#include <Game/Component/CPSpotLight.h>
#include <Game/Component/CPPointLight.h>

#include <future>
#include <algorithm>
#include <execution>

using namespace EngineCore::Core;

EngineApp::EngineApp() : mWindow("", 0, 0)
{
	Game::Component::CPCamera::mWorldCamera = nullptr;
}

EngineApp::EngineApp(unsigned int pWidth, unsigned int pHeight) : mWindow("Game", pWidth, pHeight)
{
	Game::Component::CPCamera::mWorldCamera = nullptr;
	mLightManager = new Light::LightManager();
	provideService(EngineCore::Core::EngineApp, *this);

#ifdef SHIPPING
	initRenderer();
	provideService(Rendering::Renderer::VK::VKRenderer, *rend);
#endif
}

void EngineApp::run()
{	
#ifdef NSHIPPING
	initRenderer();
#endif
	initSceneMat();
	initScene();
	initInput();
#ifdef SHIPPING
	initScripting();
#endif

	while (!mWindow.shouldClose())
	{
		eventUpdate();

		float Deltatime = clock.getDeltaTime();

		if (mScene == nullptr)
		{
			initScene();
			drawScene();
			continue;
		}

		checkResizeWindow();
		updateComponentOnce();

		updateComponent(Deltatime);
		updatePhysique(Deltatime);

		drawScene();
	}

	waitForClose();
#ifdef SHIPPING
	cleanupScene();
#endif
}

void EngineApp::initRenderer()
{
	rend = new Rendering::Renderer::VK::VKRenderer();
	rend->init(mWindow);

	mLightManager->init();

	mShadowTexture = new Rendering::Resources::ShadowTexture(*rend);
	mShadowCubeTexture = new Rendering::Resources::ShadowCubeTexture(*rend);
	rend->mainThreadAction.pushFunction([this]
	{
		delete mShadowTexture;
		mShadowTexture = new Rendering::Resources::ShadowTexture(*rend);

		delete mShadowCubeTexture;
		mShadowCubeTexture = new Rendering::Resources::ShadowCubeTexture(*rend);
	});
}

void EngineApp::initSceneMat()
{
	mat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ModelVertex.vert.spv", "Resources/Engine/Shaders/ModelFrag.frag.spv", rend->mRenderPass, false));

	mUIMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/UIVertex.vert.spv", "Resources/Engine/Shaders/UIFrag.frag.spv", rend->mRenderPass, false, false, false, false));

	mTextMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/TextVertex.vert.spv", "Resources/Engine/Shaders/TextFrag.frag.spv", rend->mRenderPass, false, false, false, false));

	sky = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/SkyVertex.vert.spv", "Resources/Engine/Shaders/SkyFrag.frag.spv", rend->mRenderPass, false));

	particleMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ParticleVertex.vert.spv", "Resources/Engine/Shaders/ParticleFrag.frag.spv", rend->mRenderPass, false, false, true, false, true));

	mShadowMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ShadowVertex.vert.spv", rend->mShadowPass.pass, false, false, true, false));

	mShadowCubeMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ShadowCubeVertex.vert.spv", "Resources/Engine/Shaders/ShadowCubeFrag.frag.spv", rend->mShadowCube.pass, false, false, true, false));
	
	mDecal = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/DecalVertex.vert.spv", "Resources/Engine/Shaders/DecalFrag.frag.spv", rend->mRenderPass, false, false, true, false, true, true));
}

void EngineApp::initInput()
{
#ifdef SHIPPING
	mInput = &service(EngineCore::EventSystem::InputManager);
#else
	mInput = new EngineCore::EventSystem::InputManager();
#endif
	
	mWindow.setInputManager(mInput);
}

void EngineApp::initScene()
{
	mScene = service(Game::SceneSys::SceneManager).mCurrentScene;
	mActors = &mScene->mActors;

	if (mLightManager != nullptr)
	{
		if (mScene != nullptr)
			mLightManager->mLights = &mScene->mLights;
	}
}

void EngineApp::initScripting()
{
	service(Scripting::ScriptInterpreter).RefreshAll();
}

void EngineApp::eventUpdate()
{
	clearThreadAction.timerFlush();
	mainThreadAction.timerFlush();
	clock.update();
	mWindow.pollEvents();

#ifdef SHIPPING
	mInput->processInput();
#endif
}

void EngineApp::checkResizeWindow()
{
	if (rend->mFramebufferResized)
	{
		int width, height;
		mWindow.getFramebufferSize(&width, &height);

		if (Game::Component::CPCamera::mWorldCamera != nullptr)
			Game::Component::CPCamera::mWorldCamera->updateProjection(width, height);

		rend->mFramebufferResized = false;

		uiProj = Maths::FMatrix4::createOrthographic(0, width, 0, height, -100, 100);
		mWidth = width;
		mHeight = height;
		for (Game::Data::Actor* actor : *mActors)
			actor->updateUIProj(uiProj, width, height);

		int x, y;
		glfwGetWindowPos(mWindow.mWindow, &x, &y);

		mScreenCenter = Maths::FVector2(mWidth / 2, mHeight / 2);
		mScreenCenter.x += x;
		mScreenCenter.y += y;
	}
}

void EngineApp::wakeActorComponent()
{
	for (Game::Data::Actor* actor : *mActors)
	{
		actor->OnAwake();
		actor->updateWorldTransform(actor->getTransform());
		actor->OnStart();
	}
}

void EngineApp::updateComponentOnce()
{
	if (mStart && !mHasPlayed)
	{
		wakeActorComponent();
		mStart = false;
		mHasPlayed = true;
		mPaused = false;
	}
	else if (mPaused)
	{
		for (Game::Data::Actor* actor : *mActors)
			actor->OnPaused();

		mPaused = false;
	}
	else if (mStop)
	{
		for (Game::Data::Actor* actor : *mActors)
			actor->OnDestroy();

		mStop = false;
	}
}

void EngineApp::updateComponent(float Dt)
{
	if (!mPlaying)
		return;

	auto task2 = std::async(std::launch::async, [this, Dt]()
		{
			std::for_each(std::execution::par,
				mActors->begin(),
				mActors->end(),
				[this, Dt](Game::Data::Actor* actor)
				{
					{
						actor->updateWorldTransform(actor->getTransform());
						actor->OnUpdate(Dt);

						/*if (mWindow.press)
						{
#ifdef  NSHIPPING
							if (mInGameMouseLock)
								mMouseLock = true;
#endif 

							Rendering::Resources::UIResource::IUIResource* tmp = actor->containMouseClick(mWindow.mousePos);
							if (tmp != nullptr)
							{
								if (mUIPressed == nullptr)
									mUIPressed = tmp;
								else if (mUIPressed->zOrder < tmp->zOrder)
									mUIPressed = tmp;
							}

							mMouseClick = true;
						}
						else
							actor->containMouse(mWindow.mousePos);

						if (!mWindow.press && mMouseClick)
						{
							mMouseClick = false;
							if (mUIPressed)
							{
								mUIPressed->CallLuaDelegatePress();
								mUIPressed = nullptr;
							}
						}*/
					}
				});
		});

	
	for (Game::Data::Actor* actor : *mActors)
	{
		{
			for (Game::Component::AComponent* Component : actor->mComponents[Game::Utils::ComponentType::Script])
				Component->OnUpdate(Dt);
			
			if (mWindow.press)
			{
#ifdef  NSHIPPING
				if (mInGameMouseLock)
					mMouseLock = true;
#endif 

				Rendering::Resources::UIResource::IUIResource* tmp = actor->containMouseClick(mWindow.mousePos);
				if (tmp != nullptr)
				{
					if (mUIPressed == nullptr)
						mUIPressed = tmp;
					else if (mUIPressed->zOrder < tmp->zOrder)
						mUIPressed = tmp;
				}

				mMouseClick = true;
			}
			else
				actor->containMouse(mWindow.mousePos);

			if (!mWindow.press && mMouseClick)
			{
				mMouseClick = false;
				if (mUIPressed)
				{
					mUIPressed->CallLuaDelegatePress();
					mUIPressed = nullptr;
				}
			}
		}
	}

	task2.wait();
}

void EngineApp::updatePhysique(float Dt)
{
	if (mPlaying)
		service(Physics::Core::BulPhysicsEngine).update(Dt);
}

void EngineApp::drawScene()
{
	rend->beginFrame();
	
	/*if (Game::Component::CPCamera::mWorldCamera != nullptr && mScene != nullptr)
		drawActorsShadowCube();*/

	rend->beginShadowPass();
	/*if (Game::Component::CPCamera::mWorldCamera != nullptr && mScene != nullptr)
		drawActorsShadow();*/
	rend->endShadowPass();

	rend->beginWorldPass();
	if (Game::Component::CPCamera::mWorldCamera != nullptr && mScene != nullptr)
	{
		drawSky();
		drawActors();
		drawParticle();
		drawDecals();
		drawUI();
	}
	rend->endWorldPass();
	
	rend->endFrame();
}

bool EngineApp::bindLights()
{
	if (mLightManager == nullptr)
		return false;

	if (mLightManager->mLights == nullptr)
	{
		if (mLightManager != nullptr)
		{
			Game::SceneSys::SceneManager& scene = service(Game::SceneSys::SceneManager);
			if (scene.mCurrentScene != nullptr)
				mLightManager->mLights = &scene.mCurrentScene->mLights;
		}
		return false;
	}

	mLightManager->mUniformBuffer->mData.mNumLight = mLightManager->mLights->size();

	{
		int i = 0;
		for (auto& it : *mLightManager->mLights)
		{
			it->fillUniform(i, mLightManager->mUniformBuffer->mData);
			i++;
		}
	}

	mLightManager->mUniformBuffer->mData.mEyePos = Game::Component::CPCamera::mWorldCamera->mPosition;
	mLightManager->mUniformBuffer->updateData();

	mat->bindDescriptor("uboFrag", mLightManager->mUniformBuffer->mDescriptorSets);
	return true;
}

void EngineApp::drawActorsShadow()
{
	if (mLightManager == nullptr)
		return;

	if (mLightManager->mLights == nullptr)
		return;

	int i = 0;
	int spotLightIdx = 0;

	int split = 0;
	if (mLightManager->spotLight <= 1)
		split = 1;
	else if (mLightManager->spotLight <= 4)
		split = 2;
	else if (mLightManager->spotLight <= 9)
		split = 3;
	else
		split = 4;

	mLightManager->mUniformBuffer->mData.shadowSize = { rend->mWindowExtent.width / float(split) , rend->mWindowExtent.height / float(split), (float)rend->mWindowExtent.width, (float)rend->mWindowExtent.height };
	mLightManager->mUniformBuffer->mData.shadowSplit = split;

	for (auto& it : *mLightManager->mLights)
	{
		Maths::FVector3& pos = it->getPos();
		Maths::FMatrix4 vp = Game::Component::CPCamera::mWorldCamera->mProjection * Maths::FMatrix4::lookAt(pos, pos + it->getDir(), Maths::FVector3::Right);

		Game::Component::CPSpotLight* light = dynamic_cast<Game::Component::CPSpotLight*>(it);
		if (light == nullptr)
		{
			i++;
			continue;
		}

		rend->drawViewPort(split, spotLightIdx);
		
		mShadowMat->bindConstant(rend->getCurrentCommandBuffer(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Maths::FMatrix4), &vp);

		for (Game::Data::Actor* actor : *mActors)
			actor->draw(rend->getCurrentCommandBuffer(), *mShadowMat);

		mLightManager->mUniformBuffer->mData.light[i].mVP = vp;
		mLightManager->mUniformBuffer->mData.light[i].mIdx = spotLightIdx;

		i++;
		spotLightIdx++;
	}
}

Maths::FMatrix4 EngineApp::getLookAt(Maths::FVector3& pPos, int i)
{
	switch (i)
	{
	case 0: return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Left, Maths::FVector3::Up);
	case 1: return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Right, Maths::FVector3::Up);
	case 2: return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Up, Maths::FVector3::Forward);
	case 3: return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Bottom, Maths::FVector3::Forward);
	case 4: return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Forward, Maths::FVector3::Up);
	case 5: return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Backward, Maths::FVector3::Up);
	default:
		return Maths::FMatrix4::lookAt(pPos, pPos + Maths::FVector3::Left, Maths::FVector3::Up);
	}
}

void EngineApp::drawActorsShadowCube()
{
	if (mLightManager == nullptr)
		return;

	if (mLightManager->mLights == nullptr)
		return;

	int j = 0;
	int pointLightIdx = 0;

	int split = 0;
	if (mLightManager->spotLight <= 1)
		split = 1;
	else if (mLightManager->spotLight <= 4)
		split = 2;
	else if (mLightManager->spotLight <= 9)
		split = 3;
	else
		split = 4;

	for (int i = 0; i < 6; i++)
	{
		rend->beginShadowCubePass(i);

		for (auto& it : *mLightManager->mLights)
		{
			
			Game::Component::CPPointLight* light = dynamic_cast<Game::Component::CPPointLight*>(it);
			if (light == nullptr || !light->ready)
			{
				j++;
				continue;
			}
			
			Maths::FVector3& pos = it->getPos();
			Maths::FMatrix4 vp = Maths::FMatrix4::createPerspective(90, 1, Game::Component::CPCamera::mWorldCamera->mNear, light->mRadius) * getLookAt(pos, i);


			light->mUniformBuffer->mData.mLigthPos = pos;
			light->mUniformBuffer->mData.farPlane = light->mRadius;
			light->mUniformBuffer->updateData();
			mShadowCubeMat->bindDescriptor("uboLight", light->mUniformBuffer->mDescriptorSets);

			rend->drawViewPortCube(split, pointLightIdx);

			for (Game::Data::Actor* actor : *mActors)
			{
				Rendering::Data::UniformDataShadowCubePush push;
				push.vp = vp;
				push.model = actor->getTransform()->mWorldMatrix;

				mShadowCubeMat->bindConstant(rend->getCurrentCommandBuffer(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Rendering::Data::UniformDataShadowCubePush), &push);
				mShadowCubeMat->bindPipeLine(rend->getCurrentCommandBuffer());

				actor->drawShadow(rend->getCurrentCommandBuffer());
			}

			mLightManager->mUniformBuffer->mData.light[i].mIdx = pointLightIdx;
			mLightManager->mUniformBuffer->mData.light[i].mPos = pos;

			j++;
			pointLightIdx++;
		}

		rend->endShadowCubePass();
	}
}

void EngineApp::drawActors()
{
	if (!bindLights())
		return;

	mat->bindDescriptor("shadowTexture", mShadowTexture->mTextureSets);
	mat->bindDescriptor("shadowCubeTexture", mShadowCubeTexture->mTextureSets);
	mat->bindPipeLine(rend->getCurrentCommandBuffer());

	for (Game::Data::Actor* actor : *mActors)
	{
		actor->updateProj(Game::Component::CPCamera::mWorldCamera->viewProj());
		actor->draw(rend->getCurrentCommandBuffer(), *mat);
	}
}

void EngineApp::drawSky()
{
	Game::Component::CPCamera::mWorldCamera->drawSkyBox(rend->getCurrentCommandBuffer(), *sky);
}

void EngineApp::drawUI()
{
	for (Game::Data::Actor* actor : *mActors)
		actor->drawUI(rend->getCurrentCommandBuffer(), *mUIMat, *mTextMat);
}

void EngineApp::drawParticle()
{
	if (Game::Component::CPCamera::mWorldCamera == nullptr)
		return;

	for (Game::Data::Actor* actor : *mActors)
		actor->drawParticle(rend->getCurrentCommandBuffer(), *particleMat,
			Game::Component::CPCamera::mWorldCamera->mVp, 
			Game::Component::CPCamera::mWorldCamera->mView);
}

void EngineApp::drawDecals()
{
	for (Game::Data::Actor* actor : *mActors)
		actor->drawDecals(rend->getCurrentCommandBuffer(), *mDecal, Game::Component::CPCamera::mWorldCamera->viewProj());
}

void EngineApp::waitForClose()
{
	rend->waitForCleanUp();

	std::unique_lock<std::mutex> lck(closeMut);
	closed = true;
	cv.notify_all();
}

void EngineApp::cleanupScene()
{
	{
		std::unique_lock lock(rend->mainThreadAction.mMutex);
		rend->mainThreadAction.mDeletors.clear();
	}
	delete mShadowTexture;
	delete mShadowCubeTexture;

	delete mat;
	delete mUIMat;
	delete mTextMat;
	delete sky;
	delete particleMat;
	delete mShadowMat;
	delete mShadowCubeMat;
	delete mDecal;

	if (mLightManager != nullptr)
		delete mLightManager;

#ifdef NSHIPPING
	delete mInput;
	delete rend;
#endif
}

void EngineApp::playFrame()
{
	float delta = clock.getDeltaTime();
	service(Physics::Core::BulPhysicsEngine).update(delta);

	std::list<Game::Data::Actor*>& mActors = service(Game::SceneSys::SceneManager).mCurrentScene->mActors;
	for (Game::Data::Actor* actor : mActors)
	{
		actor->updateWorldTransform(actor->getTransform());
		actor->OnUpdate(delta);
	}
}

void EngineApp::close()
{
	mWindow.close();
}
