#include "Rendering/Resources/UI/Button.h"

#include "EngineCore/Core/EngineApp.h"

using namespace Rendering::Resources::UIResource;

Button::Button(Rect pRect, std::string pText, float pScale)
	: IUIResource(pRect, IUI::Button), mUnibuffer(VK_SHADER_STAGE_VERTEX_BIT) 
{
	mText = new Rendering::Resources::UIResource::Text(pRect, pText, pScale);
}

void Button::updateData()
{
	mText->updateData();

	mUnibuffer.mData.mHasTexture = current->hasTexture;
	mUnibuffer.mData.mColor = current->color;

	mUnibuffer.updateData();
}

void Button::setText(std::string pText)
{
	auto& it = service(EngineCore::Core::EngineApp);

	it.mainThreadAction.pushFunction([this, pText, &it]
		{
			if (mText->text != nullptr)
			{
				it.rend->waitForCleanUp();
				delete mText->text;
				
			}
			mText->text = mText->font->createText(pText);
			updateData(it.uiProj, it.mWidth, it.mHeight);
		}
	);
}

void Button::updateData(Maths::FMatrix4 pProj, int pW, int pH)
{
	position.corner = Maths::FVector2(pW * position.ratioPos.x, pH * position.ratioPos.y);
	position.size = Maths::FVector2(pW * position.ratio.x, pH * position.ratio.y);

	float xOffset = position.size.x / 2.f;
	float yOffset = position.size.y / 2.f;

	mUnibuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix(
		{ position.corner.x + xOffset, position.corner.y + yOffset, 0 },
		{ 0, 0, 0 },
		{ position.size.x, -position.size.y, 1 });


	mUnibuffer.mData.mHasTexture = current->hasTexture;
	mUnibuffer.mData.mProj = pProj;
	mUnibuffer.mData.mColor = current->color;

	mUnibuffer.updateData();

	updateDataText(pProj, pW, pH);
}

void Button::updateDataText(Maths::FMatrix4 pProj, int pW, int pH)
{
	float scaleY = (pW * mText->mFontSize);
	float scaleFinal = (scaleY / mText->font->mSize);

	float sizeX = 0;
	for (auto it = mText->text->begin(); it != mText->text->end(); it++)
		sizeX += ((*it).glyph.advence >> 6) * scaleFinal;

	mText->position.corner.x = position.corner.x + (position.size.x / 2) - (sizeX / 2);
	mText->position.corner.y = position.corner.y + (position.size.y - (scaleY / 2));//(position.size.y / 2) - (scaleY / 2);
	
	
	Maths::FVector2 vec2 = mText->position.corner;
	for (auto it = mText->text->begin(); it != mText->text->end(); it++)
	{
		(*it).updateProj(pProj);
		(*it).updatePos(vec2, scaleFinal);
		(*it).updateColor(mText->current->color);

		vec2.x += ((*it).glyph.advence >> 6) * scaleFinal;
	}

	mText->position.size = Maths::FVector2(sizeX, scaleY);
}

void Button::onHover()
{
	isHover = true; 
	current = &hover;

	mText->isHover = true;
	mText->current = &mText->hover;

	updateData();
}

void Button::onEndHover()
{
	isHover = false;
	current = &normal;

	mText->isHover = false;
	mText->current = &mText->normal;

	updateData();
}

void Button::onPress()
{
	current = &press;
	mText->current = &mText->press;

	updateData();
}

void Button::onPressed()
{
	current = &hover;
	mText->current = &mText->hover;

	updateData();
}

void Button::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel)
{
	if (isHidden)
		return;

	pMat.bindDescriptor("texSampler", current->texture->mTextureSets);
	pMat.bindDescriptor("ubo", mUnibuffer.mDescriptorSets);
	pMat.bindPipeLine(pCmd);
	pModel.draw(pCmd);

	mText->draw(pCmd, pMat, pTextMat, pModel);
}