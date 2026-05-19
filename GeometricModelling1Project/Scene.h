#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"
#include <GLFW/glfw3.h>
#include <algorithm>

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
	bool fPressed = false;
	bool rPressed = false;
	bool mPressed = false;
	bool xPressed = false;
	bool yPressed = false;
	bool zPressed = false;
	bool AltPressed = false;
	bool CtrlPressed = false;
	bool EscPressed = false;
	bool mouseLeftButtonPressed = false;
	bool mouseRightButtonPressed = false;
	bool shiftPressed = false;
	bool cursorLocked = true;
	bool grabEnabled = false;
	bool scalingEnabled = false;
	bool rotatingEnabled = false;
	bool transformAroundCursor = false;
	aa::vec2 grabMouseOrigin;
	LockAxis lockedAxis;
	void LockXAxis();
	void LockYAxis();
	void LockZAxis();
	void LockXYAxis();
	void LockXZAxis();
	void LockYZAxis();
	void toggleGrab();
	void toggleScaling();
	void toggleRotating();
	double mouseLeftPressTime;
	double mouseRightPressTime;
	double AltPressTime;
	aa::vec2 mouseLeftPressPosition;
	Camera camera;
	Cursor cursor = Cursor::getInstance();
	void UpdateCursorPosition(double xpos, double ypos);
	//std::vector<Shape*> shapes;
	std::vector<int> shapes;
	void LeftMouseClick();
	void DrawCursorOverlay();
	void MoveSelectedObjects(aa::vec3 translation);
	void ScaleSelectedObjects(float factor);
	void RotateSelectedObjects(float angle, aa::Axis axis);
	aa::vec3 unlockedTranslationBackup = aa::vec3(0.0f, 0.0f, 0.0f);
	int selectedShape = -1; // storing the index
	void ConfirmObjectMovement();
	void CancellObjectMovement();
	void DeselectEverything(bool includingVirtualPoints = true);
	void DeleteSelectedObjects();
	void MergeSelectedPoints();
	void StartBoxSelect(aa::vec2 location);
	void EndBoxSelect(aa::vec2 location);
	bool boxSelectActive = false;
	aa::vec3 currentTranslationOrigin = aa::vec3(0.0f);
	aa::vec2 getTransformationCenterScreenSpacePosition();
	Axis movementAxis = Axis(X);
	Shader shader;
	Shader tessellationShader = Shader("Shaders/BezierVertexShader.glsl", "Shaders/BezierTessellationControlShader.glsl", "Shaders/BezierTessellationEvaluationShader.glsl", "Shaders/FragmentShader.glsl");
	Shader surfaceTessellationShader = Shader("Shaders/BezierVertexShader.glsl", "Shaders/SurfaceTessellationControlShader.glsl", "Shaders/SurfaceTessellationEvaluationShader.glsl", "Shaders/SurfaceFragmentShader.glsl");
	void DrawScene(GLFWwindow* window);
	Grid grid = Grid::getInstance();
	bool stereoscopy = false;
	aa::vec3 sceneScale = aa::vec3(1.0f, 1.0f, 1.0f);
	aa::vec2 boxSelectOrigin;
	void RemoveMarkedObjects();
	int currentItemSelectedForAdding = 0;
	void AddShape();
	char typeOfShapeCurrentlySelected = 0;
	float distanceBetweenEyes = 0.1f;
	float convergenceDistance = 5.0f;
private:
	aa::mat4 sceneMatrix = aa::mat4(1.0f);
	aa::mat4 inverseSceneMatrix = aa::mat4(1.0f);
	BoxSelect boxSelect = BoxSelect::getInstance();
	Cursor centerOfGravityIndicator = Cursor::centerOfGravityIndicator();
	int numberOfSelectedShapes = 0;
};

#endif