#pragma once

class GameObject2D
{
public:
	GameObject2D();
	GameObject2D(std::string name = "GameObject", const Mesh2D* InMeshPtr = nullptr);
	GameObject2D(const GameObject2D& rhs) = default;
	Transform2D& GetTransform2D() { return _Transform; }

	const Mesh2D* GetMesh() { return _MeshPtr; }
	bool HasMesh() const { return (_MeshPtr != nullptr); }
	
	std::string& GetName() { return _Name; }

	const size_t GetHash() { return _UID; }

protected:
	size_t _UID;
	Transform2D _Transform;
	const Mesh2D* _MeshPtr = nullptr;
	std::string _Name;

	size_t CreateHash();

#pragma region Operator Overloadding
public:
	friend bool operator<(size_t val, const GameObject2D& rhs) { return val < rhs._UID; }
	friend bool operator<(const GameObject2D& rhs, size_t val) { return rhs._UID < val; }
	bool operator<(const GameObject2D& rhs) const { return _UID < rhs._UID; }
	bool operator>(const GameObject2D& rhs) { return _UID > rhs._UID; }
	bool operator==(const GameObject2D& rhs) { return _UID == rhs._UID; }
	//size_t operator()() { return _UID; }
#pragma endregion
};