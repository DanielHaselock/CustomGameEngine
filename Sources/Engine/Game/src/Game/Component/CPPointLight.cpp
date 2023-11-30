#pragma once
#include "Game/Component/CPPointLight.h"
#include <Game/Utils/ComponentType.h>
#include "Game/Data/Actor.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Core/EngineApp.h"
#ifdef NSHIPPING
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "Editor/Widget/WidgetSceneApp.h"
#include "Rendering/LineDrawer.h"
#endif

using namespace Game::Component;

CPPointLight::CPPointLight()
{
	mUniformBuffer = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataShadowCube>(VK_SHADER_STAGE_VERTEX_BIT);

	service(EngineCore::Core::EngineApp).mLightManager->pointLight++;
#ifdef NSHIPPING
	mUniformBufferEditor = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataShadowCube>(VK_SHADER_STAGE_VERTEX_BIT);

	service(Editor::Widget::WidgetSceneApp).mLightManager->pointLight++;
	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>("Resources/Editor/Textures/glob.png", "Resources/Editor/Textures/glob.png");
	mRadiusDrawer = new Rendering::LineDrawer(*service(Editor::Widget::WidgetSceneApp).renderer);
	updateDrawer();
#endif

	ready = true;
}

CPPointLight::~CPPointLight()
{
	service(EngineCore::Core::EngineApp).mLightManager->pointLight--;
	delete mUniformBuffer;
#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).mLightManager->pointLight--;
	delete mUniformBufferEditor;
	delete mRadiusDrawer;
#endif
}

AComponent* CPPointLight::clone()
{
	return new CPPointLight(*this);
}

void CPPointLight::fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData)
{
	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	pData.light[pIdx].mLightType = Rendering::Data::POINT_LIGHT;
	pData.light[pIdx].mColor = mColor;
	pData.light[pIdx].mAtenuation = mAttenuatuion;
	pData.light[pIdx].mRadius = mRadius;
	pData.light[pIdx].mBrightness = mBrightness;
	pData.light[pIdx].mPos = actor->getTransform()->mWorldPosition;
	pData.light[pIdx].mCastShadow = mCastShadow;
}

#ifdef NSHIPPING
void CPPointLight::updateDrawer()
{
	if (!isActive)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	isInit = true;

	mRadiusDrawer->reset();
	Maths::FVector3& pos = actor->getTransform()->mWorldPosition;
	float radius = mRadius;
	mRadiusDrawer->drawCircle(pos, radius);
	mRadiusDrawer->drawCircle(pos, radius, true);
}

void CPPointLight::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

	CPLight::draw(pCmd, pPipeLineBill, pPipeLineModel, pViewProj, pView);

	if (mRadiusDrawer != nullptr)
	{
		if (!isInit)
			updateDrawer();

		mRadiusDrawer->updateViewProj(pViewProj);
		mRadiusDrawer->flushLines();
	}
}
#endif

void CPPointLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::PointLight);

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

	pWriter.Key("Radius");
	pWriter.Double(mRadius);

	pWriter.Key("Brightness");
	pWriter.Double(mBrightness);

	pWriter.Key("CastShadow");
	pWriter.Bool(mCastShadow);

	pWriter.EndObject();
}