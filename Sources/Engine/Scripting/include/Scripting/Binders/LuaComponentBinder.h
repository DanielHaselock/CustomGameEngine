#pragma once

#include <sol.hpp>

namespace Game::Component
{
	class ACollider;
}

namespace Scripting
{
	namespace Binder
	{
		class LuaPhysicsDelegationFunctions
		{
		public:
			std::function<void(void*)> OnCollisionEnter;
			std::function<void(void*)> OnTriggerEnter;
			std::function<void(void*)> OnCollisionStay;
			std::function<void(void*)> OnTriggerStay;
			std::function<void(void*)> OnCollisionExit;
			std::function<void(void*)> OnTriggerExit;
		};

		class LuaComponentBinder
		{
		public:
			void bind(sol::state& pLuaState);
			void DelegatePhysics(Game::Component::ACollider* pCollider, sol::state& pLuaState, std::string DelegateString);

			bool DelegateFromString(sol::state& pLuaState, std::string DelegateString);

			//LuaPhysicsDelegationFunctions mPhysicsFunctions;

			std::function<void(void*)>& getFunctionFromString(Game::Component::ACollider* pCollider, std::string pInput);

			sol::state* mLuaState;
		};

	}
}