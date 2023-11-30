#pragma once

#include "Maths/FVector3.h"
#include "Maths/FMatrix3.h"
#include "Maths/FMatrix4.h"

#include <utility>

namespace Maths
{
	struct FQuaternion
	{
		float x;
		float y;
		float z;
		float w;

		static const FQuaternion Identity;

		FQuaternion();

		FQuaternion(float pReal);

		FQuaternion(float pX, float pY, float pZ, float pW);

		FQuaternion(const FQuaternion& pOther);

		FQuaternion(const FMatrix3& pRotationMatrix);

		FQuaternion(const FMatrix4& pRotationMatrix);

		FQuaternion(const FVector3& pEuler);

		static FQuaternion lookAt(const FVector3& pForward, const FVector3& pUp);

		static bool isIdentity(const FQuaternion& pTarget);

		bool isIdentity();

		static bool isPure(const FQuaternion& pTarget);

		bool isPure();

		static bool isNormalized(const FQuaternion& pTarget);

		bool isNormalized();

		static float dotProduct(const FQuaternion& pLeft, const FQuaternion& pRight);

		float dotProduct(const FQuaternion& pRight);

		static FQuaternion normalize(const FQuaternion& pTarget);

		FQuaternion normalize();

		static float length(const FQuaternion& pTarget);

		float length();

		static float lengthSquared(const FQuaternion& pTarget);

		float lengthSquared();

		static float getAngle(const FQuaternion& pTarget);

		float getAngle();

		static FVector3 getRotationAxis(const FQuaternion& pTarget);

		FVector3 getRotationAxis();

		static FQuaternion inverse(const FQuaternion& pTarget);

		FQuaternion inverse();

		static FQuaternion conjugate(const FQuaternion& pTarget);

		FQuaternion conjugate();

		static FQuaternion square(const FQuaternion& pTarget);

		FQuaternion square();

		static std::pair<FVector3, float> getAxisAndAngle(const FQuaternion& pTarget);

		std::pair<FVector3, float> getAxisAndAngle();

		static float angularDistance(const FQuaternion& pLeft, const FQuaternion& pRight);

		float angularDistance(const FQuaternion& pRight);

		static FQuaternion lerp(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha);

		static FQuaternion slerp(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha);

		static FQuaternion nlerp(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha);

		static FQuaternion mix(const FQuaternion& pStart, const FQuaternion& pEnd, float pAlpha);

		static FVector3 rotatePoint(const FVector3& pPoint, const FQuaternion& pQuaternion);

		static FVector3 rotatePoint(const FVector3& pPoint, const FQuaternion& pQuaternion, const FVector3& pPivot);

		static FVector3 eulerAngles(FQuaternion& pTarget);

		FVector3 eulerAngles();

		static FMatrix3 toMatrix3(const FQuaternion& pTarget);

		FMatrix3 toMatrix3();

		static FMatrix4 toMatrix4(const FQuaternion& pTarget);

		FMatrix4 toMatrix4();



		static FQuaternion toQuaternion(const FMatrix4& pMatrix4);

		std::string toString();

		bool operator==(const FQuaternion& pOtherQuat) const;
		bool operator!=(const FQuaternion& pOtherQuat) const;
		FQuaternion operator+(const FQuaternion& pOtherQuat) const;
		FQuaternion& operator+=(const FQuaternion& pOtherQuat);
		FQuaternion operator-() const;
		FQuaternion operator-(const FQuaternion& pOtherQuat) const;
		FQuaternion& operator-=(const FQuaternion& pOtherQuat);
		float operator|(const FQuaternion& pOtherQuat) const;
		FQuaternion& operator*=(const float pScale);
		FQuaternion operator*(const float pScale) const;
		FQuaternion operator*(const FQuaternion& pOtherQuat) const;
		FQuaternion& operator*=(const FQuaternion& pOtherQuat);
		FVector3 operator*(const FVector3& pToMultiply) const;
		FMatrix3 operator*(const FMatrix3& pMultiply) const;
		FQuaternion& operator/=(const float pScale);
		FQuaternion operator/(const float pScale) const;
	};
}