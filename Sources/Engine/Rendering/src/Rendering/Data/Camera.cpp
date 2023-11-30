#pragma once
#include "Rendering/Data/Camera.h"

using namespace Rendering::Data;

Camera::Camera(int pWidth, int pHeight, Maths::FVector3& pPosition, Maths::FQuaternion& pRotation) 
    : mWidth(pWidth), mHeight(pHeight), mPosition(pPosition), mRotation(pRotation)
{
    updateProjection(pWidth, pHeight);
    updateView();
}

void Camera::updateProjection(float pWidth, float pHeight)
{
    mProjection = Maths::FMatrix4::createPerspective(mFOV, float(pWidth) / float(pHeight), mNear, mFar);
    updateViewProj();

    mWidth = pWidth;
    mHeight = pHeight;
}

void Camera::updateProjection()
{
    mProjection = Maths::FMatrix4::createPerspective(mFOV, float(mWidth) / float(mHeight), mNear, mFar);
    updateViewProj();
}

void Camera::updateView()
{
    calculateVector();

    mView = Maths::FMatrix4::lookAt(mPosition, mPosition + mForward, mUp);
    updateViewProj();
}

void Camera::updateViewProj()
{
    mVp = mProjection * mView;
}

Maths::FMatrix4& Camera::viewProj()
{
    return mVp;
}

void Camera::calculateVector()
{
    Maths::FVector3 front;
    front.z = sin(mYaw * (PI / HALF_CIRCLE)) * cos(mPitch * (PI / HALF_CIRCLE));
    front.y = -sin(mPitch * (PI / HALF_CIRCLE));
    front.x = cos(mYaw * (PI / HALF_CIRCLE)) * cos(mPitch * (PI / HALF_CIRCLE));
    front.y = sin(mPitch * (PI / HALF_CIRCLE));
    front.z = sin(mYaw * (PI / HALF_CIRCLE)) * cos(mPitch * (PI / HALF_CIRCLE));
    mForward = front.normalize();

    Maths::FVector3 up = Maths::FVector3::Up;
    mRight = up.cross(mForward).normalize();
    mUp = mForward.cross(mRight).normalize();
}

void Camera::update(Maths::FVector2 pDirection, float pDeltaTime)
{
    float velocity = mSpeed * pDeltaTime;
    mPosition += mForward * pDirection.y * velocity;
    mPosition += mRight * pDirection.x * velocity;

    updateView();
}

void Camera::updateView(const Maths::FVector2& pVector)
{
    mYaw += pVector.x * mSensitivity;
    mPitch += pVector.y * mSensitivity;
    mPitch = Maths::clamp(mPitch, -90, 90);

    calculateVector();
    updateView();
}