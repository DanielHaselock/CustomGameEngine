#include "Rendering/Context/Driver.h"

using namespace Rendering::Context;

VkInstance Driver::mInstance = nullptr;
VkPhysicalDevice Driver::mPhysicalDevice = nullptr;
VkDevice Driver::mDevice = nullptr;