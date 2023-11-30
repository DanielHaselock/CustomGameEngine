#pragma once
#include <cmath>
#include "Maths/FMatrix4.h"
#include "Maths/FVector2.h"
#include "Maths/FQuaternion.h"
#include "Maths/Utils.h"

namespace Rendering::Data
{
	class Camera
	{
	public:
		float mFOV = 45;
		float mNear = 0.01f;
		float mFar = 100;
		int mWidth = 0;
		int mHeight = 0;

		Maths::FVector3& mPosition;
		Maths::FQuaternion& mRotation;
		Maths::FMatrix4 mProjection;
		Maths::FMatrix4 mView;
		Maths::FMatrix4 mVp;


		float mYaw = 90;
		float mPitch = 0;
		Maths::FVector3 mForward = Maths::FVector3::Forward;
		Maths::FVector3 mRight = Maths::FVector3::Right;
		Maths::FVector3 mUp = Maths::FVector3::Up;

		float mSpeed = 20;
		float mSensitivity = 0.2f;

		Camera(int pWidth, int pHeight, Maths::FVector3& pPosition, Maths::FQuaternion& pRotation);

		~Camera() = default;

		void updateProjection(float pWidth, float pHeight);
		virtual void updateProjection();
		void updateView();
		void updateViewProj();

		Maths::FMatrix4& viewProj();

		void calculateVector();

		void update(Maths::FVector2 pDirection, float pDeltaTime);

		void updateView(const Maths::FVector2& pVector);
	};
}