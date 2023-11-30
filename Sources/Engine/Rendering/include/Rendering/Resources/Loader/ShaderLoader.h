#pragma once
#include "Rendering/Data/Shader.h"

namespace Rendering::Resources::Loaders
{
	class ShaderLoader
	{
	public:
		static bool loadModule(const char* pFilePath, Rendering::Data::ShaderModule* pOutShaderModule);
		static uint32_t hashDescriptorLayoutInfo(VkDescriptorSetLayoutCreateInfo* info);
	};
}