#include "Rendering/Buffers/VK/VKDynamicVertexBuffer.h"
#include "Rendering/Buffers/VK/VKBuffer.h"
#include "EngineCore/Service/ServiceLocator.h"

using namespace Rendering::Buffers::VK;

VKDynamicVertexBuffer::VKDynamicVertexBuffer(const void* pData, size_t pSize)
{
	const size_t bufferSize = pSize;
	Data::AllocatedBuffer stagingBuffer = Rendering::Buffers::VK::VKBuffer::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	Rendering::Buffers::VK::VKBuffer::mapBuffer(pData, pSize, stagingBuffer.mAllocation);
	mVertexBuffer = Rendering::Buffers::VK::VKBuffer::createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	Rendering::Buffers::VK::VKBuffer::copyBuffer(bufferSize, stagingBuffer.mBuffer, mVertexBuffer.mBuffer);
	vmaDestroyBuffer(service(VmaAllocator), stagingBuffer.mBuffer, stagingBuffer.mAllocation);
}

VKDynamicVertexBuffer::~VKDynamicVertexBuffer()
{
	vmaDestroyBuffer(service(VmaAllocator), mVertexBuffer.mBuffer, mVertexBuffer.mAllocation);
}

void VKDynamicVertexBuffer::bind(void* pCommandBuffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers((VkCommandBuffer)pCommandBuffer, 0, 1, &mVertexBuffer.mBuffer, &offset);
}

void VKDynamicVertexBuffer::unbind()
{
}