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
        Shape* newSelectedShape = NULL;
        //
        float minDistance = 1000.0f;
        int closestObject = 0;
        std::cout << "cursor: " << lastX << ", " << lastY << "\n";
        for (int i = 0; i < shapes.size(); i++)
        {
            glm::vec2 objectPosition = shapes[i]->getScreenSpacePosition(camera);
            std::cout << "object " << i << ": " << objectPosition.x << ", " << objectPosition.y << "\n";
            float distance = glm::length(objectPosition - glm::vec2(lastX,lastY));
            if (distance < minDistance)
            {
                minDistance = distance;
                closestObject = i;
            }
        }
        bool isSelected = false;
        if (minDistance < 30.0f)
            isSelected = shapes[closestObject]->Select();
        if (isSelected)
            newSelectedShape = shapes[closestObject];
        //
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("view", camera.view());
    shader.setMat4("projection", camera.projectionRight());
    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    glDepthMask(GL_FALSE);
    grid.Draw(camera, 'R');
    for (int i = 0; i < shapes.size(); i++)
        shapes[i]->Draw(shader);
    if (!grabEnabled)
        cursor.Draw(shader, 'R');
    else if (xLocked || yLocked || zLocked)
        movementAxis.Draw(shader, 'R');
    glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    shader.setMat4("projection", camera.projectionLeft());
    grid.Draw(camera, 'L');
    for (int i = 0; i < shapes.size(); i++)
        shapes[i]->Draw(shader);
    if (!grabEnabled)
        cursor.Draw(shader, 'L');
    else if (xLocked || yLocked || zLocked)
        movementAxis.Draw(shader, 'L');
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
void Scene::Scale(glm::vec3 s)
{
    //model = glm::scale(model, s);
    glm::mat4 scaleMatrix = glm::mat4(glm::vec4(s.x, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, s.y, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, s.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    sceneMatrix = scaleMatrix * sceneMatrix;
}
void Scene::Rotate(float angle, glm::vec3 axis)
{
    //model = glm::rotate(model, glm::radians(angle), axis);
    axis = glm::normalize(axis);
    float c = cos(glm::radians(angle));
    float s = sin(glm::radians(angle));
    glm::mat4 rotationMatrix;
    if (axis.x == 1.0f)
        rotationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, c, s, 0.0f), glm::vec4(0.0f, -s, c, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else if (axis.y == 1.0f)
        rotationMatrix = glm::mat4(glm::vec4(c, 0.0f, -s, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(s, 0.0f, c, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else if (axis.z == 1.0f)
        rotationMatrix = glm::mat4(glm::vec4(c, s, 0.0f, 0.0f), glm::vec4(-s, c, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else
    {
        std::cerr << "Rotation axis must be one of the cardinal axes (x,y,z).\n";
        return;
    }
    sceneMatrix = rotationMatrix * sceneMatrix;
}
void Scene::Translate(glm::vec3 t)
{
    //model = glm::translate(model, t);
    glm::mat4 translationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(t.x, t.y, t.z, 1.0f));
    sceneMatrix = translationMatrix * sceneMatrix;
}
void Scene::resetSceneMatrix()
{
    sceneMatrix = glm::mat4(1.0f);
}