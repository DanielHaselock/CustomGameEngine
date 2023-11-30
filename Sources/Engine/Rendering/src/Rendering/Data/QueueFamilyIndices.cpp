#include "Rendering/Data/QueueFamilyIndices.h"

using namespace Rendering::Data;

bool QueueFamilyIndices::isComplete()
{
    return mGraphicsFamily.has_value() && mPresentFamily.has_value();
}