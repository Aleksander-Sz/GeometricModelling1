#include "ShapeTable.h"
#include "Shapes.h"

std::vector<Shape*> ShapeTable::shapePointers;

/*ShapeTable& ShapeTable::getInstance()
{
	static ShapeTable instance;
	return instance;
}*/

ShapeTable::ShapeTable()
{
	;
}

int ShapeTable::GetShapeID(Shape* shape)
{
	for (size_t i = 0; i < shapePointers.size(); i++)
	{
		if (shapePointers[i] == shape)
		{
			return i;
		}
	}
	shapePointers.push_back(shape);
	return shapePointers.size() - 1;
}

Shape* ShapeTable::GetShapeByID(int id)
{
	if (id < 0 || id >= shapePointers.size())
		return nullptr;
	return shapePointers[id];
}

Point* ShapeTable::GetPointByID(int id)
{
	if (id < 0 || id >= shapePointers.size())
		return nullptr;
	Point* pointPointer = dynamic_cast<Point*>(shapePointers[id]);
	return pointPointer;
}

Line* ShapeTable::GetLineByID(int id)
{
	if (id < 0 || id >= shapePointers.size())
		return nullptr;
	Line* linePointer = dynamic_cast<Line*>(shapePointers[id]);
	return linePointer;
}

void ShapeTable::RemoveShape(int id)
{
	if (id < 0 || id >= shapePointers.size())
		return;
	delete shapePointers[id];
	shapePointers[id] = nullptr;
}

void ShapeTable::MergeShapes(int id1, int id2)
{
	if (id1 < 0 || id1 >= shapePointers.size() || id2 < 0 || id2 >= shapePointers.size())
		return;
	if (shapePointers[id1] == nullptr || shapePointers[id2] == nullptr)
		return;
	delete shapePointers[id2];
	shapePointers[id2] = shapePointers[id1];
}