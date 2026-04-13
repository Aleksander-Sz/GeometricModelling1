#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
	figures__REFACTORING.push_back(ShapeTable::GetShapeID(&cursor));
	cursor.setShader(shader);
	centerOfGravityIndicator.setShader(shader);
}
void Scene::LockXAxis()
{
    if (!grabEnabled && !rotatingEnabled)
        return;
    if (lockedAxis == X)
    {
        if (rotatingEnabled)
            return; // not allowing for unlocked movement in grab mode
        lockedAxis = NONE;
    }
    else
    {
        lockedAxis = X;
        movementAxis.SetAxis(X);
    }
}
void Scene::LockYAxis()
{
    if (!grabEnabled && !rotatingEnabled)
        return;
    if (lockedAxis == Y)
    {
        if (rotatingEnabled)
            return; // not allowing for unlocked movement in grab mode
        lockedAxis = NONE;
    }
    else
    {
        lockedAxis = Y;
        movementAxis.SetAxis(Y);
    }
}
void Scene::LockZAxis()
{
    if (!grabEnabled && !rotatingEnabled)
        return;
    if (lockedAxis == Z)
    {
        if (rotatingEnabled)
            return; // not allowing for unlocked movement in grab mode
        lockedAxis = NONE;
    }
    else
    {
        lockedAxis = Z;
        movementAxis.SetAxis(Z);
    }
}
void Scene::LockXYAxis()
{
    if (!grabEnabled)
        return;
    if (lockedAxis == notZ)
    {
        if (rotatingEnabled)
            return; // not allowing for unlocked movement in grab mode
        lockedAxis = NONE;
    }
    else
    {
        lockedAxis = notZ;
        movementAxis.SetAxis(notZ); // to be updated
    }
}
void Scene::LockXZAxis()
{
    if (!grabEnabled)
        return;
    if (lockedAxis == notY)
    {
        if (rotatingEnabled)
            return; // not allowing for unlocked movement in grab mode
        lockedAxis = NONE;
    }
    else
    {
        lockedAxis = notY;
        movementAxis.SetAxis(notY); // to be updated
    }
}
void Scene::LockYZAxis()
{
    if (!grabEnabled)
        return;
    if (lockedAxis == notX)
    {
        if (rotatingEnabled)
            return; // not allowing for unlocked movement in grab mode
        lockedAxis = NONE;
    }
    else
    {
        lockedAxis = notX;
        movementAxis.SetAxis(notX); // to be updated
    }
}
void Scene::toggleGrab()
{
    if (scalingEnabled || rotatingEnabled)
        return;
    lockedAxis = NONE;
	if (selectedShape != -1)
    {
        if(grabEnabled)
        {
            ConfirmObjectMovement();
		}
        else
        {
            grabMouseOrigin = aa::vec2(lastX, lastY);
			unlockedTranslationBackup = aa::vec3(0.0f);
        }
        grabEnabled = !grabEnabled;
    }
    else if (typeOfShapeCurrentlySelected == VIRTUAL_POINT_SELECTED)
    {
        if (grabEnabled)
        {
            ConfirmObjectMovement();
        }
        else
        {
            grabMouseOrigin = aa::vec2(lastX, lastY);
            unlockedTranslationBackup = aa::vec3(0.0f);
        }
        grabEnabled = !grabEnabled;
    }
}
void Scene::toggleScaling()
{
    if (grabEnabled || rotatingEnabled)
        return;
    lockedAxis = NONE;
    if (selectedShape != -1)
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
    lockedAxis = Y;
    movementAxis = Axis(Y);
    if (selectedShape != -1)
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
	cursor.UpdatePosition(camera, xpos, ypos, lockedAxis);
}
void Scene::LeftMouseClick()
{
    if(grabEnabled||scalingEnabled||rotatingEnabled)
    {
        ConfirmObjectMovement();
        rotatingEnabled = scalingEnabled = grabEnabled = false;
        lockedAxis = NONE;
	}
    else
    {
        Shape* previousShape = nullptr;
        if (!shiftPressed)
        {
            previousShape = ShapeTable::GetShapeByID(selectedShape);
            DeselectEverything(); // this will deselect all of the shapes
            typeOfShapeCurrentlySelected = NONE_SELECTED;
        }
        Shape* newSelectedShape = nullptr;
        //
        float minDistance = 1000.0f;
        int closestObject = 0;
        bool virtualPointIsClosest = false; // If the closest point is a virtual point belonging to a shape,
        // this variable will be set to true
        for (int i = 0; i < figures__REFACTORING.size(); i++)
        {
			// check distance from cursor to object position in screen space
            aa::vec2 objectPosition = ShapeTable::GetShapeByID(figures__REFACTORING[i])->getScreenSpacePosition(camera);
            float distance = aa::length(objectPosition - aa::vec2(lastX,lastY));
            if (distance < minDistance)
            {
                minDistance = distance;
                closestObject = i;
				virtualPointIsClosest = false;
            }

            // now we will deal with selecting virtual points
            IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(figures__REFACTORING[i]));
            if (shapeWithVirtualPoints != nullptr)
            {
                distance = shapeWithVirtualPoints->LeftClick(camera, aa::vec2(lastX, lastY));
				if (distance < minDistance)
                {
                    minDistance = distance;
                    closestObject = i;
					virtualPointIsClosest = true;
                }
            }
        }
        bool isSelected = false;
        if (minDistance < 30.0f)
        {
            if (virtualPointIsClosest) // selection logic for virtual points
            {
                IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(figures__REFACTORING[closestObject]));
                if (shapeWithVirtualPoints != nullptr)
                {
					shapeWithVirtualPoints->ConfirmSelection(shiftPressed);
                }
				typeOfShapeCurrentlySelected = VIRTUAL_POINT_SELECTED;
                // Now let's deselect all the real shapes
				DeselectEverything(false);
            }
            else // selection logic for shapes
            {
                isSelected = ShapeTable::GetShapeByID(figures__REFACTORING[closestObject])->Select();
                if (isSelected)
                {
                    newSelectedShape = ShapeTable::GetShapeByID(figures__REFACTORING[closestObject]);
                    typeOfShapeCurrentlySelected = SHAPE_SELECTED;
                    // now let's deselect all the virtual points
                    for (int i = 0; i < figures__REFACTORING.size(); i++)
                    {
                        IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(figures__REFACTORING[i]));
                        if (shapeWithVirtualPoints != nullptr)
                        {
                            if (shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
                                shapeWithVirtualPoints->ConfirmSelection(false, true);
                        }
                    }
                }
            }
        }
        //
        if (!shiftPressed && previousShape == newSelectedShape)
            return;
        if (newSelectedShape != nullptr)
        {
            std::cout << newSelectedShape->Name() << "\n";
            if (selectedShape != -1)
            {
                ;//selectedShape->Select(true); // Deselect current shape
            }
            if(selectedShape==-1)
                selectedShape = ShapeTable::GetShapeID(newSelectedShape);
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
    if (grabEnabled&&typeOfShapeCurrentlySelected==SHAPE_SELECTED)
    {
        ImGui::Text("Object Position:");
        aa::vec3 objectPosition = ShapeTable::GetShapeByID(selectedShape)->getPosition(); // TODO correct this
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
    if (lockedAxis==X)
        xLockFactor = 1.0f;
    else if (lockedAxis==Y)
        yLockFactor = 1.0f;
    else if (lockedAxis==Z)
        zLockFactor = 1.0f;
    else if (lockedAxis == notX)
    {
        yLockFactor = 1.0f;
        zLockFactor = 1.0f;
    }
    else if (lockedAxis == notY)
    {
        xLockFactor = 1.0f;
        zLockFactor = 1.0f;
    }
    else if (lockedAxis == notZ)
    {
        xLockFactor = 1.0f;
        yLockFactor = 1.0f;
    }
    else
    {
        xLockFactor = 1.0f;
        yLockFactor = 1.0f;
        zLockFactor = 1.0f;
    }
    translation.x *= xLockFactor;
    translation.y *= yLockFactor;
    translation.z *= zLockFactor;
    if (typeOfShapeCurrentlySelected == SHAPE_SELECTED)
    {
        for (int shapeID : figures__REFACTORING) // real shapes selected
        {
            if (ShapeTable::GetShapeByID(shapeID)->isSelected())
            {
                translation = (inverseSceneMatrix * aa::vec4(translation, 1.0f)).xyz;
                ShapeTable::GetShapeByID(shapeID)->Translate(translation);
            }
        }
    }
    else // Virtual points selected
    {
        for (int shapeID : figures__REFACTORING) // real shapes selected
        {
			IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(shapeID));
            if (shapeWithVirtualPoints!=nullptr&&shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
            {
                translation = (inverseSceneMatrix * aa::vec4(translation, 1.0f)).xyz;
                shapeWithVirtualPoints->VirtualPointsTranslate(translation);
            }
        }
    }
}
void Scene::ScaleSelectedObjects(float factor)
{
    aa::vec3 scaling = aa::vec3(factor, factor, factor);
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        if (ShapeTable::GetShapeByID(figures__REFACTORING[i])->isSelected())
        {
            scaling = (inverseSceneMatrix * aa::vec4(scaling, 1.0f)).xyz;
            if(transformAroundCursor)
                ShapeTable::GetShapeByID(figures__REFACTORING[i])->Scale(scaling, cursor.getPosition());
            else
                ShapeTable::GetShapeByID(figures__REFACTORING[i])->Scale(scaling, currentTranslationOrigin);
        }
    }
}
void Scene::RotateSelectedObjects(float angle, aa::Axis axis)
{
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        if (ShapeTable::GetShapeByID(figures__REFACTORING[i])->isSelected())
        {
            if (transformAroundCursor)
                ShapeTable::GetShapeByID(figures__REFACTORING[i])->Rotate(angle, axis, cursor.getPosition());
            else
                ShapeTable::GetShapeByID(figures__REFACTORING[i])->Rotate(angle, axis, currentTranslationOrigin);
        }
    }
}
void Scene::CancellObjectMovement()
{
    for (int shapeID : figures__REFACTORING)
    {
        if (ShapeTable::GetShapeByID(shapeID)->isSelected())
        {
            ShapeTable::GetShapeByID(shapeID)->CancelTransformations();
        }
        // now let's check if the object has selected virtual points
        IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(shapeID));
        if (shapeWithVirtualPoints != nullptr && shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
        {
            shapeWithVirtualPoints->VirtualPointsCancelTransformations();
        }
    }
}
void Scene::ConfirmObjectMovement()
{
    for (int shapeID : figures__REFACTORING)
    {
        if (ShapeTable::GetShapeByID(shapeID)->isSelected())
        {
            ShapeTable::GetShapeByID(shapeID)->ConfirmTransformations();
        }
        // now let's check if the object has selected virtual points
		IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(shapeID));
		if (shapeWithVirtualPoints != nullptr && shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
        {
			shapeWithVirtualPoints->VirtualPointsConfirmTransformations();
        }
    }
}
void Scene::DeselectEverything(bool includingVirtualPoints)
{
    selectedShape = -1;
    for (int i = 0; i < figures__REFACTORING.size(); i++)
    {
        ShapeTable::GetShapeByID(figures__REFACTORING[i])->Select(true);
        if (includingVirtualPoints)
        {
            IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(figures__REFACTORING[i]));
            if (shapeWithVirtualPoints != nullptr)
            {
                if (shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
                    shapeWithVirtualPoints->ConfirmSelection(false, true);
            }
        }
    }
}
void Scene::DeleteSelectedObjects()
{
    selectedShape = -1;
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        if (ShapeTable::GetShapeByID(figures__REFACTORING[i])->isSelected())
        {
            ShapeTable::GetShapeByID(figures__REFACTORING[i])->MarkForDeletion();
        }
    }
    RemoveMarkedObjects();
}
void Scene::MergeSelectedPoints()
{
    std::vector<int> pointsToBeMerged;
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        Point* point = ShapeTable::GetPointByID(figures__REFACTORING[i]);
        if (point != nullptr)
        {
            if (point->isSelected())
            {
                pointsToBeMerged.push_back(figures__REFACTORING[i]);
                // If this is at least the second point, we will remove its id from the scene shapes
                if (pointsToBeMerged.size() > 1)
                {
                    figures__REFACTORING.erase(figures__REFACTORING.begin() + i);
                    i--;
                }
            }
        }
    }
    if (pointsToBeMerged.size() < 2)
        return; // nothing to merge
    ShapeTable::MergeShapes(pointsToBeMerged);
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
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        aa::vec2 shapeLocation = ShapeTable::GetShapeByID(figures__REFACTORING[i])->getScreenSpacePosition(camera);
        if (shapeLocation.x >= leftBoundry && shapeLocation.x <= rightBoundry && shapeLocation.y >= bottomBoundry && shapeLocation.y <= topBoundry)
        {
            ShapeTable::GetShapeByID(figures__REFACTORING[i])->Select();
            selectedShape = figures__REFACTORING[i];
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
    tessellationShader.use();
	tessellationShader.setMat4("view", camera.view());
    tessellationShader.setVec2("viewportSize", aa::vec2((float)camera.windowWidth, (float)camera.windowHeight));
    shader.use();
    if (stereoscopy)
    {
        shader.setMat4("projection", camera.projectionRight());
        tessellationShader.use();
		tessellationShader.setMat4("projection", camera.projectionRight());
        shader.use();
    }
    else
    {
        shader.setMat4("projection", camera.projection());
		tessellationShader.use();
		tessellationShader.setMat4("projection", camera.projection());
        shader.use();
    }
    sceneMatrix = aa::scale(sceneScale);
    inverseSceneMatrix = aa::scale(aa::vec3(1.0f / sceneScale.x, 1.0f / sceneScale.y, 1.0f / sceneScale.z));
    shader.setMat4("scene", sceneMatrix);
	tessellationShader.use();
	tessellationShader.setMat4("scene", sceneMatrix);
    shader.use();
    if(stereoscopy)
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    glDepthMask(GL_FALSE);
    grid.Draw(camera, 'R');
    for (int i = 1; i < figures__REFACTORING.size(); i++)
        ShapeTable::GetShapeByID(figures__REFACTORING[i])->Draw();
    cursor.Draw();
    if ((grabEnabled || rotatingEnabled) && (lockedAxis!=NONE))
    {
        if(grabEnabled)
            movementAxis.Draw(shader, currentTranslationOrigin, 'R');
        else
        {
            if (transformAroundCursor)
                movementAxis.Draw(shader, cursor.getPosition(), 'R');
            else
                movementAxis.Draw(shader, currentTranslationOrigin, 'R');
        }
    }

    //Multiple Selection Center Of Gravity
    if (!grabEnabled && !scalingEnabled && !rotatingEnabled)
    {
        numberOfSelectedShapes = 0;
        aa::vec3 centerOfMass = aa::vec3(0.0f, 0.0f, 0.0f);
        if (typeOfShapeCurrentlySelected == VIRTUAL_POINT_SELECTED)
        {
			for (int i = 0; i < figures__REFACTORING.size(); i++)
            {
                IContainsVirtualPoints* shapeWithVirtualPoints = dynamic_cast<IContainsVirtualPoints*>(ShapeTable::GetShapeByID(figures__REFACTORING[i]));
                if (shapeWithVirtualPoints != nullptr && shapeWithVirtualPoints->containsSelectedVirtualPoints > 0)
                {
                    currentTranslationOrigin = shapeWithVirtualPoints->GetVirtualPointsPosition();
                }
            }
        }
        else if (typeOfShapeCurrentlySelected == SHAPE_SELECTED)
        {
            for (int i = 1; i < figures__REFACTORING.size(); i++)
            {
                if (ShapeTable::GetShapeByID(figures__REFACTORING[i])->isSelected())
                {
                    numberOfSelectedShapes++;
                    centerOfMass += ShapeTable::GetShapeByID(figures__REFACTORING[i])->getPosition();
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
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        Line* linePointer = (ShapeTable::GetLineByID(figures__REFACTORING[i]));
        if (linePointer != nullptr)
        {
            linePointer->RemoveDeletedPoints();
        }
    }
    for (int i = 1; i < figures__REFACTORING.size(); i++)
    {
        if (ShapeTable::GetShapeByID(figures__REFACTORING[i])->isMarkedForDeletion())
        {
            ShapeTable::RemoveShape(figures__REFACTORING[i]);
            figures__REFACTORING.erase(figures__REFACTORING.begin() + i);
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
    {
		Torus* newTorus = new Torus(1.0f, 0.3f, 50, 50);
		figures__REFACTORING.push_back(ShapeTable::AddShape(newTorus));
    }
        break;
	case 1: // Ellipsoid
    {
        std::cout << "This option has been locked, as it is out of the scope of MKMG.\n";
        break;
		Ellipsoid* newEllipsoid = new Ellipsoid(1.0f, 1.2f, 0.8f, 50);
        figures__REFACTORING.push_back(ShapeTable::AddShape(newEllipsoid));
        break;

    }
    case 2: // Point
    {
        Line* selectedLine = nullptr;
        int selectedLinesCount = 0;
        for (int i = 1; i < figures__REFACTORING.size(); i++)
        {
            Line* current = ShapeTable::GetLineByID(figures__REFACTORING[i]);
            if (current && current->isSelected())
            {
                selectedLinesCount++;
                selectedLine = current; // store the correct one
            }
        }
        Point* newPoint = new Point(aa::vec3(0.0f, 0.0f, 0.0f));
        int newPointID = ShapeTable::AddShape(newPoint);
		figures__REFACTORING.push_back(newPointID);
        if (selectedLinesCount == 1)
        {
            selectedLine->AddPoint(newPointID);
        }
    }
    break;
	case 3: // Line
    {
        std::vector<int> selectedPoints;
        for (int i = 1; i < figures__REFACTORING.size(); i++)
        {
            Point* pointer;
            if (pointer = ShapeTable::GetPointByID(figures__REFACTORING[i]))
            {
                if (pointer->isSelected())
                    selectedPoints.push_back(figures__REFACTORING[i]);
            }
        }
		Line* newLine = new Line(selectedPoints);
        figures__REFACTORING.push_back(ShapeTable::AddShape(newLine));
        isADerivedShape = true;
    }
    break;
	case 4: // Bezier Curve C0
    {
        std::vector<int> selectedPoints;
        for (int i = 1; i < figures__REFACTORING.size(); i++)
        {
            Point* pointer;
            if (pointer = ShapeTable::GetPointByID(figures__REFACTORING[i]))
            {
                if (pointer->isSelected())
                    selectedPoints.push_back(figures__REFACTORING[i]);
            }
        }
        BezierCurveC0* newCurve = new BezierCurveC0(selectedPoints);
		figures__REFACTORING.push_back(ShapeTable::AddShape(newCurve));
        newCurve->setTessellationShader(tessellationShader);
        isADerivedShape = true;
    }
    break;
    case 5: // Bezier Curve C1
    {
        std::vector<int> selectedPoints;
        for (int i = 1; i < figures__REFACTORING.size(); i++)
        {
            Point* pointer;
            if (pointer = ShapeTable::GetPointByID(figures__REFACTORING[i]))
            {
                if (pointer->isSelected())
                    selectedPoints.push_back(figures__REFACTORING[i]);
            }
        }
        BezierCurveC1* newCurve = new BezierCurveC1(selectedPoints);
		figures__REFACTORING.push_back(ShapeTable::AddShape(newCurve));
        newCurve->setTessellationShader(tessellationShader);
        isADerivedShape = true;
    }
    break;
    case 6: // Bezier Curve C0
    {
        std::vector<int> selectedPoints;
        for (int i = 1; i < figures__REFACTORING.size(); i++)
        {
            Point* pointer;
            if (pointer = ShapeTable::GetPointByID(figures__REFACTORING[i]))
            {
                if (pointer->isSelected())
                    selectedPoints.push_back(figures__REFACTORING[i]);
            }
        }
        BezierCurveC2* newCurve = new BezierCurveC2(selectedPoints);
        figures__REFACTORING.push_back(ShapeTable::AddShape(newCurve));
        newCurve->setTessellationShader(tessellationShader);
        isADerivedShape = true;
    }
    default:
        std::cerr << "Shape not implemented yet.\n";
        wasAShapeAdded = false;
        break;
    }
    ShapeTable::GetShapeByID(figures__REFACTORING[figures__REFACTORING.size() - 1])->setShader(shader);
    if (wasAShapeAdded&&(!isADerivedShape))
    {
        ShapeTable::GetShapeByID(figures__REFACTORING[figures__REFACTORING.size() - 1])->TranslateAndConfirm(cursor.getPosition());
    }
}