#include "Shapes.h"

// Shape class functions
Shape::~Shape() {
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
void Shape::Scale(aa::vec3 s, aa::vec3 origin)
{
	//model = aa::scale(model, s);
	aa::mat4 scaleMatrix = aa::mat4(aa::vec4(s.x, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, s.y, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, s.z, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	model = aa::translate(origin) * scaleMatrix * aa::translate(-origin) * modelBackup;
}
void Shape::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	float c = cos(angle);
	float s = sin(angle);
	aa::mat4 rotationMatrix;
	if (axis == aa::Axis::X)
		rotationMatrix = aa::mat4(aa::vec4(1.0f, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, c, s, 0.0f), aa::vec4(0.0f, -s, c, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	else if (axis == aa::Axis::Y)
		rotationMatrix = aa::mat4(aa::vec4(c, 0.0f, -s, 0.0f), aa::vec4(0.0f, 1.0f, 0.0f, 0.0f), aa::vec4(s, 0.0f, c, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	else if (axis == aa::Axis::Z)
		rotationMatrix = aa::mat4(aa::vec4(c, s, 0.0f, 0.0f), aa::vec4(-s, c, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, 1.0f, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	else
	{
		std::cerr << "Rotation axis must be one of the cardinal axes (x,y,z).\n";
		return;
	}
	model = aa::translate(pivot) * rotationMatrix * aa::translate(-pivot) * modelBackup;
}
void Shape::Translate(aa::vec3 t)
{
	//model = aa::translate(model, t);
	aa::mat4 translationMatrix = aa::mat4(aa::vec4(1.0f, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, 1.0f, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, 1.0f, 0.0f), aa::vec4(t.x, t.y, t.z, 1.0f));
	model = translationMatrix * modelBackup;
}
void Shape::TranslateAndConfirm(aa::vec3 t)
{
	Translate(t);
	ConfirmTransformations();
}
void Shape::setModel(aa::mat4 m)
{
	modelBackup = model = m;
}
void Shape::resetModel()
{
	modelBackup = model = aa::mat4(1.0f);
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
	static aa::vec3 scale(1.0f);
	static float rotationX = 0.0f;
	static float rotationY = 0.0f;
	static float rotationZ = 0.0f;
	static aa::vec3 translation(0.0f);
	ImGui::InputFloat3("Scale", aa::value_ptr(scale));
	if (ImGui::Button("Apply Scale"))
	{
		Scale(scale);
		ConfirmTransformations();
	}
	ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
	if(ImGui::Button("Apply Rotation X"))
	{
		Rotate(rotationX, aa::Axis::X);
		ConfirmTransformations();
	}
	ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
	if (ImGui::Button("Apply Rotation Y"))
	{
		Rotate(rotationY, aa::Axis::Y);
		ConfirmTransformations();
	}
	ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
	if (ImGui::Button("Apply Rotation Z"))
	{
		Rotate(rotationZ, aa::Axis::Z);
		ConfirmTransformations();
	}
	ImGui::InputFloat3("Translation", aa::value_ptr(translation));
	if (ImGui::Button("Apply Translation"))
	{
		//Translate(translation); // Needs to be updated, to not use this function
		ConfirmTransformations();
	}
	if (ImGui::Button("Reset Transformations"))
	{
		resetModel();
		scale = aa::vec3(1.0f);
		rotationX = rotationY = rotationZ = 0.0f;
		translation = aa::vec3(0.0f);
	}
}
bool Shape::Select(bool deselect)
{
	return selected = (deselect ? false : !selected);
}
aa::vec3 Shape::getPosition()
{
	return aa::vec3(
			model[3][0],
			model[3][1],
			model[3][2]
	);
}
aa::vec2 Shape::getScreenSpacePosition(Camera& camera)
{
	// clip space transform
	aa::vec4 clipSpacePos = camera.projection() * camera.view() * aa::vec4(getPosition(), 1.0f);//model * aa::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Perspective Division
	if (clipSpacePos.w != 0.0f) {
		aa::vec3 ndc = clipSpacePos.xyz / clipSpacePos.w;

		float screenX = ((ndc.x + 1.0f) / 2.0f) * camera.windowWidth;
		float screenY = ((1.0f - ndc.y) / 2.0f) * camera.windowHeight;

		return aa::vec2(screenX, screenY);
	}

	return aa::vec2(-1.0f);
}
bool Shape::isSelected()
{
	return selected;
}
void Shape::MarkForDeletion()
{
	markedForDeletion = true;
	model = aa::translate(model, aa::vec3(100000.0f, 100000.0f, 100000.0f));
}
bool Shape::isMarkedForDeletion()
{
	return markedForDeletion;
}
void Shape::setShader(Shader& _shader)
{
	shader = _shader;
}

// Meshable class functions // Updaty nie co klatke
void Meshable::Draw()
{
	if (dirty)
		Mesh();
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", model);
	glLineWidth((selected ? 5.0f : 1.0f)); //alter line width based on selection
	shader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
	BezierCurveC0* thisBC = dynamic_cast<BezierCurveC0*>(this);
	BezierCurveC2* thisBC2 = dynamic_cast<BezierCurveC2*>(this);
	BezierSurface* thisBS = dynamic_cast<BezierSurface*>(this);
	if (thisBS)
	{
		// This is a Bezier Surface
		thisBS->tessellationShader.use();
		glBindVertexArray(VAO);
		thisBS->tessellationShader.setMat4("model", model);
		glLineWidth((selected ? 2.5f : 1.0f)); //alter line width based on selection
		thisBS->tessellationShader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
		thisBS->tessellationShader.setFloat("tessLevelU", (float)thisBS->subdivisionsU);
		thisBS->tessellationShader.setFloat("tessLevelV", (float)thisBS->subdivisionsV);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (thisBS->displayControlNet)
		{
			shader.use();
			shader.setVec3("color", CONTROL_LINE_COLOR);
			glBindVertexArray(thisBS->netVAO);
			glLineWidth(0.5f);
			glDrawElements(GL_LINES, thisBS->netIndices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	else if (thisBC2)
	{
		// This is a BezierCurveC2
		thisBC->tessellationShader.use();
		glBindVertexArray(VAO);
		thisBC->tessellationShader.setMat4("model", model);
		glLineWidth((selected ? 5.0f : 1.0f)); //alter line width based on selection
		thisBC->tessellationShader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
		if (shapeName == "__INTERSECTION__")
			thisBC->tessellationShader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 0.5f, 0.5f)));
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		thisBC->tessellationShader.setVec2("tRange", aa::vec2(0.0f, 0.5f));
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		thisBC->tessellationShader.setVec2("tRange", aa::vec2(0.5f, 1.0f));
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		// Splitting into two draw calls to achieve subdivisions over 64
		// Let's now draw the Bernstein Points
		shader.use();
		if (thisBC2->displayBernsteinPoints)
		{
			shader.setVec3("color", aa::vec3(1.0f, 0.0f, 0.0f));
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, thisBC2->bernsteinPoints.size());
		}
		// Drawing the selected Bernstein Point:
		glPointSize(15.0f);
		shader.setVec3("color", aa::vec3(1.0f, 1.0f, 0.6f));
		if (thisBC2->selectedVirtualPoint != -1)
		{
			glDrawArrays(GL_POINTS, thisBC2->selectedVirtualPoint, 1);
		}

		// Control Polyline
		if (thisBC->displayControlPolyline)
		{
			glLineWidth(0.5f);
			shader.setVec3("color", CONTROL_LINE_COLOR);
			glDrawArrays(GL_LINE_STRIP, 0, thisBC2->bernsteinPoints.size());
		}
	}
	else if (thisBC)
	{
		// This is a BezierCurve
		if (thisBC->displayControlPolyline)
		{
			glLineWidth(0.5f);
			shader.setVec3("color", CONTROL_LINE_COLOR);
			glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);
		}
		thisBC->tessellationShader.use();
		glBindVertexArray(VAO);
		thisBC->tessellationShader.setMat4("model", model);
		glLineWidth((selected ? 5.0f : 1.0f)); //alter line width based on selection
		thisBC->tessellationShader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		thisBC->tessellationShader.setVec2("tRange", aa::vec2(0.0f, 0.5f));
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		thisBC->tessellationShader.setVec2("tRange", aa::vec2(0.5f, 1.0f));
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		// Splitting into two draw calls to achieve subdivisions over 64
	}
	else if (dynamic_cast<Line*>(this))
	{
		// This is a Line
		glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}
Meshable::~Meshable()
{
	glDeleteBuffers(1, &EBO);
}

// Point class functions
Point::Point(aa::vec3 coords)
{
	static unsigned int pointIndex = 0;
	shapeName = "Point " + std::to_string(pointIndex++);
	modelBackup = model = aa::translate(coords);
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
void Point::Draw()
{
	if (markedForDeletion)
		return;
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", model);
	glPointSize((selected ? 15.0f : 5.0f)); //alter point size based on selection
	shader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
void Point::PrintImGuiOptions()
{
	aa::vec3 location = getPosition();
	if (ImGui::DragFloat3("Point position", aa::value_ptr(location), 0.01f, -20.0f, 20.0f, "%.3f"))
	{
		model[3][0] = location.x;
		model[3][1] = location.y;
		model[3][2] = location.z;
		ConfirmTransformations();
		InvalidateDependentShapes();
	}
}
void Point::Scale(aa::vec3 s, aa::vec3 origin)
{
	Shape::Scale(s, origin);
	InvalidateDependentShapes();
}
void Point::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	Shape::Rotate(angle, axis, pivot);
	InvalidateDependentShapes();
}
void Point::Translate(aa::vec3 t)
{
	Shape::Translate(t);
	InvalidateDependentShapes();
}
void Point::TranslateAndConfirm(aa::vec3 t)
{
	this->Translate(t);
	this->ConfirmTransformations();
}
void Point::CancelTransformations()
{
	Shape::CancelTransformations();
	InvalidateDependentShapes();
}
void Point::Serialize(nlohmann::json& j)
{
	j["id"] = ShapeTable::GetShapeID(this);
	j["name"] = shapeName;
	j["position"] = { { "x", model[3][0] }, { "y", model[3][1] }, { "z", model[3][2] } };
}
void Point::InvalidateDependentShapes()
{
	for (int i = 0; i < dependentShapes.size(); i++)
	{
		ShapeTable::GetShapeByID(dependentShapes[i])->dirty = true;
	}
}

// Torus class functions
Torus::Torus(float _R, float _r, unsigned int _s1, unsigned int _s2)
{
	shapeName = "Torus";
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
	if (R < 0.1f)
	{
		std::cerr << "Big radius must be at least 0.1.\n";
		R = 0.1f;
	}
	if (r < 0.1f)
	{
		std::cerr << "Small radius must be at least 0.1.\n";
		r = 0.1f;
	}
	if (R > 25.0f)
	{
		std::cerr << "Big radius must be at most 25.\n";
		R = 25.0f;
	}
	if (r > 20.0f)
	{
		std::cerr << "Small radius must be at most 20.\n";
		r = 20.0f;
	}
	if (R < r)
	{
		std::cerr << "Small radius cannot be larger than the big radius.";
		r = R;
	}
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
	if (s1 > 500)
	{
		std::cerr << "Cannot have more than 500 subdivisions.\n";
		s1 = 500;
	}
	if (s2 > 500)
	{
		std::cerr << "Cannot have more than 500 subdivisions.\n";
		s2 = 500;
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

void Torus::Serialize(nlohmann::json& j)
{
	j["id"] = ShapeTable::GetShapeID(this);
	j["largeRadius"] = R;
	j["smallRadius"] = r;
	j["name"] = shapeName;
	j["objectType"] = "torus";
	j["position"] = { { "x", model[3][0] }, { "y", model[3][1] }, { "z", model[3][2] } };
	j["rotation"] = { { "x", aa::degrees(atan2(model[1][0], model[0][0])) }, { "y", aa::degrees(atan2(-model[2][0], sqrt(model[0][0] * model[0][0] + model[1][0] * model[1][0]))) }, { "z", aa::degrees(atan2(model[2][1], model[2][2])) } };
	j["scale"] = { { "x", sqrt(model[0][0] * model[0][0] + model[1][0] * model[1][0] + model[2][0] * model[2][0]) }, { "y", sqrt(model[0][1] * model[0][1] + model[1][1] * model[1][1] + model[2][1] * model[2][1]) }, { "z", sqrt(model[0][2] * model[0][2] + model[1][2] * model[1][2] + model[2][2] * model[2][2]) } };
	j["samples"] = { { "u", s1 }, { "v", s2 } };
}

Ellipsoid::Ellipsoid(float _a, float _b, float _c, unsigned int _s)
{
	shapeName = "Ellipsoid";
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

void Ellipsoid::Serialize(nlohmann::json& j)
{
	// TODO: Implement this
}

// Line class functions
Line::Line(std::vector<int> _points)
{
	points = _points;
	for (int i = 0; i < points.size(); i++)
	{
		ShapeTable::GetPointByID(points[i])->dependentShapes.push_back(ShapeTable::GetShapeID(this));
	}
	shapeName = "Polyline";
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	Mesh();
}
Line::~Line()
{
	for (int i = 0; i < points.size(); i++)
	{
		for (int j = 0; j < ShapeTable::GetPointByID(points[i])->dependentShapes.size(); j++)
		{
			if (ShapeTable::GetPointByID(points[i])->dependentShapes[j] == ShapeTable::GetShapeID(this))
				ShapeTable::GetPointByID(points[i])->dependentShapes.erase(ShapeTable::GetPointByID(points[i])->dependentShapes.begin() + j);
		}
	}
}
void Line::Mesh()
{
	BezierCurveC2* bc2 = dynamic_cast<BezierCurveC2*>(this);
	if (bc2!=nullptr)
	{
		bc2->Mesh(); // using the Mesh() method from the C2 curve class
		return;
	}
	dirty = false;
	vertices.clear();
	indices.clear();
	if (points.size() < 2)
		return; // Nothing to mesh
	for (int i = 0; i < points.size(); i++)
	{
		aa::vec3 pointLocation = ShapeTable::GetShapeByID(points[i])->getPosition();
		vertices.push_back(pointLocation.x);
		vertices.push_back(pointLocation.y);
		vertices.push_back(pointLocation.z);
		indices.push_back(i);
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
void Line::PrintImGuiOptions()
{
	if (ImGui::BeginListBox("##Curve Points", ImVec2(400, 200)))
	{
		for (int i = 0; i < points.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::Selectable((ShapeTable::GetShapeByID(points[i])->Name()).c_str(), ShapeTable::GetShapeByID(points[i])->isSelected()))
			{
				ShapeTable::GetPointByID(points[i])->Select();
			}
			ImGui::PopID();
		}

		ImGui::EndListBox();
	}
}
void Line::Scale(aa::vec3 s, aa::vec3 origin)
{
	for (int i = 0; i < points.size(); i++)
	{
		ShapeTable::GetPointByID(points[i])->Scale(s, origin);
	}
}
void Line::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	for (int i = 0; i < points.size(); i++)
	{
		ShapeTable::GetPointByID(points[i])->Rotate(angle, axis, pivot);
	}
}
void Line::Translate(aa::vec3 t)
{
	for (int i = 0; i < points.size(); i++)
	{
		ShapeTable::GetPointByID(points[i])->Translate(t);
	}
}
void Line::ConfirmTransformations()
{
	for (int i = 0; i < points.size(); i++)
	{
		ShapeTable::GetPointByID(points[i])->ConfirmTransformations();
	}
}
void Line::CancelTransformations()
{
	for (int i = 0; i < points.size(); i++)
	{
		ShapeTable::GetPointByID(points[i])->CancelTransformations();
	}
}
aa::vec3 Line::getPosition()
{
	aa::vec3 averagePosition(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < points.size(); i++)
	{
		averagePosition += ShapeTable::GetPointByID(points[i])->getPosition();
	}
	averagePosition /= (float)points.size();
	return averagePosition;
}
void Line::AddPoint(int point)
{
	if (ShapeTable::GetShapeByID(point) == nullptr)
		return;
	points.push_back(point);
	ShapeTable::GetPointByID(point)->dependentShapes.push_back(ShapeTable::GetShapeID(this));
	dirty = true;
}
void Line::RemoveDeletedPoints()
{
	for (int i = 0; i < points.size(); i++)
	{
		if (ShapeTable::GetShapeByID(points[i])->isMarkedForDeletion())
		{
			points.erase(points.begin() + i);
			i--;
		}
	}
	dirty = true;
}

void Line::Serialize(nlohmann::json& j)
{
	j["id"] = ShapeTable::GetShapeID(this);
	j["name"] = shapeName;
	j["objectType"] = "chain";
	j["controlPoints"] = nlohmann::json::array();
	for (int i = 0; i < points.size(); i++)
	{
		j["controlPoints"].push_back({ {"id", ShapeTable::GetShapeID(points[i])} });
	}
}

// BezierCurveC0 class functions
void BezierCurveC0::Mesh()
{
	dirty = false;
	vertices.clear();
	indices.clear();
	if (points.size() < 2)
		return; // Nothing to mesh
	int numberOfSegments = (points.size() + 1) / 3;
	// First point in the curve:
	aa::vec3 pointLocation = ShapeTable::GetPointByID(points[0])->getPosition();
	vertices.push_back(pointLocation.x);
	vertices.push_back(pointLocation.y);
	vertices.push_back(pointLocation.z);
	for (int i = 0; i < numberOfSegments; i++)
	{
		for (int j = 1; j <= 3; j++)
		{
			int pointIndex = i * 3 + j;
			if (pointIndex >= points.size())
				pointIndex = points.size() - 1; // If there are not enough points for the last segment, repeat the last point
			aa::vec3 pointLocation = ShapeTable::GetPointByID(points[pointIndex])->getPosition();
			vertices.push_back(pointLocation.x);
			vertices.push_back(pointLocation.y);
			vertices.push_back(pointLocation.z);
		}
		for (int j = 0; j < 4; j++)
		{
			indices.push_back(i * 3 + j);
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
void BezierCurveC0::PrintImGuiOptions()
{
	Line::PrintImGuiOptions();
	ImGui::Checkbox("Display Control Polyline", &displayControlPolyline);
}
void BezierCurveC0::setTessellationShader(Shader& _shader)
{
	tessellationShader = _shader;
}

void BezierCurveC0::Serialize(nlohmann::json& j)
{
	Line::Serialize(j); // Serialize common Line properties
	j["objectType"] = "bezierC0";
}

// BezierCurveC1 class functions
void BezierCurveC1::Mesh()
{
	dirty = false;
	vertices.clear();
	indices.clear();
	if (points.size() < 2)
		return; // Nothing to mesh
	int numberOfSegments = (points.size() - 1) / 2;
	if (points.size() < 4)
		numberOfSegments = 1;
	aa::vec3 previous1;
	aa::vec3 previous2;
	// First segment of the curve:
	for (int i = 0; i < 4; i++)
	{
		int pointIndex = i;
		if (pointIndex >= points.size())
			pointIndex = points.size() - 1;
		aa::vec3 pointLocation = ShapeTable::GetPointByID(points[pointIndex])->getPosition();
		vertices.push_back(pointLocation.x);
		vertices.push_back(pointLocation.y);
		vertices.push_back(pointLocation.z);
		indices.push_back(i);
		if (i == 2)
			previous2 = pointLocation;
		else if (i == 3)
			previous1 = pointLocation;
	}
	int pointsSize = points.size();
	for (int i = 1; i < numberOfSegments; i++)
	{
		// calculating the first point, to maintain first derivative continuity
	
		aa::vec3 derivativeVector = previous1 - previous2;
		aa::vec3 newPoint = previous1 + derivativeVector;
		vertices.push_back(newPoint.x);
		vertices.push_back(newPoint.y);
		vertices.push_back(newPoint.z);

		for (int j = 0; j < 2; j++)
		{
			int pointIndex = i * 2 + j + 2;
			if (pointIndex >= points.size())
				pointIndex = points.size() - 1; // If there are not enough points for the last segment, repeat the last point
			aa::vec3 pointLocation = ShapeTable::GetShapeByID(points[pointIndex])->getPosition();
			vertices.push_back(pointLocation.x);
			vertices.push_back(pointLocation.y);
			vertices.push_back(pointLocation.z);
			if(j==0)
				previous2 = pointLocation;
			else if(j==1)
				previous1 = pointLocation;
		}
		for (int j = 0; j < 4; j++)
		{
			indices.push_back(i * 3 + j);
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

void BezierCurveC1::Serialize(nlohmann::json& j)
{
	Line::Serialize(j); // Serialize common Line properties
	j["objectType"] = "bezierC1";
}

// BezierCurveC2 class functions
void BezierCurveC2::Mesh()
{
	dirty = false;
	vertices.clear();
	bernsteinPoints.clear();
	indices.clear();
	int segmentCount = points.size() - 3;
	if (segmentCount < 1)
		return; // nothing to mesh
	for (int i = 0; i < segmentCount; i++)
	{
		aa::vec3 P0 = ShapeTable::GetPointByID(points[i])->getPosition(); 
		aa::vec3 P1 = ShapeTable::GetPointByID(points[i+1])->getPosition();
		aa::vec3 P2 = ShapeTable::GetPointByID(points[i+2])->getPosition();
		aa::vec3 P3 = ShapeTable::GetPointByID(points[i+3])->getPosition();
		if(i==0)
			bernsteinPoints.push_back((P0 + 4 * P1 + P2) / 6.0f); // first point only for first segment
		bernsteinPoints.push_back((4 * P1 + 2 * P2) / 6.0f);
		bernsteinPoints.push_back((2 * P1 + 4 * P2) / 6.0f);
		bernsteinPoints.push_back((P1 + 4 * P2 + P3) / 6.0f);
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
		indices.push_back(i * 3 + 3);
	}
	//prepare for drawing
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bernsteinPoints.size() * 3 * sizeof(float), bernsteinPoints.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
void BezierCurveC2::PrintImGuiOptions()
{
	BezierCurveC0::PrintImGuiOptions();
	ImGui::Checkbox("Display Bernstein Points", &displayBernsteinPoints);
}
float BezierCurveC2::LeftClick(Camera& camera, aa::vec2 clickPos)
{
	if (bernsteinPoints.size() < 1)
		return std::numeric_limits<float>::max();
	if (displayBernsteinPoints == false)
		return std::numeric_limits<float>::max();
	float minDistance = std::numeric_limits<float>::max();
	int closestPoint = -1;
	for (int i = 0; i < bernsteinPoints.size(); i++)
	{
		// Get the screen space position of the Bernstein point
		// clip space transform
		aa::vec4 clipSpacePos = camera.projection() * camera.view() * aa::vec4(bernsteinPoints[i], 1.0f);

		aa::vec2 screenPos = aa::vec2(-1000.0f, -1000.0f);
		// Perspective Division
		if (clipSpacePos.w != 0.0f) {
			aa::vec3 ndc = clipSpacePos.xyz / clipSpacePos.w;

			float screenX = ((ndc.x + 1.0f) / 2.0f) * camera.windowWidth;
			float screenY = ((1.0f - ndc.y) / 2.0f) * camera.windowHeight;

			screenPos = aa::vec2(screenX, screenY);
		}

		float distance = aa::length(screenPos - clickPos);

		if(distance<minDistance)
		{
			minDistance = distance;
			closestPoint = i;
		}
	}
	preparedVirtualPoint = closestPoint;
	return minDistance;
}
void BezierCurveC2::ConfirmSelection(bool shiftPressed, bool justDeselectEverything)
{
	if (!shiftPressed || justDeselectEverything)
	{
		selectedVirtualPoint = -1; // deselect the point
	}
	if (preparedVirtualPoint != -1 && !justDeselectEverything)
	{
		bool wasAPointSelectedOrDeselected = (selectedVirtualPoint != preparedVirtualPoint);
		if (wasAPointSelectedOrDeselected == true)
		{
			containsSelectedVirtualPoints = 1;
			selectedVirtualPoint = preparedVirtualPoint;
		}
		else
		{
			containsSelectedVirtualPoints = 0;
			selectedVirtualPoint = -1;
		}
	}
	preparedVirtualPoint = -1;
}
void BezierCurveC2::VirtualPointsTranslate(aa::vec3 translation)
{
	if (!currentlyTranslatingVirtualPoints) // just started translating, backup the original position
	{
		virtualPointPositionBackup = bernsteinPoints[selectedVirtualPoint];
	}
	aa::vec3 newPosition = virtualPointPositionBackup + translation;
	// Three cases, the selected point can be the first, second or third point of a segment, each case has a different formula
	int deBoorIndex = (selectedVirtualPoint + 4) / 3;
	int pointInSegment = selectedVirtualPoint % 3;
	switch (pointInSegment)
	{
	case 0: // editing the point 'above' the Bernstein polyline
	case 1: // editing the previous de Boor point
	case 2: // editing the next de Boor point
		ShapeTable::GetShapeByID(points[deBoorIndex])->Translate(translation * 3.0f / 2.0f);
		break;
	}
}
void BezierCurveC2::VirtualPointsConfirmTransformations()
{
	int deBoorIndex = (selectedVirtualPoint + 4) / 3;
	ShapeTable::GetShapeByID(points[deBoorIndex])->ConfirmTransformations();
	currentlyTranslatingVirtualPoints = false;
}
void BezierCurveC2::VirtualPointsCancelTransformations()
{
	int deBoorIndex = (selectedVirtualPoint + 4) / 3;
	ShapeTable::GetShapeByID(points[deBoorIndex])->CancelTransformations();
	currentlyTranslatingVirtualPoints = false;
}
aa::vec3 BezierCurveC2::GetVirtualPointsPosition()
{
	if (selectedVirtualPoint == -1)
		return aa::vec3(0.0f, 0.0f, 0.0f); // shouldn't happen
	return bernsteinPoints[selectedVirtualPoint];
}

void BezierCurveC2::Serialize(nlohmann::json& j)
{
	Line::Serialize(j); // Serialize common Line properties
	j["objectType"] = "bezierC2";
}

// Interpolating Curve class functions

InterpolatingCurve::~InterpolatingCurve()
{
	if (shapeName == "__INTERSECTION__")
	{
		for (size_t i = 0; i < points.size(); i++)
		{
			ShapeTable::RemoveShape(points[i]);
		}
	}
}

void InterpolatingCurve::Mesh()
{
	dirty = false;
	vertices.clear();
	bernsteinPoints.clear();
	indices.clear();
	int segmentCount = points.size() - 1;
	if (segmentCount < 1)
		return; // nothing to mesh
	// Interpolation code

	size_t n = points.size() - 1;
	std::vector<float> t;
	std::vector<aa::vec3> pointPositions;
	pointPositions.push_back(ShapeTable::GetPointByID(points[0])->getPosition());
	t.push_back(0.0f);
	for (size_t i = 1; i < n + 1; i++)
	{
		float dValue = aa::distance(ShapeTable::GetPointByID(points[i])->getPosition(), ShapeTable::GetPointByID(points[i - 1])->getPosition());
		if (dValue > 0.0001f)
		{
			t.push_back(t.back() + dValue);
			pointPositions.push_back(ShapeTable::GetPointByID(points[i])->getPosition());
		}
	}
	//std::cout << "n before: " << n << "\n";
	segmentCount = n = pointPositions.size() - 1;
	//std::cout << "n after : " << n << "\n";
	if (n < 1)
	{
		return;
	}

	std::vector<float> h(n);
	for (size_t i = 0; i < n; i++)
	{
		h[i] = t[i + 1] - t[i];
	}
	std::vector<float> a(n + 1), b(n + 1), c(n + 1);
	std::vector<aa::vec3> d(n + 1);
	std::vector<aa::vec3> M(n + 1);
	b[0] = 1.0f;
	d[0] = aa::vec3(0.0f);

	b[n] = 1.0f;
	d[n] = aa::vec3(0.0f);

	for (int i = 1; i < n; i++)
	{
		a[i] = h[i - 1];
		b[i] = 2.0f * (h[i - 1] + h[i]);
		c[i] = h[i];

		aa::vec3 term1 = (pointPositions[i + 1] - pointPositions[i]) / h[i];
		aa::vec3 term2 = (pointPositions[i] - pointPositions[i - 1]) / h[i - 1];

		d[i] = 6.0f * (term1 - term2);
	}

	// Forward sweep
	for (int i = 1; i <= n; i++)
	{
		float w = a[i] / b[i - 1];
		b[i] -= w * c[i - 1];
		d[i] -= w * d[i - 1];
	}

	// Back substitution
	M[n] = d[n] / b[n];

	for (int i = n - 1; i >= 0; i--)
	{
		M[i] = (d[i] - c[i] * M[i + 1]) / b[i];
	}

	std::vector<aa::vec3> D(n + 1);

	for (int i = 0; i < n; i++)
	{
		aa::vec3 term = (pointPositions[i + 1] - pointPositions[i]) / h[i];

		D[i] = term - (h[i] / 6.0f) * (2.0f * M[i] + M[i + 1]);
		D[i + 1] = term + (h[i] / 6.0f) * (M[i] + 2.0f * M[i + 1]);
	}

	for (int i = 0; i < segmentCount; i++)
	{
		aa::vec3 P0 = pointPositions[i];
		aa::vec3 P1 = pointPositions[i + 1];
		aa::vec3 B0 = P0;
		aa::vec3 B1 = P0 + (h[i] / 3.0f) * D[i];
		aa::vec3 B2 = P1 - (h[i] / 3.0f) * D[i + 1];
		aa::vec3 B3 = P1;
		if (i == 0)
			bernsteinPoints.push_back(B0); // first point only for first segment
		bernsteinPoints.push_back(B1);
		bernsteinPoints.push_back(B2);
		bernsteinPoints.push_back(B3);
		indices.push_back(i * 3);
		indices.push_back(i * 3 + 1);
		indices.push_back(i * 3 + 2);
		indices.push_back(i * 3 + 3);
	}
	//prepare for drawing
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bernsteinPoints.size() * 3 * sizeof(float), bernsteinPoints.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}

void InterpolatingCurve::Serialize(nlohmann::json& j)
{
	Line::Serialize(j); // Serialize common Line properties
	j["objectType"] = "interpolatedC2";
}

// BezierSurface class functions
BezierSurface::BezierSurface(aa::vec3 position, int a, int b, float dimensionX, float dimensionZ, bool _isC2, bool _isCylinder)
{
	isC2 = _isC2;
	isCylinder = _isCylinder;
	shapeName = "BezierSurface ";
	shapeName += (isC2 ? "C2" : "C0");
	if (isCylinder)
	{
		int pointsAround = a * 3;
		int pointsY = b * 3 + 1;
		if (isC2)
		{
			pointsAround = a;
			pointsY = b + 3;
		}
		float stepX = dimensionX / pointsAround;
		float stepZ = dimensionZ / pointsY;
		position.x -= (dimensionX - stepX) / 2.0f;
		position.z -= (dimensionZ - stepZ) / 2.0f;
		float angleStep = 2.0f * 3.14159265359f / pointsAround;
		float currentAngle = 0.0f;
		for (size_t i = 0; i < pointsAround; i++)
		{
			controlPoints.push_back(std::vector<int>());
			position.x = cos(currentAngle) * (dimensionX);
			position.y = 0.0f;
			position.z = sin(currentAngle) * (dimensionX);
			currentAngle += angleStep;
			for (size_t j = 0; j < pointsY; j++)
			{
				Point* newPoint = new Point(position);
				int id = ShapeTable::AddShape(newPoint);
				controlPoints.back().push_back(id);
				ShapeTable::GetPointByID(id)->dependentShapes.push_back(ShapeTable::GetShapeID(this));
				position.y += stepZ;
			}
		}
	}
	else
	{
		int pointsX = a * 3 + 1;
		int pointsZ = b * 3 + 1;
		if (isC2)
		{
			pointsX = a + 3;
			pointsZ = b + 3;
		}
		float stepX = dimensionX / pointsX;
		float stepZ = dimensionZ / pointsZ;
		position.x -= (dimensionX - stepX) / 2.0f;
		position.z -= (dimensionZ - stepZ) / 2.0f;
		for (size_t i = 0; i < pointsX; i++)
		{
			controlPoints.push_back(std::vector<int>());
			for (size_t j = 0; j < pointsZ; j++)
			{
				Point* newPoint = new Point(position);
				int id = ShapeTable::AddShape(newPoint);
				controlPoints.back().push_back(id);
				ShapeTable::GetPointByID(id)->dependentShapes.push_back(ShapeTable::GetShapeID(this));
				position.z += stepZ;
			}
			position.x += stepX;
			position.z = position.z - stepZ * pointsZ;
		}
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &netVAO);
	glGenBuffers(1, &netEBO);
	Mesh();
}

BezierSurface::BezierSurface(std::vector<std::vector<int>> _controlPoints, bool _isC2, bool _isCylinder, int _subdivisionsU, int _subdivisionsV)
{
	isC2 = _isC2;
	isCylinder = _isCylinder;
	shapeName = "BezierSurface ";
	shapeName += (isC2 ? "C2" : "C0");
	controlPoints = _controlPoints;
	subdivisionsU = _subdivisionsU;
	subdivisionsV = _subdivisionsV;
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			ShapeTable::GetPointByID(controlPoints[i][j])->dependentShapes.push_back(ShapeTable::GetShapeID(this));
		}
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &netVAO);
	glGenBuffers(1, &netEBO);
	Mesh();
}

BezierSurface::~BezierSurface()
{
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			Point* shapePointer = ShapeTable::GetPointByID(controlPoints[i][j]);
			if (shapePointer != nullptr)
				shapePointer->MarkForDeletion();
		}
	}
	// TODO release GPU resources
}

void BezierSurface::Mesh()
{
	dirty = false;
	vertices.clear();
	indices.clear();
	
	if(isC2)
		MeshC2();
	else
		MeshC0();
}

void BezierSurface::PrintImGuiOptions()
{
	if (ImGui::DragInt("Subdivisions U", &subdivisionsU, 1, 3, 64))
	{
		if (subdivisionsU < 3)
			subdivisionsU = 3;
		else if(subdivisionsU > 64)
			subdivisionsU = 64;
	}
	if (ImGui::DragInt("Subdivisions V", &subdivisionsV, 1, 3, 64))
	{
		if (subdivisionsV < 3)
			subdivisionsV = 3;
		else if (subdivisionsV > 64)
			subdivisionsV = 64;
	}
	ImGui::Checkbox("Display Control Net", &displayControlNet);
}

void BezierSurface::RemoveDeletedPoints()
{
	bool deleteThisSurface = false;
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			if (ShapeTable::GetPointByID(controlPoints[i][j])->isMarkedForDeletion())
				deleteThisSurface = true;
		}
	}
	if(deleteThisSurface)
		markedForDeletion = true;
}

void BezierSurface::setTessellationShader(Shader& _shader)
{
	tessellationShader = _shader;
}

void BezierSurface::Serialize(nlohmann::json& j)
{
	j["id"] = ShapeTable::GetShapeID(this);
	j["name"] = shapeName;
	if(isC2)
		j["objectType"] = "bezierSurfaceC2";
	else
		j["objectType"] = "bezierSurfaceC0";
	j["controlPoints"] = nlohmann::json::array();
	size_t repeatRows = (isC2 ? 3 : 1);
	if (!isCylinder)
		repeatRows = 0;
	
	int sizeU = controlPoints.size();
	int sizeV = controlPoints[0].size();
	for (size_t i = 0; i < sizeV; i++)
	{
		for (size_t k = 0; k < sizeU + repeatRows; k++)
		{
			nlohmann::json pointJson = { {"id", ShapeTable::GetShapeID(controlPoints[k%sizeU][i%sizeV])} };
			j["controlPoints"].push_back(pointJson);
		}
	}
	if (isCylinder)
	{
		if (isC2)
			sizeU += 3;
		else
			sizeU++;
	}
	j["samples"]["u"] = subdivisionsV;
	j["samples"]["v"] = subdivisionsU; // temporary, TODO: find the real cause
	j["size"]["u"] = sizeU;
	j["size"]["v"] = sizeV;
	
}

std::vector<SurfaceEdge> BezierSurface::GetBoundaryEdges()
{
	std::vector<SurfaceEdge> edges;
	int u = controlPoints.size();
	int v = controlPoints[0].size();
	for (size_t i = 0; i < u; i += u - 1)
	{
		for (size_t j = 0; j < v - 1; j+=3)
		{
			SurfaceEdge newEdge;
			newEdge.boundary[0] = ShapeTable::GetPointByID(controlPoints[i][j%v]);
			newEdge.boundary[1] = ShapeTable::GetPointByID(controlPoints[i][(j+1)%v]);
			newEdge.boundary[2] = ShapeTable::GetPointByID(controlPoints[i][(j+2)%v]);
			newEdge.boundary[3] = ShapeTable::GetPointByID(controlPoints[i][(j+3)%v]);

			int interiorOffset = (i == 0) ? 1 : -1;
			newEdge.interior[0] = ShapeTable::GetPointByID(controlPoints[(i+interiorOffset)%u][j]);
			newEdge.interior[1] = ShapeTable::GetPointByID(controlPoints[(i+interiorOffset)%u][(j + 1)%v]);
			newEdge.interior[2] = ShapeTable::GetPointByID(controlPoints[(i+interiorOffset)%u][(j + 2)%v]);
			newEdge.interior[3] = ShapeTable::GetPointByID(controlPoints[(i+interiorOffset)%u][(j + 3)%v]);
			newEdge.surface = this;
			edges.push_back(newEdge);
		}
	}
	for (size_t i = 0; i < v; i += v - 1)
	{
		for (size_t j = 0; j < u - 1; j += 3)
		{
			SurfaceEdge newEdge;
			newEdge.boundary[0] = ShapeTable::GetPointByID(controlPoints[j%u][i]);
			newEdge.boundary[1] = ShapeTable::GetPointByID(controlPoints[(j + 1)%u][i]);
			newEdge.boundary[2] = ShapeTable::GetPointByID(controlPoints[(j + 2)%u][i]);
			newEdge.boundary[3] = ShapeTable::GetPointByID(controlPoints[(j + 3)%u][i]);

			int interiorOffset = (i == 0) ? 1 : -1;
			newEdge.interior[0] = ShapeTable::GetPointByID(controlPoints[j%u][(i + interiorOffset)%v]);
			newEdge.interior[1] = ShapeTable::GetPointByID(controlPoints[(j + 1)%u][(i + interiorOffset)%v]);
			newEdge.interior[2] = ShapeTable::GetPointByID(controlPoints[(j + 2)%u][(i + interiorOffset)%v]);
			newEdge.interior[3] = ShapeTable::GetPointByID(controlPoints[(j + 3)%u][(i + interiorOffset)%v]);
			newEdge.surface = this;
			edges.push_back(newEdge);
		}
	}
	return edges;
}

aa::vec3 BezierSurface::getPosition()
{
	aa::vec3 averagePosition(0.0f, 0.0f, 0.0f);
	int n = controlPoints.size();
	int m = controlPoints[0].size();
	for (int i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			averagePosition += ShapeTable::GetPointByID(controlPoints[i][j])->getPosition();
		}
	}
	averagePosition /= (float)n;
	averagePosition /= (float)m;
	return averagePosition;
}

aa::vec3 BezierSurface::Evaluate(float u, float v)
{
	// first we need to decide which control points we will use, depending on the parameters and number of subsurfaces
	aa::vec3 p[4], point;
	if (isC2)
	{
		size_t patchCountU = controlPoints.size() - 3;
		size_t patchCountV = controlPoints[0].size() - 3;
	}
	else
	{
		size_t patchCountU = controlPoints.size() / 3;
		size_t patchCountV = controlPoints[0].size() / 3;
		size_t pointOffsetU = (size_t)(floor(patchCountU * u)) * 3;
		size_t pointOffsetV = (size_t)(floor(patchCountV * v)) * 3;
		if (pointOffsetU > (patchCountU - 1) * 3)
			pointOffsetU = (patchCountU - 1) * 3;
		if (pointOffsetV > (patchCountV - 1) * 3)
			pointOffsetV = (patchCountV - 1) * 3;
		float globalU = patchCountU * u;
		size_t patchU = std::min((size_t)floor(globalU), patchCountU - 1);
		float localU = globalU - patchU;
		float globalV = patchCountV * v;
		size_t patchV = std::min((size_t)floor(globalV), patchCountV - 1);
		float localV = globalV - patchV;
		for (size_t i = 0; i < 4; i++)
		{
			// 4 bezier curves: fetch actual control point positions
			aa::vec3 c0 = ShapeTable::GetShapeByID(controlPoints[pointOffsetU + i][pointOffsetV])->getPosition();
			aa::vec3 c1 = ShapeTable::GetShapeByID(controlPoints[pointOffsetU + i][pointOffsetV + 1])->getPosition();
			aa::vec3 c2 = ShapeTable::GetShapeByID(controlPoints[pointOffsetU + i][pointOffsetV + 2])->getPosition();
			aa::vec3 c3 = ShapeTable::GetShapeByID(controlPoints[pointOffsetU + i][pointOffsetV + 3])->getPosition();
			p[i] = aa::bezier(c0, c1, c2, c3, localV);
		}
		point = aa::bezier(p[0], p[1], p[2], p[3], localU);
	}
	return point;
}

aa::vec3 BezierSurface::Du(float u, float v)
{
	aa::vec3 p[4];

	size_t patchCountU = controlPoints.size() / 3;
	size_t patchCountV = controlPoints[0].size() / 3;

	float globalU = patchCountU * u;
	float globalV = patchCountV * v;

	size_t patchU =
		std::min((size_t)floor(globalU), patchCountU - 1);

	size_t patchV =
		std::min((size_t)floor(globalV), patchCountV - 1);

	float localU = globalU - patchU;
	float localV = globalV - patchV;

	size_t offsetU = patchU * 3;
	size_t offsetV = patchV * 3;

	for (size_t i = 0; i < 4; i++)
	{
		aa::vec3 c0 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV])->getPosition();
		aa::vec3 c1 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV + 1])->getPosition();
		aa::vec3 c2 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV + 2])->getPosition();
		aa::vec3 c3 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV + 3])->getPosition();
		p[i] = aa::bezier(c0, c1, c2, c3, localV);
	}

	return patchCountU * aa::bezier_derivative(
		p[0], p[1], p[2], p[3],
		localU);
}

aa::vec3 BezierSurface::Dv(float u, float v)
{
	aa::vec3 dp[4];

	size_t patchCountU = controlPoints.size() / 3;
	size_t patchCountV = controlPoints[0].size() / 3;

	float globalU = patchCountU * u;
	float globalV = patchCountV * v;

	size_t patchU =
		std::min((size_t)floor(globalU), patchCountU - 1);

	size_t patchV =
		std::min((size_t)floor(globalV), patchCountV - 1);

	float localU = globalU - patchU;
	float localV = globalV - patchV;

	size_t offsetU = patchU * 3;
	size_t offsetV = patchV * 3;

	for (size_t i = 0; i < 4; i++)
	{
		aa::vec3 c0 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV])->getPosition();
		aa::vec3 c1 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV + 1])->getPosition();
		aa::vec3 c2 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV + 2])->getPosition();
		aa::vec3 c3 = ShapeTable::GetShapeByID(controlPoints[offsetU + i][offsetV + 3])->getPosition();
		dp[i] = aa::bezier_derivative(c0, c1, c2, c3, localV);
	}

	return patchCountV * aa::bezier(
		dp[0], dp[1], dp[2], dp[3],
		localU);
}

void BezierSurface::MeshC0()
{
	size_t n = controlPoints.size();
	size_t m = controlPoints[0].size();
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			aa::vec3 pointPos = ShapeTable::GetShapeByID(controlPoints[i][j])->getPosition();
			vertices.push_back(pointPos.x);
			vertices.push_back(pointPos.y);
			vertices.push_back(pointPos.z);
			size_t im1 = (i + n - 1) % n;
			size_t jm1 = (j + m - 1) % m;
			if (i != 0 || isCylinder)
			{
				netIndices.push_back(i * m + j);
				netIndices.push_back(im1 * m + j);
			}
			if (j != 0 || isCylinder)
			{
				netIndices.push_back(i * m + j);
				netIndices.push_back(i * m + jm1);
			}
		}
	}
	// indices
	if (isCylinder)
	{
		for (size_t i = 0; i < n; i += 3)
		{
			for (size_t j = 0; j < m - 1; j += 3)
			{
				for (size_t k = 0; k < 4; k++)
				{
					for (size_t l = 0; l < 4; l++)
					{
						size_t wrappedI = (i + k) % n;

						indices.push_back(
							wrappedI * m + (j + l)
						);
					}
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < n - 1; i += 3)
		{
			for (size_t j = 0; j < m - 1; j += 3)
			{
				for (size_t k = 0; k < 4; k++)
				{
					for (size_t l = 0; l < 4; l++)
					{
						indices.push_back((i + k) * m + (j + l));
					}
				}
			}
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

	glBindVertexArray(netVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, netEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, netIndices.size() * sizeof(unsigned int), netIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}

void BezierSurface::MeshC2()
{
	size_t n = controlPoints.size();
	size_t m = controlPoints[0].size();
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			aa::vec3 pointPos = ShapeTable::GetShapeByID(controlPoints[i][j])->getPosition();
			vertices.push_back(pointPos.x);
			vertices.push_back(pointPos.y);
			vertices.push_back(pointPos.z);
			size_t im1 = (i + n - 1) % n;
			size_t jm1 = (j + m - 1) % m;
			if (i != 0 || isCylinder)
			{
				netIndices.push_back(i * m + j);
				netIndices.push_back(im1 * m + j);
			}
			if (j != 0 || isCylinder)
			{
				netIndices.push_back(i * m + j);
				netIndices.push_back(i * m + jm1);
			}
		}
	}
	// indices
	if (isCylinder)
	{
		for (size_t i = 0; i < n; i += 1)
		{
			for (size_t j = 0; j < m - 3; j += 1)
			{
				for (size_t k = 0; k < 4; k++)
				{
					for (size_t l = 0; l < 4; l++)
					{
						size_t ii = (i + k) % n;
						size_t jj = (j + l) % m;

						indices.push_back(ii * m + jj);
					}
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < n - 3; i += 1)
		{
			for (size_t j = 0; j < m - 3; j += 1)
			{
				for (size_t k = 0; k < 4; k++)
				{
					for (size_t l = 0; l < 4; l++)
					{
						indices.push_back((i + k) * m + (j + l));
					}
				}
			}
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

	glBindVertexArray(netVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, netEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, netIndices.size() * sizeof(unsigned int), netIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
void BezierSurface::Scale(aa::vec3 s, aa::vec3 origin)
{
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			ShapeTable::GetPointByID(controlPoints[i][j])->Scale(s, origin);
		}
	}
}
void BezierSurface::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			ShapeTable::GetPointByID(controlPoints[i][j])->Rotate(angle, axis, pivot);
		}
	}
}
void BezierSurface::Translate(aa::vec3 t)
{
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			ShapeTable::GetPointByID(controlPoints[i][j])->Translate(t);
		}
	}
}
void BezierSurface::ConfirmTransformations()
{
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			ShapeTable::GetPointByID(controlPoints[i][j])->ConfirmTransformations();
		}
	}
}
void BezierSurface::CancelTransformations()
{
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		for (size_t j = 0; j < controlPoints[i].size(); j++)
		{
			ShapeTable::GetPointByID(controlPoints[i][j])->CancelTransformations();
		}
	}
}

// GregoryPatch class functions

GregoryPatch::GregoryPatch(std::vector<int> edge, std::vector<int> secondRow)
{
	edgePoints = edge;
	secondRowPoints = secondRow;
	for (size_t i = 0; i < edgePoints.size(); i++)
	{
		ShapeTable::GetPointByID(edgePoints[i])->dependentShapes.push_back(ShapeTable::GetShapeID(this));
	}
	for (size_t i = 0; i < secondRowPoints.size(); i++)
	{
		ShapeTable::GetPointByID(secondRowPoints[i])->dependentShapes.push_back(ShapeTable::GetShapeID(this));
	}
	shapeName = "Gregory Patch";
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	Mesh();
}

GregoryPatch::~GregoryPatch()
{
	;
}

void GregoryPatch::Mesh()
{
	vertices.clear();
	indices.clear();
	dirty = false;
	if (edgePoints.size() != 12 || secondRowPoints.size() != 12)
		return; // invalid Gregory patch definition
	// First let's find a midpoint for each edge
	std::vector<aa::vec3> edgeMidpoints(3);
	std::vector<aa::vec3> edgeMidpointNormals(3);
	std::vector<std::vector<aa::vec3>> subdividedEdgePoints(3);
	std::vector<std::vector<aa::vec3>> secondRowSubdividedEdgePoints(3);
	std::vector<std::vector<aa::vec3>> controlPointsFromOuterEdges(3);
	std::vector<aa::vec3> corners(3);
	for (size_t i = 0; i < 3; i++)
	{
		std::vector<aa::vec3> oneEdge(5);
		aa::vec3 p0 = corners[i] = ShapeTable::GetShapeByID(edgePoints[i * 4])->getPosition();
		aa::vec3 p1 = ShapeTable::GetShapeByID(edgePoints[i * 4 + 1])->getPosition();
		aa::vec3 p2 = ShapeTable::GetShapeByID(edgePoints[i * 4 + 2])->getPosition();
		aa::vec3 p3 = ShapeTable::GetShapeByID(edgePoints[i * 4 + 3])->getPosition();

		oneEdge[0] = p0 = (p0 + p1) * 0.5f;
		p1 = (p1 + p2) * 0.5f;
		oneEdge[4] = p2 = (p2 + p3) * 0.5f;

		oneEdge[1] = p0 = (p0 + p1) * 0.5f;
		oneEdge[3] = p1 = (p1 + p2) * 0.5f;

		oneEdge[2] = edgeMidpoints[i] = (p0 + p1) * 0.5f;
		subdividedEdgePoints[i] = oneEdge;

		//Now the second derivatives to get normal points
		p0 = ShapeTable::GetShapeByID(edgePoints[i * 4])->getPosition();
		p1 = ShapeTable::GetShapeByID(edgePoints[i * 4 + 1])->getPosition();
		p2 = ShapeTable::GetShapeByID(edgePoints[i * 4 + 2])->getPosition();
		p3 = ShapeTable::GetShapeByID(edgePoints[i * 4 + 3])->getPosition();
		aa::vec3 d0 = p1 - p0;
		aa::vec3 d1 = p2 - p1;
		aa::vec3 d2 = p3 - p2;

		d0 = d1 - d0;
		d1 = d2 - d1;

		d0 =(d0 + d1) * 0.5f;
		edgeMidpointNormals[i] = aa::normalize(d0);
		
	}
	// Now let's calculate the P^2i points
	std::vector<aa::vec3> P2i(3);
	for (size_t i = 0; i < 3; i++)
	{
		// first subdivide the second row
		std::vector<aa::vec3> oneEdge(5);
		aa::vec3 p0 = ShapeTable::GetShapeByID(secondRowPoints[i * 4])->getPosition();
		aa::vec3 p1 = ShapeTable::GetShapeByID(secondRowPoints[i * 4 + 1])->getPosition();
		aa::vec3 p2 = ShapeTable::GetShapeByID(secondRowPoints[i * 4 + 2])->getPosition();
		aa::vec3 p3 = ShapeTable::GetShapeByID(secondRowPoints[i * 4 + 3])->getPosition();

		oneEdge[0] = p0 = (p0 + p1) * 0.5f;
		p1 = (p1 + p2) * 0.5f;
		oneEdge[4] = p2 = (p2 + p3) * 0.5f;

		oneEdge[1] = p0 = (p0 + p1) * 0.5f;
		oneEdge[3] = p1 = (p1 + p2) * 0.5f;

		aa::vec3 secondRowMidpoint = oneEdge[2] = (p0 + p1) * 0.5f;

		oneEdge[2] = P2i[i] = edgeMidpoints[i] + (edgeMidpoints[i] - secondRowMidpoint);
		secondRowSubdividedEdgePoints[i] = oneEdge;
	}
	for (size_t i = 0; i < 3; i++)
	{
		controlPointsFromOuterEdges[i].push_back((subdividedEdgePoints[i][0] - secondRowSubdividedEdgePoints[i][0]) + subdividedEdgePoints[i][0]);
		controlPointsFromOuterEdges[i].push_back((subdividedEdgePoints[i][1] - secondRowSubdividedEdgePoints[i][1]) + subdividedEdgePoints[i][1]);
		controlPointsFromOuterEdges[i].push_back((subdividedEdgePoints[i][3] - secondRowSubdividedEdgePoints[i][3]) + subdividedEdgePoints[i][3]);
		controlPointsFromOuterEdges[i].push_back((subdividedEdgePoints[i][4] - secondRowSubdividedEdgePoints[i][4]) + subdividedEdgePoints[i][4]);
	}
	// Now the auxiliary points Q
	std::vector<aa::vec3> Q(3);
	for (size_t i = 0; i < 3; i++)
	{
		Q[i] = (3.0f * P2i[i] - edgeMidpoints[i]) / 2.0f;
	}
	// Average central Point P
	aa::vec3 P = (Q[0] + Q[1] + Q[2]) / 3.0f;
	// Now the P1i points
	std::vector<aa::vec3> P1i(3);
	for (size_t i = 0; i < 3; i++)
	{
		P1i[i] = (2.0f * Q[i] + P) / 3.0f;
	}
	 
	// Now the inner points not associated with outer edges
	std::vector<aa::vec3[4]> IPNO(3);
	for (size_t i = 0; i < 3; i++)
	{
		IPNO[i][0] = (subdividedEdgePoints[i][1] - subdividedEdgePoints[i][2] + (P1i[(i + 2) % 3] - P)) / 2.0f + P2i[i];
		IPNO[i][1] = (subdividedEdgePoints[i][1] - subdividedEdgePoints[i][2] + (P1i[(i + 2) % 3] - P)) / 2.0f + P1i[i];
		IPNO[i][2] = (subdividedEdgePoints[((int)i + 2) % 3][3] - subdividedEdgePoints[((int)i + 2) % 3][2] + (P1i[i] - P)) / 2.0f + P2i[((int)i + 2) % 3];
		IPNO[i][3] = (subdividedEdgePoints[((int)i + 2) % 3][3] - subdividedEdgePoints[((int)i + 2) % 3][2] + (P1i[i] - P)) / 2.0f + P1i[((int)i + 2) % 3];
	}

	// Now let's make these points fullfil the C1 condition
	std::vector<aa::vec3[4]> IPNOcorrected(3);
	for (size_t i = 0; i < 3; i++)
	{
		aa::vec3 v1 = IPNO[i][0] - IPNO[((int)i + 1) % 3][2];
		aa::vec3 v2 = IPNO[i][1] - IPNO[((int)i + 1) % 3][3];
		aa::vec3 v3 = IPNO[i][2] - IPNO[((int)i + 2) % 3][0];
		aa::vec3 v4 = IPNO[i][3] - IPNO[((int)i + 2) % 3][1];
		IPNOcorrected[i][0] = P2i[i] + v1 / 2.0f;
		IPNOcorrected[i][1] = P1i[i] + v2 / 2.0f;
		IPNOcorrected[i][2] = P2i[((int)i + 2) % 3] + v3 / 2.0f;
		IPNOcorrected[i][3] = P1i[((int)i + 2) % 3] + v4 / 2.0f;
	}

	// Now we have the 3 curves, so we will find the control points for the 3 patches
	std::vector<aa::vec3> patchControlPoints;
	for (size_t i = 0; i < 3; i++)
	{
		//first row
		patchControlPoints.push_back(corners[i]);
		patchControlPoints.push_back(subdividedEdgePoints[i][0]);
		patchControlPoints.push_back(subdividedEdgePoints[i][1]);
		patchControlPoints.push_back(subdividedEdgePoints[i][2]);
		//second row
		patchControlPoints.push_back(subdividedEdgePoints[((int)i + 2) % 3][4]);
		patchControlPoints.push_back(controlPointsFromOuterEdges[((int)i + 2) % 3][3]);
		patchControlPoints.push_back(controlPointsFromOuterEdges[i][0]);
		patchControlPoints.push_back(controlPointsFromOuterEdges[i][1]);
		patchControlPoints.push_back(IPNOcorrected[i][0]);
		patchControlPoints.push_back(P2i[i]);
		//third row
		patchControlPoints.push_back(subdividedEdgePoints[((int)i + 2) % 3][3]);
		patchControlPoints.push_back(controlPointsFromOuterEdges[((int)i + 2) % 3][2]);
		patchControlPoints.push_back(IPNOcorrected[i][2]);
		patchControlPoints.push_back(IPNOcorrected[i][3]);
		patchControlPoints.push_back(IPNOcorrected[i][1]);
		patchControlPoints.push_back(P1i[i]);
		//fourth row
		patchControlPoints.push_back(subdividedEdgePoints[((int)i + 2) % 3][2]);
		patchControlPoints.push_back(P2i[((int)i + 2) % 3]);
		patchControlPoints.push_back(P1i[((int)i + 2) % 3]);
		patchControlPoints.push_back(P);
	}
	for (size_t i = 0; i < patchControlPoints.size(); i++)
	{
		vertices.push_back(patchControlPoints[i].x);
		vertices.push_back(patchControlPoints[i].y);
		vertices.push_back(patchControlPoints[i].z);
	}
	indices.resize(22);
	indices = {1, 6, 2, 7, 4, 5, 10, 11, 9, 8, 15, 14, 17, 12, 18, 13, 3, 9, 9, 15, 15, 19};
	size_t n = indices.size();
	for (size_t i = 0; i < n * 2; i++)
	{
		indices.push_back(indices[i] + 20);
	}
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void GregoryPatch::PrintImGuiOptions()
{
	if (ImGui::DragInt("Subdivisions", &tessLevel, 1, 3, 64))
	{
		if (tessLevel < 3)
			tessLevel = 3;
		else if (tessLevel > 64)
			tessLevel = 64;
	}
	ImGui::Checkbox("Display continuity vectors", &dispVectors);
}

void GregoryPatch::Scale(aa::vec3 s, aa::vec3 origin)
{
	;// Do nothing, Gregory patches are not directly transformable, they are defined by other points that can be transformed
}

void GregoryPatch::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	;// Do nothing
}

void GregoryPatch::Translate(aa::vec3 t)
{
	;// Do nothing
}

void GregoryPatch::ConfirmTransformations()
{
	;// Do nothing
}

void GregoryPatch::CancelTransformations()
{
	;// Do nothing
}

void GregoryPatch::Serialize(nlohmann::json& j)
{
	j = NULL; // For now
}

void GregoryPatch::Draw()
{
	if (markedForDeletion)
		return;
	if (dirty)
		Mesh();
	GregoryShader.use();
	glBindVertexArray(VAO);
	GregoryShader.setMat4("model", aa::mat4(1.0f));
	glLineWidth((selected ? 5.0f : 1.0f)); //alter point size based on selection
	GregoryShader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
	GregoryShader.setFloat("tessLevel", (float)tessLevel);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPatchParameteri(GL_PATCH_VERTICES, 20);
	glDrawArrays(GL_PATCHES, 0, 60);
	// Now the continuity vectors
	if (dispVectors)
	{
		shader.use();
		shader.setMat4("model", aa::mat4(1.0f));
		shader.setVec3("color", aa::vec3(0.188f, 0.835f, 0.784f));
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}

void GregoryPatch::setGregoryShader(Shader& _shader)
{
	GregoryShader = _shader;
}

aa::vec3 GregoryPatch::getPosition()
{
	aa::vec3 averagePosition(0.0f, 0.0f, 0.0f);
	int n = edgePoints.size();
	for (int i = 0; i < n; i++)
	{
		averagePosition += ShapeTable::GetPointByID(edgePoints[i])->getPosition();
	}
	averagePosition /= (float)n;
	return averagePosition;
}

// Intersection class functions

Intersection::Intersection(ISurface* first, ISurface* second)
{
	firstSurface = first;
	secondSurface = second;
	shapeName = "Intersection Curve";
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	Mesh();
}

Intersection::~Intersection()
{
	delete curve;
}

void Intersection::Mesh()
{
	dirty = false;
	// First let's use Monte Carlo to find our initial guess
	float distance = 1.0f;
	TwoSurfacesState bestGuess;
	bestGuess.first = firstSurface;
	bestGuess.second = secondSurface;
	bestGuess.u1 = 0.5f;
	bestGuess.v1 = 0.5f;
	bestGuess.u2 = 0.5f;
	bestGuess.v2 = 0.5f;
	float width = 0.5f;
	size_t counter = 0;
	while (distance > 0.0000005f)
	{
		bestGuess = RunMonteCarlo(bestGuess, width);
		distance = MeasureDistance2(bestGuess);
		width /= 2.0f;
		counter++;
		if (counter > 20) // some limit for Monte Carlo iterations
		{
			break;
		}
	}
	// We have the initial guess
	aa::vec3 point1 = bestGuess.first->Evaluate(bestGuess.u1, bestGuess.v1);
	aa::vec3 point2 = bestGuess.second->Evaluate(bestGuess.u2, bestGuess.v2);
	float u = bestGuess.u1;
	float v = bestGuess.v1; // I need to add Newton here!!! TODO
	float s = bestGuess.u2;
	float t = bestGuess.v2;
	std::cout << "Point1: " << point1.x << ", " << point1.y << ", " << point1.z << ", u: " << bestGuess.u1 << ", v: " << bestGuess.v1 << "\n";
	std::cout << "Point1: " << point2.x << ", " << point2.y << ", " << point2.z << ", u: " << bestGuess.u2 << ", v: " << bestGuess.v2 << "\n";
	vertices.push_back(point1.x);
	vertices.push_back(point1.y);
	vertices.push_back(point1.z);
	vertices.push_back(point2.x);
	vertices.push_back(point2.y);
	vertices.push_back(point2.z);

	// Now let's walk along the line:
	std::vector<aa::vec3> pointsRight = GetThePointsInOneDirection(bestGuess, point1, false);
	std::vector<aa::vec3> pointsLeft = GetThePointsInOneDirection(bestGuess, point1, true);
	
	std::vector<int> allPoints;

	Point* pointerPoint;
	for (int i = pointsLeft.size() - 1; i >= 0 ; i--)
	{
		pointerPoint = new Point(aa::vec3(0.0f));
		pointerPoint->TranslateAndConfirm(pointsLeft[i]);
		allPoints.push_back(ShapeTable::AddShape(pointerPoint));
		vertices.push_back(pointsLeft[i].x);
		vertices.push_back(pointsLeft[i].y);
		vertices.push_back(pointsLeft[i].z);
	}
	pointerPoint = new Point(aa::vec3(0.0f));
	pointerPoint->TranslateAndConfirm(point1);
	allPoints.push_back(ShapeTable::AddShape(pointerPoint));
	vertices.push_back(point1.x);
	vertices.push_back(point1.y);
	vertices.push_back(point1.z);
	for (size_t i = 0; i < pointsRight.size() ; i++)
	{
		pointerPoint = new Point(aa::vec3(0.0f));
		pointerPoint->TranslateAndConfirm(pointsRight[i]);
		allPoints.push_back(ShapeTable::AddShape(pointerPoint));
		vertices.push_back(pointsRight[i].x);
		vertices.push_back(pointsRight[i].y);
		vertices.push_back(pointsRight[i].z);
	}

	if (curve != nullptr)
		delete curve;
	curve = new InterpolatingCurve(allPoints);
	curve->setName("__INTERSECTION__");

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void Intersection::PrintImGuiOptions()
{
	ImGui::Checkbox("Display intersection points", &displayPoints);
}

void Intersection::Scale(aa::vec3 s, aa::vec3 origin)
{
	;
}

void Intersection::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	;
}

void Intersection::Translate(aa::vec3 t)
{
	;
}

void Intersection::ConfirmTransformations()
{
	;
}

void Intersection::CancelTransformations()
{
	;
}

void Intersection::Serialize(nlohmann::json& j)
{
	j = NULL;
}

void Intersection::Draw()
{
	if (dirty)
		Mesh();
	if (displayPoints)
	{
		shader.use();
		glBindVertexArray(VAO);
		shader.setMat4("model", aa::mat4(1.0f));
		glPointSize(10.0f);
		shader.setVec3("color", aa::vec3(0.0f, 1.0f, 0.0f));
		glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
		glBindVertexArray(0);
	}
	curve->setTessellationShader(tessellationShader);
	curve->setShader(shader);
	curve->Select(true);
	if (selected)
		curve->Select();
	curve->Draw();
}

void Intersection::setTessellationShader(Shader& _shader)
{
	tessellationShader = _shader;
}

float Intersection::MeasureDistance2(TwoSurfacesState state)
{
	if (firstSurface == secondSurface)
	{
		if (distance_uv(state.u1, state.v1, state.u2, state.v2) < 0.01f)
			return std::numeric_limits<float>::max();
	}
	aa::vec3 point1 = state.first->Evaluate(state.u1, state.v1);
	aa::vec3 point2 = state.second->Evaluate(state.u2, state.v2);
	return aa::distance_2(point1, point2);
}

void swap(float& u1, float& v1, float& u2, float& v2)
{
	float temp = u1;
	u1 = u2;
	u2 = temp;
	temp = v1;
	v1 = v2;
	v2 = temp;
}

TwoSurfacesState Intersection::RunMonteCarlo(TwoSurfacesState state, float width)
{
	// The range we will be looking in:
	float u1min = std::max(state.u1 - width, 0.0f);
	float v1min = std::max(state.v1 - width, 0.0f);
	float u2min = std::max(state.u2 - width, 0.0f);
	float v2min = std::max(state.v2 - width, 0.0f);
	float u1max = std::min(state.u1 + width, 1.0f);
	float v1max = std::min(state.v1 + width, 1.0f);
	float u2max = std::min(state.u2 + width, 1.0f);
	float v2max = std::min(state.v2 + width, 1.0f);
	float u1range = u1max - u1min;
	float v1range = v1max - v1min;
	float u2range = u2max - u2min;
	float v2range = v2max - v2min;
	TwoSurfacesState bestGuess = state;
	TwoSurfacesState newGuess = bestGuess;
	Cursor cursor = Cursor::getInstance();
	float lambda = 0.4f;
	float minDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < 500; i++)
	{
		newGuess.u1 = static_cast <float>(rand()) / static_cast <float>(RAND_MAX) * u1range + u1min;
		newGuess.v1 = static_cast <float>(rand()) / static_cast <float>(RAND_MAX) * v1range + v1min;
		newGuess.u2 = static_cast <float>(rand()) / static_cast <float>(RAND_MAX) * u2range + u2min;
		newGuess.v2 = static_cast <float>(rand()) / static_cast <float>(RAND_MAX) * v2range + v2min;
		if (firstSurface == secondSurface)
		{
			if (newGuess.u1 > newGuess.u2 || (newGuess.u1 == newGuess.u2 && newGuess.v1 > newGuess.v2))
				swap(newGuess.u1, newGuess.v1, newGuess.u2, newGuess.v2);
		}
		float newDistance = MeasureDistance2(newGuess);
		// Include the cursor heuristic
		//newDistance += lambda * aa::distance_2((firstSurface->Evaluate(newGuess.u1, newGuess.v1) + firstSurface->Evaluate(newGuess.u1, newGuess.v1)) / 2.0f, cursor.getPosition());
		if (newDistance < minDistance)
		{
			minDistance = newDistance;
			bestGuess = newGuess;
		}
	}
	return bestGuess;
}

ParamDirection Intersection::ComputeTangent(const TwoSurfacesState& state, bool reverse)
{
	aa::vec3 Su = state.first->Du(state.u1, state.v1);
	aa::vec3 Sv = state.first->Dv(state.u1, state.v1);

	aa::vec3 Ts = state.second->Du(state.u2, state.v2);
	aa::vec3 Tt = state.second->Dv(state.u2, state.v2);

	aa::vec3 N1 = aa::cross(Su, Sv);
	aa::vec3 N2 = aa::cross(Ts, Tt);

	aa::vec3 curveTangent = cross(N1, N2);
	if (reverse || aa::dot(curveTangent, previousCurveTangent) < 0)
		curveTangent = -curveTangent;
	previousCurveTangent = curveTangent;

	auto firstDir =
		SolveSurfaceTangent(Su, Sv, curveTangent);

	auto secondDir =
		SolveSurfaceTangent(Ts, Tt, curveTangent);
	
	ParamDirection dir;
	dir.du1 = firstDir.du;
	dir.dv1 = firstDir.dv;
	dir.du2 = secondDir.du;
	dir.dv2 = secondDir.dv;
	return dir;
}

ParamDirection2D Intersection::SolveSurfaceTangent(const aa::vec3& Su, const aa::vec3& Sv, const aa::vec3& curveTangent)
{
	float a11 = aa::dot(Su, Su);
	float a12 = aa::dot(Su, Sv);
	float a22 = aa::dot(Sv, Sv);

	float b1 = aa::dot(Su, curveTangent);
	float b2 = aa::dot(Sv, curveTangent);

	float det = a11 * a22 - a12 * a12;

	ParamDirection2D result{};

	if (std::abs(det) < 1e-8f)
	{
		// Surface derivatives are nearly linearly dependent.
		result.du = 0.0f;
		result.dv = 0.0f;
		return result;
	}

	result.du = (b1 * a22 - b2 * a12) / det;
	result.dv = (a11 * b2 - a12 * b1) / det;

	return result;
}

TwoSurfacesState Intersection::NewtonCorrect(TwoSurfacesState state, const ParamDirection& dir)
{
	float u1 = state.u1;
	float v1 = state.v1;
	float u2 = state.u2;
	float v2 = state.v2;
	for (int iter = 0; iter < 6; iter++)
	{
		aa::vec3 P1 = firstSurface->Evaluate(u1, v1);
		aa::vec3 P2 = secondSurface->Evaluate(u2, v2);

		aa::vec3 F = P1 - P2;

		aa::vec3 Su = firstSurface->Du(u1, v1);
		aa::vec3 Sv = firstSurface->Dv(u1, v1);

		aa::vec3 Tu = secondSurface->Du(u2, v2);
		aa::vec3 Tv = secondSurface->Dv(u2, v2);

		float du1, dv1, du2, dv2;

		if (!SolveGaussNewtonStep(Su, Sv, Tu, Tv, F,
			du1, dv1, du2, dv2))
			break;
		// line search / damping to improve convergence and avoid large steps
		float prevResidual = aa::length(F);
		float alpha = 1.0f;
		float newResidual = prevResidual;
		float bestU1 = u1, bestV1 = v1, bestU2 = u2, bestV2 = v2;
		for (int ls = 0; ls < 6; ++ls)
		{
			float candU1 = aa::clip(u1 + alpha * du1, 0.0f, 1.0f);
			float candV1 = aa::clip(v1 + alpha * dv1, 0.0f, 1.0f);
			float candU2 = aa::clip(u2 + alpha * du2, 0.0f, 1.0f);
			float candV2 = aa::clip(v2 + alpha * dv2, 0.0f, 1.0f);

			aa::vec3 P1c = firstSurface->Evaluate(candU1, candV1);
			aa::vec3 P2c = secondSurface->Evaluate(candU2, candV2);
			float res = aa::length(P1c - P2c);
			if (res < newResidual)
			{
				newResidual = res;
				bestU1 = candU1; bestV1 = candV1; bestU2 = candU2; bestV2 = candV2;
				// accept and try larger alpha (not increasing here), break to apply
				break;
			}
			alpha *= 0.5f;
		}

		// apply best candidate
		u1 = bestU1;
		v1 = bestV1;
		u2 = bestU2;
		v2 = bestV2;

		if (newResidual < 1e-5f)
			break;
	}

	TwoSurfacesState out = state;
	out.u1 = u1;
	out.v1 = v1;
	out.u2 = u2;
	out.v2 = v2;
	return out;
}

bool Intersection::SolveGaussNewtonStep(
	const aa::vec3& Su, const aa::vec3& Sv,
	const aa::vec3& Tu, const aa::vec3& Tv,
	const aa::vec3& F,
	float& du1, float& dv1,
	float& du2, float& dv2)
{
	// J columns
	aa::vec3 J1 = Su;
	aa::vec3 J2 = Sv;
	aa::vec3 J3 = -1.0f * Tu;
	aa::vec3 J4 = -1.0f * Tv;

	// JT*F
	float b1 = aa::dot(J1, F);
	float b2 = aa::dot(J2, F);
	float b3 = aa::dot(J3, F);
	float b4 = aa::dot(J4, F);

	// J^T J matrix
	float A[4][4];

	aa::vec3 cols[4] = { J1, J2, J3, J4 };

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			A[i][j] = aa::dot(cols[i], cols[j]);

	float B[4] = { -b1, -b2, -b3, -b4 };

	// Gaussian elimination (4x4)
	float M[4][5];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			M[i][j] = A[i][j];
		M[i][4] = B[i];
	}

	for (int i = 0; i < 4; i++)
	{
		float pivot = M[i][i];
		if (fabs(pivot) < 1e-8f) return false;

		for (int j = i; j < 5; j++)
			M[i][j] /= pivot;

		for (int k = 0; k < 4; k++)
		{
			if (k == i) continue;
			float f = M[k][i];
			for (int j = i; j < 5; j++)
				M[k][j] -= f * M[i][j];
		}
	}

	du1 = M[0][4];
	dv1 = M[1][4];
	du2 = M[2][4];
	dv2 = M[3][4];

	return true;
}

std::vector<aa::vec3> Intersection::GetThePointsInOneDirection(TwoSurfacesState bestGuess, aa::vec3 previousPoint, bool reverse)
{
	std::vector<aa::vec3> followingPoints;
	float h = 0.1f;
	for (size_t i = 0; i < 100; i++)
	{
		ParamDirection param = ComputeTangent(bestGuess, reverse);
		reverse = false;

		TwoSurfacesState predicted = bestGuess;
		predicted.u1 += param.du1 * h;
		predicted.v1 += param.dv1 * h;
		predicted.u2 += param.du2 * h;
		predicted.v2 += param.dv2 * h;

		// Newton correction
		bestGuess = NewtonCorrect(predicted, param);

		aa::vec3 point1 = firstSurface->Evaluate(bestGuess.u1, bestGuess.v1);
		followingPoints.push_back(point1);

		// Check if we left one of the surfaces

		if (bestGuess.u1 > 1.0f || bestGuess.u1 < 0.0f ||
			bestGuess.v1 > 1.0f || bestGuess.v1 < 0.0f ||
			bestGuess.u2 > 1.0f || bestGuess.u2 < 0.0f ||
			bestGuess.v2 > 1.0f || bestGuess.v2 < 0.0f)
		{
			break;
		}

		// Correct h factor for next iteration:
		//h *= 0.1f / aa::distance(previousPoint, point1);

		//point2 = secondSurface->Evaluate(bestGuess.u2, bestGuess.v2);
		//vertices.push_back(point2.x);
		//vertices.push_back(point2.y);
		//vertices.push_back(point2.z);
	}
	return followingPoints;
}

float Intersection::distance_uv(float u1, float v1, float u2, float v2)
{
	return (u1 - u2) * (u1 - u2) + (v1 - v2) * (v1 - v2);
}

// Grid class functions
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
Grid& Grid::getInstance()
{
	static Grid instance;
	return instance;
}
void Grid::Draw(Camera &camera, char eye)
{
	glBindVertexArray(VAO);
	gridShader.use();
	gridShader.setMat4("view", camera.view());
	aa::mat4 projection;
	projection = camera.projection();
	gridShader.setMat4("projection", projection);
	gridShader.setMat4("model", aa::scale(aa::mat4(1.0f),aa::vec3(40.0f)));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
// Cursor class functions
Cursor& Cursor::getInstance()
{
	static Cursor instance(false);
	return instance;
}
Cursor& Cursor::centerOfGravityIndicator()
{
	static Cursor instance(true);
	return instance;
}
void Cursor::Draw()
{
	model[3][0] = location[0];
	model[3][1] = location[1];
	model[3][2] = location[2];
	glBindVertexArray(VAO);
	shader.use();
	shader.setMat4("model", model);
	//shader.setVec3("color", aa::vec3(1.0f, 0.0f, 1.0f));
	shader.setMat4("scene", aa::mat4(1.0f));
	//glLineWidth(5);
	glLineWidth((selected ? 6.0f : 3.0f)); //alter Cursor width based on selection
	aa::vec3 color = aa::vec3(1.0f, 0.0f, 1.0f);
	if (selected)
		color = aa::vec3(1.0f, 0.8f, 0.6f);
	if (isCenterOfGravity)
		color = aa::vec3(1.0f, 0.0f, 0.0f);
	shader.setVec3("color", color);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void Cursor::Draw(aa::vec3 position)
{
	locationBackup = location = position;
	Draw();
}
Cursor::Cursor(bool _isCenterOfGravity)
{
	isCenterOfGravity = _isCenterOfGravity;
	shapeName = "3D Cursor";
	float size = 0.1f;
	if (isCenterOfGravity)
		size = 0.04;
	model = aa::scale(aa::vec3(size, size, size));
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
void Cursor::UpdatePosition(Camera& camera, double xpos, double ypos, LockAxis lockedAxis)
{
	float x_ndc = (2.0f * xpos) / camera.windowWidth - 1.0f;
	float y_ndc = 1.0f - (2.0f * ypos) / camera.windowHeight;

	aa::vec4 ray_clip = aa::vec4(x_ndc, y_ndc, -1.0f, 1.0f);

	aa::mat4 invVP = camera.inverseViewProjection();

	// Unproject to world space
	aa::vec4 worldPos = invVP * ray_clip;

	// Perspective divide
	worldPos = worldPos / worldPos.w;

	// Compute ray direction
	aa::vec3 rayDir = aa::normalize(aa::vec3(worldPos.xyz) - camera.cameraPos);

	// Place cursor some fixed distance in front of camera
	float distance = aa::distance(camera.cameraPos, location);
	if (lockedAxis==X)
		locationBackup.x = location.x = (camera.cameraPos + rayDir * distance).x;
	else if (lockedAxis==Y)
		locationBackup.y = location.y = (camera.cameraPos + rayDir * distance).y;
	else if (lockedAxis==Z)
		locationBackup.z = location.z = (camera.cameraPos + rayDir * distance).z;
	else
		locationBackup = location = camera.cameraPos + rayDir * distance;
}
aa::vec3 Cursor::getPosition()
{
	return location;
}
void Cursor::Serialize(nlohmann::json& j)
{
	j = NULL; // do nothing, Cursor is not serialized
}
void Cursor::PrintImGuiOptions()
{
	if (ImGui::DragFloat3("Cursor position", aa::value_ptr(location), 0.01f, -20.0f, 20.0f, "%.3f"))
	{
		if (aa::length(location) > 40.0f)
		{
			location = aa::vec3(0.0f, 0.0f, 0.0f);
		}
		locationBackup = location;
	}
}

void Cursor::Translate(aa::vec3 t)
{
	location = locationBackup + t;
}

void Cursor::ConfirmTransformations()
{
	locationBackup = location;
}
void Cursor::CancelTransformations()
{
	location = locationBackup;
}

// Axis class functions
Axis::Axis()
{
	VAO = 0;
	VBO = 0;
}
Axis::Axis(LockAxis _axis)
{
	lockedAxis = _axis;
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
void Axis::SetAxis(LockAxis _axis)
{
	lockedAxis = _axis;
}
void Axis::Draw(Shader& shader, aa::vec3 translationOrigin, char eye)
{
	aa::mat4 model1;
	aa::mat4 model2;
	aa::vec3 color1;
	aa::vec3 color2;
	switch (lockedAxis)
	{
	case X:
		model1 = modelX;
		color1 = colorX;
		break;
	case Y:
		model1 = modelY;
		color1 = colorY;
		break;
	case Z:
		model1 = modelZ;
		color1 = colorZ;
		break;
	case notX:
		model1 = modelY;
		color1 = colorY;
		model2 = modelZ;
		color2 = colorZ;
		break;
	case notY:
		model1 = modelX;
		color1 = colorX;
		model2 = modelZ;
		color2 = colorZ;
		break;
	case notZ:
		model1 = modelX;
		color1 = colorX;
		model2 = modelY;
		color2 = colorY;
		break;
	}
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", aa::translate(model1, translationOrigin));
	shader.setVec3("color", color1);
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 2);
	if (lockedAxis == notX || lockedAxis == notY || lockedAxis == notZ)
	{
		shader.setMat4("model", aa::translate(model2, translationOrigin));
		shader.setVec3("color", color2);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glBindVertexArray(0);
}

BoxSelect::BoxSelect()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	int vertices[] = {
		0, 1, 2, 3
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int indices[] = {
		0, 1, 1, 3, 2, 3, 2, 0
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_INT, 0, (void*)0);

	glBindVertexArray(0);
}
BoxSelect& BoxSelect::getInstance()
{
	static BoxSelect instance;
	return instance;
}
void BoxSelect::Draw(aa::vec2 tl, aa::vec2 br)
{
	glBindVertexArray(VAO);
	glDisable(GL_DEPTH_TEST);
	boxSelectShader.use();
	boxSelectShader.setVec3("color", aa::vec3(1.0f, 1.0f, 0.0f));
	boxSelectShader.setVec2("corner[0]", tl);
	boxSelectShader.setVec2("corner[1]", br);
	glPointSize(100);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
