#include "Editor/Data/PickingArrow.h"
#include <EngineCore/Thread/ThreadPool.h>
#include <EngineCore/ResourceManagement/ResourceManager.h>
#include <EngineCore/Service/ServiceLocator.h>
#include <Rendering/Resources/VK/PipeLineBuilder.h>

#define ARROW_SIZE 8

using namespace Editor::Data;

Maths::FVector3 PickingArrow::red = {1, 0, 0};
Maths::FVector3 PickingArrow::green = { 0, 1, 0 };
Maths::FVector3 PickingArrow::blue = { 0, 0, 1 };

Arrows::Arrows(VkRenderPass pPass, VkRenderPass pOffScreenPass)
{
	EngineCore::Thread::ThreadPool& pool = service(EngineCore::Thread::ThreadPool);

	pool.queueJob([this, pPass, pOffScreenPass]
		{
			EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
			mTranslation = manager.create<Rendering::Resources::Model>("Resources/Editor/Models/Arrow_Translate.fbx", "Resources/Editor/Models/Arrow_Translate.fbx");
			mRotation = manager.create<Rendering::Resources::Model>("Resources/Editor/Models/Arrow_Rotate.fbx", "Resources/Editor/Models/Arrow_Rotate.fbx");
			mScale = manager.create<Rendering::Resources::Model>("Resources/Editor/Models/Arrow_Scale.fbx", "Resources/Editor/Models/Arrow_Scale.fbx");

			scene = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
				.initPipeLine("Resources/Editor/Shaders/Arrow.vert.spv", "Resources/Editor/Shaders/Arrow.frag.spv", pPass, false, false, false));

			picking = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
				.initPipeLine("Resources/Editor/Shaders/IdItem.vert.spv", "Resources/Editor/Shaders/IdArrow.frag.spv", pOffScreenPass, false, false, false));
		});
}

Arrows::~Arrows()
{
	delete scene;
	delete picking;
}

Rendering::Resources::Model* Arrows::getModel()
{
	switch (mMode)
	{
	case Editor::Data::ArrowMode::ETranslation: return mTranslation;
		break;
	case Editor::Data::ArrowMode::ERotation: return mRotation;
		break;
	case Editor::Data::ArrowMode::EScale: return mScale;
		break;
	default: return mTranslation;
		break;
	}
}

Arrow::Arrow() : mSceneUniform(VK_SHADER_STAGE_VERTEX_BIT), mIdUniform(VK_SHADER_STAGE_VERTEX_BIT)
{
	static int counter = 255;
	id = counter--;
}

void Arrow::setSceneData(Maths::FMatrix4& pViewProj, Maths::FMatrix4 pModelMatrix, Maths::FVector3 pColor)
{
	mSceneUniform.mData.mViewProjection = pViewProj;
	mSceneUniform.mData.mModel = pModelMatrix;
	mSceneUniform.mData.mColor = pColor;
	mSceneUniform.updateData();
}

void Arrow::setPickingData(Maths::FMatrix4& pViewProj, Maths::FMatrix4 pModelMatrix)
{
	mIdUniform.mData.mViewProjection = pViewProj;
	mIdUniform.mData.mModel = pModelMatrix;
	mIdUniform.mData.id = id;
	mIdUniform.updateData();
}

PickingArrow::PickingArrow(VkRenderPass pPass, VkRenderPass pOffScreenPass) : mAllArrow(pPass, pOffScreenPass)
{
	
}

void PickingArrow::setMode(Editor::Data::ArrowMode pMode)
{
	mAllArrow.mMode = pMode;
}

void PickingArrow::drawScene(VkCommandBuffer pCmd, Maths::FMatrix4& viewProj, Maths::FVector3& pCamView)
{
	Rendering::Resources::Model* mModelToUse = mAllArrow.getModel();
	
	Maths::FTransform& transform = *mActor->getTransform();

	Maths::FVector3 camPos = { viewProj.data[3][0], viewProj.data[3][1], viewProj.data[3][2] };
	float len = Maths::FVector3::length(pCamView - transform.mWorldPosition) / ARROW_SIZE;
		
	const Maths::FVector3& pos = transform.getWorldPosition();
	const Maths::FVector3& scale = { len, len, len };

	Maths::FQuaternion rot = /*Maths::FQuaternion::inverse*/(transform.getWorldRotation());
	
	//Right
	{
		right.setSceneData(viewProj, Maths::FMatrix4::createTransformMatrixQ(pos, rot * Maths::FQuaternion(Maths::FVector3(0, 90, 0)), scale), red);

		mAllArrow.scene->bindDescriptor("ubo", right.mSceneUniform.mDescriptorSets);
		mAllArrow.scene->bindPipeLine(pCmd);
		mModelToUse->draw(pCmd);
	}

	//up
	{
		up.setSceneData(viewProj, Maths::FMatrix4::createTransformMatrixQ(pos, rot * Maths::FQuaternion(Maths::FVector3(90, 0, 0)), scale), green);

		mAllArrow.scene->bindDescriptor("ubo", up.mSceneUniform.mDescriptorSets);
		mAllArrow.scene->bindPipeLine(pCmd);
		mModelToUse->draw(pCmd);
	}

	//forward
	{
		forward.setSceneData(viewProj, Maths::FMatrix4::createTransformMatrixQ(pos, rot * Maths::FQuaternion(Maths::FVector3(0, 0, 0)), scale), blue);

		mAllArrow.scene->bindDescriptor("ubo", forward.mSceneUniform.mDescriptorSets);
		mAllArrow.scene->bindPipeLine(pCmd);
		mModelToUse->draw(pCmd);
	}
}

void PickingArrow::drawPicking(VkCommandBuffer pCmd, Maths::FMatrix4& viewProj, Maths::FVector3& pCamView)
{
	Rendering::Resources::Model* mModelToUse = mAllArrow.getModel();

	Maths::FTransform& transform = *mActor->getTransform();

	Maths::FVector3 camPos = { viewProj.data[3][0], viewProj.data[3][1], viewProj.data[3][2] };
	float len = Maths::FVector3::length(pCamView - transform.mWorldPosition) / ARROW_SIZE;

	const Maths::FVector3& pos = transform.getWorldPosition();
	const Maths::FVector3& scale = { len, len, len };


	Maths::FQuaternion rot = /*Maths::FQuaternion::inverse*/(transform.getWorldRotation());

	//Right
	{
		right.setPickingData(viewProj, Maths::FMatrix4::createTransformMatrixQ(pos, rot * Maths::FQuaternion(Maths::FVector3(0, 90, 0)), scale));

		mAllArrow.picking->bindDescriptor("ubo", right.mIdUniform.mDescriptorSets);
		mAllArrow.picking->bindPipeLine(pCmd);
		mModelToUse->draw(pCmd);
	}

	//up
	{
		up.setPickingData(viewProj, Maths::FMatrix4::createTransformMatrixQ(pos, rot * Maths::FQuaternion(Maths::FVector3(90, 0, 0)), scale));

		mAllArrow.picking->bindDescriptor("ubo", up.mIdUniform.mDescriptorSets);
		mAllArrow.picking->bindPipeLine(pCmd);
		mModelToUse->draw(pCmd);
	}

	//forward
	{
		forward.setPickingData(viewProj, Maths::FMatrix4::createTransformMatrixQ(pos, rot * Maths::FQuaternion(Maths::FVector3(0, 0, 0)), scale));

		mAllArrow.picking->bindDescriptor("ubo", forward.mIdUniform.mDescriptorSets);
		mAllArrow.picking->bindPipeLine(pCmd);
		mModelToUse->draw(pCmd);
	}
}

void PickingArrow::beginDrag(int pId, Editor::Data::Camera& pCamera)
{
	isDrag = true;

	if (pId == right.id)
	{
		axis = Maths::FVector3::Right;
		leftRight = pCamera.mForward.z < 0 ? 1 : -1;
	}
	else if (pId == up.id)
	{
		axis = Maths::FVector3::Up;
		leftRight = pCamera.mUp.y < 0 ? -1 : 1;
	}
	else if (pId == forward.id)
	{
		axis = Maths::FVector3::Forward;
		leftRight = pCamera.mForward.x < 0 ? -1 : 1;
	}
}

void PickingArrow::moveDrag(Maths::FVector2 mousePos)
{
	float value = (mousePos.length() / sensitivity);
	if (axis == Maths::FVector3::Up)
		value *= (mousePos.y > 0 ? -1 : 1);
	else
		value *= (mousePos.x < 0 ? -1 : 1);

	switch (mAllArrow.mMode)
	{
	case Editor::Data::ArrowMode::ETranslation: translate(value);
		break;
	case Editor::Data::ArrowMode::ERotation: rotate(value);
		break;
	case Editor::Data::ArrowMode::EScale: scale(value);
		break;
	default: translate(value);
		break;
	}

	mActor->mValueChanged.dispatch();
}

void PickingArrow::endDrag()
{
	isDrag = false;
}

void PickingArrow::translate(float value)
{
	Maths::FVector3 dir = mActor->getTransform()->mWorldRotation * axis * leftRight;

	mActor->getTransform()->addLocalPosition(dir * value);
	mActor->updateTransform();
#ifdef NSHIPPING
	mActor->updateLight();
#endif
}

void PickingArrow::rotate(float value)
{
	mActor->getTransform()->addLocalRotation(axis * value);
	mActor->updateTransform();
}

void PickingArrow::scale(float value) 
{
	mActor->getTransform()->addLocalScale(axis * value);
	mActor->updateTransform();
}