#pragma once
#include <vulkan/vulkan_core.h>
#include <VulkanMemoryAllocator/vk_mem_alloc.h>
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Buffers::VK
{
	class VKBuffer
	{
		public:
			static Data::AllocatedBuffer createBuffer(size_t pAllocSize, VkBufferUsageFlags pUsage, VmaMemoryUsage pMemoryUsage);
			static void copyBuffer(size_t pAllocSize, VkBuffer pSrc, VkBuffer pDest);
			static void mapBuffer(const void* pData, size_t pAllocSize, VmaAllocation pAllocation);
	};
}