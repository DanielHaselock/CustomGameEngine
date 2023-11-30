#pragma once
#include <vulkan/vulkan_core.h>

namespace Rendering::Data
{
	struct FrameData
	{
		VkSemaphore mPresentSemaphore, mRenderSemaphore;
		VkFence mRenderFence;

		VkCommandPool mCommandPool;
		VkCommandBuffer mMainCommandBuffer;
	};
}