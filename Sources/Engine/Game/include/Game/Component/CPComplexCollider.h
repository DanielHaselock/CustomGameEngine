#pragma once
#include "ACollider.h"
#include "Physics/Resources/BulComplexCollider.h"
namespace Game::Component
{
	class CPComplexCollider : public ACollider, public Physics::Resources::BulComplexCollider
	{
	public:
		CPComplexCollider(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pScale, std::list<Game::Component::AComponent*>& pModel, void* pActor, bool shouldAddToWorld = true);
		CPComplexCollider(const CPComplexCollider& pCollider);
		
		void setPhysicsObject(void* pActor);
		void UpdateTransform(Maths::FTransform& pTransform) override;
		void UpdateRotation(Maths::FTransform& pTransform) override;
		void recreateCollider() override;
		Maths::FVector3 getPhysicsPosition() override;
		Maths::FQuaternion getPhysicsRotation() override;
		AComponent* clone() override;
		~CPComplexCollider() override;

		Physics::Data::TypeRigidBody getType() override { return mType; };


		void OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold * persistent = nullptr) override;
		void OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold * persistent = nullptr) override;
		void OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold * persistent = nullptr) override;

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>&pWriter) override;

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
