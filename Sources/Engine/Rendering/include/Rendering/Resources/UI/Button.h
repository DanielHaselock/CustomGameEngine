#pragma once
#include "Rendering/Resources/UI/IUIResource.h"
#include <Rendering/Data/Glyph.h>
#include <Rendering/Resources/Font.h>
#include "Rendering/Resources/UI/Text.h"

namespace Rendering::Resources::UIResource
{
	class Button : public IUIResource
	{
		public:
			Rendering::Resources::UIResource::Text* mText;

			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataUI> mUnibuffer;

			Button(Rect pRect, std::string pText, float pScale);

			~Button() override { delete mText; }

			void setText(std::string pText);

			virtual void updateData() override;

			virtual void updateData(Maths::FMatrix4 pProj, int pW, int pH) override;

			virtual void updateDataText(Maths::FMatrix4 pProj, int pW, int pH);


			virtual void onHover() override;
			virtual void onEndHover() override;

			virtual void onPress() override;
			virtual void onPressed() override;

			virtual void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel) override;
	};
}