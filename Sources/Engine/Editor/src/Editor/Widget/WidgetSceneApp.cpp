#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include "Editor/Widget/WidgetSceneApp.h"
#include "Tools/Time/Clock.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Editor/Data/Camera.h"
#include "EngineCore/EventSystem/InputManager.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Resources/Texture.h"
#include "Game/Data/Actor.h"
#include "Game/SceneSys/SceneManager.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"
#include "Physics/Core/BulPhysicsEngine.h"
#include "Physics/Resources/IRigidbody.h"
#include "Game/Utils/ComponentType.h"
#include <future>
#include <algorithm>
#include <execution>


using namespace Editor::Widget;

WidgetSceneApp::WidgetSceneApp() :
	ads::CDockWidget(""), mWindow("", 0, 0), mCamera(0, 0)
{
}

WidgetSceneApp::WidgetSceneApp(QSettings& pSetting, WidgetGameObjectTree& pTreeItem, QWidget* pParent) :
	ads::CDockWidget("Scene"), mWindow("", 800, 800), mTreeItem(&pTreeItem), mCamera(800, 800)
{
	QWidget* content = new QWidget(pParent);
	setWidget(content);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	layout->setContentsMargins(5, 0, 5, 5);
	layout->setSpacing(0);
	content->setLayout(layout);

	setAcceptDrops(true);

	windowId = mWindow.getWindowId();
	qWindow = QWindow::fromWinId((WId)windowId);

	renderingWindow = QWidget::createWindowContainer(qWindow);
	renderingWindow->installEventFilter(this);
	layout->addWidget(renderingWindow);

	provideService(Editor::Widget::WidgetSceneApp, *this);
	renderer = new Rendering::Renderer::VK::VKRenderer();
	renderer->init(mWindow);
	offScreenRenderer = new Rendering::Renderer::VK::VKOffScreenRenderer(renderingWindow->width(), renderingWindow->height());

	pickingArrow = new Editor::Data::PickingArrow(renderer->mRenderPass, offScreenRenderer->mRenderPass);

	mShadowTexture = new Rendering::Resources::ShadowTexture(*renderer);
	mShadowCubeTexture = new Rendering::Resources::ShadowCubeTexture(*renderer);
	renderer->mainThreadAction.pushFunction([this]
		{
			delete mShadowTexture;
			mShadowTexture = new Rendering::Resources::ShadowTexture(*renderer);

			delete mShadowCubeTexture;
			mShadowCubeTexture = new Rendering::Resources::ShadowCubeTexture(*renderer);
		});
}

void WidgetSceneApp::run()
{
	mLightManager = new EngineCore::Light::LightManager();
	mLightManager->init();
	initLight();

	initSceneMat();
	initSkyBox();
	initInput();

	service(Physics::Core::BulPhysicsEngine).createDebugDrawer(renderer);
	
	renderer->mFramebufferResized = true;
	bool newSelection = false;
	while (!mWindow.shouldClose())
	{
		eventUpdate();
		checkResizeWindow();
		checkMousePress(newSelection);
		drawScene(newSelection);
	}

	waitForCleanUp();
}

void WidgetSceneApp::close()
{
	mWindow.close();
}

WidgetSceneApp::~WidgetSceneApp()
{
	for (auto actor : mActors)
		delete actor;
}

void Editor::Widget::WidgetSceneApp::addActor(Editor::Data::Actor* pActor)
{
	mainThreadAction.pushFunction([this, pActor]
	{
		mActors.push_back(pActor);
	});
}

void WidgetSceneApp::removeActor(Editor::Data::Actor* pActor)
{
	mainThreadAction.pushFunction([this, pActor]
	{
		renderer->waitForCleanUp();
		offScreenRenderer->waitForCleanUp();

		mActors.remove(pActor);
		delete pActor;
	});
}

void WidgetSceneApp::initSceneMat()
{
	mSceneMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ModelVertex.vert.spv", "Resources/Engine/Shaders/ModelFrag.frag.spv", renderer->mRenderPass, false, false, true, false, false, false));
	
	mSceneMatStencil = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ModelVertex.vert.spv", "Resources/Engine/Shaders/ModelFrag.frag.spv", renderer->mRenderPass));

	mOutline = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/outline.vert.spv", "Resources/Editor/Shaders/outline.frag.spv", renderer->mRenderPass, false, false, false, true));

	mPickingMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/IdItem.vert.spv", "Resources/Editor/Shaders/IdItem.frag.spv", offScreenRenderer->mRenderPass, false, false, true, false));

	skyMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/SkyVertex.vert.spv", "Resources/Engine/Shaders/SkyFrag.frag.spv", renderer->mRenderPass, false, false, true, false, false, false));

	ParticleMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ParticleVertex.vert.spv", "Resources/Engine/Shaders/ParticleFrag.frag.spv", renderer->mRenderPass, false, false, true, false, true));

	LightBillMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/LightBillboardVertex.vert.spv", "Resources/Editor/Shaders/LightBillboardFrag.frag.spv", renderer->mRenderPass, false, false, true, false, true));

	mLightBillPickingMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/IdLightBillboardVertex.vert.spv", "Resources/Editor/Shaders/IdLightBillboardFrag.frag.spv", offScreenRenderer->mRenderPass, false, false, true, false));

	mShadowMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ShadowVertex.vert.spv", renderer->mShadowPass.pass, false, false, true, false));

	mShadowCubeMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/ShadowCubeVertex.vert.spv", "Resources/Engine/Shaders/ShadowCubeFrag.frag.spv", renderer->mShadowCube.pass, false, false, true, false));
	
	mDecal = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/DecalVertex.vert.spv", "Resources/Engine/Shaders/DecalFrag.frag.spv", renderer->mRenderPass, false, false, true, false, true, true));
}

void WidgetSceneApp::initSkyBox()
{
	skyBox = new Rendering::Renderer::SkyBox();
	skyBox->setDefault();
}

void WidgetSceneApp::initInput()
{
	service(EngineCore::EventSystem::InputManager).bindContinuousActionCallBack(EngineCore::EventSystem::Key::W,
		[this] {if (mInput->mMouse.isRightPressed()) mCamera.update(Maths::FVector3::Up, mClock.getDeltaTime()); });

	service(EngineCore::EventSystem::InputManager).bindContinuousActionCallBack(EngineCore::EventSystem::Key::S,
		[this] {if (mInput->mMouse.isRightPressed()) mCamera.update(-Maths::FVector3::Up, mClock.getDeltaTime()); });

	service(EngineCore::EventSystem::InputManager).bindContinuousActionCallBack(EngineCore::EventSystem::Key::A,
		[this] {if (mInput->mMouse.isRightPressed()) mCamera.update(Maths::FVector3::Right, mClock.getDeltaTime()); });

	service(EngineCore::EventSystem::InputManager).bindContinuousActionCallBack(EngineCore::EventSystem::Key::D,
		[this] {if (mInput->mMouse.isRightPressed()) mCamera.update(Maths::FVector3::Left, mClock.getDeltaTime()); });

	service(EngineCore::EventSystem::InputManager).bindContinuousActionCallBack(EngineCore::EventSystem::Key::E,
		[this] {if (mInput->mMouse.isRightPressed()) mCamera.update(Maths::FVector3::Forward, mClock.getDeltaTime()); });

	service(EngineCore::EventSystem::InputManager).bindContinuousActionCallBack(EngineCore::EventSystem::Key::Q,
		[this] {if (mInput->mMouse.isRightPressed()) mCamera.update(Maths::FVector3::Backward, mClock.getDeltaTime()); });

	service(EngineCore::EventSystem::InputManager).bindActionCallBack(EngineCore::EventSystem::KState::PRESS, EngineCore::EventSystem::Key::E,
		[this] { if (!mInput->mMouse.isRightPressed())pickingArrow->setMode(Editor::Data::ArrowMode::ETranslation); });

	service(EngineCore::EventSystem::InputManager).bindActionCallBack(EngineCore::EventSystem::KState::PRESS, EngineCore::EventSystem::Key::R,
		[this] {if (!mInput->mMouse.isRightPressed()) pickingArrow->setMode(Editor::Data::ArrowMode::ERotation); });

	service(EngineCore::EventSystem::InputManager).bindActionCallBack(EngineCore::EventSystem::KState::PRESS, EngineCore::EventSystem::Key::T,
		[this] {if (!mInput->mMouse.isRightPressed()) pickingArrow->setMode(Editor::Data::ArrowMode::EScale); });

	mInput = &service(EngineCore::EventSystem::InputManager);
	mWindow.setInputManager(mInput);
}

void WidgetSceneApp::initLight()
{
	if (mLightManager != nullptr)
	{
		Game::SceneSys::SceneManager& scene = service(Game::SceneSys::SceneManager);
		if (scene.mCurrentScene != nullptr)
			mLightManager->mLights = &scene.mCurrentScene->mLights;
	}
}

void WidgetSceneApp::eventUpdate()
{
	/*auto task2 = std::async(std::launch::async, [this]()
		{
			std::unique_lock lock1(mainThreadAction.mMutex);
			std::for_each(std::execution::par,
				mainThreadAction.mDeletors.rbegin(),
				mainThreadAction.mDeletors.rend(),
				[](std::function<void()>& job) {
					job();
				});
		});*/

	clearThreadAction.flush();
	mainThreadAction.flush();
	mClock.update();
	mWindow.pollEvents();
	mInput->processInput();
}

void WidgetSceneApp::checkResizeWindow()
{
	if (renderer->mFramebufferResized)
	{
		int width = renderingWindow->width();
		int height = renderingWindow->height();;
		mWindow.mWidth = width;
		mWindow.mHeight = height;
		mCamera.updateProjection(width, height);

		renderer->mFramebufferResized = false;
		offScreenRenderer->recreateSwapChain(width, height);
	}
}

void WidgetSceneApp::checkMousePress(bool& pSelectedActor)
{
	Maths::FVector2 dif = mInput->mMouse.getPositionDifference();
	if (mInput->mMouse.isRightPressed())
		mCamera.updateView(dif);
	
	static bool mSingleClick = false;
	auto task = std::async(std::launch::async, [this, &pSelectedActor, dif]()
		{
			if (mInput->mMouse.isLeftPressed() && !mSingleClick)
			{
				pickupPhase();
				pSelectedActor = true;
				mSingleClick = true;
			}
			else if (!mInput->mMouse.isLeftPressed())
			{
				mSingleClick = false;
				pickingArrow->endDrag();
			}

			if (pickingArrow->isDrag)
				pickingArrow->moveDrag(dif);
		});
}

void WidgetSceneApp::pickupPhase()
{
	QPoint StartPoint = renderingWindow->mapFromGlobal(QCursor::pos());
	if (StartPoint.x() <= 0 || StartPoint.y() <= 0)
		return;

	offScreenRenderer->beginFrame();
	for (Editor::Data::Actor* actor : mActors)
	{
		mPickingMat->bindDescriptor("ubo", actor->mUniformBufferid.mDescriptorSets);

		actor->mUniformBufferid.mData.id = actor->id;
		actor->mUniformBufferid.mData.mViewProjection = mCamera.viewProj();
		actor->mUniformBufferid.mData.mModel = actor->getTransform()->getWorldMatrix();
		actor->mUniformBufferid.updateData();

		mPickingMat->bindPipeLine(offScreenRenderer->getCurrentCommandBuffer());
		
		actor->draw(offScreenRenderer->getCurrentCommandBuffer(), camFrustum);

		#ifdef NSHIPPING
			actor->drawCameraOffscreen(offScreenRenderer->getCurrentCommandBuffer());

			actor->drawLightOffscreen(offScreenRenderer->getCurrentCommandBuffer(), *mLightBillPickingMat, mCamera.viewProj(), mCamera.mView);
		#endif
	}

	if (mTreeItem->mSelectedActor != nullptr)
		pickingArrow->drawPicking(offScreenRenderer->getCurrentCommandBuffer(), mCamera.viewProj(), mCamera.mPosition);

	offScreenRenderer->endFrame();
	const unsigned char* data = ((const unsigned char*)offScreenRenderer->pixelToArray());


	int yPos = StartPoint.y() * (offScreenRenderer->subResourceLayout.rowPitch / offScreenRenderer->ColorChannel);
	int PosInArray = ((StartPoint.x()) + yPos) * offScreenRenderer->ColorChannel;

	int id = data[PosInArray];
	if (id == 0 && data[PosInArray + 1] == 255)
	{
		pickingArrow->beginDrag(data[PosInArray + 2], mCamera);
		return;
	}

	if (id != mTreeItem->mIdSelected)
	{
		mTreeItem->mIdSelected = id;

		if (mTreeItem->mSelectedActor != nullptr)
		{
			mTreeItem->mSelectedActor = nullptr;
		}

		if (id == 0)
			mTreeItem->selectionModel()->clearSelection();
	}
}

void WidgetSceneApp::drawScene(bool& pSelectedActor)
{
	renderer->beginFrame();
		
	//drawActorsShadowCube();

	renderer->beginShadowPass();
	drawActorsShadow();
	renderer->endShadowPass();

	renderer->beginWorldPass();
		drawSky();
		drawActors(pSelectedActor);
		drawCollider();
		drawParticle();
		drawLightBillBoard();
		drawDecals();
		drawArrow();
	renderer->endWorldPass();
		
	renderer->endFrame();
}

bool WidgetSceneApp::bindLights()
{
	if (mLightManager == nullptr)
		return false;

	if (mLightManager->mLights == nullptr)
	{
		initLight();
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

	mLightManager->mUniformBuffer->mData.mEyePos = mCamera.mPosition;
	mLightManager->mUniformBuffer->updateData();

	mSceneMat->bindDescriptor("uboFrag", mLightManager->mUniformBuffer->mDescriptorSets);
	mSceneMatStencil->bindDescriptor("uboFrag", mLightManager->mUniformBuffer->mDescriptorSets);

	return true;
}

void WidgetSceneApp::drawActorsShadow()
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

	mLightManager->mUniformBuffer->mData.shadowSize = { float(renderer->mWindowExtent.width / (float)split) , float(renderer->mWindowExtent.height / float(split)), (float)renderer->mWindowExtent.width, (float)renderer->mWindowExtent.height };
	mLightManager->mUniformBuffer->mData.shadowSplit = split;

	for (auto& it : *mLightManager->mLights)
	{
		Maths::FVector3& pos = it->getPos();
		Maths::FMatrix4 vp = mCamera.mProjection * Maths::FMatrix4::lookAt(pos, pos + it->getDir(), Maths::FVector3::Right);

		Game::Component::CPSpotLight* light = dynamic_cast<Game::Component::CPSpotLight*>(it);
		Game::Component::CPDirLight* lightDir = dynamic_cast<Game::Component::CPDirLight*>(it);
		if (light == nullptr && lightDir == nullptr)
		{
			i++;
			continue;
		}

		renderer->drawViewPort(split, spotLightIdx);

		mShadowMat->bindConstant(renderer->getCurrentCommandBuffer(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Maths::FMatrix4), &vp);
		for (Editor::Data::Actor* actor : mActors)
			actor->draw(renderer->getCurrentCommandBuffer(), camFrustum, mShadowMat);
		
		mLightManager->mUniformBuffer->mData.light[i].mVP = vp;
		mLightManager->mUniformBuffer->mData.light[i].mIdx = spotLightIdx;
		
		i++;
		spotLightIdx++;
	}
}

Maths::FMatrix4 WidgetSceneApp::getLookAt(Maths::FVector3& pPos, int i)
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

void WidgetSceneApp::drawActorsShadowCube()
{
#ifdef NSHIPPING
	if (mLightManager == nullptr)
		return;

	if (mLightManager->mLights == nullptr)
		return;

	int j = 0;
	int pointLightIdx = 0;

	int split = 0;
	if (mLightManager->pointLight <= 1)
		split = 1;
	else if (mLightManager->pointLight <= 4)
		split = 2;
	else if (mLightManager->pointLight <= 9)
		split = 3;
	else
		split = 4;

	for (int i = 0; i < 6; i++)
	{
		renderer->beginShadowCubePass(i);

		for (auto& it : *mLightManager->mLights)
		{
			Game::Component::CPPointLight* light = dynamic_cast<Game::Component::CPPointLight*>(it);
			if (light == nullptr || !light->ready)
			{
				j++;
				continue;
			}

			Maths::FVector3& pos = it->getPos();
			Maths::FMatrix4 vp = Maths::FMatrix4::createPerspective(90, 1, mCamera.mNear, light->mRadius) * getLookAt(pos, i);


			light->mUniformBufferEditor->mData.mLigthPos = pos;
			light->mUniformBufferEditor->mData.farPlane = light->mRadius;
			light->mUniformBufferEditor->updateData();
			mShadowCubeMat->bindDescriptor("uboLight", light->mUniformBufferEditor->mDescriptorSets);

			renderer->drawViewPortCube(split, pointLightIdx);

			for (Editor::Data::Actor* actor : mActors)
			{
				Rendering::Data::UniformDataShadowCubePush push;
				push.vp = vp;
				push.model = actor->getTransform()->mWorldMatrix;

				mShadowCubeMat->bindConstant(renderer->getCurrentCommandBuffer(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Rendering::Data::UniformDataShadowCubePush), &push);
				mShadowCubeMat->bindPipeLine(renderer->getCurrentCommandBuffer());

				actor->drawShadow(renderer->getCurrentCommandBuffer());
			}

			mLightManager->mUniformBuffer->mData.light[i].mIdx = pointLightIdx;
			mLightManager->mUniformBuffer->mData.light[i].mPos = pos;

			j++;
			pointLightIdx++;
		}

		renderer->endShadowCubePass();
	}
#endif
}

void WidgetSceneApp::drawActors(bool& pSelectedActor)
{
	if (!bindLights())
		return;

	mSceneMat->bindDescriptor("shadowTexture", mShadowTexture->mTextureSets);
	mSceneMat->bindDescriptor("shadowCubeTexture", mShadowCubeTexture->mTextureSets);
	mSceneMat->bindPipeLine(renderer->getCurrentCommandBuffer());

	for (Editor::Data::Actor* actor : mActors)
	{
		if (actor->id == mTreeItem->mIdSelected && pSelectedActor)
		{
			mTreeItem->mSelectedActor = actor;

			QModelIndex idx = mTreeItem->mTreeModel.indexFromItem((Editor::Widget::WidgetGameObjectTreeItem*)actor->mItemOwner);
			mTreeItem->setCurrentIndex(idx);

			pSelectedActor = false;

			mSceneMatStencil->bindDescriptor("shadowTexture", mShadowTexture->mTextureSets);
			mSceneMatStencil->bindDescriptor("shadowCubeTexture", mShadowCubeTexture->mTextureSets);
			mSceneMatStencil->bindPipeLine(renderer->getCurrentCommandBuffer());
		}

		actor->updateProj(mCamera.viewProj());

		if (mTreeItem->mSelectedActor == actor)
		{
			mSceneMatStencil->bindDescriptor("shadowTexture", mShadowTexture->mTextureSets);
			mSceneMatStencil->bindDescriptor("shadowCubeTexture", mShadowCubeTexture->mTextureSets);
			mSceneMatStencil->bindPipeLine(renderer->getCurrentCommandBuffer());
			actor->draw(renderer->getCurrentCommandBuffer(), camFrustum, mSceneMatStencil);
		}
		else
			actor->draw(renderer->getCurrentCommandBuffer(), camFrustum, mSceneMat);

#ifdef NSHIPPING
		actor->drawCamera(renderer->getCurrentCommandBuffer());
#endif
	}
}

void WidgetSceneApp::drawCollider()
{
	service(Physics::Core::BulPhysicsEngine).debugDrawWorld(mCamera.viewProj());
}

void WidgetSceneApp::drawSky()
{
	skyBox->updateUniform(mCamera.viewProj(), mCamera.mPosition, mCamera.mFar);
	skyBox->draw(renderer->getCurrentCommandBuffer(), *skyMat);
}

void WidgetSceneApp::drawArrow()
{
	if (mTreeItem->mSelectedActor != nullptr)
	{
		mTreeItem->mSelectedActor->mUniformBuferOutLine.mData.mViewProjection = mCamera.viewProj();
		
		const Maths::FVector3& aPos = mTreeItem->mSelectedActor->getTransform()->getWorldPosition();
		Maths::FQuaternion aRot = Maths::FQuaternion::inverse(mTreeItem->mSelectedActor->getTransform()->getWorldRotation());
		Maths::FVector3 aScale = mTreeItem->mSelectedActor->getTransform()->getWorldScale()/* + outLineWidth*/;

		mTreeItem->mSelectedActor->mUniformBuferOutLine.mData.mModel = mTreeItem->mSelectedActor->getTransform()->getWorldMatrix();//Maths::FMatrix4::createTransformMatrixQ(aPos, aRot, aScale);
		mTreeItem->mSelectedActor->mUniformBuferOutLine.updateData();


		mOutline->bindDescriptor("ubo", mTreeItem->mSelectedActor->mUniformBuferOutLine.mDescriptorSets);
		mOutline->bindPipeLine(renderer->getCurrentCommandBuffer());
		mTreeItem->mSelectedActor->draw(renderer->getCurrentCommandBuffer(), camFrustum);

#ifdef NSHIPPING
		mTreeItem->mSelectedActor->drawCamera(renderer->getCurrentCommandBuffer());
#endif

		pickingArrow->mActor = mTreeItem->mSelectedActor;
		pickingArrow->drawScene(renderer->getCurrentCommandBuffer(), mCamera.viewProj(), mCamera.mPosition);
	}
}

void WidgetSceneApp::drawParticle()
{
	for (Editor::Data::Actor* actor : mActors)
	{
		actor->updateParticle(mClock.getDeltaTime(), mCamera.mPosition);
		actor->drawParticle(renderer->getCurrentCommandBuffer(), *ParticleMat, mCamera.viewProj(), mCamera.mView);
	}
}

void WidgetSceneApp::drawLightBillBoard()
{
#ifdef NSHIPPING
	if (mLightManager == nullptr)
		return;

	if (mLightManager->mLights == nullptr)
		return;


	for (auto& it : *mLightManager->mLights)
		it->draw(renderer->getCurrentCommandBuffer(), *LightBillMat, *pickingArrow->mAllArrow.scene, mCamera.viewProj(), mCamera.mView);
#endif
}

void WidgetSceneApp::drawDecals()
{
#ifdef NSHIPPING
	for (Editor::Data::Actor* actor : mActors)
		actor->drawDecals(renderer->getCurrentCommandBuffer(), *mDecal, mCamera.viewProj());
#endif
}

void WidgetSceneApp::waitForCleanUp()
{
	renderer->waitForCleanUp();
	offScreenRenderer->waitForCleanUp();

	std::unique_lock<std::mutex> lck(closeMut);
	closed = true;
	cv.notify_all();
}

void WidgetSceneApp::cleanupScene()
{	
	{
		std::unique_lock lock(renderer->mainThreadAction.mMutex);
		renderer->mainThreadAction.mDeletors.clear();
	}

	delete mShadowTexture;
	delete mShadowCubeTexture;

	delete mSceneMat;
	delete mSceneMatStencil;
	delete mOutline;
	delete mPickingMat;
	delete skyMat;
	delete ParticleMat;
	delete mLightBillPickingMat;
	delete LightBillMat;
	delete mShadowMat;
	delete mShadowCubeMat;
	delete mDecal;

	delete pickingArrow;
	delete skyBox;

	if (mLightManager != nullptr)
		delete mLightManager;

	delete offScreenRenderer;
}

void WidgetSceneApp::lookAtActor(Editor::Data::Actor& pActor)
{
	Maths::FTransform* transform = pActor.getTransform();
	const Maths::FVector3& size = transform->getWorldScale();
	float max = Maths::fmax(Maths::fmax(size.x, size.y), size.z);
	float dist = max / tan(Maths::degreesToRadians(mCamera.mFOV) / 2);

	mCamera.resetCameraAngle();
	mCamera.mPosition = transform->getWorldPosition();
	mCamera.mPosition.z -= dist;
	mCamera.updateView();
}