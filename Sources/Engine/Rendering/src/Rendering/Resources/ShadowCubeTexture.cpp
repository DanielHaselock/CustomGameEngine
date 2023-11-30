#include "Rendering/Resources/ShadowCubeTexture.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Context/VkDescriptor.h"

using namespace Rendering::Resources;

ShadowCubeTexture::ShadowCubeTexture(Rendering::Renderer::VK::VKRenderer& pRenderer)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(service(VkPhysicalDevice), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 1;
	samplerInfo.mipLodBias = 0.0f;

	if (vkCreateSampler(service(VkDevice), &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");


	VkDescriptorImageInfo mImageInfo{ mTextureSampler, pRenderer.mShadowCube.mColorView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	Context::DescriptorCache::begin()
		.bind_image(0, &mImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(mTextureSets);
}

ShadowCubeTexture::~ShadowCubeTexture()
{
	vkDestroySampler(service(VkDevice), mTextureSampler, nullptr);
}
