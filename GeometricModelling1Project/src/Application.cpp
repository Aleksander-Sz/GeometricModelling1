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
				scene->xLocked = scene->yLocked = scene->zLocked = false;
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
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		if (!scene->xPressed)
		{
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

			scene->camera.cameraPos += yOffset * aa::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
			scene->camera.cameraPos -= xOffset * aa::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
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
			float distance = aa::length(scene->selectedShape->getPosition() - scene->camera.cameraPos);

			float fovRad = aa::radians(scene->camera.zoom); // assuming zoom = FOV
			float viewHeight = 2.0f * distance * tan(fovRad * 0.5f);

			float worldPerPixel = viewHeight / scene->camera.windowHeight;

			aa::vec3 forward = aa::normalize(scene->camera.cameraFront);
			aa::vec3 right = aa::normalize(aa::cross(forward, scene->camera.cameraUp));
			aa::vec3 up = aa::normalize(aa::cross(right, forward));

			float xOffsetSinceMoveStart = xpos - scene->grabMouseOrigin.x;
			float yOffsetSinceMoveStart = ypos - scene->grabMouseOrigin.y;

			aa::vec3 worldDelta =
				right * (xOffsetSinceMoveStart * worldPerPixel)
				- up * (yOffsetSinceMoveStart * worldPerPixel);
			if (scene->grabEnabled)
				scene->MoveSelectedObjects(worldDelta);
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
					//std::cout << factor << "\n";
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
				if (scene->xLocked)
				{
					rotationOrigin.x = 0.0f;
					rotation3DStart.x = 0.0f;
					rotation3DEnd.x = 0.0f;
				}
				else if (scene->yLocked)
				{
					rotationOrigin.y = 0.0f;
					rotation3DStart.y = 0.0f;
					rotation3DEnd.y = 0.0f;
				}
				else if (scene->zLocked)
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
				if (scene->xLocked)
				{
					scene->RotateSelectedObjects(angle, aa::Axis::X);
				}
				if (scene->yLocked)
				{
					scene->RotateSelectedObjects(angle, aa::Axis::Y);
				}
				if (scene->zLocked)
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
/*	if (scene->AltPressed)
	{
		aa::vec3 cameraTarget = scene->camera.cameraPos + scene->camera.cameraFront;
		float cameraRadius = aa::length(scene->camera.cameraPos - cameraTarget);

		const float sensitivity = 0.3f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		scene->camera.yaw -= xoffset;
		scene->camera.pitch -= yoffset;

		if (scene->camera.pitch > 89.0f)
			scene->camera.pitch = 89.0f;
		if (scene->camera.pitch < -89.0f)
			scene->camera.pitch = -89.0f;

		// Convert to radians
		float yawRad = aa::radians(scene->camera.yaw);
		float pitchRad = aa::radians(scene->camera.pitch);

		// Spherical coordinates
		aa::vec3 offset;
		offset.x = cameraRadius * cos(pitchRad) * cos(yawRad);
		offset.y = cameraRadius * sin(pitchRad);
		offset.z = cameraRadius * cos(pitchRad) * sin(yawRad);

		// New position
		scene->camera.cameraPos = cameraTarget + offset;

		// Always look at target
		scene->camera.cameraFront = aa::normalize(cameraTarget - scene->camera.cameraPos);
	}*/
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

	// Rendering commands here
	
	Torus* torus = new Torus(1.0f, 0.3f, 50, 50);
	scene->shapes.push_back(torus);
	//Point point(aa::vec3(0.0f, 0.1f, 0.0f));
	//scene->shapes.push_back(&point);
	torus->Rotate(aa::radians(90.0f),aa::Axis::X);
	torus->ConfirmTransformations();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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
		//ImGui::Checkbox("Enable stereoscopy", &(scene->stereoscopy)); // Temporarily disabled
		ImGui::Separator();
		ImGui::Checkbox("Transform Around the 3D Cursor", &(scene->transformAroundCursor));
		if (ImGui::DragFloat3("Scene Scaling", aa::value_ptr(scene->sceneScale), 0.01f, 0.01f, 10.0f))
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
				scene->sceneScale.z = 0.01f;
			if (scene->sceneScale.z > 10.0f)
				scene->sceneScale.z = 10.0f;
		}
		ImGui::Separator();
		if (ImGui::BeginListBox("##Object Selection", ImVec2(400, 200)))
		{
			for (int i = 0; i < scene->shapes.size(); i++)
			{
				ImGui::PushID(i);
				std::string fullObjectName = scene->shapes[i]->Name();
				if (typeid(&(scene->shapes[i])) != typeid(Cursor))
					fullObjectName += " " + std::to_string(i);
				if (ImGui::Selectable((fullObjectName).c_str(), scene->shapes[i]->isSelected()))
				{
					if (!scene->shiftPressed)
					{
						scene->DeselectEverything();
					}
					scene->shapes[i]->Select();
					scene->selectedShape = scene->shapes[i];
				}
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}
		ImGui::Text("Object parameters:");
		Shape* shape = scene->selectedShape;
		if (shape != nullptr)
		{
			char buffer[64] = "";
			strcpy_s(buffer, (shape->Name()).c_str());
			if (typeid(&(shape)) == typeid(Cursor) && ImGui::InputText("Object name", buffer, IM_ARRAYSIZE(buffer)))
			{
				std::string newName = std::string(buffer);
				if (newName == "3D Cursor")
				{
					std::cerr << "Cannot set the name to '3D Cursor'\n";
				}
				else
				{
					shape->setName(newName);
				}
			}
			shape->PrintImGuiOptions();
			ImGui::Separator();
			//scene->shapes[i]->PrintImGuiTransformOptions(); // Temporarily Disabled
			if (shape->Name() != "3D Cursor" && ImGui::Button("Delete object"))
			{
				scene->grabEnabled = false;
				scene->DeselectEverything();
				delete shape;
				auto it = std::find(scene->shapes.begin(), scene->shapes.end(), shape);
				scene->shapes.erase(it);
			}
		}
		ImGui::Separator();
		ImGui::Text("Add objects");
		const char* items[] = { "Torus", "Ellipsoid", "Point", "Future objects..." };
		static int current_item_index = 0;
		ImGui::Combo("Shapes", &current_item_index, items, IM_ARRAYSIZE(items));
		if (ImGui::Button("Add Shape"))
		{
			bool wasAShapeAdded = true;
			switch (current_item_index)
			{
			case 0:
				scene->shapes.push_back(new Torus(1.0f, 0.3f, 50, 50));
				break;
			case 1:
				std::cout << "This option has been locked, as it is out of the scope of MKMG.\n";
				break;
				scene->shapes.push_back(new Ellipsoid(1.0f, 1.2f, 0.8f, 50));
				break;
			case 2: 
				scene->shapes.push_back(new Point(aa::vec3(0.0f, 0.0f, 0.0f)));
				break;
			default:
				std::cerr << "Shape not implemented yet.\n";
				wasAShapeAdded = false;
				break;
			}
			if(wasAShapeAdded)
				scene->shapes[scene->shapes.size()-1]->TranslateAndConfirm(scene->cursor.getPosition());
		}
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		ImGui::End();

		//rendering commands here
		
		scene->DrawScene(window);

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