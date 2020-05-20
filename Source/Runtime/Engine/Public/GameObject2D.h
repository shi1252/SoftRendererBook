#pragma once

class GameObject2D
{
public:
	GameObject2D(std::string name = "GameObject", const Mesh2D* InMeshPtr = nullptr) 
		: _Name(name), _MeshPtr(InMeshPtr) {}
	Transform2D& GetTransform2D() { return _Transform; }

	const Mesh2D* GetMesh() { return _MeshPtr; }
	bool HasMesh() const { return (_MeshPtr != nullptr); }
	
	std::string& GetName() { return _Name; }

protected:
	Transform2D _Transform;
	const Mesh2D* _MeshPtr = nullptr;
	std::string _Name;
};