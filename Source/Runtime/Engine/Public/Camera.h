#pragma once

class Camera : public GameObject
{
public:
	Camera(std::string name) : GameObject(name) {}

	Matrix4x4 GetViewMatrix() const;
	Matrix4x4 GetPerspectiveMatrix(int width, int height) const;

	void SetLookAtRotation(const Vector3& InTargetPosition);

	//void SetCameraViewSize(const ScreenPoint& InScreenSize);

	//Circle& GetCircleBound() { return _CircleBound; }
	//Rectangle& GetRectBound() { return _RectBound; }

private:
	float nearZ = 0.3f;
	float farZ = 1000.f;
	float Fov = 60.f;
	float aspectRatio = 4 / 3;
	//Circle _CircleBound;
	//Rectangle _RectBound;
};