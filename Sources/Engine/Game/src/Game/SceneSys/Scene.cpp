#include "Game/SceneSys/Scene.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Core/EngineApp.h"
#ifdef NSHIPPING
#include "Editor/Widget/WidgetSceneApp.h"
#endif

using namespace Game::SceneSys;

Game::SceneSys::Scene::~Scene()
{
	for (auto actor : mActors)
		delete actor;
}

void Game::SceneSys::Scene::clear()
{
	for (auto actor : mActors)
		delete actor;
	mActors.clear();
}

void Scene::addActor(Data::Actor* mActor)
{
	service(EngineCore::Core::EngineApp).mainThreadAction.pushFunction([this, mActor]
		{
		mActors.push_back(mActor); 
		});
}

void Scene::removeActor(Data::Actor* mActor)
{
	service(EngineCore::Core::EngineApp).mainThreadAction.pushFunction([this, mActor]
	{
		service(EngineCore::Core::EngineApp).rend->waitForCleanUp();
		mActors.remove(mActor); 

#ifdef NSHIPPING
		service(Editor::Widget::WidgetSceneApp).mainThreadAction.pushFunction([mActor]
			{
				service(Editor::Widget::WidgetSceneApp).renderer->waitForCleanUp();
				service(Editor::Widget::WidgetSceneApp).offScreenRenderer->waitForCleanUp();
				delete mActor;
			});
#else
		delete mActor;
#endif
	});
}

void Scene::removeRecursiveActor(Data::Actor* mActor)
{
	for (auto child : mActor->mChildActors)
		removeRecursiveActor(child);

	removeActor(mActor);
}

std::vector<Game::Data::Actor*> Game::SceneSys::Scene::getActorsByTag(std::string pTag)
{
	std::list<Game::Data::Actor*>::iterator it;
	std::vector<Game::Data::Actor*> CurrentActors;
	for (it = mActors.begin(); it != mActors.end(); ++it) 
	{
		for (auto& tag : (*it)->mTags)
		{
			if (tag._Equal(pTag))
			{
				Game::Data::Actor* actor = *it;
				CurrentActors.push_back(actor);
			}
		}
	}

	return sol::as_table(CurrentActors);
}


Game::Data::Actor* Game::SceneSys::Scene::getActorByTag(std::string pTag)
{
	std::list<Game::Data::Actor*>::iterator it;
	for (it = mActors.begin(); it != mActors.end(); ++it)
		for (auto& tag : (*it)->mTags)
			if (tag._Equal(pTag))
				return *it;

	return nullptr;
}