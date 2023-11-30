#pragma	once

#include "Game/Component/AComponent.h"
#include "Sounds/Data/Sound.h"
#include <string>

namespace Game::Component
{
	class CPSound : public AComponent
	{
	public:
		std::string mPath;
		std::string mName;
		bool mute = false;
		bool loop = false;
		bool playOnAwake = true;
		bool is3DSound = false;
		float minDist = 5;
		int volume = 100;
		bool paused = false;
		Sounds::Data::Sound* sound = nullptr;

		~CPSound() override;

		AComponent* clone() override;

		void setClip(const std::string& pName, const char* pClip);
		void setClip(const char* pPath);
		void setClipWithLua(const char* pPath);

		void OnStart() override;
		void OnPaused() override;
		void OnUpdate(float pDeltaTime) override;
		void OnDestroy() override;

		void play();
		void pause();
		void setVolume(int pVolume);
		void setMute(bool pMute);
		void setLoop(bool pLoop);
		void setMinDist(float pMinDist);
		void set3DSound(bool p3DSound);

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}