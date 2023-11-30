#pragma once
#include "Game/Component/CPLight.h"
#include <Game/Utils/ComponentType.h>
#include "EngineCore/Service/ServiceLocator.h"
#include "Game/SceneSys/SceneManager.h"

using namespace Game::Component;

CPLight::CPLight()
{
	service(Game::SceneSys::SceneManager).mCurrentScene->mLights.push_back(this);
#ifdef NSHIPPING
	mUniformBuffer = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataLightBillboard>(VK_SHADER_STAGE_VERTEX_BIT);
	mUniformBufferId = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataLightBillboardId>(VK_SHADER_STAGE_VERTEX_BIT);
	mQuad = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>("Resources/Engine/Models/FaceQuad.obj", "Resources/Engine/Models/FaceQuad.obj");
#endif;
}

CPLight::~CPLight()
{
	service(Game::SceneSys::SceneManager).mCurrentScene->mLights.remove(this);
#ifdef NSHIPPING
	delete mUniformBuffer;
	delete mUniformBufferId;
#endif
}

AComponent* CPLight::clone()
{
	return new CPLight(*this);
}

#ifdef NSHIPPING
void CPLight::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	mUniformBuffer->mData.mViewProjection = pViewProj;
	mUniformBuffer->mData.mRight = Maths::FVector3(pView.data[0][0], pView.data[1][0], pView.data[2][0]);
	mUniformBuffer->mData.mUp = Maths::FVector3(pView.data[0][1], pView.data[1][1], pView.data[2][1]);
	mUniformBuffer->mData.mPos = actor->getTransform()->mWorldPosition;
	mUniformBuffer->updateData();

	pPipeLineBill.bindDescriptor("texSampler", mTexture->mTextureSets);
	pPipeLineBill.bindDescriptor("ubo", mUniformBuffer->mDescriptorSets);
	pPipeLineBill.bindPipeLine(pCmd);

	mQuad->draw(pCmd);
};

void CPLight::drawOffscreen(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	mUniformBufferId->mData.mViewProjection = pViewProj;
	mUniformBufferId->mData.mRight = Maths::FVector3(pView.data[0][0], pView.data[1][0], pView.data[2][0]);
	mUniformBufferId->mData.mUp = Maths::FVector3(pView.data[0][1], pView.data[1][1], pView.data[2][1]);
	mUniformBufferId->mData.mPos = actor->getTransform()->mWorldPosition;
	mUniformBufferId->mData.id = actor->id;
	mUniformBufferId->updateData();

	pPipeLine.bindDescriptor("ubo", mUniformBufferId->mDescriptorSets);
	pPipeLine.bindPipeLine((VkCommandBuffer)pCmd);

	mQuad->draw(pCmd);
}
#endif

void CPLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Light);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.EndObject();
}

Maths::FVector3 CPLight::getPos()
{
	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return Maths::FVector3::Bottom;


	return actor->getTransform()->mWorldPosition;
}

Maths::FVector3 CPLight::getDir()
{
	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return Maths::FVector3::Bottom;


	return (actor->getTransform()->mWorldRotation * Maths::FVector3::Bottom).normalize();
}