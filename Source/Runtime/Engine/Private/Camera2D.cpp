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
