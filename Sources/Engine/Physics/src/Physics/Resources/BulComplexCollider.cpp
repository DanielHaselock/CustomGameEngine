#include "Physics/Resources/BulComplexCollider.h"

Physics::Resources::BulComplexCollider::BulComplexCollider(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pSize, std::list<Game::Component::AComponent*>& pModel, void* pActor, bool pShouldAddToWorld) :
	IRigidbody(pPosition, pShouldAddToWorld), mRotation(pRotation), mSize(pSize), mModel(pModel)
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

Physics::Resources::BulComplexCollider::~BulComplexCollider()
{
	if (!shouldAddToWorld)
		return;
		
	service(Core::BulPhysicsEngine).removeCollider(*mCollider);
	delete mCollider;
}

void Physics::Resources::BulComplexCollider::setTransform(Maths::FVector3 pPosition, Maths::FQuaternion pRotation, Maths::FVector3 pScale)
{
	mPosition = pPosition;
	mRotation = pRotation;
	mSize = pScale;

	if (!shouldAddToWorld)
		return;
	
	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::BulComplexCollider::setModel(std::list<Game::Component::AComponent*>& pModel)
{
	mModel = pModel;
}

void Physics::Resources::BulComplexCollider::updateTransform()
{
	if (!shouldAddToWorld)
		return;
	
	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	delete mCollider;
	createCollider();

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::BulComplexCollider::updateTransformWithConstraints(bool pLockRotX, bool pLockRotY, bool pLockRotZ)
{
	if (!shouldAddToWorld)
		return;

	service(Core::BulPhysicsEngine).removeCollider(*mCollider);

	delete mCollider;
	createCollider();
	mCollider->setAngularFactor(btVector3(pLockRotX ? 0 : 1, pLockRotY ? 0 : 1, pLockRotZ ? 0 : 1));

	service(Core::BulPhysicsEngine).addCollider(*mCollider);
}

void Physics::Resources::BulComplexCollider::createCollider()
{
	if (!shouldAddToWorld)
		return;

	mShape = btConvexHullShape();

	if (mModel.size() == 0)
		return;

	for (Game::Component::AComponent* aModel : mModel)
	{
		Game::Component::CPModel* model = (Game::Component::CPModel*)aModel;

		if (!model->mModel)
			continue;
		
		for (Rendering::Resources::VK::VKMesh* mesh : model->mModel->mMeshes)
		{

			for (int i = 0; i < mesh->mVertices.size(); ++i)
			{
				mShape.addPoint
				(
					btVector3(mesh->mVertices[i].mPosition.x * abs(mSize.x), mesh->mVertices[i].mPosition.y * abs(mSize.y), mesh->mVertices[i].mPosition.z * abs(mSize.z)),
					true
				);
			}


		}
	}

	btVector3 inertia;
	mShape.calculateLocalInertia(0, inertia);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(mPosition.x + mCenter.x, mPosition.y + mCenter.y, mPosition.z + mCenter.z));
	Maths::FMatrix4 mat4 =  Maths::FQuaternion::inverse(mRotation).toMatrix4();
	transform.setBasis(btMatrix3x3(mat4.data[0][0], mat4.data[1][0], mat4.data[2][0], mat4.data[0][1], mat4.data[1][1], mat4.data[2][1], mat4.data[0][2], mat4.data[1][2], mat4.data[2][2]));
	mMotion = btDefaultMotionState(transform);


	btRigidBody::btRigidBodyConstructionInfo info(mMass, &mMotion, &mShape, inertia);

	mCollider = new btRigidBody(info);
	mCollider->setUserPointer(mCallback);
	mType = Data::TypeRigidBody::E_STATIC;

	mCollider->setCollisionFlags((btCollisionObject::CollisionFlags)mType);
}
