#include "Shapes.h"

// Shape class functions
void Meshable::Draw(Shader& shader)
{
	if (dirty)
		Mesh();
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", model);
	glLineWidth((selected ? 5.0f : 1.0f)); //alter line width based on selection
	shader.setVec3("color", (selected ? glm::vec3(1.0f,1.0f,0.6f) : glm::vec3(1.0f,1.0f,1.0f)));
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
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
	modelBackup = model = m;
}
void Shape::resetModel()
{
	modelBackup = model = glm::mat4(1.0f);
}
void Shape::ConfirmTransformations()
{
	modelBackup = model;
}
void Shape::CancelTransformations()
{
	model = modelBackup;
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
		ConfirmTransformations();
	}
	ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
	if(ImGui::Button("Apply Rotation X"))
	{
		Rotate(rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
		ConfirmTransformations();
	}
	ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
	if (ImGui::Button("Apply Rotation Y"))
	{
		Rotate(rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
		ConfirmTransformations();
	}
	ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
	if (ImGui::Button("Apply Rotation Z"))
	{
		Rotate(rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
		ConfirmTransformations();
	}
	ImGui::InputFloat3("Translation", glm::value_ptr(translation));
	if (ImGui::Button("Apply Translation"))
	{
		Translate(translation);
		ConfirmTransformations();
	}
	if (ImGui::Button("Reset Transformations"))
	{
		resetModel();
		scale = glm::vec3(1.0f);
		rotationX = rotationY = rotationZ = 0.0f;
		translation = glm::vec3(0.0f);
	}
}
bool Shape::Select(bool deselect)
{
	return selected = (deselect ? false : !selected);
}
glm::vec3 Shape::getPosition()
{
	return glm::vec3(
			model[3][0],
			model[3][1],
			model[3][2]
	);
}
bool Shape::isSelected()
{
	return selected;
}
// Point class functions
Point::Point(glm::vec3 coords)
{
	model = glm::translate(model, coords);
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
void Point::Draw(Shader &shader)
{
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", model);
	glPointSize((selected ? 15.0f : 5.0f)); //alter point size based on selection
	shader.setVec3("color", (selected ? glm::vec3(1.0f, 1.0f, 0.6f) : glm::vec3(1.0f, 1.0f, 1.0f)));
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
void Point::PrintImGuiOptions()
{
	ImGui::Text("This is a point, it has no mesh options.");
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

Ellipsoid::Ellipsoid(float _a, float _b, float _c, unsigned int _s)
{
	a = _a;
	b = _b;
	c = _c;
	s = _s;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	Mesh();
}
void Ellipsoid::Mesh()
{
	if (s < 3)
	{
		std::cerr << "Subdivision must be at least 1.\n";
		s = 3;
	}
	vertices.clear();
	indices.clear();
	dirty = false;
	for (int i = 0; i <= s; i++)
	{
		float phi = (float)i / s * 1.0f * 3.14159265359f;
		for (int j = 0; j <= s; j++)
		{
			// vertices
			float theta = (float)j / s * 2.0f * 3.14159265359f;
			float x = a * sin(phi) * cos(theta);
			float y = b * sin(phi) * sin(theta);
			float z = c * cos(phi);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			// indices
			int v0 = i * (s + 1) + j;
			int v1 = v0 + 1;
			int v2 = v0 + (s + 1);
			int v3 = v2 + 1;
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);
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
void Ellipsoid::PrintImGuiOptions()
{
	dirty |= ImGui::InputFloat("a", &a, 0.1f, 5.0f, "%.1f");
	dirty |= ImGui::InputFloat("b", &b, 0.1f, 5.0f, "%.1f");
	dirty |= ImGui::InputFloat("c", &c, 0.1f, 5.0f, "%.1f");
	dirty |= ImGui::InputInt("s", (int*)&s, 3, 500);
}

Grid::Grid()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	float vertices[] = {
		-1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f,  1.0f,
		 1.0f, 0.0f, -1.0f,
		 1.0f, 0.0f,  1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
Grid Grid::getInstance()
{
	static Grid instance;
	return instance;
}
void Grid::Draw(Camera &camera, char eye)
{
	glBindVertexArray(VAO);
	gridShader.use();
	gridShader.setMat4("view", camera.view());
	glm::mat4 projection;
	switch (eye)
	{
	case 0:
		projection = camera.projection();
		break;
	case 'R':
		projection = camera.projectionRight();
		break;
	case 'L':
		projection = camera.projectionLeft();
		break;
	}
	gridShader.setMat4("projection", projection);
	gridShader.setMat4("model", glm::scale(glm::mat4(1.0f),glm::vec3(40.0f)));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
// Cursor class functions
Cursor Cursor::getInstance()
{
	static Cursor instance;
	return instance;
}
void Cursor::Draw(Shader &shader, char eye)
{
	glBindVertexArray(VAO);
	shader.use();
	/*glm::mat4 projection;
	switch (eye)
	{
	case 0:
		projection = camera.projection();
		break;
	case 'R':
		projection = camera.projectionRight();
		break;
	case 'L':
		projection = camera.projectionLeft();
		break;
	}
	gridShader.setMat4("projection", projection);*/
	shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), location), glm::vec3(0.1f, 0.1f, 0.1f)));
	shader.setVec3("color", glm::vec3(1.0f, 0.0f, 1.0f));
	glLineWidth(5);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
Cursor::Cursor()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	float vertices[] = {
		-1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f,  1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int indices[] = {
		0, 1,
		2, 3,
		4, 5
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
void Cursor::UpdatePosition(Camera& camera, double xpos, double ypos)
{
	float x_ndc = (2.0f * xpos) / camera.windowWidth - 1.0f;
	float y_ndc = 1.0f - (2.0f * ypos) / camera.windowHeight;

	glm::vec4 ray_clip = glm::vec4(x_ndc, y_ndc, -1.0f, 1.0f);

	glm::mat4 invVP = camera.inverseViewProjection();

	// Unproject to world space
	glm::vec4 worldPos = invVP * ray_clip;

	// Perspective divide
	worldPos /= worldPos.w;

	// Compute ray direction
	glm::vec3 rayDir = glm::normalize(glm::vec3(worldPos) - camera.cameraPos);

	// Place cursor some fixed distance in front of camera
	float distance = 5.0f;
	location = camera.cameraPos + rayDir * distance;
}
glm::vec3 Cursor::getPosition()
{
	return location;
}
Shape* Cursor::Click(std::vector<Shape*> shapes)
{
	float minDistance = 1000.0f;
	int closestObject = 0;
	for (int i = 0; i < shapes.size(); i++)
	{
		glm::vec3 objectPosition = shapes[i]->getPosition();
		float distance = glm::length(objectPosition - location);
		if (distance < minDistance)
		{
			minDistance = distance;
			closestObject = i;
		}
	}
	bool isSelected = false;
	if (minDistance < 1.0f)
		isSelected = shapes[closestObject]->Select();
	if(isSelected)
		return shapes[closestObject];
	return nullptr;
}

// Axis class functions
Axis::Axis()
{
	model = glm::mat4(1.0f);
	color = glm::vec3(0.0f);
	VAO = 0;
	VBO = 0;
}
Axis::Axis(char _axis, glm::vec3 translationOrigin)
{
	model = glm::translate(model, translationOrigin);
	switch (_axis)
	{
	case 'x':
		color = glm::vec3(0.8f, 0.1f, 0.1f);
		break;
	case 'y':
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.1f, 0.8f, 0.1f);
		break;
	case 'z':
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		color = glm::vec3(0.1f, 0.1f, 0.8f);
		break;
	default:
		std::cerr << "Invalid axis specified, defaulting to x-axis.\n";
		break;
	}
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	this->SetAxis(model, color);
}
void Axis::SetAxis(glm::mat4 _model, glm::vec3 _color)
{
	model = _model;
	color = _color;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	float vertices[] = {
		-1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glBindVertexArray(0);
}
void Axis::Draw(Shader& shader, char eye)
{
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", model);
	shader.setVec3("color", color);
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}