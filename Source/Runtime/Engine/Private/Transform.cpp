#include "Precompiled.h"
#include "Transform.h"

Matrix4x4 Transform::GetModelingMatrix() const
{
	Matrix4x4 translateMat(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(Position));

	Matrix4x4 rotationMat(
		Vector4(Right, false),
		Vector4(Up, false),
		Vector4(Forward, false),
		Vector4::UnitW);

	Matrix4x4 scaleMat(Vector4::UnitX * Scale.X, Vector4::UnitY * Scale.Y, Vector4::UnitZ * Scale.Z, Vector4::UnitW);
	return translateMat * rotationMat * scaleMat;
}

Matrix4x4 Transform::GetInvModelingMatrix() const
{
	Matrix4x4 translateMat(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-Position));

	float sin, cos;
	Math::GetSinCos(sin, cos, WorldRotation.Yaw);

	Vector3 WorldRight = Vector3::UnitX;//Vector3(cos, sin);
	Vector3 WorldUp = Vector3::UnitY;//Vector3(-sin, cos);
	Vector3 WorldForward = Vector3::UnitZ;

	Matrix4x4 rotationMat(
		Vector4(WorldRight, false),
		Vector4(WorldUp, false),
		Vector4(WorldForward, false),
		Vector4::UnitZ);
	rotationMat = rotationMat.Tranpose();

	Matrix4x4 scaleMat(Vector4::UnitX * (1.f / WorldScale.X), Vector4::UnitY * (1.f / WorldScale.Y), Vector4::UnitZ * (1.f / WorldScale.Z), Vector4::UnitW);
	return scaleMat * rotationMat * translateMat;
}

Matrix4x4 Transform::GetWorldModelingMatrix() const
{
	return _WorldTRS;
}

Matrix4x4 Transform::GetInvWorldModelingMatrix() const
{
	Matrix4x4 translateMat(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-WorldPosition));

	Matrix4x4 rotationMat(
		Vector4(Right, false),
		Vector4(Up, false),
		Vector4(Forward, false),
		Vector4::UnitZ);
	rotationMat = rotationMat.Tranpose();

	Matrix4x4 scaleMat(Vector4::UnitX * (1.f / Scale.X), Vector4::UnitY * (1.f / Scale.Y), Vector4::UnitZ * (1.f / Scale.Z), Vector4::UnitW);

	Matrix4x4 result = scaleMat * rotationMat * translateMat;
	if (_ParentPtr)
	{
		result = _ParentPtr->GetInvWorldModelingMatrix() * result;
	}
	return result;
}

bool Transform::SetParent(Transform* InTransformPtr)
{
	if (InTransformPtr == nullptr)
		return false;

	for (Transform* current = InTransformPtr; current != nullptr; current = current->GetParent())
	{
		if (this == current)
			return false;
	}

	if (_ParentPtr != nullptr)
	{
		auto it = std::find(_Children.begin(), _Children.end(), this);
		assert(it != _ParentPtr->_Children.end());
		_ParentPtr->_Children.erase(it);
		_ParentPtr = nullptr;
	}

	InTransformPtr->_Children.push_back(this);
	_ParentPtr = InTransformPtr;

	Vector3 invScale = Vector3(1.f / _ParentPtr->WorldScale.X, 1.f / _ParentPtr->WorldScale.Y, 1.f / _ParentPtr->WorldScale.Z);
	Scale = Vector3(Scale.X * invScale.X, Scale.Y * invScale.Y, Scale.Z * invScale.Z);
	Rotation -= _ParentPtr->WorldRotation;

	Position = _ParentPtr->GetInvWorldModelingMatrix() * Position;
	Update();

	return true;
}

void Transform::Update()
{
	CalculateLocalAxis();
	CalculateMatrices();

	for (auto&& i : _Children)
	{
		i->Update();
	}
}

void Transform::CalculateLocalAxis()
{
	float cy, sy, cp, sp, cr, sr;
	Math::GetSinCos(sy, cy, Rotation.Yaw);
	Math::GetSinCos(sp, cp, Rotation.Pitch);
	Math::GetSinCos(sr, cr, Rotation.Roll);

	Right = Vector3(cy * cr + sy * sp * sr, cp * sr, -sy * cr + cy * sp * sr);
	Up = Vector3(-cy * sr + sy * sp * cr, cp * cr, sy * sr + cy * sp * cr);
	Forward = Vector3(sy * cp, -sp, cy * cp);
}

void Transform::CalculateMatrices()
{
	_LocalTRS = GetModelingMatrix();

	if (_ParentPtr == nullptr)
	{
		WorldScale = Scale;
		WorldRotation = Rotation;
		WorldPosition = Position;
		_WorldTRS = _LocalTRS;
	}
	else
	{
		WorldScale = Vector3(_ParentPtr->WorldScale.X * Scale.X, _ParentPtr->WorldScale.Y * Scale.Y, _ParentPtr->WorldScale.Z * Scale.Z);
		WorldRotation = _ParentPtr->WorldRotation + Rotation;
		WorldPosition = _ParentPtr->_WorldTRS * Position;
		_WorldTRS = _ParentPtr->_WorldTRS * _LocalTRS;
	}
}

void Transform::SetLocalPosition(const Vector3& InPosition)
{
	Position = InPosition;
	Update();
}

void Transform::AddLocalPosition(const Vector3& InDeltaPosition)
{
	Position += InDeltaPosition;
	Update();
}

void Transform::SetLocalScale(const Vector3& InScale)
{
	Scale = InScale;
	Update();
}

void Transform::SetLocalRotation(Rotator InDegree)
{
	Rotation = InDegree;
	Update();
}

void Transform::AddLocalRotation(Rotator InDeltaDegree)
{
	Rotation += InDeltaDegree;
	Update();
}

void Transform::SetWorldScale(const Vector3& InScale)
{
	WorldScale = InScale;
	if (_ParentPtr == nullptr)
	{
		Scale = WorldScale;
	}
	else
	{
		Vector3 invScale = Vector3(1.f / _ParentPtr->WorldScale.X, 1.f / _ParentPtr->WorldScale.Y, 1.f / _ParentPtr->WorldScale.Z);
		Scale = Vector3(WorldScale.X * invScale.X, WorldScale.Y * invScale.Y, WorldScale.Z * invScale.Z);
	}
	Update();
}

void Transform::SetWorldRotation(Rotator InDegree)
{
	WorldRotation = InDegree;
	if (_ParentPtr == nullptr)
	{
		Rotation = WorldRotation;
	}
	else
	{
		Rotation = WorldRotation - _ParentPtr->WorldRotation;
	}
	Update();
}

void Transform::AddWorldRotation(Rotator InDeltaDegree)
{
	WorldRotation += InDeltaDegree;
	if (_ParentPtr == nullptr)
	{
		Rotation = WorldRotation;
	}
	else
	{
		Rotation = WorldRotation - _ParentPtr->WorldRotation;
	}
	Update();
}

void Transform::SetWorldPosition(const Vector3& InPosition)
{
	WorldPosition = InPosition;
	if (_ParentPtr == nullptr)
	{
		Position = WorldPosition;
	}
	else
	{
		Position = _ParentPtr->GetInvWorldModelingMatrix() * WorldPosition;
	}
	Update();
}

void Transform::AddWorldPosition(const Vector3& InDeltaPosition)
{
	WorldPosition += InDeltaPosition;
	if (_ParentPtr == nullptr)
	{
		Position = WorldPosition;
	}
	else
	{
		Position = _ParentPtr->GetInvWorldModelingMatrix() * WorldPosition;
	}
	Update();
}