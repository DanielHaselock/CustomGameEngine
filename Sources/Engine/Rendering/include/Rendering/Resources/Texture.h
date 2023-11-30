#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Rendering/Resources/IResource.h"
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Resources
{
	class Texture : public IResource
	{
		public:
			Data::AllocatedImage mTexture;
			VkImageView mImageView;
			VkSampler mTextureSampler;
			VkDescriptorSet mTextureSets;
			static VkFormat imageFormat;
			static VkFormat imageFormatText;

			Texture(const std::string& pFileName);
			Texture(unsigned int width, unsigned int height, unsigned char* data);
			~Texture() override;

			void createTextureImage(const std::string& pFileName);
			void createTextureImageText(unsigned int width, unsigned int height, unsigned char* data);
			void createTextureImageView();
			void createTextureImageViewText();
			void createTextureSampler();
			void createTextureSamplerText();
			void createDescritorSet();
	};
}