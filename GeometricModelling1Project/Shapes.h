#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "Shader.h"

class Shape
{
public:
	virtual void Draw(Shader& Shader) = 0;
	virtual void Mesh() = 0;
protected:
	bool dirty = true;
	unsigned int VAO, VBO, EBO;
};

class Torus : public Shape
{
public:
	Torus(float R, float r, unsigned int s1, unsigned int s2);
	void Draw(Shader& shader) override;
	void Mesh() override;
	void setSubdivision(unsigned int _s1, unsigned int _s2, bool force = false);
	void setR(float _R);
	void setr(float _r);
private:
	float R, r;
	unsigned int s1, s2;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

#endif