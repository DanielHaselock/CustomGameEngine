#pragma once
#include <vulkan/vulkan_core.h>
#include <array>

namespace Rendering::Geometry
{
	VkVertexInputBindingDescription getDebugBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> getDebugAttributeDescriptions();
}