#pragma once

#include "Rendering/Resources/IMesh.h"
#include "Rendering/Data/VKTypes.h"
#include "Rendering/Buffers/VK/VKVertexBuffer.h"
#include "Rendering/Buffers/VK/VKIndiceBuffer.h"
#include "Rendering/Data/BoundingBox.h"

namespace Rendering::Resources::VK
{
	class VKMesh : public IMesh
	{
		public:
			Buffers::VK::VKVertexBuffer mVertexBuffer;
			Buffers::VK::VKIndiceBuffer mIndicesBuffer;
			std::string mName;
			Data::BoundingBox mBoundingBox;

			VKMesh(const std::vector<Geometry::Vertex>& pVertices, const std::vector<uint32_t>& pIndices, const std::string& pName);
			~VKMesh();

			void draw(void* pCommandBuffer) override;
	};
}