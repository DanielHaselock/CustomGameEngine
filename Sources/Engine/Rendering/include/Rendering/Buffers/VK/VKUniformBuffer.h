#pragma once
#include <vulkan/vulkan.h>
#include "Rendering/Data/VKTypes.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Buffers/VK/VKBuffer.h"
#include "Rendering/Context/VkDescriptor.h"

namespace Rendering::Buffers::VK
{
	template <class T> class VKUniformBuffer
	{
		public:
			Rendering::Data::AllocatedBuffer mBuffer;
			VkDescriptorSet mDescriptorSets;
			unsigned int mSize;
			T mData;
			std::mutex mMutex;

			VKUniformBuffer(VkShaderStageFlags pStage) : mSize(sizeof(T))
			{
				mBuffer = Rendering::Buffers::VK::VKBuffer::createBuffer(mSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = mBuffer.mBuffer;
				bufferInfo.offset = 0;
				bufferInfo.range = mSize;

				Context::DescriptorCache::begin()
					.bind_buffer(0, &bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, pStage)
					.build(mDescriptorSets);
			}

			~VKUniformBuffer()
			{
				vmaDestroyBuffer(service(VmaAllocator), mBuffer.mBuffer, mBuffer.mAllocation);
			};

			void updateData()
			{
				std::unique_lock lock(mMutex);

				void* data;
				vmaMapMemory(service(VmaAllocator), mBuffer.mAllocation, &data);
				memcpy(data, &mData, mSize);
				vmaUnmapMemory(service(VmaAllocator), mBuffer.mAllocation);
			}
	};
}