#pragma once
#include "Game/Component/CPSpotLight.h"
#include <Game/Utils/ComponentType.h>
#include "Game/Data/Actor.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Game/SceneSys/SceneManager.h"
#include "EngineCore/Core/EngineApp.h"
#ifdef NSHIPPING
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "Editor/Widget/WidgetSceneApp.h"
#endif

using namespace Game::Component;

CPSpotLight::CPSpotLight()
{
	service(EngineCore::Core::EngineApp).mLightManager->spotLight++;
#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).mLightManager->spotLight++;

	mArrow = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>("Resources/Editor/Models/Arrow_Translate.fbx", "Resources/Editor/Models/Arrow_Translate.fbx");
	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>("Resources/Editor/Textures/flashlight.png", "Resources/Editor/Textures/flashlight.png");
	
	mUniformBufferArrow = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataWithColor>(VK_SHADER_STAGE_VERTEX_BIT);
#endif

	updateRad();
}

CPSpotLight::~CPSpotLight()
{
	service(EngineCore::Core::EngineApp).mLightManager->spotLight--;
#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).mLightManager->spotLight--;
	delete mUniformBufferArrow;
#endif
}

AComponent* CPSpotLight::clone()
{
	return new CPSpotLight(*this);
}

void CPSpotLight::updateRad()
{
	mCutOffRad = std::cosf(Maths::degreesToRadians(mCutOff));
	mOuterCutOffRad = std::cosf(Maths::degreesToRadians(mOuterCutOff));
}

void CPSpotLight::fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData)
{
	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	pData.light[pIdx].mLightType = Rendering::Data::SPOT_LIGHT;
	pData.light[pIdx].mColor = mColor;
	pData.light[pIdx].mAtenuation = mAttenuatuion;
	pData.light[pIdx].mPos = actor->getTransform()->mWorldPosition;
	pData.light[pIdx].mDirection = (actor->getTransform()->mWorldRotation * Maths::FVector3::Bottom).normalize();
	pData.light[pIdx].mCutOff = mCutOffRad;
	pData.light[pIdx].mOuterCutOff = mOuterCutOffRad;
	pData.light[pIdx].mBrightness = mBrightness;
	pData.light[pIdx].mCastShadow = mCastShadow;
}

#ifdef NSHIPPING
void CPSpotLight::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

	CPLight::draw(pCmd, pPipeLineBill, pPipeLineModel, pViewProj, pView);

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	Maths::FTransform& transform = *actor->getTransform();
	const Maths::FVector3& pos = transform.getWorldPosition();
	const Maths::FVector3& scale = { 2, 2, 2 };

	mUniformBufferArrow->mData.mViewProjection = pViewProj;
	mUniformBufferArrow->mData.mModel = Maths::FMatrix4::createTransformMatrixQ(pos, transform.mWorldRotation * Maths::FQuaternion(Maths::FVector3(-90, 0, 0)), scale);
	mUniformBufferArrow->mData.mColor = Maths::FVector3(0, 0, 0);
	mUniformBufferArrow->updateData();

	pPipeLineModel.bindDescriptor("ubo", mUniformBufferArrow->mDescriptorSets);
	pPipeLineModel.bindPipeLine(pCmd);

	mArrow->draw(pCmd);
}
#endif

void CPSpotLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::SpotLight);

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

	pWriter.Key("CutOff");
	pWriter.Double(mCutOff);

	pWriter.Key("OuterCutOff");
	pWriter.Double(mOuterCutOff);

	pWriter.Key("Brightness");
	pWriter.Double(mBrightness);

	pWriter.Key("CastShadow");
	pWriter.Bool(mCastShadow);

	pWriter.EndObject();
}