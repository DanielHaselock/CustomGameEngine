#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Context/VkDescriptor.h"
#include <algorithm>

using namespace Rendering::Context;

std::mutex DescriptorAllocator::instance;

VkDescriptorPool createPool(VkDevice pDevice, const DescriptorAllocator::PoolSizes& pPoolSizes, int pCount, VkDescriptorPoolCreateFlags pFlags)
{
	std::vector<VkDescriptorPoolSize> sizes;
	sizes.reserve(pPoolSizes.mSizes.size());
	for (auto sz : pPoolSizes.mSizes) {
		sizes.push_back({ sz.first, uint32_t(sz.second * pCount) });
	}
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = pFlags;
	pool_info.maxSets = pCount;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	VkDescriptorPool descriptorPool;
	vkCreateDescriptorPool(pDevice, &pool_info, nullptr, &descriptorPool);

	return descriptorPool;
}

DescriptorAllocator::~DescriptorAllocator()
{
	cleanup();
}

void DescriptorAllocator::resetPools()
{
	for (auto p : mUsedPools)
	{
		vkResetDescriptorPool(service(VkDevice), p, 0);
	}

	mFreePools = mUsedPools;
	mUsedPools.clear();
	mCurrentPool = VK_NULL_HANDLE;
}

bool DescriptorAllocator::allocate(VkDescriptorSet* pSet, VkDescriptorSetLayout pLayout)
{
	if (mCurrentPool == VK_NULL_HANDLE)
	{
		mCurrentPool = grabPool();
		mUsedPools.push_back(mCurrentPool);
	}

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;

	allocInfo.pSetLayouts = &pLayout;
	allocInfo.descriptorPool = mCurrentPool;
	allocInfo.descriptorSetCount = 1;

	VkResult allocResult;

	{	
		std::unique_lock lock(instance);
		allocResult = vkAllocateDescriptorSets(service(VkDevice), &allocInfo, pSet);
	}
	bool needReallocate = false;

	switch (allocResult) {
	case VK_SUCCESS:
		//all good, return
		return true;

		break;
	case VK_ERROR_FRAGMENTED_POOL:
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		//reallocate pool
		needReallocate = true;
		break;
	default:
		//unrecoverable error
		return false;
	}

	if (needReallocate)
	{
		//allocate a new pool and retry
		mCurrentPool = grabPool();
		mUsedPools.push_back(mCurrentPool);

		{
			std::unique_lock lock(instance);
			allocResult = vkAllocateDescriptorSets(service(VkDevice), &allocInfo, pSet);
		}

		//if it still fails then we have big issues
		if (allocResult == VK_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

void DescriptorAllocator::cleanup()
{
	for (auto p : mFreePools)
		vkDestroyDescriptorPool(service(VkDevice), p, nullptr);
	mFreePools.clear();

	for (auto p : mUsedPools)
		vkDestroyDescriptorPool(service(VkDevice), p, nullptr);
	mUsedPools.clear();
}

VkDescriptorPool DescriptorAllocator::grabPool()
{
	if (mFreePools.size() > 0)
	{
		VkDescriptorPool pool = mFreePools.back();
		mFreePools.pop_back();
		return pool;
	}
	else
		return createPool(service(VkDevice), mDescriptorSizes, 100000, 0);
}

VkDescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(VkDescriptorSetLayoutCreateInfo* pInfo)
{
	DescriptorLayoutInfo layoutinfo;
	layoutinfo.mBindings.reserve(pInfo->bindingCount);
	bool isSorted = true;
	int32_t lastBinding = -1;
	for (uint32_t i = 0; i < pInfo->bindingCount; i++) {
		layoutinfo.mBindings.push_back(pInfo->pBindings[i]);

		//check that the bindings are in strict increasing order
		if (static_cast<int32_t>(pInfo->pBindings[i].binding) > lastBinding)
		{
			lastBinding = pInfo->pBindings[i].binding;
		}
		else {
			isSorted = false;
		}
	}
	if (!isSorted)
	{
		std::sort(layoutinfo.mBindings.begin(), layoutinfo.mBindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
			return a.binding < b.binding;
			});
	}

	auto it = mLayoutCache.find(layoutinfo);
	if (it != mLayoutCache.end())
	{
		return (*it).second;
	}
	else {
		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(service(VkDevice), pInfo, nullptr, &layout);

		//layoutCache.emplace()
		//add to cache
		mLayoutCache[layoutinfo] = layout;
		return layout;
	}
}

void DescriptorLayoutCache::cleanup()
{
	if (service(VkDevice) == nullptr)
		return;
	for (auto pair : mLayoutCache)
		vkDestroyDescriptorSetLayout(service(VkDevice), pair.second, nullptr);
	mLayoutCache.clear();
}

DescriptorLayoutCache::~DescriptorLayoutCache()
{
	cleanup();
}

DescriptorBuilder DescriptorBuilder::begin(DescriptorLayoutCache* pLayoutCache, DescriptorAllocator* pAllocator)
{
	DescriptorBuilder builder;

	builder.mCache = pLayoutCache;
	builder.mAlloc = pAllocator;
	return builder;
}

VkDescriptorSetLayoutBinding DescriptorBuilder::descriptorsetLayoutBinding(VkDescriptorType pType, VkShaderStageFlags pStageFlags, uint32_t pBinding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.descriptorCount = 1;
	setbind.descriptorType = pType;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = pStageFlags;
	setbind.binding = pBinding;

	mBindings.push_back(setbind);

	return setbind;
}

VkWriteDescriptorSet DescriptorBuilder::writeDescriptorBuffer(VkDescriptorType pType, VkDescriptorSet pDstSet, VkDescriptorBufferInfo* pBufferInfo, uint32_t pBinding)
{
	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = pBinding;
	write.dstSet = pDstSet;
	write.descriptorCount = 1;
	write.descriptorType = pType;
	write.pBufferInfo = pBufferInfo;

	mWrites.push_back(write);
	return write;
}

DescriptorBuilder& DescriptorBuilder::bind_buffer(uint32_t pBinding, VkDescriptorBufferInfo* pBufferInfo, VkDescriptorType pType, VkShaderStageFlags pStageFlags)
{
	VkDescriptorSetLayoutBinding newBinding{};

	newBinding.descriptorCount = 1;
	newBinding.descriptorType = pType;
	newBinding.pImmutableSamplers = nullptr;
	newBinding.stageFlags = pStageFlags;
	newBinding.binding = pBinding;

	mBindings.push_back(newBinding);

	VkWriteDescriptorSet newWrite{};
	newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWrite.pNext = nullptr;

	newWrite.descriptorCount = 1;
	newWrite.descriptorType = pType;
	newWrite.pBufferInfo = pBufferInfo;
	newWrite.dstBinding = pBinding;

	mWrites.push_back(newWrite);
	return *this;
}

DescriptorBuilder& DescriptorBuilder::bind_image(uint32_t pBinding, VkDescriptorImageInfo* pImageInfo, VkDescriptorType pType, VkShaderStageFlags pStageFlags)
{
	VkDescriptorSetLayoutBinding newBinding{};

	newBinding.descriptorCount = 1;
	newBinding.descriptorType = pType;
	newBinding.pImmutableSamplers = nullptr;
	newBinding.stageFlags = pStageFlags;
	newBinding.binding = pBinding;

	mBindings.push_back(newBinding);

	VkWriteDescriptorSet newWrite{};
	newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWrite.pNext = nullptr;

	newWrite.descriptorCount = 1;
	newWrite.descriptorType = pType;
	newWrite.pImageInfo = pImageInfo;
	newWrite.dstBinding = pBinding;

	mWrites.push_back(newWrite);
	return *this;
}

DescriptorBuilder& DescriptorBuilder::bind_images(uint32_t pSize, uint32_t pBinding, VkDescriptorImageInfo* pImageInfo, VkDescriptorType pType, VkShaderStageFlags pStageFlags)
{
	VkDescriptorSetLayoutBinding newBinding{};

	newBinding.descriptorCount = pSize;
	newBinding.descriptorType = pType;
	newBinding.pImmutableSamplers = nullptr;
	newBinding.stageFlags = pStageFlags;
	newBinding.binding = pBinding;

	mBindings.push_back(newBinding);

	VkWriteDescriptorSet newWrite{};
	newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWrite.pNext = nullptr;

	newWrite.descriptorCount = pSize;
	newWrite.descriptorType = pType;
	newWrite.pImageInfo = pImageInfo;
	newWrite.dstBinding = pBinding;

	mWrites.push_back(newWrite);
	return *this;
}

bool DescriptorBuilder::build(VkDescriptorSet& pSet, VkDescriptorSetLayout& pLayout)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;

	layoutInfo.pBindings = mBindings.data();
	layoutInfo.bindingCount = static_cast<uint32_t>(mBindings.size());

	pLayout = mCache->createDescriptorLayout(&layoutInfo);


	//allocate descriptor
	bool success = mAlloc->allocate(&pSet, pLayout);
	if (!success) { return false; };

	//write descriptor

	for (VkWriteDescriptorSet& w : mWrites) {
		w.dstSet = pSet;
	}

	vkUpdateDescriptorSets(service(VkDevice), static_cast<uint32_t>(mWrites.size()), mWrites.data(), 0, nullptr);

	return true;
}

bool DescriptorBuilder::build(VkDescriptorSet& pSet)
{
	VkDescriptorSetLayout layout;
	return build(pSet, layout);
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& pOther) const
{
	if (pOther.mBindings.size() != mBindings.size())
		return false;
	else 
	{
		for (int i = 0; i < mBindings.size(); i++) 
		{
			if (pOther.mBindings[i].binding != mBindings[i].binding)
				return false;
			if (pOther.mBindings[i].descriptorType != mBindings[i].descriptorType)
				return false;
			if (pOther.mBindings[i].descriptorCount != mBindings[i].descriptorCount)
				return false;
			if (pOther.mBindings[i].stageFlags != mBindings[i].stageFlags)
				return false;
		}
		return true;
	}
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const
{
	using std::size_t;
	using std::hash;

	size_t result = hash<size_t>()(mBindings.size());

	for (const VkDescriptorSetLayoutBinding& b : mBindings)
	{
		//pack the binding data into a single int64. Not fully correct but its ok
		size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

		//shuffle the packed binding data and xor it with the main hash
		result ^= hash<size_t>()(binding_hash);
	}

	return result;
}


DescriptorLayoutCache DescriptorCache::mDescriptorLayoutCache;
DescriptorAllocator DescriptorCache::mDescriptorAllocator;

DescriptorBuilder DescriptorCache::begin()
{
	return DescriptorBuilder::begin(&mDescriptorLayoutCache, &mDescriptorAllocator);
}