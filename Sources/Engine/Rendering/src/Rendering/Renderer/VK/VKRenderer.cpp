#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1002000

#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Data/VKTypes.h"
#include "Rendering/Renderer/VK/VKInitializers.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Context/VkDescriptor.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"

#include <VkBootstrap/VkBootstrap.h>
#include <VulkanMemoryAllocator/vk_mem_alloc.h>
#include <set>
#include <iostream>

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

using namespace Rendering::Renderer::VK;

VKRenderer::VKRenderer()
{
	{
		std::unique_lock lock(IRenderer::mInstanceLock);
		IRenderer::mInstance++;
		
		if (IRenderer::mInstance == 1)
		{
			provideService(VkDevice, mDriver.mDevice);
			provideService(VkPhysicalDevice, mDriver.mPhysicalDevice);
			provideService(VmaAllocator, mAllocator);
			provideService(Data::DeletionQueue, mMainDeletionQueue);
			provideService(VKRenderer, *this);
		}
	}
}

VKRenderer::~VKRenderer()
{
	cleanUp();
}

void VKRenderer::init(EngineWindow::Core::Window& pWindow)
{
	mWindow = &pWindow;
	mWindow->setWindowUserPointer(this);

	initVulkan();
	getGraphicsQueue();
	initSwapchain();
	initCommands();
	initDefaultRenderpass();
	initFramebuffers();
	initSyncStructures();

	//Shadow
	createShadowResources();
	createShadowRenderPass();
	createShadowFramebuffers();

	//Shadow cube
	createCubeShadowResources();
	createCubeShadowRemderPass();
	createCubeShadowFramebuffers();
}
VkBool32 debugUtilsMessengerCallbackEXT(
	VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cout << pCallbackData->pMessage << std::endl;
	return false;
}


void VKRenderer::initVulkan()
{
	vkb::InstanceBuilder builder;
	vkb::Instance vkb_inst;
	
	{
		std::unique_lock lock(IRenderer::mInstanceLock);
		auto inst_ret = builder.set_app_name("Vukan Renderer")
			.request_validation_layers(true/*enableValidationLayers && mInstance == 1*/)
			.set_debug_callback(&debugUtilsMessengerCallbackEXT)
			.require_api_version(1, 0, 0)
			.build();

		vkb_inst = inst_ret.value();
		
		
		if (mDriver.mInstance == nullptr)
		{
			mDriver.mInstance = vkb_inst.instance;
			mDebugMessenger = vkb_inst.debug_messenger;
		}
		else
			vkb_inst.instance = mDriver.mInstance;
	

		int res = mWindow->createWindowSurface<VkInstance, VkSurfaceKHR*>(mDriver.mInstance, &mSurface);

		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 0)
			.set_surface(mSurface)
			.select()
			.value();
		
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	
		if (mDriver.mDevice == nullptr)
			mDriver.mPhysicalDevice = physicalDevice.physical_device;
	}
}

Rendering::Data::QueueFamilyIndices VKRenderer::findQueueFamilies(VkPhysicalDevice& pdevice)
{
	Rendering::Data::QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.mGraphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(pdevice, i, mSurface, &presentSupport);

		if (presentSupport)
			indices.mPresentFamily = i;

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

void VKRenderer::getGraphicsQueue()
{
	Rendering::Data::QueueFamilyIndices indices = findQueueFamilies(mDriver.mPhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.mGraphicsFamily.value(), indices.mPresentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	{
		std::unique_lock lock(IRenderer::mInstanceLock);

		if (mDriver.mDevice == nullptr)
			if (vkCreateDevice(mDriver.mPhysicalDevice, &createInfo, nullptr, &mDriver.mDevice) != VK_SUCCESS)
				throw std::runtime_error("failed to create logical device!");

		

		vkGetDeviceQueue(mDriver.mDevice, indices.mGraphicsFamily.value(), 0, &mGraphicsQueue);
		mGraphicsQueueFamily = indices.mGraphicsFamily.value();

		if (mAllocator == nullptr)
		{
			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.physicalDevice = mDriver.mPhysicalDevice;
			allocatorInfo.device = mDriver.mDevice;
			allocatorInfo.instance = mDriver.mInstance;
			vmaCreateAllocator(&allocatorInfo, &mAllocator);
		}
	}
}

void VKRenderer::initSwapchain()
{
	int width, height;
	mWindow->getFramebufferSize(&width, &height);
	mWindowExtent = { (unsigned int)width, (unsigned int)height };

	vkb::SwapchainBuilder swapchainBuilder{ mDriver.mPhysicalDevice, mDriver.mDevice, mSurface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //VK_PRESENT_MODE_FIFO_KHR -- vsync 60 fps max / VK_PRESENT_MODE_MAILBOX_KHR
		.set_desired_extent(mWindowExtent.width, mWindowExtent.height)
		.build()
		.value();


	mSwapchain = vkbSwapchain.swapchain;
	mSwapchainImages = vkbSwapchain.get_images().value();
	mSwapchainImageViews = vkbSwapchain.get_image_views().value();
	mSwapchainImageFormat = vkbSwapchain.image_format;

	VkExtent3D depthImageExtent = 
	{
		mWindowExtent.width,
		mWindowExtent.height,
		1
	};

	VkImageCreateInfo dimg_info = VKInit::imageCreateInfo(mDepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vmaCreateImage(mAllocator, &dimg_info, &dimg_allocinfo, &mDepthImage.mImage, &mDepthImage.mAllocation, nullptr);

	VkImageViewCreateInfo dview_info = VKInit::imageviewCreateInfo(mDepthFormat, mDepthImage.mImage, VK_IMAGE_ASPECT_DEPTH_BIT);
	if (mDepthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) 
	{
		dview_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	
	vkCreateImageView(mDriver.mDevice, &dview_info, nullptr, &mDepthImageView);
}

void VKRenderer::initCommands()
{
	VkCommandPoolCreateInfo commandPoolInfo = VKInit::commandPoolCreateInfo(mGraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	for (int i = 0; i < FRAME_OVERLAP; i++) 
	{
		vkCreateCommandPool(mDriver.mDevice, &commandPoolInfo, nullptr, &mFrames[i].mCommandPool);


		VkCommandBufferAllocateInfo cmdAllocInfo = VKInit::commandBufferAllocateInfo(mFrames[i].mCommandPool, 1);
		vkAllocateCommandBuffers(mDriver.mDevice, &cmdAllocInfo, & mFrames[i].mMainCommandBuffer);

		mMainDeletionQueue.pushFunction([=]() 
		{
			vkDestroyCommandPool(mDriver.mDevice, mFrames[i].mCommandPool, nullptr);
		});
	}

	VkCommandPoolCreateInfo uploadCommandPoolInfo = VKInit::commandPoolCreateInfo(mGraphicsQueueFamily);
	vkCreateCommandPool(mDriver.mDevice, &uploadCommandPoolInfo, nullptr, &mUploadContext.mCommandPool);

	mMainDeletionQueue.pushFunction([=]() 
	{
		vkDestroyCommandPool(mDriver.mDevice, mUploadContext.mCommandPool, nullptr);
	});

	VkCommandBufferAllocateInfo cmdAllocInfo = VKInit::commandBufferAllocateInfo(mUploadContext.mCommandPool, 1);
	vkAllocateCommandBuffers(mDriver.mDevice, &cmdAllocInfo, &mUploadContext.mCommandBuffer);
}

void VKRenderer::initDefaultRenderpass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = mSwapchainImageFormat;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment = {};
	depth_attachment.flags = 0;
	depth_attachment.format = mDepthFormat;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	dependency.dependencyFlags = 0;

	VkSubpassDependency depth_dependency = {};
	depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	depth_dependency.dstSubpass = 0;
	depth_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	depth_dependency.srcAccessMask = 0;
	depth_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	depth_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	depth_dependency.dependencyFlags = 0;

	VkSubpassDependency dependencies[2] = { dependency, depth_dependency };

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 2;
	render_pass_info.pAttachments = &attachments[0];
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 2;
	render_pass_info.pDependencies = &dependencies[0];

	vkCreateRenderPass(mDriver.mDevice, &render_pass_info, nullptr, &mRenderPass);

	mMainDeletionQueue.pushFunction([=]() 
	{
		vkDestroyRenderPass(mDriver.mDevice, mRenderPass, nullptr);
	});
}

void VKRenderer::initFramebuffers()
{
	VkFramebufferCreateInfo fb_info = VKInit::framebufferCreateInfo(mRenderPass, mWindowExtent);

	const uint32_t swapchain_imagecount = mSwapchainImages.size();
	mFramebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) 
	{
		VkImageView attachments[2];
		attachments[0] = mSwapchainImageViews[i];
		attachments[1] = mDepthImageView;

		fb_info.pAttachments = attachments;
		fb_info.attachmentCount = 2;

		vkCreateFramebuffer(mDriver.mDevice, &fb_info, nullptr, &mFramebuffers[i]);
	}
}

void VKRenderer::initSyncStructures()
{
	VkFenceCreateInfo fenceCreateInfo = VKInit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = VKInit::semaphoreCreateInfo();

	for (int i = 0; i < FRAME_OVERLAP; i++) 
	{
		vkCreateFence(mDriver.mDevice, &fenceCreateInfo, nullptr, &mFrames[i].mRenderFence);

		mMainDeletionQueue.pushFunction([=]() 
		{
			vkDestroyFence(mDriver.mDevice, mFrames[i].mRenderFence, nullptr);
		});


		vkCreateSemaphore(mDriver.mDevice, &semaphoreCreateInfo, nullptr, &mFrames[i].mPresentSemaphore);
		vkCreateSemaphore(mDriver.mDevice, &semaphoreCreateInfo, nullptr, &mFrames[i].mRenderSemaphore);

		mMainDeletionQueue.pushFunction([=]() 
		{
			vkDestroySemaphore(mDriver.mDevice, mFrames[i].mPresentSemaphore, nullptr);
			vkDestroySemaphore(mDriver.mDevice, mFrames[i].mRenderSemaphore, nullptr);
		});
	}

	VkFenceCreateInfo uploadFenceCreateInfo = VKInit::fenceCreateInfo();

	vkCreateFence(mDriver.mDevice, &uploadFenceCreateInfo, nullptr, &mUploadContext.mUploadFence);
	mMainDeletionQueue.pushFunction([=]()
	{
		vkDestroyFence(mDriver.mDevice, mUploadContext.mUploadFence, nullptr);
	});
}

void VKRenderer::beginFrame()
{
	vkWaitForFences(mDriver.mDevice, 1, &getCurrentFrame().mRenderFence, true, UINT64_MAX);
	
	VkResult result = vkAcquireNextImageKHR(mDriver.mDevice, mSwapchain, UINT64_MAX, getCurrentFrame().mPresentSemaphore, nullptr, &mSwapchainImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("failed to acquire swap chain image!");
	
	vkResetFences(mDriver.mDevice, 1, &getCurrentFrame().mRenderFence);

	vkResetCommandBuffer(getCurrentFrame().mMainCommandBuffer, 0);

	VkCommandBufferBeginInfo cmdBeginInfo = VKInit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	vkBeginCommandBuffer(getCurrentFrame().mMainCommandBuffer, &cmdBeginInfo);
}

void VKRenderer::endFrame()
{
	std::unique_lock lock(IRenderer::mInstanceLock);
	vkEndCommandBuffer(getCurrentFrame().mMainCommandBuffer);
	
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit = VKInit::submitInfo(&getCurrentFrame().mMainCommandBuffer);
	submit.pWaitDstStageMask = &waitStage;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &getCurrentFrame().mPresentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &getCurrentFrame().mRenderSemaphore;

	vkQueueSubmit(mGraphicsQueue, 1, &submit, getCurrentFrame().mRenderFence);


	VkPresentInfoKHR presentInfo = VKInit::presentInfo();
	presentInfo.pSwapchains = &mSwapchain;
	presentInfo.swapchainCount = 1;
	presentInfo.pWaitSemaphores = &getCurrentFrame().mRenderSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pImageIndices = &mSwapchainImageIndex;

	VkResult result = vkQueuePresentKHR(mGraphicsQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized)
	{
		//mFramebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to present swap chain image!");

	mFrameNumber = (mFrameNumber + 1) % FRAME_OVERLAP;
}

void VKRenderer::beginWorldPass()
{
	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkClearValue depthClear;
	depthClear.depthStencil = { 1.0f, 0 };

	VkClearValue clearValues[] = { clearValue, depthClear };

	VkRenderPassBeginInfo rpInfo = VKInit::renderpassBeginInfo(mRenderPass, mWindowExtent, mFramebuffers[mSwapchainImageIndex]);
	rpInfo.clearValueCount = 2;
	rpInfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(getCurrentFrame().mMainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mWindowExtent.width;
	viewport.height = (float)mWindowExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = mWindowExtent;

	vkCmdSetViewport(getCurrentFrame().mMainCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(getCurrentFrame().mMainCommandBuffer, 0, 1, &scissor);
	vkCmdSetDepthBias(getCurrentFrame().mMainCommandBuffer, 0, 0, 0);
}

void VKRenderer::endWorldPass()
{
	std::unique_lock lock(IRenderer::mInstanceLock);
	vkCmdEndRenderPass(getCurrentFrame().mMainCommandBuffer);
}

void VKRenderer::beginShadowPass()
{
	VkClearValue depthClear;
	depthClear.depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo rpInfo = VKInit::renderpassBeginInfo(mShadowPass.pass, mWindowExtent, mShadowPass.framebuffers[mSwapchainImageIndex]);
	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &depthClear;

	vkCmdBeginRenderPass(getCurrentFrame().mMainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VKRenderer::drawViewPort(int pSpotLightCount, int pIdx)
{
	float tileSizeX = mWindowExtent.width / (float)pSpotLightCount;
	float tileSizeY = mWindowExtent.height / (float)pSpotLightCount;

	float tileOffsetX = pIdx % pSpotLightCount;
	float tileOffsetY = pIdx / pSpotLightCount;

	VkViewport viewport;
	viewport.x = tileOffsetX * tileSizeX;
	viewport.y = tileOffsetY * tileSizeY;
	viewport.width = (float)tileSizeX;
	viewport.height = (float)tileSizeY;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.offset = { int(viewport.x + 4), int(viewport.y + 4) };
	scissor.extent.width = tileSizeX - 8;
	scissor.extent.height = tileSizeY - 8;

	vkCmdSetViewport(getCurrentFrame().mMainCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(getCurrentFrame().mMainCommandBuffer, 0, 1, &scissor);
	vkCmdSetDepthBias(getCurrentFrame().mMainCommandBuffer, 4, 0, 1.5f);
}

void VKRenderer::endShadowPass()
{
	std::unique_lock lock(IRenderer::mInstanceLock);
	vkCmdEndRenderPass(getCurrentFrame().mMainCommandBuffer);
}

void VKRenderer::beginShadowCubePass(int pFaceIdx)
{
	VkClearValue clearValues[2];
	clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo rp_begin;
	rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_begin.pNext = NULL;
	rp_begin.renderPass = mShadowCube.pass;
	rp_begin.framebuffer = mShadowCube.framebuffers[pFaceIdx];
	rp_begin.renderArea.offset.x = 0;
	rp_begin.renderArea.offset.y = 0;
	rp_begin.renderArea.extent.width = SHADOW_CUBE_SIZE;
	rp_begin.renderArea.extent.height = SHADOW_CUBE_SIZE;
	rp_begin.clearValueCount = 2;
	rp_begin.pClearValues = clearValues;

	vkCmdBeginRenderPass(getCurrentFrame().mMainCommandBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
}

void VKRenderer::drawViewPortCube(int pPointLightCount, int pIdx)
{
	float tileSizeX = mWindowExtent.height / (float)pPointLightCount;
	float tileSizeY = mWindowExtent.height / (float)pPointLightCount;

	float tileOffsetX = pIdx % pPointLightCount;
	float tileOffsetY = pIdx / pPointLightCount;

	VkViewport viewport;
	viewport.x = 0;//tileOffsetX * tileSizeX;
	viewport.y = 0;//tileOffsetY * tileSizeY;
	viewport.width = SHADOW_CUBE_SIZE;// (float)tileSizeX;
	viewport.height = SHADOW_CUBE_SIZE;// (float)tileSizeY;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent.width = SHADOW_CUBE_SIZE;
	scissor.extent.height = SHADOW_CUBE_SIZE;
	/*scissor.offset = { int(viewport.x + 4), int(viewport.y + 4) };
	scissor.extent.width = tileSizeX - 8;
	scissor.extent.height = tileSizeY - 8;*/

	vkCmdSetViewport(getCurrentFrame().mMainCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(getCurrentFrame().mMainCommandBuffer, 0, 1, &scissor);
	vkCmdSetDepthBias(getCurrentFrame().mMainCommandBuffer, 4, 0, 1.5f);
}

void VKRenderer::endShadowCubePass()
{
	std::unique_lock lock(IRenderer::mInstanceLock);
	vkCmdEndRenderPass(getCurrentFrame().mMainCommandBuffer);
}

void VKRenderer::cleanUp()
{
	vkWaitForFences(mDriver.mDevice, 1, &getCurrentFrame().mRenderFence, true, UINT64_MAX);

	cleanupSwapChain();

	mMainDeletionQueue.flush();
	
	vkDestroySurfaceKHR(mDriver.mInstance, mSurface, nullptr);
	
	{
		std::unique_lock lock(IRenderer::mInstanceLock);
		IRenderer::mInstance--;

		if (IRenderer::mInstance == 0)
		{
			if (Rendering::Context::DescriptorCache::mDescriptorLayoutCache.mLayoutCache.size() != 0)
			{
				Rendering::Context::DescriptorCache::mDescriptorLayoutCache.cleanup();
				Rendering::Context::DescriptorCache::mDescriptorLayoutCache = {};
			}

			if (Rendering::Context::DescriptorCache::mDescriptorAllocator.mUsedPools.size() != 0)
			{
				Rendering::Context::DescriptorCache::mDescriptorAllocator.cleanup();
				Rendering::Context::DescriptorCache::mDescriptorAllocator = {};
			}

			#ifdef NSHIPPING
			if (Rendering::Renderer::Resources::VK::PipeLineBuilder::mModuleCache.mModuleCache.size() != 0)
			{
				Rendering::Renderer::Resources::VK::PipeLineBuilder::mModuleCache.mModuleCache.clear();
				Rendering::Renderer::Resources::VK::PipeLineBuilder::mModuleCache = {};
			}
			#endif

			vmaDestroyAllocator(mAllocator);
			vkDestroyDevice(mDriver.mDevice, nullptr);
			vkb::destroy_debug_utils_messenger(mDriver.mInstance, mDebugMessenger);
			vkDestroyInstance(mDriver.mInstance, nullptr);

			mAllocator = nullptr;
			mDriver.mDevice = nullptr;
			mDriver.mInstance = nullptr;
			mDriver.mInstance = nullptr;
		}
	}
}

void VKRenderer::recreateSwapChain()
{
	int width = 0, height = 0;
	mWindow->getFramebufferSize(&width, &height);
	while (width == 0 || height == 0)
		mWindow->getFramebufferSize(&width, &height);
	
	vkDeviceWaitIdle(mDriver.mDevice);
	cleanupSwapChain();
	initSwapchain();
	initFramebuffers();

	createShadowResources();
	createShadowFramebuffers();

	createCubeShadowResources();
	createCubeShadowFramebuffers();

	mainThreadAction.executeLoop();
}

void VKRenderer::cleanupSwapChain()
{
	for (int i = 0; i < mFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(mDriver.mDevice, mFramebuffers[i], nullptr);
		vkDestroyImageView(mDriver.mDevice, mSwapchainImageViews[i], nullptr);

		vkDestroyFramebuffer(mDriver.mDevice, mShadowPass.framebuffers[i], nullptr);
	}


	vkDestroyImageView(mDriver.mDevice, mDepthImageView, nullptr);
	vmaDestroyImage(mAllocator, mDepthImage.mImage, mDepthImage.mAllocation);

	vkDestroyImageView(mDriver.mDevice, mShadowPass.mDepthImageView, nullptr);
	vmaDestroyImage(mAllocator, mShadowPass.mDepthImage.mImage, mShadowPass.mDepthImage.mAllocation);


	vkDestroyImageView(mDriver.mDevice, mShadowCube.mColorView, nullptr);
	vmaDestroyImage(mAllocator, mShadowCube.mColorImage.mImage, mShadowCube.mColorImage.mAllocation);
	for (int i = 0; i < 6; i++)
	{
		vkDestroyFramebuffer(mDriver.mDevice, mShadowCube.framebuffers[i], nullptr);
		vkDestroyImageView(mDriver.mDevice, mShadowCube.shadowCubeMapFaceImageViews[i], nullptr);
	}

	vkDestroyImageView(mDriver.mDevice, mShadowCube.mDepthImageView, nullptr);
	vmaDestroyImage(mAllocator, mShadowCube.mDepthImage.mImage, mShadowCube.mDepthImage.mAllocation);

	vkDestroySwapchainKHR(mDriver.mDevice, mSwapchain, nullptr);
}

Rendering::Data::FrameData& VKRenderer::getCurrentFrame()
{
	return mFrames[mFrameNumber];
}

VkCommandBuffer VKRenderer::getCurrentCommandBuffer()
{
	return getCurrentFrame().mMainCommandBuffer;
}

void VKRenderer::immediateSubmit(std::function<void(VkCommandBuffer pCmd)>&& function, VKRenderer* pRenderer)
{
	std::unique_lock lock(IRenderer::mInstanceLock);

	VKRenderer& renderer = pRenderer == nullptr ? service(VKRenderer) : *pRenderer;
	
	VkCommandBufferBeginInfo cmdBeginInfo = VKInit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	vkBeginCommandBuffer(renderer.mUploadContext.mCommandBuffer, &cmdBeginInfo);

	function(renderer.mUploadContext.mCommandBuffer);

	vkEndCommandBuffer(renderer.mUploadContext.mCommandBuffer);

	VkSubmitInfo submit = VKInit::submitInfo(&renderer.mUploadContext.mCommandBuffer);

	vkQueueSubmit(renderer.mGraphicsQueue, 1, &submit, renderer.mUploadContext.mUploadFence);

	vkWaitForFences(renderer.mDriver.mDevice, 1, &renderer.mUploadContext.mUploadFence, true, UINT64_MAX);
	vkResetFences(renderer.mDriver.mDevice, 1, &renderer.mUploadContext.mUploadFence);

	vkResetCommandPool(renderer.mDriver.mDevice, renderer.mUploadContext.mCommandPool, 0);
}

void VKRenderer::createShadowResources()
{
	VkExtent3D depthImageExtent =
	{
		mWindowExtent.width,
		mWindowExtent.height,
		1
	};

	VkImageCreateInfo dimg_info = VKInit::imageCreateInfo(mDepthFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vmaCreateImage(mAllocator, &dimg_info, &dimg_allocinfo, &mShadowPass.mDepthImage.mImage, &mShadowPass.mDepthImage.mAllocation, nullptr);

	VkImageViewCreateInfo dview_info = VKInit::imageviewCreateInfo(mDepthFormat, mShadowPass.mDepthImage.mImage, VK_IMAGE_ASPECT_DEPTH_BIT);
	vkCreateImageView(mDriver.mDevice, &dview_info, nullptr, &mShadowPass.mDepthImageView);
}

void VKRenderer::createShadowRenderPass()
{
	VkAttachmentDescription attachmentDescription{};
	attachmentDescription.format = mDepthFormat;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 0;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;
	subpass.pDepthStencilAttachment = &depthReference;

	// Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDescription;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCreateInfo.pDependencies = dependencies.data();
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.flags = 0;

	vkCreateRenderPass(mDriver.mDevice, &renderPassCreateInfo, NULL, &mShadowPass.pass);

	mMainDeletionQueue.pushFunction([=]()
	{
		vkDestroyRenderPass(mDriver.mDevice, mShadowPass.pass, nullptr);
	});
}

void VKRenderer::createShadowFramebuffers()
{
	VkFramebufferCreateInfo fb_info = VKInit::framebufferCreateInfo(mShadowPass.pass, mWindowExtent);

	const uint32_t swapchain_imagecount = mSwapchainImages.size();
	mShadowPass.framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++)
	{
		fb_info.pAttachments = &mShadowPass.mDepthImageView;
		fb_info.attachmentCount = 1;

		vkCreateFramebuffer(mDriver.mDevice, &fb_info, nullptr, &mShadowPass.framebuffers[i]);
	}
}





void VKRenderer::createImage(uint32_t pWidth, uint32_t pHeight, uint32_t pLayerCount, uint32_t pMipLevels, VkFormat pFormat, VkImageTiling pTiling, VkImageUsageFlags pUsage, VkImageCreateFlags pFlags, VkMemoryPropertyFlags pProperties, VkImage& pImage, VkDeviceMemory& pImageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = pWidth;
	imageInfo.extent.height = pHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = pMipLevels;
	imageInfo.arrayLayers = pLayerCount;
	imageInfo.format = pFormat;
	imageInfo.tiling = pTiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = pUsage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.flags = pFlags;

	if (vkCreateImage(mDriver.mDevice, &imageInfo, nullptr, &pImage) != VK_SUCCESS)
		throw std::runtime_error("failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(mDriver.mDevice, pImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, pProperties);

	if (vkAllocateMemory(mDriver.mDevice, &allocInfo, nullptr, &pImageMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate image memory!");

	vkBindImageMemory(mDriver.mDevice, pImage, pImageMemory, 0);
}

void VKRenderer::createCubeShadowResources()
{
	VkFormat format = VK_FORMAT_R32_SFLOAT;

	VkExtent3D depthImageExtent =
	{
		SHADOW_CUBE_SIZE,
		SHADOW_CUBE_SIZE,
		1
	};

	{
		VkImageCreateInfo dimg_info = VKInit::imageCreateInfo(format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, depthImageExtent);
		dimg_info.arrayLayers = 6;
		dimg_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		VmaAllocationCreateInfo dimg_allocinfo = {};
		dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vmaCreateImage(mAllocator, &dimg_info, &dimg_allocinfo, &mShadowCube.mColorImage.mImage, &mShadowCube.mColorImage.mAllocation, nullptr);


		immediateSubmit([this](VkCommandBuffer pCmd)
			{
				// Image barrier for optimal image (target)
				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = 1;
				subresourceRange.layerCount = 6;

				setImageLayout(
					pCmd,
					mShadowCube.mColorImage.mImage,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					subresourceRange);
			}, this);


		VkImageViewCreateInfo view = {};
		view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		view.format = format;
		view.components = { VK_COMPONENT_SWIZZLE_R };
		view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		view.subresourceRange.layerCount = 6;
		view.image = mShadowCube.mColorImage.mImage;

		vkCreateImageView(mDriver.mDevice, &view, nullptr, &mShadowCube.mColorView);

		view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view.subresourceRange.layerCount = 1;
		for (int i = 0; i < 6; i++)
		{
			view.subresourceRange.baseArrayLayer = i;
			vkCreateImageView(mDriver.mDevice, &view, nullptr, &mShadowCube.shadowCubeMapFaceImageViews[i]);
		}
	}

	{
		VkImageCreateInfo dimg_info = VKInit::imageCreateInfo(VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, depthImageExtent);
		VmaAllocationCreateInfo dimg_allocinfo = {};
		dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vmaCreateImage(mAllocator, &dimg_info, &dimg_allocinfo, &mShadowCube.mDepthImage.mImage, &mShadowCube.mDepthImage.mAllocation, nullptr);

		immediateSubmit([this](VkCommandBuffer pCmd)
			{
				setImageLayout(
					pCmd,
					mShadowCube.mDepthImage.mImage,
					VK_IMAGE_ASPECT_DEPTH_BIT,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			}, this);

		VkImageViewCreateInfo dview_info = VKInit::imageviewCreateInfo(VK_FORMAT_D32_SFLOAT, mShadowCube.mDepthImage.mImage, VK_IMAGE_ASPECT_DEPTH_BIT);
		vkCreateImageView(mDriver.mDevice, &dview_info, nullptr, &mShadowCube.mDepthImageView);
	}
}

void VKRenderer::createCubeShadowRemderPass()
{
	VkFormat format = VK_FORMAT_R32_SFLOAT;

	VkAttachmentDescription osAttachments[2] = {};

	osAttachments[0].format = VK_FORMAT_R32_SFLOAT;
	osAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	osAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	osAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	osAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	osAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	osAttachments[0].initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	osAttachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// Depth attachment
	osAttachments[1].format = VK_FORMAT_D32_SFLOAT;
	osAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	osAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	osAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	osAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	osAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	osAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	osAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.pDepthStencilAttachment = &depthReference;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 2;
	renderPassCreateInfo.pAttachments = osAttachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;

	vkCreateRenderPass(mDriver.mDevice, &renderPassCreateInfo, nullptr, &mShadowCube.pass);

	mMainDeletionQueue.pushFunction([=]()
	{
		vkDestroyRenderPass(mDriver.mDevice, mShadowCube.pass, nullptr);
	});
}

void VKRenderer::createCubeShadowFramebuffers()
{
	VkExtent2D ImageExtent =
	{
		SHADOW_CUBE_SIZE,
		SHADOW_CUBE_SIZE
	};

	VkImageView attachments[2];
	attachments[1] = mShadowCube.mDepthImageView;

	VkFramebufferCreateInfo fbufCreateInfo = {};
	fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbufCreateInfo.renderPass = mShadowCube.pass;
	fbufCreateInfo.attachmentCount = 2;
	fbufCreateInfo.pAttachments = attachments;
	fbufCreateInfo.width = ImageExtent.width;
	fbufCreateInfo.height = ImageExtent.height;
	fbufCreateInfo.layers = 1;

	mShadowCube.framebuffers.resize(6);
	for (int i = 0; i < 6; i++)
	{
		attachments[0] = mShadowCube.shadowCubeMapFaceImageViews[i];
		vkCreateFramebuffer(mDriver.mDevice, &fbufCreateInfo, nullptr, &mShadowCube.framebuffers[i]);
	}
}



void VKRenderer::setImageLayout(
	VkCommandBuffer cmdbuffer,
	VkImage image,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkImageSubresourceRange subresourceRange,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier = initImageMemoryBarrier();
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		imageMemoryBarrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		cmdbuffer,
		srcStageMask,
		dstStageMask,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}

void VKRenderer::setImageLayout(
	VkCommandBuffer cmdbuffer,
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;
	setImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
}

VkImageMemoryBarrier VKRenderer::initImageMemoryBarrier()
{
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	return imageMemoryBarrier;
}