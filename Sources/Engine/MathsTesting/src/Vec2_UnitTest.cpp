#include "Maths/FVector2.h"

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define CHECK_VECTOR2(Vec, VecGlm) CHECK(Vec.x == Catch::Approx(VecGlm.x)); CHECK(Vec.y == Catch::Approx(VecGlm.y))

TEST_CASE("Vector2", "[.all][vector][vector2]")
{
	SECTION("Instantiation")
	{
		// default constructor
		Maths::FVector2 empty;
		glm::vec2 emptyGlm{};
		CHECK_VECTOR2(empty, emptyGlm);

		// destructor
		{
			Maths::FVector2 temp;
		}

		// basic constructor
		Maths::FVector2 oneParam{ 2.5f };
		glm::vec2 oneParamGlm{ 2.5f };
		CHECK_VECTOR2(oneParam, oneParamGlm);

		Maths::FVector2 allParam{ 2.5f, .5f };
		glm::vec2 allParamGlm{ 2.5f, .5f };
		CHECK_VECTOR2(allParam, allParamGlm);

		// copy constructor
		Maths::FVector2 copy{ allParam };
		glm::vec2 copyGlm{ allParamGlm };
		CHECK_VECTOR2(copy, copyGlm);

		// assignment operator
		empty = allParam;
		emptyGlm = allParamGlm;
		CHECK_VECTOR2(empty, emptyGlm);

		CHECK(sizeof Maths::FVector2 == sizeof glm::vec2);

		CHECK(memcmp(&allParam, &allParamGlm, sizeof Maths::FVector2) == 0);
	}

	SECTION("Accessor")
	{
		float const x = 2.5f;
		float const y = .5f;
		Maths::FVector2 quaternion{ x, y };

		{
			Maths::FVector2 const& vectorConst = quaternion;
			CHECK(vectorConst.x == Catch::Approx(x));
			CHECK(vectorConst.y == Catch::Approx(y));
		}

		{
			Maths::FVector2& self = quaternion;
			CHECK(self.x == Catch::Approx(x));
			CHECK(self.y == Catch::Approx(y));
		}
	}

	SECTION("Comparator")
	{
		// Compare with self
		Maths::FVector2 self{ 2.5f, .5f };
		CHECK(self == self);

		// Compare with self
		CHECK_FALSE(self != self);

		// Trying to find copy-paste error
		Maths::FVector2 two{ 2.f, 2.f };
		CHECK_FALSE(Maths::FVector2{ .5f, .2f } == two);
		CHECK_FALSE(Maths::FVector2{ .2f, .5f } == two);
		//CHECK_FALSE(two == Maths::FVector2{ .5f, .2 f });
		//CHECK_FALSE(two == Maths::FVector2{ .2f, .5 f });

		// Testing compare functionality
		Maths::FVector2 small{ 2.5f, .5f };
		Maths::FVector2 big{ 3.75f, 3.f };
	}

	SECTION("Constante")
	{
		// TODO : Fix this
		//CHECK_VECTOR2(Maths::FVector2::Zero, glm::vec2{ 0.f, 0.f });

		//CHECK_VECTOR2(Maths::FVector2::One, glm::vec2{ 1.f, 1.f });

		//CHECK_VECTOR2(Maths::FVector2::UnitX, glm::vec2{ 1.f, 0.f });

		//CHECK_VECTOR2(Maths::FVector2::UnitY, glm::vec2{ 0.f, 1.f });
	}

	SECTION("Arithmetic")
	{
		Maths::FVector2 const small{ 2.5f, .5f };
		Maths::FVector2 const big{ 3.75f, 3.f };

		glm::vec2 const smallGlm{ 2.5f, .5f };
		glm::vec2 const bigGlm{ 3.75f, 3.f };

		SECTION("Addition")
		{
			{
				Maths::FVector2 sumAssignement = big;
				sumAssignement += small;

				glm::vec2 sumAssignementGlm = bigGlm;
				sumAssignementGlm += smallGlm;

				CHECK_VECTOR2(sumAssignement, sumAssignementGlm);
			}

			{
				Maths::FVector2 sum = big + small;

				glm::vec2 sumGlm = bigGlm + smallGlm;

				CHECK_VECTOR2(sum, sumGlm);
			}
		}

		SECTION("Inverse")
		{
			Maths::FVector2 inverse = -Maths::FVector2{ 3.7f, 3.f };

			glm::vec2 inverseGlm = -glm::vec2{ 3.7f, 3.f };

			CHECK_VECTOR2(inverse, inverseGlm);
		}

		SECTION("Substraction")
		{
			{
				Maths::FVector2 subAssignement = big;
				subAssignement -= small;

				glm::vec2 subAssignementGlm = bigGlm;
				subAssignementGlm -= smallGlm;

				CHECK_VECTOR2(subAssignement, subAssignementGlm);
			}

			{
				Maths::FVector2 sub = big - small;

				glm::vec2 subGlm = bigGlm - smallGlm;

				CHECK_VECTOR2(sub, subGlm);
			}
		}

		SECTION("Multiplication")
		{
			{
				Maths::FVector2 productAssignment = big;
				productAssignment *= small;

				glm::vec2 productAssignmentGlm = bigGlm;
				productAssignmentGlm *= smallGlm;

				CHECK_VECTOR2(productAssignment, productAssignmentGlm);
			}

			{
				Maths::FVector2 product = big * small;

				glm::vec2 productGlm = bigGlm * smallGlm;

				CHECK_VECTOR2(product, productGlm);
			}
		}

		SECTION("Division")
		{
			{
				Maths::FVector2 quotientAssignment = big;
				quotientAssignment /= small;

				glm::vec2 quotientAssignmentGlm = bigGlm;
				quotientAssignmentGlm /= smallGlm;

				CHECK_VECTOR2(quotientAssignment, quotientAssignmentGlm);
			}

			{
				Maths::FVector2 quotient = big / small;

				glm::vec2 quotientGlm = bigGlm / smallGlm;

				CHECK_VECTOR2(quotient, quotientGlm);
			}
		}
	}

	SECTION("Functionality")
	{
		Maths::FVector2 const base{ 2.5f, .5f };
		Maths::FVector2 const other{ 3.75f, 3.f };

		glm::vec2 const baseGlm{ 2.5f, .5f };
		glm::vec2 const otherGlm{ 3.75f, 3.f };

		SECTION("Dot")
		{
			float dot = Maths::FVector2::dot(base, other);

			float dotGlm = glm::dot(baseGlm, otherGlm);

			CHECK(dot == dotGlm);
		}

		SECTION("Distance")
		{
			{
				float distance = Maths::FVector2::distance(base, other);

				float distanceGlm = glm::distance(baseGlm, otherGlm);

				CHECK(distance == Catch::Approx(distanceGlm));
			}

			{
				float distanceSquare = Maths::FVector2::distanceSquared(base, other);

				float distanceSquareGlm = glm::distance2(baseGlm, otherGlm);

				CHECK(distanceSquare == Catch::Approx(distanceSquareGlm));
			}
		}

		SECTION("Magnitude")
		{
			{
				float magnitude = Maths::FVector2::magnitude(base);

				float magnitudeGlm = glm::length(baseGlm);

				CHECK(magnitude == Catch::Approx(magnitudeGlm));
			}

			{
				float magnitudeSquare = Maths::FVector2::magnitudeSquared(base);

				float magnitudeSquareGlm = glm::length2(baseGlm);

				CHECK(magnitudeSquare == Catch::Approx(magnitudeSquareGlm));
			}
		}
	}
}