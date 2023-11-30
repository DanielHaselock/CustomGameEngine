#include "Rendering/Resources/Prefab.h"
#include "Game/SceneSys/SceneManager.h"
#include "EngineCore/Service/ServiceLocator.h"

#ifdef NSHIPPING
	#include <Editor/Widget/WidgetGameObjectTreeItem.h>
	#include <Editor/Widget/WidgetEditor.h>
#endif

using namespace Rendering::Resources;

Prefab::Prefab(const std::string& pFileName)
{
	mActor = service(Game::SceneSys::SceneManager).parsePrefab(pFileName.c_str(), false, false);
	if (mActor == nullptr)
		return;
	mActor->isPrefab = true;
}

Game::Data::Actor* Prefab::spawnInstance()
{
	if (mActor == nullptr)
		return nullptr;

	Game::Data::Actor* clone = new Game::Data::Actor(*mActor);

	return clone;
}

void Prefab::addToScene(Game::Data::Actor* pActor)
{
	service(Game::SceneSys::SceneManager).mCurrentScene->addActor(pActor);
	for (Game::Data::Actor* child : pActor->mChildActors)
		addToScene(child);
}

void Prefab::addToEditor(Game::Data::Actor* pActor)
{
	#ifdef SHIPPING
		service(Game::SceneSys::SceneManager).mCurrentScene->addActor(pActor);
	#else
		Editor::Widget::WidgetGameObjectTreeItem* item = new Editor::Widget::WidgetGameObjectTreeItem(service(Editor::Widget::WidgetEditor).mSettings, "GameObject", pActor);

		if (pActor->mParent == nullptr)
			service(Editor::Widget::WidgetEditor).mHierarchy->mTree.append(item);
		else
			((Editor::Widget::WidgetGameObjectTreeItem*)pActor->mParent->mItemOwner)->appendRow(item);

		for (auto ListComp : pActor->mComponents)
		{
			if (ListComp.first == Game::Utils::ComponentType::Camera)
				for (auto Comp : ListComp.second)
					item->mDatas->addCamera((Game::Component::CPCamera*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::MeshRenderer)
				for (auto Comp : ListComp.second)
					item->mDatas->addModel((Game::Component::CPModel*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::Animator)
				for (auto Comp : ListComp.second)
					item->mDatas->addAnimator((Game::Component::CPAnimator*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::Collider)
				for (auto Comp : ListComp.second)
				{
					if(dynamic_cast<Game::Component::CPBoxCollider*>(Comp))
						item->mDatas->addBoxCollider((Game::Component::CPBoxCollider*)Comp);
					else if(dynamic_cast<Game::Component::CPCapsuleCollider*>(Comp))
						item->mDatas->addCapsuleCollider((Game::Component::CPCapsuleCollider*)Comp);
					else if (dynamic_cast<Game::Component::CPComplexCollider*>(Comp))
						item->mDatas->addComplexCollider((Game::Component::CPComplexCollider*)Comp);
				}
			else if (ListComp.first == Game::Utils::ComponentType::Script)
				for (auto Comp : ListComp.second)
					item->mDatas->addScript((Game::Component::CPScript*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::Sound)
				for (auto Comp : ListComp.second)
					item->mDatas->addSound((Game::Component::CPSound*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::SoundListener)
				for (auto Comp : ListComp.second)
					item->mDatas->addSoundListener((Game::Component::CPSoundListener*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::UI)
				for (auto Comp : ListComp.second)
					item->mDatas->addUI((Game::Component::CPUI*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::Particle)
				for (auto Comp : ListComp.second)
					item->mDatas->addParticle((Game::Component::CPParticle*)Comp);
			else if (ListComp.first == Game::Utils::ComponentType::Light)
				for (auto Comp : ListComp.second)
				{
					if(dynamic_cast<Game::Component::CPDirLight*>(Comp))
						item->mDatas->addDirLight((Game::Component::CPDirLight*)Comp);
					else if(dynamic_cast<Game::Component::CPPointLight*>(Comp))
						item->mDatas->addPointLight((Game::Component::CPPointLight*)Comp);
					else if(dynamic_cast<Game::Component::CPSpotLight*>(Comp))
						item->mDatas->addSpotLight((Game::Component::CPSpotLight*)Comp);
				}	
			else if (ListComp.first == Game::Utils::ComponentType::Decal)
				for (auto Comp : ListComp.second)
					item->mDatas->addDecal((Game::Component::CPDecal*)Comp);
		}


	
	#endif

	for (Game::Data::Actor* child : pActor->mChildActors)
		addToEditor(child);
}