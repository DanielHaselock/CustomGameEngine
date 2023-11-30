#include "Maths/FMatrix4.h"
#include "Maths/FQuaternion.h"

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define CHECK_MAT4(Mat,GlmMat)\
 CHECK(Mat(0, 0) == Catch::Approx(GlmMat[0][0]));\
 CHECK(Mat(0, 1) == Catch::Approx(GlmMat[0][1]));\
 CHECK(Mat(0, 2) == Catch::Approx(GlmMat[0][2]));\
 CHECK(Mat(0, 3) == Catch::Approx(GlmMat[0][3]));\
 CHECK(Mat(1, 0) == Catch::Approx(GlmMat[1][0]));\
 CHECK(Mat(1, 1) == Catch::Approx(GlmMat[1][1]));\
 CHECK(Mat(1, 2) == Catch::Approx(GlmMat[1][2]));\
 CHECK(Mat(1, 3) == Catch::Approx(GlmMat[1][3]));\
 CHECK(Mat(2, 0) == Catch::Approx(GlmMat[2][0]));\
 CHECK(Mat(2, 1) == Catch::Approx(GlmMat[2][1]));\
 CHECK(Mat(2, 2) == Catch::Approx(GlmMat[2][2]));\
 CHECK(Mat(2, 3) == Catch::Approx(GlmMat[2][3]));\
 CHECK(Mat(3, 0) == Catch::Approx(GlmMat[3][0]));\
 CHECK(Mat(3, 1) == Catch::Approx(GlmMat[3][1]));\
 CHECK(Mat(3, 2) == Catch::Approx(GlmMat[3][2]));\
 CHECK(Mat(3, 3) == Catch::Approx(GlmMat[3][3]))

TEST_CASE("Matrice4", "[.all][matrice][Mat4]")
{
	SECTION("Instantiation")
	{
		// default constructor
		Maths::FMatrix4 empty;
		glm::mat4		emptyGlm{};
		CHECK_MAT4(empty, emptyGlm);

		// destructor
		{
			Maths::FMatrix4 temp;
		}

		// basic constructor
		Maths::FMatrix4 oneParam{ 2.5f };
		glm::mat4		oneParamGlm{ 2.5f };
		CHECK_MAT4(oneParam, oneParamGlm);

		Maths::FMatrix4 allParam{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f, 2.f, 2.5f, .5f, .5f, 1.f, 2.f, 2.5f };
		glm::mat4		allParamGlm{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f, 2.f, 2.5f, .5f, .5f, 1.f, 2.f, 2.5f };
		CHECK_MAT4(allParam, allParamGlm);

		// copy constructor
		Maths::FMatrix4 copy{ allParam };
		glm::mat4		copyGlm{ allParamGlm };
		CHECK_MAT4(copy, copyGlm);

		// assignment operator
		empty = allParam;
		emptyGlm = allParamGlm;
		CHECK_MAT4(empty, emptyGlm);

		// Check size
		CHECK(sizeof Maths::FMatrix4 == sizeof glm::mat4);

		// Check memory layout
		CHECK(memcmp(&allParam, &allParamGlm, sizeof Maths::FMatrix4) == 0);
	} // end SECTION["Instantiation"]

	SECTION("Accessor")
	{
		float const x = 2.5f;
		float const y = .5f;
		float const z = 2.f;
		float const w = 1.f;

		Maths::FMatrix4 mat{ x, y, z, w, x, y, z, w,x, y, z, w,x, y, z, w };

		CHECK(mat(0, 0) == x);
		CHECK(mat(0, 1) == y);
		CHECK(mat(0, 2) == z);
		CHECK(mat(0, 3) == w);
		CHECK(mat(1, 0) == x);
		CHECK(mat(1, 1) == y);
		CHECK(mat(1, 2) == z);
		CHECK(mat(1, 3) == w);
		CHECK(mat(2, 0) == x);
		CHECK(mat(2, 1) == y);
		CHECK(mat(2, 2) == z);
		CHECK(mat(2, 3) == w);
		CHECK(mat(3, 0) == x);
		CHECK(mat(3, 1) == y);
		CHECK(mat(3, 2) == z);
		CHECK(mat(3, 3) == w);
	} // end SECTION["Accessor"]

	SECTION("Comparator")
	{
		Maths::FMatrix4 mat{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f, 2.f, 2.5f, .5f, .5f, 1.f, 2.f, 2.5f };
		Maths::FMatrix4 mat2{ 2.5f, .5f, 2.f, 1.f, 2.f, 1.f, .5f, 2.5f, 1.f, 2.f, 2.5f, .5f, .5f, 1.f, 2.f, 2.5f };

		CHECK(mat == mat2);
		CHECK(mat != Maths::FMatrix4::Identity);
	} // end SECTION["Comparator"]

	SECTION("Arithmetic")
	{
		Maths::FMatrix4 const small{ 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f };
		Maths::FMatrix4 const big{ 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f };

		glm::mat4 const smallGlm{ 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f };
		glm::mat4 const bigGlm{ 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f, 2.5f, .5f, 2.f, 1.f };

		SECTION("Addition")
		{
			{
				Maths::FMatrix4 sumAssignment = big;
				sumAssignment += small;

				glm::mat4 sumAssignmentGlm = bigGlm;
				sumAssignmentGlm += smallGlm;

				CHECK_MAT4(sumAssignment, sumAssignmentGlm);
			}

			{
				Maths::FMatrix4 sum = big + small;

				glm::mat4 sumGlm = bigGlm + smallGlm;

				CHECK_MAT4(sum, sumGlm);
			}
		} // end SECTION("Addition")

		SECTION("Inverse")
		{
			Maths::FMatrix4 inverse = -Maths::FMatrix4::Identity;

			glm::mat4 inverseGlm = -glm::mat4(1.f);

			CHECK_MAT4(inverse, inverseGlm);
		} // end SECTION("Inverse")

		SECTION("Subtraction")
		{
			{
				Maths::FMatrix4 differenceAssignment = big;
				differenceAssignment -= small;

				glm::mat4 differenceAssignmentGlm = bigGlm;
				differenceAssignmentGlm -= smallGlm;

				CHECK_MAT4(differenceAssignment, differenceAssignmentGlm);
			}

			{
				Maths::FMatrix4 difference = big - small;

				glm::mat4 differenceGlm = bigGlm - smallGlm;

				CHECK_MAT4(difference, differenceGlm);
			}
		} // end SECTION("Subtraction")

		SECTION("Multiplication")
		{
			{
				Maths::FMatrix4 producAssignment = big;
				producAssignment *= small;

				glm::mat4 producAssignmentGlm = bigGlm;
				producAssignmentGlm *= smallGlm;

				CHECK_MAT4(producAssignment, producAssignmentGlm);
			}

			{
				Maths::FMatrix4 product = big * small;

				glm::mat4 productGlm = bigGlm * smallGlm;

				CHECK_MAT4(product, productGlm);
			}
		} // end SECTION("Multiplication")
	} // // end SECTION("Arithmetic")

	SECTION("Functionality")
	{
		Maths::FMatrix4 const mat{ 1,0,0,1,0,2,1,2,2,1,0,1,2,0,1,4 };
		Maths::FVector3 const vec{ 2.f, 3.f, 4.f };

		glm::mat4 const	matGlm{ 1,0,0,1,0,2,1,2,2,1,0,1,2,0,1,4 };
		glm::vec3 const vecGlm{ 2.f, 3.f, 4.f };

		SECTION("Transpose")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::transpose(mat);
			glm::mat4 tempGlm = glm::transpose(matGlm);

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Inverse")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::inverse(mat);
			glm::mat4 tempGlm = glm::inverse(matGlm);

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Identity")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::Identity;
			glm::mat4 tempGlm = glm::mat4(1.f);

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Translation")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::translation(vec);
			glm::mat4 tempGlm = glm::translate(glm::mat4(1.f), vecGlm);

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Scale")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::scale(mat, vec);
			glm::mat4 tempGlm = glm::scale(matGlm, vecGlm);

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Orthographic")
		{
			// TODO: Fix this
			/*Maths::FMatrix4 temp = Maths::FMatrix4::createOrthographic(2.f / 3.f, 4.f / 5.f, 6.f, 7.f);
			glm::mat4 tempGlm = glm::ortho(2.f, 3.f, 4.f, 5.f, 6.f, 7.f);

			CHECK_MAT4(temp, tempGlm);*/
		}

		SECTION("Perspective")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::createPerspective(2.f, 3.f, 4.f, 5.f);
			glm::mat4 tempGlm = glm::perspective(glm::radians(2.f), 3.f, 4.f, 5.f);

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("LookAt")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::lookAt(Maths::FVector3(1.f, 2.f, 3.f), Maths::FVector3(4.f, 5.f, 6.f), Maths::FVector3(7.f, 8.f, 9.f));
			glm::mat4 tempGlm = glm::lookAt(glm::vec3(1.f, 2.f, 3.f), glm::vec3(4.f, 5.f, 6.f), glm::vec3(7.f, 8.f, 9.f));

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Rotation-Quat")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::rotation(Maths::FQuaternion::Identity);
			glm::mat4 tempGlm = glm::mat4_cast(glm::quat(1.f, 0.f, 0.f, 0.f));

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Rotation-X")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::rotateOnAxisX(mat, 2.f);
			glm::mat4 tempGlm = glm::rotate(matGlm, glm::radians(2.f), glm::vec3(1.f, 0.f, 0.f));

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Rotation-Y")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::rotateOnAxisY(mat, 2.f);
			glm::mat4 tempGlm = glm::rotate(matGlm, glm::radians(2.f), glm::vec3(0.f, 1.f, 0.f));

			CHECK_MAT4(temp, tempGlm);
		}

		SECTION("Rotation-Z")
		{
			Maths::FMatrix4 temp = Maths::FMatrix4::rotateOnAxisZ(mat, 2.f);
			glm::mat4 tempGlm = glm::rotate(matGlm, glm::radians(2.f), glm::vec3(0.f, 0.f, 1.f));

			CHECK_MAT4(temp, tempGlm);
		}
	}
} // end TEST_CASE("Mat4", "[.all][matrice][Mat4]")