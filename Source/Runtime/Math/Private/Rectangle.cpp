#include "Precompiled.h"
#include "Platform.h"
#include "Vector2.h"
#include "Rectangle.h"

Rectangle::Rectangle(const Vector2* InVertices, size_t InCount)
{
	Vector2 min(FLT_MAX, FLT_MAX);
	Vector2 max(FLT_MIN, FLT_MIN);

	for (int i = 0; i < InCount; ++i)
	{
		if (InVertices[i].X < min.X)
			min.X = InVertices[i].X;
		if (InVertices[i].X > max.X)
			max.X = InVertices[i].X;
		if (InVertices[i].Y < min.Y)
			min.Y = InVertices[i].Y;
		if (InVertices[i].Y > max.Y)
			max.Y = InVertices[i].Y;
	}

	Min = min;
	Max = max;
}