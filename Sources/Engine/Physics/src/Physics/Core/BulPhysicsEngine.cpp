#include "Physics/Core/BulPhysicsEngine.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Physics/Data/CollisionCallbacks.h"
#include "Physics/Data/CollisionGroup.h"
#include <algorithm>
#include <iterator>


using namespace Physics::Core;

BulPhysicsEngine::BulPhysicsEngine()
{
	setGravity(0, -EARTH_GRAVITY, 0);
	//mCollisionPairs = new CollisionSet();
}

BulPhysicsEngine::~BulPhysicsEngine()
{
	delete mPhysicsDrawer;
}

void BulPhysicsEngine::update(float pDeltaTime)
{
	if (pDeltaTime == 0)
		return;

	std::unique_lock lock(mMutex);
	stepSimulation(pDeltaTime);
	checkForCollisionEvents();
}

void BulPhysicsEngine::checkForCollisionEvents()
{
	btDispatcher* dispatcher = getDispatcher();
	if (!dispatcher)
		return;

	CollisionSet NewCollisions;
	int numManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);

		if (!contactManifold || contactManifold->getNumContacts() <= 0)
			continue;

		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		bool const Swapped = obA > obB;
		const btCollisionObject* RigidA = Swapped ? obA : obB;
		const btCollisionObject* RigidB = Swapped ? obB : obA;

		NewCollisions.emplace(RigidA, RigidB, contactManifold);
	}

	CollisionSet differenceSet;
	std::set_symmetric_difference(mCollisionPairs.begin(), mCollisionPairs.end(), NewCollisions.begin(), NewCollisions.end(), std::inserter(differenceSet, differenceSet.begin()));

	//CollisionSet stayColision;
	//std::set_union((*mCollisionPairs).begin(), (*mCollisionPairs).end(), NewCollisions.begin(), NewCollisions.end(), std::inserter(stayColision, stayColision.begin()));
	//for (std::tuple<const btCollisionObject*, const btCollisionObject*, const btPersistentManifold*> const& pair : stayColision)
	//{
	//	Data::CollisionCallbacks* Callbacks = (Data::CollisionCallbacks*)std::get<0>(pair)->getUserPointer();
	//	Data::CollisionCallbacks* Callbacks2 = (Data::CollisionCallbacks*)std::get<1>(pair)->getUserPointer();
	//	btRigidBody* body0 = (btRigidBody*)std::get<0>(pair);
	//	btRigidBody* body1 = (btRigidBody*)std::get<1>(pair);
	//	btPersistentManifold* manifold = (btPersistentManifold*)std::get<2>(pair);
	//	if (Callbacks && Callbacks2->rigidbody)
	//	{
	//		if (Callbacks->stay)
	//			Callbacks->stay(Callbacks2->rigidbody, Callbacks2->physicsObject, manifold);
	//	}

	//	if (Callbacks2 && Callbacks->rigidbody)
	//	{
	//		if (Callbacks2->stay)
	//			Callbacks2->stay(Callbacks->rigidbody, Callbacks->physicsObject, manifold);
	//	}
	//}

	for (std::tuple<const btCollisionObject*, const btCollisionObject*, const btPersistentManifold*> const& pair : differenceSet)
	{
		if (mCollisionPairs.find(pair) == mCollisionPairs.end())
		{
			Data::CollisionCallbacks* Callbacks = (Data::CollisionCallbacks*)std::get<0>(pair)->getUserPointer();
			Data::CollisionCallbacks* Callbacks2 = (Data::CollisionCallbacks*)std::get<1>(pair)->getUserPointer();
			btRigidBody* body0 = (btRigidBody*)std::get<0>(pair);
			btRigidBody* body1 = (btRigidBody*)std::get<1>(pair);
			btPersistentManifold* manifold = (btPersistentManifold*)std::get<2>(pair);

			if (Callbacks == nullptr || Callbacks2 == nullptr)
				continue;

			if (Callbacks->enter)
				Callbacks->enter(Callbacks2->rigidbody, Callbacks2->physicsObject, manifold);

			if (Callbacks2->enter)
				Callbacks2->enter(Callbacks->rigidbody, Callbacks->physicsObject, manifold);

			mCollisionPairs.emplace(pair);
		}
		else
		{
			Data::CollisionCallbacks* Callbacks = (Data::CollisionCallbacks*)std::get<0>(pair)->getUserPointer();
			Data::CollisionCallbacks* Callbacks2 = (Data::CollisionCallbacks*)std::get<1>(pair)->getUserPointer();
			btRigidBody* body0 = (btRigidBody*)std::get<0>(pair);
			btRigidBody* body1 = (btRigidBody*)std::get<1>(pair);
			btPersistentManifold* manifold = (btPersistentManifold*)std::get<2>(pair);

			if (Callbacks == nullptr || Callbacks2 == nullptr)
				continue;

			if (Callbacks->exit)
				Callbacks->exit(Callbacks2->rigidbody, Callbacks2->physicsObject, manifold);

			if (Callbacks2->exit)
				Callbacks2->exit(Callbacks->rigidbody, Callbacks->physicsObject, manifold);
		}
	}
	mCollisionPairs = NewCollisions;
}

void BulPhysicsEngine::addCollider(btRigidBody& pCollider)
{
	std::unique_lock lock(mMutex);
	PhysicWorld::addCollider(pCollider);
	pCollider.setActivationState(DISABLE_DEACTIVATION);

	mColliders.push_back(&pCollider);
}

void BulPhysicsEngine::removeCollider(btRigidBody& pCollider)
{
	std::unique_lock lock(mMutex);
	CollisionSet::iterator it = mCollisionPairs.begin();
	for (it; it != mCollisionPairs.end(); ++it)
	{
		if (std::get<0>(*it) == &pCollider || std::get<1>(*it) == &pCollider)
		{
			mCollisionPairs.erase(it);
			return removeCollider(pCollider);
		}
	}

	PhysicWorld::removeCollider(pCollider);


	mColliders.remove(&pCollider);
}

void BulPhysicsEngine::createDebugDrawer(void* pRenderer)
{
	mPhysicsDrawer = new Renderer::PhysicsDebugDrawer(pRenderer);
	setDebugDrawer(mPhysicsDrawer);
}

Physics::Data::RaycastCallback Physics::Core::BulPhysicsEngine::Raycast(Maths::FVector3 pStartPos, Maths::FVector3 pEndPos, CollisionGroups pGroups, int pLayermask)
{
	btVector3 startPos(pStartPos.x, pStartPos.y, pStartPos.z);
	btVector3 endPos(pEndPos.x, pEndPos.y, pEndPos.z);

	btCollisionWorld::ClosestRayResultCallback RayCallback(startPos, endPos);
	RayCallback.m_collisionFilterGroup = (int)pGroups;
	RayCallback.m_collisionFilterMask = pLayermask;
	service(Physics::Core::BulPhysicsEngine).mWorld.rayTest(startPos, endPos, RayCallback);

	if (service(Physics::Core::BulPhysicsEngine).mWorld.getDebugDrawer())
		service(Physics::Core::BulPhysicsEngine).mWorld.getDebugDrawer()->drawLine(startPos, endPos, btVector3(1, 0, 0));
	
	if (RayCallback.m_collisionObject == nullptr)
		return Physics::Data::RaycastCallback{};

	if (!RayCallback.hasHit())
	{
		return Physics::Data::RaycastCallback{};
	}
	Physics::Data::CollisionCallbacks* Callbacks = (Physics::Data::CollisionCallbacks*)RayCallback.m_collisionObject->getUserPointer();

	Maths::FVector3 ColPoint(RayCallback.m_hitPointWorld.x(), RayCallback.m_hitPointWorld.y(), RayCallback.m_hitPointWorld.z());
	Maths::FVector3 ColNormal(RayCallback.m_hitNormalWorld.x(), RayCallback.m_hitNormalWorld.y(), RayCallback.m_hitNormalWorld.z());
	void* ColObject = Callbacks->physicsObject;
	
	return Physics::Data::RaycastCallback{ ColPoint, ColNormal, ColObject };
}

void BulPhysicsEngine::reset()
{
	for (btRigidBody* collider : mColliders)
		removeRigidBody(collider);

	mColliders.clear();
	mCollisionPairs.clear();
}

void BulPhysicsEngine::debugDrawWorld(Maths::FMatrix4& pViewProj)
{
	std::unique_lock lock(mMutex);
	mPhysicsDrawer->updateViewProj(pViewProj);
	mWorld.debugDrawWorld();
}