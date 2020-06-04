#include "Precompiled.h"
#include "Transform2D.h"

Matrix3x3 Transform2D::GetModelingMatrix() const
{
	Matrix3x3 translateMat(Vector3::UnitX, Vector3::UnitY, Vector3(Position));

	Matrix3x3 rotationMat(
		Vector3(Right, false),
		Vector3(Up, false),
		Vector3::UnitZ);

	Matrix3x3 scaleMat(Vector3::UnitX * Scale.X, Vector3::UnitY * Scale.Y, Vector3::UnitZ);
	return translateMat * rotationMat * scaleMat;
}

Matrix3x3 Transform2D::GetInvModelingMatrix() const
{
	Matrix3x3 translateMat(Vector3::UnitX, Vector3::UnitY, Vector3(-Position));

	float sin, cos;
	Math::GetSinCos(sin, cos, WorldRotation);

	Vector2 WorldRight = Vector2(cos, sin);
	Vector2 WorldUp = Vector2(-sin, cos);

	Matrix3x3 rotationMat(
		Vector3(WorldRight, false),
		Vector3(WorldUp, false),
		Vector3::UnitZ);
	rotationMat = rotationMat.Tranpose();

	Matrix3x3 scaleMat(Vector3::UnitX * (1.f / WorldScale.X), Vector3::UnitY * (1.f / WorldScale.Y), Vector3::UnitZ);
	return scaleMat * rotationMat * translateMat;
}

Matrix3x3 Transform2D::GetWorldModelingMatrix() const
{
	return _WorldTRS;
}

Matrix3x3 Transform2D::GetInvWorldModelingMatrix() const
{
	Matrix3x3 translateMat(Vector3::UnitX, Vector3::UnitY, Vector3(-WorldPosition));

	Matrix3x3 rotationMat(
		Vector3(Right, false),
		Vector3(Up, false),
		Vector3::UnitZ);
	rotationMat = rotationMat.Tranpose();

	Matrix3x3 scaleMat(Vector3::UnitX * (1.f / Scale.X), Vector3::UnitY * (1.f / Scale.Y), Vector3::UnitZ);

	Matrix3x3 result = scaleMat * rotationMat * translateMat;
	if (_ParentPtr)
	{
		result = _ParentPtr->GetInvWorldModelingMatrix() * result;
	}
	return result;
}

bool Transform2D::SetParent(Transform2D* InTransformPtr)
{
	if (InTransformPtr == nullptr)
		return false;

	for (Transform2D* current = InTransformPtr; current != nullptr; current = current->GetParent())
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

	Vector2 invScale = Vector2(1.f / _ParentPtr->WorldScale.X, 1.f / _ParentPtr->WorldScale.Y);
	Scale = Vector2(Scale.X * invScale.X, Scale.Y * invScale.Y);
	Rotation -= _ParentPtr->WorldRotation;

	Position = _ParentPtr->GetInvWorldModelingMatrix() * Position;
	Update();

	return true;
}

void Transform2D::Update()
{
	CalculateLocalAxis();
	CalculateMatrices();

	for (auto&& i : _Children)
	{
		i->Update();
	}
}

void Transform2D::CalculateLocalAxis()
{
	float sin, cos;
	Math::GetSinCos(sin, cos, Rotation);

	Right = Vector2(cos, sin);
	Up = Vector2(-sin, cos);
}

void Transform2D::CalculateMatrices()
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
		WorldScale = Vector2(_ParentPtr->WorldScale.X * Scale.X, _ParentPtr->WorldScale.Y * Scale.Y);
		WorldRotation = _ParentPtr->WorldRotation + Rotation;
		WorldPosition = _ParentPtr->_WorldTRS * Position;
		_WorldTRS = _ParentPtr->_WorldTRS * _LocalTRS;
	}
}

void Transform2D::SetLocalPosition(const Vector2& InPosition)
{
	Position = InPosition;
	Update();
}

void Transform2D::AddLocalPosition(const Vector2& InDeltaPosition)
{
	Position += InDeltaPosition;
	Update();
}

void Transform2D::SetLocalScale(const Vector2& InScale)
{
	Scale = InScale;
	Update();
}

void Transform2D::SetLocalRotation(float InDegree)
{
	Rotation = InDegree;
	Update();
}

void Transform2D::AddLocalRotation(float InDeltaDegree)
{
	Rotation += InDeltaDegree;
	Update();
}

void Transform2D::SetWorldScale(const Vector2& InScale)
{
	WorldScale = InScale;
	if (_ParentPtr == nullptr)
	{
		Scale = WorldScale;
	}
	else
	{
		Vector2 invScale = Vector2(1.f / _ParentPtr->WorldScale.X, 1.f / _ParentPtr->WorldScale.Y);
		Scale = Vector2(WorldScale.X * invScale.X, WorldScale.Y * invScale.Y);
	}
	Update();
}

void Transform2D::SetWorldRotation(float InDegree)
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

void Transform2D::AddWorldRotation(float InDeltaDegree)
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

void Transform2D::SetWorldPosition(const Vector2& InPosition)
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

void Transform2D::AddWorldPosition(const Vector2& InDeltaPosition)
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
