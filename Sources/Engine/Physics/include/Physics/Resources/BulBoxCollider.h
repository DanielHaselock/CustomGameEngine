#pragma once
#include "Physics/Resources/IRigidbody.h"
#include "Physics/Core/BulPhysicsEngine.h"
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include "Maths/FVector3.h"
#include "Maths/FQuaternion.h"
#include "Physics/Data/CollisionCallbacks.h"
#include "Physics/Data/TypeRigidBody.h"
#include <Game/Component/ACollider.h>

namespace Physics::Resources
{
	class BulBoxCollider : public IRigidbody
	{
		public:
			
			btBoxShape mBox;
			btDefaultMotionState mMotion;

			Maths::FVector3 mSize;
			Maths::FVector3 mBaseSize;
			Maths::FQuaternion mRotation;

			bool mLockRotX = false;
			bool mLockRotY = false;
			bool mLockRotZ = false;
			
			Data::CollisionCallbacks* mCallback = nullptr;

			BulBoxCollider(Maths::FVector3 pPosition, Maths::FVector3 pSize, void* pActor, bool shouldAddToWorld = true);
			BulBoxCollider(BulBoxCollider& pOther);
			~BulBoxCollider();

			virtual void OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
			virtual void OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
			virtual void OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;

			void setTransform(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pSize);

			void updateTransform();

			void updateTransformWithConstraints(bool pLockRotX, bool pLockRotY, bool pLockRotZ);

			void createCollider();
	};
}