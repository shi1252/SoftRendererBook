#include "Precompiled.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

Matrix4x4 Camera::GetViewMatrix() const
{
	return Matrix4x4(Vector4(_Transform.Right, false),	Vector4(_Transform.Up, false),	Vector4(_Transform.Forward, false), Vector4::UnitW).Tranpose()
		* Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-_Transform.GetWorldPosition()));
}

Matrix4x4 Camera::GetPerspectiveMatrix(int width, int height) const
{
	return Matrix4x4();
}

void Camera::SetLookAtRotation(const Vector3& InTargetPosition)
{
	_Transform.Forward = (_Transform.GetWorldPosition() - InTargetPosition).Normalize();
	if (_Transform.Forward.EqualsInTolerance(Vector3::UnitY) || _Transform.Forward.EqualsInTolerance(-Vector3::UnitY))
	{
		_Transform.Right = Vector3::UnitX;
	}
	else
	{
		_Transform.Right = Vector3::UnitY.Cross(_Transform.Forward).Normalize();
	}
	_Transform.Up = _Transform.Forward.Cross(_Transform.Right).Normalize();
}