#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Loader/ModelLoader.h"

using namespace Rendering::Resources;

Model::Model(const std::string& pFilePath)
{
	Rendering::Resources::Loaders::ModelLoader::importMesh(pFilePath.c_str(), mMeshes, mBoneInfoMap, mBox, mBoneCounter);
}

Model::~Model()
{
	for (VK::VKMesh* mesh : mMeshes)
		delete mesh;
	mMeshes.clear();
}

void Model::draw(void* pCommandBuffer)
{
	for (VK::VKMesh* mesh : mMeshes)
		mesh->draw(pCommandBuffer);
}