#pragma once
#include "Maths/FMatrix4.h"

namespace Rendering::Data
{
	struct BoneInfo
	{
		int mId;
		Maths::FMatrix4 mOffset;
	};
}