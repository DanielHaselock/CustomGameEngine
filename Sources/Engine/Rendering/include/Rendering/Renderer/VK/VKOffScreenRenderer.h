#pragma once
#include "Rendering/Renderer/IRenderer.h"
#include "Rendering/Data/FrameData.h"

namespace Rendering::Renderer::VK
{
	class VKOffScreenRenderer : public IRenderer
	{
		public:
			VkFramebuffer mFramebuffer;
			Data::FrameData mFrame;

			VkImageView mColorImageView;
			Data::AllocatedImage mColorImage;
			int ColorChannel = 4;

			VkSubresourceLayout subResourceLayout;

			VKOffScreenRenderer(unsigned int pWidth, unsigned int pHeight);
			~VKOffScreenRenderer();

			void recreateSwapChain(unsigned int pWidth, unsigned int pHeight);
			void cleanupSwapChain();

			void initGraphicsQueue();
			void createCommandPool();
			void createFrameBufferAttachement();
			void createRenderPass();
			void createFrameBuffer();
			void beginFrame();
			void endFrame();

			VkCommandBuffer getCurrentCommandBuffer() override;
			const void* pixelToArray();

			void saveToPNG(const char* pFile);

			void saveToJPG(const char* pFile);
	};
}