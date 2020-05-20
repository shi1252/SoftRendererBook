#pragma once

class Mesh2D
{
public:
	Mesh2D() = default;

	std::vector<Vector2> _Vertices;
	std::vector<int> _Indices;
	std::vector<Vector2>& GetVertices() { return _Vertices; }
	std::vector<int>& GetIndices() { return _Indices; }

private:

};