#include "Maths/Utils.h"

#include <random>

const double Maths::radiansToDegrees(const double pRad)
{
	return pRad * (HALF_CIRCLE / PI);
}

const double Maths::degreesToRadians(const double pDeg)
{
	return pDeg * (PI / HALF_CIRCLE);
}

const float Maths::lerp(const float pA, const float pB, const float pT)
{
	Maths::clamp(0, 1, pT);
	return (pA + (pB - pA) * pT);
}

const float Maths::unLerp(const float pA, const float pB, const float pT)
{
	return Maths::clamp(((pT - pA) / (pB - pA)), 0, 1);
}

const float Maths::clamp(const float pVal, const float pMin, const float pMax)
{
	const float t = pVal < pMin ? pMin : pVal;
	return t > pMax ? pMax : t;
}

const float Maths::fmax(const float pA, const float pB)
{
	return (pA < pB) ? pB : pA;
}

const float Maths::fmin(const float pA, const float pB)
{
	return !(pB < pA) ? pA : pB;
}

const int Maths::randomInt(const int pMin, const int pMax)
{
	return rand() % (pMax - pMin + 1) + pMin;
}

const float Maths::randomFloat(const float pMin, const float pMax)
{
	return pMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (pMax - pMin)));
}

const int Maths::sign(const float pValue)
{
	if (pValue > 0) return 1;
	if (pValue < 0) return -1;
	return 0;
}
