#include "Rendering/Resources/IMesh.h"

using namespace Rendering::Resources;

IMesh::IMesh(const std::vector<Geometry::Vertex>& pVertices, const std::vector<uint32_t>& pIndices) : mVertices(pVertices), mIndices(pIndices)
{}