#pragma once
#include "Game/Component/CPDirLight.h"
#include <Game/Utils/ComponentType.h>
#include "Game/Data/Actor.h"
#include "EngineCore/Core/EngineApp.h"
#ifdef NSHIPPING
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "Editor/Widget/WidgetSceneApp.h"
#endif


using namespace Game::Component;

CPDirLight::CPDirLight()
{
	service(EngineCore::Core::EngineApp).mLightManager->spotLight++;
#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).mLightManager->spotLight++;

	mArrow = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>("Resources/Editor/Models/Arrow_Translate.fbx", "Resources/Editor/Models/Arrow_Translate.fbx");
	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>("Resources/Editor/Textures/sun.png", "Resources/Editor/Textures/sun.png");

	mUniformBufferArrow = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataWithColor>(VK_SHADER_STAGE_VERTEX_BIT);
#endif
}

CPDirLight::~CPDirLight()
{
	service(EngineCore::Core::EngineApp).mLightManager->spotLight--;
#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).mLightManager->spotLight--;
	delete mUniformBufferArrow;
#endif
}

AComponent* CPDirLight::clone()
{
	return new CPDirLight(*this);
}

void CPDirLight::fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData)
{
	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	pData.light[pIdx].mLightType = Rendering::Data::DIR_LIGHT;
	pData.light[pIdx].mDirection = (actor->getTransform()->mWorldRotation * Maths::FVector3::Bottom).normalize();
	pData.light[pIdx].mColor = mColor;
	pData.light[pIdx].mAtenuation = mAttenuatuion;
	pData.light[pIdx].mCastShadow = mCastShadow;
	pData.light[pIdx].mBrightness = mBrightness;
}

#ifdef NSHIPPING
void CPDirLight::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

	CPLight::draw(pCmd, pPipeLineBill, pPipeLineModel, pViewProj, pView);

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	Maths::FTransform& transform = *actor->getTransform();
	const Maths::FVector3& pos = transform.getWorldPosition();
	const Maths::FVector3& scale = {2, 2, 2};

	mUniformBufferArrow->mData.mViewProjection = pViewProj;
	mUniformBufferArrow->mData.mModel = Maths::FMatrix4::createTransformMatrixQ(pos, transform.mWorldRotation * Maths::FQuaternion(Maths::FVector3(-90, 0, 0)), scale);
	mUniformBufferArrow->mData.mColor = Maths::FVector3(0, 0, 0);
	mUniformBufferArrow->updateData();

	pPipeLineModel.bindDescriptor("ubo", mUniformBufferArrow->mDescriptorSets);
	pPipeLineModel.bindPipeLine(pCmd);

	mArrow->draw(pCmd);
}
#endif

void CPDirLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::DirLight);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Color");
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	pWriter.StartArray();
		pWriter.Double(mColor.x);
		pWriter.Double(mColor.y);
		pWriter.Double(mColor.z);
	pWriter.EndArray();
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	pWriter.Key("Attenuation");
	pWriter.Double(mAttenuatuion);

	pWriter.Key("CastShadow");
	pWriter.Bool(mCastShadow);

	pWriter.Key("Brightness");
	pWriter.Double(mBrightness);

	pWriter.EndObject();
}