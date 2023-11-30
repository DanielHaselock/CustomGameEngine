#pragma once

#include "Game/Component/AComponent.h"
#include "Game/Component/CPTransform.h"
#include <list>
#include <unordered_map>
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include "Game/Utils/ComponentType.h"
#include "EngineCore/EventSystem/Event.h"
#include "Maths/FMatrix4.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "Game/Component/CPModel.h"
#include "Game/Component/ACollider.h"
#include "Rendering/Data/Material.h"
#include "Game/Component/CPCamera.h"
#include "Game/Component/CPDecal.h"
#include "Game/Component/CPScript.h"
#include "Game/Component/CPSound.h"
#include "Game/Component/CPUI.h"
#include "Game/Component/CPParticle.h"
#include "Game/Component/CPAnimator.h"

namespace Game::Data
{
	class Actor
	{
	public:
		std::unordered_map<Utils::ComponentType, std::list<Component::AComponent*>> mComponents;
		EngineCore::EventSystem::Event<> mValueChanged;
		EngineCore::EventSystem::Event<Maths::FMatrix4> mValueChangedFromEditor;
		std::mutex mMutex;
		unsigned int id;
		bool isPrefab = false;
		static int counterID;
#ifdef NSHIPPING
		void* mItemOwner = nullptr; // EditorOnly
#endif
		// Parameters
		bool mIsAwake = false;
		bool mIsStarted = false;
		bool mShouldUpdatePhysics = false;
		bool mShouldUpdatePhysicsRotationOnly = false;
		std::list<std::string> mTags;
		std::list<Game::Data::Actor*> mChildActors; //TODO Fix Delete
		Game::Data::Actor* mParent = nullptr;//TODO Fix Delete

		Actor();
		Actor(const Actor& pOther);
		~Actor();

		Component::CPTransform* getTransform(unsigned pIndex = 0);

		Component::CPTransform* setTransform(unsigned pIndex = 0);

		Component::CPTransform* setTransformRotation(unsigned pIndex = 0);

		Game::Component::CPModel* getModel(unsigned pIndex = 0);

		Game::Component::CPDecal* Actor::getDecal(unsigned pIndex = 0);

		Game::Component::ACollider* getCollider(unsigned pIndex = 0);

		Game::Component::CPCamera* getCamera(unsigned pIndex = 0);

		sol::table getScript(std::string pScriptName);

		Game::Component::CPSound* getSoundByName(std::string pName);

		Game::Component::CPSound* getSound(unsigned pIndex);

		Game::Component::CPUI* getUI(unsigned pIndex = 0);

		Game::Component::CPParticle* getParticle(unsigned pIndex = 0);

		Game::Component::CPAnimator* getAnimator(unsigned pIndex = 0);

		void addComponent(Utils::ComponentType pType, Component::AComponent* pComponent);

		void removeComponent(Utils::ComponentType pType, Component::AComponent* pComponent);

		void draw(void* pCmd, Rendering::Data::Material& pPipeLine);

		void drawShadow(void* pCmd);

		void drawUI(void* pCmd, Rendering::Data::Material& pPipeLine, Rendering::Data::Material& pPipeLineText);

		void drawParticle(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);
	
		void drawDecals(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj);

		void containMouse(Maths::FVector2 p);

		Rendering::Resources::UIResource::IUIResource* containMouseClick(Maths::FVector2 p);

		void updateUIProj(Maths::FMatrix4 uiProj, int width, int height);

		void updateWorldTransform(Component::CPTransform* pTransform);

		void SetPhysicsPosition();

		void SetPhysicsRotation();

		void updateWorldTransformEditor(Component::CPTransform* pTransform);

		void OnAwake();

		void OnStart();

		void OnPaused();

		void OnDestroy();

		void OnUpdate(float pDeltaTime);

		void setActive(bool pStatus);

		void updateProj(Maths::FMatrix4& pProj);

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter);

		void setTag(std::string pTag);

		void changeTag(std::string pOldTag, std::string pNewTag);

		void deleteTag(std::string pTag);

		std::string getTag(int pIndex);

		int getNumtags();

		void clearTags();

		void detachFromParent();

		void AddParent(Actor* pParent);
	};
}