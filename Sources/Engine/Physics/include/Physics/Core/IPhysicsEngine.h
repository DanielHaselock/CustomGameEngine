#pragma once

namespace Physics::Core
{
	class IPhysicsEngine
	{
		virtual void update(float pDeltaTime) = 0;
		virtual void reset() = 0;
		virtual void checkForCollisionEvents() = 0;
	}; 
}