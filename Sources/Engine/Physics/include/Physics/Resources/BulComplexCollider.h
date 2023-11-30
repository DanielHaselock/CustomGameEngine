#pragma once
#include "Physics/Data/TypeRigidBody.h"
#include "Physics/Resources/IRigidbody.h"
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include "Game/Component/CPModel.h"
namespace Physics::Resources
{
	class BulComplexCollider : public IRigidbody
	{
	public:

		btConvexHullShape mShape;
		btDefaultMotionState mMotion;
		Maths::FQuaternion mRotation;
		Maths::FVector3 mSize;

		Data::CollisionCallbacks* mCallback = nullptr;

		std::list<Game::Component::AComponent*>& mModel;

		BulComplexCollider(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pSize, std::list<Game::Component::AComponent*>& pModel, void* pActor, bool shouldAddToWorld = true);
		//BulComplexCollider(BulComplexCollider& pOther);
		~BulComplexCollider();

		void setTransform(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pScale);

		void setModel(std::list<Game::Component::AComponent*>& pModel);

		void updateTransform();

		void updateTransformWithConstraints(bool pLockRotX, bool pLockRotY, bool pLockRotZ);

		void createCollider();

		virtual void OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
		virtual void OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
		virtual void OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) = 0;
	};

}

