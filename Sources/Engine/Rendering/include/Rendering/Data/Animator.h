#pragma once
#include "Rendering/Resources/Animation.h"

#define MAX_BONE 200

namespace Rendering::Data
{
    struct Pose
    {
        std::vector<Maths::FMatrix4> mFinalMatrix;

        Pose()
        {
            mFinalMatrix.reserve(MAX_BONE);
            for (unsigned int i = 0; i < MAX_BONE; i++)
                mFinalMatrix.push_back(Maths::FMatrix4(1.0f));
        }

        static void addPose(Pose& pOut, Pose& pA, Pose& pB, Pose& pBase);
        void addPose(Pose& pOut, Pose& pB, Pose& pBase);
    };

    struct Clip
    {
        Rendering::Resources::Animation* mAnimation = nullptr;
        float mCurrentTime = 0;

        Clip(Rendering::Resources::Animation* pAnimation = nullptr) : mAnimation(pAnimation) {}
        void update(float pDeltaTime, bool pLoop);
        void reset() { mCurrentTime = 0; }
        float synchronizeSpeed(Clip& pOtherClip, float pFactor, bool pUp);

        void calculatePose(Pose& finalPose);
        void calculateFinalPose(Pose& finalPose, const Rendering::Resources::NodeData* pNode, Maths::FMatrix4 pParentTransform);
    
        static void calculateBlendPose(Pose& finalPose, Clip& pClipA, Clip& pClipB, float pFactor);
        void calculateBlendPose(Pose& finalPose, Clip& pClipB, float pFactor);
        void calculateBlendFinalPose(Pose& finalPose, const Rendering::Resources::NodeData* pNodeA, Clip& pClipB, const Rendering::Resources::NodeData* pNodeB, Maths::FMatrix4 pParentTransform, float pFactor);
    
        static void calculateAddPose(Pose& finalPose, Clip& pClipA, Clip& pClipB, Clip& pClipBase);
        void calculateAddPose(Pose& finalPose, Clip& pClipB, Clip& pClipBase);
        void calculateAddFinalPose(Pose& finalPose, const Rendering::Resources::NodeData* pNodeA, Clip& pClipB, const Rendering::Resources::NodeData* pNodeB, Clip& pClipBase, const Rendering::Resources::NodeData* pNodeBase, Maths::FMatrix4 pParentTransform);
    };

    enum class TrackMode
    {
        Normal,
        Blend,
        Fade,
        Additive
    };

    class Animator
    {
    public:
        TrackMode mMode = TrackMode::Normal;
        float blendFactor = 0;
        float mFadeElapsedTime = 0;
        float fadeSpeed = 1;

        Pose mFinalPose;
        
        Clip mBaseClip;
        Clip mBlendClip;
        Clip mAddClip;

        Animator();
        void setAnimationClip(Rendering::Resources::Animation* pAnimation);
        void setBlendAnimationClip(Rendering::Resources::Animation* pAnimation);
        void setFadeAnimationClip(Rendering::Resources::Animation* pAnimation, float pFadeSpeed);
        void setAdditiveAnimationClip(Rendering::Resources::Animation* pAnimation);

        void setBlendFactor(float pBlendFactor);
        
        void updateAnimation(float pDeltaTime, bool looping);
        void updateSingleAnimation(float pDeltaTime, bool looping);
        void updateBlendAnimation(float pDeltaTime, bool looping);
        void updateFadeAnimation(float pDeltaTime, bool looping);
        void updateAdditiveAnimation(float pDeltaTime, bool looping);

        void reset();
    };
}