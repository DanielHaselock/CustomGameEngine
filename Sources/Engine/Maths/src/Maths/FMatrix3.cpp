#include <string>
#include <stdexcept>
#include <cmath>

#include "Maths/Utils.h"
#include "Maths/FMatrix3.h"

using namespace Maths;

const FMatrix3 FMatrix3::Identity = FMatrix3(1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f);

FMatrix3::FMatrix3()
{
	for (FVector3& element : data)
		element = FVector3::Zero;
}

FMatrix3::FMatrix3(float pValue)
{
	data[0] = FVector3(pValue, 0.0f, 0.0f);
	data[1] = FVector3(0.0f, pValue, 0.0f);
	data[2] = FVector3(0.0f, 0.0f, pValue);
}

FMatrix3::FMatrix3(float pElement1, float pElement2, float pElement3, float pElement4, float pElement5, float pElement6, float pElement7, float pElement8, float pElement9)
{
	data[0] = FVector3(pElement1, pElement2, pElement3);
	data[1] = FVector3(pElement4, pElement5, pElement6);
	data[2] = FVector3(pElement7, pElement8, pElement9);
}

FMatrix3::FMatrix3(const FVector3& pRow1, const FVector3& pRow2, const FVector3& pRow3)
{
	data[0] = pRow1;
	data[1] = pRow2;
	data[2] = pRow3;
}

FMatrix3::FMatrix3(const FMatrix3& pOther)
{
	*this = pOther;
}

FMatrix3& FMatrix3::operator=(const FMatrix3& pOther)
{
	memcpy(this->data, pOther.data, 3 * sizeof(FVector3));
	return *this;
}

bool FMatrix3::operator==(const FMatrix3& pOther)
{
	return areEquals(*this, pOther);
}

FMatrix3 FMatrix3::operator+(const FMatrix3& pOther) const
{
	return add(*this, pOther);
}

FMatrix3& FMatrix3::operator+=(const FMatrix3& pOther)
{
	*this = add(*this, pOther);
	return *this;
}

FMatrix3 FMatrix3::operator-() const
{
	return negate(*this);
}

FMatrix3 FMatrix3::operator-(const FMatrix3& pOther) const
{
	return subtract(*this, pOther);
}

FMatrix3& FMatrix3::operator-=(const FMatrix3& pOther)
{
	*this = subtract(*this, pOther);
	return *this;
}

FMatrix3 FMatrix3::operator*(float pScalar) const
{
	return multiply(*this, pScalar);
}

FMatrix3& FMatrix3::operator*=(float pScalar)
{
	*this = multiply(*this, pScalar);
	return *this;
}

FVector3 FMatrix3::operator*(const FVector3& pVector) const
{
	return multiply(*this, pVector);
}

FMatrix3 FMatrix3::operator*(const FMatrix3& pOther) const
{
	return multiply(*this, pOther);
}

FMatrix3& FMatrix3::operator*=(const FMatrix3& pOther)
{
	*this = multiply(*this, pOther);
	return *this;
}

FMatrix3 FMatrix3::operator/(float pScalar) const
{
	return divide(*this, pScalar);
}

FMatrix3& FMatrix3::operator/=(float pScalar)
{
	*this = divide(*this, pScalar);
	return *this;
}

FMatrix3 FMatrix3::operator/(const FMatrix3& pOther) const
{
	return divide(*this, pOther);
}

FMatrix3& FMatrix3::operator/=(const FMatrix3& pOther)
{
	*this = divide(*this, pOther);
	return *this;
}

float& FMatrix3::operator()(uint8_t pRow, uint8_t pColumn)
{
	if (pRow >= 3 || pColumn >= 3)
		throw std::out_of_range("Invalid index : " + std::to_string(pRow) + "," + std::to_string(pColumn) + " is out of range");
	return data[pRow][pColumn];
}

bool FMatrix3::areEquals(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	return memcmp(&pLeft, &pRight, 3 * sizeof(FVector3)) == 0;
}

bool Maths::FMatrix3::areEquals(const FMatrix3& pRight)
{
	return areEquals(*this, pRight);
}

FMatrix3 FMatrix3::add(const FMatrix3& pLeft, float pScalar)
{
	FMatrix3 result(pLeft);
	for (uint8_t i = 0; i < 3; ++i)
		result.data[i] += pScalar;
	return result;
}

FMatrix3 FMatrix3::add(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	FMatrix3 result(pLeft);
	for (uint8_t i = 0; i < 3; ++i)
		result.data[i] += pRight.data[i];
	return result;
}

FMatrix3 FMatrix3::subtract(const FMatrix3& pLeft, float pScalar)
{
	FMatrix3 result(pLeft);
	for (FVector3& element : result.data)
		element -= pScalar;
	return result;
}

FMatrix3 FMatrix3::subtract(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	FMatrix3 result(pLeft);
	for (uint8_t i = 0; i < 3; ++i)
		result.data[i] -= pRight.data[i];
	return result;
}

FMatrix3 FMatrix3::multiply(const FMatrix3& pLeft, float pScalar)
{
	FMatrix3 result(pLeft);
	for (FVector3& element : result.data)
		element *= pScalar;
	return result;
}

FVector3 FMatrix3::multiply(const FMatrix3& pMatrix, const FVector3& pVector)
{
	FVector3 result;
	result.x = (pMatrix.data[0].x * pVector.x) + (pMatrix.data[1].x * pVector.y) + (pMatrix.data[2].x * pVector.z);
	result.y = (pMatrix.data[0].y * pVector.x) + (pMatrix.data[1].y * pVector.y) + (pMatrix.data[2].y * pVector.z);
	result.z = (pMatrix.data[0].z * pVector.x) + (pMatrix.data[1].z * pVector.y) + (pMatrix.data[2].z * pVector.z);
	return result;
}

FMatrix3 FMatrix3::multiply(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	FMatrix3 newMat3;

	for (unsigned int i = 0; i < 3; i++)
	{
		FVector3 vec3;
		for (unsigned int j = 0; j < 3; j++)
		{
			vec3[j] = pLeft.data[i].x * pRight.data[0][j]
				+ pLeft.data[i].y * pRight.data[1][j]
				+ pLeft.data[i].z * pRight.data[2][j];
		}
		newMat3.data[i] = vec3;
	}

	return newMat3;
}

FMatrix3 FMatrix3::divide(const FMatrix3& pLeft, float pScalar)
{
	FMatrix3 result(pLeft);
	for (FVector3& element : result.data)
		element /= pScalar;
	return result;
}

FMatrix3 FMatrix3::divide(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	return pLeft * inverse(pRight);
}

bool FMatrix3::isIdentity(const FMatrix3& pMatrix)
{
	return memcmp(Identity.data, pMatrix.data, 3 * sizeof(FVector3)) == 0;
}

bool Maths::FMatrix3::isIdentity()
{
	return isIdentity(*this);
}

float FMatrix3::determinant(const FMatrix3& pMatrix)
{
	return ((pMatrix.data[0].x * ((pMatrix.data[1].y * pMatrix.data[2].z) - (pMatrix.data[1].z * pMatrix.data[2].y))) -
		(pMatrix.data[0].y * ((pMatrix.data[1].x * pMatrix.data[2].z) - (pMatrix.data[1].z * pMatrix.data[2].x))) +
		(pMatrix.data[0].z * ((pMatrix.data[1].x * pMatrix.data[2].y) - (pMatrix.data[1].y * pMatrix.data[2].x))));
}

float Maths::FMatrix3::determinant()
{
	return determinant(*this);
}

FMatrix3 FMatrix3::transpose(const FMatrix3& pMatrix)
{
	return FMatrix3(FVector3(pMatrix.data[0].x, pMatrix.data[1].x, pMatrix.data[2].x),
		FVector3(pMatrix.data[0].y, pMatrix.data[1].y, pMatrix.data[2].y),
		FVector3(pMatrix.data[0].z, pMatrix.data[1].z, pMatrix.data[2].z));
}

FMatrix3 Maths::FMatrix3::transpose()
{
	return transpose(*this);
}

FMatrix3 FMatrix3::cofactor(const FMatrix3& pMatrix)
{
	FMatrix3 min = minor(pMatrix);
	min.data[0].y *= -1;
	min.data[1].x *= -1;
	min.data[1].z *= -1;
	min.data[2].y *= -1;
	return min;
}

FMatrix3 Maths::FMatrix3::cofactor()
{
	return cofactor(*this);
}

FMatrix3 FMatrix3::minor(const FMatrix3& pMatrix)
{
	FMatrix3 newMat3;
	newMat3.data[0].x = ((pMatrix.data[1].y * pMatrix.data[2].z) - (pMatrix.data[1].z * pMatrix.data[2].y));
	newMat3.data[0].y = ((pMatrix.data[1].x * pMatrix.data[2].z) - (pMatrix.data[1].z * pMatrix.data[2].x));
	newMat3.data[0].z = ((pMatrix.data[1].x * pMatrix.data[2].y) - (pMatrix.data[1].y * pMatrix.data[2].x));

	newMat3.data[1].x = ((pMatrix.data[0].y * pMatrix.data[2].z) - (pMatrix.data[0].z * pMatrix.data[2].y));
	newMat3.data[1].y = ((pMatrix.data[0].x * pMatrix.data[2].z) - (pMatrix.data[0].z * pMatrix.data[2].x));
	newMat3.data[1].z = ((pMatrix.data[0].x * pMatrix.data[2].y) - (pMatrix.data[0].y * pMatrix.data[2].x));

	newMat3.data[2].x = ((pMatrix.data[0].y * pMatrix.data[1].z) - (pMatrix.data[0].z * pMatrix.data[1].y));
	newMat3.data[2].y = ((pMatrix.data[0].x * pMatrix.data[1].z) - (pMatrix.data[0].z * pMatrix.data[1].x));
	newMat3.data[2].z = ((pMatrix.data[0].x * pMatrix.data[1].y) - (pMatrix.data[0].y * pMatrix.data[1].x));

	return newMat3;
}

FMatrix3 Maths::FMatrix3::minor()
{
	return minor(*this);
}

FMatrix3 FMatrix3::adjoint(const FMatrix3& pOther)
{
	return transpose(cofactor(pOther));
}

FMatrix3 Maths::FMatrix3::adjoint()
{
	return adjoint(*this);
}

FMatrix3 FMatrix3::inverse(const FMatrix3& pMatrix)
{
	const float aDeterminant = determinant(pMatrix);
	if (aDeterminant == 0)
		throw std::logic_error("Division by 0");

	return adjoint(pMatrix) / aDeterminant;
}

FMatrix3 Maths::FMatrix3::inverse()
{
	return inverse(*this);
}

FMatrix3 FMatrix3::negate(const FMatrix3& pMatrix)
{
	return FMatrix3(-pMatrix.data[0].x, -pMatrix.data[0].y, -pMatrix.data[0].z,
		-pMatrix.data[1].x, -pMatrix.data[1].y, -pMatrix.data[1].z,
		-pMatrix.data[2].x, -pMatrix.data[2].y, -pMatrix.data[2].z);
}

FMatrix3 Maths::FMatrix3::negate()
{
	return negate(*this);
}

FMatrix3 FMatrix3::translation(const FVector2& pTranslation)
{
	return FMatrix3(1, 0, 0,
		0, 1, 0,
		pTranslation.x, pTranslation.y, 1);
}

FMatrix3 FMatrix3::translate(const FMatrix3& pMatrix, const FVector2& pTranslation)
{
	return pMatrix * translation(pTranslation);
}

FMatrix3 Maths::FMatrix3::translate(const FVector2& pTranslation)
{
	return translate(*this, pTranslation);
}

FMatrix3 FMatrix3::rotation(float pRotation)
{
	pRotation = degreesToRadians(pRotation);

	return FMatrix3(std::cos(pRotation), -std::sin(pRotation), 0,
		std::sin(pRotation), std::cos(pRotation), 0,
		0, 0, 1);
}

FMatrix3 FMatrix3::rotate(const FMatrix3& pMatrix, float pRotation)
{
	return pMatrix * rotation(pRotation);
}

FMatrix3 Maths::FMatrix3::rotate(float pRotation)
{
	return rotate(*this, pRotation);
}

FMatrix3 FMatrix3::scaling(const FVector2& pScale)
{
	return FMatrix3(pScale.x, 0, 0,
		0, pScale.y, 0,
		0, 0, 1);
}

FMatrix3 FMatrix3::scale(const FMatrix3& pMatrix, const FVector2& pScale)
{
	return pMatrix * scaling(pScale);
}

FMatrix3 Maths::FMatrix3::scale(const FVector2& pScale)
{
	return scale(*this, pScale);
}

FVector3 FMatrix3::getRow(const FMatrix3& pMatrix, uint8_t pRow)
{
	if (pRow >= 3)
		throw std::out_of_range("Invalid index : " + std::to_string(pRow) + " is out of range");

	return FVector3(pMatrix.data[pRow].x, pMatrix.data[pRow].y, pMatrix.data[pRow].z);
}

FVector3 Maths::FMatrix3::getRow(uint8_t pRow)
{
	return getRow(*this, pRow);
}

FVector3 FMatrix3::getColumn(const FMatrix3& pMatrix, uint8_t pColumn)
{
	if (pColumn >= 3)
		throw std::out_of_range("Invalid index : " + std::to_string(pColumn) + " is out of range");

	return FVector3(pMatrix.data[0][pColumn], pMatrix.data[1][pColumn], pMatrix.data[2][pColumn]);
}

FVector3 Maths::FMatrix3::getColumn(uint8_t pColumn)
{
	return getColumn(*this, pColumn);
}

bool Maths::operator==(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	return pLeft.data[0] == pRight.data[0] &&
		pLeft.data[1] == pRight.data[1] &&
		pLeft.data[2] == pRight.data[2];
}

bool Maths::operator!=(const FMatrix3& pLeft, const FMatrix3& pRight)
{
	return !(pLeft == pRight);
}