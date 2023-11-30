#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Resources/Texture.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Resources/Loader/ImageLoader.h"
#include "Rendering/Buffers/VK/VKBuffer.h"
#include "Rendering/Renderer/VK/VKInitializers.h"
#include "Rendering/Context/VkDescriptor.h"

using namespace Rendering::Resources;

VkFormat Texture::imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
VkFormat Texture::imageFormatText = VK_FORMAT_R8_UNORM;

Texture::Texture(const std::string& pFileName)
{
	createTextureImage(pFileName);
	createTextureImageView();
	createTextureSampler();
	createDescritorSet();
}

Texture::Texture(unsigned int width, unsigned int height, unsigned char* data)
{
	createTextureImageText(width, height, data);
	createTextureImageViewText();
	createTextureSamplerText();
	createDescritorSet();
}

Texture::~Texture()
{
	vkDestroySampler(service(VkDevice), mTextureSampler, nullptr);
	vmaDestroyImage(service(VmaAllocator), mTexture.mImage, mTexture.mAllocation);
	vkDestroyImageView(service(VkDevice), mImageView, nullptr);
}

void Texture::createTextureImage(const std::string& pFileName)
{
	int texWidth, texHeight;
	unsigned char* pixels = Rendering::Resources::Loaders::ImageLoader::import(pFileName.c_str(), texWidth, texHeight);

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	Data::AllocatedBuffer stagingBuffer = Buffers::VK::VKBuffer::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	Buffers::VK::VKBuffer::mapBuffer(pixels, imageSize, stagingBuffer.mAllocation);

	Resources::Loaders::ImageLoader::cleanUpImport(pixels);

	VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;
	VkImageCreateInfo dimg_info = Renderer::VK::VKInit::imageCreateInfo(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaCreateImage(service(VmaAllocator), &dimg_info, &dimg_allocinfo, &mTexture.mImage, &mTexture.mAllocation, nullptr);

	Rendering::Renderer::VK::VKRenderer::immediateSubmit([&](VkCommandBuffer cmd)
		{
			VkImageSubresourceRange range;
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseMipLevel = 0;
			range.levelCount = 1;
			range.baseArrayLayer = 0;
			range.layerCount = 1;

			VkImageMemoryBarrier imageBarrier_toTransfer = {};
			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toTransfer.image = mTexture.mImage;
			imageBarrier_toTransfer.subresourceRange = range;

			imageBarrier_toTransfer.srcAccessMask = 0;
			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			//barrier the image into the transfer-receive layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = imageExtent;

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, stagingBuffer.mBuffer, mTexture.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			//barrier the image into the shader readable layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
		});

	vmaDestroyBuffer(service(VmaAllocator), stagingBuffer.mBuffer, stagingBuffer.mAllocation);
}

void Texture::createTextureImageText(unsigned int texWidth, unsigned int texHeight, unsigned char* pixels)
{
	VkDeviceSize imageSize = texWidth * texHeight;

	Data::AllocatedBuffer stagingBuffer = Buffers::VK::VKBuffer::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	Buffers::VK::VKBuffer::mapBuffer(pixels, imageSize, stagingBuffer.mAllocation);

	VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;
	VkImageCreateInfo dimg_info = Renderer::VK::VKInit::imageCreateInfo(imageFormatText, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaCreateImage(service(VmaAllocator), &dimg_info, &dimg_allocinfo, &mTexture.mImage, &mTexture.mAllocation, nullptr);

	Rendering::Renderer::VK::VKRenderer::immediateSubmit([&](VkCommandBuffer cmd)
		{
			VkImageSubresourceRange range;
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseMipLevel = 0;
			range.levelCount = 1;
			range.baseArrayLayer = 0;
			range.layerCount = 1;

			VkImageMemoryBarrier imageBarrier_toTransfer = {};
			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toTransfer.image = mTexture.mImage;
			imageBarrier_toTransfer.subresourceRange = range;

			imageBarrier_toTransfer.srcAccessMask = 0;
			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			//barrier the image into the transfer-receive layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = imageExtent;

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, stagingBuffer.mBuffer, mTexture.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			//barrier the image into the shader readable layout
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
		});

	vmaDestroyBuffer(service(VmaAllocator), stagingBuffer.mBuffer, stagingBuffer.mAllocation);
}

void Texture::createTextureImageView()
{
	VkImageViewCreateInfo imageinfo = Renderer::VK::VKInit::imageviewCreateInfo(imageFormat, mTexture.mImage, VK_IMAGE_ASPECT_COLOR_BIT);
	vkCreateImageView(service(VkDevice), &imageinfo, nullptr, &mImageView);
}

void Texture::createTextureImageViewText()
{
	VkImageViewCreateInfo imageinfo = Renderer::VK::VKInit::imageviewCreateInfo(imageFormatText, mTexture.mImage, VK_IMAGE_ASPECT_COLOR_BIT);
	vkCreateImageView(service(VkDevice), &imageinfo, nullptr, &mImageView);
}

void Texture::createTextureSampler()
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
}

void Texture::createTextureSamplerText()
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(service(VkPhysicalDevice), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
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
}

void Texture::createDescritorSet()
{
	VkDescriptorImageInfo mImageInfo{ mTextureSampler, mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	Context::DescriptorCache::begin().bind_image(0, &mImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(mTextureSets);
}