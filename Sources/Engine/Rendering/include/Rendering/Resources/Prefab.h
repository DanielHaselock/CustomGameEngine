#pragma once
#include "Rendering/Resources/IResource.h"
#include <Game/Data/Actor.h>

namespace Rendering::Resources
{
	class Prefab : public IResource
	{
	public:
		Game::Data::Actor* mActor = nullptr;

		std::string mFilename;

		Prefab(const std::string& pFileName);
		Game::Data::Actor* spawnInstance();
		static void addToScene(Game::Data::Actor* pActor);
		static void addToEditor(Game::Data::Actor* pActor);
		~Prefab() override { delete mActor; }
	};
}