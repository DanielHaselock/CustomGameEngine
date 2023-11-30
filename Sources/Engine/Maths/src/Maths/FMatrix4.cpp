#include <cstring>
#include <stdexcept>
#include <string>
#include <cmath>

#include "Maths/Utils.h"
#include "Maths/FMatrix4.h"
#include "Maths/FVector3.h"
#include "Maths/FQuaternion.h"

using namespace Maths;

const FMatrix4 FMatrix4::Identity = FMatrix4(1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f);

FMatrix4::FMatrix4()
{
	for (FVector4& element : data)
	{
		element = FVector4(0.f);
	}
}

FMatrix4::FMatrix4(const float pValue)
{
	data[0] = FVector4(pValue, 0, 0, 0);
	data[1] = FVector4(0, pValue, 0, 0);
	data[2] = FVector4(0, 0, pValue, 0);
	data[3] = FVector4(0, 0, 0, pValue);
}

FMatrix4::FMatrix4(float pelement1, float pelement2, float pelement3, float pelement4, float pelement5, float pelement6, float pelement7, float pelement8, float pelement9, float pelement10, float pelement11, float pelement12, float pelement13, float pelement14, float pelement15, float pelement16)
{
	data[0] = FVector4(pelement1, pelement2, pelement3, pelement4);
	data[1] = FVector4(pelement5, pelement6, pelement7, pelement8);
	data[2] = FVector4(pelement9, pelement10, pelement11, pelement12);
	data[3] = FVector4(pelement13, pelement14, pelement15, pelement16);
}

FMatrix4::FMatrix4(const FVector4& pElement1, const FVector4& pElement2, const FVector4& pElement3, const FVector4& pElement4)
{
	data[0] = pElement1;
	data[1] = pElement2;
	data[2] = pElement3;
	data[3] = pElement4;
}

FMatrix4::FMatrix4(const FMatrix4& pother)
{
	*this = pother;
}

FMatrix4& FMatrix4::operator=(const FMatrix4& pother)
{
	memcpy(this->data, pother.data, 4 * sizeof(FVector4));
	return *this;
}

bool FMatrix4::operator==(const FMatrix4& pother)
{
	return areEquals(*this, pother);
}

bool FMatrix4::operator!=(const FMatrix4& pother)
{
	return !areEquals(*this, pother);
}

FMatrix4 FMatrix4::operator+(const FMatrix4& pother) const
{
	return add(*this, pother);
}

FMatrix4& FMatrix4::operator+=(const FMatrix4& pother)
{
	*this = add(*this, pother);
	return *this;
}

FMatrix4 FMatrix4::operator-() const
{
	return negate(*this);
}

FMatrix4 FMatrix4::operator-(float pscalar) const
{
	return subtract(*this, pscalar);
}

FMatrix4 FMatrix4::operator-(const FMatrix4& pother) const
{
	return subtract(*this, pother);
}

FMatrix4& FMatrix4::operator-=(const FMatrix4& pother)
{
	*this = subtract(*this, pother);
	return *this;
}

FMatrix4 FMatrix4::operator*(float pscalar) const
{
	return multiply(*this, pscalar);
}

FMatrix4& FMatrix4::operator*=(float pscalar)
{
	*this = multiply(*this, pscalar);
	return *this;
}

FVector4 FMatrix4::operator*(const FVector4& pvector) const
{
	return multiply(*this, pvector);
}

FVector3 FMatrix4::operator*(const FVector3& pvector) const
{
	return multiply(*this, pvector);
}

FMatrix4 FMatrix4::operator*(const FMatrix4& pother) const
{
	return multiply(*this, pother);
}

FMatrix4& FMatrix4::operator*=(const FMatrix4& pother)
{
	*this = multiply(*this, pother);
	return *this;
}

FMatrix4 FMatrix4::operator/(float pscalar) const
{
	return divide(*this, pscalar);
}

FMatrix4& FMatrix4::operator/=(float pscalar)
{
	*this = divide(*this, pscalar);
	return *this;
}

FMatrix4 FMatrix4::operator/(const FMatrix4& pother) const
{
	return divide(*this, pother);
}

FMatrix4& FMatrix4::operator/=(const FMatrix4& pother)
{
	*this = divide(*this, pother);
	return *this;
}

float& FMatrix4::operator()(uint8_t prow, uint8_t pcolumn)
{
	if (prow >= 4 || pcolumn >= 4)
		throw std::out_of_range("Invalid index : " + std::to_string(prow) + "," + std::to_string(pcolumn) + " is out of range");
	return data[prow][pcolumn];
}

bool FMatrix4::areEquals(const FMatrix4& pleft, const FMatrix4& pright)
{
	return memcmp(&pleft, &pright, 4 * sizeof(FVector4)) == 0;
}

bool Maths::FMatrix4::areEquals(const FMatrix4& pright)
{
	return areEquals(*this, pright);
}

FMatrix4 FMatrix4::add(const FMatrix4& pleft, float pscalar)
{
	FMatrix4 result(pleft);
	for (int8_t i = 0; i < 4; i++)
		result.data[i] += pscalar;
	return result;
}

FMatrix4 FMatrix4::add(const FMatrix4& pleft, const FMatrix4& pright)
{
	FMatrix4 result(pleft);
	for (int8_t i = 0; i < 4; i++)
		result.data[i] += pright.data[i];
	return result;
}

FMatrix4 FMatrix4::subtract(const FMatrix4& pleft, float pscalar)
{
	FMatrix4 result(pleft);
	for (int8_t i = 0; i < 4; ++i)
		result.data[i] -= pscalar;
	return result;
}

FMatrix4 FMatrix4::subtract(const FMatrix4& pleft, const FMatrix4& pright)
{
	FMatrix4 result(pleft);
	for (int8_t i = 0; i < 4; ++i)
		result.data[i] -= pright.data[i];
	return result;
}

FMatrix4 FMatrix4::multiply(const FMatrix4& pleft, float pscalar)
{
	FMatrix4 result(pleft);
	for (int8_t i = 0; i < 4; ++i)
		result.data[i] *= pscalar;
	return result;
}

FVector4 FMatrix4::multiply(const FMatrix4& pMatrix, const FVector4& pVector)
{
	FVector4 result;
	result.x = (pMatrix.data[0].x * pVector.x) + (pMatrix.data[1].x * pVector.y) + (pMatrix.data[2].x * pVector.z) + (pMatrix.data[3].x * pVector.w);
	result.y = (pMatrix.data[0].y * pVector.x) + (pMatrix.data[1].y * pVector.y) + (pMatrix.data[2].y * pVector.z) + (pMatrix.data[3].y * pVector.w);
	result.z = (pMatrix.data[0].z * pVector.x) + (pMatrix.data[1].z * pVector.y) + (pMatrix.data[2].z * pVector.z) + (pMatrix.data[3].z * pVector.w);
	result.w = (pMatrix.data[0].w * pVector.x) + (pMatrix.data[1].w * pVector.y) + (pMatrix.data[2].w * pVector.z) + (pMatrix.data[3].w * pVector.w);
	return result;
}

FVector3 FMatrix4::multiply(const FMatrix4& pMatrix, const FVector3& pVector)
{
	FVector3 result;
	result.x = (pMatrix.data[0].x * pVector.x) + (pMatrix.data[1].x * pVector.y) + (pMatrix.data[2].x * pVector.z) + (pMatrix.data[3].x * 0);
	result.y = (pMatrix.data[0].y * pVector.x) + (pMatrix.data[1].y * pVector.y) + (pMatrix.data[2].y * pVector.z) + (pMatrix.data[3].y * 0);
	result.z = (pMatrix.data[0].z * pVector.x) + (pMatrix.data[1].z * pVector.y) + (pMatrix.data[2].z * pVector.z) + (pMatrix.data[3].z * 0);
	return result;
}

FMatrix4 FMatrix4::multiply(const FMatrix4& pleft, const FMatrix4& pright)
{
	FMatrix4 newMat4;

	for (unsigned int i = 0; i < 4; i++)
	{
		FVector4 vec4;
		for (unsigned int j = 0; j < 4; j++)
		{
			vec4[j] = pleft.data[0][j] * pright.data[i].x
				+ pleft.data[1][j] * pright.data[i].y
				+ pleft.data[2][j] * pright.data[i].z
				+ pleft.data[3][j] * pright.data[i].w;
		}
		newMat4.data[i] = vec4;
	}

	return newMat4;
}

FMatrix4 FMatrix4::divide(const FMatrix4& pleft, float pscalar)
{
	FMatrix4 result(pleft);
	for (FVector4& element : result.data)
		element /= pscalar;

	return result;
}

FMatrix4 FMatrix4::divide(const FMatrix4& pleft, const FMatrix4& pright)
{
	FMatrix4 leftCopy(pleft);
	return leftCopy * inverse(pright);
}

bool FMatrix4::isIdentity(const FMatrix4& pmatrix)
{
	return memcmp(Identity.data, pmatrix.data, 4 * sizeof(FVector4)) == 0;
}

bool Maths::FMatrix4::isIdentity()
{
	return isIdentity(*this);
}

float FMatrix4::getMinor(float pminor0, float pminor1, float pminor2, float pminor3, float pminor4, float pminor5, float pminor6, float pminor7, float pminor8)
{
	/*return pminor0 * (pminor4 * pminor8 - pminor5 * pminor7)
		   - pminor1 * (pminor3 * pminor8 - pminor5 * pminor6)
		   + pminor2 * (pminor3 * pminor7 - pminor4 * pminor6);*/

	return -1;
}

float FMatrix4::determinant(const FMatrix4& pmatrix)
{
	/*return pmatrix.data[0] * getMinor(pmatrix.data[5], pmatrix.data[9], pmatrix.data[13], pmatrix.data[6], pmatrix.data[10], pmatrix.data[14],
		pmatrix.data[7], pmatrix.data[11], pmatrix.data[15])
		- pmatrix.data[4] * getMinor(pmatrix.data[1], pmatrix.data[9], pmatrix.data[13], pmatrix.data[2], pmatrix.data[10], pmatrix.data[14],
			pmatrix.data[3], pmatrix.data[11], pmatrix.data[15])
		+ pmatrix.data[8] * getMinor(pmatrix.data[1], pmatrix.data[5], pmatrix.data[13], pmatrix.data[2], pmatrix.data[6], pmatrix.data[14],
			pmatrix.data[3], pmatrix.data[7], pmatrix.data[15])
		- pmatrix.data[12] * getMinor(pmatrix.data[1], pmatrix.data[5], pmatrix.data[9], pmatrix.data[2], pmatrix.data[6], pmatrix.data[10],
			pmatrix.data[3], pmatrix.data[7], pmatrix.data[11]);*/

	return 0;
}

float Maths::FMatrix4::determinant()
{
	return determinant(*this);
}

FMatrix4 FMatrix4::transpose(const FMatrix4& pmatrix)
{
	FMatrix4 transposedMatrix(pmatrix);

	std::swap(transposedMatrix.data[0].y, transposedMatrix.data[1].x);
	std::swap(transposedMatrix.data[0].z, transposedMatrix.data[2].x);
	std::swap(transposedMatrix.data[0].w, transposedMatrix.data[3].x);

	std::swap(transposedMatrix.data[1].z, transposedMatrix.data[2].y);
	std::swap(transposedMatrix.data[1].w, transposedMatrix.data[3].y);

	std::swap(transposedMatrix.data[2].w, transposedMatrix.data[3].z);
	return transposedMatrix;
}

FMatrix4 Maths::FMatrix4::transpose()
{
	return transpose(*this);
}

FMatrix4 FMatrix4::inverse(const FMatrix4& pmatrix)
{
	/*const float aDeterminant = determinant(pmatrix);
	if (aDeterminant == 0)
		throw std::logic_error("Division by 0");

	const float cof0 = getMinor(pmatrix.data[5], pmatrix.data[9], pmatrix.data[13], pmatrix.data[6], pmatrix.data[10], pmatrix.data[14],
		pmatrix.data[7], pmatrix.data[11], pmatrix.data[15]);
	const float cof1 = getMinor(pmatrix.data[1], pmatrix.data[9], pmatrix.data[13], pmatrix.data[2], pmatrix.data[10], pmatrix.data[14],
		pmatrix.data[3], pmatrix.data[11], pmatrix.data[15]);
	const float cof2 = getMinor(pmatrix.data[1], pmatrix.data[5], pmatrix.data[13], pmatrix.data[2], pmatrix.data[6], pmatrix.data[14],
		pmatrix.data[3], pmatrix.data[7], pmatrix.data[15]);
	const float cof3 = getMinor(pmatrix.data[1], pmatrix.data[5], pmatrix.data[9], pmatrix.data[2], pmatrix.data[6], pmatrix.data[10],
		pmatrix.data[3], pmatrix.data[7], pmatrix.data[11]);

	const float det = pmatrix.data[0] * cof0 - pmatrix.data[4] * cof1 + pmatrix.data[8] * cof2 - pmatrix.data[12] * cof3;
	if (fabs(det) <= EPSILON)
		return Identity;
	const float cof4 = getMinor(pmatrix.data[4], pmatrix.data[8], pmatrix.data[12], pmatrix.data[6], pmatrix.data[10], pmatrix.data[14],
		pmatrix.data[7], pmatrix.data[11], pmatrix.data[15]);
	const float cof5 = getMinor(pmatrix.data[0], pmatrix.data[8], pmatrix.data[12], pmatrix.data[2], pmatrix.data[10], pmatrix.data[14],
		pmatrix.data[3], pmatrix.data[11], pmatrix.data[15]);
	const float cof6 = getMinor(pmatrix.data[0], pmatrix.data[4], pmatrix.data[12], pmatrix.data[2], pmatrix.data[6], pmatrix.data[14],
		pmatrix.data[3], pmatrix.data[7], pmatrix.data[15]);
	const float cof7 = getMinor(pmatrix.data[0], pmatrix.data[4], pmatrix.data[8], pmatrix.data[2], pmatrix.data[6], pmatrix.data[10],
		pmatrix.data[3], pmatrix.data[7], pmatrix.data[11]);

	const float cof8 = getMinor(pmatrix.data[4], pmatrix.data[8], pmatrix.data[12], pmatrix.data[5], pmatrix.data[9], pmatrix.data[13],
		pmatrix.data[7], pmatrix.data[11], pmatrix.data[15]);
	const float cof9 = getMinor(pmatrix.data[0], pmatrix.data[8], pmatrix.data[12], pmatrix.data[1], pmatrix.data[9], pmatrix.data[13],
		pmatrix.data[3], pmatrix.data[11], pmatrix.data[15]);
	const float cof10 = getMinor(pmatrix.data[0], pmatrix.data[4], pmatrix.data[12], pmatrix.data[1], pmatrix.data[5], pmatrix.data[13],
		pmatrix.data[3], pmatrix.data[7], pmatrix.data[15]);
	const float cof11 = getMinor(pmatrix.data[0], pmatrix.data[4], pmatrix.data[8], pmatrix.data[1], pmatrix.data[5], pmatrix.data[9],
		pmatrix.data[3], pmatrix.data[7], pmatrix.data[11]);

	const float cof12 = getMinor(pmatrix.data[4], pmatrix.data[8], pmatrix.data[12], pmatrix.data[5], pmatrix.data[9], pmatrix.data[13],
		pmatrix.data[6], pmatrix.data[10], pmatrix.data[14]);
	const float cof13 = getMinor(pmatrix.data[0], pmatrix.data[8], pmatrix.data[12], pmatrix.data[1], pmatrix.data[9], pmatrix.data[13],
		pmatrix.data[2], pmatrix.data[10], pmatrix.data[14]);
	const float cof14 = getMinor(pmatrix.data[0], pmatrix.data[4], pmatrix.data[12], pmatrix.data[1], pmatrix.data[5], pmatrix.data[13],
		pmatrix.data[2], pmatrix.data[6], pmatrix.data[14]);
	const float cof15 = getMinor(pmatrix.data[0], pmatrix.data[4], pmatrix.data[8], pmatrix.data[1], pmatrix.data[5], pmatrix.data[9],
		pmatrix.data[2], pmatrix.data[6], pmatrix.data[10]);

	const float detInv = 1.0f / det;
	FMatrix4 inverse;

	inverse.data[0] = detInv * cof0;
	inverse.data[4] = -detInv * cof4;
	inverse.data[8] = detInv * cof8;
	inverse.data[12] = -detInv * cof12;
	inverse.data[1] = -detInv * cof1;
	inverse.data[5] = detInv * cof5;
	inverse.data[9] = -detInv * cof9;
	inverse.data[13] = detInv * cof13;
	inverse.data[2] = detInv * cof2;
	inverse.data[6] = -detInv * cof6;
	inverse.data[10] = detInv * cof10;
	inverse.data[14] = -detInv * cof14;
	inverse.data[3] = -detInv * cof3;
	inverse.data[7] = detInv * cof7;
	inverse.data[11] = -detInv * cof11;
	inverse.data[15] = detInv * cof15;

	return inverse;*/

	const float m[16] = { pmatrix.data[0][0], pmatrix.data[0][1], pmatrix.data[0][2], pmatrix.data[0][3],
									pmatrix.data[1][0], pmatrix.data[1][1], pmatrix.data[1][2], pmatrix.data[1][3],
									pmatrix.data[2][0], pmatrix.data[2][1], pmatrix.data[2][2], pmatrix.data[2][3],
									pmatrix.data[3][0], pmatrix.data[3][1], pmatrix.data[3][2], pmatrix.data[3][3] };

	float inv[16];

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return Identity;

	det = 1.0 / det;

	FMatrix4 inverted;
	inverted.data[0][0] = inv[0] * det;
	inverted.data[0][1] = inv[1] * det;
	inverted.data[0][2] = inv[2] * det;
	inverted.data[0][3] = inv[3] * det;

	inverted.data[1][0] = inv[4] * det;
	inverted.data[1][1] = inv[5] * det;
	inverted.data[1][2] = inv[6] * det;
	inverted.data[1][3] = inv[7] * det;

	inverted.data[2][0] = inv[8] * det;
	inverted.data[2][1] = inv[9] * det;
	inverted.data[2][2] = inv[10] * det;
	inverted.data[2][3] = inv[11] * det;

	inverted.data[3][0] = inv[12] * det;
	inverted.data[3][1] = inv[13] * det;
	inverted.data[3][2] = inv[14] * det;
	inverted.data[3][3] = inv[15] * det;

	return inverted;
}

FMatrix4 Maths::FMatrix4::inverse()
{
	return inverse(*this);
}

FMatrix4 FMatrix4::negate(const FMatrix4& pMatrix)
{
	FMatrix4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.data[i][j] = -pMatrix.data[i][j];
		}
	}
	return result;
}

FMatrix4 FMatrix4::negate()
{
	return negate(*this);
}

FMatrix4 FMatrix4::createTransformMatrix(const FVector3& position, const FVector3& rotation, const FVector3& scaleVec)
{
	return translation(position) * rotationOnAxisY(rotation.y) * rotationOnAxisX(rotation.x) * rotationOnAxisZ(rotation.z) * scaling(scaleVec);
}

FMatrix4 FMatrix4::createTransformMatrixQ(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale)
{
	return FMatrix4::translation(p_position) * FQuaternion::toMatrix4(FQuaternion::normalize(p_rotation)) * FMatrix4::scaling(p_scale);
}

FMatrix4 FMatrix4::translation(const FVector3& ptranslation)
{
	return FMatrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		ptranslation.x, ptranslation.y, ptranslation.z, 1);
}

FMatrix4 FMatrix4::translate(const FMatrix4& pmatrix, const FVector3& ptranslation)
{
	return pmatrix * translation(ptranslation);
}

FMatrix4 Maths::FMatrix4::translate(const FVector3& pTranslation)
{
	return translate(*this, pTranslation);
}

FMatrix4 FMatrix4::rotationOnAxisX(float protation)
{
	protation = degreesToRadians(protation);

	return FMatrix4(1, 0, 0, 0,
		0, std::cosf(protation), std::sinf(protation), 0,
		0, -std::sinf(protation), std::cosf(protation), 0,
		0, 0, 0, 1);
}

FMatrix4 FMatrix4::rotateOnAxisX(const FMatrix4& pmatrix, float protation)
{
	return pmatrix * rotationOnAxisX(protation);
}

FMatrix4 Maths::FMatrix4::rotateOnAxisX(float protation)
{
	return rotateOnAxisX(*this, protation);
}

FMatrix4 FMatrix4::rotationOnAxisY(float protation)
{
	protation = degreesToRadians(protation);

	return FMatrix4(std::cosf(protation), 0, -std::sinf(protation), 0,
		0, 1, 0, 0,
		std::sinf(protation), 0, std::cosf(protation), 0,
		0, 0, 0, 1);
}

FMatrix4 FMatrix4::rotateOnAxisY(const FMatrix4& pmatrix, float protation)
{
	return pmatrix * rotationOnAxisY(protation);
}

FMatrix4 Maths::FMatrix4::rotateOnAxisY(float protation)
{
	return rotateOnAxisY(*this, protation);
}

FMatrix4 FMatrix4::rotationOnAxisZ(float protation)
{
	protation = degreesToRadians(protation);

	return FMatrix4(std::cosf(protation), std::sinf(protation), 0, 0,
		-std::sinf(protation), std::cosf(protation), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

FMatrix4 FMatrix4::rotateOnAxisZ(const FMatrix4& pmatrix, float protation)
{
	return pmatrix * rotationOnAxisZ(protation);
}

FMatrix4 Maths::FMatrix4::rotateOnAxisZ(float protation)
{
	return rotateOnAxisZ(*this, protation);
}

FMatrix4 FMatrix4::rotationYXZ(float protation)
{
	FMatrix4 Xrot = rotationOnAxisX(protation);
	FMatrix4 Yrot = rotationOnAxisY(protation);
	FMatrix4 Zrot = rotationOnAxisZ(protation);

	return Yrot * Xrot * Zrot;
}

FMatrix4 FMatrix4::rotateYXZ(const FMatrix4& pmatrix, float protation)
{
	return pmatrix * rotationYXZ(protation);
}

FMatrix4 Maths::FMatrix4::rotateYXZ(float protation)
{
	return rotateYXZ(*this, protation);
}

FMatrix4 FMatrix4::scaling(const FVector3& pscale)
{
	return FMatrix4(pscale.x, 0, 0, 0,
		0, pscale.y, 0, 0,
		0, 0, pscale.z, 0,
		0, 0, 0, 1);
}

FMatrix4 FMatrix4::scale(const FMatrix4& pmatrix, const FVector3& pscale)
{
	return pmatrix * scaling(pscale);
}

FMatrix4 Maths::FMatrix4::scale(const FVector3& pscale)
{
	return scale(*this, pscale);
}

FMatrix4 FMatrix4::rotation(const FQuaternion& pquaternion)
{
	return FQuaternion::toMatrix4(pquaternion);
}

FMatrix4 FMatrix4::rotate(const FMatrix4& pmatrix, const FQuaternion& pquaternion)
{
	return pmatrix * rotation(pquaternion);
}

FMatrix4 Maths::FMatrix4::rotate(const FQuaternion& pquaternion)
{
	return rotate(*this, pquaternion);
}

FMatrix4 FMatrix4::createPerspective(const float pfov, const float paspectRatio, const float pzNear, const float pzFar)
{
	const float tangent = tanf(pfov / 2.0f * PI / 180.0f);
	const float height = pzNear * tangent;
	const float width = height * paspectRatio;

	return createFrustum(-width, width, -height, height, pzNear, pzFar);
}

FMatrix4 FMatrix4::createOrthographic(const float pleft, const float pright, const float pbottom, const float ptop, const float pzNear, const float pzFar)
{
	FMatrix4 ortho = FMatrix4::Identity;

	ortho.data[0][0] = 2.0f / (pright - pleft);
	ortho.data[1][1] = 2.0f / (ptop - pbottom);
	ortho.data[2][2] = -2.0f / (pzFar - pzNear);
	ortho.data[3][0] = -(pright + pleft) / (pright - pleft);
	ortho.data[3][1] = -(ptop + pbottom) / (ptop - pbottom);
	ortho.data[3][2] = -(pzFar + pzNear) / (pzFar - pzNear);
	ortho.data[3][3] = 1.0f;

	return ortho;
}

FMatrix4 Maths::FMatrix4::lookAt(const float peyeX, const float peyeY, const float peyeZ, const float plookX, const float plookY, const float plookZ, const float pupX, const float pupY, const float pupZ)
{
	return lookAt(FVector3(peyeX, peyeY, peyeZ), FVector3(plookX, plookY, plookZ), FVector3(pupX, pupY, pupZ));
}

FMatrix4 FMatrix4::lookAt(const FVector3& pEye, const FVector3& pLook, const FVector3& pUp)
{
	FVector3  f = FVector3::normalize(pLook - pEye);
	FVector3  u = FVector3::normalize(pUp);
	FVector3  r = FVector3::normalize(FVector3::cross(f, u));
	u = FVector3::cross(f, r);

	FMatrix4 Result = FMatrix4::Identity;
	Result.data[0][0] = r.x;
	Result.data[1][0] = r.y;
	Result.data[2][0] = r.z;

	Result.data[0][1] = u.x;
	Result.data[1][1] = u.y;
	Result.data[2][1] = u.z;

	Result.data[0][2] = -f.x;
	Result.data[1][2] = -f.y;
	Result.data[2][2] = -f.z;

	Result.data[3][0] = -FVector3::dot(r, pEye);
	Result.data[3][1] = -FVector3::dot(u, pEye);
	Result.data[3][2] = FVector3::dot(f, pEye);
	return Result;
}

FMatrix4 FMatrix4::createFrustum(const float pleft, const float pright, const float pbottom, const float ptop, const float pzNear, const float pzFar)
{
	const float maxView = 2.0f * pzNear;
	const float width = pright - pleft;
	const float height = ptop - pbottom;
	const float zRange = pzFar - pzNear;
	FMatrix4 Frustum;

	Frustum.data[0][0] = maxView / width;
	Frustum.data[1][1] = maxView / height;
	Frustum.data[2][0] = (pright + pleft) / width;
	Frustum.data[2][1] = (ptop + pbottom) / height;
	Frustum.data[2][2] = (-pzFar - pzNear) / zRange;
	Frustum.data[2][3] = -1.0f;
	Frustum.data[3][2] = (-maxView * pzFar) / zRange;
	Frustum.data[3][3] = 0.0f;

	return Frustum;
}

FVector4 FMatrix4::getRow(const FMatrix4& pmatrix, uint8_t prow)
{
	if (prow >= 4)
		throw std::out_of_range("Invalid index : " + std::to_string(prow) + " is out of range");

	return FVector4(pmatrix.data[prow].x, pmatrix.data[prow].y, pmatrix.data[prow].z, pmatrix.data[prow].w);
}

FVector4 Maths::FMatrix4::getRow(uint8_t prow)
{
	return getRow(*this, prow);
}

FVector4 FMatrix4::getColumn(const FMatrix4& pmatrix, uint8_t pcolumn)
{
	if (pcolumn >= 4)
		throw std::out_of_range("Invalid index : " + std::to_string(pcolumn) + " is out of range");

	return FVector4(pmatrix.data[0][pcolumn], pmatrix.data[1][pcolumn], pmatrix.data[2][pcolumn], pmatrix.data[3][pcolumn]);
}

FVector4 Maths::FMatrix4::getColumn(uint8_t pcolumn)
{
	return getColumn(*this, pcolumn);
}

bool Maths::operator==(const FMatrix4& pLeft, const FMatrix4& pRight)
{
	return Maths::FMatrix4::areEquals(pLeft, pRight);
}

bool Maths::operator!=(const FMatrix4& pLeft, const FMatrix4& pRight)
{
	return !(pLeft == pRight);
}