#include "Maths/FMatrix3.h"

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define CHECK_MAT3(Mat, MatGlm)\
 CHECK(Mat(0, 0) == Catch::Approx(MatGlm[0][0]));\
 CHECK(Mat(0, 1) == Catch::Approx(MatGlm[0][1]));\
 CHECK(Mat(0, 2) == Catch::Approx(MatGlm[0][2]));\
 CHECK(Mat(1, 0) == Catch::Approx(MatGlm[1][0]));\
 CHECK(Mat(1, 1) == Catch::Approx(MatGlm[1][1]));\
 CHECK(Mat(1, 2) == Catch::Approx(MatGlm[1][2]));\
 CHECK(Mat(2, 0) == Catch::Approx(MatGlm[2][0]));\
 CHECK(Mat(2, 1) == Catch::Approx(MatGlm[2][1]));\
 CHECK(Mat(2, 2) == Catch::Approx(MatGlm[2][2]))

TEST_CASE("Matrice3", "[.all][matrice][Mat3]")
{
	SECTION("Instantiation")
	{
		// default constructor
		Maths::FMatrix3 empty;
		glm::mat3		emptyGlm{};
		CHECK_MAT3(empty, emptyGlm);

		// destructor
		{
			Maths::FMatrix3 temp;
		}

		// basic constructor
		Maths::FMatrix3 oneParam{ 2.5f };
		glm::mat3		oneParamGlm{ 2.5f };
		CHECK_MAT3(oneParam, oneParamGlm);

		Maths::FMatrix3 allParam{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };
		glm::mat3		allParamGlm{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };
		CHECK_MAT3(allParam, allParamGlm);

		// copy constructor
		Maths::FMatrix3 copy{ allParam };
		glm::mat3		copyGlm{ allParamGlm };
		CHECK_MAT3(copy, copyGlm);

		// assignment operator
		empty = allParam;
		emptyGlm = allParamGlm;
		CHECK_MAT3(empty, emptyGlm);

		// Check size
		CHECK(sizeof Maths::FMatrix3 == sizeof glm::mat3);

		// Check memory layout
		CHECK(memcmp(&allParam, &allParamGlm, sizeof Maths::FMatrix3) == 0);
	} // end Instantiation

	SECTION("Accessor")
	{
		float const x = 2.5f;
		float const y = .5f;
		float const z = 2.f;

		Maths::FMatrix3 mat{ x, y, z, x, y, z, x, y, z };

		CHECK(mat(0, 0) == x);
		CHECK(mat(0, 1) == y);
		CHECK(mat(0, 2) == z);
		CHECK(mat(1, 0) == x);
		CHECK(mat(1, 1) == y);
		CHECK(mat(1, 2) == z);
		CHECK(mat(2, 0) == x);
		CHECK(mat(2, 1) == y);
		CHECK(mat(2, 2) == z);
	} // end Accessor

	SECTION("Comparator")
	{
		Maths::FMatrix3 mat{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };
		Maths::FMatrix3 mat2{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };

		CHECK(mat == mat2);
		CHECK(mat != Maths::FMatrix3::Identity);
	} // end Comparator

	SECTION("Arithmetic")
	{
		Maths::FMatrix3 const small{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };
		Maths::FMatrix3 const big{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };

		glm::mat3 const smallGlm{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };
		glm::mat3 const bigGlm{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f };

		SECTION("Addition")
		{
			{
				Maths::FMatrix3 sumAssigment = big;
				sumAssigment += small;

				glm::mat3 sumAssigmentGlm = bigGlm;
				sumAssigmentGlm += smallGlm;

				CHECK_MAT3(sumAssigment, sumAssigmentGlm);
			}

			{
				Maths::FMatrix3 sum = big + small;

				glm::mat3 sumGlm = bigGlm + smallGlm;

				CHECK_MAT3(sum, sumGlm);
			}
		}

		SECTION("Inverse")
		{
			Maths::FMatrix3 inverse = -Maths::FMatrix3::Identity;

			glm::mat3 inverseGlm = -glm::mat3(1.f);

			CHECK_MAT3(inverse, inverseGlm);
		}

		SECTION("Subtraction")
		{
			{
				Maths::FMatrix3 differenceAssignment = big;
				differenceAssignment -= small;

				glm::mat3 differenceAssignmentGlm = bigGlm;
				differenceAssignmentGlm -= smallGlm;

				CHECK_MAT3(differenceAssignment, differenceAssignmentGlm);
			}

			{
				Maths::FMatrix3 difference = big - small;

				glm::mat3 differenceGlm = bigGlm - smallGlm;

				CHECK_MAT3(difference, differenceGlm);
			}
		} // end SECTION("Subtraction")

		SECTION("Multiplication")
		{
			{
				Maths::FMatrix3 producAssignment = big;
				producAssignment *= small;

				glm::mat3 producAssignmentGlm = bigGlm;
				producAssignmentGlm *= smallGlm;

				CHECK_MAT3(producAssignment, producAssignmentGlm);
			}

			{
				Maths::FMatrix3 product = big * small;

				glm::mat3 productGlm = bigGlm * smallGlm;

				CHECK_MAT3(product, productGlm);
			}
		} // end SECTION("Multiplication")
	}

	SECTION("Functionality")
	{
		Maths::FMatrix3 const mat{ 1.f, 0.f, 0.f, 1.f, 0.f, 2.f, 1.f, 2.f, 2.f };
		Maths::FVector2 const vec{ 2.f, 3.f };

		glm::mat3 const	matGlm{ 1.f, 0.f, 0.f, 1.f, 0.f, 2.f, 1.f, 2.f, 2.f };
		glm::vec3 const vecGlm{ 2.f, 3.f, 4.f };

		SECTION("Transpose")
		{
			Maths::FMatrix3 temp = Maths::FMatrix3::transpose(mat);
			glm::mat3 tempGlm = glm::transpose(matGlm);

			CHECK_MAT3(temp, tempGlm);
		}

		SECTION("Inverse")
		{
			Maths::FMatrix3 temp = Maths::FMatrix3::inverse(mat);
			glm::mat3 tempGlm = glm::inverse(matGlm);

			CHECK_MAT3(temp, tempGlm);
		}

		SECTION("Identity")
		{
			Maths::FMatrix3 temp = Maths::FMatrix3::Identity;
			glm::mat3 tempGlm = glm::mat3(1.f);

			CHECK_MAT3(temp, tempGlm);
		}
	}
}