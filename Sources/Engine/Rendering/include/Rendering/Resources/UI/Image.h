#pragma once
#include "Rendering/Resources/UI/IUIResource.h"
namespace Rendering::Resources::UIResource
{
	class Image : public IUIResource
	{
		public:
			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataUI> mUnibuffer;

			Image(Rect pRect) : IUIResource(pRect, IUI::Image), mUnibuffer(VK_SHADER_STAGE_VERTEX_BIT) {}

			~Image() override {}

			virtual void updateData() override;

			virtual void updateData(Maths::FMatrix4 pProj, int pW, int pH) override;

			virtual void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel) override;
	};
}