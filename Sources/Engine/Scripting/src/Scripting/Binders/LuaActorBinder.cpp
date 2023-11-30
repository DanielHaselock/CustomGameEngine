#include "Scripting/Binders/LuaActorBinder.h"
#include "Game/Data/Actor.h"
#include "Game/SceneSys/SceneManager.h"
#include "Physics/Core/BulPhysicsEngine.h"
#include "EngineCore/Thread/ThreadPool.h"
#ifdef NSHIPPING
//#include "Editor/Widget/WidgetSceneApp.h"
#include "Editor/Widget/WidgetEditor.h"

#endif
void Scripting::Binder::LuaActorBinder::bind(sol::state& pLuaState)
{
	pLuaState.new_usertype<Game::Data::Actor>("Actor",
		//Transform
		"GetTransform", &Game::Data::Actor::getTransform,
		"SetTransform", &Game::Data::Actor::setTransform,  //ResetsPhysics
		"SetTransformRotation", &Game::Data::Actor::setTransformRotation,  //ResetsPhysicsRotationOnly
		//Model
		"GetModel", &Game::Data::Actor::getModel,
		"GetCollider", &Game::Data::Actor::getCollider,
		"GetDecal", &Game::Data::Actor::getDecal,
		//Tags
		"SetTag", &Game::Data::Actor::setTag,
		"GetTag", &Game::Data::Actor::getTag,
		"GetTagCount", &Game::Data::Actor::getNumtags,
		"ClearAllTags", &Game::Data::Actor::clearTags,
		"DeleteTag", &Game::Data::Actor::deleteTag,
		"ChangeTag", &Game::Data::Actor::changeTag,
		"GetCamera", &Game::Data::Actor::getCamera,
		"GetScript", &Game::Data::Actor::getScript,
		"GetSoundByName", &Game::Data::Actor::getSoundByName,
		"GetSound", &Game::Data::Actor::getSound,
		"GetUI", &Game::Data::Actor::getUI,
		"GetParticle", &Game::Data::Actor::getParticle,
		"GetAnimator", &Game::Data::Actor::getAnimator,
		"GetChildren", [](Game::Data::Actor& pThis) {return sol::as_table(pThis.mChildActors); },
		"GetParent", [](Game::Data::Actor& pThis) {return (pThis.mParent); },
		"DetachFromParent", [](Game::Data::Actor& pThis)  { pThis.detachFromParent(); },
		"AddParent", &Game::Data::Actor::AddParent,
		"SetActive", &Game::Data::Actor::setActive,
		"Delete", [](Game::Data::Actor& pThis) 
		{
			service(EngineCore::Thread::ThreadPool).queueJob([&pThis] {
				auto Collider = pThis.getCollider();
				if (Collider)
				{
					pThis.removeComponent(Game::Utils::ComponentType::Collider, Collider);
				}


				for (Game::Component::AComponent* script : pThis.mComponents[Game::Utils::ComponentType::Script])
				{
					script->isActive = false;
					pThis.removeComponent(Game::Utils::ComponentType::Script, script);
				}


#ifdef NSHIPPING
				QMetaObject::invokeMethod(&service(Editor::Widget::WidgetSceneApp), [&pThis]
					{
						Editor::Widget::WidgetGameObjectTreeItem* item = ((Editor::Widget::WidgetGameObjectTreeItem*)pThis.mItemOwner);
						if (item == nullptr)
							return;

						pThis.mValueChanged.mActions.clear();
						pThis.mValueChangedFromEditor.mActions.clear();

						item->deleteItem(&service(Editor::Widget::WidgetEditor).mHierarchy->mTree);
						pThis.mItemOwner = nullptr;
					});
#else
				service(Game::SceneSys::SceneManager).mCurrentScene->removeRecursiveActor(&pThis);
#endif	
			});
		});
}