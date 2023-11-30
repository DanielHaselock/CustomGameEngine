#pragma once
#include <vector>
#include "Rendering/Geometry/Vertex.h"

namespace Rendering::Resources
{
	class IMesh
	{
		public:
			std::vector<Geometry::Vertex> mVertices;
			std::vector<uint32_t> mIndices;

			IMesh(const std::vector<Geometry::Vertex>& pVertices, const std::vector<uint32_t>& pIndices);
			virtual void draw(void* pCommandBuffer) = 0;
	};
}