#pragma once

class Camera2D : public GameObject2D
{
public:
	Camera2D(std::string name) : GameObject2D(name) {}

	Matrix3x3 GetViewMatrix() const;
};