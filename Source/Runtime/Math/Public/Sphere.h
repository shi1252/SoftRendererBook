#pragma once

struct Sphere
{
public:
	FORCEINLINE Sphere() = default;
	FORCEINLINE Sphere(const Sphere& InSphere) : Center(InSphere.Center), Radius(InSphere.Radius) {};
	Sphere(const Vector3* InVertices, size_t InCount);

	FORCEINLINE bool IsInside(const Vector3& InVector) const;
	FORCEINLINE bool Intersect(const Sphere& InCircle) const;

public:
	Vector3 Center = Vector3::Zero;
	float Radius = 0.f;
};

FORCEINLINE bool Sphere::IsInside(const Vector3& InVector) const
{
	return ((Center - InVector).SizeSquared() <= (Radius * Radius));
}

FORCEINLINE bool Sphere::Intersect(const Sphere& InSphere) const
{
	float twoRadiusSum = Radius + InSphere.Radius;
	return (Center - InSphere.Center).SizeSquared() < (twoRadiusSum * twoRadiusSum);
}