#include "Rendering/Data/Bone.h"

using namespace Rendering::Data;

Bone::Bone(const std::string& pName, int pId, const aiNodeAnim* pChannel) :
    mName(pName),
    mID(pId)
{
    if (pChannel == nullptr)
        return;

    mNumPositions = pChannel->mNumPositionKeys;
    for (int positionIndex = 0; positionIndex < mNumPositions; ++positionIndex)
    {
        aiVector3D aiPosition = pChannel->mPositionKeys[positionIndex].mValue;
        float timeStamp = pChannel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.mPosition = getVec(aiPosition);
        data.mTimeStamp = timeStamp;
        mPositions.push_back(data);
    }

    mNumRotations = pChannel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < mNumRotations; ++rotationIndex)
    {
        aiQuaternion aiOrientation = pChannel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = pChannel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.mOrientation = getQuat(aiOrientation);
        data.mTimeStamp = timeStamp;
        mRotations.push_back(data);
    }

    mNumScalings = pChannel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < mNumScalings; ++keyIndex)
    {
        aiVector3D scale = pChannel->mScalingKeys[keyIndex].mValue;
        float timeStamp = pChannel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.mScale = getVec(scale);
        data.mTimeStamp = timeStamp;
        mScales.push_back(data);
    }
}

void Bone::update(float pAnimationTime)
{
    Maths::FMatrix4 translation = interpolatePosition(pAnimationTime);
    Maths::FMatrix4 rotation = interpolateRotation(pAnimationTime);
    Maths::FMatrix4 scale = interpolateScaling(pAnimationTime);
    mLocalTransform = translation * rotation * scale;
}

int Bone::getPositionIndex(float pAnimationTime)
{
    for (int index = 0; index < mNumPositions - 1; ++index)
        if (pAnimationTime < mPositions[index + 1].mTimeStamp)
            return index;
    return 0;
}

int Bone::getRotationIndex(float pAnimationTime)
{
    for (int index = 0; index < mNumRotations - 1; ++index)
        if (pAnimationTime < mRotations[index + 1].mTimeStamp)
            return index;
    return 0;
}

int Bone::getScaleIndex(float pAnimationTime)
{
    for (int index = 0; index < mNumScalings - 1; ++index)
        if (pAnimationTime < mScales[index + 1].mTimeStamp)
            return index;
    return 0;
}

float Bone::getScaleFactor(float pLastTimeStamp, float pNextTimeStamp, float pAnimationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = pAnimationTime - pLastTimeStamp;
    float framesDiff = pNextTimeStamp - pLastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

Maths::FMatrix4 Bone::interpolatePosition(float pAnimationTime)
{
  /*  if (1 == mNumPositions)*/
        return Maths::FMatrix4::translation(mPositions[0].mPosition);

    int p0Index = getPositionIndex(pAnimationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(mPositions[p0Index].mTimeStamp, mPositions[p1Index].mTimeStamp, pAnimationTime);
    Maths::FVector3 finalPosition = Maths::FVector3::lerp(mPositions[p0Index].mPosition, mPositions[p1Index].mPosition, scaleFactor);
    return Maths::FMatrix4::translation(finalPosition);
}

Maths::FMatrix4 Bone::interpolateRotation(float pAnimationTime)
{
    if (1 == mNumRotations)
    {
        Maths::FQuaternion rotation = mRotations[0].mOrientation;
        return rotation.toMatrix4();
    }


    int p0Index = getRotationIndex(pAnimationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(mRotations[p0Index].mTimeStamp, mRotations[p1Index].mTimeStamp, pAnimationTime);
    Maths::FQuaternion finalRotation = Maths::FQuaternion::slerp(mRotations[p0Index].mOrientation, mRotations[p1Index].mOrientation, scaleFactor);
    return finalRotation.toMatrix4();
}

Maths::FMatrix4 Bone::interpolateScaling(float pAnimationTime)
{
    if (1 == mNumScalings)
        return Maths::FMatrix4::scaling(mScales[0].mScale);

    int p0Index = getScaleIndex(pAnimationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(mScales[p0Index].mTimeStamp, mScales[p1Index].mTimeStamp, pAnimationTime);
    Maths::FVector3 finalScale = Maths::FVector3::lerp(mScales[p0Index].mScale, mScales[p1Index].mScale, scaleFactor);
    return Maths::FMatrix4::scaling(finalScale);
}

Maths::FVector3 Bone::getVec(const aiVector3D& pVec)
{
    return Maths::FVector3(pVec.x, pVec.y, pVec.z);
}

Maths::FQuaternion Bone::getQuat(const aiQuaternion& pOrientation)
{
    return Maths::FQuaternion(pOrientation.x, pOrientation.y, pOrientation.z, pOrientation.w);
}

Maths::FMatrix4 Bone::convertMatrix(const aiMatrix4x4& pFrom)
{
    Maths::FMatrix4 to;
    to.data[0][0] = pFrom.a1; to.data[1][0] = pFrom.a2; to.data[2][0] = pFrom.a3; to.data[3][0] = pFrom.a4;
    to.data[0][1] = pFrom.b1; to.data[1][1] = pFrom.b2; to.data[2][1] = pFrom.b3; to.data[3][1] = pFrom.b4;
    to.data[0][2] = pFrom.c1; to.data[1][2] = pFrom.c2; to.data[2][2] = pFrom.c3; to.data[3][2] = pFrom.c4;
    to.data[0][3] = pFrom.d1; to.data[1][3] = pFrom.d2; to.data[2][3] = pFrom.d3; to.data[3][3] = pFrom.d4;
    return to;
}