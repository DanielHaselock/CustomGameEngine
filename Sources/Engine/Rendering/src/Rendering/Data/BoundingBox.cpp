#include "Rendering/Data/BoundingBox.h"

using namespace Rendering::Data;

void BoundingBox::computeCorner()
{
	corners[0] = {mMin.x, mMin.y, mMin.z, 1.0}; // x y z
	corners[1] = {mMax.x, mMin.y, mMin.z, 1.0}; // X y z
	corners[2] = {mMin.x, mMax.y, mMin.z, 1.0}; // x Y z
	corners[3] = {mMax.x, mMax.y, mMin.z, 1.0}; // X Y z
					 ;
	corners[4] = {mMin.x, mMin.y, mMax.z, 1.0}; // x y Z
	corners[5] = {mMax.x, mMin.y, mMax.z, 1.0}; // X y Z
	corners[6] = {mMin.x, mMax.y, mMax.z, 1.0}; // x Y Z
	corners[7] = {mMax.x, mMax.y, mMax.z, 1.0}; // X Y Z
}

//bool BoundingBox::isOnOrForwardPlane(const Maths::Plane& plane) const
//{
//	return plane.getSignedDistanceToPlane(mCenter) > -radius;
//}
//
//bool BoundingBox::isOnFrustum(const Maths::Frustum& camFrustum, const Maths::FTransform& transform)
//{
//	if (mCenter.x == -1)
//	{
//		mSize = mMax - mMin;
//		mCenter = Maths::FVector3(mMin.x + (mSize.x / 2.f), mMin.y + (mSize.y / 2.f), mMin.z + (mSize.z / 2.f));
//		radius = Maths::fmax(Maths::fmax(mSize.x / 2.f, mSize.y / 2.f), mSize.z / 2.f);
//	}
//
//	//Get global scale thanks to our transform
//	const  Maths::FVector3& globalScale = transform.getWorldScale();
//
//	//Get our global center with process it with the global model matrix of our transform
//	const Maths::FVector4 globalCenter = transform.mWorldMatrix * Maths::FVector4(mCenter.x, mCenter.y, mCenter.z, 1);
//
//	//To wrap correctly our shape, we need the maximum scale scalar.
//	const float maxScale = Maths::fmax(Maths::fmax(globalScale.x, globalScale.y), globalScale.z);
//
//	//Max scale is assuming for the diameter. So, we need the half to apply it to our radius
//	BoundingBox globalSphere(*this);
//	globalSphere.mCenter = { globalCenter.x, globalCenter.y, globalCenter.z };
//	globalSphere.radius = radius * maxScale * 0.5f;
//
//	//Check Firstly the result that have the most chance to failure to avoid to call all functions.
//	return (globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
//		globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
//		globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
//		globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
//		globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
//		globalSphere.isOnOrForwardPlane(camFrustum.bottomFace));
//};