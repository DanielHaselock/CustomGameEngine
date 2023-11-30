#pragma	once

#include "CPModel.h"
#include "Game/Component/AComponent.h"
#include <string>
#include "Rendering/Data/Animator.h"
#include "Rendering/Resources/Animation.h"
#include "Rendering/Resources/Model.h"

namespace Game::Component
{
	class CPAnimator : public AComponent, public Rendering::Data::Animator
	{
	public:
		Game::Component::CPModel* mCPModel = nullptr;
		int cpModelIdx = -1;

		std::string mPath;
		std::string mName;
		
		bool playOnAwake = true;
		bool loop = false;
		bool paused = false;
		bool playing = false;

		~CPAnimator() override;

		AComponent* clone() override;

		void updateModel();
		void updateAnimationFromModel();

		void setAnimation(const std::string& pName, const char* pClip);
		void setAnimation(const char* pPath);
		void setAnimationWithLua(const char* pPath);

		void setBlendAnimation(const char* pPath);
		void setCrossFadeAnimation(const char* pPath, float pFadeSpeed);
		void setAdditiveAnimation(const char* pPath);
		
		void OnStart() override;
		void OnPaused() override;
		void OnUpdate(float pDeltaTime) override;
		void OnDestroy() override;

		void play();
		void pause();
		void stop();

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}