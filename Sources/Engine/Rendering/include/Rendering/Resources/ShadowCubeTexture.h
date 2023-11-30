#pragma once
#include <Rendering/Renderer/VK/VKRenderer.h>

namespace Rendering::Resources
{
    class ShadowCubeTexture
    {
    public:
        VkSampler mTextureSampler;
        VkDescriptorSet mTextureSets;

        ShadowCubeTexture(Rendering::Renderer::VK::VKRenderer& pRenderer);
        ~ShadowCubeTexture();
    };
}