#pragma once
#include <vector>
#include <mutex>
#include <VulkanMemoryAllocator/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include "Rendering/Context/Driver.h"
#include "Rendering/Data/VKTypes.h"
#include "Rendering/Data/DeletionQueue.h"

namespace Rendering::Renderer
{
	class IRenderer
	{
		public:
			static int mInstance;
			static std::recursive_mutex mInstanceLock;
			static VmaAllocator mAllocator;
			const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
			const std::vector<const char*> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME };
			Context::Driver mDriver;
			VkQueue mGraphicsQueue;
			uint32_t mGraphicsQueueFamily;
			VkImageView mDepthImageView;
			Data::AllocatedImage mDepthImage;
			VkFormat mDepthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
			VkFormat mColorFormat = VK_FORMAT_R8G8B8A8_UNORM;

			VkRenderPass mRenderPass;
			Data::DeletionQueue mMainDeletionQueue;
			VkExtent2D mWindowExtent;

			void waitForCleanUp();
			virtual VkCommandBuffer getCurrentCommandBuffer() = 0;
			void insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
				VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);
			
			uint32_t findMemoryType(uint32_t pTypeFilter, VkMemoryPropertyFlags pProperties);
			void createImage(uint32_t pWidth, uint32_t pHeight, uint32_t pLayerCount, uint32_t pMipLevels, VkFormat pFormat, VkImageTiling pTiling, VkImageUsageFlags pUsage, VkImageCreateFlags pFlags, VkMemoryPropertyFlags pProperties, VkImage& pImage, VkDeviceMemory& pImageMemory);
		
	};
}