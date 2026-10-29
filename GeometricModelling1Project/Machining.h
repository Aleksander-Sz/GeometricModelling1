#pragma once
#include <vector>
#include "Shader.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>

enum CommandType
{
	UNDEF,
	G00,
	G01,
};

struct GCommand
{
public:
	GCommand(CommandType _command, float _x = 0.0f, float _y = 0.0f, float _z = 0.0f);
	GCommand(std::string line);
	CommandType command;
	float x, y, z;
	aa::vec3 xyz();
};

class MachinedMaterial
{
public:
	MachinedMaterial(float x, float z, unsigned int _resolutionX, unsigned int _resolutionZ);
	void Draw(Shader& shader);
	void ResetHeight(float height);
	void ApplyToolpathSegment(const aa::vec3& start, const aa::vec3& end, float toolDiameter);
private:
	unsigned int VAO, VBO, EBO;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	unsigned int heightTexture;
	unsigned int resolutionX, resolutionZ;
	float sizeX, sizeZ;
	std::vector<float> heights;
};

class PathSimulator
{
public:
	PathSimulator();
	void LoadGCode(const char* filename);
	void StartSimulation();
	void Draw(Shader& shader);
	MachinedMaterial Material = MachinedMaterial(5.0f, 4.0f, 500, 400);
	bool displayMaterial = false;
	float materialHeight = 1.0f;
	float toolDiameter = 0.05f;
private:
	std::vector<GCommand> commands;
	unsigned int VAO, VBO;
	bool simulationInProgress = false;
	unsigned int finishedSteps = 0;
	unsigned int millisecondsPerStep = 1000;
	std::chrono::steady_clock::time_point startTimestamp;
};

class Machining
{
public:
	PathSimulator Simulator;
};