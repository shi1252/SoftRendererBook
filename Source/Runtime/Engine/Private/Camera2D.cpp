#include "Precompiled.h"
#include "Mesh2D.h"
#include "Transform2D.h"
#include "GameObject2D.h"
#include "Camera2D.h"

Matrix3x3 Camera2D::GetViewMatrix() const
{
	return Matrix3x3(Vector3(_Transform.GetLocalX(), false), Vector3(_Transform.GetLocalY(), false), Vector3::UnitZ) 
		* Matrix3x3(Vector3::UnitX, Vector3::UnitY, Vector3(-_Transform.GetPosition()));
}

void Camera2D::SetCameraViewSize(const ScreenPoint& InScreenSize)
{
	Vector2 halfSize = Vector2(InScreenSize.X, InScreenSize.Y) * 0.5f;

	_CircleBound.Center = Vector2::Zero;
	_CircleBound.Radius = sqrtf(halfSize.X * halfSize.X + halfSize.Y * halfSize.Y);

	_RectBound.Min = -halfSize;
	_RectBound.Max = halfSize;
}
