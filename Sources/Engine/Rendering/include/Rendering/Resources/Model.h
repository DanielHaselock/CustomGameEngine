#pragma once
#include <list>
#include <string>
#include <map>
#include "Rendering/Data/BoneInfo.h"
#include "Rendering/Data/BoundingBox.h"
#include "Rendering/Resources/IResource.h"
#include <vector>
#include "Rendering/Resources/VK/VKMesh.h"

namespace Rendering::Resources
{
	class Model : public IResource
	{
		public:
			std::vector<VK::VKMesh*> mMeshes;
			std::map<std::string, Data::BoneInfo> mBoneInfoMap;
			Data::BoundingBox mBox;
			int mBoneCounter = 0;
			bool previewCreated = false;

			Model(const std::string& pFilePath);
			~Model();
			void draw(void* pCommandBuffer);
	};
}