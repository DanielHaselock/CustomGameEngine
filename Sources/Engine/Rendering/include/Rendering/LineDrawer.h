#pragma once
#include "Rendering/Geometry/DebugVertex.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Buffers/VK/VKDynamicVertexBuffer.h"
#include "Renderer/VK/VKRenderer.h"

namespace Rendering
{
	class LineDrawer
	{
		public:
			static const int MAX_LINE = 10000;

			Rendering::Renderer::VK::VKRenderer& mRenderer;
			std::vector<Rendering::Geometry::DebugVertex> mLineVertices;
			Rendering::Data::Material mMat;
			Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformData> mUniformBuffer;
			Rendering::Buffers::VK::VKDynamicVertexBuffer* mVertexBuffer = nullptr;

			LineDrawer(Rendering::Renderer::VK::VKRenderer& pRenderer);

			~LineDrawer();

			void drawLine(const Maths::FVector3& pFrom, const Maths::FVector3& pTo, const Maths::FVector3& pColor);
			void drawCircle(Maths::FVector3& pCenter, float r, bool useZ = false, int num_segments = 100);
			void flushLines();

			void updateViewProj(Maths::FMatrix4 pViewProj);

			void reset();
	};
}