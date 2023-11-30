#include "Rendering/Data/Animator.h"
#include "Maths/Utils.h"

using namespace Rendering::Data;


void Pose::addPose(Pose& pOut, Pose& pA, Pose& pB, Pose& pBase)
{
    pA.addPose(pOut, pB, pBase);
}

void Pose::addPose(Pose& pOut, Pose& pB, Pose& pBase)
{
    for (int i = 0; i < mFinalMatrix.size(); i++)
    {
        Maths::FVector4 pos = mFinalMatrix[i].data[3] + (pB.mFinalMatrix[i].data[3] - pBase.mFinalMatrix[i].data[3]);

        Maths::FQuaternion rotA = Maths::FQuaternion(mFinalMatrix[i]);
        Maths::FQuaternion rotB = Maths::FQuaternion(pB.mFinalMatrix[i]);
        Maths::FQuaternion rotBasic = Maths::FQuaternion(pBase.mFinalMatrix[i]);

        Maths::FQuaternion finale = Maths::FQuaternion::normalize(rotA * (Maths::FQuaternion::inverse(rotBasic) * rotB));

        pOut.mFinalMatrix[i] = Maths::FQuaternion::toMatrix4(finale);
        pOut.mFinalMatrix[i].data[3] = pos;
    }

}




void Clip::update(float pDeltaTime, bool pLoop)
{
    if (mAnimation == nullptr)
        return;

    if (!pLoop)
        if (mCurrentTime >= mAnimation->mDuration)
            return;

    mCurrentTime += mAnimation->mTicksPerSecond * pDeltaTime;
    if (!pLoop)
        if (mCurrentTime >= mAnimation->mDuration)
        {
            mCurrentTime = mAnimation->mDuration;
            return;
        }

    mCurrentTime = std::fmod(mCurrentTime, mAnimation->mDuration);
}

float Clip::synchronizeSpeed(Clip& pOtherClip, float pFactor, bool pUp)
{
    if (mAnimation == nullptr || pOtherClip.mAnimation == nullptr)
        return -1;

    float b = mAnimation->mDuration / pOtherClip.mAnimation->mDuration;
    if (pUp)
        return Maths::lerp(1, b, pFactor);
    return Maths::lerp(b, 1, pFactor);
}

void Clip::calculatePose(Pose& finalPose)
{
    if (mAnimation == nullptr)
        return;

    calculateFinalPose(finalPose, &mAnimation->mRootNode, Maths::FMatrix4::Identity);
}

void Clip::calculateFinalPose(Pose& finalPose, const Rendering::Resources::NodeData* pNode, Maths::FMatrix4 pParentTransform)
{
    std::string nodeName = pNode->mName;
    Maths::FMatrix4 nodeTransform = pNode->mTransformation;

    Bone* Bone = mAnimation->findBone(nodeName);

    if (Bone)
    {
        Bone->update(mCurrentTime);
        nodeTransform = Bone->mLocalTransform;
    }

    Maths::FMatrix4 globalTransformation = pParentTransform * nodeTransform;

    std::map<std::string, BoneInfo>& boneInfoMap = mAnimation->mBoneInfoMap;
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].mId;
        Maths::FMatrix4 offset = boneInfoMap[nodeName].mOffset;
        finalPose.mFinalMatrix[index] = globalTransformation * offset;
    }

    for (int i = 0; i < pNode->mChildren.size(); i++)
        calculateFinalPose(finalPose, &pNode->mChildren[i], globalTransformation);
}

void Clip::calculateBlendPose(Pose& finalPose, Clip& pClipA, Clip& pClipB, float pFactor)
{
    pClipA.calculateBlendPose(finalPose, pClipB, pFactor);
}

void Clip::calculateBlendPose(Pose& finalPose, Clip& pClipB, float pFactor)
{
    calculateBlendFinalPose(finalPose, &mAnimation->mRootNode, pClipB, &pClipB.mAnimation->mRootNode, Maths::FMatrix4(1.0f), pFactor);
}

void Clip::calculateBlendFinalPose(Pose& finalPose, const Rendering::Resources::NodeData* pNodeA, Clip& pClipB, const Rendering::Resources::NodeData* pNodeB, Maths::FMatrix4 pParentTransform, float pFactor)
{
    const std::string& nodeName = pNodeA->mName;

    Maths::FMatrix4 nodeTransform = pNodeA->mTransformation;
    Bone* pBone = mAnimation->findBone(nodeName);

    if (pBone)
    {
        pBone->update(mCurrentTime);
        nodeTransform = pBone->mLocalTransform;
    }

    Maths::FMatrix4 layeredNodeTransform = pNodeB->mTransformation;
    pBone = pClipB.mAnimation->findBone(nodeName);
    if (pBone)
    {
        pBone->update(pClipB.mCurrentTime);
        layeredNodeTransform = pBone->mLocalTransform;
    }

    ////Blend matrix
    const Maths::FQuaternion rot0 = Maths::FQuaternion(nodeTransform);
    const Maths::FQuaternion rot1 = Maths::FQuaternion(layeredNodeTransform);
    const Maths::FQuaternion finalRot = Maths::FQuaternion::slerp(rot0, rot1, pFactor);
    Maths::FMatrix4 blendedMat = Maths::FQuaternion::toMatrix4(finalRot);
    blendedMat.data[3] = Maths::FVector4::lerp(nodeTransform.data[3], layeredNodeTransform.data[3], pFactor);

    Maths::FMatrix4 globalTransformation = pParentTransform * blendedMat;

    const auto& boneInfoMap = mAnimation->mBoneInfoMap;
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        const int index = boneInfoMap.at(nodeName).mId;
        const Maths::FMatrix4& offset = boneInfoMap.at(nodeName).mOffset;

        finalPose.mFinalMatrix[index] = globalTransformation * offset;
    }

    for (size_t i = 0; i < pNodeA->mChildren.size(); ++i)
        calculateBlendFinalPose(finalPose , &pNodeA->mChildren[i], pClipB, &pNodeB->mChildren[i], globalTransformation, pFactor);
}

void Clip::calculateAddPose(Pose& finalPose, Clip& pClipA, Clip& pClipB, Clip& pClipBase)
{
    pClipA.calculateAddPose(finalPose, pClipB, pClipBase);
}

void Clip::calculateAddPose(Pose& finalPose, Clip& pClipB, Clip& pClipBase)
{
    calculateAddFinalPose(finalPose, &mAnimation->mRootNode, pClipB, &pClipB.mAnimation->mRootNode, pClipBase, &pClipBase.mAnimation->mRootNode, Maths::FMatrix4(1.0f));
}

void Clip::calculateAddFinalPose(Pose& finalPose, const Rendering::Resources::NodeData* pNodeA, Clip& pClipB, const Rendering::Resources::NodeData* pNodeB, Clip& pClipBase, const Rendering::Resources::NodeData* pNodeBase, Maths::FMatrix4 pParentTransform)
{
    Maths::FMatrix4 globalTransformation = pParentTransform;

    const std::string& nodeName = pNodeA->mName;

    Maths::FMatrix4 nodeTransform = pNodeA->mTransformation;
    Bone* pBone = mAnimation->findBone(nodeName);

    if (pBone)
    {
        pBone->update(mCurrentTime);
        nodeTransform = pBone->mLocalTransform;
    }

    Maths::FMatrix4 layeredNodeTransform = pNodeB->mTransformation;
    pBone = pClipB.mAnimation->findBone(nodeName);
    if (pBone)
    {
        pBone->update(pClipB.mCurrentTime);
        layeredNodeTransform = pBone->mLocalTransform;
    }

    bool continu = true;

    Maths::FMatrix4 layeredBaseNodeTransform = pNodeBase->mTransformation;
    pBone = pClipBase.mAnimation->findBone(nodeName);
    if (pBone)
    {
        pBone->update(0);
        layeredBaseNodeTransform = pBone->mLocalTransform;
    }

        ////Blend matrix
        const Maths::FQuaternion rot0 = Maths::FQuaternion(nodeTransform);
        const Maths::FQuaternion rot1 = Maths::FQuaternion(layeredNodeTransform);
        const Maths::FQuaternion rotB = Maths::FQuaternion(layeredBaseNodeTransform);
        Maths::FQuaternion finalRot = Maths::FQuaternion::normalize(rot0 * (Maths::FQuaternion::inverse(rotB) * rot1));

        Maths::FMatrix4 blendedMat = Maths::FQuaternion::toMatrix4(finalRot);
        blendedMat.data[3] = nodeTransform.data[3] + (layeredNodeTransform.data[3] - layeredBaseNodeTransform.data[3]);

        globalTransformation = pParentTransform * blendedMat;

        const auto& boneInfoMap = mAnimation->mBoneInfoMap;
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            const int index = boneInfoMap.at(nodeName).mId;
            const Maths::FMatrix4& offset = boneInfoMap.at(nodeName).mOffset;

            finalPose.mFinalMatrix[index] = globalTransformation * offset;
        }


    for (size_t i = 0; i < pNodeA->mChildren.size(); ++i)
        calculateAddFinalPose(finalPose, &pNodeA->mChildren[i], pClipB, &pNodeB->mChildren[i], pClipBase, &pNodeBase->mChildren[i], globalTransformation);
}



Animator::Animator()
{
    mBaseClip.reset();
}

void Animator::reset()
{
    mBaseClip.reset();
}

void Animator::setAnimationClip(Rendering::Resources::Animation* pAnimation)
{
    mBaseClip = Clip(pAnimation);
}

void Animator::setBlendAnimationClip(Rendering::Resources::Animation* pAnimation)
{
    mBlendClip = Clip(pAnimation);
    mMode = TrackMode::Blend;
}

void Animator::setFadeAnimationClip(Rendering::Resources::Animation* pAnimation, float pFadeSpeed)
{
    mBlendClip = Clip(pAnimation);
    mMode = TrackMode::Fade;
    mFadeElapsedTime = 0;
    fadeSpeed = pFadeSpeed;

    float speedA = mBlendClip.synchronizeSpeed(mBaseClip, 0, false);
    mBlendClip.mCurrentTime = mBaseClip.mCurrentTime * speedA;
}

void Animator::setAdditiveAnimationClip(Rendering::Resources::Animation* pAnimation)
{
    mBlendClip = Clip(pAnimation);
    mAddClip = Clip(pAnimation);
    
    mMode = TrackMode::Additive;
}

void Animator::setBlendFactor(float pBlendFactor)
{
    blendFactor = Maths::clamp(pBlendFactor, 0, 1);
}

void Animator::updateAnimation(float pDeltaTime, bool looping)
{
    switch (mMode)
    {
    case Rendering::Data::TrackMode::Normal:
        updateSingleAnimation(pDeltaTime, looping);
        break;
    case Rendering::Data::TrackMode::Blend:
        updateBlendAnimation(pDeltaTime, looping);
        break;
    case Rendering::Data::TrackMode::Fade:
        updateFadeAnimation(pDeltaTime, looping);
        break;
    case Rendering::Data::TrackMode::Additive:
        updateAdditiveAnimation(pDeltaTime, looping);
        break;
    default:
        break;
    }
}

void Animator::updateSingleAnimation(float pDeltaTime, bool looping)
{
    mBaseClip.update(pDeltaTime, looping);
    mBaseClip.calculatePose(mFinalPose);
}

void Animator::updateBlendAnimation(float pDeltaTime, bool looping)
{
    float speedA = mBaseClip.synchronizeSpeed(mBlendClip, blendFactor, true);
    mBaseClip.update(pDeltaTime * speedA, looping);

    float speedB = mBlendClip.synchronizeSpeed(mBaseClip, blendFactor, false);
    mBlendClip.update(pDeltaTime * speedB, looping);

    Clip::calculateBlendPose(mFinalPose, mBaseClip, mBlendClip, blendFactor);
}

void Animator::updateFadeAnimation(float pDeltaTime, bool looping)
{
    mFadeElapsedTime += pDeltaTime;
    float factor = mFadeElapsedTime / fadeSpeed;
    factor = Maths::clamp(factor, 0, 1);

    float speedA = mBaseClip.synchronizeSpeed(mBlendClip, factor, true);
    mBaseClip.update(pDeltaTime * speedA, looping);

    float speedB = mBlendClip.synchronizeSpeed(mBaseClip, factor, false);
    mBlendClip.update(pDeltaTime * speedB, looping);

    Clip::calculateBlendPose(mFinalPose, mBaseClip, mBlendClip, factor);

    if (mFadeElapsedTime > fadeSpeed)
    {
        float currentTime = mBlendClip.mCurrentTime;
        setAnimationClip(mBlendClip.mAnimation);
        mBaseClip.mCurrentTime = currentTime;
        mMode = TrackMode::Normal;
    }
}

void Animator::updateAdditiveAnimation(float pDeltaTime, bool looping)
{
    mBaseClip.update(pDeltaTime, looping);
    mBlendClip.update(pDeltaTime, looping);
    mAddClip.update(0, looping);

    Clip::calculateAddPose(mFinalPose, mBaseClip, mBlendClip, mAddClip);
}