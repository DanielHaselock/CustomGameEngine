#pragma once


namespace Maths
{

	struct IVector4
	{
		static const IVector4 One;
		static const IVector4 Zero;

		int x;
		int y;
		int z;
		int w;

		IVector4(int pX = 0, int pY = 0, int pZ = 0, int pW = 0);

		IVector4(const IVector4& pToCopy);

		IVector4(IVector4&& pToMove) noexcept = default;

		IVector4 operator-() const;

		IVector4 operator=(const IVector4& pOther);

		IVector4 operator+(const IVector4& pOther) const;

		IVector4& operator+=(const IVector4& pOther);

		IVector4 operator-(const IVector4& pOther) const;

		IVector4& operator-=(const IVector4& pOther);

		IVector4 operator*(int pScalar) const;

		IVector4& operator*=(int pScalar);

		IVector4 operator/(int pScalar) const;

		IVector4& operator/=(int pScalar);

		bool operator==(const IVector4& pOther);

		bool operator!=(const IVector4& pOther);

		int& operator[](int pIdx);

		int operator[](int pIdx) const;

		static IVector4 add(const IVector4& pLeft, const IVector4& pRight);

		static IVector4 substract(const IVector4& pLeft, const IVector4& pRight);

		static IVector4 multiply(const IVector4& pTarget, int pScalar);

		static IVector4 divide(const IVector4& pLeft, int pScalar);

		static int length(const IVector4& pTarget);

		int length();

		static int dot(const IVector4& pLeft, const IVector4& pRight);

		int dot(const IVector4& pRight);

		static IVector4 normalize(const IVector4& pTarget);

		IVector4 normalize();

		static IVector4 lerp(const IVector4& pA, const IVector4& pB, int pT);
	};
}