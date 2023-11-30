#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Rendering/Resources/IResource.h"
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Resources
{
	class TextureCube : public IResource
	{
		public:
			Data::AllocatedImage mTexture;
			VkImageView mImageView;
			VkSampler mTextureSampler;
			VkDescriptorSet mTextureSets;
			VkExtent3D imageExtent;
			static VkFormat imageFormat;

			TextureCube(const std::string& pFileName);
			TextureCube(const std::string& pFileNameLeft, const std::string& pFileNameRight,
				const std::string& pFileNameTop, const std::string& pFileNamebottom, 
				const std::string& pFileNameFront, const std::string& pFileNameBack);
			~TextureCube() override;

			void createTextureImage(const std::string& pFileName);

			void createTextureImage(const std::string& pFileNameLeft, const std::string& pFileNameRight,
				const std::string& pFileNameTop, const std::string& pFileNamebottom,
				const std::string& pFileNameFront, const std::string& pFileNameBack);

			void createTextureImageView();
			void createTextureSampler();
			void createDescritorSet();
	};
}