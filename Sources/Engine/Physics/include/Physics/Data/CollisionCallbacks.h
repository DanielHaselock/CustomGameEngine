#pragma once

#include <functional>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <Bullet/BulletDynamics/ConstraintSolver/btBatchedConstraints.h>
#include "Maths/FVector3.h"
using CollisionEnterCallback = std::function<void(void*, void*, btPersistentManifold*)>;
using CollisionExitCallback = std::function<void(void*, void*, btPersistentManifold*)>;

namespace Physics::Data
{
	struct RaycastCallback
	{
		Maths::FVector3 mCollisionPoint;
		Maths::FVector3 mCollisionNormal;
		void* mCollisionObject = nullptr;
	};

	struct CollisionCallbacks
	{
		CollisionEnterCallback enter;
		CollisionEnterCallback stay;
		CollisionExitCallback exit;
		void* physicsObject = nullptr;
		void* rigidbody = nullptr;
	};


}