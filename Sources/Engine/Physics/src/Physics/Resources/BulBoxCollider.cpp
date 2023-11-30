#include "Physics/Resources/BulBoxCollider.h"
#include "EngineCore/Service/ServiceLocator.h"
#include <Bullet/btBulletDynamicsCommon.h>

using namespace Physics::Resources;

BulBoxCollider::BulBoxCollider(Maths::FVector3 pPosition, Maths::FVector3 pSize, void* pActor, bool pShouldAddToWorld)
	: IRigidbody(pPosition, pShouldAddToWorld), mBaseSize(pSize), mBox(btVector3(mSize.x, mSize.y, mSize.z))
{
	if (!shouldAddToWorld)
		return;

	createCollider();
	service(Core::BulPhysicsEngine).addCollider(*mCollider);

	std::function<void(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)> collisionEnterCallback = [this](void* pRigidbody, void* pOtherCollider, btPersistentManifold* pPersistent) {
		OnAnyCollisionEnter(pRigidbody, pOtherCollider, pPersistent);
	};

	std::function<void(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)> collisionStayCallback = [this](void* pRigidbody, void* pOtherCollider, btPersistentManifold* pPersistent) {
		OnAnyCollisionStay(pRigidbody, pOtherCollider, pPersistent);
	};

	std::function<void(void* pRigidbody, void* pOtherCollider, btPersistentManifold* manifold)> collisionExitCallback = [this](void* pRigidbody, void* pOtherCollider, btPersistentManifold* pPersistent) {
		OnAnyCollisionExit(pRigidbody, pOtherCollider, pPersistent);
	};

	mCallback = new Data::CollisionCallbacks();
	mCallback->physicsObject = pActor;
	mCallback->enter = collisionEnterCallback;
	mCallback->stay = collisionStayCallback;
	mCallback->exit = collisionExitCallback;
	mCallback->rigidbody = this;
}

BulBoxCollider::BulBoxCollider(BulBoxCollider& pOther)
	: IRigidbody(pOther), mBaseSize(pOther.mBaseSize), mBox(pOther.mBox), mMotion(pOther.mMotion), mCallback(pOther.mCallback)
{
	mCollider = std::move(pOther.mCollider);
}

BulBoxCollider::~BulBoxCollider()
{
	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);
	mCollider->setUserPointer(nullptr);
	delete mCollider;
}

void BulBoxCollider::setTransform(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pSize)
{
	mPosition = pPosition;
	mRotation = pRotation;
	mSize = mBaseSize * pSize;

	if (!shouldAddToWorld)
		return;
	
	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	mCollider->setUserPointer(nullptr);
	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void BulBoxCollider::updateTransform()
{
	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	mCollider->setUserPointer(nullptr);
	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::BulBoxCollider::updateTransformWithConstraints(bool pLockRotX, bool pLockRotY, bool pLockRotZ)
{
	mLockRotX = pLockRotX;
	mLockRotY = pLockRotY;
	mLockRotZ = pLockRotZ;

	if (!shouldAddToWorld)
		return;
	
	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	mCollider->setUserPointer(nullptr);
	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void BulBoxCollider::createCollider()
{
	if (!shouldAddToWorld)
		return;

	btVector3 inertia;
	mBox = btBoxShape(btVector3(abs(mSize.x) / 2, abs(mSize.y) / 2, abs(mSize.z) / 2));
	mBox.calculateLocalInertia(mMass, inertia);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(mPosition.x + mCenter.x, mPosition.y + mCenter.y, mPosition.z + mCenter.z));
	transform.setRotation(btQuaternion(mRotation.x, mRotation.y, mRotation.z, mRotation.w));
	mMotion = btDefaultMotionState(transform);
	
	btRigidBody::btRigidBodyConstructionInfo info(mMass, &mMotion, &mBox, inertia);

	mCollider = new btRigidBody(info);

	mCollider->setUserPointer(mCallback);
	if (mType == Data::TypeRigidBody::E_TRIGGER)
		mCollider->setCollisionFlags(btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
	else
		mCollider->setCollisionFlags(mCollider->getCollisionFlags() | (btCollisionObject::CollisionFlags)mType);

	mCollider->setAngularFactor(btVector3(mLockRotX ? 0 : 1, mLockRotY ? 0 : 1, mLockRotZ ? 0 : 1));
}