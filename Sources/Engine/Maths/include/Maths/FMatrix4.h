#pragma once

#include <stdint.h>

#include "Maths/FVector3.h"
#include "Maths/FVector4.h"

namespace Maths
{
	struct FQuaternion;

	struct FMatrix4
	{
		FVector4 data[4];
		static const FMatrix4 Identity;

		FMatrix4();

		FMatrix4(const float pValue);

		FMatrix4(float pElement1, float pElement2, float pElement3,
			float pElement4, float pElement5, float pElement6,
			float pElement7, float pelement8, float pelement9,
			float pelement10, float pelement11, float pelement12,
			float pelement13, float pelement14, float pelement15, float pelement16);

		FMatrix4(const FVector4& pElement1, const FVector4& pElement2, const FVector4& pElement3, const FVector4& pElement4);

		FMatrix4(const FMatrix4& pother);

		FMatrix4& operator=(const FMatrix4& pother);

		bool operator==(const FMatrix4& pother);

		bool operator!=(const FMatrix4& pother);

		FMatrix4 operator+(const FMatrix4& pother) const;

		FMatrix4& operator+=(const FMatrix4& pother);

		FMatrix4 operator-() const;

		FMatrix4 operator-(float pscalar) const;

		FMatrix4 operator-(const FMatrix4& pother) const;

		FMatrix4& operator-=(const FMatrix4& pother);

		FMatrix4 operator*(float pscalar) const;

		FMatrix4& operator*=(float pscalar);

		FVector4 operator*(const FVector4& pvector) const;

		FVector3 operator*(const FVector3& pvector) const;

		FMatrix4 operator*(const FMatrix4& pother) const;

		FMatrix4& operator*=(const FMatrix4& pother);

		FMatrix4 operator/(float pscalar) const;

		FMatrix4& operator/=(float pscalar);

		FMatrix4 operator/(const FMatrix4& pother) const;

		FMatrix4& operator/=(const FMatrix4& pother);

		float& operator()(uint8_t prow, uint8_t pcolumn);

		static bool areEquals(const FMatrix4& pleft, const FMatrix4& pright);

		bool areEquals(const FMatrix4& pright);

		static FMatrix4 add(const FMatrix4& pleft, float pscalar);

		static FMatrix4 add(const FMatrix4& pleft, const FMatrix4& pright);

		static FMatrix4 subtract(const FMatrix4& pleft, float pscalar);

		static FMatrix4 subtract(const FMatrix4& pleft, const FMatrix4& pright);

		static FMatrix4 multiply(const FMatrix4& pleft, float pscalar);

		static FVector4 multiply(const FMatrix4& pmatrix, const FVector4& pVector);

		static FVector3 multiply(const FMatrix4& pmatrix, const FVector3& pVector);

		static FMatrix4 multiply(const FMatrix4& pleft, const FMatrix4& pRight);

		static FMatrix4 divide(const FMatrix4& pleft, float pscalar);

		static FMatrix4 divide(const FMatrix4& pleft, const FMatrix4& pright);

		static bool isIdentity(const FMatrix4& pmatrix);

		bool isIdentity();

		static float getMinor(float pminor0, float pminor1, float pminor2,
			float pminor3, float pminor4, float pminor5,
			float pminor6, float pminor7, float pminor8);

		static float determinant(const FMatrix4& pmatrix);

		float determinant();

		static FMatrix4 transpose(const FMatrix4& pmatrix);

		FMatrix4 transpose();

		static FMatrix4 inverse(const FMatrix4& pmatrix);

		FMatrix4 inverse();

		static FMatrix4 negate(const FMatrix4& pmatrix);

		FMatrix4 negate();

		static FMatrix4 createTransformMatrix(const FVector3& position, const FVector3& rotation, const FVector3& scaleVec);

		static FMatrix4 createTransformMatrixQ(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale);

		static FMatrix4 translation(const FVector3& ptranslation);

		static FMatrix4 translate(const FMatrix4& pmatrix, const FVector3& ptranslation);

		FMatrix4 translate(const FVector3& pTranslation);

		static FMatrix4 rotationOnAxisX(float protation);

		static FMatrix4 rotateOnAxisX(const FMatrix4& pmatrix, float protation);

		FMatrix4 rotateOnAxisX(float protation);

		static FMatrix4 rotationOnAxisY(float protation);

		static FMatrix4 rotateOnAxisY(const FMatrix4& pmatrix, float protation);

		FMatrix4 rotateOnAxisY(float protation);

		static FMatrix4 rotationOnAxisZ(float protation);

		static FMatrix4 rotateOnAxisZ(const FMatrix4& pmatrix, float protation);

		FMatrix4 rotateOnAxisZ(float protation);

		static FMatrix4 rotationYXZ(float protation);

		static FMatrix4 rotateYXZ(const FMatrix4& pmatrix, float protation);

		FMatrix4 rotateYXZ(float protation);

		static FMatrix4 scaling(const FVector3& pscale);

		static FMatrix4 scale(const FMatrix4& pmatrix, const FVector3& pscale);

		FMatrix4 scale(const FVector3& pscale);

		static FMatrix4 rotation(const FQuaternion& pquaternion);

		static FMatrix4 rotate(const FMatrix4& pmatrix, const FQuaternion& pquaternion);

		FMatrix4 rotate(const FQuaternion& pquaternion);

		static FMatrix4 createPerspective(const float pfov, const float paspectRatio, const float pzNear, const float pzFar);

		static FMatrix4 createOrthographic(const float pleft, const float pright, const float pbottom, const float ptop, const float pzNear, const float pzFar);

		static FMatrix4 lookAt(const float peyeX, const float peyeY, const float peyeZ, const float plookX, const float plookY, const float plookZ, const float pupX, const float pupY, const float pupZ);

		static FMatrix4 lookAt(const FVector3& pEye, const FVector3& pLook, const FVector3& pUp);

		static FMatrix4 createFrustum(const float pleft, const float pright, const float pbottom, const float ptop, const float pzNear, const float pzFar);

		static FVector4 getRow(const FMatrix4& pmatrix, uint8_t prow);

		FVector4 getRow(uint8_t prow);

		static FVector4 getColumn(const FMatrix4& pmatrix, uint8_t pcolumn);

		FVector4 getColumn(uint8_t pcolumn);
	};

	bool operator==(const FMatrix4& pleft, const FMatrix4& pRight);
	bool operator!=(const FMatrix4& pLeft, const FMatrix4& pRight);
}