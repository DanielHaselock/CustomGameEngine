#include "EngineCore/Light/LightManager.h"

using namespace EngineCore::Light;

LightManager::LightManager()
{
}

void LightManager::init()
{
	mUniformBuffer = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataLight>(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}

LightManager::~LightManager()
{
	delete mUniformBuffer;
}
