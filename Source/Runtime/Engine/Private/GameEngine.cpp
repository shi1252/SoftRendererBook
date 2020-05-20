#include "Precompiled.h"
#include "InputManager.h"
#include "Mesh2D.h"
#include "Transform2D.h"
#include "GameObject2D.h"
#include "Camera2D.h"
#include "GameEngine.h"
#include <random>

bool GameEngine::Init()
{
	if (!_InputManager.GetXAxis || !_InputManager.GetYAxis || !_InputManager.SpacePressed)
	{
		return false;
	}

	if (!LoadResource())
	{
		return false;
	}

	if (!LoadScene())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	static float initScale = 100.f;
	_GameObject.push_back(std::make_unique<GameObject2D>("Player", _Mesh["QuadMesh"].get()));
	_GameObject[_GameObject.size() - 1]->GetTransform2D().SetScale(Vector2(initScale, initScale));

	_GameObject.push_back(std::make_unique<Camera2D>("Camera"));

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(-1000.f, 1000.f);

	for (int i = 0; i < 100; ++i)
	{
		_GameObject.push_back(std::make_unique<GameObject2D>("QuadObject", _Mesh["QuadMesh"].get()));
		_GameObject[_GameObject.size() - 1]->GetTransform2D().SetScale(Vector2(initScale, initScale));
		_GameObject[_GameObject.size() - 1]->GetTransform2D().SetPosition(Vector2(dist(mt), dist(mt)));
	}

	return true;
}

bool GameEngine::LoadResource()
{
	_Mesh["QuadMesh"] = std::make_unique<Mesh2D>();
	static float squareHalfSize = .5f;
	static int vertexCount = 4;
	static int triangleCount = 2;
	static int indexCount = triangleCount * 3;

	_Mesh["QuadMesh"].get()->GetVertices() = {
		Vector2(-squareHalfSize, -squareHalfSize),
		Vector2(-squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, squareHalfSize),
		Vector2(squareHalfSize, -squareHalfSize)
	};

	_Mesh["QuadMesh"].get()->GetIndices() = {
		0, 1, 2,
		0, 2, 3
	};

	return true;
}

GameObject2D* GameEngine::FindGameObjectWithName(std::string name)
{
	for (auto& go : _GameObject)
	{
		if (go.get()->GetName().compare(name) == 0)
			return go.get();
	}

	return nullptr;
}