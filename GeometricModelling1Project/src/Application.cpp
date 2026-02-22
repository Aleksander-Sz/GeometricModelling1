#define GLAD_GL_IMPLEMENTATION
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Shader.h"
#include "../Camera.h"
#include "../Shapes.h"
#include "../Scene.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Scene* scene;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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
			if (scene->grabEnabled)
			{
				scene->CancellObjectMovement();
				scene->grabEnabled = false;
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
		scene->camera.cameraPos += cameraDisplacement * glm::normalize(scene->camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * glm::normalize(scene->camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * glm::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * glm::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * glm::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * glm::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if(!scene->lPressed)
			scene->cursorLocked = !scene->cursorLocked;
		scene->lPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
		scene->lPressed = false;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		if (!scene->gPressed)
			scene->toggleGrab();
		scene->gPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
		scene->gPressed = false;
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
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		scene->AltPressed = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
		scene->AltPressed = false;
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
		scene->mouseLeftPressPosition = glm::vec2(scene->lastX, scene->lastY);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		scene->mouseLeftButtonPressed = false;
		double mouseReleaseTime = glfwGetTime();
		float movement = glm::length(glm::vec2(scene->lastX, scene->lastY) - scene->mouseLeftPressPosition);
		if (mouseReleaseTime - scene->mouseLeftPressTime <= 0.2 && movement < 5.0f)
		{
			scene->LeftMouseClick();
		}
	}
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

			scene->camera.cameraPos += yOffset * glm::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
			scene->camera.cameraPos -= xOffset * glm::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
		}
		else
		{
			const float sensitivity = 0.1f;
			xOffset *= sensitivity;
			yOffset *= sensitivity;

			scene->camera.yaw -= xOffset;
			scene->camera.pitch += yOffset;

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

		scene->camera.yaw += xOffset;
		scene->camera.pitch -= yOffset;

		if (scene->camera.pitch > 89.0f)
			scene->camera.pitch = 89.0f;
		if (scene->camera.pitch < -89.0f)
			scene->camera.pitch = -89.0f;
	}
	else
	{
		if (scene->grabEnabled)
		{
			//float sensitivity = 0.003f;
			//xOffset *= sensitivity;
			//yOffset *= sensitivity;

			float distance = glm::length(scene->selectedShape->getPosition() - scene->camera.cameraPos);

			float fovRad = glm::radians(scene->camera.zoom); // assuming zoom = FOV
			float viewHeight = 2.0f * distance * tan(fovRad * 0.5f);

			float worldPerPixel = viewHeight / scene->camera.windowHeight;

			glm::vec3 forward = glm::normalize(scene->camera.cameraFront);
			glm::vec3 right = glm::normalize(glm::cross(forward, scene->camera.cameraUp));
			glm::vec3 up = glm::normalize(glm::cross(right, forward));

			glm::vec3 worldDelta =
				right * (xOffset * worldPerPixel)
				- up * (yOffset * worldPerPixel);
			float xLockFactor = 0.0f;
			float yLockFactor = 0.0f;
			float zLockFactor = 0.0f;
			if (scene->xLocked)
				xLockFactor = 1.0f;
			else if (scene->yLocked)
				yLockFactor = 1.0f;
			else if (scene->zLocked)
				zLockFactor = 1.0f;
			else
			{
				xLockFactor = 1.0f;
				yLockFactor = 1.0f;
				zLockFactor = 1.0f;
			}
			worldDelta *= glm::vec3(xLockFactor, yLockFactor, zLockFactor);
			scene->MoveSelectedObjects(worldDelta);
		}
		/*if (!scene->cursorLocked)
			scene->UpdateCursorPosition(xpos, ypos);*/
	}
	scene->lastX = xpos;
	scene->lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	if (scene->AltPressed)
	{
		glm::vec3 cameraTarget = scene->camera.cameraPos + scene->camera.cameraFront;
		float cameraRadius = glm::length(scene->camera.cameraPos - cameraTarget);

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
		float yawRad = glm::radians(scene->camera.yaw);
		float pitchRad = glm::radians(scene->camera.pitch);

		// Spherical coordinates
		glm::vec3 offset;
		offset.x = cameraRadius * cos(pitchRad) * cos(yawRad);
		offset.y = cameraRadius * sin(pitchRad);
		offset.z = cameraRadius * cos(pitchRad) * sin(yawRad);

		// New position
		scene->camera.cameraPos = cameraTarget + offset;

		// Always look at target
		scene->camera.cameraFront = glm::normalize(cameraTarget - scene->camera.cameraPos);
	}
	else
	{
		scene->camera.zoom -= (float)yoffset;
		if (scene->camera.zoom < 10.0f)
			scene->camera.zoom = 10.0f;
		if (scene->camera.zoom > 45.0f)
			scene->camera.zoom = 45.0f;
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	int windowWidth = 900; // Your desired width
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
	Scene sceneObject = Scene(900, 900, ourShader);
	scene = &sceneObject;

	// Rendering commands here
	
	Torus torus(1.0f, 0.3f, 50, 50);
	scene->shapes.push_back(&torus);
	Point point(glm::vec3(0.0f, 0.1f, 0.0f));
	scene->shapes.push_back(&point);
	torus.Rotate(90.0f,glm::vec3(1.0f,0.0f,0.0f));
	torus.ConfirmTransformations();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glViewport(0, 0, windowWidth, windowHeight);
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
		if (ImGui::CollapsingHeader("Scene Transformations"))
		{
			static glm::vec3 scale(1.0f);
			static float rotationX = 0.0f;
			static float rotationY = 0.0f;
			static float rotationZ = 0.0f;
			static glm::vec3 translation(0.0f);
			ImGui::InputFloat3("Scale", glm::value_ptr(scale));
			if (ImGui::Button("Apply Scale"))
			{
				scene->Scale(scale);
			}
			ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
			if (ImGui::Button("Apply Rotation X"))
			{
				scene->Rotate(rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
			if (ImGui::Button("Apply Rotation Y"))
			{
				scene->Rotate(rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
			if (ImGui::Button("Apply Rotation Z"))
			{
				scene->Rotate(rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			ImGui::InputFloat3("Translation", glm::value_ptr(translation));
			if (ImGui::Button("Apply Translation"))
			{
				scene->Translate(translation);
			}
			if (ImGui::Button("Reset Transformations"))
			{
				scene->resetSceneMatrix();
				scale = glm::vec3(1.0f);
				rotationX = rotationY = rotationZ = 0.0f;
				translation = glm::vec3(0.0f);
			}
		}
		ImGui::Separator();
		ImGui::Text("Object parameters:");
		for (int i = 0; i < scene->shapes.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader((scene->shapes[i]->Name() + " " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				char buffer[64] = "";
				strcpy_s(buffer, (scene->shapes[i]->Name()).c_str());
				if (ImGui::InputText("Object name", buffer, IM_ARRAYSIZE(buffer)))
				{
					scene->shapes[i]->setName(std::string(buffer));
				}
				if (ImGui::Button("Select this object"))
				{
					scene->shapes[i]->Select();
				}
				scene->shapes[i]->PrintImGuiOptions();
				ImGui::Separator();
				scene->shapes[i]->PrintImGuiTransformOptions();
				if (ImGui::Button("Delete object"))
				{
					scene->grabEnabled = false;
					scene->DeselectEverything();
					scene->shapes.erase(scene->shapes.begin() + i);
				}
			}
			ImGui::PopID();
		}
		ImGui::Separator();
		ImGui::Text("Add objects");
		const char* items[] = { "Torus", "Ellipsoid", "Point", "Future objects..." };
		static int current_item_index = 0;
		ImGui::Combo("Shapes", &current_item_index, items, IM_ARRAYSIZE(items));
		if (ImGui::Button("Add Shape"))
		{
			switch (current_item_index)
			{
			case 0:
				scene->shapes.push_back(new Torus(1.0f, 0.3f, 50, 50));
				break;
			case 1:
				scene->shapes.push_back(new Ellipsoid(1.0f, 1.2f, 0.8f, 50));
				break;
			case 2:
				scene->shapes.push_back(new Point(glm::vec3(0.0f, 0.0f, 0.0f)));
				break;
			default:
				std::cerr << "Shape not implemented yet.\n";
				break;
			}
		}
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		ImGui::End();

		//rendering commands here
		
		scene->DrawScene();
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