#pragma once
#include <optional>

namespace Rendering::Data
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> mGraphicsFamily;
        std::optional<uint32_t> mPresentFamily;

        bool isComplete();
    };
}