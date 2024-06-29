#include "processClass.h"

void EventsLookupTable::clear(){
    BootTriggered.clear();
    InitTriggered.clear();
    IterationTriggered.clear();
    TimeTriggered.clear();
    KeyPressedTriggered.clear();
    KeyPressingTriggered.clear();
    KeyReleasedTriggered.clear();
    MouseMovedTriggered.clear();
    MouseNotMovedTriggered.clear();
    MousePressedTriggered.clear();
    MousePressingTriggered.clear();
    MouseReleasedTriggered.clear();
    ObjectsTriggered.clear();
    VariablesTriggered.clear();
    CollisionTriggered.clear();
    EditableTextTriggered.clear();
    MovementTriggered.clear();
    StillnessTriggered.clear();
    ResizeTriggered.clear();
}

string ProcessClass::getID() const{
    return ID;
}
void ProcessClass::setID(string newID, vector<string> &listOfIDs){
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(listOfIDs, ID);
    ID = findNewUniqueID(listOfIDs, newID);
    listOfIDs.push_back(ID);
}
void ProcessClass::create(string EXE_PATH_FROM_ENGINE, bool allowNotAscii, vec2i screenSize, string initFilePath, string newID,
    string newLayerID, string newObjectID, vector<string> &listOfIDs
){
    setID(newID, listOfIDs);
    isActive = true;
    canUserInteract = true;
    
    EXE_PATH = EXE_PATH_FROM_ENGINE;
    workingDirectory = "";
    firstIteration = true;
    wasDeleteExecuted = false;
    wasNewExecuted = false;
    wasAnyEventUpdated = false;
    wereGlobalVariablesCreated = false;
    drawCameraBorders = true;
    drawTextFieldBorders = false;
    drawHitboxes = false;
    ignoreDistantObjects = false;
    drawOnlyVisibleObjects = false;
    wasMousePressedInSelectedObject = false;
    activeCameraMoveType = NONE;
    timeToInterruptMovement = 1;
    timeToInterruptParticles = 1;
    totalNumberOfBitmapLayers = 3;
    selectedObjectLayerID = "";
    selectedObjectID = "";

    printOutLogicalEvaluations = false;
    printOutInstructions = false;
    printOutStackAutomatically = false;
    reservationMultiplier = 1.5;

    if(reservationMultiplier < 1.0){
        cout << "Error: RESERVATION_MULTIPLIER is lower than 1.\n";
    }

    SelectedCamera = nullptr;
    SelectedLayer = nullptr;
    SelectedObject = nullptr;
    ActiveEditableText = nullptr;

    isDraggingScrollbar = false;

    isRendering = true;
    windowPos.set(0.0, 0.0);
    windowSize.set(screenSize.x, screenSize.y);
    minWindowSize.set(50, 50);
    std::fill_n(windowTint, 4, 1);

    Layers.push_back(LayerClass(newLayerID, layersIDs, true, vec2d(0.0, 0.0), screenSize));
    Layers.back().Objects.push_back(AncestorObject());
    Layers.back().Objects.back().primaryConstructor(newObjectID, &Layers.back().objectsIDs, Layers.back().getID(), "");
    if(initFilePath != ""){
        Layers.back().Objects.back().bindedScripts.push_back(EXE_PATH + initFilePath);
        Layers.back().Objects.back().translateAllScripts(true, allowNotAscii);
    }
    
    if(isLayersUniquenessViolated()){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Layers id uniqueness has been violeted on the start of the process!\n";
    }

    for(LayerClass & Layer : Layers){
        for(AncestorObject & obj : Layer.Objects){
            obj.createVectorsOfIds();
        }
        if(Layer.isObjectsUniquenessViolated()){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Layers id uniqueness has been violeted on the start of the process!\n";
        }
    }

    updateBaseOfTriggerableObjects();
}
void ProcessClass::clear(){
    SelectedLayer = nullptr;
    for(LayerClass & Layer : Layers){
        Layer.clear();
    }
    Layers.clear();
    SelectedCamera = nullptr;
    SelectedObject = nullptr;
    for(Camera2D & Camera : Cameras){
        Camera.clear();
    }
    Cameras.clear();
    layersIDs.clear();
    camerasIDs.clear();
    BaseOfTriggerableObjects.clear();
}
void ProcessClass::resizeWindow(vec2d newSize){
    windowSize.set(newSize);
    if(windowSize.x < minWindowSize.x){
        windowSize.x = minWindowSize.x;
    }
    if(windowSize.y < minWindowSize.y){
        windowSize.y = minWindowSize.y;
    }
}
void ProcessClass::resizeWindow(double x, double y){
    resizeWindow(vec2d(x, y));
}
bool ProcessClass::isLayersUniquenessViolated(){
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Layers.size(); i++){
        for(j = 0; j < Layers.size(); j++){
            if(i == j || Layers[i].getID() != Layers[j].getID()){
                continue;
            }
            if(!violated){
                cout << "\n\n";
            }
            cout << "Uniqueness has been violated by \'" << Layers[j].getID() << "\' layer.\n";
            violated = true;
        }
    }

    if(violated){
        cout << "\n";
    }
    return violated;
}
bool ProcessClass::isCamerasUniquenessViolated(){
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Cameras.size(); i++){
        for(j = 0; j < Cameras.size(); j++){
            if(i == j || Cameras[i].getID() != Cameras[j].getID()){
                continue;
            }
            if(!violated){
                cout << "\n\n";
            }
            cout << "Uniqueness has been violated by \'" << Cameras[j].getID() << "\' camera.\n";
            violated = true;
        }
    }

    if(violated){
        cout << "\n";
    }
    return violated;
}

void ProcessClass::executeIteration(EngineClass & Engine, vector<ProcessClass> & Processes){
    if(Engine.closeProgram || !isActive){
        return;
    }
    Engine.Mouse.translateAllPos(-windowPos);
    switch(Engine.event.type){
        case ALLEGRO_EVENT_TIMER:
            delayEditableTextFields();
            if(ActiveEditableText != nullptr && ActiveEditableText->isEditingActive
                && ActiveEditableText->currentInputDelay > 0.0
            ){
                ActiveEditableText->currentInputDelay -= 1/FPS; 
            }
            selectCamera(true, Engine.Mouse, Engine.pressedKeys, Engine.releasedKeys, Engine.focusedProcessID);
            moveObjects(Engine.pressedKeys, Engine.Mouse);
            moveParticles(Engine.pressedKeys, Engine.releasedKeys);
            if(canUserInteract && SelectedCamera != nullptr && SelectedCamera->getIsActive() && !SelectedCamera->getIsMinimized()){
                SelectedCamera->update(Engine.pressedKeys);
                checkMouseCollisions(Engine);
            }
            
            triggerEve(Engine, Processes);

            if(Engine.closeProgram){
                return;
            }
            firstIteration = false;
            if(Engine.reboot){
                //rebooted = false;
                //firstIteration = true;
                return;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            if(!canUserInteract || Engine.focusedProcessID != getID()){
                break;
            }
            changeCursor(Engine.display, Engine.Mouse);
            moveSelectedObject(Engine.Mouse);
            dragScrollbars(Engine.Mouse);
            if(SelectedCamera != nullptr && SelectedCamera->getIsActive() && !SelectedCamera->getIsMinimized() && SelectedCamera->isVisionAffectedByMouse
                && Engine.Mouse.firstPositionInRectangle(SelectedCamera->pos, SelectedCamera->size, 2, true, SelectedCamera)
            ){
                SelectedCamera->visionShift = Engine.Mouse.getZoomedPos(SelectedCamera) - dragCameraStaringPos;
            }
            updateCamerasPositions(Engine);
            if(Engine.Mouse.didMouseMove){
                selectLettersInText(Engine.Mouse);
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if(!canUserInteract){
                break;
            }
            selectCamera(false, Engine.Mouse, Engine.pressedKeys, Engine.releasedKeys, Engine.focusedProcessID);
            if(Engine.focusedProcessID != getID()){
                break;
            }
            if(Engine.Mouse.isFirstPressed()){
                startScrollbarDragging(Engine.Mouse);
            }
            selectObject(Engine.Mouse);
            detectStartPosOfDraggingCamera(Engine.display, Engine.Mouse);
            detectStartPosOfDraggingObjects(Engine.Mouse);
            if(SelectedCamera != nullptr && SelectedCamera->getIsActive() && !SelectedCamera->getIsMinimized()){
                dragCameraStaringPos.set(Engine.Mouse.getZoomedPos(SelectedCamera)-SelectedCamera->visionShift);
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if(SelectedCamera != nullptr){
                SelectedCamera->grabbed = false;
            }
            changeCursor(Engine.display, Engine.Mouse);
            stopScrollbarDragging();
            break;
    }

    if(canUserInteract && Engine.focusedProcessID == getID()){
        if(ActiveEditableText != nullptr && ActiveEditableText->isEditingActive
            && (Engine.releasedKeys.size() > 0 || Engine.pressedKeys.size() > 0)
        ){
            ActiveEditableText->edit(Engine.releasedKeys, Engine.pressedKeys, Engine.display,
                Engine.ENABLE_al_set_clipboard_text, Engine.internalClipboard, Engine.CopiedFormatting,
                Engine.EXE_PATH, Engine.allowNotAscii);
        }
        
        if(Engine.Mouse.isPressed() || Engine.releasedKeys.size() != 0 || Engine.pressedKeys.size() != 0){
            if(Engine.key[ALLEGRO_KEY_LCTRL]){
                for(unsigned int i = 0; i < Engine.releasedKeys.size(); i++){
                    if(Engine.releasedKeys[i] >= ALLEGRO_KEY_0 && Engine.releasedKeys[i] <= ALLEGRO_KEY_9
                        && int(Layers.size()) >= Engine.releasedKeys[i]-26)
                    {
                        Layers[Engine.releasedKeys[i]-27].setIsActive(!Layers[Engine.releasedKeys[i]-27].getIsActive());
                        if(!Layers[Engine.releasedKeys[i]-27].getIsActive()){
                            unselectObject();
                        }
                    }
                }
            }
            updateEditableTextFields(Engine);
        }

        if(Engine.Mouse.didMouseMove && SelectedCamera != nullptr && SelectedCamera->getIsActive() && !SelectedCamera->getIsMinimized()
            && SelectedCamera->canZoomWithMouse
            && Engine.Mouse.inRectangle(SelectedCamera->pos, SelectedCamera->size, true, SelectedCamera)
        ){
            Engine.Mouse.updateZoomForCamera(SelectedCamera);
        }
    }
}
void ProcessClass::selectLettersInText(const MouseClass & Mouse){
    if(SelectedCamera == nullptr){
        return;
    }
    if(isDraggingScrollbar || !Mouse.isPressed(0) || SelectedObject == nullptr || ActiveEditableText == nullptr
        || !SelectedObject->getIsActive()
        || !Mouse.pressedInRectangle(
                SelectedCamera->pos + SelectedObject->getPos(), SelectedObject->getSize(), 0,
                SelectedObject->getIsScrollable(), SelectedCamera
            )
    ){
        return;
    }
    //cout << "Selected object: '" << SelectedObject->getID() << "'; Select text: " << ActiveEditableText->getID() << "\n";
    ActiveEditableText->cursorPos = 0;
    ActiveEditableText->setCursorsWithMouse(SelectedCamera->pos + SelectedObject->getPos(), Mouse, SelectedCamera);
    if(Mouse.firstPressedInRectangle(
        SelectedCamera->pos + SelectedObject->getPos()
        + ActiveEditableText->getPos(),
        ActiveEditableText->getSize(),
        0, ActiveEditableText->getIsScrollable(), SelectedCamera
    )){
        ActiveEditableText->secondCursorPos = ActiveEditableText->cursorPos;
        ActiveEditableText->localSecondCursorPos = ActiveEditableText->localCursorPos;
        ActiveEditableText->lineWithSecondCursorIdx = ActiveEditableText->lineWithCursorIdx;
        ActiveEditableText->lineWidthToSecondCursor = ActiveEditableText->lineWidthToCursor;
    }
    ActiveEditableText->divideFormattingByCursor();
}
void ProcessClass::checkMouseCollisions(EngineClass &Engine){
    for(LayerClass & Layer : Layers){
        for(AncestorObject & Object : Layer.Objects){
            for(CollisionModule & Collision : Object.CollisionContainer){
                Collision.setMouseCollision('n');
            }
        }
    }
    if(!Engine.Mouse.isReleased()){
        return;
    }
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive() || Layer.getIsDeleted()){
            continue;
        }
        bool onCamera = false;
        for(string layerID : SelectedCamera->accessibleLayersIDs){
            if(Layer.getID() == layerID){
                onCamera = true;
                break;
            }
        }
        if(!onCamera){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive() || Object.getIsDeleted()){
                continue;
            }
            for(CollisionModule & Collision : Object.CollisionContainer){
                if(!Collision.getIsActive() || Collision.getIsDeleted()){
                    continue;
                }
                Collision.setMouseCollision('n');
                vec2d pos(Collision.getPos());
                pos.translate(Layer.pos + Object.getPos() + SelectedCamera->pos);
                vec2d size(Collision.getSize());
                if((Collision.getIsCircle() && Engine.Mouse.releasedInRadius(pos, size.x, 0, Collision.getIsScrollable(), SelectedCamera))
                    || (!Collision.getIsCircle() && Engine.Mouse.releasedInRectangle(pos, size, 0, Collision.getIsScrollable(), SelectedCamera))
                ){
                    Collision.setMouseCollision('r');
                }
            }
        }
    }
}
void ProcessClass::renderOnDisplay(EngineClass & Engine){
    if(!isActive || !isRendering){
        return;
    }
    
    timeToInterruptMovement = 20000;

    timeToInterruptParticles = 500;

    al_set_target_bitmap(Engine.backbuffer);

    al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
    
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    drawEverything(Engine);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

    al_set_target_backbuffer(Engine.display);
    al_draw_bitmap(Engine.backbuffer, 0, 0, 0);

    string updatedFpsLabel = "FPS: " + intToStr(Engine.fps.get());

    if(Layers[0].Objects.size() > 0){
        if(Layers[0].Objects[0].TextContainer.size() > 0){
            Layers[0].Objects[0].TextContainer[0].modifyContentAndResize(0, updatedFpsLabel, Engine.FontContainer);
        }
    }
}
void unfocusCameras(vector<Camera2D> & Cameras, Camera2D *& SelectedCamera, string currentProcessID, string &focusedProcessID){
    for(Camera2D & Camera : Cameras){
        Camera.isFocused = false;
    }
    if(SelectedCamera != nullptr){
        SelectedCamera = nullptr;
    }
    if(currentProcessID == focusedProcessID){
        focusedProcessID = "";
    }
}
void focusCamera(vector<Camera2D> & Cameras, Camera2D *& SelectedCamera, string currentProcessID, string &focusedProcessID, Camera2D * NewSelectedCamera){
    for(Camera2D & Camera : Cameras){
        Camera.isFocused = false;
    }
    SelectedCamera = NewSelectedCamera;
    SelectedCamera->isFocused = true;
    
    //Focus all cameras from the selected camera to the root camera.
    Camera2D * LeafCamera = SelectedCamera;
    bool isRoot = true;
    while(LeafCamera->pinnedCameraID != "" && LeafCamera->pinnedCameraID != LeafCamera->getID()){
        isRoot = true;
        for(Camera2D & Camera : Cameras){
            if(LeafCamera->pinnedCameraID == Camera.getID()){
                Camera.isFocused = true;
                LeafCamera = &Camera;
                isRoot = false;
                break;
            }
        }
        if(isRoot){
            break;
        }
    }
    
    focusedProcessID = currentProcessID;
}
void ProcessClass::updateBaseOfTriggerableObjects(){
    BaseOfTriggerableObjects.clear();

    unsigned objectIndex;
    
    for(unsigned layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
        for(objectIndex = 0; objectIndex < Layers[layerIndex].Objects.size(); objectIndex++){
            for(const EveModule & Event : Layers[layerIndex].Objects[objectIndex].EveContainer){
                for(const string & type : Event.primaryTriggerTypes){
                    if(type == "on_boot"){
                        BaseOfTriggerableObjects.BootTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "on_init"){
                        BaseOfTriggerableObjects.InitTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "each_iteration"){
                        BaseOfTriggerableObjects.IterationTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "second_passed"){
                        BaseOfTriggerableObjects.TimeTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "key_pressed"){
                        BaseOfTriggerableObjects.KeyPressedTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "key_pressing"){
                        BaseOfTriggerableObjects.KeyPressingTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "key_released"){
                        BaseOfTriggerableObjects.KeyReleasedTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "mouse_moved"){
                        BaseOfTriggerableObjects.MouseMovedTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "mouse_not_moved"){
                        BaseOfTriggerableObjects.MouseNotMovedTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "mouse_pressed"){
                        BaseOfTriggerableObjects.MousePressedTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "mouse_pressing"){
                        BaseOfTriggerableObjects.MousePressingTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "mouse_released"){
                        BaseOfTriggerableObjects.MouseReleasedTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "objects"){
                        BaseOfTriggerableObjects.ObjectsTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "variables"){
                        BaseOfTriggerableObjects.VariablesTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "collision"){
                        BaseOfTriggerableObjects.CollisionTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "editables"){
                        BaseOfTriggerableObjects.EditableTextTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "movement"){
                        BaseOfTriggerableObjects.MovementTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "stillness"){
                        BaseOfTriggerableObjects.StillnessTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                    else if(type == "on_display_resize"){
                        BaseOfTriggerableObjects.ResizeTriggered.push_back(AncestorIndex(layerIndex, objectIndex));
                    }
                }
            }
        }
    }
}
void ProcessClass::detectTriggeredEvents(const EngineClass & Engine, vector <AncestorObject*> & TriggeredObjects){
    if(printOutInstructions){
        cout << "\nAll triggered objects: \n";
    }
    AncestorObject * tempObject = nullptr;
    TriggeredObjects.clear();
    if(firstIteration){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.BootTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
                if(printOutInstructions){
                    cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (on_boot), ";
                }
            }
        }
    }
    for(AncestorIndex & Index : BaseOfTriggerableObjects.InitTriggered){
        tempObject = Index.object(Layers);
        if(tempObject != nullptr && tempObject->getIsActive()){
            TriggeredObjects.push_back(&(*tempObject));
            if(printOutInstructions){
                cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (on_init), ";
            }
        }
    }
    BaseOfTriggerableObjects.InitTriggered.clear();
    for(AncestorIndex & Index : BaseOfTriggerableObjects.IterationTriggered){
        tempObject = Index.object(Layers);
        if(tempObject != nullptr && tempObject->getIsActive()){
            TriggeredObjects.push_back(&(*tempObject));
            if(printOutInstructions){
                cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (each_iteration), ";
            }
        }
    }
    if(Engine.secondHasPassed()){ 
        for(AncestorIndex & Index : BaseOfTriggerableObjects.TimeTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
                if(printOutInstructions){
                    cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (second_passed), ";
                }
            }
        }
    }
    if(canUserInteract){
        if(Engine.firstPressedKeys.size() > 0){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.KeyPressedTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (key_pressed), ";
                    }
                }
            }
        }
        if(Engine.pressedKeys.size() > 0){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.KeyPressingTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (key_pressing), ";
                    }
                }
            }
        }
        if(Engine.releasedKeys.size() > 0){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.KeyReleasedTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (key_released), ";
                    }
                }
            }
        }
        if(Engine.Mouse.didMouseMove){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.MouseMovedTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (mouse_moved), ";
                    }
                }
            }
        }
        if(!Engine.Mouse.didMouseMove){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.MouseNotMovedTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (mouse_not_moved), ";
                    }
                }
            }
        }
        if(Engine.Mouse.isFirstPressed()){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.MousePressedTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (mouse_pressed), ";
                    }
                }
            }
        }
        if(Engine.Mouse.isPressed()){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.MousePressingTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (mouse_pressing), ";
                    }
                }
            }
        }
        if(Engine.Mouse.isReleased()){
            for(AncestorIndex & Index : BaseOfTriggerableObjects.MouseReleasedTriggered){
                tempObject = Index.object(Layers);
                if(tempObject != nullptr && tempObject->getIsActive()){
                    TriggeredObjects.push_back(&(*tempObject));
                    if(printOutInstructions){
                        cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (mouse_released), ";
                    }
                }
            }
        }
    }
    
    if(Engine.displayResized){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.ResizeTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
                if(printOutInstructions){
                    cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (on_display_resize), ";
                }
            }
        }
    }
    
    for(AncestorIndex & Index : BaseOfTriggerableObjects.MovementTriggered){
        tempObject = Index.object(Layers);
        if(tempObject == nullptr || !tempObject->getIsActive()){
            continue;
        }
        for(const MovementModule & Movement : tempObject->MovementContainer){
            if(!Movement.getIsActive()){
                continue;
            }
            if(Movement.isMoving()){
                TriggeredObjects.push_back(&(*tempObject));
                if(printOutInstructions){
                    cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (movement), ";
                }
                break;
            }
        }
    }
    bool triggered;
    for(AncestorIndex & Index : BaseOfTriggerableObjects.StillnessTriggered){
        tempObject = Index.object(Layers);
        if(tempObject == nullptr || !tempObject->getIsActive()){
            continue;
        }
        triggered = true;
        for(const MovementModule & Movement : tempObject->MovementContainer){
            if(Movement.getIsActive() && Movement.isMoving()){
                triggered = false;
                break;
            }
        }
        if(triggered){
            TriggeredObjects.push_back(&(*tempObject));
            if(printOutInstructions){
                cout << tempObject->getLayerID() << "::" << tempObject->getID() << " (stillness), ";
            }
        }
    }

    //Remove duplicates
    for(unsigned int i=0; i < TriggeredObjects.size(); i++){
        for(unsigned int j=i+1; j < TriggeredObjects.size(); j++){
            if(TriggeredObjects[i] != TriggeredObjects[j]){
                continue;
            }
            TriggeredObjects.erase(TriggeredObjects.begin()+j);
            j--;
        }
    }

    bool noEventsActive;
    for(auto Object = TriggeredObjects.begin(); Object != TriggeredObjects.end();){
        noEventsActive = true;
        for(const EveModule & Event : (*Object)->EveContainer){
            if(Event.getIsActive()){
                noEventsActive = false;
                break;
            }
        }
        if(noEventsActive){
            Object = TriggeredObjects.erase(Object);
        }
        else{
            ++Object;
        }
    }
    if(printOutInstructions){
        cout << "\n";
    }
}
size_t ProcessClass::countLayers() const{
    return Layers.size();
}
size_t ProcessClass::countCameras() const{
    return Cameras.size();
}
void ContextClass::clear(){
    ID = "";
    type = "";
    Variables.clear();
    BasePointers.clear();
    Modules.Texts.clear();
    Modules.EditableTexts.clear();
    Modules.SuperTexts.clear();
    Modules.SuperEditableTexts.clear();
    Modules.Images.clear();
    Modules.Movements.clear();
    Modules.Collisions.clear();
    Modules.Particles.clear();
    Modules.Events.clear();
    Modules.Variables.clear();
    Modules.Scrollbars.clear();
    Objects.clear();
    Layers.clear();
    Cameras.clear();
}
size_t ContextClass::getVectorSize() const{
    if(type == "value"){
        return Variables.size();
    }
    if(type == "pointer"){
        return BasePointers.size();
    }
    if(type == "variable"){
        return Modules.Variables.size();
    }
    if(type == "camera"){
        return Cameras.size();
    }
    if(type == "layer"){
        return Layers.size();
    }
    if(type == "object"){
        return Objects.size();
    }
    if(type == "text"){
        return Modules.Texts.size();
    }
    if(type == "editable_text"){
        return Modules.EditableTexts.size();
    }
    if(type == "image"){
        return Modules.Images.size();
    }
    if(type == "movement"){
        return Modules.Movements.size();
    }
    if(type == "collision"){
        return Modules.Collisions.size();
    }
    if(type == "particles"){
        return Modules.Particles.size();
    }
    if(type == "event"){
        return Modules.Events.size();
    }
    if(type == "scrollbar"){
        return Modules.Scrollbars.size();
    }
    if(type == "primitives"){
        return Modules.Primitives.size();
    }
    if(type == "vector"){
        return Modules.Vectors.size();
    }
    return 0;
}
string ContextClass::getValue(EventDescription EventIds){
    string buffer = "";
    if(type == ""){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context does not have type.\n";
    }
    else if(type == "value"){
        buffer = "[";
        for(const VariableModule & Variable: Variables){
            buffer += Variable.getStringUnsafe();
            buffer += ", ";
        }
        buffer += "]<";
        buffer += uIntToStr(Variables.size());
        buffer += ">";
    }
    else if(type == "pointer"){
        buffer = "[";
        for(const BasePointersStruct & Pointer: BasePointers){
            buffer += Pointer.getString();
            if(Pointer.readOnly){
                buffer += "(R)";
            }
            buffer += ", ";
        }
        buffer += "]<";
        buffer += uIntToStr(BasePointers.size());
        buffer += ">";
    }
    else if(type == "variable"){
        buffer = "[";
        for(const VariableModule * Variable: Modules.Variables){
            if(Variable == nullptr){
                buffer += "<nullptr>, ";
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In the context \'" << ID << "\' the pointer to the variable has a nullptr value.\n";
                continue;
            }
            buffer += Variable->getStringUnsafe();
            buffer += ", ";
        }
        buffer += "]<";
        buffer += uIntToStr(Modules.Variables.size());
        buffer += ">";
    }
    else if(type == "vector"){
        buffer = "[";
        for(const VectorModule * Vector: Modules.Vectors){
            if(Vector == nullptr){
                buffer += "<nullptr>, ";
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In the context \'" << ID << "\' the pointer to the vector has a nullptr value.\n";
                continue;
            }
            buffer += "[";
            for(size_t i = 0; i < Vector->getSize(); i++){
                buffer += Vector->getAnyStringValueUnsafe(i) + ", ";
            }
            buffer += "]";

            buffer += ", ";
        }
        buffer += "]<";
        buffer += uIntToStr(Modules.Variables.size());
        buffer += ">";
    }
    else{
        buffer += "<";
        buffer += uIntToStr(getVectorSize());
        buffer += ">";
    }
    if(readOnly){
        buffer += "(R)";
    }

    for(char & letter : buffer){
        if(letter == '\n'){
            letter = '~'; //New line
        }
    }

    return buffer;
}
bool ContextClass::getUnsignedOrAbort(unsigned &number, EngineInstr instruction, EventDescription EventIds){
    int temp = 0;
    if(type == "value"){
        if(Variables.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'"
                << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Variables.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'"
                << transInstrToStr(instruction)
                << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        temp = Variables.back().getInt();
    }
    else if(type == "pointer"){
        if(BasePointers.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(BasePointers.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        temp = BasePointers.back().getInt();
    }
    else if(type == "variable"){
        if(Modules.Variables.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Modules.Variables.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        temp = Modules.Variables.back()->getInt();
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context \'" << ID
            << "\' has invalid type: \'" << type << "\'.\n";
        return false;
    }
    if(temp < 0){
        number = 0;
        cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Value is not unsigned - returning 0.\n";
    }
    number = temp;
    return true;
}
bool ContextClass::getIntOrAbort(int &number, EngineInstr instruction, EventDescription EventIds){
    if(type == "value"){
        if(Variables.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Variables.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        number = Variables.back().getInt();
    }
    else if(type == "pointer"){
        if(BasePointers.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(BasePointers.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        number = BasePointers.back().getInt();
    }
    else if(type == "variable"){
        if(Modules.Variables.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Modules.Variables.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        number = Modules.Variables.back()->getInt();
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context \'" << ID
            << "\' has invalid type: \'" << type << "\'.\n";
        return false;
    }
    return true;
}
bool ContextClass::getStringOrAbort(string & text, EngineInstr instruction, EventDescription EventIds){
    if(type == "value"){
        if(Variables.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Variables.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        text = Variables.back().getString();
    }
    else if(type == "pointer"){
        if(BasePointers.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(BasePointers.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        text = BasePointers.back().getString();
    }
    else if(type == "variable"){
        if(Modules.Variables.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Modules.Variables.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        text = Modules.Variables.back()->getString();
    }
    else if(type == "vector"){
        if(Modules.Vectors.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context is empty.\n";
            return false;
        }
        if(Modules.Vectors.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context has more than 1 value - only the last value will be used.\n";
        }
        vector <string> textVector = Modules.Vectors.back()->getAllStrings(); 
        text = "";
        for(string textIter : textVector){
            text += textIter + "\n";
        }
        text.pop_back(); //Remove the last new line.
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\': Context \'" << ID
            << "\' has invalid type: '" << type << "'.\n";
        return false;
    }
    return true;
}
bool ContextClass::getStringOrIgnore(string & text, EngineInstr instruction){
    if(type == "value"){
        if(Variables.size() == 0 || Variables.back().getType() != 's'){
            return false;
        }
        text = Variables.back().getStringUnsafe();
    }
    else if(type == "pointer"){
        if(BasePointers.size() == 0 || BasePointers.back().type != "string"){
            return false;
        }
        text = BasePointers.back().getString();
    }
    else if(type == "variable"){
        if(Modules.Variables.size() == 0 || Modules.Variables.back()->getType() != 's'){
            return false;
        }
        text = Modules.Variables.back()->getStringUnsafe();
    }
    else{
        return false;
    }
    return true;
}
bool ContextClass::getStringVectorOrIgnore(vector<string> & result, EngineInstr instruction){
    if(type == "value"){
        for(const VariableModule & Variable : Variables){
            result.push_back(Variable.getAnyValue());
        }
    }
    else if(type == "pointer"){
        for(const BasePointersStruct & Pointer : BasePointers){
            result.push_back(Pointer.getString());
        }
    }
    else if(type == "variable"){
        for(VariableModule * Variable : Modules.Variables){
            result.push_back(Variable->getAnyValue());
        }
    }
    else if(type == "vector"){
        for(VectorModule * Vector : Modules.Vectors){
            vector <string> extractedVector = Vector->getAllStrings();
            result.insert(result.end(), extractedVector.begin(), extractedVector.end());
        }
    }
    else{
        return false;
    }
    return true;
}
ContextClass::ContextClass()
{
    ID = "";
    type = "";
    readOnly = false;
}
template<typename T>
void ContextClass::addBasePointer(T * pointer){
    if(pointer == nullptr){
        return;
    }
    BasePointers.push_back(BasePointersStruct());
    BasePointers.back().setPointer(pointer);
    type = "pointer";
}
template<typename T>
void ContextClass::setFirstBasePointer(T * pointer){
    if(pointer == nullptr){
        return;
    }
    if(BasePointers.size() == 0){
        BasePointers.push_back(BasePointersStruct());
        type = "pointer";
    }
    BasePointers.back().setPointer(pointer);
}
void ContextClass::addModule(TextModule * Module){
    if(Module != nullptr){
        Modules.Texts.push_back(Module);
    }
}
void ContextClass::addModule(EditableTextModule * Module){
    if(Module != nullptr){
        Modules.EditableTexts.push_back(Module);
    }
}
void ContextClass::addModule(SuperTextModule *Module){
    if(Module != nullptr){
        Modules.SuperTexts.push_back(Module);
    }
}
void ContextClass::addModule(SuperEditableTextModule *Module){
    if(Module != nullptr){
        Modules.SuperEditableTexts.push_back(Module);
    }
}
void ContextClass::addModule(ImageModule *Module)
{
    if(Module != nullptr){
        Modules.Images.push_back(Module);
    }
}
void ContextClass::addModule(MovementModule * Module){
    if(Module != nullptr){
        Modules.Movements.push_back(Module);
    }
}
void ContextClass::addModule(CollisionModule * Module){
    if(Module != nullptr){
        Modules.Collisions.push_back(Module);
    }
}
void ContextClass::addModule(ParticleEffectModule * Module){
    if(Module != nullptr){
        Modules.Particles.push_back(Module);
    }
}
void ContextClass::addModule(EveModule * Module){
    if(Module != nullptr){
        Modules.Events.push_back(Module);
    }
}
void ContextClass::addModule(VariableModule * Module){
    if(Module != nullptr){
        Modules.Variables.push_back(Module);
    }
}
void ContextClass::addModule(ScrollbarModule * Module){
    if(Module != nullptr){
        Modules.Scrollbars.push_back(Module);
    }
}
void ContextClass::addModule(PrimitivesModule * Module){
    if(Module != nullptr){
        Modules.Primitives.push_back(Module);
    }
}
void ContextClass::addModule(VectorModule * Module){
    if(Module != nullptr){
        Modules.Vectors.push_back(Module);
    }
}
void ContextClass::setFirstModule(TextModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Texts.size() == 0){
        type = "text";
        addModule(Module);
    }
    else{
        Modules.Texts.back() = Module;
    }
}
void ContextClass::setFirstModule(EditableTextModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.EditableTexts.size() == 0){
        type = "editable_text";
        addModule(Module);
    }
    else{
        Modules.EditableTexts.back() = Module;
    }
}
void ContextClass::setFirstModule(SuperTextModule *Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.SuperTexts.size() == 0){
        type = "super_text";
        addModule(Module);
    }
    else{
        Modules.SuperTexts.back() = Module;
    }
}
void ContextClass::setFirstModule(SuperEditableTextModule *Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.SuperEditableTexts.size() == 0){
        type = "super_editable_text";
        addModule(Module);
    }
    else{
        Modules.SuperEditableTexts.back() = Module;
    }
}
void ContextClass::setFirstModule(ImageModule *Module)
{
    if(Module == nullptr){
        return;
    }
    if(Modules.Images.size() == 0){
        type = "image";
        addModule(Module);
    }
    else{
        Modules.Images.back() = Module;
    }
}
void ContextClass::setFirstModule(MovementModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Movements.size() == 0){
        type = "movement";
        addModule(Module);
    }
    else{
        Modules.Movements.back() = Module;
    }
}
void ContextClass::setFirstModule(CollisionModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Collisions.size() == 0){
        type = "collision";
        addModule(Module);
    }
    else{
        Modules.Collisions.back() = Module;
    }
}
void ContextClass::setFirstModule(ParticleEffectModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Particles.size() == 0){
        type = "particles";
        addModule(Module);
    }
    else{
        Modules.Particles.back() = Module;
    }
}
void ContextClass::setFirstModule(EveModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Events.size() == 0){
        type = "event";
        addModule(Module);
    }
    else{
        Modules.Events.back() = Module;
    }
}
void ContextClass::setFirstModule(VariableModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Variables.size() == 0){
        type = "variable";
        addModule(Module);
    }
    else{
        Modules.Variables.back() = Module;
    }
}
void ContextClass::setFirstModule(ScrollbarModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Scrollbars.size() == 0){
        type = "scrollbar";
        addModule(Module);
    }
    else{
        Modules.Scrollbars.back() = Module;
    }
}
void ContextClass::setFirstModule(PrimitivesModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Primitives.size() == 0){
        type = "primitives";
        addModule(Module);
    }
    else{
        Modules.Primitives.back() = Module;
    }
}
void ContextClass::setFirstModule(VectorModule * Module){
    if(Module == nullptr){
        return;
    }
    if(Modules.Vectors.size() == 0){
        type = "vector";
        addModule(Module);
    }
    else{
        Modules.Vectors.back() = Module;
    }
}
void ContextClass::leaveOneRandomBasePointer(){
    if(BasePointers.size() == 0){
        return;
    }
    BasePointersStruct randomPointer = BasePointers[rand() % BasePointers.size()];
    BasePointers.clear();
    BasePointers.push_back(randomPointer);
}
unsigned ContextClass::size() const{
    return Cameras.size() + Layers.size() + Objects.size() + Modules.size();
}
template <class Entity>
void chooseRandomEntity(vector <Entity*> & Vector){
    if(Vector.size() == 0){
        return;
    }
    Entity * randomEntity = Vector[rand() % Vector.size()];
    Vector.clear();
    Vector.push_back(randomEntity);
}
bool ProcessClass::chooseRandomModule(ContextClass & NewContext){
    if(NewContext.type == "text"){
        chooseRandomEntity(NewContext.Modules.Texts);
    }
    else if(NewContext.type == "editable_text"){
        chooseRandomEntity(NewContext.Modules.EditableTexts);
    }
    else if(NewContext.type == "super_text"){
        chooseRandomEntity(NewContext.Modules.SuperTexts);
    }
    else if(NewContext.type == "super_editable_text"){
        chooseRandomEntity(NewContext.Modules.SuperEditableTexts);
    }
    else if(NewContext.type == "image"){
        chooseRandomEntity(NewContext.Modules.Images);
    }
    else if(NewContext.type == "movement"){
        chooseRandomEntity(NewContext.Modules.Movements);
    }
    else if(NewContext.type == "collision"){
        chooseRandomEntity(NewContext.Modules.Collisions);
    }
    else if(NewContext.type == "particles"){
        chooseRandomEntity(NewContext.Modules.Particles);
    }
    else if(NewContext.type == "event"){
        chooseRandomEntity(NewContext.Modules.Events);
    }
    else if(NewContext.type == "variable"){
        chooseRandomEntity(NewContext.Modules.Variables);
    }
    else if(NewContext.type == "scrollbar"){
        chooseRandomEntity(NewContext.Modules.Scrollbars);
    }
    else if(NewContext.type == "primitives"){
        chooseRandomEntity(NewContext.Modules.Primitives);
    }
    else if(NewContext.type == "vector"){
        chooseRandomEntity(NewContext.Modules.Vectors);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Type \'" << NewContext.type << "\' does not exist.\n";
        return false;
    }
    return true;
}

void ProcessClass::aggregateCameras(OperaClass &Operation, ContextClass &NewContext, vector<Camera2D*> AggregatedCameras,
    const EngineClass & Engine, vector<ContextClass> &EventContext
){
    Camera2D * Camera = nullptr;
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == EngineInstr::last){
        if(AggregatedCameras.size() > 0){
            Camera = lastNotDeletedInVector(AggregatedCameras);
        }
        else if(Cameras.size() > 0){
            Camera = lastNotDeletedInVector(Cameras);
        }
        if(Camera != nullptr && (Operation.Location.cameraID == "" || Operation.Location.cameraID == Camera->getID())){
            findContextInCamera(Operation.Location.attribute, NewContext, Camera);
        }
        return;
    }

    ContextClass TempContext;
    AncestorObject * TempObject = new AncestorObject();
    LayerClass * TempLayer = new LayerClass();
    vector <LayerClass> TempLayers;
    
    unsigned i = 0, vector_end = 0;

    if(AggregatedCameras.size() > 0){
        vector_end = AggregatedCameras.size();
    }
    else{
        vector_end = Cameras.size();
    }
    
    while(i < vector_end){
        Camera = nullptr;
        if(AggregatedCameras.size() > 0){
            Camera = AggregatedCameras[i];
        }
        else if(Cameras.size() > 0){
            Camera = &Cameras[i];
        }
        if(Camera == nullptr || Camera->getIsDeleted() || (Operation.Location.cameraID != "" && Operation.Location.cameraID != Camera->getID())){
            i++;
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.cameraID = Camera->getID();
        }
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, TempObject, TempLayer, Engine, EventContext) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInCamera(Operation.Location.attribute, NewContext, Camera);
            }
            else{
                TempContext.clear();
                findContextInCamera(Operation.Location.attribute, TempContext, Camera);
                if(TempContext.type != ""){
                    NewContext = TempContext;
                }
            }
        }
        if(Operation.Location.cameraID != "" || (Operation.instruction == EngineInstr::first && (NewContext.BasePointers.size() == 1 || NewContext.Cameras.size() == 1))){
            break;
        }
        i++;
    }

    delete TempObject;
    delete TempLayer;
    if(Operation.instruction == EngineInstr::random_i){
        if(NewContext.type == "camera"){
            chooseRandomEntity(NewContext.Cameras);
        }
        else{
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregateLayers(OperaClass & Operation, ContextClass & NewContext, vector <LayerClass*> AggregatedLayers,
    const EngineClass & Engine, vector<ContextClass> &EventContext
){
    LayerClass * Layer = nullptr;
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == EngineInstr::last){
        if(AggregatedLayers.size() > 0){
            Layer = lastNotDeletedInVector(AggregatedLayers);
        }
        else if(Cameras.size() > 0){
            Layer = lastNotDeletedInVector(Layers);
        }
        if(Layer != nullptr && (Operation.Location.layerID == "" || Operation.Location.layerID == Layer->getID())){
            findContextInLayer(Operation.Location, NewContext, Layer);
        }
        return;
    }

    ContextClass TempContext;
    AncestorObject * TempObject = new AncestorObject();
    
    unsigned i = 0, vector_end = 0;

    if(AggregatedLayers.size() > 0){
        vector_end = AggregatedLayers.size();
    }
    else if(Layers.size() > 0){
        vector_end = Layers.size();
    }

    while(i < vector_end){
        Layer = nullptr;
        if(AggregatedLayers.size() > 0){
            Layer = AggregatedLayers[i];
        }
        else if(Layers.size() > 0){
            Layer = &Layers[i];
        }
        if(Layer == nullptr || Layer->getIsDeleted() || (Operation.Location.layerID != "" && Operation.Location.layerID != Layer->getID())){
            i++;
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.layerID = Layer->getID();
        }
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, TempObject, Layer, Engine, EventContext) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInLayer(Operation.Location, NewContext, Layer);
            }
            else{
                TempContext.clear();
                findContextInLayer(Operation.Location, TempContext, Layer);
                if(TempContext.type != ""){
                    NewContext = TempContext;
                }
            }
        }
        if(Operation.Location.layerID != "" || (Operation.instruction == EngineInstr::first && (NewContext.BasePointers.size() == 1 || NewContext.Layers.size() == 1))){
            break;
        }
        i++;
    }

    delete TempObject;
    if(Operation.instruction == EngineInstr::random_i){
        if(NewContext.type == "layer"){
            chooseRandomEntity(NewContext.Layers);
        }
        else if(NewContext.type == "object"){
            chooseRandomEntity(NewContext.Objects);
        }
        else if(!chooseRandomModule(NewContext)){
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregateObjects(OperaClass &Operation, ContextClass &NewContext, vector<AncestorObject *> AggregatedObjects,
    const EngineClass &Engine, vector<ContextClass> &EventContext
){
    if(AggregatedObjects.size() == 0){
        return;
    }
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == EngineInstr::last){
        AncestorObject * Object = lastNotDeletedInVector(AggregatedObjects);
        if(Object != nullptr && (Operation.Location.objectID == "" || Operation.Location.objectID == Object->getID())){
            findContextInObject(Operation.Location, NewContext, Object);
        }
        return;
    }

    ContextClass TempContext;
    LayerClass * EmptyLayer = nullptr;

    for(AncestorObject * Object : AggregatedObjects){
        if(Object->getIsDeleted() || (Operation.Location.objectID != "" && Operation.Location.objectID != Object->getID())){
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.layerID = Object->getLayerID();
            Condition.Location.objectID = Object->getID();
        }
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, Object, EmptyLayer, Engine, EventContext) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInObject(Operation.Location, NewContext, Object);
            }
            else{
                TempContext.clear();
                findContextInObject(Operation.Location, TempContext, Object);
                if(TempContext.type != ""){
                    NewContext = TempContext;
                }
            }
        }
        if(Operation.instruction == EngineInstr::first && (NewContext.BasePointers.size() == 1 || NewContext.Objects.size() == 1)){
            break;
        }
    }

    if(Operation.instruction == EngineInstr::random_i){
        if(NewContext.type == "object"){
            chooseRandomEntity(NewContext.Objects);
        }
        else if(!chooseRandomModule(NewContext)){
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
template<class ModuleClass>
void ProcessClass::aggregateModuleContextFromVectors(vector<ModuleClass*> AggregatedModules, const string & aggregatedType, OperaClass & Operation,
    ContextClass & NewContext, AncestorObject * Object, const EngineClass & Engine, vector<ContextClass> &EventContext
){
    ContextClass TempContext;
    LayerClass * EmptyLayer = nullptr;

    for(ModuleClass * Instance : AggregatedModules){
        if(Instance == nullptr || Instance->getIsDeleted() || (Operation.Location.moduleID != "" && Operation.Location.moduleID != Instance->getID())){
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.layerID = Instance->getLayerID();
            Condition.Location.objectID = Instance->getObjectID();
            Condition.Location.moduleID = Instance->getID();
        }
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, Object, EmptyLayer, Engine, EventContext) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInModule(aggregatedType, Operation.Location.attribute, NewContext, Instance);
            }
            else{
                TempContext.clear();
                findContextInModule(aggregatedType, Operation.Location.attribute, TempContext, Instance);
                if(TempContext.type != ""){
                    NewContext = TempContext;
                }
            }
        }
        if(Operation.instruction == EngineInstr::first && NewContext.Modules.hasInstanceOfAnyModule()){
            return;
        }
    }
}
template<class ModuleClass>
void ProcessClass::findContextInModule(string type, string attribute, ContextClass & NewContext, ModuleClass * Module){
    if(Module == nullptr){
        return;
    }
    if(attribute == "" || isStringInGroup(attribute, 13, "text", "editable_text", "super_text", "super_editable_text", "image", "movement",
        "collision", "particles", "event", "variable", "scrollbar", "primitives", "vector"
    )){
        NewContext.type = type;
        NewContext.addModule(Module);
    }
    else{
        vector <BasePointersStruct> BasePointers;
        Module->getContext(attribute, BasePointers);
        if(BasePointers.size() > 0){
            NewContext.BasePointers.insert(NewContext.BasePointers.end(), BasePointers.begin(), BasePointers.end());
            NewContext.type = "pointer";
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context not found.\n";
        }
    }
}
template<class ModuleClass>
void ProcessClass::getContextFromModuleVectorById(string moduleType, string moduleID, string attribute, ContextClass & NewContext, vector <ModuleClass*> AggregatedModules){
    for(ModuleClass * Module : AggregatedModules){
        if(Module == nullptr || (moduleID != "" && Module->getID() != moduleID)){
            continue;
        }
        if(Module->getIsDeleted()){
            if(moduleID != ""){
                return;
            }
            continue;
        }

        if(attribute == ""){
            NewContext.type = moduleType;
            NewContext.addModule(Module);
        }
        else{
            vector <BasePointersStruct> BasePointers;
            Module->getContext(attribute, BasePointers);
            if(BasePointers.size() > 0){
                NewContext.BasePointers.insert(NewContext.BasePointers.end(), BasePointers.begin(), BasePointers.end());
                NewContext.type = "pointer";
            }
            else{
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No type.\n";
            }
        }

        if(moduleID != ""){
            return;
        }
    }
}
template<class ModuleClass>
ModuleClass * findLastModule(vector <ModuleClass*> & Vector, const string & moduleID){
    ModuleClass * Module = lastNotDeletedInVector(Vector);
    if(Module == nullptr || moduleID == "" || moduleID == Module->getID()){
        return Module;
    }
    return nullptr;
}
void ProcessClass::aggregateModules(OperaClass & Operation, ContextClass & NewContext, ContextClass * OldContext,
    vector<ContextClass> &EventContext, const EngineClass & Engine
){
    ModulesPointers * AggregatedModules = &OldContext->Modules;
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == EngineInstr::last){
        if(OldContext->type == "text" && AggregatedModules->Texts.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Texts, Operation.Location.moduleID));
        }
        else if(OldContext->type == "editable_text" && AggregatedModules->EditableTexts.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->EditableTexts, Operation.Location.moduleID));
        }
        else if(OldContext->type == "super_text" && AggregatedModules->SuperTexts.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->SuperTexts, Operation.Location.moduleID));
        }
        else if(OldContext->type == "super_editable_text" && AggregatedModules->SuperEditableTexts.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->SuperEditableTexts, Operation.Location.moduleID));
        }
        else if(OldContext->type == "image" && AggregatedModules->Images.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Images, Operation.Location.moduleID));
        }
        else if(OldContext->type == "movement" && AggregatedModules->Movements.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Movements, Operation.Location.moduleID));
        }
        else if(OldContext->type == "collision" && AggregatedModules->Collisions.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Collisions, Operation.Location.moduleID));
        }
        else if(OldContext->type == "particles" && AggregatedModules->Particles.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Particles, Operation.Location.moduleID));
        }
        else if(OldContext->type == "event" && AggregatedModules->Events.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Events, Operation.Location.moduleID));
        }
        else if(OldContext->type == "variable" && AggregatedModules->Variables.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Variables, Operation.Location.moduleID));
        }
        else if(OldContext->type == "scrollbar" && AggregatedModules->Scrollbars.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Scrollbars, Operation.Location.moduleID));
        }
        else if(OldContext->type == "primitives" && AggregatedModules->Primitives.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Primitives, Operation.Location.moduleID));
        }
        else if(OldContext->type == "vector" && AggregatedModules->Vectors.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Vectors, Operation.Location.moduleID));
        }
        return;
    }

    AncestorObject * EmptyObject = new AncestorObject();
    if(OldContext->type == "text"){
        aggregateModuleContextFromVectors(AggregatedModules->Texts, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "editable_text"){
        aggregateModuleContextFromVectors(AggregatedModules->EditableTexts, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "super_text"){
        aggregateModuleContextFromVectors(AggregatedModules->SuperTexts, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "super_editable_text"){
        aggregateModuleContextFromVectors(AggregatedModules->SuperEditableTexts, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "image"){
        aggregateModuleContextFromVectors(AggregatedModules->Images, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "movement"){
        aggregateModuleContextFromVectors(AggregatedModules->Movements, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "collision"){
        aggregateModuleContextFromVectors(AggregatedModules->Collisions, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "particles"){
        aggregateModuleContextFromVectors(AggregatedModules->Particles, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "event"){
        aggregateModuleContextFromVectors(AggregatedModules->Events, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "variable"){
        aggregateModuleContextFromVectors(AggregatedModules->Variables, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "scrollbar"){
        aggregateModuleContextFromVectors(AggregatedModules->Scrollbars, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "primitives"){
        aggregateModuleContextFromVectors(AggregatedModules->Primitives, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "vector"){
        aggregateModuleContextFromVectors(AggregatedModules->Vectors, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Module type \'" << OldContext->type << "\' does not exist.\n";
        delete EmptyObject;
        return;
    }
    delete EmptyObject;

    if(Operation.instruction == EngineInstr::random_i){
        if(!chooseRandomModule(NewContext)){
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregatePointers(EngineInstr instruction, ContextClass & NewContext, vector <BasePointersStruct> & AggregatedPointers){
    if(instruction == EngineInstr::first){
        NewContext.BasePointers.push_back(AggregatedPointers.front());
    }
    else if(instruction == EngineInstr::last){
        NewContext.BasePointers.push_back(AggregatedPointers.back());
    }
    else if(instruction == EngineInstr::all){
        NewContext.BasePointers.insert(NewContext.BasePointers.end(), AggregatedPointers.begin(), AggregatedPointers.end());
    }
    else if(instruction == EngineInstr::random_i && AggregatedPointers.size() > 0){
        NewContext.BasePointers.push_back(AggregatedPointers[rand() % AggregatedPointers.size()]);
    }
    if(NewContext.BasePointers.size() > 0){
        NewContext.type = "pointer";
    }
}
void ProcessClass::aggregateVariables(EngineInstr instruction, ContextClass & NewContext, vector <VariableModule> & AggregatedVariables){
    if(instruction == EngineInstr::first){
        NewContext.Variables.push_back(AggregatedVariables.front());
    }
    else if(instruction == EngineInstr::last){
        NewContext.Variables.push_back(AggregatedVariables.back());
    }
    else if(instruction == EngineInstr::all){
        NewContext.Variables.insert(NewContext.Variables.end(), AggregatedVariables.begin(), AggregatedVariables.end());
    }
    else if(instruction == EngineInstr::random_i && AggregatedVariables.size() > 0){
        NewContext.Variables.push_back(AggregatedVariables[rand() % AggregatedVariables.size()]);
    }
    if(NewContext.Variables.size() > 0){
        NewContext.type = "value";
    }
}
void ProcessClass::findContextInCamera(string attribute, ContextClass & NewContext, Camera2D * Camera){
    if(Camera == nullptr){
        return;
    }
    if(attribute == "camera" || attribute == ""){
        NewContext.type = "camera";
        NewContext.Cameras.push_back(Camera);
    }
    else if(attribute == "is_Active"){
        NewContext.addBasePointer(&Camera->isActive);
    }
    else if(attribute == "id"){
        if(isStringInVector(reservedIDs, Camera->ID)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Access to the reserved ID \'" << Camera->ID << "\' address was denied.\n";
            return;
        }
        NewContext.addBasePointer(&Camera->ID);
        NewContext.readOnly = true;
    }
    else if(attribute == "pos_x"){
        NewContext.addBasePointer(&Camera->pos.x);
    }
    else if(attribute == "pos_y"){
        NewContext.addBasePointer(&Camera->pos.y);
    }
    else if(attribute == "relative_pos_x"){
        NewContext.addBasePointer(&Camera->relativePos.x);
    }
    else if(attribute == "relative_pos_y"){
        NewContext.addBasePointer(&Camera->relativePos.y);
    }
    else if(attribute == "vision_shift_x"){
        NewContext.addBasePointer(&Camera->visionShift.x);
    }
    else if(attribute == "vision_shift_y"){
        NewContext.addBasePointer(&Camera->visionShift.y);
    }
    else if(attribute == "size_x"){
        NewContext.addBasePointer(&Camera->size.x);
        NewContext.readOnly = true;
    }
    else if(attribute == "size_y"){
        NewContext.addBasePointer(&Camera->size.y);
        NewContext.readOnly = true;
    }
    else if(attribute == "zoom"){
        NewContext.addBasePointer(&Camera->zoom);
        NewContext.readOnly = true;
    }
    else if(attribute == "min_zoom"){
        NewContext.addBasePointer(&Camera->minZoom);
        NewContext.readOnly = true;
    }
    else if(attribute == "max_zoom"){
        NewContext.addBasePointer(&Camera->maxZoom);
        NewContext.readOnly = true;
    }
    else if(attribute == "speed"){
        NewContext.addBasePointer(&Camera->speed);
    }
    else if(attribute == "zoom_in_key"){
        NewContext.addBasePointer(&Camera->zoomInKey);
    }
    else if(attribute == "zoom_out_key"){
        NewContext.addBasePointer(&Camera->zoomOutKey);
    }
    else if(attribute == "zoom_reset_key"){
        NewContext.addBasePointer(&Camera->zoomResetKey);
    }
    else if(attribute == "up_key"){
        NewContext.addBasePointer(&Camera->upKey);
    }
    else if(attribute == "right_key"){
        NewContext.addBasePointer(&Camera->rightKey);
    }
    else if(attribute == "down_key"){
        NewContext.addBasePointer(&Camera->downKey);
    }
    else if(attribute == "left_key"){
        NewContext.addBasePointer(&Camera->leftKey);
    }
    else if(attribute == "pinned_camera_id"){
        NewContext.addBasePointer(&Camera->pinnedCameraID);
    }
    else if(attribute == "followed_layer_id"){
        NewContext.addBasePointer(&Camera->followedLayerID);
    }
    else if(attribute == "followed_object_id"){
        NewContext.addBasePointer(&Camera->followedObjectID);
    }
    else if(attribute == "followed_image_id"){
        NewContext.addBasePointer(&Camera->followedImageID);
    }
    else if(attribute == "is_pinned_to_camera"){
        NewContext.addBasePointer(&Camera->isPinnedToCamera);
    }
    else if(attribute == "is_following_object"){
        NewContext.addBasePointer(&Camera->isFollowingObject);
    }
    else if(attribute == "is_using_keyboard_to_move"){
        NewContext.addBasePointer(&Camera->isUsingKeyboardToMove);
    }
    else if(attribute == "is_using_keyboard_to_zoom"){
        NewContext.addBasePointer(&Camera->isUsingKeyboardToZoom);
    }
    else if(attribute == "can_be_modified_by_mouse"){
        NewContext.addBasePointer(&Camera->canBeModifiedByMouse);
    }
    else if(attribute == "can_move_objects"){
        NewContext.addBasePointer(&Camera->canMoveObjects);
    }
    else if(attribute == "can_edit_text"){
        NewContext.addBasePointer(&Camera->canEditText);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << attribute << "' is not valid.\n";
    }
}
void ProcessClass::findContextInLayer(ValueLocation Location, ContextClass & NewContext, LayerClass * Layer){
    if(Layer == nullptr){
        return;
    }
    if(Location.attribute == "object" || Location.objectID != "" || Location.moduleType != "" || Location.moduleID != ""){
        for(AncestorObject & Object : Layer->Objects){
            if(Location.objectID != "" && Object.getID() != Location.objectID){
                continue;
            }
            if(!Object.getIsDeleted()){
                findContextInObject(Location, NewContext, &Object);
            }
            if(Location.objectID != ""){
                break;
            }
        }
    }
    else if(Location.attribute == "layer" || Location.attribute == ""){
        NewContext.type = "layer";
        NewContext.Layers.push_back(Layer);
    }
    else if(Location.attribute == "is_active"){
        NewContext.addBasePointer(Layer->getIsActiveAddr());
    }
    else if(Location.attribute == "id"){
        if(isStringInVector(reservedIDs, Layer->getID())){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Access to the reserved ID \'" << Layer->getID() << "\' address was denied.\n";
            return;
        }
        NewContext.addBasePointer(Layer->getIDAddr());
        NewContext.readOnly = true;
    }
    else if(Location.attribute == "group"){
        for(string & group : Layer->getGroupsAddr()){
            NewContext.addBasePointer(&group);
        }
    }
    else if(Location.attribute == "pos_x"){
        NewContext.addBasePointer(&Layer->pos.x);
    }
    else if(Location.attribute == "pos_y"){
        NewContext.addBasePointer(&Layer->pos.y);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute \'" << Location.attribute << "\' does not exist.\n";
    }
}
template <class Module>
void ProcessClass::findContextInModuleVector(const ValueLocation & Location, ContextClass & NewContext, vector<Module> & Source){
    for(Module & Instance : Source){
        if(Location.moduleID != "" && Location.moduleID != Instance.getID()){
            continue;
        }
        if(!Instance.getIsDeleted()){
            findContextInModule(Location.moduleType, Location.attribute, NewContext, &Instance);
        }
        if(Location.moduleID != ""){
            break;
        }
    }
}
void ProcessClass::findContextInObject(ValueLocation Location, ContextClass & NewContext, AncestorObject * Object){
    if(Object == nullptr){
        return;
    }
    if(Location.moduleType != ""){
        if(Location.moduleType == "text"){
            findContextInModuleVector(Location, NewContext, Object->TextContainer);
        }
        else if(Location.moduleType == "editable_text"){
            findContextInModuleVector(Location, NewContext, Object->EditableTextContainer);
        }
        else if(Location.moduleType == "super_text"){
            findContextInModuleVector(Location, NewContext, Object->SuperTextContainer);
        }
        else if(Location.moduleType == "super_editable_text"){
            findContextInModuleVector(Location, NewContext, Object->SuperEditableTextContainer);
        }
        else if(Location.moduleType == "image"){
            findContextInModuleVector(Location, NewContext, Object->ImageContainer);
        }
        else if(Location.moduleType == "movement"){
            findContextInModuleVector(Location, NewContext, Object->MovementContainer);
        }
        else if(Location.moduleType == "collision"){
            findContextInModuleVector(Location, NewContext, Object->CollisionContainer);
        }
        else if(Location.moduleType == "particle"){
            findContextInModuleVector(Location, NewContext, Object->ParticlesContainer);
        }
        else if(Location.moduleType == "event"){
            findContextInModuleVector(Location, NewContext, Object->EveContainer);
        }
        else if(Location.moduleType == "variable"){
            findContextInModuleVector(Location, NewContext, Object->VariablesContainer);
        }
        else if(Location.moduleType == "scrollbar"){
            findContextInModuleVector(Location, NewContext, Object->ScrollbarContainer);
        }
        else if(Location.moduleType == "primitives"){
            findContextInModuleVector(Location, NewContext, Object->PrimitivesContainer);
        }
        else if(Location.moduleType == "vector"){
            findContextInModuleVector(Location, NewContext, Object->VectorContainer);
        }
    }
    else if(Location.attribute == "layer_id"){
        NewContext.addBasePointer(&Object->getLayerIDAddr());
    }
    else if(Location.attribute == "object" || Location.attribute == ""){
        NewContext.type = "object";
        NewContext.Objects.push_back(Object);
    }
    else{
        vector <BasePointersStruct> BasePointers;
        Object->getPrimaryContext(Location.attribute, BasePointers);
        if(BasePointers.size() > 0){
            NewContext.BasePointers.insert(NewContext.BasePointers.end(), BasePointers.begin(), BasePointers.end());
            NewContext.type = "pointer";
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context not found.\n";
        }
    }
}
bool ProcessClass::findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer,
    LayerClass *& CurrentLayer, AncestorObject *& CurrentObject
){
    if(OwnerLayer != nullptr && Owner != nullptr && Location.layerID == Owner->getLayerID()){
        CurrentLayer = OwnerLayer;
    }
    else{
        for(LayerClass & Layer : Layers){
            if(Layer.getID() == Location.layerID){
                CurrentLayer = &Layer;
                break;
            }
        }
    }
    if(CurrentLayer == nullptr || CurrentLayer->getIsDeleted()){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Layer \'" << Location.layerID << "\' does not exist.\n";
        return false;
    }
    if(OwnerLayer != nullptr && CurrentLayer == OwnerLayer && Owner != nullptr && Location.objectID == Owner->getID()){
        CurrentObject = Owner;
    }
    else{
        for(AncestorObject & Object : CurrentLayer->Objects){
            if(Object.getID() == Location.objectID){
                CurrentObject = &Object;
                break;
            }
        }
    }
    
    if(CurrentObject == nullptr || CurrentObject->getIsDeleted()){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object does not exist.\n";
        return false;
    }

    return true;
}
template <class Entity>
Entity *lastNotDeletedInVector(vector<Entity> &Vector){
    for(typename vector<Entity>::reverse_iterator Instance = Vector.rbegin(); Instance != Vector.rend(); ++Instance){
        if(!Instance->getIsDeleted()){
            return &(*Instance);
        }
    }
    return nullptr;
}
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity*> &Vector){
    for(typename vector<Entity*>::reverse_iterator Instance = Vector.rbegin(); Instance != Vector.rend(); ++Instance){
        if(!(*Instance)->getIsDeleted()){
            return *Instance;
        }
    }
    return nullptr;
}
void ProcessClass::aggregateCamerasAndLayersById(ValueLocation & Location, ContextClass & NewContext, AncestorObject * Owner,
    LayerClass * OwnerLayer
){
    if(Location.source == "layer"){
        for(LayerClass & Layer : Layers){
            if(Location.layerID != "" && Location.layerID != Layer.getID()){
                continue;
            }
            if(!Layer.getIsDeleted()){
                findContextInLayer(Location, NewContext, &Layer);
            }
            if(Location.layerID != ""){
                return;
            }
        }
    }
    else if(Location.source == "camera"){
        for(Camera2D & Camera : Cameras){
            if(Location.cameraID != "" && Location.cameraID != Camera.getID()){
                continue;
            }
            if(!Camera.getIsDeleted()){
                findContextInCamera(Location.attribute, NewContext, &Camera);
            }
            if(Location.cameraID != ""){
                return;
            }
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No valid source provided.\n";
    }
}
void ProcessClass::aggregateModulesById(string moduleType, string moduleID, string attribute,
    ContextClass & NewContext, ModulesPointers & AggregatedModules
){
    if(moduleType == "text" && AggregatedModules.Texts.size() > 0){
        getContextFromModuleVectorById<TextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Texts);
    }
    else if(moduleType == "editable_text" && AggregatedModules.EditableTexts.size() > 0){
        getContextFromModuleVectorById<EditableTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.EditableTexts);
    }
    else if(moduleType == "super_text" && AggregatedModules.SuperTexts.size() > 0){
        getContextFromModuleVectorById<SuperTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.SuperTexts);
    }
    else if(moduleType == "super_editable_text" && AggregatedModules.SuperEditableTexts.size() > 0){
        getContextFromModuleVectorById<SuperEditableTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.SuperEditableTexts);
    }
    else if(moduleType == "image" && AggregatedModules.Images.size() > 0){
        getContextFromModuleVectorById<ImageModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Images);
    }
    else if(moduleType == "movement" && AggregatedModules.Movements.size() > 0){
        getContextFromModuleVectorById<MovementModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Movements);
    }
    else if(moduleType == "collision" && AggregatedModules.Collisions.size() > 0){
        getContextFromModuleVectorById<CollisionModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Collisions);
    }
    else if(moduleType == "particles" && AggregatedModules.Particles.size() > 0){
        getContextFromModuleVectorById<ParticleEffectModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Particles);
    }
    else if(moduleType == "event" && AggregatedModules.Events.size() > 0){
        getContextFromModuleVectorById<EveModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Events);
    }
    else if(moduleType == "variable" && AggregatedModules.Variables.size() > 0){
        getContextFromModuleVectorById<VariableModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Variables);
    }
    else if(moduleType == "scrollbar" && AggregatedModules.Scrollbars.size() > 0){
        getContextFromModuleVectorById<ScrollbarModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Scrollbars);
    }
    else if(moduleType == "primitives" && AggregatedModules.Primitives.size() > 0){
        getContextFromModuleVectorById<PrimitivesModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Primitives);
    }
    else if(moduleType == "vector" && AggregatedModules.Vectors.size() > 0){
        getContextFromModuleVectorById<VectorModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Vectors);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no instances of the \'" << moduleType << "\' module.\n";
    }
}
void ProcessClass::findLowerContextById(ValueLocation & Location, ContextClass & NewContext, ContextClass * OldContext){
    if(OldContext->type == "object"){
        for(AncestorObject * Object : OldContext->Objects){
            if(Location.objectID != "" && Location.objectID != Object->getID()){
                continue;
            }
            if(!Object->getIsDeleted()){
                findContextInObject(Location, NewContext, Object);
            }
            if(Location.objectID != ""){
                return;
            }
        }
    }
    else if(isStringInGroup(OldContext->type, 13, "text", "editable_text", "super_text", "super_editable_text", "image", "movement", "collision",
        "particles", "event", "variable", "scrollbar", "primitives", "vector")
    ){
        aggregateModulesById(OldContext->type, Location.moduleID, Location.attribute, NewContext, OldContext->Modules);
    }
    else if(OldContext->type == "layer"){
        for(LayerClass * Layer : OldContext->Layers){
            if(Location.layerID != "" && Location.layerID != Layer->getID()){
                continue;
            }
            if(!Layer->getIsDeleted()){
                findContextInLayer(Location, NewContext, Layer);
            }
            if(Location.layerID != ""){
                return;
            }
        }
    }
    else if(OldContext->type == "camera"){
        for(Camera2D * Camera : OldContext->Cameras){
            if(Location.cameraID != "" && Location.cameraID != Camera->getID()){
                continue;
            }
            if(!Camera->getIsDeleted()){
                findContextInCamera(Location.attribute, NewContext, Camera);
            }
            if(Location.cameraID != ""){
                return;
            }
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No valid source provided.\n";
    }
}
ContextClass * getContextByID(vector<ContextClass> & AllContexts, string contextID, bool printError, EventDescription EventIds){
    for(ContextClass & Context : AllContexts){
        if(Context.ID == contextID){
            return &Context;
        }
    }
    if(printError){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Dynamic variable with id \'" << contextID << "\' does not exist.\n";
    }
    return nullptr;
}
bool ProcessClass::getPairOfContexts(ContextClass *& LeftOperand, ContextClass *& RightOperand,
    vector<ContextClass> & AllContexts, vector<string> contextIDs
){
    if(AllContexts.size() < 2){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no contexts to choose from.\n";
    }
    if(contextIDs.size() == 0){
        LeftOperand = &AllContexts[AllContexts.size()-2];
        RightOperand = &AllContexts.back();
    }
    else if(contextIDs.size() == 1 && contextIDs[0] != ""){
        LeftOperand = getContextByID(AllContexts, contextIDs[0], true, EventIds);
        RightOperand = &AllContexts.back();
    }
    else if(contextIDs.size() > 1 && contextIDs[0] == "" && contextIDs[1] != ""){
        LeftOperand = &AllContexts.back();
        RightOperand = getContextByID(AllContexts, contextIDs[1], true, EventIds);
    }
    else if(contextIDs.size() > 1 && contextIDs[0] != "" && contextIDs[1] != ""){
        LeftOperand = getContextByID(AllContexts, contextIDs[0], true, EventIds);
        RightOperand = getContextByID(AllContexts, contextIDs[1], true, EventIds);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Incorrect input.\n";
        return false;
    }

    if(LeftOperand == nullptr){
        if(contextIDs.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Left/First operand does not exist.\n";
        }
        return false;
    }
    if(RightOperand == nullptr){
        if(contextIDs.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Right/Second operand does not exist.\n";
        }
        return false;
    }

    return true;
}
bool ProcessClass::getOneContext(ContextClass *& SelectedContext, vector<ContextClass> & AllContexts, vector<string> contextIDs){
    if(AllContexts.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no contexts to choose from.\n";
    }

    if(contextIDs.size() == 0){
        SelectedContext = &AllContexts[AllContexts.size()-1];
    }
    else{
        SelectedContext = getContextByID(AllContexts, contextIDs[0], true, EventIds);
    }

    if(SelectedContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Left operand does not exist.\n";
        return false;
    }
    
    return true;
}
bool ProcessClass::getAllSelectedContexts(vector<ContextClass*> & SelectedContexts, vector<ContextClass> & AllContexts,
    const vector<string> & contextIDs
){
    if(contextIDs.size() == 0){
        return false;
    }
    
    if(AllContexts.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no contexts to choose from.\n";
    }

    for(const string & ID : contextIDs){
        SelectedContexts.push_back(getContextByID(AllContexts, ID, true, EventIds));
        if(SelectedContexts.back() == nullptr){
            SelectedContexts.pop_back();
        }
    }

    if(SelectedContexts.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No contexts found.\n";
        return false;
    }
    
    return true;
}
bool ProcessClass::checkDefaultCondition(VariableModule * Left, VariableModule * Right){
    return Left->isConditionMet(EngineInstr::equal, Right);
}
bool ProcessClass::checkDefaultCondition(BasePointersStruct * Left, BasePointersStruct * Right){
    return Left->areEqual(Right);
}
template<class Entity>
void ProcessClass::executeOperationsOnSets(EngineInstr instruction, vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand){
    if(instruction == EngineInstr::sum){
        NewContext = LeftOperand;
        NewContext.insert(NewContext.end(), RightOperand.begin(), RightOperand.end());
    }
    else if(instruction == EngineInstr::intersection){
        for(Entity * LeftEntity : LeftOperand){
            for(Entity * RightEntity : RightOperand){
                if(LeftEntity->getID() != RightEntity->getID()){
                    continue;
                }
                NewContext.push_back(LeftEntity);
                break;
            }
        }
    }
    else if(instruction == EngineInstr::diff){
        bool sameEntity;
        for(Entity * LeftEntity : LeftOperand){
            sameEntity = false;
            for(Entity * RightEntity : RightOperand){
                if(LeftEntity->getID() == RightEntity->getID()){
                    sameEntity = true;
                    break;
                }
            }
            if(!sameEntity){
                NewContext.push_back(LeftEntity);
            }
        }
    }
}
template<class Entity>
void ProcessClass::executeOperationsOnSets(EngineInstr instruction, vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand){
    if(instruction == EngineInstr::sum){
        NewContext = LeftOperand;
        NewContext.insert(NewContext.end(), RightOperand.begin(), RightOperand.end());
    }
    else if(instruction == EngineInstr::intersection){
        for(Entity & LeftEntity : LeftOperand){
            for(Entity & RightEntity : RightOperand){
                if(!checkDefaultCondition(&LeftEntity, &RightEntity)){
                    continue;
                }
                NewContext.push_back(LeftEntity);
                break;
            }
        }
    }
    else if(instruction == EngineInstr::diff){
        bool sameEntity;
        for(Entity & LeftEntity : LeftOperand){
            sameEntity = false;
            for(Entity & RightEntity : RightOperand){
                if(checkDefaultCondition(&LeftEntity, &RightEntity)){
                    sameEntity = true;
                    break;
                }
            }
            if(!sameEntity){
                NewContext.push_back(LeftEntity);
            }
        }
    }
}
void ProcessClass::addNewContext(vector<ContextClass> & EventContext, const ContextClass & NewContext, string type, string newID){
    EventContext.push_back(NewContext);
    EventContext.back().type = type;
    EventContext.back().setID(EventContext, newID, printOutInstructions, EventIds);
}
void ProcessClass::aggregateTwoSets(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass NewContext;
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;

    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        return;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << LeftOperand->ID << ":" << LeftOperand->type << ":"
            << LeftOperand->getValue(EventIds) << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue(EventIds) << "\n";
    }

    if(LeftOperand->type != RightOperand->type){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Right operand has a different type than the left operand.\n";
        return;
    }

    if(LeftOperand->type == "value"){
        executeOperationsOnSets(Operation.instruction, NewContext.Variables, LeftOperand->Variables, RightOperand->Variables);
    }
    else if(LeftOperand->type == "pointer"){
        executeOperationsOnSets(Operation.instruction, NewContext.BasePointers, LeftOperand->BasePointers, RightOperand->BasePointers);
    }
    else if(LeftOperand->type == "camera"){
        executeOperationsOnSets(Operation.instruction, NewContext.Cameras, LeftOperand->Cameras, RightOperand->Cameras);
    }
    else if(LeftOperand->type == "layer"){
        executeOperationsOnSets(Operation.instruction, NewContext.Layers, LeftOperand->Layers, RightOperand->Layers);
    }
    else if(LeftOperand->type == "layer"){
        executeOperationsOnSets(Operation.instruction, NewContext.Layers, LeftOperand->Layers, RightOperand->Layers);
    }
    else if(LeftOperand->type == "object"){
        executeOperationsOnSets(Operation.instruction, NewContext.Objects, LeftOperand->Objects, RightOperand->Objects);
    }
    else if(LeftOperand->type == "text"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Texts, LeftOperand->Modules.Texts, RightOperand->Modules.Texts);
    }
    else if(LeftOperand->type == "editable_text"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.EditableTexts, LeftOperand->Modules.EditableTexts, RightOperand->Modules.EditableTexts);
    }
    else if(LeftOperand->type == "super_text"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.SuperTexts, LeftOperand->Modules.SuperTexts, RightOperand->Modules.SuperTexts);
    }
    else if(LeftOperand->type == "super_editable_text"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.SuperEditableTexts, LeftOperand->Modules.SuperEditableTexts, RightOperand->Modules.SuperEditableTexts);
    }
    else if(LeftOperand->type == "image"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Images, LeftOperand->Modules.Images, RightOperand->Modules.Images);
    }
    else if(LeftOperand->type == "movement"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Movements, LeftOperand->Modules.Movements, RightOperand->Modules.Movements);
    }
    else if(LeftOperand->type == "collision"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Collisions, LeftOperand->Modules.Collisions, RightOperand->Modules.Collisions);
    }
    else if(LeftOperand->type == "particles"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Particles, LeftOperand->Modules.Particles, RightOperand->Modules.Particles);
    }
    else if(LeftOperand->type == "event"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Events, LeftOperand->Modules.Events, RightOperand->Modules.Events);
    }
    else if(LeftOperand->type == "variable"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Variables, LeftOperand->Modules.Variables, RightOperand->Modules.Variables);
    }
    else if(LeftOperand->type == "scrollbar"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Scrollbars, LeftOperand->Modules.Scrollbars, RightOperand->Modules.Scrollbars);
    }
    else if(LeftOperand->type == "primitives"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Primitives, LeftOperand->Modules.Primitives, RightOperand->Modules.Primitives);
    }
    else if(LeftOperand->type == "vector"){
        executeOperationsOnSets(Operation.instruction, NewContext.Modules.Vectors, LeftOperand->Modules.Vectors, RightOperand->Modules.Vectors);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": type \'" << LeftOperand->type << "\' does not exist.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
        return;
    }

    addNewContext(EventContext, NewContext, LeftOperand->type, Operation.newContextID);
}
void ProcessClass::aggregateEntities(OperaClass & Operation, vector<ContextClass> & EventContext, const EngineClass & Engine){
    ContextClass NewContext;

    if(Operation.Location.source == "layer" || Operation.Location.source == "camera"){
        if(printOutInstructions){
            cout << transInstrToStr(Operation.instruction) << " ";
            Operation.Location.print("");
            cout << "\n";
        }
        if(Operation.Location.source == "camera"){
            aggregateCameras(Operation, NewContext, vector<Camera2D*>(), Engine, EventContext);
        }
        else if(Operation.Location.source == "layer"){
            aggregateLayers(Operation, NewContext, vector<LayerClass*>(), Engine, EventContext);
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Source type \'" << Operation.Location.source << "\' does not exist in this context.\n";
        }
    }
    else if(Operation.dynamicIDs.size() > 0){
        for(const string & contextID : Operation.dynamicIDs){
            ContextClass * SourceContext = getContextByID(EventContext, contextID, true, EventIds);
            if(SourceContext == nullptr){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context \'" << contextID << "\' does not exist.\n";
                continue;
            }
            if(printOutInstructions){
                cout << transInstrToStr(Operation.instruction) << " ";
                Operation.Location.print(contextID);
                cout << "\n";
            }
            if(SourceContext->type == "camera"){
                if(SourceContext->Cameras.size() > 0){
                    aggregateCameras(Operation, NewContext, SourceContext->Cameras, Engine, EventContext);
                }
            }
            else if(SourceContext->type == "layer"){
                if(SourceContext->Layers.size() > 0){
                    aggregateLayers(Operation, NewContext, SourceContext->Layers, Engine, EventContext);
                }
            }
            else if(SourceContext->type == "object"){
                if(SourceContext->Objects.size() > 0){
                    aggregateObjects(Operation, NewContext, SourceContext->Objects, Engine, EventContext);
                }
            }
            else if(isStringInGroup(SourceContext->type, 13, "text", "editable_text", "super_text", "super_editable_text", "image", "movement", "collision",
                "particles", "event", "variable", "scrollbar", "primitives", "vector")
            ){
                if(SourceContext->Modules.hasInstanceOfAnyModule()){
                    aggregateModules(Operation, NewContext, SourceContext, EventContext, Engine);
                }
            }
            else if(SourceContext->type == "pointer"){
                if(SourceContext->BasePointers.size() > 0){
                    aggregatePointers(Operation.instruction, NewContext, SourceContext->BasePointers);
                }
            }
            else if(SourceContext->type == "value"){
                if(SourceContext->Variables.size() > 0){
                    aggregateVariables(Operation.instruction, NewContext, SourceContext->Variables);
                }
            }
            else{
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context type \'" << SourceContext->type << "\' does not exist.\n";
            }
        }
    }

    if(NewContext.type != ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
//Moving values will not change a vector structure nor value type. 
void moveRightToLeft(EngineInstr instruction, ContextClass * LeftOperand, ContextClass * RightOperand, EventDescription EventIds){
    if(LeftOperand->type != "pointer" && LeftOperand->type != "value"
        && LeftOperand->type != "variable" && LeftOperand->type != "vector"
    ){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction)
            << "\' instruction: Left operand has an invalid type: \'" << LeftOperand->type << "\'.\n";
        return;
    }
    else if(RightOperand->type != "pointer" && RightOperand->type != "value"
        && RightOperand->type != "variable" && RightOperand->type != "vector"
    ){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction)
            << "\' instruction: Right operand has an invalid type: \'" << RightOperand->type << "\'.\n";
        return;
    }

    if(LeftOperand->readOnly){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction)
            << "\' instruction: Left operand is read-only. Instruction \'" << transInstrToStr(instruction) << "\' failed.\n";
        return;
    }

    bool sameSize = false;
    unsigned i = 0, j = 0;

    if(LeftOperand->type == "pointer" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
            LeftOperand->BasePointers[i].move(RightOperand->BasePointers[j], instruction);
        }
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            LeftOperand->Variables[i].move(&RightOperand->Variables[j], instruction);
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "variable"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            LeftOperand->Modules.Variables[i]->move(RightOperand->Modules.Variables[j], instruction);
        }
    }
    else if(LeftOperand->type == "vector" && RightOperand->type == "vector"){
        if(!checkForVectorSize(LeftOperand->Modules.Vectors.size(), RightOperand->Modules.Vectors.size(), sameSize, __FUNCTION__)){
            return;
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In '"
            << transInstrToStr(instruction) << "': Not yet implemented!\n";
        for(; i < LeftOperand->Modules.Vectors.size(); i++, j+=sameSize){
            //LeftOperand->Modules.Vectors[i]->move(RightOperand->Modules.Variables[j], instruction);
        }
    }
    else if(LeftOperand->type == "vector" && RightOperand->type == "value"){
        //if(!checkForVectorSize(LeftOperand->Modules.Vectors.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
        //    return;
        //}

        if(LeftOperand->Modules.Vectors.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In '"
                << transInstrToStr(instruction) << "': Left operand is empty.\n";
            return;
        }

        if(LeftOperand->Modules.Vectors.size() > 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In '"
                << transInstrToStr(instruction)
                << "': Left operand has more than 1 vector - only the last vector will be used .\n";
            return;
        }

        LeftOperand->Modules.Vectors[i]->move(RightOperand->Modules.Variables, instruction, EventIds);
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "variable"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            LeftOperand->Variables[i].move(RightOperand->Modules.Variables[j], instruction);
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            LeftOperand->Modules.Variables[i]->move(&RightOperand->Variables[j], instruction);
        }
    }
    else if(LeftOperand->type == "pointer" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        BaseVariableStruct RightVariable;
        for(; i < LeftOperand->BasePointers.size(); i++){
            if(sameSize || i == 0){
                RightVariable = RightOperand->Variables[i].getVariableStruct();
                if(RightVariable.type == ""){
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(instruction) << "\' instruction: Failed to fetch a variable.\n";
                    if(!sameSize){
                        return;
                    }
                    continue;
                }
            }
            LeftOperand->BasePointers[i].move(RightVariable, instruction);
        }
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            LeftOperand->Variables[i].move(&RightOperand->BasePointers[j], instruction);
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            LeftOperand->Modules.Variables[i]->move(&RightOperand->BasePointers[j], instruction);
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'"
            << transInstrToStr(instruction) << "\' instruction: You cannot move a value of \'"
            << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
    }
}
void ProcessClass::moveOrRename(vector<ContextClass> & EventContext, ContextClass * NewContext, string newContextID){
    ContextClass * OldContext = getContextByID(EventContext, newContextID, false, EventIds);
    if(OldContext != nullptr){
        moveRightToLeft(EngineInstr::move, OldContext, NewContext, EventIds);
    }
    else{
        addNewContext(EventContext, *NewContext, "value", newContextID);
    }
}
void ProcessClass::aggregateValues(vector<ContextClass> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer,
    AncestorObject *Owner, const EngineClass & Engine, vector<ProcessClass> * Processes
){
    ContextClass NewContext;
    for(ConditionClass & ValueLocation : Operation.ConditionalChain){
        if(printOutInstructions){
            cout << "inner_find ";
            ValueLocation.Location.print("");
            cout << "\n";
        }
        NewContext.Variables.push_back(findNextValue(ValueLocation, Owner, OwnerLayer, Engine, Processes, EventContext));
    }
    NewContext.type = "value";

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << NewContext.getValue(EventIds) << " " << Operation.newContextID << "\n";
    }

    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::aggregateOnlyById(vector<ContextClass> &EventContext, OperaClass & Operation,
    LayerClass *OwnerLayer, AncestorObject *Owner
){
    ContextClass NewContext;
    ContextClass * LeftOperand = nullptr;

    if(Operation.Location.source == "layer" || Operation.Location.source == "camera"){
        if(printOutInstructions){
            cout << "find_by_id (in environment)\n";
        }
        if(printOutInstructions){
            cout << "inner_find ";
            Operation.Location.print("");
            cout << "\n";
        }
        aggregateCamerasAndLayersById(Operation.Location, NewContext, Owner, OwnerLayer);
    }
    else if(Operation.dynamicIDs.size() > 0){
        if(printOutInstructions){
            cout << "find_by_id (context)<" << Operation.dynamicIDs.size() << ">\n";
        }
        for(string dynamicID : Operation.dynamicIDs){
            LeftOperand = getContextByID(EventContext, dynamicID, true, EventIds);
            if(LeftOperand == nullptr){
                continue;
            }
            if(printOutInstructions){
                cout << "inner_find ";
                Operation.Location.print(dynamicID);
                cout << "\n";
            }
            findLowerContextById(Operation.Location, NewContext, LeftOperand);
        }
    }

    if(NewContext.type != ""){
        if(printOutInstructions){
            cout << "found: " << NewContext.getValue(EventIds) << "\n";
        }
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ContextClass::setID(vector<ContextClass> &EventContext, string newID, const bool & printOutInstructions, EventDescription EventIds){
    if(newID == ""){
        return;
    }
    
    ContextClass * OldVariable = getContextByID(EventContext, newID, false, EventIds);

    if(OldVariable != nullptr){
        if(printOutInstructions){
            cout << "delete "<< OldVariable->ID << ":" << OldVariable->type << ":" << OldVariable->getValue(EventIds) << "\n";
            cout << "let " << newID << " " << ID << ":" << type << ":" << getValue(EventIds) << "\n";
        }
        *OldVariable = *this;
        OldVariable->ID = newID;
        EventContext.pop_back();
    }
    else{
        if(printOutInstructions){
            cout << "let " << newID << " " << ID << ":" << type << ":" << getValue(EventIds) << "\n";
        }
        ID = newID;
    }
}
void ProcessClass::nameVariable(vector<ContextClass> &EventContext, OperaClass &Operation){
    ContextClass * LeftOperand = nullptr;

    if(!getOneContext(LeftOperand, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Variable \'" << Operation.newContextID << "\' cannot be created.\n";
        return;
    }

    LeftOperand->setID(EventContext, Operation.newContextID, printOutInstructions, EventIds);
}
bool checkForVectorSize(size_t leftSize, size_t rightSize, bool & sameSize, string functionName){
    if(leftSize == rightSize){
        sameSize = true;
    }
    else if(rightSize != 1){
        cout << "Error: In " << functionName << ": Vector sizes " << leftSize << " (left) and " << rightSize << " (right) are incorrect.\n";
        return false;
    }
    return true;
}
template<class Entity>
void ProcessClass::cloneRightToLeft(vector <Entity*> & LeftOperand, vector <Entity*> & RightOperand, vector<LayerClass> & Layers, const bool & changeOldID){
    bool sameSize = false;
    if(!checkForVectorSize(LeftOperand.size(), RightOperand.size(), sameSize, __FUNCTION__)){
        return;
    }
    unsigned i = 0, j = 0;
    for(; i < LeftOperand.size(); i++, j+=sameSize){
        for(LayerClass & Layer : Layers){
            if(LeftOperand[i]->getLayerID() != Layer.getID()){
                continue;
            }
            for(AncestorObject & Object : Layer.Objects){
                if(LeftOperand[i]->getObjectID() != Object.getID()){
                    continue;
                }
                if(isStringInVector(reservedIDs, RightOperand[j]->getID())){
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Module with a reserved ID \'" << RightOperand[j]->getID() << "\' cannot be cloned.\n";
                    return;
                }
                if(isStringInVector(reservedIDs, LeftOperand[i]->getID())){
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Module with a reserved ID \'" << LeftOperand[i]->getID() << "\' cannot be changed.\n";
                    return;
                }
                LeftOperand[i]->clone(*RightOperand[j], Object.imageContainerIDs, Layer.getID(), Object.getID(), changeOldID);
                break;
            }
            break;
        }
    }
}
void ProcessClass::moveValues(OperaClass & Operation, vector<ContextClass> &EventContext){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;

    if(Operation.instruction == EngineInstr::inc || Operation.instruction == EngineInstr::dec){
        if(!getOneContext(LeftOperand, EventContext, Operation.dynamicIDs)){
            return;
        }
        if(LeftOperand->type != "pointer" && LeftOperand->type != "value" && LeftOperand->type != "variable" && LeftOperand->type != "vector"){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Left operand has an invalid type: \'" << LeftOperand->type << "\'.\n";
        }
        RightOperand = LeftOperand; //Cloning the left operand allows to reuse this function for incrementing and decrementing.
    }
    else if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << ": Failed to get a pair of contexts.\n";
        return;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue(EventIds)
            << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue(EventIds) << "\n";
    }
    
    moveRightToLeft(Operation.instruction, LeftOperand, RightOperand, EventIds);
}
void ProcessClass::cloneEntities(vector<string> dynamicIDs, bool changeOldID, vector<ContextClass> &EventContext, vector<LayerClass> &Layers){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;
    
    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'clone\': Failed to get a pair of contexts.\n";
        return;
    }

    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(printOutInstructions){
        cout << "clone " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue(EventIds)
            << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue(EventIds) << "\n";
    }

    if(LeftOperand->readOnly){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Left operand is read-only. Instruction \'clone\' failed.\n";
        return;
    }

    if(LeftOperand->type != RightOperand->type){
        if(LeftOperand->type == "pointer" && RightOperand->type == "value"){
            if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
                return;
            }
            BaseVariableStruct RightVariable;
            for(; i < LeftOperand->BasePointers.size(); i++){
                if(sameSize || i == 0){
                    RightVariable = RightOperand->Variables[i].getVariableStruct();
                    if(RightVariable.type == ""){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                        if(!sameSize){
                            return;
                        }
                        continue;
                    }
                }
                LeftOperand->BasePointers[i].tryToSetValue(RightVariable);
            }
        }
        else if(LeftOperand->type == "pointer" && RightOperand->type == "variable"){
            if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
                return;
            }
            BaseVariableStruct RightVariable;
            for(; i < LeftOperand->BasePointers.size(); i++){
                if(sameSize || i == 0){
                    RightVariable = RightOperand->Modules.Variables[i]->getVariableStruct();
                    if(RightVariable.type == ""){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                        if(!sameSize){
                            return;
                        }
                        continue;
                    }
                }
                LeftOperand->BasePointers[i].tryToSetValue(RightVariable);
            }
        }
        else if(LeftOperand->type == "value" && RightOperand->type == "pointer"){
            if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
                LeftOperand->Variables[i].setValueFromPointer(RightOperand->BasePointers[j]);
            }
        }
        else if(LeftOperand->type == "variable" && RightOperand->type == "pointer"){
            if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
                LeftOperand->Modules.Variables[i]->setValueFromPointer(RightOperand->BasePointers[j]);
            }
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": You cannot assign a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
        }
    }
    else{
        if(LeftOperand->type == "pointer"){
            if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
                LeftOperand->BasePointers[i].move(RightOperand->BasePointers[j], EngineInstr::clone_i);
            }
        }
        else if(LeftOperand->type == "value"){
            if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
                LeftOperand->Variables[i] = RightOperand->Variables[j];
            }
        }
        else if(LeftOperand->type == "camera"){
            if(!checkForVectorSize(LeftOperand->Cameras.size(), RightOperand->Cameras.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->Cameras.size(); i++, j+=sameSize){
                LeftOperand->Cameras[i]->clone(*RightOperand->Cameras[j], camerasIDs, changeOldID);
            }
        }
        else if(LeftOperand->type == "layer"){
            if(!checkForVectorSize(LeftOperand->Layers.size(), RightOperand->Layers.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->Layers.size(); i++, j+=sameSize){
                LeftOperand->Layers[i]->clone(*RightOperand->Layers[j], layersIDs, changeOldID);
            }
            wasNewExecuted = true;
        }
        else if(LeftOperand->type == "object"){
            if(!checkForVectorSize(LeftOperand->Objects.size(), RightOperand->Objects.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->Objects.size(); i++, j+=sameSize){
                for(LayerClass & Layer : Layers){
                    if(Layer.getID() != LeftOperand->Objects[i]->getLayerID()){
                        continue;
                    }
                    LeftOperand->Objects[i]->clone(*RightOperand->Objects[j], Layer.objectsIDs, Layer.getID(), changeOldID);
                    break;
                }
            }
            wasNewExecuted = true;
        }
        else if(LeftOperand->type == "text"){
            cloneRightToLeft(LeftOperand->Modules.Texts, RightOperand->Modules.Texts, Layers, changeOldID);
        }
        else if(LeftOperand->type == "editable_text"){
            cloneRightToLeft(LeftOperand->Modules.EditableTexts, RightOperand->Modules.EditableTexts, Layers, changeOldID);
        }
        else if(LeftOperand->type == "super_text"){
            cloneRightToLeft(LeftOperand->Modules.SuperTexts, RightOperand->Modules.SuperTexts, Layers, changeOldID);
        }
        else if(LeftOperand->type == "super_editable_text"){
            cloneRightToLeft(LeftOperand->Modules.SuperEditableTexts, RightOperand->Modules.SuperEditableTexts, Layers, changeOldID);
        }
        else if(LeftOperand->type == "image"){
            cloneRightToLeft(LeftOperand->Modules.Images, RightOperand->Modules.Images, Layers, changeOldID);
        }
        else if(LeftOperand->type == "movement"){
            cloneRightToLeft(LeftOperand->Modules.Movements, RightOperand->Modules.Movements, Layers, changeOldID);
        }
        else if(LeftOperand->type == "collision"){
            cloneRightToLeft(LeftOperand->Modules.Collisions, RightOperand->Modules.Collisions, Layers, changeOldID);
        }
        else if(LeftOperand->type == "particles"){
            cloneRightToLeft(LeftOperand->Modules.Particles, RightOperand->Modules.Particles, Layers, changeOldID);
        }
        else if(LeftOperand->type == "event"){
            cloneRightToLeft(LeftOperand->Modules.Events, RightOperand->Modules.Events, Layers, changeOldID);
            wasNewExecuted = true;
        }
        else if(LeftOperand->type == "variable"){
            cloneRightToLeft(LeftOperand->Modules.Variables, RightOperand->Modules.Variables, Layers, changeOldID);
        }
        else if(LeftOperand->type == "scrollbar"){
            cloneRightToLeft(LeftOperand->Modules.Scrollbars, RightOperand->Modules.Scrollbars, Layers, changeOldID);
        }
        else if(LeftOperand->type == "primitives"){
            cloneRightToLeft(LeftOperand->Modules.Primitives, RightOperand->Modules.Primitives, Layers, changeOldID);
        }
        else if(LeftOperand->type == "vector"){
            cloneRightToLeft(LeftOperand->Modules.Vectors, RightOperand->Modules.Vectors, Layers, changeOldID);
        }
    }
}
void ProcessClass::executeArithmetics(OperaClass & Operation, vector<ContextClass> &EventContext){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;

    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << ": Failed to get a pair of contexts.\n";
        return;
    }

    ContextClass NewContext;
    BaseVariableStruct result;
    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << LeftOperand->ID << ":" << LeftOperand->type
            << ":" << LeftOperand->getValue(EventIds) << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue(EventIds) << "\n";
    }

    if(LeftOperand->type == "pointer" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
            result = LeftOperand->BasePointers[i].executeArithmetics(RightOperand->BasePointers[j], Operation.instruction);
            if(result.type == ""){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to execute arithmetic equation.\n";
                continue;
            }
            NewContext.Variables.push_back(VariableModule());
            NewContext.Variables.back().set(result);
            result.type = "";
        }
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            if(LeftOperand->Variables[i].getType() == 'd' || RightOperand->Variables[j].getType() == 'd'){
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setDouble(LeftOperand->Variables[i].floatingOperation(Operation.instruction, &RightOperand->Variables[j]));
            }
            else if(LeftOperand->Variables[i].getType() != 's' || RightOperand->Variables[j].getType() != 's'){
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setInt(LeftOperand->Variables[i].intOperation(Operation.instruction, &RightOperand->Variables[j]));
            }
            else{
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setString(LeftOperand->Variables[i].stringOperation(Operation.instruction, &RightOperand->Variables[j]));
            }
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "variable"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            if(LeftOperand->Modules.Variables[i]->getType() == 'd' || RightOperand->Modules.Variables[j]->getType() == 'd'){
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setDouble(LeftOperand->Modules.Variables[i]->floatingOperation(Operation.instruction, RightOperand->Modules.Variables[j]));
            }
            else if(LeftOperand->Modules.Variables[i]->getType() != 's' || RightOperand->Modules.Variables[j]->getType() != 's'){
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setInt(LeftOperand->Modules.Variables[i]->intOperation(Operation.instruction, RightOperand->Modules.Variables[j]));
            }
            else{
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setString(LeftOperand->Modules.Variables[i]->stringOperation(Operation.instruction, RightOperand->Modules.Variables[j]));
            }
        }
    }
    else if(LeftOperand->type == "pointer" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            return;
        }

        BaseVariableStruct RightVariable;
        
        for(; i < LeftOperand->BasePointers.size(); i++){
            if(sameSize || i == 0){
                RightVariable = RightOperand->Variables[i].getVariableStruct();
                if(RightVariable.type == ""){
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                    if(!sameSize){
                        return;
                    }
                    continue;
                }
            }
            
            result = LeftOperand->BasePointers[i].executeArithmetics(RightVariable, Operation.instruction);

            if(result.type == ""){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to execute arithmetic equation.\n";
                continue;
            }
            NewContext.Variables.push_back(VariableModule());
            NewContext.Variables.back().set(result);
            result.type = "";
        }
    }
    else if(LeftOperand->type == "pointer" && RightOperand->type == "variable"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
            return;
        }

        BaseVariableStruct RightVariable;
        
        for(; i < LeftOperand->BasePointers.size(); i++){
            if(sameSize || i == 0){
                RightVariable = RightOperand->Modules.Variables[i]->getVariableStruct();
                if(RightVariable.type == ""){
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                    if(!sameSize){
                        return;
                    }
                    continue;
                }
            }
            
            result = LeftOperand->BasePointers[i].executeArithmetics(RightVariable, Operation.instruction);

            if(result.type == ""){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to execute arithmetic equation.\n";
                continue;
            }
            NewContext.Variables.push_back(VariableModule());
            NewContext.Variables.back().set(result);
            result.type = "";
        }
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            if(LeftOperand->Variables[i].getType() == 'd' || isStringInGroup(RightOperand->BasePointers[j].type, 2, "float", "double")){
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setDouble(LeftOperand->Variables[i].floatingOperation(Operation.instruction, &RightOperand->BasePointers[j]));
            }
            else{
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setInt(LeftOperand->Variables[i].intOperation(Operation.instruction, &RightOperand->BasePointers[j]));
            }
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            if(LeftOperand->Modules.Variables[i]->getType() == 'd' || isStringInGroup(RightOperand->BasePointers[j].type, 2, "float", "double")){
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setDouble(LeftOperand->Modules.Variables[i]->floatingOperation(Operation.instruction, &RightOperand->BasePointers[j]));
            }
            else{
                NewContext.Variables.push_back(VariableModule());
                NewContext.Variables.back().setInt(LeftOperand->Modules.Variables[i]->intOperation(Operation.instruction, &RightOperand->BasePointers[j]));
            }
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": You cannot execute arithmetic equation on variables of types: \'" << RightOperand->type << "\' and \'" << LeftOperand->type << "\'.\n";
    }

    NewContext.type = "value";
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::createLiteral(vector<ContextClass> &EventContext, const OperaClass & Operation){
    ContextClass NewContext;
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " {";
        for(const VariableModule & Literal : Operation.Literals){
            cout << Literal.getString() << ", ";
            NewContext.Variables.push_back(Literal);
        }
        cout << "}\n";
    }
    else{
        for(const VariableModule & Literal : Operation.Literals){
            NewContext.Variables.push_back(Literal);
        }
    }

    NewContext.type = "value";
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::generateRandomVariable(vector<ContextClass> &EventContext, const OperaClass & Operation){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;
    bool deletePointers = false;

    if(Operation.Literals.size() == 2){
        LeftOperand = new ContextClass();
        LeftOperand->Variables.push_back(Operation.Literals[0]);
        LeftOperand->type = "value";
        RightOperand = new ContextClass();
        RightOperand->Variables.push_back(Operation.Literals[1]);
        RightOperand->type = "value";
        deletePointers = true;
    }
    else if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << ": Failed to get a pair of contexts.\n";
        return;
    }

    ContextClass NewContext;
    VariableModule Result;
    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << LeftOperand->ID << ":" << LeftOperand->type
            << ":" << LeftOperand->getValue(EventIds) << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue(EventIds) << "\n";
    }

    if(LeftOperand->type == "pointer" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->BasePointers[i].getInt(), RightOperand->BasePointers[j].getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->Variables[i].getInt(), RightOperand->Variables[j].getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "variable"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->Modules.Variables[i]->getInt(), RightOperand->Modules.Variables[j]->getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else if(LeftOperand->type == "pointer" && RightOperand->type == "value"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Variables.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->BasePointers[i].getInt(), RightOperand->Variables[j].getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else if(LeftOperand->type == "pointer" && RightOperand->type == "variable"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->BasePointers[i].getInt(), RightOperand->Modules.Variables[j]->getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else if(LeftOperand->type == "value" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->Variables.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->Variables[i].getInt(), RightOperand->BasePointers[j].getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else if(LeftOperand->type == "variable" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->Modules.Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            if(deletePointers){
                delete LeftOperand;
                delete RightOperand;
            }
            return;
        }
        for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
            Result.setInt(randomInt(LeftOperand->Modules.Variables[i]->getInt(), RightOperand->BasePointers[j].getInt()));
            NewContext.Variables.push_back(Result);
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": You cannot move a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
    }
    NewContext.type = "value";

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << NewContext.getValue(EventIds) << " " << Operation.newContextID << "\n";
    }

    moveOrRename(EventContext, &NewContext, Operation.newContextID);

    if(deletePointers){
        delete LeftOperand;
        delete RightOperand;
    }
}
template <class Module>
bool containsTheSameModule(const vector <Module> & LeftModules, const vector <Module> & RightModules){
    unsigned i, j;
    for(i = 0; i < LeftModules.size(); i++){
        for(j = 0; j < RightModules.size(); j++){
            if(LeftModules[i]->getID() == RightModules[j]->getID()
                && LeftModules[i]->getLayerID() == RightModules[j]->getLayerID()
                && LeftModules[i]->getObjectID() == RightModules[j]->getObjectID()
            ){
                return true;
            }
        }
    }
    return false;
}
void ProcessClass::checkIfVectorContainsVector(OperaClass & Operation, vector<ContextClass> &EventContext){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;
    
    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << ": Failed to get a pair of contexts.\n";
        return;
    }

    bool result = false;
    unsigned i = 0, j = 0;

    if(printOutInstructions){
        cout << "in " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue(EventIds) << " "
            << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue(EventIds) << "\n";
    }

    if(LeftOperand->type != RightOperand->type){
        if(LeftOperand->type == "pointer" && RightOperand->type == "value"){
            BaseVariableStruct RightVariable;
            for(i = 0; i < LeftOperand->BasePointers.size(); i++){
                for(j = 0; j < RightOperand->Variables.size(); j++){
                    RightVariable = RightOperand->Variables[j].getVariableStruct();
                    if(RightVariable.type == ""){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                        continue;
                    }
                    if(LeftOperand->BasePointers[i].areEqual(&RightVariable)){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        if(LeftOperand->type == "pointer" && RightOperand->type == "variable"){
            BaseVariableStruct RightVariable;
            for(i = 0; i < LeftOperand->BasePointers.size(); i++){
                for(j = 0; j < RightOperand->Modules.Variables.size(); j++){
                    RightVariable = RightOperand->Modules.Variables[j]->getVariableStruct();
                    if(RightVariable.type == ""){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                        continue;
                    }
                    if(LeftOperand->BasePointers[i].areEqual(&RightVariable)){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "value" && RightOperand->type == "pointer"){
            for(i = 0; i < LeftOperand->Variables.size(); i++){
                for(j = 0; j < RightOperand->BasePointers.size(); j++){
                    if(LeftOperand->Variables[i].isConditionMet(EngineInstr::equal, RightOperand->BasePointers[j])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "variable" && RightOperand->type == "pointer"){
            for(i = 0; i < LeftOperand->Modules.Variables.size(); i++){
                for(j = 0; j < RightOperand->BasePointers.size(); j++){
                    if(LeftOperand->Modules.Variables[i]->isConditionMet(EngineInstr::equal, RightOperand->BasePointers[j])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "variable" && RightOperand->type == "value"){
            for(i = 0; i < LeftOperand->Modules.Variables.size(); i++){
                for(j = 0; j < RightOperand->Variables.size(); j++){
                    if(LeftOperand->Modules.Variables[i]->isConditionMet(EngineInstr::equal, &RightOperand->Variables[j])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "variable" && RightOperand->type == "vector"){
            for(i = 0; i < LeftOperand->Modules.Variables.size(); i++){
                for(j = 0; j < RightOperand->Modules.Vectors.size(); j++){
                    if(RightOperand->Modules.Vectors[j]->contains(*LeftOperand->Modules.Variables[i])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "value" && RightOperand->type == "vector"){
            for(i = 0; i < LeftOperand->Variables.size(); i++){
                for(j = 0; j < RightOperand->Modules.Vectors.size(); j++){
                    if(RightOperand->Modules.Vectors[j]->contains(LeftOperand->Variables[i])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "pointer" && RightOperand->type == "vector"){
            for(i = 0; i < LeftOperand->BasePointers.size(); i++){
                for(j = 0; j < RightOperand->Modules.Vectors.size(); j++){
                    if(RightOperand->Modules.Vectors[j]->contains(LeftOperand->BasePointers[i])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": You cannot assign a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
        }
    }
    else{
        if(LeftOperand->type == "pointer"){
            for(i = 0; i < LeftOperand->BasePointers.size(); i++){
                for(j = 0; j < RightOperand->BasePointers.size(); j++){
                    if(LeftOperand->BasePointers[i].areEqual(&RightOperand->BasePointers[j])){
                        result = true;
                        break;
                    }
                }
            }
        }
        else if(LeftOperand->type == "value"){
            for(i = 0; i < LeftOperand->Variables.size(); i++){
                for(j = 0; j < RightOperand->Variables.size(); j++){
                    if(LeftOperand->Variables[i].isConditionMet(EngineInstr::equal, &RightOperand->Variables[j])){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "camera"){
            for(i = 0; i < LeftOperand->Cameras.size(); i++){
                for(j = 0; j < RightOperand->Cameras.size(); j++){
                    if(LeftOperand->Cameras[i]->getID() == RightOperand->Cameras[j]->getID()){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "layer"){
            for(i = 0; i < LeftOperand->Layers.size(); i++){
                for(j = 0; j < RightOperand->Layers.size(); j++){
                    if(LeftOperand->Layers[i]->getID() == RightOperand->Layers[j]->getID()){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "object"){
            for(i = 0; i < LeftOperand->Objects.size(); i++){
                for(j = 0; j < RightOperand->Objects.size(); j++){
                    if(LeftOperand->Objects[i]->getID() == RightOperand->Objects[j]->getID()
                        && LeftOperand->Objects[i]->getLayerID() == RightOperand->Objects[j]->getLayerID()
                    ){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
        }
        else if(LeftOperand->type == "text"){
            result = containsTheSameModule(LeftOperand->Modules.Texts, RightOperand->Modules.Texts);
        }
        else if(LeftOperand->type == "editable_text"){
            result = containsTheSameModule(LeftOperand->Modules.EditableTexts, RightOperand->Modules.EditableTexts);
        }
        else if(LeftOperand->type == "super_text"){
            result = containsTheSameModule(LeftOperand->Modules.SuperTexts, RightOperand->Modules.SuperTexts);
        }
        else if(LeftOperand->type == "super_editable_text"){
            result = containsTheSameModule(LeftOperand->Modules.SuperEditableTexts, RightOperand->Modules.SuperEditableTexts);
        }
        else if(LeftOperand->type == "image"){
            result = containsTheSameModule(LeftOperand->Modules.Images, RightOperand->Modules.Images);
        }
        else if(LeftOperand->type == "movement"){
            result = containsTheSameModule(LeftOperand->Modules.Movements, RightOperand->Modules.Movements);
        }
        else if(LeftOperand->type == "collision"){
            result = containsTheSameModule(LeftOperand->Modules.Collisions, RightOperand->Modules.Collisions);
        }
        else if(LeftOperand->type == "particles"){
            result = containsTheSameModule(LeftOperand->Modules.Particles, RightOperand->Modules.Particles);
        }
        else if(LeftOperand->type == "event"){
            result = containsTheSameModule(LeftOperand->Modules.Events, RightOperand->Modules.Events);
        }
        else if(LeftOperand->type == "variable"){
            result = containsTheSameModule(LeftOperand->Modules.Variables, RightOperand->Modules.Variables);
        }
        else if(LeftOperand->type == "scrollbar"){
            result = containsTheSameModule(LeftOperand->Modules.Scrollbars, RightOperand->Modules.Scrollbars);
        }
        else if(LeftOperand->type == "primitives"){
            result = containsTheSameModule(LeftOperand->Modules.Primitives, RightOperand->Modules.Primitives);
        }
        else if(LeftOperand->type == "vector"){
            result = containsTheSameModule(LeftOperand->Modules.Vectors, RightOperand->Modules.Vectors);
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << LeftOperand->type << "\' type does not exist.\n";
        }
    }

    ContextClass NewContext;
    NewContext.Variables.push_back(VariableModule::newBool(result));
    NewContext.type = "value";
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
template <class Module>
void createNewModule(vector <Module> & Container, vector <string> & allIDs, vector<Module*> & Context, const unsigned & newVectorSize,
    const vector <string> & newIDs, string & layerID, string & objectID, vector<LayerClass> & Layers, vector<ContextClass> & EventContext,
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack,
    double reservationMultiplier, SuperEditableTextModule *& ActiveEditableText, EventDescription EventIds
){
    if(Container.size() + newVectorSize > Container.capacity()){
        PointerRecalculator Recalculator;
        Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
        Container.reserve((Container.size() + newVectorSize) * reservationMultiplier);
        Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
    }
    string ID = "";
    for(unsigned i = 0; i < newVectorSize; i++){
        if(i < newIDs.size()){
            ID = newIDs[i];
        }
        Container.push_back(Module(ID, &allIDs, layerID, objectID));
        Context.push_back(&Container.back());
    }
}
bool ProcessClass::prepareVectorSizeAndIDsForNew(vector<ContextClass> & EventContext, const vector<string> & dynamicIDs, const vector<VariableModule> & Literals, unsigned & newVectorSize, vector <string> & newIDs){
    if(Literals.size() == 0 || Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'new\' instruction requires destination type (the first string literal of the operation).\n";
        return false;
    }
    
    ContextClass * SizeContext = nullptr;
    ContextClass * IdContext = nullptr;
    if(Literals.size() > 1 && Literals.back().getType() == 'i'){
        newVectorSize = Literals.back().getInt();
        if(dynamicIDs.size() > 0){
            if(!getOneContext(IdContext, EventContext, dynamicIDs)){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
                return false;
            }
        }
    }
    else if(dynamicIDs.size() == 1){
        if(!getOneContext(SizeContext, EventContext, dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return false;
        }
    }
    else if(dynamicIDs.size() >= 2){
        if(!getPairOfContexts(SizeContext, IdContext, EventContext, dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return false;
        }
    }
    
    if(SizeContext != nullptr){
        if(!SizeContext->getUnsignedOrAbort(newVectorSize, EngineInstr::new_i, EventIds)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'new\' failed.\n";
            return false;
        }
    }
        
    if(IdContext != nullptr){
        if(IdContext->type == "value"){
            for(const VariableModule & ID : IdContext->Variables){
                newIDs.push_back(ID.getStringUnsafe());
            }
        }
        else if(IdContext->type == "pointer"){
            for(const BasePointersStruct & ID : IdContext->BasePointers){
                newIDs.push_back(ID.getString());
            }
        }
        else if(IdContext->type == "variable"){
            for(const VariableModule * ID : IdContext->Modules.Variables){
                newIDs.push_back(ID->getString());
            }
        }
    }
    return true;
}
bool ProcessClass::prepareDestinationForNew(OperaClass & Operation, vector<ContextClass> & EventContext, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, string & layerID, string & objectID, vector<LayerClass> &Layers){
    if(Operation.Literals.size() == 0 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'new\' instruction requires destination type (the first string literal of the operation).\n";
        return false;
    }
    
    if(Operation.Location.source == "camera" || Operation.Location.source == "layer"){
        return true;
    }

    if((Operation.Literals[0].getString() == "location" || Operation.Literals[0].getString() == "l") && Operation.Location.layerID != ""){
        for(LayerClass & Layer : Layers){
            if(Layer.getID() != Operation.Location.layerID){
                continue;
            }
            if(!Layer.getIsDeleted()){
                CurrentLayer = &Layer;
                layerID = CurrentLayer->getID();
                if(Operation.Location.source == "object"){
                    return true;
                }
            }
            break;
        }
        if(CurrentLayer != nullptr && Operation.Location.objectID != ""){
            for(AncestorObject & Object : CurrentLayer->Objects){
                if(Object.getID() != Operation.Location.objectID){
                    continue;
                }
                if(!Object.getIsDeleted()){
                    CurrentObject = &Object;
                    objectID = CurrentObject->getID();
                }
                break;
            }
        }   
    }
    else if(Operation.Literals[0].getString() == "context" || Operation.Literals[0].getString() == "c"){
        ContextClass * Context;
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return false;
        }
        if(Context->type == "layer" && Operation.Location.source == "object"){
            if(Context->Layers.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context has no layers.\n";
                return false;
            }
            CurrentLayer = Context->Layers.back();
            layerID = CurrentLayer->getID();
            return true;
        }
        else if(Context->type == "object" && Operation.Location.source != "object"){
            if(Context->Objects.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Context has no objects.\n";
                return false;
            }
            CurrentObject = Context->Objects.back();
            objectID = CurrentObject->getID();
            layerID = CurrentObject->getLayerID();
            return true;
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": You cannot create an entity of \'" << Operation.Location.source << "\' type in a context of \'" << Context->type << "\' type.\n";
            return false;
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": wrong destination type in the \'new\' instruction.\n";
        return false;
    }

    if(CurrentLayer == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Layer \'" << Operation.Location.layerID << "\' does not exist.\n";
        return false;
    }

    if(CurrentObject == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object \'" << Operation.Location.objectID << "\' does not exist.\n";
        return false;
    }

    return true;
}
void ProcessClass::createNewEntities(OperaClass & Operation, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer,
    AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, string & focusedProcessID
){
    LayerClass * CurrentLayer = nullptr;
    AncestorObject * CurrentObject = nullptr;
    string layerID = "", objectID = "";

    if(!prepareDestinationForNew(Operation, EventContext, CurrentLayer, CurrentObject, layerID, objectID, Layers)){
        return;
    }
    
    unsigned newVectorSize = 0;
    vector <string> newIDs;

    vector <string> contextIDs;

    if(Operation.Literals[0].getString() == "context" || Operation.Literals[0].getString() == "c"){
        contextIDs = vector<string>(Operation.dynamicIDs.begin()+1, Operation.dynamicIDs.end());
    }
    else{
        contextIDs = Operation.dynamicIDs;
    }

    if(!prepareVectorSizeAndIDsForNew(EventContext, contextIDs, Operation.Literals, newVectorSize, newIDs)){
        return;
    }

    if(newVectorSize == 0){
        cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No new entity will be created - the count of new entities is zero.\n";
        return;
    }

    string ID = "";

    if(printOutInstructions){
        cout << "new " << newVectorSize << " ";
        Operation.Location.print("");
        if(newIDs.size() > 0){
            cout << "[";
            for(const string & newID : newIDs){
                cout << newID << ", ";
            }
            cout << "]<" << newIDs.size() << ">";
        }
        cout << "\n";
    }

    ContextClass NewContext;

    NewContext.type = Operation.Location.source;
    if(Operation.Location.source == "camera"){
        if(Cameras.size() + newVectorSize > Cameras.capacity()){
            for(LayerClass & Layer : Layers){
                Layer.nullifyAllPointers();
            }
            PointerRecalculator Recalculator;
            Recalculator.findIndexesForCameras(Cameras, EventContext, SelectedCamera);
            Cameras.reserve((Cameras.size() + newVectorSize) * reservationMultiplier);
            Recalculator.updatePointersToCameras(Cameras, EventContext, SelectedCamera, getID(), focusedProcessID, EventIds);
        }
        for(unsigned i = 0; i < newVectorSize; i++){
            if(i < newIDs.size()){
                ID = newIDs[i];
            }
            Cameras.push_back(Camera2D());
            Cameras.back().setUpInstance(ID, camerasIDs, false, vec2d(0.0, 0.0), vec2d(50.0, 50.0), vec2d(0.0, 0.0));
            NewContext.Cameras.push_back(&Cameras.back());
            camerasOrder.push_back(camerasOrder.size());
        }
    }
    else if(Operation.Location.source == "layer"){
        if(Layers.size() + newVectorSize > Layers.capacity()){
            PointerRecalculator Recalculator;
            Recalculator.findIndexesForLayers(Layers, EventContext, OwnerLayer);
            Recalculator.findIndexesForObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject);
            Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
            Layers.reserve((Layers.size() + newVectorSize) * reservationMultiplier);
            Recalculator.updatePointersToLayers(Layers, EventContext, OwnerLayer, EventIds);
            Recalculator.updatePointersToObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject, EventIds);
            Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
        }
        for(unsigned i = 0; i < newVectorSize; i++){
            if(i < newIDs.size()){
                ID = newIDs[i];
            }
            Layers.push_back(LayerClass(ID, layersIDs));
            NewContext.Layers.push_back(&Layers.back());
        }
    }
    else if(Operation.Location.source == "object"){
        if(CurrentLayer->Objects.size() + newVectorSize > CurrentLayer->Objects.capacity()){
            PointerRecalculator Recalculator;
            Recalculator.findIndexesForObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject);
            Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
            CurrentLayer->Objects.reserve((CurrentLayer->Objects.size() + newVectorSize) * reservationMultiplier);
            Recalculator.updatePointersToObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject, EventIds);
            Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
        }
        for(unsigned i = 0; i < newVectorSize; i++){
            if(i < newIDs.size()){
                ID = newIDs[i];
            }
            CurrentLayer->Objects.push_back(AncestorObject());
            CurrentLayer->Objects.back().primaryConstructor(ID, &CurrentLayer->objectsIDs, layerID, "");
            NewContext.Objects.push_back(&CurrentLayer->Objects.back());
        }
    }
    else if(Operation.Location.source == "text"){
        createNewModule(CurrentObject->TextContainer, CurrentObject->textContainerIDs, NewContext.Modules.Texts,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->TextContainer.size() - 1; i >= long(CurrentObject->TextContainer.size() - newVectorSize); i--){
            CurrentObject->TextContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "editable_text"){
        createNewModule(CurrentObject->EditableTextContainer, CurrentObject->editableTextContainerIDs, NewContext.Modules.EditableTexts,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->EditableTextContainer.size() - 1; i >= long(CurrentObject->EditableTextContainer.size() - newVectorSize); i--){
            CurrentObject->EditableTextContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "super_text"){
        createNewModule(CurrentObject->SuperTextContainer, CurrentObject->superTextContainerIDs, NewContext.Modules.SuperTexts,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->SuperTextContainer.size() - 1; i >= long(CurrentObject->SuperTextContainer.size() - newVectorSize); i--){
            CurrentObject->SuperTextContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "super_editable_text"){
        createNewModule(CurrentObject->SuperEditableTextContainer, CurrentObject->superEditableTextContainerIDs, NewContext.Modules.SuperEditableTexts,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->SuperEditableTextContainer.size() - 1; i >= long(CurrentObject->SuperEditableTextContainer.size() - newVectorSize); i--){
            CurrentObject->SuperEditableTextContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "image"){
        createNewModule(CurrentObject->ImageContainer, CurrentObject->imageContainerIDs, NewContext.Modules.Images,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->ImageContainer.size() - 1; i >= long(CurrentObject->ImageContainer.size() - newVectorSize); i--){
            CurrentObject->ImageContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "movement"){
        createNewModule(CurrentObject->MovementContainer, CurrentObject->movementContainerIDs, NewContext.Modules.Movements,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->MovementContainer.size() - 1; i >= long(CurrentObject->MovementContainer.size() - newVectorSize); i--){
            CurrentObject->MovementContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "collision"){
        createNewModule(CurrentObject->CollisionContainer, CurrentObject->collisionContainerIDs, NewContext.Modules.Collisions,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->CollisionContainer.size() - 1; i >= long(CurrentObject->CollisionContainer.size() - newVectorSize); i--){
            CurrentObject->CollisionContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "particles"){
        createNewModule(CurrentObject->ParticlesContainer, CurrentObject->particlesContainerIDs, NewContext.Modules.Particles,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->ParticlesContainer.size() - 1; i >= long(CurrentObject->ParticlesContainer.size() - newVectorSize); i--){
            CurrentObject->ParticlesContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "event"){
        createNewModule(CurrentObject->EveContainer, CurrentObject->eveContainerIDs, NewContext.Modules.Events,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
    }
    else if(Operation.Location.source == "variable"){
        createNewModule(CurrentObject->VariablesContainer, CurrentObject->variablesContainerIDs, NewContext.Modules.Variables,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
    }
    else if(Operation.Location.source == "scrollbar"){
        createNewModule(CurrentObject->ScrollbarContainer, CurrentObject->scrollbarContainerIDs, NewContext.Modules.Scrollbars,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->ScrollbarContainer.size() - 1; i >= long(CurrentObject->ScrollbarContainer.size() - newVectorSize); i--){
            CurrentObject->ScrollbarContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "primitives"){
        createNewModule(CurrentObject->PrimitivesContainer, CurrentObject->primitivesContainerIDs, NewContext.Modules.Primitives,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
        for(long i = CurrentObject->PrimitivesContainer.size() - 1; i >= long(CurrentObject->PrimitivesContainer.size() - newVectorSize); i--){
            CurrentObject->PrimitivesContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
        }
    }
    else if(Operation.Location.source == "vector"){
        createNewModule(CurrentObject->VectorContainer, CurrentObject->vectorContainerIDs, NewContext.Modules.Vectors,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack,
            reservationMultiplier, ActiveEditableText, EventIds
        );
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Entity type \'" << transInstrToStr(Operation.instruction) << "\' does not exist.\n";
    }

    if(NewContext.type != ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
        wasNewExecuted = true;
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
template <class T>
void clearDeletedPointersFromVector(vector<T*> & Vector){
    for(auto Instance = Vector.begin(); Instance != Vector.end();){
        if((*Instance)->getIsDeleted()){
            Instance = Vector.erase(Instance);
        }
        else{
            ++Instance;
        }
    }
}
void ProcessClass::markEntitiesForDeletion(OperaClass & Operation, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer,
    AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, string & focusedProcessID
){
    ContextClass * DeletedContext = nullptr;
    if(!getOneContext(DeletedContext, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
        return;
    }

    if(DeletedContext->size() == 0 || DeletedContext->type == ""){
        cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Nothing to delete from selected context.\n";
        return;
    }

    if(printOutInstructions){
        cout << "delete " << DeletedContext->getValue(EventIds) << "\n";
    }

    wasDeleteExecuted = true;

    if(DeletedContext->type == "camera"){
        for(Camera2D * Camera : DeletedContext->Cameras){
            if(Camera != nullptr){
                Camera->deleteLater();
            }
        }
        if(SelectedCamera != nullptr && SelectedCamera->getIsDeleted()){
            unfocusCameras(Cameras, SelectedCamera, getID(), focusedProcessID);
        }
        for(ContextClass & Context : EventContext){
            if(Context.type == "camera"){
                clearDeletedPointersFromVector(Context.Cameras);
            }
        }
    }
    else if(DeletedContext->type == "layer"){
        for(LayerClass * Layer : DeletedContext->Layers){
            if(Layer != nullptr){
                Layer->deleteLater();
            }
        }
        if(OwnerLayer != nullptr && OwnerLayer->getIsDeleted()){
            OwnerLayer = nullptr;
        }
        if(SelectedLayer != nullptr && SelectedLayer->getIsDeleted()){
            SelectedLayer = nullptr;
        }
        for(ContextClass & Context : EventContext){
            if(Context.type == "layer"){
                clearDeletedPointersFromVector(Context.Layers);
            }
        }
    }
    else if(DeletedContext->type == "object"){
        for(AncestorObject * Object : DeletedContext->Objects){
            if(Object != nullptr){
                Object->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "text"){
        for(TextModule * Text : DeletedContext->Modules.Texts){
            if(Text != nullptr){
                Text->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "editable_text"){
        for(EditableTextModule * EditableText : DeletedContext->Modules.EditableTexts){
            if(EditableText != nullptr){
                EditableText->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "super_text"){
        for(SuperTextModule * SuperText : DeletedContext->Modules.SuperTexts){
            if(SuperText != nullptr){
                SuperText->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "super_editable_text"){
        for(SuperEditableTextModule * SuperEditableText : DeletedContext->Modules.SuperEditableTexts){
            if(SuperEditableText != nullptr){
                SuperEditableText->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "image"){
        for(ImageModule * Image : DeletedContext->Modules.Images){
            if(Image != nullptr){
                Image->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "movement"){
        for(MovementModule * Movement : DeletedContext->Modules.Movements){
            if(Movement != nullptr){
                Movement->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "collision"){
        for(CollisionModule * Collision : DeletedContext->Modules.Collisions){
            if(Collision != nullptr){
                Collision->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "particles"){
        for(ParticleEffectModule * Particle : DeletedContext->Modules.Particles){
            if(Particle != nullptr){
                Particle->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "event"){
        for(EveModule * Event : DeletedContext->Modules.Events){
            if(Event != nullptr){
                Event->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "variable"){
        for(VariableModule * Variable : DeletedContext->Modules.Variables){
            if(Variable != nullptr){
                Variable->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "scrollbar"){
        for(ScrollbarModule * Scrollbar : DeletedContext->Modules.Scrollbars){
            if(Scrollbar != nullptr){
                Scrollbar->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "primitives"){
        for(PrimitivesModule * Primitives : DeletedContext->Modules.Primitives){
            if(Primitives != nullptr){
                Primitives->deleteLater();
            }
        }
    }
    else if(DeletedContext->type == "vector"){
        for(VectorModule * Vector : DeletedContext->Modules.Vectors){
            if(Vector != nullptr){
                Vector->deleteLater();
            }
        }
    }

    if(DeletedContext->type == "layer" || DeletedContext->type == "object"){
        if(Owner != nullptr && Owner->getIsDeleted()){
            Owner = nullptr;
        }
        if(SelectedObject != nullptr && SelectedObject->getIsDeleted()){
            SelectedObject = nullptr;
        }
        if(ActiveEditableText != nullptr && ActiveEditableText->getIsDeleted()){
            ActiveEditableText = nullptr;
        }
        for(AncestorObject *& Object : TriggeredObjects){
            if(Object != nullptr && Object->getIsDeleted()){
                Object = nullptr;
            }
        }
        for(ContextClass & Context : EventContext){
            if(Context.type == "object"){
                clearDeletedPointersFromVector(Context.Objects);
            }
        }
    }

    if(isStringInGroup(DeletedContext->type, 15, "layer", "object", "text", "editable_text", "super_text", "super_editable_text", "image", "movement",
        "collision", "particles", "event", "variable", "scrollbar", "primitives", "vector")
    ){
        for(ContextClass & Context : EventContext){
            if(Context.type == "text"){
                clearDeletedPointersFromVector(Context.Modules.Texts);
            }
            else if(Context.type == "editable_text"){
                clearDeletedPointersFromVector(Context.Modules.EditableTexts);
            }
            else if(Context.type == "super_text"){
                clearDeletedPointersFromVector(Context.Modules.SuperTexts);
            }
            else if(Context.type == "super_editable_text"){
                clearDeletedPointersFromVector(Context.Modules.SuperEditableTexts);
            }
            else if(Context.type == "image"){
                clearDeletedPointersFromVector(Context.Modules.Images);
            }
            else if(Context.type == "movement"){
                clearDeletedPointersFromVector(Context.Modules.Movements);
            }
            else if(Context.type == "collision"){
                clearDeletedPointersFromVector(Context.Modules.Collisions);
            }
            else if(Context.type == "particles"){
                clearDeletedPointersFromVector(Context.Modules.Particles);
            }
            else if(Context.type == "event"){
                clearDeletedPointersFromVector(Context.Modules.Events);
            }
            else if(Context.type == "variable"){
                clearDeletedPointersFromVector(Context.Modules.Variables);
            }
            else if(Context.type == "scrollbar"){
                clearDeletedPointersFromVector(Context.Modules.Scrollbars);
            }
            else if(Context.type == "primitives"){
                clearDeletedPointersFromVector(Context.Modules.Primitives);
            }
            else if(Context.type == "vector"){
                clearDeletedPointersFromVector(Context.Modules.Vectors);
            }
        }
    }
}
template <class Entity>
void findInstanceInVectorByIndex(vector<unsigned> indexes, vector<Entity> & Aggregated, string type, vector<Entity*> & NewVector, string & newType){
    unsigned realIndex = 0;
    for(unsigned index : indexes){
        for(realIndex = 0; realIndex < Aggregated.size(); realIndex++){
            if(Aggregated[realIndex].getIsDeleted()){
                index++;
            }
            else if(index == realIndex){
                NewVector.push_back(&Aggregated[index]);
                newType = type;
                break;
            }
        }
    }
}
template <class Entity>
void findInstanceInVectorByIndex(vector<unsigned> indexes, vector<Entity> & Aggregated, string type, vector<Entity> & NewVector, string & newType, EventDescription EventIds){
    if(type != "pointer" && type != "value"){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": This function allows only entities of \'pointer\' and \'value\' types.\n";
        return;
    }
    for(unsigned index : indexes){
        if(index < Aggregated.size()){
            NewVector.push_back(Aggregated[index]);
            newType = type;
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Index is out of vector's scope.\n";
        }
    }
}
template <class Entity>
void findInstanceInVectorByIndex(vector<unsigned> indexes, vector<Entity*> & Aggregated, string type, vector<Entity*> & NewVector, string & newType){
    unsigned realIndex = 0;
    for(unsigned index : indexes){
        for(realIndex = 0; realIndex < Aggregated.size(); realIndex++){
            if(Aggregated[realIndex]->getIsDeleted()){
                index++;
            }
            else if(index == realIndex){
                NewVector.push_back(Aggregated[index]);
                newType = type;
                break;
            }
        }
    }
}
void ProcessClass::getIndexes(const vector<VariableModule> & Literals, const vector<string> & dynamicIDs, vector<unsigned> & indexes, vector<ContextClass> & EventContext){
    for(const VariableModule & Variable : Literals){
        if(Variable.getType() == 'i'){
            indexes.push_back(Variable.getInt());
        }
        else{
            indexes.push_back(0);
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Each literal should be of integer type.\n";
        }
    }

    if(dynamicIDs.size() == 0){
        return;
    }

    vector<ContextClass*> IndexContexts;
    int singleIndex;

    if(getAllSelectedContexts(IndexContexts, EventContext, dynamicIDs)){
        for(ContextClass * Index : IndexContexts){
            if(!Index->getIntOrAbort(singleIndex, EngineInstr::index_i, EventIds)){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'index\' failed.\n";
                return;
            }
            indexes.push_back(singleIndex);
        }
    }

    if(indexes.size() == 0){
        indexes.push_back(0);
    }
}
void ProcessClass::getReferenceByIndex(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass * Context = nullptr, NewContext;
    vector<unsigned> indexes;
    unsigned realIndex = 0, entityIndex = 0;

    if(Operation.Location.source != "" && Operation.Location.source != "context" && Operation.Location.source != "c"){
        getIndexes(Operation.Literals, Operation.dynamicIDs, indexes, EventContext);

        if(indexes.size() == 0){
            indexes.push_back(0);
        }

        if(printOutInstructions){
            if(Operation.Location.attribute != ""){
                cout << "index " << Operation.Location.source << " " << Operation.Location.attribute << " [";
            }
            else{
                cout << "index " << Operation.Location.source << " [";
            }
            
            for(const unsigned & i : indexes){
                cout << i << ", ";
            }
            cout << "]\n";
        }

        if(Operation.Location.source == "camera"){
            findInstanceInVectorByIndex(indexes, Cameras, "camera", NewContext.Cameras, NewContext.type);
        }
        else if(Operation.Location.source == "layer"){
            if(Operation.Location.attribute == "layer" || Operation.Location.attribute == ""){
                findInstanceInVectorByIndex(indexes, Layers, "layer", NewContext.Layers, NewContext.type);
            }
            else if(isStringInGroup(Operation.Location.attribute, 14, "object", "text", "editable_text", "super_text", "super_editable_text", "image", "movement",
                "collision", "particles", "event", "variable", "scrollbar", "primitives", "vector")
            ){
                if(indexes.size() < 2){
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__ << ": In order to find an object by its index, you must provide at least 2 indexes.\n";
                    return;
                }
                entityIndex = indexes[0];
                LayerClass * Layer = nullptr;
                for(realIndex = 0; realIndex < Layers.size(); realIndex++){
                    if(Layers[realIndex].getIsDeleted()){
                        entityIndex++;
                    }
                    else if(entityIndex == realIndex){
                        Layer = &Layers[entityIndex];
                        break;
                    }
                }
                if(Layer == nullptr){
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__ << ": Index is out of layers vector's scope.\n";
                    return;
                }
                if(Operation.Location.attribute == "object"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Layer->Objects, "object", NewContext.Objects, NewContext.type);
                }
                else{
                    if(indexes.size() < 3){
                        cout << "Error: In " << EventIds.describe()
                            << ": In " << __FUNCTION__ << ": In order to find a module instance by its index, you must provide at least 3 indexes.\n";
                        return;
                    }
                    entityIndex = indexes[1];
                    AncestorObject * Object = nullptr;
                    for(realIndex = 0; realIndex < Layer->Objects.size(); realIndex++){
                        if(Layer->Objects[realIndex].getIsDeleted()){
                            entityIndex++;
                        }
                        else if(entityIndex == realIndex){
                            Object = &Layer->Objects[entityIndex];
                            break;
                        }
                    }
                    if(Object == nullptr){
                        cout << "Error: In " << EventIds.describe()
                            << ": In " << __FUNCTION__ << ": Index is out of objects vector's scope.\n";
                        return;
                    }
                    if(Operation.Location.attribute == "text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->TextContainer, Operation.Location.attribute, NewContext.Modules.Texts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "editable_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->EditableTextContainer, Operation.Location.attribute, NewContext.Modules.EditableTexts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "super_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->SuperTextContainer, Operation.Location.attribute, NewContext.Modules.SuperTexts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "super_editable_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->SuperEditableTextContainer, Operation.Location.attribute, NewContext.Modules.SuperEditableTexts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "image"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->ImageContainer, Operation.Location.attribute, NewContext.Modules.Images, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "movement"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->MovementContainer, Operation.Location.attribute, NewContext.Modules.Movements, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "collision"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->CollisionContainer, Operation.Location.attribute, NewContext.Modules.Collisions, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "particles"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->ParticlesContainer, Operation.Location.attribute, NewContext.Modules.Particles, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "event"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->EveContainer, Operation.Location.attribute, NewContext.Modules.Events, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "variable"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->VariablesContainer, Operation.Location.attribute, NewContext.Modules.Variables, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "scrollbar"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->ScrollbarContainer, Operation.Location.attribute, NewContext.Modules.Scrollbars, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "primitives"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->PrimitivesContainer, Operation.Location.attribute, NewContext.Modules.Primitives, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "vector"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->VectorContainer, Operation.Location.attribute, NewContext.Modules.Vectors, NewContext.type);
                    }
                    else{
                        cout << "Error: In " << EventIds.describe()
                            << ": In " << __FUNCTION__ << ": Module type \'" << Operation.Location.attribute << "\' does not exist.\n";
                        return;
                    }
                }
            }
        }
        else{
            cout << "Error: In " << EventIds.describe()
                << ": In " << __FUNCTION__ << ": Source cannot have \'" << Operation.Location.source << "\' value.\n";
            return;
        }

        if(NewContext.type == ""){
            cout << "Error: In " << EventIds.describe()
                << ": In " << __FUNCTION__ << ": Index is out of vector's scope.\n";
            return;
        }
    }
    else{
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe()
                << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }

        getIndexes(Operation.Literals, vector<string>(Operation.dynamicIDs.begin()+1, Operation.dynamicIDs.end()), indexes, EventContext);
        
        if(indexes.size() == 0){
            indexes.push_back(0);
        }

        if(printOutInstructions){
            if(Operation.Location.attribute != ""){
                cout << "index " << Context->type << " " << Operation.Location.attribute << " [";
            }
            else{
                cout << "index " << Context->type << " [";
            }
            
            for(const unsigned & i : indexes){
                cout << i << ", ";
            }
            cout << "]\n";
        }
        
        if(Context->type == "camera"){
            findInstanceInVectorByIndex(indexes, Context->Cameras, "camera", NewContext.Cameras, NewContext.type);
        }
        else if(Context->type == "layer"){
            if(Operation.Location.attribute == "layer"){
                findInstanceInVectorByIndex(indexes, Context->Layers, "layer", NewContext.Layers, NewContext.type);
            }
            else{
                if(indexes.size() < 2){
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__ << ": In order to find an object by its index, you must provide at least 2 index.\n";
                    return;
                }
                LayerClass * Layer = nullptr;
                entityIndex = indexes[0];
                for(realIndex = 0; realIndex < Context->Layers.size(); realIndex++){
                    if(Context->Layers[realIndex]->getIsDeleted()){
                        entityIndex++;
                    }
                    else if(entityIndex == realIndex){
                        Layer = Context->Layers[entityIndex];
                        break;
                    }
                }
                
                if(Layer == nullptr){
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__ << ": Index is out of layers vector's scope.\n";
                    return;
                }

                if(Operation.Location.attribute == "object"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Layer->Objects, "object", NewContext.Objects, NewContext.type);
                }
                else{
                    if(indexes.size() < 3){
                        cout << "Error: In " << EventIds.describe()
                            << ": In " << __FUNCTION__ << ": In order to find a module instance by its index, you must provide at least 3 indexes.\n";
                        return;
                    }
                    entityIndex = indexes[1];
                    AncestorObject * Object = nullptr;
                    for(realIndex = 0; realIndex < Layer->Objects.size(); realIndex++){
                        if(Layer->Objects[realIndex].getIsDeleted()){
                            entityIndex++;
                        }
                        else if(entityIndex == realIndex){
                            Object = &Layer->Objects[entityIndex];
                            break;
                        }
                    }
                    if(Object == nullptr){
                        cout << "Error: In " << EventIds.describe()
                            << ": In " << __FUNCTION__ << ": Index is out of objects vector's scope.\n";
                        return;
                    }

                    if(Operation.Location.attribute == "text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->TextContainer, Operation.Location.attribute, NewContext.Modules.Texts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "editable_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->EditableTextContainer, Operation.Location.attribute, NewContext.Modules.EditableTexts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "super_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->SuperTextContainer, Operation.Location.attribute, NewContext.Modules.SuperTexts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "super_editable_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->SuperEditableTextContainer, Operation.Location.attribute, NewContext.Modules.SuperEditableTexts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "image"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->ImageContainer, Operation.Location.attribute, NewContext.Modules.Images, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "movement"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->MovementContainer, Operation.Location.attribute, NewContext.Modules.Movements, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "collision"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->CollisionContainer, Operation.Location.attribute, NewContext.Modules.Collisions, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "particles"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->ParticlesContainer, Operation.Location.attribute, NewContext.Modules.Particles, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "event"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->EveContainer, Operation.Location.attribute, NewContext.Modules.Events, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "variable"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->VariablesContainer, Operation.Location.attribute, NewContext.Modules.Variables, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "scrollbar"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->ScrollbarContainer, Operation.Location.attribute, NewContext.Modules.Scrollbars, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "primitives"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->PrimitivesContainer, Operation.Location.attribute, NewContext.Modules.Primitives, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "vector"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->VectorContainer, Operation.Location.attribute, NewContext.Modules.Vectors, NewContext.type);
                    }
                    else{
                        cout << "Error: In " << EventIds.describe()
                            << ": In " << __FUNCTION__ << ": Module type \'" << Operation.Location.attribute << "\' does not exist.\n";
                        return;
                    }
                }
            }
        }
        else if(Context->type == "object"){
            if(Operation.Location.attribute == "object"){
                findInstanceInVectorByIndex(indexes, Context->Objects, "object", NewContext.Objects, NewContext.type);
            }
            else{
                if(indexes.size() < 2){
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__  << ": In order to find a module instance by its index, you must provide at least 2 indexes.\n";
                    return;
                }
                unsigned entityIndex = indexes[0];
                AncestorObject * Object = nullptr;
                for(realIndex = 0; realIndex < Context->Objects.size(); realIndex++){
                    if(Context->Objects[realIndex]->getIsDeleted()){
                        entityIndex++;
                    }
                    else if(entityIndex == realIndex){
                        Object = Context->Objects[entityIndex];
                        break;
                    }
                }
                if(Object == nullptr){
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__ << ": Index is out of objects vector's scope.\n";
                    return;
                }
                
                if(Operation.Location.attribute == "text"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->TextContainer, Operation.Location.attribute, NewContext.Modules.Texts, NewContext.type);
                }
                else if(Operation.Location.attribute == "editable_text"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->EditableTextContainer, Operation.Location.attribute, NewContext.Modules.EditableTexts, NewContext.type);
                }
                else if(Operation.Location.attribute == "super_text"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->SuperTextContainer, Operation.Location.attribute, NewContext.Modules.SuperTexts, NewContext.type);
                }
                else if(Operation.Location.attribute == "super_editable_text"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->SuperEditableTextContainer, Operation.Location.attribute, NewContext.Modules.SuperEditableTexts, NewContext.type);
                }
                else if(Operation.Location.attribute == "image"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->ImageContainer, Operation.Location.attribute, NewContext.Modules.Images, NewContext.type);
                }
                else if(Operation.Location.attribute == "movement"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->MovementContainer, Operation.Location.attribute, NewContext.Modules.Movements, NewContext.type);
                }
                else if(Operation.Location.attribute == "collision"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->CollisionContainer, Operation.Location.attribute, NewContext.Modules.Collisions, NewContext.type);
                }
                else if(Operation.Location.attribute == "particles"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->ParticlesContainer, Operation.Location.attribute, NewContext.Modules.Particles, NewContext.type);
                }
                else if(Operation.Location.attribute == "event"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->EveContainer, Operation.Location.attribute, NewContext.Modules.Events, NewContext.type);
                }
                else if(Operation.Location.attribute == "variable"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->VariablesContainer, Operation.Location.attribute, NewContext.Modules.Variables, NewContext.type);
                }
                else if(Operation.Location.attribute == "scrollbar"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->ScrollbarContainer, Operation.Location.attribute, NewContext.Modules.Scrollbars, NewContext.type);
                }
                else if(Operation.Location.attribute == "primitives"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->PrimitivesContainer, Operation.Location.attribute, NewContext.Modules.Primitives, NewContext.type);
                }
                else if(Operation.Location.attribute == "vector"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->VectorContainer, Operation.Location.attribute, NewContext.Modules.Vectors, NewContext.type);
                }
                else{
                    cout << "Error: In " << EventIds.describe()
                        << ": In " << __FUNCTION__ << ": Module type \'" << Operation.Location.attribute << "\' does not exist.\n";
                    return;
                }
            }
        }
        else if(Context->type == "text"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Texts, Context->type, NewContext.Modules.Texts, NewContext.type);
        }
        else if(Context->type == "editable_text"){
            findInstanceInVectorByIndex(indexes, Context->Modules.EditableTexts, Context->type, NewContext.Modules.EditableTexts, NewContext.type);
        }
        else if(Context->type == "super_text"){
            findInstanceInVectorByIndex(indexes, Context->Modules.SuperTexts, Context->type, NewContext.Modules.SuperTexts, NewContext.type);
        }
        else if(Context->type == "super_editable_text"){
            findInstanceInVectorByIndex(indexes, Context->Modules.SuperEditableTexts, Context->type, NewContext.Modules.SuperEditableTexts, NewContext.type);
        }
        else if(Context->type == "image"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Images, Context->type, NewContext.Modules.Images, NewContext.type);
        }
        else if(Context->type == "movement"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Movements, Context->type, NewContext.Modules.Movements, NewContext.type);
        }
        else if(Context->type == "collision"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Collisions, Context->type, NewContext.Modules.Collisions, NewContext.type);
        }
        else if(Context->type == "particles"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Particles, Context->type, NewContext.Modules.Particles, NewContext.type);
        }
        else if(Context->type == "event"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Events, Context->type, NewContext.Modules.Events, NewContext.type);
        }
        else if(Context->type == "variable"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Variables, Context->type, NewContext.Modules.Variables, NewContext.type);
        }
        else if(Context->type == "scrollbar"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Scrollbars, Context->type, NewContext.Modules.Scrollbars, NewContext.type);
        }
        else if(Context->type == "primitives"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Primitives, Context->type, NewContext.Modules.Primitives, NewContext.type);
        }
        else if(Context->type == "vector"){
            findInstanceInVectorByIndex(indexes, Context->Modules.Vectors, Context->type, NewContext.Modules.Vectors, NewContext.type);
        }
        else if(Context->type == "pointer"){
            findInstanceInVectorByIndex(indexes, Context->BasePointers, Context->type, NewContext.BasePointers, NewContext.type, EventIds);
        }
        else if(Context->type == "value"){
            findInstanceInVectorByIndex(indexes, Context->Variables, Context->type, NewContext.Variables, NewContext.type, EventIds);
        }
        else{
            cout << "Error: In " << EventIds.describe()
                << ": In " << __FUNCTION__ << ": Context of \'" << Context->type << "\' type is invalid.\n";
            return;
        }
    }

    if(NewContext.type != ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
    }
    else{
        cout << "Error: In " << EventIds.describe()
            << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ProcessClass::bindFilesToObjects(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind requires at least one context.\n";
        return;
    }
    if(Operation.Literals.size() == 0 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind requires at least one literal of string type.\n";
        return;
    }
    if(isStringInGroup(Operation.Literals[0].getString(), 5, "literal", "l", "remove_literal", "rliteral", "rl")){
        if(Operation.Literals.size() == 1){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind requires at least two literals of string type.\n";
            return;
        }
        ContextClass * ContextA;
        if(!getOneContext(ContextA, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(ContextA->Objects.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one object.\n";
            return;
        }
        if(printOutInstructions){
            cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
        }
        for(AncestorObject * Object : ContextA->Objects){
            for(const VariableModule & Variable : vector<VariableModule>(Operation.Literals.begin()+1, Operation.Literals.end())){
                if(Variable.getType() != 's'){
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only literals of \'string'; type.\n";
                    continue;
                }
                if(printOutInstructions){
                    cout << "\"" << Variable.getString() << "\", ";
                }
                if(isStringInGroup(Operation.Literals[0].getString(), 2, "literal", "l")){
                    Object->bindedScripts.push_back(EXE_PATH + workingDirectory + Variable.getString());
                }
                else{
                    removeFromStringVector(Object->bindedScripts, EXE_PATH + workingDirectory + Variable.getString());
                }
            }
        }
        if(printOutInstructions){
            cout << "]\n";
        }
    }
    else if(isStringInGroup(Operation.Literals[0].getString(), 5, "context", "c", "remove_context", "rcontext", "rc")){
        ContextClass * ContextObject, * ContextFiles;
        if(!getPairOfContexts(ContextObject, ContextFiles, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(ContextObject->Objects.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one object.\n";
            return;
        }
        if(ContextFiles->type == "pointer"){
            if(ContextFiles->BasePointers.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one pointer.\n";
                return;
            }
            if(printOutInstructions){
                cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
            }
            for(AncestorObject * Object : ContextObject->Objects){
                for(const BasePointersStruct & Pointer : ContextFiles->BasePointers){
                    if(Pointer.type != "string"){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only pointers of \'string'; type.\n";
                        continue;
                    }
                    if(printOutInstructions){
                        cout << "\"" << Pointer.getString() << "\", ";
                    }
                    if(isStringInGroup(Operation.Literals[0].getString(), 2, "context", "c")){
                        Object->bindedScripts.push_back(EXE_PATH + workingDirectory + Pointer.getString());
                    }
                    else{
                        removeFromStringVector(Object->bindedScripts, EXE_PATH + workingDirectory + Pointer.getString());
                    }
                }
            }
            if(printOutInstructions){
                cout << "]\n";
            }
        }
        else if(ContextFiles->type == "value"){
            if(ContextFiles->Variables.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one pointer.\n";
                return;
            }
            if(printOutInstructions){
                cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
            }
            for(AncestorObject * Object : ContextObject->Objects){
                for(const VariableModule & Variable : ContextFiles->Variables){
                    if(Variable.getType() != 's'){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only literals of \'string'; type.\n";
                        continue;
                    }
                    if(printOutInstructions){
                        cout << "\"" << Variable.getString() << "\", ";
                    }
                    if(isStringInGroup(Operation.Literals[0].getString(), 2, "context", "c")){
                        Object->bindedScripts.push_back(EXE_PATH + workingDirectory + Variable.getString());
                    }
                    else{
                        removeFromStringVector(Object->bindedScripts, EXE_PATH + workingDirectory + Variable.getString());
                    }
                }
            }
            if(printOutInstructions){
                cout << "]\n";
            }
        }
        else if(ContextFiles->type == "variable"){
            if(ContextFiles->Modules.Variables.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one pointer.\n";
                return;
            }
            if(printOutInstructions){
                cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
            }
            for(AncestorObject * Object : ContextObject->Objects){
                for(const VariableModule * Variable : ContextFiles->Modules.Variables){
                    if(Variable->getType() != 's'){
                        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only literals of \'string'; type.\n";
                        continue;
                    }
                    if(printOutInstructions){
                        cout << "\"" << Variable->getString() << "\", ";
                    }
                    if(isStringInGroup(Operation.Literals[0].getString(), 2, "context", "c")){
                        Object->bindedScripts.push_back(EXE_PATH + workingDirectory + Variable->getString());
                    }
                    else{
                        removeFromStringVector(Object->bindedScripts, EXE_PATH + workingDirectory + Variable->getString());
                    }
                }
            }
            if(printOutInstructions){
                cout << "]\n";
            }
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " only accepts file input of types: \'pointer\' and \'value\'.\n";
            return;
        }
    }
    else if(Operation.Literals[0].getString() == "reset" || Operation.Literals[0].getString() == "r"){
        ContextClass * ContextObject;
        if(!getOneContext(ContextObject, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(ContextObject->Objects.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one object.\n";
            return;
        }
        if(printOutInstructions){
            cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << "\n";
        }
        for(AncestorObject * Object : ContextObject->Objects){
            Object->bindedScripts.clear();
        }
    }
}
void ProcessClass::buildEventsInObjects(OperaClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, bool allowNotAscii
){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Build requires at least one context.\n";
        return;
    }
    ContextClass * ContextA;
    if(!getOneContext(ContextA, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(ContextA->Objects.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Build requires at least one object.\n";
        return;
    }
    bool canResetEvents = false;
    if(Operation.Literals.size() > 0 && Operation.Literals[0].getType() == 'b'){
        canResetEvents = Operation.Literals[0].getBool();
    }
    if(printOutInstructions){
        if(canResetEvents){
            cout << "build " << Operation.dynamicIDs[0] << " true\n";
        }
        else{
            cout << "build " << Operation.dynamicIDs[0] << " false\n";
        }
    }

    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
    
    for(AncestorObject * Object : ContextA->Objects){
        if(canResetEvents && Object == Owner){
            std::cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Cannot delete events of the owner of the currently executed event.\n";
            continue;
        }
        Object->translateAllScripts(canResetEvents, allowNotAscii);
        wasAnyEventUpdated = true;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
}
void ProcessClass::customBuildEventsInObjects(OperaClass & Operation, vector<ContextClass> & EventContext, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, char mode, bool allowNotAscii
){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " instruction requires at least one context.\n";
        return;
    }
    ContextClass * ObjectContext = nullptr;
    if(!getOneContext(ObjectContext, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(ObjectContext->Objects.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " requires at least one object.\n";
        return;
    }

    vector <string> stringVec;

    if(Operation.dynamicIDs.size() > 1){
        ContextClass * StringVecContext = getContextByID(EventContext, Operation.dynamicIDs[1], false, EventIds);
        if(StringVecContext == nullptr){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(StringVecContext->type == "value"){
            for(const VariableModule & Variable : StringVecContext->Variables){
                stringVec.push_back(Variable.getString());
            }
        }
        else if(StringVecContext->type == "pointer"){
            for(const BasePointersStruct & Pointer : StringVecContext->BasePointers){
                stringVec.push_back(Pointer.getString());
            }
        }
        else if(StringVecContext->type == "variable"){
            for(const VariableModule * Variable : StringVecContext->Modules.Variables){
                stringVec.push_back(Variable->getString());
            }
        }
        else{
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Cannot extract string value from context of \'" << StringVecContext->type << "\' type.\n";
        }
    }

    for(const VariableModule & Variable : Operation.Literals){
        stringVec.push_back(Variable.getString());
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << " " << Operation.dynamicIDs[1] << " ";
        if(Operation.Literals.size() > 0){
            cout << "[\"";
            for(const VariableModule & Variable : Operation.Literals){
                cout << Variable.getString() << ", ";
            }
            cout << "]";
        }
        cout << "\n";
    }

    if(stringVec.size() == 0){
        cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No strings were provided.\n";
        return;
    }

    //All indexes linked with events must be recalculated - after adding new events, some contexts of event type might be invalid.  
    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
    
    for(AncestorObject * Object : ObjectContext->Objects){
        if(mode == 'p'){
            for(string & path : stringVec){
                path = EXE_PATH + workingDirectory + path;
            }
            Object->translateScriptsFromPaths(stringVec, allowNotAscii);
        }
        else if(mode == 's'){
            for(string & path : stringVec){
                path = EXE_PATH + workingDirectory + path;
            }
            Object->translateSubsetBindedScripts(stringVec, allowNotAscii);
        }
        else if(mode == 'c'){
            Object->injectCode(stringVec);
        }
        else if(mode == 'i'){
            bool isInsideStringSector = false;
            vector <string> preprocessed = {""};
            for(size_t i = 0; i < stringVec.size(); i++){
                for(size_t j = 0; j < stringVec[i].size(); j++){
                    if(stringVec[i][j] == '\"' && (j == 0 || stringVec[i][j - 1] != '\\')){
                        isInsideStringSector = !isInsideStringSector;
                    }
                    if(!isInsideStringSector && stringVec[i][j] == '\n'){
                        preprocessed.push_back("");
                    }
                    else{
                        preprocessed.back() += stringVec[i][j];
                    }
                }
            }
            if(isInsideStringSector){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": String section was not closed in lines:\n";
                for(string line : stringVec){
                    cout << "\t" << line << "\n";
                }
            }
            Object->injectInstructions(preprocessed);
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << mode << "\' mode does not exist."
                << "Allowed modes: p (uses translateScriptsFromPaths), s (uses translateSubsetBindedScripts),"
                << "c (uses injectCode), i (uses injectInstructions)\n";
            return;
        }
        
        wasAnyEventUpdated = true;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
}
void ProcessClass::clearEventsInObjects(OperaClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " instruction requires at least one context.\n";
        return;
    }
    ContextClass * ContextA;
    if(!getOneContext(ContextA, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(ContextA->Objects.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " requires at least one object.\n";
        return;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << "\n";
    }

    for(AncestorObject * Object : ContextA->Objects){
        if(Object == Owner){
            std::cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Cannot delete events of the owner of the currently executed event.\n";
            continue;
        }
        Object->clearAllEvents();
        wasAnyEventUpdated = true;
    }
}
bool findObjectForFunction(AncestorObject *& ModuleObject, vector<LayerClass> &Layers, const string & objectID, const string & layerID){
    if(ModuleObject == nullptr || ModuleObject->getID() != objectID){
        for(LayerClass & Layer : Layers){
            if(Layer.getID() != layerID){
                continue;
            }
            for(AncestorObject & Object : Layer.Objects){
                if(Object.getID() != objectID){
                    continue;
                }
                ModuleObject = &Object;
                return true;
            }
            return false;
        }
    }
    if(ModuleObject == nullptr){
        return false;
    }
    return true;
}
void ProcessClass::executeFunctionForCameras(OperaClass & Operation, vector <VariableModule> & Variables,
    vector<Camera2D*> CamerasFromContext, Camera2D *& SelectedCamera, string & focusedProcessID
){
    unsigned cameraIndex = 0;
    for(Camera2D * Camera : CamerasFromContext){
        if(Operation.Location.attribute == "set_id" && Variables.size() > 0){
            Camera->setID(Variables[0].getStringUnsafe(), camerasIDs);
        }
        else if(Operation.Location.attribute == "draw_one_frame" && Variables.size() > 0){
            Camera->drawOneFrame = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "clear_bitmap" && Variables.size() > 0){
            Camera->clearBitmap = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_active" && Variables.size() > 0){
            Camera->setIsActive(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "activate"){
            Camera->activate();
        }
        else if(Operation.Location.attribute == "deactivate"){
            Camera->deactivate();
        }
        else if(Operation.Location.attribute == "toggle"){
            Camera->toggleIsActive();
        }
        else if(Operation.Location.attribute == "set_position" && Variables.size() > 1){
            Camera->setPos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
            updateTreeOfCamerasFromSelectedRoot(Camera);
            for(Camera2D & Pin : Cameras){
                if(Pin.getID() == Camera->pinnedCameraID){
                    updateTreeOfCamerasFromSelectedRoot(&Pin);
                    break;
                }
            }
        }
        else if(Operation.Location.attribute == "set_relative_position" && Variables.size() > 1){
            Camera->setRelativePos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
            updateTreeOfCamerasFromSelectedRoot(Camera);
            for(Camera2D & Pin : Cameras){
                if(Pin.getID() == Camera->pinnedCameraID){
                    updateTreeOfCamerasFromSelectedRoot(&Pin);
                    break;
                }
            }
        }
        else if(Operation.Location.attribute == "resize" && Variables.size() > 1){
            Camera->setSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "set_min_size" && Variables.size() > 1){
            Camera->setMinSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "set_zoom" && Variables.size() > 0){
            Camera->zoom = Variables[0].getDoubleUnsafe();
        }
        else if(Operation.Location.attribute == "set_zoom_increase" && Variables.size() > 0){
            Camera->zoomIncrease = Variables[0].getDoubleUnsafe();
        }
        else if(Operation.Location.attribute == "set_min_zoom" && Variables.size() > 0){
            Camera->minZoom = Variables[0].getDoubleUnsafe();
        }
        else if(Operation.Location.attribute == "set_max_zoom" && Variables.size() > 0){
            Camera->maxZoom = Variables[0].getDoubleUnsafe();
        }
        else if(Operation.Location.attribute == "set_speed" && Variables.size() > 0){
            Camera->setSpeed(Variables[0].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "bind_keys" && Variables.size() > 6){
            Camera->setKeyBinds(Variables[0].getIntUnsafe(), Variables[1].getIntUnsafe(), Variables[2].getIntUnsafe(),
                Variables[3].getIntUnsafe(), Variables[4].getIntUnsafe(), Variables[5].getIntUnsafe(), Variables[6].getIntUnsafe());
        }
        else if(Operation.Location.attribute == "set_key_zoom_in" && Variables.size() > 0){
            Camera->zoomInKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "set_key_zoom_out" && Variables.size() > 0){
            Camera->zoomOutKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "set_key_zoom_reset" && Variables.size() > 0){
            Camera->zoomResetKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "set_key_up" && Variables.size() > 0){
            Camera->upKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "set_key_right" && Variables.size() > 0){
            Camera->rightKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "set_key_down" && Variables.size() > 0){
            Camera->downKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "set_key_left" && Variables.size() > 0){
            Camera->leftKey = Variables[0].getIntUnsafe();
        }
        else if(Operation.Location.attribute == "pin_to_camera" && Variables.size() > 0){
            Camera->pinnedCameraID = Variables[0].getStringUnsafe();
        }
        else if(Operation.Location.attribute == "follow_layer" && Variables.size() > 0){
            Camera->followedLayerID = Variables[0].getStringUnsafe();
        }
        else if(Operation.Location.attribute == "follow_object" && Variables.size() > 0){
            Camera->followedObjectID = Variables[0].getStringUnsafe();
        }
        else if(Operation.Location.attribute == "follow_image" && Variables.size() > 0){
            Camera->followedImageID = Variables[0].getStringUnsafe();
        }
        else if(Operation.Location.attribute == "set_is_pinned_to_camera" && Variables.size() > 0){
            Camera->isPinnedToCamera = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_is_forcefully_pinned" && Variables.size() > 0){
            Camera->setIsForcefullyPinned(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "activate_pin"){
            Camera->activatePin();
        }
        else if(Operation.Location.attribute == "deactivate_pin"){
            Camera->deactivatePin();
        }
        else if(Operation.Location.attribute == "toggle_pin"){
            Camera->togglePin();
        }
        else if(Operation.Location.attribute == "set_is_following_object" && Variables.size() > 0){
            Camera->isFollowingObject = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_move_with_keyboard" && Variables.size() > 0){
            Camera->isUsingKeyboardToMove = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_zoom_with_keyboard" && Variables.size() > 0){
            Camera->isUsingKeyboardToZoom = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_is_vision_affected_by_mouse" && Variables.size() > 0){
            Camera->isVisionAffectedByMouse = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_zoom_with_mouse" && Variables.size() > 0){
            Camera->canZoomWithMouse = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_be_modified_by_mouse" && Variables.size() > 0){
            Camera->canBeModifiedByMouse = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_move_objects" && Variables.size() > 0){
            Camera->setCanMoveObjects(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "set_can_edit_text" && Variables.size() > 0){
            Camera->setCanEditText(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "add_visible_layer" && Variables.size() > 0){
            Camera->addVisibleLayer(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "add_accessible_layer" && Variables.size() > 0){
            Camera->addAccessibleLayer(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "remove_visible_layer" && Variables.size() > 0){
            Camera->removeVisibleLayer(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "remove_accessible_layer" && Variables.size() > 0){
            Camera->removeAccessibleLayer(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "clear_visible_layers"){
            Camera->clearVisibleLayers();
        }
        else if(Operation.Location.attribute == "clear_accessible_layers"){
            Camera->clearAccessibleLayers();
        }
        else if(Operation.Location.attribute == "set_tint" && Variables.size() > 3){
            Camera->setTint(Variables[0].getIntUnsafe(), Variables[1].getIntUnsafe(), Variables[2].getIntUnsafe(), Variables[3].getIntUnsafe());
        }
        else if(Operation.Location.attribute == "set_drawing_borders" && Variables.size() > 0){
            Camera->allowsDrawingBorders = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "allow_drawing_object_borders"){
            Camera->canDrawObjectBorders = true;
        }
        else if(Operation.Location.attribute == "forbid_drawing_object_borders"){
            Camera->canDrawObjectBorders = false;
        }
        else if(Operation.Location.attribute == "set_can_draw_object_borders" && Variables.size() > 0){
            Camera->canDrawObjectBorders = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "minimize"){
            Camera->minimize();
            for(;cameraIndex < Cameras.size(); cameraIndex++){
                if(Cameras[cameraIndex].getID() == Camera->getID()){
                    break;
                }
            }
            for(unsigned i = 0; i < camerasOrder.size(); i++){
                if(camerasOrder[i] == cameraIndex){
                    auto it = camerasOrder.rbegin() + camerasOrder.size() - 1 - i;
                    std::rotate(it, it + 1, camerasOrder.rend());
                    break;
                }
            }
        }
        else if(Operation.Location.attribute == "bring_forward"){
            for(;cameraIndex < Cameras.size(); cameraIndex++){
                if(Cameras[cameraIndex].getID() == Camera->getID()){
                    break;
                }
            }
            for(unsigned xd = 0; xd < camerasOrder.size(); xd++){
                if(camerasOrder[xd] == cameraIndex){
                    bringCameraForward(xd, Camera);
                    break;
                }
            }
        }
        else if(Operation.Location.attribute == "set_can_be_grabbed" && Variables.size() > 0){
            Camera->canBeGrabbed = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_grabbing_area_position" && Variables.size() > 1){
            Camera->setGrabbingAreaPos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "set_grabbing_area_size" && Variables.size() > 1){
            Camera->setGrabbingAreaSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "set_can_draw_on_camera" && Variables.size() > 0){
            Camera->canDrawOnCamera = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_clear_bitmap" && Variables.size() > 0){
            Camera->canClearBitmap = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_keep_inside_screen" && Variables.size() > 0){
            Camera->keepInsideScreen = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_mouse_resize" && Variables.size() > 0){
            Camera->canMouseResizeNow = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "focus"){
            focusCamera(Cameras, SelectedCamera, getID(), focusedProcessID, Camera);
        }
        else if(Operation.Location.attribute == "unfocus"){
            unfocusCameras(Cameras, SelectedCamera, getID(), focusedProcessID);
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": function " << Operation.Location.attribute << "<" << Variables.size() << "> does not exist.\n";
        }
    }

    if(Operation.Location.attribute == "pin_to_camera"){
        updateWholeForestOfCameras();
    }
}
void ProcessClass::executeFunctionForLayers(OperaClass & Operation, vector <VariableModule> & Variables, vector<LayerClass*> & Layers){
    for(LayerClass * Layer : Layers){
        if(Operation.Location.attribute == "set_id" && Variables.size() > 0){
            Layer->setID(Variables[0].getStringUnsafe(), layersIDs);
        }
        else if(Operation.Location.attribute == "set_is_active" && Variables.size() > 0){
            Layer->setIsActive(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "activate"){
            Layer->setIsActive(true);
        }
        else if(Operation.Location.attribute == "deactivate"){
            Layer->setIsActive(false);
        }
        else if(Operation.Location.attribute == "toggle"){
            Layer->setIsActive(!Layer->getIsActive());
        }
        else if(Operation.Location.attribute == "add_group" && Variables.size() > 0){
            Layer->addGroup(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "remove_group" && Variables.size() > 0){
            Layer->removeGroup(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "set_position" && Variables.size() > 1){
            Layer->pos.x = Variables[0].getDoubleUnsafe();
            Layer->pos.y = Variables[1].getDoubleUnsafe();
        }
        else if(Operation.Location.attribute == "set_size" && Variables.size() > 1){
            Layer->size.x = Variables[0].getDoubleUnsafe();
            Layer->size.y = Variables[1].getDoubleUnsafe();
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": function " << Operation.Location.attribute << "<" << Variables.size() << "> does not exist.\n";
        }
    }
}
void ProcessClass::executeFunctionForObjects(OperaClass & Operation, vector <VariableModule> & Variables, vector<AncestorObject*> & Objects){
    if(Operation.Location.attribute == "set_id" && Variables.size() > 0){
        LayerClass * ObjectsLayer = nullptr;
        for(AncestorObject * Object : Objects){
            if(ObjectsLayer == nullptr || ObjectsLayer->getID() != Object->getLayerID()){
                for(LayerClass & Layer : Layers){
                    if(Layer.getID() == Object->getLayerID()){
                        ObjectsLayer = &Layer;
                        break;
                    }    
                }
            }
            if(ObjectsLayer == nullptr){
                continue;
            }
            Object->setID(Variables[0].getStringUnsafe(), ObjectsLayer->objectsIDs);
            Object->propagateObjectID();
        }
        return;
    }
    for(AncestorObject * Object : Objects){
        if(Operation.Location.attribute == "set_active" && Variables.size() > 0){
            Object->setIsActive(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "activate"){
            Object->activate();
        }
        else if(Operation.Location.attribute == "deactivate"){
            Object->deactivate();
        }
        else if(Operation.Location.attribute == "toggle"){
            Object->toggleIsActive();
        }
        else if(Operation.Location.attribute == "set_position" && Variables.size() > 1){
            Object->setPos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "set_size" && Variables.size() > 1){
            Object->setSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
        }
        else if(Operation.Location.attribute == "add_group" && Variables.size() > 0){
            Object->addGroup(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "remove_group" && Variables.size() > 0){
            Object->removeGroup(Variables[0].getStringUnsafe());
        }
        else if(Operation.Location.attribute == "remove_group"){
            Object->clearGroups();
        }
        else if(Operation.Location.attribute == "disable_scrolling"){
            Object->setIsScrollable(false);
        }
        else if(Operation.Location.attribute == "enable_scrolling"){
            Object->setIsScrollable(true);
        }
        else if(Operation.Location.attribute == "set_is_scrollable" && Variables.size() > 0){
            Object->setIsScrollable(Variables[0].getBoolUnsafe());
        }
        else if(Operation.Location.attribute == "enable_selection_border"){
            Object->canDrawSelectionBorder = true;
        }
        else if(Operation.Location.attribute == "disable_selection_border"){
            Object->canDrawSelectionBorder = false;
        }
        else if(Operation.Location.attribute == "set_can_draw_selection_border" && Variables.size() > 0){
            Object->canDrawSelectionBorder = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_be_moved_with_mouse" && Variables.size() > 0){
            Object->canBeMovedWithMouse = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "update_scrollbars"){
            for(ScrollbarModule & Scrollbar : Object->ScrollbarContainer){
                if(!Scrollbar.getIsActive()){
                    continue;
                }
                Scrollbar.countScrollShift();
                for(Camera2D & Camera : Cameras){
                    if(isStringInVector(Scrollbar.cameraIDs, Camera.getID())){
                        Camera.setVisionShift(-Scrollbar.scrollShift);
                    }
                }
            }
        }
        else{
            bool temp = false;
            if(Variables.size() > 0){
                temp = Variables[0].getBoolUnsafe();
            }
            Object->control(Operation.Location.attribute, temp, Variables.size());
        }
    }
}
void ProcessClass::executeFunction(OperaClass Operation, vector<ContextClass> & EventContext,
    vector<EveModule>::iterator & Event, EngineClass & Engine
){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Function requires at least one context.\n";
        return;
    }

    vector <VariableModule> & Variables = Operation.Literals;
    ContextClass * Context = nullptr;
    
    if(/*Operation.Literals.size() > 0 ||*/ Operation.dynamicIDs.size() == 1){
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
    }
    else if(Operation.dynamicIDs.size() > 1){
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            return;
        }

        ContextClass * ContextValues = nullptr;

        for(unsigned i = 1; i < Operation.dynamicIDs.size(); i++){
            ContextValues = getContextByID(EventContext, Operation.dynamicIDs[i], true, EventIds);
            if(ContextValues == nullptr){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no context with id: \'" << Operation.dynamicIDs[i] << "\'.\n";
                continue;
            }
            if(ContextValues->type == "value"){
                Variables.insert(Variables.end(), ContextValues->Variables.begin(), ContextValues->Variables.end());
            }
            else if(ContextValues->type == "pointer"){
                for(const BasePointersStruct & Pointer : ContextValues->BasePointers){
                    Variables.push_back(VariableModule());
                    Variables.back().setValueFromPointer(Pointer);
                }
            }
            else if(ContextValues->type == "variable"){
                for(const VariableModule * Variable : ContextValues->Modules.Variables){
                    Variables.push_back(*Variable);
                }
            }
            else{
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Second context must be of type: pointer or value.\n";
            }
        }
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << "." << Operation.Location.attribute << "()";
        if(Variables.size() > 0){
            cout << "[";
            for(const VariableModule & Var : Variables){
                cout << Var.getStringUnsafe() << ", ";
            }
            cout << "]<" << Variables.size() << ">";
        }
        cout << "\n";
    }

    AncestorObject * ModulesObject = nullptr;
    vector<string> emptyString;

    
    if(Context->type == "camera"){
        executeFunctionForCameras(Operation, Variables, Context->Cameras, SelectedCamera, Engine.focusedProcessID);
    }
    else if(Context->type == "layer"){
        executeFunctionForLayers(Operation, Variables, Context->Layers);
    }
    else if(Context->type == "object"){
        executeFunctionForObjects(Operation, Variables, Context->Objects);
    }
    else if(Context->type == "text"){
        if(Operation.Location.attribute == "set_id"){
            for(TextModule * Text : Context->Modules.Texts){
                if(!findObjectForFunction(ModulesObject, Layers, Text->getObjectID(), Text->getLayerID())){
                    continue;
                }
                Event->controlText(Text, Operation.Location.attribute, Variables, ModulesObject->textContainerIDs, Engine.FontContainer);
            }
            return;
        }
        for(TextModule * Text : Context->Modules.Texts){
            Event->controlText(Text, Operation.Location.attribute, Variables, emptyString, Engine.FontContainer);
        }
    }
    else if(Context->type == "editable_text"){
        if(Operation.Location.attribute == "set_id"){
            for(EditableTextModule * EditableText : Context->Modules.EditableTexts){
                if(!findObjectForFunction(ModulesObject, Layers, EditableText->getObjectID(), EditableText->getLayerID())){
                    continue;
                }
                Event->controlEditableText(EditableText, Operation.Location.attribute, Variables, ModulesObject->textContainerIDs, Engine.FontContainer);
            }
            return;
        }
        for(EditableTextModule * EditableText : Context->Modules.EditableTexts){
            Event->controlEditableText(EditableText, Operation.Location.attribute, Variables, emptyString, Engine.FontContainer);
        }
    }
    else if(Context->type == "super_text"){
        if(Operation.Location.attribute == "set_id"){
            for(SuperTextModule * SuperText : Context->Modules.SuperTexts){
                if(!findObjectForFunction(ModulesObject, Layers, SuperText->getObjectID(), SuperText->getLayerID())){
                    continue;
                }
                Event->controlSuperText(SuperText, Operation.Location.attribute, Variables, ModulesObject->superTextContainerIDs, Engine.FontContainer);
            }
            return;
        }
        for(SuperTextModule * SuperText : Context->Modules.SuperTexts){
            Event->controlSuperText(SuperText, Operation.Location.attribute, Variables, emptyString, Engine.FontContainer);
        }
    }
    else if(Context->type == "super_editable_text"){
        if(Operation.Location.attribute == "set_id"){
            for(SuperEditableTextModule *& SuperEditableText : Context->Modules.SuperEditableTexts){
                if(!findObjectForFunction(ModulesObject, Layers, SuperEditableText->getObjectID(), SuperEditableText->getLayerID())){
                    continue;
                }
                Event->controlSuperEditableText(SuperEditableText, Operation.Location.attribute, Variables,
                    ModulesObject->superEditableTextContainerIDs, Engine.FontContainer, ActiveEditableText
                );
            }
            return;
        }
        for(SuperEditableTextModule *& SuperEditableText : Context->Modules.SuperEditableTexts){
            Event->controlSuperEditableText(SuperEditableText, Operation.Location.attribute, Variables, emptyString, Engine.FontContainer, ActiveEditableText);
        }
    }
    else if(Context->type == "image"){
        if(isStringInGroup(Operation.Location.attribute, 6, "set_id", "set_position", "set_size", "set_scale", "resize", "connect_bitmap")){
            for(ImageModule * Image : Context->Modules.Images){
                if(!findObjectForFunction(ModulesObject, Layers, Image->getObjectID(), Image->getLayerID())){
                    continue;
                }
                Event->controlImage(Image, Operation.Location.attribute, Variables, ModulesObject->imageContainerIDs, Engine.BitmapContainer, EXE_PATH + workingDirectory);
                if(Operation.Location.attribute != "set_id"){
                    ModulesObject->refreshCoordinates();
                }
            }
            return;
        }
        for(ImageModule * Image : Context->Modules.Images){
            Event->controlImage(Image, Operation.Location.attribute, Variables, emptyString, Engine.BitmapContainer, EXE_PATH + workingDirectory);
        }
    }
    else if(Context->type == "movement"){
        if(Operation.Location.attribute == "set_id"){
            for(MovementModule * Movement : Context->Modules.Movements){
                if(!findObjectForFunction(ModulesObject, Layers, Movement->getObjectID(), Movement->getLayerID())){
                    continue;
                }
                Event->controlMovement(Movement, Operation.Location.attribute, Variables, ModulesObject->movementContainerIDs);
            }
            return;
        }
        for(MovementModule * Movement : Context->Modules.Movements){
            Event->controlMovement(Movement, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else if(Context->type == "collision"){
        if(Operation.Location.attribute == "set_id"){
            for(CollisionModule * Collision : Context->Modules.Collisions){
                if(!findObjectForFunction(ModulesObject, Layers, Collision->getObjectID(), Collision->getLayerID())){
                    continue;
                }
                Event->controlCollision(Collision, Operation.Location.attribute, Variables, ModulesObject->collisionContainerIDs);
            }
            return;
        }
        for(CollisionModule * Collision : Context->Modules.Collisions){
            Event->controlCollision(Collision, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else if(Context->type == "particles"){
        if(Operation.Location.attribute == "set_id"){
            for(ParticleEffectModule * Particles : Context->Modules.Particles){
                if(!findObjectForFunction(ModulesObject, Layers, Particles->getObjectID(), Particles->getLayerID())){
                    continue;
                }
                Event->controlParticles(Particles, Operation.Location.attribute, Variables, ModulesObject->particlesContainerIDs);
            }
            return;
        }
        for(ParticleEffectModule * Particles : Context->Modules.Particles){
            Event->controlParticles(Particles, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else if(Context->type == "event"){
        /*
        if(Operation.Location.attribute == "set_id"){
            for(EveModule * Event : Context->Modules.Events){
                if(!findObjectForFunction(ModuleObject, Layers, EditableText->getObjectID(), EditableText->getLayerID())){
                    continue;
                }
                Event->controlEvent(Event, Operation.Location.attribute, Variables);
            }
        }
        return;
        for(EveModule * Event : Context->Modules.Events){
            Event->controlEvent(Event, Operation.Location.attribute, Variables, emptyString);
        }*/
    }
    else if(Context->type == "variable"){
        if(Operation.Location.attribute == "set_id"){
            for(VariableModule * Variable : Context->Modules.Variables){
                if(!findObjectForFunction(ModulesObject, Layers, Variable->getObjectID(), Variable->getLayerID())){
                    continue;
                }
                Event->controlVariables(Variable, Operation.Location.attribute, Variables, ModulesObject->variablesContainerIDs);
            }
            return;
        }
        for(VariableModule * Variable : Context->Modules.Variables){
            Event->controlVariables(Variable, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else if(Context->type == "scrollbar"){
        if(Operation.Location.attribute == "set_id"){
            for(ScrollbarModule * Scrollbar : Context->Modules.Scrollbars){
                if(!findObjectForFunction(ModulesObject, Layers, Scrollbar->getObjectID(), Scrollbar->getLayerID())){
                    continue;
                }
                Event->controlScrollbar(Scrollbar, Operation.Location.attribute, Variables, ModulesObject->scrollbarContainerIDs);
            }
            return;
        }
        for(ScrollbarModule * Scrollbar : Context->Modules.Scrollbars){
            Event->controlScrollbar(Scrollbar, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else if(Context->type == "primitives"){
        if(Operation.Location.attribute == "set_id"){
            for(PrimitivesModule * Primitives : Context->Modules.Primitives){
                if(!findObjectForFunction(ModulesObject, Layers, Primitives->getObjectID(), Primitives->getLayerID())){
                    continue;
                }
                Event->controlPrimitives(Primitives, Operation.Location.attribute, Variables, ModulesObject->primitivesContainerIDs);
            }
            return;
        }
        for(PrimitivesModule * Primitives : Context->Modules.Primitives){
            Event->controlPrimitives(Primitives, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else if(Context->type == "vector"){
        if(Operation.Location.attribute == "set_id"){
            for(VectorModule * Vector : Context->Modules.Vectors){
                if(!findObjectForFunction(ModulesObject, Layers, Vector->getObjectID(), Vector->getLayerID())){
                    continue;
                }
                Event->controlVector(Vector, Operation.Location.attribute, Variables, ModulesObject->vectorContainerIDs);
            }
            return;
        }
        for(VectorModule * Vector : Context->Modules.Vectors){
            Event->controlVector(Vector, Operation.Location.attribute, Variables, emptyString);
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": type \'" << Context->type << "\' does not exist.\n";
    }
}
void ProcessClass::changeEngineVariables(OperaClass & Operation, EngineClass & Engine){
    if(Operation.Literals.size() < 2){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires at least two literals.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires first literal being of a string type.\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << " ";
        for(unsigned i = 1; i < Operation.Literals.size(); i++){
            cout << Operation.Literals[i].getStringUnsafe() << " ";
        }
        cout << "\n";
    }
    if(Operation.Literals[0].getString() == "window_title"){
        if(Engine.windowTitle == Operation.Literals[1].getStringUnsafe()){
            return;
        }
        Engine.windowTitle = Operation.Literals[1].getStringUnsafe();
        al_set_window_title(Engine.display, Engine.windowTitle.c_str());
    }
    else if(Operation.Literals[0].getString() == "window_size"){
        if(Operation.Literals.size() < 3){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        Engine.displaySize.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
        if(!al_resize_display(Engine.display, Engine.displaySize.x, Engine.displaySize.y)){
            //cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": al_resize_display() failed to resize the display.\n";
            /*#if __WIN32__
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": al_resize_display() failed to resize the display.\n";
            #else
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": al_resize_display() stops to work on Linux systems if the display resolution is set too high.\n";
            #endif*/
        }
    }
    else if(Operation.Literals[0].getString() == "fullscreen"){
        if(Operation.Literals[1].getBool() == Engine.fullscreen){
            return;
        }
        Engine.fullscreen = Operation.Literals[1].getBool();
        al_set_display_flag(Engine.display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(Engine.display) & ALLEGRO_FULLSCREEN_WINDOW));
        #if __WIN32__
        al_set_display_flag(Engine.display, ALLEGRO_MAXIMIZED, !(al_get_display_flags(Engine.display) & ALLEGRO_MAXIMIZED));
        #endif
        //al_set_display_flag(window, ALLEGRO_NOFRAME, !(al_get_display_flags(window) & ALLEGRO_NOFRAME));   
    }
    else if(Operation.Literals[0].getString() == "pixel_art"){
        if(Operation.Literals[1].getBool() == Engine.isPixelArt){
            return;
        }
        Engine.isPixelArt = Operation.Literals[1].getBool();
        if(!Engine.isPixelArt){
            al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
        }
        else{
            al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
        }   
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute \'" << Operation.Literals[0].getString() << "\' does not exist.\n";
    }
}
void ProcessClass::changeProcessVariables(OperaClass & Operation, vector <string> & processIDs){
    if(Operation.Literals.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires at least one literal.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires first literal being of a string type.\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << " ";
        for(unsigned i = 1; i < Operation.Literals.size(); i++){
            cout << Operation.Literals[i].getStringUnsafe() << " ";
        }
        cout << "\n";
    }
    
    if(Operation.Literals[0].getString() == "clear_layers"){
        wasDeleteExecuted = true;
        for(LayerClass & Layer : Layers){
            Layer.deleteLater();
        }
        return;
    }
    else if(Operation.Literals[0].getString() == "clear_cameras"){
        wasDeleteExecuted = true;
        for(Camera2D & Camera : Cameras){
            Camera.deleteLater();
        }
        return;
    }
    if(Operation.Literals.size() == 1){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires at least two literals.\n";
        return;
    }
    if(Operation.Literals[0].getString() == "id"){
        setID(Operation.Literals[1].getString(), processIDs); 
    }
    else if(Operation.Literals[0].getString() == "is_active"){
        isActive = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "can_interact_with_user"){
        canUserInteract = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "is_rendering"){
        isRendering = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "draw_camera_borders"){
        drawCameraBorders = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "draw_text_borders"){
        drawTextFieldBorders = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "draw_hitboxes"){
        drawHitboxes = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "ignore_distant"){
        ignoreDistantObjects = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "draw_only_visible"){
        drawOnlyVisibleObjects = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "bitmap_layers_number"){
        totalNumberOfBitmapLayers = Operation.Literals[1].getInt(); 
    }
    else if(Operation.Literals[0].getString() == "print_logical_evaluations"){
        printOutLogicalEvaluations = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "print_instructions"){
        printOutInstructions = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "auto_print_stack"){
        printOutStackAutomatically = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "reservation_multiplier"){
        reservationMultiplier = Operation.Literals[1].getDouble(); 
    }
    else if(Operation.Literals[0].getString() == "window_pos"){
        if(Operation.Literals.size() < 3){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        windowPos.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
    }
    else if(Operation.Literals[0].getString() == "window_size"){
        if(Operation.Literals.size() < 3){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        windowSize.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
        resizeWindow(windowSize);
    }
    else if(Operation.Literals[0].getString() == "min_window_size"){
        if(Operation.Literals.size() < 3){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        minWindowSize.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
    }
    else if(Operation.Literals[0].getString() == "window_tint"){
        if(Operation.Literals.size() < 5){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()
            || !Operation.Literals[3].isNumeric() || !Operation.Literals[4].isNumeric()
        ){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        windowTint[0] = Operation.Literals[1].getDouble();
        windowTint[1] = Operation.Literals[2].getDouble();
        windowTint[2] = Operation.Literals[3].getDouble();
        windowTint[3] = Operation.Literals[4].getDouble();
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute \'" << Operation.Literals[0].getString() << "\' does not exist.\n";
    }
}
void ProcessClass::loadBitmap(OperaClass & Operation, vector<SingleBitmap> & BitmapContainer){
    if(Operation.Literals.size() < 2 || Operation.Literals[0].getType() != 's' || Operation.Literals[1].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires two strings.\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << " " << Operation.Literals[1].getString() << "\n";
    }
    bool createLightBitmap = false, ignoreWarnings = false;
    if(Operation.Literals.size() > 2){
        createLightBitmap = Operation.Literals[2].getBool();
    }
    if(Operation.Literals.size() > 3){
        ignoreWarnings = Operation.Literals[3].getBool();
    }
    for(const SingleBitmap & Bitmap : BitmapContainer){
        if(Bitmap.ID == Operation.Literals[1].getString()){
            if(!ignoreWarnings){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\' instruction: "
                    << "Loading failed. Bitmap with the id \'" << Bitmap.ID << "\' already exists.\n";
            }
            return;
        }
    }

    BitmapContainer.push_back(SingleBitmap());
    BitmapContainer.back().loadBitmap(Operation.Literals[1].getString(), Operation.Literals[0].getString(), EXE_PATH + workingDirectory, createLightBitmap);
}
void ProcessClass::createDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires one string (a path to the new directory).\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + Operation.Literals[0].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::create_directory(EXE_PATH + workingDirectory + Operation.Literals[0].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": No such directory \'" << EXE_PATH + workingDirectory + Operation.Literals[0].getString() << "\'.\n";
    }
}
void ProcessClass::removeFileOrDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires one string (a path to the new directory).\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + Operation.Literals[0].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::remove(EXE_PATH + workingDirectory + Operation.Literals[0].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": " << ex.what() << "\n";
    }
}
void ProcessClass::removeRecursivelyFileOrDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires one string (a path to the new directory).\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + Operation.Literals[0].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::remove_all(EXE_PATH + workingDirectory + Operation.Literals[0].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": " << ex.what() << "\n";
    }
}
void ProcessClass::renameFileOrDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 2 || Operation.Literals[0].getType() != 's' || Operation.Literals[1].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires two strings (two paths to files or directories).\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << " " << Operation.Literals[1].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + Operation.Literals[0].getString() << "\'.\n"; 
    }
    if(Operation.Literals[1].getString() == "" || Operation.Literals[1].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + Operation.Literals[1].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::rename(EXE_PATH + workingDirectory + Operation.Literals[0].getString(), EXE_PATH + workingDirectory + Operation.Literals[1].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": " << ex.what() << "\n";
    }
}
template <class T>
string getStringOfIDs(const vector<T*> Instances, string delimeter){
    string buffer = "";
    for(const T * Instance : Instances){
        buffer += Instance->getID() + delimeter;
    }
    return buffer;
}
string catchQuotes(string input){
    string output;
    for(size_t i = 0; i < input.size(); i++){
        if(input[i] == '\\' && i + 1 != input.size() && input[i+1] == '\"'){
            output += '\"';
            i++;
            continue;
        }
        output += input[i];
    }
    return output;
}
void ProcessClass::executePrint(OperaClass & Operation, vector<ContextClass> & EventContext){
    string delimeter = "";

    if(Operation.Literals.size() > 0){
        delimeter = Operation.Literals[0].getStringUnsafe();
    }

    if(printOutInstructions){
        cout << "print ";
        if(delimeter != ""){
            cout << "delimeter=\'" << delimeter << "\' ";
        }
        if(Operation.Literals.size() > 1){
            cout << "literals_c=\'" << Operation.Literals.size()-1 << "\' ";
        }
        if(Operation.dynamicIDs.size() > 0){
            cout << "literals_c=\'" << Operation.dynamicIDs.size() << "\' ";
        }
        cout << "\n";
    }

    string buffer = "";

    for(auto mIter = std::next(Operation.Literals.begin()); mIter != Operation.Literals.end(); ++mIter){
        buffer += catchQuotes(mIter->getStringUnsafe()) + delimeter;
    }

    ContextClass * ContextValues = nullptr;

    for(string contextID : Operation.dynamicIDs){
        ContextValues = getContextByID(EventContext, contextID, true, EventIds);
        if(ContextValues == nullptr){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no context with id: \'" << contextID << "\'.\n";
            continue;
        }
        if(ContextValues->type == "value"){
            for(const VariableModule & Variable : ContextValues->Variables){
                buffer += catchQuotes(Variable.getStringUnsafe()) + delimeter;
            }
        }
        else if(ContextValues->type == "pointer"){
            for(const BasePointersStruct & Pointer : ContextValues->BasePointers){
                buffer += catchQuotes(Pointer.getString()) + delimeter;
            }
        }
        else if(ContextValues->type == "variable"){
            for(const VariableModule * Variable : ContextValues->Modules.Variables){
                buffer += catchQuotes(Variable->getStringUnsafe()) + delimeter;
            }
        }
        else if(ContextValues->type == "vector"){
            for(const VectorModule * Vector : ContextValues->Modules.Vectors){
                vector<string> stringsFromVector = Vector->getAllValuesAsStringVector();
                for(string text : stringsFromVector){
                    buffer += catchQuotes(text) + delimeter;
                }
            }
        }
        else if(ContextValues->type == "camera"){
            buffer += getStringOfIDs(ContextValues->Cameras, delimeter);
        }
        else if(ContextValues->type == "layer"){
            buffer += getStringOfIDs(ContextValues->Layers, delimeter);
        }
        else if(ContextValues->type == "object"){
            buffer += getStringOfIDs(ContextValues->Objects, delimeter);
        }
        else if(ContextValues->type == "text"){
            buffer += getStringOfIDs(ContextValues->Modules.Texts, delimeter);
        }
        else if(ContextValues->type == "editable_text"){
            buffer += getStringOfIDs(ContextValues->Modules.EditableTexts, delimeter);
        }
        else if(ContextValues->type == "super_text"){
            buffer += getStringOfIDs(ContextValues->Modules.SuperTexts, delimeter);
        }
        else if(ContextValues->type == "super_editable_text"){
            buffer += getStringOfIDs(ContextValues->Modules.SuperEditableTexts, delimeter);
        }
        else if(ContextValues->type == "image"){
            buffer += getStringOfIDs(ContextValues->Modules.Images, delimeter);
        }
        else if(ContextValues->type == "movement"){
            buffer += getStringOfIDs(ContextValues->Modules.Movements, delimeter);
        }
        else if(ContextValues->type == "collision"){
            buffer += getStringOfIDs(ContextValues->Modules.Collisions, delimeter);
        }
        else if(ContextValues->type == "movement"){
            buffer += getStringOfIDs(ContextValues->Modules.Movements, delimeter);
        }
        else if(ContextValues->type == "particles"){
            buffer += getStringOfIDs(ContextValues->Modules.Particles, delimeter);
        }
        else if(ContextValues->type == "event"){
            buffer += getStringOfIDs(ContextValues->Modules.Events, delimeter);
        }
        else if(ContextValues->type == "scrollbar"){
            buffer += getStringOfIDs(ContextValues->Modules.Scrollbars, delimeter);
        }
        else if(ContextValues->type == "primitive"){
            buffer += getStringOfIDs(ContextValues->Modules.Primitives, delimeter);
        }
        else{
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Invalid context type \'" << ContextValues->type << "\'.\n";
        }
    }

    if(buffer == ""){
        buffer += delimeter;
    }

    if(Operation.newContextID == ""){
        cout << buffer;
    }
    else{
        ContextClass NewContext;
        NewContext.type = "value";
        NewContext.Variables.push_back(VariableModule::newString(buffer));
        moveOrRename(EventContext, &NewContext, Operation.newContextID);
    }
}
void ProcessClass::loadFileAsString(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires at least 1 context of string type.\n";
    }
    ContextClass * PathContext = nullptr;
    if(!getOneContext(PathContext, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    string pathToTheFile = "";
    if(!PathContext->getStringOrAbort(pathToTheFile, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << pathToTheFile << " " << Operation.newContextID << "\n";
    }
    if(pathToTheFile == "" || pathToTheFile[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + pathToTheFile << "\'.\n"; 
    }

    string loadedText = "";

    std::ifstream File(EXE_PATH + workingDirectory + pathToTheFile);

	if(!File){
		std::cerr << "Error: In " << __FUNCTION__ << ": Cannot open file: " << EXE_PATH + workingDirectory + pathToTheFile << "\n";
        return;
    }
    for(string line; std::getline(File, line);){
        loadedText += line + "\n";
    }
    File.close();

    ContextClass NewContext;
    NewContext.type = "value";
    NewContext.Variables.push_back(VariableModule::newString(loadedText));
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::saveStringAsFile(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass * PathContext = nullptr, * TextContext = nullptr;
    if(!getPairOfContexts(PathContext, TextContext, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
        return;
    }

    string pathToTheFile = "";
    if(!PathContext->getStringOrAbort(pathToTheFile, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': getting path from context failed.\n";
        return;
    }
    if(pathToTheFile == "" || pathToTheFile == "~/" || pathToTheFile[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + workingDirectory + pathToTheFile << "\'.\n"; 
    }
    string textFromContext = "";
    if(!TextContext->getStringOrAbort(textFromContext, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': getting text from context failed.\n";
        return;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << " " << Operation.dynamicIDs[1] << "\n";
    }

    string finalPath = EXE_PATH + workingDirectory + pathToTheFile;

    if(pathToTheFile.substr(0, 2) == "~/"){
        finalPath = EXE_PATH + pathToTheFile.substr(2, pathToTheFile.size()-2);
    }

    std::ofstream File(finalPath);

	if(!File){
		std::cerr << "Error: In " << __FUNCTION__ << ": Cannot open file: '" << finalPath << "'.\n";
        return;
    }
	File << textFromContext;
    File.close();
}
void ProcessClass::listOutEntities(OperaClass & Operation, const vector<ProcessClass> & Processes, const EngineClass & Engine){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' requires at least one string.\n";
        return;
    }
    bool printDetails = false;
    if(Operation.Literals.size() >= 2){
        printDetails = Operation.Literals[1].getBoolUnsafe();
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << " " << printDetails << "\n";
    }
    if(Operation.Literals[0].getString() == "p" || Operation.Literals[0].getString() == "processes"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tLayers\tCameras\n";
            for(const ProcessClass & Process : Processes){
                cout << i << "\t" << Process.getID()
                    << "\t" << Process.countLayers()
                    << "\t" << Process.countCameras()
                    << "\n";
                i++;
            }
        }
        else{
            for(const ProcessClass & Process : Processes){
                cout << Process.getID() << " ";
            }
        }
    }
    else if(Operation.Literals[0].getString() == "l" || Operation.Literals[0].getString() == "layers"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tProcess\tActive\tObjects\n";
            for(const ProcessClass & Process : Processes){
                for(const LayerClass & Layer : Process.Layers){
                    cout << i
                    << "\t" << Layer.getID()
                    << "\t" << Process.getID()
                    << "\t" << Layer.getIsActive()
                    << "\t" << Layer.Objects.size()
                    << "\n";
                    i++;
                }
            }
        }
        else{
            for(const ProcessClass & Process : Processes){
                for(const LayerClass & Layer : Process.Layers){
                    cout << Layer.getID() << " ";
                }
            }
        }
    }
    else if(Operation.Literals[0].getString() == "c" || Operation.Literals[0].getString() == "cameras"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tProcess\tActive\tMinimized\tPinned\n";
            for(const ProcessClass & Process : Processes){
                for(const Camera2D & Camera : Process.Cameras){
                    cout << i
                    << "\t" << Camera.getID()
                    << "\t" << Process.getID()
                    << "\t" << Camera.getIsActive()
                    << "\t" << Camera.getIsMinimized()
                    << "\t" << Camera.pinnedCameraID
                    << "\n";
                    i++;
                }
            }
        }
        else{
            for(const ProcessClass & Process : Processes){
                for(const Camera2D & Camera : Process.Cameras){
                    cout << Camera.getID() << " ";
                }
            }
        }
    }
    else if(Operation.Literals[0].getString() == "b" || Operation.Literals[0].getString() == "bitmaps"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tPath\n";
            for(const SingleBitmap & Bitmap : Engine.BitmapContainer){
                cout << i << "\t" << Bitmap.ID
                    << "\t" << Bitmap.filePath << "\n";
                i++;
            }
        }
        else{
            for(const SingleBitmap & Bitmap : Engine.BitmapContainer){
                cout << Bitmap.ID << " ";
            }
        }
    }
    else if(Operation.Literals[0].getString() == "f" || Operation.Literals[0].getString() == "fonts"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tSize\n";
            for(const SingleFont & Font : Engine.FontContainer){
                cout << i << "\t" << Font.ID
                    << "\t" << Font.size << "\n";
                i++;
            }
        }
        else{
            for(const SingleFont & Font : Engine.FontContainer){
                cout << Font.ID << " ";
            }
        }
    }
}
void ProcessClass::createNewProcess(OperaClass & Operation, vector<ProcessClass> & Processes, vector<ContextClass> &EventContext,
    AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, EngineClass & Engine
){
    if(Operation.Literals.size() == 0 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": "
            << transInstrToStr(Operation.instruction) << " requires at least one string literal.\n";
        return;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction);
    }

    string layerID = "", objectID = "", pathToTheScript = "";

    if(Operation.Literals.size() > 1 && Operation.Literals[1].getType() == 's'){
        layerID = Operation.Literals[1].getString();
        if(printOutInstructions){
            cout << " " << layerID;
        }
    }
    if(Operation.Literals.size() > 2 && Operation.Literals[2].getType() == 's'){
        objectID = Operation.Literals[2].getString();
        if(printOutInstructions){
            cout << " " << objectID;
        }
    }
    if(Operation.Literals.size() > 3 && Operation.Literals[3].getType() == 's'){
        pathToTheScript = Operation.Literals[3].getString();
        if(printOutInstructions){
            cout << " " << pathToTheScript;
        }
    }

    if(printOutInstructions){
        cout << "\n";
    }
    
    if(Processes.size() + 1 <= Processes.capacity()){
        Processes.push_back(ProcessClass());
        Processes.back().create(Engine.EXE_PATH + workingDirectory, Engine.allowNotAscii, Engine.getDisplaySize(),
            pathToTheScript, Operation.Literals[0].getString(), layerID, objectID, Engine.processIDs
        );
    }
    else{
        
    }
}
void ProcessClass::createNewOwnerVariable(OperaClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
){
    if(Operation.Literals.size() < 2){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " requires two literals.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " requires the first literal to be of a string type.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString()
            << " " << Operation.Literals[1].getStringUnsafe() << " " << Operation.newContextID << "\n";
    }

    for(const VariableModule & Variable : Owner->VariablesContainer){
        if(Variable.getID() == Operation.newContextID){
            cout << "Error: In " << EventIds.describe() << ": In \'" << transInstrToStr(Operation.instruction) << "\' instruction: Cannot create a variable with id \'"
                << Operation.newContextID << "\', because a variable with the same id already exists.\n";
            return;
        }
    }

    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
    
    if(Operation.Literals[0].getString() == "bool" || Operation.Literals[0].getString() == "b"){
        if(Operation.Literals[1].getType() != 'b'){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newBool(
            Operation.Literals[1].getBool(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else if(Operation.Literals[0].getString() == "int" || Operation.Literals[0].getString() == "i"){
        if(Operation.Literals[1].getType() != 'i'){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newInt(
            Operation.Literals[1].getInt(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else if(Operation.Literals[0].getString() == "double" || Operation.Literals[0].getString() == "d"){
        if(Operation.Literals[1].getType() != 'd'){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newDouble(
            Operation.Literals[1].getDouble(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else if(Operation.Literals[0].getString() == "string" || Operation.Literals[0].getString() == "s"){
        if(Operation.Literals[1].getType() != 's'){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newString(
            Operation.Literals[1].getString(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction)
            << " instruction the first parameter has incorrect value: \'" << Operation.Literals[0].getString() << "\'.\n";
        return;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);

    ContextClass NewContext;

    if(Owner->VariablesContainer.size() > 0){
        NewContext.Modules.Variables.push_back(&Owner->VariablesContainer.back());
        addNewContext(EventContext, NewContext, "variable", Operation.newContextID);
        wereGlobalVariablesCreated = true;
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ProcessClass::createNewOwnerVector(OperaClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
){
    if(Operation.Literals.size() == 0){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " instruction requires at least one literal.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": " << transInstrToStr(Operation.instruction) << " instruction requires the first literal to be of a string type.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.Literals[0].getString() << " [";
        for(size_t i = 1; i < Operation.Literals.size(); i++){
            cout << Operation.Literals[i].getStringUnsafe() << " ";
        }
        cout << "]" << Operation.Literals[1].getStringUnsafe() << " " << Operation.newContextID << "\n";
    }

    for(const VectorModule & Vector : Owner->VectorContainer){
        if(Vector.getID() == Operation.newContextID){
            cout << "Error: In " << EventIds.describe() << ": In \'" << transInstrToStr(Operation.instruction) << "\' instruction: Cannot create a vector with id '"
                << Operation.newContextID << "', because a vector with the same id already exists.\n";
            return;
        }
    }

    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);
    
    Owner->VectorContainer.push_back(VectorModule(Operation.newContextID, &Owner->vectorContainerIDs, Owner->getLayerID(), Owner->getID()));

    if(Operation.Literals[0].getString() == "bool" || Operation.Literals[0].getString() == "b"){
        Owner->VectorContainer.back().setType('b');
        for(size_t i = 1; i < Operation.Literals.size(); i++){
            if(Operation.Literals[i].getType() != 'b'){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction: Literal ("
                    << Operation.Literals[i].getType() << ") is not of a " << Operation.Literals[0].getString() << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushBool(Operation.Literals[i].getBool());
        }
    }
    else if(Operation.Literals[0].getString() == "int" || Operation.Literals[0].getString() == "i"){
        Owner->VectorContainer.back().setType('i');
        for(size_t i = 1; i < Operation.Literals.size(); i++){
            if(Operation.Literals[i].getType() != 'i'){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction: Literal ("
                    << Operation.Literals[i].getType() << ") is not of a " << Operation.Literals[0].getString() << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushInt(Operation.Literals[i].getInt());
        }
    }
    else if(Operation.Literals[0].getString() == "double" || Operation.Literals[0].getString() == "d"){
        Owner->VectorContainer.back().setType('d');
        for(size_t i = 1; i < Operation.Literals.size(); i++){
            if(Operation.Literals[i].getType() != 'd'){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction: Literal ("
                    << Operation.Literals[i].getType() << ") is not of a " << Operation.Literals[0].getString() << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushDouble(Operation.Literals[i].getDouble());
        }
    }
    else if(Operation.Literals[0].getString() == "string" || Operation.Literals[0].getString() == "s"){
        Owner->VectorContainer.back().setType('s');
        for(size_t i = 1; i < Operation.Literals.size(); i++){
            if(Operation.Literals[i].getType() != 's'){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction) << " instruction: Literal ("
                    << Operation.Literals[i].getType() << ") is not of a " << Operation.Literals[0].getString() << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushString(Operation.Literals[i].getString());
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In " << transInstrToStr(Operation.instruction)
            << " instruction the first parameter has incorrect value: \'" << Operation.Literals[0].getString() << "\'.\n";
        return;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack, ActiveEditableText, EventIds);

    ContextClass NewContext;

    if(Owner->VectorContainer.size() > 0){
        NewContext.Modules.Vectors.push_back(&Owner->VectorContainer.back());
        addNewContext(EventContext, NewContext, "vector", Operation.newContextID);
        wereGlobalVariablesCreated = true;
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ProcessClass::tokenizeStringFromContext(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.Literals.size() == 0 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__
            << ": '" << transInstrToStr(Operation.instruction) << "' requires one string literal.\n";
        return;
    }
    
    ContextClass * Context = nullptr;
    if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__
            << ": \'" << transInstrToStr(Operation.instruction) << "\' requires at least one context.\n";
        return;
    }

    string input = "";
    if(!Context->getStringOrAbort(input, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    vector <string> words = tokenizeString(input, Operation.Literals[0].getString()[0]);
    ContextClass NewContext;
    NewContext.type = "value";
    if(Operation.dynamicIDs.size() == 1){
        if(printOutInstructions){
            cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << "\n";
        }
        for(string word : words){
            NewContext.Variables.push_back(VariableModule::newString(word));
        }
        addNewContext(EventContext, NewContext, "value", "");
    }
    else if(Operation.dynamicIDs.size() == 2){
        if(printOutInstructions){
            cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << " " << Operation.dynamicIDs[1] << "\n";
        }
        for(string word : words){
            NewContext.Variables.push_back(VariableModule::newString(word));
        }
        addNewContext(EventContext, NewContext, "value", Operation.dynamicIDs[1]);
    }
    else{
        if(printOutInstructions){
            cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << " ";
            for(size_t i = 0; i < words.size() && i+1 < Operation.dynamicIDs.size(); i++){
                cout << Operation.dynamicIDs[i+1] << " ";
            }
            cout << "\n";
        }
        NewContext.Variables.push_back(VariableModule::newString(""));
        for(size_t i = 0; i < words.size() && i+1 < Operation.dynamicIDs.size(); i++){
            NewContext.Variables.back().setString(words[i]);
            moveOrRename(EventContext, &NewContext, Operation.dynamicIDs[i+1]);
        }
    }
}
void ProcessClass::printTree(OperaClass & Operation, vector<ContextClass> & EventContext, vector<ProcessClass> & Processes){
    string buffor;
    for(const ProcessClass & Process : Processes){
        buffor += "Process " + Process.getID() + "\n";
        for(const Camera2D & Camera : Process.Cameras){
            buffor += "\tCamera " + Camera.getID();
            if(!Camera.getIsActive()){
                buffor += " (disabled)";
            }
            buffor += "\n";
            if(Camera.pinnedCameraID != ""){
                buffor += "\t\tPinnedTo " + Camera.pinnedCameraID + "\n";
            }
            for(string visible : Camera.visibleLayersIDs){
                buffor += "\t\tVisible " + visible + "\n";
            }
            for(string accessible : Camera.accessibleLayersIDs){
                buffor += "\t\tAccesible " + accessible + "\n";
            }
        }
        for(const LayerClass & Layer : Process.Layers){
            buffor += "\tLayer " + Layer.getID() + " <" + intToStr(Layer.Objects.size()) + ">";
            if(!Layer.getIsActive()){
                buffor += " (disabled)";
            }
            buffor += "\n";
            for(const AncestorObject & Object : Layer.Objects){
                buffor += "\t\tObject " + Object.getID();
                if(!Object.getIsActive()){
                    buffor += " (disabled)";
                }
                buffor += "\n";
                for(const TextModule & Text : Object.TextContainer){
                    buffor += "\t\t\tText " + Text.getID();
                    if(!Text.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const EditableTextModule & Text : Object.EditableTextContainer){
                    buffor += "\t\t\tEditableText " + Text.getID();
                    if(!Text.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const SuperTextModule & SuperText : Object.SuperTextContainer){
                    buffor += "\t\t\tSuperText " + SuperText.getID();
                    if(!SuperText.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const SuperEditableTextModule & SuperEditableText : Object.SuperEditableTextContainer){
                    buffor += "\t\t\tSuperEditableText " + SuperEditableText.getID();
                    if(!SuperEditableText.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const ImageModule & Image : Object.ImageContainer){
                    buffor += "\t\t\tImage " + Image.getID();
                    if(!Image.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const MovementModule & Movement : Object.MovementContainer){
                    buffor += "\t\t\tMovement " + Movement.getID();
                    if(!Movement.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const CollisionModule & Collision : Object.CollisionContainer){
                    buffor += "\t\t\tCollision " + Collision.getID();
                    if(!Collision.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const ParticleEffectModule & Particle : Object.ParticlesContainer){
                    buffor += "\t\t\tParticle " + Particle.getID();
                    if(!Particle.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const EveModule & Event : Object.EveContainer){
                    buffor += "\t\t\tEvent " + Event.getID();
                    if(!Event.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                    for(const ChildStruct & Child : Event.Children){
                        buffor += "\t\t\t\tEvent::Child " + Child.ID;
                        buffor += "\n";
                    }
                    if(Event.elseChildID != ""){
                        buffor += "\t\t\t\tEvent::Else " + Event.elseChildID;
                        buffor += "\n";
                    }
                }
                for(const VariableModule & Variable : Object.VariablesContainer){
                    buffor += "\t\t\tVariable::";
                    buffor += Variable.getType();
                    buffor += " " + Variable.getID();
                    buffor += "\n";
                }
                for(const ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
                    buffor += "\t\t\tScrollbar " + Scrollbar.getID();
                    if(!Scrollbar.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const PrimitivesModule & Primitive : Object.PrimitivesContainer){
                    buffor += "\t\t\tPrimitive::" + translatePrimitiveType(Primitive.type) + " " + Primitive.getID();
                    if(!Primitive.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                }
                for(const VectorModule & Vector : Object.VectorContainer){
                    buffor += "\t\t\tVector::";
                    buffor += Vector.getType();
                    buffor += " " + Vector.getID();
                    buffor += "\n";
                }
            }
        }
    }
    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.newContextID << "\n";
    }
    if(Operation.newContextID == ""){
        cout << buffor;
    }
    ContextClass NewContext;
    NewContext.type = "value";
    NewContext.Variables.push_back(VariableModule::newString(buffor));
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::getStringSizeFromContext(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass * Context = nullptr;
    if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << transInstrToStr(Operation.instruction) << "\' requires at least one context.\n";
        return;
    }
    
    string text = "";
    if(!Context->getStringOrAbort(text, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << " " << "\n";
    }

    ContextClass NewContext;
    NewContext.type = "value";
    NewContext.Variables.push_back(VariableModule::newInt(text.size()));
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::getSizeOfContext(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass * Context = nullptr;
    if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << transInstrToStr(Operation.instruction) << "\' requires at least one context.\n";
        return;
    }
    
    int size = 0;
    size = Context->getVectorSize();

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << "\n";
    }

    ContextClass NewContext;
    NewContext.type = "value";
    NewContext.Variables.push_back(VariableModule::newInt(size));
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::getSubStringFromContext(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.dynamicIDs.size() < 3){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << transInstrToStr(Operation.instruction) << "\' requires at least three contexts.\n";
        return;
    }

    ContextClass * TextContext = getContextByID(EventContext, Operation.dynamicIDs[0], true, EventIds);
    if(TextContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': Failed to get context.\n";
        return;
    }
    ContextClass * BeginContext = getContextByID(EventContext, Operation.dynamicIDs[1], true, EventIds);
    if(BeginContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': Failed to get context.\n";
        return;
    }
    ContextClass * LengthContext = getContextByID(EventContext, Operation.dynamicIDs[2], true, EventIds);
    if(LengthContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': Failed to get context.\n";
        return;
    }

    string text = "";
    if(!TextContext->getStringOrAbort(text, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    unsigned beginning = 0;
    if(!BeginContext->getUnsignedOrAbort(beginning, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    unsigned length = 0;
    if(!LengthContext->getUnsignedOrAbort(length, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    if(beginning >= text.size()){
        beginning = 0;
        length = 0;   
    }
    else if(beginning + length > text.size()){
        length = text.size() - beginning;
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << Operation.dynamicIDs[0] << " " << beginning << " " << length << "\n";
    }

    ContextClass NewContext;
    NewContext.type = "value";
    NewContext.Variables.push_back(VariableModule::newString(text.substr(beginning, length)));
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::loadFontFromContext(OperaClass & Operation, vector<ContextClass> & EventContext, EngineClass & Engine){
    if(Operation.dynamicIDs.size() < 3){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << transInstrToStr(Operation.instruction) << "\' requires at least three contexts.\n";
        return;
    }

    ContextClass * FilePathContext = getContextByID(EventContext, Operation.dynamicIDs[0], true, EventIds);
    if(FilePathContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': Failed to get context.\n";
        return;
    }
    ContextClass * SizeContext = getContextByID(EventContext, Operation.dynamicIDs[1], true, EventIds);
    if(SizeContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': Failed to get context.\n";
        return;
    }
    ContextClass * IDContext = getContextByID(EventContext, Operation.dynamicIDs[2], true, EventIds);
    if(IDContext == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\': Failed to get context.\n";
        return;
    }

    string filePath = "";
    if(!FilePathContext->getStringOrAbort(filePath, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    int fontSize = 0;
    if(!SizeContext->getIntOrAbort(fontSize, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    string fontID = "";
    if(!IDContext->getStringOrAbort(fontID, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed.\n";
        return;
    }

    bool ignoreWarnings = false;
    if(Operation.Literals.size() > 0){
        ignoreWarnings = Operation.Literals[0].getBool();
    }

    for(const SingleFont & Font : Engine.FontContainer){
        if(Font.ID == fontID){
            if(!ignoreWarnings){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": In \'" << transInstrToStr(Operation.instruction) << "\' instruction: "
                    << "Loading failed. Font with the id \'" << Font.ID << "\' already exists.\n";
            }
            return;
        }
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << filePath << " " << fontSize << " " << fontID << "\n";
    }

    Engine.loadNewFont(filePath, fontSize, fontID);
}
void ProcessClass::findByIDInEventContext(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass * VectorContext = nullptr, * IDContext = nullptr;
    if(!getPairOfContexts(VectorContext, IDContext, EventContext, Operation.dynamicIDs)){
        return;
    }
    string id = "";
    IDContext->getStringOrIgnore(id, Operation.instruction);

    ContextClass NewContext;
    NewContext.type = VectorContext->type;

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << VectorContext->type << " " << id << " " << Operation.newContextID << "\n";
    }

    if(id == ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
        return;
    }
    
    if(VectorContext->type == "variable"){
        for(VariableModule * Variable : VectorContext->Modules.Variables){
            if(Variable->getID() == id){
                VectorContext->Modules.Variables.push_back(Variable);
                break;
            }
        }
    }
    else if(VectorContext->type == "camera"){
        for(Camera2D * Camera : VectorContext->Cameras){
            if(Camera->getID() == id){
                NewContext.Cameras.push_back(Camera);
                break;
            }
        }
    }
    else if(VectorContext->type == "layer"){
        for(LayerClass * Layer : VectorContext->Layers){
            if(Layer->getID() == id){
                NewContext.Layers.push_back(Layer);
                break;
            }
        }
    }
    else if(VectorContext->type == "object"){
        for(AncestorObject * Object : VectorContext->Objects){
            if(Object->getID() == id){
                NewContext.Objects.push_back(Object);
                break;
            }
        }
    }
    else if(VectorContext->type == "text"){
        for(TextModule * Text : VectorContext->Modules.Texts){
            if(Text->getID() == id){
                NewContext.Modules.Texts.push_back(Text);
                break;
            }
        }
    }
    else if(VectorContext->type == "editable_text"){
        for(EditableTextModule * EditableText : VectorContext->Modules.EditableTexts){
            if(EditableText->getID() == id){
                NewContext.Modules.EditableTexts.push_back(EditableText);
                break;
            }
        }
    }
    else if(VectorContext->type == "super_text"){
        for(SuperTextModule * SuperText : VectorContext->Modules.SuperTexts){
            if(SuperText->getID() == id){
                NewContext.Modules.SuperTexts.push_back(SuperText);
                break;
            }
        }
    }
    else if(VectorContext->type == "super_editable_text"){
        for(SuperEditableTextModule * SuperEditableText : VectorContext->Modules.SuperEditableTexts){
            if(SuperEditableText->getID() == id){
                NewContext.Modules.SuperEditableTexts.push_back(SuperEditableText);
                break;
            }
        }
    }
    else if(VectorContext->type == "image"){
        for(ImageModule * Image : VectorContext->Modules.Images){
            if(Image->getID() == id){
                NewContext.Modules.Images.push_back(Image);
                break;
            }
        }
    }
    else if(VectorContext->type == "movement"){
        for(MovementModule * Movement : VectorContext->Modules.Movements){
            if(Movement->getID() == id){
                NewContext.Modules.Movements.push_back(Movement);
                break;
            }
        }
    }
    else if(VectorContext->type == "collision"){
        for(CollisionModule * Collision : VectorContext->Modules.Collisions){
            if(Collision->getID() == id){
                NewContext.Modules.Collisions.push_back(Collision);
                break;
            }
        }
    }
    else if(VectorContext->type == "particles"){
        for(ParticleEffectModule * Particle : VectorContext->Modules.Particles){
            if(Particle->getID() == id){
                NewContext.Modules.Particles.push_back(Particle);
                break;
            }
        }
    }
    else if(VectorContext->type == "event"){
        for(EveModule * Event : VectorContext->Modules.Events){
            if(Event->getID() == id){
                NewContext.Modules.Events.push_back(Event);
                break;
            }
        }
    }
    else if(VectorContext->type == "scrollbar"){
        for(ScrollbarModule * Scrollbar : VectorContext->Modules.Scrollbars){
            if(Scrollbar->getID() == id){
                NewContext.Modules.Scrollbars.push_back(Scrollbar);
                break;
            }
        }
    }
    else if(VectorContext->type == "primitives"){
        for(PrimitivesModule * Primitive : VectorContext->Modules.Primitives){
            if(Primitive->getID() == id){
                NewContext.Modules.Primitives.push_back(Primitive);
                break;
            }
        }
    }
    else if(VectorContext->type == "vector"){
        for(VectorModule * Vector : VectorContext->Modules.Vectors){
            if(Vector->getID() == id){
                NewContext.Modules.Vectors.push_back(Vector);
                break;
            }
        }
    }
    addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
}
vector<string> getAllFilesNamesWithinFolder(string directory, char mode, EventDescription EventIds){
    vector<string> names;
    if(!std::filesystem::exists(directory)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Directory '" << directory
            << "' does not exist.\n";
        return names;
    }
    if(mode == 'f'){
        for(const auto & entry : std::filesystem::directory_iterator(directory)){
            names.push_back(entry.path().string());
        }
    }
    else{
        for(const auto & entry : std::filesystem::directory_iterator(directory)){
            names.push_back(entry.path().filename().string());
        }
    }
    return names;
}
void ProcessClass::listOutFiles(OperaClass & Operation, vector<ContextClass> & EventContext){
    string directory = "";
    
    if(Operation.dynamicIDs.size() > 0){
        ContextClass * DirContext = getContextByID(EventContext, Operation.dynamicIDs[0], 0, EventIds);
        if(DirContext != nullptr){
            DirContext->getStringOrIgnore(directory, Operation.instruction);
        }
    }

    if(printOutInstructions){
        cout << transInstrToStr(Operation.instruction) << " " << directory << " " << Operation.newContextID << "\n";
    }

    vector <string> fileNames;
    #if _WIN32
        fileNames = getAllFilesNamesWithinFolder(EXE_PATH + workingDirectory + directory, 'n', EventIds);
    #elif __linux__
        fileNames = getAllFilesNamesWithinFolder(EXE_PATH + workingDirectory + directory, 'n', EventIds);
    #endif

    vector <string> directories, normalFiles;
    for(string file : fileNames){
        string fullPath = EXE_PATH + workingDirectory;
        if(directory.size() > 0){
            fullPath += directory + "/";
        }
        fullPath += file;
        if(std::filesystem::is_directory(fullPath)){
            directories.push_back(file);
        }
        else{
            normalFiles.push_back(file);
        }
    }

    fileNames.clear();
    fileNames.insert(fileNames.end(), directories.begin(), directories.end());
    fileNames.insert(fileNames.end(), normalFiles.begin(), normalFiles.end());

    string buffer = "";
    for(string file : fileNames){
        for(char chara : file){
            if(isspace(chara)){
                file = "\"" + file + "\"";
                break;
            }
        }
        buffer += file + " ";
    }
    if(Operation.newContextID == ""){
        cout << buffer;
    }

    ContextClass NewContext;
    NewContext.Variables.push_back(VariableModule::newString(buffer));
    addNewContext(EventContext, NewContext, "value", Operation.newContextID);
}
void ProcessClass::changeWorkingDirectory(OperaClass & Operation, vector<ContextClass> & EventContext){
    string newDirectory = "";
    if(Operation.Literals.size() > 0){
        newDirectory = Operation.Literals[0].getString();
    }
    else if(Operation.dynamicIDs.size() > 0){
        ContextClass * NewDirectoryContext = getContextByID(EventContext, Operation.dynamicIDs[0], false, EventIds);
        if(NewDirectoryContext != nullptr){
            NewDirectoryContext->getStringOrIgnore(newDirectory, Operation.instruction);
        }
    }

    if(newDirectory == ""){
        workingDirectory = "";
        return;
    }
    else if(newDirectory == "."){
        return;
    }
    else if(newDirectory == ".."){
        if(workingDirectory == ""){
            return;
        }
        if(workingDirectory[workingDirectory.size()-1] != '/' || workingDirectory.size() == 1){
            workingDirectory = "";
            return;
        }
        unsigned lastSlash = workingDirectory.size() - 2;
        for(; lastSlash > 0; lastSlash--){
            if(workingDirectory[lastSlash] == '/'){
                lastSlash++;
                break;
            }
        }
        workingDirectory = workingDirectory.substr(0, lastSlash);
        return;
    }
    else{
        string testPath = workingDirectory + newDirectory + "/";
        if(!std::filesystem::exists(EXE_PATH + testPath)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Directory '" << EXE_PATH + testPath << "' does not exist.\n";
            return;
        }
        workingDirectory = testPath;
    }
    
}
void ProcessClass::printWorkingDirectory(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.newContextID == ""){
        cout << workingDirectory;
    }
    ContextClass NewContext;
    NewContext.type = "value";
    NewContext.Variables.push_back(VariableModule::newString(workingDirectory));
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::findSimilarStrings(OperaClass &Operation, vector<ContextClass> &EventContext){
    ContextClass * Pattern = nullptr, * StringVector = nullptr;
    if(!getPairOfContexts(Pattern, StringVector, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << transInstrToStr(Operation.instruction) << "\' requires at least one context.\n";
        return;
    }
    string pattern = "";
    if(!Pattern->getStringOrAbort(pattern, Operation.instruction, EventIds)){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed. First parameter must be a string.\n";
        return;
    }
    vector <string> stringVector;
    if(!StringVector->getStringVectorOrIgnore(stringVector, Operation.instruction)){
        cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Instruction \'" << transInstrToStr(Operation.instruction) << "\' failed. Vector of strings not found.\n";
        return;
    }
    ContextClass NewContext;
    NewContext.type = "value";
    bool match;
    for(string text : stringVector){
        if(text.size() < pattern.size()){
            continue;
        }
        match = true;
        for(size_t i = 0; i < pattern.size(); i++){
            if(pattern[i] != text[i]){
                match = false;
                break;
            }
        }
        if(match){
            NewContext.Variables.push_back(VariableModule::newString(text));
        }
    }
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
OperaClass ProcessClass::executeInstructions(vector<OperaClass> Operations, LayerClass *&OwnerLayer,
    AncestorObject *&Owner, vector<ContextClass> &EventContext, vector<AncestorObject *> &TriggeredObjects,
    vector<ProcessClass> &Processes, vector<EveModule>::iterator &StartingEvent,
    vector<EveModule>::iterator &Event, vector<MemoryStackStruct> &MemoryStack, EngineClass &Engine
){
    string buffor;
    EventIds = EventDescription(OwnerLayer->getID(), Owner->getID(), Event->getID());
    for(OperaClass & Operation : Operations){
        switch(Operation.instruction){
            case continue_i:
                if(printOutInstructions){
                    cout << transInstrToStr(Operation.instruction) << "\n";
                }
                return Operation;
            case break_i:
                if(printOutInstructions){
                    cout << transInstrToStr(Operation.instruction) << "\n";
                }
                return Operation;
            case return_i:
                if(printOutInstructions){
                    cout << transInstrToStr(Operation.instruction) << "\n";
                }
                return Operation;
            case reboot:
                if(printOutInstructions){
                    cout << transInstrToStr(Operation.instruction) << "\n";
                }
                return Operation;
            case power_off:
                if(printOutInstructions){
                    cout << transInstrToStr(Operation.instruction) << "\n";
                }
                return Operation;
            case first: //Aggregate entities and push them on the Variables Stack.
                aggregateEntities(Operation, EventContext, Engine);
                break;
            case last: //Aggregate entities and push them on the Variables Stack.
                aggregateEntities(Operation, EventContext, Engine);
                break;
            case all: //Aggregate entities and push them on the Variables Stack.
                aggregateEntities(Operation, EventContext, Engine);
                break;
            case random_i: //Aggregate entities and push them on the Variables Stack.
                aggregateEntities(Operation, EventContext, Engine);
                break;
            case index_i:
                getReferenceByIndex(Operation, EventContext);
                break;
            case sum: //Execute operations on sets.
                aggregateTwoSets(Operation, EventContext);
                break;
            case intersection: //Execute operations on sets.
                aggregateTwoSets(Operation, EventContext);
                break;
            case diff: //Execute operations on sets.
                aggregateTwoSets(Operation, EventContext);
                break;
            case value: //Get only values from the environment.
                aggregateValues(EventContext, Operation, OwnerLayer, Owner, Engine, &Processes);
                break;
            case bool_i: //Get literals prepared in the event.
                createLiteral(EventContext, Operation);
                break;
            case int_i: //Get literals prepared in the event.
                createLiteral(EventContext, Operation);
                break;
            case double_i: //Get literals prepared in the event.
                createLiteral(EventContext, Operation);
                break;
            case string_i: //Get literals prepared in the event.
                createLiteral(EventContext, Operation);
                break;
            case rand_int: //Generate random int value between
                generateRandomVariable(EventContext, Operation);
                break;
            case find_by_id: //Aggregate context only by id.
                aggregateOnlyById(EventContext, Operation, OwnerLayer, Owner);
                break;
            case let: //Assign a name to the previously aggregated entities.
                nameVariable(EventContext, Operation);
                break;
            case clone_i:
                if(Operation.Literals.size() > 0){
                    cloneEntities(Operation.dynamicIDs, Operation.Literals.back().getBoolUnsafe(true), EventContext, Layers);
                }
                else{
                    cloneEntities(Operation.dynamicIDs, true, EventContext, Layers);
                }
                break;
            case add:
                executeArithmetics(Operation, EventContext);
                break;
            case sub:
                executeArithmetics(Operation, EventContext);
                break;
            case mul:
                executeArithmetics(Operation, EventContext);
                break;
            case div_i:
                executeArithmetics(Operation, EventContext);
                break;
            case mod:
                executeArithmetics(Operation, EventContext);
                break;
            case pow_i:
                executeArithmetics(Operation, EventContext);
                break;
            case inc:
                moveValues(Operation, EventContext);
                break;
            case dec:
                moveValues(Operation, EventContext);
                break;
            case move:
                moveValues(Operation, EventContext);
                break;
            case add_move:
                moveValues(Operation, EventContext);
                break;
            case sub_move:
                moveValues(Operation, EventContext);
                break;
            case mul_move:
                moveValues(Operation, EventContext);
                break;
            case div_move:
                moveValues(Operation, EventContext);
                break;
            case in:
                checkIfVectorContainsVector(Operation, EventContext);
                break;
            case new_i:
                createNewEntities(Operation, EventContext, OwnerLayer, Owner, TriggeredObjects, StartingEvent, Event, MemoryStack, Engine.focusedProcessID);
                break;
            case del:
                markEntitiesForDeletion(Operation, EventContext, OwnerLayer, Owner, TriggeredObjects, Engine.focusedProcessID);
                if(OwnerLayer == nullptr || Owner == nullptr || Event->getIsDeleted()){
                    return OperaClass();
                }
                break;
            case delete_this_event:
                Event->deleteLater();
                wasDeleteExecuted = true;
                if(printOutInstructions){
                    cout << "delete_this_event\n";
                }
                return Operation;
            case bind_i:
                bindFilesToObjects(Operation, EventContext);
                break;
            case build:
                buildEventsInObjects(Operation, EventContext, Owner, StartingEvent, Event, MemoryStack, Engine.allowNotAscii);
                break;
            case load_build:
                customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 'p', Engine.allowNotAscii);
                break;
            case build_subset:
                customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 's', Engine.allowNotAscii);
                break;
            case inject_code:
                customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 'c', Engine.allowNotAscii);
                break;
            case inject_instr:
                customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 'i', Engine.allowNotAscii);
                break;
            case demolish:
                clearEventsInObjects(Operation, EventContext, Owner);
                break;
            case fun:
                executeFunction(Operation, EventContext, Event, Engine);
                break;
            case env:
                changeEngineVariables(Operation, Engine);
                break;
            case edit_proc:
                changeProcessVariables(Operation, Engine.processIDs);
                break;
            case load_bitmap:
                loadBitmap(Operation, Engine.BitmapContainer);
                break;
            case mkdir_i:
                createDirectory(Operation);
                break;
            case rm:
                removeFileOrDirectory(Operation);
                break;
            case rmll:
                removeRecursivelyFileOrDirectory(Operation);
                break;
            case rename_i:
                renameFileOrDirectory(Operation);
                break;
            case print:
                executePrint(Operation, EventContext);
                break;
            case load_text:
                loadFileAsString(Operation, EventContext);
                break;
            case save_text:
                saveStringAsFile(Operation, EventContext);
                break;
            case ls:
                listOutFiles(Operation, EventContext);
                break;
            case lse:
                listOutEntities(Operation, Processes, Engine);
                break;
            case new_proc:
                createNewProcess(Operation, Processes, EventContext, Owner, TriggeredObjects,
                    StartingEvent, Event, MemoryStack, Engine);
                break;
            case var:
                createNewOwnerVariable(Operation, EventContext, Owner, StartingEvent, Event, MemoryStack);
                break;
            case vec:
                createNewOwnerVector(Operation, EventContext, Owner, StartingEvent, Event, MemoryStack);
                break;
            case tokenize:
                tokenizeStringFromContext(Operation, EventContext);
                break;
            case tree:
                printTree(Operation, EventContext, Processes);
                break;
            case len:
                getStringSizeFromContext(Operation, EventContext);
                break;
            case size:
                getSizeOfContext(Operation, EventContext);
                break;
            case substr:
                getSubStringFromContext(Operation, EventContext);
                break;
            case load_font:
                loadFontFromContext(Operation, EventContext, Engine);
                break;
            case reset_keyboard: //TODO: fix and add to the documentation
                Engine.pressedKeys.clear();
                Engine.releasedKeys.clear();
                Engine.firstPressedKeys.clear();
                break;
            case find_by_id_2:
                findByIDInEventContext(Operation, EventContext);
                break;
            case dump_context_stack:
                buffor = "\nStack: ";
                for(auto context : EventContext){
                    buffor += context.ID + ":" + context.type + ":" + context.getValue(EventIds) + ", ";
                }
                buffor += "\n\n";
                printInColor(buffor, 11);
                break;
            case restart_drag:
                detectStartPosOfDraggingCamera(Engine.display, Engine.Mouse);
                break;
            case cd:
                changeWorkingDirectory(Operation, EventContext);
                break;
            case pwd:
                printWorkingDirectory(Operation, EventContext);
                break;
            case similar:
                findSimilarStrings(Operation, EventContext);
                break;
            default:
                cout << "Error: In " << OwnerLayer->getID() << "::" << Owner->getID() << "::" << Event->getID()
                    << "': In " << __FUNCTION__ << ": Instruction '" << transInstrToStr(Operation.instruction) << "' does not exist.\n";
                break;
        }

        cout.flush();
        
        if(printOutInstructions && printOutStackAutomatically && Operation.instruction != EngineInstr::dump_context_stack){
            buffor = "\nStack: ";
            for(auto context : EventContext){
                buffor += context.ID + ":" + context.type + ":" + context.getValue(EventIds) + ", ";
            }
            buffor += "\n\n";
            printInColor(buffor, 11);
        }
    }
    return OperaClass();
}
VariableModule ProcessClass::findNextValueInMovementModule(ConditionClass &Condition, AncestorObject *CurrentObject){
    if(CurrentObject == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    VariableModule NewValue(Condition.Location.moduleType + "_" + Condition.Location.attribute, nullptr, "", "");
    if(Condition.Location.attribute == "is_moving"){
        NewValue.setBool(false);
        for(const MovementModule & Movement : CurrentObject->MovementContainer){
            if(!Movement.getIsDeleted() && Movement.isMoving()){
                NewValue.setBool(true);
                break;
            }
        }
        return NewValue;
    }
    else if(Condition.Location.attribute == "is_still"){
        NewValue.setBool(true);
        for(const MovementModule & Movement : CurrentObject->MovementContainer){
            if(!Movement.getIsDeleted() && Movement.isMoving()){
                NewValue.setBool(false);
                break;
            }
        }
        return NewValue;
    }
    for(const MovementModule & Movement : CurrentObject->MovementContainer){
        if(Movement.getID() != Condition.Location.moduleID){
            continue;
        }
        if(Movement.getIsDeleted()){
            break;
        }
        if(Condition.Location.attribute == "allowed_jumps"){
            NewValue.setInt(Movement.getAllowedJumps());
            return NewValue;
        }
        if(Condition.Location.attribute == "jumps_count"){
            NewValue.setInt(Movement.getJumpsCount());
            return NewValue;
        }
        if(Condition.Location.attribute == "jump_cooldown"){
            NewValue.setDouble(Movement.getJumpCooldown());
            return NewValue;
        }
        if(Condition.Location.attribute == "jump_cooldown_duration"){
            NewValue.setDouble(Movement.getJumpCooldownDuration());
            return NewValue;
        }
        if(Condition.Location.attribute == "can_jump"){
            NewValue.setBool(Movement.getCanJump());
            return NewValue;
        }
        if(Condition.Location.attribute == "body_mass"){
            NewValue.setDouble(Movement.getBodyMass());
            return NewValue;
        }
        if(Condition.Location.attribute == "walking_speed"){
            NewValue.setDouble(Movement.getWalkingSpeed());
            return NewValue;
        }
        if(Condition.Location.attribute == "running_speed"){
            NewValue.setDouble(Movement.getRunningSpeed());
            return NewValue;
        }
        if(Condition.Location.attribute == "jump_speed"){
            NewValue.setDouble(Movement.getJumpSpeed());
            return NewValue;
        }
        if(Condition.Location.attribute == "gravitation"){
            NewValue.setDouble(Movement.getGravitation());
            return NewValue;
        }
        if(Condition.Location.attribute == "base_friction"){
            NewValue.setDouble(Movement.getBaseFriction());
            return NewValue;
        }
        if(Condition.Location.attribute == "momentum_x"){
            NewValue.setDouble(Movement.getMomentum().x);
            return NewValue;
        }
        if(Condition.Location.attribute == "momentum_y"){
            NewValue.setDouble(Movement.getMomentum().y);
            return NewValue;
        }
        if(isStringInGroup(Condition.Location.attribute, 4, "is_moving_up", "is_moving_right", "is_moving_down", "is_moving_left")){
            NewValue.setBool(Movement.isMovingInThisDirection(Condition.Location.attribute));
            return NewValue;
        }
        break;
    }
    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << Condition.Location.attribute << "' is not valid.\n";
    NewValue.setBool(false);
    NewValue.setID("null", nullptr);
    return NewValue;
}
VariableModule ProcessClass::getValueFromObjectInCamera(AncestorObject * CurrentObject, const string & attribute, const string & cameraID){
    if(CurrentObject == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    bool newValue = false;
    if(attribute == "visible"){
        for(const Camera2D & Camera : Cameras){
            if(!Camera.getIsDeleted() && Camera.isObjectVisible(CurrentObject->getPos(), CurrentObject->getSize())){
                newValue = true;
                break;
            }
        }
    }
    else if(attribute == "visible_specific"){
        for(const Camera2D & Camera : Cameras){
            if(Camera.getID() == cameraID){
                if(!Camera.getIsDeleted() && Camera.isObjectVisible(CurrentObject->getPos(), CurrentObject->getSize())){
                    newValue = true;
                }
                break;
            }
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << attribute << "' is not valid.\n";
    }
    return VariableModule::newBool(newValue);
}
VariableModule ProcessClass::getValueFromMouseClickingObject(const MouseClass & Mouse, AncestorObject * CurrentObject, const string & attribute, const short & button){
    if(CurrentObject == nullptr){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    bool result = false;
    if(attribute == "pressed"){
        if(SelectedCamera != nullptr && Mouse.firstPressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            result = Mouse.firstPressedInRectangle(
                CurrentObject->getPos(),
                CurrentObject->getSize(),
                button,
                CurrentObject->getIsScrollable(),
                SelectedCamera
            );
        }
    }
    else if(attribute == "pressing"){
        if(SelectedCamera != nullptr && Mouse.pressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            result = Mouse.pressedInRectangle(
                CurrentObject->getPos(),
                CurrentObject->getSize(),
                button,
                CurrentObject->getIsScrollable(),
                SelectedCamera
            );
        }
    }
    else if(attribute == "released"){
        if(SelectedCamera != nullptr && Mouse.releasedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            result = Mouse.releasedInRectangle(
                CurrentObject->getPos(),
                CurrentObject->getSize(),
                button,
                CurrentObject->getIsScrollable(),
                SelectedCamera
            );
        }
    }
    else{
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << attribute << "' is not valid.\n";
    }
    return VariableModule::newBool(result);
}
VariableModule ProcessClass::getValueFromObjectInCollision(ConditionClass &Condition, AncestorObject * CurrentObject, LayerClass * CurrentLayer){
    if(CurrentObject == nullptr || CurrentObject->getIsDeleted()){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    if(Condition.Location.attribute == "has_solid_hitbox"){
        for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
            if(!Collision.getIsDeleted() && Collision.getIsSolid()){
                return VariableModule::newBool(true);
            }
        }
        return VariableModule::newBool(false);
    }
    else if(Condition.Location.attribute == "is_fully_solid"){
        bool noTransparent = true;
        bool anySolid = false;
        for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
            if(!Collision.getIsDeleted()){
                if(Collision.getIsSolid()){
                    anySolid = true;
                }
                else{
                    noTransparent = false;
                    break;
                }
            }
        }
        return VariableModule::newBool(noTransparent && anySolid);
    }
    else if(Condition.Location.attribute == "detected"){
        for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
            if(Collision.getIsDeleted()){
                continue;
            }
            for(const DetectedCollision & Detected : Collision.Detected){
                if(Detected.collisionType > 0){
                    return VariableModule::newBool(true);
                }
            }
        }
        return VariableModule::newBool(false);
    }
    else if(Condition.Location.attribute == "with_object"){
        for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
            if(Collision.getIsDeleted()){
                continue;
            }
            for(const DetectedCollision & Detected : Collision.Detected){
                if(Detected.collisionType > 0 && Detected.solidID == Condition.Literal.getStringUnsafe()){
                    return VariableModule::newBool(true);
                }
            }
        }
        return VariableModule::newBool(false);
    }
    else if(Condition.Location.attribute == "with_hitbox"){
        for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
            if(Collision.getIsDeleted()){
                continue;
            }
            for(const DetectedCollision & Detected : Collision.Detected){
                if(Detected.collisionType > 0
                    && Detected.solidID == Condition.Literal.getStringUnsafe()
                    && Detected.collisionID == Condition.Location.spareID
                ){
                    return VariableModule::newBool(true);
                }
            }
        }
        return VariableModule::newBool(false);
    }
    if(CurrentLayer == nullptr || CurrentLayer->getIsDeleted()){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Layer does not exist.\n";
        return VariableModule::newBool(false);
    }
    for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
        if(Collision.getID() != Condition.Location.moduleID){
            continue;
        }
        if(Collision.getIsDeleted()){
            break;
        }
        if(Condition.Location.attribute == "in_group"){
            return VariableModule::newBool(Collision.isInAGroup(Condition.Literal.getStringUnsafe()));
        }
        else if(Condition.Location.attribute == "hitbox_is_solid"){
            return VariableModule::newBool(Collision.getIsSolid());
        }
        else if(Condition.Location.attribute == "hitbox_can_penetrate"){
            return VariableModule::newBool(Collision.getCanPenetrateSolids());
        }
        else if(Condition.Location.attribute == "hitbox_ignores_object"){
            return VariableModule::newBool(Collision.ignores("object", Condition.Literal.getStringUnsafe()));
        }
        else if(Condition.Location.attribute == "hitbox_ignores_object_group"){
            for(const PrimaryModule & Primary : CurrentLayer->Objects){
                if(Primary.getID() != Condition.Literal.getStringUnsafe()){
                    continue;
                }
                if(!Primary.getIsDeleted()){
                    for(const string & group : Primary.getGroups()){
                        return VariableModule::newBool(Collision.ignores("groups_of_objects", group));
                    }
                }
                return VariableModule::newBool(false);
            }
            return VariableModule::newBool(false);
        }
        else if(Condition.Location.attribute == "hitbox_ignores_hitbox"){
            return VariableModule::newBool(Collision.ignores("hitboxes", Condition.Literal.getStringUnsafe()));
        }
        else if(Condition.Location.attribute == "hitbox_ignores_hitbox_group"){
            for(const AncestorObject & NextObject : CurrentLayer->Objects){
                if(NextObject.getID() != Condition.Literal.getStringUnsafe()){
                    continue;
                }
                if(NextObject.getIsDeleted()){
                    return VariableModule::newBool(false);
                }
                for(const PrimaryModule & PrimaryCollision : NextObject.CollisionContainer){
                    if(PrimaryCollision.getID() != Condition.Location.spareID){
                        continue;
                    }
                    if(PrimaryCollision.getIsDeleted()){
                        return VariableModule::newBool(false);
                    }
                    for(const string & group : PrimaryCollision.getGroups()){
                        return VariableModule::newBool(Collision.ignores("groups_of_hitboxes", group));
                    }
                    return VariableModule::newBool(false);
                }
                return VariableModule::newBool(false);
            }
            return VariableModule::newBool(false);
        }
        else if(Condition.Location.attribute == "hitbox_detected"){
            for(const DetectedCollision & Detected : Collision.Detected){
                if(Detected.collisionType > 0){
                    return VariableModule::newBool(true);
                }
            }
            return VariableModule::newBool(false);
        }
        else if(Condition.Location.attribute == "hitbox_detected_hitbox"){
            for(const DetectedCollision & Detected : Collision.Detected){
                if(Detected.collisionType > 0
                    && Detected.solidID == Condition.Literal.getStringUnsafe()
                    && Detected.collisionID == Condition.Location.spareID
                ){
                    return VariableModule::newBool(true);
                }
            }
            return VariableModule::newBool(false);
        }
        break;
    }
    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << Condition.Location.attribute << "' is not valid.\n";
    return VariableModule::newBool(false, "null");
}
VariableModule ProcessClass::findNextValueAmongObjects(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, const MouseClass & Mouse){
    LayerClass * CurrentLayer = nullptr;
    AncestorObject * CurrentObject = nullptr;

    if(!findLayerAndObject(Condition.Location, Owner, OwnerLayer, CurrentLayer, CurrentObject)){
        return VariableModule::newBool(false, "null");
    }

    if(Condition.Location.moduleType == "variable"){
        for(const VariableModule & Variable : CurrentObject->VariablesContainer){
            if(Variable.getID() == Condition.Location.moduleID && Variable.getType() != 'n'){
                if(Variable.getIsDeleted()){
                    break;
                }
                return Variable;
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no variable with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "ancestor" || Condition.Location.moduleType == ""){
        return CurrentObject->getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
    }
    else if(Condition.Location.moduleType == "camera"){
        return getValueFromObjectInCamera(CurrentObject, Condition.Location.attribute, Condition.Location.moduleID);
    }
    else if(Condition.Location.moduleType == "mouse"){
        if(canUserInteract){
            return getValueFromMouseClickingObject(Mouse, CurrentObject, Condition.Location.attribute, Condition.Literal.getIntUnsafe());
        }
        else{
            return VariableModule::newBool(false, "null");
        }
    }
    else if(Condition.Location.moduleType == "movement"){
        return findNextValueInMovementModule(Condition, CurrentObject);
    }
    else if(Condition.Location.moduleType == "text"){
        for(const TextModule & Text : CurrentObject->TextContainer){
            if(Text.getID() == Condition.Location.moduleID){
                if(Text.getIsDeleted()){
                    break;
                }
                return Text.getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "editable_text"){
        for(const EditableTextModule & EditableText : CurrentObject->EditableTextContainer){
            if(EditableText.getID() == Condition.Location.moduleID){
                if(EditableText.getIsDeleted()){
                    break;
                }
                return EditableText.getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no editable text with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "super_text"){
        for(const SuperTextModule & SuperText : CurrentObject->SuperTextContainer){
            if(SuperText.getID() == Condition.Location.moduleID){
                if(SuperText.getIsDeleted()){
                    break;
                }
                return SuperText.getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe(), EventIds);
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "super_editable_text"){
        for(const SuperEditableTextModule & SuperEditableText : CurrentObject->SuperEditableTextContainer){
            if(SuperEditableText.getID() == Condition.Location.moduleID){
                if(SuperEditableText.getIsDeleted()){
                    break;
                }
                return SuperEditableText.getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe(), EventIds);
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "collision"){
        return getValueFromObjectInCollision(Condition, CurrentObject, CurrentLayer);
    }
    else if(Condition.Location.moduleType == "scrollbar"){
        for(const ScrollbarModule & Scrollbar : CurrentObject->ScrollbarContainer){
            if(Scrollbar.getID() == Condition.Location.moduleID){
                if(Scrollbar.getIsDeleted()){
                    break;
                }
                return Scrollbar.getValue(Condition.Location.attribute, EventIds);
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    
    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Value not found.\n";
    return VariableModule::newBool(false, "null");
}
VariableModule ProcessClass::findNextValue(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer,
    const EngineClass & Engine, vector<ProcessClass> * Processes, vector<ContextClass> &EventContext
){
    VariableModule NewValue(Condition.Location.source, nullptr, "", "");
    if(Condition.Location.source == "fullscreen"){
        NewValue.setBool(Engine.fullscreen);
        return NewValue;
    }
    if(Condition.Location.source == "on_display_resize"){
        NewValue.setBool(Engine.displayResized);
        return NewValue;
    }
    if(Condition.Location.source == "second_passed"){
        NewValue.setBool(Engine.secondHasPassed());
        return NewValue;
    }
    if(Condition.Location.source == "exists" || Condition.Location.source == "is_directory"){
        ContextClass * Context = getContextByID(EventContext, Condition.Literal.getString(), true, EventIds);
        if(Context == nullptr){
            NewValue.setBool(false);
        }
        else{
            string file = "";
            Context->getStringOrIgnore(file, EngineInstr::value);
            if(file == ""){
                NewValue.setBool(false);
            }
            else{
                file = EXE_PATH + workingDirectory + file;
                if(Condition.Location.source == "exists"){
                    NewValue.setBool(std::filesystem::exists(file));
                }
                else if(Condition.Location.source == "is_directory"){
                    NewValue.setBool(std::filesystem::is_directory(file));
                }
            }
        }
        return NewValue;
    }
    if(Condition.Location.source == "used_os"){
        #if __WIN32__
            NewValue.setString("windows");
        #elif __linux__
            NewValue.setString("linux");
        #elif __APPLE__
            NewValue.setString("orange");
        #else
            NewValue.setString("garlic_bread");
        #endif
        return NewValue;
    }
    if(Condition.Location.source == "screen_w" || Condition.Location.source == "screen_h"){
        if(Condition.Literal.getType() != 'i'){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__
                << ": Accessing value \'" << Condition.Location.source << "\' requires a screen id of the int type.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        int SCREEN_W = 0, SCREEN_H = 0;
        getDesktopResolution(Condition.Literal.getInt(), &SCREEN_W, &SCREEN_H);
        if(Condition.Location.source == "screen_w"){
            NewValue.setInt(SCREEN_W);
        }
        else{
            NewValue.setInt(SCREEN_H);
        }
        return NewValue;
    }
    if(canUserInteract){
        if(Condition.Location.source == "key_pressed"){
            NewValue.setBool(isKeyFirstPressed(Condition.Literal.getInt(), Engine.firstPressedKeys));
            return NewValue;
        }
        if(Condition.Location.source == "key_pressing"){
            NewValue.setBool(isKeyPressed(Condition.Literal.getInt(), Engine.pressedKeys));
            return NewValue;
        }
        if(Condition.Location.source == "key_released"){
            NewValue.setBool(isKeyReleased(Condition.Literal.getInt(), Engine.releasedKeys));
            return NewValue;
        }
        if(Condition.Location.source == "any_key_pressed"){
            NewValue.setBool(Engine.firstPressedKeys.size() > 0);
            return NewValue;
        }
        if(Condition.Location.source == "any_key_pressing"){
            NewValue.setBool(Engine.pressedKeys.size() > 0);
            return NewValue;
        }
        if(Condition.Location.source == "any_key_released"){
            NewValue.setBool(Engine.releasedKeys.size() > 0);
            return NewValue;
        }
        if(Condition.Location.source == "mouse_moved"){
            NewValue.setBool(Engine.Mouse.didMouseMove);
            return NewValue;
        }
        if(Condition.Location.source == "mouse_pressed"){
            NewValue.setBool(Engine.Mouse.isFirstPressed(Condition.Literal.getInt()));
            return NewValue;
        }
        if(Condition.Location.source == "mouse_pressing"){
            NewValue.setBool(Engine.Mouse.isPressed(Condition.Literal.getInt()));
            return NewValue;
        }
        if(Condition.Location.source == "mouse_released"){
            NewValue.setBool(Engine.Mouse.isReleased(Condition.Literal.getInt()));
            return NewValue;
        }
        if(Condition.Location.source == "mouse_x"){
            NewValue.setDouble(Engine.Mouse.getPos().x);
            return NewValue;
        }
        if(Condition.Location.source == "mouse_y"){
            NewValue.setDouble(Engine.Mouse.getPos().y);
            return NewValue;
        }
    }
    else{
        if(isStringInGroup(Condition.Location.source, 10, "key_pressed", "key_pressing",
            "key_released", "any_key_pressed", "any_key_pressing", "any_key_released",
            "mouse_moved", "mouse_pressed", "mouse_pressing", "mouse_released")
        ){
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute + "<user_cannot_interract>", nullptr);
            NewValue.setBool(0);
            return NewValue;
        }
    }
    if(Condition.Location.source == "window_w"){
        NewValue.setInt(Engine.displaySize.x);
        return NewValue;
    }
    if(Condition.Location.source == "window_h"){
        NewValue.setInt(Engine.displaySize.y);
        return NewValue;
    }
    if(Condition.Location.source == "number_of_processes"){
        NewValue.setInt(Processes->size());
        return NewValue;
    }
    if(Condition.Location.source == "number_of_cameras"){
        NewValue.setInt(0);
        for(const ProcessClass & ProcessIter : *Processes){
            NewValue.addInt(ProcessIter.Cameras.size());
        }
        return NewValue;
    }
    if(Condition.Location.source == "number_of_layers"){
        NewValue.setInt(0);
        for(const ProcessClass & ProcessIter : *Processes){
            NewValue.addInt(ProcessIter.Layers.size());
        }
        return NewValue;
    }
    if(Condition.Location.source == "number_of_objects"){
        NewValue.setInt(0);
        for(const ProcessClass & ProcessIter : *Processes){
            for(const LayerClass & Layer : ProcessIter.Layers){
                NewValue.addInt(Layer.Objects.size());
            }
        }
        return NewValue;
    }
    
    ProcessClass * Process = this;
    if(Condition.Location.process != "" && Condition.Location.process != ID && Processes != nullptr){
        Process = nullptr;
        for(ProcessClass & ProcessIter : *Processes){
            if(ProcessIter.ID == Condition.Location.process){
                Process = &ProcessIter;
                break;
            }
        }
        if(Process == nullptr){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__
                << ": Process with id \'" << Condition.Location.process << "\' does not exist.\n";
            return VariableModule::newBool(false);
        }
    }
    if(Condition.Location.source == "on_boot"){
        NewValue.setBool(Process->firstIteration);
        return NewValue;
    }
    if(Condition.Location.source == "process"){
        if(Condition.Location.attribute == "number_of_cameras"){
            NewValue.setInt(Process->Cameras.size());
            return NewValue;
        }
        if(Condition.Location.attribute == "number_of_layers"){
            NewValue.setInt(Process->Layers.size());
            return NewValue;
        }
        if(Condition.Location.attribute == "number_of_objects"){
            NewValue.setInt(0);
            for(const LayerClass & Layer : Process->Layers){
                NewValue.addInt(Layer.Objects.size());
            }
            return NewValue;
        }
    }
    if(Condition.Location.source == "camera"){
        NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
        for(const Camera2D & Camera : Process->Cameras){
            if(Camera.getID() != Condition.Location.cameraID){
                continue;
            }
            if(Camera.getIsDeleted()){
                break;
            }
            if(Condition.Location.attribute == "is_selected"){
                NewValue.setBool(SelectedCamera == &Camera);
            }
            else{
                NewValue = Camera.getValue(Condition.Location.attribute);
            }
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
        return NewValue;
    }
    if(Condition.Location.source == "layer"){
        NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
        for(const LayerClass & Layer : Process->Layers){
            if(Layer.getID() != Condition.Location.layerID){
                continue;
            }
            if(Layer.getIsDeleted()){
                break;
            }
            NewValue = Layer.getValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
        return NewValue;
    }
    if(Condition.Location.source == "object"){
        return Process->findNextValueAmongObjects(Condition, Owner, OwnerLayer, Engine.Mouse);
    }
    if(Condition.Location.source == "vector" || Condition.Location.source == "v"){
        ContextClass * Context = nullptr;
        vector<string> dynamicIDs = {Condition.Location.moduleID};
        if(!getOneContext(Context, EventContext, dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        if(Context->Modules.Vectors.size() == 0){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no Vectors in the context.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        if(Context->Modules.Vectors.size() != 1){
            cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                << ": There are several Vectors in the context. Program will proceed with the last added vector.\n";
        }
        if(Condition.Literal.getType() == 'i'){
            return Context->Modules.Vectors.back()->getValue("value", Condition.Literal.getIntUnsafe());
        }
        else if(Condition.Literal.getType() == 's'){
            ContextClass * IndexContext = getContextByID(EventContext, Condition.Literal.getStringUnsafe(), true, EventIds);
            unsigned vectorIdx = 0;
            if(IndexContext == nullptr || !IndexContext->getUnsignedOrAbort(vectorIdx, EngineInstr::value, EventIds)){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Cannot find a context for an index.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            return Context->Modules.Vectors.back()->getValue("value", vectorIdx);
        }
        else if(Condition.Location.attribute == "size" || Condition.Location.attribute == "back"){
            return Context->Modules.Vectors.back()->getValue(Condition.Location.attribute, 0);
        }
    }
    if(Condition.Location.source == "context" || Condition.Location.source == "c"){
        ContextClass * Context = nullptr;
        vector<string> dynamicIDs = {Condition.Literal.getStringUnsafe()};
        if(!getOneContext(Context, EventContext, dynamicIDs)){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No context found.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        if(Context->type == "value"){
            if(Condition.Location.attribute == "size"){
                return VariableModule::newInt(Context->Variables.size());
            }
            if(Context->Variables.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no literals in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Variables.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several literals in the context. Program will proceed with the last added literal.\n";
            }
            return Context->Variables.back();
        }
        if(Context->type == "pointer"){
            if(Condition.Location.attribute == "size"){
                return VariableModule::newInt(Context->BasePointers.size());
            }
            if(Context->BasePointers.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no pointers in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->BasePointers.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several pointers in the context. Program will proceed with the last added pointer.\n";
            }
            NewValue.setValueFromPointer(Context->BasePointers.back());
            return NewValue;
        }
        if(Context->type == "variable"){
            if(Condition.Location.attribute == "size"){
                return VariableModule::newInt(Context->Modules.Variables.size());
            }
            if(Context->Modules.Variables.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no variables in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.Variables.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several variables in the context. Program will proceed with the last added variable.\n";
            }
            return *Context->Modules.Variables.back();
        }
        if(Context->type == "collision"){
            if(Context->Modules.Collisions.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no collisions in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.Collisions.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several collisions in the context. Program will proceed with the last added collision.\n";
            }
            if(Condition.Location.attribute == "detected"){
                for(const DetectedCollision & Detected : Context->Modules.Collisions.back()->Detected){
                    if(Detected.collisionType == Condition.Literal.getIntUnsafe()){
                        NewValue.setBool(true);
                        return NewValue;
                    }
                }
                NewValue.setBool(false);
                return NewValue;
            }
            if(Condition.Location.attribute == "release_on"){
                NewValue.setBool(Context->Modules.Collisions.back()->getMouseCollision() == 'r');
                return NewValue;
            }
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << Condition.Location.attribute
                << "' does not exist in the " << Context->type << " module.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        if(Context->type == "primitives"){
            if(Context->Modules.Primitives.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no Primitives in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.Primitives.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several Primitives in the context. Program will proceed with the last added Primitive.\n";
            }
            if(Condition.Location.attribute == "pos_x"){
                NewValue.setDouble(Context->Modules.Primitives.back()->getPos().x);
                return NewValue;
            }
            else if(Condition.Location.attribute == "pos_y"){
                NewValue.setDouble(Context->Modules.Primitives.back()->getPos().y);
                return NewValue;
            }
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Attribute '" << Condition.Location.attribute
                << "' does not exist in the " << Context->type << " module.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        if(Context->type == "editable_text"){
            if(Context->Modules.EditableTexts.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no EditableTexts in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.EditableTexts.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several EditableTexts in the context. Program will proceed with the last added EditableText.\n";
            }
            return Context->Modules.EditableTexts.back()->getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
        }
        if(Context->type == "super_text"){
            if(Context->Modules.SuperTexts.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no SuperTexts in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.SuperTexts.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several SuperTexts in the context. Program will proceed with the last added SuperTexts.\n";
            }
            return Context->Modules.SuperTexts.back()->getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe(), EventIds);
        }
        if(Context->type == "super_editable_text"){
            if(Context->Modules.SuperEditableTexts.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no SuperEditableTexts in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.SuperEditableTexts.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several SuperEditableTexts in the context. Program will proceed with the last added SuperEditableText.\n";
            }
            return Context->Modules.SuperEditableTexts.back()->getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe(), EventIds);
        }
        if(Context->type == "vector"){
            if(Context->Modules.Vectors.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no Vectors in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.Vectors.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several Vectors in the context. Program will proceed with the last added vector.\n";
            }
            return Context->Modules.Vectors.back()->getValue(Condition.Location.attribute, Context->Modules.Vectors.back()->getSize());
        }
        if(Context->type == "object"){
            if(Context->Objects.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no objects in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Objects.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several objects in the context. Program will proceed with the last added object.\n";
            }
            Condition.Location.layerID = Context->Objects.back()->getLayerID();
            Condition.Location.objectID = Context->Objects.back()->getID();
            return findNextValueAmongObjects(Condition, Owner, OwnerLayer, Engine.Mouse);
        }
        if(Context->type == "layer"){
            if(Context->Layers.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no layers in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Layers.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several layers in the context. Program will proceed with the last added layer.\n";
            }
            NewValue = Context->Layers.back()->getValue(Condition.Location.attribute, Condition.Location.spareID);
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
        if(Context->type == "camera"){
            if(Context->Cameras.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no cameras in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Cameras.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several cameras in the context. Program will proceed with the last added camera.\n";
            }
            if(Condition.Location.attribute == "is_selected"){
                NewValue.setBool(SelectedCamera == Context->Cameras.back());
            }
            else{
                NewValue = Context->Cameras.back()->getValue(Condition.Location.attribute);
            }
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
        if(Context->type == "scrollbar"){
            if(Context->Modules.Scrollbars.size() == 0){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There are no Scrollbars in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.Scrollbars.size() != 1){
                cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__
                    << ": There are several Scrollbars in the context. Program will proceed with the last added Scrollbar.\n";
            }
            return Context->Modules.Scrollbars.back()->getValue(Condition.Location.attribute, EventIds);
        }
        
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": No value can be extracted from the context.\n";
        return NewValue;
    }
    if(Condition.Location.source == "literal"){
        return Condition.Literal;
    }
    if(Condition.Location.source == "variable"){
        for(const VariableModule & Variable : Owner->VariablesContainer){
            if(Variable.getID() == Condition.Location.moduleID){
                if(Variable.getIsDeleted()){
                    break;
                }
                return Variable;
            }
        }
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": There is no variable with id: \'" << Condition.Location.moduleID << "\'.\n";
        return NewValue;
    }
    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": \'" << Condition.Location.source << "\' is not a valid source.\n";
    NewValue.setBool(false);
    return NewValue;
}
char ProcessClass::evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, AncestorObject * Owner, LayerClass * OwnerLayer, const EngineClass & Engine, vector<ContextClass> &EventContext){
    short ignoreFlagOr = 0, ignoreFlagAnd = 0;
    bool comparasion;
    int resultInt;
    double resultDouble;
    VariableModule leftOperand, rightOperand;
    vector<VariableModule> resultStack;
    string newID;
    
    for(ConditionClass & Condition : ConditionalChain){
        if(ignoreFlagOr == 0 && ignoreFlagAnd == 0){
            resultStack.push_back(findNextValue(Condition, Owner, OwnerLayer, Engine, nullptr, EventContext));
        }
        if(resultStack.size() == 0){
            continue;
        }
        for(EngineInstr op : Condition.operators){
            if(ignoreFlagOr > 0 || ignoreFlagAnd > 0){
                if(op == EngineInstr::or_i && ignoreFlagOr > 0){
                    ignoreFlagOr--;
                }
                if(op == EngineInstr::and_i && ignoreFlagAnd > 0){
                    ignoreFlagAnd--;
                }
            }
            else if(op == EngineInstr::not_i){
                leftOperand = resultStack.back();
                leftOperand.negate();
                if(printOutLogicalEvaluations){
                    cout << "!(" << resultStack.back().getID() << ":" << resultStack.back().getAnyValue() << ") -> " << leftOperand.getAnyValue() << "\n";
                }
                resultStack.pop_back();
                leftOperand.setID("!("+leftOperand.getID()+")", nullptr);
                resultStack.push_back(leftOperand);
            }
            else if(op == EngineInstr::igT){
                if(resultStack.back().isConditionMet<bool>(true, EngineInstr::equal, 'b')){
                    if(printOutLogicalEvaluations){
                        cout << resultStack.back().getID() << ":" << "true || ... -> true\n";
                    }
                    ignoreFlagOr++;
                }
            }
            else if(op == EngineInstr::igF){
                if(resultStack.back().isConditionMet<bool>(false, EngineInstr::equal, 'b')){
                    if(printOutLogicalEvaluations){
                        cout << resultStack.back().getID() << ":" << "false && ... -> false\n";
                    }
                    ignoreFlagAnd++;
                }
            }
            else if(resultStack.size() >= 2){
                rightOperand = resultStack.back();
                resultStack.pop_back();
                leftOperand = resultStack.back();
                resultStack.pop_back();

                newID = "(";
                newID += leftOperand.getID();
                newID += op;
                newID += rightOperand.getID();
                newID += ")";

                resultStack.push_back(VariableModule(newID, nullptr, "", ""));
                
                if(op == EngineInstr::and_i || op == EngineInstr::or_i || op == EngineInstr::equal
                    || op == EngineInstr::not_equal || op == EngineInstr::more || op == EngineInstr::less
                    || op == EngineInstr::more_equal || op == EngineInstr::less_equal
                ){
                    comparasion = leftOperand.isConditionMet(op, &rightOperand);
                    
                    if(printOutLogicalEvaluations){
                        cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << transInstrToStr(op) << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << comparasion << "\n";
                    }
                    
                    resultStack.back().setBool(comparasion);
                }
                else if(op == EngineInstr::add || op == EngineInstr::sub || op == EngineInstr::mul
                     || op == EngineInstr::div_i || op == EngineInstr::mod || op == EngineInstr::pow_i
                ){
                    if(leftOperand.getType() == 'd' || rightOperand.getType() == 'd'){
                        resultDouble = leftOperand.floatingOperation(op, &rightOperand);

                        if(printOutLogicalEvaluations){
                            cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << transInstrToStr(op) << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << resultDouble << "\n";
                        }

                        resultStack.back().setDouble(resultDouble);
                    }
                    else{
                        resultInt = leftOperand.intOperation(op, &rightOperand);

                        if(printOutLogicalEvaluations){
                            cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << transInstrToStr(op) << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << resultInt << "\n";
                        }

                        resultStack.back().setInt(resultInt);
                    }
                }
                else{
                    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Operator '" << transInstrToStr(op) << "' unrecognized in the if statement.\n";
                }
            }
        }
    }
    if(resultStack.size() == 1){
        if(resultStack.back().getType() == 's'){
            if(isStringInGroup(resultStack.back().getString(), 3, "true", "t", "1")){
                return 't';
            }
            else if(isStringInGroup(resultStack.back().getString(), 3, "false", "f", "0")){
                return 'f';
            }
            else{
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": String is not a valid booleon value.\n";
            }
        }
        else if(resultStack.back().getBool()){
            return 't';
        }
        return 'f';
    }

    //al_draw_filled_circle(SCREEN_W/2, SCREEN_H/2, 10, al_map_rgb_f(1.0, 0.0, 0.0));
    
    return 't';
}
vector<EveModule>::iterator ProcessClass::FindUnfinishedEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event){
    vector<EveModule>::iterator Unfinished;
    bool doesChildExist;
    for(ChildStruct & Child : Event->Children){
        doesChildExist = false;
        if(Child.finished){
           continue; 
        }
        for(Unfinished = Triggered->EveContainer.begin(); Unfinished != Triggered->EveContainer.end(); Unfinished++){
            if(Unfinished->getID() == Child.ID){
                doesChildExist = true;
                if(!Unfinished->getIsDeleted() && Unfinished->getIsActive()){
                    Child.finished = true;
                    return Unfinished;
                }
            }
        }
        Child.finished = true;
        if(!doesChildExist){
            cout << "Error: In " << EventIds.describe() << ": Child with ID '" << Child.ID << "' does not exist.\n";
        }
    }
    return Event;
}
vector<EveModule>::iterator ProcessClass::FindElseEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event){
    vector<EveModule>::iterator ElseEvent;
    for(ElseEvent = Triggered->EveContainer.begin(); ElseEvent != Triggered->EveContainer.end(); ElseEvent++){
        if(!ElseEvent->getIsDeleted() && ElseEvent->getIsActive() && ElseEvent->getID() == Event->elseChildID){
            Event->elseChildFinished = true;
            return ElseEvent;
        }
    }
    return Event;
}
template <class Module>
void deleteModuleInstance(vector<Module> & Container, vector<string> & IDs, bool & layersWereModified){
    for(auto Instance = Container.begin(); Instance != Container.end();){
        if(Instance->getIsDeleted()){
            removeFromVector(IDs, Instance->getID());
            layersWereModified = true;
            Instance->clear();
            Instance = Container.erase(Instance);
        }
        else{
            ++Instance;
        }
    }
}
void deleteEventInstance(vector<EveModule> & Container, vector<string> & IDs, bool & layersWereModified){
    for(auto Instance = Container.begin(); Instance != Container.end();){
        if(Instance->getIsDeleted() || Instance->willBeDeleted){
            removeFromVector(IDs, Instance->getID());
            layersWereModified = true;
            Instance->clear();
            Instance = Container.erase(Instance);
        }
        else{
            ++Instance;
        }
    }
}
bool ProcessClass::deleteEntities(){
    bool layersWereModified = false, foundInOrder = false;
    unsigned cameraIndex = 0, camPosInOrder = 0;
    for(auto Camera = Cameras.begin(); Camera != Cameras.end(); cameraIndex++){
        if(Camera->getIsDeleted()){
            removeFromVector(camerasIDs, Camera->getID());
            Camera->clear();
            Camera = Cameras.erase(Camera);
            foundInOrder = false;
            for(camPosInOrder = 0; camPosInOrder < camerasOrder.size(); camPosInOrder++){
                if(camerasOrder[camPosInOrder] == cameraIndex){
                    foundInOrder = true;
                    break;
                }
            }
            if(!foundInOrder || camerasOrder.size() == 0){
                std::cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Camera's index was not included in cameras' order.\n";
                continue;
            }
            for(; camPosInOrder < camerasOrder.size()-1; camPosInOrder++){
                camerasOrder[camPosInOrder] = camerasOrder[camPosInOrder + 1];
            }
            camerasOrder.pop_back();
            for(camPosInOrder = 0; camPosInOrder < camerasOrder.size(); camPosInOrder++){
                if(camerasOrder[camPosInOrder] > cameraIndex){
                    camerasOrder[camPosInOrder]--;
                }
            }
        }
        else{
            ++Camera;
        }
    }
    for(auto Layer = Layers.begin(); Layer != Layers.end();){
        if(Layer->getIsDeleted()){
            removeFromVector(layersIDs, Layer->getID());
            layersWereModified = true;
            Layer->clear();
            Layer = Layers.erase(Layer);
        }
        else{
            for(vector<AncestorObject>::iterator Object = Layer->Objects.begin(); Object != Layer->Objects.end();){
                if(Object->getIsDeleted()){
                    removeFromVector(Layer->objectsIDs, Object->getID());
                    layersWereModified = true;
                    Object->clear();
                    Object = Layer->Objects.erase(Object);
                }
                else{
                    deleteModuleInstance(Object->TextContainer, Object->textContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->EditableTextContainer, Object->editableTextContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->SuperTextContainer, Object->superTextContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->SuperEditableTextContainer, Object->superEditableTextContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->ImageContainer, Object->imageContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->MovementContainer, Object->movementContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->CollisionContainer, Object->collisionContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->ParticlesContainer, Object->particlesContainerIDs, layersWereModified);
                    deleteEventInstance(Object->EveContainer, Object->eveContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->VariablesContainer, Object->variablesContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->ScrollbarContainer, Object->scrollbarContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->PrimitivesContainer, Object->primitivesContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->VectorContainer, Object->vectorContainerIDs, layersWereModified);
                    ++Object;
                }
            }
            ++Layer;
        }
    }
    return layersWereModified;
}
void ProcessClass::resetChildren(vector<EveModule>::iterator & Event, AncestorObject * Triggered){
    vector<EveModule*> StackOfEvents = {&(*Event)};
    EveModule * CurrentEvent;
    while(StackOfEvents.size() > 0){
        CurrentEvent = StackOfEvents.back();
        StackOfEvents.pop_back();

        for(ChildStruct & Child : CurrentEvent->Children){
            Child.finished = false;
            for(EveModule & ChildEvent: Triggered->EveContainer){
                if(ChildEvent.getID() == Child.ID){
                    ChildEvent.conditionalStatus = 'n';
                    ChildEvent.areDependentOperationsDone = false;
                    ChildEvent.elseChildFinished = false;
                    StackOfEvents.push_back(&ChildEvent);
                    break;
                }
            }
        }

        if(CurrentEvent->elseChildID != ""){
            for(EveModule & ChildEvent: Triggered->EveContainer){
                if(ChildEvent.getID() == CurrentEvent->elseChildID){
                    ChildEvent.conditionalStatus = 'n';
                    ChildEvent.areDependentOperationsDone = false;
                    ChildEvent.elseChildFinished = false;
                    StackOfEvents.push_back(&ChildEvent);
                    break;
                }
            }
        }
    }
}
void addGlobalVariables(vector<ContextClass> & EventContext, vector<VariableModule> & VariablesContainer){
    bool exists = false;
    for(VariableModule & Variable : VariablesContainer){
        for(const ContextClass & Context : EventContext){
            if(Variable.getID() == Context.ID){
                exists = true;
                break;
            }
        }
        if(exists){
            continue;
        }
        EventContext.push_back(ContextClass());
        EventContext.back().type = "variable";
        EventContext.back().Modules.Variables.push_back(&Variable);
        EventContext.back().ID = Variable.getID();
    }
}
void addGlobalVectors(vector<ContextClass> & EventContext, vector<VectorModule> & VectorContainer){
    bool exists = false;
    for(VectorModule & Vector : VectorContainer){
        for(const ContextClass & Context : EventContext){
            if(Vector.getID() == Context.ID){
                exists = true;
                break;
            }
        }
        if(exists){
            continue;
        }
        EventContext.push_back(ContextClass());
        EventContext.back().type = "vector";
        EventContext.back().Modules.Vectors.push_back(&Vector);
        EventContext.back().ID = Vector.getID();
    }
}
unsigned returnRealPrimaryTriggers(vector<string> primaryTriggerTypes){
    unsigned realTriggersSum = 0;
    for(string trigger : primaryTriggerTypes){
        if(isStringInGroup(trigger, 19, "on_init", "each_iteration", "second_passed", "key_pressed", "key_pressing",
            "key_released", "mouse_moved", "mouse_not_moved", "mouse_pressed", "mouse_pressing", "mouse_released",
            "objects", "variables", "collision", "editables", "movement", "stillness", "on_display_resize")
        ){
            realTriggersSum++;
        }
    }
    return realTriggersSum;
}
void removeOnInitTrigger(vector<string> & primaryTriggerTypes){
    for(auto primaryTrigger = primaryTriggerTypes.begin(); primaryTrigger != primaryTriggerTypes.end();){
        if((*primaryTrigger) == "on_init"){
            primaryTrigger = primaryTriggerTypes.erase(primaryTrigger);
        }
        else{
            ++primaryTrigger;
        }
    }
}
void ProcessClass::triggerEve(EngineClass & Engine, vector<ProcessClass> & Processes){
    //Only events from TriggeredObjects can be executed in the current iteration - events of newly created objects 
    //must wait with execution for the next iteration, unless run() command will be used.
    vector <AncestorObject*> TriggeredObjects;
    if(wasDeleteExecuted && deleteEntities()){
        updateBaseOfTriggerableObjects();
        wasDeleteExecuted = false;
    }
    if(wasNewExecuted || wasAnyEventUpdated){
        updateBaseOfTriggerableObjects();
        wasNewExecuted = false;
        wasAnyEventUpdated = false;
    }
    
    
    detectTriggeredEvents(Engine, TriggeredObjects);

    if(TriggeredObjects.size() == 0){
        return;
    }

    if(printOutInstructions){
        cout << "Final triggered "<< TriggeredObjects.size() << " objects: ";
        for(AncestorObject * Triggered : TriggeredObjects){
            cout << Triggered->getLayerID() << "::" << Triggered->getID() << ", ";
        }
        cout << "\n";
    }

    //Remember to delete pointers to destroyed objects during the iteration
    
    vector<EveModule>::iterator StartingEvent, Event;
    vector<MemoryStackStruct> MemoryStack;
    vector<ContextClass> Context; //All dynamic context created from instructions. It's inherited by the children of an event.

    bool noTriggerableEvents = true;
    

    unsigned triObjIdx = 0;

    unsigned preGeneratedContextSize = 0;

    LayerClass * TriggeredLayer;
    OperaClass Interrupt;
    AncestorObject * Triggered = nullptr;

    for(triObjIdx = 0; triObjIdx < TriggeredObjects.size(); triObjIdx++){
        Triggered = TriggeredObjects[triObjIdx];
        if(Triggered == nullptr || Triggered->getIsDeleted() || !Triggered->getIsActive()){
            continue;
        }
        for(EveModule & Eve : Triggered->EveContainer){
            Eve.conditionalStatus = 'n';
            Eve.areDependentOperationsDone = false;
            Eve.elseChildFinished = false;
            for(ChildStruct & Unfinished : Eve.Children){
                Unfinished.finished = false;
            }
        }

        //Find the first triggerable event.
        noTriggerableEvents = true;
        Event = Triggered->EveContainer.begin();
        for(; Event < Triggered->EveContainer.end(); Event++){
            if(!Event->getIsDeleted() && Event->getIsActive() && Event->primaryTriggerTypes.size() > 0){
                noTriggerableEvents = false;
                break;
            }
        }

        if(noTriggerableEvents){
            continue;
        }

        TriggeredLayer = nullptr;

        for(LayerClass & Layer : Layers){
            if(Triggered->getLayerID() == Layer.getID()){
                TriggeredLayer = &Layer;
                break;
            }
        }

        if(TriggeredLayer == nullptr || TriggeredLayer->getIsDeleted() || !TriggeredLayer->getIsActive()){
            continue;
        }
        
        StartingEvent = Event;

        for(auto _ : Context){
            _.clear();
        }
        Context.clear();
        MemoryStack.clear();

        Context.push_back(ContextClass());
        Context.back().type = "object";
        Context.back().Objects.push_back(Triggered);
        Context.back().ID = "me";
        Context.push_back(ContextClass());
        Context.back().type = "layer";
        Context.back().Layers.push_back(TriggeredLayer);
        Context.back().ID = "my_layer";
        addGlobalVariables(Context, Triggered->VariablesContainer);
        addGlobalVectors(Context, Triggered->VectorContainer);
        preGeneratedContextSize = Context.size();

        wereGlobalVariablesCreated = false;

        do{
            removeOnInitTrigger(Event->primaryTriggerTypes);
            if(printOutInstructions){
                printInColor("\nCurrent event: " + TriggeredLayer->getID() + "::" + Triggered->getID() + "::" + Event->getID() + "\n", 14);
            }
            if(wereGlobalVariablesCreated){
                addGlobalVariables(Context, Triggered->VariablesContainer);
                addGlobalVectors(Context, Triggered->VectorContainer);
            }
            if(Event->conditionalStatus == 'n' && Interrupt.instruction != EngineInstr::break_i){
                Event->conditionalStatus = evaluateConditionalChain(Event->ConditionalChain, Triggered, TriggeredLayer, Engine, Context);
            }
            if(Event->conditionalStatus == 't' && Interrupt.instruction != EngineInstr::break_i){ //if true
                if(!Event->areDependentOperationsDone){
                    Interrupt = executeInstructions(Event->DependentOperations, TriggeredLayer, Triggered, Context, TriggeredObjects,
                        Processes, StartingEvent, Event, MemoryStack, Engine
                    );
                    if(Interrupt.instruction == EngineInstr::power_off){
                        Engine.closeProgram = true;
                        return;
                    }
                    else if(Interrupt.instruction == EngineInstr::reboot){
                        Engine.reboot = true;
                        return;
                    }
                    else if(Interrupt.instruction == EngineInstr::return_i){
                        Interrupt.instruction = EngineInstr::null;
                        break;
                    }
                    if(TriggeredLayer == nullptr || Triggered == nullptr){
                        //cout << "Aborting! The owner of the event has been deleted.\n";
                        break;
                    }
                    Event->areDependentOperationsDone = true;
                }
                if(!Event->checkIfAllChildrenFinished() && Interrupt.instruction != EngineInstr::break_i){
                    MemoryStack.push_back(MemoryStackStruct(Event, Context.size()));
                    Event = FindUnfinishedEvent(Triggered, Event);
                    if(Event != MemoryStack.back().Event){
                        continue;
                    }
                    MemoryStack.pop_back();
                }
            }
            else if(Event->conditionalStatus == 'f' && Interrupt.instruction != EngineInstr::break_i && Event->elseChildID != "" && !Event->elseChildFinished){ //else
                MemoryStack.push_back(MemoryStackStruct(Event, Context.size()));
                Event = FindElseEvent(Triggered, Event);
                if(MemoryStack.back().Event->elseChildFinished){ //True if else event has been found.
                    continue;
                }
                MemoryStack.pop_back();
            }

            if(Event->loop && Event->conditionalStatus != 'f' && Interrupt.instruction != EngineInstr::break_i){ //loop back
                if(MemoryStack.size() > 0){
                    Context.erase(Context.begin() + MemoryStack.back().contextSize, Context.end());
                }
                if(wereGlobalVariablesCreated){
                    addGlobalVariables(Context, Triggered->VariablesContainer);
                    addGlobalVectors(Context, Triggered->VectorContainer);
                }
                
                Event->conditionalStatus = 'n';
                Event->areDependentOperationsDone = false;
                Event->elseChildFinished = false;
                resetChildren(Event, Triggered);
                continue;
            }
            if(Event->loop){
                Interrupt.instruction = EngineInstr::null;
            }
            if(Event->conditionalStatus == 't' && Interrupt.instruction != EngineInstr::break_i){ //operations after loop/if
                Interrupt = executeInstructions(Event->PostOperations, TriggeredLayer, Triggered, Context, TriggeredObjects,
                    Processes, StartingEvent, Event, MemoryStack, Engine
                );
                if(Interrupt.instruction == EngineInstr::power_off){
                    Engine.closeProgram = true;
                    return;
                }
                else if(Interrupt.instruction == EngineInstr::reboot){
                    Engine.reboot = true;
                    return;
                }
                else if(Interrupt.instruction == EngineInstr::return_i){
                    Interrupt.instruction = EngineInstr::null;
                    break;
                }
                if(TriggeredLayer == nullptr || Triggered == nullptr){
                    //cout << "Aborting! The owner of the event has been deleted.\n";
                    break;
                }
            }
            if(StartingEvent != Event){ //jump back in event stack
                Event->conditionalStatus = 'n';
                Event->areDependentOperationsDone = false;
                Event->elseChildFinished = false;
                resetChildren(Event, Triggered);

                Event = MemoryStack.back().Event;

                Context.erase(Context.begin() + MemoryStack.back().contextSize, Context.end());
                
                MemoryStack.pop_back();
                continue;
            }
            for(unsigned i = preGeneratedContextSize; i < Context.size(); i++){
                Context[i].clear();
            }
            Context.erase(Context.begin() + preGeneratedContextSize, Context.end());
            MemoryStack.clear();
            
            do{
                Event++;
                StartingEvent++;
            }while(Event != Triggered->EveContainer.end() && (Event->getIsDeleted() || !Event->getIsActive() || Event->primaryTriggerTypes.size() == 0));
        }while(Event != Triggered->EveContainer.end());

        for(auto _ : Context){
            _.clear();
        }
        Context.clear();
        MemoryStack.clear();

        if(wasDeleteExecuted){
            unsigned deletedBeforeIndex = 0;
            for(unsigned objIdx = 0; objIdx <= triObjIdx; objIdx++){
                if(TriggeredObjects[objIdx] != nullptr && TriggeredObjects[objIdx]->getIsDeleted()){
                    deletedBeforeIndex++;
                }
            }
            if(deleteEntities()){
                updateBaseOfTriggerableObjects();
                detectTriggeredEvents(Engine, TriggeredObjects);
                triObjIdx -= deletedBeforeIndex - 1;
                wasDeleteExecuted = false;
            }
        }

        if(wasNewExecuted || wasAnyEventUpdated){
            updateBaseOfTriggerableObjects();
            wasNewExecuted = false;
            wasAnyEventUpdated = false;
        }
    }

    TriggeredObjects.clear();
}
void ProcessClass::updateTreeOfCamerasFromSelectedRoot(Camera2D * Selected){
    unsigned short tokensOfChanges[Cameras.size()];
    unsigned int i, j;
    bool somethingChanged;
    for(i = 0; i < Cameras.size(); i++){
        tokensOfChanges[i] = 0;
        if(&Cameras[i] == Selected){
            tokensOfChanges[i] = 1;
        }
    }
    do{
        somethingChanged = false;
        for(i = 0; i < Cameras.size(); i++){
            if(tokensOfChanges[i] == 1){
                somethingChanged = true;
                tokensOfChanges[i] = 2;
                for(j = 0; j < Cameras.size(); j++){
                    if(i == j || tokensOfChanges[j] > 0){
                        continue;
                    }
                    if(Cameras[j].isPinnedToCamera && Cameras[j].pinnedCameraID == Cameras[i].ID){
                        Cameras[j].pos = Cameras[i].pos + Cameras[j].relativePos;
                        tokensOfChanges[j] = 1;
                    }
                }
            }
        }
    }while(somethingChanged);
}
void ProcessClass::updateWholeForestOfCameras(){
    //Find all roots of Cameras' graph forest and update each tree.
    for(Camera2D & Camera : Cameras){
        if((!Camera.isPinnedToCamera) || (Camera.isPinnedToCamera && Camera.pinnedCameraID == "")){
            updateTreeOfCamerasFromSelectedRoot(&Camera);
        }
    }
}
void ProcessClass::adjustPositionOfAllCameras(){
    if(SelectedCamera->isPinnedToCamera){
        SelectedCamera->pos = SelectedCamera->relativePos;
        for(Camera2D & Camera : Cameras){
            if(SelectedCamera->pinnedCameraID == Camera.ID){
                SelectedCamera->relativePos -= Camera.pos;
                break;
            }
        }
    }
    updateTreeOfCamerasFromSelectedRoot(SelectedCamera);
}
void ProcessClass::keepPositionInsideScreen(vec2d & pos, vec2d & size, vec2i displaySize){
    if(!SelectedCamera->keepInsideScreen){
        return;
    }
    size.x = std::min(size.x, double(displaySize.x));
    size.y = std::min(size.y, double(displaySize.y));
    pos.x = std::max(pos.x, 0.0);
    pos.y = std::max(pos.y, 0.0);
    pos.x = std::min(pos.x, double(displaySize.x - size.x));
    pos.y = std::min(pos.y, double(displaySize.y - size.y));
}
void ProcessClass::updateCamerasPositions(const EngineClass & Engine){
    if(!canUserInteract || SelectedCamera == nullptr || !SelectedCamera->getIsActive()
        || SelectedCamera->getIsMinimized() || !SelectedCamera->canBeModifiedByMouse
    ){
        return;
    }
    if(!Engine.Mouse.isPressed(0) || activeCameraMoveType == NONE){
        return;
    }
    switch(activeCameraMoveType){
        case CAMERA_FULL:
            if(SelectedCamera->isPinnedToCamera){
                SelectedCamera->relativePos = Engine.Mouse.getPos() - dragStartingPos;
                keepPositionInsideScreen(SelectedCamera->relativePos, SelectedCamera->size, Engine.displaySize);
            }
            else{
                SelectedCamera->pos = Engine.Mouse.getPos() - dragStartingPos;
                keepPositionInsideScreen(SelectedCamera->pos, SelectedCamera->size, Engine.displaySize);
            }
            adjustPositionOfAllCameras();
            break;
        case CAMERA_N:
            SelectedCamera->setSize(SelectedCamera->size.x, dragStartingPos.y - Engine.Mouse.getPos().y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(SelectedCamera->size.x, std::min(SelectedCamera->size.y, dragStartingPos.y - dragStartingPos2.y));
            }
            if(SelectedCamera->isPinnedToCamera){
                SelectedCamera->relativePos.set(SelectedCamera->pos.x, std::min(Engine.Mouse.getPos().y - dragStartingPos2.y, dragLimit.y - dragStartingPos2.y));
                keepPositionInsideScreen(SelectedCamera->relativePos, SelectedCamera->size, Engine.displaySize);
            }
            else{
                SelectedCamera->pos.set(SelectedCamera->pos.x, std::min(Engine.Mouse.getPos().y - dragStartingPos2.y, dragLimit.y - dragStartingPos2.y));
                keepPositionInsideScreen(SelectedCamera->pos, SelectedCamera->size, Engine.displaySize);
            }
            adjustPositionOfAllCameras();
            break;
        case CAMERA_NE:
            SelectedCamera->setSize(Engine.Mouse.getPos().x - dragStartingPos.x, dragStartingPos.y - Engine.Mouse.getPos().y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(std::min(SelectedCamera->size.x, Engine.displaySize.x - SelectedCamera->pos.x),
                                        std::min(SelectedCamera->size.y, dragStartingPos.y - dragStartingPos2.y));
            }
            if(SelectedCamera->isPinnedToCamera){
                SelectedCamera->relativePos.set(SelectedCamera->pos.x, std::min(Engine.Mouse.getPos().y - dragStartingPos2.y, dragLimit.y - dragStartingPos2.y));
                keepPositionInsideScreen(SelectedCamera->relativePos, SelectedCamera->size, Engine.displaySize);
            }
            else{
                SelectedCamera->pos.set(SelectedCamera->pos.x, std::min(Engine.Mouse.getPos().y - dragStartingPos2.y, dragLimit.y - dragStartingPos2.y));
                keepPositionInsideScreen(SelectedCamera->pos, SelectedCamera->size, Engine.displaySize);
            }
            adjustPositionOfAllCameras();
            break;
        case CAMERA_E:
            SelectedCamera->setSize(Engine.Mouse.getPos().x - dragStartingPos.x, SelectedCamera->size.y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(std::min(SelectedCamera->size.x, Engine.displaySize.x - SelectedCamera->pos.x), SelectedCamera->size.y);
            }
            break;
        case CAMERA_SE:
            SelectedCamera->setSize(Engine.Mouse.getPos().x - dragStartingPos.x, Engine.Mouse.getPos().y - dragStartingPos.y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(std::min(SelectedCamera->size.x, Engine.displaySize.x - SelectedCamera->pos.x),
                                        std::min(SelectedCamera->size.y, Engine.displaySize.y - SelectedCamera->pos.y));
            }
            break;
        case CAMERA_S:
            SelectedCamera->setSize(SelectedCamera->size.x, Engine.Mouse.getPos().y - dragStartingPos.y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(SelectedCamera->size.x, std::min(SelectedCamera->size.y, Engine.displaySize.y - SelectedCamera->pos.y));
            }
            break;
        case CAMERA_SW:
            SelectedCamera->setSize(-Engine.Mouse.getPos().x + dragStartingPos.x, Engine.Mouse.getPos().y - dragStartingPos.y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(std::min(SelectedCamera->size.x, dragStartingPos.x - dragStartingPos2.x),
                                        std::min(SelectedCamera->size.y, Engine.displaySize.y - SelectedCamera->pos.y));
            }
            if(SelectedCamera->isPinnedToCamera){
                SelectedCamera->relativePos.set(std::min(Engine.Mouse.getPos().x - dragStartingPos2.x, dragLimit.x - dragStartingPos2.x), SelectedCamera->pos.y);
                keepPositionInsideScreen(SelectedCamera->relativePos, SelectedCamera->size, Engine.displaySize);
            }
            else{
                SelectedCamera->pos.set(std::min(Engine.Mouse.getPos().x - dragStartingPos2.x, dragLimit.x - dragStartingPos2.x), SelectedCamera->pos.y);
                keepPositionInsideScreen(SelectedCamera->pos, SelectedCamera->size, Engine.displaySize);
            }
            adjustPositionOfAllCameras();
            break;
        case CAMERA_W:
            SelectedCamera->setSize(dragStartingPos.x - Engine.Mouse.getPos().x, SelectedCamera->size.y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(std::min(SelectedCamera->size.x, dragStartingPos.x - dragStartingPos2.x), SelectedCamera->size.y);
            }
            if(SelectedCamera->isPinnedToCamera){
                SelectedCamera->relativePos.set(std::min(Engine.Mouse.getPos().x - dragStartingPos2.x, dragLimit.x - dragStartingPos2.x), SelectedCamera->pos.y);
                keepPositionInsideScreen(SelectedCamera->relativePos, SelectedCamera->size, Engine.displaySize);
            }
            else{
                SelectedCamera->pos.set(std::min(Engine.Mouse.getPos().x - dragStartingPos2.x, dragLimit.x - dragStartingPos2.x), SelectedCamera->pos.y);
                keepPositionInsideScreen(SelectedCamera->pos, SelectedCamera->size, Engine.displaySize);
            }
            adjustPositionOfAllCameras();
            break;
        case CAMERA_NW:
            SelectedCamera->setSize(-Engine.Mouse.getPos().x + dragStartingPos.x, dragStartingPos.y - Engine.Mouse.getPos().y);
            if(SelectedCamera->keepInsideScreen){
                SelectedCamera->setSize(std::min(SelectedCamera->size.x, dragStartingPos.x - dragStartingPos2.x),
                                        std::min(SelectedCamera->size.y, dragStartingPos.y - dragStartingPos2.y));
            }
            if(SelectedCamera->isPinnedToCamera){
                SelectedCamera->relativePos.set(std::min(Engine.Mouse.getPos().x - dragStartingPos2.x, dragLimit.x - dragStartingPos2.x),
                                                std::min(Engine.Mouse.getPos().y - dragStartingPos2.y, dragLimit.y - dragStartingPos2.y));
                keepPositionInsideScreen(SelectedCamera->relativePos, SelectedCamera->size, Engine.displaySize);
            }
            else{
                SelectedCamera->pos.set(std::min(Engine.Mouse.getPos().x - dragStartingPos2.x, dragLimit.x - dragStartingPos2.x),
                                        std::min(Engine.Mouse.getPos().y - dragStartingPos2.y, dragLimit.y - dragStartingPos2.y));
                keepPositionInsideScreen(SelectedCamera->pos, SelectedCamera->size, Engine.displaySize);
            }
            adjustPositionOfAllCameras();
            break;
        default:
            al_set_system_mouse_cursor(Engine.display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
            break;
    }
}
bool isALeaf(string leafID, string rootID, const vector <Camera2D> & Cameras){
    if(leafID == "" || rootID == ""){
        return false;
    }
    const Camera2D * Leaf = nullptr;
    for(const Camera2D & Camera : Cameras){
        if(Camera.getID() == leafID){
            Leaf = &Camera;
            break;
        }
    }
    if(Leaf == nullptr || Leaf->pinnedCameraID == "" || !Leaf->isPinnedToCamera){
        return false;
    }
    if(Leaf->pinnedCameraID == rootID || Leaf->getID() == rootID){
        return true;
    }
    return isALeaf(Leaf->pinnedCameraID, rootID, Cameras);
}
void ProcessClass::bringCameraForward(unsigned index, Camera2D * ChosenCamera){
    if(!ChosenCamera->getIsActive()){
        return;
    }
    ChosenCamera->bringBack();
    auto it = camerasOrder.begin() + index;
    std::rotate(it, it + 1, camerasOrder.end());

    if(index == camerasOrder.size() - 1){
        return;
    }

    unsigned treshold = index;

    index = camerasOrder.size() - 1;
    
    for(unsigned j = 0; j < index;){
        if(camerasOrder[j] >= Cameras.size()){
            cout << "Error: Camera index <" << camerasOrder[j] << "> in the cameras' order is out of scope of camera's Container<"
                << Cameras.size() << ">.\n";
            j++;
            continue;
        }
        if(Cameras[camerasOrder[j]].getIsActive() && j >= treshold
            && isALeaf(Cameras[camerasOrder[j]].getID(), ChosenCamera->getID(), Cameras)
        ){
            Cameras[camerasOrder[j]].bringBack();
            index--;
            auto it = camerasOrder.begin() + j;
            std::rotate(it, it + 1, camerasOrder.end());
            j = 0;
            continue;
        }
        j++;
    }

    if(!ChosenCamera->isForcefullyPinned){
        return;
    }

    for(int j = camerasOrder.size() - 1; j >= 0; j--){
        if(!Cameras[camerasOrder[j]].isForcefullyPinned){
            continue;
        }
        for(int k = j - 1; k >= 0; k--){
            if(Cameras[camerasOrder[j]].pinnedCameraID == Cameras[camerasOrder[k]].getID()){
                Cameras[camerasOrder[j]].bringBack();
                for(int l = k; l < j - 1; l++){
                    std::swap(camerasOrder[l], camerasOrder[l + 1]);
                }
                break;
            }
        }
    }
}
void ProcessClass::selectCamera(bool fromAltTab, const MouseClass & Mouse, const vector<short> & pressedKeys,
    const vector<short> & releasedKeys, string & focusedProcessID
){
    if(Cameras.size() == 0 || !canUserInteract){
        return;
    }
    if(fromAltTab && isKeyPressed(ALLEGRO_KEY_LCTRL, pressedKeys) && isKeyReleased(ALLEGRO_KEY_TAB, releasedKeys)){
        unsigned activeCameraIndex = 0;
        for(; activeCameraIndex < camerasOrder.size(); activeCameraIndex++){
            if(Cameras[camerasOrder[activeCameraIndex]].getIsActive()){
                break;
            }
        }
        
        if(camerasOrder[activeCameraIndex] >= Cameras.size()){
            cout << "Error: Camera index <" << camerasOrder[activeCameraIndex] << "> in the cameras' order is out of scope of camera's Container<"
                << Cameras.size() << ">.\n";
            return;
        }

        if(!Cameras[camerasOrder[activeCameraIndex]].getIsActive()){
            return;
        }
        
        focusCamera(Cameras, SelectedCamera, getID(), focusedProcessID, &Cameras[camerasOrder[activeCameraIndex]]);
        SelectedCamera->bringBack();
        focusedProcessID = getID();

        //bringCameraForward(activeCameraIndex, SelectedCamera);

        auto it = camerasOrder.begin();
        std::rotate(it, it + 1, camerasOrder.end());

        int i = camerasOrder.size() - 1;
            
        for(int j = activeCameraIndex; j < i; j++){
            if(camerasOrder[j] >= Cameras.size()){
                cout << "Error: Camera index <" << camerasOrder[j] << "> in the cameras' order is out of scope of camera's Container<"
                    << Cameras.size() << ">.\n";
                continue;
            }
            if(Cameras[camerasOrder[j]].getIsActive() && Cameras[camerasOrder[j]].isForcefullyPinned
                && Cameras[camerasOrder[j]].pinnedCameraID == SelectedCamera->getID()
            ){
                Cameras[camerasOrder[j]].bringBack();
                i--;
                auto it = camerasOrder.begin() + j;
                std::rotate(it, it + 1, camerasOrder.end());
                j = -1;
            }
        }
        return;
    }

    if(fromAltTab){
        return;
    }

    Camera2D * Camera = nullptr;

    for(int i = camerasOrder.size() - 1; i >= 0 ; i--){
        if(camerasOrder[i] >= Cameras.size()){
            cout << "Error: Camera index <" << camerasOrder[i] << "> in the cameras' order is out of scope of camera's Container<"
                << Cameras.size() << ">.\n";
            continue;
        }
        Camera = &Cameras[camerasOrder[i]];
        if(!Camera->getIsActive() || Camera->getIsMinimized()){
            continue;
        }
        if(Mouse.inRectangle(Camera->pos, Camera->size, true, SelectedCamera)){
            focusCamera(Cameras, SelectedCamera, getID(), focusedProcessID, Camera);
            
            focusedProcessID = getID();

            bringCameraForward(i, Camera);

            return;
        }
    }
}
bool ProcessClass::isKeyFirstPressed(short key, vector <short> firstPressedKeys){
    for(short firstPressed : firstPressedKeys){
        if(firstPressed == key){
            return true;
        }
    }
    return false;
}
bool ProcessClass::isKeyPressed(short key, vector <short> pressedKeys){
    for(short pressed : pressedKeys){
        if(pressed == key){
            return true;
        }
    }
    return false;
}
bool ProcessClass::isKeyReleased(short key, vector <short> releasedKeys){
    for(short released : releasedKeys){
        if(released == key){
            return true;
        }
    }
    return false;
}
void ProcessClass::detectStartPosOfDraggingObjects(const MouseClass & Mouse){
    wasMousePressedInSelectedObject = false;

    if(activeCameraMoveType == NONE
        && SelectedCamera != nullptr && SelectedCamera->getIsActive()
        && SelectedCamera->canMoveObjects
        && !SelectedCamera->getIsMinimized() && SelectedLayer != nullptr
        && SelectedObject != nullptr
        && Mouse.inRectangle(SelectedCamera->pos, SelectedCamera->size, true, SelectedCamera)
        && SelectedCamera->isLayerVisible(SelectedLayer->getID())
        && SelectedCamera->isLayerAccessible(SelectedLayer->getID())
    ){
        if(SelectedObject->getIsScrollable()){
            if(Mouse.inRectangle(SelectedCamera->pos + SelectedObject->getPos() + SelectedLayer->pos,
                SelectedObject->getSize(), true, SelectedCamera)
            ){
                wasMousePressedInSelectedObject = true;
                dragStartingPos.set(Mouse.getZoomedPos(SelectedCamera)-SelectedObject->getPos()-SelectedCamera->visionShift);
            }
        }
        else{
            if(Mouse.inRectangle(SelectedCamera->pos + SelectedObject->getPos() + SelectedLayer->pos,
                SelectedObject->getSize(), false, SelectedCamera)
            ){
                wasMousePressedInSelectedObject = true;
                dragStartingPos.set(Mouse.getPos()-SelectedObject->getPos());
            }
        }
    }
}
void ProcessClass::changeCursor(ALLEGRO_DISPLAY *display, const MouseClass & Mouse){
    if(Mouse.isPressed(0)){
        return;
    }
    if(SelectedCamera == nullptr){
        return;
    }
    
    if(!SelectedCamera->getIsActive() || SelectedCamera->getIsMinimized()
        || !SelectedCamera->canBeModifiedByMouse || !SelectedCamera->canMouseResizeNow
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
        return;
    }
    if(Mouse.inRectangle(SelectedCamera->pos + vec2d(5.0, 0.0), vec2d(SelectedCamera->size.x - 10.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(SelectedCamera->size.x - 5.0, 0.0), vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(SelectedCamera->size.x - 5.0, 5.0), vec2d(5.0, SelectedCamera->size.y - 10.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + SelectedCamera->size - vec2d(5.0, 5.0), vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(5.0, SelectedCamera->size.y - 5.0), vec2d(SelectedCamera->size.x - 10.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(0.0, SelectedCamera->size.y - 5.0), vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(0.0, 5.0), vec2d(5.0, SelectedCamera->size.y - 10.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos, vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW);
    }
    else{
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }
}
void ProcessClass::detectStartPosOfDraggingCamera(ALLEGRO_DISPLAY *display, const MouseClass & Mouse){
    activeCameraMoveType = NONE;
    if(!Mouse.isPressed(0) || SelectedCamera == nullptr || !SelectedCamera->getIsActive()
        || SelectedCamera->getIsMinimized() || !SelectedCamera->canBeModifiedByMouse
    ){
        return;
    }
    if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + vec2d(5.0, 0.0), vec2d(SelectedCamera->size.x - 10.0, 5.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N);
        activeCameraMoveType = CAMERA_N;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos() - SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + vec2d(SelectedCamera->size.x - 5.0, 0.0), vec2d(5.0, 5.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE);
        activeCameraMoveType = CAMERA_NE;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x - SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos()-SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + vec2d(SelectedCamera->size.x - 5.0, 5.0),
        vec2d(5.0, SelectedCamera->size.y - 10.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E);
        activeCameraMoveType = CAMERA_E;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x - SelectedCamera->size.x, Mouse.getPos().y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + SelectedCamera->size - vec2d(5.0, 5.0), vec2d(5.0, 5.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE);
        activeCameraMoveType = CAMERA_SE;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x - SelectedCamera->size.x, Mouse.getPos().y - SelectedCamera->size.y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + vec2d(5.0, SelectedCamera->size.y - 5.0),
        vec2d(SelectedCamera->size.x - 10.0, 5.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S);
        activeCameraMoveType = CAMERA_S;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x, Mouse.getPos().y-SelectedCamera->size.y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + vec2d(0.0, SelectedCamera->size.y - 5.0), vec2d(5.0, 5.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW);
        activeCameraMoveType = CAMERA_SW;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y - SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos()-SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos + vec2d(0.0, 5.0), vec2d(5.0, SelectedCamera->size.y - 10.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W);
        activeCameraMoveType = CAMERA_W;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos() - SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(SelectedCamera->canMouseResizeNow
        && Mouse.inRectangle(SelectedCamera->pos, vec2d(5.0, 5.0), true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW);
        activeCameraMoveType = CAMERA_NW;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos()-SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(SelectedCamera->canBeGrabbed
        && Mouse.inRectangle(SelectedCamera->pos + SelectedCamera->grabbingAreaPos, SelectedCamera->grabbingAreaSize, true, SelectedCamera)
    ){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
        activeCameraMoveType = CAMERA_FULL;
        wasMousePressedInSelectedObject = false;
        SelectedCamera->grabbed = true;
        dragStartingPos.set(Mouse.getPos()-SelectedCamera->pos);
    }
}
void ProcessClass::startScrollbarDragging(const MouseClass & Mouse){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive() || SelectedCamera->getIsMinimized()){
        return;
    }
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive() || !SelectedCamera->isLayerVisible(Layer.getID())
            || !SelectedCamera->isLayerAccessible(Layer.getID())){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                continue;
            }
            for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
                if(!Scrollbar.getIsActive()){
                    continue;
                }
                if(Scrollbar.startDragging(Object.getPos(), Mouse, SelectedCamera)){
                    isDraggingScrollbar = true;
                    return;
                }
            }
        }
    }
}
void ProcessClass::stopScrollbarDragging(){
    isDraggingScrollbar = false;
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive() || SelectedCamera->getIsMinimized()){
        return;
    }
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive() || !SelectedCamera->isLayerVisible(Layer.getID())
            || !SelectedCamera->isLayerAccessible(Layer.getID())){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                continue;
            }
            for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
                Scrollbar.stopDragging();
            }
        }
    }
}
void ProcessClass::dragScrollbars(const MouseClass &Mouse){
    // if(SelectedCamera == nullptr || !SelectedCamera->getIsActive() || SelectedCamera->getIsMinimized()){
    //     return;
    // }
    vec2d scrollShift(0.0, 0.0);
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive()){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                continue;
            }
            for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
                if(!Scrollbar.getIsActive()){
                    continue;
                }
                scrollShift.set(0.0, 0.0);
                if(Mouse.scrollPos != Mouse.lastScrollPos){
                    scrollShift = Scrollbar.dragThumbWithMouseWheel(Mouse);
                }
                else if(Scrollbar.dragThumb(Object.getPos(), Mouse)){
                    //Object.setScrollShift(Scrollbar.countScrollShift());
                    scrollShift = Scrollbar.countScrollShift();
                }
                else{
                    continue;
                }
                for(Camera2D & Camera : Cameras){
                    if(isStringInVector(Scrollbar.cameraIDs, Camera.getID())){
                        Camera.setVisionShift(-scrollShift);
                    }
                }
            }
        }
    }
}
void allowParticlesSpawnFromUserInput(const vector<short> & pressedKeys, const vector<short> & releasedKeys, ParticleEffectModule & ParticleEffect){
    if(ParticleEffect.getSpawnOnKeyRelease()){
        for(unsigned int i = 0; i < releasedKeys.size(); i++){
            if(releasedKeys[i] == ParticleEffect.getSpawnKeyBind()){
                ParticleEffect.allowSpawning();
                break;
            }
        }
    }
    else{
        for(unsigned int i = 0; i < pressedKeys.size(); i++){
            if(pressedKeys[i] == ParticleEffect.getSpawnKeyBind()){
                ParticleEffect.allowSpawning();
                break;
            }
        }
    }
}
void ProcessClass::moveParticles(const vector<short> & pressedKeys, const vector<short> & releasedKeys){
    timeToInterruptParticles--;
    if(timeToInterruptParticles <= 0)
        return;

    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive()){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                continue;
            }
            for(ParticleEffectModule & ParticleEffect : Object.ParticlesContainer){
                if(!ParticleEffect.getIsActive()){
                    continue;
                }
                if(canUserInteract){
                    allowParticlesSpawnFromUserInput(pressedKeys, releasedKeys, ParticleEffect);
                }
                ParticleEffect.spawnParticles(Object.getPos());
                ParticleEffect.updateParticles();
            }
        }
    }
}
void ProcessClass::detectBackgroundCollisions(LayerClass & Layer, AncestorObject & Object, vec2d momentum){
    //Clear all collisions and check if some objects are overlaping
    for(CollisionModule & Collision : Object.CollisionContainer){
        Collision.Detected.clear();
        if(!Collision.getIsActive()){
            continue;
        }
        for(AncestorObject & SolidObject : Layer.Objects){
            if(!SolidObject.getIsActive() || &Object == &SolidObject){
                continue;
            }
            for(CollisionModule & SolidCollision : SolidObject.CollisionContainer){
                if(!SolidCollision.getIsActive() || !SolidCollision.getIsSolid()){
                    continue;
                }

                if(ignoreDistantObjects){
                    //Check if objects are in range to collide
                    if(!SolidCollision.isCloseEnough(SolidObject.getPos(), SolidObject.getID(),
                            Object.getPos(), momentum, &Collision)){
                        continue;
                    }
                }

                Collision.detectOverlaping(
                    SolidObject.getID(),
                    SolidCollision.getID(),
                    SolidObject.getPos() + SolidCollision.getPos(),
                    SolidCollision.getSize(),
                    Object.getPos(),
                    momentum
                );
            }
        }
    }
}
bool ProcessClass::shouldCheckOverlapingOnly(CollisionModule & Collision, AncestorObject & SolidObject, CollisionModule & SolidCollision){
    return Collision.getCanPenetrateSolids()
        || Collision.ignores("objects", SolidObject.getID())
        || Collision.ignores("hitboxes", SolidCollision.getID());

    for(string groupID : SolidObject.getGroups()){
        if(Collision.ignores("groups_of_objects", groupID)){
            return true;
        }
    }

    for(string groupID : SolidCollision.getGroups()){
        if(Collision.ignores("groups_of_hitboxes", groupID)){
            return true;
        }
    }

    return false;
}
void ProcessClass::detectRealCollisions(LayerClass & Layer, AncestorObject & Object, MovementModule & Movement){
    for(CollisionModule & Collision : Object.CollisionContainer){
        Collision.Detected.clear();
        if(!Collision.getIsActive()){
            continue;
        }
        for(AncestorObject & SolidObject : Layer.Objects){
            if(!SolidObject.getIsActive() || &Object == &SolidObject){
                continue;
            }
            for(CollisionModule & SolidCollision : SolidObject.CollisionContainer){
                if(!SolidCollision.getIsActive() || !SolidCollision.getIsSolid()){
                    continue;
                }

                if(ignoreDistantObjects && !SolidCollision.isCloseEnough(SolidObject.getPos(), SolidObject.getID(), Object.getPos(), Movement.getMomentum(), &Collision)){
                    continue;
                }

                if(shouldCheckOverlapingOnly(Collision, SolidObject, SolidCollision)){
                    Collision.detectOverlaping(
                        SolidObject.getID(), SolidCollision.getID(), SolidObject.getPos() + SolidCollision.getPos(),
                        SolidCollision.getSize(), Object.getPos(), Movement.getMomentum()
                    );
                    continue;
                }

                Collision.detectCollision(
                    SolidObject.getID(), SolidCollision.getID(), SolidObject.getPos()+SolidCollision.getPos(),
                    SolidCollision.getSize(), Object.getPos(), Movement.getMomentum()
                );
            }
        }
        Collision.removeImaginaryCollisions();
    }
}
void ProcessClass::adjustAndStopMomentum(AncestorObject & Object, MovementModule & Movement){
    for(CollisionModule & Collision : Object.CollisionContainer){
        if(Movement.getMomentum().isEqual(0.0, 0.0)){
            break;
        }
        if(Collision.Detected.size() == 0){
            continue;
        }

        for(DetectedCollision Detected : Collision.Detected){
            //Ignore invalid collisions (0) and overlaping "collisions" (4)
            if(Detected.collisionType == 0 || Detected.collisionType == 4){
                continue;
            }
            //Update momentum on the X axis. Third type of collisions is collision with corners - you could either adjust X or Y axis, but I chose X axis.
            if(Detected.collisionType == 1 || Detected.collisionType == 3){
                if(abs(Movement.getMomentum().x) >= abs(Detected.momentum.x)){
                    Movement.setMomentum(Detected.momentum.x, Movement.getMomentum().y);
                }
            }
            //Update momentum on the Y axis
            if(Detected.collisionType == 2){
                if(abs(Movement.getMomentum().y) >= abs(Detected.momentum.y)){
                    if(Movement.getMomentum().y > 0.0){
                            Movement.resetJump();
                    }
                    Movement.setMomentum(Movement.getMomentum().x, Detected.momentum.y);
                }
            }
        }
    }
    Object.translatePos(Movement.getMomentum());
}
void ProcessClass::updateCameraPosition(Camera2D & Camera, AncestorObject * FollowedByCamera){
    if(Camera.isFollowingObject && FollowedByCamera != nullptr){
        vec2d newPos(-FollowedByCamera->getPos().x+Camera.size.x/2, -FollowedByCamera->getPos().y+Camera.size.y/2);
        for(ImageModule Image : FollowedByCamera->ImageContainer){
            if(!Image.getIsActive()){
                continue;
            }
            if(Image.getID() == Camera.followedImageID){
                vec2d size(Image.getSize());
                if(Image.getIsScaledFromCenter())
                    newPos.translate(-size.x/2, -size.y/2);
                else{
                    size.multiply(Image.getScale());
                    newPos.translate(-size.x/2, -size.y/2);
                }
                break;
            }
        }
        Camera.setVisionShift(newPos);
    }
}
void ProcessClass::moveObjects(const vector<short> & pressedKeys, const MouseClass & Mouse){
    timeToInterruptMovement--;
    if(timeToInterruptMovement <= 0)
        return;

    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive()){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                continue;
            }
            //Detecting objects' overlaps in objects without MovementModule instance.
            if(Object.MovementContainer.size() == 0){
                detectBackgroundCollisions(Layer, Object, vec2d(0.0, 0.0));
            }
            for(MovementModule & Movement : Object.MovementContainer){
                if(!Movement.getIsActive()){
                    continue;
                }
                bool pressedMouse[] = {0, 0, 0, 0, 0};
                bool releasedMouse[] = {0, 0, 0, 0, 0};
                if(canUserInteract){
                    Mouse.getPressed(pressedMouse);
                    Mouse.getReleased(releasedMouse);
                }
                Movement.updateStatesAndVectors(canUserInteract, pressedKeys, pressedMouse, releasedMouse, Mouse.getZoomedPos(SelectedCamera), Object.getPos());
                Movement.updateMomentum(Object.getPos());

                //If object doesn't move, clear movement's collisions and detect overlaping objects
                if(Movement.getMomentum().isEqual(0.0, 0.0)){
                    detectBackgroundCollisions(Layer, Object, vec2d(0.0, 0.0));
                    continue;
                }

                //Detect all collisions with solid collision boxes and put them in a vector
                detectRealCollisions(Layer, Object, Movement);

                //Adjust or stop momentum in case of collision
                adjustAndStopMomentum(Object, Movement);
            }
            for(Camera2D & Camera : Cameras){
                if(!Camera.getIsActive() || Camera.getIsMinimized() || !Camera.isLayerVisible(Layer.getID()))
                    continue;
                if(Camera.isFollowingObject && Camera.followedLayerID == Layer.getID() && Camera.followedObjectID == Object.getID()){
                    updateCameraPosition(Camera, &Object);
                }
            }
        }
    }
}
void ProcessClass::moveSelectedObject(const MouseClass & Mouse){
    if(SelectedLayer != nullptr && SelectedObject != nullptr
        && Mouse.isPressed(0) && wasMousePressedInSelectedObject
        && SelectedCamera != nullptr && SelectedCamera->getIsActive()
        && SelectedCamera->canMoveObjects
        && !SelectedCamera->getIsMinimized()
        && SelectedCamera->isLayerVisible(SelectedLayer->getID())
        && SelectedCamera->isLayerAccessible(SelectedLayer->getID())
        && SelectedObject->canBeMovedWithMouse
    ){
        if(SelectedObject->getIsScrollable()){
            SelectedObject->setPos(Mouse.getZoomedPos(SelectedCamera)-SelectedCamera->visionShift-dragStartingPos);
        }
        else{
            SelectedObject->setPos(Mouse.getPos()-dragStartingPos);
        }
    }
}
void ProcessClass::drawEverything(EngineClass & Engine){
    //al_set_clipping_rectangle(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
    
    size_t numberOfDrawnObjects;
    Camera2D * Camera;

    for(const unsigned & cameraIndex : camerasOrder){
        if(cameraIndex >= Cameras.size()){
            cout << "Error: Camera index <" << cameraIndex << "> in the cameras' order is out of scope of camera's Container<"
                << Cameras.size() << ">.\n";
            continue;
        }
        Camera = &Cameras[cameraIndex];
        if(!Camera->getIsActive() || Camera->getIsMinimized() || (!Camera->canDrawOnCamera && !Camera->drawOneFrame)){
            continue;
        }
        Camera->drawOneFrame = false;
        al_set_clipping_rectangle(Camera->pos.x, Camera->pos.y, Camera->size.x, Camera->size.y);
        for(LayerClass & Layer : Layers){
            if(!Layer.getIsActive() || !Camera->isLayerVisible(Layer.getID())){
                continue;
            }
            
            numberOfDrawnObjects = 0;

            for(size_t i = 0; i < Layer.Objects.size(); i++){
                if(!Layer.Objects[i].getIsActive()){
                    continue;
                }
                drawModules(Layer.Objects[i], i, *Camera, Engine.FontContainer, numberOfDrawnObjects, Engine.displaySize);
            }
            if(SelectedLayer == &Layer){
                drawSelectionBorder(*Camera);
            }
        }
        al_reset_clipping_rectangle();
        
        if(drawCameraBorders && Camera->allowsDrawingBorders){
            if(SelectedCamera != Camera){
                al_draw_rectangle(Camera->pos.x, Camera->pos.y, Camera->pos.x + Camera->size.x, Camera->pos.y + Camera->size.y, al_map_rgb(0, 155, 145), 6);
            }
            else{
                al_draw_rectangle(Camera->pos.x, Camera->pos.y, Camera->pos.x + Camera->size.x, Camera->pos.y + Camera->size.y, al_map_rgb(62, 249, 239), 6);
            }
        }
        /*if(Engine.Mouse.isPressed(0)){
            al_draw_rectangle(Engine.Mouse.getPressedPos().x, Engine.Mouse.getPressedPos().y,
                Engine.Mouse.getPos().x, Engine.Mouse.getPos().y, al_map_rgb(200, 200, 200), 1);
        }*/
    }

    //draw_horizontal_gradient_rect(200, 200, 840, 680, al_map_rgba(255, 0, 255, 255), al_map_rgba(0, 255, 0, 255));
}
void ProcessClass::drawModules(AncestorObject & Object, size_t iteration, Camera2D & Camera,
    vector <SingleFont> & FontContainer, size_t & numberOfDrawnObjects, vec2i displaySize
){
    vec2d newPos;
    vec2d objectSize;
    vec2d scaledObjectSize;

    for(PrimitivesModule & Primitives : Object.PrimitivesContainer){
        if(!Primitives.getIsActive()){
            continue;
        }
        
        if(drawOnlyVisibleObjects && Primitives.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(Primitives.getPos());
            objectSize.set(Primitives.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Primitives.getScale());
            if(!Primitives.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }
        Primitives.draw(Object.getPos()+Camera.pos, Camera, false);
        numberOfDrawnObjects++;
    }

    for(ImageModule & Image : Object.ImageContainer){
        if(!Image.getIsActive()){
            continue;
        }
        //Image.drawFrame(x, y);
        if(drawOnlyVisibleObjects && Image.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(Image.getPos());
            objectSize.set(Image.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Image.getScale());
            if(!Image.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }
        Image.drawImage(Object.getPos()+Camera.pos, Camera, false);
        numberOfDrawnObjects++;
    }

    for(TextModule & Text : Object.TextContainer){
        if(!Text.getIsActive()){
            continue;
        }
        if(drawOnlyVisibleObjects && Text.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(Text.getPos());
            objectSize.set(Text.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Text.getScale());
            if(!Text.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }

        //If a font exists in FontContainer, draw the text on screen.
        for(SingleFont font : FontContainer){
            if(Text.getFontID() == font.ID){
                Text.drawText(Object.getPos()+Camera.pos, font.font, drawTextFieldBorders, Camera, 0, 0, false);
                break;
            }
        }

        numberOfDrawnObjects++;
    }

    for(SuperTextModule & SuperText : Object.SuperTextContainer){
        if(!SuperText.getIsActive()){
            continue;
        }
        if(drawOnlyVisibleObjects && SuperText.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(SuperText.getPos());
            objectSize.set(SuperText.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(SuperText.getScale());
            if(!SuperText.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }

        SuperText.draw(Object.getPos()+Camera.pos, drawTextFieldBorders, Camera, 0, 0, false, displaySize);

        numberOfDrawnObjects++;
    }

    for(SuperEditableTextModule & SuperEditableText : Object.SuperEditableTextContainer){
        if(!SuperEditableText.getIsActive()){
            continue;
        }
        if(drawOnlyVisibleObjects && SuperEditableText.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(SuperEditableText.getPos());
            objectSize.set(SuperEditableText.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(SuperEditableText.getScale());
            if(!SuperEditableText.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }

        SuperEditableText.draw(Object.getPos()+Camera.pos, drawTextFieldBorders, Camera, 0, 0, false, displaySize);

        numberOfDrawnObjects++;
    }

    for(EditableTextModule & Editable : Object.EditableTextContainer){
        if(!Editable.getIsActive()){
            continue;
        }
        if(drawOnlyVisibleObjects && Editable.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(Editable.getPos());
            objectSize.set(Editable.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Editable.getScale());
            if(!Editable.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }

        //If a font exists in FontContainer, draw the text on screen.
        for(auto font : FontContainer){
            if(Editable.getFontID() == font.ID){
                Editable.drawText(Object.getPos()+Camera.pos, font.font, drawTextFieldBorders,
                    Camera, Editable.getCursorPos(), Editable.secondCursorPos, Editable.getEditingIsActive()
                );
            }
        }
        numberOfDrawnObjects++;
    }

    for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
        if(!Scrollbar.getIsActive() || !Scrollbar.canBeDrawn){
            continue;
        }

        if(drawOnlyVisibleObjects && Scrollbar.getIsScrollable()){
            newPos.set(Object.getPos());
            newPos.translate(Scrollbar.getPos());
            objectSize.set(Scrollbar.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Scrollbar.getScale());
            if(!Scrollbar.getIsScaledFromCenter()){
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            }
            else{
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);
            }

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
        }
        Scrollbar.draw(Object.getPos()+Camera.pos, Object.ImageContainer, Camera);
        numberOfDrawnObjects++;
    }

    if(drawHitboxes){
        vec2d hitboxSize;
        for(CollisionModule & Hitbox : Object.CollisionContainer){
            if(!Hitbox.getIsActive()){
                continue;
            }
            newPos = Object.getPos() + Hitbox.getPos();
            hitboxSize = Hitbox.getSize();
            
            if(Hitbox.getIsScrollable()){
                newPos.set(Camera.translateWithZoom(newPos));
                hitboxSize.multiply(Camera.zoom);
            }
            
            vec4d borderColor(0, 0, 255, 127);
            for(DetectedCollision Detection : Hitbox.Detected){
                if(Detection.collisionType > 0){
                    borderColor.val[1] = 255;
                    borderColor.val[2] = 0;
                    break;
                }
            }
            if(Hitbox.getIsCircle()){
                al_draw_circle(newPos.x+Camera.pos.x, newPos.y+Camera.pos.y, hitboxSize.x,
                    al_map_rgba(borderColor.val[0], borderColor.val[1], borderColor.val[2], borderColor.val[3]), 2
                );
            }
            else{
                al_draw_rectangle(newPos.x+Camera.pos.x, newPos.y+Camera.pos.y, newPos.x+Camera.pos.x+hitboxSize.x, newPos.y+Camera.pos.y+hitboxSize.y,
                    al_map_rgba(borderColor.val[0], borderColor.val[1], borderColor.val[2], borderColor.val[3]), 2
                );
            }
        }
    }

    for(ParticleEffectModule & ParticleEffect : Object.ParticlesContainer){
        if(!ParticleEffect.getIsActive()){
            continue;
        }
        ParticleEffect.drawParticles(Object.ImageContainer, displaySize, Camera);
        numberOfDrawnObjects++;
    }
}
void ProcessClass::delayEditableTextFields(){
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive()){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                continue;
            }
            for(EditableTextModule & TextField : Object.EditableTextContainer){
                if(!TextField.getIsActive() || !TextField.getCanBeEdited() || !TextField.getEditingIsActive() || TextField.currentInputDelay == 0.0){
                    continue;
                }
                TextField.currentInputDelay -= 1/FPS; 
            }
        }
    }
}
void ProcessClass::updateEditableTextFields(EngineClass & Engine){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()
        || SelectedCamera->getIsMinimized() || !SelectedCamera->canEditText){
        return;
    }
    if(Engine.Mouse.isPressed()){
        vec2d finalPos, finalSize;
        for(LayerClass & Layer : Layers){
            if(!Layer.getIsActive()){
                continue;
            }
            for(AncestorObject & Object : Layer.Objects){
                if(!Object.getIsActive()){
                    continue;
                }
                for(EditableTextModule & TextField : Object.EditableTextContainer){
                    if(!TextField.getIsActive()){
                        continue;
                    }
                    if(SelectedCamera->isLayerAccessible(Layer.getID()) && TextField.getCanBeEdited()){
                        finalPos.set(Object.getPos());
                        finalPos.translate(TextField.getPos());
                        if(TextField.getIsScrollable()){
                            finalPos.translate(SelectedCamera->pos / SelectedCamera->zoom + SelectedCamera->visionShift);
                        }
                        else{
                            finalPos.translate(SelectedCamera->pos);
                        }
                        finalSize.set(TextField.getSize());
                        finalSize.multiply(TextField.getScale());
                        if(Engine.Mouse.firstPositionInRectangle(finalPos, finalSize, 0, TextField.getIsScrollable(), SelectedCamera)){
                            TextField.setEditingIsActive(true);
                            TextField.setCursorPos(finalPos, finalSize, Engine.FontContainer, Engine.Mouse, *SelectedCamera);
                            if(Engine.Mouse.firstPressedInRectangle(finalPos, finalSize, 0, TextField.getIsScrollable(), SelectedCamera)){
                                TextField.secondCursorPos = TextField.cursorPos;
                            }
                            
                            //TextField.setCursorPos(TextField.getCurrentContent().size());
                            continue;
                        }
                    }
                    if(TextField.getEditingIsActive()){
                        //Object.operateTextFieldUpdate(TextField, Layer.Objects, Engine.BitmapContainer, Layer.objectsIDs, EXE_PATH + workingDirectory);
                    }
                    TextField.setEditingIsActive(false);
                }
            }
        }
    }
    else{
        for(LayerClass & Layer : Layers){
            if(!Layer.getIsActive()){
                continue;
            }
            for(AncestorObject & Object : Layer.Objects){
                if(!Object.getIsActive()){
                    continue;
                }
                for(EditableTextModule & TextField : Object.EditableTextContainer){
                    if(!TextField.getIsActive() || !TextField.getCanBeEdited() || !TextField.getEditingIsActive()){
                        continue;
                    }
                    TextField.editText(Engine.releasedKeys, Engine.pressedKeys, Engine.FontContainer, Engine.display);
                    if(!TextField.getUpdateConnectedVariable()){
                        continue;
                    }
                    Object.operateTextFieldUpdate(TextField, Layer.Objects, Engine.BitmapContainer, Layer.objectsIDs, EXE_PATH + workingDirectory);
                    TextField.setUpdateConnectedVariable(false);

                    if(SelectedLayer != nullptr && SelectedObject != nullptr && Object.getID() == "editor_window"){
                        Object.EditableTextContainer[0].modifyContent(0, SelectedObject->getID());
                    }
                }
            }
        }
    }
}
void ProcessClass::selectObject(const MouseClass & Mouse){
    if(!Mouse.isPressed(0) || SelectedCamera == nullptr || !SelectedCamera->getIsActive()
        || SelectedCamera->getIsMinimized()){
        return;
    }
    
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive() || !SelectedCamera->isLayerVisible(Layer.getID()) || !SelectedCamera->isLayerAccessible(Layer.getID())){
            continue;
        }
        
        //Check if mouse cursor was in the camera box while mouse button was pressed.  
        if(!Mouse.pressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            continue;
        }

        for(AncestorObject & Object : Layer.Objects){
            //You can select text only if you can select its object. Without selecting the object, selecting more than one letter in a text field will be harder.
            if(!Object.getIsActive() || !Object.getCanBeSelected()
                || !Mouse.pressedInRectangle(SelectedCamera->pos + Object.getPos(),
                    Object.getSize(), 0, Object.getIsScrollable(), SelectedCamera)
            ){
                continue;
            }
            if(ActiveEditableText != nullptr){
                ActiveEditableText->isEditingActive = false;
                for(FormatClass & Format : ActiveEditableText->Formatting){
                    Format.selected = false;
                }
                ActiveEditableText->update();
                ActiveEditableText = nullptr;
            }
            if(SelectedCamera->canEditText && !isDraggingScrollbar){
                for(SuperEditableTextModule & SuperEditableText : Object.SuperEditableTextContainer){
                    if(!SuperEditableText.getIsActive() || !SuperEditableText.canBeEdited ||
                        !Mouse.pressedInRectangle(
                            SelectedCamera->pos + Object.getPos() + SuperEditableText.getPos(), SuperEditableText.getSize(), 0,
                            SuperEditableText.getIsScrollable(), SelectedCamera
                        )
                    ){
                        continue;
                    }
                    ActiveEditableText = &SuperEditableText;
                    ActiveEditableText->isEditingActive = true;
                    ActiveEditableText->cursorPos = 0;
                    ActiveEditableText->setCursorsWithMouse(SelectedCamera->pos + Object.getPos(), Mouse, SelectedCamera);
                    if(Mouse.pressedInRectangle(
                        SelectedCamera->pos + Object.getPos() + SuperEditableText.getPos(), SuperEditableText.getSize(),
                        0, SuperEditableText.getIsScrollable(), SelectedCamera
                    )){
                        ActiveEditableText->secondCursorPos = ActiveEditableText->cursorPos;
                        //Update variables needed for moving up and down in the text.
                        ActiveEditableText->localSecondCursorPos = ActiveEditableText->localCursorPos;
                        ActiveEditableText->lineWithSecondCursorIdx = ActiveEditableText->lineWithCursorIdx;
                        ActiveEditableText->lineWidthToSecondCursor = ActiveEditableText->lineWidthToCursor;
                    }
                    ActiveEditableText->divideFormattingByCursor();
                    break;
                }
            }
            if(&Layer == SelectedLayer && &Object == SelectedObject){
                return;
            }
            selectedObjectLayerID = Layer.getID();
            selectedObjectID = Object.getID();
            SelectedLayer = &Layer;
            SelectedObject = &Object;
            SelectedObject->refreshCoordinates();
            return;
        }
    }
    if(ActiveEditableText != nullptr){
        ActiveEditableText->isEditingActive = false;
        for(FormatClass & Format : ActiveEditableText->Formatting){
            Format.selected = false;
        }
        ActiveEditableText->update();
        ActiveEditableText = nullptr;
    }
}
void ProcessClass::unselectObject(){
    selectedObjectLayerID = "";
    selectedObjectID = "";
    SelectedLayer = nullptr;
    SelectedObject = nullptr;
}

void ProcessClass::drawSelectionBorder(Camera2D Camera){
    if(SelectedObject == nullptr || !SelectedObject->canDrawSelectionBorder|| !Camera.canDrawObjectBorders){
        return;
    }
    vec2d borderPos(SelectedObject->getPos() + Camera.pos);
    vec2d borderSize(SelectedObject->getSize());
    if(SelectedObject->getIsScrollable()){
        borderSize.multiply(Camera.zoom);
        borderPos.set(Camera.translateWithZoom(borderPos));
        al_draw_rectangle(borderPos.x, borderPos.y, borderPos.x + borderSize.x, borderPos.y + borderSize.y, al_map_rgb(80, 154, 213), 6*Camera.zoom);
    }
    else{
        al_draw_rectangle(borderPos.x, borderPos.y, borderPos.x + borderSize.x, borderPos.y + borderSize.y, al_map_rgb(216, 78, 213), 6);
    }
}

AncestorObject *AncestorIndex::object(vector<LayerClass> &Layers){
    if(Layers.size() <= layerIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex(" << layerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
        return nullptr;
    }
    if(Layers[layerIndex].Objects.size() <= objectIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": objectIndex(" << objectIndex << ") goes out of scope of Layers[].Objects<" << Layers[layerIndex].Objects.size() << ">.\n";
        return nullptr;
    }
    return &Layers[layerIndex].Objects[objectIndex];
}
template <class Module>
Module * ModuleIndex::getModulePointer(vector<LayerClass> &Layers){
    if(Layers.size() <= layerIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex goes out of scope of Layers.\n";
        return nullptr;
    }
    if(Layers[layerIndex].Objects.size() <= objectIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": objectIndex goes out of scope of Layers[].Objects.\n";
        return nullptr;
    }
    if constexpr (std::is_same<Module, TextModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].TextContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].TextContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, EditableTextModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].EditableTextContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].EditableTextContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, SuperTextModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].SuperTextContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].SuperTextContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, SuperEditableTextModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].SuperEditableTextContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].SuperEditableTextContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, ImageModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].ImageContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].ImageContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, MovementModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].MovementContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].MovementContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, CollisionModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].CollisionContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].CollisionContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, ParticleEffectModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].ParticlesContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].ParticlesContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, EveModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].EveContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].EveContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, VariableModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].VariablesContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].VariablesContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, PrimitivesModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].PrimitivesContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].PrimitivesContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, VectorModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].VectorContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].VectorContainer[moduleIndex];
    }
    else{
        if(Layers[layerIndex].Objects[objectIndex].ScrollbarContainer.size() <= moduleIndex){
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].ScrollbarContainer[moduleIndex];
    }
}
ModuleIndex::ModuleIndex(unsigned layer, unsigned object, unsigned module){
    layerIndex = layer;
    objectIndex = object;
    moduleIndex = module;
}
vector<EveModule>::iterator ModuleIndex::module(vector<LayerClass> &Layers)
{
    if(Layers.size() <= layerIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex(" << layerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
        return vector<EveModule>::iterator();
    }
    if(Layers[layerIndex].Objects.size() <= objectIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": objectIndex(" << objectIndex << ") goes out of scope of Layers["
            << layerIndex << "].Objects<" << Layers[layerIndex].Objects.size() << ">.\n";
        return vector<EveModule>::iterator();
    }
    if(Layers[layerIndex].Objects[objectIndex].EveContainer.size() <= moduleIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex(" << moduleIndex
            << ") goes out of scope of Layers[" << layerIndex << "].Objects[" << objectIndex
            << "].EveContainer<" << Layers[layerIndex].Objects[objectIndex].EveContainer.size() << ">\n";
        return vector<EveModule>::iterator();
    }
    return Layers[layerIndex].Objects[objectIndex].EveContainer.begin() + moduleIndex;
}

void PointerRecalculator::clear(){
    ObjectIndexes.clear();
    CameraIndexes.clear();
    TriggeredObjectIndexes.clear();
    didActiveEditableTextExist = false;
}
void PointerRecalculator::findIndexesForCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext, Camera2D *& SelectedCamera){ 
    for(ContextClass & Context : EventContext){
        CameraIndexes.push_back(vector<unsigned>());
        for(Camera2D * Camera : Context.Cameras){
            CameraIndexes.back().push_back(Camera - &Cameras[0]);
        }
    }
    if(SelectedCamera != nullptr){
        selectedCameraIndex = SelectedCamera - &Cameras[0];
    }
}
void PointerRecalculator::findIndexesForLayers(vector<LayerClass> &Layers, vector<ContextClass> &EventContext, LayerClass *& OwnerLayer){
    for(ContextClass & Context : EventContext){
        LayerIndexes.push_back(vector<unsigned>());
        for(LayerClass * Layer : Context.Layers){
            LayerIndexes.back().push_back(Layer - &Layers[0]);
        }
    }
    if(OwnerLayer != nullptr){
        for(LayerClass & Layer : Layers){
            if(Layer.getID() == OwnerLayer->getID()){
                eventOwnerLayerIndex = &Layer - &Layers[0];
                break;
            }
        }
    }
}
void PointerRecalculator::findIndexesForObjects(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, AncestorObject *& Owner,
    vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject
){
    unsigned layerIndex;   
    for(ContextClass & Context : EventContext){
        ObjectIndexes.push_back(vector<AncestorIndex>());
        for(AncestorObject * Object : Context.Objects){
            for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
                if(Layers[layerIndex].getID() == Object->getLayerID()){
                    ObjectIndexes.back().push_back(AncestorIndex(layerIndex, Object - &Layers[layerIndex].Objects[0]));
                    break;
                }
            }
        }
    }
    for(AncestorObject * Triggered : TriggeredObjects){
        TriggeredObjectIndexes.push_back(AncestorIndex(0, 0));
        for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
            if(Layers[layerIndex].getID() == Triggered->getLayerID()){
                TriggeredObjectIndexes.back() = AncestorIndex(layerIndex, Triggered - &Layers[layerIndex].Objects[0]);
                break;
            }
        }
    }
    if(SelectedLayer != nullptr){
        selectedLayerIndex = SelectedLayer - &Layers[0];
    }
    if(Owner != nullptr){
        for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
            if(Layers[layerIndex].getID() == Owner->getLayerID()){
                EventOwnerIndex = AncestorIndex(layerIndex, Owner - &Layers[layerIndex].Objects[0]);
                break;
            }
        }
    }
    if(SelectedObject != nullptr){
        for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
            if(Layers[layerIndex].getID() == SelectedObject->getLayerID()){
                SelectedObjectIndex = AncestorIndex(layerIndex, SelectedObject - &Layers[layerIndex].Objects[0]);
                break;
            }
        }
    }
}
template <class Module>
ModuleIndex PointerRecalculator::getIndex(Module *& Instance, vector<LayerClass> & Layers, EventDescription EventIds){
    if(Instance->getLayerID() == ""){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Module instance does not belong to any layer.\n";
    }
    if(Instance->getObjectID() == ""){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Module instance does not belong to any object.\n";
    }
    if(Instance->getLayerID() == "" || Instance->getObjectID() == ""){
        return ModuleIndex(0, 0, 0);
    }
    unsigned layer, object;
    for(layer = 0; layer < Layers.size(); layer++){
        if(Layers[layer].getID() != Instance->getLayerID()){
            continue;
        }
        for(object = 0; object < Layers[layer].Objects.size(); object++){
            if(Layers[layer].Objects[object].getID() != Instance->getObjectID()){
                continue;
            }
            if constexpr (std::is_same<Module, TextModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].TextContainer[0]);
            }
            else if constexpr (std::is_same<Module, EditableTextModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].EditableTextContainer[0]);
            }
            else if constexpr (std::is_same<Module, SuperTextModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].SuperTextContainer[0]);
            }
            else if constexpr (std::is_same<Module, SuperEditableTextModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].SuperEditableTextContainer[0]);
            }
            else if constexpr (std::is_same<Module, ImageModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].ImageContainer[0]);
            }
            else if constexpr (std::is_same<Module, MovementModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].MovementContainer[0]);
            }
            else if constexpr (std::is_same<Module, CollisionModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].CollisionContainer[0]);
            }
            else if constexpr (std::is_same<Module, ParticleEffectModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].ParticlesContainer[0]);
            }
            else if constexpr (std::is_same<Module, EveModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].EveContainer[0]);
            }
            else if constexpr (std::is_same<Module, VariableModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].VariablesContainer[0]);
            }
            else if constexpr (std::is_same<Module, ScrollbarModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].ScrollbarContainer[0]);
            }
            else if constexpr (std::is_same<Module, PrimitivesModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].PrimitivesContainer[0]);
            }
            else if constexpr (std::is_same<Module, VectorModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].VectorContainer[0]);
            }
            break;
        }
        break;
    }
    return ModuleIndex(0, 0, 0);
}
template <class Module>
void PointerRecalculator::findIndexesInModule(vector<Module*> Instances, vector<LayerClass> & Layers, EventDescription EventIds){
    for(Module * Instance : Instances){
        ModuleIndexes.back().push_back(getIndex(Instance, Layers, EventIds));
    }
}
ModuleIndex PointerRecalculator::getIndex(vector<EveModule>::iterator & Instance, vector<LayerClass> & Layers, EventDescription EventIds){
    if(Instance->getLayerID() == ""){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Event instance does not belong to any layer.\n";
    }
    if(Instance->getObjectID() == ""){
        cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Event instance does not belong to any object.\n";
    }
    if(Instance->getLayerID() == "" || Instance->getObjectID() == ""){
        return ModuleIndex(0, 0, 0);
    }
    unsigned layer, object;
    for(layer = 0; layer < Layers.size(); layer++){
        if(Layers[layer].getID() != Instance->getLayerID()){
            continue;
        }
        for(object = 0; object < Layers[layer].Objects.size(); object++){
            if(Layers[layer].Objects[object].getID() == Instance->getObjectID()){
                return ModuleIndex(layer, object, Instance - Layers[layer].Objects[object].EveContainer.begin());
            }   
        }
        break;
    }
    return ModuleIndex(0, 0, 0);
}
void PointerRecalculator::findIndexesForModules(vector<LayerClass> & Layers, vector<ContextClass> & EventContext, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, SuperEditableTextModule *& ActiveEditableText, EventDescription EventIds
){
    startingEventIndex = getIndex(StartingEvent, Layers, EventIds);
    eventIndex = getIndex(Event, Layers, EventIds);
    for(MemoryStackStruct & Memory : MemoryStack){
        PastEvents.push_back(getIndex(Memory.Event, Layers, EventIds));
    }
    for(ContextClass & Context : EventContext){
        ModuleIndexes.push_back(vector<ModuleIndex>());
        if(Context.type == "text"){
            findIndexesInModule(Context.Modules.Texts, Layers, EventIds);
        }
        else if(Context.type == "editable_text"){
            findIndexesInModule(Context.Modules.EditableTexts, Layers, EventIds);
        }
        else if(Context.type == "super_text"){
            findIndexesInModule(Context.Modules.SuperTexts, Layers, EventIds);
        }
        else if(Context.type == "super_editable_text"){
            findIndexesInModule(Context.Modules.SuperEditableTexts, Layers, EventIds);
        }
        else if(Context.type == "image"){
            findIndexesInModule(Context.Modules.Images, Layers, EventIds);
        }
        else if(Context.type == "movement"){
            findIndexesInModule(Context.Modules.Movements, Layers, EventIds);
        }
        else if(Context.type == "collision"){
            findIndexesInModule(Context.Modules.Collisions, Layers, EventIds);
        }
        else if(Context.type == "particles"){
            findIndexesInModule(Context.Modules.Particles, Layers, EventIds);
        }
        else if(Context.type == "event"){
            findIndexesInModule(Context.Modules.Events, Layers, EventIds);
        }
        else if(Context.type == "variable"){
            findIndexesInModule(Context.Modules.Variables, Layers, EventIds);
        }
        else if(Context.type == "scrollbar"){
            findIndexesInModule(Context.Modules.Scrollbars, Layers, EventIds);
        }
        else if(Context.type == "primitives"){
            findIndexesInModule(Context.Modules.Primitives, Layers, EventIds);
        }
        else if(Context.type == "vector"){
            findIndexesInModule(Context.Modules.Vectors, Layers, EventIds);
        }
    }
    didActiveEditableTextExist = false;
    if(ActiveEditableText != nullptr){
        ActiveEditableTextIndex = getIndex(ActiveEditableText, Layers, EventIds);
        didActiveEditableTextExist = true;
    }
}
void PointerRecalculator::updatePointersToCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext,
    Camera2D *& SelectedCamera, string processID, string & focusedProcessID, EventDescription EventIds
){
    unsigned context, camera;
    for(context = 0; context < CameraIndexes.size(); context++){
        for(camera = 0; camera < CameraIndexes[context].size(); camera++){
            if(Cameras.size() <= CameraIndexes[context][camera]){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": CameraIndexes[context][camera] goes out of scope of Cameras.\n";
                EventContext[context].Cameras[camera] = nullptr;
                continue;
            }
            EventContext[context].Cameras[camera] = &Cameras[CameraIndexes[context][camera]];
        }
    }
    if(SelectedCamera != nullptr){
        if(Cameras.size() <= selectedCameraIndex){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": selectedCameraIndex goes out of scope of Cameras.\n";
            unfocusCameras(Cameras, SelectedCamera, processID, focusedProcessID);
            return;
        }
        focusCamera(Cameras, SelectedCamera, processID, focusedProcessID, &Cameras[selectedCameraIndex]);
    }
}
void PointerRecalculator::updatePointersToLayers(vector<LayerClass> &Layers, vector<ContextClass> &EventContext, LayerClass *& OwnerLayer, EventDescription EventIds){
    unsigned context, layer;
    for(context = 0; context < LayerIndexes.size(); context++){
        for(layer = 0; layer < LayerIndexes[context].size(); layer++){
            if(Layers.size() <= LayerIndexes[context][layer]){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": LayerIndexes[" << context
                    << "][" << layer << "]=" << LayerIndexes[context][layer]
                    << " goes out of scope of Layers<" << Layers.size()  << ">.\n";
                EventContext[context].Layers[layer] = nullptr;
                continue;
            }
            EventContext[context].Layers[layer] = &Layers[LayerIndexes[context][layer]];
        }
    }
    if(OwnerLayer != nullptr){
        OwnerLayer = getOwnerLayer(Layers);
    }
}
void PointerRecalculator::updatePointersToObjects(vector<LayerClass> &Layers, vector<ContextClass> &EventContext, AncestorObject *&Owner,
    vector<AncestorObject *> &TriggeredObjects, LayerClass *&SelectedLayer, AncestorObject *&SelectedObject, EventDescription EventIds)
{
    unsigned i;
    for(i = 0; i < EventContext.size(); i++){
        EventContext[i].Objects.clear();
        for(AncestorIndex & Index: ObjectIndexes[i]){
            EventContext[i].Objects.push_back(Index.object(Layers));
        }
    }
    for(i = 0; i < TriggeredObjects.size(); i++){
        TriggeredObjects[i] = TriggeredObjectIndexes[i].object(Layers);
    }
    if(SelectedLayer != nullptr){
        if(Layers.size() <= selectedLayerIndex){
            cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": selectedLayerIndex(" << selectedLayerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
            SelectedLayer = nullptr;
        }
        else{
            SelectedLayer = &Layers[selectedLayerIndex];
        }
    }
    if(Owner != nullptr){
        Owner = EventOwnerIndex.object(Layers);
    }
    if(SelectedObject != nullptr){
        SelectedObject = SelectedObjectIndex.object(Layers);
    }
}
void PointerRecalculator::updatePointersToModules(vector<LayerClass> & Layers, vector<ContextClass> & EventContext, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, SuperEditableTextModule *& ActiveEditableText, EventDescription EventIds
){
    StartingEvent = startingEventIndex.module(Layers);
    Event = eventIndex.module(Layers);
    for(unsigned memory = 0; memory < MemoryStack.size(); memory++){
        MemoryStack[memory].Event = PastEvents[memory].module(Layers);
    }
    unsigned context, module;
    ModuleIndex Index(0, 0, 0);
    AncestorObject * Object;
    for(context = 0; context < ModuleIndexes.size(); context++){
        for(module = 0; module < ModuleIndexes[context].size(); module++){
            Index = ModuleIndexes[context][module];

            Object = Index.object(Layers);

            if(Object == nullptr){
                cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": Object pointer is a null value.\n";
                continue;
            }

            if(EventContext[context].type == "text"){
                EventContext[context].Modules.Texts[module] = &(*Object).TextContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "editable_text"){
                EventContext[context].Modules.EditableTexts[module] = &(*Object).EditableTextContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "super_text"){
                EventContext[context].Modules.SuperTexts[module] = &(*Object).SuperTextContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "super_editable_text"){
                EventContext[context].Modules.SuperEditableTexts[module] = &(*Object).SuperEditableTextContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "image"){
                EventContext[context].Modules.Images[module] = &(*Object).ImageContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "movement"){
                EventContext[context].Modules.Movements[module] = &(*Object).MovementContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "collision"){
                EventContext[context].Modules.Collisions[module] = &(*Object).CollisionContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "particles"){
                EventContext[context].Modules.Particles[module] = &(*Object).ParticlesContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "event"){
                EventContext[context].Modules.Events[module] = &(*Object).EveContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "variable"){
                EventContext[context].Modules.Variables[module] = &(*Object).VariablesContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "scrollbar"){
                EventContext[context].Modules.Scrollbars[module] = &(*Object).ScrollbarContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "primitives"){
                EventContext[context].Modules.Primitives[module] = &(*Object).PrimitivesContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "vector"){
                EventContext[context].Modules.Vectors[module] = &(*Object).VectorContainer[Index.moduleIndex];
            }
        }
    }
    if(didActiveEditableTextExist){
        
        ActiveEditableText = ActiveEditableTextIndex.getModulePointer<SuperEditableTextModule>(Layers);
    }
}
LayerClass * PointerRecalculator::getOwnerLayer(vector <LayerClass> & Layers){
    if(Layers.size() <= eventOwnerLayerIndex){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex(" << eventOwnerLayerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
        return nullptr;
    }
    return &Layers[eventOwnerLayerIndex];
}
