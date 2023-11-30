#pragma once
#include "Maths/FVector3.h"
#include "Maths/FVector4.h"
#include "Maths/Frustrum.h"
#include "Maths/FTransform.h"
#include "Maths/Utils.h"

namespace Rendering::Data
{
	struct BoundingBox
	{
		Maths::FVector3 mMin;
		Maths::FVector3 mMax;
		Maths::FVector3 mSize;
		Maths::FVector4 corners[8];

		void computeCorner();

		/*bool isOnOrForwardPlane(const Maths::Plane& plane) const;

		bool isOnFrustum(const Maths::Frustum& camFrustum, const Maths::FTransform& transform);*/
	};
}