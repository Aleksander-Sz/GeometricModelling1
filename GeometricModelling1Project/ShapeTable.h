#ifndef SHAPETABLE_H
#define SHAPETABLE_H

#include "Shapes.h"
#include <vector>


class ShapeTable
{
public:
	ShapeTable();
	int GetShapeID(Shape* shape);
	Shape* GetShapeByID(int id);
	void RemoveShape(int id);
	void MergeShapes(int id1, int id2); // Shape id2 will be merged into id1
private:
	std::vector<Shape*> shapePointers;
};

#endif
