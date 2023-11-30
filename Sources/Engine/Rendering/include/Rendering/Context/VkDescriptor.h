#pragma once
#include <vulkan/vulkan_core.h>
#include <unordered_map>
#include "mutex"

namespace Rendering::Context
{
	class DescriptorAllocator 
	{
	public:
		static std::mutex instance;
		struct PoolSizes 
		{
			std::vector<std::pair<VkDescriptorType, float>> mSizes =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
			};
		};

		~DescriptorAllocator();

		VkDescriptorPool mCurrentPool{ VK_NULL_HANDLE };
		PoolSizes mDescriptorSizes;
		std::vector<VkDescriptorPool> mUsedPools;
		std::vector<VkDescriptorPool> mFreePools;

		void resetPools();
		bool allocate(VkDescriptorSet* pSet, VkDescriptorSetLayout pLayout);
		void cleanup();
		VkDescriptorPool grabPool();
	};

	class DescriptorLayoutCache 
	{
		public:
			struct DescriptorLayoutInfo
			{
				std::vector<VkDescriptorSetLayoutBinding> mBindings;
				bool operator==(const DescriptorLayoutInfo& mOther) const;
				size_t hash() const;
			};


			struct DescriptorLayoutHash
			{

				std::size_t operator()(const DescriptorLayoutInfo& pK) const
				{
					return pK.hash();
				}
			};

			~DescriptorLayoutCache();

			std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> mLayoutCache;

			void cleanup();

			VkDescriptorSetLayout createDescriptorLayout(VkDescriptorSetLayoutCreateInfo* pInfo);

			
	};

	class DescriptorBuilder 
	{
		public:
			std::vector<VkWriteDescriptorSet> mWrites;
			std::vector<VkDescriptorSetLayoutBinding> mBindings;

			DescriptorLayoutCache* mCache;
			DescriptorAllocator* mAlloc;


			static DescriptorBuilder begin(DescriptorLayoutCache* pLayoutCache, DescriptorAllocator* pAllocator);

			VkDescriptorSetLayoutBinding DescriptorBuilder::descriptorsetLayoutBinding(VkDescriptorType pType, VkShaderStageFlags pStageFlags, uint32_t pBinding);
			VkWriteDescriptorSet writeDescriptorBuffer(VkDescriptorType pType, VkDescriptorSet pSet, VkDescriptorBufferInfo* pInfo, uint32_t pBinding);

			DescriptorBuilder& bind_buffer(uint32_t pBinding, VkDescriptorBufferInfo* pBufferInfo, VkDescriptorType pType, VkShaderStageFlags pStageFlags);

			DescriptorBuilder& bind_image(uint32_t pBinding, VkDescriptorImageInfo* pImageInfo, VkDescriptorType pType, VkShaderStageFlags pStageFlags);
			DescriptorBuilder& bind_images(uint32_t pSize, uint32_t pBinding, VkDescriptorImageInfo* pImageInfo, VkDescriptorType pType, VkShaderStageFlags pStageFlags);

			bool build(VkDescriptorSet& pSet, VkDescriptorSetLayout& pLayout);
			bool build(VkDescriptorSet& pSet);
	};


	struct DescriptorCache
	{
		static DescriptorLayoutCache mDescriptorLayoutCache;
		static DescriptorAllocator mDescriptorAllocator;

		static DescriptorBuilder begin();
	};
}		