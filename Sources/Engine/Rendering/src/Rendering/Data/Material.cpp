#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Renderer/VK/VKInitializers.h"
#include "Rendering/Context/VkDescriptor.h"
#include <algorithm>

using namespace Rendering::Data;

Material::~Material()
{
	delete mShader;
}

void Material::bindBuffer(const char* name, const VkDescriptorBufferInfo& pBufferInfo)
{
	bindDynamicBuffer(name, -1, pBufferInfo);
}

void Material::bindDynamicBuffer(const char* pName, uint32_t pOffset, const VkDescriptorBufferInfo& pBufferInfo)
{
	auto found = mShader->mBindings.find(pName);
	if (found == mShader->mBindings.end())
		return;
	
	const Shader::ReflectedBinding& bind = (*found).second;
	
	for (BufferWriteDescriptor& write : mBufferWrites)
	{
		if (write.mDstBinding == bind.mBinding && write.mDstSet == bind.mSet)
		{
			if (write.mBufferInfo.buffer != pBufferInfo.buffer ||
				write.mBufferInfo.range != pBufferInfo.range ||
				write.mBufferInfo.offset != pBufferInfo.offset) 
			{
				write.mBufferInfo = pBufferInfo;
				write.mDynamicOffset = pOffset;
	
				mCachedDescriptorSets[write.mDstSet].first = VK_NULL_HANDLE;
			}
			else
				write.mDynamicOffset = pOffset;
	
			return;
		}
	}
	
	BufferWriteDescriptor newWrite;
	newWrite.mDstSet = bind.mSet;
	newWrite.mDstBinding = bind.mBinding;
	newWrite.mDescriptorType = bind.mType;
	newWrite.mBufferInfo = pBufferInfo;
	newWrite.mDynamicOffset = pOffset;
	
	
	mCachedDescriptorSets[bind.mSet].first = VK_NULL_HANDLE;
	
	mBufferWrites.push_back(newWrite);
}

void Material::bindImage(const char* pName, const VkDescriptorImageInfo& pImageInfo)
{
	auto found = mShader->mBindings.find(pName);
	if (found != mShader->mBindings.end())
	{

		const Shader::ReflectedBinding& bind = (*found).second;

		for (auto& write : mBufferWrites)
		{
			if (write.mDstBinding == bind.mBinding
				&& write.mDstSet == bind.mSet)
			{
				if (write.mImageInfo.imageLayout != pImageInfo.imageLayout ||
					write.mImageInfo.imageView != pImageInfo.imageView ||
					write.mImageInfo.sampler != pImageInfo.sampler
					)
				{
					write.mImageInfo = pImageInfo;
					write.mDynamicOffset = -1;

					mCachedDescriptorSets[write.mDstSet].first = VK_NULL_HANDLE;
				}
				else
					write.mDynamicOffset = -1;

				return;
			}
		}

		BufferWriteDescriptor newWrite;
		newWrite.mDstSet = bind.mSet;
		newWrite.mDstBinding = bind.mBinding;
		newWrite.mDescriptorType = bind.mType;
		newWrite.mImageInfo = pImageInfo;
		newWrite.mDynamicOffset = -1;

		mCachedDescriptorSets[bind.mSet].first = VK_NULL_HANDLE;

		mBufferWrites.push_back(newWrite);
	}
}

void Material::buildSets()
{
	std::array<std::vector<VkWriteDescriptorSet>, 8> writes{};

	std::sort(mBufferWrites.begin(), mBufferWrites.end(), [](BufferWriteDescriptor& a, BufferWriteDescriptor& b) {
		if (b.mDstSet == a.mDstSet)
			return a.mDstSet < b.mDstSet;
		else
			return a.mDstBinding < b.mDstBinding;
	});	
	
	//reset the dynamic offsets
	for (auto& s : mSetOffsets)
		s.count = 0;

	for (BufferWriteDescriptor& w : mBufferWrites) {
		uint32_t set = w.mDstSet;
		VkWriteDescriptorSet write;
		if (w.mBufferInfo.buffer != nullptr)
			write = Rendering::Renderer::VK::VKInit::writeDescriptorBuffer(w.mDescriptorType, VK_NULL_HANDLE, &w.mBufferInfo, w.mDstBinding);
		else
			write = Rendering::Renderer::VK::VKInit::writeDescriptorImage(w.mDescriptorType, VK_NULL_HANDLE, &w.mImageInfo, w.mDstBinding);

		writes[set].push_back(write);

		//dynamic offsets
		if (w.mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC || w.mDescriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
			DynOffsets& offsetSet = mSetOffsets[set];
			offsetSet.offsets[offsetSet.count] = w.mDynamicOffset;
			offsetSet.count++;
		}
	}


	for (int i = 0; i < 8; i++) {
		//there are writes for this set
		if (writes[i].size() > 0) {

			if (mCachedDescriptorSets[i].first == VK_NULL_HANDLE) {
				//alloc
				auto layout = mShader->mSetLayouts[i];

				VkDescriptorSet newDescriptor;
				Rendering::Context::DescriptorCache::mDescriptorAllocator.allocate(&newDescriptor, layout);

				for (auto& w : writes[i])
					w.dstSet = newDescriptor;

				vkUpdateDescriptorSets(service(VkDevice), (uint32_t)writes[i].size(), writes[i].data(), 0, nullptr);

				mCachedDescriptorSets[i].first = newDescriptor;
			}
		}
	}
}


void Material::bindDescriptor(const char* pName, const VkDescriptorSet& pImageInfo, const int pSize)
{
	if (mShader == nullptr)
		return;

	auto found = mShader->mBindings.find(pName);
	if (found == mShader->mBindings.end())
		return;

	const Shader::ReflectedBinding& bind = (*found).second;
	if (mCachedDescriptorSets[bind.mSet].first != pImageInfo)
	{
		mCachedDescriptorSets[bind.mSet].first = pImageInfo;
		mCachedDescriptorSets[bind.mSet].second = pSize;
	}
}

void Material::bindConstant(VkCommandBuffer pCmd, VkShaderStageFlags pStage, int pOffSet, int size, const void* pData)
{
	vkCmdPushConstants(pCmd, mShader->mBuiltLayout, pStage, pOffSet, size, pData);
}

void Material::applyBinds(VkCommandBuffer cmd)
{
	for (int i = 0; i < mCachedDescriptorSets.size(); i++)
		if (mCachedDescriptorSets[i].first != VK_NULL_HANDLE)
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mShader->mBuiltLayout, i, mCachedDescriptorSets[i].second, &mCachedDescriptorSets[i].first, mSetOffsets[i].count, mSetOffsets[i].offsets.data());
}

void Material::bindPipeLine(VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
	applyBinds(cmd);
}