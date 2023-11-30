#pragma once
#include "Maths/FMatrix4.h"

namespace Rendering::Data
{
	struct GridData
	{
		alignas(16) Maths::FMatrix4 mView;
		alignas(16) Maths::FMatrix4 mProjection;
	};
}