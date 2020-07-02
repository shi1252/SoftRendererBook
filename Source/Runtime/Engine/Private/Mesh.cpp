#include "Precompiled.h"
#include "Mesh.h"

void Mesh::CreateBound()
{
	_SphereBound = Sphere(_Vertices.data(), _Vertices.size());
	//_CircleBound = Circle(_Vertices.data(), _Vertices.size());
	//_RectBound = Rectangle(_Vertices.data(), _Vertices.size());
}
