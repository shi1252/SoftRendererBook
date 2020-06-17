#pragma once

class Mesh
{
public:
	Mesh() = default;

	std::vector<Vector3> _Vertices;
	std::vector<int> _Indices;
	std::vector<Vector3>& GetVertices() { return _Vertices; }
	std::vector<int>& GetIndices() { return _Indices; }

	void CreateBound();
	//Circle GetCircleBound() const { return _CircleBound; }
	//Rectangle GetRectBound() const { return _RectBound; }

private:
	//Circle _CircleBound;
	//Rectangle _RectBound;
};