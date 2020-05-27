#include "Precompiled.h"
#include "Transform.h"

Matrix3x3 Transform::GetModelingMatrix() const
{
	Vector3 tCol(Position.X, Position.Y, 1);
	Matrix3x3 translationMat(Matrix3x3::Identity.Cols[0], Matrix3x3::Identity.Cols[1], tCol);
	float rotation = Math::Deg2Rad(Rotation);
	Vector3 rXAxis(cosf(rotation), sinf(rotation), 0), rYAxis(-sinf(rotation), cosf(rotation), 0);
	Matrix3x3 rotationMat(rXAxis, rYAxis, Matrix3x3::Identity.Cols[2]);
	Matrix3x3 scaleMat(Matrix3x3::Identity.Cols[0] * Scale.X, Matrix3x3::Identity.Cols[0] * Scale.Y, Matrix3x3::Identity.Cols[2]);
	return translationMat * rotationMat * scaleMat;
}

void Transform::CalculateLocalAxis()
{
	float cos = cosf(Math::Deg2Rad(Rotation));
	float sin = sinf(Math::Deg2Rad(Rotation));
	Right = Vector2(cos, sin);
	Up = Vector2(-sin, cos);
}