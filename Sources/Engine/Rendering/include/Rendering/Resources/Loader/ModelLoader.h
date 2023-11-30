#pragma once
#include <map>
#include <vector>
#include <assimp/scene.h>
#include "Rendering/Data/BoneInfo.h"
#include "Rendering/Data/BoundingBox.h"
#include "Rendering/Resources/VK/VKMesh.h"

#define MAX_BONE_INFLUENCE 4

namespace Rendering::Resources::Loaders
{
	class ModelLoader
	{
		public:
			static bool importMesh(const char* pFilePath, std::vector<VK::VKMesh*>& pMesh, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, Data::BoundingBox& pBox, int& pBoneCount);
			static void exportMesh(const char* pFilePath, VK::VKMesh& aMesh);
			static void processNode(const aiNode* pNode, const aiScene* pScene, std::vector<VK::VKMesh*>& pMesh, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, int& pBoneCount);
			static void processMesh(const aiMesh* pMesh, const aiScene* pScene, std::vector<VK::VKMesh*>& pVKMesh, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, int& pBoneCount);
	
			static void extractBoneWeightForVertices(std::vector<Rendering::Geometry::Vertex>& pVertices, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, int& pBoneCount, const aiMesh* pMesh, const aiScene* pScene);
			static void setVertexBoneData(Rendering::Geometry::Vertex& pVertex, int pBoneId, float pWeight);
			static Maths::FMatrix4 convertMatrix(const aiMatrix4x4& pFrom);
			static Rendering::Data::BoundingBox convertBoundingBox(const aiAABB& pFrom);
	};
}