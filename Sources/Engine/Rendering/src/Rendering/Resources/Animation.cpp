#include "Rendering/Resources/Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

using namespace Rendering::Resources;

Animation::Animation(const std::string& pAnimationPath, Model* pModel)
{
    if (pModel == nullptr)
        return;

    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(pAnimationPath, aiProcess_ConvertToLeftHanded);

    if (!scene || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    if (scene->HasAnimations())
    {
        aiAnimation* animation = scene->mAnimations[0];
        mDuration = animation->mDuration;
        mTicksPerSecond = animation->mTicksPerSecond;
        readHeirarchyData(mRootNode, scene->mRootNode);
        readMissingBones(animation, *pModel);
    }
}

Animation::~Animation() {}

Rendering::Data::Bone* Animation::findBone(const std::string& pName)
{
    std::vector<Rendering::Data::Bone>::iterator iter = std::find_if(mBones.begin(), mBones.end(),
        [&](const Rendering::Data::Bone& Bone) {
            return Bone.mName == pName;
        }
    );

    if (iter == mBones.end())
        return nullptr;
    else
        return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* pAnimation, Model& pModel)
{
    int size = pAnimation->mNumChannels;

    std::map<std::string, Rendering::Data::BoneInfo>& boneInfoMap = pModel.mBoneInfoMap;
    int& boneCount = pModel.mBoneCounter;


    for (int i = 0; i < size; i++)
    {
        aiNodeAnim* channel = pAnimation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].mId = boneCount;
            boneCount++;
        }
        mBones.emplace_back(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].mId, channel);
    }

    mBoneInfoMap = boneInfoMap;
}

void Animation::readHeirarchyData(NodeData& pDest, const aiNode* pSrc)
{
    if (pSrc == nullptr)
        return;

    pDest.mName = pSrc->mName.data;
    pDest.mTransformation = Rendering::Data::Bone::convertMatrix(pSrc->mTransformation);
    pDest.mChildrenCount = pSrc->mNumChildren;

    for (int i = 0; i < pSrc->mNumChildren; i++)
    {
        NodeData newData;
        readHeirarchyData(newData, pSrc->mChildren[i]);
        pDest.mChildren.push_back(newData);
    }
}