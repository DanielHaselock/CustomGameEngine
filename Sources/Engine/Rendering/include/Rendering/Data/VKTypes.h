#pragma once

#include <VulkanMemoryAllocator/vk_mem_alloc.h>
#include <vector>
#include <vulkan/vulkan.h>

namespace Rendering::Data
{
    struct AllocatedBuffer 
    {
        VkBuffer mBuffer;
        VmaAllocation mAllocation;
    };

    struct AllocatedImage 
    {
        VkImage mImage;
        VmaAllocation mAllocation;
    };
}