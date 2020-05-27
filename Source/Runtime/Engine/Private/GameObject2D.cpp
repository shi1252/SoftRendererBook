#include "Precompiled.h"
#include "Mesh2D.h"
#include "Transform2D.h"
#include "GameObject2D.h"

#include <chrono>

GameObject2D::GameObject2D()
	: _Name("GameObject"), _MeshPtr(nullptr)
{
	_UID = CreateHash();
}

GameObject2D::GameObject2D(std::string name, const Mesh2D* InMeshPtr)
	: _Name(name), _MeshPtr(InMeshPtr)
{
	_UID = CreateHash();
}

size_t GameObject2D::CreateHash()
{
	std::hash<std::string> hash_func;
	const auto t = std::chrono::high_resolution_clock::now();
	const auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
	return hash_func(_Name + std::to_string(nano));;
}
