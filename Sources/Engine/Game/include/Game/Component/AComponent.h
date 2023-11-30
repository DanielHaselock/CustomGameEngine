#pragma once
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

//namespace Game::Data { class Actor; }

namespace Game::Component
{
	class AComponent
	{
	public:
		bool mIsAwaked = false;
		bool mIsStarted = false;
		bool isActive = true;

		void* mOwner = nullptr;

		virtual AComponent* clone() = 0;
		virtual ~AComponent() = default;

		virtual void OnAwake() {};

		virtual void OnStart() {};

		virtual void OnPaused() {};

		virtual void OnDestroy() {};

		virtual void OnUpdate(float pDeltaTime) {};

		virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) = 0;
	};
}