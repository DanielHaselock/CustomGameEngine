#pragma once

#include "Maths/Internal/TransformNotifier.h"
#include "Maths/FQuaternion.h"
#include "Maths/FMatrix4.h"
#include "Maths/FVector3.h"

namespace Maths
{
	class FTransform
	{
	public:

		FTransform(FVector3 p_localPosition = FVector3(0.0f, 0.0f, 0.0f), FQuaternion p_localRotation = FQuaternion::Identity, FVector3 p_localScale = FVector3(1.0f, 1.0f, 1.0f));

		~FTransform();

		void printWorld();

		void notificationHandler(Internal::TransformNotifier::ENotification p_notification);

		void setParent(FTransform& p_parent);

		bool removeParent();

		bool hasParent() const;

		void createTransformMatrix(FVector3 p_position, FQuaternion p_rotation, FVector3 p_scale);

		void updateWorldMatrix();

		void updateLocalMatrix();

		void setLocalPosition(FVector3 p_newPosition);

		void setLocalRotation(FQuaternion p_newRotation);

		void setLocalScale(FVector3 p_newScale);

		void setWorldPosition(FVector3 p_newPosition);

		void setWorldRotation(FQuaternion p_newRotation);

		void setWorldScale(FVector3 p_newScale);

		void translateLocal(const FVector3& p_translation);

		void rotateLocal(const FQuaternion& p_rotation);

		void scaleLocal(const FVector3& p_scale);

		const FVector3& getLocalPosition() const;

		const FQuaternion& getLocalRotation() const;

		const FVector3& getLocalScale() const;

		const FVector3& getWorldPosition() const;

		const FQuaternion& getWorldRotation() const;

		const FVector3& getWorldScale() const;

		const FMatrix4& getLocalMatrix() const;

		FMatrix4& getWorldMatrix();

		FVector3 getWorldForward() const;

		FVector3 getWorldUp() const;

		FVector3 getWorldRight() const;

		FVector3 getLocalForward() const;

		FVector3 getLocalUp() const;

		FVector3 getLocalRight() const;

		Internal::TransformNotifier notifier;
		Internal::TransformNotifier::NotificationHandlerID notificationHandlerID;

		void decomposeWorldMatrix();
		void decomposeLocalMatrix();

		FVector3 mLocalPosition;
		FQuaternion mLocalRotation;
		FVector3 mLocalScale;

		FVector3 mWorldPosition;
		FQuaternion mWorldRotation;
		FVector3 mWorldScale;

		FMatrix4 mLocalMatrix;
		FMatrix4 mWorldMatrix;

		FTransform* mParent;
	};
}