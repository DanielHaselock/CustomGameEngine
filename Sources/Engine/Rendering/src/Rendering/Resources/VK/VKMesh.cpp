#include "Rendering/Resources/VK/VKMesh.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Data/DeletionQueue.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Buffers/VK/VKBuffer.h"

using namespace Rendering::Resources::VK;

VKMesh::VKMesh(const std::vector<Geometry::Vertex>& pVertices, const std::vector<uint32_t>& pIndices, const std::string& pName) :
	IMesh(pVertices, pIndices), mVertexBuffer(pVertices.data(), pVertices.size() * sizeof(Geometry::Vertex)),
	mIndicesBuffer(mIndices.data(), mIndices.size() * sizeof(uint32_t)), mName(pName)
{
}

VKMesh::~VKMesh()
{
}

void VKMesh::draw(void* pCommandBuffer)
{
	mVertexBuffer.bind(pCommandBuffer);
	mIndicesBuffer.bind(pCommandBuffer);
	
	vkCmdDrawIndexed((VkCommandBuffer)pCommandBuffer, mIndices.size(), 1, 0, 0, 0);
}