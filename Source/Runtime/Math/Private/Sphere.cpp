#include "Precompiled.h"
#include "Platform.h"
#include "Vector3.h"
#include "Sphere.h"

Sphere::Sphere(const Vector3* InVertices, size_t InCount)
{
	Vector3 s;
	for (int i = 0; i < InCount; ++i)
	{
		s += InVertices[i];
	}

	Center = s / (float)InCount;

	float distance = 0.f;
	for (int i = 0; i < InCount; ++i)
	{
		float curr = (Center - InVertices[i]).SizeSquared();
		if (curr > distance)
			distance = curr;
	}
	Radius = sqrtf(distance);
}