#pragma once

class Camera : public GameObject
{
public:
	Camera(std::string name) : GameObject(name) {}

	Matrix4x4 GetViewMatrix() const;
	Matrix4x4 GetPerspectiveMatrix(int width, int height) const;

	void SetLookAtRotation(const Vector3& InTargetPosition);

	bool ViewSpaceFrustumCulling(Sphere& sphere);
	bool ClipSpaceFrustumCulling(const Vector2& ScreenSize, const Sphere& sphere);

	//void SetCameraViewSize(const ScreenPoint& InScreenSize);

	//Circle& GetCircleBound() { return _CircleBound; }
	//Rectangle& GetRectBound() { return _RectBound; }

private:
	float nearZ = 5.5f;
	float farZ = 3000.f;
	float Fov = 60.f;
	float aspectRatio = 4.f / 3.f;
	//Circle _CircleBound;
	//Rectangle _RectBound;
};