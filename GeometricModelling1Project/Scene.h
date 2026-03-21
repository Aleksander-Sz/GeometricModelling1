#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"
#include <GLFW/glfw3.h>

class Scene
{
public:
	Scene(int windowWidth, int windowHeight, Shader shader);
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = 400, lastY = 300;
	bool firstMovement = true;
	bool cPressed = false;
	bool lPressed = false;
	bool gPressed = false;
	bool xPressed = false;
	bool yPressed = false;
	bool zPressed = false;
	bool AltPressed = false;
	bool EscPressed = false;
	bool mouseLeftButtonPressed = false;
	bool shiftPressed = false;
	bool cursorLocked = true;
	bool grabEnabled = false;
	aa::vec2 grabMouseOrigin;
	bool xLocked = false;
	bool yLocked = false;
	bool zLocked = false;
	void LockXAxis();
	void LockYAxis();
	void LockZAxis();
	void toggleGrab();
	double mouseLeftPressTime;
	double AltPressTime;
	aa::vec2 mouseLeftPressPosition;
	Camera camera;
	Cursor cursor = Cursor::getInstance();
	void UpdateCursorPosition(double xpos, double ypos);
	std::vector<Shape*> shapes;
	void LeftMouseClick();
	void DrawCursorOverlay();
	void MoveSelectedObjects(aa::vec3 translation);
	aa::vec3 unlockedTranslationBackup = aa::vec3(0.0f, 0.0f, 0.0f);
	Shape* selectedShape = NULL;
	void ConfirmObjectMovement(); // TODO add floating axis during locked movement, save the movement for use after axis change
	void CancellObjectMovement();
	void DeselectEverything();
	void DeleteSelectedObjects();
	aa::vec3 currentTranslationOrigin = aa::vec3(0.0f);
	Axis movementAxis;
	Shader shader;
	void DrawScene(GLFWwindow* window);
	Grid grid = Grid::getInstance();
	bool stereoscopy = false;
	aa::vec3 sceneScale = aa::vec3(1.0f, 1.0f, 1.0f);
private:
	aa::mat4 sceneMatrix = aa::mat4(1.0f);
	aa::mat4 inverseSceneMatrix = aa::mat4(1.0f);
};

#endif