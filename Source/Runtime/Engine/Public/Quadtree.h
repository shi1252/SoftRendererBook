#pragma once

class Quadtree
{
public:
	Quadtree(const Vector2& inMinVector = Vector2::One * (-FLT_MAX), const Vector2& inMaxVector = Vector2::One * FLT_MAX)
		: size(Rectangle(inMinVector, inMaxVector)) {}
	Quadtree(const Rectangle& rect) : size(rect) {}
	~Quadtree();

	void Insert(GameObject2D* go, const Rectangle& rect);
	bool GetIntersectingObjects(const Rectangle& rect, std::vector<GameObject2D*>& outObjs);
	void GetWholeObjects(std::vector<GameObject2D*>& outObjs);
private:
	char IsOnBorder(const Rectangle& rect);

public:
	enum Section
	{
		NW = 0,
		NE,
		SW,
		SE,
		ON
	};

private:
	std::vector<GameObject2D*> list;
	Rectangle size;
	Quadtree* childs[4] = { nullptr, nullptr, nullptr, nullptr };
};