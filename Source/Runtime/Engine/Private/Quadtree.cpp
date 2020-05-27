#include "Precompiled.h"
#include "Mesh2D.h"
#include "Transform2D.h"
#include "GameObject2D.h"
#include "Quadtree.h"

Quadtree::~Quadtree()
{
	for (int i = NW; i < SE; ++i)
	{
		if (childs[i] != nullptr)
		{
			delete childs[i];
			childs[i] = nullptr;
		}
	}
}

void Quadtree::Insert(GameObject2D* go, const Rectangle& rect)
{
	if (go->GetMesh() == nullptr || rect.GetSize() == Vector2::Zero)
		return;

	char section = IsOnBorder(rect);
	if (section == ON || size.GetSize() <= Vector2(50.f, 50.f) )
	{
 		list.push_back(go);
	}
	else
	{
		switch (section)
		{
		case NW:
			if (childs[NW] == nullptr)
			{
				Vector2 min = Vector2(size.Min.X, (size.Min.Y + size.Max.Y) / 2.f);
				Vector2 max = Vector2((size.Min.X + size.Max.X) / 2.f, size.Max.Y);
				childs[NW] = new Quadtree(min, max);
			}
			childs[NW]->Insert(go, rect);
			return;
		case NE:
			if (childs[NE] == nullptr)
			{
				Vector2 min = Vector2((size.Min.X + size.Max.X) / 2.f, (size.Min.Y + size.Max.Y) / 2.f);
				Vector2 max = Vector2(size.Max.X, size.Max.Y);
				childs[NE] = new Quadtree(min, max);
			}
			childs[NE]->Insert(go, rect);
			return;
		case SW:
			if (childs[SW] == nullptr)
			{
				Vector2 min = Vector2(size.Min.X, size.Min.Y);
				Vector2 max = Vector2((size.Min.X + size.Max.X) / 2.f, (size.Min.Y + size.Max.Y) / 2.f);
				childs[SW] = new Quadtree(min, max);
			}
			childs[SW]->Insert(go, rect);
			return;
		case SE:
			if (childs[SE] == nullptr)
			{
				Vector2 min = Vector2((size.Min.X + size.Max.X) / 2.f, size.Min.Y);
				Vector2 max = Vector2(size.Max.X, (size.Min.Y + size.Max.Y) / 2.f);
				childs[SE] = new Quadtree(min, max);
			}
			childs[SE]->Insert(go, rect);
			return;
		default:
			return;
		}
	}
}

bool Quadtree::GetIntersectingObjects(const Rectangle& rect, std::vector<GameObject2D*>& outObjs)
{
	bool result = false;

	if (rect.Intersect(this->size))
	{
		if (list.size() > 0)
		{
			for (int i = 0; i < list.size(); ++i)
			{
				outObjs.push_back(list[i]);
			}

			for (int i = NW; i < SE+1; ++i)
			{
				if (childs[i] != nullptr)
				{
					childs[i]->GetIntersectingObjects(rect, outObjs);
				}
			}

			result = true;
		}
		else
		{
			for (int i = NW; i < SE+1; ++i)
			{
				if (childs[i] != nullptr)
				{
					if (childs[i]->GetIntersectingObjects(rect, outObjs))
					{
						result = true;
					}
				}
			}
		}
	}
	return result;
}

void Quadtree::GetWholeObjects(std::vector<GameObject2D*>& outObjs)
{
	if (list.size() > 0)
	{
		for (int i = 0; i < list.size(); ++i)
		{
			outObjs.push_back(list[i]);
		}

		for (int i = NW; i < SE + 1; ++i)
		{
			if (childs[i] != nullptr)
			{
				childs[i]->GetWholeObjects(outObjs);
			}
		}
	}
	else
	{
		for (int i = NW; i < SE + 1; ++i)
		{
			if (childs[i] != nullptr)
			{
				childs[i]->GetWholeObjects(outObjs);
			}
		}
	}
}

char Quadtree::IsOnBorder(const Rectangle& rect)
{
	char result = ON;

	//Rectangle rect = go->GetMesh()->GetRectBound();
	Vector2 center = Vector2((size.Min.X + size.Max.X) / 2.f, (size.Min.Y + size.Max.Y) / 2.f);
	if (rect.Min.Y >= center.Y)
	{
		if (rect.Max.X <= center.X)
		{
			result = NW;
		}
		else if (rect.Min.X >= center.X)
		{
			result = NE;
		}
	}
	else if (rect.Max.Y <= center.Y)
	{
		if (rect.Max.X <= center.X)
		{
			result = SW;
		}
		else if (rect.Min.X >= center.X)
		{
			result = SE;
		}
	}
	return result;
}
