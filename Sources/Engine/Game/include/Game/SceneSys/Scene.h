#pragma once
#include "Game/Data/Actor.h"
#include "Game/Component/CPLight.h"
#include <list>

namespace Game::SceneSys
{
	class Scene
	{
		public:
			std::list<Data::Actor*> mActors;
			std::list<Component::CPLight*> mLights;
			std::mutex mMutex;

			Scene() = default;
			void clear();
			~Scene();
			void addActor(Data::Actor* mActor);
			void removeActor(Data::Actor* mActor);
			void removeRecursiveActor(Data::Actor* mActor);

			std::vector<Data::Actor*> getActorsByTag(std::string pTag);
			Data::Actor* getActorByTag(std::string pTag);
	};
}