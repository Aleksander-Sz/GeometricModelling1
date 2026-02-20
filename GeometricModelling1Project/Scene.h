#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"

class Scene
{
public:
	Scene(int windowWidth, int windowHeight);
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = 400, lastY = 300;
	bool firstMovement = true;
	bool Lpressed = false;
	bool AltPressed = false;
	bool mouseLeftButtonPressed = false;
	bool shiftPressed = false;
	bool cursorLocked = false;
	double mouseLeftPressTime;
	double AltPressTime;
	glm::vec2 mouseLeftPressPosition;
	Camera camera;
	Cursor cursor = Cursor::getInstance();
	void UpdateCursorPosition(double xpos, double ypos);
	std::vector<Shape*> shapes;
	void LeftMouseClick();
	void DrawCursorOverlay();
};

#endif