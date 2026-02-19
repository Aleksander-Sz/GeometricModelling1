#define GLAD_GL_IMPLEMENTATION
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Shader.h"
#include "../Camera.h"
#include "../Shapes.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMovement = true;
bool mouseButtonPressed = false;
bool shiftPressed = false;
Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	float cameraSpeed(shiftPressed ? 10.0f : 2.5f);
	const float cameraDisplacement = cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		shiftPressed = true;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		shiftPressed = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.cameraPos += cameraDisplacement * glm::normalize(camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.cameraPos -= cameraDisplacement * glm::normalize(camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.cameraPos -= cameraDisplacement * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.cameraPos += cameraDisplacement * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.cameraPos += cameraDisplacement * glm::normalize(cross(cross(camera.cameraFront, camera.cameraUp), camera.cameraFront));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.cameraPos -= cameraDisplacement * glm::normalize(cross(cross(camera.cameraFront, camera.cameraUp), camera.cameraFront));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse) // Check if ImGui wants to capture the mouse input
		{
			mouseButtonPressed = true;
			firstMovement = true; // Reset first movement flag when the button is pressed
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseButtonPressed = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!mouseButtonPressed)
		return;
	if (firstMovement)
	{
		firstMovement = false;
		lastX = xpos;
		lastY = ypos;
	}
	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	

	if (shiftPressed)
	{
		const float sensitivity = 0.003f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		camera.cameraPos += yOffset * glm::normalize(cross(cross(camera.cameraFront, camera.cameraUp), camera.cameraFront));
		camera.cameraPos -= xOffset * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	}
	else
	{
		const float sensitivity = 0.2f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		camera.yaw += xOffset;
		camera.pitch -= yOffset;

		if (camera.pitch > 89.0f)
			camera.pitch = 89.0f;
		if (camera.pitch < -89.0f)
			camera.pitch = -89.0f;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	camera.zoom -= (float)yoffset;
	if (camera.zoom < 1.0f)
		camera.zoom = 1.0f;
	if (camera.zoom > 45.0f)
		camera.zoom = 45.0f;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	int windowWidth = 1200; // Your desired width
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


	camera = Camera(fbWidth, fbHeight);

	// Rendering commands here
	
	Shader ourShader("Shaders/VertexShader.glsl","Shaders/FragmentShader.glsl");
	std::vector<Shape*> shapes;
	Torus torus(1.0f, 0.3f, 50, 50);
	shapes.push_back(&torus);
	Point point(glm::vec3(0.0f, 0.1f, 0.0f));
	shapes.push_back(&point);
	torus.Rotate(90.0f,glm::vec3(1.0f,0.0f,0.0f));
	Grid grid = Grid::getInstance();

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
		ImGui::Text("Object parameters:");
		for (int i = 0; i < shapes.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader((shapes[i]->Name() + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				shapes[i]->PrintImGuiOptions();
				ImGui::Separator();
				shapes[i]->PrintImGuiTransformOptions();
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
				shapes.push_back(new Torus(1.0f, 0.3f, 50, 50));
				break;
			case 1:
				shapes.push_back(new Ellipsoid(1.0f, 1.2f, 0.8f, 50));
				break;
			case 2:
				shapes.push_back(new Point(glm::vec3(0.0f, 0.0f, 0.0f)));
				break;
			default:
				std::cerr << "Shape not implemented yet.\n";
				break;
			}
		}
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		ImGui::End();

		//rendering commands here
		glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		ourShader.setMat4("view", camera.view());
		ourShader.setMat4("projection", camera.projectionRight());
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		glDepthMask(GL_FALSE);
		grid.Draw(camera, 'R');
		for (int i = 0; i < shapes.size(); i++)
			shapes[i]->Draw(ourShader);
		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		ourShader.setMat4("projection", camera.projectionLeft());
		grid.Draw(camera, 'L');
		for (int i = 0; i < shapes.size(); i++)
			shapes[i]->Draw(ourShader);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		// -----
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
		frame++;
	}
	glfwTerminate();
	return 0;
}