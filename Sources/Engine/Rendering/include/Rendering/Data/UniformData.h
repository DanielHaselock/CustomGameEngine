#pragma once
#include "Maths/FMatrix4.h"
#include "Rendering/Data/Animator.h"

namespace Rendering::Data
{
	struct UniformData
	{
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(16) Maths::FMatrix4 mFinalBonesMatrices[MAX_BONE];
		alignas(16) Maths::FVector4 mColor = {0, 0, 0, 1};
		alignas(4) int mHasAnimation = 0;
		alignas(4) int mHasTextureAlbedo = 1;
		alignas(4) int mHasTextureNormal = 0;
		alignas(4) int mHasTextureMetalic = 0;
		alignas(4) int mHasTextureRoughness = 0;
		alignas(4) int mHasAO = 0;
	};		

	struct UniformDataMat
	{
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(16) Maths::FVector4 mColor = { 0, 0, 0, 1 };
		alignas(4) int mHasTextureAlbedo = 1;
		alignas(4) int mHasTextureNormal = 0;
		alignas(4) int mHasTextureMetalic = 0;
		alignas(4) int mHasTextureRoughness = 0;
		alignas(4) int mHasTextureAO = 0;
	};

	struct UniformDataWithId
	{
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(4) int id;
	};

	struct UniformDataWithColor
	{
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(16) Maths::FVector3 mColor;
	};

	struct UniformDataUI
	{
		alignas(16) Maths::FMatrix4 mProj;
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FVector4 mColor = { 0, 0, 0, 0 };
		alignas(4) int mHasTexture = 1;
	};

	struct UniformDataText
	{
		alignas(16) Maths::FMatrix4 mProj;
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FVector4 mColor = { 1, 1, 1, 1 };
	};

	struct UniformDataSky
	{
		alignas(16) Maths::FMatrix4 mProj;
		alignas(16) Maths::FMatrix4 mModel;
	};

	struct UniformDataAssetViewer
	{
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FMatrix4 mViewProjection;
	};

	struct UniformDataParticleSystem
	{
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(16) Maths::FVector3 mRight;
		alignas(16) Maths::FVector3 mUp;
		alignas(4) int mHasTexure;
		alignas(4) float mSize;
	};

	struct UniformDataParticle
	{
		alignas(16) Maths::FVector3 mPos;
		alignas(16) Maths::FVector4 mColor;
	};

	enum
	{
		DIR_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};

	struct Light
	{
		alignas(16) Maths::FMatrix4 mVP;
		alignas(16) Maths::FVector3 mColor;
		alignas(16) Maths::FVector3 mDirection;
		alignas(16) Maths::FVector3 mPos;
		alignas(4) int mLightType;
		alignas(4) float mAtenuation;
		alignas(4) float mRadius;
		alignas(4) float mBrightness;
		alignas(4) float mCutOff;
		alignas(4) float mOuterCutOff;
		alignas(4) int mCastShadow = 0;
		alignas(4) int mIdx = 0;
	};

	#define MAX_LIGHT 30
	struct UniformDataLight
	{
		Light light[MAX_LIGHT];
		alignas(16) Maths::FVector3 mEyePos;
		alignas(16) Maths::FVector4 shadowSize;
		alignas(16) Maths::FVector4 shadowCubeSize;
		alignas(4) int shadowSplit = 0;
		alignas(4) int shadowCubeSplit = 0;
		alignas(4) int mNumLight = 0;
	};

	struct UniformDataLightBillboard
	{
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(16) Maths::FVector3 mRight;
		alignas(16) Maths::FVector3 mUp;
		alignas(16) Maths::FVector3 mPos;
	};

	struct UniformDataLightBillboardId
	{
		alignas(16) Maths::FMatrix4 mViewProjection;
		alignas(16) Maths::FVector3 mRight;
		alignas(16) Maths::FVector3 mUp;
		alignas(16) Maths::FVector3 mPos;
		alignas(4)	int id;
	};

	struct UniformDataShadowCube
	{
		alignas(16) Maths::FVector3 mLigthPos;
		alignas(4) float farPlane;
	};

	struct UniformDataShadowCubePush
	{
		Maths::FMatrix4 vp;
		Maths::FMatrix4 model;
	};

	struct UniformDataDecal
	{
		alignas(16) Maths::FMatrix4 mModel;
		alignas(16) Maths::FMatrix4 mViewProjection;
	};
}