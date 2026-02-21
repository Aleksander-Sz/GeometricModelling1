#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
}
void Scene::LockXAxis()
{
    if (xLocked = !xLocked)
    {
        yLocked = false;
        zLocked = false;
        movementAxis = Axis('x', currentTranslationOrigin);
        CancellObjectMovement();
    }
}
void Scene::LockYAxis()
{
    if (yLocked = !yLocked)
    {
        xLocked = false;
        zLocked = false;
        movementAxis = Axis('y', currentTranslationOrigin);
        std::cout << "y axis locked\n";
        CancellObjectMovement();
    }
}
void Scene::LockZAxis()
{
    if (zLocked = !zLocked)
    {
        xLocked = false;
        yLocked = false;
        movementAxis = Axis('z', currentTranslationOrigin);
        CancellObjectMovement();
    }
}
void Scene::toggleGrab()
{
    xLocked = false;
    yLocked = false;
    zLocked = false;
	if (selectedShape != NULL)
    {
        if(grabEnabled)
        {
            ConfirmObjectMovement();
		}
        else
        {
            currentTranslationOrigin = selectedShape->getPosition();
        }
        grabEnabled = !grabEnabled;
    }
}
void Scene::UpdateCursorPosition(double xpos, double ypos)
{
	cursor.UpdatePosition(camera, xpos, ypos);
}
void Scene::LeftMouseClick()
{
    if(grabEnabled)
    {
        ConfirmObjectMovement();
        grabEnabled = false;
	}
    else
    {
        Shape* previousShape = NULL;
        if (!shiftPressed)
        {
            previousShape = selectedShape;
            DeselectEverything();
        }
        Shape* newSelectedShape = cursor.Click(shapes);
        if (!shiftPressed && previousShape == newSelectedShape)
            return;
        if (newSelectedShape != NULL)
        {
            std::cout << newSelectedShape->Name() << "\n";
            if (selectedShape != NULL)
            {
                ;//selectedShape->Select(true); // Deselect current shape
            }
            selectedShape = newSelectedShape;
        }
        else
        {
            grabEnabled = false;
        }
    }
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
void Scene::MoveSelectedObjects(glm::vec3 translation)
{
    for (Shape* shape : shapes)
    {
        if (shape->isSelected())
        {
            shape->Translate(translation);
        }
    }
}
void Scene::CancellObjectMovement()
{
    for (Shape* shape : shapes)
    {
        if (shape->isSelected())
        {
            shape->CancelTransformations();
        }
    }
}
void Scene::ConfirmObjectMovement()
{
    for (Shape* shape : shapes)
    {
        if (shape->isSelected())
        {
            shape->ConfirmTransformations();
        }
    }
}
void Scene::DeselectEverything()
{
    selectedShape = NULL;
    for (int i = 0; i < shapes.size(); i++)
    {
        shapes[i]->Select(true);
    }
}
void Scene::DrawScene()
{
    if (grabEnabled && (xLocked || yLocked || zLocked))
    {
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        shader.setMat4("projection", camera.projectionRight());
        movementAxis.Draw(shader, 'R');
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        shader.setMat4("projection", camera.projectionLeft());
        movementAxis.Draw(shader, 'L');
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}