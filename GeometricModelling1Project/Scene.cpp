#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
    shapes.push_back(&cursor);
}
void Scene::LockXAxis()
{
    if (!grabEnabled)
        return;
    if (xLocked = !xLocked)
    {
        yLocked = false;
        zLocked = false;
        movementAxis = Axis('x', currentTranslationOrigin);
    }
    MoveSelectedObjects(unlockedTranslationBackup);
}
void Scene::LockYAxis()
{
    if (!grabEnabled)
        return;
    if (yLocked = !yLocked)
    {
        xLocked = false;
        zLocked = false;
        movementAxis = Axis('y', currentTranslationOrigin);
    }
    MoveSelectedObjects(unlockedTranslationBackup);
}
void Scene::LockZAxis()
{
    if (!grabEnabled)
        return;
    if (zLocked = !zLocked)
    {
        xLocked = false;
        yLocked = false;
        movementAxis = Axis('z', currentTranslationOrigin);
    }
    MoveSelectedObjects(unlockedTranslationBackup);
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
            currentTranslationOrigin = (sceneMatrix * aa::vec4(selectedShape->getPosition(), 1.0f)).xyz;
            grabMouseOrigin = aa::vec2(lastX, lastY);
        }
        grabEnabled = !grabEnabled;
    }
}
void Scene::UpdateCursorPosition(double xpos, double ypos)
{
	cursor.UpdatePosition(camera, xpos, ypos, xLocked, yLocked, zLocked);
}
void Scene::LeftMouseClick()
{
    if(grabEnabled)
    {
        ConfirmObjectMovement();
        grabEnabled = false;
        xLocked = yLocked = zLocked = false;
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
        for (int i = 0; i < shapes.size(); i++)
        {
            aa::vec2 objectPosition = shapes[i]->getScreenSpacePosition(camera);
            float distance = aa::length(objectPosition - aa::vec2(lastX,lastY));
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

    //show object position in grab mode
    if (grabEnabled)
    {
        ImGui::Text("Object Position:");
        aa::vec3 objectPosition = selectedShape->getPosition();
        ImGui::Text("X: %.2f", objectPosition.x);
        ImGui::Text("Y: %.2f", objectPosition.y);
        ImGui::Text("Z: %.2f", objectPosition.z);
    }

	ImGui::Text("2D Position:");
    ImGui::Text("X: %.1f", lastX);
    ImGui::Text("Y: %.1f", lastY);
	ImGui::Text("3D Cursor:");
	aa::vec3  cursorPos = cursor.getPosition();
	ImGui::Text("X: %.2f", cursorPos.x);
	ImGui::Text("Y: %.2f", cursorPos.y);
	ImGui::Text("Z: %.2f", cursorPos.z);
    ImGui::End();
}
void Scene::MoveSelectedObjects(aa::vec3 translation)
{
    unlockedTranslationBackup = translation;
    float xLockFactor = 0.0f;
    float yLockFactor = 0.0f;
    float zLockFactor = 0.0f;
    if (xLocked)
        xLockFactor = 1.0f;
    else if (yLocked)
        yLockFactor = 1.0f;
    else if (zLocked)
        zLockFactor = 1.0f;
    else
    {
        xLockFactor = 1.0f;
        yLockFactor = 1.0f;
        zLockFactor = 1.0f;
    }
    translation.x *= xLockFactor;
    translation.y *= yLockFactor;
    translation.z *= zLockFactor;
    for (Shape* shape : shapes)
    {
        if (shape->isSelected())
        {
            translation = (inverseSceneMatrix * aa::vec4(translation, 1.0f)).xyz;
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
void Scene::DrawScene(GLFWwindow* window)
{
    glDisable(GL_DEPTH_TEST); // temporary, might work, but I don't know
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("view", camera.view());
    if (stereoscopy)
        shader.setMat4("projection", camera.projectionRight());
    else
        shader.setMat4("projection", camera.projection());
    sceneMatrix = aa::scale(sceneScale);
    shader.setMat4("scene", sceneMatrix);
    if(stereoscopy)
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    glDepthMask(GL_FALSE);
    grid.Draw(camera, 'R');
    for (int i = 1; i < shapes.size(); i++)
        shapes[i]->Draw(shader);
    cursor.Draw(shader);
    if (grabEnabled && (xLocked || yLocked || zLocked))
        movementAxis.Draw(shader, 'R');
    if (stereoscopy)
    {
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        shader.setMat4("projection", camera.projectionLeft());
        shader.setMat4("scene", sceneMatrix);
        grid.Draw(camera, 'L');
        for (int i = 1; i < shapes.size(); i++)
            shapes[i]->Draw(shader);
        cursor.Draw(shader);
        if (grabEnabled && (xLocked || yLocked || zLocked))
            movementAxis.Draw(shader, 'L');
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}