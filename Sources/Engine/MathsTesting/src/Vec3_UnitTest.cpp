#include "Maths/FVector3.h"

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define CHECK_VECTOR3(Vec, VecGlm) CHECK(Vec.x == Catch::Approx(VecGlm.x)); CHECK(Vec.y == Catch::Approx(VecGlm.y)); CHECK(Vec.z == Catch::Approx(VecGlm.z))

TEST_CASE("Vector3", "[.all][vector][Vector3]")
{
	SECTION("Instantiation")
	{
		// default constructor
		Maths::FVector3 empty;
		glm::vec3 emptyGlm{};
		CHECK_VECTOR3(empty, emptyGlm);

		// destructor
		{
			Maths::FVector3 temp;
		}

		// basic constructor
		Maths::FVector3 oneParam{ 2.5f };
		glm::vec3 oneParamGlm{ 2.5f };
		CHECK_VECTOR3(oneParam, oneParamGlm);

		Maths::FVector3 allParam{ 2.5f, .5f, 2.f };
		glm::vec3 allParamGlm{ 2.5f, .5f, 2.f };
		CHECK_VECTOR3(allParam, allParamGlm);

		// copy constructor
		Maths::FVector3 copy{ allParam };
		glm::vec3 copyGlm{ allParamGlm };
		CHECK_VECTOR3(copy, copyGlm);

		// assignment operator
		empty = allParam;
		emptyGlm = allParamGlm;
		CHECK_VECTOR3(empty, emptyGlm);

		CHECK(sizeof Maths::FVector3 == sizeof glm::vec3);

		CHECK(memcmp(&allParam, &allParamGlm, sizeof Maths::FVector3) == 0);
	}

	SECTION("Accessor")
	{
		float const x = 2.5f;
		float const y = .5f;
		float const z = 2.f;
		Maths::FVector3 quaternion{ x, y, z };

		{
			Maths::FVector3 const& vectorConst = quaternion;
			CHECK(vectorConst[0] == x);
			CHECK(vectorConst[1] == y);
			CHECK(vectorConst[2] == z);
		}

		{
			quaternion[0] += 1.f;
			quaternion[1] += 1.f;
			quaternion[2] += 1.f;
			CHECK(quaternion[0] == x + 1.f);
			CHECK(quaternion[1] == y + 1.f);
			CHECK(quaternion[2] == z + 1.f);
		}
	}

	SECTION("Comparator")
	{
		// compare with self
		Maths::FVector3 self{ 2.5f, .5f, 2.f };
		CHECK(self == self);

		// compare with same
		CHECK(Maths::FVector3{ 2.5f, .5f, 2.f } == Maths::FVector3{ 2.5f, .5f, 2.f });

		// trying to find copy/paste mistake
		Maths::FVector3 twos{ 2.f, 2.f, 2.f };
		CHECK_FALSE(Maths::FVector3{ .5f, 2.f, 2.f } == twos);
		CHECK_FALSE(Maths::FVector3{ 2.f, .5f, 2.f } == twos);
		CHECK_FALSE(Maths::FVector3{ 2.f, 2.f, .5f } == twos);
		CHECK_FALSE(twos == Maths::FVector3{ .5f, 2.f, 2.f });
		CHECK_FALSE(twos == Maths::FVector3{ 2.f, .5f, 2.f });
		CHECK_FALSE(twos == Maths::FVector3{ 2.f, 2.f, .5f });

		// testing compare functionality
		Maths::FVector3 small{ 2.5f, .5f, 2.f };
		Maths::FVector3 big{ 3.75f, 3.f, .75f };

		//big.isLongerThan(small);
		//CHECK(big.isLongerThan(small));
		//CHECK_FALSE(small.isLongerThan(big));

		//CHECK(small.isShorterThan(big));
		//CHECK_FALSE(big.isShorterThan(small));

		//CHECK_FALSE(self.isUnit());
		//CHECK(Maths::FVector3{ 0.771516740f, 0.154303357f, 0.617213428f }.isUnit());
	}

	SECTION("Constant")
	{
		CHECK_VECTOR3(Maths::FVector3::Backward, glm::vec3(0.f, 0.f, -1.f));

		CHECK_VECTOR3(Maths::FVector3::Bottom, glm::vec3(0.f, -1.f, 0.f));

		CHECK_VECTOR3(Maths::FVector3::Forward, glm::vec3(0.f, 0.f, 1.f));

		CHECK_VECTOR3(Maths::FVector3::Left, glm::vec3(-1.f, 0.f, 0.f));

		CHECK_VECTOR3(Maths::FVector3::One, glm::vec3(1.f, 1.f, 1.f));

		CHECK_VECTOR3(Maths::FVector3::Right, glm::vec3(1.f, 0.f, 0.f));

		CHECK_VECTOR3(Maths::FVector3::Up, glm::vec3(0.f, 1.f, 0.f));

		CHECK_VECTOR3(Maths::FVector3::Zero, glm::vec3(0.f, 0.f, 0.f));
	}

	SECTION("Arithmetic")
	{
		Maths::FVector3 const small{ 2.5f, .5f, 2.f };
		Maths::FVector3 const big{ 3.75f, 3.f, .75f };

		glm::vec3 const smallGlm{ 2.5f, .5f, 2.f };
		glm::vec3 const bigGlm{ 3.75f, 3.f, .75f };

		SECTION("Addition")
		{
			{
				Maths::FVector3 sumAssignment = big;
				sumAssignment += small;

				glm::vec3 sumAssignmentGlm = bigGlm;
				sumAssignmentGlm += smallGlm;

				CHECK_VECTOR3(sumAssignment, sumAssignmentGlm);
			}

			{
				Maths::FVector3 sum = big + small;

				glm::vec3 sumGlm = bigGlm + smallGlm;

				CHECK_VECTOR3(sum, sumGlm);
			}
		}

		SECTION("Inverse")
		{
			Maths::FVector3 inverse = -Maths::FVector3{ 3.7f, 3.f, .7f };

			glm::vec3 inverseGlm = -glm::vec3{ 3.7f, 3.f, .7f };

			CHECK_VECTOR3(inverse, inverseGlm);
		}

		SECTION("Substraction")
		{
			{
				Maths::FVector3 differenceAssignment = big;
				differenceAssignment -= small;

				glm::vec3 differenceAssignmentGlm = bigGlm;
				differenceAssignmentGlm -= smallGlm;

				CHECK_VECTOR3(differenceAssignment, differenceAssignmentGlm);
			}

			{
				Maths::FVector3 difference = big - small;

				glm::vec3 differenceGlm = bigGlm - smallGlm;

				CHECK_VECTOR3(difference, differenceGlm);
			}
		}

		SECTION("Multiplication")
		{
			{
				Maths::FVector3 productAssignment = big;
				productAssignment *= small;

				glm::vec3 productAssignmentGlm = bigGlm;
				productAssignmentGlm *= smallGlm;

				CHECK_VECTOR3(productAssignment, productAssignmentGlm);
			}

			{
				Maths::FVector3 product = big * small;

				glm::vec3 productGlm = bigGlm * smallGlm;

				CHECK_VECTOR3(product, productGlm);
			}
		}

		SECTION("Division")
		{
			{
				Maths::FVector3 quotientAssignment = big;
				quotientAssignment /= small;

				glm::vec3 quotientAssignmentGlm = bigGlm;
				quotientAssignmentGlm /= smallGlm;

				CHECK_VECTOR3(quotientAssignment, quotientAssignmentGlm);
			}

			{
				Maths::FVector3 quotient = big / small;

				glm::vec3 quotientGlm = bigGlm / smallGlm;

				CHECK_VECTOR3(quotient, quotientGlm);
			}
		}
	}

	SECTION("Functionality")
	{
		Maths::FVector3 const base{ 2.5f, .5f, 2.f };
		Maths::FVector3 const other{ 3.75f, 3.f, .75f };

		glm::vec3 const baseGlm{ 2.5f, .5f, 2.f };
		glm::vec3 const otherGlm{ 3.75f, 3.f, .75f };

		SECTION("Cross")
		{
			Maths::FVector3 cross = Maths::FVector3::cross(base, other);

			glm::vec3 crossGlm = glm::cross(baseGlm, otherGlm);

			CHECK_VECTOR3(cross, crossGlm);
		}

		SECTION("Dot")
		{
			float dot = Maths::FVector3::dot(base, other);

			float dotGlm = glm::dot(baseGlm, otherGlm);

			CHECK(dot == dotGlm);
		}

		SECTION("Distance")
		{
			{
				float distance = Maths::FVector3::distance(base, other);

				float distanceGlm = glm::distance(baseGlm, otherGlm);

				CHECK(distance == Catch::Approx(distanceGlm));
			}

			{
				float distanceSquare = Maths::FVector3::distanceSquared(base, other);

				float distanceSquareGlm = glm::distance2(baseGlm, otherGlm);

				CHECK(distanceSquare == Catch::Approx(distanceSquareGlm));
			}

			{
				float distance2D = Maths::FVector3::distance2DFrom(base, other);

				float distance2DGlm = glm::distance(glm::vec2(baseGlm), glm::vec2(otherGlm));

				CHECK(distance2D == Catch::Approx(distance2DGlm));
			}

			{
				float distance2DSquare = Maths::FVector3::distance2DSquaredFrom(base, other);

				float distance2DSquareGlm = glm::distance2(glm::vec2(baseGlm), glm::vec2(otherGlm));

				CHECK(distance2DSquare == Catch::Approx(distance2DSquareGlm));
			}
		}

		SECTION("Magnitude")
		{
			{
				float magnitude = Maths::FVector3::magnitude(base);

				float magnitudeGlm = glm::length(baseGlm);

				CHECK(magnitude == Catch::Approx(magnitudeGlm));
			}

			{
				float magnitudeSquare = Maths::FVector3::magnitudeSquared(base);

				float magnitudeSquareGlm = glm::length2(baseGlm);

				CHECK(magnitudeSquare == Catch::Approx(magnitudeSquareGlm));
			}
		}

		SECTION("Normal")
		{
			Maths::FVector3 normalize = Maths::FVector3::normalize(base);

			glm::vec3 normalizeGlm = glm::normalize(baseGlm);

			CHECK_VECTOR3(normalize, normalizeGlm);
		}

		SECTION("ProjectOnto")
		{
			// TODO: implement project onto

			//Maths::FVector3 project = base;
			//project.project(other);

			//glm::vec3 projectGlm = glm::proj(baseGlm, otherGlm);

			//CHECK_VECTOR3(project, projectGlm);
		}

		SECTION("ReflectOnto")
		{
			// TODO: implement reflect onto

			//Maths::FVector3 reflect = base;
			//reflect.reflect(other);

			//glm::vec3 reflectGlm = glm::reflect(baseGlm, glm::normalize(otherGlm));

			//CHECK_VECTOR3(reflect, reflectGlm);
		}
	}
}