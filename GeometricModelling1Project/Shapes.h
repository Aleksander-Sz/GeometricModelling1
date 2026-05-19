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

// system constants
#define NONE_SELECTED 0
#define SHAPE_SELECTED 10
#define VIRTUAL_POINT_SELECTED 11

enum LockAxis {
	NONE, X, Y, Z, notX, notY, notZ
};

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
	void CancelTransformations() override;
	std::vector<int> dependentShapes;
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

class IDependentOnOtherShapes
{
public:
	virtual void RemoveDeletedPoints() = 0;
};

class Line : public Meshable, public IDependentOnOtherShapes
{
public:
	Line(std::vector<int> _points);
	~Line() override;
	virtual void Mesh();
	void PrintImGuiOptions() override;
	void Scale(aa::vec3 s, aa::vec3 origin = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Rotate(float angle, aa::Axis axis, aa::vec3 pivot = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Translate(aa::vec3 t) override;
	void ConfirmTransformations() override;
	void CancelTransformations() override;
	void AddPoint(int point);
	aa::vec3 getPosition() override;
	void RemoveDeletedPoints() override; // removes all the points marked for deletion from the points vector
protected:
	std::vector<int> points;
};

class BezierCurveC0 : public Line
{
public:
	BezierCurveC0(std::vector<int> _controlPoints) : Line(_controlPoints) { shapeName = "Bezier Curve C0"; Mesh(); };
	void Mesh() override;
	void PrintImGuiOptions() override;
	bool displayControlPolyline = false;
	void setTessellationShader(Shader& _shader);
	Shader tessellationShader;
};

class BezierCurveC1 : public BezierCurveC0
{
public:
	BezierCurveC1(std::vector<int> _controlPoints) : BezierCurveC0(_controlPoints) { shapeName = "Bezier Curve C1"; Mesh(); };
	void Mesh() override;
};

class IContainsVirtualPoints
{
public:
	virtual float LeftClick(Camera& camera, aa::vec2 clickPos) = 0; // returns the distance to the closest point, stashes it for selection
	virtual void ConfirmSelection(bool shiftPressed, bool justDeselectEverything = false) = 0; // Used, if no closer point was found
	int containsSelectedVirtualPoints = 0;
	virtual void VirtualPointsTranslate(aa::vec3 t) = 0;
	virtual void VirtualPointsConfirmTransformations() = 0;
	virtual void VirtualPointsCancelTransformations() = 0;
	virtual aa::vec3 GetVirtualPointsPosition() = 0;
protected:
	int preparedVirtualPoint = -1; // Used to store the index of the virtual point that is currently prepared for selection, -1 if no virtual point is prepared
};

class BezierCurveC2 : public BezierCurveC0, public IContainsVirtualPoints
{
public:
	BezierCurveC2(std::vector<int> _controlPoints) : BezierCurveC0(_controlPoints) { shapeName = "Bezier Curve C2"; Mesh(); };
	void Mesh() override;
	std::vector<aa::vec3> bernsteinPoints;
	int selectedVirtualPoint = -1;
	void PrintImGuiOptions() override;
	bool displayBernsteinPoints = false;
	// IContainsVirtualPoints interface functions
	float LeftClick(Camera& camera, aa::vec2 clickPos) override;
	void ConfirmSelection(bool shiftPressed, bool justDeselectEverything) override;
	void VirtualPointsTranslate(aa::vec3 t) override;
	void VirtualPointsConfirmTransformations() override;
	void VirtualPointsCancelTransformations() override;
	aa::vec3 GetVirtualPointsPosition() override;
	bool currentlyTranslatingVirtualPoints = false;
	aa::vec3 virtualPointPositionBackup = NULL;
};

class InterpolatingCurve : public BezierCurveC2
{
	public:
	InterpolatingCurve(std::vector<int> _points) : BezierCurveC2(_points) { shapeName = "Interpolating Curve"; Mesh(); };
	void Mesh() override;
};

//Surfaces

class BezierSurface : public Meshable, public IDependentOnOtherShapes
{
public:
	BezierSurface(aa::vec3 position, int a, int b, float dimensionX, float dimensionZ, bool _isC2);
	~BezierSurface() override;
	void Mesh() override;
	std::vector<std::vector<int>> controlPoints;
	void PrintImGuiOptions() override;
	void RemoveDeletedPoints() override;
	bool isC2;
	void setTessellationShader(Shader& _shader);
	Shader tessellationShader;
	int subdivisions = 16;
	void Scale(aa::vec3 s, aa::vec3 origin = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Rotate(float angle, aa::Axis axis, aa::vec3 pivot = aa::vec3(0.0f, 0.0f, 0.0f)) override;
	void Translate(aa::vec3 t) override;
	void ConfirmTransformations() override;
	void CancelTransformations() override;
private:
	void MeshC0();
	void MeshC2();
};

// Auxiliary shapes

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
	void UpdatePosition(Camera& camera, double xpos, double ypos, LockAxis lockedAxis);
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
	Axis(LockAxis _axis);
	void SetAxis(LockAxis _axis);
	void Draw(Shader& shader, aa::vec3 translationOrigin, char eye = 0);
private:
	LockAxis lockedAxis = NONE;
	unsigned int VAO = 0, VBO = 0;
	aa::mat4 modelX = aa::scale(aa::vec3(100.0f, 100.0f, 100.0f));
	aa::mat4 modelY = aa::scale(aa::rotate(aa::Axis::Z, aa::radians(90.0f)), aa::vec3(100.0f, 100.0f, 100.0f));
	aa::mat4 modelZ = aa::scale(aa::rotate(aa::Axis::Y, aa::radians(90.0f)), aa::vec3(100.0f, 100.0f, 100.0f));
	aa::vec3 colorX = aa::vec3(0.8f, 0.1f, 0.1f);
	aa::vec3 colorY = aa::vec3(0.1f, 0.8f, 0.1f);
	aa::vec3 colorZ = aa::vec3(0.1f, 0.1f, 0.8f);
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