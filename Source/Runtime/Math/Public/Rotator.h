#pragma once

struct Rotator
{
public:
	Rotator() = default;
	FORCEINLINE Rotator(float InYaw, float InRoll, float InPitch) : Yaw(InYaw), Roll(InRoll), Pitch(InPitch) { }
	FORCEINLINE void Clamp()
	{
		Yaw = GetAxisClampedValue(Yaw);
		Roll = GetAxisClampedValue(Roll);
		Pitch = GetAxisClampedValue(Pitch);
	}

	FORCEINLINE float GetAxisClampedValue(float InRotatorValue)
	{
		float angle = Math::FMod(InRotatorValue, 360.f);
		if (angle < 0.f)
		{
			angle += 360.f;
		}

		return angle;
	}

	static const Rotator Identity;

	FORCEINLINE Rotator operator+(const Rotator& rhs);
	FORCEINLINE Rotator operator-(const Rotator& rhs);
	FORCEINLINE Rotator& operator+=(const Rotator& rhs);
	FORCEINLINE Rotator& operator-=(const Rotator& rhs);

public:
	float Yaw = 0.f;
	float Roll = 0.f;
	float Pitch = 0.f;
};

FORCEINLINE Rotator Rotator::operator+(const Rotator& rhs)
{
	return Rotator(Pitch + rhs.Pitch, Yaw + rhs.Yaw, Roll + rhs.Roll);
}

FORCEINLINE Rotator Rotator::operator-(const Rotator& rhs)
{
	return Rotator(Pitch - rhs.Pitch, Yaw - rhs.Yaw, Roll - rhs.Roll);
}

FORCEINLINE Rotator& Rotator::operator+=(const Rotator& rhs)
{
	Pitch += rhs.Pitch;
	Yaw += rhs.Yaw;
	Roll += rhs.Roll;
	return *this;
}

FORCEINLINE Rotator& Rotator::operator-=(const Rotator& rhs)
{
	Pitch -= rhs.Pitch;
	Yaw -= rhs.Yaw;
	Roll -= rhs.Roll;
	return *this;
}