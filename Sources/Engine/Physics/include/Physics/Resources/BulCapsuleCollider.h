#pragma once
#include "Physics/Data/TypeRigidBody.h"
#include "Physics/Resources/IRigidbody.h"
#include "Physics/Core/BulPhysicsEngine.h"
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include "Maths/FVector3.h"
#include "Maths/FQuaternion.h"
#include "Physics/Data/CollisionCallbacks.h"
namespace Physics::Resources
{
	class BulCapsuleCollider : public IRigidbody
	{
	public:

		btCapsuleShape mCapsule;
		btDefaultMotionState mMotion;
		float mRadius;
		float mHeight;
		Maths::FQuaternion mRotation;

		bool mLockRotX = false;
		bool mLockRotY = false;
		bool mLockRotZ = false;

		Data::CollisionCallbacks* mCallback = nullptr;

		BulCapsuleCollider(Maths::FVector3 pPosition, float pRadius, float pHeight, void* pActor, bool shouldAddToWorld = true);
		BulCapsuleCollider(BulCapsuleCollider& pOther);
		~BulCapsuleCollider();

		void setTransform(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, float pRadius, float pHeight);

		void updateTransform();

		void updateTransformWithConstraints(bool pLockRotX, bool pLockRotY, bool pLockRotZ);

		void createCollider();

		virtual void OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
		virtual void OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
		virtual void OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
	};

}