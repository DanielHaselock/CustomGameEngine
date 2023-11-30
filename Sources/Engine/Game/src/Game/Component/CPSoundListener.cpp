#include "Game/Component/CPSoundListener.h"
#include <Game/Utils/ComponentType.h>
#include <EngineCore/Service/ServiceLocator.h>
#include "Sounds/Core/SoundEngine.h"
#include <Game/Data/Actor.h>

using namespace Game::Component;


AComponent* CPSoundListener::clone()
{
	return new CPSoundListener(*this);
}

void CPSoundListener::OnStart()
{
	if (!isActive)
		return;

	Sounds::Core::SoundEngine& engine = service(Sounds::Core::SoundEngine);
	engine.setListenerPosition(((Game::Data::Actor*)mOwner)->getTransform()->getWorldPosition(), {0, 0, 1});
}

void CPSoundListener::OnUpdate(float pDeltaTime)
{
	if (!isActive)
		return;

	Sounds::Core::SoundEngine& engine = service(Sounds::Core::SoundEngine);
	engine.setListenerPosition(((Game::Data::Actor*)mOwner)->getTransform()->getWorldPosition(), { 0, 0, 1 });
}

void CPSoundListener::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::SoundListener);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.EndObject();
}