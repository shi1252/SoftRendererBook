#include "Precompiled.h"
#include "Platform.h"
#include "Vector2.h"
#include "Circle.h"

Circle::Circle(const Vector2* InVertices, size_t InCount)
{
	Vector2 s;
	for (int i = 0; i < InCount; ++i)
	{
		s += InVertices[i];
	}

	Center = s / InCount;

	float distance = 0.f;
	for (int i = 0; i < InCount; ++i)
	{
		float curr = (Center - InVertices[i]).SizeSquared();
		if (curr > distance)
			distance = curr;
	}
	Radius = sqrtf(distance);
}