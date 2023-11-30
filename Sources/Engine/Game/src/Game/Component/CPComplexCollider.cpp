#include "Game/Component/CPComplexCollider.h"
#include <Game/Utils/ComponentType.h>

using namespace Game::Component;

CPComplexCollider::CPComplexCollider(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pScale, std::list<Game::Component::AComponent*>& pModel, void* pActor, bool shouldAddToWorld)
	: BulComplexCollider(pPosition, pRotation, pScale, pModel, pActor, shouldAddToWorld)
{
}

CPComplexCollider::CPComplexCollider(const CPComplexCollider& pCollider)
	: BulComplexCollider(pCollider.mPosition, pCollider.mRotation, 
		pCollider.mSize, pCollider.mModel, mOwner, true)
{
	
}

AComponent* Game::Component::CPComplexCollider::clone()
{
	return new CPComplexCollider(*this);
}

CPComplexCollider::~CPComplexCollider()
{
}

void CPComplexCollider::setPhysicsObject(void* pActor)
{
	mCallback->physicsObject = pActor;
}

void Game::Component::CPComplexCollider::UpdateTransform(Maths::FTransform& pTransform)
{
	setTransform(pTransform.mWorldPosition, pTransform.mWorldRotation, pTransform.mWorldScale);
}

void Game::Component::CPComplexCollider::UpdateRotation(Maths::FTransform& pTransform)
{
	setTransform(pTransform.mWorldPosition, pTransform.mWorldRotation, pTransform.mWorldScale);
}

void Game::Component::CPComplexCollider::recreateCollider()
{
	updateTransformWithConstraints(true, true, true);
}

Maths::FVector3 Game::Component::CPComplexCollider::getPhysicsPosition()
{
	return getPositionFromPhysics();
}

Maths::FQuaternion Game::Component::CPComplexCollider::getPhysicsRotation()
{
	Maths::FQuaternion RotatedQuat = getRotationFromPhysics();
	Maths::FVector3 Angles = RotatedQuat.eulerAngles();

	return Maths::FQuaternion(Angles);
}

void Game::Component::CPComplexCollider::OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
	if (!isActive)
		return;

	IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

	if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
		ACollider::OnTriggerEnter(pOtherCollider, (ACollider*)pRigidbody);
	else
		ACollider::OnCollisionEnter(pOtherCollider, (ACollider*)pOtherCollider);
}

void Game::Component::CPComplexCollider::OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
	if (!isActive)
		return;

	IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

	if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
		ACollider::OnTriggerStay(pOtherCollider, (ACollider*)pOtherCollider);
	else
		ACollider::OnCollisionStay(pOtherCollider, (ACollider*)pOtherCollider);
}

void Game::Component::CPComplexCollider::OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
	if (!isActive)
		return;

	IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

	if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
		ACollider::OnTriggerExit(pOtherCollider, (ACollider*)pOtherCollider);
	else
		ACollider::OnCollisionExit(pOtherCollider, (ACollider*)pOtherCollider);
}

void Game::Component::CPComplexCollider::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::ComplexCollider);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Mass");
	pWriter.Double(mMass);

	pWriter.EndObject();
}

void Game::Component::CPComplexCollider::addForce(Maths::FVector3 Force)
{
	addForcePhysics(Force);
}

void Game::Component::CPComplexCollider::clearForces()
{
	clearForcesPhysics();
}

void Game::Component::CPComplexCollider::setLinearVelocity(Maths::FVector3 pVelocity)
{
	setLinearVelocityPhysics(pVelocity);
}

void Game::Component::CPComplexCollider::setAngularVelocity(Maths::FVector3 pVelocity)
{
	setAngularVelocityPhysics(pVelocity);
}

Maths::FVector3 Game::Component::CPComplexCollider::getAngularVelocity()
{
	return getAngularVelocityPhysics();
}

Maths::FVector3 Game::Component::CPComplexCollider::getLinearVelocity()
{
	return getLinearVelocityPhysics();
}

void Game::Component::CPComplexCollider::clearLinearVelocity()
{
	clearLinearVelocityPhysics();
}

void Game::Component::CPComplexCollider::clearAngularVelocity()
{
	return clearAngularVelocityPhysics();
}
