#include <utility>
#include <string>
#include <stdexcept>

#include "Maths/IVector4.h"

using namespace Maths;

const IVector4 IVector4::One(1, 1, 1, 1);
const IVector4 IVector4::Zero(0, 0, 0, 0);

IVector4::IVector4(int pX, int pY, int pZ, int pW) : x(pX), y(pY), z(pZ), w(pW)
{
}

IVector4::IVector4(const IVector4& pToCopy) : x(pToCopy.x), y(pToCopy.y), z(pToCopy.z), w(pToCopy.w)
{
}

IVector4 IVector4::operator-() const
{
	return operator*(-1);
}

IVector4 IVector4::operator=(const IVector4& pOther)
{
	x = pOther.x;
	y = pOther.y;
	z = pOther.z;
	w = pOther.w;

	return *this;
}

IVector4 IVector4::operator+(const IVector4& pOther) const
{
	return add(*this, pOther);
}

IVector4& IVector4::operator+=(const IVector4& pOther)
{
	*this = add(*this, pOther);
	return *this;
}

IVector4 IVector4::operator-(const IVector4& pOther) const
{
	return substract(*this, pOther);
}

IVector4& IVector4::operator-=(const IVector4& pOther)
{
	*this = substract(*this, pOther);
	return *this;
}

IVector4 IVector4::operator*(int pScalar) const
{
	return multiply(*this, pScalar);
}

IVector4& IVector4::operator*=(int pScalar)
{
	*this = multiply(*this, pScalar);
	return *this;
}

IVector4 IVector4::operator/(int pScalar) const
{
	return divide(*this, pScalar);
}

IVector4& IVector4::operator/=(int pScalar)
{
	*this = divide(*this, pScalar);
	return *this;
}

bool IVector4::operator==(const IVector4 & pOther)
{
	return x == pOther.x && y == pOther.y && z == pOther.z && w == pOther.w;
}

bool IVector4::operator!=(const IVector4 & pOther)
{
	return !operator==(pOther);
}

int& IVector4::operator[](int pIdx)
{
	if (pIdx >= 4)
		throw std::out_of_range("Invalid index : " + std::to_string(pIdx) + " is out of range");

	if (pIdx == 0)
		return x;
	else if (pIdx == 1)
		return y;
	else if (pIdx == 2)
		return z;
	return w;
}

int IVector4::operator[](int pIdx) const
{
	if (pIdx >= 4)
		throw std::out_of_range("Invalid index : " + std::to_string(pIdx) + " is out of range");

	if (pIdx == 0)
		return x;
	else if (pIdx == 1)
		return y;
	else if (pIdx == 2)
		return z;
	return w;
}

IVector4 IVector4::add(const IVector4& pLeft, const IVector4& pRight)
{
	return IVector4(pLeft.x + pRight.x, pLeft.y + pRight.y, pLeft.z + pRight.z, pLeft.w + pRight.w);
}

IVector4 IVector4::substract(const IVector4& pLeft, const IVector4& pRight)
{
	return IVector4(pLeft.x - pRight.x, pLeft.y - pRight.y, pLeft.z - pRight.z, pLeft.w - pRight.w);
}

IVector4 IVector4::multiply(const IVector4& pTarget, int pScalar)
{
	return IVector4(pTarget.x * pScalar, pTarget.y * pScalar, pTarget.z * pScalar, pTarget.w * pScalar);
}

IVector4 IVector4::divide(const IVector4& pLeft, int pScalar)
{
	IVector4 result(pLeft);

	if (pScalar == 0)
		throw std::logic_error("Division by 0");

	result.x /= pScalar;
	result.y /= pScalar;
	result.z /= pScalar;
	result.w /= pScalar;

	return result;
}

int IVector4::length(const IVector4& pTarget)
{
	return sqrtf(pTarget.x * pTarget.x + pTarget.y * pTarget.y + pTarget.z * pTarget.z + pTarget.w * pTarget.w);
}

int Maths::IVector4::length()
{
	return length(*this);
}

int IVector4::dot(const IVector4& pLeft, const IVector4& pRight)
{
	return pLeft.x * pRight.x + pLeft.y * pRight.y + pLeft.z * pRight.z + pLeft.w * pRight.w;
}

int Maths::IVector4::dot(const IVector4& pRight)
{
	return dot(*this, pRight);
}

IVector4 IVector4::normalize(const IVector4& pTarget)
{
	int aLength = length(pTarget);

	if (aLength > 0.0f)
	{
		int targetLength = 1.0f / aLength;

		return IVector4(pTarget.x * targetLength, pTarget.y * targetLength, pTarget.z * targetLength, pTarget.w * targetLength);
	}

	return IVector4::Zero;
}

IVector4 Maths::IVector4::normalize()
{
	return normalize(*this);
}

IVector4 IVector4::lerp(const IVector4& pA, const IVector4& pB, int pT)
{
	return (pA + (pB - pA) * pT);
}