#include "Precompiled.h"
#include "Vector3.h"
#include "Plane.h"

Plane::Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3)
{
	Vector3 v1 = InPoint2 - InPoint1;
	Vector3 v2 = InPoint3 - InPoint1;

	Normal = v1.Cross(v2).Normalize();
	InverseDistance = -Normal.Dot(InPoint1);
}

Plane Plane::Normalize()
{
	float normalSize = Normal.Size();
	return Plane(Normal /= normalSize, InverseDistance /= normalSize);
}