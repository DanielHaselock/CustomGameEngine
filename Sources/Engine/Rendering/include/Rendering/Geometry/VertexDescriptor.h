#pragma once
#include <vulkan/vulkan_core.h>
#include <array>

namespace Rendering::Geometry
{
	VkVertexInputBindingDescription getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 7> getAttributeDescriptions();
}