#pragma once
#include "ACollider.h"
#include "Physics/Resources/bulBoxCollider.h"

namespace Game::Component
{
	class CPBoxCollider : public ACollider, public Physics::Resources::BulBoxCollider
	{
		public:
			CPBoxCollider(Maths::FVector3 pPosition, Maths::FVector3 pSize, void* pActor, bool shouldAddToWorld = true);
			CPBoxCollider(const CPBoxCollider& pCollider);

			void setPhysicsObject(void* pActor);
			void UpdateTransform(Maths::FTransform& pTransform) override;
			void UpdateRotation(Maths::FTransform& pTransform) override;
			void recreateCollider() override;
			Maths::FVector3 getPhysicsPosition() override;
			Maths::FQuaternion getPhysicsRotation() override;

			bool& getLockX() { return mLockRotX; }
			bool& getLockY() { return mLockRotY; }
			bool& getLockZ() { return mLockRotZ; }

			AComponent* clone() override;
			~CPBoxCollider() override ;

			Physics::Data::TypeRigidBody getType() override { return mType; };


			void OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) override;
			void OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) override;
			void OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent = nullptr) override;

			void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;

			virtual void addForce(Maths::FVector3 Force) override;

			virtual void clearForces() override;

			virtual void setLinearVelocity(Maths::FVector3 pVelocity) override;
			virtual void setAngularVelocity(Maths::FVector3 pVelocity) override;


			virtual Maths::FVector3 getAngularVelocity() override;

			virtual Maths::FVector3 getLinearVelocity() override;


			virtual void clearLinearVelocity() override;
			virtual void clearAngularVelocity() override;
	};
}

