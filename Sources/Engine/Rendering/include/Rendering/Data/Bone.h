#pragma once
#include <assimp/anim.h>
#include <vector>
#include <Maths/FMatrix4.h>
#include <Maths/FQuaternion.h>

namespace Rendering::Data
{
    struct KeyPosition
    {
        Maths::FVector3 mPosition;
        float mTimeStamp;
    };

    struct KeyRotation
    {
        Maths::FQuaternion mOrientation;
        float mTimeStamp;
    };

    struct KeyScale
    {
        Maths::FVector3 mScale;
        float mTimeStamp;
    };

    class Bone
    {
    public:
        std::vector<KeyPosition> mPositions;
        std::vector<KeyRotation> mRotations;
        std::vector<KeyScale> mScales;
        int mNumPositions;
        int mNumRotations;
        int mNumScalings;

        Maths::FMatrix4 mLocalTransform;
        std::string mName;
        int mID;

        Bone(const std::string& pName, int pId, const aiNodeAnim* pChannel);

        void update(float pAnimationTime);

        int getPositionIndex(float pAnimationTime);
        int getRotationIndex(float pAnimationTime);
        int getScaleIndex(float pAnimationTime);
        float getScaleFactor(float pLastTimeStamp, float pNextTimeStamp, float pAnimationTime);

        Maths::FMatrix4 interpolatePosition(float pAnimationTime);
        Maths::FMatrix4 interpolateRotation(float pAnimationTime);
        Maths::FMatrix4 interpolateScaling(float pAnimationTime);

        static Maths::FVector3 getVec(const aiVector3D& pVec);
        static Maths::FQuaternion getQuat(const aiQuaternion& pOrientation);
        static Maths::FMatrix4 convertMatrix(const aiMatrix4x4& pFrom);
    };
}