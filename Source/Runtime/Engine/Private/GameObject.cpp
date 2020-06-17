#include "Precompiled.h"
#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"

#include <chrono>

GameObject::GameObject()
	: _Name("GameObject"), _MeshPtr(nullptr)
{
	_UID = CreateHash();
}

GameObject::GameObject(std::string name, const Mesh* InMeshPtr)
	: _Name(name), _MeshPtr(InMeshPtr)
{
	_UID = CreateHash();
}

size_t GameObject::CreateHash()
{
	std::hash<std::string> hash_func;
	const auto t = std::chrono::high_resolution_clock::now();
	const auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
	return hash_func(_Name + std::to_string(nano));;
}
