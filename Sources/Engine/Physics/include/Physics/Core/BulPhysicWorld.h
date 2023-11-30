#pragma once

#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <Bullet/BulletDynamics/ConstraintSolver/btBatchedConstraints.h>

namespace Physics::Core
{
	struct PhysicWorld
	{
		btDefaultCollisionConfiguration mCollConfig;
		btCollisionDispatcher mDispatcher;
		btDbvtBroadphase mBroadPhase;
		btSequentialImpulseConstraintSolver mSolver;
		btDiscreteDynamicsWorld mWorld;

		PhysicWorld();
		PhysicWorld(const PhysicWorld& pOther);
		PhysicWorld& operator=(const PhysicWorld& pOther);
		virtual ~PhysicWorld();
		void setGravity(float pX, float pY, float pZ);
		void stepSimulation(float pDeltaTime);
		void removeRigidBody(btRigidBody* pCollider);
		btDispatcher* getDispatcher();
		virtual void addCollider(btRigidBody& pCollider);
		virtual void removeCollider(btRigidBody& pCollider);
		void setDebugDrawer(btIDebugDraw* pDebugger);
	};
}