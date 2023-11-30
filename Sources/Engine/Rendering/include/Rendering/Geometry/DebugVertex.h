#pragma once

namespace Rendering::Geometry
{
	struct DebugVertex
	{
		DebugVertex() {};
		DebugVertex(float pX, float pY, float pZ, float pR, float pG, float pB)
		{
			mPosition[0] = pX;
			mPosition[1] = pY;
			mPosition[2] = pZ;

			mColor[0] = pR;
			mColor[1] = pG;
			mColor[2] = pB;
		}

		float mPosition[3];
		float mColor[3];

	};
}