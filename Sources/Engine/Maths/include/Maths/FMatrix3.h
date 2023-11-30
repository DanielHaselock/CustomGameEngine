#pragma once

#include <stdint.h>

#include "Maths/FVector3.h"
#include "Maths/FVector2.h"

namespace Maths
{
	struct FMatrix3
	{
		FVector3 data[3];
		static const FMatrix3 Identity;

		FMatrix3();

		FMatrix3(const float pValue);

		FMatrix3(float pElement1, float pElement2, float pElement3,
			float pElement4, float pElement5, float pElement6,
			float pElement7, float pElement8, float pElement9);

		FMatrix3(const FVector3& pRow1, const FVector3& pRow2, const FVector3& pRow3);

		FMatrix3(const FMatrix3& pOther);

		FMatrix3& operator=(const FMatrix3& pOther);

		bool operator==(const FMatrix3& pOther);

		FMatrix3 operator+(const FMatrix3& pOther) const;

		FMatrix3& operator+=(const FMatrix3& pOther);

		FMatrix3 operator-() const;

		FMatrix3 operator-(const FMatrix3& pOther) const;

		FMatrix3& operator-=(const FMatrix3& pOther);

		FMatrix3 operator*(float pScalar) const;

		FMatrix3& operator*=(float pScalar);

		FVector3 operator*(const FVector3& pVector) const;

		FMatrix3 operator*(const FMatrix3& pOther) const;

		FMatrix3& operator*=(const FMatrix3& pOther);

		FMatrix3 operator/(float pScalar) const;

		FMatrix3& operator/=(float pScalar);

		FMatrix3 operator/(const FMatrix3& pOther) const;

		FMatrix3& operator/=(const FMatrix3& pOther);

		float& operator()(uint8_t pRow, uint8_t pColumn);

		static bool areEquals(const FMatrix3& pLeft, const FMatrix3& pRight);

		bool areEquals(const FMatrix3& pRight);

		static FMatrix3 add(const FMatrix3& pLeft, float pScalar);

		static FMatrix3 add(const FMatrix3& pLeft, const FMatrix3& pRight);

		static FMatrix3 subtract(const FMatrix3& pLeft, float pScalar);

		static FMatrix3 subtract(const FMatrix3& pLeft, const FMatrix3& pRight);

		static FMatrix3 multiply(const FMatrix3& pLeft, float pScalar);

		static FVector3 multiply(const FMatrix3& pMatrix, const FVector3& pVector);

		static FMatrix3 multiply(const FMatrix3& pLeft, const FMatrix3& pRight);

		static FMatrix3 divide(const FMatrix3& pLeft, float pScalar);

		static FMatrix3 divide(const FMatrix3& pLeft, const FMatrix3& pRight);

		static bool isIdentity(const FMatrix3& pMatrix);

		bool isIdentity();

		static float determinant(const FMatrix3& pMatrix);

		float determinant();

		static FMatrix3 transpose(const FMatrix3& pMatrix);

		FMatrix3 transpose();

		static FMatrix3 cofactor(const FMatrix3& pMatrix);

		FMatrix3 cofactor();

		static FMatrix3 minor(const FMatrix3& pMatrix);

		FMatrix3 minor();

		static FMatrix3 adjoint(const FMatrix3& pOther);

		FMatrix3 adjoint();

		static FMatrix3 inverse(const FMatrix3& pMatrix);

		FMatrix3 inverse();

		static FMatrix3 negate(const FMatrix3& pMatrix);

		FMatrix3 negate();

		static FMatrix3 translation(const FVector2& pTranslation);

		static FMatrix3 translate(const FMatrix3& pMatrix, const FVector2& pTranslation);

		FMatrix3 translate(const FVector2& pTranslation);

		static FMatrix3 rotation(float pRotation);

		static FMatrix3 rotate(const FMatrix3& pMatrix, float pRotation);

		FMatrix3 rotate(float pRotation);

		static FMatrix3 scaling(const FVector2& pScale);

		static FMatrix3 scale(const FMatrix3& pMatrix, const FVector2& pScale);

		FMatrix3 scale(const FVector2& pScale);

		static FVector3 getRow(const FMatrix3& pMatrix, uint8_t pRow);

		FVector3 getRow(uint8_t pRow);

		static FVector3 getColumn(const FMatrix3& pMatrix, uint8_t pColumn);

		FVector3 getColumn(uint8_t pColumn);
	};

	bool operator==(const FMatrix3& pLeft, const FMatrix3& pRight);
	bool operator!=(const FMatrix3& pLeft, const FMatrix3& pRight);
}