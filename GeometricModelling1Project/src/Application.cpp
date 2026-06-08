#define GLAD_GL_IMPLEMENTATION
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Shader.h"
#include "../Camera.h"
#include "../Shapes.h"
#include "../Scene.h"
#include "../UserInterface.h"
#include "../AlexAlgebra.h"


#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Scene* scene;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	scene->camera.windowWidth = width;
	scene->camera.windowHeight = height;
}

void processInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;
	float cameraSpeed(scene->shiftPressed ? 10.0f : 2.5f);
	const float cameraDisplacement = cameraSpeed * scene->deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (!scene->EscPressed)
		{
			if (scene->grabEnabled||scene->scalingEnabled||scene->rotatingEnabled)
			{
				scene->CancellObjectMovement();
				scene->grabEnabled = false;
				scene->scalingEnabled = false;
				scene->rotatingEnabled = false;
				scene->lockedAxis = NONE;
			}
			else
			{
				scene->DeselectEverything();
			}
		}
		scene->EscPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
		scene->EscPressed = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		scene->shiftPressed = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		scene->shiftPressed = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if(scene->camera.orbitingCamera)
			scene->camera.radius -= cameraDisplacement;
		else
			scene->camera.cameraPos += cameraDisplacement * aa::normalize(scene->camera.cameraFront);

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (scene->camera.orbitingCamera)
			scene->camera.radius += cameraDisplacement;
		else
			scene->camera.cameraPos -= cameraDisplacement * aa::normalize(scene->camera.cameraFront);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * aa::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * aa::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * aa::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * aa::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if(!scene->lPressed)
			scene->cursorLocked = !scene->cursorLocked;
		scene->lPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
		scene->lPressed = false;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (!scene->cPressed)
			scene->camera.orbitingCamera = !scene->camera.orbitingCamera;
		scene->cPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
		scene->cPressed = false;
	if (glfwGetKey(window, GRAB_KEY) == GLFW_PRESS)
	{
		if (!scene->gPressed)
			scene->toggleGrab();
		scene->gPressed = true;
	}
	if (glfwGetKey(window, GRAB_KEY) == GLFW_RELEASE)
		scene->gPressed = false;
	if (glfwGetKey(window, SCALE_KEY) == GLFW_PRESS)
	{
		if (!scene->fPressed)
			scene->toggleScaling();
		scene->fPressed = true;
	}
	if (glfwGetKey(window, SCALE_KEY) == GLFW_RELEASE)
		scene->fPressed = false;
	if (glfwGetKey(window, ROTATE_KEY) == GLFW_PRESS)
	{
		if (!scene->rPressed)
			scene->toggleRotating();
		scene->rPressed = true;
	}
	if (glfwGetKey(window, ROTATE_KEY) == GLFW_RELEASE)
		scene->rPressed = false;
	if (glfwGetKey(window, MERGE_POINTS_KEY) == GLFW_PRESS)
	{
		if (!scene->mPressed)
			scene->MergeSelectedPoints();
		scene->mPressed = true;
	}
	if (glfwGetKey(window, MERGE_POINTS_KEY) == GLFW_RELEASE)
		scene->mPressed = false;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		if (!scene->xPressed)
		{
			if (scene->shiftPressed)
				scene->LockYZAxis();
			else
				scene->LockXAxis();
		}
		scene->xPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
		scene->xPressed = false;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		if (!scene->yPressed)
		{
			if (scene->shiftPressed)
				scene->LockXZAxis();
			else
				scene->LockYAxis();
		}
		scene->yPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
		scene->yPressed = false;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		if (!scene->zPressed)
		{
			if (scene->shiftPressed)
				scene->LockXYAxis();
			else
				scene->LockZAxis();
		}
		scene->zPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		scene->zPressed = false;
	if (glfwGetKey(window, CAMERA_ORBIT_KEY) == GLFW_PRESS)
		scene->AltPressed = true;
	if (glfwGetKey(window, CAMERA_ORBIT_KEY) == GLFW_RELEASE)
		scene->AltPressed = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		scene->CtrlPressed = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		scene->CtrlPressed = false;
	if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS)
		scene->DeleteSelectedObjects();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) // Check if ImGui wants to capture the mouse input
		return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		scene->mouseLeftButtonPressed = true;
		scene->firstMovement = true; // Reset first movement flag when the button is pressed
		scene->mouseLeftPressTime = glfwGetTime();
		scene->mouseLeftPressPosition = aa::vec2(scene->lastX, scene->lastY);

		// Box select
		if (scene->CtrlPressed)
		{
			if (!scene->boxSelectActive)
			{
				scene->StartBoxSelect(aa::vec2(scene->lastX, scene->lastY));
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		scene->mouseLeftButtonPressed = false;
		double mouseReleaseTime = glfwGetTime();
		float movement = aa::length(aa::vec2(scene->lastX, scene->lastY) - scene->mouseLeftPressPosition);
		if (mouseReleaseTime - scene->mouseLeftPressTime <= 0.2 && movement < 5.0f)
		{
			scene->LeftMouseClick();
		}
		if (scene->boxSelectActive)
		{
			scene->EndBoxSelect(aa::vec2(scene->lastX, scene->lastY));
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		scene->mouseRightButtonPressed = true;
		scene->mouseRightPressTime = glfwGetTime();
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		double mouseReleaseTime = glfwGetTime();
		if (mouseReleaseTime - scene->mouseRightPressTime <= 0.2)
		{
			scene->cursor.UpdatePosition(scene->camera, scene->lastX, scene->lastY, NONE);
			scene->AddShape();
		}
	}
}

aa::vec3 project3DPoint(double xpos, double ypos)
{
	// Below is code pasted from an old unused function, keep in mind
	float x_ndc = (2.0f * xpos) / scene->camera.windowWidth - 1.0f;
	float y_ndc = 1.0f - (2.0f * ypos) / scene->camera.windowHeight;

	aa::vec4 ray_clip = aa::vec4(x_ndc, y_ndc, -1.0f, 1.0f);

	aa::mat4 invVP = scene->camera.inverseViewProjection();

	// Unproject to world space
	aa::vec4 worldPos = invVP * ray_clip;

	// Perspective divide
	worldPos = worldPos / worldPos.w;

	// Compute ray direction
	aa::vec3 rayDir = aa::normalize(aa::vec3(worldPos.xyz) - scene->camera.cameraPos);

	// Place cursor some fixed distance in front of camera
	float distance = std::max(aa::distance(scene->camera.cameraPos, scene->currentTranslationOrigin), 1.0f);
	return scene->camera.cameraPos + rayDir * distance;
}

aa::vec3 project3DPointOnAPlane(double xpos, double ypos, aa::vec3 planeNormal)
{
	// Below is code pasted from an old unused function, keep in mind
	float x_ndc = (2.0f * xpos) / scene->camera.windowWidth - 1.0f;
	float y_ndc = 1.0f - (2.0f * ypos) / scene->camera.windowHeight;

	aa::vec4 ray_clip = aa::vec4(x_ndc, y_ndc, -1.0f, 1.0f);

	aa::mat4 invVP = scene->camera.inverseViewProjection();

	// Unproject to world space
	aa::vec4 worldPos = invVP * ray_clip;

	// Perspective divide
	worldPos = worldPos / worldPos.w;

	// Compute ray direction
	aa::vec3 rayDir = aa::normalize(aa::vec3(worldPos.xyz) - scene->camera.cameraPos);

	// Calculate the intersection with the plane
	//aa::vec3 planeNormal = aa::normalize(scene->camera.cameraFront);
	if (aa::dot(rayDir, planeNormal) == 0.0f)
		return aa::vec3(0.0f); // Just in case, this should never happen, if the FoV is smaller than 180 degrees
	float t = aa::dot(scene->currentTranslationOrigin - scene->camera.cameraPos, planeNormal) / aa::dot(rayDir, planeNormal);
	aa::vec3 intersectionPoint = scene->camera.cameraPos + rayDir * t;
	return intersectionPoint;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xOffset = xpos - scene->lastX;
	float yOffset = ypos - scene->lastY;
	if (scene->mouseLeftButtonPressed)
	{
		if (scene->firstMovement)
		{
			scene->firstMovement = false;
			scene->lastX = xpos;
			scene->lastY = ypos;
		}

		scene->lastX = xpos;
		scene->lastY = ypos;



		if (scene->shiftPressed)
		{
			const float sensitivity = 0.003f;
			xOffset *= sensitivity;
			yOffset *= sensitivity;

			aa::vec3 forward = yOffset * aa::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
			aa::vec3 sideways = xOffset * aa::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
			scene->camera.cameraPos += forward;
			scene->camera.cameraPos -= sideways;
			scene->camera.orbitingCameraTarget += forward;
			scene->camera.orbitingCameraTarget -= sideways;
		}
		else if (scene->boxSelectActive)
		{
			; // Do nothing
		}
		else
		{
			const float sensitivity = 0.1f;
			xOffset *= sensitivity;
			yOffset *= sensitivity;

			if (scene->camera.orbitingCamera)
			{
				scene->camera.pitch -= yOffset;
				scene->camera.yaw += xOffset;
			}
			else
			{
				scene->camera.pitch += yOffset;
				scene->camera.yaw -= xOffset;
			}

			if (scene->camera.pitch > 89.0f)
				scene->camera.pitch = 89.0f;
			if (scene->camera.pitch < -89.0f)
				scene->camera.pitch = -89.0f;
		}
	}
	else if (scene->AltPressed)
	{
		if (scene->firstMovement)
		{
			scene->firstMovement = false;
			scene->lastX = xpos;
			scene->lastY = ypos;
		}
		scene->lastX = xpos;
		scene->lastY = ypos;
		const float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		if (scene->camera.orbitingCamera)
		{
			scene->camera.yaw -= xOffset;
			scene->camera.pitch += yOffset;
		}
		else
		{
			scene->camera.yaw += xOffset;
			scene->camera.pitch -= yOffset;
		}

		if (scene->camera.pitch > 89.0f)
			scene->camera.pitch = 89.0f;
		if (scene->camera.pitch < -89.0f)
			scene->camera.pitch = -89.0f;
	}
	else
	{
		if (scene->grabEnabled||scene->scalingEnabled||scene->rotatingEnabled)
		{
			float distance;
			if (scene->typeOfShapeCurrentlySelected == SHAPE_SELECTED)
			{
				distance = aa::length(ShapeTable::GetShapeByID(scene->selectedShape)->getPosition() - scene->camera.cameraPos);
			}
			else
			{
				distance = aa::length(scene->currentTranslationOrigin - scene->camera.cameraPos);
				if (distance < 0.1f)
					distance = 0.1f;
			}

			//float fovRad = aa::radians(scene->camera.zoom); // assuming zoom = FOV
			//float viewHeight = 2.0f * distance * tan(fovRad * 0.5f);

			//float worldPerPixel = viewHeight / scene->camera.windowHeight;

			//aa::vec3 forward = aa::normalize(scene->camera.cameraFront);
			//aa::vec3 right = aa::normalize(aa::cross(forward, scene->camera.cameraUp));
			//aa::vec3 up = aa::normalize(aa::cross(right, forward));

			if (scene->grabEnabled)
			{
				// Let's cast two rays, one from the initial mouse position and one from the current mouse position
				// We will calculate the intersection of those rays with a plane parrallel to the camera's view plane
				// and going through the currentTranslationOrigin
				aa::vec3 planeNormal = aa::normalize(scene->camera.cameraFront);
				if(scene->lockedAxis==notX)
					planeNormal = aa::vec3(1.0f, 0.0f, 0.0f);
				else if (scene->lockedAxis == notY)
					planeNormal = aa::vec3(0.0f, 1.0f, 0.0f);
				else if (scene->lockedAxis == notZ)
					planeNormal = aa::vec3(0.0f, 0.0f, 1.0f);
				else if (scene->lockedAxis == X)
					planeNormal = aa::normalize(aa::cross(scene->camera.cameraUp, aa::vec3(1.0f, 0.0f, 0.0f)));
				else if (scene->lockedAxis == Y)
				{
					// Here there is a chance, that the camera is looking perfectly up or down, in that case the cross product will be zero
					if (aa::dot(scene->camera.cameraUp, aa::vec3(0.0f, 1.0f, 0.0f)) > 0.8f)
					{
						aa::vec3 cameraRight = aa::normalize(aa::cross(scene->camera.cameraFront, scene->camera.cameraUp));
						planeNormal = aa::normalize(aa::cross(cameraRight, aa::vec3(0.0f, 1.0f, 0.0f)));
					}
					else
						planeNormal = aa::normalize(aa::cross(scene->camera.cameraUp, aa::vec3(0.0f, 1.0f, 0.0f)));
				}
				else if (scene->lockedAxis == Z)
					planeNormal = aa::normalize(aa::cross(scene->camera.cameraUp, aa::vec3(0.0f, 0.0f, 1.0f)));
				if (planeNormal.x == 0.0f && planeNormal.y == 0.0f && planeNormal.z == 0.0f)
					planeNormal = aa::normalize(scene->camera.cameraFront);
				aa::vec3 projectedInitialPoint = project3DPointOnAPlane(scene->grabMouseOrigin.x, scene->grabMouseOrigin.y, planeNormal);
				aa::vec3 projectedTranslationPoint = project3DPointOnAPlane(xpos, ypos, planeNormal);
			
				aa::vec3 worldDelta = projectedTranslationPoint - projectedInitialPoint;

				if(aa::length(worldDelta) > 50.0f)
					worldDelta = worldDelta / aa::length(worldDelta) * 50.0f;
				scene->MoveSelectedObjects(worldDelta);
			}
			else if (scene->scalingEnabled)
			{
				aa::vec2 scalingOrigin = scene->getTransformationCenterScreenSpacePosition();
				if (scene->transformAroundCursor)
					scalingOrigin = scene->cursor.getScreenSpacePosition(scene->camera);
				float startDistance = aa::distance(scalingOrigin, scene->grabMouseOrigin);
				float endDistance = aa::distance(scalingOrigin, aa::vec2(xpos, ypos));
				if (startDistance > 5.0f)
				{
					float factor = endDistance / startDistance;
					scene->ScaleSelectedObjects(factor);
				}
			}
			else if (scene->rotatingEnabled)
			{
				aa::vec3 rotationOrigin = scene->currentTranslationOrigin;
				if (scene->transformAroundCursor)
					rotationOrigin = scene->cursor.getPosition();
				aa::vec3 rotation3DStart = project3DPoint(scene->mouseLeftPressPosition.x, scene->mouseLeftPressPosition.y);
				aa::vec3 rotation3DEnd = project3DPoint(xpos, ypos);
				if (scene->lockedAxis==X)
				{
					rotationOrigin.x = 0.0f;
					rotation3DStart.x = 0.0f;
					rotation3DEnd.x = 0.0f;
				}
				else if (scene->lockedAxis==Y)
				{
					rotationOrigin.y = 0.0f;
					rotation3DStart.y = 0.0f;
					rotation3DEnd.y = 0.0f;
				}
				else if (scene->lockedAxis==Z)
				{
					rotationOrigin.z = 0.0f;
					rotation3DStart.z = 0.0f;
					rotation3DEnd.z = 0.0f;
				}
				aa::vec3 firstRotationVector = aa::normalize(rotation3DStart - rotationOrigin);
				aa::vec3 secondRotationVector = aa::normalize(rotation3DEnd - rotationOrigin);
				float angle = std::acos(aa::dot(firstRotationVector, secondRotationVector));
				aa::vec3 crossProduct = aa::normalize(aa::cross(firstRotationVector, secondRotationVector));
				//std::cout << crossProduct.x + crossProduct.y + crossProduct.z << " sign\n";
				angle *= crossProduct.x + crossProduct.y + crossProduct.z;
				if (scene->lockedAxis==X)
				{
					scene->RotateSelectedObjects(angle, aa::Axis::X);
				}
				if (scene->lockedAxis==Y)
				{
					scene->RotateSelectedObjects(angle, aa::Axis::Y);
				}
				if (scene->lockedAxis==Z)
				{
					scene->RotateSelectedObjects(angle, aa::Axis::Z);
				}
			}
		}
	}
	scene->lastX = xpos;
	scene->lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	if (scene->camera.orbitingCamera)
	{
		scene->camera.radius -= (float)yoffset * 0.1f;
	}
	else
	{
		scene->camera.zoom -= (float)yoffset;
		if (scene->camera.zoom < 10.0f)
			scene->camera.zoom = 10.0f;
		if (scene->camera.zoom > 80.0f)
			scene->camera.zoom = 80.0f;
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	int windowWidth = 1600; // Your desired width
	int windowHeight = 900; // Your desired height

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Geometric Modelling 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// 2. Center the window
	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);

	glfwSetWindowPos(
		window,
		monitorX + (mode->width - windowWidth) / 2,
		monitorY + (mode->height - windowHeight) / 2
	);

	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	Shader ourShader("Shaders/VertexShader.glsl","Shaders/FragmentShader.glsl");
	Scene sceneObject = Scene(windowWidth, windowHeight, ourShader);
	scene = &sceneObject;
	
	// For checking max tessellation level:
	int maxLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxLevel);
	std::cout << "Maximum tessellation level: " << maxLevel << "\n";

	// Rendering commands here
	
	//Torus* torus = new Torus(1.0f, 0.3f, 50, 50);
	//torus->setShader(scene->shader);
	//scene->shapes.push_back(ShapeTable::GetShapeID(torus));
	//Point point(aa::vec3(0.0f, 0.1f, 0.0f));
	//scene->shapes.push_back(&point);
	//torus->Rotate(aa::radians(90.0f),aa::Axis::X);
	//torus->ConfirmTransformations();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::cout << glGetString(GL_VERSION) << "\n";
	
	//glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	unsigned int frame = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Menu");
		ImGui::Text("Use WASD to move, mouse to look around, scroll to zoom.");
		ImGui::Separator();
		ImGui::Checkbox("Transform Around the 3D Cursor", &(scene->transformAroundCursor));
		/*if (ImGui::DragFloat3("Scene Scaling", aa::value_ptr(scene->sceneScale), 0.01f, 0.01f, 10.0f))
		{
			if (scene->sceneScale.x < 0.01f)
				scene->sceneScale.x = 0.01f;
			if (scene->sceneScale.x > 10.0f)
				scene->sceneScale.x = 10.0f;
			if (scene->sceneScale.y < 0.01f)
				scene->sceneScale.y = 0.01f;
			if (scene->sceneScale.y > 10.0f)
				scene->sceneScale.y = 10.0f;
			if (scene->sceneScale.z < 0.01f)
				scene->sceneScale.z = 0.01f;  // To be removed TODO: remove together with sceneScale!!!!!!!!!!!!
			if (scene->sceneScale.z > 10.0f)
				scene->sceneScale.z = 10.0f;
		}*/
		ImGui::Checkbox("Enable stereoscopy", &(scene->stereoscopy)); // Temporarily disabled
		if (scene->stereoscopy)
		{
			if (ImGui::SliderFloat("Distance between the eyes", &(scene->distanceBetweenEyes), 0.001f, 0.05f))
			{
				if (scene->distanceBetweenEyes < 0.001f)
					scene->distanceBetweenEyes = 0.001f;
				if (scene->distanceBetweenEyes > 0.05f)
					scene->distanceBetweenEyes = 0.05f;
			}
			if (ImGui::SliderFloat("Convergence distance", &(scene->convergenceDistance), 0.1f, 5.0f))
			{
				if (scene->convergenceDistance < 0.1f)
					scene->convergenceDistance = 0.1f;
				if (scene->convergenceDistance > 5.0f)
					scene->convergenceDistance = 5.0f;
			}
		}
		ImGui::Separator();
		if (ImGui::BeginListBox("##Object Selection", ImVec2(400, 200)))
		{
			for (int i = 0; i < scene->shapes.size(); i++)
			{
				if (ShapeTable::GetShapeByID(scene->shapes[i])->isMarkedForDeletion())
					continue;
				ImGui::PushID(i);
				std::string fullObjectName = ShapeTable::GetShapeByID(scene->shapes[i])->Name();
				if (ImGui::Selectable((fullObjectName).c_str(), ShapeTable::GetShapeByID(scene->shapes[i])->isSelected()))
				{
					if (!scene->shiftPressed)
					{
						scene->DeselectEverything();
					}
					ShapeTable::GetShapeByID(scene->shapes[i])->Select();
					if (ShapeTable::GetShapeByID(scene->shapes[i])->isSelected())
					{
						if (scene->selectedShape == -1)
						{
							scene->selectedShape = scene->shapes[i];
							scene->typeOfShapeCurrentlySelected = SHAPE_SELECTED;
							// now let's deselect all the virtual points
							for (int i = 0; i < scene->shapes.size(); i++)
							{
								IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(scene->shapes[i]));
								if (shapeWithVirtualPoints != nullptr)
								{
									if (shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
										shapeWithVirtualPoints->ConfirmSelection(false, true);
								}
							}
						}
					}
					else
					{
						scene->typeOfShapeCurrentlySelected = NONE_SELECTED;
					}
				}
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}
		ImGui::Text("Object parameters:");
		int shape = scene->selectedShape;
		if (shape != -1)
		{
			char buffer[64] = "";
			strcpy_s(buffer, (ShapeTable::GetShapeByID(shape)->Name()).c_str());
			if (ShapeTable::GetShapeByID(shape)->Name() != "3D Cursor" && ImGui::InputText("Object name", buffer, IM_ARRAYSIZE(buffer)))
			{
				std::string newName = std::string(buffer);
				if (newName == "3D Cursor")
				{
					std::cerr << "Cannot set the name to '3D Cursor'\n";
				}
				else
				{
					ShapeTable::GetShapeByID(shape)->setName(newName);
				}
			}
			ShapeTable::GetShapeByID(shape)->PrintImGuiOptions();
			ImGui::Separator();
			//scene->shapes[i]->PrintImGuiTransformOptions(); // Temporarily Disabled
			if (ShapeTable::GetShapeByID(shape)->Name() != "3D Cursor" && ImGui::Button("Delete object"))
			{
				scene->grabEnabled = false;
				scene->DeselectEverything();
				ShapeTable::GetShapeByID(shape)->MarkForDeletion();
				scene->RemoveMarkedObjects();
			}
			else
			{
				Line* linePointer = ShapeTable::GetLineByID(shape);
				if (linePointer != nullptr)
				{
					if (ImGui::Button("Add Selected Points"))
					{
						// traversing all of the selected objects and adding all the points to the line
						for (int i = 1; i < scene->shapes.size(); i++)
						{
							Point* pointPointer = ShapeTable::GetPointByID(scene->shapes[i]);
							if (pointPointer != nullptr && ShapeTable::GetShapeByID(scene->shapes[i])->isSelected())
							{
								linePointer->AddPoint(scene->shapes[i]);
							}
						}
					}
				}
			}
		}
		ImGui::Separator();
		ImGui::Text("Add objects");
		const char* items[] = { "Torus", "Ellipsoid", "Point", "Polyline", "Bezier C0", "Bezier C1", "Bezier C2", "Interpolating Curve", "Bezier Surface", "Gregory Patch", "Future objects..."};
		ImGui::Combo("Shapes", &(scene->currentItemSelectedForAdding), items, IM_ARRAYSIZE(items));
		if (scene->currentItemSelectedForAdding == 8)
		{
			const char* surfaceContinuity[] = { "C0 Continuity", "C2 Continuity" };
			ImGui::Combo("Surface Continuity", &(scene->currentSurfaceContinuitySelectedForAdding), surfaceContinuity, IM_ARRAYSIZE(surfaceContinuity));
			const char* surfaceShape[] = { "Flat", "Cylindrical" };
			ImGui::Combo("Surface Shape", &(scene->currentSurfaceShapeSelectedForAdding), surfaceShape, IM_ARRAYSIZE(surfaceShape));
			ImGui::Text("Number of Segments");
			if (scene->currentSurfaceShapeSelectedForAdding == 0)
			{
				ImGui::SliderInt("x##x1", &(scene->newSurfaceN), 1, 20);
				ImGui::SliderInt("x##y1", &(scene->newSurfaceM), 1, 20);
				ImGui::Text("Surface Size");
				ImGui::SliderFloat("x##x2", &(scene->newSurfaceU), 0.1f, 20.0f);
				ImGui::SliderFloat("y##y2", &(scene->newSurfaceV), 0.1f, 20.0f);
			}
			else
			{
				ImGui::SliderInt("Circumference", &(scene->newSurfaceN), (scene->currentSurfaceContinuitySelectedForAdding == 1 ? 4 : 2), 20);
				if (scene->newSurfaceN < 4 && scene->currentSurfaceContinuitySelectedForAdding == 1)
					scene->newSurfaceN = 4;
				else if (scene->newSurfaceN < 2)
					scene->newSurfaceN = 2;
				ImGui::SliderInt("y", &(scene->newSurfaceM), 1, 20);
				ImGui::Text("Cylinder Size");
				ImGui::SliderFloat("Radius", &(scene->newSurfaceU), 0.1f, 20.0f);
				ImGui::SliderFloat("Height", &(scene->newSurfaceV), 0.1f, 20.0f);
			}
		}
		if (ImGui::Button("Add Shape"))
		{
			scene->AddShape();
		}
		if (ImGui::CollapsingHeader("File"))
		{
			static char sceneLoadPathBuffer[256] = "SceneFiles/1.json";
			ImGui::InputText("Scene path", sceneLoadPathBuffer, IM_ARRAYSIZE(sceneLoadPathBuffer));
			if (ImGui::Button("Load Scene"))
			{
				scene->LoadFile(sceneLoadPathBuffer);
			}
			static char sceneSavePathBuffer[256] = "SceneFiles/2.json";
			ImGui::InputText("Scene path ##something", sceneSavePathBuffer, IM_ARRAYSIZE(sceneSavePathBuffer));
			if (ImGui::Button("Save Scene"))
			{
				scene->SaveFile(sceneSavePathBuffer);
			}
		}
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		ImGui::End();

		//rendering commands here

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (scene->stereoscopy)
		{
			glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE); // Red channel for the left eye
			scene->camera.eyeOffset = -scene->distanceBetweenEyes / 2.0f;
			scene->camera.convergenceDistance = scene->convergenceDistance;
			scene->DrawScene(window);
			
			glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE); // Cyan channel for the right eye
			scene->camera.eyeOffset = scene->distanceBetweenEyes / 2.0f;
			scene->DrawScene(window);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Reset to normal rendering
		}
		else
		{
			scene->DrawScene(window);
		}

		// -----
		float currentFrame = glfwGetTime();
		scene->deltaTime = currentFrame - scene->lastFrame;
		scene->lastFrame = currentFrame;
		scene->DrawCursorOverlay();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
		frame++;
	}
	glfwTerminate();
	return 0;
}