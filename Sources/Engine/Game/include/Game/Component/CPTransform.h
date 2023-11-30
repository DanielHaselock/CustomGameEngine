#pragma once
#include "Game/Component/AComponent.h"
#include "Maths/FTransform.h"

namespace Game::Component
{
	class CPTransform : public AComponent, public Maths::FTransform
	{
	public:
		AComponent* clone() override;
		~CPTransform() override = default;

		void addLocalPosition(const Maths::FVector3& pPosition);
		void addLocalRotation(const Maths::FQuaternion& pRotation);
		void addLocalScale(const Maths::FVector3& pScale);

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}