#pragma once

class Transform2D
{
public:
	Transform2D() = default;

public:
	void SetLocalPosition(const Vector2& InPosition);
	void AddLocalPosition(const Vector2& InDeltaPosition);
	void SetLocalScale(const Vector2& InScale);
	void SetLocalRotation(float InDegree);
	void AddLocalRotation(float InDeltaDegree);
	void SetWorldScale(const Vector2& InScale);
	void SetWorldRotation(float InDegree);
	void AddWorldRotation(float InDeltaDegree);
	void SetWorldPosition(const Vector2& InPosition);
	void AddWorldPosition(const Vector2& InDeltaPosition);

	Vector2 GetLocalPosition() const { return Position; }
	Vector2 GetLocalScale() const { return Scale; }
	float GetLocalRotation() const { return Rotation; }

	Vector2 GetWorldPosition() const { return WorldPosition; }
	Vector2 GetWorldScale() const { return WorldScale; }
	float GetWorldRotation() const { return WorldRotation; }

	//void SetPosition(const Vector2& InPosition) { Position = InPosition; }
	//void AddPosition(const Vector2& InDeltaPosition) { Position += InDeltaPosition; }
	//void SetScale(const Vector2& InScale) { Scale = InScale; }
	//void SetRotation(float InDegree) { Rotation = InDegree; CalculateLocalAxis(); }
	//void AddRotation(float InDegree) { Rotation += InDegree; CalculateLocalAxis(); }

	//Vector2 GetPosition() const { return Position; }
	//Vector2 GetScale() const { return Scale; }
	//float GetRotation() const { return Rotation; }

	Matrix3x3 GetModelingMatrix() const;
	Matrix3x3 GetInvModelingMatrix() const;
	Matrix3x3 GetWorldModelingMatrix() const;
	Matrix3x3 GetInvWorldModelingMatrix() const;
	const Vector2& GetLocalX() const { return Right; }
	const Vector2& GetLocalY() const { return Up; }

	FORCEINLINE Transform2D* GetParent() const { return _ParentPtr; }
	FORCEINLINE Transform2D& GetRoot() {
		Transform2D* current = this;
		Transform2D* parent = nullptr;
		while ((parent = current->GetParent()) != nullptr)
		{
			current = parent;
		}
		return *current;
	}
	FORCEINLINE Transform2D& GetChild(BYTE InIndex) const
	{
		assert(InIndex < _Children.size());
		return *_Children[InIndex];
	}
	bool SetParent(Transform2D* InTransformPtr);
	
	void Update();

private:
	void CalculateLocalAxis();
	void CalculateMatrices();

	Vector2 Position = Vector2::Zero;
	float Rotation = 0.f;
	Vector2 Scale = Vector2::One;

	Vector2 Right = Vector2::UnitX;
	Vector2 Up = Vector2::UnitY;

	Vector2 WorldPosition = Vector2::Zero;
	float WorldRotation = 0.f;
	Vector2 WorldScale = Vector2::One;

	Matrix3x3 _LocalTRS;
	Matrix3x3 _WorldTRS;

	Transform2D* _ParentPtr = nullptr;
	std::vector<Transform2D*> _Children;

	friend class GameObject2D;
};