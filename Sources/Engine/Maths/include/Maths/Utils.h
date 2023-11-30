#pragma once

#define PI 3.14159265358979323846
#define EPSILON 0.00001f
#define HALF_CIRCLE 180.0f

namespace Maths
{
	const double radiansToDegrees(const double pRad);

	const double degreesToRadians(const double pDeg);

	const float lerp(const float pA, const float pB, const float pT);

	const float unLerp(const float pA, const float pB, const float pT);

	const float clamp(const float pVal, const float pMin, const float pMax);

	const float fmax(const float pA, const float pB);

	const float fmin(const float pA, const float pB);

	const int randomInt(const int pMin, const int pMax);

	const float randomFloat(const float pMin, const float pMax);

	const int sign(const float pValue);
}