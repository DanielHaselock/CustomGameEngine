#pragma once
#include "Rendering/Data/Shader.h"
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Data
{
	struct BufferWriteDescriptor
	{
		int mDstSet;
		int mDstBinding;
		VkDescriptorType mDescriptorType;
		VkDescriptorBufferInfo mBufferInfo{ nullptr, 0, 0 };
		VkDescriptorImageInfo mImageInfo{ nullptr, nullptr, VK_IMAGE_LAYOUT_UNDEFINED };
		uint32_t mDynamicOffset;
	};

	struct DynOffsets
	{
		std::array<uint32_t, 16> offsets;
		uint32_t count{ 0 };
	};

	struct Material
	{
		VkPipeline mPipeline;
		Shader* mShader = nullptr;

		std::vector<BufferWriteDescriptor> mBufferWrites;
		std::array<std::pair<VkDescriptorSet, int>, 8> mCachedDescriptorSets;
		std::array<DynOffsets, 8> mSetOffsets;

		~Material();
		void bindBuffer(const char* pName, const VkDescriptorBufferInfo& pBufferInfo);
		void bindDynamicBuffer(const char* pName, uint32_t pOffset, const VkDescriptorBufferInfo& pBufferInfo);
		void bindImage(const char* pName, const VkDescriptorImageInfo& pImageInfo);
		void buildSets();

		void bindDescriptor(const char* pName, const VkDescriptorSet& pImageInfo, const int pSize = 1);
		void bindConstant(VkCommandBuffer cmd, VkShaderStageFlags pStage, int pOffSet, int size, const void* pData);
		void applyBinds(VkCommandBuffer cmd);
		void bindPipeLine(VkCommandBuffer cmd);
	};
}