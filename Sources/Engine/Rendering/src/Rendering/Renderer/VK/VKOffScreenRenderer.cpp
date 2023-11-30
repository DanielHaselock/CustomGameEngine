#include "Rendering/Renderer/VK/VKOffScreenRenderer.h"
#include "Rendering/Renderer/VK/VKInitializers.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Resources/Loader/ImageLoader.h"
#include <array>

using namespace Rendering::Renderer::VK;

VKOffScreenRenderer::VKOffScreenRenderer(unsigned int pWidth, unsigned int pHeight)
{
	if (mDriver.mInstance == nullptr || mDriver.mDevice == nullptr || mDriver.mPhysicalDevice == nullptr)
		return;

	{
		std::unique_lock lock(IRenderer::mInstanceLock);
		IRenderer::mInstance++;
	}

    mWindowExtent = { pWidth , pHeight };

    initGraphicsQueue();
    createCommandPool();
    createFrameBufferAttachement();
    createRenderPass();
    createFrameBuffer();
}

VKOffScreenRenderer::~VKOffScreenRenderer()
{
    cleanupSwapChain();
    mMainDeletionQueue.flush();

    std::unique_lock lock(IRenderer::mInstanceLock);
    IRenderer::mInstance--;
}

void VKOffScreenRenderer::recreateSwapChain(unsigned int pWidth, unsigned int pHeight)
{
    mWindowExtent = { pWidth, pHeight };

    {
        std::unique_lock lock(IRenderer::mInstanceLock);
        vkDeviceWaitIdle(mDriver.mDevice);
    }

    cleanupSwapChain();

    createFrameBufferAttachement();
    createFrameBuffer();
}

void VKOffScreenRenderer::cleanupSwapChain()
{
    vkDestroyFramebuffer(mDriver.mDevice, mFramebuffer, nullptr);
    
    vkDestroyImageView(mDriver.mDevice, mColorImageView, nullptr);
    vmaDestroyImage(mAllocator, mColorImage.mImage, mColorImage.mAllocation);

    vkDestroyImageView(mDriver.mDevice, mDepthImageView, nullptr);
    vmaDestroyImage(mAllocator, mDepthImage.mImage, mDepthImage.mAllocation);
}

void VKOffScreenRenderer::initGraphicsQueue()
{
    float defaultQueuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mDriver.mPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mDriver.mPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
    {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            mGraphicsQueueFamily = i;
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = i;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
            break;
        }
    }

    vkGetDeviceQueue(mDriver.mDevice, mGraphicsQueueFamily, 0, &mGraphicsQueue);
}

void VKOffScreenRenderer::createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolInfo = VKInit::commandPoolCreateInfo(mGraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    vkCreateCommandPool(mDriver.mDevice, &commandPoolInfo, nullptr, &mFrame.mCommandPool);

    VkCommandBufferAllocateInfo cmdAllocInfo = VKInit::commandBufferAllocateInfo(mFrame.mCommandPool, 1);
    vkAllocateCommandBuffers(mDriver.mDevice, &cmdAllocInfo, &mFrame.mMainCommandBuffer);

    mMainDeletionQueue.pushFunction([=]()
    {
        vkDestroyCommandPool(mDriver.mDevice, mFrame.mCommandPool, nullptr);
    });
}

void VKOffScreenRenderer::createFrameBufferAttachement()
{
    VkExtent3D ImageExtent =
    {
        mWindowExtent.width,
        mWindowExtent.height,
        1
    };

    VkImageCreateInfo cimg_info = VKInit::imageCreateInfo(mColorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, ImageExtent);
    
    VmaAllocationCreateInfo cimg_allocinfo = {};
    cimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    cimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(mAllocator, &cimg_info, &cimg_allocinfo, &mColorImage.mImage, &mColorImage.mAllocation, nullptr);

    VkImageViewCreateInfo cview_info = VKInit::imageviewCreateInfo(mColorFormat, mColorImage.mImage, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(mDriver.mDevice, &cview_info, nullptr, &mColorImageView);

    VkImageCreateInfo dimg_info = VKInit::imageCreateInfo(mDepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, ImageExtent);

    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(mAllocator, &dimg_info, &dimg_allocinfo, &mDepthImage.mImage, &mDepthImage.mAllocation, nullptr);

    VkImageViewCreateInfo dview_info = VKInit::imageviewCreateInfo(mDepthFormat, mDepthImage.mImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    vkCreateImageView(mDriver.mDevice, &dview_info, nullptr, &mDepthImageView);
}

void VKOffScreenRenderer::createRenderPass()
{
    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = mColorFormat;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = mDepthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(mDriver.mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
        throw std::runtime_error("failed to create render pass!");

    mMainDeletionQueue.pushFunction([=]()
    {
        vkDestroyRenderPass(mDriver.mDevice, mRenderPass, nullptr);
    });
}

void VKOffScreenRenderer::createFrameBuffer()
{
    VkImageView attachments[2];
    attachments[0] = mColorImageView;
    attachments[1] = mDepthImageView;

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = mRenderPass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = mWindowExtent.width;
    framebufferCreateInfo.height = mWindowExtent.height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(mDriver.mDevice, &framebufferCreateInfo, nullptr, &mFramebuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create framebuffer!");
}

void VKOffScreenRenderer::beginFrame()
{
    //Begin draw clear
    VkCommandBufferBeginInfo cmdBeginInfo = VKInit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkBeginCommandBuffer(mFrame.mMainCommandBuffer, &cmdBeginInfo);

    VkClearValue clearValue;
    clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;

    VkClearValue clearValues[] = { clearValue, depthClear };

    VkRenderPassBeginInfo rpInfo = VKInit::renderpassBeginInfo(mRenderPass, mWindowExtent, mFramebuffer);
    rpInfo.clearValueCount = 2;
    rpInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(mFrame.mMainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

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

    vkCmdSetViewport(mFrame.mMainCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(mFrame.mMainCommandBuffer, 0, 1, &scissor);
    vkCmdSetDepthBias(mFrame.mMainCommandBuffer, 0, 0, 0);
}

void VKOffScreenRenderer::endFrame()
{
    std::unique_lock lock(IRenderer::mInstanceLock);

    vkCmdEndRenderPass(mFrame.mMainCommandBuffer);
    vkEndCommandBuffer(mFrame.mMainCommandBuffer);

   
    VkSubmitInfo submitInfo = VKInit::submitInfo(&mFrame.mMainCommandBuffer);
    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        
    vkQueueWaitIdle(mGraphicsQueue);
}

VkCommandBuffer VKOffScreenRenderer::getCurrentCommandBuffer()
{
    return mFrame.mMainCommandBuffer;
}



const void* VKOffScreenRenderer::pixelToArray()
{
    const char* imagedata;
    
    VkImage dstImage = nullptr;
    VkDeviceMemory dstImageMemory = nullptr;

    createImage(mWindowExtent.width, mWindowExtent.height, 1, 1,
        mColorFormat,
        VK_IMAGE_TILING_LINEAR,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT, 0,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        dstImage, dstImageMemory);

    mMainDeletionQueue.pushFunction([=]()
    {
            vkUnmapMemory(mDriver.mDevice, dstImageMemory);
            vkFreeMemory(mDriver.mDevice, dstImageMemory, nullptr);
            vkDestroyImage(mDriver.mDevice, dstImage, nullptr);
    });


    Rendering::Renderer::VK::VKRenderer::immediateSubmit([=](VkCommandBuffer cmd)
    {
         // Transition destination image to transfer destination layout
        insertImageMemoryBarrier(
            cmd,
            dstImage,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        // colorAttachment.image is already in VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be transitioned
        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = mWindowExtent.width;
        imageCopyRegion.extent.height = mWindowExtent.height;
        imageCopyRegion.extent.depth = 1;

        vkCmdCopyImage(
            cmd,
            mColorImage.mImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageCopyRegion);

        // Transition destination image to general layout, which is the required layout for mapping the image memory later on
        insertImageMemoryBarrier(
            cmd,
            dstImage,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
    });

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    
    vkGetImageSubresourceLayout(mDriver.mDevice, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(mDriver.mDevice, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    return imagedata;
}

void VKOffScreenRenderer::saveToPNG(const char* pFile)
{
    Rendering::Resources::Loaders::ImageLoader::exportPNG(pFile, subResourceLayout.rowPitch / 4, mWindowExtent.height, 4, pixelToArray());
}

void VKOffScreenRenderer::saveToJPG(const char* pFile)
{
    Rendering::Resources::Loaders::ImageLoader::exportJPG(pFile, subResourceLayout.rowPitch / 4, mWindowExtent.height, 4, pixelToArray());
}