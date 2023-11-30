#include "Editor/Data/Actor.h"
#include "Game/Component/CPModel.h"
#include "Game/Component/CPCamera.h"
#include "Game/Component/CPParticle.h"
#include "Game/Component/CPLight.h"
#include <Game/Component/CPPointLight.h>
#include <Game/Component/CPDecal.h>

using namespace Editor::Data;

Actor::Actor(Game::Data::Actor& pActor, void* pOwner)
	: mComponents(pActor.mComponents), mValueChanged(pActor.mValueChanged),
	mValueChangedFromEditor(pActor.mValueChangedFromEditor), mMutex(pActor.mMutex), 
	id(pActor.id), mUniformBufferid(VK_SHADER_STAGE_VERTEX_BIT), mItemOwner(pOwner), 
	mUniformBuferOutLine(VK_SHADER_STAGE_VERTEX_BIT), mChildActors(pActor.mChildActors)
{

}

Game::Component::CPTransform* Actor::getTransform(unsigned pIndex)
{
	if (mComponents[Game::Utils::ComponentType::Transform].size() <= pIndex)
		return nullptr;

	return (Game::Component::CPTransform*)mComponents[Game::Utils::ComponentType::Transform].front() + pIndex;
}

void Actor::draw(void* pCmd, const Maths::Frustum& frustrum, Rendering::Data::Material* pPipeLine)
{
	if (mComponents.find(Game::Utils::ComponentType::MeshRenderer) == mComponents.end())
		return;

	for (Game::Component::AComponent* model : mComponents[Game::Utils::ComponentType::MeshRenderer])
		((Game::Component::CPModel*)model)->drawEditor(pCmd, frustrum, pPipeLine);
}

void Actor::drawShadow(void* pCmd)
{
	if (mComponents.find(Game::Utils::ComponentType::MeshRenderer) == mComponents.end())
		return;

	for (Game::Component::AComponent* model : mComponents[Game::Utils::ComponentType::MeshRenderer])
		((Game::Component::CPModel*)model)->drawEditorShadow(pCmd);
}

void Actor::updateParticle(float pDeltaTime, Maths::FVector3& pCamPos)
{
	if (mComponents.find(Game::Utils::ComponentType::Particle) == mComponents.end())
		return;

	for (Game::Component::AComponent* particle : mComponents[Game::Utils::ComponentType::Particle])
		((Game::Component::CPParticle*)particle)->OnUpdateEditor(pDeltaTime, pCamPos);
}

void Actor::drawParticle(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (mComponents.find(Game::Utils::ComponentType::Particle) == mComponents.end())
		return;

	for (Game::Component::AComponent* particle : mComponents[Game::Utils::ComponentType::Particle])
		((Game::Component::CPParticle*)particle)->drawEditor(pCmd, pPipeLine, pViewProj, pView);
}

void Actor::updateTransform()
{
	Maths::FTransform* transform = getTransform();
	if (mComponents.find(Game::Utils::ComponentType::Collider) != mComponents.end())
		for (Game::Component::AComponent* Collider : mComponents[Game::Utils::ComponentType::Collider])
			((Game::Component::ACollider*)Collider)->UpdateTransform(*transform);


	transform->updateLocalMatrix();
	mValueChanged.dispatch();
	mValueChangedFromEditor.dispatch(transform->getWorldMatrix());

	for (auto& child : mChildActors)
		child->updateWorldTransformEditor(child->getTransform());

#ifdef NSHIPPING
	if (mComponents.find(Game::Utils::ComponentType::Light) != mComponents.end())
		for (Game::Component::AComponent* light : mComponents[Game::Utils::ComponentType::Light])
		{
			Game::Component::CPPointLight* aPoint = dynamic_cast<Game::Component::CPPointLight*>(light);
			if (aPoint != nullptr)
				aPoint->updateDrawer();
		}
#endif
}

#ifdef NSHIPPING
void Actor::drawCamera(void* pCmd)
{
	if (mComponents.find(Game::Utils::ComponentType::Camera) == mComponents.end())
		return;

	for (Game::Component::AComponent* camera : mComponents[Game::Utils::ComponentType::Camera])
		((Game::Component::CPCamera*)camera)->draw(pCmd);
}

void Actor::drawCameraOffscreen(void* pCmd)
{
	if (mComponents.find(Game::Utils::ComponentType::Camera) == mComponents.end())
		return;

	for (Game::Component::AComponent* camera : mComponents[Game::Utils::ComponentType::Camera])
		((Game::Component::CPCamera*)camera)->drawOffScreen(pCmd);
}

void Actor::drawLightOffscreen(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (mComponents.find(Game::Utils::ComponentType::Light) == mComponents.end())
		return;

	for (Game::Component::AComponent* light : mComponents[Game::Utils::ComponentType::Light])
		((Game::Component::CPLight*)light)->drawOffscreen(pCmd, pPipeLine, pViewProj, pView);
}

void Actor::updateLight()
{
	if (mComponents.find(Game::Utils::ComponentType::Light) != mComponents.end())
		for (Game::Component::AComponent* light : mComponents[Game::Utils::ComponentType::Light])
		{
			Game::Component::CPPointLight* aPoint = dynamic_cast<Game::Component::CPPointLight*>(light);
			if (aPoint != nullptr)
				aPoint->updateDrawer();
		}
}

void Actor::drawDecals(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj)
{
	for (Game::Component::AComponent* decal : mComponents[Game::Utils::ComponentType::Decal])
		((Game::Component::CPDecal*)decal)->drawEditor((VkCommandBuffer)pCmd, pPipeLine, pViewProj);
}
#endif

void Actor::updateProj(Maths::FMatrix4& pProj)
{
	for (Game::Component::AComponent* model : mComponents[Game::Utils::ComponentType::MeshRenderer])
		((Game::Component::CPModel*)model)->updateMatEditor(pProj, getTransform()->getWorldMatrix());

#ifdef NSHIPPING
	std::unique_lock lock(mMutex);
	if (mComponents.find(Game::Utils::ComponentType::Camera) != mComponents.end() && mComponents.find(Game::Utils::ComponentType::Camera)->second.size() != 0)
		((Game::Component::CPCamera*)mComponents[Game::Utils::ComponentType::Camera].front())->updateProjection(pProj);
#endif
}