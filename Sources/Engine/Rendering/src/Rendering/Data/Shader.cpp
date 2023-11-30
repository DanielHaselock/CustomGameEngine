#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Data/Shader.h"
#include "Rendering/Renderer/VK/VKInitializers.h"
#include "Rendering/Resources/Loader/ShaderLoader.h"
#include <spirv_reflect.h>
#include <cassert>
#include <algorithm>
#include <iostream>

using namespace Rendering::Data;

Rendering::Data::ShaderModule* ShaderCache::getShader(const std::string& path)
{
	auto it = mModuleCache.find(path);
	if (it == mModuleCache.end())
	{
		Rendering::Data::ShaderModule newShader;

		bool result = Rendering::Resources::Loaders::ShaderLoader::loadModule(path.c_str(), &newShader);
		if (!result)
		{
			std::cout << "Error when compiling shader " << path << std::endl;
			return nullptr;
		}

		mModuleCache[path] = newShader;
	}
	return &mModuleCache[path];
}

Shader::~Shader()
{
	vkDestroyPipelineLayout(service(VkDevice), mBuiltLayout, nullptr);

	for (int i = 0; i < mSetLayouts.size(); i++)
		if (mSetLayouts[i] != nullptr)
			vkDestroyDescriptorSetLayout(service(VkDevice), mSetLayouts[i], nullptr);
}

void Shader::addStage(Rendering::Data::ShaderModule* pShaderModule, VkShaderStageFlagBits pStage)
{
	ShaderStage newStage = { pShaderModule, pStage };
	mStages.push_back(newStage);
}

struct DescriptorSetLayoutData
{
	uint32_t mSetNumber;
	VkDescriptorSetLayoutCreateInfo mCreateInfo;
	std::vector<VkDescriptorSetLayoutBinding> mBindings;
};

void Shader::reflectLayout()
{
	std::vector<DescriptorSetLayoutData> set_layouts;

	std::vector<VkPushConstantRange> constant_ranges;
	for (auto& s : mStages) {

		SpvReflectShaderModule spvmodule;
		SpvReflectResult result = spvReflectCreateShaderModule(s.mShaderModule->mCode.size() * sizeof(uint32_t), s.mShaderModule->mCode.data(), &spvmodule);

		uint32_t count = 0;
		result = spvReflectEnumerateDescriptorSets(&spvmodule, &count, NULL);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectDescriptorSet*> sets(count);
		result = spvReflectEnumerateDescriptorSets(&spvmodule, &count, sets.data());
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		for (size_t i_set = 0; i_set < sets.size(); ++i_set) {

			const SpvReflectDescriptorSet& refl_set = *(sets[i_set]);

			DescriptorSetLayoutData layout = {};

			layout.mBindings.resize(refl_set.binding_count);
			for (uint32_t i_binding = 0; i_binding < refl_set.binding_count; ++i_binding)
			{
				const SpvReflectDescriptorBinding& refl_binding = *(refl_set.bindings[i_binding]);
				VkDescriptorSetLayoutBinding& layout_binding = layout.mBindings[i_binding];
				layout_binding.binding = refl_binding.binding;
				layout_binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);

				layout_binding.descriptorCount = 1;
				for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim)
					layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];

				layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(spvmodule.shader_stage);

				ReflectedBinding reflected{};
				reflected.mBinding = layout_binding.binding;
				reflected.mSet = refl_set.set;
				reflected.mType = layout_binding.descriptorType;

				mBindings[refl_binding.name] = reflected;
			}
			layout.mSetNumber = refl_set.set;
			layout.mCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout.mCreateInfo.bindingCount = refl_set.binding_count;
			layout.mCreateInfo.pBindings = layout.mBindings.data();

			set_layouts.push_back(layout);
		}

		//pushconstants	
		result = spvReflectEnumeratePushConstantBlocks(&spvmodule, &count, NULL);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		std::vector<SpvReflectBlockVariable*> pconstants(count);
		result = spvReflectEnumeratePushConstantBlocks(&spvmodule, &count, pconstants.data());
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		if (count > 0) {
			VkPushConstantRange pcs{};
			pcs.offset = pconstants[0]->offset;
			pcs.size = pconstants[0]->size;
			pcs.stageFlags = s.mStage;

			constant_ranges.push_back(pcs);
		}
	}

	std::array<DescriptorSetLayoutData, 8> merged_layouts;

	for (int i = 0; i < 8; i++) {

		DescriptorSetLayoutData& ly = merged_layouts[i];

		ly.mSetNumber = i;

		ly.mCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		std::unordered_map<int, VkDescriptorSetLayoutBinding> binds;
		for (auto& s : set_layouts)
		{
			if (s.mSetNumber == i)
			{
				for (auto& b : s.mBindings)
				{
					auto it = binds.find(b.binding);
					if (it == binds.end())
						binds[b.binding] = b;
					else
						binds[b.binding].stageFlags |= b.stageFlags;
				}
			}
		}
		for (auto& [k, v] : binds)
			ly.mBindings.push_back(v);

		//sort the bindings, for hash purposes
		std::sort(ly.mBindings.begin(), ly.mBindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b)
			{
				return a.binding < b.binding;
			});


		ly.mCreateInfo.bindingCount = (uint32_t)ly.mBindings.size();
		ly.mCreateInfo.pBindings = ly.mBindings.data();
		ly.mCreateInfo.flags = 0;
		ly.mCreateInfo.pNext = 0;


		if (ly.mCreateInfo.bindingCount > 0)
		{
			mSetHashes[i] = Rendering::Resources::Loaders::ShaderLoader::hashDescriptorLayoutInfo(&ly.mCreateInfo);
			vkCreateDescriptorSetLayout(service(VkDevice), &ly.mCreateInfo, nullptr, &mSetLayouts[i]);
		}
		else {
			mSetHashes[i] = 0;
			mSetLayouts[i] = VK_NULL_HANDLE;
		}
	}

	//we start from just the default empty pipeline layout info
	VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = Rendering::Renderer::VK::VKInit::pipelineLayoutCreateInfo();

	mesh_pipeline_layout_info.pPushConstantRanges = constant_ranges.data();
	mesh_pipeline_layout_info.pushConstantRangeCount = (uint32_t)constant_ranges.size();

	std::array<VkDescriptorSetLayout, 8> compactedLayouts{};
	int s = 0;
	for (int i = 0; i < 8; i++) {
		if (mSetLayouts[i] != VK_NULL_HANDLE) {
			compactedLayouts[s] = mSetLayouts[i];
			s++;
		}
	}

	mesh_pipeline_layout_info.setLayoutCount = s;
	mesh_pipeline_layout_info.pSetLayouts = compactedLayouts.data();


	vkCreatePipelineLayout(service(VkDevice), &mesh_pipeline_layout_info, nullptr, &mBuiltLayout);
}

void Shader::fillStages(std::vector<VkPipelineShaderStageCreateInfo>& pipelineStages)
{
	for (auto& s : mStages)
		pipelineStages.push_back(Rendering::Renderer::VK::VKInit::pipelineShaderStageCreateInfo(s.mStage, s.mShaderModule->mModule));
}