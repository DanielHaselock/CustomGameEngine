#pragma once
#include <vulkan/vulkan_core.h>

namespace Rendering::Context
{
	struct Driver
	{
		static VkInstance mInstance;
		static VkPhysicalDevice mPhysicalDevice;
		static VkDevice mDevice;
	};
}