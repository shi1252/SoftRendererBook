#include "Precompiled.h"
#include "Mesh2D.h"

void Mesh2D::CreateBound()
{
	_CircleBound = Circle(_Vertices.data(), _Vertices.size());
	_RectBound = Rectangle(_Vertices.data(), _Vertices.size());
}
