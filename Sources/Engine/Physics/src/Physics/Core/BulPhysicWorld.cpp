#include "Physics/Core/BulPhysicWorld.h"
#include "Physics/Data/CollisionGroup.h"

using namespace Physics::Core;

PhysicWorld::PhysicWorld() : mDispatcher(&mCollConfig), mWorld(&mDispatcher, &mBroadPhase, &mSolver, &mCollConfig) {}

Physics::Core::PhysicWorld::PhysicWorld(const PhysicWorld& pOther) : mDispatcher(&mCollConfig), mWorld(&mDispatcher, &mBroadPhase, &mSolver, &mCollConfig)
{
	mCollConfig = pOther.mCollConfig;
	mDispatcher = pOther.mDispatcher;
	mBroadPhase = pOther.mBroadPhase;
	mSolver = pOther.mSolver;
	mWorld = pOther.mWorld;
}

PhysicWorld& Physics::Core::PhysicWorld::operator=(const PhysicWorld& pOther)
{
	mCollConfig = pOther.mCollConfig;
	mDispatcher = pOther.mDispatcher;
	mBroadPhase = pOther.mBroadPhase;
	mSolver = pOther.mSolver;
	mWorld = pOther.mWorld;
	return *this;
}

PhysicWorld::~PhysicWorld()
{
}

void PhysicWorld::setGravity(float pX, float pY, float pZ)
{
	mWorld.setGravity(btVector3(pX, pY, pZ));
}

void PhysicWorld::stepSimulation(float pDeltaTime)
{
	mWorld.stepSimulation(pDeltaTime);
}

void PhysicWorld::removeRigidBody(btRigidBody* pCollider)
{
	mWorld.removeRigidBody(pCollider);
}

btDispatcher* PhysicWorld::getDispatcher()
{
	return mWorld.getDispatcher();
}

void PhysicWorld::addCollider(btRigidBody& pCollider)
{
	mWorld.addRigidBody(&pCollider, Data::CollisionGroups::Default, btBroadphaseProxy::AllFilter);
}

void PhysicWorld::removeCollider(btRigidBody& pCollider)
{
	int num = pCollider.getNumConstraintRefs();
	for (int i = 0; i < num; i++)
		mWorld.removeConstraint(pCollider.getConstraintRef(i));

	mWorld.removeRigidBody(&pCollider);
}

void PhysicWorld::setDebugDrawer(btIDebugDraw* pDebugger)
{
	mWorld.setDebugDrawer(pDebugger);
}