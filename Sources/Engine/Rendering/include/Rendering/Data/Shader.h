#pragma once
#include <unordered_map>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>
#include <string>
namespace Rendering::Data
{    
	struct ShaderModule
    {
        std::vector<uint32_t> mCode;
        VkShaderModule mModule;
    };

	struct ShaderCache
	{
		public:
			std::unordered_map<std::string, Rendering::Data::ShaderModule> mModuleCache;
			Rendering::Data::ShaderModule* getShader(const std::string& pPpath);
	};

	struct Shader
	{
		struct ReflectedBinding
		{
			uint32_t mSet;
			uint32_t mBinding;
			VkDescriptorType mType;
		};

		struct ShaderStage
		{
			Rendering::Data::ShaderModule* mShaderModule;
			VkShaderStageFlagBits mStage;
		};

		VkPipelineLayout mBuiltLayout = nullptr;
		std::unordered_map<std::string, ReflectedBinding> mBindings;
		std::array<VkDescriptorSetLayout, 8> mSetLayouts = { nullptr };
		std::array<uint32_t, 8> mSetHashes;
		std::vector<ShaderStage> mStages;

		~Shader();
		void addStage(Rendering::Data::ShaderModule* pShaderModule, VkShaderStageFlagBits pStage);
		void reflectLayout();
		void fillStages(std::vector<VkPipelineShaderStageCreateInfo>& pPipelineStages);
	};
}