#ifndef SHAPES_H
#define SHAPES_H

#include <vector>

class Torus
{
public:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	Torus(float R, float r, unsigned int s1, unsigned int s2);
};

#endif