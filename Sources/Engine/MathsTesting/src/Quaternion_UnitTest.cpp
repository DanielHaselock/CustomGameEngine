#include "Maths/FQuaternion.h"

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_XYZW_ONLY
#include <glm/gtc/quaternion.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define CHECK_QUAT(quat, quatGlm) CHECK(quat.x == Catch::Approx(quatGlm.x)); CHECK(quat.y == Catch::Approx(quatGlm.y)); CHECK(quat.z == Catch::Approx(quatGlm.z)); CHECK(quat.w == Catch::Approx(quatGlm.w))

TEST_CASE("Quaternion", "[.all][Quaternion]")
{
	SECTION("Instantiation")
	{
		// Default Constructor
		Maths::FQuaternion empty;
		glm::quat emptyGlm{ 1.f, 0.f, 0.f, 0.f };

		CHECK_QUAT(empty, emptyGlm);

		// Destructor
		{
			Maths::FQuaternion temp;
		}

		// Basic Constructor
		Maths::FQuaternion allParam{ .5f, 2.f, 5.f, 2.5f }; // Axis X, Axis, Y, Axis Z, Angle W
		glm::quat allParamGlm{ 2.5f, .5f, 2.f, 5.f }; // Angle W, Axis X, Axis, Y, Axis Z
		CHECK_QUAT(allParam, allParamGlm);

		// Copy Constructor
		Maths::FQuaternion copy{ allParam };
		glm::quat copyGlm{ allParamGlm };
		CHECK_QUAT(copy, copyGlm);

		// Assignment Operator
		empty = allParam;
		emptyGlm = allParamGlm;
		CHECK_QUAT(empty, emptyGlm);

		// Size compare
		CHECK(sizeof Maths::FQuaternion == sizeof glm::quat);

		// Memory
		CHECK(memcmp(&allParam, &allParamGlm, sizeof Maths::FQuaternion) == 0);
	}

	SECTION("Accessor")
	{
		SECTION("X")
		{
			Maths::FQuaternion quat{ 1.f, 2.f, 3.f, 4.f };
			glm::quat quatGlm{ 4.f, 1.f, 2.f, 3.f };

			CHECK_QUAT(quat, quatGlm);

			quat.x = 5.f;
			quatGlm.x = 5.f;
			CHECK_QUAT(quat, quatGlm);

			CHECK(quat.x == Catch::Approx(quatGlm.x));
		}

		SECTION("Y")
		{
			Maths::FQuaternion quat{ 1.f, 2.f, 3.f, 4.f };
			glm::quat quatGlm{ 4.f, 1.f, 2.f, 3.f };

			CHECK_QUAT(quat, quatGlm);

			quat.y = 5.f;
			quatGlm.y = 5.f;
			CHECK(quat.y == Catch::Approx(quatGlm.y));
		}

		SECTION("Z")
		{
			Maths::FQuaternion quat{ 1.f, 2.f, 3.f, 4.f };
			glm::quat quatGlm{ 4.f, 1.f, 2.f, 3.f };

			CHECK_QUAT(quat, quatGlm);

			quat.z = 5.f;
			quatGlm.z = 5.f;
			CHECK_QUAT(quat, quatGlm);

			CHECK(quat.z == Catch::Approx(quatGlm.z));
		}
	}

	SECTION("Comparator")
	{
		// Compare with self
		Maths::FQuaternion self{ 1.f, 2.f, 3.f, 4.f };
		CHECK(self == self);

		// Trying to find != mistake
		CHECK_FALSE(self != self);

		// Compare with self
		CHECK(Maths::FQuaternion{ 1.f, 2.f, 3.f, 4.f } == Maths::FQuaternion{ 1.f, 2.f, 3.f, 4.f });

		// trying to find copy/paste mistake
		Maths::FQuaternion twos{ 2.f, 2.f,2.f,2.f };
		CHECK_FALSE(Maths::FQuaternion{ .5f, 2.f, 2.f, 2.f } == twos);
		CHECK_FALSE(Maths::FQuaternion{ 2.f, .5f, 2.f, 2.f } == twos);
		CHECK_FALSE(Maths::FQuaternion{ 2.f, 2.f, .5f, 2.f } == twos);
		CHECK_FALSE(Maths::FQuaternion{ 2.f, 2.f, 2.f, .5f } == twos);
		CHECK_FALSE(twos == Maths::FQuaternion{ .5f, 2.f, 2.f, 2.f });
		CHECK_FALSE(twos == Maths::FQuaternion{ 2.f, .5f, 2.f, 2.f });
		CHECK_FALSE(twos == Maths::FQuaternion{ 2.f, 2.f, .5f, 2.f });
		CHECK_FALSE(twos == Maths::FQuaternion{ 2.f, 2.f, 2.f, .5f });
	}

	SECTION("Arithmetic")
	{
		Maths::FQuaternion const small{ 2.f, 3.f, 4.f, 1.f };
		Maths::FQuaternion const big{ 3.f, 2.f, 1.f, 4.f, };

		glm::quat const smallGlm{ 1.f, 2.f, 3.f, 4.f };
		glm::quat const bigGlm{ 4.f, 3.f, 2.f, 1.f };

		SECTION("Addition")
		{
			{
				Maths::FQuaternion sum = big;
				sum += small;

				glm::quat sumGlm = bigGlm;
				sumGlm += smallGlm;

				CHECK_QUAT(sum, sumGlm);
			}

			{
				Maths::FQuaternion sum = big + small;

				glm::quat sumGlm = bigGlm + smallGlm;

				CHECK_QUAT(sum, sumGlm);
			}
		}

		SECTION("Opposite")
		{
			Maths::FQuaternion opposite = -big;

			glm::quat oppositeGlm = -bigGlm;

			CHECK_QUAT(opposite, oppositeGlm);
		}

		SECTION("Subtraction")
		{
			Maths::FQuaternion difference = big - small;

			glm::quat differenceGlm = bigGlm - smallGlm;

			CHECK_QUAT(difference, differenceGlm);
		}

		SECTION("Multiplication")
		{
			{
				Maths::FQuaternion product = big * small;

				glm::quat productGlm = bigGlm * smallGlm;

				CHECK_QUAT(product, productGlm);
			}

			{
				Maths::FQuaternion product = big * 2.f;

				glm::quat productGlm = bigGlm * 2.f;

				CHECK_QUAT(product, productGlm);
			}
		}

		SECTION("Division")
		{
			{
				Maths::FQuaternion quotient = big / 2.f;

				glm::quat quotientGlm = bigGlm / 2.f;

				CHECK_QUAT(quotient, quotientGlm);
			}
		}
	}

	SECTION("Transformation")
	{
		Maths::FQuaternion const origin{ .5f, 2.f , 1.f, 2.5f };
		Maths::FQuaternion const rotation{ 0.5f, 0.5f, 0.5f, 0.5f };

		glm::quat const originGlm{ 2.5f, .5f, 2.f, 1.f };
		glm::quat const rotationGlm{ 0.5f, 0.5f, 0.5f, 0.5f };

		SECTION("Rotate")
		{
			Maths::FQuaternion rotated = rotation * origin;

			glm::quat rotatedGlm = rotationGlm * originGlm;

			CHECK_QUAT(rotated, rotatedGlm);
		}
	}

	SECTION("Functionality")
	{
		Maths::FQuaternion const base{ .5f, 2.f, 7.f, 2.5f };
		Maths::FQuaternion const other{ 3.f, 2.f, 1.f, 4.f };

		glm::quat const baseGlm{ 2.5f, .5f, 2.f, 7.f };
		glm::quat const otherGlm{ 4.f, 3.f, 2.f, 1.f };

		SECTION("Dot")
		{
			float dot = Maths::FQuaternion::dotProduct(base, other);

			float dotGlm = glm::dot(baseGlm, otherGlm);

			CHECK(dot == dotGlm);
		}

		SECTION("Length")
		{
			float length = Maths::FQuaternion::length(base);

			float lengthGlm = glm::length(baseGlm);

			CHECK(length == lengthGlm);
		}

		SECTION("length2")
		{
			float lengthSquared = Maths::FQuaternion::lengthSquared(base);

			float lengthSquaredGlm = glm::length(baseGlm) * glm::length(baseGlm);

			CHECK(lengthSquared == Catch::Approx(lengthSquaredGlm));
		}

		SECTION("Inverse")
		{
			Maths::FQuaternion inverse = Maths::FQuaternion::inverse(base);

			glm::quat inverseGlm = glm::inverse(baseGlm);

			CHECK_QUAT(inverse, inverseGlm);
		}

		SECTION("Normalize")
		{
			Maths::FQuaternion normalized = Maths::FQuaternion::normalize(base);

			glm::quat normalizedGlm = glm::normalize(baseGlm);

			CHECK_QUAT(normalized, normalizedGlm);
		}
	}
}