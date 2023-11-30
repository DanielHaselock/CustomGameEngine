#pragma	once

#include "Game/Component/AComponent.h"
#include "Sounds/Data/Sound.h"
#include <string>

namespace Game::Component
{
	class CPSoundListener : public AComponent
	{
	public:
		
		AComponent* clone() override;

		
		void OnStart() override;
		void OnUpdate(float pDeltaTime) override;
		
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}