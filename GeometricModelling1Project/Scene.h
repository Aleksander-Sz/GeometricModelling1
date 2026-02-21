#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"

class Scene
{
public:
	Scene(int windowWidth, int windowHeight, Shader shader);
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = 400, lastY = 300;
	bool firstMovement = true;
	bool lPressed = false;
	bool gPressed = false;
	bool xPressed = false;
	bool yPressed = false;
	bool zPressed = false;
	bool AltPressed = false;
	bool EscPressed = false;
	bool mouseLeftButtonPressed = false;
	bool shiftPressed = false;
	bool cursorLocked = false;
	bool grabEnabled = false;
	bool xLocked = false;
	bool yLocked = false;
	bool zLocked = false;
	void LockXAxis();
	void LockYAxis();
	void LockZAxis();
	void toggleGrab();
	double mouseLeftPressTime;
	double AltPressTime;
	glm::vec2 mouseLeftPressPosition;
	Camera camera;
	Cursor cursor = Cursor::getInstance();
	void UpdateCursorPosition(double xpos, double ypos);
	std::vector<Shape*> shapes;
	void LeftMouseClick();
	void DrawCursorOverlay();
	void MoveSelectedObjects(glm::vec3 translation);
	Shape* selectedShape = NULL;
	void ConfirmObjectMovement(); // TODO add floating axis during locked movement, save the movement for use after axis change
	void CancellObjectMovement();
	void DeselectEverything();
	glm::vec3 currentTranslationOrigin = glm::vec3(0.0f);
	Axis movementAxis;
	Shader shader;
	void DrawScene();
};

#endif