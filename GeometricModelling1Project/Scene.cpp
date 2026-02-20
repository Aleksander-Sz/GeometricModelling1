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
void Scene::DrawCursorOverlay()
{
    const float padding = 10.0f;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos, windowPosPivot;

    // Position at bottom-right
    windowPos.x = io.DisplaySize.x - padding;
    windowPos.y = io.DisplaySize.y - padding;
    windowPosPivot.x = 1.0f;
    windowPosPivot.y = 1.0f;

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background (optional)

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        //ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    ImGui::Begin("CursorOverlay", nullptr, flags);

	ImGui::Text("2D Position:");
    ImGui::Text("X: %.1f", lastX);
    ImGui::Text("Y: %.1f", lastY);
	ImGui::Text("3D Position:");
	glm::vec3  cursorPos = cursor.getPosition();
	ImGui::Text("X: %.2f", cursorPos.x);
	ImGui::Text("Y: %.2f", cursorPos.y);
	ImGui::Text("Z: %.2f", cursorPos.z);
    ImGui::End();
}