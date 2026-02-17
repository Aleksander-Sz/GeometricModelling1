#include "Shapes.h"

// Shape class functions
void Shape::Scale(glm::vec3 s)
{
	//model = glm::scale(model, s);
	glm::mat4 scaleMatrix = glm::mat4(glm::vec4(s.x, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, s.y, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, s.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	model = scaleMatrix * model;
}
void Shape::Rotate(float angle, glm::vec3 axis)
{
	//model = glm::rotate(model, glm::radians(angle), axis);
	axis = glm::normalize(axis);
	float c = cos(glm::radians(angle));
	float s = sin(glm::radians(angle));
	glm::mat4 rotationMatrix;
	if(axis.x == 1.0f)
		rotationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, c, s, 0.0f), glm::vec4(0.0f, -s, c, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	else if(axis.y==1.0f)
		rotationMatrix = glm::mat4(glm::vec4(c, 0.0f, -s, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(s, 0.0f, c, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	else if(axis.z==1.0f)
		rotationMatrix = glm::mat4(glm::vec4(c, s, 0.0f, 0.0f), glm::vec4(-s, c, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	else
	{
		std::cerr << "Rotation axis must be one of the cardinal axes (x,y,z).\n";
		return;
	}
	model = rotationMatrix * model;
}
void Shape::Translate(glm::vec3 t)
{
	//model = glm::translate(model, t);
	glm::mat4 translationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(t.x, t.y, t.z, 1.0f));
	model = translationMatrix * model;
}
void Shape::setModel(glm::mat4 m)
{
	model = m;
}
void Shape::resetModel()
{
	model = glm::mat4(1.0f);
}
void Shape::PrintImGuiTransformOptions()
{
	static glm::vec3 scale(1.0f);
	static float rotationX = 0.0f;
	static float rotationY = 0.0f;
	static float rotationZ = 0.0f;
	static glm::vec3 translation(0.0f);
	ImGui::InputFloat3("Scale", glm::value_ptr(scale));
	if (ImGui::Button("Apply Scale"))
	{
		Scale(scale);
	}
	ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
	if(ImGui::Button("Apply Rotation X"))
	{
		Rotate(rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
	if (ImGui::Button("Apply Rotation Y"))
	{
		Rotate(rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
	if (ImGui::Button("Apply Rotation Z"))
	{
		Rotate(rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	ImGui::InputFloat3("Translation", glm::value_ptr(translation));
	if (ImGui::Button("Apply Translation"))
	{
		Translate(translation);
	}
	if (ImGui::Button("Reset Transformations"))
	{
		resetModel();
		scale = glm::vec3(1.0f);
		rotationX = rotationY = rotationZ = 0.0f;
		translation = glm::vec3(0.0f);
	}
}
// Torus class functions
Torus::Torus(float _R, float _r, unsigned int _s1, unsigned int _s2)
{
	R = _R;
	r = _r;
	s1 = _s1;
	s2 = _s2;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	Mesh();
}
void Torus::Draw(Shader& shader)
{
	if (dirty)
		Mesh();
	//shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", model);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void Torus::Mesh()
{
	if (s1 < 3)
	{
		std::cerr << "Subdivision must be at least 1.\n";
		s1 = 3;
	}
	if (s2 < 3)
	{
		std::cerr << "Subdivision must be at least 1.\n";
		s2 = 3;
	}
	vertices.clear();
	indices.clear();
	dirty = false;
	for (int i = 0; i < s1; i++)
	{
		float phi = (float)i / s1 * 2.0f * 3.14159265359f;
		for (int j = 0; j < s2; j++)
		{
			// vertices
			float theta = (float)j / s2 * 2.0f * 3.14159265359f;
			float x = (R + r * cos(theta)) * cos(phi);
			float y = (R + r * cos(theta)) * sin(phi);
			float z = r * sin(theta);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			// indices
			unsigned int nextI = (i + 1) % s1;
			unsigned int nextJ = (j + 1) % s2;
			/*unsigned int v0 = i * s2 + j;
			unsigned int v1 = i * s2 + nextJ;
			unsigned int v2 = nextI * s2 + j;
			unsigned int v3 = nextI * s2 + nextJ;
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);*/
			unsigned int v0 = i * s2 + j;
			unsigned int v1 = i * s2 + nextJ;
			unsigned int v2 = nextI * s2 + j;
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v0);
			indices.push_back(v2);
		}
	}
	//prepare for drawing
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
void Torus::setSubdivision(unsigned int _s1, unsigned int _s2, bool force)
{
	if ((_s1 > 500 || _s2 > 500) && !force)
	{
		std::cerr << "Subdivision very high, clamping to 500, if you want to use higher subdivisions, set the force flag to true.\n";
		s1 = 500;
		s2 = 500;
	}
	else
	{
		s1 = _s1;
		s2 = _s2;
	}
	dirty = true;
}
void Torus::setR(float _R)
{
	R = _R;
	dirty = true;
}
void Torus::setr(float _r)
{
	r = _r;
	dirty = true;
}
void Torus::PrintImGuiOptions()
{
	dirty |= ImGui::InputFloat("R", &R, 0.1f, 5.0f, "%.1f");
	dirty |= ImGui::InputFloat("r", &r, 0.1f, 5.0f, "%.1f");
	dirty |= ImGui::InputInt("s1", (int*)&s1, 3, 500);
	dirty |= ImGui::InputInt("s2", (int*)&s2, 3, 500);
}