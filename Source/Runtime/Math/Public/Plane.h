#pragma once

struct Plane
{
public:
	Plane() = default;
	Plane(const Vector3& InNormal, float InInverseDistance) : Normal(InNormal), InverseDistance(InInverseDistance) {}
	Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3);

	Plane Normalize();

public:
	Vector3 Normal = Vector3::UnitY;
	float InverseDistance = 0.f;
};