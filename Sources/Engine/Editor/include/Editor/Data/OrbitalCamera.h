#pragma once
#include <cmath>
#include "Maths/FMatrix4.h"
#include "Maths/FVector2.h"
#include "Maths/Utils.h"

namespace Editor::Data
{
	struct OrbitalCamera
	{
		Maths::FVector3 mDistanceToTarget;
		Maths::FVector3 mTarget;
		float mAngleX = 0;
		float mAngleY = -20;
		float rotationSpeed = 1;
		float mFOV = 45;
		Maths::FVector3 mPosition;
		Maths::FMatrix4 mProjection;

		OrbitalCamera(int pWidth, int pHeight, float pDistanceToTarget = 3);

		void updateHeight(const float& pHeight);

		void updateDistance();

		void update(float pX, float pY);

		void update(const Maths::FVector2& pVector);

		void zoom(float pZoom);

		void updateProjection(float pWidth, float pHeight);

		Maths::FMatrix4 viewProj();

		Maths::FMatrix4 lookAt();
	};
}