#pragma once
#include "Game/Component/CPLight.h"
#include "List"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"

namespace EngineCore::Light
{
	class LightManager
	{
		public:
			std::list<Game::Component::CPLight*>* mLights = nullptr;
			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataLight>* mUniformBuffer = nullptr;
			int spotLight = 0;
			int pointLight = 0;
			LightManager();
			void init();
			~LightManager();
	};
}