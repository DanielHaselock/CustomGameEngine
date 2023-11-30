#include "Scripting/Binders/LuaComponentBinder.h"
#include "EngineCore/Service/ServiceLocator.h"

#include "Game/Component/CPTransform.h"
#include "Game/Component/CPModel.h"

#include "Game/Component/ACollider.h"
#include <Game/Component/CPCamera.h>

#include "Game/Component/CPSound.h"
#include "Game/Component/CPUI.h"

#include "Game/Component/CPParticle.h"

#include "Game/Component/CPAnimator.h"

#ifdef NSHIPPING
#include "Editor/Widget/WidgetConsole.h"
#endif
#include "Rendering/Resources/UI/Text.h"
#include "Rendering/Resources/UI/Button.h"
#include <Game/Component/CPDecal.h>

using namespace Scripting::Binder;
using namespace Game::Component;
using namespace Maths;

void LuaComponentBinder::bind(sol::state& pLuaState)
{
	mLuaState = &pLuaState;

	// Bind the transform component


	pLuaState.new_usertype<AComponent>("AComponent",
		"SetActive", [](AComponent& pThis, bool pIActive) {pThis.isActive = pIActive; }
		);


	pLuaState.new_usertype<CPTransform>("Transform",
		sol::base_classes, sol::bases<AComponent>(),
		"SetPosition", &CPTransform::setLocalPosition,
		"SetRotation", &CPTransform::setLocalRotation,
		"SetScale", &CPTransform::setLocalScale,
		"SetLocalPosition", &CPTransform::setLocalPosition,
		"SetLocalRotation", &CPTransform::setLocalRotation,
		"SetLocalScale", &CPTransform::setLocalScale,
		"SetWorldPosition", &CPTransform::setWorldPosition,
		"SetWorldRotation", &CPTransform::setWorldRotation,
		"SetWorldScale", &CPTransform::setWorldScale,
		"AddPosition", &CPTransform::addLocalPosition,
		"AddRotation", &CPTransform::addLocalRotation,
		"AddScale", &CPTransform::addLocalScale,
		"AddLocalPosition", &CPTransform::addLocalPosition,
		"AddLocalRotation", &CPTransform::addLocalRotation,
		"AddWorldRotation",  [](CPTransform& pThis, const Maths::FQuaternion& pRotation)->void {
			pThis.addLocalRotation(pRotation);
			pThis.mWorldRotation = pRotation * pThis.mWorldRotation;
			pThis.updateWorldMatrix();
		},
		"AddLocalScale", &CPTransform::addLocalScale,
		"GetPosition", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldPosition(); },
		"GetRotation", [](CPTransform& pThis)->Maths::FQuaternion {return pThis.getWorldRotation(); },
		"GetScale", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldScale(); },
		"GetLocalPosition", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getLocalPosition(); },
		"GetLocalRotation", [](CPTransform& pThis)->Maths::FQuaternion {return pThis.getLocalRotation(); },
		"GetLocalScale", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getLocalScale(); },
		"GetWorldPosition", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldPosition(); },
		"GetWorldRotation", [](CPTransform& pThis)->Maths::FQuaternion {return pThis.getWorldRotation(); },
		"GetWorldScale", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldScale(); },
		"GetWorldForward", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldForward(); },
		"GetWorldRight", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldRight(); },
		"GetWorldUp", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getWorldUp(); },
		"GetLocalForward", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getLocalForward(); },
		"GetLocalRight", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getLocalRight(); },
		"GetLocalUp", [](CPTransform& pThis)->Maths::FVector3 {return pThis.getLocalUp(); }
	);

	// Bind the model component
	pLuaState.new_usertype<CPModel>("Model",
		sol::base_classes, sol::bases<AComponent>(),
		"SetModel", &CPModel::setModelWithPathLua
		);

	//Physics
	pLuaState.new_usertype<ACollider>("Collider",
		sol::base_classes, sol::bases<AComponent>(),
		"Delegate", [&pLuaState, this](ACollider& pCollider, std::string DelegateString)->void
		{
			DelegatePhysics(&pCollider, pLuaState, DelegateString);
		},
		"AddForce", &ACollider::addForce,
			"ClearForces", &ACollider::clearForces,
			"SetLinearVelocity", &ACollider::setLinearVelocity,
			"SetAngularVelocity", &ACollider::setAngularVelocity,
			"ClearLinearVelocity", &ACollider::clearLinearVelocity,
			"ClearAngularVelocity", &ACollider::clearAngularVelocity,
			"GetLinearVelocity", &ACollider::getLinearVelocity,
			"GetAngularVelocity", &ACollider::getAngularVelocity
			);

	//	DelegatePhysics(pLuaState);

		//Camera
	pLuaState.new_usertype<CPCamera>("Camera",
		sol::base_classes, sol::bases<AComponent>(),
		"GetForward", &CPCamera::getForward,
		"GetUp", &CPCamera::getUp,
		"GetRight", &CPCamera::getRight,
		"AddPitch", &CPCamera::AddPitch,
		"AddYaw", &CPCamera::AddYaw,
		"AddAxisAngle", &CPCamera::addAxisAngle,
		"AddUpDownAngle", &CPCamera::addAngleUpDown,
		"SetWorldCamera", [](CPCamera& pThis) {CPCamera::mWorldCamera = &pThis; }
		);

	//Sound
	pLuaState.new_usertype<CPSound>("Sound",
		sol::base_classes, sol::bases<AComponent>(),
		"Play", &CPSound::play,
		"Pause", &CPSound::pause,
		"SetVolume", &CPSound::setVolume,
		"SetMute", &CPSound::setMute,
		"SetLoop", &CPSound::setLoop,
		"SetMinDist", &CPSound::setMinDist,
		"Set3DSound", &CPSound::set3DSound,
		"SetClip", &CPSound::setClipWithLua
		);

	//UI
	pLuaState.new_usertype<CPUI>("UI",
		sol::base_classes, sol::bases<AComponent>(),
		"DelegateUI", [&pLuaState, this](CPUI& pThis, unsigned int pPressHold, std::string pFunctionName, std::string pName)->void
		{
			if (DelegateFromString(pLuaState, pFunctionName))
			{
				pThis.DelegateLuaFunctionCP(pPressHold, pLuaState[pFunctionName], pName);
			}
		},
		"SetText", [](CPUI& pThis, std::string pUIName, std::string pNewText)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
			{
				Rendering::Resources::UIResource::Text* text = dynamic_cast<Rendering::Resources::UIResource::Text*>(Resource);
				if (text)
				{
					text->setText(pNewText);
					return;
				}
				Rendering::Resources::UIResource::Button* butt = dynamic_cast<Rendering::Resources::UIResource::Button*>(Resource);
				if (butt)
				{
					butt->setText(pNewText);
					return;
				}
			}
		},
		"SetAllColor", [](CPUI& pThis, std::string pUIName, Maths::FVector4 pNewColor)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
				Resource->setAllColor(pNewColor);
		},
		"SetNormalColor", [](CPUI& pThis, std::string pUIName, Maths::FVector4 pNewColor)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
				Resource->setNormalColor(pNewColor);
		},
		"SetHoverColor", [](CPUI& pThis, std::string pUIName, Maths::FVector4 pNewColor)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
				Resource->setHoverColor(pNewColor);
		},
		"SetPressColor", [](CPUI& pThis, std::string pUIName, Maths::FVector4 pNewColor)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
				Resource->setPressColor(pNewColor);
		},
		"SetOrder", [](CPUI& pThis, std::string pUIName, int order)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			pThis.ui->mResources[Resource->zOrder].remove(Resource);

			Resource->zOrder = order;
			pThis.ui->mResources[Resource->zOrder].push_back(Resource);
		},
		"SetVisibility", [](CPUI& pThis, std::string pUIName, bool pVisibility)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
			{
				Resource->setVisible(pVisibility);
			}
		},
		"GetVisibility", [](CPUI& pThis, std::string pUIName)->bool
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
			{
				return !Resource->isHidden;
			}

			return false;
		},
		"SetScale", [](CPUI& pThis, std::string pUIName, Maths::FVector2 pNewScale)->void
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
			{
				Resource->setScale(pNewScale);
			}
		},
		"GetScale", [](CPUI& pThis, std::string pUIName)->Maths::FVector2
		{
			Rendering::Resources::UIResource::IUIResource* Resource = pThis.ui->findResource(pUIName);
			if (Resource)
				return Resource->getScale();

			return Maths::FVector2();
		}
	);

	//Particles
	pLuaState.new_usertype<CPParticle>("Particle",
		sol::base_classes, sol::bases<AComponent>(),
		"SetParticle", &CPParticle::setParticleWithLua,
		"Play", &CPParticle::play,
		"ResetAndPlay", &CPParticle::resetAndPlay,
		"Pause", &CPParticle::pause,
		"SetLoop", [this](CPParticle& pThis, bool pTrue) {pThis.loop = pTrue; },
		"Stop", &CPParticle::stop
		);

	//Decal
	pLuaState.new_usertype<CPDecal>("Decal",
		sol::base_classes, sol::bases<AComponent>(),
		"UpdateDrawer", &CPDecal::updateDrawer
#ifdef NSHIPPING
		,"UpdateDrawerLine", &CPDecal::updateDrawerLine
#endif // NSHIPPING

	);

	//Animator
	pLuaState.new_usertype<CPAnimator>("Animator",
		sol::base_classes, sol::bases<AComponent>(),
		"SetAnimation", &CPAnimator::setAnimationWithLua,
		"Play", &CPAnimator::play,
		"Pause", &CPAnimator::pause,
		"Stop", &CPAnimator::stop,
		"SetBlendAnimation", [this](CPAnimator& pThis, std::string pPath) { pThis.setBlendAnimation(pPath.c_str()); },
		"SetFadeAnimation", [this](CPAnimator& pThis, std::string pPath, float pSpeed) { pThis.setCrossFadeAnimation(pPath.c_str(), pSpeed); },
		"SetAdditiveAnimation", [this](CPAnimator& pThis, std::string pPath) { pThis.setAdditiveAnimation(pPath.c_str()); },
		"SetBlendFactor", [this](CPAnimator& pThis, float pBlendFactor) { pThis.setBlendFactor(pBlendFactor); }
	);
}

void Scripting::Binder::LuaComponentBinder::DelegatePhysics(Game::Component::ACollider* pCollider, sol::state& pLuaState, std::string DelegateString)
{
	if (DelegateFromString(pLuaState, DelegateString))
	{
		getFunctionFromString(pCollider, DelegateString) = pLuaState[DelegateString];
	}
}

bool Scripting::Binder::LuaComponentBinder::DelegateFromString(sol::state& pLuaState, std::string DelegateString)
{
	if (pLuaState[DelegateString].valid())
	{
		return true;
	}
	else
	{
#ifdef NSHIPPING
		std::string err = DelegateString + " Does not exist";
		service(Editor::Widget::WidgetConsole).errorPrint(err.c_str());
#endif
	}
	return false;
}

std::function<void(void*)>& Scripting::Binder::LuaComponentBinder::getFunctionFromString(Game::Component::ACollider* pCollider, std::string pInput)
{
	if (pInput.find("OnCollisionEnter") != std::string::npos)
		return pCollider->mPhysicsFunctions.OnCollisionEnter;

	if (pInput.find("OnCollisionStay") != std::string::npos)
		return pCollider->mPhysicsFunctions.OnCollisionStay;

	if (pInput.find("OnCollisionExit") != std::string::npos)
		return pCollider->mPhysicsFunctions.OnCollisionExit;

	if (pInput.find("OnTriggerEnter") != std::string::npos)
		return pCollider->mPhysicsFunctions.OnTriggerEnter;

	if (pInput.find("OnTriggerStay") != std::string::npos)
		return pCollider->mPhysicsFunctions.OnTriggerStay;

	if (pInput.find("OnTriggerExit") != std::string::npos)
		return pCollider->mPhysicsFunctions.OnTriggerExit;

	return std::function<void(void*)>();
}