#include "Rendering/Renderer/IRenderer.h"

using namespace Rendering::Renderer;

int IRenderer::mInstance = 0;
std::recursive_mutex IRenderer::mInstanceLock;
VmaAllocator IRenderer::mAllocator = nullptr;

void IRenderer::waitForCleanUp()
{
    std::unique_lock lock(IRenderer::mInstanceLock);
	vkDeviceWaitIdle(mDriver.mDevice);
}

void IRenderer::insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);
}

uint32_t IRenderer::findMemoryType(uint32_t pTypeFilter, VkMemoryPropertyFlags pProperties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(mDriver.mPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if ((pTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & pProperties) == pProperties)
            return i;

    throw std::runtime_error("failed to find suitable memory type!");
}

void IRenderer::createImage(uint32_t pWidth, uint32_t pHeight, uint32_t pLayerCount, uint32_t pMipLevels, VkFormat pFormat, VkImageTiling pTiling, VkImageUsageFlags pUsage, VkImageCreateFlags pFlags, VkMemoryPropertyFlags pProperties, VkImage& pImage, VkDeviceMemory& pImageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = pWidth;
    imageInfo.extent.height = pHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = pMipLevels;
    imageInfo.arrayLayers = pLayerCount;
    imageInfo.format = pFormat;
    imageInfo.tiling = pTiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = pUsage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = pFlags;

    if (vkCreateImage(mDriver.mDevice, &imageInfo, nullptr, &pImage) != VK_SUCCESS)
        throw std::runtime_error("failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(mDriver.mDevice, pImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, pProperties);

    if (vkAllocateMemory(mDriver.mDevice, &allocInfo, nullptr, &pImageMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate image memory!");

    vkBindImageMemory(mDriver.mDevice, pImage, pImageMemory, 0);
}