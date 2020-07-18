#pragma once

class Transform
{
public:
	Transform() = default;

public:
	void SetLocalPosition(const Vector3& InPosition);
	void AddLocalPosition(const Vector3& InDeltaPosition);
	void SetLocalScale(const Vector3& InScale);
	void AddYawRotation(float yaw);
	void AddRollRotation(float roll);
	void AddPitchRotation(float pitch);
	void SetLocalRotation(Rotator InDegree);
	void SetLocalRotation(Quaternion InQuaternion);
	void AddLocalRotation(Rotator InDeltaDegree);
	void SetWorldScale(const Vector3& InScale);
	void SetWorldRotation(Rotator InDegree);
	void SetWorldRotation(Quaternion InQuaternion);
	void AddWorldRotation(Rotator InDeltaDegree);
	void SetWorldPosition(const Vector3& InPosition);
	void AddWorldPosition(const Vector3& InDeltaPosition);

	Vector3 GetLocalPosition() const { return Position; }
	Vector3 GetLocalScale() const { return Scale; }
	Quaternion GetLocalRotation() const { return Rotation; }

	Vector3 GetWorldPosition() const { return WorldPosition; }
	Vector3 GetWorldScale() const { return WorldScale; }
	Quaternion GetWorldRotation() const { return WorldRotation; }

	Matrix4x4 GetModelingMatrix() const;
	Matrix4x4 GetInvModelingMatrix() const;
	Matrix4x4 GetWorldModelingMatrix() const;
	Matrix4x4 GetInvWorldModelingMatrix() const;
	const Vector3& GetLocalX() const { return Right; }
	const Vector3& GetLocalY() const { return Up; }

	FORCEINLINE Transform* GetParent() const { return _ParentPtr; }
	FORCEINLINE Transform& GetRoot() {
		Transform* current = this;
		Transform* parent = nullptr;
		while ((parent = current->GetParent()) != nullptr)
		{
			current = parent;
		}
		return *current;
	}
	FORCEINLINE Transform& GetChild(BYTE InIndex) const
	{
		assert(InIndex < _Children.size());
		return *_Children[InIndex];
	}
	bool SetParent(Transform* InTransformPtr);

	void Update();

	Vector3 GetForward() const { return Forward; }

private:
	void CalculateLocalAxis();
	void CalculateMatrices();

	Vector3 Position = Vector3::Zero;
	Quaternion Rotation = Quaternion::Identity;
	Vector3 Scale = Vector3::One;

	Vector3 Right = Vector3::UnitX;
	Vector3 Up = Vector3::UnitY;
	Vector3 Forward = Vector3::UnitZ;

	Vector3 WorldPosition = Vector3::Zero;
	Quaternion WorldRotation = Quaternion::Identity;
	Vector3 WorldScale = Vector3::One;

	Matrix4x4 _LocalTRS;
	Matrix4x4 _WorldTRS;

	Transform* _ParentPtr = nullptr;
	std::vector<Transform*> _Children;

	friend class GameObject;
	friend class Camera;
};