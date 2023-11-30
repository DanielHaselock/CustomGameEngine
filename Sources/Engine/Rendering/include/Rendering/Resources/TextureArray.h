#pragma once
#include "Rendering/Resources/IResource.h"
#include <vulkan/vulkan_core.h>
#include "Rendering/Data/VKTypes.h"
#include "vector"
#include "Rendering/Resources/Texture.h"

namespace Rendering::Resources
{
	class TextureArray : public IResource
	{
		public:
			std::vector<VkDescriptorImageInfo> descriptorImageInfos;
			VkDescriptorSet mTextureSets;

			TextureArray(int pSize);
			void addTexture(const VkDescriptorImageInfo& pInfo);
			void addTexture(Rendering::Resources::Texture& pTexture);
			int size();
			void createDescritorSet();
	};
}