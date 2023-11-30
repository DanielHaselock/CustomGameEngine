#pragma once

#include <vulkan/vulkan_core.h>

namespace Rendering::Data
{
	struct UploadContext 
	{
		VkFence mUploadFence;
		VkCommandPool mCommandPool;
		VkCommandBuffer mCommandBuffer;
	};
}