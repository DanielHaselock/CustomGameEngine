#include "Scripting/Binders/LuaMathsBinder.h"

#include "Maths/FVector2.h"
#include "Maths/FVector3.h"
#include "Maths/FVector4.h"
#include "Maths/FQuaternion.h"
#include "Maths/FMatrix3.h"
#include "Maths/FMatrix4.h"

using namespace Scripting::Binder;
using namespace Maths;

void LuaMathsBinder::bind(sol::state& pLuaState)
{
	// Bind the Vector2 class
	// TODO: Fix this
	pLuaState.new_usertype<FVector2>("Vector2",

		/* Constructors */
		sol::constructors<
		FVector2(),
		FVector2(float),
		FVector2(float, float),
		FVector2(FVector2)
		>(),

		/* Operators */
		sol::meta_function::addition, &FVector2::operator+,
		sol::meta_function::subtraction, sol::resolve<FVector2(const FVector2&) const>(&FVector2::operator-),
		sol::meta_function::unary_minus, sol::resolve<FVector2() const>(&FVector2::operator-),
		sol::meta_function::multiplication, sol::resolve<FVector2(const FVector2&) const>(&FVector2::operator*),
		sol::meta_function::division, sol::resolve<FVector2(const FVector2&) const>(&FVector2::operator/),
		// TODO: implement toString

		/* Accessors */
		"x", &FVector2::x,
		"y", &FVector2::y,

		/* Data */
		"One", []() { return FVector2::One; },
		"Zero", []() {return FVector2::Zero; },
		"UnitX", []() {return FVector2::UnitX; },
		"UnitY", []() {return FVector2::UnitY; },

		/* Methods */
		"AngleBetween", &FVector2::angleBetween,
		"Length", sol::resolve<float()>(&FVector2::length),
		"LengthSquared", sol::resolve<float()>(&FVector2::lengthSquared),
		"Dot", sol::resolve<float(const FVector2&)>(&FVector2::dot),
		"Distance", sol::resolve<float(const FVector2&)>(&FVector2::distance),
		"DistanceSquared", sol::resolve<float(const FVector2&)>(&FVector2::distanceSquared),
		"Magnitude", sol::resolve<float()>(&FVector2::magnitude),
		"MagnitudeSquared", sol::resolve<float()>(&FVector2::magnitudeSquared)
		);

	// Bind the Vector3 class
	pLuaState.new_usertype<FVector3>("Vector3",

		/* Constructors */
		sol::constructors<
		FVector3(),
		FVector3(float, float, float),
		FVector3(FVector3)
		>(),

		/* Operators */
		sol::meta_function::addition, &FVector3::operator+,
		sol::meta_function::subtraction, sol::resolve<FVector3(const FVector3&) const>(&FVector3::operator-),
		sol::meta_function::unary_minus, sol::resolve<FVector3() const>(&FVector3::operator-),
		sol::meta_function::multiplication, sol::resolve<FVector3(const FVector3&) const>(&FVector3::operator*),
		sol::meta_function::multiplication, sol::resolve<FVector3(const float) const>(&FVector3::operator*),
		sol::meta_function::division, sol::resolve<FVector3(const FVector3&) const>(&FVector3::operator/),
		sol::meta_function::division, sol::resolve<FVector3(const float) const>(&FVector3::operator/),
		// TODO: implement toString

		/* Accessors */
		"x", &FVector3::x,
		"y", &FVector3::y,
		"z", &FVector3::z,

		/* Data */
		"One", []() { return FVector3::One; },
		"Zero", []() { return FVector3::Zero; },
		"Forward", []() { return FVector3::Forward; },
		"Backward", []() { return FVector3::Backward; },
		"Right", []() { return FVector3::Right; },
		"Left", []() { return FVector3::Left; },
		"Up", []() { return FVector3::Up; },
		"Down", []() { return FVector3::Bottom; },

		/* Methods */
		"Length", sol::resolve<float()>(&FVector3::length),
		"LengthSquared", sol::resolve<float()>(&FVector3::lengthSquared),
		"Dot", sol::resolve<float(const FVector3&)>(&FVector3::dot),
		"Cross", sol::resolve<FVector3(const FVector3&)>(&FVector3::cross),
		"Normalize", sol::resolve<FVector3()>(&FVector3::normalize),
		"Lerp", sol::resolve<FVector3(const FVector3&, float)>(&FVector3::lerp),
		"Distance", sol::resolve<float(const FVector3&)>(&FVector3::distance)
		);

	pLuaState.new_usertype<FVector4>("Vector4",

		/* Constructors */
		sol::constructors<
		FVector4(),
		FVector4(float),
		FVector4(float, float, float, float),
		FVector4(FVector4)
		>(),

		/* Operators */
		sol::meta_function::addition, &FVector4::operator+,
		sol::meta_function::subtraction, sol::resolve<FVector4(const FVector4&) const>(&FVector4::operator-),
		sol::meta_function::unary_minus, sol::resolve<FVector4() const>(&FVector4::operator-),
		sol::meta_function::multiplication, sol::resolve<FVector4(const FVector4&) const>(&FVector4::operator*),
		sol::meta_function::division, sol::resolve<FVector4(const FVector4&) const>(&FVector4::operator/),
		sol::meta_function::to_string, &FVector4::toString,

		/* Accessors */
		"x", &FVector4::x,
		"y", &FVector4::y,
		"z", &FVector4::z,
		"w", &FVector4::w,

		/* Data */
		"One", []() { return FVector4::One; },
		"Zero", []() { return FVector4::Zero; },

		/* Methods */
		"Length", sol::resolve<float()>(&FVector4::length),
		"LengthSquared", sol::resolve<float()>(&FVector4::lengthSquared),
		"Magnitude", sol::resolve<float()>(&FVector4::magnitude),
		"MagnitudeSquared", sol::resolve<float()>(&FVector4::magnitudeSquared),
		"Distance", sol::resolve<float(const FVector4&)>(&FVector4::distance),
		"DistanceSquared", sol::resolve<float(const FVector4&)>(&FVector4::distanceSquared),
		"Dot", sol::resolve<float(const FVector4&)>(&FVector4::dot),
		"Normalize", sol::resolve<FVector4()>(&FVector4::normalize),
		"Lerp", &FVector4::lerp
		);

	// Bind the Quaternion class
	pLuaState.new_usertype<FQuaternion>("Quaternion",

		/* Constructors */
		sol::constructors<
		FQuaternion(), // Empty
		FQuaternion(float), // 1 float
		FQuaternion(float, float, float, float), // 4 floats
		FQuaternion(FQuaternion), // Copy
		FQuaternion(FVector3), // Euler Angle
		FQuaternion(FMatrix3), // Rotation Mat3
		FQuaternion(FMatrix4) // Rotation Mat4
		>(),

		/* Operators */
		sol::meta_function::addition, &FQuaternion::operator+,
		sol::meta_function::subtraction, sol::resolve<FQuaternion(const FQuaternion&) const>(&FQuaternion::operator-),
		sol::meta_function::unary_minus, sol::resolve<FQuaternion() const>(&FQuaternion::operator-),
		sol::meta_function::multiplication, sol::resolve<FQuaternion(const FQuaternion&) const>(&FQuaternion::operator*),
		//sol::meta_function::multiplication, sol::resolve<FVector3(const FVector3&)const>(&FQuaternion::operator*),
		sol::meta_function::to_string, &FQuaternion::toString,

		/* Accessors */
		"x", &FQuaternion::x,
		"y", &FQuaternion::y,
		"z", &FQuaternion::z,
		"w", &FQuaternion::w,

		/* Data */
		"Identity", []() { return FQuaternion::Identity; },

		/* Methods */
		"Length", sol::resolve<float()>(&FQuaternion::length),
		"LengthSquared", sol::resolve<float()>(&FQuaternion::lengthSquared),
		"Dot", sol::resolve<float(const FQuaternion&)>(&FQuaternion::dotProduct),
		"Normalize", sol::resolve<FQuaternion()>(&FQuaternion::normalize),
		"Conjugate", sol::resolve<FQuaternion()>(&FQuaternion::conjugate),
		"Inverse", sol::resolve<FQuaternion()>(&FQuaternion::inverse),
		"Slerp", &FQuaternion::slerp,
		"Nlerp", &FQuaternion::nlerp,
		"EulerAngles", [](FQuaternion& pThis)->Maths::FVector3 {return pThis.eulerAngles(); }
		);

	
}