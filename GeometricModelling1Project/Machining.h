#pragma once
#include <vector>
#include "Shader.h"

enum CommandType
{
	G00,
	G01,
};

class GCommand
{
public:
	GCommand(CommandType _command, float _x = 0.0f, float _y = 0.0f, float _z = 0.0f);
	CommandType command;
	float x, y, z;
};

class PathSimulator
{
public:
	PathSimulator();
	void LoadGCode(const char* filename);
	//void StartSimulation();
	void Draw(Shader& shader);
private:
	std::vector<GCommand> commands;
	unsigned int VAO, VBO;
};

class Machining
{
public:
	PathSimulator Simulator;
};