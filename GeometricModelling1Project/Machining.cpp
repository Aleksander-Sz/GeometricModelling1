#include "Machining.h"

PathSimulator::PathSimulator()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	float center[] = { 0.0f,0.0f,0.0f };
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), center, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}

void PathSimulator::LoadGCode(const char* filename)
{
	commands.clear();
	std::ifstream file(filename);
	if (!file.is_open())
		throw(std::runtime_error("Could not open file"));
	std::string line;
	while (std::getline(file, line))
	{
		commands.push_back(GCommand(line));
	}
	file.close();
}

void PathSimulator::StartSimulation()
{
	if (commands.size() < 1)
		return;
	Material.ResetHeight(materialHeight);
	simulationInProgress = true;
	finishedSteps = 0;
	startTimestamp = std::chrono::high_resolution_clock::now();
}

void PathSimulator::Draw(Shader& shader)
{
	if(displayMaterial)
		Material.Draw(shader);
	if (simulationInProgress)
	{
		// get time difference
		auto newTimestamp = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(newTimestamp - startTimestamp);
		long long elapsedMiliseconds = duration.count();
		size_t simulationStep = elapsedMiliseconds / millisecondsPerStep;
		float stepProgress = (float)(elapsedMiliseconds % millisecondsPerStep) / millisecondsPerStep;
		if (simulationStep >= commands.size()-1)
		{
			simulationInProgress = false;
		}
		else
		{
			aa::vec3 headLocationA = commands[simulationStep].xyz();
			aa::vec3 headLocationB = commands[simulationStep+1].xyz();

			aa::vec3 headLocationInterpolated = headLocationA * (1.0f - stepProgress) + headLocationB * stepProgress;

			shader.use();
			glBindVertexArray(VAO);
			aa::mat4 model = aa::translate(headLocationInterpolated);
			shader.setMat4("model", model);
			shader.setVec3("color", aa::vec3(0.0f, 1.0f, 0.0f));
			glPointSize(10.0f);
			glDrawArrays(GL_POINTS, 0, 1);

			// Remove the material if the next segment is finished

			if (simulationStep > finishedSteps)
			{
				aa::vec3 start = commands[simulationStep - 1].xyz();
				aa::vec3 end = commands[simulationStep].xyz();
				Material.ApplyToolpathSegment(start, end, toolDiameter);
				finishedSteps++;
			}
		}
	}
	
	glBindVertexArray(0);
	
}

GCommand::GCommand(CommandType _command, float _x, float _y, float _z)
{
	command = _command;
	x = _x;
	y = _y;
	z = _z;
}

GCommand::GCommand(std::string line)
{
	static float prevX, prevY, prevZ;
	command = CommandType::UNDEF;
	x = prevX;
	y = prevY;
	z = prevZ;
	size_t i = 0;
	size_t len = line.length();
	while (i < len && std::isspace(line[i])) {
		i++;
	}

	if (i < len && line[i] == 'N') {
		i++;

		std::string num_str = "";
		while (i < line.length() && std::isdigit(line[i])) {
			num_str += line[i];
			i++;
		}
		if (!num_str.empty()) {
			//result.line_number = std::stoi(num_str);
		}
	}
	if (i < len && std::isalpha(line[i])) {
		char command_letter = line[i];
		i++;

		int command_num = -1;
		std::string cmd_num_str = "";
		while (i < len && std::isdigit(line[i])) {
			cmd_num_str += line[i];
			i++;
		}
		if (!cmd_num_str.empty()) {
			command_num = std::stoi(cmd_num_str);
		}

		if (command_letter == 'G' && command_num == 0)
		{
			command = CommandType::G00;
		}
		else if (command_letter == 'G' && command_num == 1)
		{
			command = CommandType::G01;
		}
	}
	while (i < len) {
		if (std::isspace(line[i])) {
			i++;
			continue;
		}
		if (i < len - 1)
		{
			if (line[i] == '/' && line[i] == '/')
				break;
		}

		if (std::isalpha(line[i])) {
			char param_letter = line[i++];
			std::string val_str = "";

			while (i < len && (std::isdigit(line[i]) || line[i] == '.' || line[i] == '-')) {
				val_str += line[i++];
			}

			if (!val_str.empty()) {
				float value = std::stof(val_str);
				if (param_letter == 'X')
					prevX = x = value;
				else if (param_letter == 'Y')
					prevY = y = value;
				else if (param_letter == 'Z')
					prevZ = z = value;
			}
		}
		else {
			i++;
		}
	}
}

aa::vec3 GCommand::xyz()
{
	return aa::vec3(x, y, z);
}

MachinedMaterial::MachinedMaterial(float x, float z, unsigned int _resolutionX, unsigned int _resolutionZ)
{
	resolutionX = _resolutionX;
	resolutionZ = _resolutionZ;
	sizeX = x;
	sizeZ = z;
	heights.assign(static_cast<size_t>(resolutionX + 1) * (resolutionZ + 1), 0.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	vertices.clear();
	indices.clear();
	for (int i = 0; i <= resolutionX; i++)
	{
		for (int j = 0; j <= resolutionZ; j++)
		{
			// vertices
			float xval = x * static_cast<float>(i) / resolutionX;
			float yval = 0.0f;
			float zval = z * static_cast<float>(j) / resolutionZ;
			vertices.push_back(xval);
			vertices.push_back(yval);
			vertices.push_back(zval);
			// uv coords
			vertices.push_back(static_cast<float>(i) / resolutionX);
			vertices.push_back(static_cast<float>(j) / resolutionZ);
			// indices
			if (i == resolutionX || j == resolutionZ)
				continue; // skip the last row and column to avoid wrapping issues
			unsigned int v0 = i * (resolutionZ + 1) + j;
			unsigned int v1 = i * (resolutionZ + 1) + (j + 1);
			unsigned int v2 = (i + 1) * (resolutionZ + 1) + j;
			unsigned int v3 = (i + 1) * (resolutionZ + 1) + (j + 1);
			// Triangle 1
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);

			// Triangle 2
			indices.push_back(v2);
			indices.push_back(v1);
			indices.push_back(v3);
		}
	}
	//prepare for drawing
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Define the stride: Total size of ONE vertex (3 floats for Pos + 2 floats for UV)
	GLsizei stride = sizeof(float) * 5;

	// Attribute 0: Position (3D Coordinates)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	// Attribute 1: UV Coordinates (2D Texture Coordinates)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3)); // Starts after 3 floats

	glBindVertexArray(0);

	//Texture:

	const int width = static_cast<int>(resolutionX) + 1;
	const int height = static_cast<int>(resolutionZ) + 1;

	glGenTextures(1, &heightTexture);
	glBindTexture(GL_TEXTURE_2D, heightTexture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_R32F,
		width,
		height,
		0,
		GL_RED,
		GL_FLOAT,
		heights.data()
	);

	// One texel per vertex: no interpolation between heights.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Keep sampling within the texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void MachinedMaterial::Draw(Shader& shader)
{
	shader.use();
	shader.setVec3("color", aa::vec3(0.8f, 0.8f, 0.8f));
	shader.setMat4("model", aa::mat4::identity());
	shader.setInt("useHeight", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightTexture);
	shader.setInt("heightMap", 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	shader.setInt("useHeight", 0);
}

void MachinedMaterial::ResetHeight(float height)
{
	const unsigned int width = resolutionX + 1;
	const unsigned int heightCount = resolutionZ + 1;

	heights.assign(static_cast<size_t>(resolutionX + 1) * (resolutionZ + 1), height);

	glBindTexture(GL_TEXTURE_2D, heightTexture);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0, 0,
		width,
		heightCount,
		GL_RED,
		GL_FLOAT,
		heights.data()
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void MachinedMaterial::ApplyToolpathSegment(
	const aa::vec3& start,
	const aa::vec3& end,
	float toolDiameter)
{
	if (toolDiameter <= 0.0f)
		return;

	const float radius = toolDiameter * 0.5f;
	const float radiusSquared = radius * radius;

	const unsigned int width = resolutionX + 1;
	const unsigned int heightCount = resolutionZ + 1;

	// Tool movement in the horizontal XZ plane.
	const float dx = end.x - start.x;
	const float dz = end.z - start.z;
	const float lengthSquared = dx * dx + dz * dz;

	// Ignore purely vertical movements.
	if (lengthSquared < 1e-12f)
		return;

	// Limit iteration to the segment's bounding box,
	// expanded by the tool radius.
	const float minX = std::min(start.x, end.x) - radius;
	const float maxX = std::max(start.x, end.x) + radius;
	const float minZ = std::min(start.z, end.z) - radius;
	const float maxZ = std::max(start.z, end.z) + radius;

	const int firstI = std::max(0, static_cast<int>(std::ceil(minX / sizeX * resolutionX)));

	const int lastI = std::min(
		static_cast<int>(resolutionX),
		static_cast<int>(std::floor(maxX / sizeX * resolutionX)));

	const int firstJ = std::max(0, static_cast<int>(std::ceil(minZ / sizeZ * resolutionZ)));

	const int lastJ = std::min(static_cast<int>(resolutionZ), static_cast<int>(std::floor(maxZ / sizeZ * resolutionZ)));

	for (int i = firstI; i <= lastI; ++i)
	{
		const float x = sizeX * static_cast<float>(i) / resolutionX;

		for (int j = firstJ; j <= lastJ; ++j)
		{
			const float z = sizeZ * static_cast<float>(j) / resolutionZ;

			// Vector from segment start to this grid vertex.
			const float px = x - start.x;
			const float pz = z - start.z;

			// Closest-point parameter along the segment.
			float t = (px * dx + pz * dz) / lengthSquared;
			t = std::clamp(t, 0.0f, 1.0f);

			// Closest point on the toolpath in XZ.
			const float closestX = start.x + t * dx;
			const float closestZ = start.z + t * dz;

			const float ex = x - closestX;
			const float ez = z - closestZ;

			if (ex * ex + ez * ez > radiusSquared)
				continue;

			// Interpolate the tool height along the segment.
			const float toolHeight =
				start.y + t * (end.y - start.y);

			// Texture data is stored row by row: x varies fastest.
			const size_t index =
				static_cast<size_t>(j) * width + i;

			heights[index] = std::min(heights[index], toolHeight);
		}
	}

	// Upload the resulting height map.
	glBindTexture(GL_TEXTURE_2D, heightTexture);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0, 0,
		width,
		heightCount,
		GL_RED,
		GL_FLOAT,
		heights.data()
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}