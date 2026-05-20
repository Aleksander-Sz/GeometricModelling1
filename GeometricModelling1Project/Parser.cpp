#include "Parser.h"

void Parser::SaveScene(const char* filename, std::vector<int>)
{
	;
}

std::vector<int> Parser::LoadScene(const char* filename)
{
	std::vector<int> shapes;
	std::unordered_map<uint32_t, int> points; // mapping point IDs from the file to their IDs in the ShapeTable

	nlohmann::json j;
	try
	{
		std::ifstream file(filename);
		if (!file.is_open())
			throw(std::runtime_error("Could not open file"));
		file >> j;
		file.close();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error loading scene: " << e.what() << std::endl;
		return shapes;
	}

	// Parsing points
	for (auto& p : j["points"])
	{
		uint32_t id = p["id"];
		aa::vec3 pos(
			p["position"]["x"],
			p["position"]["y"],
			p["position"]["z"]
		);

		points[id] = ShapeTable::AddShape(new Point(pos));
		shapes.push_back(points[id]);
	}

	//Parsing geometry
	for (auto& g : j["geometry"])
	{
		std::string type = g["objectType"];

		if (type == "torus")
		{
			Torus* newTorus = new Torus(g["largeRadius"], g["smallRadius"], g["samples"]["u"], g["samples"]["v"]);
			aa::vec3 pos(
				g["position"]["x"],
				g["position"]["y"],
				g["position"]["z"]
			);
			newTorus->Rotate(aa::radians(90.0f), aa::Axis::X);
			newTorus->ConfirmTransformations();
			newTorus->TranslateAndConfirm(pos);
			shapes.push_back(ShapeTable::AddShape(newTorus));
		}
		else if ((type == "chain") || (type == "bezierC0") || (type == "bezierC2") || (type == "interpolatedC2"))
		{
			std::vector<int> linePoints;
			for (auto& pointID : g["controlPoints"])
			{
				if (points.find(pointID["id"]) != points.end())
				{
					linePoints.push_back(points[pointID["id"]]);
				}
				else
				{
					std::cerr << "Error loading line: Point ID " << pointID["id"] << " not found.\n";
				}
			}
			Line* newLine;
			if (type == "chain")
				newLine = new Line(linePoints);
			else if (type == "bezierC0")
				newLine = new BezierCurveC0(linePoints);
			else if (type == "bezierC2")
				newLine = new BezierCurveC2(linePoints);
			else // type == "interpolatedC2"
				newLine = new InterpolatingCurve(linePoints);
			shapes.push_back(ShapeTable::AddShape(newLine));
		}
		else if (type == "bezierSurfaceC0" || type == "bezierSurfaceC2")
		{
			std::vector<std::vector<int>> controlPoints;

			int u = g["size"]["u"];
			int v = g["size"]["v"];

			controlPoints.resize(v);

			auto& cps = g["controlPoints"];

			if (cps.size() != static_cast<size_t>(u * v))
			{
				std::cerr << "Error loading surface: Invalid control point count.\n";
				return shapes;
			}

			for (int j = 0; j < v; j++)
			{
				controlPoints[j].resize(u);

				for (int i = 0; i < u; i++)
				{
					int index = j * u + i;

					int pointID = cps[index]["id"];

					if (points.find(pointID) != points.end())
					{
						controlPoints[j][i] = points[pointID];
					}
					else
					{
						std::cerr << "Error loading surface: Point ID "
							<< pointID
							<< " not found.\n";
					}
				}
			}
			bool isC2 = (type == "bezierSurfaceC2");
			bool isCylinder = false;
			int subdivisionsU = g["samples"]["u"];
			int subdivisionsV = g["samples"]["v"];
			BezierSurface* newSurface = new BezierSurface(controlPoints, isC2, isCylinder, subdivisionsU, subdivisionsV);
			shapes.push_back(ShapeTable::AddShape(newSurface));
		}
		else
		{
			std::cerr << "Unknown object type: " << type << "\n";
		}
	}

	return shapes;
}
