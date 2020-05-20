#pragma once

class GameEngine
{
public:
	GameEngine() = default;

public:
	bool Init();
	bool LoadScene();
	bool LoadResource();

	InputManager& GetInputManager() { return _InputManager; }
	std::vector<std::unique_ptr<GameObject2D>>& GetGameObjects() { return _GameObject; }

	GameObject2D* FindGameObjectWithName(std::string name);

private:
	std::vector<std::unique_ptr<GameObject2D>> _GameObject;
	std::unordered_map<std::string, std::unique_ptr<Mesh2D>> _Mesh;

	InputManager _InputManager;
};