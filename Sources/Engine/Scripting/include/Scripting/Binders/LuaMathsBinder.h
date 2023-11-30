#pragma once

#include <sol.hpp>

namespace Scripting
{
	namespace Binder
	{
		class LuaMathsBinder
		{
		public:
			void bind(sol::state& pLuaState);
		};
	}
}