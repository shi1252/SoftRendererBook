#pragma once

struct Quaternion
{
public:
	FORCEINLINE Quaternion() = default;
	FORCEINLINE explicit Quaternion(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }
	FORCEINLINE explicit Quaternion(const Rotator & InRotator)
	{
		float cy = cosf(Math::Deg2Rad(InRotator.Yaw * 0.5f)), sy = sinf(Math::Deg2Rad(InRotator.Yaw * 0.5f));
		float cp = cosf(Math::Deg2Rad(InRotator.Pitch * 0.5f)), sp = sinf(Math::Deg2Rad(InRotator.Pitch * 0.5f));
		float cr = cosf(Math::Deg2Rad(InRotator.Roll * 0.5f)), sr = sinf(Math::Deg2Rad(InRotator.Roll * 0.5f));
		W = cy * cp * cr + sy * sp * sr;
		X = cy * sp * cr + sy * cp * sr;
		Y = sy * cp * cr - cy * sp * sr;
		Z = cy * cp * sr - sy * sp * cr;
	}

	FORCEINLINE Quaternion operator*(const Quaternion & InQuaternion) const;
	FORCEINLINE Quaternion& operator*=(const Quaternion & InQuaternion);
	FORCEINLINE Quaternion operator*(const float& value) const;
	FORCEINLINE Quaternion& operator*=(const float& value);
	FORCEINLINE Quaternion operator+(const Quaternion& InQuaternion) const;
	FORCEINLINE Vector3 RotateVector(const Vector3 & InVector) const;
	FORCEINLINE Vector3 GetForwardVector() const;
	FORCEINLINE Quaternion Inverse() const;
	FORCEINLINE void Normalize();
	FORCEINLINE Rotator ToRotator() const;
	FORCEINLINE bool IsUnitQuaternion() const
	{
		float size = sqrtf(X * X + Y * Y + Z * Z + W * W);
		if (Math::NearlyEqual(size - 1.f))
		{
			return true;
		}

		return false;
	}
	FORCEINLINE Matrix4x4 ToRotationMatrix() const;
	FORCEINLINE float Scalar() { return W; }
	FORCEINLINE Vector3 Vector() { return Vector3(X, Y, Z); }
	static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, const float& t);
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float& t);

	static const Quaternion Identity;

public:
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
	float W = 1.f;
};

FORCEINLINE Quaternion Quaternion::operator*(const Quaternion& InQuaternion) const
{
	Quaternion result;
	
	Vector3 v1 = Vector3(X, Y, Z), v2 = Vector3(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);
	Vector3 v = v2 * W + v1 * InQuaternion.W + v1.Cross(v2);

	result.W = W * InQuaternion.W - v1.Dot(v2);
	result.X = v.X;
	result.Y = v.Y;
	result.Z = v.Z;

	return result;
}

FORCEINLINE Quaternion& Quaternion::operator*=(const Quaternion& InQuaternion)
{
	Vector3 v1 = Vector3(X, Y, Z), v2 = Vector3(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);
	Vector3 v = v2 * W + v1 * InQuaternion.W + v1.Cross(v2);

	W = W * InQuaternion.W - v1.Dot(v2);
	X = v.X;
	Y = v.Y;
	Z = v.Z;

	return *this;
}

FORCEINLINE Quaternion Quaternion::operator*(const float& value) const
{
	Quaternion result(*this);
	result.W *= value;
	result.X *= value;
	result.Y *= value;
	result.Z *= value;
	return result;
}

FORCEINLINE Quaternion& Quaternion::operator*=(const float& value)
{
	W *= value;
	X *= value;
	Y *= value;
	Z *= value;

	return *this;
}

FORCEINLINE Quaternion Quaternion::operator+(const Quaternion& InQuaternion) const
{
	Quaternion result(*this);
	result.W += InQuaternion.W;
	result.X += InQuaternion.X;
	result.Y += InQuaternion.Y;
	result.Z += InQuaternion.Z;
	return result;
}

FORCEINLINE Vector3 Quaternion::RotateVector(const Vector3& InVector) const
{
	Vector3 q(X, Y, Z);
	Vector3 t = q.Cross(InVector) * 2.f;
	return (InVector + t * W + q.Cross(t));
}

FORCEINLINE Vector3 Quaternion::GetForwardVector() const
{
	return RotateVector(Vector3::UnitZ);
}

FORCEINLINE Quaternion Quaternion::Inverse() const
{
	return Quaternion(-X, -Y, -Z, W);
}

FORCEINLINE void Quaternion::Normalize()
{
	float sqrMag = X * X + Y * Y + Z * Z + W * W;
	if (sqrMag >= KINDA_SMALL_NUMBER)
	{
		float magInv = 1.f / sqrtf(sqrMag);
		W *= magInv;
		X *= magInv;
		Y *= magInv;
		Z *= magInv;
	}
	else
	{
		*this = Quaternion::Identity;
	}
}

FORCEINLINE Rotator Quaternion::ToRotator() const
{
	Rotator result;

	float pitchHalfRad = W * X - Y * Z;
	float yawY = 2.f * (W * Y + X * Z);
	float yawX = 1.f - 2.f * (X * X + Y * Y);

	result.Yaw = Math::Rad2Deg(atan2f(yawY, yawX));

	float singularity = 0.499999f;
	if (pitchHalfRad > singularity)
	{
		result.Pitch = 90.f;
		result.Roll = result.Yaw - Math::Rad2Deg(2.f * atan2f(Y, W));
	}
	else if (pitchHalfRad < -singularity)
	{
		result.Pitch = -90.f;
		result.Roll = -result.Yaw + Math::Rad2Deg(2.f * atan2f(Y, W));
	}
	else
	{
		result.Pitch = Math::Rad2Deg(asinf(2.f * pitchHalfRad));
		result.Roll = Math::Rad2Deg(atan2f(2.f * (W * Z + X * Y), 1.f - 2.f * (Z * Z + X * X)));
	}

	return result;
}

FORCEINLINE Matrix4x4 Quaternion::ToRotationMatrix() const
{
	Matrix4x4 result;

	float xs = X * X, ys = Y * Y, zs = Z * Z;
	float wx = W * X, wy = W * Y, wz = W * Z;
	float xy = X * Y, xz = X * Z;
	float yz = Y * Z;

	result.Cols[0] = Vector4(1.f - 2.f * (ys + zs), 2.f * (xy + wz), 2.f * (xz - wy), 0.f);
	result.Cols[1] = Vector4(2.f * (xy - wz), 1.f - 2.f * (xs + zs), 2.f * (wx + yz), 0.f);
	result.Cols[2] = Vector4(2.f * (wy + xz), 2.f * (yz - wx), 1.f - 2.f * (xs + ys), 0.f);
	result.Cols[3] = Vector4(0.f, 0.f, 0.f, 1.f);

	return result;
}