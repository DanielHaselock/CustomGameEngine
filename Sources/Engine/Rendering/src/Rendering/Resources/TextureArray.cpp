#include "Rendering/Resources/TextureArray.h"
#include "Rendering/Context/VkDescriptor.h"
#include "vector"

using namespace Rendering::Resources;

TextureArray::TextureArray(int pSize)
{
	descriptorImageInfos.reserve(pSize);
}

void TextureArray::addTexture(const VkDescriptorImageInfo& pInfo)
{
	descriptorImageInfos.push_back(pInfo);
}

void TextureArray::addTexture(Rendering::Resources::Texture& pTexture)
{
	addTexture({ pTexture.mTextureSampler, pTexture.mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
}

int TextureArray::size()
{
	return descriptorImageInfos.size();
}

void TextureArray::createDescritorSet()
{
	Context::DescriptorCache::begin().bind_images(descriptorImageInfos.size(), 0, &descriptorImageInfos[0], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(mTextureSets);
}