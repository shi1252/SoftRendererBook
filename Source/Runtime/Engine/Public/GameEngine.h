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

#pragma region 2D Member functions
	std::vector<std::unique_ptr<GameObject2D>>& GetGameObjects2D() { return _GameObject2D; }
	Camera2D* GetCamera2D() { return _Camera2D.get(); }
	Quadtree* GetQuadtree() { return _Quadtree.get(); }

	void PushGameObject(GameObject2D* go);
	GameObject2D* FindGameObjectWithName2D(std::string name);
#pragma endregion

#pragma region 3D Member functions
	std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return _GameObject; }
	Camera* GetCamera() { return _Camera.get(); }

	void PushGameObject(GameObject* go);
	GameObject* FindGameObjectWithName(std::string name);
#pragma endregion
private:
#pragma region 2D Member Properties
	std::vector<std::unique_ptr<GameObject2D>> _GameObject2D;
	std::unordered_map<std::string, std::vector<GameObject2D*>> _GameObjectWithName2D;
	std::unique_ptr<Quadtree> _Quadtree;
	std::unique_ptr<Camera2D> _Camera2D;
	std::unordered_map<std::string, std::unique_ptr<Mesh2D>> _Mesh2D;
#pragma endregion

#pragma region 3D Member Properties
	std::vector<std::unique_ptr<GameObject>> _GameObject;
	std::unordered_map<std::string, std::vector<GameObject*>> _GameObjectWithName;
	std::unique_ptr<Camera> _Camera;
	std::unordered_map<std::string, std::unique_ptr<Mesh>> _Mesh;
#pragma endregion

	ScreenPoint _ViewportSize;

	InputManager _InputManager;
};