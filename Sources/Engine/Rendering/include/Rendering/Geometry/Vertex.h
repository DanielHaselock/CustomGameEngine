#pragma once

#include "Maths/FVector2.h"
#include "Maths/FVector3.h"
#include "Maths/FVector4.h"
#include "Maths/IVector4.h"

namespace Rendering::Geometry
{
	struct Vertex
	{
		Maths::FVector3 mPosition;
		Maths::FVector2 mTexCoords;
		Maths::FVector3 mNormals;
		Maths::FVector3 mTangent;
		Maths::FVector3 mBitangent;
		Maths::IVector4 mBoneIDs;
		Maths::FVector4 mWeights;

		Vertex();
	};
}