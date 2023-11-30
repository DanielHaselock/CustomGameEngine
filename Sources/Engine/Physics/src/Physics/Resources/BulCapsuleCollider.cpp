#include "Physics\Resources\BulCapsuleCollider.h"
#include "EngineCore/Service/ServiceLocator.h"
#include <Bullet/btBulletDynamicsCommon.h>
Physics::Resources::BulCapsuleCollider::BulCapsuleCollider(Maths::FVector3 pPosition, float pRadius, float pHeight, void* pActor, bool pShouldAddToWorld)
	: IRigidbody(pPosition, pShouldAddToWorld), mHeight(pHeight), mRadius(pRadius), mCapsule(mRadius, mHeight)
{	
	if (!shouldAddToWorld)
		return;

	createCollider();
	
	service(Core::BulPhysicsEngine).addCollider(*mCollider);

	std::function<void(void* pRigidbody,void* pOtherCollider, btPersistentManifold* persistent)> collisionEnterCallback = [this](void* pRigidbody, void* pOtherCollider, btPersistentManifold* pPersistent) {
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

Physics::Resources::BulCapsuleCollider::BulCapsuleCollider(BulCapsuleCollider& pOther)
	: IRigidbody(pOther), mHeight(pOther.mHeight), mRadius(pOther.mRadius), mCapsule(pOther.mCapsule), mMotion(pOther.mMotion), mCallback(pOther.mCallback)
{
	mCollider = std::move(pOther.mCollider);
}

Physics::Resources::BulCapsuleCollider::~BulCapsuleCollider()
{
	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);
	mCollider->setUserPointer(nullptr);
	delete mCollider;
}

void Physics::Resources::BulCapsuleCollider::setTransform(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, float pRadius, float pHeight)
{
	mPosition = pPosition;
	mRotation = pRotation;
	mRadius = pRadius;
	mHeight = pHeight;

	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	btVector3 velo = mCollider->getLinearVelocity();

	mCollider->setUserPointer(nullptr);
	delete mCollider;
	createCollider();

	mCollider->setLinearVelocity(velo);

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::BulCapsuleCollider::updateTransform()
{
	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	btVector3 velo = mCollider->getLinearVelocity();

	mCollider->setUserPointer(nullptr);
	delete mCollider;
	createCollider();

	mCollider->setLinearVelocity(velo);

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::BulCapsuleCollider::updateTransformWithConstraints(bool pLockRotX, bool pLockRotY, bool pLockRotZ)
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

void Physics::Resources::BulCapsuleCollider::createCollider()
{
	if (!shouldAddToWorld)
		return;

	btVector3 inertia;
	mCapsule = btCapsuleShape(abs(mRadius), abs(mHeight));
	mCapsule.calculateLocalInertia(mMass, inertia);

	Maths::FVector3& centerBaseRotation = mCenter;// ((mRotation * Maths::FVector3::Right) * mCenter) + ((mRotation * Maths::FVector3::Up) * mCenter) + ((mRotation * Maths::FVector3::Forward) * mCenter);


	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(mPosition.x + centerBaseRotation.x, mPosition.y + centerBaseRotation.y, mPosition.z + centerBaseRotation.z));
	Maths::FMatrix4 mat4 = mRotation.toMatrix4();
	transform.setBasis(btMatrix3x3(mat4.data[0][0], mat4.data[1][0], mat4.data[2][0], mat4.data[0][1], mat4.data[1][1], mat4.data[2][1], mat4.data[0][2], mat4.data[1][2], mat4.data[2][2]));
	mMotion = btDefaultMotionState(transform);
	
	
	btRigidBody::btRigidBodyConstructionInfo info(mMass, &mMotion, &mCapsule, inertia);

	mCollider = new btRigidBody(info);
	mCollider->setUserPointer(mCallback);
	if (mType == Data::TypeRigidBody::E_TRIGGER)
		mCollider->setCollisionFlags(btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
	else
		mCollider->setCollisionFlags(mCollider->getCollisionFlags() | (btCollisionObject::CollisionFlags)mType);

	mCollider->setAngularFactor(btVector3(mLockRotX ? 0 : 1, mLockRotY ? 0 : 1, mLockRotZ ? 0 : 1));
}
