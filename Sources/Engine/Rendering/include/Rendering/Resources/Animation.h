#pragma once
#include <assimp/scene.h>
#include "Model.h"
#include <Maths/FMatrix4.h>
#include "Rendering/Data/Bone.h"
#include "Rendering/Data/BoneInfo.h"

namespace Rendering::Resources
{
    struct NodeData
    {
        Maths::FMatrix4 mTransformation;
        std::string mName;
        int mChildrenCount;
        std::vector<NodeData> mChildren;
    };

    class Animation : public IResource
    {
    public:
        float mDuration = 0;
        int mTicksPerSecond = 0;
        std::vector<Rendering::Data::Bone> mBones;
        NodeData mRootNode;
        std::map<std::string, Rendering::Data::BoneInfo> mBoneInfoMap;

        Animation(const std::string& pAnimationPath, Model* pModel);
        ~Animation();

        Rendering::Data::Bone* findBone(const std::string& pName);
        void readMissingBones(const aiAnimation* pAnimation, Model& pModel);
        void readHeirarchyData(NodeData& pDest, const aiNode* pSrc);
    };
}