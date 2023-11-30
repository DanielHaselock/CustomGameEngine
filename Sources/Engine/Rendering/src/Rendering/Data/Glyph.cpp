#include "Rendering/Data/Glyph.h"

using namespace Rendering::Data;

Glyph::Glyph(Maths::FVector2 pSize, Maths::FVector2 pBearing, unsigned int pAdvence, unsigned char* data)
    : size(pSize), bearing(pBearing), advence(pAdvence)
{
    if (data != nullptr)
        texture = new Rendering::Resources::Texture(pSize.x, pSize.y, data);
}

Glyph::~Glyph()
{
    if (texture != nullptr)
        delete texture;
}

Character::Character(Glyph& pGlyph) : glyph(pGlyph), mUniformBuffer(VK_SHADER_STAGE_VERTEX_BIT)
{
}

void Character::bind(Rendering::Data::Material& pMat)
{
    pMat.bindDescriptor("texSampler", glyph.texture->mTextureSets);
    pMat.bindDescriptor("ubo", mUniformBuffer.mDescriptorSets);
}

void Character::updatePos(Maths::FVector2 pos, float pScale)
{
    float xpos = pos.x + glyph.bearing.x;
    float ypos = (pos.y - (glyph.bearing.y));

    float w = glyph.size.x * pScale;
    float h = glyph.size.y * pScale;

    mUniformBuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix(
        { xpos + (w / 2.f), ypos + (h / 2.f), 0 },
        { 0, 180, 0 },
        { -w, h, 1 });
    mUniformBuffer.updateData();
}

void Character::updateProj(Maths::FMatrix4 pProj)
{
    mUniformBuffer.mData.mProj = pProj;
    mUniformBuffer.updateData();
}

void Character::updateColor(Maths::FVector4 color)
{
    mUniformBuffer.mData.mColor = color;
    mUniformBuffer.updateData();
}