#pragma once
#include <Rendering/Renderer/VK/VKRenderer.h>

namespace Rendering::Resources
{
    class ShadowTexture
    {
    public:
        VkSampler mTextureSampler;
        VkDescriptorSet mTextureSets;

        ShadowTexture(Rendering::Renderer::VK::VKRenderer& pRenderer);
        ~ShadowTexture();
    };
}