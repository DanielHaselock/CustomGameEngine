#pragma once

#include "Rendering/Renderer/IRenderer.h"
#include "vulkan/vulkan_core.h"
#include "EngineWindow/Core/Window.h"
#include "Rendering/Data/VKTypes.h"
#include "Rendering/Data/FrameData.h"
#include "Rendering/Data/UploadContext.h"
#include "Rendering/Context/Driver.h"
#include "Rendering/Data/QueueFamilyIndices.h"
#include "array"

namespace Rendering::Renderer::VK
{
	struct Shadow
	{
		VkImageView mDepthImageView;
		Data::AllocatedImage mDepthImage;
		std::vector<VkFramebuffer> framebuffers;
		VkRenderPass pass;
	};

	struct CubeShadow
	{
		VkImageView mDepthImageView;
		Data::AllocatedImage mDepthImage;

		VkImageView mColorView;
		Data::AllocatedImage mColorImage;

		std::array<VkImageView, 6> shadowCubeMapFaceImageViews;

		std::vector<VkFramebuffer> framebuffers;
		VkRenderPass pass;
	};

#define SHADOW_CUBE_SIZE 1080
	class VKRenderer : public IRenderer
	{
		public:
			static const unsigned int FRAME_OVERLAP = 3;
			bool mFramebufferResized = false;

			EngineWindow::Core::Window* mWindow = nullptr;

			int mFrameNumber { 0 };
			uint32_t mSwapchainImageIndex { 0 };

			VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
			VkSurfaceKHR mSurface = nullptr;
			VkSwapchainKHR mSwapchain;
			VkFormat mSwapchainImageFormat;
			std::vector<VkImage> mSwapchainImages;
			std::vector<VkImageView> mSwapchainImageViews;
			std::vector<VkFramebuffer> mFramebuffers;
			Data::FrameData mFrames[FRAME_OVERLAP];
			
			Data::UploadContext mUploadContext;
			Rendering::Data::DeletionQueue mainThreadAction;

			Shadow mShadowPass;
			CubeShadow mShadowCube;

			VKRenderer();
			~VKRenderer();

			void init(EngineWindow::Core::Window& pWindow);
			void initVulkan();
			Rendering::Data::QueueFamilyIndices VKRenderer::findQueueFamilies(VkPhysicalDevice& pdevice);
			void getGraphicsQueue();
			void initSwapchain();
			void initCommands();
			void initDefaultRenderpass();
			void initFramebuffers();
			void initSyncStructures();
			
			void beginFrame();
			void endFrame();

			void beginWorldPass();
			void endWorldPass();

			void beginShadowPass();
			void drawViewPort(int pSpotLightCount, int pIdx);
			void endShadowPass();

			void beginShadowCubePass(int pIdx);
			void drawViewPortCube(int pPointLightCount, int pIdx);
			void endShadowCubePass();

			void cleanUp();
			void recreateSwapChain();
			void cleanupSwapChain();

			Data::FrameData& getCurrentFrame();
			VkCommandBuffer getCurrentCommandBuffer() override;
			static void immediateSubmit(std::function<void(VkCommandBuffer pCmd)>&& function, VKRenderer* pRenderer = nullptr);

			void createShadowResources();
			void createShadowRenderPass();
			void createShadowFramebuffers();

			void createCubeShadowResources();
			void createCubeShadowRemderPass();
			void createCubeShadowFramebuffers();

			void setImageLayout(
				VkCommandBuffer cmdbuffer,
				VkImage image,
				VkImageLayout oldImageLayout,
				VkImageLayout newImageLayout,
				VkImageSubresourceRange subresourceRange,
				VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

			void setImageLayout(
				VkCommandBuffer cmdbuffer,
				VkImage image,
				VkImageAspectFlags aspectMask,
				VkImageLayout oldImageLayout,
				VkImageLayout newImageLayout,
				VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

			VkImageMemoryBarrier initImageMemoryBarrier();

			void createImage(uint32_t pWidth, uint32_t pHeight, uint32_t pLayerCount, uint32_t pMipLevels, VkFormat pFormat, VkImageTiling pTiling, VkImageUsageFlags pUsage, VkImageCreateFlags pFlags, VkMemoryPropertyFlags pProperties, VkImage& pImage, VkDeviceMemory& pImageMemory);
	};
}