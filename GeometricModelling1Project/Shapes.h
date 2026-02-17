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
	void Draw(Shader& Shader);
	virtual void Mesh() = 0;
	void Scale(glm::vec3 s);
	void Rotate(float angle, glm::vec3 axis);
	void Translate(glm::vec3 t);
	void setModel(glm::mat4 m);
	void resetModel();
	virtual void PrintImGuiOptions() = 0;
	void PrintImGuiTransformOptions();
	virtual std::string Name() = 0;
protected:
	bool dirty = true;
	unsigned int VAO, VBO, EBO;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	glm::mat4 model = glm::mat4(1.0f);
};

class Torus : public Shape
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


class Grid
{
public:
	void Draw(Camera &camera);
	static Grid getInstance();
private:
	Grid();
	unsigned int VAO, VBO, EBO;
	Shader gridShader = Shader("Shaders/GridVertexShader.glsl", "Shaders/GridFragmentShader.glsl");
};
#endif