#ifndef PARSER_H
#define PARSER_H

#include "Shapes.h"
#include <fstream>
#include <json.hpp>

class Parser
{
public:
	static void SaveScene(const char* filename, std::vector<int> shapes);
	static std::vector<int> LoadScene(const char* filename);
};

#endif