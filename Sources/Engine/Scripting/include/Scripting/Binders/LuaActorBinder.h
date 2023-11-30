#include <sol.hpp>

namespace Scripting
{
	namespace Binder
	{
		class LuaActorBinder
		{
		public:
			void bind(sol::state& pLuaState);
		};
	}
}