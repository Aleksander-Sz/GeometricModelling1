#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
    shapes.push_back(&cursor);
	cursor.setShader(shader);
	centerOfGravityIndicator.setShader(shader);
}
void Scene::LockXAxis()
{
    if (!grabEnabled && !rotatingEnabled)
        return;
    if (xLocked = !xLocked)
    {
        yLocked = false;
        zLocked = false;
        movementAxis = Axis('x');
    }
    if (rotatingEnabled)
        xLocked = true;
    MoveSelectedObjects(unlockedTranslationBackup);
}
void Scene::LockYAxis()
{
    if (!grabEnabled && !rotatingEnabled)
        return;
    if (yLocked = !yLocked)
    {
        xLocked = false;
        zLocked = false;
        movementAxis = Axis('y');
    }
    if (rotatingEnabled)
        yLocked = true;
    MoveSelectedObjects(unlockedTranslationBackup);
}
void Scene::LockZAxis()
{
    if (!grabEnabled && !rotatingEnabled)
        return;
    if (zLocked = !zLocked)
    {
        xLocked = false;
        yLocked = false;
        movementAxis = Axis('z');
    }
    if (rotatingEnabled)
        zLocked = true;
    MoveSelectedObjects(unlockedTranslationBackup);
}
void Scene::toggleGrab()
{
    if (scalingEnabled || rotatingEnabled)
        return;
    xLocked = false;
    yLocked = false;
    zLocked = false;
	if (selectedShape != nullptr)
    {
        if(grabEnabled)
        {
            ConfirmObjectMovement();
		}
        else
        {
            grabMouseOrigin = aa::vec2(lastX, lastY);
        }
        grabEnabled = !grabEnabled;
    }
}
void Scene::toggleScaling()
{
    if (grabEnabled || rotatingEnabled)
        return;
    xLocked = false;
    yLocked = false;
    zLocked = false;
    if (selectedShape != nullptr)
    {
        if (scalingEnabled)
        {
            ConfirmObjectMovement();
        }
        else
        {
            grabMouseOrigin = aa::vec2(lastX, lastY);
        }
        scalingEnabled = !scalingEnabled;
    }
}
void Scene::toggleRotating()
{
    if (grabEnabled || scalingEnabled)
        return;
    xLocked = false;
    yLocked = true; // default rotation axis
    zLocked = false;
    if (selectedShape != nullptr)
    {
        if (rotatingEnabled)
        {
            ConfirmObjectMovement();
        }
        else
        {
            grabMouseOrigin = aa::vec2(lastX, lastY);
        }
        rotatingEnabled = !rotatingEnabled;
    }
}
void Scene::UpdateCursorPosition(double xpos, double ypos)
{
	cursor.UpdatePosition(camera, xpos, ypos, xLocked, yLocked, zLocked);
}
void Scene::LeftMouseClick()
{
    if(grabEnabled||scalingEnabled||rotatingEnabled)
    {
        ConfirmObjectMovement();
        rotatingEnabled = scalingEnabled = grabEnabled = false;
        xLocked = yLocked = zLocked = false;
	}
    else
    {
        Shape* previousShape = nullptr;
        if (!shiftPressed)
        {
            previousShape = selectedShape;
            DeselectEverything();
        }
        Shape* newSelectedShape = nullptr;
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
        if (newSelectedShape != nullptr)
        {
            std::cout << newSelectedShape->Name() << "\n";
            if (selectedShape != nullptr)
            {
                ;//selectedShape->Select(true); // Deselect current shape
            }
            if(selectedShape==nullptr)
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
        aa::vec3 objectPosition = selectedShape->getPosition(); // TODO correct this
        if(numberOfSelectedShapes>1)
            objectPosition = centerOfGravityIndicator.getPosition();
        ImGui::Text("X: %.2f", objectPosition.x);
        ImGui::Text("Y: %.2f", objectPosition.y);
        ImGui::Text("Z: %.2f", objectPosition.z);
    }

	ImGui::Text("2D Position:");
    ImGui::Text("X: %.1f", lastX);
    ImGui::Text("Y: %.1f", lastY);
    ImGui::Separator();
	ImGui::Text("3D Cursor:");
    ImGui::Text("Position:");
	aa::vec3 cursorPos3D = cursor.getPosition();
	ImGui::Text("X: %.2f", cursorPos3D.x);
	ImGui::Text("Y: %.2f", cursorPos3D.y);
	ImGui::Text("Z: %.2f", cursorPos3D.z);
    ImGui::Text("Screen Pos:");
    aa::vec2 cursorPos2D = cursor.getScreenSpacePosition(camera);
    ImGui::Text("X: %.2f", cursorPos2D.x);
    ImGui::Text("Y: %.2f", cursorPos2D.y);
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
void Scene::ScaleSelectedObjects(float factor)
{
    aa::vec3 scaling = aa::vec3(factor, factor, factor);
    for (int i = 1; i < shapes.size(); i++)
    {
        if (shapes[i]->isSelected())
        {
            scaling = (inverseSceneMatrix * aa::vec4(scaling, 1.0f)).xyz;
            if(transformAroundCursor)
                shapes[i]->Scale(scaling, cursor.getPosition());
            else
                shapes[i]->Scale(scaling, currentTranslationOrigin);
        }
    }
}
void Scene::RotateSelectedObjects(float angle, aa::Axis axis)
{
    for (int i = 1; i < shapes.size(); i++)
    {
        if (shapes[i]->isSelected())
        {
            if (transformAroundCursor)
                shapes[i]->Rotate(angle, axis, cursor.getPosition());
            else
                shapes[i]->Rotate(angle, axis, currentTranslationOrigin);
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
void Scene::DeleteSelectedObjects()
{
    selectedShape = NULL;
    for (int i = 1; i < shapes.size(); i++)
    {
        if (shapes[i]->isSelected())
        {
            shapes[i]->MarkForDeletion();
        }
    }
    RemoveMarkedObjects();
}
void Scene::StartBoxSelect(aa::vec2 location)
{
    boxSelectOrigin = location;
    boxSelectActive = true;
}
void Scene::EndBoxSelect(aa::vec2 location)
{
    DeselectEverything();
    boxSelectActive = false;
    int leftBoundry     = std::min(boxSelectOrigin.x, location.x);
    int rightBoundry    = std::max(boxSelectOrigin.x, location.x);
    int topBoundry      = std::max(boxSelectOrigin.y, location.y);
    int bottomBoundry   = std::min(boxSelectOrigin.y, location.y);
    for (int i = 1; i < shapes.size(); i++)
    {
        aa::vec2 shapeLocation = shapes[i]->getScreenSpacePosition(camera);
        if (shapeLocation.x >= leftBoundry && shapeLocation.x <= rightBoundry && shapeLocation.y >= bottomBoundry && shapeLocation.y <= topBoundry)
        {
            shapes[i]->Select();
            selectedShape = shapes[i];
        }
    }
}
aa::vec2 Scene::getTransformationCenterScreenSpacePosition()
{
    // clip space transform
    aa::vec4 clipSpacePos = camera.projection() * camera.view() * aa::vec4(currentTranslationOrigin, 1.0f);
    // Perspective Division
    if (clipSpacePos.w != 0.0f) {
        aa::vec3 ndc = clipSpacePos.xyz / clipSpacePos.w;

        float screenX = ((ndc.x + 1.0f) / 2.0f) * camera.windowWidth;
        float screenY = ((1.0f - ndc.y) / 2.0f) * camera.windowHeight;

        return aa::vec2(screenX, screenY);
    }

    return aa::vec2(-1.0f);
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
    tesselationShader.use();
	tesselationShader.setMat4("view", camera.view());
    shader.use();
    if (stereoscopy)
    {
        shader.setMat4("projection", camera.projectionRight());
        tesselationShader.use();
		tesselationShader.setMat4("projection", camera.projectionRight());
        shader.use();
    }
    else
    {
        shader.setMat4("projection", camera.projection());
		tesselationShader.use();
		tesselationShader.setMat4("projection", camera.projection());
        shader.use();
    }
    sceneMatrix = aa::scale(sceneScale);
    inverseSceneMatrix = aa::scale(aa::vec3(1.0f / sceneScale.x, 1.0f / sceneScale.y, 1.0f / sceneScale.z));
    shader.setMat4("scene", sceneMatrix);
	tesselationShader.use();
	tesselationShader.setMat4("scene", sceneMatrix);
    shader.use();
    if(stereoscopy)
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    glDepthMask(GL_FALSE);
    grid.Draw(camera, 'R');
    for (int i = 1; i < shapes.size(); i++)
        shapes[i]->Draw();
    cursor.Draw();
    if (grabEnabled && (xLocked || yLocked || zLocked))
        movementAxis.Draw(shader, currentTranslationOrigin, 'R');
    if (stereoscopy)
    {
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        shader.setMat4("projection", camera.projectionLeft());
        shader.setMat4("scene", sceneMatrix);
        grid.Draw(camera, 'L');
        for (int i = 1; i < shapes.size(); i++)
            shapes[i]->Draw();
        cursor.Draw();
        if (grabEnabled && (xLocked || yLocked || zLocked))
            movementAxis.Draw(shader, currentTranslationOrigin, 'L');
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    //Multiple Selection Center Of Gravity
    if (!grabEnabled && !scalingEnabled && !rotatingEnabled)
    {
        numberOfSelectedShapes = 0;
        aa::vec3 centerOfMass = aa::vec3(0.0f, 0.0f, 0.0f);
        for (int i = 1; i < shapes.size(); i++)
        {
            if (shapes[i]->isSelected())
            {
                numberOfSelectedShapes++;
                centerOfMass += shapes[i]->getPosition();
            }
        }
        if (numberOfSelectedShapes > 0)
        {
            currentTranslationOrigin = centerOfMass / numberOfSelectedShapes;
            if (numberOfSelectedShapes > 1)
            {
                centerOfGravityIndicator.Draw(currentTranslationOrigin);
            }
        }
        else
        {
            currentTranslationOrigin = cursor.getPosition();
        }
    }

    //Box Select
    if (boxSelectActive)
    {
        aa::vec2 tl(boxSelectOrigin.x / camera.windowWidth * 2.0f - 1.0f, boxSelectOrigin.y / camera.windowHeight * -2.0f + 1.0f);
        aa::vec2 br(lastX / camera.windowWidth * 2.0f - 1.0f, lastY / camera.windowHeight * -2.0f + 1.0f);
        boxSelect.Draw(tl, br);
    }
}

void Scene::RemoveMarkedObjects()
{
    for (int i = 1; i < shapes.size(); i++)
    {
        Line* linePointer = dynamic_cast<Line*>(shapes[i]);
        if (linePointer != nullptr)
        {
            linePointer->RemoveDeletedPoints();
        }
    }
    for (int i = 1; i < shapes.size(); i++)
    {
        if (shapes[i]->isMarkedForDeletion())
        {
            delete shapes[i];
            shapes.erase(shapes.begin() + i);
            i--;
        }
    }
}

void Scene::AddShape()
{
    bool wasAShapeAdded = true;
	bool isADerivedShape = false;
    switch (currentItemSelectedForAdding)
    {
	case 0: // Torus
        shapes.push_back(new Torus(1.0f, 0.3f, 50, 50));
        shapes[shapes.size() - 1]->setShader(shader);
        break;
	case 1: // Ellipsoid
        std::cout << "This option has been locked, as it is out of the scope of MKMG.\n";
        break;
        shapes.push_back(new Ellipsoid(1.0f, 1.2f, 0.8f, 50));
        shapes[shapes.size() - 1]->setShader(shader);
        break;
    case 2: // Point
    {
        Line* selectedLine = nullptr;
        int selectedLinesCount = 0;
        for (int i = 1; i < shapes.size(); i++)
        {
            Line* current = dynamic_cast<Line*>(shapes[i]);
            if (current && current->isSelected())
            {
                selectedLinesCount++;
                selectedLine = current; // store the correct one
            }
        }
        Point* newPoint = new Point(aa::vec3(0.0f, 0.0f, 0.0f));
        shapes.push_back(newPoint);
        shapes[shapes.size() - 1]->setShader(shader);
        if (selectedLinesCount == 1)
        {
            selectedLine->AddPoint(newPoint);
        }
    }
    break;
	case 3: // Line
    {
        std::vector<Point*> selectedPoints;
        for (int i = 1; i < shapes.size(); i++)
        {
            Point* pointer;
            if (pointer = dynamic_cast<Point*>(shapes[i]))
            {
                if (pointer->isSelected())
                    selectedPoints.push_back(pointer);
            }
        }
        shapes.push_back(new Line(selectedPoints));
        shapes[shapes.size() - 1]->setShader(shader);
        isADerivedShape = true;
    }
    break;
	case 4: // Bezier Curve
    {
        std::vector<Point*> selectedPoints;
        for (int i = 1; i < shapes.size(); i++)
        {
            Point* pointer;
            if (pointer = dynamic_cast<Point*>(shapes[i]))
            {
                if (pointer->isSelected())
                    selectedPoints.push_back(pointer);
            }
        }
        shapes.push_back(new BezierCurve(selectedPoints));
        shapes[shapes.size() - 1]->setShader(tesselationShader);
        isADerivedShape = true;
    }
    break;
    default:
        std::cerr << "Shape not implemented yet.\n";
        wasAShapeAdded = false;
        break;
    }
    if (wasAShapeAdded&&(!isADerivedShape))
    {
        shapes[shapes.size() - 1]->TranslateAndConfirm(cursor.getPosition());
    }
}