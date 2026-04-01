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
	if (thisBC)
	{
		// This is a BezierCurve
		if (thisBC->displayControlPolyline)
		{
			glLineWidth(0.5f);
			shader.setVec3("color", aa::vec3(0.8f, 0.8f, 0.8f));
			glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);
		}
		thisBC->tessellationShader.use();
		glBindVertexArray(VAO);
		thisBC->tessellationShader.setMat4("model", model);
		glLineWidth((selected ? 5.0f : 1.0f)); //alter line width based on selection
		thisBC->tessellationShader.setVec3("color", (selected ? aa::vec3(1.0f, 1.0f, 0.6f) : aa::vec3(1.0f, 1.0f, 1.0f)));
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		
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
	model = aa::translate(model, coords);
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
	ImGui::Text("This is a point, it has no mesh options.");
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
void Point::InvalidateDependentShapes()
{
	for (int i = 0; i < dependentShapes.size(); i++)
	{
		dependentShapes[i]->dirty = true;
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

Line::Line(std::vector<Point*> _points)
{
	points = _points;
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->dependentShapes.push_back(this);
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
		for (int j = 0; j < points[i]->dependentShapes.size(); j++)
		{
			if (points[i]->dependentShapes[j] == this)
				points[i]->dependentShapes.erase(points[i]->dependentShapes.begin() + j);
		}
	}
}
void Line::Mesh()
{
	dirty = false;
	vertices.clear();
	indices.clear();
	if (points.size() < 2)
		return; // Nothing to mesh
	for (int i = 0; i < points.size(); i++)
	{
		aa::vec3 pointLocation = points[i]->getPosition();
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
			if (ImGui::Selectable((points[i]->Name()).c_str(), points[i]->isSelected()))
			{
				points[i]->Select();
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
		points[i]->Scale(s, origin);
	}
}
void Line::Rotate(float angle, aa::Axis axis, aa::vec3 pivot)
{
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->Rotate(angle, axis, pivot);
	}
}
void Line::Translate(aa::vec3 t)
{
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->Translate(t);
	}
}
void Line::ConfirmTransformations()
{
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->ConfirmTransformations();
	}
}
void Line::CancelTransformations()
{
	for (int i = 0; i < points.size(); i++)
	{
		points[i]->CancelTransformations();
	}
}
aa::vec3 Line::getPosition()
{
	aa::vec3 averagePosition(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < points.size(); i++)
	{
		averagePosition += points[i]->getPosition();
	}
	averagePosition /= (float)points.size();
	return averagePosition;
}
void Line::AddPoint(Point* point)
{
	if (point == nullptr)
		return;
	points.push_back(point);
	point->dependentShapes.push_back(this);
	dirty = true;
}
void Line::RemoveDeletedPoints()
{
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i]->isMarkedForDeletion())
		{
			points.erase(points.begin() + i);
			i--;
		}
	}
	dirty = true;
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
	aa::vec3 pointLocation = points[0]->getPosition();
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
			aa::vec3 pointLocation = points[pointIndex]->getPosition();
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
	std::cout << "Number of segments: " << numberOfSegments << "\n";
	aa::vec3 previous1;
	aa::vec3 previous2;
	// First segment of the curve:
	for (int i = 0; i < 4; i++)
	{
		int pointIndex = i;
		if (pointIndex >= points.size())
			pointIndex = points.size() - 1;
		aa::vec3 pointLocation = points[pointIndex]->getPosition();
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
		// calculating the first point, to maintaing first derivative continuity
	
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
			aa::vec3 pointLocation = points[pointIndex]->getPosition();
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
void Cursor::UpdatePosition(Camera& camera, double xpos, double ypos, bool xLocked, bool yLocked, bool zLocked)
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
	if (xLocked)
		locationBackup.x = location.x = (camera.cameraPos + rayDir * distance).x;
	else if (yLocked)
		locationBackup.y = location.y = (camera.cameraPos + rayDir * distance).y;
	else if (zLocked)
		locationBackup.z = location.z = (camera.cameraPos + rayDir * distance).z;
	else
		locationBackup = location = camera.cameraPos + rayDir * distance;
}
aa::vec3 Cursor::getPosition()
{
	return location;
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
	model = aa::mat4(1.0f);
	color = aa::vec3(0.0f);
	VAO = 0;
	VBO = 0;
}
Axis::Axis(char _axis)
{
	model = aa::mat4::identity();
	switch (_axis)
	{
	case 'x':
		color = aa::vec3(0.8f, 0.1f, 0.1f);
		break;
	case 'y':
		model = aa::rotate(model, aa::Axis::Z, aa::radians(90.0f));
		color = aa::vec3(0.1f, 0.8f, 0.1f);
		break;
	case 'z':
		model = aa::rotate(model, aa::Axis::Y, aa::radians(90.0f));
		color = aa::vec3(0.1f, 0.1f, 0.8f);
		break;
	default:
		std::cerr << "Invalid axis specified, defaulting to x-axis.\n";
		break;
	}
	model = aa::scale(model, aa::vec3(100.0f, 100.0f, 100.0f));
	this->SetAxis(model, color);
}
void Axis::SetAxis(aa::mat4 _model, aa::vec3 _color)
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
void Axis::Draw(Shader& shader, aa::vec3 translationOrigin, char eye)
{
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", aa::translate(model, translationOrigin));
	shader.setVec3("color", color);
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 2);
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