#include "Maths/FTransform.h"
#include <iostream>

using namespace Maths;

FTransform::FTransform(FVector3 p_localPosition, FQuaternion p_localRotation, FVector3 p_localScale) :
	notificationHandlerID(-1),
	mParent(nullptr)
{
	createTransformMatrix(p_localPosition, p_localRotation, p_localScale);
}

FTransform::~FTransform()
{
	notifier.notifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED);
}

void FTransform::notificationHandler(Internal::TransformNotifier::ENotification p_notification)
{
	switch (p_notification)
	{
	case Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED:
		updateWorldMatrix();
		break;

	case Internal::TransformNotifier::ENotification::TRANSFORM_DESTROYED:
		/*
		* RemoveParent() is not called here because it is unsafe to remove a notification handler
		* while the parent is iterating on his notification handlers (Segfault otherwise)
		*/
		createTransformMatrix(mWorldPosition, mWorldRotation, mWorldScale);
		mParent = nullptr;
		updateWorldMatrix();
		break;
	}
}

void FTransform::setParent(FTransform& p_parent)
{
	if (mParent != nullptr)
		removeParent();

	mParent = &p_parent;

	notificationHandlerID = mParent->notifier.addNotificationHandler(std::bind(&FTransform::notificationHandler, this, std::placeholders::_1));

	updateWorldMatrix();
}

bool FTransform::removeParent()
{
	if (mParent == nullptr)
		return false;

	mParent->notifier.removeNotificationHandler(notificationHandlerID);
	mParent = nullptr;
	updateWorldMatrix();
	return true;
}

bool FTransform::hasParent() const
{
	return mParent != nullptr;
}

void FTransform::createTransformMatrix(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale)
{
	mLocalMatrix = FMatrix4::translation(p_position) * FQuaternion::toMatrix4(FQuaternion::normalize(p_rotation)) * FMatrix4::scaling(p_scale);
	mLocalPosition = p_position;
	mLocalRotation = p_rotation;
	mLocalScale = p_scale;

	updateWorldMatrix();
}

void FTransform::updateWorldMatrix()
{
	mWorldMatrix = hasParent() ? mParent->mWorldMatrix * mLocalMatrix : mLocalMatrix;

	if (hasParent())
		decomposeWorldMatrix();
	else
	{
		mWorldPosition = mLocalPosition;
		mWorldRotation = mLocalRotation;
		mWorldScale = mLocalScale;
	}

	notifier.notifyChildren(Internal::TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void Maths::FTransform::updateLocalMatrix()
{
	mLocalMatrix = FMatrix4::translation(mLocalPosition) * FQuaternion::toMatrix4(FQuaternion::normalize(mLocalRotation)) * FMatrix4::scaling(mLocalScale);

	updateWorldMatrix();
}

void FTransform::printWorld()
{
	std::cout << "World Position: X: " << mWorldPosition.x << " Y: " << mWorldPosition.y << " Z: " << mWorldPosition.z << std::endl;
	std::cout << "World Rotation: X: " << mWorldRotation.x << " Y: " << mWorldRotation.y << " Z: " << mWorldRotation.z << " W: " << mWorldRotation.w << std::endl;
	std::cout << "World Scale: X: " << mWorldScale.x << " Y: " << mWorldScale.y << " Z: " << mWorldScale.z << std::endl;
}

void FTransform::setLocalPosition(FVector3 p_newPosition)
{
	createTransformMatrix(p_newPosition, mLocalRotation, mLocalScale);
}

void FTransform::setLocalRotation(FQuaternion p_newRotation)
{
	createTransformMatrix(mLocalPosition, p_newRotation, mLocalScale);
}

void FTransform::setLocalScale(FVector3 p_newScale)
{
	createTransformMatrix(mLocalPosition, mLocalRotation, p_newScale);
}

void FTransform::setWorldPosition(FVector3 p_newPosition)
{
	createTransformMatrix(p_newPosition, mWorldRotation, mWorldScale);
}

void FTransform::setWorldRotation(FQuaternion p_newRotation)
{
	createTransformMatrix(mWorldPosition, p_newRotation, mWorldScale);
}

void FTransform::setWorldScale(FVector3 p_newScale)
{
	createTransformMatrix(mWorldPosition, mWorldRotation, p_newScale);
}

void FTransform::translateLocal(const FVector3& p_translation)
{
	setLocalPosition(mLocalPosition + p_translation);
}

void FTransform::rotateLocal(const FQuaternion& p_rotation)
{
	setLocalRotation(mLocalRotation * p_rotation);
}

void FTransform::scaleLocal(const FVector3& p_scale)
{
	setLocalScale(FVector3
	(
		mLocalScale.x * p_scale.x,
		mLocalScale.y * p_scale.y,
		mLocalScale.z * p_scale.z
	));
}

const FVector3& FTransform::getLocalPosition() const
{
	return mLocalPosition;
}

const FQuaternion& FTransform::getLocalRotation() const
{
	return mLocalRotation;
}

const FVector3& FTransform::getLocalScale() const
{
	return mLocalScale;
}

const FVector3& FTransform::getWorldPosition() const
{
	return mWorldPosition;
}

const FQuaternion& FTransform::getWorldRotation() const
{
	return mWorldRotation;
}

const FVector3& FTransform::getWorldScale() const
{
	return mWorldScale;
}

const FMatrix4& FTransform::getLocalMatrix() const
{
	return mLocalMatrix;
}

FMatrix4& FTransform::getWorldMatrix()
{
	return mWorldMatrix;
}

FVector3 FTransform::getWorldForward() const
{
	return mWorldRotation * FVector3::Forward;
}

FVector3 FTransform::getWorldUp() const
{
	return mWorldRotation * FVector3::Up;
}

FVector3 FTransform::getWorldRight() const
{
	return mWorldRotation * FVector3::Right;
}

FVector3 FTransform::getLocalForward() const
{
	return mLocalRotation * FVector3::Forward;
}

FVector3 FTransform::getLocalUp() const
{
	return mLocalRotation * FVector3::Up;
}

FVector3 FTransform::getLocalRight() const
{
	return mLocalRotation * FVector3::Right;
}

void FTransform::decomposeWorldMatrix()
{
	mWorldPosition.x = mWorldMatrix(3, 0);
	mWorldPosition.y = mWorldMatrix(3, 1);
	mWorldPosition.z = mWorldMatrix(3, 2);

	FVector3 columns[3] =
	{
		{ mWorldMatrix(0, 0), mWorldMatrix(0, 1), mWorldMatrix(0, 2)},
		{ mWorldMatrix(1, 0), mWorldMatrix(1, 1), mWorldMatrix(1, 2)},
		{ mWorldMatrix(2, 0), mWorldMatrix(2, 1), mWorldMatrix(2, 2)},
	};

	mWorldScale.x = FVector3::length(columns[0]);
	mWorldScale.y = FVector3::length(columns[1]);
	mWorldScale.z = FVector3::length(columns[2]);

	if (mWorldScale.x)
		columns[0] /= mWorldScale.x;

	if (mWorldScale.y)
		columns[1] /= mWorldScale.y;

	if (mWorldScale.z)
		columns[2] /= mWorldScale.z;

	FMatrix3 rotationMatrix
	(
		columns[0].x, columns[1].x, columns[2].x,
		columns[0].y, columns[1].y, columns[2].y,
		columns[0].z, columns[1].z, columns[2].z
	);

	mWorldRotation = FQuaternion(rotationMatrix);
}

void FTransform::decomposeLocalMatrix()
{
	mLocalPosition.x = mLocalMatrix(3, 0);
	mLocalPosition.y = mLocalMatrix(3, 1);
	mLocalPosition.z = mLocalMatrix(3, 2);

	FVector3 columns[3] =
	{
		{ mLocalMatrix(0, 0), mLocalMatrix(0, 1), mLocalMatrix(0, 2)},
		{ mLocalMatrix(1, 0), mLocalMatrix(1, 1), mLocalMatrix(1, 2)},
		{ mLocalMatrix(2, 0), mLocalMatrix(2, 1), mLocalMatrix(2, 2)},
	};

	mLocalScale.x = FVector3::length(columns[0]);
	mLocalScale.y = FVector3::length(columns[1]);
	mLocalScale.z = FVector3::length(columns[2]);

	if (mLocalScale.x)
	{
		columns[0] /= mLocalScale.x;
	}
	if (mLocalScale.y)
	{
		columns[1] /= mLocalScale.y;
	}
	if (mLocalScale.z)
	{
		columns[2] /= mLocalScale.z;
	}

	FMatrix3 rotationMatrix
	(
		columns[0].x, columns[1].x, columns[2].x,
		columns[0].y, columns[1].y, columns[2].y,
		columns[0].z, columns[1].z, columns[2].z
	);

	mLocalRotation = FQuaternion(rotationMatrix);
}