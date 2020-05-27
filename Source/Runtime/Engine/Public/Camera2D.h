#pragma once

class Camera2D : public GameObject2D
{
public:
	Camera2D(std::string name) : GameObject2D(name), _CircleBound(Circle()) {}

	Matrix3x3 GetViewMatrix() const;

	void SetCameraViewSize(const ScreenPoint& InScreenSize);

	Circle& GetCircleBound() { return _CircleBound; }
	Rectangle& GetRectBound() { return _RectBound; }

private:
	Circle _CircleBound;
	Rectangle _RectBound;
};