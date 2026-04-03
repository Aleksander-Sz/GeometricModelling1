#include "ShapeTable.h"

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

void ShapeTable::RemoveShape(int id)
{
	if (id < 0 || id >= shapePointers.size())
		return;
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