#include "Rendering/Resources/UI/Image.h"
using namespace Rendering::Resources::UIResource;

void Image::updateData()
{
	mUnibuffer.mData.mHasTexture = current->hasTexture;
	mUnibuffer.mData.mColor = current->color;

	mUnibuffer.updateData();
}


void Image::updateData(Maths::FMatrix4 pProj, int pW, int pH)
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
}

void Image::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel)
{
	if (isHidden)
		return;

	pMat.bindDescriptor("texSampler", current->texture->mTextureSets);
	pMat.bindDescriptor("ubo", mUnibuffer.mDescriptorSets);
	pMat.bindPipeLine(pCmd);
	pModel.draw(pCmd);
}