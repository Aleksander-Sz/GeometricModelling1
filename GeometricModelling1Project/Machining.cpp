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

}

void PathSimulator::Draw(Shader& shader)
{
	shader.use();
	glBindVertexArray(VAO);
	aa::mat4 model = aa::translate(aa::vec3(1.0f, 2.0f, 3.0f));
	shader.setMat4("model", model);
	shader.setVec3("color", aa::vec3(0.0f, 1.0f, 0.0f));
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

GCommand::GCommand(CommandType _command, float _x, float _y, float _z)
{
	command = _command;
	x = _x;
	y = _y;
	z = _z;
}
