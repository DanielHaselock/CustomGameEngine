#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>

#include "Maths/Utils.h"
#include "Maths/FQuaternion.h"

#undef PI

#define PI (4.f * atan(1.f))
#define TO_RADIANS(value) value * PI / 180.f
#define TO_DEGREES(value) value * 180.f / PI

using namespace Maths;

const FQuaternion FQuaternion::Identity = FQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

FQuaternion::FQuaternion() :
	x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{
}

FQuaternion::FQuaternion(float pReal) :
	x(0.0f), y(0.0f), z(0.0f), w(pReal)
{
}

FQuaternion::FQuaternion(float pX, float pY, float pZ, float pW) :
	x(pX), y(pY), z(pZ), w(pW)
{
}

FQuaternion::FQuaternion(const FQuaternion& pOther) :
	x(pOther.x), y(pOther.y), z(pOther.z), w(pOther.w)
{
}

FQuaternion::FQuaternion(const FMatrix3& pRotationMatrix)
{
	float trace = pRotationMatrix.data[0][0] + pRotationMatrix.data[1][1] + pRotationMatrix.data[2][2];
	if (trace > 0)
	{
		w = std::sqrtf(trace + 1.0f) * 0.5f;
		float s = 0.25f / w;

		x = (pRotationMatrix.data[1][2] - pRotationMatrix.data[2][1]) * s;
		y = (pRotationMatrix.data[2][0] - pRotationMatrix.data[0][2]) * s;
		z = (pRotationMatrix.data[0][1] - pRotationMatrix.data[1][0]) * s;
	}
	else if (pRotationMatrix.data[0][0] > pRotationMatrix.data[1][1] && pRotationMatrix.data[0][0] > pRotationMatrix.data[2][2])
	{
		x = std::sqrtf(pRotationMatrix.data[0][0] - pRotationMatrix.data[1][1] - pRotationMatrix.data[2][2] + 1.0f) * 0.5f;
		float s = 0.25f / x;

		y = (pRotationMatrix.data[0][1] + pRotationMatrix.data[1][0]) * s;
		z = (pRotationMatrix.data[2][0] + pRotationMatrix.data[0][2]) * s;
		w = (pRotationMatrix.data[1][2] - pRotationMatrix.data[2][1]) * s;
	}
	else if (pRotationMatrix.data[1][1] > pRotationMatrix.data[2][2])
	{
		y = std::sqrtf(pRotationMatrix.data[1][1] - pRotationMatrix.data[0][0] - pRotationMatrix.data[2][2] + 1.0f) * 0.5f;
		float s = 0.25f / y;

		x = (pRotationMatrix.data[0][1] + pRotationMatrix.data[1][0]) * s;
		z = (pRotationMatrix.data[1][2] + pRotationMatrix.data[2][1]) * s;
		w = (pRotationMatrix.data[2][0] - pRotationMatrix.data[0][2]) * s;
	}
	else
	{
		z = std::sqrtf(pRotationMatrix.data[2][2] - pRotationMatrix.data[0][0] - pRotationMatrix.data[1][1] + 1.0f) * 0.5f;
		float s = 0.25f / z;

		x = (pRotationMatrix.data[2][0] + pRotationMatrix.data[0][2]) * s;
		y = (pRotationMatrix.data[1][2] + pRotationMatrix.data[2][1]) * s;
		w = (pRotationMatrix.data[0][1] - pRotationMatrix.data[1][0]) * s;
	}
}

FQuaternion::FQuaternion(const FMatrix4& pRotationMatrix)
{
	float trace = pRotationMatrix.data[0][0] + pRotationMatrix.data[1][1] + pRotationMatrix.data[2][2];
	if (trace > 0)
	{
		w = std::sqrtf(trace + 1.0f) * 0.5f;
		float s = 0.25f / w;

		x = (pRotationMatrix.data[1][2] - pRotationMatrix.data[2][1]) * s;
		y = (pRotationMatrix.data[2][0] - pRotationMatrix.data[0][2]) * s;
		z = (pRotationMatrix.data[0][1] - pRotationMatrix.data[1][0]) * s;
	}
	else if (pRotationMatrix.data[0][0] > pRotationMatrix.data[1][1] && pRotationMatrix.data[0][0] > pRotationMatrix.data[2][2])
	{
		x = std::sqrtf(pRotationMatrix.data[0][0] - pRotationMatrix.data[1][1] - pRotationMatrix.data[2][2] + 1.0f) * 0.5f;
		float s = 0.25f / x;

		y = (pRotationMatrix.data[0][1] + pRotationMatrix.data[1][0]) * s;
		z = (pRotationMatrix.data[2][0] + pRotationMatrix.data[0][2]) * s;
		w = (pRotationMatrix.data[1][2] - pRotationMatrix.data[2][1]) * s;
	}
	else if (pRotationMatrix.data[1][1] > pRotationMatrix.data[2][2])
	{
		y = std::sqrtf(pRotationMatrix.data[1][1] - pRotationMatrix.data[0][0] - pRotationMatrix.data[2][2] + 1.0f) * 0.5f;
		float s = 0.25f / y;

		x = (pRotationMatrix.data[0][1] + pRotationMatrix.data[1][0]) * s;
		z = (pRotationMatrix.data[1][2] + pRotationMatrix.data[2][1]) * s;
		w = (pRotationMatrix.data[2][0] - pRotationMatrix.data[0][2]) * s;
	}
	else
	{
		z = std::sqrtf(pRotationMatrix.data[2][2] - pRotationMatrix.data[0][0] - pRotationMatrix.data[1][1] + 1.0f) * 0.5f;
		float s = 0.25f / z;

		x = (pRotationMatrix.data[2][0] + pRotationMatrix.data[0][2]) * s;
		y = (pRotationMatrix.data[1][2] + pRotationMatrix.data[2][1]) * s;
		w = (pRotationMatrix.data[0][1] - pRotationMatrix.data[1][0]) * s;
	}
}

FQuaternion::FQuaternion(const FVector3& pEuler)
{
	/* Degree to radians then times 0.5f = 0.0087f */
	float yaw = TO_RADIANS(pEuler.z) * 0.5f;
	float pitch = TO_RADIANS(pEuler.y) * 0.5f;
	float roll = TO_RADIANS(pEuler.x) * 0.5f;

	float cy = cos(yaw);
	float sy = sin(yaw);
	float cp = cos(pitch);
	float sp = sin(pitch);
	float cr = cos(roll);
	float sr = sin(roll);

	x = sr * cp * cy - cr * sp * sy;
	y = cr * sp * cy + sr * cp * sy;
	z = cr * cp * sy - sr * sp * cy;
	w = cr * cp * cy + sr * sp * sy;
}

FQuaternion FQuaternion::lookAt(const FVector3& pForward, const FVector3& pUp)
{
	auto vector = FVector3::normalize(pForward);
	auto vector2 = FVector3::normalize(FVector3::cross(pUp, vector));
	auto vector3 = FVector3::cross(vector, vector2);
	auto m00 = vector2.x;
	auto m01 = vector2.y;
	auto m02 = vector2.z;
	auto m10 = vector3.x;
	auto m11 = vector3.y;
	auto m12 = vector3.z;
	auto m20 = vector.x;
	auto m21 = vector.y;
	auto m22 = vector.z;

	float num8 = (m00 + m11) + m22;
	FQuaternion quaternion = FQuaternion::Identity;
	if (num8 > 0.f)
	{
		auto num = sqrt(num8 + 1.f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		auto num7 = sqrt(((1.f + m00) - m11) - m22);
		auto num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		auto num6 = sqrt(((1.f + m11) - m00) - m22);
		auto num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	auto num5 = sqrt(((1.f + m22) - m00) - m11);
	auto num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

bool FQuaternion::isIdentity(const FQuaternion& pTarget)
{
	return pTarget.w == 1.0f && length(pTarget) == 1.0f;
}

bool Maths::FQuaternion::isIdentity()
{
	return isIdentity(*this);
}

bool FQuaternion::isPure(const FQuaternion& pTarget)
{
	return pTarget.w == 0.0f && (pTarget.x != 0.0f || pTarget.y != 0.0f || pTarget.z != 0.0f);
}

bool Maths::FQuaternion::isPure()
{
	return isPure(*this);
}

bool FQuaternion::isNormalized(const FQuaternion& pTarget)
{
	return abs(length(pTarget) - 1.0f) < 0.0001f;
}

bool Maths::FQuaternion::isNormalized()
{
	return isNormalized(*this);
}

float FQuaternion::dotProduct(const FQuaternion& pLeft, const FQuaternion& pRight)
{
	return
		pLeft.x * pRight.x +
		pLeft.y * pRight.y +
		pLeft.z * pRight.z +
		pLeft.w * pRight.w;
}

float Maths::FQuaternion::dotProduct(const FQuaternion& pRight)
{
	return dotProduct(*this, pRight);
}

FQuaternion FQuaternion::normalize(const FQuaternion& pTarget)
{
	return pTarget / length(pTarget);
}

FQuaternion Maths::FQuaternion::normalize()
{
	return normalize(*this);
}

float FQuaternion::length(const FQuaternion& pTarget)
{
	return sqrtf(lengthSquared(pTarget));
}

float Maths::FQuaternion::length()
{
	return length(*this);
}

float FQuaternion::lengthSquared(const FQuaternion& pTarget)
{
	return pTarget.x * pTarget.x + pTarget.y * pTarget.y + pTarget.z * pTarget.z + pTarget.w * pTarget.w;
}

float Maths::FQuaternion::lengthSquared()
{
	return lengthSquared(*this);
}

float FQuaternion::getAngle(const FQuaternion& pTarget)
{
	return 2.0f * acos(pTarget.w);
}

float Maths::FQuaternion::getAngle()
{
	return getAngle(*this);
}

FVector3 FQuaternion::getRotationAxis(const FQuaternion& pTarget)
{
	const float S = sqrt(std::max(1.f - (pTarget.w * pTarget.w), 0.f));

	if (S >= EPSILON)
		return FVector3(pTarget.x / S, pTarget.y / S, pTarget.z / S);

	return FVector3(1.f, 0.f, 0.f);
}

FVector3 Maths::FQuaternion::getRotationAxis()
{
	return getRotationAxis(*this);
}

FQuaternion FQuaternion::inverse(const FQuaternion& pTarget)
{
	return conjugate(pTarget) / lengthSquared(pTarget);
}

FQuaternion Maths::FQuaternion::inverse()
{
	return inverse(*this);
}

FQuaternion FQuaternion::conjugate(const FQuaternion& pTarget)
{
	return { -pTarget.x, -pTarget.y, -pTarget.z, pTarget.w };
}

FQuaternion Maths::FQuaternion::conjugate()
{
	return conjugate(*this);
}

FQuaternion FQuaternion::square(const FQuaternion& pTarget)
{
	return pTarget * pTarget;
}

FQuaternion Maths::FQuaternion::square()
{
	return square(*this);
}

std::pair<FVector3, float> FQuaternion::getAxisAndAngle(const FQuaternion& pTarget)
{
	return std::pair<FVector3, float>(getRotationAxis(pTarget), getAngle(pTarget));
}

std::pair<FVector3, float> Maths::FQuaternion::getAxisAndAngle()
{
	return getAxisAndAngle(*this);
}

float FQuaternion::angularDistance(const FQuaternion& pLeft, const FQuaternion& pRight)
{
	float innerProd = (pLeft | pRight);
	return acos((2.0f * innerProd * innerProd) - 1.0f);
}

float Maths::FQuaternion::angularDistance(const FQuaternion& pRight)
{
	return angularDistance(*this, pRight);
}

FQuaternion FQuaternion::lerp(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha)
{
	pAlpha = std::clamp(pAlpha, 0.f, 1.f);

	FQuaternion q;

	if (FQuaternion::dotProduct(pStart, pEnd) < 0.f)
	{
		q.x = pStart.x + pAlpha * (-pEnd.x - pStart.x);
		q.y = pStart.y + pAlpha * (-pEnd.y - pStart.y);
		q.z = pStart.z + pAlpha * (-pEnd.z - pStart.z);
		q.w = pStart.w + pAlpha * (-pEnd.w - pStart.w);
	}
	else
	{
		q.x = Maths::lerp(pStart.x, pEnd.x, pAlpha);
		q.y = Maths::lerp(pStart.y, pEnd.y, pAlpha);
		q.z = Maths::lerp(pStart.z, pEnd.z, pAlpha);
		q.w = Maths::lerp(pStart.w, pEnd.w, pAlpha);
	}

	return FQuaternion::normalize(q);
}

FQuaternion FQuaternion::slerp(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha)
{
	FQuaternion from = pStart;
	FQuaternion to = pEnd;

	pAlpha = std::clamp(pAlpha, 0.f, 1.f);
	float cosAngle = FQuaternion::dotProduct(from, to);

	if (cosAngle < 0.f)
	{
		cosAngle = -cosAngle;
		to = FQuaternion(-to.x, -to.y, -to.z, -to.w);
	}

	if (cosAngle < 0.95f)
	{
		float angle = std::acos(cosAngle);
		float sinAngle = std::sin(angle);
		float invSinAngle = 1.f / sinAngle;
		float t1 = std::sin((1 - pAlpha) * angle) * invSinAngle;
		float t2 = std::sin(pAlpha * angle) * invSinAngle;
		return FQuaternion(from.x * t1 + to.x * t2, from.y * t1 + to.y * t2, from.z * t1 + to.z * t2, from.w * t1 + to.w * t2);
	}
	else
		return FQuaternion::lerp(from, to, pAlpha);
}

FQuaternion FQuaternion::nlerp(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha)
{
	return normalize(lerp(pStart, pEnd, pAlpha));
}

FQuaternion FQuaternion::mix(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha)
{
	return pStart * (1.0f - pAlpha) + pEnd * pAlpha;
}

FVector3 FQuaternion::rotatePoint(const FVector3& pPoint, const FQuaternion& pQuaternion)
{
	FVector3 q(pQuaternion.x, pQuaternion.y, pQuaternion.z);
	FVector3 t = FVector3::cross(q, pPoint) * 2.0f;

	return pPoint + (t * pQuaternion.w) + FVector3::cross(q, t);
}

FVector3 FQuaternion::rotatePoint(const FVector3& pPoint, const FQuaternion& pQuaternion, const FVector3& pPivot)
{
	FVector3 toRotate = pPoint - pPivot;
	return rotatePoint(toRotate, pQuaternion);
}

FVector3 FQuaternion::eulerAngles(FQuaternion& pTarget)
{
	pTarget = pTarget.normalize();

	//float roll, pitch, yaw = 0.f;

	// This is a kind of hack because when the input Quaternion is {0.5f, 0.5f, -0.5f, 0.5f} or
	// {0.5f, 0.5f, 0.5f, -0.5f}, the output value is incorrect.
	if (pTarget == FQuaternion{ 0.5f, 0.5f, -0.5f, 0.5f })  return { 0.0f, 90.0f, -90.0f };
	if (pTarget == FQuaternion{ 0.5f, 0.5f, 0.5f, -0.5f })  return { -90.0f, -90.0f, 0.0f };
	if (pTarget == FQuaternion{ -0.5f, 0.5f, 0.5f, 0.5f }) return { 0.0f, 90.0f, 90.0f };

	// roll (x-axis rotation)
	const float sinr_cosp = 2.0f * (pTarget.w * pTarget.x + pTarget.y * pTarget.z);
	const float cosr_cosp = 1.0f - 2.0f * (pTarget.x * pTarget.x + pTarget.y * pTarget.y);
	const float roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float pitch = 0.f;
	const float sinp = +2.0f * (pTarget.w * pTarget.y - pTarget.z * pTarget.x);
	if (fabs(sinp) >= 1)
		pitch = static_cast<float>(copysign(PI / 2.0f, sinp)); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	const float siny_cosp = 2.0f * (pTarget.w * pTarget.z + pTarget.x * pTarget.y);
	const float cosy_cosp = 1.0f - 2.0f * (pTarget.y * pTarget.y + pTarget.z * pTarget.z);
	const float yaw = atan2(siny_cosp, cosy_cosp);

	//float sqw = pTarget.w * pTarget.w;
	//float sqx = pTarget.x * pTarget.x;
	//float sqy = pTarget.y * pTarget.y;
	//float sqz = pTarget.z * pTarget.z;
	//float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	//float test = pTarget.x * pTarget.y + pTarget.z * pTarget.w;
	//if (test > 0.499 * unit) { // singularity at north pole
	//	roll = 2 * atan2(pTarget.x, pTarget.w);
	//	pitch = PI / 2;
	//	yaw = 0;
	//	return TO_DEGREES(FVector3(roll, pitch, yaw));;
	//}
	//if (test < -0.499 * unit) { // singularity at south pole
	//	roll = -2 * atan2(pTarget.x, pTarget.w);
	//	pitch = -PI / 2;
	//	yaw = 0;
	//	return TO_DEGREES(FVector3(roll, pitch, yaw));;
	//}
	//roll = atan2(2 * pTarget.y * pTarget.w - 2 * pTarget.x * pTarget.z, sqx - sqy - sqz + sqw);
	//pitch = asin(2 * test / unit);
	//yaw = atan2(2 * pTarget.x * pTarget.w - 2 * pTarget.y * pTarget.z, -sqx + sqy - sqz + sqw);

	return TO_DEGREES(FVector3(roll, pitch, yaw)); // XYZ
}

FVector3 Maths::FQuaternion::eulerAngles()
{
	return eulerAngles(*this);
}

FMatrix3 FQuaternion::toMatrix3(const FQuaternion& pTarget)
{
	FQuaternion base = normalize(pTarget);
	FMatrix3 mat3;

	mat3.data[0][0] = 1 - (2 * (base.y * base.y)) - (2 * (base.z * base.z));
	mat3.data[1][0] = 2 * ((base.x * base.y) - (base.z * base.w));
	mat3.data[2][0] = 2 * ((base.x * base.z) + (base.w * base.y));
	mat3.data[3][0] = 0;

	mat3.data[0][1] = 2 * ((base.x * base.y) + (base.w * base.z));
	mat3.data[1][1] = 1 - (2 * (base.x * base.x)) - (2 * (base.z * base.z));
	mat3.data[2][1] = 2 * ((base.y * base.z) - (base.w * base.x));
	mat3.data[3][1] = 0;

	mat3.data[0][2] = 2 * ((base.x * base.z) - (base.w * base.y));
	mat3.data[1][2] = 2 * ((base.y * base.z) + (base.w * base.x));
	mat3.data[2][2] = 1 - (2 * (base.x * base.x)) - (2 * (base.y * base.y));
	mat3.data[3][2] = 0;

	return mat3;
}

FMatrix3 Maths::FQuaternion::toMatrix3()
{
	return toMatrix3(*this);
}

FMatrix4 FQuaternion::toMatrix4(const FQuaternion& pTarget)
{
	FQuaternion base = normalize(pTarget);
	FMatrix4 result = FMatrix4::Identity;

	float qxx = base.x * base.x;
	float qyy = base.y * base.y;
	float qzz = base.z * base.z;
	float qxz = base.x * base.z;
	float qxy = base.x * base.y;
	float qyz = base.y * base.z;
	float qwx = base.w * base.x;
	float qwy = base.w * base.y;
	float qwz = base.w * base.z;

	result.data[0][0] = 1 - 2 * (qyy + qzz);
	result.data[0][1] = 2 * (qxy + qwz);
	result.data[0][2] = 2 * (qxz - qwy);

	result.data[1][0] = 2 * (qxy - qwz);
	result.data[1][1] = 1 - 2 * (qxx + qzz);
	result.data[1][2] = 2 * (qyz + qwx);

	result.data[2][0] = 2 * (qxz + qwy);
	result.data[2][1] = 2 * (qyz - qwx);
	result.data[2][2] = 1 - 2 * (qxx + qyy);

	return result;
}

FMatrix4 Maths::FQuaternion::toMatrix4()
{
	return toMatrix4(*this);
}

FQuaternion Maths::FQuaternion::toQuaternion(const FMatrix4& pMatrix4)
{
	return FQuaternion();
}

std::string Maths::FQuaternion::toString()
{
	return std::string("Quaternion: x: " + std::to_string(x) + " y: " + std::to_string(y) + " z: " + std::to_string(z) + " w: " + std::to_string(w));
}

bool FQuaternion::operator==(const FQuaternion& pOtherQuat) const
{
	return x == pOtherQuat.x && y == pOtherQuat.y && z == pOtherQuat.z && w == pOtherQuat.w;
}

bool FQuaternion::operator!=(const FQuaternion& pOtherQuat) const
{
	return x != pOtherQuat.x || y != pOtherQuat.y || z != pOtherQuat.z || w != pOtherQuat.w;
}

FQuaternion FQuaternion::operator+(const FQuaternion& pOtherQuat) const
{
	return FQuaternion(x + pOtherQuat.x, y + pOtherQuat.y,
		z + pOtherQuat.z, w + pOtherQuat.w);
}

FQuaternion& FQuaternion::operator+=(const FQuaternion& pOtherQuat)
{
	x += pOtherQuat.x;
	y += pOtherQuat.y;
	z += pOtherQuat.z;
	w += pOtherQuat.w;
	return *this;
}

FQuaternion FQuaternion::operator-() const
{
	return FQuaternion(-x, -y, -z, -w);
}

FQuaternion FQuaternion::operator-(const FQuaternion& pOtherQuat) const
{
	return FQuaternion(x - pOtherQuat.x, y - pOtherQuat.y,
		z - pOtherQuat.z, w - pOtherQuat.w);
}

FQuaternion& FQuaternion::operator-=(const FQuaternion& pOtherQuat)
{
	x -= pOtherQuat.x;
	y -= pOtherQuat.y;
	z -= pOtherQuat.z;
	w -= pOtherQuat.w;
	return *this;
}

float FQuaternion::operator|(const FQuaternion& pOtherQuat) const
{
	return (x * pOtherQuat.x + y * pOtherQuat.x + z *
		pOtherQuat.z + w * pOtherQuat.w);
}

FQuaternion FQuaternion::operator*(const float pScale) const
{
	FQuaternion result(*this);
	result.x *= pScale;
	result.y *= pScale;
	result.z *= pScale;
	result.w *= pScale;

	return result;
}

FQuaternion& FQuaternion::operator*=(const float pScale)
{
	x *= pScale;
	y *= pScale;
	z *= pScale;
	w *= pScale;

	return *this;
}

FQuaternion FQuaternion::operator*(const FQuaternion& pOtherQuat) const
{
	return FQuaternion
	(
		x * pOtherQuat.w + y * pOtherQuat.z - z * pOtherQuat.y + w * pOtherQuat.x,
		-x * pOtherQuat.z + y * pOtherQuat.w + z * pOtherQuat.x + w * pOtherQuat.y,
		x * pOtherQuat.y - y * pOtherQuat.x + z * pOtherQuat.w + w * pOtherQuat.z,
		-x * pOtherQuat.x - y * pOtherQuat.y - z * pOtherQuat.z + w * pOtherQuat.w
	);
}

FQuaternion& FQuaternion::operator*=(const FQuaternion& pOtherQuat)
{
	FQuaternion temp(
		x * pOtherQuat.w + y * pOtherQuat.z - z * pOtherQuat.x +
		w * pOtherQuat.x,
		-x * pOtherQuat.z + y * pOtherQuat.w + z * pOtherQuat.x +
		w * pOtherQuat.x,
		x * pOtherQuat.x - y * pOtherQuat.x + z * pOtherQuat.w -
		w * pOtherQuat.z,
		-x * pOtherQuat.x - y * pOtherQuat.x - z * pOtherQuat.z +
		w * pOtherQuat.w);

	x = temp.x;
	y = temp.y;
	z = temp.z;
	w = temp.w;

	return *this;
}

FVector3 FQuaternion::operator*(const FVector3& pToMultiply) const
{
	const float num = x * 2.0f;
	const float num2 = y * 2.0f;
	const float num3 = z * 2.0f;
	const float num4 = x * num;
	const float num5 = y * num2;
	const float num6 = z * num3;
	const float num7 = x * num2;
	const float num8 = x * num3;
	const float num9 = y * num3;
	const float num10 = w * num;
	const float num11 = w * num2;
	const float num12 = w * num3;
	FVector3 result;
	result.x = (1.f - (num5 + num6)) * pToMultiply.x + (num7 - num12) * pToMultiply.y + (num8 + num11) *
		pToMultiply.z;
	result.y = (num7 + num12) * pToMultiply.x + (1.f - (num4 + num6)) * pToMultiply.y + (num9 - num10) *
		pToMultiply.z;
	result.z = (num8 - num11) * pToMultiply.x + (num9 + num10) * pToMultiply.y + (1.f - (num4 + num5)) *
		pToMultiply.z;
	return result;
}

FMatrix3 FQuaternion::operator*(const FMatrix3& pMultiply) const
{
	return (toMatrix3(*this) * pMultiply);
}

FQuaternion& FQuaternion::operator/=(const float pScale)
{
	const float reciprocate = 1.0f / pScale;
	x *= reciprocate;
	y *= reciprocate;
	z *= reciprocate;
	w *= reciprocate;

	return *this;
}

FQuaternion FQuaternion::operator/(const float pScale) const
{
	FQuaternion temp(*this);
	const float reciprocate = 1.0f / pScale;
	temp.x *= reciprocate;
	temp.y *= reciprocate;
	temp.z *= reciprocate;
	temp.w *= reciprocate;

	return temp;
}