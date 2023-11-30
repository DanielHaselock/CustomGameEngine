#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Resources/TextureCube.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Resources/Loader/ImageLoader.h"
#include "Rendering/Buffers/VK/VKBuffer.h"
#include "Rendering/Renderer/VK/VKInitializers.h"
#include "Rendering/Context/VkDescriptor.h"
#include "Maths/Utils.h"

using namespace Rendering::Resources;

VkFormat TextureCube::imageFormat = VK_FORMAT_R8G8B8A8_UNORM;

TextureCube::TextureCube(const std::string& pFileName)
{
	createTextureImage(pFileName);
	createTextureImageView();
	createTextureSampler();
	createDescritorSet();
}

TextureCube::TextureCube(const std::string& pFileNameLeft, const std::string& pFileNameRight,
	const std::string& pFileNameTop, const std::string& pFileNamebottom,
	const std::string& pFileNameFront, const std::string& pFileNameBack)
{
	createTextureImage(pFileNameLeft, pFileNameRight, pFileNameTop, pFileNamebottom, pFileNameFront, pFileNameBack);
	createTextureImageView();
	createTextureSampler();
	createDescritorSet();
}

TextureCube::~TextureCube()
{
	vkDestroySampler(service(VkDevice), mTextureSampler, nullptr);
	vmaDestroyImage(service(VmaAllocator), mTexture.mImage, mTexture.mAllocation);
	vkDestroyImageView(service(VkDevice), mImageView, nullptr);
}

//void TextureCube::createTextureImage(const std::string& pFileName)
//{
//	int texWidth, texHeight;
//	unsigned char* pixels = Rendering::Resources::Loaders::ImageLoader::import(pFileName.c_str(), texWidth, texHeight);
//
//	VkDeviceSize imageSize = texWidth * texHeight * 4;
//
//	Data::AllocatedBuffer stagingBuffer = Buffers::VK::VKBuffer::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
//	Buffers::VK::VKBuffer::mapBuffer(pixels, imageSize, stagingBuffer.mAllocation);
//
//	Resources::Loaders::ImageLoader::cleanUpImport(pixels);
//
//	
//	imageExtent.width = static_cast<uint32_t>(texWidth / 4);
//	imageExtent.height = static_cast<uint32_t>(texHeight / 3);
//	imageExtent.depth = 1;
//
//	VkImageCreateInfo dimg_info = Renderer::VK::VKInit::imageCreateInfo(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);
//	dimg_info.arrayLayers = 6;
//	dimg_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
//
//	VmaAllocationCreateInfo dimg_allocinfo = {};
//	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//	vmaCreateImage(service(VmaAllocator), &dimg_info, &dimg_allocinfo, &mTexture.mImage, &mTexture.mAllocation, nullptr);
//
//	Rendering::Renderer::VK::VKRenderer::immediateSubmit([&](VkCommandBuffer cmd)
//		{
//			VkImageSubresourceRange range;
//			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//			range.baseMipLevel = 0;
//			range.levelCount = 1;
//			range.baseArrayLayer = 0;
//			range.layerCount = 6;
//
//			VkImageMemoryBarrier imageBarrier_toTransfer = {};
//			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//
//			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//			imageBarrier_toTransfer.image = mTexture.mImage;
//			imageBarrier_toTransfer.subresourceRange = range;
//
//			imageBarrier_toTransfer.srcAccessMask = 0;
//			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//			//barrier the image into the transfer-receive layout
//			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);
//
//
//			//copy the buffer into the image
//			std::vector<VkBufferImageCopy> bufferCopyRegions;
//			uint32_t offset = 0;
//
//			for (uint32_t face = 0; face < 6; face++)
//			{
//				// Calculate offset into staging buffer for the current mip level and face
//				int offset;
//				switch (face)
//				{
//				case 0: offset = imageExtent.width * 2;
//					break;
//				case 1: offset = 0;
//					break;
//				case 2: offset = imageExtent.width * 1;
//					break;
//				case 3: offset = imageExtent.width * 1;
//					break;
//				case 4: offset = imageExtent.width * 1;
//					break;
//				case 5: offset = imageExtent.width * 3;
//					break;
//				}
//					
//				VkBufferImageCopy bufferCopyRegion = {};
//				bufferCopyRegion.bufferOffset = offset;
//				bufferCopyRegion.bufferRowLength = imageExtent.width;
//				bufferCopyRegion.bufferImageHeight = imageExtent.height;
//
//				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//				bufferCopyRegion.imageSubresource.mipLevel = 0;
//				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
//				bufferCopyRegion.imageSubresource.layerCount = 1;
//				bufferCopyRegion.imageExtent.width = imageExtent.width;
//				bufferCopyRegion.imageExtent.height = imageExtent.height;
//				bufferCopyRegion.imageExtent.depth = 1;
//				bufferCopyRegions.push_back(bufferCopyRegion);
//			}
//			vkCmdCopyBufferToImage(cmd, stagingBuffer.mBuffer, mTexture.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
//
//			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;
//
//			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			//barrier the image into the shader readable layout
//			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
//		});
//
//	vmaDestroyBuffer(service(VmaAllocator), stagingBuffer.mBuffer, stagingBuffer.mAllocation);
//}


//pFileNameRight.c_str(),
//pFileNameLeft.c_str(), t
//pFileNameTop.c_str(), te
//pFileNamebottom.c_str(),
//pFileNameFront.c_str(),
//pFileNameBack.c_str(), t

void ProcessChunk(unsigned char* pImage, int x, int y, int width, int height)
{
	int pos = (y * width + x) * 4;
	unsigned char* posData = &pImage[pos];

	printf("%d\n", pos);

	static int i = 0;
	std::string path = std::to_string(i++) + "_Test.png";

	Rendering::Resources::Loaders::ImageLoader::exportPNG(path.c_str(), width, height, 4, posData);
} 
#define CHUNKSIZE 1024

void ProcessImage(unsigned char* pImage, int nWidth, int nHeight)
{
	int i = 0;
	for (int x = 0; x < nWidth; x += CHUNKSIZE, i++)
		for (int y = 0; y < nHeight; y += CHUNKSIZE)
			ProcessChunk(pImage, x * 4, y, CHUNKSIZE, CHUNKSIZE);
}

void TextureCube::createTextureImage(const std::string& pFileName)
{
	int texWidth, texHeight;
	unsigned char* pixelData = Rendering::Resources::Loaders::ImageLoader::import(pFileName.c_str(), texWidth, texHeight);
	

	Rendering::Resources::Loaders::ImageLoader::exportJPG("Left.jpg", texWidth, texHeight, 4, pixelData + 4096);
	//ProcessImage(pixelData, texWidth, texHeight);

	imageExtent.width = texWidth / 4;
	imageExtent.height = imageExtent.width;
	imageExtent.depth = 1;

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	

	

	Data::AllocatedBuffer stagingBuffer = Buffers::VK::VKBuffer::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	Buffers::VK::VKBuffer::mapBuffer(pixelData, imageSize, stagingBuffer.mAllocation);

	Resources::Loaders::ImageLoader::cleanUpImport(pixelData);


	VkImageCreateInfo dimg_info = Renderer::VK::VKInit::imageCreateInfo(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);
	dimg_info.arrayLayers = 6;
	dimg_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

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
			range.layerCount = 6;

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


			//copy the buffer into the image
			std::vector<VkBufferImageCopy> bufferCopyRegions;
			for (int i = 0; i < 6; i++)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = 0;
				bufferCopyRegion.imageSubresource.baseArrayLayer = i;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent = imageExtent;
				bufferCopyRegion.bufferOffset = (imageExtent.width * imageExtent.height * 4) * i;
				bufferCopyRegions.push_back(bufferCopyRegion);

			}

			//copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, stagingBuffer.mBuffer, mTexture.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6, bufferCopyRegions.data());


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

void TextureCube::createTextureImage(const std::string& pFileNameLeft, const std::string& pFileNameRight,
	const std::string& pFileNameTop, const std::string& pFileNamebottom,
	const std::string& pFileNameFront, const std::string& pFileNameBack)
{
	unsigned char* pixels[6];

	int texWidth, texHeight;
	pixels[0] = Rendering::Resources::Loaders::ImageLoader::import(pFileNameRight.c_str(), texWidth, texHeight);
	pixels[1] = Rendering::Resources::Loaders::ImageLoader::import(pFileNameLeft.c_str(), texWidth, texHeight);
	pixels[2] = Rendering::Resources::Loaders::ImageLoader::import(pFileNameTop.c_str(), texWidth, texHeight);
	pixels[3] = Rendering::Resources::Loaders::ImageLoader::import(pFileNamebottom.c_str(), texWidth, texHeight);
	pixels[4] = Rendering::Resources::Loaders::ImageLoader::import(pFileNameFront.c_str(), texWidth, texHeight);
	pixels[5] = Rendering::Resources::Loaders::ImageLoader::import(pFileNameBack.c_str(), texWidth, texHeight);

	VkDeviceSize imageSize = texWidth * texHeight * 4 * 6;
	VkDeviceSize layerSize = imageSize / 6;

	Data::AllocatedBuffer stagingBuffer = Buffers::VK::VKBuffer::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	
	void* data;
	for (int i = 0; i < 6; i++)
	{
		vmaMapMemory(service(VmaAllocator), stagingBuffer.mAllocation, &data);
		memcpy((char*)data + (layerSize * i), pixels[i], layerSize);
		vmaUnmapMemory(service(VmaAllocator), stagingBuffer.mAllocation);
	}

	for (int i = 0; i < 6; i++)
		Resources::Loaders::ImageLoader::cleanUpImport(pixels[i]);


	imageExtent.width = texWidth;
	imageExtent.height = texHeight;
	imageExtent.depth = 1;

	VkImageCreateInfo dimg_info = Renderer::VK::VKInit::imageCreateInfo(imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);
	dimg_info.arrayLayers = 6;
	dimg_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

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
			range.layerCount = 6;

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


			//copy the buffer into the image
			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 6;
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

void TextureCube::createTextureImageView()
{
	VkImageViewCreateInfo imageinfo = Renderer::VK::VKInit::imageviewCreateInfo(imageFormat, mTexture.mImage, VK_IMAGE_ASPECT_COLOR_BIT);
	imageinfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	imageinfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	imageinfo.subresourceRange.layerCount = 6;
	imageinfo.subresourceRange.levelCount = 1;
	vkCreateImageView(service(VkDevice), &imageinfo, nullptr, &mImageView);
}

void TextureCube::createTextureSampler()
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
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 1;
	samplerInfo.mipLodBias = 0.0f;

	if (vkCreateSampler(service(VkDevice), &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");
}

void TextureCube::createDescritorSet()
{
	VkDescriptorImageInfo mImageInfo{ mTextureSampler, mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	Context::DescriptorCache::begin().bind_image(0, &mImageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(mTextureSets);
}