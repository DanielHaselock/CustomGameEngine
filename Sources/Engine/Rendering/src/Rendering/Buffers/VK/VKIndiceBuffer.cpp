#include "Rendering/Buffers/VK/VKIndiceBuffer.h"
#include "Rendering/Buffers/VK/VKBuffer.h"
#include "EngineCore/Service/ServiceLocator.h"

using namespace Rendering::Buffers::VK;

VKIndiceBuffer::VKIndiceBuffer(const void* pData, size_t pSize)
{
	const size_t bufferSize = pSize;
	Data::AllocatedBuffer stagingBuffer = Rendering::Buffers::VK::VKBuffer::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	Rendering::Buffers::VK::VKBuffer::mapBuffer(pData, pSize, stagingBuffer.mAllocation);
	mIndicesBuffer = Rendering::Buffers::VK::VKBuffer::createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	Rendering::Buffers::VK::VKBuffer::copyBuffer(bufferSize, stagingBuffer.mBuffer, mIndicesBuffer.mBuffer);
	vmaDestroyBuffer(service(VmaAllocator), stagingBuffer.mBuffer, stagingBuffer.mAllocation);
}

VKIndiceBuffer::~VKIndiceBuffer()
{
	vmaDestroyBuffer(service(VmaAllocator), mIndicesBuffer.mBuffer, mIndicesBuffer.mAllocation);
}

void VKIndiceBuffer::bind(void* pCommandBuffer)
{
	vkCmdBindIndexBuffer((VkCommandBuffer)pCommandBuffer, mIndicesBuffer.mBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void VKIndiceBuffer::unbind()
{
}