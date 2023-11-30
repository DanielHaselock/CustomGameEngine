#pragma once
#include <sol.hpp>
namespace Scripting
{
	namespace Binder
	{
		class LuaSceneBinder
		{
		public:
			void bind(sol::state& pLuaState);
		};

	}
}