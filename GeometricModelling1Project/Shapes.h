#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "ShapeTable.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

class Shape
{
public:
	virtual ~Shape();
	virtual void Draw() = 0;
	virtual void Scale(aa::vec3 s, aa::vec3 origin = aa::vec3(0.0f, 0.0f, 0.0f));
	virtual void Rotate(float angle, aa::Axis axis, aa::vec3 pivot = aa::vec3(0.0f, 0.0f, 0.0f));
	virtual void Translate(aa::vec3 t);
	virtual void TranslateAndConfirm(aa::vec3 t);
	void setModel(aa::mat4 m);
	void resetModel();
	virtual void ConfirmTransformations();
	virtual void CancelTransformations();
	virtual void PrintImGuiOptions() = 0;
	void PrintImGuiTransformOptions();
	std::string Name() { return shapeName; }
	void setName(std::string _name) { shapeName = _name; }
	bool Select(bool deselect = false);
	virtual aa::vec3 getPosition();
	aa::vec2 getScreenSpacePosition(Camera& camera);
	bool isSelected();
	void MarkForDeletion();
	bool isMarkedForDeletion();
	void setShader(Shader& _shader);
	bool dirty = true;
protected:
	unsigned int VAO = 0, VBO = 0;
	aa::mat4 model = aa::mat4(1.0f);
	aa::mat4 modelBackup = aa::mat4(1.0f);
	bool selected = false;
	std::string shapeName;
	bool markedForDeletion = false;
	Shader shader;
};

class Meshable : public Shape
{
public:
	~Meshable() override;
	void Draw() override;
	virtual void Mesh() = 0;
protected:
	unsigned int EBO = 0;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

class Point : public Shape
{
public:
	Point(aa::vec3 coords);
	void Draw() override;
	void PrintImGuiOptions() override;
	void Scale(aa::vec3 s, aa::vec3 origin = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Rotate(float angle, aa::Axis axis, aa::vec3 pivot = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Translate(aa::vec3 t) override;
	void TranslateAndConfirm(aa::vec3 t) override;
	std::vector<Shape*> dependentShapes;
private:
	void InvalidateDependentShapes();
};

class Torus : public Meshable
{
public:
	Torus(float R, float r, unsigned int s1, unsigned int s2);
	void Mesh() override;
	// torus specific functions
	void setSubdivision(unsigned int _s1, unsigned int _s2, bool force = false);
	void setR(float _R);
	void setr(float _r);
	void PrintImGuiOptions() override;
private:
	float R, r;
	int s1, s2;
};

class Ellipsoid : public Meshable
{
public:
	Ellipsoid(float _a, float _b, float _c, unsigned int _s);
	void Mesh() override;
	// elipsoid specific functions
	void PrintImGuiOptions() override;
private:
	float a, b, c;
	unsigned int s;
};

class Line : public Meshable
{
public:
	Line(std::vector<Point*> _points);
	~Line() override;
	virtual void Mesh();
	void PrintImGuiOptions() override;
	void Scale(aa::vec3 s, aa::vec3 origin = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Rotate(float angle, aa::Axis axis, aa::vec3 pivot = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Translate(aa::vec3 t) override;
	void ConfirmTransformations() override;
	void CancelTransformations() override;
	void AddPoint(Point* point);
	aa::vec3 getPosition() override;
	void RemoveDeletedPoints(); // removes all the points marked for deletion from the points vector
protected:
	std::vector<Point*> points;
};

class BezierCurveC0 : public Line
{
public:
	BezierCurveC0(std::vector<Point*> _controlPoints) : Line(_controlPoints) { shapeName = "Bezier Curve"; };
	void Mesh() override;
	void PrintImGuiOptions() override;
	bool displayControlPolyline = false;
	void setTessellationShader(Shader& _shader);
	Shader tessellationShader;
};

class BezierCurveC1 : public BezierCurveC0
{
public:
	BezierCurveC1(std::vector<Point*> _controlPoints) : BezierCurveC0(_controlPoints) { };
	void Mesh() override;
};

class Grid
{
public:
	void Draw(Camera &camera, char eye = 0);
	static Grid& getInstance();
private:
	Grid();
	unsigned int VAO, VBO, EBO;
	Shader gridShader = Shader("Shaders/GridVertexShader.glsl", "Shaders/GridFragmentShader.glsl");
};

class Cursor : public Shape
{
public:
	static Cursor& getInstance();
	static Cursor& centerOfGravityIndicator();
	void Draw() override;
	void Draw(aa::vec3 position);
	void UpdatePosition(Camera& camera, double xpos, double ypos, bool xLocked, bool yLocked, bool zLocked);
	void PrintImGuiOptions();
	void Translate(aa::vec3 t) override;
	void ConfirmTransformations() override;
	void CancelTransformations() override;
	aa::vec3 getPosition();
private:
	Cursor(bool _isCenterOfGravity = false);
	aa::vec3 location = aa::vec3(0.0f,0.0f,0.0f);
	aa::vec3 locationBackup = aa::vec3(0.0f, 0.0f, 0.0f);
	unsigned int VAO, VBO, EBO;
	bool isCenterOfGravity = false;
};

class Axis
{
public:
	Axis();
	Axis(char _axis);
	void SetAxis(aa::mat4 _model, aa::vec3 _color);
	void Draw(Shader& shader, aa::vec3 translationOrigin, char eye = 0);
private:
	unsigned int VAO = 0, VBO = 0;
	aa::mat4 model = aa::mat4(1.0f);
	aa::vec3 color = aa::vec3(0.0f);
};

class BoxSelect
{
public:
	static BoxSelect& getInstance();
	void Draw(aa::vec2 tl, aa::vec2 br);
private:
	BoxSelect();
	unsigned int VAO, VBO, EBO;
	Shader boxSelectShader = Shader("Shaders/BoxSelectVertexShader.glsl", "Shaders/BoxSelectFragmentShader.glsl");
};
#endif