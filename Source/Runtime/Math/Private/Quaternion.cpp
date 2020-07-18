#include "Precompiled.h"
#include "Platform.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Rotator.h"
#include "Quaternion.h"

const Quaternion Quaternion::Identity(0.f, 0.f, 0.f, 1.f);

Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, const float& t)
{
	Quaternion result;
	result = q1 * (1.f - t) + q2 * t;
	result.Normalize();
	return result;
}


Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, const float& t)
{
	Quaternion result;
	
	float cosTheta = Math::Clamp(q1.W * q2.W + q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z, -1.f, 1.f);

	float s1, s2;
	if (-0.99999f < cosTheta && cosTheta < 0.99999f)
	{
		float theta = acosf(cosTheta);
		float invSin = 1.f / sinf(theta);
		s1 = sinf((1.f - t) * theta) * invSin;
		s2 = sinf(t * theta) * invSin;
	}
	else
	{
		return Lerp(q1, q2, t);
		//s1 = 1 - t;
		//s2 = t;
	}
	
	result = q1 * s1 + q2 * s2;

	return result;
}