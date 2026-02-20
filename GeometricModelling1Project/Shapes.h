#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

class Shape
{
public:
	virtual void Draw(Shader& shader) = 0;
	void Scale(glm::vec3 s);
	void Rotate(float angle, glm::vec3 axis);
	void Translate(glm::vec3 t);
	void setModel(glm::mat4 m);
	void resetModel();
	void ConfirmTransformations();
	void CancelTransformations();
	virtual void PrintImGuiOptions() = 0;
	void PrintImGuiTransformOptions();
	virtual std::string Name() = 0;
	bool Select(bool deselect = false);
	glm::vec3 getPosition();
	bool isSelected();
protected:
	bool dirty = true;
	unsigned int VAO, VBO;
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 modelBackup = glm::mat4(1.0f);
	bool selected = false;
};
class Meshable : public Shape
{
public:
	void Draw(Shader& shader) override;
	virtual void Mesh() = 0;
protected:
	unsigned int EBO;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

class Point : public Shape
{
public:
	Point(glm::vec3 coords);
	void Draw(Shader& shader) override;
	void PrintImGuiOptions() override;
	std::string Name() override { return "Point"; }
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
	std::string Name() override { return "Torus"; }
private:
	float R, r;
	unsigned int s1, s2;
};

class Ellipsoid : public Meshable
{
public:
	Ellipsoid(float _a, float _b, float _c, unsigned int _s);
	void Mesh() override;
	// elipsoid specific functions
	void PrintImGuiOptions() override;
	std::string Name() override { return "Ellipsoid"; }
private:
	float a, b, c;
	unsigned int s;
};


class Grid
{
public:
	void Draw(Camera &camera, char eye = 0);
	static Grid getInstance();
private:
	Grid();
	unsigned int VAO, VBO, EBO;
	Shader gridShader = Shader("Shaders/GridVertexShader.glsl", "Shaders/GridFragmentShader.glsl");
};

class Cursor
{
public:
	static Cursor getInstance();
	void Draw(Shader& shader, char eye = 0);
	void UpdatePosition(Camera& camera, double xpos, double ypos);
	Shape* Click(std::vector<Shape*> shapes);
	glm::vec3 getPosition();
private:
	Cursor();
	glm::vec3 location = glm::vec3(0.0f,0.0f,0.0f);
	unsigned int VAO, VBO, EBO;
};
#endif