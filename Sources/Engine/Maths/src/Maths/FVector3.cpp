#include <utility>
#include <string>
#include <stdexcept>
#include <cmath>

#include "Maths/FVector3.h"
#include "Maths/Utils.h"

using namespace Maths;

const FVector3 FVector3::One(1.0f, 1.0f, 1.0f);
const FVector3 FVector3::Zero(0.0f, 0.0f, 0.0f);
const FVector3 FVector3::Forward(0.0f, 0.0f, 1.0f);
const FVector3 FVector3::Backward(0.0f, 0.0f, -1.0f);
const FVector3 FVector3::Right(1.0f, 0.0f, 0.0f);
const FVector3 FVector3::Left(-1.0f, 0.0f, 0.0f);
const FVector3 FVector3::Up(0.0f, 1.0f, 0.0f);
const FVector3 FVector3::Bottom(0.0f, -1.0f, 0.0f);

FVector3::FVector3() : x(0.0f), y(0.0f), z(0.0f) {}

FVector3::FVector3(float pX, float pY, float pZ) : x(pX), y(pY), z(pZ) {}

FVector3::FVector3(const FVector3& pToCopy) : x(pToCopy.x), y(pToCopy.y), z(pToCopy.z) {}

FVector3::FVector3(float pValue) : x(pValue), y(pValue), z(pValue) {}

FVector3 FVector3::operator-() const
{
	return operator*(-1);
}

FVector3 FVector3::operator=(const FVector3& pOther)
{
	x = pOther.x;
	y = pOther.y;
	z = pOther.z;

	return *this;
}

FVector3 FVector3::operator+(const FVector3& pOther) const
{
	return add(*this, pOther);
}

FVector3& FVector3::operator+=(const FVector3& pOther)
{
	*this = add(*this, pOther);
	return *this;
}

FVector3 FVector3::operator-(const FVector3& pOther) const
{
	return substract(*this, pOther);
}

FVector3& FVector3::operator-=(const FVector3& pOther)
{
	*this = substract(*this, pOther);
	return *this;
}

FVector3 Maths::FVector3::operator*(const FVector3& pOther) const
{
	return FVector3(x * pOther.x, y * pOther.y, z * pOther.z);
}

FVector3 Maths::FVector3::operator*=(const FVector3& pOther)
{
	*this = FVector3(x * pOther.x, y * pOther.y, z * pOther.z);
	return *this;
}

FVector3 FVector3::operator*(float pScalar) const
{
	return multiply(*this, pScalar);
}

FVector3& FVector3::operator*=(float pScalar)
{
	*this = multiply(*this, pScalar);
	return *this;
}

FVector3 FVector3::operator/(float pScalar) const
{
	return divide(*this, pScalar);
}

FVector3 FVector3::operator/(const FVector3& pOther) const
{
	return FVector3(x / pOther.x, y / pOther.y, z / pOther.z);
}

FVector3& FVector3::operator/=(float pScalar)
{
	*this = divide(*this, pScalar);
	return *this;
}

FVector3& FVector3::operator/=(const FVector3& pOther)
{
	*this = FVector3(x / pOther.x, y / pOther.y, z / pOther.z);
	return *this;
}

bool FVector3::operator==(const FVector3& pOther)
{
	return x == pOther.x && y == pOther.y && z == pOther.z;
}

bool FVector3::operator!=(const FVector3& pOther)
{
	return !operator==(pOther);
}

float& FVector3::operator[](int pIdx)
{
	if (pIdx >= 3)
		throw std::out_of_range("Invalid index : " + std::to_string(pIdx) + " is out of range");

	if (pIdx == 0)
		return x;
	else if (pIdx == 1)
		return y;
	return z;
}

float FVector3::operator[](int pIdx) const
{
	if (pIdx >= 3)
		throw std::out_of_range("Invalid index : " + std::to_string(pIdx) + " is out of range");

	if (pIdx == 0)
		return x;
	else if (pIdx == 1)
		return y;
	return z;
}

FVector3 FVector3::add(const FVector3& pLeft, const FVector3& pRight)
{
	return FVector3(pLeft.x + pRight.x, pLeft.y + pRight.y, pLeft.z + pRight.z);
}

FVector3 FVector3::substract(const FVector3& pLeft, const FVector3& pRight)
{
	return FVector3(pLeft.x - pRight.x, pLeft.y - pRight.y, pLeft.z - pRight.z);
}

FVector3 FVector3::multiply(const FVector3& pTarget, float pScalar)
{
	return FVector3(pTarget.x * pScalar, pTarget.y * pScalar, pTarget.z * pScalar);
}

FVector3 FVector3::divide(const FVector3& pLeft, float pScalar)
{
	FVector3 result(pLeft);

	if (pScalar == 0)
		throw std::logic_error("Division by 0");

	result.x /= pScalar;
	result.y /= pScalar;
	result.z /= pScalar;

	return result;
}

float FVector3::length(const FVector3& pTarget)
{
	return sqrtf(lengthSquared(pTarget));
}

float Maths::FVector3::length()
{
	return length(*this);
}

float FVector3::lengthSquared(const FVector3& pTarget)
{
	return pTarget.x * pTarget.x + pTarget.y * pTarget.y + pTarget.z * pTarget.z;
}

float Maths::FVector3::lengthSquared()
{
	return lengthSquared(*this);
}

float FVector3::dot(const FVector3& pLeft, const FVector3& pRight)
{
	return pLeft.x * pRight.x + pLeft.y * pRight.y + pLeft.z * pRight.z;
}

float Maths::FVector3::dot(const FVector3& pRight)
{
	return dot(*this, pRight);
}

float FVector3::distance(const FVector3& pLeft, const FVector3& pRight)
{
	return std::sqrtf((pLeft.x - pRight.x) * (pLeft.x - pRight.x) + (pLeft.y - pRight.y) * (pLeft.y - pRight.y) + (pLeft.z - pRight.z) * (pLeft.z - pRight.z));
}

float Maths::FVector3::distance(const FVector3& pRight)
{
	return distance(*this, pRight);
}

float FVector3::distanceSquared(const FVector3& pLeft, const FVector3& pRight)
{
	return (pLeft.x - pRight.x) * (pLeft.x - pRight.x) + (pLeft.y - pRight.y) * (pLeft.y - pRight.y) + (pLeft.z - pRight.z) * (pLeft.z - pRight.z);
}

float Maths::FVector3::distanceSquared(const FVector3& pRight)
{
	return distanceSquared(*this, pRight);
}

float FVector3::distance2DFrom(const FVector3& pLeft, const FVector3& pRight)
{
	return std::sqrt((pLeft.x - pRight.x) * (pLeft.x - pRight.x) + (pLeft.y - pRight.y) * (pLeft.y - pRight.y));
}

float Maths::FVector3::distance2DFrom(const FVector3& pRight)
{
	return distance2DFrom(*this, pRight);
}

float FVector3::distance2DSquaredFrom(const FVector3& pLeft, const FVector3& pRight)
{
	return (pLeft.x - pRight.x) * (pLeft.x - pRight.x) + (pLeft.y - pRight.y) * (pLeft.y - pRight.y);
}

float Maths::FVector3::distance2DSquaredFrom(const FVector3& pRight)
{
	return distance2DSquaredFrom(*this, pRight);
}

FVector3 FVector3::cross(const FVector3& pLeft, const FVector3& pRight)
{
	return FVector3(pLeft.y * pRight.z - pLeft.z * pRight.y,
		pLeft.z * pRight.x - pLeft.x * pRight.z,
		pLeft.x * pRight.y - pLeft.y * pRight.x);
}

FVector3 Maths::FVector3::cross(const FVector3& pRight)
{
	return cross(*this, pRight);
}

FVector3 FVector3::normalize(const FVector3& pTarget)
{
	float aLength = length(pTarget);

	if (aLength > 0.0f)
	{
		float targetLength = 1.0f / aLength;

		return FVector3(pTarget.x * targetLength, pTarget.y * targetLength, pTarget.z * targetLength);
	}

	return FVector3::Zero;
}

FVector3 FVector3::normalize()
{
	return normalize(*this);
}

float FVector3::magnitude(const FVector3& pTarget)
{
	return length(pTarget);
}

float FVector3::magnitude()
{
	return length(*this);
}

float FVector3::magnitudeSquared(const FVector3& pTarget)
{
	return lengthSquared(pTarget);
}

float FVector3::magnitudeSquared()
{
	return lengthSquared(*this);
}

FVector3 FVector3::lerp(const FVector3& pA, const FVector3& pB, float pT)
{
	return (pA + (pB - pA) * pT);
}

FVector3 Maths::FVector3::lerp(const FVector3& pB, float pT)
{
	return lerp(*this, pB, pT);
}

float FVector3::angleBetween(const FVector3& pFrom, const FVector3& pTo)
{
	float lengthProduct = length(pFrom) * length(pTo);

	if (lengthProduct > 0.0f)
	{
		float fractionResult = dot(pFrom, pTo) / lengthProduct;

		if (fractionResult >= -1.0f && fractionResult <= 1.0f)
			return acosf(fractionResult);
	}

	return 0.0f;
}

FVector3 FVector3::vectorMax(const FVector3& pA, const FVector3& pB)
{
	return FVector3(Maths::fmax(pA.x, pB.x), Maths::fmax(pA.y, pB.y), Maths::fmax(pA.z, pB.z));
}

void FVector3::vectorMax(const FVector3& pB)
{
	*this = vectorMax(*this, pB);
}

FVector3 FVector3::vectorMin(const FVector3& pA, const FVector3& pB)
{
	return FVector3(Maths::fmin(pA.x, pB.x), Maths::fmin(pA.y, pB.y), Maths::fmin(pA.z, pB.z));
}

void FVector3::vectorMin(const FVector3& pB)
{
	*this = vectorMin(*this, pB);
}

bool Maths::operator==(const FVector3& pLeft, const FVector3& pRight)
{
	return pLeft.x == pRight.x && pLeft.y == pRight.y && pLeft.z == pRight.z;
}

bool Maths::operator!=(const FVector3& pLeft, const FVector3& pRight)
{
	return !(pLeft == pRight);
}