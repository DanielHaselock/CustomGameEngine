#include "Rendering/Buffers/VK/VKBuffer.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Renderer/VK/VKRenderer.h"

using namespace Rendering::Buffers::VK;

Rendering::Data::AllocatedBuffer VKBuffer::createBuffer(size_t pAllocSize, VkBufferUsageFlags pUsage, VmaMemoryUsage pMemoryUsage)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.size = pAllocSize;
	bufferInfo.usage = pUsage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = pMemoryUsage;

	Rendering::Data::AllocatedBuffer newBuffer;
	vmaCreateBuffer(service(VmaAllocator), &bufferInfo, &vmaallocInfo,
		&newBuffer.mBuffer,
		&newBuffer.mAllocation,
		nullptr);

	return newBuffer;
}

void VKBuffer::copyBuffer(size_t pAllocSize, VkBuffer pSrc, VkBuffer pDest)
{
	Rendering::Renderer::VK::VKRenderer::immediateSubmit([=](VkCommandBuffer cmd)
	{
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = pAllocSize;
		vkCmdCopyBuffer(cmd, pSrc, pDest, 1, &copy);
	});
}

void VKBuffer::mapBuffer(const void* pData, size_t pAllocSize, VmaAllocation pAllocation)
{
	void* data;
	vmaMapMemory(service(VmaAllocator), pAllocation, &data);
	memcpy(data, pData, pAllocSize);
	vmaUnmapMemory(service(VmaAllocator), pAllocation);
}