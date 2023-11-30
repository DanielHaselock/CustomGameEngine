#include "Rendering/Resources/UI/Text.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "EngineCore/Service/ServiceLocator.h"

#include "EngineCore/Core/EngineApp.h"

using namespace Rendering::Resources::UIResource;

Text::Text(Rect pRect, std::string pText, float pScale) : IUIResource(pRect, IUI::Text), mFontSize(pScale)
{
	font = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Font>("Resources/Engine/Fonts/Antonio-Bold.ttf", "Resources/Engine/Fonts/Antonio-Bold.ttf");
	text = font->createText(pText);
}

Text::~Text() 
{ 
	if (text != nullptr) 
	{ 
		delete text; 
		text = nullptr; 
	}
}

void Text::setText(std::string pText)
{		
	auto& it = service(EngineCore::Core::EngineApp);
	it.mainThreadAction.pushFunction([this, pText, &it]
	{
		if (text != nullptr)
		{
			it.rend->waitForCleanUp();
			delete text;

		}
		text = font->createText(pText);
		updateData(it.uiProj, it.mWidth, it.mHeight);
	});

}

void Text::updateData()
{
	for (auto it = text->begin(); it != text->end(); it++)
		(*it).updateColor(current->color);
}

void Text::updateData(Maths::FMatrix4 pProj, int pW, int pH)
{
	position.corner = Maths::FVector2(pW * position.ratioPos.x, pH * position.ratioPos.y);

	float scaleY = (pW * mFontSize);
	float scaleFinal = (scaleY / font->mSize);

	Maths::FVector2 vec2 = position.corner;
	float sizeX = 0;
	for (auto it = text->begin(); it != text->end(); it++)
	{
		(*it).updateProj(pProj);
		(*it).updatePos(vec2, scaleFinal);
		(*it).updateColor(current->color);

		vec2.x += ((*it).glyph.advence >> 6) * scaleFinal;
		sizeX += ((*it).glyph.advence >> 6) * scaleFinal;
	}

	position.size = Maths::FVector2(sizeX, scaleY);
	oldProj = pProj;
	oldWidth = pW;
	oldHeight = pH;
}

void Text::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel)
{
	if (isHidden)
		return;

	for (auto it = text->begin(); it != text->end(); it++)
	{
		if ((*it).glyph.texture == nullptr)
			continue;

		(*it).bind(pTextMat);
		pTextMat.bindPipeLine(pCmd);
		pModel.draw(pCmd);
	}
}

void Rendering::Resources::UIResource::Text::setScale(Maths::FVector2 pNewScale)
{
	mFontSize = pNewScale.magnitude();
	updateData(oldProj, oldWidth, oldHeight);
}
