#pragma once
#include "Maths/FVector3.h"
#include "Maths/FMatrix4.h"
#include "Physics/Data/TypeRigidBody.h"
#include "Physics/Core/BulPhysicsEngine.h"
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include "Maths/FVector3.h"
#include "Maths/FQuaternion.h"
#include "Physics/Data/CollisionCallbacks.h"

namespace Physics::Resources
{
	class IRigidbody
	{
	public:
		btRigidBody* mCollider = nullptr;
		Maths::FVector3 mPosition;
		Maths::FVector3 mCenter;
		Data::TypeRigidBody mType = Data::TypeRigidBody::E_DYNAMIC;
		float mMass = 10;
		bool shouldAddToWorld;

		IRigidbody(Maths::FVector3 pPosition, bool pShouldAddToWorld = true);

		IRigidbody(const IRigidbody& pOther);

		Maths::FVector3 getPositionFromPhysics(); // From Bullet -> Gameobject

		Maths::FQuaternion getRotationFromPhysics(); // From Bullet -> Gameobject

		void setType(Data::TypeRigidBody pType);

		void setMass(double pValue);

		virtual void createCollider() = 0;

		void addForcePhysics(Maths::FVector3 pForce);

		void clearForcesPhysics();

		void setLinearVelocityPhysics(Maths::FVector3 pVelocity);

		void setAngularVelocityPhysics(Maths::FVector3 pVelocity);

		Maths::FVector3 getLinearVelocityPhysics();

		Maths::FVector3 getAngularVelocityPhysics();

		void clearLinearVelocityPhysics();
		void clearAngularVelocityPhysics();
	};
}