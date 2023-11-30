#include "Physics/Resources/IRigidbody.h"
#include <Bullet/LinearMath/btScalar.h>
#include <Bullet/btBulletDynamicsCommon.h>
#include <EngineCore/Service/ServiceLocator.h>

using namespace Physics::Resources;

IRigidbody::IRigidbody(Maths::FVector3 pPosition, bool pShouldAddToWorld) 
	: mPosition(pPosition), shouldAddToWorld(pShouldAddToWorld)
{
}

IRigidbody::IRigidbody(const IRigidbody& pOther) 
	: mType(pOther.mType), mCenter(pOther.mCenter), mMass(pOther.mMass), 
	mPosition(pOther.mPosition), shouldAddToWorld(pOther.shouldAddToWorld)
{
}

Maths::FVector3 IRigidbody::getPositionFromPhysics()
{
	btTransform transform;
	btScalar worldmatrix[16]{};
	transform = mCollider->getWorldTransform();
	mPosition = Maths::FVector3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());
	return mPosition - mCenter;
}

Maths::FQuaternion IRigidbody::getRotationFromPhysics()
{
	btTransform transform;
	btScalar worldmatrix[16]{};
	transform = mCollider->getWorldTransform();
	return Maths::FQuaternion(transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w());
}

void IRigidbody::setType(Data::TypeRigidBody pType)
{
	mType = pType;

	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void IRigidbody::setMass(double pValue)
{
	mMass = pValue;

	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::IRigidbody::addForcePhysics(Maths::FVector3 pForce)
{
	if (mCollider)
	{
		mCollider->applyForce(btVector3(pForce.x, pForce.y, pForce.z), btVector3(0,0,0));
	}
}

void Physics::Resources::IRigidbody::clearForcesPhysics()
{
	if (mCollider)
	{
		mCollider->clearForces();
	}
}

void Physics::Resources::IRigidbody::setLinearVelocityPhysics(Maths::FVector3 pVelocity)
{
	btVector3 linearVel = mCollider->getLinearVelocity();
	mCollider->setLinearVelocity(btVector3(pVelocity.x, pVelocity.y + linearVel.y(), pVelocity.z));
}

void Physics::Resources::IRigidbody::setAngularVelocityPhysics(Maths::FVector3 pVelocity)
{
	btVector3 AngularVel = mCollider->getAngularVelocity();

	mCollider->setAngularVelocity(AngularVel + btVector3(pVelocity.x, pVelocity.y, pVelocity.z));
}

Maths::FVector3 Physics::Resources::IRigidbody::getLinearVelocityPhysics()
{
	btVector3 linearVel = mCollider->getLinearVelocity();
	return Maths::FVector3(linearVel.x(), linearVel.y(), linearVel.z());
}

Maths::FVector3 Physics::Resources::IRigidbody::getAngularVelocityPhysics()
{
	btVector3 AngVel = mCollider->getAngularVelocity();
	return Maths::FVector3(AngVel.x(), AngVel.y(), AngVel.z());
}

void Physics::Resources::IRigidbody::clearLinearVelocityPhysics()
{
	btVector3 linearVel = mCollider->getLinearVelocity();
	mCollider->setLinearVelocity(btVector3(0, linearVel.y(), 0));
	mCollider->setAngularVelocity(btVector3(0, 0, 0));
}

void Physics::Resources::IRigidbody::clearAngularVelocityPhysics()
{
	btVector3 AngVel = mCollider->getAngularVelocity();
	mCollider->setAngularVelocity(btVector3(0, 0, 0));
}
