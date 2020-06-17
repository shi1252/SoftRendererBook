#pragma once

class GameObject
{
public:
	GameObject();
	GameObject(std::string name = "GameObject", const Mesh* InMeshPtr = nullptr);
	GameObject(const GameObject& rhs) = default;
	Transform& GetTransform() { return _Transform; }

	const Mesh* GetMesh() { return _MeshPtr; }
	bool HasMesh() const { return (_MeshPtr != nullptr); }
	
	std::string& GetName() { return _Name; }

	const size_t GetHash() { return _UID; }

protected:
	size_t _UID;
	Transform _Transform;
	const Mesh* _MeshPtr = nullptr;
	std::string _Name;

	size_t CreateHash();

#pragma region Operator Overloadding
public:
	friend bool operator<(size_t val, const GameObject& rhs) { return val < rhs._UID; }
	friend bool operator<(const GameObject& rhs, size_t val) { return rhs._UID < val; }
	bool operator<(const GameObject& rhs) const { return _UID < rhs._UID; }
	bool operator>(const GameObject& rhs) { return _UID > rhs._UID; }
	bool operator==(const GameObject& rhs) { return _UID == rhs._UID; }
	//size_t operator()() { return _UID; }
#pragma endregion
};