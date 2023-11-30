#include "Rendering/Resources/Loader/ShaderLoader.h"
#include <fstream>
#include <vector>
#include <sstream>
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Data/DeletionQueue.h"

using namespace Rendering::Resources::Loaders;

bool ShaderLoader::loadModule(const char* pFilePath, Rendering::Data::ShaderModule* pOutShaderModule)
{
	std::ifstream file(pFilePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return false;

	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);
	file.close();


	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();


	VkShaderModule shaderModule;
	if (vkCreateShaderModule(service(VkDevice), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		return false;

	service(Rendering::Data::DeletionQueue).pushFunction([=]()
	{
		vkDestroyShaderModule(service(VkDevice), shaderModule, nullptr);
	});

	pOutShaderModule->mCode = std::move(buffer);
	pOutShaderModule->mModule = shaderModule;
	return true;
}

constexpr uint32_t fnv1a_32(char const* s, std::size_t count)
{
	return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

uint32_t ShaderLoader::hashDescriptorLayoutInfo(VkDescriptorSetLayoutCreateInfo* info)
{
	std::stringstream ss;

	ss << info->flags;
	ss << info->bindingCount;

	for (auto i = 0u; i < info->bindingCount; i++) 
	{
		const VkDescriptorSetLayoutBinding& binding = info->pBindings[i];

		ss << binding.binding;
		ss << binding.descriptorCount;
		ss << binding.descriptorType;
		ss << binding.stageFlags;
	}

	auto str = ss.str();

	return fnv1a_32(str.c_str(), str.length());
}