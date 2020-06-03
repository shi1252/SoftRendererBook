#include "Precompiled.h"
#include "InputManager.h"
#include "Mesh2D.h"
#include "Transform2D.h"
#include "GameObject2D.h"
#include "Quadtree.h"
#include "Camera2D.h"
#include "GameEngine.h"
#include <random>

bool GameEngine::Init(const ScreenPoint& InScreenSize)
{
	_ViewportSize = InScreenSize;

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
	static float initScale = 50.f;
	auto player = GameObject2D("Player", _Mesh["QuadMesh"].get());
	player.GetTransform2D().SetLocalScale(Vector2(initScale, initScale));
	PushGameObject(&player);

	auto camera = Camera2D("Camera");
	camera.SetCameraViewSize(_ViewportSize);
	_Camera = std::make_unique<Camera2D>(camera);//PushGameObject(&camera);

	GameObject2D earth = GameObject2D("Earth", _Mesh["QuadMesh"].get());
	earth.GetTransform2D().SetLocalScale(Vector2(initScale / 2.f, initScale / 2.f));
	earth.GetTransform2D().SetLocalPosition(Vector2(100.f,  0.f));
	PushGameObject(&earth);

	GameObject2D moon = GameObject2D("Moon", _Mesh["QuadMesh"].get());
	moon.GetTransform2D().SetLocalScale(Vector2(initScale / 4.f, initScale / 4.f));
	moon.GetTransform2D().SetLocalPosition(Vector2(135.f, 0.f));
	PushGameObject(&moon);

	FindGameObjectWithName("Earth")->GetTransform2D().SetParent(&FindGameObjectWithName("Player")->GetTransform2D());
	FindGameObjectWithName("Moon")->GetTransform2D().SetParent(&FindGameObjectWithName("Earth")->GetTransform2D());

	//std::random_device rd;
	//std::mt19937 mt(rd());
	//std::uniform_real_distribution<float> dist(-10000.f, 10000.f);

	//for (int i = 0; i < 10000; ++i)
	//{
	//	GameObject2D go = GameObject2D("QuadObject", _Mesh["QuadMesh"].get());
	//	go.GetTransform2D().SetScale(Vector2(initScale, initScale));
	//	go.GetTransform2D().SetPosition(Vector2(dist(mt), dist(mt)));
	//	PushGameObject(&go);
	//	//_GameObject.push_back(std::move(ptr));
	//	//_GameObject.push_back(std::make_unique<GameObject2D>("QuadObject", _Mesh["QuadMesh"].get()));
	//	//_GameObject[_GameObject.size() - 1]->GetTransform2D().SetScale(Vector2(initScale, initScale));
	//	//_GameObject[_GameObject.size() - 1]->GetTransform2D().SetPosition(Vector2(dist(mt), dist(mt)));
	//}

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

	_Mesh["QuadMesh"].get()->CreateBound();

	return true;
}

auto lower_bound(std::vector<std::unique_ptr<GameObject2D>>::iterator first, std::vector<std::unique_ptr<GameObject2D>>::iterator last, GameObject2D* value)
{
	std::vector<std::unique_ptr<GameObject2D>>::iterator it;
	typename std::iterator_traits<std::vector<std::unique_ptr<GameObject2D>>::iterator>::difference_type count, step;
	count = std::distance(first, last);

	while (count > 0) {
		it = first;
		step = count / 2;
		std::advance(it, step);
		if ((*(*it).get()) < (*value)) {
			first = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}
	return first;
}

void GameEngine::PushGameObject(GameObject2D* go)
{
	auto lb = lower_bound(_GameObject.begin(), _GameObject.end(), go);

	auto ptr = std::make_unique<GameObject2D>((*go));
	int idx = 0;
	if (lb == _GameObject.end())
	{
		idx = _GameObject.size();
		_GameObject.push_back(std::move(ptr));
	}
	else
	{
		if ((*lb).get()->GetHash() == (*go).GetHash())
			return;

		idx = (_GameObject.insert(lb, std::move(ptr))) - _GameObject.begin();
	}

	if (_GameObjectWithName.find((*go).GetName()) == _GameObjectWithName.end())
	{
		_GameObjectWithName[(*go).GetName()] = std::vector<GameObject2D*>();
	}
	_GameObjectWithName[(*go).GetName()].push_back(_GameObject[idx].get());

#pragma region Quadtree
	if (!_Quadtree)
		_Quadtree = std::make_unique<Quadtree>();

	Rectangle rect = _GameObject[idx].get()->GetMesh()->GetRectBound();
	rect.Min *= _GameObject[idx].get()->GetTransform2D().GetWorldScale().Max();
	rect.Max *= _GameObject[idx].get()->GetTransform2D().GetWorldScale().Max();
	rect.Min += _GameObject[idx].get()->GetTransform2D().GetWorldPosition();
	rect.Max += _GameObject[idx].get()->GetTransform2D().GetWorldPosition();
	_Quadtree->Insert(_GameObject[idx].get(), rect);
#pragma endregion
}

GameObject2D* GameEngine::FindGameObjectWithName(std::string name)
{
	if (_GameObjectWithName.find(name) == _GameObjectWithName.end()
		|| _GameObjectWithName[name].size() <= 0)
		return nullptr;

	return _GameObjectWithName[name][0];

	//for (auto& go : _GameObject)
	//{
	//	if (go.get()->GetName().compare(name) == 0)
	//		return go.get();
	//}

	//return nullptr;
}