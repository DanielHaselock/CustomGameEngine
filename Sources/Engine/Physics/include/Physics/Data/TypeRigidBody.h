#pragma once

namespace Physics::Data
{
	enum class TypeRigidBody
	{
		E_DYNAMIC = 0,
		E_STATIC = 1,
		E_KINEMATIC = 2,
		E_TRIGGER = 4
	};
}