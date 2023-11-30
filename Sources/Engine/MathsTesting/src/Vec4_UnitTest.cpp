#include "Maths/FVector4.h"

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define CHECK_VECTOR4(Vec, VecGlm) CHECK(Vec.x == Catch::Approx(VecGlm.x)); CHECK(Vec.y == Catch::Approx(VecGlm.y)); CHECK(Vec.z == Catch::Approx(VecGlm.z)); CHECK(Vec.w == Catch::Approx(VecGlm.w))

TEST_CASE("Vector4", "[.all][vector][Vector4]")
{
	SECTION("Instantiation")
	{
		// default constructor
		Maths::FVector4 empty;
		glm::vec4 emptyGlm{};
		CHECK_VECTOR4(empty, emptyGlm);

		// destructor
		{
			Maths::FVector4 temp;
		}

		// basic constructor
		Maths::FVector4 oneParam{ 2.5f };
		glm::vec4 oneParamGlm{ 2.5f };
		CHECK_VECTOR4(oneParam, oneParamGlm);

		Maths::FVector4 allParam{ 2.5f, .5f, 2.f, 1.f };
		glm::vec4 allParamGlm{ 2.5f, .5f, 2.f, 1.f };
		CHECK_VECTOR4(allParam, allParamGlm);

		// copy constructor
		Maths::FVector4 copy{ allParam };
		glm::vec4 copyGlm{ allParamGlm };
		CHECK_VECTOR4(copy, copyGlm);

		// assignment operator
		empty = allParam;
		emptyGlm = allParamGlm;
		CHECK_VECTOR4(empty, emptyGlm);

		CHECK(sizeof Maths::FVector4 == sizeof glm::vec4);

		CHECK(memcmp(&allParam, &allParamGlm, sizeof Maths::FVector4) == 0);
	}

	SECTION("Accessor")
	{
		float const x = 2.5f;
		float const y = .5f;
		float const z = 2.f;
		float const w = 1.f;
		Maths::FVector4 quaternion{ x, y, z, w };

		{
			Maths::FVector4 const& vectorConst = quaternion;
			CHECK(vectorConst.x == x);
			CHECK(vectorConst.y == y);
			CHECK(vectorConst.z == z);
			CHECK(vectorConst.w == w);
		}

		{
			Maths::FVector4& vector = quaternion;
			CHECK(vector.x == x);
			CHECK(vector.y == y);
			CHECK(vector.z == z);
			CHECK(vector.w == w);
		}
	}

	SECTION("Constant")
	{
		CHECK_VECTOR4(Maths::FVector4::One, glm::vec4{ 1.f });
		CHECK_VECTOR4(Maths::FVector4::Zero, glm::vec4{ 0.f });
	}

	SECTION("Arithmetic")
	{
		Maths::FVector4 const small{ 2.5f, .5f, 2.f, 1.f };
		Maths::FVector4 const big{ 3.75f, 3.f, .75f, 2.f };

		glm::vec4 const smallGlm{ 2.5f, .5f, 2.f, 1.f };
		glm::vec4 const bigGlm{ 3.75f, 3.f, .75f, 2.f };

		SECTION("Addition")
		{
			{
				Maths::FVector4 sumAssignment = big;
				sumAssignment += small;

				glm::vec4 sumAssignmentGlm = bigGlm;
				sumAssignmentGlm += smallGlm;

				CHECK_VECTOR4(sumAssignment, sumAssignmentGlm);
			}

			{
				Maths::FVector4 sum = big + small;

				glm::vec4 sumGlm = bigGlm + smallGlm;

				CHECK_VECTOR4(sum, sumGlm);
			}
		}

		SECTION("Inverse")
		{
			Maths::FVector4 inverse = -Maths::FVector4{ 3.7f, 3.f, .7f, 2.f };

			glm::vec4 inverseGlm = -glm::vec4{ 3.7f, 3.f, .7f, 2.f };

			CHECK_VECTOR4(inverse, inverseGlm);
		}

		SECTION("Subtraction")
		{
			{
				Maths::FVector4 differenceAssignment = big;
				differenceAssignment -= small;

				glm::vec4 differenceAssignmentGlm = bigGlm;
				differenceAssignmentGlm -= smallGlm;

				CHECK_VECTOR4(differenceAssignment, differenceAssignmentGlm);
			}

			{
				Maths::FVector4 difference = big - small;

				glm::vec4 differenceGlm = bigGlm - smallGlm;

				CHECK_VECTOR4(difference, differenceGlm);
			}
		}

		SECTION("Multiplication")
		{
			{
				Maths::FVector4 productAssignment = big;
				productAssignment *= small;

				glm::vec4 productAssignmentGlm = bigGlm;
				productAssignmentGlm *= smallGlm;

				CHECK_VECTOR4(productAssignment, productAssignmentGlm);
			}

			{
				Maths::FVector4 product = big * small;

				glm::vec4 productGlm = bigGlm * smallGlm;

				CHECK_VECTOR4(product, productGlm);
			}
		}

		SECTION("Division")
		{
			{
				Maths::FVector4 quotientAssignment = big;
				quotientAssignment /= small;

				glm::vec4 quotientAssignmentGlm = bigGlm;
				quotientAssignmentGlm /= smallGlm;

				CHECK_VECTOR4(quotientAssignment, quotientAssignmentGlm);
			}

			{
				Maths::FVector4 quotient = big / small;

				glm::vec4 quotientGlm = bigGlm / smallGlm;

				CHECK_VECTOR4(quotient, quotientGlm);
			}
		}
	}

	SECTION("Functionality")
	{
		Maths::FVector4 const base{ 2.5f, .5f, 2.f, 1.f };
		Maths::FVector4 const other{ 3.75f, 3.f, .75f, 2.f };

		glm::vec4 const baseGlm{ 2.5f, .5f, 2.f, 1.f };
		glm::vec4 const otherGlm{ 3.75f, 3.f, .75f, 2.f };

		SECTION("Dot")
		{
			float dot = Maths::FVector4::dot(base, other);

			float dotGlm = glm::dot(baseGlm, otherGlm);

			CHECK(dot == dotGlm);
		}

		SECTION("Distance")
		{
			{
				float distance = Maths::FVector4::distance(base, other);

				float distanceGlm = glm::distance(baseGlm, otherGlm);

				CHECK(distance == Catch::Approx(distanceGlm));
			}

			{
				float distanceSquared = Maths::FVector4::distanceSquared(base, other);

				float distanceSquaredGlm = glm::distance2(baseGlm, otherGlm);

				CHECK(distanceSquared == Catch::Approx(distanceSquaredGlm));
			}

			{
				float distance2D = Maths::FVector4::distance2DFrom(base, other);

				float distance2DGlm = glm::distance(glm::vec2{ baseGlm }, glm::vec2{ otherGlm });

				CHECK(distance2D == Catch::Approx(distance2DGlm));
			}

			{
				float distance2DSquared = Maths::FVector4::distance2DSquaredFrom(base, other);

				float distance2DSquaredGlm = glm::distance2(glm::vec2{ baseGlm }, glm::vec2{ otherGlm });

				CHECK(distance2DSquared == Catch::Approx(distance2DSquaredGlm));
			}
		}

		SECTION("Magnitude")
		{
			{
				float magnitude = Maths::FVector4::magnitude(base);

				float magnitudeGlm = glm::length(baseGlm);

				CHECK(magnitude == Catch::Approx(magnitudeGlm));
			}

			{
				float magnitudeSquared = Maths::FVector4::magnitudeSquared(base);

				float magnitudeSquaredGlm = glm::length2(baseGlm);

				CHECK(magnitudeSquared == Catch::Approx(magnitudeSquaredGlm));
			}
		}

		SECTION("Normal")
		{
			Maths::FVector4 normalize = Maths::FVector4::normalize(base);

			glm::vec4 normalizeGlm = glm::normalize(baseGlm);

			CHECK_VECTOR4(normalize, normalizeGlm);
		}
	}
}