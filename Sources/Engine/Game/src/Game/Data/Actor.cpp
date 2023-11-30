#include "Game/Data/Actor.h"
#include "Game/Component/CPModel.h"
#include "Game/Component/CPUI.h"
#include "Game/Component/CPCamera.h"
#include "Game/Component/CPComplexCollider.h"
#include "Game/Component/CPBoxCollider.h"
#include "Game/Component/CPCapsuleCollider.h"
#include "Game/Component/CPParticle.h"
#include "Game/Component/ACollider.h"
#include "EngineCore/Core/EngineApp.h"
#include <Game/Component/CPPointLight.h>
#include <Game/Component/CPDecal.h>
#include "EngineCore/Service/ServiceLocator.h"
#include "Scripting/ScriptInterpreter.h"

#include "Game/Component/CPCapsuleCollider.h"

#ifdef NSHIPPING
#include "EngineCore/Service/ServiceLocator.h"
#include "Editor/Widget/WidgetSceneApp.h"
#endif

using namespace Game::Data;

int Actor::counterID = 0;

Actor::Actor()
{
	id = ++counterID;
	mComponents[Utils::ComponentType::Transform].push_back(new Component::CPTransform());
}

Actor::Actor(const Actor& pOther)
{
	id = ++counterID;

	auto it = pOther.mComponents.begin();
	while (it != pOther.mComponents.end())
	{
		for (Game::Component::AComponent* Components : it->second)
		{
			Game::Component::AComponent* comp = Components->clone();
			addComponent(it->first, comp);

			if (dynamic_cast<Game::Component::CPScript*>(comp) != nullptr)
				service(Scripting::ScriptInterpreter).registerScript((Game::Component::CPScript*)comp);
			else if (dynamic_cast<Game::Component::CPBoxCollider*>(comp) != nullptr)
				((Game::Component::CPBoxCollider*)comp)->setPhysicsObject(this);
			else if (dynamic_cast<Game::Component::CPCapsuleCollider*>(comp) != nullptr)
				((Game::Component::CPCapsuleCollider*)comp)->setPhysicsObject(this);
			else if (dynamic_cast<Game::Component::CPComplexCollider*>(comp) != nullptr)
				((Game::Component::CPComplexCollider*)comp)->setPhysicsObject(this);
		}

		++it;
	}

	mIsAwake = pOther.mIsAwake;
	mIsStarted = pOther.mIsAwake;

	for (auto& tag : pOther.mTags)
		mTags.push_back(tag);

	for (auto* aChild : pOther.mChildActors)
	{
		Actor* child = new Actor(*aChild);
		child->mParent = this;
		child->getTransform()->setParent(*this->getTransform());
		mChildActors.push_back(child);
	}

	mParent = pOther.mParent;
}

Actor::~Actor()
{
	if (mComponents.find(Utils::ComponentType::Transform) != mComponents.end()) //delete notifHandler BUGFIX with destoying child then parent
	{
		for (Game::Component::AComponent* Transform : mComponents[Utils::ComponentType::Transform])
		{
			if (((Game::Component::CPTransform*)Transform)->hasParent())
				((Game::Component::CPTransform*)Transform)->mParent->notifier.removeNotificationHandler(((Game::Component::CPTransform*)Transform)->notificationHandlerID);
		}
	}

	auto it = mComponents.begin();
	while (it != mComponents.end())
	{
		for (Game::Component::AComponent* Component : it->second)
		{
			if (Game::Component::CPCamera::mWorldCamera == Component)
			{
				if (service(EngineCore::Core::EngineApp).closed)
					Game::Component::CPCamera::mWorldCamera = nullptr;
				else
				{
#ifdef NSHIPPING
					std::mutex mtx;
					std::condition_variable cv;
					bool ready = false;

					service(EngineCore::Core::EngineApp).mainThreadAction.pushFunction([this, Component, &mtx, &ready, &cv]
						{
							service(EngineCore::Core::EngineApp).rend->waitForCleanUp();
							Game::Component::CPCamera::mWorldCamera = nullptr;
							std::unique_lock<std::mutex> lck(mtx);
							ready = true;
							cv.notify_all();
						});

					{
						std::unique_lock<std::mutex> lck(mtx);
						while (!ready) cv.wait(lck);
					}
#endif
				}
			}
				
			
			Component->mOwner = nullptr;

			if (isPrefab)
			{
				delete Component;
				continue;
			}

#ifdef NSHIPPING
			service(Editor::Widget::WidgetSceneApp).clearThreadAction.pushFunction([this, Component]
				{
					service(Editor::Widget::WidgetSceneApp).renderer->waitForCleanUp();
					delete Component;
				});
#else
			service(EngineCore::Core::EngineApp).clearThreadAction.pushFunction([this, Component]
			{
				service(EngineCore::Core::EngineApp).rend->waitForCleanUp();
				delete Component;
			});
#endif
		}

		++it;
	}
}

Game::Component::CPTransform* Actor::getTransform(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Transform].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Transform].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPTransform*)(*it);
}

Game::Component::CPTransform* Game::Data::Actor::setTransform(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Transform].size() <= pIndex)
		return nullptr;

	mShouldUpdatePhysics = true;

	auto& it = mComponents[Utils::ComponentType::Transform].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPTransform*)(*it);
}

Game::Component::CPTransform* Game::Data::Actor::setTransformRotation(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Transform].size() <= pIndex)
		return nullptr;

	mShouldUpdatePhysicsRotationOnly = true;
	auto& it = mComponents[Utils::ComponentType::Transform].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPTransform*)(*it);
}

Game::Component::CPModel* Actor::getModel(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::MeshRenderer].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::MeshRenderer].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPModel*)(*it);
}

Game::Component::CPDecal* Actor::getDecal(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Decal].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Decal].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPDecal*)(*it);
}

Game::Component::ACollider* Game::Data::Actor::getCollider(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Collider].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Collider].begin();
	std::advance(it, pIndex);

	return (Game::Component::ACollider*)(*it);
}

Game::Component::CPCamera* Game::Data::Actor::getCamera(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Camera].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Camera].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPCamera*)(*it);
}

sol::table Game::Data::Actor::getScript(std::string pScriptName)
{
	std::list<Game::Component::AComponent*> mScripts = mComponents[Utils::ComponentType::Script];
	std::list<Game::Component::AComponent*>::iterator it;
	for (it = mScripts.begin(); it != mScripts.end(); ++it)
	{
		Game::Component::CPScript* Script = (Game::Component::CPScript*)*it;
		if (pScriptName._Equal(Script->mName))
			return Script->mLuaTable;
	}
	return sol::nil;
}

Game::Component::CPSound* Game::Data::Actor::getSoundByName(std::string pName)
{
	if (mComponents[Utils::ComponentType::Sound].size() <= 0)
		return nullptr;

	for (Game::Component::AComponent* sound : mComponents[Utils::ComponentType::Sound])
	{
		Game::Component::CPSound* Casted = (Game::Component::CPSound*)sound;
		if (Casted->mName._Equal(pName))
			return Casted;
	}

	return nullptr;
}

Game::Component::CPSound* Game::Data::Actor::getSound(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Sound].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Sound].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPSound*)(*it);
}

Game::Component::CPUI* Actor::getUI(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::UI].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::UI].begin();
	std::advance(it, pIndex);
		
	return (Game::Component::CPUI*)(*it);
}

Game::Component::CPParticle* Actor::getParticle(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Particle].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Particle].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPParticle*)(*it);
}

Game::Component::CPAnimator* Actor::getAnimator(unsigned pIndex)
{
	if (mComponents[Utils::ComponentType::Animator].size() <= pIndex)
		return nullptr;

	auto& it = mComponents[Utils::ComponentType::Animator].begin();
	std::advance(it, pIndex);

	return (Game::Component::CPAnimator*)(*it);
}

void Actor::addComponent(Utils::ComponentType pType, Component::AComponent* pComponent)
{
	if (pComponent == nullptr)
		return;

	pComponent->mOwner = this;

	if (pType == Utils::ComponentType::Camera)
		if (Game::Component::CPCamera::mWorldCamera == nullptr)
			Game::Component::CPCamera::mWorldCamera = (Component::CPCamera*)pComponent;

//	std::unique_lock lock(mMutex);
	mComponents[pType].push_back(pComponent);
}

void Game::Data::Actor::removeComponent(Utils::ComponentType pType, Component::AComponent* pComponent)
{
#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).clearThreadAction.pushFunction([this, pType, pComponent]
		{
			service(Editor::Widget::WidgetSceneApp).renderer->waitForCleanUp();
			mComponents[pType].remove(pComponent);

			if (pType == Utils::ComponentType::Camera)
				if (Game::Component::CPCamera::mWorldCamera != nullptr)
					Game::Component::CPCamera::mWorldCamera = nullptr;

			delete pComponent;
		});

	return;
#else
	//std::unique_lock lock(mMutex);
	mComponents[pType].remove(pComponent);
#endif

	if (pType == Utils::ComponentType::Camera)
		if (Game::Component::CPCamera::mWorldCamera != nullptr)
			Game::Component::CPCamera::mWorldCamera = nullptr;

	delete pComponent;
}

void Actor::draw(void* pCmd, Rendering::Data::Material& pPipeLine)
{
	if (mComponents.find(Utils::ComponentType::MeshRenderer) == mComponents.end())
		return;

	for (Game::Component::AComponent* model : mComponents[Utils::ComponentType::MeshRenderer])
		((Game::Component::CPModel*)model)->draw(pCmd, &pPipeLine);
}

void Actor::drawShadow(void* pCmd)
{
	if (mComponents.find(Utils::ComponentType::MeshRenderer) == mComponents.end())
		return;

	for (Game::Component::AComponent* model : mComponents[Utils::ComponentType::MeshRenderer])
		((Game::Component::CPModel*)model)->drawShadow(pCmd);
}

void Actor::drawUI(void* pCmd, Rendering::Data::Material& pPipeLine, Rendering::Data::Material& pPipeLineText)
{
	if (mComponents.find(Utils::ComponentType::UI) == mComponents.end())
		return;

	for (Game::Component::AComponent* ui : mComponents[Utils::ComponentType::UI])
		((Game::Component::CPUI*)ui)->draw(pCmd, &pPipeLine, &pPipeLineText);
}

void Actor::drawParticle(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (mComponents.find(Utils::ComponentType::Particle) == mComponents.end())
		return;

	for (Game::Component::AComponent* particle : mComponents[Utils::ComponentType::Particle])
		((Game::Component::CPParticle*)particle)->draw(pCmd, pPipeLine, pViewProj, pView);
}

void Actor::drawDecals(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj)
{
	for (Game::Component::AComponent* decal : mComponents[Game::Utils::ComponentType::Decal])
		((Game::Component::CPDecal*)decal)->draw((VkCommandBuffer)pCmd, pPipeLine, pViewProj);
}

void Actor::containMouse(Maths::FVector2 p)
{
	if (mComponents.find(Utils::ComponentType::UI) == mComponents.end())
		return;

	for (Game::Component::AComponent* ui : mComponents[Utils::ComponentType::UI])
		((Game::Component::CPUI*)ui)->contain(p);
}

Rendering::Resources::UIResource::IUIResource* Actor::containMouseClick(Maths::FVector2 p)
{
	if (mComponents.find(Utils::ComponentType::UI) == mComponents.end())
		return nullptr;

	Rendering::Resources::UIResource::IUIResource* fin = nullptr;
	for (Game::Component::AComponent* ui : mComponents[Utils::ComponentType::UI])
	{
		Rendering::Resources::UIResource::IUIResource* tmp = ((Game::Component::CPUI*)ui)->containClick(p);
		if (tmp != nullptr)
		{
			if (fin == nullptr)
				fin = tmp;
			else if (fin->zOrder < tmp->zOrder)
				fin = tmp;
		}
	}
	return fin;
}

void Actor::updateUIProj(Maths::FMatrix4 uiProj, int width, int height)
{
	if (mComponents.find(Utils::ComponentType::UI) == mComponents.end())
		return;

	for (Game::Component::AComponent* ui : mComponents[Utils::ComponentType::UI])
		((Game::Component::CPUI*)ui)->updateUI(uiProj, width, height);
}

void Actor::updateWorldTransform(Component::CPTransform* pTransform)
{

	if (mShouldUpdatePhysics)
	{
		SetPhysicsPosition();
		SetPhysicsRotation();
		mShouldUpdatePhysics = false;
	}
	

	if (mComponents.find(Utils::ComponentType::Collider) != mComponents.end() && mComponents.find(Utils::ComponentType::Collider)->second.size() != 0)
	{
		Game::Component::ACollider* body = ((Game::Component::ACollider*)mComponents[Utils::ComponentType::Collider].front());
		if (body->getType() != Physics::Data::TypeRigidBody::E_DYNAMIC)
		{
			//body->UpdateTransform(*pTransform);
			return;
		}

		pTransform->mLocalPosition = body->getPhysicsPosition();
		if(!mShouldUpdatePhysicsRotationOnly)
			pTransform->mLocalRotation = body->getPhysicsRotation();
		else
		{
			mShouldUpdatePhysicsRotationOnly = false;
		}
	}
	pTransform->updateLocalMatrix();

	mValueChanged.dispatch();
	mValueChangedFromEditor.dispatch(pTransform->getWorldMatrix());

#ifdef NSHIPPING
	if (mComponents.find(Utils::ComponentType::Light) != mComponents.end())
		for (Game::Component::AComponent* light : mComponents[Utils::ComponentType::Light])
		{
			Game::Component::CPPointLight* aPoint = dynamic_cast<Game::Component::CPPointLight*>(light);
			if (aPoint != nullptr)
				aPoint->updateDrawer();
		}
#endif
}

void Game::Data::Actor::SetPhysicsPosition()
{
	if (mComponents.find(Utils::ComponentType::Collider) != mComponents.end() && mComponents[Utils::ComponentType::Collider].size() > 0)
	{
		Game::Component::ACollider* body = ((Game::Component::ACollider*)mComponents[Utils::ComponentType::Collider].front());
		body->UpdateTransform(*getTransform());
	}
}

void Game::Data::Actor::SetPhysicsRotation()
{
	if (mComponents.find(Utils::ComponentType::Collider) != mComponents.end() && mComponents[Utils::ComponentType::Collider].size() > 0)
	{
		Game::Component::ACollider* body = ((Game::Component::ACollider*)mComponents[Utils::ComponentType::Collider].front());
		body->UpdateRotation(*getTransform());
	}
}

void Game::Data::Actor::updateWorldTransformEditor(Component::CPTransform* pTransform)
{
	if (mComponents.find(Utils::ComponentType::Collider) != mComponents.end())
		for (Game::Component::AComponent* Collider : mComponents[Utils::ComponentType::Collider])
			((Game::Component::ACollider*)Collider)->UpdateTransform(*pTransform);


	pTransform->updateLocalMatrix();
	mValueChangedFromEditor.dispatch(pTransform->getWorldMatrix());

#ifdef NSHIPPING
	if (mComponents.find(Utils::ComponentType::Light) != mComponents.end())
		for (Game::Component::AComponent* light : mComponents[Utils::ComponentType::Light])
		{
			Game::Component::CPPointLight* aPoint = dynamic_cast<Game::Component::CPPointLight*>(light);
			if (aPoint != nullptr)
				aPoint->updateDrawer();
		}
#endif
}

void Actor::OnAwake()
{
	// Call Awake on all components
	auto it = mComponents.begin();

	while (it != mComponents.end())
	{
		for (Game::Component::AComponent* Component : it->second)
		{
			Component->OnAwake();
		}

		++it;
	}
}

void Actor::OnStart()
{
	// Call Start on all components
	auto it = mComponents.begin();

	while (it != mComponents.end())
	{
		for (Game::Component::AComponent* Component : it->second)
		{
			Component->OnStart();
		}

		++it;
	}
}

void Actor::OnPaused()
{
	// Call Awake on all components
	auto it = mComponents.begin();

	while (it != mComponents.end())
	{
		for (Game::Component::AComponent* Component : it->second)
		{
			Component->OnPaused();
		}

		++it;
	}
}

void Actor::OnDestroy()
{
	// Call Destroy on all components
	auto it = mComponents.begin();
	while (it != mComponents.end())
	{
		for (Game::Component::AComponent* Component : it->second)
		{
			Component->OnDestroy();
		}

		++it;
	}
}

void Actor::OnUpdate(float pDeltaTime)
{
	// Call Update on all components
	auto it = mComponents.begin();

	while (it != mComponents.end())
	{
		if (it->first == Game::Utils::ComponentType::Script)
		{
			++it;
			continue;
		}

		for (Game::Component::AComponent* Component : it->second)
		{
			Component->OnUpdate(pDeltaTime);
		}

		++it;
	}
}

void Actor::setActive(bool pStatus)
{
	for (auto AllComp : mComponents)
		for (auto Comp : AllComp.second)
			Comp->isActive = pStatus;

	for (auto child : mChildActors)
		child->setActive(pStatus);
}

void Actor::updateProj(Maths::FMatrix4& pProj)
{
	for (Game::Component::AComponent* model : mComponents[Utils::ComponentType::MeshRenderer])
		((Game::Component::CPModel*)model)->updateMat(pProj, getTransform()->getWorldMatrix());
}

void Actor::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.Key("Tags");
	pWriter.StartArray();
	for (auto it = mTags.begin(); it != mTags.end(); it++)
		if (!it->empty())
			pWriter.String(it->c_str());
	pWriter.EndArray();

	pWriter.Key("Components");
	pWriter.StartArray();

	for (auto it = mComponents.begin(); it != mComponents.end(); it++)
		for (Game::Component::AComponent* Component : it->second)
			Component->serialize(pWriter);

	pWriter.EndArray();
}

void Actor::setTag(std::string pTag)
{
	if (std::find(mTags.begin(), mTags.end(), pTag) == mTags.end())
	{
		mTags.push_back(pTag);
	}
}

void Actor::changeTag(std::string pOldTag, std::string pNewTag)
{
	std::list<std::string>::iterator it = std::find(mTags.begin(), mTags.end(), pOldTag);
	if (it == mTags.end())
		mTags.push_back(pNewTag);
	else if (std::find(mTags.begin(), mTags.end(), pNewTag) == mTags.end())
		(*it) = pNewTag;
}

void Actor::deleteTag(std::string pTag)
{
	mTags.remove(pTag);
}

std::string Actor::getTag(int pIndex)
{
	if (pIndex < 0 || pIndex >= mTags.size())
		return "";

	auto& it = mTags.begin();
	for (int i = 0; i < pIndex; i++)
		it++;

	return *it;
}

int Actor::getNumtags()
{
	return mTags.size();
}

void Actor::clearTags()
{
	mTags.clear();
}

void Actor::detachFromParent()
{
	if (mParent == nullptr)
		return;

	mParent->mChildActors.remove(this);
	mParent = nullptr;
	
	for (Game::Component::AComponent* aTransform : mComponents[Game::Utils::ComponentType::Transform])
	{
		Game::Component::CPTransform* transform = (Game::Component::CPTransform*)aTransform;
		if (transform->hasParent())
		{
			transform->mLocalMatrix = transform->mWorldMatrix;
			transform->mLocalPosition = transform->mWorldPosition;
			transform->mLocalRotation = transform->mWorldRotation;
			transform->mLocalScale = transform->mWorldScale;

			transform->mParent->notifier.removeNotificationHandler(transform->notificationHandlerID);
			transform->mParent = nullptr;
		}
	}
}

void Game::Data::Actor::AddParent(Actor* pParent)
{
	if (mParent)
		return;


	mParent = pParent;
	mParent->mChildActors.emplace_back(this);

	this->getTransform()->setParent(*(pParent->getTransform()));
}
