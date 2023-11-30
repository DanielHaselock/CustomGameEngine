#pragma once
#include <Rendering/Data/Material.h>
#include "Texture.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include <string>
#include "rapidjson/document.h"
#include "Rendering/Resources/IResource.h"
#include "Rendering/Resources/TextureArray.h"

namespace Rendering::Resources
{
	class Material : public IResource
	{
		public:
			std::string mFilename;

			Rendering::Resources::TextureArray* mTextureArray = nullptr;

			Rendering::Resources::Texture* mTextureAlbedo = nullptr;
			Rendering::Resources::Texture* mTextureNormal = nullptr;
			Rendering::Resources::Texture* mTextureMetallic = nullptr;
			Rendering::Resources::Texture* mTextureRoughness = nullptr;
			Rendering::Resources::Texture* mTextureAO = nullptr;

			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformData> mUnibuffer;

			std::list<Material*> mInstance;

			bool previewCreated = false;
			bool ready = false;
			Material(const std::string& pFileName);
			~Material() override;

			void bindMat(Rendering::Data::Material& pPipeLine);

			void setModelData(Maths::FMatrix4& pVP, Maths::FMatrix4& pModel);

			void update();

			void updateInstance();
			Material* getInstance();

			Rendering::Resources::Texture* loadTexture(rapidjson::Value& pValue);
	};
}