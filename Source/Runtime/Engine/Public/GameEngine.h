#pragma once

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init(const ScreenPoint& InScreenSize);
	bool LoadScene();
	bool LoadResource();

	InputManager& GetInputManager() { return _InputManager; }
	std::vector<std::unique_ptr<GameObject2D>>& GetGameObjects() { return _GameObject; }
	Camera2D* GetCamera() { return _Camera.get(); }
	Quadtree* GetQuadtree() { return _Quadtree.get(); }

	void PushGameObject(GameObject2D* go);
	GameObject2D* FindGameObjectWithName(std::string name);

private:
	std::vector<std::unique_ptr<GameObject2D>> _GameObject;
	std::unordered_map<std::string, std::vector<GameObject2D*>> _GameObjectWithName;
	std::unique_ptr<Quadtree> _Quadtree;
	std::unique_ptr<Camera2D> _Camera;
	std::unordered_map<std::string, std::unique_ptr<Mesh2D>> _Mesh;

	ScreenPoint _ViewportSize;

	InputManager _InputManager;
};