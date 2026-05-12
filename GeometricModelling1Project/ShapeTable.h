#ifndef SHAPETABLE_H
#define SHAPETABLE_H

#include <vector>

class Shape;
class Point; // defined in Shapes.h
class Line;
class IDependentOnOtherShapes;

class ShapeTable
{
public:
	//ShapeTable& getInstance();
	static int AddShape(Shape* shape);
	static int GetShapeID(Shape* shape);
	static Shape* GetShapeByID(int id);
	static Point* GetPointByID(int id);
	static Line* GetLineByID(int id);
	static IDependentOnOtherShapes* GetShapeWithVirtualPointsByID(int id);
	static void RemoveShape(int id);
	static void MergeShapes(std::vector<int> _shapes); // All of the shapes will be merged to the first one
	// any attribute edition or object transformations should be done beforehand
	Shape* operator[](int id) { return GetShapeByID(id); };
private:
	ShapeTable();
	static std::vector<Shape*> shapePointers;
};

#endif
