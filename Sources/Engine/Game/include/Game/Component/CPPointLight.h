#pragma once
#include "Game/Component/CPLight.h"
#include <Rendering/Data/UniformData.h>
#include "Rendering/Data/UniformData.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"

#ifdef NSHIPPING
namespace Rendering
{
	class LineDrawer;
}
#endif

namespace Game::Component
{
	class CPPointLight : public CPLight
	{
	public:
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataShadowCube>* mUniformBuffer;
#ifdef NSHIPPING
		Rendering::LineDrawer* mRadiusDrawer = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataShadowCube>* mUniformBufferEditor;
#endif
		float mRadius = 5;
		float mBrightness = 3;
		bool isInit = false;
		bool ready = false;

		CPPointLight();
		AComponent* clone() override;
		~CPPointLight() override;

		virtual void fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData) override;
#ifdef NSHIPPING
		void updateDrawer();
		void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView) override;
#endif
		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}