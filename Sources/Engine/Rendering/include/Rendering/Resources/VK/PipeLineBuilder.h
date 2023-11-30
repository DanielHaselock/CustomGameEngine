#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Rendering/Data/DeletionQueue.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Data/Shader.h"

namespace Rendering::Renderer::Resources::VK
{
	

	class PipeLineBuilder
	{
	public:
		static Data::ShaderCache mModuleCache;

		std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
		VkPipelineVertexInputStateCreateInfo mVertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo mInputAssembly;
		VkViewport mViewport;
		VkRect2D mScissor;
		VkPipelineRasterizationStateCreateInfo mRasterizer;
		VkPipelineColorBlendAttachmentState mColorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo mMultisampling;
		VkPipelineLayout mPipelineLayout;
		VkPipelineDepthStencilStateCreateInfo mDepthStencil;
		
		VkPipeline buildPipeline(VkRenderPass pPass);
		void clearVertexInput();
		
		static Rendering::Data::Material initPipeLine(const std::string& pVertex, const std::string& pfrag, VkRenderPass pPass, bool pClearVertex = false, bool pDebugVertex = false, bool pDethTest = true, bool pStencil = false, bool pParticle = false, bool pStencilTest = true);
		static Rendering::Data::Material initPipeLine(const std::string& pVertex, VkRenderPass pPass, bool pClearVertex = false, bool pDebugVertex = false, bool pDethTest = true, bool pStencil = false, bool pParticle = false, bool pStencilTest = true);


		static Rendering::Data::Material initPipeLine(Data::Shader& pShader, VkRenderPass pPass, bool pClearVertex, bool pDebugVertex, bool pDethTest, bool pStencil, bool pParticle, bool pStencilTest);
	};
}