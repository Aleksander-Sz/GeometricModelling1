#include "Shader.h"

Shader::Shader()
{
	ID = 0;
}
Shader::Shader(const char* vertexPath, const char* tessellationControlPath, const char* tessellationEvalPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string tessellationControlCode;
	std::string tessellationEvalCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream tCShaderFile;
	std::ifstream tEShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tCShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tEShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	bool tessellation = (tessellationControlPath != nullptr) && (tessellationEvalPath != nullptr);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		if (tessellation)
		{
			tCShaderFile.open(tessellationControlPath);
			tEShaderFile.open(tessellationEvalPath);
		}
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, tCShaderStream, tEShaderStream, fShaderStream;
		// read file’s buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		if (tessellation)
		{
			tCShaderStream << tCShaderFile.rdbuf();
			tEShaderStream << tEShaderFile.rdbuf();
		}
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		if (tessellation)
		{
			tCShaderFile.close();
			tEShaderFile.close();
		}
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		if (tessellation)
		{
			tessellationControlCode = tCShaderStream.str();
			tessellationEvalCode = tEShaderStream.str();
		}
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		return;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* tCShaderCode = tessellationControlCode.c_str();
	const char* tEShaderCode = tessellationEvalCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, tessellationControl, tessellationEval, fragment;
	int success;
	char infoLog[512];
	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	};
	if (tessellation)
	{
		// tesselation control Shader
		tessellationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tessellationControl, 1, &tCShaderCode, NULL);
		glCompileShader(tessellationControl);
		// print compile errors if any
		glGetShaderiv(tessellationControl, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(tessellationControl, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::TESSELLATION_CONTROL::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
		};
		// tesselation evaluation Shader
		tessellationEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tessellationEval, 1, &tEShaderCode, NULL);
		glCompileShader(tessellationEval);
		// print compile errors if any
		glGetShaderiv(tessellationEval, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(tessellationEval, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::TESSELLATION_EVAL::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
		};
	}
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	};

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	if (tessellation)
	{
		glAttachShader(ID, tessellationControl);
		glAttachShader(ID, tessellationEval);
	}
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
			infoLog << std::endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertex);
	if (tessellation)
	{
		glDeleteShader(tessellationControl);
		glDeleteShader(tessellationEval);
	}
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, aa::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, aa::value_ptr(value));
}
void Shader::setVec2(const std::string& name, aa::vec2 value) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}
void Shader::setVec3(const std::string& name, aa::vec3 value) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}
void Shader::setVec4(const std::string& name, aa::vec4 value) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}