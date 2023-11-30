#pragma once
#include "Maths/FVector3.h"
#include <math.h>

namespace Maths
{
    struct Plane
    {
        Maths::FVector3 normal = { 0.f, 1.f, 0.f };
        float distance = 0.f;

        Plane() = default;

        Plane(const Maths::FVector3& p1, const Maths::FVector3& norm)
            : normal(Maths::FVector3::normalize(norm)),
            distance(Maths::FVector3::dot(normal, p1))
        {}

        float getSignedDistanceToPlane(const Maths::FVector3& point) const
        {
            return Maths::FVector3::dot(normal, point) - distance;
        }
    };

    struct Frustum
    {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;
    };

    Frustum createFrustumFromCamera(const Maths::FVector3& position, const Maths::FVector3& right, const Maths::FVector3& forward, const Maths::FVector3& up, float aspect, float fovY,
        float zNear, float zFar);
}