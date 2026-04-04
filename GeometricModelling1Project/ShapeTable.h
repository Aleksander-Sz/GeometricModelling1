#ifndef SHAPETABLE_H
#define SHAPETABLE_H

#include <vector>

class Shape;
class Point; // defined in Shapes.h
class Line;

class ShapeTable
{
public:
	//ShapeTable& getInstance();
	static int AddShape(Shape* shape);
	static int GetShapeID(Shape* shape);
	static Shape* GetShapeByID(int id);
	static Point* GetPointByID(int id);
	static Line* GetLineByID(int id);
	static void RemoveShape(int id);
	static void MergeShapes(int id1, int id2); // Shape id2 will be merged into id1
	Shape* operator[](int id) { return GetShapeByID(id); };
private:
	ShapeTable();
	static std::vector<Shape*> shapePointers;
};

#endif
