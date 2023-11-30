#pragma once
#include "Game/Component/AComponent.h"
#include "string"
#include "Maths/FVector3.h"
#include "Rendering/Resources/Texture.h"
#include "Maths/FMatrix4.h"
#include "EngineCore/EventSystem/Event.h"
#include "Rendering/Geometry/Vertex.h"
#include "Rendering/Buffers/VK/VKDynamicVertexBuffer.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"

namespace Rendering
{
	class LineDrawer;
}

namespace Game::Component
{
	class CPDecal : public AComponent
	{
	public:
		bool isInit = false;

		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataDecal>* mUniformBuffer;

#ifdef NSHIPPING
		Rendering::LineDrawer* mCubeDrawer = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataDecal>* mUniformBufferEditor;
#endif

		Rendering::Resources::Texture* mTexture = nullptr;

		std::vector<Rendering::Geometry::Vertex> vertices;
		Rendering::Buffers::VK::VKDynamicVertexBuffer* mVertexBuffer = nullptr;

		std::string mPath;
		std::string mName;

		Maths::FMatrix4 previousPos;
		bool isCalculating = false;

		Maths::FVector3 mSize = Maths::FVector3::One;

		CPDecal(EngineCore::EventSystem::Event<Maths::FMatrix4>& pOnValueChanged);
		CPDecal(const CPDecal& pOther);
		~CPDecal();
		AComponent* clone() override;

		void setDecal(const std::string& pName, const char* pModel);
		void setDecalWithPath(const char* pModel);
		void setDecalWithPathLua(const char* pPath);

		void OnStart() override {};
		void OnPaused() override {};
		void OnUpdate(float pDeltaTime) override {};
		void OnDestroy() override {};

#ifdef NSHIPPING
		void updateDrawerLine();
		void drawLine(Maths::FMatrix4& pViewProj);
		void drawEditor(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj);
#endif

		void updateDrawer();
		void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj);

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}