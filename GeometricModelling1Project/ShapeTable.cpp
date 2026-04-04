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

int ShapeTable::AddShape(Shape* shape)
{
	shapePointers.push_back(shape);
	return shapePointers.size() - 1;
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

void ShapeTable::MergeShapes(std::vector<int> _shapes)
{
	for (size_t i = 0; i < _shapes.size(); i++)
	{
		if (_shapes[i] < 0 || _shapes[i] >= shapePointers.size())
			return;
		if (shapePointers[_shapes[i]] == nullptr)
			return;
	}
	std::vector<int> dependentShapesToBeReassigned;
	for (size_t i = 1; i < _shapes.size(); i++)
	{
		Point* currentPoint = GetPointByID(_shapes[i]);
		if (currentPoint != nullptr)
		{
			for (size_t j = 0; j < currentPoint->dependentShapes.size(); j++)
			{
				//GetShapeByID(currentPoint->dependentShapes[j])->dirty = true;
				//dependentShapesToBeReassigned.push_back(currentPoint->dependentShapes[j]);
				GetPointByID(_shapes[0])->dependentShapes.push_back(currentPoint->dependentShapes[j]);
			}

		}
		delete shapePointers[_shapes[i]];
		shapePointers[_shapes[i]] = shapePointers[_shapes[0]];
	}
	Point* firstPoint = GetPointByID(_shapes[0]);
	if (firstPoint != nullptr)
	{
		for (size_t i = 0; i < firstPoint->dependentShapes.size(); i++)
		{
			GetShapeByID(firstPoint->dependentShapes[i])->dirty = true;
		}
	}
}