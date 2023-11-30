#pragma once
#include <Rendering/Resources/TextureCube.h>
#include <Rendering/Resources/Model.h>
#include <Rendering/Data/Material.h>
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"

namespace Rendering::Renderer
{
	class SkyBox
	{
		public:
			std::vector<std::string> paths;
			bool isDefault = true;

			Rendering::Resources::TextureCube* texture = nullptr;
			Rendering::Resources::Model* model = nullptr;
			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataSky> mUniform;

			SkyBox();
			~SkyBox();

			void setDefault();
			void setTexture(const std::string& pFileNameLeft, const std::string& pFileNameRight,
				const std::string& pFileNameTop, const std::string& pFileNamebottom,
				const std::string& pFileNameFront, const std::string& pFileNameBack);
			
#ifdef NSHIPPING
			void setTextureEditor(const std::string& pFileNameLeft, const std::string& pFileNameRight,
				const std::string& pFileNameTop, const std::string& pFileNamebottom,
				const std::string& pFileNameFront, const std::string& pFileNameBack);
#endif

			void updateUniform(Maths::FMatrix4& pProj, Maths::FVector3& pos, float far);
			void draw(VkCommandBuffer cmd, Rendering::Data::Material& pMat);
	};
}