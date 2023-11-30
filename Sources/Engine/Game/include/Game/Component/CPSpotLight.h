#pragma once
#include "Game/Component/CPLight.h"
#include <Rendering/Data/UniformData.h>

namespace Game::Component
{
	class CPSpotLight : public CPLight
	{
	public:
#ifdef NSHIPPING
		Rendering::Resources::Model* mArrow = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataWithColor>* mUniformBufferArrow = nullptr;
#endif

		float mCutOff = 12.5f;
		float mCutOffRad = 0;
		float mOuterCutOff = 17.5;
		float mOuterCutOffRad = 0;
		float mBrightness = 2;

		CPSpotLight();
		AComponent* clone() override;
		~CPSpotLight() override;

		void updateRad();

		virtual void fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData) override;
#ifdef NSHIPPING
		void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView) override;
#endif
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}