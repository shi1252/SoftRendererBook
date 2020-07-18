#include "Precompiled.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"

Matrix4x4 Camera::GetViewMatrix() const
{
	//Matrix4x4 rot = Matrix4x4(Vector4(_Transform.Right, false), Vector4(_Transform.Up, false), Vector4(_Transform.Forward, false), Vector4::UnitW).Tranpose();
	//Matrix4x4 
	return Matrix4x4(Vector4(_Transform.Right, false),	Vector4(_Transform.Up, false),	Vector4(_Transform.Forward, false), Vector4::UnitW).Tranpose()
		* Matrix4x4(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-_Transform.GetWorldPosition()));
}

Matrix4x4 Camera::GetPerspectiveMatrix(int width, int height) const
{
	// tan(theta/2) = 1/d
	float d = 1 / tanf(Math::Deg2Rad(Fov) * 0.5f);
	float a = (float)height / (float)width;//1 / aspectRatio;
	return Matrix4x4(Vector4(d * a, 0.f, 0.f, 0.f), 
		Vector4(0.f, d, 0.f, 0.f), 
		//Vector4(0.f, 0.f, -(nearZ + farZ) / (farZ - nearZ), -(2 * nearZ * farZ) / (farZ - nearZ)),
		Vector4(0.f, 0.f, (nearZ + farZ) / (nearZ - farZ), -1.f), 
		Vector4(0.f, 0.f, (2 * nearZ * farZ) / (nearZ - farZ), 0.f));
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

	Rotator Rotation;
	Rotation.Pitch = Math::Rad2Deg(asinf(-_Transform.Forward.Y));
	float cp = cosf(Math::Deg2Rad(Rotation.Pitch));
	Rotation.Roll = Math::Rad2Deg(asinf(_Transform.Right.Y / cp));
	Rotation.Yaw = Math::Rad2Deg(acosf(_Transform.Forward.Z / cp));
	_Transform.SetWorldRotation(Rotation);
}

bool Camera::ViewSpaceFrustumCulling(Sphere& sphere)
{
	float tanHalfFov = tanf(Math::Deg2Rad(Fov * 0.5f));
	Plane frustum[6] =
	{
		Plane(Vector3::UnitZ, nearZ),
		Plane(-Vector3::UnitZ, -farZ),
		Plane(Vector3(-1.f, 0.f, tanHalfFov), 0.f).Normalize(),
		Plane(Vector3(1.f, 0.f, tanHalfFov), 0.f).Normalize(),
		Plane(Vector3(0.f, -1.f, tanHalfFov), 0.f).Normalize(),
		Plane(Vector3(0.f, 1.f, tanHalfFov), 0.f).Normalize(),
	};

	bool inside = true;
	for (int i = 0; i < 6; ++i)
	{
		float distance = frustum[i].Normal.Dot(sphere.Center);
		distance -= sphere.Radius;
		//if (signbit(distance))
		//	distance += sphere.Radius;
		//else
		//	distance -= sphere.Radius;
		if (distance + frustum[i].InverseDistance > 0.f)
		{
			inside = false;
			break;
		}
	}
	return inside;
}

bool Camera::ClipSpaceFrustumCulling(const Vector2& ScreenSize, const Sphere& sphere)
{
	Matrix4x4 projMat = GetPerspectiveMatrix(ScreenSize.X, ScreenSize.Y).Tranpose();

	Vector4 center = Vector4(sphere.Center);
	Vector4 xRad = Vector4(sphere.Radius, 0.f, 0.f, 0.f);
	Vector4 yRad = Vector4(0.f, sphere.Radius, 0.f, 0.f);
	Vector4 zRad = Vector4(0.f, 0.f, sphere.Radius, 0.f);

	bool isLeftInside = (center + xRad).Dot(projMat[3] + projMat[0]) >= 0.f;
	bool isRightInside = (center - xRad).Dot(projMat[3] - projMat[0]) >= 0.f;
	bool isTopInside = (center + yRad).Dot(projMat[3] + projMat[1]) >= 0.f;
	bool isBottomInside = (center - yRad).Dot(projMat[3] - projMat[1]) >= 0.f;
	bool isNearInside = (center - zRad).Dot(projMat[3] + projMat[2]) >= 0.f;
	bool isFarInside = (center + zRad).Dot(projMat[3] - projMat[2]) >= 0.f;

	if (isLeftInside && isRightInside && isTopInside && isBottomInside && isNearInside && isFarInside)
		return true;
	return false;
}