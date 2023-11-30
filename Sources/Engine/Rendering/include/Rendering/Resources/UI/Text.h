#pragma once

#include "Rendering/Resources/UI/IUIResource.h"
#include "Rendering/Data/Glyph.h"
#include "Rendering/Resources/Font.h"

namespace Rendering::Resources::UIResource
{
	class Text : public IUIResource
	{
		public:
			Rendering::Resources::Font* font = nullptr;
			float mFontSize = 48;
			std::list<Rendering::Data::Character>* text = nullptr;

			Text(Rect pRect, std::string pText, float pScale);
			~Text() override;

			virtual void updateData() override;

			void setText(std::string pText);

			virtual void updateData(Maths::FMatrix4 pProj, int pW, int pH) override;

			virtual void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel) override;
	
			virtual void setScale(Maths::FVector2 pNewScale) override; 

			Maths::FMatrix4 oldProj;
			int oldWidth;
			int oldHeight;
	};
}