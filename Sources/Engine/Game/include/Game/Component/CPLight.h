#pragma once
#include "Game/Component/AComponent.h"
#include "Maths/FVector3.h"
#include "Rendering/Data/UniformData.h"
#ifdef NSHIPPING
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#endif
#include <Rendering/Data/Material.h>

namespace Game::Component
{
	class CPLight : public AComponent
	{
	public:
		Maths::FVector3 mColor = {1, 1, 1};
		float mAttenuatuion = 0.25;
		bool mCastShadow = false;

#ifdef NSHIPPING
		Rendering::Resources::Model* mQuad = nullptr;
		Rendering::Resources::Texture* mTexture = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataLightBillboard>* mUniformBuffer = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataLightBillboardId>* mUniformBufferId = nullptr;
#endif
		

		CPLight();
		virtual AComponent* clone() override;
		virtual ~CPLight() override;

		virtual void fillUniform(const int& pIdx, Rendering::Data::UniformDataLight& pData) {};
		virtual void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;

#ifdef NSHIPPING
		virtual void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLineBill, Rendering::Data::Material& pPipeLineModel, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);
		void drawOffscreen(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);
#endif

		Maths::FVector3 getPos();
		Maths::FVector3 getDir();
	};
}