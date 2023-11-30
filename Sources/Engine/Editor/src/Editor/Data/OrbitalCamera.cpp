#include "Editor/Data/OrbitalCamera.h"

using namespace Editor::Data;

OrbitalCamera::OrbitalCamera(int pWidth, int pHeight, float pDistanceToTarget)
	: mDistanceToTarget(0, 0, pDistanceToTarget), mPosition(0, 0, pDistanceToTarget), mTarget(0, 0, 0)
{
	updateProjection(pWidth, pHeight);
}

void OrbitalCamera::updateHeight(const float& pHeight)
{
	mDistanceToTarget.y = pHeight;
	mTarget.y = pHeight;
	updateDistance();
}

void OrbitalCamera::updateDistance()
{
	mPosition = Maths::FMatrix4::rotateOnAxisY(Maths::FMatrix4(1), mAngleX).rotateOnAxisX(mAngleY) * (mDistanceToTarget - mTarget) + mTarget;
}

void OrbitalCamera::update(float pX, float pY)
{
	mAngleX -= pX * rotationSpeed;
	mAngleY += pY * rotationSpeed;
	mAngleY = Maths::clamp(mAngleY, -89, 89);

	updateDistance();
}

void OrbitalCamera::update(const Maths::FVector2& pVector)
{
	update(pVector.x, pVector.y);
}

void OrbitalCamera::zoom(float pZoom)
{
	mDistanceToTarget.z -= pZoom / 5.f;
	mDistanceToTarget.z = Maths::clamp(mDistanceToTarget.z, 1.f, 10.0f);
	updateDistance();
}

void OrbitalCamera::updateProjection(float pWidth, float pHeight)
{
	mProjection = Maths::FMatrix4::createPerspective(mFOV, float(pWidth) / float(pHeight), 0.01f, 500.0f);
}

Maths::FMatrix4 OrbitalCamera::viewProj()
{
	return mProjection * lookAt();
}

Maths::FMatrix4 OrbitalCamera::lookAt()
{
	return Maths::FMatrix4::lookAt(mPosition, mTarget, Maths::FVector3::Up);
}