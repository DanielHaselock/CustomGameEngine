#include "Scripting/Binders/LuaSceneBinder.h"
#include "Game/SceneSys/SceneManager.h"
void Scripting::Binder::LuaSceneBinder::bind(sol::state& pLuaState)
{


	pLuaState.new_usertype<Game::SceneSys::Scene>("Scene",
		"GetActorsByTag", &Game::SceneSys::Scene::getActorsByTag,
		"GetActorByTag", & Game::SceneSys::Scene::getActorByTag

		);
}
