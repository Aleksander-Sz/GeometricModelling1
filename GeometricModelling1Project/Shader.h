#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h> // include glad to get the required OpenGL headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "AlexAlgebra.h"


class Shader
{
public:
	// the program ID
	unsigned int ID;
	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, aa::mat4 value) const;
	void setVec3(const std::string& name, aa::vec3 value) const;
	void setVec4(const std::string& name, aa::vec4 value) const;
};
#endif