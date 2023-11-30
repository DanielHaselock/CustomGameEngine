#pragma	once

#include "CPModel.h"
#include "Game/Component/AComponent.h"
#include <string>
#include "Rendering/Data/Animator.h"
#include "Rendering/Resources/Animation.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Particle.h"

namespace Game::Component
{
	class CPParticle : public AComponent
	{
	public:
		Rendering::Resources::ParticleSystem* mPat = nullptr;
#ifdef NSHIPPING
		Rendering::Resources::ParticleSystem* mPatEditor = nullptr;
#endif
		std::string mPath;
		std::string mName;
		
		bool playOnAwake = true;
		bool loop = true;
		bool paused = false;
		bool playing = false;
		bool playInEditor = true;
		bool mStop = false;

		float mPlaybackSpeed = 1;

		~CPParticle() override = default;
		AComponent* clone() override;

		void setParticle(const std::string& pName, const char* pClip);
		void setParticle(const char* pPath);
		void setParticleWithLua(const char* pPath);

		void OnStart() override;
		void OnPaused() override;
		void OnUpdate(float pDeltaTime) override;
		void OnUpdateEditor(float pDeltaTime, Maths::FVector3& pCamPos);
		void OnDestroy() override;

		void play();
		void pause();
		void stop();
		void reset();
		void resetAndPlay();

		void draw(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);
		void drawEditor(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}