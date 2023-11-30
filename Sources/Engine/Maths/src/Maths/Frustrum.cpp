#include "Maths/Frustrum.h"
#include "Maths/Utils.h"
Maths::Frustum Maths::createFrustumFromCamera(const Maths::FVector3& position, const Maths::FVector3& right, const Maths::FVector3& forward, const Maths::FVector3& up, float aspect, float fovY,
    float zNear, float zFar)
{
    Maths::Frustum     frustum;
    const float halfVSide = zFar * tanf(Maths::degreesToRadians(fovY) * .5f);
    const float halfHSide = halfVSide * aspect;
    const Maths::FVector3 frontMultFar = forward * zFar;

    frustum.nearFace = { (position + zNear) * forward, forward };
    frustum.farFace = { position + frontMultFar, -forward };
    frustum.rightFace = { position,
                            Maths::FVector3::cross(up, frontMultFar - right * halfHSide) };
    frustum.leftFace = { position,
                            Maths::FVector3::cross(frontMultFar + right * halfHSide, up) };
    frustum.topFace = { position,
                            Maths::FVector3::cross(right, frontMultFar - up * halfVSide) };
    frustum.bottomFace = { position,
                            Maths::FVector3::cross(frontMultFar + up * halfVSide, right) };

    return frustum;
}