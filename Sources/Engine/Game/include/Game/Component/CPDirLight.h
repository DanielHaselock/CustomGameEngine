#pragma once
#include "Game/Component/CPLight.h"
#include <Rendering/Data/UniformData.h>

namespace Game::Component
{
	class CPDirLight : public CPLight
	{
	public:
		float mBrightness = 0.5f;

#ifdef NSHIPPING
		Rendering::Resources::Model* mArrow = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataWithColor>* mUniformBufferArrow = nullptr;
#endif

		CPDirLight();
		AComponent* clone() override;
		~CPDirLight() override;

		virtual void fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData) override;
#ifdef NSHIPPING
		void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView) override;
#endif
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}