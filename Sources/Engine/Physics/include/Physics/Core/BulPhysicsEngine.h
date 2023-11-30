#pragma once
#include "Physics/Core/IPhysicsEngine.h"
#include "Maths/FVector3.h"
#include "Physics/Data/CollisionCallbacks.h"
#include "Physics/Core/BulPhysicWorld.h"
#include <set>
#include <mutex>
#include "Physics/Renderer/PhysicsDebugDrawer.h"

namespace Physics::Core
{
	enum class CollisionGroups
	{
		Default = 1
	};

	const float	EARTH_GRAVITY = 9.807f;

	using CollisionSet = std::set<std::tuple<const btCollisionObject*, const btCollisionObject*, const btPersistentManifold*>>;

	class BulPhysicsEngine : public IPhysicsEngine, public PhysicWorld
	{
	public:
		std::recursive_mutex mMutex;
		Renderer::PhysicsDebugDrawer* mPhysicsDrawer = nullptr;

		std::list<btRigidBody*> mColliders;
		CollisionSet mCollisionPairs;

		BulPhysicsEngine();
		virtual ~BulPhysicsEngine() override;

		void update(float pDeltaTime) override;
		void checkForCollisionEvents() override;

		void debugDrawWorld(Maths::FMatrix4& pViewProj);

		void addCollider(btRigidBody& pCollider) override;
		void removeCollider(btRigidBody& pCollider) override;

		void createDebugDrawer(void* pRenderer);

		static Physics::Data::RaycastCallback Raycast(Maths::FVector3 pStartPos, Maths::FVector3 pEndPos, CollisionGroups pGroups = CollisionGroups::Default, int pLayermask = -1); //Returns the object that the raycast touches

		void reset() override;
	};
}