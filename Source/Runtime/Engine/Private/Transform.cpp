#include "Precompiled.h"
#include "Transform.h"

Matrix4x4 Transform::GetModelingMatrix() const
{
	Matrix4x4 translateMat(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(Position));

	Matrix4x4 rotationMat = Rotation.ToRotationMatrix();

	Matrix4x4 scaleMat(Vector4::UnitX * Scale.X, Vector4::UnitY * Scale.Y, Vector4::UnitZ * Scale.Z, Vector4::UnitW);
	return translateMat * rotationMat * scaleMat;
}

Matrix4x4 Transform::GetInvModelingMatrix() const
{
	Matrix4x4 translateMat(Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4(-Position));

	Matrix4x4 rotationMat = WorldRotation.ToRotationMatrix();
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

	Matrix4x4 rotationMat = Rotation.ToRotationMatrix();
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
	Rotation *= _ParentPtr->WorldRotation.Inverse();

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
	Right = Rotation.RotateVector(Vector3(1, 0, 0));
	Up = Rotation.RotateVector(Vector3(0, 1, 0));
	Forward = Rotation.RotateVector(Vector3(0, 0, 1));
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

void Transform::AddYawRotation(float yaw)
{
	Rotation *= Quaternion(Rotator(yaw, 0, 0));
	Update();
}

void Transform::AddRollRotation(float roll)
{
	Rotation *= Quaternion(Rotator(0, roll, 0));
	Update();
}

void Transform::AddPitchRotation(float pitch)
{
	Rotation *= Quaternion(Rotator(0, 0, pitch));
	Update();
}

void Transform::SetLocalRotation(Rotator InDegree)
{
	Rotation = Quaternion(InDegree);
	Update();
}

void Transform::SetLocalRotation(Quaternion InQuaternion)
{
	Rotation = InQuaternion;
	Update();
}

void Transform::AddLocalRotation(Rotator InDeltaDegree)
{
	Rotation *= Quaternion(InDeltaDegree);
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
	WorldRotation = Quaternion(InDegree);
	if (_ParentPtr == nullptr)
	{
		Rotation = WorldRotation;
	}
	else
	{
		Rotation = WorldRotation * _ParentPtr->WorldRotation.Inverse();
	}
	Update();
}

void Transform::SetWorldRotation(Quaternion InQuaternion)
{
	WorldRotation = InQuaternion;
	if (_ParentPtr == nullptr)
	{
		Rotation = WorldRotation;
	}
	else
	{
		Rotation = WorldRotation * _ParentPtr->WorldRotation.Inverse();
	}
	Update();
}

void Transform::AddWorldRotation(Rotator InDeltaDegree)
{
	WorldRotation *= Quaternion(InDeltaDegree);
	if (_ParentPtr == nullptr)
	{
		Rotation = WorldRotation;
	}
	else
	{
		Rotation = WorldRotation * _ParentPtr->WorldRotation.Inverse();
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