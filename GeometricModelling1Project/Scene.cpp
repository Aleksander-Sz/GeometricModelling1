#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight)
{
	camera = Camera(windowWidth, windowHeight);
}
void Scene::UpdateCursorPosition(double xpos, double ypos)
{
	cursor.UpdatePosition(camera, xpos, ypos);
}
void Scene::LeftMouseClick()
{
	cursor.Click(shapes);
}