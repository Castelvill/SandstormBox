#include "processClass.h"

void EventsLookupTable::clear(){
    BootTriggered.clear();
    InitTriggered.clear();
    IterationTriggered.clear();
    IdleTriggered.clear();
    TimeTriggered.clear();
    KeyPressedTriggered.clear();
    KeyPressingTriggered.clear();
    KeyReleasedTriggered.clear();
    MouseMovedTriggered.clear();
    MouseStillnessTriggered.clear();
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
        cerr << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(listOfIDs, ID);
    ID = findNewUniqueID(listOfIDs, newID);
    listOfIDs.push_back(ID);
}
void detectRecursionWithRecursion(vector<EventModule> & EventContainer, vector<string> & calledEvents, EventModule & Event){
    calledEvents.push_back(Event.getID());
    for(ChildStruct & Child : Event.Children){
        EventModule & ChildEvent = EventContainer[Child.containerIndex];
        if(isStringInVector(calledEvents, ChildEvent.getID())){
            Child.isRecursiveCall = true;
            continue;
        }
        detectRecursionWithRecursion(EventContainer, calledEvents, ChildEvent);
    }
    calledEvents.pop_back();
}
void detectRecursionInEvents(vector<EventModule> & EventContainer, const InstrDescription & CurrentInstr){
    //If a child is called by recursion, don't check its children, because you are either doing it now or you've already done it. 
    
    vector<string> calledEvents;

    // for(EventModule & Event : EventContainer){
    //     detectRecursionWithRecursion(EventContainer, calledEvents, Event);
    // }

    vector<std::pair<EventModule*, size_t>> nodeStack;

    for(EventModule & Event : EventContainer){

        //Reset the previous recursion check
        for(ChildStruct & Child : Event.Children){
            Child.isRecursiveCall = false;
        }

        calledEvents.clear();
        nodeStack.clear();

        calledEvents.push_back(Event.getID());
        nodeStack.emplace_back(&Event, 0);

        while(!nodeStack.empty()){
            auto &[currentEvent, childIndex] = nodeStack.back();

            //Backtrack if all children are processed
            if(childIndex >= currentEvent->Children.size()){
                nodeStack.pop_back();
                calledEvents.pop_back();
                continue;
            }

            ChildStruct &Child = currentEvent->Children[childIndex++];
            EventModule &ChildEvent = EventContainer[Child.containerIndex];

            //If the current event was already called on the current stack, don't check its children.
            //Why? Because you are either doing it right now (an event directly called itself) or you've already done it.
            if(isStringInVector(calledEvents, ChildEvent.getID())){
                Child.isRecursiveCall = true;
                continue;
            }

            //Go deeper and check the children of the current event
            if(!ChildEvent.Children.empty()){
                calledEvents.push_back(ChildEvent.getID());
                nodeStack.emplace_back(&ChildEvent, 0);
            }
        }
    }
}
void ProcessClass::allocateBuiltInVariables(ObjectMemoryStruct &CurrentMap, AncestorObject &Object, LayerClass &Layer){
    if(CurrentMap.topAddress > 0){
        return; //User-defined global variables will be added to the global scope during the compilation.
    }
    const bool & isReadOnly = true;
    const bool & writable = false;
    const bool & isLocal = false;
    const bool & isReference = true;

    allocateRealMemory("NULL", null_dt, isReadOnly, isLocal, isReference, builtInVarAddr::NULL_bv, 0, CurrentMap);
    if(allocateRealMemory("me", object_inst, writable, isLocal, isReference, builtInVarAddr::me_bv, 0, CurrentMap)){
        CurrentMap.MemoryMap.back().Objects.push_back(&Object);
    }
    if(allocateRealMemory("my_layer", layer_inst, writable, isLocal, isReference, builtInVarAddr::my_layer_bv, 0, CurrentMap)){
        CurrentMap.MemoryMap.back().Layers.push_back(&Layer);
    }
    CurrentMap.topAddress = 3;
}
inline bool wasMemberVariableDefined(const vector<VariableLocationStruct> & GlobalScope, const string & variableName, DataType variableType){
    for(const VariableLocationStruct & GlobalScopeVar : GlobalScope){
        if(GlobalScopeVar.name == variableName && GlobalScopeVar.type == variableType){
            return true;
        }
    }
    return false;
}
void ProcessClass::allocatePredefinedMemberParameters(ObjectMemoryStruct &CurrentMap, AncestorObject &Object){
    for(VariableModule & MemberParameter : Object.VariablesContainer){
        if(wasMemberVariableDefined(CurrentMap.MemberVarsScope, MemberParameter.getID(), variable_mod)){
            continue;
        }
        if(allocateRealMemory(MemberParameter.getID(), variable_mod, false, false, false, CurrentMap.topAddress++, 0, CurrentMap)){
            CurrentMap.MemoryMap.back().Modules.Variables.push_back(&MemberParameter);
            CurrentMap.MemoryMap.back().isPointingToMember = true;
            CurrentMap.MemoryMap.back().containerIndex = &MemberParameter - &Object.VariablesContainer[0];
        }
    }
    for(VectorModule & MemberVector : Object.VectorContainer){
        if(wasMemberVariableDefined(CurrentMap.MemberVarsScope, MemberVector.getID(), vector_mod)){
            continue;
        }
        if(allocateRealMemory(MemberVector.getID(), vector_mod, false, false, false, CurrentMap.topAddress++, 0, CurrentMap)){
            CurrentMap.MemoryMap.back().Modules.Vectors.push_back(&MemberVector);
            CurrentMap.MemoryMap.back().isPointingToMember = true;
            CurrentMap.MemoryMap.back().containerIndex = &MemberVector - &Object.VectorContainer[0];
        }
    }
}
bool ProcessClass::allocateRealMemory(const string &variableId, const DataType &variableType,
    bool readOnly, bool isLocal, bool isReference, unsigned newRealAddress,
    unsigned localIndex, ObjectMemoryStruct & CurrentMap
){
    if(newRealAddress < CurrentMap.MemoryMap.size()){
        return false;
    }
    if(newRealAddress > CurrentMap.MemoryMap.size()){
        if(newRealAddress == 0 || newRealAddress - CurrentMap.MemoryMap.size() > 100){
            cerr << "Error: You cannot allocate more than 100 new memory addresses.\n";
            return false; //???
        }
        cerr << "Warning: Allocating empty memory block between addresses: "
            << CurrentMap.MemoryMap.size() << " and " << newRealAddress-1 << ".\n";
        CurrentMap.MemoryMap.resize(newRealAddress-1);
    }
    CurrentMap.MemoryMap.emplace_back(ContextClass());
    CurrentMap.MemoryMap.back().ID = variableId;
    CurrentMap.MemoryMap.back().type = variableType;
    CurrentMap.MemoryMap.back().readOnly = readOnly;
    if(!isLocal){
        CurrentMap.MemberVarsScope.emplace_back(variableId, variableType, isLocal, isReference, localIndex, newRealAddress);
    }
    return true;
}
void ProcessClass::allocateAllLocalVariables(ObjectMemoryStruct &CurrentMap,
    const vector<EventModule> & EventContainer
){
    const bool writable = false;
    const bool isLocal = true;
    for(const EventModule & eventIt : EventContainer){
        for(unsigned localVarIdx = 0; localVarIdx < eventIt.LocalVariables.size(); ++localVarIdx){
            const VariableInfo & variableIt = eventIt.LocalVariables[localVarIdx];
            if(variableIt.isReference){
                //There's no need to allocate memory for a reference since its real address 
                //will not be used. 
                continue; 
            }
            allocateRealMemory(variableIt.name, variableIt.type, writable, isLocal,
                variableIt.isReference, variableIt.defaultAddress, localVarIdx, CurrentMap
            );
        }
    }
}
void ProcessClass::create(string EXE_PATH_FROM_ENGINE, bool allowNotAscii, vec2i screenSize,
    string initFilePath, string newID, string newLayerID, string newObjectID,
    vector<string> &listOfIDs
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
        cerr << "Error: RESERVATION_MULTIPLIER is lower than 1.\n";
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

    Layers.emplace_back(LayerClass(newLayerID, layersIDs, topLayerUniqueIndex, true,
        vec2d(0.0, 0.0), screenSize
    ));
    layersOrder.emplace_back(Layers.size() - 1);
    Layers.back().Objects.emplace_back(AncestorObject());
    Layers.back().objectsOrder.emplace_back(Layers.back().Objects.size() - 1);
    AncestorObject & InitObject = Layers.back().Objects.back();

    PrimaryData initData = {
        .topIndex = &topObjectUniqueIndex,
        .layerUniqueIndex = Layers.back().getUniqueIndex(),
        .newID = newObjectID,
        .listOfIDs = &Layers.back().objectsIDs,
        .newLayerID = Layers.back().getID()
    };
    InitObject.primaryConstructor(initData);
    
    ObjectMemoryStruct & CurrentMap = ProcessMemory[InitObject.getUniqueIndex()];
    allocateBuiltInVariables(CurrentMap, InitObject, Layers.back());
    allocatePredefinedMemberParameters(CurrentMap, InitObject);
    if(initFilePath != ""){
        // Script paths that start with a question mark are built-in scripts.
        if(initFilePath[0] == '?'){ 
            InitObject.bindedScripts.push_back(initFilePath);
        }
        else{
            InitObject.bindedScripts.push_back(EXE_PATH + initFilePath);
        }
        InitObject.translateAllScripts(EXE_PATH, true, allowNotAscii, CurrentMap.MemberVarsScope,
            CurrentMap.topAddress, topModuleUniqueIndex
        );
        allocateAllLocalVariables(CurrentMap, InitObject.EventContainer);
        InitObject.findIndexesOfEventChildren();
        detectRecursionInEvents(InitObject.EventContainer, CurrentInstr);
    }
    
    if(isLayersUniquenessViolated()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Layers id uniqueness has been violated during the initialization of the process '"
            << ID << "'!\n";
    }

    for(LayerClass & Layer : Layers){
        for(AncestorObject & obj : Layer.Objects){
            obj.createVectorsOfIds();
        }
        if(Layer.isObjectsUniquenessViolated()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Layers id uniqueness has been violeted on the start of the process!\n";
        }
    }

    updateBaseOfTriggerableObjects(nullptr);
}
void ProcessClass::clear(){
    SelectedLayer = nullptr;
    for(LayerClass & Layer : Layers){
        Layer.clear();
    }
    Layers.clear();
    layersIDs.clear();
    layersOrder.clear();
    SelectedCamera = nullptr;
    SelectedObject = nullptr;
    for(Camera2D & Camera : Cameras){
        Camera.clear();
    }
    Cameras.clear();
    camerasIDs.clear();
    camerasOrder.clear();
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

template<class Module>
void checkIndexes(const vector<Module> & container){
    for(const Module & instance : container){
        if(instance.getUniqueIndex() == 0){
            cerr << "Module '" << instance.getID() << "' has index 0\n";
            abort();
        }
        if(instance.getObjectUniqueIndex() == 0){
            cerr << "Module '" << instance.getID() << "' has object index 0\n";
            abort();
        }
        if(instance.getLayerUniqueIndex() == 0){
            cerr << "Module '" << instance.getID() << "' has layer index 0\n";
            abort();
        }
    }
}

void ProcessClass::executeIteration(EngineClass & Engine, vector<ProcessClass> & Processes){
    if(Engine.closeProgram || !isActive){
        return;
    }
    Engine.Mouse.translateAllPos(-windowPos);
    switch(Engine.event.type){
        case ALLEGRO_EVENT_TIMER:
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
            
            detectAndExecuteTriggeredEvents(Engine, Processes);

            if(Engine.closeProgram){
                return;
            }
            firstIteration = false;
            if(Engine.reboot){
                return;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            if(/*Engine.display == nullptr ||*/ !canUserInteract || Engine.focusedProcessID != getID()){
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
                if(Engine.mouseTextSelectionSkip != 0
                    && al_get_timer_count(Engine.timer) % Engine.mouseTextSelectionSkip != 0
                ){
                    break;
                }
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
    if(SelectedCamera == nullptr || isDraggingScrollbar || !Mouse.isPressed(0)
        || SelectedObject == nullptr || ActiveEditableText == nullptr || !SelectedObject->getIsActive()
    ){
        return;
    }
    if(!Mouse.pressedInRectangle(
        SelectedCamera->pos + SelectedObject->getPos(), SelectedObject->getSize(), 0,
        SelectedObject->getIsScrollable(), SelectedCamera)
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
    if(Engine.autoScaleBackbuffer){
        float destinationWidth = Engine.displaySize.x;
        float destinationHeight = Engine.displaySize.y;
        if(destinationWidth < Engine.backbufferSize.x){
            destinationWidth = Engine.backbufferSize.x;
        }
        if(destinationHeight < Engine.backbufferSize.y){
            destinationHeight = Engine.backbufferSize.y;
        }
        al_draw_scaled_bitmap(Engine.backbuffer, 0, 0, Engine.backbufferSize.x, Engine.backbufferSize.y,
            0, 0, destinationWidth, destinationHeight, 0);
    }
    else{
        al_draw_bitmap(Engine.backbuffer, 0, 0, 0);
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
void ProcessClass::updateBaseOfTriggerableObjects(Triggers * CurrentTriggers){
    BaseOfTriggerableObjects.clear();

    unsigned objectIndex;
    
    for(unsigned layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
        for(objectIndex = 0; objectIndex < Layers[layerIndex].Objects.size(); objectIndex++){
            for(const EventModule & Event : Layers[layerIndex].Objects[objectIndex].EventContainer){
                for(const TriggerType & type : Event.primaryTriggerTypes){
                    switch(type){
                        case on_boot:
                            BaseOfTriggerableObjects.BootTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_init:
                            BaseOfTriggerableObjects.InitTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case each_iteration:
                            BaseOfTriggerableObjects.IterationTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_idle:
                            BaseOfTriggerableObjects.IdleTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case each_second:
                            BaseOfTriggerableObjects.TimeTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_key_press:
                            BaseOfTriggerableObjects.KeyPressedTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_key_pressing:
                            BaseOfTriggerableObjects.KeyPressingTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_key_release:
                            BaseOfTriggerableObjects.KeyReleasedTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_mouse_move:
                            BaseOfTriggerableObjects.MouseMovedTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case when_mouse_still:
                            BaseOfTriggerableObjects.MouseStillnessTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_mouse_press:
                            BaseOfTriggerableObjects.MousePressedTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_mouse_pressing:
                            BaseOfTriggerableObjects.MousePressingTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_mouse_release:
                            BaseOfTriggerableObjects.MouseReleasedTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case by_objects:
                            BaseOfTriggerableObjects.ObjectsTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case by_variables:
                            BaseOfTriggerableObjects.VariablesTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case by_collision:
                            BaseOfTriggerableObjects.CollisionTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case by_editables:
                            BaseOfTriggerableObjects.EditableTextTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case by_movement:
                            BaseOfTriggerableObjects.MovementTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case by_stillness:
                            BaseOfTriggerableObjects.StillnessTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        case on_display_resize:
                            BaseOfTriggerableObjects.ResizeTriggered.emplace_back(AncestorIndex(layerIndex, objectIndex));
                            continue;
                        default:
                            cerr << "Error: In " << __FUNCTION__ << ": Trigger type '" << triggerToStr(type) << "' is not valid.\n";
                            continue;
                    }
                }
            }
        }
    }
}
inline bool canObjectBeTriggered(AncestorObject * Object, LayerClass * Layer){
    return Object != nullptr && Layer->getIsActive() && !Layer->getIsDeleted()
        && Object->getIsActive() && !Object->getIsDeleted();
}
inline void addTriggeredObjectById(const string & strTrigger, AncestorObject * TempObject,
    LayerClass * Layer, vector<AncestorObject*> & TriggeredObjects,
    std::unordered_map<string, std::unordered_set<string>> & consecutiveTriggers,
    const bool & printOutInstructions
){
    if(canObjectBeTriggered(TempObject, Layer)){
        if(printOutInstructions){
            cout << TempObject->getLayerID() << "::" << TempObject->getID() << " (" << strTrigger << "), ";
            consecutiveTriggers[TempObject->getID()+TempObject->getLayerID()].emplace(strTrigger);
        }
        TriggeredObjects.push_back(&(*TempObject));
    }
}
inline void addAllObjectsByCurrentTrigger(const TriggerType & trigger,
    vector<AncestorIndex> & ObjectIndexes, Triggers & CurrentTriggers,
    vector<LayerClass> & Layers, vector<AncestorObject*> & TriggeredObjects,
    std::unordered_map<string, std::unordered_set<string>> & consecutiveTriggers,
    const bool & printOutInstructions
){
    CurrentTriggers.active.insert(trigger);
    for(const AncestorIndex & Index : ObjectIndexes){
        addTriggeredObjectById(triggerToStr(trigger), Index.object(Layers), &Layers[Index.layerIndex],
            TriggeredObjects, consecutiveTriggers, printOutInstructions
        );
    }
}
inline void addAllObjectsByInitTrigger(vector<AncestorIndex> & ObjectIndexes,
    Triggers & CurrentTriggers, vector<LayerClass> & Layers,
    vector<AncestorObject*> & TriggeredObjects,
    std::unordered_map<string, std::unordered_set<string>> & consecutiveTriggers,
    const bool & printOutInstructions
){
    for(const AncestorIndex & Index : ObjectIndexes){
        AncestorObject * Object = Index.object(Layers);
        if(canObjectBeTriggered(Object, &Layers[Index.layerIndex])){
            if(printOutInstructions){
                cout << Object->getLayerID() << "::" << Object->getID() << " (on_init), ";
                consecutiveTriggers[Object->getID()+Object->getLayerID()].emplace("on_init");
            }
            CurrentTriggers.active.insert(on_init);
            TriggeredObjects.push_back(&(*Object));
        }
    }
    ObjectIndexes.clear();
}
inline void addAllObjectsByMovementTrigger(vector<AncestorIndex> & ObjectIndexes,
    Triggers & CurrentTriggers, vector<LayerClass> & Layers,
    vector<AncestorObject*> & TriggeredObjects,
    std::unordered_map<string, std::unordered_set<string>> & consecutiveTriggers,
    const bool & printOutInstructions
){
    for(const AncestorIndex & Index : ObjectIndexes){
        AncestorObject * TempObject = Index.object(Layers);
        if(!canObjectBeTriggered(TempObject, &Layers[Index.layerIndex])){
            continue;
        }
        for(const MovementModule & Movement : TempObject->MovementContainer){
            if(!Movement.getIsActive()){
                continue;
            }
            if(Movement.isMoving()){
                if(printOutInstructions){
                    cout << TempObject->getLayerID() << "::" << TempObject->getID() << " (by_movement), ";
                    consecutiveTriggers[TempObject->getID()+TempObject->getLayerID()].emplace("by_movement");
                }
                CurrentTriggers.active.insert(by_movement);
                CurrentTriggers.movingObjects.insert(TempObject->getID());
                TriggeredObjects.push_back(&(*TempObject));
                break;
            }
        }
    }
}
inline bool isObjectMoving(const vector<MovementModule> & MovementContainer){
    for(const MovementModule & movementIt : MovementContainer){
        if(movementIt.getIsActive() && movementIt.isMoving()){
            return true;
        }
    }
    return false;
}
inline void addAllObjectsByStillnessTrigger(vector<AncestorIndex> & ObjectIndexes,
    Triggers & CurrentTriggers, vector<LayerClass> & Layers,
    vector<AncestorObject*> & TriggeredObjects,
    std::unordered_map<string, std::unordered_set<string>> & consecutiveTriggers,
    const bool & printOutInstructions
){
    for(const AncestorIndex & Index : ObjectIndexes){
        AncestorObject * TempObject = Index.object(Layers);
        if(!canObjectBeTriggered(TempObject, &Layers[Index.layerIndex])
            || isObjectMoving(TempObject->MovementContainer)
        ){ continue; }
        if(printOutInstructions){
            cout << TempObject->getLayerID() << "::" << TempObject->getID() << " (by_stillness), ";
            consecutiveTriggers[TempObject->getID()+TempObject->getLayerID()].emplace("by_stillness");
        }
        CurrentTriggers.active.insert(by_stillness);
        CurrentTriggers.stillObjects.insert(TempObject->getID());
        TriggeredObjects.push_back(&(*TempObject));
    }
}
inline void removeObjectDuplicates(vector<AncestorObject*> & TriggeredObjects){
    for(unsigned int i=0; i < TriggeredObjects.size(); i++){
        for(unsigned int j=i+1; j < TriggeredObjects.size(); j++){
            if(TriggeredObjects[i] != TriggeredObjects[j]){
                continue;
            }
            TriggeredObjects.erase(TriggeredObjects.begin()+j);
            j--;
        }
    }
}
inline bool areAllEventsDisabled(const vector<AncestorObject*>::iterator & objectIt){
    for(const EventModule & Event : (*objectIt)->EventContainer){
        if(Event.getIsActive() && !Event.getIsDeleted()){
            return false;
        }
    }
    return true;
}
inline void removeObjectsWithDisabledEvents(vector<AncestorObject*> & TriggeredObjects,
    std::unordered_map<string, std::unordered_set<string>> & consecutiveTriggers,
    const bool & printOutInstructions
){
    for(auto objectIt = TriggeredObjects.begin(); objectIt != TriggeredObjects.end();){
        if(areAllEventsDisabled(objectIt)){
            objectIt = TriggeredObjects.erase(objectIt);
            continue;
        }
        if(printOutInstructions){
            cout << (*objectIt)->getLayerID() << "::" << (*objectIt)->getID() << "<";
            for(const string & triggers : consecutiveTriggers[(*objectIt)->getID()+(*objectIt)->getLayerID()]){
                cout << triggers << ", ";
            }
            cout << ">, ";
        }
        ++objectIt;
    }
}
void ProcessClass::detectTriggeredEvents(const EngineClass & Engine, vector<AncestorObject*> & TriggeredObjects, Triggers & CurrentTriggers){
    if(printOutInstructions){
        cout << "\n\n=====All triggered objects=====\n";
    }
    std::unordered_map<string, std::unordered_set<string>> consecutiveTriggers; //Used only for debugging.
    TriggeredObjects.clear();
    CurrentTriggers.clear();
    addAllObjectsByInitTrigger(BaseOfTriggerableObjects.InitTriggered,
        CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
    );
    addAllObjectsByCurrentTrigger(each_iteration, BaseOfTriggerableObjects.IterationTriggered,
        CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
    );
    if(firstIteration){
        addAllObjectsByCurrentTrigger(on_boot, BaseOfTriggerableObjects.BootTriggered,
            CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
        );
    }
    if(Engine.secondHasPassed()){
        addAllObjectsByCurrentTrigger(each_second, BaseOfTriggerableObjects.TimeTriggered,
            CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
        );
    }
    if(canUserInteract){
        if(Engine.firstPressedKeys.size() > 0){
            addAllObjectsByCurrentTrigger(on_key_press, BaseOfTriggerableObjects.KeyPressedTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(Engine.pressedKeys.size() > 0){
            addAllObjectsByCurrentTrigger(on_key_pressing, BaseOfTriggerableObjects.KeyPressingTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(Engine.releasedKeys.size() > 0){
            addAllObjectsByCurrentTrigger(on_key_release, BaseOfTriggerableObjects.KeyReleasedTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(Engine.Mouse.didMouseMove){
            addAllObjectsByCurrentTrigger(on_mouse_move, BaseOfTriggerableObjects.MouseMovedTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(!Engine.Mouse.didMouseMove){
            addAllObjectsByCurrentTrigger(when_mouse_still, BaseOfTriggerableObjects.MouseStillnessTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(Engine.Mouse.isFirstPressed()){
            addAllObjectsByCurrentTrigger(on_mouse_press, BaseOfTriggerableObjects.MousePressedTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(Engine.Mouse.isPressed()){
            addAllObjectsByCurrentTrigger(on_mouse_pressing, BaseOfTriggerableObjects.MousePressingTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
        if(Engine.Mouse.isReleased()){
            addAllObjectsByCurrentTrigger(on_mouse_release, BaseOfTriggerableObjects.MouseReleasedTriggered,
                CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
            );
        }
    }
    
    if(Engine.displayResized){
        addAllObjectsByCurrentTrigger(on_display_resize, BaseOfTriggerableObjects.ResizeTriggered,
            CurrentTriggers, Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
        );
    }

    addAllObjectsByMovementTrigger(BaseOfTriggerableObjects.MovementTriggered, CurrentTriggers,
        Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
    );

    addAllObjectsByStillnessTrigger(BaseOfTriggerableObjects.StillnessTriggered, CurrentTriggers,
        Layers, TriggeredObjects, consecutiveTriggers, printOutInstructions
    );

    if(printOutInstructions){
        cout << "\n---Filtered triggered objects---\n";
    }

    removeObjectDuplicates(TriggeredObjects);

    removeObjectsWithDisabledEvents(TriggeredObjects, consecutiveTriggers, printOutInstructions);

    if(printOutInstructions){
        cout << "\n";
    }
}
void ProcessClass::detectTriggeredByIdleEvents(const EngineClass & Engine, vector<AncestorObject*> & TriggeredObjects, Triggers & CurrentTriggers){
    if(printOutInstructions){
        cout << "\n\n=====All triggered objects=====\n";
    }

    std::unordered_map<string, std::unordered_set<string>> consecutiveTriggers; //Used only for debugging.
    TriggeredObjects.clear();
    CurrentTriggers.clear();

    for(const AncestorIndex & Index : BaseOfTriggerableObjects.IdleTriggered){
        AncestorObject * TempObject = Index.object(Layers);
        if(canObjectBeTriggered(TempObject, &Layers[Index.layerIndex])){
            if(printOutInstructions){
                cout << TempObject->getLayerID() << "::" << TempObject->getID() << " (on_idle), ";
                consecutiveTriggers[TempObject->getID()+TempObject->getLayerID()].emplace("on_idle");
            }
            CurrentTriggers.active.insert(on_idle);
            TriggeredObjects.push_back(&(*TempObject));
        }
    }

    if(printOutInstructions){
        cout << "\n---Filtered triggered objects---\n";
    }

    removeObjectDuplicates(TriggeredObjects);

    removeObjectsWithDisabledEvents(TriggeredObjects, consecutiveTriggers, printOutInstructions);

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
    type = any_dt;
    clearState();
    type = null_dt;
    isPointingToMember = false;
}
void ContextClass::clearState(){
    switch(type){
        case value_inst:
            if(!Values.empty()){
                Values[0].clear();
            }
            else{
                Values.emplace_back(VariableModule());
            }
            return;
        case value_vec:
            Values.clear();
            return;
        case pointer_inst:
        case pointer_vec:
            BasePointers.clear();
            return;
        case camera_inst:
        case camera_vec:
            Cameras.clear();
            return;
        case layer_inst:
        case layer_vec:
            Layers.clear();
            return;
        case object_inst:
        case object_vec:
            Objects.clear();
            return;
        case variable_mod:
        case variable_mod_vec:
            Modules.Variables.clear();
            return;
        case vector_mod:
        case vector_mod_vec:
            Modules.Vectors.clear();
            return;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts.clear();
            return;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts.clear();
            return;
        case image_mod:
        case image_mod_vec:
            Modules.Images.clear();
            return;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements.clear();
            return;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions.clear();
            return;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles.clear();
            return;
        case event_mod:
        case event_mod_vec:
            Modules.Events.clear();
            return;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars.clear();
            return;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives.clear();;
            return;
        case any_dt:
            Values.clear();
            BasePointers.clear();
            Cameras.clear();
            Layers.clear();
            Objects.clear();
            Modules.Variables.clear();
            Modules.Vectors.clear();
            Modules.SuperTexts.clear();
            Modules.SuperEditableTexts.clear();
            Modules.Images.clear();
            Modules.Movements.clear();
            Modules.Collisions.clear();
            Modules.Particles.clear();
            Modules.Events.clear();
            Modules.Scrollbars.clear();
            Modules.Primitives.clear();
            return;
        case null_dt:
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Type '" << dataTypeToStr(type) << "' is not valid.\n";
            type = null_dt;
            return;
    }
}
void ContextClass::clearPointers(){
    switch(type){
        case camera_inst:
        case camera_vec:
            Cameras.clear();
            return;
        case layer_inst:
        case layer_vec:
            Layers.clear();
            return;
        case object_inst:
        case object_vec:
            Objects.clear();
            return;
        case variable_mod:
        case variable_mod_vec:
            Modules.Variables.clear();
            return;
        case vector_mod:
        case vector_mod_vec:
            Modules.Vectors.clear();
            return;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts.clear();
            return;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts.clear();
            return;
        case image_mod:
        case image_mod_vec:
            Modules.Images.clear();
            return;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements.clear();
            return;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions.clear();
            return;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles.clear();
            return;
        case event_mod:
        case event_mod_vec:
            Modules.Events.clear();
            return;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars.clear();
            return;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives.clear();;
            return;
        case any_dt:
            Cameras.clear();
            Layers.clear();
            Objects.clear();
            Modules.Variables.clear();
            Modules.Vectors.clear();
            Modules.SuperTexts.clear();
            Modules.SuperEditableTexts.clear();
            Modules.Images.clear();
            Modules.Movements.clear();
            Modules.Collisions.clear();
            Modules.Particles.clear();
            Modules.Events.clear();
            Modules.Scrollbars.clear();
            Modules.Primitives.clear();
            return;
        default:
            return;
    }
}
size_t ContextClass::getVectorSize() const{
    switch(type){
        case value_inst:
        case value_vec:
            return Values.size();
        case pointer_inst:
        case pointer_vec:
            return BasePointers.size();
        case variable_mod:
        case variable_mod_vec:
            return Modules.Variables.size();
        case camera_inst:
        case camera_vec:
            return Cameras.size();
        case layer_inst:
        case layer_vec:
            return Layers.size();
        case object_inst:
        case object_vec:
            return Objects.size();
        case super_text_mod:
        case super_text_mod_vec:
            return Modules.SuperTexts.size();
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            return Modules.SuperEditableTexts.size();
        case image_mod:
        case image_mod_vec:
            return Modules.Images.size();
        case movement_mod:
        case movement_mod_vec:
            return Modules.Movements.size();
        case collision_mod:
        case collision_mod_vec:
            return Modules.Collisions.size();
        case particles_mod:
        case particles_mod_vec:
            return Modules.Particles.size();
        case event_mod:
        case event_mod_vec:
            return Modules.Events.size();
        case scrollbar_mod:
        case scrollbar_mod_vec:
            return Modules.Scrollbars.size();
        case primitives_mod:
        case primitives_mod_vec:
            return Modules.Primitives.size();
        case vector_mod:
        case vector_mod_vec:{ //Should we also add Modules.Vectors.size()? 
            size_t finalSize = 0;
            for(const VectorModule * vec : Modules.Vectors){
                if(vec != nullptr)
                    finalSize += vec->getSize();
            }
            return finalSize;}
        case any_dt:
            return Values.size() + BasePointers.size() + Modules.Variables.size()
            + Cameras.size() + Layers.size() + Objects.size()
            + Modules.SuperTexts.size() + Modules.SuperEditableTexts.size() + Modules.Images.size()
            + Modules.Movements.size() + Modules.Collisions.size()
            + Modules.Particles.size() + Modules.Events.size()
            + Modules.Scrollbars.size() + Modules.Primitives.size() + Modules.Vectors.size();
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Type '" << dataTypeToStr(type) << "' is not valid.\n";
            return 0;
    }
    return 0;
}
template <typename T>
inline string getAllIndexes(const vector<T*> & entityVec){
    string allIndexes;
    for(const T * entity : entityVec){
        if(entity != nullptr){
            allIndexes += std::to_string(entity->getUniqueIndex());
        }
        allIndexes += ", ";
    }
    return allIndexes;
}
string ContextClass::getUniqueIndexes() const{
    switch(type){
        case variable_mod:
        case variable_mod_vec:
            return getAllIndexes(Modules.Variables);
        case layer_inst:
        case layer_vec:
            return getAllIndexes(Layers);
        case object_inst:
        case object_vec:
            return getAllIndexes(Objects);
        case super_text_mod:
        case super_text_mod_vec:
            return getAllIndexes(Modules.SuperTexts);
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            return getAllIndexes(Modules.SuperEditableTexts);
        case image_mod:
        case image_mod_vec:
            return getAllIndexes(Modules.Images);
        case movement_mod:
        case movement_mod_vec:
            return getAllIndexes(Modules.Movements);
        case collision_mod:
        case collision_mod_vec:
            return getAllIndexes(Modules.Collisions);
        case particles_mod:
        case particles_mod_vec:
            return getAllIndexes(Modules.Particles);
        case event_mod:
        case event_mod_vec:
            return getAllIndexes(Modules.Events);
        case scrollbar_mod:
        case scrollbar_mod_vec:
            return getAllIndexes(Modules.Scrollbars);
        case primitives_mod:
        case primitives_mod_vec:
            return getAllIndexes(Modules.Primitives);
        case vector_mod:
        case vector_mod_vec:
            return getAllIndexes(Modules.Vectors);
        default:
            return "";
    }
}
inline string shortenText(const string & text, const int textLimit){
    string newText = text;
    if(textLimit >= 0 && text.size() > unsigned(textLimit)){
        newText = text.substr(0, textLimit);
        if(newText[0] == '\"'){
            newText += "...\"";
        }
        newText += "(+" + intToStr(text.size()-textLimit);
        newText += ")";
    }
    string squeezedText = "";
    for(char letter : newText){
        if(letter == '\n'){
            squeezedText += "\\n";
        }
        else{
            squeezedText += letter;
        }
    }
    return squeezedText;
}
string ContextClass::getValue(const InstrDescription & CurrentInstr, int maxLengthOfValuesPrinting) const{
    string buffer = "";
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                buffer += shortenText(Values[0].getAnyValue(), maxLengthOfValuesPrinting);
            }
            else{
                buffer += "null";
            }
            break;
        case value_vec:
            buffer = "[";
            for(const VariableModule & Variable: Values){
                buffer += shortenText(Variable.getAnyValue(), maxLengthOfValuesPrinting);
                buffer += ", ";
            }
            buffer += "]<";
            buffer += uIntToStr(Values.size());
            buffer += ">";
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                buffer += shortenText(BasePointers[0].getString(), maxLengthOfValuesPrinting);
                if(BasePointers[0].readOnly){
                    buffer += "(R)";
                }
            }
            else{
                buffer += "null";
            }
            break;
        case pointer_vec:
            buffer = "[";
            for(const BasePointersStruct & Pointer: BasePointers){
                buffer += shortenText(Pointer.getString(), maxLengthOfValuesPrinting);
                if(Pointer.readOnly){
                    buffer += "(R)";
                }
                buffer += ", ";
            }
            buffer += "]<";
            buffer += uIntToStr(BasePointers.size());
            buffer += ">";
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                if(Modules.Variables[0] == nullptr){
                    buffer += "nullptr";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the variable has a nullptr value.\n";
                }
                else{
                    buffer += shortenText(Modules.Variables[0]->getAnyValue(),
                        maxLengthOfValuesPrinting
                    );
                }
            }
            else{
                buffer += "null";
            }
            break;
        case variable_mod_vec:
            buffer = "[";
            for(const VariableModule * Variable: Modules.Variables){
                if(Variable == nullptr){
                    buffer += "<nullptr>, ";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the variable has a nullptr value.\n";
                    continue;
                }
                buffer += shortenText(Variable->getAnyValue(), maxLengthOfValuesPrinting);
                buffer += ", ";
            }
            buffer += "]<";
            buffer += uIntToStr(Modules.Variables.size());
            buffer += ">";
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                if(Modules.Vectors[0] == nullptr){
                    buffer += "nullptr";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the vector has a nullptr value.\n";
                }
                else{
                    buffer += "[";
                    for(size_t i = 0; i < Modules.Vectors[0]->getSize(); i++){
                        buffer += shortenText(Modules.Vectors[0]->getAnyStringValueUnsafe(i),
                            maxLengthOfValuesPrinting) + ", ";
                    }
                    buffer += "]<";
                    buffer += uIntToStr(Modules.Vectors[0]->getSize());
                    buffer += ">";
                }
            }
            else{
                buffer += "null";
            }
            break;
        case vector_mod_vec:
            buffer = "[";
            for(const VectorModule * Vector: Modules.Vectors){
                if(Vector == nullptr){
                    buffer += "<nullptr>, ";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the vector has a nullptr value.\n";
                    continue;
                }
                buffer += "[";
                for(size_t i = 0; i < Vector->getSize(); i++){
                    buffer += shortenText(Vector->getAnyStringValueUnsafe(i),
                        maxLengthOfValuesPrinting) + ", ";
                }
                buffer += "]<";
                buffer += uIntToStr(Vector->getSize());
                buffer += ">, ";
            }
            buffer += "]<";
            buffer += uIntToStr(Modules.Variables.size());
            buffer += ">";
            break;
        case null_dt:
            buffer += "null";
            break;
        default:
            buffer += "<";
            buffer += uIntToStr(getVectorSize());
            buffer += ">";
            break;
    }
    
    if(readOnly){
        buffer += "(R)";
    }

    return buffer;
}
bool ContextClass::getUnsignedOrAbort(unsigned &number, const InstrDescription & CurrentInstr){
    int temp = 0;
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Context is empty.\n";
                return false;
            }
            if(Values.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "\': Context has more than 1 value - only the last value will be used.\n";
            }
            temp = Values.back().getInt();
            break;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(BasePointers.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the last value will be used.\n";
            }
            temp = BasePointers.back().getInt();
            break;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Modules.Variables.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the last value will be used.\n";
            }
            temp = Modules.Variables.back()->getInt();
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context \'" << ID << "\' has invalid type: \'" << dataTypeToStr(type) << "\'.\n";
            return false;
    }
    if(temp < 0){
        number = 0;
        cout << instructionWarning(CurrentInstr, __FUNCTION__)
            << "Value is not unsigned - returning 0.\n";
    }
    number = temp;
    return true;
}
bool ContextClass::getIntOrAbort(int &number, const InstrDescription & CurrentInstr){
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return true;
            }
            if(Values.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            number = Values[0].getInt();
            return false;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return true;
            }
            if(BasePointers.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            number = BasePointers[0].getInt();
            return false;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return true;
            }
            if(Modules.Variables.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            number = Modules.Variables[0]->getInt();
            return false;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context \'" << ID << "\' has invalid type: \'" << dataTypeToStr(type)
                << "\'Function expects an integer or a container of integers.\n";
            return true;
    }
    return false;
}
bool ContextClass::getStringOrAbort(string & text, const InstrDescription & CurrentInstr){
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Values.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            text = Values[0].getString();
            return true;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(BasePointers.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            text = BasePointers[0].getString();
            return true;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Modules.Variables.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            text = Modules.Variables[0]->getString();
            return true;
        case vector_mod:
        case vector_mod_vec:
            if(Modules.Vectors.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Modules.Vectors.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            {
                vector<string> textVector = Modules.Vectors[0]->getAllStrings(); 
                text = "";
                for(string textIter : textVector){
                    text += textIter + "\n";
                }
            }
            text.pop_back(); //Remove the last new line.
            return true;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context \'" << ID << "\' has invalid type: '" << dataTypeToStr(type)
                << "'Function expects a string or a container of strings.\n";
            return false;
    }
    return true;
}
bool ContextClass::getStringOrIgnore(string & text, const EngineInstr & instruction){
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0 || Values[0].getType() != 's'){
                return false;
            }
            text = Values[0].getStringUnsafe();
            return true;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0 || BasePointers[0].type != string_bt){
                return false;
            }
            text = BasePointers[0].getString();
            return true;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0 || Modules.Variables[0]->getType() != 's'){
                return false;
            }
            text = Modules.Variables[0]->getStringUnsafe();
            return true;
        default:
            return false;
    }
    return true;
}
bool ContextClass::getStringVectorOrIgnore(vector<string> & result, const EngineInstr & instruction){
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                result.emplace_back(Values[0].getAnyValue());
                return true;
            }
            return false;
        case value_vec:
            for(const VariableModule & Variable : Values){
                result.emplace_back(Variable.getAnyValue());
            }
            return true;
        case pointer_inst:
            if(BasePointers.size() > 0){
                result.emplace_back(BasePointers[0].getString());
                return true;
            }
            return false;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                result.emplace_back(Pointer.getString());
            }
            return true;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                result.emplace_back(Modules.Variables[0]->getAnyValue());
                return true;
            }
            return false;
        case variable_mod_vec:
            for(VariableModule * Variable : Modules.Variables){
                result.emplace_back(Variable->getAnyValue());
            }
            return true;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getAllStrings(result);
                return true;
            }
            return false;
        case vector_mod_vec:
            for(VectorModule * Vector : Modules.Vectors){
                Vector->getAllStrings(result);
            }
            return true;
        default:
            return false;
    }
    return true;
}
ContextClass::ContextClass(){}
ContextClass::ContextClass(const ContextClass &Original){
    ID = Original.ID;
    type = Original.type;
    readOnly = Original.readOnly;
    isPointingToMember = Original.isPointingToMember;
    containerIndex = Original.containerIndex;
    switch(Original.type){
        case value_inst:
            if(Original.Values.size() == 0){
                return;
            }
            if(Values.size() == 0){
                Values.emplace_back(VariableModule());
            }
            Values[0] = Original.Values[0];
            break;
        case value_vec:
            Values = Original.Values;
            break;
        case pointer_inst:
            if(Original.BasePointers.size() == 0){
                return;
            }
            if(BasePointers.size() == 0){
                BasePointers.emplace_back(BasePointersStruct());
            }
            BasePointers[0] = Original.BasePointers[0];
            break;
        case pointer_vec:
            BasePointers = Original.BasePointers;
            break;
        case camera_inst:
            if(Original.Cameras.size() == 0){
                return;
            }
            if(Cameras.size() == 0){
                Cameras.emplace_back(nullptr);
            }
            Cameras[0] = Original.Cameras[0];
            break;
        case camera_vec:
            Cameras = Original.Cameras;
            break;
        case layer_inst:
            if(Original.Layers.size() == 0){
                return;
            }
            if(Layers.size() == 0){
                Layers.emplace_back(nullptr);
            }
            Layers[0] = Original.Layers[0];
            break;
        case layer_vec:
            Layers = Original.Layers;
            break;
        case object_inst:
            if(Original.Objects.size() == 0){
                return;
            }
            if(Objects.size() == 0){
                Objects.emplace_back(nullptr);
            }
            Objects[0] = Original.Objects[0];
            break;
        case object_vec:
            Objects = Original.Objects;
            break;
        case variable_mod:
            if(Original.Modules.Variables.size() == 0){
                return;
            }
            if(Modules.Variables.size() == 0){
                Modules.Variables.emplace_back(nullptr);
            }
            Modules.Variables[0] = Original.Modules.Variables[0];
            break;
        case variable_mod_vec:
            Modules.Variables = Original.Modules.Variables;
            break;
        case vector_mod:
            if(Original.Modules.Vectors.size() == 0){
                return;
            }
            if(Modules.Vectors.size() == 0){
                Modules.Vectors.emplace_back(nullptr);
            }
            Modules.Vectors[0] = Original.Modules.Vectors[0];
            break;
        case vector_mod_vec:
            Modules.Vectors = Original.Modules.Vectors;
            break;
        case super_text_mod:
            if(Original.Modules.SuperTexts.size() == 0){
                return;
            }
            if(Modules.SuperTexts.size() == 0){
                Modules.SuperTexts.emplace_back(nullptr);
            }
            Modules.SuperTexts[0] = Original.Modules.SuperTexts[0];
            break;
        case super_text_mod_vec:
            Modules.SuperTexts = Original.Modules.SuperTexts;
            break;
        case super_editable_text_mod:
            if(Original.Modules.SuperEditableTexts.size() == 0){
                return;
            }
            if(Modules.SuperEditableTexts.size() == 0){
                Modules.SuperEditableTexts.emplace_back(nullptr);
            }
            Modules.SuperEditableTexts[0] = Original.Modules.SuperEditableTexts[0];
            break;
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts = Original.Modules.SuperEditableTexts;
            break;
        case image_mod:
            if(Original.Modules.Images.size() == 0){
                return;
            }
            if(Modules.Images.size() == 0){
                Modules.Images.emplace_back(nullptr);
            }
            Modules.Images[0] = Original.Modules.Images[0];
            break;
        case image_mod_vec:
            Modules.Images = Original.Modules.Images;
            break;
        case movement_mod:
            if(Original.Modules.Movements.size() == 0){
                return;
            }
            if(Modules.Movements.size() == 0){
                Modules.Movements.emplace_back(nullptr);
            }
            Modules.Movements[0] = Original.Modules.Movements[0];
            break;
        case movement_mod_vec:
            Modules.Movements = Original.Modules.Movements;
            break;
        case collision_mod:
            if(Original.Modules.Collisions.size() == 0){
                return;
            }
            if(Modules.Collisions.size() == 0){
                Modules.Collisions.emplace_back(nullptr);
            }
            Modules.Collisions[0] = Original.Modules.Collisions[0];
            break;
        case collision_mod_vec:
            Modules.Collisions = Original.Modules.Collisions;
            break;
        case particles_mod:
            if(Original.Modules.Particles.size() == 0){
                return;
            }
            if(Modules.Particles.size() == 0){
                Modules.Particles.emplace_back(nullptr);
            }
            Modules.Particles[0] = Original.Modules.Particles[0];
            break;
        case particles_mod_vec:
            Modules.Particles = Original.Modules.Particles;
            break;
        case event_mod:
            if(Original.Modules.Events.size() == 0){
                return;
            }
            if(Modules.Events.size() == 0){
                Modules.Events.emplace_back(nullptr);
            }
            Modules.Events[0] = Original.Modules.Events[0];
            break;
        case event_mod_vec:
            Modules.Events = Original.Modules.Events;
            break;
        case scrollbar_mod:
            if(Original.Modules.Scrollbars.size() == 0){
                return;
            }
            if(Modules.Scrollbars.size() == 0){
                Modules.Scrollbars.emplace_back(nullptr);
            }
            Modules.Scrollbars[0] = Original.Modules.Scrollbars[0];
            break;
        case scrollbar_mod_vec:
            Modules.Scrollbars = Original.Modules.Scrollbars;
            break;
        case primitives_mod:
            if(Original.Modules.Primitives.size() == 0){
                return;
            }
            if(Modules.Primitives.size() == 0){
                Modules.Primitives.emplace_back(nullptr);
            }
            Modules.Primitives[0] = Original.Modules.Primitives[0];
            break;
        case primitives_mod_vec:
            Modules.Primitives = Original.Modules.Primitives;
            break;
        case null_dt:
            break;
        case any_dt:
            break;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": DataType code '" << type << "' is undefined.\n";
            type = null_dt;
            break;
    }
}
ContextClass &ContextClass::operator=(const ContextClass &Original){
    ID = Original.ID;
    type = Original.type;
    readOnly = Original.readOnly;
    isPointingToMember = Original.isPointingToMember;
    containerIndex = Original.containerIndex;
    switch(Original.type){
        case value_inst:
        case value_vec:
            Values = Original.Values;
            break;
        case pointer_inst:
        case pointer_vec:
            BasePointers = Original.BasePointers;
            break;
        case camera_inst:
        case camera_vec:
            Cameras = Original.Cameras;
            break;
        case layer_inst:
        case layer_vec:
            Layers = Original.Layers;
            break;
        case object_inst:
        case object_vec:
            Objects = Original.Objects;
            break;
        case variable_mod:
        case variable_mod_vec:
            Modules.Variables = Original.Modules.Variables;
            break;
        case vector_mod:
        case vector_mod_vec:
            Modules.Vectors = Original.Modules.Vectors;
            break;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts = Original.Modules.SuperTexts;
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts = Original.Modules.SuperEditableTexts;
            break;
        case image_mod:
        case image_mod_vec:
            Modules.Images = Original.Modules.Images;
            break;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements = Original.Modules.Movements;
            break;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions = Original.Modules.Collisions;
            break;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles = Original.Modules.Particles;
            break;
        case event_mod:
        case event_mod_vec:
            Modules.Events = Original.Modules.Events;
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars = Original.Modules.Scrollbars;
            break;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives = Original.Modules.Primitives;
            break;
        case null_dt:
        case any_dt:
            break;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Type '" << dataTypeToStr(Original.type) << "' is not valid.\n";
            type = null_dt;
            break;
    }
    return *this;
}
template <typename T>
void ContextClass::addBasePointer(T *pointer){
    if(pointer == nullptr){
        return;
    }
    BasePointers.emplace_back(BasePointersStruct());
    BasePointers.back().setPointer(pointer);
    type = pointer_vec;
}
template<typename T>
void ContextClass::setFirstBasePointer(T * pointer){
    if(pointer == nullptr){
        return;
    }
    if(BasePointers.size() == 0){
        BasePointers.emplace_back(BasePointersStruct());
        type = pointer_inst;
    }
    BasePointers.back().setPointer(pointer);
}
void ContextClass::updateType(size_t vecSize){
    if(vecSize > 1){
        type = vectorizeEntityDataType(InstrDescription(), type);
    }
}
void ContextClass::addModule(SuperTextModule *Module){
    if(Module != nullptr){
        Modules.SuperTexts.push_back(Module);
        updateType(Modules.SuperTexts.size());
    }
}
void ContextClass::addModule(SuperEditableTextModule *Module){
    if(Module != nullptr){
        Modules.SuperEditableTexts.push_back(Module);
        updateType(Modules.SuperEditableTexts.size());
    }
}
void ContextClass::addModule(ImageModule *Module){
    if(Module != nullptr){
        Modules.Images.push_back(Module);
        updateType(Modules.Images.size());
        
    }
}
void ContextClass::addModule(MovementModule * Module){
    if(Module != nullptr){
        Modules.Movements.push_back(Module);
        updateType(Modules.Movements.size());
    }
}
void ContextClass::addModule(CollisionModule * Module){
    if(Module != nullptr){
        Modules.Collisions.push_back(Module);
        updateType(Modules.Collisions.size());
    }
}
void ContextClass::addModule(ParticleEffectModule * Module){
    if(Module != nullptr){
        Modules.Particles.push_back(Module);
        updateType(Modules.Particles.size());
    }
}
void ContextClass::addModule(EventModule * Module){
    if(Module != nullptr){
        Modules.Events.push_back(Module);
        updateType(Modules.Events.size());
    }
}
void ContextClass::addModule(VariableModule * Module){
    if(Module != nullptr){
        Modules.Variables.push_back(Module);
        updateType(Modules.Variables.size());
    }
}
void ContextClass::addModule(ScrollbarModule * Module){
    if(Module != nullptr){
        Modules.Scrollbars.push_back(Module);
        updateType(Modules.Scrollbars.size());
    }
}
void ContextClass::addModule(PrimitivesModule * Module){
    if(Module != nullptr){
        Modules.Primitives.push_back(Module);
        updateType(Modules.Primitives.size());
    }
}
void ContextClass::addModule(VectorModule * Module){
    if(Module != nullptr){
        Modules.Vectors.push_back(Module);
        updateType(Modules.Vectors.size());
    }
}
bool translateIndexToTreeRoots(const vector<ParameterStruct> & Parameters, unsigned index, unsigned & realIndex){
    unsigned rootIndex = 0;
    for(realIndex = 0; realIndex < Parameters.size(); ++realIndex){
        if(Parameters[realIndex].treeLevel != 0){
            continue;
        }
        if(rootIndex == index){
            return false;
        }
        ++rootIndex;
    }
    return true;
}
ContextClass * getVariableByAddress(const InstrDescription & CurrentInstr,
    MemoryMapType & MemoryMap, const DynamicVariableInfo & Variable,
    const string & variableName, const bool & printError
){
    if(Variable.dynamicAddress > MemoryMap.size()){
        if(printError){
            if(!variableName.empty()){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Address " << Variable.dynamicAddress <<" is not allocated.\n";
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Variable '" << variableName << "' does not exist at the address: "
                    << Variable.dynamicAddress <<".\n";
            }
        }
        return nullptr;
    }
    return &MemoryMap[Variable.dynamicAddress];
    
    // if(Variable.isReference){
    //     if(!ObjectMemory.ReferencesMap.contains(Variable.dynamicAddress)){
    //         printError && cerr << instructionError(CurrentInstr, __FUNCTION__) << "Reference '" << variableName << "' does not exist.\n";
    //         return nullptr;
    //     }
    //     if(!ObjectMemory.MemoryMap.contains(ObjectMemory.ReferencesMap[Variable.dynamicAddress].realAddress)){
    //         printError && cerr << instructionError(CurrentInstr, __FUNCTION__)
    //             << "Variable '" << ObjectMemory.ReferencesMap[Variable.dynamicAddress].name
    //             << "' from reference '" << variableName << "' does not exist.\n";
    //         return nullptr;
    //     }
    //     return &ObjectMemory.MemoryMap[ObjectMemory.ReferencesMap[Variable.dynamicAddress].realAddress];
    // }
    // if(!ObjectMemory.MemoryMap.contains(Variable.dynamicAddress)){
    //     printError && cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << variableName << "' does not exist.\n";
    //     return nullptr;
    // }
    // return &ObjectMemory.MemoryMap[Variable.dynamicAddress];
}
bool ContextClass::copyFromTheParameter(
    std::vector<ContextClass> & MemoryMap, const vector<DynamicVariableInfo> & EventLocalVariables,
    const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    bool & negateAfterCopy, unsigned index, bool printErrors
){
    clear();
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " does not exist.\n";
        return true;
    }
    const ParameterStruct & CurrentParameter = Parameters[realIndex];
    if(CurrentParameter.type == 'e'){
        return false;
    }
    if(CurrentParameter.type == 'c'){
        if(EventLocalVariables.empty()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Local variables container is empty. This may cause undefined behavior.\n";
            return true;
        }
        ContextClass * TempContext = getVariableByAddress(CurrentInstr, MemoryMap,
            EventLocalVariables[CurrentParameter.localAddress],
            CurrentParameter.variableID, printErrors
        );
        if(TempContext == nullptr){ 
            printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Variable '" << CurrentParameter.variableID << "' from the parameter "
                << index + 2 << " does not exist.\n";
            return true;
        }
        *this = *TempContext;
        //Can't negate a value in a possible pointer since it's not temporary.
        //Also remember that lvalues/outputs can't be negated from obvious reasons.
        negateAfterCopy = CurrentParameter.negateVariable;
    }
    else if(CurrentParameter.type == 'l'){
        type = value_inst;
        Values.push_back(CurrentParameter.Literal);
        if(CurrentParameter.negateVariable)
            Values.back().negate();
    }
    else if(CurrentParameter.type == 'v'){
        type = value_vec;
        for(++realIndex; realIndex < Parameters.size(); ++realIndex){
            const ParameterStruct & parameterIt = Parameters[realIndex];
            if(parameterIt.treeLevel != 1){
                return false;
            }
            if(parameterIt.type == 'l'){
                Values.push_back(parameterIt.Literal);
                if(parameterIt.negateVariable)
                    Values.back().negate();
                continue;
            }
            if(parameterIt.type != 'c'){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "In the parameter " << index+2 << ": Value with index " << realIndex << " is of '" << parameterIt.type << "' type.\n";
                return true;
            }

            ContextClass * TempContext = getVariableByAddress(CurrentInstr, MemoryMap,
                EventLocalVariables[parameterIt.localAddress],
                parameterIt.variableID, printErrors
            );
            if(TempContext == nullptr){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Variable '" << parameterIt.variableID
                    << "' from the parameter " << index+2 << " does not exist.\n";
                return true;
            }
            switch(TempContext->type){
                case value_inst:
                    if(TempContext->Values.size() > 0){
                        Values.push_back(TempContext->Values[0]);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case value_vec:
                    if(!parameterIt.negateVariable){
                        Values.insert(Values.end(), TempContext->Values.begin(),
                            TempContext->Values.end()
                        );
                    }
                    else{
                        for(const VariableModule & var : TempContext->Values){
                            Values.push_back(var);
                            Values.back().negate();
                        }
                    }
                    break;
                case pointer_inst:
                    if(!TempContext->BasePointers.empty()){
                        Values.emplace_back(VariableModule());
                        Values.back().move(&TempContext->BasePointers[0], CurrentInstr.instruction, CurrentInstr);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case pointer_vec:
                    for(const BasePointersStruct & Pointer : TempContext->BasePointers){
                        Values.emplace_back(VariableModule());
                        Values.back().move(&Pointer, CurrentInstr.instruction, CurrentInstr);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case variable_mod:
                    if(TempContext->Modules.Variables.size() > 0){
                        Values.emplace_back(VariableModule());
                        Values.back().copyValue(TempContext->Modules.Variables[0]);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case variable_mod_vec:
                    for(const VariableModule * Variable : TempContext->Modules.Variables){
                        Values.emplace_back(VariableModule());
                        Values.back().copyValue(Variable);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case vector_mod:
                    for(VectorModule * Vector: TempContext->Modules.Vectors){
                        Vector->getValuesIntoContext(Values, parameterIt.negateVariable);
                    }
                    break;
                default:
                    printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Context with ID '" << parameterIt.variableID << "' found in the parameter "
                        << index << " is of the invalid type: '" << dataTypeToStr(TempContext->type)
                        << "'. Function expects only numeric and string values.\n";
                    return true;
            }
        }
    }
    else{
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " has invalid type: '" << CurrentParameter.type << "'.\n";
        return true;
    }
    return false;
}
std::pair<vector<ContextClass*>, bool> getAllVariablesPointerFromParameters(
    std::vector<ContextClass> & MemoryMap, const vector<DynamicVariableInfo> & EventLocalVariables,
    const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    const unsigned & index, const bool & printErrors
){
    vector<ContextClass*> GatheredVariables;
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " does not exist.\n";
        return {vector<ContextClass*>(), true};
    }
    const ParameterStruct & CurrentParameter = Parameters[realIndex];
    if(CurrentParameter.type == 'e'){
        return {GatheredVariables, false};
    }
    if(CurrentParameter.type == 'c'){
        ContextClass * TempContext = getVariableByAddress(CurrentInstr, MemoryMap,
            EventLocalVariables[CurrentParameter.localAddress],
            CurrentParameter.variableID, printErrors
        );
        if(TempContext == nullptr){ 
            printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Variable '" << CurrentParameter.variableID << "' from the parameter " << index+2 << " does not exist.\n";
            return {vector<ContextClass*>(), true};
        }
        GatheredVariables.push_back(TempContext);
        return {GatheredVariables, false};
    }
    else if(CurrentParameter.type == 'l'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " contains a literal instead of a variable.\n";
        return {vector<ContextClass*>(), true};
    }
    else if(CurrentParameter.type == 'v'){
        for(++realIndex; realIndex < Parameters.size(); ++realIndex){
            const ParameterStruct & parameterIt = Parameters[realIndex];
            if(parameterIt.treeLevel != 1){
                return {GatheredVariables, false};
            }
            if(parameterIt.type == 'l'){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Parameter " << realIndex << " contains a literal instead of a variable.\n";
                return {vector<ContextClass*>(), true};
            }
            if(parameterIt.type != 'c'){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "In the parameter " << index+2 << ": Value with index " << realIndex << " is of '" << parameterIt.type << "' type.\n";
                return {vector<ContextClass*>(), true};
            }

            ContextClass * TempContext = getVariableByAddress(CurrentInstr, MemoryMap,
                EventLocalVariables[parameterIt.localAddress],
                parameterIt.variableID, printErrors
            );
            if(TempContext == nullptr){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Variable '" << parameterIt.variableID
                    << "' from the parameter " << index+2 << " does not exist.\n";
                return {vector<ContextClass*>(), true};
            }
            GatheredVariables.push_back(TempContext);
        }
        return {GatheredVariables, false};
    }
    else{
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " has invalid type: '" << CurrentParameter.type << "'.\n";
        return {vector<ContextClass*>(), true};
    }
    return {GatheredVariables, false};
}
void ContextClass::copyOnlyCurrentType(const ContextClass *Original){
    type = Original->type;
    switch(Original->type){
        case value_inst:
            if(Original->Values.empty()){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(Values.empty()){
                Values.push_back(Original->Values[0]);
            }
            else{
                Values[0] = Original->Values[0];
            }
            break;
        case value_vec:
            Values = Original->Values;
        case pointer_inst:
            if(Original->BasePointers.size() == 0){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(BasePointers.size() == 0){
                BasePointers.emplace_back(BasePointersStruct());
            }
            BasePointers[0] = Original->BasePointers[0];
            break;
        case pointer_vec:
            BasePointers = Original->BasePointers;
        case variable_mod:
            if(Original->Modules.Variables.size() == 0){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(Modules.Variables.size() == 0){
                Modules.Variables.emplace_back(nullptr);
            }
            Modules.Variables[0] = Original->Modules.Variables[0];
            break;
        case variable_mod_vec:
            Modules.Variables = Original->Modules.Variables;
        case vector_mod:
            if(Original->Modules.Vectors.size() == 0){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(Modules.Vectors.size() == 0){
                Modules.Vectors.emplace_back(nullptr);
            }
            Modules.Vectors[0] = Original->Modules.Vectors[0];
            break;
        case vector_mod_vec:
            Modules.Vectors = Original->Modules.Vectors;
        case camera_inst:
        case camera_vec:
            Cameras = Original->Cameras;
            break;
        case layer_inst:
        case layer_vec:
            Layers = Original->Layers;
            break;
        case object_inst:
        case object_vec:
            Objects = Original->Objects;
            break;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts = Original->Modules.SuperTexts;
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts = Original->Modules.SuperEditableTexts;
            break;
        case image_mod:
        case image_mod_vec:
            Modules.Images = Original->Modules.Images;
            break;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements = Original->Modules.Movements;
            break;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions = Original->Modules.Collisions;
            break;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles = Original->Modules.Particles;
            break;
        case event_mod:
        case event_mod_vec:
            Modules.Events = Original->Modules.Events;
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars = Original->Modules.Scrollbars;
            break;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives = Original->Modules.Primitives;
            break;
        default:
            type = null_dt;
    }
}
bool getContextPointerFromTheParameter(ContextClass *&NewContext, ObjectMemoryStruct & ObjectMemory,
    vector<DynamicVariableInfo> & EventLocalVariables, const InstrDescription &CurrentInstr,
    const vector<ParameterStruct> &Parameters, const unsigned & index, const bool & printErrors
){
    NewContext = nullptr;
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " does not exist.\n";
        return true;
    }
    const ParameterStruct & CurrentParameter = Parameters[realIndex];
    if(CurrentParameter.type != 'c'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " is of '"
            << CurrentParameter.type << "' type. Provide a variable instead.\n";
        return true;
    }
    if(EventLocalVariables.empty()){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Local variables container is empty. This may cause undefined behavior.\n";
        return true;
    }
    NewContext = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        EventLocalVariables[CurrentParameter.localAddress],
        CurrentParameter.variableID, printErrors
    );
    if(NewContext == nullptr){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Variable '" << CurrentParameter.variableID
            << "' from the parameter " << index+1 << " does not exist.\n";
        return true;
    }
    return false;
}
void ContextClass::leaveOneRandomBasePointer(){
    if(BasePointers.size() == 0){
        return;
    }
    BasePointersStruct randomPointer = BasePointers[rand() % BasePointers.size()];
    BasePointers.clear();
    BasePointers.push_back(randomPointer);
}
void ContextClass::printOutObjects(){
    if(Objects.size() > 1){
        cout << " [";
        for(AncestorObject * Object : Objects){
            cout << Object->getID() << ", ";
        }
        cout << "] ";
    }
    else if(Objects.size() == 1){
        cout << Objects.back()->getID() << " ";
    }
}
unsigned ContextClass::size() const{
    return Cameras.size() + Layers.size() + Objects.size() + Modules.size() + Values.size() + BasePointers.size();
}
bool ContextClass::empty() const{
    return Cameras.empty() && Layers.empty() && Objects.empty() && Modules.empty() && Values.empty() && BasePointers.empty();
}
ReturnType ContextClass::getAllValues(vector<VariableModule> & NewValues, bool negate) const {
    if(negate){
        return getAllNegatedValues(NewValues);
    }
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Values[0]);
            }
            break;
        case value_vec:
            NewValues.insert(NewValues.end(), Values.begin(), Values.end());
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                NewValues.emplace_back();
                NewValues.back().setValueFromPointer(BasePointers[0]);
            }
            break;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                NewValues.emplace_back(VariableModule());
                NewValues.back().setValueFromPointer(Pointer);
            }
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            for(const VariableModule * Variable : Modules.Variables){
                NewValues.push_back(*Variable);
            }
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(NewValues);
            }
            break;
        case vector_mod_vec:
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(NewValues);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getAllNegatedValues(vector<VariableModule> & NewValues) const {
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Values[0]);
                NewValues.back().negate();
            }
            break;
        case value_vec:
            for(const VariableModule & var : Values){
                NewValues.push_back(var);
                NewValues.back().negate();
            }
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                NewValues.emplace_back();
                NewValues.back().setValueFromPointer(BasePointers[0]);
                NewValues.back().negate();
            }
            break;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                NewValues.emplace_back(VariableModule());
                NewValues.back().setValueFromPointer(Pointer);
                NewValues.back().negate();
            }
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Modules.Variables[0]);
                NewValues.back().negate();
            }
            break;
        case variable_mod_vec:
            for(const VariableModule * Variable : Modules.Variables){
                NewValues.push_back(*Variable);
                NewValues.back().negate();
            }
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(NewValues, true);
            }
            break;
        case vector_mod_vec:
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(NewValues, true);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::moveAllValues(vector<VariableModule> & NewValues, bool negate, unsigned realIndex) const {
    if(negate){
        return getAllNegatedValues(NewValues);
    }
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Values[0]);
            }
            break;
        case value_vec:
            NewValues.insert(NewValues.end(), Values.begin(), Values.end());
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                NewValues.emplace_back();
                NewValues.back().setValueFromPointer(BasePointers[0]);
            }
            break;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                NewValues.emplace_back(VariableModule());
                NewValues.back().setValueFromPointer(Pointer);
            }
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            for(const VariableModule * Variable : Modules.Variables){
                NewValues.push_back(*Variable);
            }
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(NewValues);
            }
            break;
        case vector_mod_vec:
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(NewValues);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getDoubleValues(vector<double> & NewValues, bool negate) const {
    if(negate){
        return getNegatedDoubleValues(NewValues);
    }
    switch(type){
        case value_inst:
        case value_vec:
            for(const VariableModule & val : Values){
                NewValues.push_back(val.getDouble());
            }
            break;
        case pointer_inst:
        case pointer_vec:
            for(const BasePointersStruct & ptr : BasePointers){
                NewValues.push_back(ptr.getDouble());
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            for(const VariableModule * var : Modules.Variables){
                NewValues.push_back(var->getDouble());
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            for(const VectorModule * vec : Modules.Vectors){
                vec->getDoubles(NewValues);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getNegatedDoubleValues(vector<double> & NewValues) const {
    switch(type){
        case value_inst:
        case value_vec:
            for(const VariableModule & val : Values){
                NewValues.push_back(-val.getDouble());
            }
            break;
        case pointer_inst:
        case pointer_vec:
            for(const BasePointersStruct & ptr : BasePointers){
                NewValues.push_back(-ptr.getDouble());
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            for(const VariableModule * var : Modules.Variables){
                NewValues.push_back(-var->getDouble());
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            for(const VectorModule * vec : Modules.Vectors){
                vec->getDoubles(NewValues, true);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getValue(VariableModule & NewValue){
    NewValue.clear();
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                return ReturnType::EMPTY;
            }
            NewValue.copyValue(Values[0]);
            return ReturnType::OK;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                return ReturnType::EMPTY;
            }
            NewValue.setValueFromPointer(BasePointers[0]);
            break;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                return ReturnType::EMPTY;
            }
            NewValue.copyValue(Modules.Variables[0]);
            return ReturnType::OK;
        case vector_mod:
        case vector_mod_vec:{
            if(Modules.Vectors.size() == 0){
                return ReturnType::EMPTY;
            }
            if(Modules.Vectors[0]->getSize() == 0){
                return ReturnType::EMPTY;
            }
            if(Modules.Vectors[0]->setVariableWithFirstValue(NewValue)){
                return ReturnType::EMPTY;
            }
            return ReturnType::OK;
        }
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::collapseToValue(){
    switch(type){
        case value_inst:
        case value_vec:
            return ReturnType::OK;
        case pointer_inst:
            type = DataType::value_inst;
            Values.clear();
            Values.emplace_back();
            if(!BasePointers.empty()){
                Values.back().setValueFromPointer(BasePointers[0]);
            }
            break;
        case pointer_vec:
            type = DataType::value_vec;
            Values.clear();
            for(const BasePointersStruct & Pointer : BasePointers){
                Values.emplace_back();
                Values.back().setValueFromPointer(Pointer);
            }
            break;
        case variable_mod:
            type = DataType::value_inst;
            Values.clear();
            Values.emplace_back();
            if(Modules.Variables.size() > 0){
                Values.back().copyValue(Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            type = DataType::value_vec;
            Values.clear();
            for(const VariableModule * Variable : Modules.Variables){
                Values.push_back(*Variable);
            }
            break;
        case vector_mod:
            type = DataType::value_inst;
            Values.clear();
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(Values);
            }
            break;
        case vector_mod_vec:
            type = DataType::value_vec;
            Values.clear();
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(Values, true);
            }
            break;
        default:
            type = DataType::value_inst;
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
bool getValuesFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & EventLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, vector<VariableModule> & NewValues, bool printErrors
){
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " does not exist.\n";
        return true;
    }

    const ParameterStruct & CurrentParameter = Parameters[realIndex];
    
    if(CurrentParameter.type == 'e'){
        return false;
    }
    if(CurrentParameter.type == 'l'){
        if(NewValues.size() <= realIndex)
            NewValues.push_back(CurrentParameter.Literal);
        else
            NewValues[realIndex].copyValue(CurrentParameter.Literal);
        if(CurrentParameter.negateVariable)
            NewValues[realIndex].negate();
        return false;
    }
    else if(CurrentParameter.type != 'c' && CurrentParameter.type != 'v'){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " has an invalid type: " << CurrentParameter.type << ".\n";
        }
        return true;
    }

    //Parameter is a variable or a vector
    bool negateWholeContextAfterCopy = false;
    HelpContext.clear();
    if(HelpContext.copyFromTheParameter(ObjectMemory.MemoryMap, EventLocalVariables, CurrentInstr,
        Parameters, negateWholeContextAfterCopy, index, printErrors
    )){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Failed to find context in the parameter " << index+1 << ".\n";
        }
        return true;
    }

    ReturnType response = HelpContext.moveAllValues(NewValues, negateWholeContextAfterCopy, realIndex);
    if(response == ReturnType::INVALID_TYPE){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " has an invalid type: " << dataTypeToStr(HelpContext.type) << ".\n";
        return true;
    }
    return false;
}
bool getDoubleVectorFromTheParameterV2(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & EventLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, vector<double> & NewValues, bool printErrors
){
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " does not exist.\n";
        return true;
    }

    const ParameterStruct & CurrentParameter = Parameters[realIndex];
    
    if(CurrentParameter.type == 'e'){
        return false;
    }
    if(CurrentParameter.type == 'l'){
        NewValues.push_back(CurrentParameter.Literal.getDouble());
        if(CurrentParameter.negateVariable)
            NewValues.back() *= -1;
        return false;
    }
    else if(CurrentParameter.type != 'c' && CurrentParameter.type != 'v'){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " has an invalid type: " << CurrentParameter.type << ".\n";
        }
        return true;
    }

    //Parameter is a variable or a vector
    bool negateWholeContextAfterCopy = false;
    HelpContext.clear();
    if(HelpContext.copyFromTheParameter(ObjectMemory.MemoryMap, EventLocalVariables, CurrentInstr,
        Parameters, negateWholeContextAfterCopy, index, printErrors
    )){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Failed to find context in the parameter " << index+1 << ".\n";
        }
        return true;
    }

    ReturnType response = HelpContext.getDoubleValues(NewValues, negateWholeContextAfterCopy);
    if(response == ReturnType::INVALID_TYPE){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " has an invalid type: " << dataTypeToStr(HelpContext.type) << ".\n";
        return true;
    }
    return false;
}
bool getSingleValueFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & EventLocalVariables, 
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, VariableModule & NewValue, bool printErrors
){
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " does not exist.\n";
        return true;
    }

    const ParameterStruct & CurrentParameter = Parameters[realIndex];

    if(CurrentParameter.type == 'e'){
        return false;
    }
    if(CurrentParameter.type == 'l'){
        NewValue.copyValue(CurrentParameter.Literal);
        return false;
    }
    else if(CurrentParameter.type != 'c' && CurrentParameter.type != 'v'){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " has an invalid type: " << CurrentParameter.type << ".\n";
        }
        return true;
    }

    bool negateWholeContextAfterCopy = false;
    HelpContext.clear();
    //Parameter is a variable or a vector
    if(HelpContext.copyFromTheParameter(ObjectMemory.MemoryMap, EventLocalVariables,
        CurrentInstr, Parameters, negateWholeContextAfterCopy, index, true
    )){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Failed to find context in the parameter " << index+1 << ".\n";
        }
        return true;
    }

    ReturnType response = HelpContext.getValue(NewValue);
    if(response == ReturnType::INVALID_TYPE){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " has an invalid type: " << dataTypeToStr(HelpContext.type) << ".\n";
        return true;
    }
    if(response == ReturnType::EMPTY){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " is empty.\n";
        return true;
    }
    if(negateWholeContextAfterCopy)
        NewValue.negate();
    return false;
}
bool getValueFromParameter(ObjectMemoryStruct & ObjectMemory, vector<DynamicVariableInfo> & DynamicLocalVariables, ContextClass & HelpContext,
    const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters, unsigned index, VariableModule & NewValue, bool printErrors
){
    vector<VariableModule> Literals;
    if(getValuesFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literals, printErrors)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    if(Literals.size() == 0){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No string value in the parameter " << index+1 << ".\n";
        return true;
    }
    NewValue.copyValue(Literals[0]);
    
    return false;
}
bool getUnsignedVectorFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, vector<unsigned> & NewUnsignedIntegers
){
    vector<VariableModule> Literals;
    if(getValuesFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literals, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get value from the parameter " << index+1 << ".\n";
        return true;
    }
    for(const VariableModule & Integer : Literals){
        if(Integer.getType() != 'i'){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Type " << Integer.getType() << "' is invalid in the parameter " << index+1 << ". Integer was expected.\n";
            return true;
        }
        NewUnsignedIntegers.emplace_back(Integer.getInt());
    }
    return false;
}
bool getDoubleVectorFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, vector<double> & newDoubles
){
    vector<VariableModule> Literals;
    if(getValuesFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literals, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get value from the parameter " << index+1 << ".\n";
        return true;
    }
    for(const VariableModule & val : Literals){
        // if(val.getType() != 'd'){
        //     cerr << instructionError(CurrentInstr, __FUNCTION__)
        //         << "Type " << val.getType() << "' is invalid in the parameter " << index+1 << ". Double was expected.\n";
        //     return true;
        // }
        newDoubles.emplace_back(val.vDouble);
    }
    return false;
}
bool getStringFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters, unsigned index,
    string & newString, bool printErrors
){
    VariableModule Literal;
    if(getSingleValueFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literal, printErrors)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    if(Literal.getType() != 's'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No string value in the parameter " << index+1 << ".\n";
        return true;
    }
    newString = Literal.vString;
    return false;
}
bool getStringVectorFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables, 
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, vector<string> & NewStrings, bool printErrors
){
    vector<VariableModule> Literals;
    if(getValuesFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literals, printErrors)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    for(const VariableModule & StringValue : Literals){
        if(StringValue.getType() != 's'){
            printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Type " << StringValue.getType() << "' is invalid in the parameter " << index+1 << ". String was expected.\n";
            return true;
        }
        NewStrings.emplace_back(StringValue.getString());
    }
    return false;
}
bool getBoolFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables, ContextClass & HelpContext,
    const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters, unsigned index, bool & newBool, bool printErrors
){
    VariableModule Literal;
    if(getSingleValueFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literal, printErrors)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    if(Literal.getType() != 'b' && Literal.getType() != 'i'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No bool value in the parameter " << index+1 << ".\n";
        return true;
    }
    newBool = Literal.getBoolUnsafe();
    return false;
}
bool getUnsignedFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables,
    ContextClass & HelpContext, VariableModule & Literal, const InstrDescription & CurrentInstr,
    const vector<ParameterStruct> & Parameters, unsigned index, unsigned & newUnsigned, bool printErrors
){
    Literal.clear();
    if(getSingleValueFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literal, printErrors)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    if(Literal.getType() != 'b' && Literal.getType() != 'i'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No bool value in the parameter " << index+1 << ".\n";
        return true;
    }
    int newInt = Literal.getIntUnsafe();
    if(newInt < 0){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Integer " << newInt << " is not usigned in the parameter " << index+1 << ".\n";
        return true;
    }
    newUnsigned = newInt;
    return false;
}
bool getUnsignedFromTheParameterOptimized(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & EventLocalVariables,
    const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, unsigned & newUnsigned, bool printErrors
){
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " does not exist.\n";
        return true;
    }

    const ParameterStruct & CurrentParameter = Parameters[realIndex];

    if(CurrentParameter.type == 'e'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+1 << " is empty.\n";
        return true;
    }
    if(CurrentParameter.type == 'l'){
        if(CurrentParameter.Literal.type != 'b' && CurrentParameter.Literal.type != 'i'){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " is not an integer.\n";
            return true;
        }
        int newInt = CurrentParameter.Literal.getIntUnsafe();
        if(newInt < 0){
            printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Integer " << newInt << " is not usigned in the parameter " << index+1 << ".\n";
            return true;
        }
        newUnsigned = newInt;
        return false;
    }
    else if(CurrentParameter.type != 'c'){
        if(printErrors){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter " << index+1 << " has an invalid type: " << CurrentParameter.type << ".\n";
        }
        return true;
    }

    //Parameter is a variable or a vector
    ContextClass * HelpContext = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        EventLocalVariables[CurrentParameter.localAddress],
        CurrentParameter.variableID, printErrors
    );
    if(HelpContext == nullptr){ 
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Variable '" << CurrentParameter.variableID << "' from the parameter " << index+2 << " does not exist.\n";
        return true;
    }

    int newInt = 0;
    switch(HelpContext->type){
        case value_inst:
        case value_vec:
            if(HelpContext->Values.size() == 0){
                return true;
            }
            if(HelpContext->Values[0].type != 'b' && HelpContext->Values[0].type != 'i'){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Parameter " << index+1 << " is not an integer.\n";
                return true;
            }
            newInt = HelpContext->Values[0].getIntUnsafe();
            if(newInt < 0){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Integer " << newInt << " is not usigned in the parameter " << index+1 << ".\n";
                return true;
            }
            newUnsigned = newInt;
            return false;
        case pointer_inst:
        case pointer_vec:
            if(HelpContext->BasePointers.size() == 0){
                return true;
            }
            newInt = HelpContext->BasePointers[0].getInt();
            if(newInt < 0){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Integer " << newInt << " is not usigned in the parameter " << index+1 << ".\n";
                return true;
            }
            newUnsigned = newInt;
            return false;
        case variable_mod:
        case variable_mod_vec:
            if(HelpContext->Modules.Variables.size() == 0){
                return true;
            }
            if(HelpContext->Modules.Variables[0]->type != 'b' && HelpContext->Modules.Variables[0]->type != 'i'){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Parameter " << index+1 << " is not an integer.\n";
                return true;
            }
            newInt = HelpContext->Modules.Variables[0]->getIntUnsafe();
            if(newInt < 0){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Integer " << newInt << " is not usigned in the parameter " << index+1 << ".\n";
                return true;
            }
            newUnsigned = newInt;
            return false;
        case vector_mod:
        case vector_mod_vec:{
            if(HelpContext->Modules.Vectors.size() == 0){
                return true;
            }
            if(HelpContext->Modules.Vectors[0]->getSize() == 0){
                return true;
            }
            if(HelpContext->Modules.Vectors[0]->getType() != 'b' && HelpContext->Modules.Vectors[0]->getType() != 'i'){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Parameter " << index+1 << " is not an integer.\n";
                return true;
            }
            newInt = HelpContext->Modules.Vectors[0]->getIntUnsafe(0);
            if(newInt < 0){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Integer " << newInt << " is not usigned in the parameter " << index+1 << ".\n";
                return true;
            }
            newUnsigned = newInt;
            return false;
        }
        default:
            if(printErrors){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Parameter " << index+1 << " has an invalid type: " << dataTypeToStr(HelpContext->type) << ".\n";
            }
            return true;
    }
    return false;
}
bool getIntFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, int & newInteger, bool printErrors
){
    VariableModule Literal;
    if(getSingleValueFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext, CurrentInstr, Parameters, index, Literal, printErrors)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    if(Literal.getType() != 'b' && Literal.getType() != 'i'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No bool value in the parameter " << index+1 << ".\n";
        return true;
    }
    newInteger = Literal.getIntUnsafe();
    return false;
}
bool getDoubleFromTheParameter(ObjectMemoryStruct & ObjectMemory, const vector<DynamicVariableInfo> & DynamicLocalVariables,
    ContextClass & HelpContext, const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    unsigned index, double & newDouble, bool printErrors
){
    VariableModule Literal;
    if(getSingleValueFromTheParameter(ObjectMemory, DynamicLocalVariables, HelpContext,
        CurrentInstr, Parameters, index, Literal, printErrors
    )){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter " << index+1 << ".\n";
        return true;
    }
    if(Literal.getType() != 'd' && Literal.getType() != 'i'){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No double value in the parameter " << index+1 << ".\n";
        return true;
    }
    newDouble = Literal.getDoubleUnsafe();
    return false;
}

template <class Entity>
void chooseRandomEntity(vector<Entity*> & Vector){
    if(Vector.size() == 0){
        return;
    }
    Entity * randomEntity = Vector[rand() % Vector.size()];
    Vector.clear();
    Vector.push_back(randomEntity);
}
template <class Entity>
bool checkIfContainerIsNotEmpty(vector<Entity*> & Vector){
    if(Vector.size() == 0){
        cerr << "Error: In " << __FUNCTION__ << ": Container is empty.\n";
        return false;
    }
    return true;
}
bool ProcessClass::chooseRandomModuleInstance(ContextClass & NewContext){
    switch(NewContext.type){
        case super_text_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.SuperTexts);
        case super_text_mod_vec:
            chooseRandomEntity(NewContext.Modules.SuperTexts);
            return true;
        case super_editable_text_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.SuperEditableTexts);
        case super_editable_text_mod_vec:
            chooseRandomEntity(NewContext.Modules.SuperEditableTexts);
            return true;
        case image_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Images);
        case image_mod_vec:
            chooseRandomEntity(NewContext.Modules.Images);
            return true;
        case movement_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Movements);
        case movement_mod_vec:
            chooseRandomEntity(NewContext.Modules.Movements);
            return true;
        case collision_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Collisions);
        case collision_mod_vec:
            chooseRandomEntity(NewContext.Modules.Collisions);
            return true;
        case particles_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Particles);
        case particles_mod_vec:
            chooseRandomEntity(NewContext.Modules.Particles);
            return true;
        case event_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Events);
        case event_mod_vec:
            chooseRandomEntity(NewContext.Modules.Events);
            return true;
        case variable_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Variables);
        case variable_mod_vec:
            chooseRandomEntity(NewContext.Modules.Variables);
            return true;
        case scrollbar_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Scrollbars);
        case scrollbar_mod_vec:
            chooseRandomEntity(NewContext.Modules.Scrollbars);
            return true;
        case primitives_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Primitives);
        case primitives_mod_vec:
            chooseRandomEntity(NewContext.Modules.Primitives);
            return true;
        case vector_mod:
            return checkIfContainerIsNotEmpty(NewContext.Modules.Vectors);
        case vector_mod_vec:
            chooseRandomEntity(NewContext.Modules.Vectors);
            return true;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Type \'" << dataTypeToStr(NewContext.type) << "\' is not valid for this function.\n";
            return false;
    }
    return true;
}

template <class Entity>
Entity *getFirstNotDeletedInVector(vector<Entity> &Vector){
    for(typename vector<Entity>::iterator Instance = Vector.begin(); Instance != Vector.end(); ++Instance){
        if(!Instance->getIsDeleted()){
            return &(*Instance);
        }
    }
    return nullptr;
}
template<class Entity>
Entity * getFirstNotDeletedInVector(vector<Entity*> &Vector){
    for(typename vector<Entity*>::iterator Instance = Vector.begin(); Instance != Vector.end(); ++Instance){
        if(!(*Instance)->getIsDeleted()){
            return *Instance;
        }
    }
    return nullptr;
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

void ProcessClass::aggregateCameras(OperationClass &Operation, ContextClass &NewContext, vector<Camera2D*> AggregatedCameras,
    const EngineClass & Engine, ObjectMemoryStruct & ObjectMemory, bool onlyFirstRequired
){
    Camera2D * Camera = nullptr;
    if(Operation.ConditionalChain.size() == 0 && onlyFirstRequired){
        if(AggregatedCameras.size() > 0){
            Camera = getFirstNotDeletedInVector(AggregatedCameras);
        }
        else if(Cameras.size() > 0){
            Camera = getFirstNotDeletedInVector(Cameras);
        }
        if(Camera != nullptr && (Operation.Location.cameraID == "" || Operation.Location.cameraID == Camera->getID())){
            findContextInCamera(Operation.Location.attribute, NewContext, Camera);
        }
        return;
    }
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
    vector<LayerClass> TempLayers;
    
    unsigned cameraIdx = 0, vector_end = 0;

    if(AggregatedCameras.size() > 0){
        vector_end = AggregatedCameras.size();
    }
    else{
        vector_end = Cameras.size();
    }
    
    while(cameraIdx < vector_end){
        Camera = nullptr;
        if(AggregatedCameras.size() > 0){
            Camera = AggregatedCameras[cameraIdx];
        }
        else if(Cameras.size() > 0){
            Camera = &Cameras[cameraIdx];
        }
        if(Camera == nullptr || Camera->getIsDeleted()){
            cameraIdx++;
            continue;
        }
        if(Operation.Location.cameraID != "" && Operation.Location.cameraID != Camera->getID()){
            cameraIdx++;
            if(onlyFirstRequired){
                break;
            }
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.cameraID = Camera->getID();
        }
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, Operation.resultStack, TempObject, TempLayer, Engine, ObjectMemory) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInCamera(Operation.Location.attribute, NewContext, Camera);
            }
            else{
                TempContext.clear();
                findContextInCamera(Operation.Location.attribute, TempContext, Camera);
                NewContext = TempContext;
            }
        }
        if(onlyFirstRequired){
            break;
        }
        if(Operation.Location.cameraID != "" || (Operation.instruction == EngineInstr::first
            && (NewContext.BasePointers.size() == 1 || NewContext.Cameras.size() == 1))){
            break;
        }
        cameraIdx++;
    }

    delete TempObject;
    delete TempLayer;
    if(Operation.instruction == EngineInstr::random_i){
        if(NewContext.type == camera_vec){
            chooseRandomEntity(NewContext.Cameras);
        }
        else if(NewContext.type == camera_inst){
            checkIfContainerIsNotEmpty(NewContext.Cameras);
        }
        else{
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregateLayers(OperationClass & Operation, ContextClass & NewContext, vector<LayerClass*> AggregatedLayers,
    const EngineClass & Engine, ObjectMemoryStruct & ObjectMemory, bool onlyFirstRequired
){
    LayerClass * Layer = nullptr;
    if(Operation.ConditionalChain.size() == 0 && onlyFirstRequired){
        if(AggregatedLayers.size() > 0){
            Layer = getFirstNotDeletedInVector(AggregatedLayers);
        }
        else if(Cameras.size() > 0){
            Layer = getFirstNotDeletedInVector(Layers);
        }
        if(Layer != nullptr && (Operation.Location.layerID == "" || Operation.Location.layerID == Layer->getID())){
            findContextInLayer(Operation.Location, NewContext, Layer);
        }
        return;
    }
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
    
    unsigned layerIdx = 0, vector_end = 0;

    if(AggregatedLayers.size() > 0){
        vector_end = AggregatedLayers.size();
    }
    else if(Layers.size() > 0){
        vector_end = Layers.size();
    }

    while(layerIdx < vector_end){
        Layer = nullptr;
        if(AggregatedLayers.size() > 0){
            Layer = AggregatedLayers[layerIdx];
        }
        else if(Layers.size() > 0){
            Layer = &Layers[layerIdx];
        }
        if(Layer == nullptr || Layer->getIsDeleted()){
            layerIdx++;
            continue;
        }
        if(Operation.Location.layerID != "" && Operation.Location.layerID != Layer->getID()){
            layerIdx++;
            if(onlyFirstRequired){
                break;
            }
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.layerID = Layer->getID();
        }
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, Operation.resultStack, TempObject, Layer, Engine, ObjectMemory) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInLayer(Operation.Location, NewContext, Layer);
            }
            else{
                TempContext.clear();
                findContextInLayer(Operation.Location, TempContext, Layer);
                NewContext = TempContext;
            }
        }
        if(onlyFirstRequired){
            break;
        }
        if(Operation.Location.layerID != "" || (Operation.instruction == EngineInstr::first
            && (NewContext.BasePointers.size() == 1 || NewContext.Layers.size() == 1))){
            break;
        }
        layerIdx++;
    }

    delete TempObject;
    if(Operation.instruction == EngineInstr::random_i){
        switch(NewContext.type){
            case layer_inst:
                checkIfContainerIsNotEmpty(NewContext.Layers);
                break;
            case layer_vec:
                chooseRandomEntity(NewContext.Layers);
                break;
            case object_inst:
                checkIfContainerIsNotEmpty(NewContext.Objects);
                break;
            case object_vec:
                chooseRandomEntity(NewContext.Objects);
                break;
            default:
                NewContext.leaveOneRandomBasePointer();
                break;
        }
    }
}
void ProcessClass::aggregateObjects(OperationClass &Operation, ContextClass &NewContext, vector<AncestorObject *> AggregatedObjects,
    const EngineClass &Engine, ObjectMemoryStruct & ObjectMemory, bool onlyFirstRequired
){
    if(AggregatedObjects.size() == 0){
        return;
    }
    if(Operation.ConditionalChain.size() == 0 && onlyFirstRequired){
        AncestorObject * Object = getFirstNotDeletedInVector(AggregatedObjects);
        if(Object != nullptr && (Operation.Location.objectID == "" || Operation.Location.objectID == Object->getID())){
            findContextInObject(Operation.Location, NewContext, Object);
        }
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
        if(Object == nullptr || Object->getIsDeleted()){
            continue;
        }
        if(Operation.Location.objectID != "" && Operation.Location.objectID != Object->getID()){
            if(onlyFirstRequired){
                return;
            }
            continue;
        }
        for(ConditionClass & Condition : Operation.ConditionalChain){
            Condition.Location.layerID = Object->getLayerID();
            Condition.Location.objectID = Object->getID();
        }
        if(Operation.ConditionalChain.size() == 0
            || evaluateConditionalChain(Operation.ConditionalChain, Operation.resultStack, Object, EmptyLayer, Engine, ObjectMemory) == 't'
        ){
            if(Operation.instruction != EngineInstr::last){
                findContextInObject(Operation.Location, NewContext, Object);
            }
            else{
                TempContext.clear();
                findContextInObject(Operation.Location, TempContext, Object);
                NewContext = TempContext;
            }
        }
        if(onlyFirstRequired){
            break;
        }
        if(Operation.instruction == EngineInstr::first && (NewContext.BasePointers.size() == 1 || NewContext.Objects.size() == 1)){
            break;
        }
    }

    if(Operation.instruction == EngineInstr::random_i){
        if(NewContext.type == object_vec){
            checkIfContainerIsNotEmpty(NewContext.Objects);
        }
        else if(NewContext.type == object_vec){
            chooseRandomEntity(NewContext.Objects);
        }
        else if(!chooseRandomModuleInstance(NewContext)){
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
template<class ModuleClass>
void ProcessClass::aggregateModuleContextFromVectors(vector<ModuleClass*> AggregatedModules, const DataType & aggregatedType, OperationClass & Operation,
    ContextClass & NewContext, AncestorObject * Object, const EngineClass & Engine, ObjectMemoryStruct & ObjectMemory, bool onlyFirstRequired
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
        if(Operation.ConditionalChain.size() == 0 || evaluateConditionalChain(Operation.ConditionalChain, Operation.resultStack, Object, EmptyLayer, Engine, ObjectMemory) == 't'){
            if(Operation.instruction != EngineInstr::last){
                findContextInModule(aggregatedType, Operation.Location.attribute, NewContext, Instance);
            }
            else{
                TempContext.clear();
                findContextInModule(aggregatedType, Operation.Location.attribute, TempContext, Instance);
                NewContext = TempContext;
            }
        }
        if((Operation.instruction == EngineInstr::first || onlyFirstRequired) && NewContext.Modules.hasInstanceOfAnyModule()){
            return;
        }
    }
}
template<class ModuleClass>
void ProcessClass::findContextInModule(DataType type, AttributeType attribute, ContextClass & NewContext, ModuleClass * Module){
    if(Module == nullptr){
        return;
    }
    switch(attribute){
        case null_a:
        case super_text_a:
        case super_editable_text_a:
        case image_a:
        case movement_a:
        case collision_a:
        case particles_a:
        case event_a:
        case variable_a:
        case scrollbar_a:
        case primitives_a:
        case vector_a:
            NewContext.type = type;
            NewContext.addModule(Module);
            return;
        default:
            vector<BasePointersStruct> BasePointers;
            Module->getContext(attribute, BasePointers);
            if(BasePointers.size() > 0){
                NewContext.BasePointers.insert(NewContext.BasePointers.end(), BasePointers.begin(), BasePointers.end());
                if(BasePointers.size() == 1){
                    NewContext.type = pointer_inst;
                }
                else{
                    NewContext.type = pointer_vec;
                }
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Context not found.\n";
            }
            return;
    }
}
template<class ModuleClass>
void ProcessClass::getContextFromModuleVectorById(DataType moduleType, string moduleID,
    AttributeType attribute, ContextClass & NewContext, vector<ModuleClass*> AggregatedModules,
    bool onlyFirstRequired
){
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

        if(attribute == null_a){
            NewContext.type = moduleType;
            NewContext.addModule(Module);
        }
        else{
            vector<BasePointersStruct> BasePointers;
            Module->getContext(attribute, BasePointers);
            if(BasePointers.size() > 0){
                NewContext.BasePointers.insert(NewContext.BasePointers.end(), BasePointers.begin(), BasePointers.end());
                if(BasePointers.size() == 1){
                    NewContext.type = pointer_inst;
                }
                else{
                    NewContext.type = pointer_vec;
                }
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "No type.\n";
            }
        }

        if(onlyFirstRequired || moduleID != ""){
            return;
        }
    }
}
template<class ModuleClass>
ModuleClass * findLastModule(vector<ModuleClass*> & Vector, const string & moduleID){
    ModuleClass * Module = lastNotDeletedInVector(Vector);
    if(Module == nullptr || moduleID == "" || moduleID == Module->getID()){
        return Module;
    }
    return nullptr;
}
void ProcessClass::aggregateModules(OperationClass & Operation, ContextClass & NewContext,
    ContextClass * OldContext, ObjectMemoryStruct & ObjectMemory, const EngineClass & Engine
){
    ModulesPointers * AggregatedModules = &OldContext->Modules;
    if(Operation.ConditionalChain.empty() && Operation.instruction == EngineInstr::last){
        switch(OldContext->type){
            case super_text_mod:
                if(AggregatedModules->SuperTexts.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->SuperTexts[0]
                );
                break;
            case super_text_mod_vec:
                if(AggregatedModules->SuperTexts.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->SuperTexts, Operation.Location.moduleID)
                );
                break;
            case super_editable_text_mod:
                if(AggregatedModules->SuperEditableTexts.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->SuperEditableTexts[0]
                );
                break;
            case super_editable_text_mod_vec:
                if(AggregatedModules->SuperEditableTexts.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->SuperEditableTexts,
                        Operation.Location.moduleID
                    )
                );
                break;
            case image_mod:
                if(AggregatedModules->Images.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Images[0]
                );
                break;
            case image_mod_vec:
                if(AggregatedModules->Images.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Images, Operation.Location.moduleID)
                );
                break;
            case movement_mod:
                if(AggregatedModules->Movements.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Movements[0]
                );
                break;
            case movement_mod_vec:
                if(AggregatedModules->Movements.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Movements, Operation.Location.moduleID)
                );
                break;
            case collision_mod:
                if(AggregatedModules->Collisions.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Collisions[0]
                );
                break;
            case collision_mod_vec:
                if(AggregatedModules->Collisions.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Collisions, Operation.Location.moduleID)
                );
                break;
            case particles_mod:
                if(AggregatedModules->Particles.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Particles[0]
                );
                break;
            case particles_mod_vec:
                if(AggregatedModules->Particles.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Particles, Operation.Location.moduleID)
                );
                break;
            case event_mod:
                if(AggregatedModules->Events.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Events[0]
                );
                break;
            case event_mod_vec:
                if(AggregatedModules->Events.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Events, Operation.Location.moduleID)
                );
                break;
            case variable_mod:
                if(AggregatedModules->Variables.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Variables[0]
                );
                break;
            case variable_mod_vec:
                if(AggregatedModules->Variables.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Variables, Operation.Location.moduleID)
                );
                break;
            case scrollbar_mod:
                if(AggregatedModules->Scrollbars.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Scrollbars[0]
                );
                break;
            case scrollbar_mod_vec:
                if(AggregatedModules->Scrollbars.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Scrollbars, Operation.Location.moduleID)
                );
                break;
            case primitives_mod:
                if(AggregatedModules->Primitives.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Primitives[0]
                );
                break;
            case primitives_mod_vec:
                if(AggregatedModules->Primitives.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Primitives, Operation.Location.moduleID)
                );
                break;
            case vector_mod:
                if(AggregatedModules->Vectors.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    AggregatedModules->Vectors[0]
                );
                break;
            case vector_mod_vec:
                if(AggregatedModules->Vectors.empty()){
                    break;
                }
                findContextInModule(OldContext->type, Operation.Location.attribute, NewContext,
                    findLastModule(AggregatedModules->Vectors, Operation.Location.moduleID)
                );
                break;
            default:
                break;
        }
        return;
    }

    AncestorObject * EmptyObject = new AncestorObject();
    switch(OldContext->type){
        case super_text_mod:
            aggregateModuleContextFromVectors(AggregatedModules->SuperTexts, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case super_text_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->SuperTexts, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case super_editable_text_mod:
            aggregateModuleContextFromVectors(AggregatedModules->SuperEditableTexts,
                OldContext->type, Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case super_editable_text_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->SuperEditableTexts,
                OldContext->type, Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case image_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Images, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case image_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Images, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case movement_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Movements, OldContext->type, 
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case movement_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Movements, OldContext->type, 
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case collision_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Collisions, OldContext->type, 
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case collision_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Collisions, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case particles_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Particles, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case particles_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Particles, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case event_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Events, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case event_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Events, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case variable_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Variables, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case variable_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Variables, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case scrollbar_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Scrollbars, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case scrollbar_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Scrollbars, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case primitives_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Primitives, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case primitives_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Primitives, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        case vector_mod:
            aggregateModuleContextFromVectors(AggregatedModules->Vectors, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, true
            );
            break;
        case vector_mod_vec:
            aggregateModuleContextFromVectors(AggregatedModules->Vectors, OldContext->type,
                Operation, NewContext, EmptyObject, Engine, ObjectMemory, false
            );
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Module type \'" << dataTypeToStr(OldContext->type) << "\' does not exist.\n";
            delete EmptyObject;
            return;
    }
    
    delete EmptyObject;

    if(Operation.instruction == EngineInstr::random_i){
        if(!chooseRandomModuleInstance(NewContext)){
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregatePointers(ContextClass & NewContext, vector<BasePointersStruct> & AggregatedPointers, bool onlyFirstRequired){
    NewContext.type = pointer_inst;
    if(AggregatedPointers.size() == 0){
        NewContext.type = null_dt;
        return;
    }
    if(onlyFirstRequired){
        NewContext.BasePointers.push_back(AggregatedPointers.front());
        return;
    }
    switch(CurrentInstr.instruction){
        case EngineInstr::first:
            NewContext.BasePointers.push_back(AggregatedPointers.front());
            break;
        case EngineInstr::last:
            NewContext.BasePointers.push_back(AggregatedPointers.back());
            break;
        case EngineInstr::all:
            NewContext.BasePointers.insert(NewContext.BasePointers.end(), AggregatedPointers.begin(), AggregatedPointers.end());
            NewContext.type = pointer_vec;
            break;
        case random_i:
            NewContext.BasePointers.push_back(AggregatedPointers[rand() % AggregatedPointers.size()]);
            break;
        default:
            NewContext.type = null_dt;
            break;
    }
}
void ProcessClass::aggregateVariables(ContextClass & NewContext, vector<VariableModule> & AggregatedVariables, bool onlyFirstRequired){
    if(AggregatedVariables.size() == 0){
        NewContext.type = null_dt;
        return;
    }
    NewContext.type = value_inst;
    if(onlyFirstRequired){
        NewContext.Values.push_back(AggregatedVariables.front());
    }
    else if(CurrentInstr.instruction == EngineInstr::first){
        NewContext.Values.push_back(AggregatedVariables.front());
    }
    else if(CurrentInstr.instruction == EngineInstr::last){
        NewContext.Values.push_back(AggregatedVariables.back());
    }
    else if(CurrentInstr.instruction == EngineInstr::all){
        NewContext.Values.insert(NewContext.Values.end(), AggregatedVariables.begin(), AggregatedVariables.end());
        NewContext.type = value_vec;
    }
    else if(CurrentInstr.instruction == EngineInstr::random_i && AggregatedVariables.size() > 0){
        NewContext.Values.push_back(AggregatedVariables[rand() % AggregatedVariables.size()]);
    }
}
void ProcessClass::findContextInCamera(AttributeType attribute, ContextClass & NewContext, Camera2D * Camera){
    if(Camera == nullptr){
        return;
    }
    switch(attribute){
        case null_a:
        case camera_a:
            NewContext.type = camera_inst;
            NewContext.Cameras.push_back(Camera);
            if(NewContext.Cameras.size() > 1){
                NewContext.type = camera_vec;
            }
            return;
        case is_active:
            NewContext.addBasePointer(&Camera->isActive);
            return;
        case id:
            if(isStringInVector(reservedIDs, Camera->ID)){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Access to the reserved ID \'" << Camera->ID << "\' address was denied.\n";
                return;
            }
            NewContext.addBasePointer(&Camera->ID);
            NewContext.readOnly = true;
            return;
        case pos_x:
            NewContext.addBasePointer(&Camera->pos.x);
            return;
        case pos_y:
            NewContext.addBasePointer(&Camera->pos.y);
            return;
        case relative_pos_x:
            NewContext.addBasePointer(&Camera->relativePos.x);
            return;
        case relative_pos_y:
            NewContext.addBasePointer(&Camera->relativePos.y);
            return;
        case vision_shift_x:
            NewContext.addBasePointer(&Camera->visionShift.x);
            return;
        case vision_shift_y:
            NewContext.addBasePointer(&Camera->visionShift.y);
            return;
        case size_x:
            NewContext.addBasePointer(&Camera->size.x);
            NewContext.readOnly = true;
            return;
        case size_y:
            NewContext.addBasePointer(&Camera->size.y);
            NewContext.readOnly = true;
            return;
        case zoom:
            NewContext.addBasePointer(&Camera->zoom);
            NewContext.readOnly = true;
            return;
        case min_zoom:
            NewContext.addBasePointer(&Camera->minZoom);
            NewContext.readOnly = true;
            return;
        case max_zoom:
            NewContext.addBasePointer(&Camera->maxZoom);
            NewContext.readOnly = true;
            return;
        case speed:
            NewContext.addBasePointer(&Camera->speed);
            return;
        case zoom_in_key:
            NewContext.addBasePointer(&Camera->zoomInKey);
            return;
        case zoom_out_key:
            NewContext.addBasePointer(&Camera->zoomOutKey);
            return;
        case zoom_reset_key:
            NewContext.addBasePointer(&Camera->zoomResetKey);
            return;
        case up_key:
            NewContext.addBasePointer(&Camera->upKey);
            return;
        case right_key:
            NewContext.addBasePointer(&Camera->rightKey);
            return;
        case down_key:
            NewContext.addBasePointer(&Camera->downKey);
            return;
        case left_key:
            NewContext.addBasePointer(&Camera->leftKey);
            return;
        case pinned_camera_id:
            NewContext.addBasePointer(&Camera->pinnedCameraID);
            return;
        case followed_layer_id:
            NewContext.addBasePointer(&Camera->followedLayerID);
            return;
        case followed_object_id:
            NewContext.addBasePointer(&Camera->followedObjectID);
            return;
        case followed_image_id:
            NewContext.addBasePointer(&Camera->followedImageID);
            return;
        case is_pinned_to_camera:
            NewContext.addBasePointer(&Camera->isPinnedToCamera);
            return;
        case is_following_object:
            NewContext.addBasePointer(&Camera->isFollowingObject);
            return;
        case is_using_keyboard_to_move:
            NewContext.addBasePointer(&Camera->isUsingKeyboardToMove);
            return;
        case is_using_keyboard_to_zoom:
            NewContext.addBasePointer(&Camera->isUsingKeyboardToZoom);
            return;
        case can_be_modified_by_mouse:
            NewContext.addBasePointer(&Camera->canBeModifiedByMouse);
            return;
        case can_move_objects:
            NewContext.addBasePointer(&Camera->canMoveObjects);
            return;
        case can_edit_text:
            NewContext.addBasePointer(&Camera->canEditText);
            return;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '"
                << attributeToStr(attribute) << "' is not valid.\n";
            return;
    }
}
void ProcessClass::findContextInLayer(const ValueLocation & Location, ContextClass & NewContext,
    LayerClass * Layer
){
    if(Layer == nullptr){
        return;
    }
    if(Location.attribute == object_a || Location.objectID != "" || Location.moduleType != null_s
        || Location.moduleID != ""
    ){
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
        return;
    }
    switch(Location.attribute){
        case null_a:
        case layer_a:
            NewContext.type = layer_inst;
            NewContext.Layers.push_back(Layer);
            if(NewContext.Layers.size() > 1){
                NewContext.type = layer_vec;
            }
            return;
        case is_active:
            NewContext.addBasePointer(Layer->getIsActiveAddr());
            return;
        case id:
            if(isStringInVector(reservedIDs, Layer->getID())){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Access to the reserved ID \'" << Layer->getID()
                    << "\' address was denied.\n";
                return;
            }
            NewContext.addBasePointer(Layer->getIDAddr());
            NewContext.readOnly = true;
            return;
        case group:
            for(string & group : Layer->getGroupsAddr()){
                NewContext.addBasePointer(&group);
            }
            return;
        case pos_x:
            NewContext.addBasePointer(&Layer->pos.x);
            return;
        case pos_y:
            NewContext.addBasePointer(&Layer->pos.y);
            return;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Attribute \'" << attributeToStr(Location.attribute) << "\' does not exist.\n";
            return;
    }
}
inline DataType instantiateEntityDataType(const InstrDescription & CurrentInstr, const DataType & oldType){
    switch(oldType){
        case camera_vec:
            return camera_inst;
        case layer_vec:
            return layer_inst;
        case object_vec:
            return object_inst;
        case super_text_mod_vec:
            return super_text_mod;
        case super_editable_text_mod_vec:
            return super_editable_text_mod;
        case image_mod_vec:
            return image_mod;
        case movement_mod_vec:
            return movement_mod;
        case collision_mod_vec:
            return collision_mod;
        case particles_mod_vec:
            return particles_mod;
        case event_mod_vec:
            return event_mod;
        case variable_mod_vec:
            return variable_mod;
        case scrollbar_mod_vec:
            return scrollbar_mod;
        case primitives_mod_vec:
            return primitives_mod;
        case vector_mod_vec:
            return vector_mod;
        case value_vec:
            return value_inst;
        case pointer_vec:
            return pointer_inst;
        case camera_inst:
        case layer_inst:
        case object_inst:
        case super_text_mod:
        case super_editable_text_mod:
        case image_mod:
        case movement_mod:
        case collision_mod:
        case particles_mod:
        case event_mod:
        case variable_mod:
        case scrollbar_mod:
        case primitives_mod:
        case vector_mod:
        case value_inst:
        case pointer_inst:
            return oldType;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << dataTypeToStr(oldType) << "\' is not valid for this operation.\n";
            return null_dt;
    }
}
template <class Module>
void ProcessClass::findContextInModuleVector(const ValueLocation & Location,
    ContextClass & NewContext, vector<Module> & Source
){
    for(Module & Instance : Source){
        if(Location.moduleID != "" && Location.moduleID != Instance.getID()){
            continue;
        }
        if(!Instance.getIsDeleted()){
            findContextInModule(sourceToEntityType(CurrentInstr, Location.moduleType),
                Location.attribute, NewContext, &Instance
            );
        }
        if(Location.moduleID != ""){
            break;
        }
    }
}
void ProcessClass::findContextInObject(const ValueLocation & Location, ContextClass & NewContext,
    AncestorObject * Object
){
    if(Object == nullptr){
        return;
    }
    if(Location.moduleType != null_s){
        switch(Location.moduleType){
            case super_text:
                findContextInModuleVector(Location, NewContext, Object->SuperTextContainer);
                break;
            case super_editable_text:
                findContextInModuleVector(Location, NewContext, Object->SuperEditableTextContainer);
                break;
            case image:
                findContextInModuleVector(Location, NewContext, Object->ImageContainer);
                break;
            case movement:
                findContextInModuleVector(Location, NewContext, Object->MovementContainer);
                break;
            case collision:
                findContextInModuleVector(Location, NewContext, Object->CollisionContainer);
                break;
            case particles:
                findContextInModuleVector(Location, NewContext, Object->ParticlesContainer);
                break;
            case event:
                findContextInModuleVector(Location, NewContext, Object->EventContainer);
                break;
            case variable:
                findContextInModuleVector(Location, NewContext, Object->VariablesContainer);
                break;
            case scrollbar:
                findContextInModuleVector(Location, NewContext, Object->ScrollbarContainer);
                break;
            case primitives:
                findContextInModuleVector(Location, NewContext, Object->PrimitivesContainer);
                break;
            case vector_s:
                findContextInModuleVector(Location, NewContext, Object->VectorContainer);
                break;
            default:
                NewContext.type = null_dt;
                break;
        }
    }
    else if(Location.attribute == layer_id){
        NewContext.addBasePointer(&Object->getLayerIDAddr());
    }
    else if(Location.attribute == object_a || Location.attribute == null_a){
        NewContext.type = object_inst;
        NewContext.Objects.push_back(Object);
        if(NewContext.Objects.size() > 1){
            NewContext.type = object_vec;
        }
    }
    else{
        vector<BasePointersStruct> BasePointers;
        Object->getPrimaryContext(Location.attribute, BasePointers);
        if(BasePointers.size() > 0){
            NewContext.type = pointer_inst;
            NewContext.BasePointers.insert(NewContext.BasePointers.end(), BasePointers.begin(), BasePointers.end());
            if(NewContext.BasePointers.size() > 1){
                NewContext.type = pointer_vec;
            }
        }
        else{
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Context not found.\n";
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
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Layer \'" << Location.layerID << "\' does not exist.\n";
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
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object does not exist.\n";
        return false;
    }

    return true;
}
void ProcessClass::aggregateCamerasAndLayersById(ValueLocation & Location, ContextClass & NewContext, AncestorObject * Owner,
    LayerClass * OwnerLayer
){
    if(Location.source == ValueSource::layer){
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
    else if(Location.source == ValueSource::camera){
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
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "No valid source provided.\n";
    }
}
void ProcessClass::aggregateModulesById(DataType moduleType, string moduleID, AttributeType attribute,
    ContextClass & NewContext, ModulesPointers & AggregatedModules
){
    switch (moduleType){
        case super_text_mod:
            if(AggregatedModules.SuperTexts.size() == 0){
                break;
            }
            getContextFromModuleVectorById<SuperTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.SuperTexts, true);
            break;
        case super_text_mod_vec:
            if(AggregatedModules.SuperTexts.size() == 0){
                break;
            }
            getContextFromModuleVectorById<SuperTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.SuperTexts, false);
            break;
        case super_editable_text_mod:
            if(AggregatedModules.SuperEditableTexts.size() == 0){
                break;
            }
            getContextFromModuleVectorById<SuperEditableTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.SuperEditableTexts, true);
            break;
        case super_editable_text_mod_vec:
            if(AggregatedModules.SuperEditableTexts.size() == 0){
                break;
            }
            getContextFromModuleVectorById<SuperEditableTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.SuperEditableTexts, false);
            break;
        case image_mod:
            if(AggregatedModules.Images.size() == 0){
                break;
            }
            getContextFromModuleVectorById<ImageModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Images, true);
            break;
        case image_mod_vec:
            if(AggregatedModules.Images.size() == 0){
                break;
            }
            getContextFromModuleVectorById<ImageModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Images, false);
            break;
        case movement_mod:
            if(AggregatedModules.Movements.size() == 0){
                break;
            }
            getContextFromModuleVectorById<MovementModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Movements, true);
            break;
        case movement_mod_vec:
            if(AggregatedModules.Movements.size() == 0){
                break;
            }
            getContextFromModuleVectorById<MovementModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Movements, false);
            break;
        case collision_mod:
            if(AggregatedModules.Collisions.size() == 0){
                break;
            }
            getContextFromModuleVectorById<CollisionModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Collisions, true);
            break;
        case collision_mod_vec:
            if(AggregatedModules.Collisions.size() == 0){
                break;
            }
            getContextFromModuleVectorById<CollisionModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Collisions, false);
            break;
        case particles_mod:
            if(AggregatedModules.Particles.size() == 0){
                break;
            }
            getContextFromModuleVectorById<ParticleEffectModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Particles, true);
            break;
        case particles_mod_vec:
            if(AggregatedModules.Particles.size() == 0){
                break;
            }
            getContextFromModuleVectorById<ParticleEffectModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Particles, false);
            break;
        case event_mod:
            if(AggregatedModules.Events.size() == 0){
                break;
            }
            getContextFromModuleVectorById<EventModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Events, true);
            break;
        case event_mod_vec:
            if(AggregatedModules.Events.size() == 0){
                break;
            }
            getContextFromModuleVectorById<EventModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Events, false);
            break;
        case variable_mod:
            if(AggregatedModules.Variables.size() == 0){
                break;
            }
            getContextFromModuleVectorById<VariableModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Variables, true);
            break;
        case variable_mod_vec:
            if(AggregatedModules.Variables.size() == 0){
                break;
            }
            getContextFromModuleVectorById<VariableModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Variables, false);
            break;
        case scrollbar_mod:
            if(AggregatedModules.Scrollbars.size() == 0){
                break;
            }
            getContextFromModuleVectorById<ScrollbarModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Scrollbars, true);
            break;
        case scrollbar_mod_vec:
            if(AggregatedModules.Scrollbars.size() == 0){
                break;
            }
            getContextFromModuleVectorById<ScrollbarModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Scrollbars, false);
            break;
        case primitives_mod:
            if(AggregatedModules.Primitives.size() == 0){
                break;
            }
            getContextFromModuleVectorById<PrimitivesModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Primitives, true);
            break;
        case primitives_mod_vec:
            if(AggregatedModules.Primitives.size() == 0){
                break;
            }
            getContextFromModuleVectorById<PrimitivesModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Primitives, false);
            break;
        case vector_mod:
            if(AggregatedModules.Vectors.size() == 0){
                break;
            }
            getContextFromModuleVectorById<VectorModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Vectors, true);
            break;
        case vector_mod_vec:
            if(AggregatedModules.Vectors.size() == 0){
                break;
            }
            getContextFromModuleVectorById<VectorModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Vectors, false);
            break;
        default:
            // cerr << instructionError(CurrentInstr, __FUNCTION__)
            //     << "There are no instances of the \'" << dataTypeToStr(moduleType) << "\' module.\n";
            break;
    }
}
void ProcessClass::findLowerContextById(ValueLocation & Location, ContextClass & NewContext, ContextClass * OldContext){
    switch(OldContext->type){
        case object_inst:
            for(AncestorObject * Object : OldContext->Objects){
                if(Object->getIsDeleted()){
                    continue;
                }
                if(Location.objectID != "" && Location.objectID != Object->getID()){
                    return;
                }
                findContextInObject(Location, NewContext, Object);
                return;
            }
        case object_vec:
            for(AncestorObject * Object : OldContext->Objects){
                if(Object->getIsDeleted()){
                    continue;
                }
                if(Location.objectID != "" && Location.objectID != Object->getID()){
                    continue;
                }
                findContextInObject(Location, NewContext, Object);
                if(Location.objectID != ""){
                    return;
                }
            }
            break;
        case layer_inst:
            for(LayerClass * Layer : OldContext->Layers){
                if(Layer->getIsDeleted()){
                    continue;
                }
                if(Location.layerID != "" && Location.layerID != Layer->getID()){
                    return;
                }
                findContextInLayer(Location, NewContext, Layer);
                return;
            }
            break;
        case layer_vec:
            for(LayerClass * Layer : OldContext->Layers){
                if(Layer->getIsDeleted()){
                    continue;
                }
                if(Location.layerID != "" && Location.layerID != Layer->getID()){
                    continue;
                }
                findContextInLayer(Location, NewContext, Layer);
                if(Location.layerID != ""){
                    return;
                }
            }
            break;
        case camera_inst:
            for(Camera2D * Camera : OldContext->Cameras){
                if(Camera->getIsDeleted()){
                    continue;
                }
                if(Location.cameraID != "" && Location.cameraID != Camera->getID()){
                    return;
                }
                findContextInCamera(Location.attribute, NewContext, Camera);
                return;
            }
            break;
        case camera_vec:
            for(Camera2D * Camera : OldContext->Cameras){
                if(Camera->getIsDeleted()){
                    continue;
                }
                if(Location.cameraID != "" && Location.cameraID != Camera->getID()){
                    continue;
                }
                findContextInCamera(Location.attribute, NewContext, Camera);
                if(Location.cameraID != ""){
                    return;
                }
            }
            break;
        default:
            aggregateModulesById(OldContext->type, Location.moduleID, Location.attribute, NewContext, OldContext->Modules);
            break;
    }
}
inline bool validate(char type){
    switch(type){
        case 'b':
        case 'i':
        case 'd':
        case 's':
            return true;
        case 'n':
            return true;
        default:
            cerr << "TMP_ERROR: OPERATOR TYPE IS CORRUPTED\n";
            return false;
    }
}
inline bool cmpVars(VariableModule * Left, EngineInstr op, VariableModule * Right, const InstrDescription &CurrentInstr){
    // if(!validate(Left->type)){
    //     return false;
    // }
    // if(!validate(Right->type)){
    //     return false;
    // }
    auto[result, status] = Left->isConditionMet(op, Right);
    if(status != OK){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Invalid comparison: "
            << Left->getID() << ":" << Left->type << ":" << Left->getAnyValue() << " "
            << instrToStr(op) << " "
            << Right->getID() << ":" << Right->getType() << ":" << Right->getAnyValue() << "\n";
    }
    return result;
}
inline bool areEqual(VariableModule * Left, BasePointersStruct & Right, const InstrDescription &CurrentInstr){
    auto[result, status] = Left->isConditionMet(EngineInstr::equal, Right);
    if(status != OK){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Invalid comparison: "
            << Left->getID() << ":" << Left->type << ":" << Left->getAnyValue() << " "
            << instrToStr(EngineInstr::equal) << " "
            << Right.type << ":" << Right.getString() << "\n";
    }
    return result;
}
bool ProcessClass::checkDefaultCondition(VariableModule * Left, VariableModule * Right){
    return cmpVars(Left, equal, Right, CurrentInstr);
}
bool ProcessClass::checkDefaultCondition(BasePointersStruct * Left, BasePointersStruct * Right){
    return Left->areEqual(Right);
}
template<class Entity>
void ProcessClass::executeOperationsOnSets(vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand){
    if(CurrentInstr.instruction == EngineInstr::sum){
        NewContext = LeftOperand;
        NewContext.insert(NewContext.end(), RightOperand.begin(), RightOperand.end());
    }
    else if(CurrentInstr.instruction == EngineInstr::intersection){
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
    else if(CurrentInstr.instruction == EngineInstr::diff){
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
void ProcessClass::executeOperationsOnSets(vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand){
    if(CurrentInstr.instruction == EngineInstr::sum){
        NewContext = LeftOperand;
        NewContext.insert(NewContext.end(), RightOperand.begin(), RightOperand.end());
    }
    else if(CurrentInstr.instruction == EngineInstr::intersection){
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
    else if(CurrentInstr.instruction == EngineInstr::diff){
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
void ProcessClass::aggregateTwoSets(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    NewContext.clear();
    ContextClass LeftOperand;
    ContextClass RightOperand;

    bool negateWholeContextAfterCopy = false;
    if(LeftOperand.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateWholeContextAfterCopy, 0, true
    )){
        return;
    }

    if(RightOperand.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateWholeContextAfterCopy, 1, true)){
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << LeftOperand.ID
            << ":" << dataTypeToStr(LeftOperand.type) << ":"
            << LeftOperand.getValue(CurrentInstr, maxLengthOfValuesPrinting)
            << " " << RightOperand.ID << ":" << dataTypeToStr(RightOperand.type)
            << ":" << RightOperand.getValue(CurrentInstr, maxLengthOfValuesPrinting) << "\n";
    }

    if(LeftOperand.type != RightOperand.type){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Right operand has a different type than the left operand.\n";
        return;
    }

    if(negateWholeContextAfterCopy){
        cerr << instructionWarning(CurrentInstr, __FUNCTION__)
            << "This instruction ignores negating values, because it can copy pointers.\n";
    }

    NewContext.type = LeftOperand.type;

    switch(LeftOperand.type){
        case value_inst:
        case value_vec:
            executeOperationsOnSets(NewContext.Values, LeftOperand.Values, RightOperand.Values);
            NewContext.type = value_vec;
            break;
        case pointer_inst:
        case pointer_vec:
            executeOperationsOnSets(NewContext.BasePointers, LeftOperand.BasePointers,
                RightOperand.BasePointers
            );
            NewContext.type = pointer_vec;
            break;
        case camera_inst:
        case camera_vec:
            executeOperationsOnSets(NewContext.Cameras, LeftOperand.Cameras, RightOperand.Cameras);
            NewContext.type = camera_vec;
            break;
        case layer_inst:
        case layer_vec:
            executeOperationsOnSets(NewContext.Layers, LeftOperand.Layers, RightOperand.Layers);
            NewContext.type = layer_vec;
            break;
        case object_inst:
        case object_vec:
            executeOperationsOnSets(NewContext.Objects, LeftOperand.Objects, RightOperand.Objects);
            NewContext.type = object_vec;
            break;
        case super_text_mod:
        case super_text_mod_vec:
            executeOperationsOnSets(NewContext.Modules.SuperTexts, LeftOperand.Modules.SuperTexts,
                RightOperand.Modules.SuperTexts
            );
            NewContext.type = super_text_mod_vec;
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            executeOperationsOnSets(NewContext.Modules.SuperEditableTexts,
                LeftOperand.Modules.SuperEditableTexts, RightOperand.Modules.SuperEditableTexts
            );
            NewContext.type = super_editable_text_mod_vec;
            break;
        case image_mod:
        case image_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Images, LeftOperand.Modules.Images,
                RightOperand.Modules.Images
            );
            NewContext.type = image_mod_vec;
            break;
        case movement_mod:
        case movement_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Movements, LeftOperand.Modules.Movements,
                RightOperand.Modules.Movements
            );
            NewContext.type = movement_mod_vec;
            break;
        case collision_mod:
        case collision_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Collisions, LeftOperand.Modules.Collisions,
                RightOperand.Modules.Collisions
            );
            NewContext.type = collision_mod_vec;
            break;
        case particles_mod:
        case particles_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Particles, LeftOperand.Modules.Particles,
                RightOperand.Modules.Particles
            );
            NewContext.type = particles_mod_vec;
            break;
        case event_mod:
        case event_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Events, LeftOperand.Modules.Events,
                RightOperand.Modules.Events
            );
            NewContext.type = event_mod_vec;
            break;
        case variable_mod:
        case variable_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Variables, LeftOperand.Modules.Variables,
                RightOperand.Modules.Variables
            );
            NewContext.type = variable_mod_vec;
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Scrollbars, LeftOperand.Modules.Scrollbars,
                RightOperand.Modules.Scrollbars
            );
            NewContext.type = scrollbar_mod_vec;
            break;
        case primitives_mod:
        case primitives_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Primitives, LeftOperand.Modules.Primitives,
                RightOperand.Modules.Primitives
            );
            NewContext.type = primitives_mod_vec;
            break;
        case vector_mod:
        case vector_mod_vec:
            executeOperationsOnSets(NewContext.Modules.Vectors, LeftOperand.Modules.Vectors,
                RightOperand.Modules.Vectors
            );
            NewContext.type = vector_mod_vec;
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Type \'" << dataTypeToStr(LeftOperand.type) << "\' is not valid.\n";
            return;
    }

    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::aggregateEntities(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, const EngineClass & Engine){
    NewContext.clear();

    if(Operation.Location.source == ValueSource::layer || Operation.Location.source == ValueSource::camera){
        if(printOutInstructions){
            cout << instrToStr(Operation.instruction) << " ";
            Operation.Location.print("");
            cout << "\n";
        }
        if(Operation.Location.source == ValueSource::camera){
            aggregateCameras(Operation, NewContext, vector<Camera2D*>(), Engine, ObjectMemory, false);
        }
        else if(Operation.Location.source == ValueSource::layer){
            aggregateLayers(Operation, NewContext, vector<LayerClass*>(), Engine, ObjectMemory, false);
        }
        else{
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Source type \'" << sourceToStr(Operation.Location.source)
                << "\' does not exist in this context.\n";
        }
    }
    else if(Operation.rootParametersSize > 0){
        ContextClass * SourceContext;
        for(unsigned index = 0; index < Operation.rootParametersSize; index++){
            if(getContextPointerFromTheParameter(SourceContext, ObjectMemory, LocalToGlobalTranslation,
                CurrentInstr, Operation.Parameters, index, true
            )){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Failed to get context from the parameter " << index+1 << ".\n";
                return;
            }
            if(printOutInstructions){
                cout << instrToStr(Operation.instruction) << " ";
                Operation.Location.print(SourceContext->ID);
                cout << "\n";
            }
            NewContext.type = SourceContext->type;
            switch(SourceContext->type){
                case camera_inst:
                    if(SourceContext->Cameras.size() > 0){
                        aggregateCameras(Operation, NewContext, SourceContext->Cameras, Engine, ObjectMemory, true);
                    }
                    break;
                case camera_vec:
                    if(SourceContext->Cameras.size() > 0){
                        aggregateCameras(Operation, NewContext, SourceContext->Cameras, Engine, ObjectMemory, false);
                    }
                    break;
                case layer_inst:
                    if(SourceContext->Layers.size() > 0){
                        aggregateLayers(Operation, NewContext, SourceContext->Layers, Engine, ObjectMemory, true);
                    }
                    break;
                case layer_vec:
                    if(SourceContext->Layers.size() > 0){
                        aggregateLayers(Operation, NewContext, SourceContext->Layers, Engine, ObjectMemory, false);
                    }
                    break;
                case object_inst:
                    aggregateObjects(Operation, NewContext, SourceContext->Objects, Engine, ObjectMemory, true);
                    break;
                case object_vec:
                    aggregateObjects(Operation, NewContext, SourceContext->Objects, Engine, ObjectMemory, false);
                    break;
                case pointer_inst:
                    aggregatePointers(NewContext, SourceContext->BasePointers, true);
                    break;
                case pointer_vec:
                    aggregatePointers(NewContext, SourceContext->BasePointers, false);
                    break;
                case value_inst:
                    aggregateVariables(NewContext, SourceContext->Values, true);
                    break;
                case value_vec:
                    aggregateVariables(NewContext, SourceContext->Values, false);
                    break;
                default:
                    if(SourceContext->Modules.hasInstanceOfAnyModule()){
                        aggregateModules(Operation, NewContext, SourceContext, ObjectMemory, Engine);
                    }
                    break;
            }
        }
    }

    if(NewContext.type != null_dt){
        assignVariable(ObjectMemory, Operation.Output);
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "New context does not have a type.\n";
    }
}
inline bool doesOperandContainSingleElement(const InstrDescription & CurrentInstr, string functionName,
    const DataType & leftOperandType, const DataType & rightOperandType, const size_t & rightOperandSize
){
    if(rightOperandSize != 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Cannot move '" << dataTypeToStr(rightOperandType)
            << "'<" << rightOperandSize << "> to a variable of '"
            << dataTypeToStr(leftOperandType) << "' type.\n";
        return true;
    }
    return false;
}
inline bool didFnFailed(const InstrDescription & CurrentInstr, const string & functionName, const ReturnType & result){
    switch(result){
        case ReturnType::EMPTY:
            cerr << instructionError(CurrentInstr, functionName)
                << "Operand is empty.\n";
            return true;
        case ReturnType::NULL_VAL:
            cerr << instructionError(CurrentInstr, functionName)
                << "Operand is null.\n";
            return true;
        case ReturnType::CORRUPTED:
            cerr << instructionError(CurrentInstr, functionName)
                << "Operand is corrupted.\n";
            return true;
        default:
            return false;
    }
}
bool isEntityTypeEqual(DataType leftType, DataType rightType){
    switch(leftType){
        case camera_inst:
        case camera_vec:
            switch(rightType){
                case camera_inst:
                case camera_vec:
                    return true;
                default:
                    return false;
            }
        case layer_inst:
        case layer_vec:
            switch(rightType){
                case layer_inst:
                case layer_vec:
                    return true;
                default:
                    return false;
            }
        case object_inst:
        case object_vec:
            switch(rightType){
                case object_inst:
                case object_vec:
                    return true;
                default:
                    return false;
            }
        case super_text_mod:
        case super_text_mod_vec:
            switch(rightType){
                case super_text_mod:
                case super_text_mod_vec:
                    return true;
                default:
                    return false;
            }
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            switch(rightType){
                case super_editable_text_mod:
                case super_editable_text_mod_vec:
                    return true;
                default:
                    return false;
            }
        case image_mod:
        case image_mod_vec:
            switch(rightType){
                case image_mod:
                case image_mod_vec:
                    return true;
                default:
                    return false;
            }
        case movement_mod:
        case movement_mod_vec:
            switch(rightType){
                case movement_mod:
                case movement_mod_vec:
                    return true;
                default:
                    return false;
            }
        case collision_mod:
        case collision_mod_vec:
            switch(rightType){
                case collision_mod:
                case collision_mod_vec:
                    return true;
                default:
                    return false;
            }
        case particles_mod:
        case particles_mod_vec:
            switch(rightType){
                case particles_mod:
                case particles_mod_vec:
                    return true;
                default:
                    return false;
            }
        case event_mod:
        case event_mod_vec:
            switch(rightType){
                case event_mod:
                case event_mod_vec:
                    return true;
                default:
                    return false;
            }
        case scrollbar_mod:
        case scrollbar_mod_vec:
            switch(rightType){
                case scrollbar_mod:
                case scrollbar_mod_vec:
                    return true;
                default:
                    return false;
            }
        case primitives_mod:
        case primitives_mod_vec:
            switch(rightType){
                case primitives_mod:
                case primitives_mod_vec:
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}
void assignRightToLeft(const InstrDescription & CurrentInstr, ContextClass * LeftOperand, ContextClass & RightOperand){
    //Move a starting value to an empty variable. In other words, clear the left operand and assign it with new values or pointers.
    if(LeftOperand->readOnly){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Left operand is read-only.\n";
        return;
    }
    auto printAssignRightToLeftError = [](const InstrDescription & CurrentInstr, const DataType & leftType, const DataType & rightType) { 
        cerr << instructionError(CurrentInstr, __FUNCTION__)
        << "Cannot assign '" << dataTypeToStr(rightType)
        << "' to a variable of '" << dataTypeToStr(leftType) << "' type.\n";
    };
    LeftOperand->clearState();

    if(LeftOperand->type == any_dt){
        LeftOperand->type = RightOperand.type;
        LeftOperand->clearState();
    }
    if(RightOperand.size() == 0){
        return;
    }
    ReturnType result;
    switch (LeftOperand->type){
        case value_inst:
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->Values[0].copyValue(RightOperand.Values[0]);
                    return;
                case value_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Values.size())){
                        return;
                    }
                    LeftOperand->Values[0].copyValue(RightOperand.Values[0]);
                    return;
                case pointer_inst:
                    LeftOperand->Values[0].setValueFromPointer(RightOperand.BasePointers[0]);
                    return;
                case pointer_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.BasePointers.size())){
                        return;
                    }
                    LeftOperand->Values[0].setValueFromPointer(RightOperand.BasePointers[0]);
                    return;
                case variable_mod:
                    LeftOperand->Values[0].copyValue(RightOperand.Modules.Variables[0]);
                    return;
                case variable_mod_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Variables.size())){
                        return;
                    }
                    LeftOperand->Values[0].copyValue(RightOperand.Modules.Variables[0]);
                    return;
                case vector_mod:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors[0]->getSize())){
                        return;
                    }
                    result = RightOperand.Modules.Vectors[0]->setVariableWithFirstValue(LeftOperand->Values[0]);
                    didFnFailed(CurrentInstr, __FUNCTION__, result);
                    return;
                case vector_mod_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors.size())){
                        return;
                    }
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors[0]->getSize())){
                        return;
                    }
                    result = RightOperand.Modules.Vectors[0]->setVariableWithFirstValue(LeftOperand->Values[0]);
                    didFnFailed(CurrentInstr, __FUNCTION__, result);
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case value_vec:
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->Values.emplace_back(VariableModule());
                    LeftOperand->Values.back().copyValue(RightOperand.Values[0]);
                    return;
                case value_vec:
                    LeftOperand->Values.reserve(LeftOperand->Values.size() + RightOperand.Values.size());
                    for(const VariableModule & Variable : RightOperand.Values){
                        LeftOperand->Values.emplace_back(VariableModule());
                        LeftOperand->Values.back().copyValue(Variable);
                    }
                    return;
                case pointer_inst:
                    LeftOperand->Values.emplace_back(VariableModule());
                    LeftOperand->Values.back().setValueFromPointer(RightOperand.BasePointers[0]);
                    return;
                case pointer_vec:
                    LeftOperand->Values.reserve(LeftOperand->Values.size() + RightOperand.BasePointers.size());
                    for(const BasePointersStruct & Pointer : RightOperand.BasePointers){
                        LeftOperand->Values.emplace_back(VariableModule());
                        LeftOperand->Values.back().setValueFromPointer(Pointer);
                    }
                    return;
                case variable_mod:
                    LeftOperand->Values.emplace_back(VariableModule());
                    LeftOperand->Values.back().copyValue(RightOperand.Modules.Variables[0]);
                    return;
                case variable_mod_vec:
                    LeftOperand->Values.reserve(LeftOperand->Values.size() + RightOperand.Modules.Variables.size());
                    for(VariableModule * Variable : RightOperand.Modules.Variables){
                        LeftOperand->Values.emplace_back(VariableModule());
                        LeftOperand->Values.back().copyValue(Variable);
                    }
                    return;
                case vector_mod:
                    result = RightOperand.Modules.Vectors[0]->getValuesIntoContext(LeftOperand->Values);
                    if(didFnFailed(CurrentInstr, __FUNCTION__, result)){
                        LeftOperand->Values.clear();
                        return;
                    }
                    return;
                case vector_mod_vec:
                    LeftOperand->Values.reserve(LeftOperand->Values.size() + RightOperand.Modules.Vectors.size());
                    for(const VectorModule * Vector : RightOperand.Modules.Vectors){
                        result = Vector->getValuesIntoContext(LeftOperand->Values);
                        if(didFnFailed(CurrentInstr, __FUNCTION__, result)){
                            LeftOperand->Values.clear();
                            return;
                        }
                    }
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case pointer_inst:
            LeftOperand->BasePointers.emplace_back(BasePointersStruct());
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Values[0].getBasePointersStruct(), CurrentInstr);
                    return;
                case value_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Values.size())){
                        return;
                    }
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Values[0].getBasePointersStruct(), CurrentInstr);
                    return;
                case pointer_inst:
                    LeftOperand->BasePointers[0].setPointer(RightOperand.BasePointers[0], CurrentInstr);
                    return;
                case pointer_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Values.size())){
                        return;
                    }
                    LeftOperand->BasePointers[0].setPointer(RightOperand.BasePointers[0], CurrentInstr);
                    return;
                case variable_mod:
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Modules.Variables[0]->getBasePointersStruct(), CurrentInstr);
                    return;
                case variable_mod_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Values.size())){
                        return;
                    }
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Modules.Variables[0]->getBasePointersStruct(), CurrentInstr);
                    return;
                case vector_mod:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors[0]->getSize())){
                        return;
                    }
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Modules.Vectors[0]->getBasePointersStruct(0), CurrentInstr);
                    return;
                case vector_mod_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors.size())){
                        return;
                    }
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors[0]->getSize())){
                        return;
                    }
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Modules.Vectors[0]->getBasePointersStruct(0), CurrentInstr);
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case pointer_vec:
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Values[0].getBasePointersStruct(), CurrentInstr);
                    return;
                case value_vec:
                    LeftOperand->BasePointers.reserve(LeftOperand->BasePointers.size() + RightOperand.Values.size());
                    for(VariableModule & Variable : RightOperand.Values){
                        LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                        LeftOperand->BasePointers.back().setPointer(Variable.getBasePointersStruct(), CurrentInstr);
                    }
                    return;
                case pointer_inst:
                    LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                    LeftOperand->BasePointers[0].setPointer(RightOperand.BasePointers[0], CurrentInstr);
                    return;
                case pointer_vec:
                    LeftOperand->BasePointers.reserve(LeftOperand->BasePointers.size() + RightOperand.BasePointers.size());
                    for(const BasePointersStruct & Pointer : RightOperand.BasePointers){
                        LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                        LeftOperand->BasePointers.back().setPointer(Pointer, CurrentInstr);
                    }
                    return;
                case variable_mod:
                    LeftOperand->BasePointers.push_back(BasePointersStruct());
                    LeftOperand->BasePointers[0].setPointer(RightOperand.Modules.Variables[0]->getBasePointersStruct(), CurrentInstr);
                    return;
                case variable_mod_vec:
                    LeftOperand->BasePointers.reserve(LeftOperand->BasePointers.size() + RightOperand.Modules.Variables.size());
                    for(VariableModule * Variable : RightOperand.Modules.Variables){
                        LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                        LeftOperand->BasePointers.back().setPointer(Variable->getBasePointersStruct(), CurrentInstr);
                    }
                    return;
                case vector_mod:{
                    size_t vectorSize = RightOperand.Modules.Vectors[0]->getSize();
                    LeftOperand->BasePointers.reserve(LeftOperand->BasePointers.size() + vectorSize);
                    for(size_t valueIndex = 0; valueIndex < vectorSize; ++valueIndex){
                        LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                        LeftOperand->BasePointers.back().setPointer(RightOperand.Modules.Vectors[0]->getBasePointersStruct(valueIndex), CurrentInstr);
                    }}
                    return;
                case vector_mod_vec:{
                    size_t newMemoryToReserve = 0;
                    for(VectorModule * Vector : RightOperand.Modules.Vectors){
                        newMemoryToReserve += Vector->getSize();
                    }
                    LeftOperand->BasePointers.reserve(LeftOperand->BasePointers.size() + newMemoryToReserve);
                    size_t vectorSize = 0, valueIndex = 0;
                    for(VectorModule * Vector : RightOperand.Modules.Vectors){
                        vectorSize = Vector->getSize();
                        for(valueIndex = 0; valueIndex < vectorSize; ++valueIndex){
                            LeftOperand->BasePointers.emplace_back(BasePointersStruct());
                            LeftOperand->BasePointers.back().setPointer(Vector->getBasePointersStruct(valueIndex), CurrentInstr);
                        }
                    }}
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case variable_mod:
            switch(RightOperand.type){
                case variable_mod:
                    LeftOperand->Modules.Variables.push_back(RightOperand.Modules.Variables[0]);
                    return;
                case variable_mod_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Variables.size())){
                        return;
                    }
                    LeftOperand->Modules.Variables.push_back(RightOperand.Modules.Variables[0]);
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case variable_mod_vec:
            switch(RightOperand.type){
                case variable_mod:
                    LeftOperand->Modules.Variables.push_back(RightOperand.Modules.Variables[0]);
                    return;
                case variable_mod_vec:
                    LeftOperand->Modules.Variables.insert(LeftOperand->Modules.Variables.end(),
                        RightOperand.Modules.Variables.begin(), RightOperand.Modules.Variables.end()
                    );
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case vector_mod:
            switch(RightOperand.type){
                case vector_mod:
                    LeftOperand->Modules.Vectors.push_back(RightOperand.Modules.Vectors[0]);
                    return;
                case vector_mod_vec:
                    if(doesOperandContainSingleElement(CurrentInstr, __FUNCTION__, LeftOperand->type, RightOperand.type, RightOperand.Modules.Vectors.size())){
                        return;
                    }
                    LeftOperand->Modules.Vectors.push_back(RightOperand.Modules.Vectors[0]);
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        case vector_mod_vec:
            switch(RightOperand.type){
                case vector_mod:
                    LeftOperand->Modules.Vectors.push_back(RightOperand.Modules.Vectors[0]);
                    return;
                case vector_mod_vec:
                    LeftOperand->Modules.Vectors.insert(LeftOperand->Modules.Vectors.end(),
                        RightOperand.Modules.Vectors.begin(), RightOperand.Modules.Vectors.end()
                    );
                    return;
                default:
                    printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
                    return;
            }
            return;
        default:
            if(isEntityTypeEqual(LeftOperand->type, RightOperand.type)){
                LeftOperand->copyOnlyCurrentType(&RightOperand);
                return;
            }
            printAssignRightToLeftError(CurrentInstr, LeftOperand->type, RightOperand.type);
            return;
    }
}
void moveRightToLeft(const InstrDescription & CurrentInstr, const EngineInstr & customInstruction,
    ContextClass * LeftOperand, ContextClass & RightOperand
){
    if(customInstruction == assign){
        assignRightToLeft(CurrentInstr, LeftOperand, RightOperand);
        return;
    }
    if(LeftOperand->readOnly){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Left operand is read-only.\n";
        return;
    }

    auto printMoveRightToLeftError = [](const DataType & leftType, const DataType & rightType,
        const InstrDescription & CurrentInstr, const string &functionName
    ) { 
        cerr << instructionError(CurrentInstr, functionName)
            << "Cannot move a value of '" << dataTypeToStr(rightType)
            << "' type to a variable of '" << dataTypeToStr(leftType) << "' type.\n";
    };
    auto printLeftNotInitialized = [](const string &id, const DataType &type,
        const InstrDescription &CurrentInstr, const string &functionName
    ) { 
        cerr << instructionError(CurrentInstr, functionName)
            << "Left operand '" << id
            << "' of '" << dataTypeToStr(type)
            << "' type was not initialized.\n";
    };

    bool incLeftIdx = false, incRightIdx = false;
    unsigned leftIndex = 0, rightIndex = 0, maxIndex = 0;

    switch(LeftOperand->type){
        case value_inst:
            if(LeftOperand->Values.empty()){
                printLeftNotInitialized(LeftOperand->ID, LeftOperand->type, CurrentInstr,
                    __FUNCTION__
                );
                return;
            }
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->Values[0].move(&RightOperand.Values[0], customInstruction,
                        CurrentInstr
                    );
                    return;
                case value_vec:
                    for(; rightIndex < RightOperand.Values.size(); ++rightIndex){
                        LeftOperand->Values[0].move(&RightOperand.Values[rightIndex],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case pointer_inst:
                    LeftOperand->Values[0].move(&RightOperand.BasePointers[0], customInstruction,
                        CurrentInstr
                    );
                    return;
                case pointer_vec:
                    for(; rightIndex < RightOperand.BasePointers.size(); ++rightIndex){
                        LeftOperand->Values[0].move(&RightOperand.BasePointers[rightIndex],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case variable_mod:
                    LeftOperand->Values[0].move(RightOperand.Modules.Variables[0],
                        customInstruction, CurrentInstr
                    );
                    return;
                case variable_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Variables.size(); ++rightIndex){
                        LeftOperand->Values[0].move(RightOperand.Modules.Variables[rightIndex],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case vector_mod:
                    RightOperand.Modules.Vectors[0]->moveFrom(&LeftOperand->Values[0],
                        customInstruction, CurrentInstr
                    );
                    return;
                case vector_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Vectors.size(); ++rightIndex){
                        RightOperand.Modules.Vectors[rightIndex]->moveFrom(&LeftOperand->Values[0],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr,
                        __FUNCTION__
                    );
                    return;
            }
            return;
        case value_vec:
            switch(RightOperand.type){
                case value_inst:
                    for(; leftIndex < LeftOperand->Values.size(); ++leftIndex){
                        LeftOperand->Values[leftIndex].move(&RightOperand.Values[0],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case value_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Values.size(),
                        RightOperand.Values.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx,
                        rightIndex+=incRightIdx
                    ){
                        LeftOperand->Values[leftIndex].move(&RightOperand.Values[rightIndex],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case pointer_inst:
                    for(; leftIndex < LeftOperand->Values.size(); ++leftIndex){
                        LeftOperand->Values[leftIndex].move(&RightOperand.BasePointers[0],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case pointer_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Values.size(),
                        RightOperand.BasePointers.size(), incLeftIdx, incRightIdx, maxIndex,
                        __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx,
                        rightIndex+=incRightIdx
                    ){
                        LeftOperand->Values[leftIndex].move(&RightOperand.BasePointers[rightIndex],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case variable_mod:
                    for(; leftIndex < LeftOperand->Values.size(); ++leftIndex){
                        LeftOperand->Values[leftIndex].move(RightOperand.Modules.Variables[0],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case variable_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Values.size(),
                        RightOperand.Modules.Variables.size(), incLeftIdx, incRightIdx, maxIndex,
                        __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx,
                        rightIndex+=incRightIdx
                    ){
                        LeftOperand->Values[leftIndex].move(
                            RightOperand.Modules.Variables[rightIndex], customInstruction,
                            CurrentInstr
                        );
                    }
                    return;
                case vector_mod:
                    for(; leftIndex < LeftOperand->Values.size(); ++leftIndex){
                        RightOperand.Modules.Vectors[0]->moveFrom(&LeftOperand->Values[leftIndex],
                            customInstruction, CurrentInstr
                        );
                    }
                    return;
                case vector_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Values.size(),
                        RightOperand.Modules.Vectors.size(), incLeftIdx, incRightIdx, maxIndex,
                        __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx,
                        rightIndex+=incRightIdx
                    ){
                        RightOperand.Modules.Vectors[rightIndex]->moveFrom(
                            &LeftOperand->Values[leftIndex], customInstruction, CurrentInstr
                        );
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr,
                        __FUNCTION__
                    );
                    return;
            }
            return;
        case pointer_inst:
            if(LeftOperand->BasePointers.empty()){
                printLeftNotInitialized(LeftOperand->ID, LeftOperand->type, CurrentInstr,
                    __FUNCTION__
                );
                return;
            }
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->BasePointers[0].move(RightOperand.Values[0].getBasePointersStruct(), customInstruction);
                    return;
                case value_vec:
                    for(; rightIndex < RightOperand.Values.size(); ++rightIndex){
                        LeftOperand->BasePointers[0].move(RightOperand.Values[rightIndex].getBasePointersStruct(), customInstruction);
                    }
                    return;
                case pointer_inst:
                    LeftOperand->BasePointers[0].move(RightOperand.BasePointers[0], customInstruction);
                    return;
                case pointer_vec:
                    for(; rightIndex < RightOperand.BasePointers.size(); ++rightIndex){
                        LeftOperand->BasePointers[0].move(RightOperand.BasePointers[rightIndex], customInstruction);
                    }
                    return;
                case variable_mod:
                    LeftOperand->BasePointers[0].move(RightOperand.Modules.Variables[0]->getBasePointersStruct(), customInstruction);
                    return;
                case variable_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Variables.size(); ++rightIndex){
                        LeftOperand->BasePointers[0].move(RightOperand.Modules.Variables[rightIndex]->getBasePointersStruct(), customInstruction);
                    }
                    return;
                case vector_mod:
                    RightOperand.Modules.Vectors[0]->moveFrom(&LeftOperand->BasePointers[0], customInstruction, CurrentInstr);
                    return;
                case vector_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Vectors.size(); ++rightIndex){    
                        RightOperand.Modules.Vectors[rightIndex]->moveFrom(&LeftOperand->BasePointers[0], customInstruction, CurrentInstr);
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr, __FUNCTION__);
                    return;
            }
            return;
        case pointer_vec:
            switch(RightOperand.type){
                case value_inst:
                    for(; leftIndex < LeftOperand->BasePointers.size(); ++leftIndex){
                        LeftOperand->BasePointers[leftIndex].move(RightOperand.Values[0].getBasePointersStruct(), customInstruction);
                    }
                    return;
                case value_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(),
                        RightOperand.Values.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
                    ){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->BasePointers[leftIndex].move(RightOperand.Values[rightIndex].getBasePointersStruct(), customInstruction);
                    }
                    return;
                case pointer_inst:
                    for(; leftIndex < LeftOperand->BasePointers.size(); leftIndex++){
                        LeftOperand->BasePointers[leftIndex].move(RightOperand.BasePointers[0], customInstruction);
                    }
                    return;
                case pointer_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(),
                        RightOperand.BasePointers.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
                    ){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->BasePointers[leftIndex].move(RightOperand.BasePointers[rightIndex], customInstruction);
                    }
                    return;
                case variable_mod:
                    for(; leftIndex < LeftOperand->BasePointers.size(); ++leftIndex){
                        LeftOperand->BasePointers[leftIndex].move(RightOperand.Modules.Variables[0]->getBasePointersStruct(), customInstruction);
                    }
                    return;
                case variable_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(),
                        RightOperand.Modules.Variables.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
                    ){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->BasePointers[leftIndex].move(RightOperand.Modules.Variables[rightIndex]->getBasePointersStruct(), customInstruction);
                    }
                    return;
                case vector_mod:
                    for(; leftIndex < LeftOperand->BasePointers.size(); ++leftIndex){
                        RightOperand.Modules.Vectors[0]->moveFrom(&LeftOperand->BasePointers[leftIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case vector_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(),
                        RightOperand.Modules.Vectors.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
                    ){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        RightOperand.Modules.Vectors[rightIndex]->moveFrom(&LeftOperand->BasePointers[leftIndex], customInstruction, CurrentInstr);
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr, __FUNCTION__);
                    return;
            }
            return;
        case variable_mod:
            if(LeftOperand->Modules.Variables.empty()){
                printLeftNotInitialized(LeftOperand->ID, LeftOperand->type, CurrentInstr, __FUNCTION__);
                return;
            }
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->Modules.Variables[0]->move(&RightOperand.Values[0], customInstruction, CurrentInstr);
                    return;
                case value_vec:
                    for(; rightIndex < RightOperand.Values.size(); ++rightIndex){
                        LeftOperand->Modules.Variables[0]->move(&RightOperand.Values[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case pointer_inst:
                    LeftOperand->Modules.Variables[0]->move(&RightOperand.BasePointers[0], customInstruction, CurrentInstr);
                    return;
                case pointer_vec:
                    for(; rightIndex < RightOperand.BasePointers.size(); ++rightIndex){
                        LeftOperand->Modules.Variables[0]->move(&RightOperand.BasePointers[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case variable_mod:
                    LeftOperand->Modules.Variables[0]->move(RightOperand.Modules.Variables[0], customInstruction, CurrentInstr);
                    return;
                case variable_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Variables.size(); ++rightIndex){
                        LeftOperand->Modules.Variables[0]->move(RightOperand.Modules.Variables[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case vector_mod:
                    RightOperand.Modules.Vectors[0]->moveFrom(LeftOperand->Modules.Variables[0], customInstruction, CurrentInstr);
                    return;
                case vector_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Vectors.size(); ++rightIndex){
                        RightOperand.Modules.Vectors[rightIndex]->moveFrom(LeftOperand->Modules.Variables[0], customInstruction, CurrentInstr);
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr, __FUNCTION__);
                    return;
            }
            return;
        case variable_mod_vec:
            switch(RightOperand.type){
                case value_inst:
                    for(; leftIndex < LeftOperand->Modules.Variables.size(); ++leftIndex){
                        LeftOperand->Modules.Variables[leftIndex]->move(&RightOperand.Values[0], customInstruction, CurrentInstr);
                    }
                    return;
                case value_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Modules.Variables.size(),
                        RightOperand.Values.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
                    ){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->Modules.Variables[leftIndex]->move(&RightOperand.Values[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case pointer_inst:
                    for(; leftIndex < LeftOperand->Modules.Variables.size(); ++leftIndex){
                        LeftOperand->Modules.Variables[leftIndex]->move(&RightOperand.BasePointers[0], customInstruction, CurrentInstr);
                    }
                    return;
                case pointer_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Modules.Variables.size(),
                        RightOperand.BasePointers.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
                    ){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->Modules.Variables[leftIndex]->move(&RightOperand.BasePointers[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case variable_mod:
                    for(; leftIndex < LeftOperand->Modules.Variables.size(); ++leftIndex){
                        LeftOperand->Modules.Variables[leftIndex]->move(RightOperand.Modules.Variables[0], customInstruction, CurrentInstr);
                    }
                    return;
                case variable_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Modules.Variables.size(),
                        RightOperand.Modules.Variables.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->Modules.Variables[leftIndex]->move(RightOperand.Modules.Variables[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case vector_mod:
                    for(; leftIndex < LeftOperand->Modules.Variables.size(); ++leftIndex){
                        RightOperand.Modules.Vectors[0]->moveFrom(&LeftOperand->Values[leftIndex], customInstruction, CurrentInstr);
                    }
                    return;
                case vector_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Modules.Variables.size(),
                        RightOperand.Modules.Vectors.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        RightOperand.Modules.Vectors[rightIndex]->moveFrom(LeftOperand->Modules.Variables[leftIndex], customInstruction, CurrentInstr);
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr, __FUNCTION__);
                    return;
            }
            return;
        case vector_mod:
            if(LeftOperand->Modules.Vectors.empty()){
                printLeftNotInitialized(LeftOperand->ID, LeftOperand->type, CurrentInstr, __FUNCTION__);
                return;
            }
            switch(RightOperand.type){
                case value_inst:
                    LeftOperand->Modules.Vectors[0]->move(&RightOperand.Values[0],
                        customInstruction, CurrentInstr
                    );
                    return;
                case value_vec:
                    LeftOperand->Modules.Vectors[0]->move(&RightOperand.Values, customInstruction, 
                        CurrentInstr
                    );
                    return;
                case pointer_inst:
                    LeftOperand->Modules.Vectors[0]->move(&RightOperand.BasePointers[0],
                        customInstruction, CurrentInstr
                    );
                    return;
                case pointer_vec:
                    LeftOperand->Modules.Vectors[0]->move(&RightOperand.BasePointers,
                        customInstruction, CurrentInstr
                    );
                    return;
                case variable_mod:
                    LeftOperand->Modules.Vectors[0]->move(RightOperand.Modules.Variables[0],
                        customInstruction, CurrentInstr
                    );
                    return;
                case variable_mod_vec:
                    LeftOperand->Modules.Vectors[0]->move(&RightOperand.Modules.Variables,
                        customInstruction, CurrentInstr
                    );
                    return;
                case vector_mod:
                    LeftOperand->Modules.Vectors[0]->move(RightOperand.Modules.Vectors[0],
                        customInstruction, CurrentInstr
                    );
                    return;
                case vector_mod_vec:
                    for(; rightIndex < RightOperand.Modules.Vectors.size(); ++rightIndex){
                        LeftOperand->Modules.Vectors[0]->move(
                            RightOperand.Modules.Vectors[rightIndex], customInstruction,
                            CurrentInstr
                        );
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr,
                        __FUNCTION__
                    );
                    return;
            }
            return;
        case vector_mod_vec:
            switch(RightOperand.type){
                case value_inst:
                    for(; leftIndex < LeftOperand->Modules.Vectors.size(); ++leftIndex){
                        LeftOperand->Modules.Vectors[leftIndex]->move(&RightOperand.Values[0], customInstruction, CurrentInstr);
                    }
                    return;
                case value_vec:
                    for(; leftIndex < maxIndex; leftIndex+=incLeftIdx){
                        LeftOperand->Modules.Vectors[leftIndex]->move(&RightOperand.Values, customInstruction, CurrentInstr);
                    }
                    return;
                case pointer_inst:
                    for(; leftIndex < LeftOperand->Modules.Vectors.size(); ++leftIndex){
                        LeftOperand->Modules.Vectors[leftIndex]->move(&RightOperand.BasePointers[0], customInstruction, CurrentInstr);
                    }
                    return;
                case pointer_vec:
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->Modules.Vectors[leftIndex]->move(&RightOperand.BasePointers, customInstruction, CurrentInstr);
                    }
                    return;
                case variable_mod:
                    for(; leftIndex < LeftOperand->Modules.Vectors.size(); ++leftIndex){
                        LeftOperand->Modules.Vectors[leftIndex]->move(RightOperand.Modules.Variables[0], customInstruction, CurrentInstr);
                    }
                    return;
                case variable_mod_vec:
                    for(; leftIndex < maxIndex; leftIndex+=incLeftIdx){
                        LeftOperand->Modules.Vectors[leftIndex]->move(&RightOperand.Modules.Variables, customInstruction, CurrentInstr);
                    }
                    return;
                case vector_mod:
                    for(; leftIndex < LeftOperand->Modules.Vectors.size(); ++leftIndex){
                        LeftOperand->Modules.Vectors[leftIndex]->move(RightOperand.Modules.Vectors[0], customInstruction, CurrentInstr);
                    }
                    return;
                case vector_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Modules.Vectors.size(),
                        RightOperand.Modules.Vectors.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__
                    )){
                        return;
                    }
                    for(; leftIndex < maxIndex && rightIndex < maxIndex; leftIndex+=incLeftIdx, rightIndex+=incRightIdx){
                        LeftOperand->Modules.Vectors[leftIndex]->move(RightOperand.Modules.Vectors[rightIndex], customInstruction, CurrentInstr);
                    }
                    return;
                default:
                    printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr, __FUNCTION__);
                    return;
            }
            return;
        default:
            printMoveRightToLeftError(LeftOperand->type, RightOperand.type, CurrentInstr, __FUNCTION__);
            return;
    }
}
void ProcessClass::assignVariable(ObjectMemoryStruct & ObjectMemory, const OutputParameterStruct & Output){
    ContextClass * Variable = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        LocalToGlobalTranslation[Output.localAddress], Output.variableID, false
    );
    if(Variable == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << Output.variableID << "' does not exist.\n";
        return;
    }
    //Variable->clearState();
    if(Variable->type == null_dt){
        Variable->type = NewContext.type;   
    }
    Variable->isPointingToMember = NewContext.isPointingToMember;
    Variable->containerIndex = NewContext.containerIndex;
    moveRightToLeft(CurrentInstr, EngineInstr::assign, Variable, NewContext);        
    
    // NewContext.type = literal;
    // addNewContext(ObjectMemory, NewContext, "value", newContextID, global);
}
void ProcessClass::assignVariable(ObjectMemoryStruct & ObjectMemory, ContextClass * Variable){
    if(Variable == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << Variable->ID << "' does not exist.\n";
        return;
    }
    if(Variable->type == null_dt){
        Variable->type = NewContext.type;   
    }
    switch(Variable->type){
        case bool_inst:
        case bool_vec:
        case int_inst:
        case int_vec:
        case double_inst:
        case double_vec:
        case string_inst:
        case string_vec:
        case value_inst:
        case value_vec:
        case pointer_inst:
        case pointer_vec:
        case any_dt:
            moveRightToLeft(CurrentInstr, EngineInstr::assign, Variable, NewContext);
            return;
        default:
            moveRightToLeft(CurrentInstr, EngineInstr::move, Variable, NewContext);
            return;
    }
}
void ProcessClass::moveToVariable(ObjectMemoryStruct & ObjectMemory,
    const OutputParameterStruct & Output
){
    ContextClass * Variable = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        LocalToGlobalTranslation[Output.localAddress], Output.variableID, false
    );
    if(Variable == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << Output.variableID
            << "' does not exist.\n";
        return;
    }
    if(Variable->type == null_dt){
        Variable->type = NewContext.type;   
    }

    //TODO: Remove next line if nothing breaks. This flag should be changed only when assigning a variable
    //Variable->isPointingToMember = NewContext.isPointingToMember;

    Variable->containerIndex = NewContext.containerIndex;
    moveRightToLeft(CurrentInstr, EngineInstr::move, Variable, NewContext);        
}
void ProcessClass::aggregateValues(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory,
    LayerClass *OwnerLayer, AncestorObject *Owner, const EngineClass & Engine,
    vector<ProcessClass> * Processes
){
    NewContext.clear();
    VariableModule NewValue;
    for(ConditionClass & ValueLocation : Operation.ConditionalChain){
        if(printOutInstructions){
            cout << "inner_find ";
            ValueLocation.Location.print("");
            cout << "\n";
        }
        findNextValue(ValueLocation, Owner, OwnerLayer, Engine, Processes, ObjectMemory, NewValue,
            &NewContext.Values
        );
        if(NewValue.type != 'n'){
            NewContext.Values.emplace_back(NewValue);
        }
    }
    NewContext.type = value_inst;
    if(NewContext.Values.size() > 1){
        NewContext.type = value_vec;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " "
            << Operation.Output.variableID << " "
            << NewContext.getValue(CurrentInstr, maxLengthOfValuesPrinting) << "\n";
    }

    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::aggregateOnlyById(ObjectMemoryStruct & ObjectMemory, OperationClass & Operation,
    LayerClass *OwnerLayer, AncestorObject *Owner
){
    NewContext.clear();
    ContextClass * Context = nullptr;

    if(Operation.Location.source == ValueSource::layer
        || Operation.Location.source == ValueSource::camera
    ){
        if(printOutInstructions){
            cout << "find_by_id (in environment)\n";
            cout << ">inner_find ";
            Operation.Location.print("");
            cout << "\n";
        }
        aggregateCamerasAndLayersById(Operation.Location, NewContext, Owner, OwnerLayer);
    }
    else if(Operation.rootParametersSize > 0){
        if(printOutInstructions){
            cout << "find_by_id (context)<" << Operation.rootParametersSize << ">\n";
        }
        for(unsigned index = 0; index < Operation.rootParametersSize; index++){
            if(getContextPointerFromTheParameter(Context, ObjectMemory, LocalToGlobalTranslation,
                 CurrentInstr, Operation.Parameters, index, true
            )){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Failed to get context from the parameter " << index+1 << ".\n";
                return;
            }
            if(printOutInstructions){
                cout << ">inner_find ";
                Operation.Location.print(Context->ID);
                cout << "\n";
            }
            findLowerContextById(Operation.Location, NewContext, Context);
        }
    }

    if(NewContext.type != null_dt){
        if(printOutInstructions){
            cout << ">found: " << NewContext.getValue(CurrentInstr, maxLengthOfValuesPrinting)
            << "\n";
        }
        assignVariable(ObjectMemory, Operation.Output);
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed.\n";
    }
}

inline bool checkForVectorSize(const InstrDescription & CurrentInstr, size_t leftSize,
    size_t rightSize, bool & sameSize, const string & functionName
){
    if(leftSize == rightSize){
        sameSize = true;
    }
    else if(rightSize != 1){
        cerr << instructionError(CurrentInstr, functionName)
            << "Vector sizes " << leftSize << " (left) and " << rightSize
            << " (right) are incorrect.\n";
        return false;
    }
    return true;
}
template<class Module>
void cloneRightToLeft(vector<Module*> & LeftOperand, vector<Module*> & RightOperand,
    vector<LayerClass> & Layers, bool changeOldID, const InstrDescription & CurrentInstr,
    size_t & topModuleUniqueIndex
){
    bool sameSize = false;
    if(!checkForVectorSize(CurrentInstr, LeftOperand.size(), RightOperand.size(), sameSize,
        __FUNCTION__
    )){
        return;
    }
    unsigned i = 0, j = 0;
    PrimaryData initData = {
        .topIndex = &topModuleUniqueIndex
    };
    for(; i < LeftOperand.size(); i++, j+=sameSize){
        for(LayerClass & Layer : Layers){
            if(LeftOperand[i]->getLayerUniqueIndex() != Layer.getUniqueIndex()){
                continue;
            }
            initData.layerUniqueIndex = Layer.getUniqueIndex();
            initData.newLayerID = Layer.getID();
            for(AncestorObject & Object : Layer.Objects){
                if(LeftOperand[i]->getObjectUniqueIndex() != Object.getUniqueIndex()){
                    continue;
                }
        
                if(isStringInVector(reservedIDs, RightOperand[j]->getID())){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Module with a reserved ID \'" << RightOperand[j]->getID()
                        << "\' cannot be cloned.\n";
                    return;
                }
                if(isStringInVector(reservedIDs, LeftOperand[i]->getID())){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Module with a reserved ID \'" << LeftOperand[i]->getID()
                        << "\' cannot be changed.\n";
                    return;
                }

                initData.objectUniqueIndex = Object.getUniqueIndex();
                initData.newObjectID = Object.getID();
                initData.listOfIDs = &getModuleIdList<Module>(Object);
                
                LeftOperand[i]->clone(*RightOperand[j], initData, changeOldID);
                break;
            }
            break;
        }
    }
}
template<class Module>
inline vector<string> & getModuleIdList(AncestorObject & object){
    if constexpr (std::is_same<Module, SuperTextModule>::value){
        return object.superTextContainerIDs;
    }
    else if constexpr (std::is_same<Module, SuperEditableTextModule>::value){
        return object.superEditableTextContainerIDs;
    }
    else if constexpr (std::is_same<Module, ImageModule>::value){
        return object.imageContainerIDs;
    }
    else if constexpr (std::is_same<Module, MovementModule>::value){
        return object.movementContainerIDs;
    }
    else if constexpr (std::is_same<Module, CollisionModule>::value){
        return object.collisionContainerIDs;
    }
    else if constexpr (std::is_same<Module, ParticleEffectModule>::value){
        return object.particlesContainerIDs;
    }
    else if constexpr (std::is_same<Module, EventModule>::value){
        return object.eventContainerIDs;
    }
    else if constexpr (std::is_same<Module, VariableModule>::value){
        return object.variablesContainerIDs;
    }
    else if constexpr (std::is_same<Module, ScrollbarModule>::value){
        return object.scrollbarContainerIDs;
    }
    else if constexpr (std::is_same<Module, PrimitivesModule>::value){
        return object.primitivesContainerIDs;
    }
    return object.vectorContainerIDs;
}
void ProcessClass::moveValues(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    ContextClass * LeftOperand = nullptr;
    ContextClass RightOperand;

    if(getContextPointerFromTheParameter(LeftOperand, ObjectMemory, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 1.\n";
        return;
    }
    if(LeftOperand->readOnly){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Left operand is read-only.\n";
        return;
    }

    switch(LeftOperand->type){
        case value_inst:
        case value_vec:
        case pointer_inst:
        case pointer_vec:
        case variable_mod:
        case variable_mod_vec:
        case vector_mod:
        case vector_mod_vec:
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Left operand has an invalid type: \'"
                << dataTypeToStr(LeftOperand->type) << "\'.\n";
            return;
    }

    bool negateAfterCopy = false;
    if(Operation.instruction == EngineInstr::inc || Operation.instruction == EngineInstr::dec){
        //Cloning the left operand allows to reuse this function for incrementing and decrementing.
        RightOperand.copyOnlyCurrentType(LeftOperand); 
    }
    else{
        if(RightOperand.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
            CurrentInstr, Operation.Parameters, negateAfterCopy, 1, true
        )){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Parameter 2 does not exist.\n";
            return;
        }
        if(negateAfterCopy){
            RightOperand.collapseToValue();
            for(VariableModule & var : RightOperand.Values){
                var.negate();
            }
        }
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << LeftOperand->ID << ":"
            << LeftOperand->type << ":"
            << LeftOperand->getValue(CurrentInstr, maxLengthOfValuesPrinting)
            << " " << RightOperand.ID << ":" << dataTypeToStr(RightOperand.type) << ":"
            << RightOperand.getValue(CurrentInstr, maxLengthOfValuesPrinting) << "\n";
    }
    
    moveRightToLeft(CurrentInstr, CurrentInstr.instruction, LeftOperand, RightOperand);
}
void ProcessClass::incrementInteger(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    ContextClass * LeftOperand = nullptr;

    if(getContextPointerFromTheParameter(LeftOperand, ObjectMemory, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 1.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << LeftOperand->ID << ":"
            << LeftOperand->type << ":"
            << LeftOperand->getValue(CurrentInstr, maxLengthOfValuesPrinting) << ".\n";
    }
    
    switch(LeftOperand->type){
        case value_inst:
        case value_vec:
            for(VariableModule & Value : LeftOperand->Values){
                ++Value.vInt;
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            for(VariableModule * Value : LeftOperand->Modules.Variables){
                ++Value->vInt;
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            for(VectorModule * Vector : LeftOperand->Modules.Vectors){
                for(int & value : Vector->vInt){
                    ++value;
                }
            }
            break;
        case pointer_inst:
        case pointer_vec:
            for(BasePointersStruct & Pointer : LeftOperand->BasePointers){
                switch(Pointer.type){
                    case char_bt:
                        ++(*Pointer.pChar);
                        break;
                    case short_bt:
                        ++(*Pointer.pShort);
                        break;
                    case u_short_bt:
                        ++(*Pointer.pUShort);
                        break;
                    case int_bt:
                        ++(*Pointer.pInt);
                        break;
                    case u_int_bt:
                        ++(*Pointer.pUInt);
                        break;
                    default:
                        cerr << instructionError(CurrentInstr, __FUNCTION__)
                            << "Cannot increment a pointer of '" << baseTypeToStr(Pointer.type) << "' type.\n";
                        return;
                }
            }
            break;
        default:
            break;
    }
}
inline void cloneEntitiesOfDifferentType(ContextClass * LeftOperand, ContextClass * RightOperand, const InstrDescription & CurrentInstr){
    unsigned i = 0, j = 0;
    bool sameSize = false;
    switch(LeftOperand->type){
        case value_inst:
        case value_vec:
            switch(RightOperand->type){
                case pointer_inst:
                case pointer_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Values.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                        return;
                    }
                    for(; i < LeftOperand->Values.size(); i++, j+=sameSize){
                        LeftOperand->Values[i].setValueFromPointer(RightOperand->BasePointers[j]);
                    }
                    break;
                case variable_mod:
                case variable_mod_vec:
                    break;
                case vector_mod:
                case vector_mod_vec:
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Cannot assign a value of \'" << dataTypeToStr(RightOperand->type)
                        << "\' type to a variable of \'" << dataTypeToStr(LeftOperand->type) << "\' type.\n";
                    break;
            }
            break;
        case pointer_inst:
        case pointer_vec:
            switch(RightOperand->type){
                case value_inst:
                case value_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(), RightOperand->Values.size(), sameSize, __FUNCTION__)){
                        return;
                    }
                    {BaseVariableStruct RightVariable;
                    for(; i < LeftOperand->BasePointers.size(); i++){
                        if(sameSize || i == 0){
                            RightVariable = RightOperand->Values[i].getBaseVariableStruct();
                            if(RightVariable.type == null_bt){
                                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to fetch a variable.\n";
                                if(!sameSize){
                                    return;
                                }
                                continue;
                            }
                        }
                        LeftOperand->BasePointers[i].tryToSetValue(RightVariable);
                    }}
                    break;
                case variable_mod:
                case variable_mod_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(), RightOperand->Modules.Variables.size(), sameSize, __FUNCTION__)){
                        return;
                    }
                    {BaseVariableStruct RightVariable;
                    for(; i < LeftOperand->BasePointers.size(); i++){
                        if(sameSize || i == 0){
                            RightVariable = RightOperand->Modules.Variables[i]->getBaseVariableStruct();
                            if(RightVariable.type == null_bt){
                                cerr << instructionError(CurrentInstr, __FUNCTION__)
                                    << "Failed to fetch a variable.\n";
                                if(!sameSize){
                                    return;
                                }
                                continue;
                            }
                        }
                        LeftOperand->BasePointers[i].tryToSetValue(RightVariable);
                    }}
                    break;
                case vector_mod:
                case vector_mod_vec:
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Cannot assign a value of \'" << dataTypeToStr(RightOperand->type)
                        << "\' type to a variable of \'" << dataTypeToStr(LeftOperand->type) << "\' type.\n";
                    break;
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            switch(RightOperand->type){
                case value_inst:
                case value_vec:
                    break;
                case pointer_inst:
                case pointer_vec:
                    if(!checkForVectorSize(CurrentInstr, LeftOperand->Modules.Variables.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                        return;
                    }
                    for(; i < LeftOperand->Modules.Variables.size(); i++, j+=sameSize){
                        LeftOperand->Modules.Variables[i]->setValueFromPointer(RightOperand->BasePointers[j]);
                    }
                    break;
                case vector_mod:
                case vector_mod_vec:
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Cannot assign a value of \'" << dataTypeToStr(RightOperand->type)
                        << "\' type to a variable of \'" << dataTypeToStr(LeftOperand->type) << "\' type.\n";
                    break;
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            switch(RightOperand->type){
                case value_inst:
                case value_vec:
                    break;
                case pointer_inst:
                case pointer_vec:
                    break;
                case variable_mod:
                case variable_mod_vec:
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Cannot assign a value of \'" << dataTypeToStr(RightOperand->type)
                        << "\' type to a variable of \'" << dataTypeToStr(LeftOperand->type) << "\' type.\n";
                    break;
            }
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Cannot assign a value of \'" << dataTypeToStr(RightOperand->type)
                << "\' type to a variable of \'" << dataTypeToStr(LeftOperand->type) << "\' type.\n";
            break;
    }
}
void ObjectMemoryStruct::cloneMemory(ObjectMemoryStruct & OriginalMemory, AncestorObject * OriginalObject, AncestorObject * ClonedObject,
    LayerClass * CloneLayer, vector<LayerClass> & Layers
){
    *this = OriginalMemory;
    MemoryMap[1].Objects[0] = ClonedObject;
    MemoryMap[2].Layers[0] = CloneLayer;
}
void ProcessClass::cloneEntitiesOfTheSameType(ObjectMemoryStruct & ObjectMemory, ContextClass * LeftOperand, ContextClass * RightOperand,
    AncestorObject *& Owner, vector<AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack, bool & wasNewExecuted, bool & changeOldID
){
    bool sameSize = false;
    switch(LeftOperand->type){
        case pointer_inst:
        case pointer_vec:
            if(!checkForVectorSize(CurrentInstr, LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(unsigned i = 0, j = 0; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
                LeftOperand->BasePointers[i].move(RightOperand->BasePointers[j], EngineInstr::clone_i);
            }
            break;
        case value_inst:
        case value_vec:
            if(!checkForVectorSize(CurrentInstr, LeftOperand->Values.size(), RightOperand->Values.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(unsigned i = 0, j = 0; i < LeftOperand->Values.size(); i++, j+=sameSize){
                LeftOperand->Values[i] = RightOperand->Values[j];
            }
            break;
        case camera_inst:
        case camera_vec:
            if(!checkForVectorSize(CurrentInstr, LeftOperand->Cameras.size(), 
                RightOperand->Cameras.size(), sameSize, __FUNCTION__
            )){
                return;
            }
            for(unsigned i = 0, j = 0; i < LeftOperand->Cameras.size(); i++, j+=sameSize){
                LeftOperand->Cameras[i]->clone(*RightOperand->Cameras[j], camerasIDs, changeOldID);
            }
            break;
        case layer_inst:
        case layer_vec:{
            if(!checkForVectorSize(CurrentInstr, LeftOperand->Layers.size(), 
                RightOperand->Layers.size(), sameSize, __FUNCTION__
            )){
                return;
            }
            PointerRecalculator Recalculator;
            Recalculator.findIndexesForObjects(Layers, ObjectMemory, Owner, TriggeredObjects,
                SelectedLayer, SelectedObject
            );
            Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, 
                MemoryStack, ActiveEditableText, CurrentInstr
            );
            for(unsigned i = 0, j = 0; i < LeftOperand->Layers.size(); i++, j+=sameSize){
                LeftOperand->Layers[i]->clone(*RightOperand->Layers[j], layersIDs, changeOldID, 
                    topObjectUniqueIndex, topModuleUniqueIndex
                );
            }
            Recalculator.updatePointersToObjects(Layers, ObjectMemory, Owner, TriggeredObjects,
                SelectedLayer, SelectedObject, CurrentInstr
            );
            Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, ActiveEditableText, CurrentInstr
            );
            
            //Copy memory of each cloned object with events
            for(unsigned i = 0, j = 0; i < LeftOperand->Layers.size(); i++, j+=sameSize){
                for(size_t objectIdx = 0; objectIdx < RightOperand->Layers[j]->Objects.size(); ++objectIdx){
                    AncestorObject * OriginalObject = &RightOperand->Layers[j]->Objects[objectIdx];
                    if(ProcessMemory.contains(OriginalObject->getUniqueIndex())){
                        AncestorObject * ClonedObject = &LeftOperand->Layers[i]->Objects[objectIdx];
                        ObjectMemoryStruct & OriginalMemory = ProcessMemory[OriginalObject->getUniqueIndex()];
                        ObjectMemoryStruct & ClonedMemory = ProcessMemory[ClonedObject->getUniqueIndex()];
                        ClonedMemory.cloneMemory(OriginalMemory, OriginalObject, ClonedObject, LeftOperand->Layers[i], Layers);
                    }
                }
            }
            wasNewExecuted = true;

            } break;
        case object_inst:
        case object_vec:{
            if(!checkForVectorSize(CurrentInstr, LeftOperand->Objects.size(),
                RightOperand->Objects.size(), sameSize, __FUNCTION__)
            ){
                return;
            }
            PointerRecalculator Recalculator;
            Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, ActiveEditableText, CurrentInstr
            );
            vector<size_t> copiedObjectsLayerIdxs;
            for(unsigned i = 0, j = 0; i < LeftOperand->Objects.size(); i++, j+=sameSize){
                for(LayerClass & Layer : Layers){
                    if(Layer.getID() != LeftOperand->Objects[i]->getLayerID()){
                        continue;
                    }
                    copiedObjectsLayerIdxs.push_back(&Layer - &Layers[0]);
                    LeftOperand->Objects[i]->clone(*RightOperand->Objects[j], Layer.objectsIDs,
                        Layer.getUniqueIndex(), Layer.getID(), changeOldID, topModuleUniqueIndex);
                    break;
                }
            }
            Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, ActiveEditableText, CurrentInstr
            );
            for(unsigned i = 0, j = 0; i < LeftOperand->Objects.size(); i++, j+=sameSize){
                AncestorObject * OriginalObject = RightOperand->Objects[j];
                if(ProcessMemory.contains(OriginalObject->getUniqueIndex())){
                    AncestorObject * ClonedObject = LeftOperand->Objects[i];
                    LayerClass * CloneLayer = &Layers[copiedObjectsLayerIdxs[i]];
                    ObjectMemoryStruct & ClonedMemory = ProcessMemory[
                        ClonedObject->getUniqueIndex()
                    ];
                    ClonedMemory.cloneMemory(ProcessMemory[OriginalObject->getUniqueIndex()],
                        OriginalObject, ClonedObject, CloneLayer, Layers
                    );
                }
            }
            wasNewExecuted = true;
            } break;
        case super_text_mod:
        case super_text_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.SuperTexts, RightOperand->Modules.SuperTexts,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.SuperEditableTexts,
                RightOperand->Modules.SuperEditableTexts, Layers, changeOldID, CurrentInstr,
                topModuleUniqueIndex
                
            );
            break;
        case image_mod:
        case image_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Images, RightOperand->Modules.Images, Layers,
                changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case movement_mod:
        case movement_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Movements, RightOperand->Modules.Movements,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case collision_mod:
        case collision_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Collisions, RightOperand->Modules.Collisions,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case particles_mod:
        case particles_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Particles, RightOperand->Modules.Particles,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case event_mod:
        case event_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Events, RightOperand->Modules.Events, Layers,
                changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            wasNewExecuted = true;
            break;
        case variable_mod:
        case variable_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Variables, RightOperand->Modules.Variables,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Scrollbars, RightOperand->Modules.Scrollbars,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case primitives_mod:
        case primitives_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Primitives, RightOperand->Modules.Primitives,
                Layers, changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        case vector_mod:
        case vector_mod_vec:
            cloneRightToLeft(LeftOperand->Modules.Vectors, RightOperand->Modules.Vectors, Layers,
                changeOldID, CurrentInstr, topModuleUniqueIndex
            );
            break;
        default:
            break;
    }
}
void ProcessClass::cloneEntities(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory,
    vector<LayerClass> &Layers, AncestorObject *& Owner, vector<AncestorObject*> & TriggeredObjects,
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & MemoryStack
){
    if(Operation.rootParametersSize < 3){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction requires at least 3 parameters.\n";
        return;
    }
    
    ContextClass * LeftOperand = nullptr;
    if(getContextPointerFromTheParameter(LeftOperand, ObjectMemory, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 1.\n";
        return;
    }
    ContextClass * RightOperand = nullptr;
    if(getContextPointerFromTheParameter(RightOperand, ObjectMemory, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, 1, true)
    ){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 2.\n";
        return;
    }

    bool changeOldID = false;
    if(getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 2, changeOldID, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a bool value from the parameter 3.\n";
        return;
    }

    if(printOutInstructions){
        cout << "clone " << LeftOperand->ID << ":" << dataTypeToStr(LeftOperand->type) << ":"
            << LeftOperand->getValue(CurrentInstr, maxLengthOfValuesPrinting) << " "
            << RightOperand->ID << ":" << dataTypeToStr(RightOperand->type) << ":"
            << RightOperand->getValue(CurrentInstr, maxLengthOfValuesPrinting) << " "
            << changeOldID << "\n";
    }

    if(LeftOperand->readOnly){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Left operand is read-only.\n";
        return;
    }

    if(LeftOperand->type == RightOperand->type){
        cloneEntitiesOfTheSameType(ObjectMemory, LeftOperand, RightOperand, Owner, TriggeredObjects,
             startingEventIt,
            eventIt, MemoryStack, wasNewExecuted, changeOldID
        );
    }
    else{
        cloneEntitiesOfDifferentType(LeftOperand, RightOperand, CurrentInstr);
    }
}
inline void printValuesForDebug(const vector<VariableModule> & values){
    if(values.size() == 1){
        cout << values[0].getAnyValue();
    }
    else{
        cout << "[";
        for(const VariableModule & val : values){
            cout << val.getAnyValue() << ", ";
        }
        cout << "]";
    }
}
inline void printValuesForDebug(const vector<double> & values){
    if(values.size() == 1){
        cout << values[0];
    }
    else{
        cout << "[";
        for(const double & val : values){
            cout << val << ", ";
        }
        cout << "]";
    }
}
void ProcessClass::executeArithmetics(OperationClass & Operation, ObjectMemoryStruct& ObjectMemory){
    vector<VariableModule> leftValues;
    if(getValuesFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 0, leftValues, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get values from the parameter 1.\n";
        return;
    }

    if(leftValues.empty()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 1 is empty. Aborting.\n";
        return;
    }

    vector<VariableModule> rightValues;
    if(getValuesFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 1, rightValues, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get values from the parameter 2.\n";
        return;
    }

    if(rightValues.empty()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 2 is empty. Aborting.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        printValuesForDebug(leftValues);
        cout << " ";
        printValuesForDebug(rightValues);
        cout << " " << Operation.Output.variableID << "\n";
    }

    NewContext.clear();
    BaseVariableStruct result;
    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(!checkForVectorSize(CurrentInstr, leftValues.size(),
        rightValues.size(), sameSize, __FUNCTION__
    )){
        return;
    }

    for(; i < leftValues.size(); i++, j+=sameSize){
        if(leftValues[i].getType() == 'd'
            || rightValues[j].getType() == 'd'
        ){
            NewContext.Values.emplace_back(VariableModule());
            NewContext.Values.back().setDouble(leftValues[i].floatingOperation(
                Operation.instruction, &rightValues[j]
            ));
        }
        else if(leftValues[i].getType() != 's' || rightValues[j].getType() != 's'){
            NewContext.Values.emplace_back(VariableModule());
            NewContext.Values.back().setInt(leftValues[i].intOperation(
                Operation.instruction, &rightValues[j]
            ));
        }
        else{
            NewContext.Values.emplace_back(VariableModule());
            NewContext.Values.back().setString(leftValues[i].stringOperation(Operation.instruction,
                &rightValues[j]
            ));
        }
    }

    NewContext.type = value_inst;
    if(NewContext.Values.size() > 1){
        NewContext.type = value_vec;
    }
    moveToVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::createLiteral(const OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory
){
    NewContext.clear();
    vector<VariableModule> NewVariables;

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        if(Operation.Output.variableID != ""){
            cout << Operation.Output.variableID << " ";
        }
        cout << "{";
    }   

    for(unsigned index = 0; index < Operation.rootParametersSize; index++){
        if(getValuesFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext,
            CurrentInstr, Operation.Parameters, index, NewVariables, true
        )){
            return;
        }
        if(printOutInstructions){
            cout << "[";
            for(const VariableModule & Value : NewVariables){
                cout << shortenText(Value.getAnyValue(), maxLengthOfValuesPrinting) << ", ";
            }
            cout << "], ";
        }

        for(VariableModule & rawVal : NewVariables){
            switch(Operation.instruction){
                case bool_i:
                case bool_vec_i:
                    rawVal.vBool = rawVal.getBool();
                    rawVal.setType('b');
                    break;
                case int_i:
                case int_vec_i:
                    rawVal.vInt = rawVal.getInt();
                    rawVal.setType('i');
                    break;
                case double_i:
                case double_vec_i:
                    rawVal.vDouble = rawVal.getDouble();
                    rawVal.setType('d');
                    break;
                case string_i:
                case string_vec_i:
                    rawVal.vString = rawVal.getString();
                    rawVal.setType('s');
                    break;
                default:
                    break;
            }
        }
        
        NewContext.Values.insert(NewContext.Values.end(), NewVariables.begin(), NewVariables.end());
    }

    if(printOutInstructions){
        cout << "}\n";
    }

    NewContext.type = value_inst;
    if(NewContext.Values.size() > 1){
        NewContext.type = value_vec;
    }
    assignVariable(ObjectMemory, Operation.Output);
}
inline int randomIntWithNegation(int minValue, int maxValue, bool negateMin, bool negateMax){
    if(negateMin)
        minValue *= -1;
    if(negateMax)
        maxValue *= -1;
    return randomInt(minValue, maxValue);
}
void ProcessClass::generateRandomValue(const OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, bool isDouble
){
    vector<double> minValues;
    if(getDoubleVectorFromTheParameterV2(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 0, minValues, false
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get value from the parameter 1.\n";
        return;
    }

    vector<double> maxValues;
    if(getDoubleVectorFromTheParameterV2(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 1, maxValues, false
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get value from the parameter 1.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        printValuesForDebug(minValues);
        cout << " ";
        printValuesForDebug(maxValues);
        cout << " " << Operation.Output.variableID << "\n";
    }

    bool sameSize = false;
    if(!checkForVectorSize(CurrentInstr, minValues.size(), maxValues.size(), sameSize,
        __FUNCTION__
    )){
        return;
    }

    NewContext.clear();
    VariableModule Result;
    unsigned i = 0, j = 0;

    if(!isDouble){
        Result.setType('i');
        for(; i < minValues.size(); i++, j+=sameSize){
            Result.vInt = randomInt(minValues[i], maxValues[j]);
            NewContext.Values.push_back(Result);
        }
    }
    else{
        Result.setType('d');
        for(; i < minValues.size(); i++, j+=sameSize){
            Result.vDouble = randomDouble(minValues[i], maxValues[j]);
            NewContext.Values.push_back(Result);
        }
    }

    NewContext.type = NewContext.Values.size() <= 1 ? value_inst : value_vec;

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " "
            << NewContext.getValue(CurrentInstr, maxLengthOfValuesPrinting)
            << " " << Operation.Output.variableID << "\n";
    }

    assignVariable(ObjectMemory, Operation.Output);
}
template <class Module>
bool containsTheSameModule(const vector<Module> & LeftModules, const vector<Module> & RightModules){
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
inline void checkIfVectorContainsVectorOfTheSameType(ContextClass & LeftOperand,
    ContextClass & RightOperand, bool & result, const InstrDescription & CurrentInstr
){
    unsigned i = 0, j = 0;
    switch(LeftOperand.type){
        case pointer_inst:
        case pointer_vec:
            for(i = 0; i < LeftOperand.BasePointers.size(); i++){
                for(j = 0; j < RightOperand.BasePointers.size(); j++){
                    if(LeftOperand.BasePointers[i].areEqual(&RightOperand.BasePointers[j])){
                        result = true;
                        break;
                    }
                }
            }
            break;
        case value_inst:
        case value_vec:
            for(i = 0; i < LeftOperand.Values.size(); i++){
                for(j = 0; j < RightOperand.Values.size(); j++){
                    if(cmpVars(&LeftOperand.Values[i], equal, &RightOperand.Values[j], CurrentInstr)){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
            break;
        case camera_inst:
        case camera_vec:
            for(i = 0; i < LeftOperand.Cameras.size(); i++){
                for(j = 0; j < RightOperand.Cameras.size(); j++){
                    if(LeftOperand.Cameras[i]->getID() == RightOperand.Cameras[j]->getID()){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
            break;
        case layer_inst:
        case layer_vec:
            for(i = 0; i < LeftOperand.Layers.size(); i++){
                for(j = 0; j < RightOperand.Layers.size(); j++){
                    if(LeftOperand.Layers[i]->getID() == RightOperand.Layers[j]->getID()){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
            break;
        case object_inst:
        case object_vec:
            for(i = 0; i < LeftOperand.Objects.size(); i++){
                for(j = 0; j < RightOperand.Objects.size(); j++){
                    if(LeftOperand.Objects[i]->getID() == RightOperand.Objects[j]->getID()
                        && LeftOperand.Objects[i]->getLayerID() == RightOperand.Objects[j]->getLayerID()
                    ){
                        result = true;
                        break;
                    }
                }
                if(result){
                    break;
                }
            }
            break;
        case super_text_mod:
        case super_text_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.SuperTexts, RightOperand.Modules.SuperTexts);
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.SuperEditableTexts, RightOperand.Modules.SuperEditableTexts);
            break;
        case image_mod:
        case image_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Images, RightOperand.Modules.Images);
            break;
        case movement_mod:
        case movement_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Movements, RightOperand.Modules.Movements);
            break;
        case collision_mod:
        case collision_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Collisions, RightOperand.Modules.Collisions);
            break;
        case particles_mod:
        case particles_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Particles, RightOperand.Modules.Particles);
            break;
        case event_mod:
        case event_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Events, RightOperand.Modules.Events);
            break;
        case variable_mod:
        case variable_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Variables, RightOperand.Modules.Variables);
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Scrollbars, RightOperand.Modules.Scrollbars);
            break;
        case primitives_mod:
        case primitives_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Primitives, RightOperand.Modules.Primitives);
            break;
        case vector_mod:
        case vector_mod_vec:
            result = containsTheSameModule(LeftOperand.Modules.Vectors, RightOperand.Modules.Vectors);
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "\'"
                << dataTypeToStr(LeftOperand.type) << "\' type does not exist.\n";
            break;
    }
}
inline void checkIfVectorContainsVectorOfDifferentType(ContextClass & LeftOperand,
    ContextClass & RightOperand, bool & result, const InstrDescription & CurrentInstr
){
    unsigned i = 0, j = 0;
    switch(LeftOperand.type){
        case value_inst:
        case value_vec:
            switch(RightOperand.type){
                case value_inst:
                case value_vec:
                    break;
                case pointer_inst:
                case pointer_vec:
                    for(i = 0; i < LeftOperand.Values.size(); i++){
                        for(j = 0; j < RightOperand.BasePointers.size(); j++){
                            if(areEqual(&LeftOperand.Values[i], RightOperand.BasePointers[j], CurrentInstr)){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }
                    break;
                case variable_mod:
                case variable_mod_vec:
                    break;
                case vector_mod:
                case vector_mod_vec:
                    for(i = 0; i < LeftOperand.Values.size(); i++){
                        for(j = 0; j < RightOperand.Modules.Vectors.size(); j++){
                            if(RightOperand.Modules.Vectors[j]->contains(LeftOperand.Values[i])){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Cannot assign a value of \'"
                        << dataTypeToStr(RightOperand.type) << "\' type to a variable of \'"
                        << dataTypeToStr(LeftOperand.type) << "\' type.\n";
                    break;
            }
            break;
        case pointer_inst:
        case pointer_vec:
            switch(RightOperand.type){
                case value_inst:
                case value_vec:{
                    BaseVariableStruct RightVariable;
                    for(i = 0; i < LeftOperand.BasePointers.size(); i++){
                        for(j = 0; j < RightOperand.Values.size(); j++){
                            RightVariable = RightOperand.Values[j].getBaseVariableStruct();
                            if(RightVariable.type == null_bt){
                                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to fetch a variable.\n";
                                continue;
                            }
                            if(LeftOperand.BasePointers[i].areEqual(&RightVariable)){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }}
                    break;
                case pointer_inst:
                case pointer_vec:
                    break;
                case variable_mod:
                case variable_mod_vec:{
                    BaseVariableStruct RightVariable;
                    for(i = 0; i < LeftOperand.BasePointers.size(); i++){
                        for(j = 0; j < RightOperand.Modules.Variables.size(); j++){
                            RightVariable = RightOperand.Modules.Variables[j]->getBaseVariableStruct();
                            if(RightVariable.type == null_bt){
                                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to fetch a variable.\n";
                                continue;
                            }
                            if(LeftOperand.BasePointers[i].areEqual(&RightVariable)){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }}
                    break;
                case vector_mod:
                case vector_mod_vec:
                    for(i = 0; i < LeftOperand.BasePointers.size(); i++){
                        for(j = 0; j < RightOperand.Modules.Vectors.size(); j++){
                            if(RightOperand.Modules.Vectors[j]->contains(LeftOperand.BasePointers[i])){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Cannot assign a value of \'"
                        << dataTypeToStr(RightOperand.type) << "\' type to a variable of \'"
                        << dataTypeToStr(LeftOperand.type) << "\' type.\n";
                    break;
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            switch(RightOperand.type){
                case value_inst:
                case value_vec:
                    for(i = 0; i < LeftOperand.Modules.Variables.size(); i++){
                        for(j = 0; j < RightOperand.Values.size(); j++){
                            if(cmpVars(LeftOperand.Modules.Variables[i], equal, &RightOperand.Values[j], CurrentInstr)){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }
                    break;
                case pointer_inst:
                case pointer_vec:
                    for(i = 0; i < LeftOperand.Modules.Variables.size(); i++){
                        for(j = 0; j < RightOperand.BasePointers.size(); j++){
                            if(areEqual(LeftOperand.Modules.Variables[i], RightOperand.BasePointers[j], CurrentInstr)){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }
                    break;
                case variable_mod:
                case variable_mod_vec:
                    break;
                case vector_mod:
                case vector_mod_vec:
                    for(i = 0; i < LeftOperand.Modules.Variables.size(); i++){
                        for(j = 0; j < RightOperand.Modules.Vectors.size(); j++){
                            if(RightOperand.Modules.Vectors[j]->contains(*LeftOperand.Modules.Variables[i])){
                                result = true;
                                break;
                            }
                        }
                        if(result){
                            break;
                        }
                    }
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Cannot assign a value of \'"
                        << dataTypeToStr(RightOperand.type) << "\' type to a variable of \'"
                        << dataTypeToStr(LeftOperand.type) << "\' type.\n";
                    break;
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            switch(RightOperand.type){
                case value_inst:
                case value_vec:
                    break;
                case pointer_inst:
                case pointer_vec:
                    break;
                case variable_mod:
                case variable_mod_vec:
                    break;
                case vector_mod:
                case vector_mod_vec:
                    break;
                default:
                    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Cannot assign a value of \'"
                        << dataTypeToStr(RightOperand.type) << "\' type to a variable of \'"
                        << dataTypeToStr(LeftOperand.type) << "\' type.\n";
                    break;
            }
            break;
        default:
            break;
    }
}
void ProcessClass::checkIfVectorContainsVector(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory
){
    ContextClass LeftOperand;
    ContextClass RightOperand;

    bool negateAfterCopy = false; //ignore
    if(LeftOperand.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get the parameter 1.\n";
        return;
    }

    if(RightOperand.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 1, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get the parameter 2.\n";
        return;
    }


    if(printOutInstructions){
        cout << "in " << LeftOperand.ID << ":" << dataTypeToStr(LeftOperand.type) << ":"
            << LeftOperand.getValue(CurrentInstr, maxLengthOfValuesPrinting) << " "
            << RightOperand.ID << ":" << dataTypeToStr(RightOperand.type) << ":"
            << RightOperand.getValue(CurrentInstr, maxLengthOfValuesPrinting) << "\n";
    }

    bool result = false;

    if(LeftOperand.type == RightOperand.type){
        checkIfVectorContainsVectorOfTheSameType(LeftOperand, RightOperand, result, CurrentInstr);
    }
    else{
        checkIfVectorContainsVectorOfDifferentType(LeftOperand, RightOperand, result, CurrentInstr);
    }

    NewContext.clear();
    NewContext.Values.emplace_back(VariableModule::newBool(result));
    NewContext.type = value_inst;
    if(NewContext.Values.size() > 1){
        NewContext.type = value_vec;
    }
    assignVariable(ObjectMemory, Operation.Output);
}
template <class Module>
void createNewModule(vector<Module> & Container, vector<Module*> & Context, PrimaryData & initData,
    unsigned newVectorSize, const vector<string> & newIDs, vector<LayerClass> & Layers,
    ObjectMemoryStruct & ObjectMemory, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack,
    double reservationMultiplier, SuperEditableTextModule *& ActiveEditableText,
    const InstrDescription & CurrentInstr
){
    if(Container.size() + newVectorSize > Container.capacity()){
        PointerRecalculator Recalculator;
        Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, 
            MemoryStack, ActiveEditableText, CurrentInstr
        );
        size_t newCapacity = static_cast<std::size_t>(
            (Container.size() + newVectorSize) * reservationMultiplier
        );
        Container.reserve(newCapacity);
        Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt, 
            MemoryStack, ActiveEditableText, CurrentInstr
        );
    }
    for(unsigned i = 0; i < newVectorSize; i++){
        if(i < newIDs.size()){
            initData.newID = newIDs[i];
        }
        Container.emplace_back(Module(initData));
        Context.push_back(&Container.back());
    }
}
bool ProcessClass::prepareVectorSizeAndIDsForNew(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, unsigned & newVectorSize, vector<string> & newIDs
){
    bool skipOneParameter = false;
    
    if(Operation.Parameters[0].Literal.getString() == "variable"){
        skipOneParameter = true;
    }

    if(getUnsignedFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext,
        LeftOperandProc, CurrentInstr, Operation.Parameters, skipOneParameter + 1, newVectorSize,
        false
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to get an unsigned int.\n";
        return false;
    }

    getStringVectorFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext,
        CurrentInstr, Operation.Parameters, skipOneParameter + 2, newIDs, false
    );

    return true;
}
bool ProcessClass::prepareDestinationForNew(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject,
    PrimaryData & initData, vector<LayerClass> &Layers
){
    if(Operation.Parameters[0].type != 'l'){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Destination type was not provided - the first string value of the instruction.\n";
        return false;
    }
    
    if(Operation.Location.source == ValueSource::camera
        || Operation.Location.source == ValueSource::layer
    ){
        return true;
    }

    if(Operation.Parameters[0].Literal.getString() == "location"
        && Operation.Location.layerID != ""
    ){
        for(LayerClass & Layer : Layers){
            if(Layer.getID() != Operation.Location.layerID){
                continue;
            }
            if(!Layer.getIsDeleted()){
                CurrentLayer = &Layer;
                initData.layerUniqueIndex = CurrentLayer->getUniqueIndex();
                initData.newLayerID = CurrentLayer->getID();
                if(Operation.Location.source == ValueSource::object){
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
                    initData.objectUniqueIndex = CurrentObject->getLayerUniqueIndex();
                    initData.layerUniqueIndex = CurrentLayer->getUniqueIndex();
                    initData.newObjectID = CurrentObject->getID();
                }
                break;
            }
        }   
    }
    else if(Operation.Parameters[0].Literal.getString() == "variable"){
        ContextClass Context;
        bool negateAfterCopy = false; //ignore
        if(Context.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
            CurrentInstr, Operation.Parameters, negateAfterCopy, 1, true
        )){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "No context found.\n";
            return false;
        }
        if((Context.type == layer_inst || Context.type == layer_vec)
            && Operation.Location.source == object
        ){
            if(Context.Layers.empty()){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Context has no layers.\n";
                return false;
            }
            CurrentLayer = Context.Layers.back();
            initData.layerUniqueIndex = CurrentLayer->getUniqueIndex();
            initData.newLayerID = CurrentLayer->getID();
            return true;
        }
        else if((Context.type == object_inst || Context.type == object_vec)
            && Operation.Location.source != object
        ){
            if(Context.Objects.empty()){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Context has no objects.\n";
                return false;
            }
            CurrentObject = Context.Objects.back();
            initData.objectUniqueIndex = CurrentObject->getUniqueIndex();
            initData.layerUniqueIndex = CurrentObject->getLayerUniqueIndex();
            initData.newObjectID = CurrentObject->getID();
            initData.newLayerID = CurrentObject->getLayerID();
            return true;
        }
        else{
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Cannot create an entity of \'"
                << sourceToStr(Operation.Location.source) << "\' type in a context of \'"
                << dataTypeToStr(Context.type) << "\' type.\n";
            return false;
        }
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Wrong destination type.\n";
        return false;
    }

    if(CurrentLayer == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Layer \'"
            << Operation.Location.layerID << "\' does not exist.\n";
        return false;
    }

    if(CurrentObject == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object \'"
            << Operation.Location.objectID << "\' does not exist.\n";
        return false;
    }

    return true;
}
void ProcessClass::assignEntities(ObjectMemoryStruct & ObjectMemory, ContextClass & NewValue,
    OutputParameterStruct & Output
){
    if(Output.localAddress >= LocalToGlobalTranslation.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Local address '"
            << Output.localAddress << "' is out of scope.\n";
        return;
    }
    ContextClass * Variable = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        LocalToGlobalTranslation[Output.localAddress], Output.variableID, false
    );
    if(Variable == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << Output.variableID
            << "' does not exist.\n";
        return;
    }

    Variable->clearState();
    if(Variable->type == null_dt || Variable->type == any_dt){
        Variable->type = NewValue.type;
        Output.type = NewValue.type;
    }
    else if(Variable->type != NewValue.type){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Cannot assign an entity of '" << dataTypeToStr(NewValue.type)
            << "' type to the entity of '" << dataTypeToStr(Variable->type) << "' type.\n";
        return;
    }

    switch (Variable->type){
        case camera_inst:
        case camera_vec:
            for(Camera2D * Camera : NewValue.Cameras){
                Variable->Cameras.push_back(Camera);
            }
            break;
        case layer_inst:
        case layer_vec:
            for(LayerClass * Layer : NewValue.Layers){
                Variable->Layers.push_back(Layer);
            }
            break;
        case object_inst:
        case object_vec:
            for(AncestorObject * Object : NewValue.Objects){
                Variable->Objects.push_back(Object);
            }
            break;
        case super_text_mod:
        case super_text_mod_vec:
            for(SuperTextModule * SuperText : NewValue.Modules.SuperTexts){
                Variable->Modules.SuperTexts.push_back(SuperText);
            }
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            for(SuperEditableTextModule * SuperEditableText : NewValue.Modules.SuperEditableTexts){
                Variable->Modules.SuperEditableTexts.push_back(SuperEditableText);
            }
            break;
        case image_mod:
        case image_mod_vec:
            for(ImageModule * Image : NewValue.Modules.Images){
                Variable->Modules.Images.push_back(Image);
            }
            break;
        case movement_mod:
        case movement_mod_vec:
            for(MovementModule * Movement : NewValue.Modules.Movements){
                Variable->Modules.Movements.push_back(Movement);
            }
            break;
        case collision_mod:
        case collision_mod_vec:
            for(CollisionModule * Collision : NewValue.Modules.Collisions){
                Variable->Modules.Collisions.push_back(Collision);
            }
            break;
        case particles_mod:
        case particles_mod_vec:
            for(ParticleEffectModule * Particles : NewValue.Modules.Particles){
                Variable->Modules.Particles.push_back(Particles);
            }
            break;
        case event_mod:
        case event_mod_vec:
            for(EventModule * Event : NewValue.Modules.Events){
                Variable->Modules.Events.push_back(Event);
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            for(VariableModule * VariablePointer : NewValue.Modules.Variables){
                Variable->Modules.Variables.push_back(VariablePointer);
            }
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            for(ScrollbarModule * Scrollbar : NewValue.Modules.Scrollbars){
                Variable->Modules.Scrollbars.push_back(Scrollbar);
            }
            break;
        case primitives_mod:
        case primitives_mod_vec:
            for(PrimitivesModule * Primitive : NewValue.Modules.Primitives){
                Variable->Modules.Primitives.push_back(Primitive);
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            for(VectorModule * Vector : NewValue.Modules.Vectors){
                Variable->Modules.Vectors.push_back(Vector);
            }
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << dataTypeToStr(Output.type) << "\' does not exist.\n";
            break;
    }
}
void ProcessClass::reserveMemoryForNewLayers(ObjectMemoryStruct & ObjectMemory,
    LayerClass *& OwnerLayer, AncestorObject *& Owner, vector<AncestorObject*> & TriggeredObjects,
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & MemoryStack, unsigned newVectorSize
){
    if(Layers.size() + newVectorSize <= Layers.capacity()){
        return;
    }
    PointerRecalculator Recalculator;
    Recalculator.findIndexesForLayers(Layers, ObjectMemory, OwnerLayer);
    Recalculator.findIndexesForObjects(Layers, ObjectMemory, Owner, TriggeredObjects, SelectedLayer,
        SelectedObject
    );
    Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, MemoryStack,
        ActiveEditableText, CurrentInstr
    );
    size_t newCapacity = static_cast<std::size_t>(
        (Layers.size() + newVectorSize) * reservationMultiplier
    );
    Layers.reserve(newCapacity);
    Recalculator.updatePointersToLayers(Layers, ObjectMemory, OwnerLayer, CurrentInstr);
    Recalculator.updatePointersToObjects(Layers, ObjectMemory, Owner, TriggeredObjects,
        SelectedLayer, SelectedObject, CurrentInstr
    );
    Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
        MemoryStack, ActiveEditableText, CurrentInstr
    );
    
    //Invalidate memory in all different objects
    for(LayerClass & layerIt : Layers){
        for(AncestorObject & objectIt : layerIt.Objects){
            objectIt.hasInvalidatedMemory = true;
        }
    }
    Owner->hasInvalidatedMemory = false;
}
void ProcessClass::reserveMemoryForNewObjects(ObjectMemoryStruct & ObjectMemory,
    LayerClass *& OwnerLayer, LayerClass *& CurrentLayer, AncestorObject *& Owner,
    vector<AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack,
    unsigned newVectorSize
){
    if(CurrentLayer->Objects.size() + newVectorSize <= CurrentLayer->Objects.capacity()){
        return;
    }
    PointerRecalculator Recalculator;
    Recalculator.findIndexesForObjects(Layers, ObjectMemory, Owner, TriggeredObjects, SelectedLayer,
        SelectedObject
    );
    Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, MemoryStack,
        ActiveEditableText, CurrentInstr
    );
    size_t newCapacity = static_cast<std::size_t>(
        (CurrentLayer->Objects.size() + newVectorSize) * reservationMultiplier
    );
    CurrentLayer->Objects.reserve(newCapacity);
    Recalculator.updatePointersToObjects(Layers, ObjectMemory, Owner, TriggeredObjects,
        SelectedLayer, SelectedObject, CurrentInstr
    );
    Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
        MemoryStack, ActiveEditableText, CurrentInstr
    );
    for(AncestorObject & objectIt : OwnerLayer->Objects){
        objectIt.hasInvalidatedMemory = true;
    }
    Owner->hasInvalidatedMemory = false;
}
void ProcessClass::createNewEntities(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory,
    LayerClass *& OwnerLayer, AncestorObject *& Owner, vector<AncestorObject*> & TriggeredObjects,
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & MemoryStack, string & focusedProcessID
){
    if(Operation.rootParametersSize == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No parameters provided.\n";
        return;
    }
    
    LayerClass * CurrentLayer = nullptr;
    AncestorObject * CurrentObject = nullptr;
    PrimaryData initData = {};

    if(!prepareDestinationForNew(Operation, ObjectMemory, CurrentLayer, CurrentObject, initData,
        Layers
    )){
        return;
    }
    
    unsigned newVectorSize = 0;
    vector<string> newIDs;

    if(!prepareVectorSizeAndIDsForNew(Operation, ObjectMemory, newVectorSize, newIDs)){
        return;
    }

    if(newVectorSize == 0){
        cout << instructionWarning(CurrentInstr, __FUNCTION__)
            << "No new entity will be created - the count of new entities is zero.\n";
        return;
    }

    string ID = "";

    if(printOutInstructions){
        cout << "new ";
        Operation.Location.print("");
        cout << newVectorSize << " ";
        if(newIDs.size() > 0){
            cout << "[";
            for(const string & newID : newIDs){
                cout << newID << ", ";
            }
            cout << "]<" << newIDs.size() << ">";
        }
        cout << Operation.Output.variableID << "\n";
    }

    NewContext.clear();

    NewContext.type = sourceToEntityType(CurrentInstr, Operation.Location.source);
    if(newVectorSize > 1){
        NewContext.type = vectorizeEntityDataType(CurrentInstr, NewContext.type);
    }

    switch(Operation.Location.source){
        case super_text:
        case super_editable_text:
        case image:
        case movement:
        case collision:
        case particles:
        case event:
        case variable:
        case scrollbar:
        case primitives:
        case vector_s:
            initData.topIndex = &topModuleUniqueIndex;
            break;
        default:
            break;
    }
    
    switch (Operation.Location.source){
        case camera:
            if(Cameras.size() + newVectorSize > Cameras.capacity()){
                for(LayerClass & Layer : Layers){
                    Layer.nullifyAllPointers();
                }
                PointerRecalculator Recalculator;
                Recalculator.findIndexesForCameras(Cameras, ObjectMemory, SelectedCamera);
                size_t newCapacity = static_cast<std::size_t>(
                    (Cameras.size() + newVectorSize) * reservationMultiplier
                );
                Cameras.reserve(newCapacity);
                Recalculator.updatePointersToCameras(Cameras, ObjectMemory, SelectedCamera, getID(),
                    focusedProcessID, CurrentInstr
                );
            }
            for(unsigned i = 0; i < newVectorSize; i++){
                if(i < newIDs.size()){
                    ID = newIDs[i];
                }
                Cameras.emplace_back(Camera2D());
                Cameras.back().setUpInstance(ID, camerasIDs, true, vec2d(0.0, 0.0),
                    vec2d(50.0, 50.0), vec2d(0.0, 0.0)
                );
                NewContext.Cameras.push_back(&Cameras.back());
                camerasOrder.emplace_back(camerasOrder.size());
            }
            break;
        case layer:
            reserveMemoryForNewLayers(ObjectMemory, OwnerLayer, Owner, TriggeredObjects, 
                startingEventIt, eventIt, MemoryStack, newVectorSize
            );
            for(unsigned i = 0; i < newVectorSize; i++){
                if(i < newIDs.size()){
                    ID = newIDs[i];
                }
                Layers.emplace_back(LayerClass(ID, layersIDs, topLayerUniqueIndex));
                layersOrder.emplace_back(Layers.size() - 1);
                NewContext.Layers.push_back(&Layers.back());
            }
            break;
        case object:
            reserveMemoryForNewObjects(ObjectMemory, OwnerLayer, CurrentLayer, Owner,
                TriggeredObjects, startingEventIt, eventIt, MemoryStack, newVectorSize
            );
            for(unsigned i = 0; i < newVectorSize; i++){
                if(i < newIDs.size()){
                    initData.newID = newIDs[i];
                }
                CurrentLayer->Objects.emplace_back(AncestorObject());
                CurrentLayer->objectsOrder.emplace_back(CurrentLayer->Objects.size() - 1);

                initData.topIndex = &topObjectUniqueIndex;
                initData.listOfIDs = &CurrentLayer->objectsIDs;
                
                CurrentLayer->Objects.back().primaryConstructor(initData);
                NewContext.Objects.push_back(&CurrentLayer->Objects.back());
            }
            break;
        case super_text:
            initData.listOfIDs = &CurrentObject->superTextContainerIDs;
            createNewModule(CurrentObject->SuperTextContainer, NewContext.Modules.SuperTexts,
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->SuperTextContainer.size() - 1;
                i >= long(CurrentObject->SuperTextContainer.size() - newVectorSize); i--
            ){
                CurrentObject->SuperTextContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case super_editable_text:
            initData.listOfIDs = &CurrentObject->superEditableTextContainerIDs;
            createNewModule(CurrentObject->SuperEditableTextContainer,
                NewContext.Modules.SuperEditableTexts,
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->SuperEditableTextContainer.size() - 1;
                i >= long(CurrentObject->SuperEditableTextContainer.size() - newVectorSize); i--
            ){
                CurrentObject->SuperEditableTextContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case image:
            initData.listOfIDs = &CurrentObject->imageContainerIDs;
            createNewModule(CurrentObject->ImageContainer, NewContext.Modules.Images, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->ImageContainer.size() - 1;
                i >= long(CurrentObject->ImageContainer.size() - newVectorSize); i--
            ){
                CurrentObject->ImageContainer[i].setIsScrollable(CurrentObject->getIsScrollable());
            }
            break;
        case movement:
            initData.listOfIDs = &CurrentObject->movementContainerIDs;
            createNewModule(CurrentObject->MovementContainer, NewContext.Modules.Movements,
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->MovementContainer.size() - 1;
                i >= long(CurrentObject->MovementContainer.size() - newVectorSize); i--
            ){
                CurrentObject->MovementContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case collision:
            initData.listOfIDs = &CurrentObject->collisionContainerIDs;
            createNewModule(CurrentObject->CollisionContainer, NewContext.Modules.Collisions,
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->CollisionContainer.size() - 1;
                i >= long(CurrentObject->CollisionContainer.size() - newVectorSize); i--
            ){
                CurrentObject->CollisionContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case particles:
            initData.listOfIDs = &CurrentObject->particlesContainerIDs;
            createNewModule(CurrentObject->ParticlesContainer, NewContext.Modules.Particles, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->ParticlesContainer.size() - 1;
                i >= long(CurrentObject->ParticlesContainer.size() - newVectorSize); i--
            ){
                CurrentObject->ParticlesContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case event:
            initData.listOfIDs = &CurrentObject->eventContainerIDs;
            createNewModule(CurrentObject->EventContainer, NewContext.Modules.Events, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            break;
        case variable:
            initData.listOfIDs = &CurrentObject->variablesContainerIDs;
            createNewModule(CurrentObject->VariablesContainer, NewContext.Modules.Variables, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            break;
        case scrollbar:
            initData.listOfIDs = &CurrentObject->scrollbarContainerIDs;
            createNewModule(CurrentObject->ScrollbarContainer, NewContext.Modules.Scrollbars, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->ScrollbarContainer.size() - 1;
                i >= long(CurrentObject->ScrollbarContainer.size() - newVectorSize); i--
            ){
                CurrentObject->ScrollbarContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case primitives:
            initData.listOfIDs = &CurrentObject->primitivesContainerIDs;
            createNewModule(CurrentObject->PrimitivesContainer, NewContext.Modules.Primitives, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            for(long i = CurrentObject->PrimitivesContainer.size() - 1;
                i >= long(CurrentObject->PrimitivesContainer.size() - newVectorSize); i--
            ){
                CurrentObject->PrimitivesContainer[i].setIsScrollable(
                    CurrentObject->getIsScrollable()
                );
            }
            break;
        case vector_s:
            initData.listOfIDs = &CurrentObject->vectorContainerIDs;
            createNewModule(CurrentObject->VectorContainer, NewContext.Modules.Vectors, 
                initData, newVectorSize, newIDs, Layers, ObjectMemory, startingEventIt, eventIt,
                MemoryStack, reservationMultiplier, ActiveEditableText, CurrentInstr
            );
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << sourceToStr(Operation.Location.source) << "\' does not exist.\n";
            break;
    }

    if(NewContext.type != null_dt){
        assignEntities(ObjectMemory, NewContext, Operation.Output);
        wasNewExecuted = true;
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction failed.\n";
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
void ProcessClass::markEntitiesForDeletion(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, LayerClass *& OwnerLayer,
    AncestorObject *& Owner, vector<AncestorObject*> & TriggeredObjects, string & focusedProcessID
){
    ContextClass * DeletedContext = nullptr;
    
    if(getContextPointerFromTheParameter(DeletedContext, ObjectMemory, LocalToGlobalTranslation, CurrentInstr, Operation.Parameters, 0, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to get context.\n";
        return;
    }

    if(DeletedContext->size() == 0 || DeletedContext->type == null_dt){
        cout << instructionWarning(CurrentInstr, __FUNCTION__) << "Nothing to delete from selected context.\n";
        return;
    }

    if(printOutInstructions){
        cout << "delete " << DeletedContext->getValue(CurrentInstr, maxLengthOfValuesPrinting) << "\n";
    }

    wasDeleteExecuted = true;

    switch(DeletedContext->type){
        case camera_inst:
        case camera_vec:
            for(Camera2D * Camera : DeletedContext->Cameras){
                if(Camera != nullptr){
                    Camera->deleteLater();
                }
            }
            if(SelectedCamera != nullptr && SelectedCamera->getIsDeleted()){
                unfocusCameras(Cameras, SelectedCamera, getID(), focusedProcessID);
            }
            for(ContextClass & Context : ObjectMemory.MemoryMap){
                if(Context.type == camera_inst || Context.type == camera_vec){
                    clearDeletedPointersFromVector(Context.Cameras);
                }
            }
            break;
        case layer_inst:
        case layer_vec:
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
            for(ContextClass & Context : ObjectMemory.MemoryMap){
                if(Context.type == layer_inst || Context.type == layer_vec){
                    clearDeletedPointersFromVector(Context.Layers);
                }
            }
            break;
        case object_inst:
            DeletedContext->Objects[0]->deleteLater();
            break;
        case object_vec:
            for(AncestorObject * Object : DeletedContext->Objects){
                if(Object != nullptr){
                    Object->deleteLater();
                }
            }
            break;
        case super_text_mod:
            DeletedContext->Modules.SuperTexts[0]->deleteLater();
            break;
        case super_text_mod_vec:
            for(SuperTextModule * SuperText : DeletedContext->Modules.SuperTexts){
                if(SuperText != nullptr){
                    SuperText->deleteLater();
                }
            }
            break;
        case super_editable_text_mod:
            DeletedContext->Modules.SuperEditableTexts[0]->deleteLater();
            break;
        case super_editable_text_mod_vec:
            for(SuperEditableTextModule * SuperEditableText : DeletedContext->Modules.SuperEditableTexts){
                if(SuperEditableText != nullptr){
                    SuperEditableText->deleteLater();
                }
            }
            break;
        case image_mod:
            DeletedContext->Modules.Images[0]->deleteLater();
            break;
        case image_mod_vec:
            for(ImageModule * Image : DeletedContext->Modules.Images){
                if(Image != nullptr){
                    Image->deleteLater();
                }
            }
            break;
        case movement_mod:
            DeletedContext->Modules.Movements[0]->deleteLater();
            break;
        case movement_mod_vec:
            for(MovementModule * Movement : DeletedContext->Modules.Movements){
                if(Movement != nullptr){
                    Movement->deleteLater();
                }
            }
            break;
        case collision_mod:
            DeletedContext->Modules.Collisions[0]->deleteLater();
            break;
        case collision_mod_vec:
            for(CollisionModule * Collision : DeletedContext->Modules.Collisions){
                if(Collision != nullptr){
                    Collision->deleteLater();
                }
            }
            break;
        case particles_mod:
            DeletedContext->Modules.Particles[0]->deleteLater();
            break;
        case particles_mod_vec:
            for(ParticleEffectModule * Particle : DeletedContext->Modules.Particles){
                if(Particle != nullptr){
                    Particle->deleteLater();
                }
            }
            break;
        case event_mod:
            DeletedContext->Modules.Events[0]->deleteLater();
            break;
        case event_mod_vec:
            for(EventModule * Event : DeletedContext->Modules.Events){
                if(Event != nullptr){
                    Event->deleteLater();
                }
            }
            break;
        case variable_mod:
            DeletedContext->Modules.Variables[0]->deleteLater();
            break;
        case variable_mod_vec:
            for(VariableModule * Variable : DeletedContext->Modules.Variables){
                if(Variable != nullptr){
                    Variable->deleteLater();
                }
            }
            break;
        case scrollbar_mod:
            DeletedContext->Modules.Scrollbars[0]->deleteLater();
            break;
        case scrollbar_mod_vec:
            for(ScrollbarModule * Scrollbar : DeletedContext->Modules.Scrollbars){
                if(Scrollbar != nullptr){
                    Scrollbar->deleteLater();
                }
            }
            break;
        case primitives_mod:
            DeletedContext->Modules.Primitives[0]->deleteLater();
            break;
        case primitives_mod_vec:
            for(PrimitivesModule * Primitives : DeletedContext->Modules.Primitives){
                if(Primitives != nullptr){
                    Primitives->deleteLater();
                }
            }
            break;
        case vector_mod:
            DeletedContext->Modules.Vectors[0]->deleteLater();
            break;
        case vector_mod_vec:
            for(VectorModule * Vector : DeletedContext->Modules.Vectors){
                if(Vector != nullptr){
                    Vector->deleteLater();
                }
            }
            break;
        default:
            break;
    }

    if(DeletedContext->type == layer_inst || DeletedContext->type == layer_vec
        || DeletedContext->type == object_inst || DeletedContext->type == object_vec
    ){
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
        for(ContextClass & Context : ObjectMemory.MemoryMap){
            if(Context.type == object_inst || Context.type == object_vec){
                clearDeletedPointersFromVector(Context.Objects);
            }
        }
    }

    switch(DeletedContext->type){
        case layer_inst:
        case layer_vec:
        case object_inst:
        case object_vec:
        case super_text_mod:
        case super_text_mod_vec:
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
        case image_mod:
        case image_mod_vec:
        case movement_mod:
        case movement_mod_vec:
        case collision_mod:
        case collision_mod_vec:
        case particles_mod:
        case particles_mod_vec:
        case event_mod:
        case event_mod_vec:
        case variable_mod:
        case variable_mod_vec:
        case scrollbar_mod:
        case scrollbar_mod_vec:
        case primitives_mod:
        case primitives_mod_vec:
        case vector_mod:
        case vector_mod_vec:
            for(ContextClass & Context : ObjectMemory.MemoryMap){
                switch(Context.type){
                    case super_text_mod:
                    case super_text_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.SuperTexts);
                        break;
                    case super_editable_text_mod:
                    case super_editable_text_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.SuperEditableTexts);
                        break;
                    case image_mod:
                    case image_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Images);
                        break;
                    case movement_mod:
                    case movement_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Movements);
                        break;
                    case collision_mod:
                    case collision_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Collisions);
                        break;
                    case particles_mod:
                    case particles_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Particles);
                        break;
                    case event_mod:
                    case event_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Events);
                        break;
                    case variable_mod:
                    case variable_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Variables);
                        break;
                    case scrollbar_mod:
                    case scrollbar_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Scrollbars);
                        break;
                    case primitives_mod:
                    case primitives_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Primitives);
                        break;
                    case vector_mod:
                    case vector_mod_vec:
                        clearDeletedPointersFromVector(Context.Modules.Vectors);
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }

    DeletedContext->type = DataType::null_dt;
}
template <class Entity>
void findInstanceInVectorByIndex(vector<unsigned> indexes, vector<Entity> & Aggregated, DataType type,
    vector<Entity*> & NewVector, DataType & newType, const InstrDescription & CurrentInstr
){
    unsigned realIndex = 0;
    unsigned realSize = Aggregated.size();
    for(const Entity & Instance : Aggregated){
        if(Instance.getIsDeleted()){
            --realSize;
        }
    }
    for(unsigned index : indexes){
        for(realIndex = 0; realIndex < Aggregated.size(); realIndex++){
            if(Aggregated[realIndex].getIsDeleted()){
                index++;
            }
            else if(index == realIndex){
                NewVector.push_back(&Aggregated[index]);
                newType = type;
                if(indexes.size() > 1){
                    newType = vectorizeEntityDataType(CurrentInstr, newType);
                }
                break;
            }
        }
        if(realIndex == Aggregated.size()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index " << index << " is out of vector's scope<" << realSize << ">.\n";
        }
    }
}
template <class Entity>
void findInstanceInVectorByIndex(const EngineInstr & instruction, vector<unsigned> indexes, vector<Entity> & Aggregated, DataType type,
    vector<Entity> & NewVector, DataType & newType, const InstrDescription & CurrentInstr
){
    if(type != pointer_inst && type != pointer_vec && type != value_inst && type != value_vec){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "This function allows only entities of \'pointer\' and \'value\' types.\n";
        return;
    }
    for(unsigned index : indexes){
        if(index < Aggregated.size()){
            NewVector.push_back(Aggregated[index]);
            newType = type;
            if(indexes.size() > 1){
                newType = vectorizeEntityDataType(CurrentInstr, newType);
            }
        }
        else{
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index " << index << " is out of vector's scope<" << Aggregated.size() << ">.\n";
        }
    }
}
template <class Entity>
void findInstanceInVectorByIndex(vector<unsigned> indexes, vector<Entity*> & Aggregated, DataType type,
    vector<Entity*> & NewVector, DataType & newType, const InstrDescription & CurrentInstr, bool skipTheFirstIndex = false
){
    unsigned realIndex = 0;
    for(unsigned i = 0; i < indexes.size(); i++){
        if(indexes[i] > Aggregated.size()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index " << skipTheFirstIndex + i << " is equal to " << indexes[i]
                << ", while the size of the vector is " << Aggregated.size() << ".\n";
            return;
        }
        for(realIndex = 0; realIndex < Aggregated.size(); realIndex++){
            if(Aggregated[realIndex]->getIsDeleted()){
                indexes[i]++;
            }
            else if(indexes[i] == realIndex){
                NewVector.push_back(Aggregated[indexes[i]]);
                newType = type;
                if(indexes.size() > 1){
                    newType = vectorizeEntityDataType(CurrentInstr, newType);
                }
                break;
            }
        }
    }
}
template <class Entity>
inline void findInstanceInVectorByIndexNoReference(const vector<unsigned> & indexes, const vector<Entity> & Aggregated, DataType type,
    vector<Entity> & NewVector, const InstrDescription & CurrentInstr, bool skipTheFirstIndex = false
){
    for(unsigned i = 0; i < indexes.size(); i++){
        if(indexes[i] >= Aggregated.size()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index " << skipTheFirstIndex + i << " is equal to " << indexes[i]
                << ", while the size of the vector is " << Aggregated.size() << ".\n";
            return;
        }
        NewVector.push_back(Aggregated[i]);
    }
}
void ProcessClass::getIndexes(ObjectMemoryStruct & ObjectMemory, const vector<ParameterStruct> & Parameters, vector<unsigned> & indexes, bool skipContext){
    ContextClass IndexContext;

    if(getUnsignedVectorFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Parameters, skipContext, indexes)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get value from the parameter " << skipContext + 1 << ".\n";
        return;
    }

    if(indexes.empty()){
        indexes.emplace_back(0);
    }
}
ValueSource attributeToSource(const AttributeType & attribute){
    switch(attribute){
        case super_text_a:
            return super_text;
        case super_editable_text_a:
            return super_editable_text;
        case image_a:
            return image;
        case movement_a:
            return movement;
        case collision_a:
            return collision;
        case particles_a:
            return particles;
        case event_a:
            return event;
        case variable_a:
            return variable;
        case scrollbar_a:
            return scrollbar;
        case primitives_a:
            return primitives;
        case vector_a:
            return vector_s;
        default:
            //cerr << instructionError(CurrentInstr, __FUNCTION__)
            //  << "Module type \'" << transAttributeToString(Operation.Location.attribute) << "\' does not exist.\n";
            break;
    }
    return null_s;
}
inline DataType attributeToVecDataType(const InstrDescription & CurrentInstr, const AttributeType &attribute){
    switch(attribute){
        case camera_a:
            return camera_vec;
        case layer_a:
            return layer_vec;
        case object_a:
            return object_vec;
        case super_text_a:
            return super_text_mod_vec;
        case super_editable_text_a:
            return super_editable_text_mod_vec;
        case image_a:
            return image_mod_vec;
        case movement_a:
            return movement_mod_vec;
        case collision_a:
            return collision_mod_vec;
        case particles_a:
            return particles_mod_vec;
        case event_a:
            return event_mod_vec;
        case variable_a:
            return variable_mod_vec;
        case scrollbar_a:
            return scrollbar_mod_vec;
        case primitives_a:
            return primitives_mod_vec;
        case vector_a:
            return vector_mod_vec;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << attributeToStr(attribute) << "\' is not valid for this operation.\n";
            return null_dt;
    }
}
void findInstanceInModuleVectors(const OperationClass & Operation, AncestorObject * Object,
    const vector<unsigned> & indexes, ContextClass & NewContext, const InstrDescription & CurrentInstr
){
    DataType vectorDataType = attributeToVecDataType(CurrentInstr, Operation.Location.attribute);
    DataType instType = instantiateEntityDataType(CurrentInstr, vectorDataType);

    switch(vectorDataType){
        case super_text_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->SuperTextContainer, instType, NewContext.Modules.SuperTexts, NewContext.type, CurrentInstr);
            break;
        case super_editable_text_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->SuperEditableTextContainer, instType, NewContext.Modules.SuperEditableTexts, NewContext.type, CurrentInstr);
            break;
        case image_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->ImageContainer, instType, NewContext.Modules.Images, NewContext.type, CurrentInstr);
            break;
        case movement_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->MovementContainer, instType, NewContext.Modules.Movements, NewContext.type, CurrentInstr);
            break;
        case collision_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->CollisionContainer, instType, NewContext.Modules.Collisions, NewContext.type, CurrentInstr);
            break;
        case particles_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->ParticlesContainer, instType, NewContext.Modules.Particles, NewContext.type, CurrentInstr);
            break;
        case event_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->EventContainer, instType, NewContext.Modules.Events, NewContext.type, CurrentInstr);
            break;
        case variable_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->VariablesContainer, instType, NewContext.Modules.Variables, NewContext.type, CurrentInstr);
            break;
        case scrollbar_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->ScrollbarContainer, instType, NewContext.Modules.Scrollbars, NewContext.type, CurrentInstr);
            break;
        case primitives_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->PrimitivesContainer, instType, NewContext.Modules.Primitives, NewContext.type, CurrentInstr);
            break;
        case vector_mod_vec:
            findInstanceInVectorByIndex(indexes, Object->VectorContainer, instType, NewContext.Modules.Vectors, NewContext.type, CurrentInstr);
            break;
        default:
            //cerr << instructionError(CurrentInstr, __FUNCTION__)
            //  << "Module type \'" << transAttributeToString(Operation.Location.attribute) << "\' does not exist.\n";
            break;
    }
}
void ProcessClass::getReferenceFromLayer(const OperationClass & Operation, ContextClass & SourceContext, vector<unsigned> & indexes, ContextClass & NewContext){
    if(Operation.Location.attribute == layer_a){
        findInstanceInVectorByIndex(indexes, SourceContext.Layers, layer_inst, NewContext.Layers, NewContext.type, CurrentInstr, false);
        return;
    }
    

    if(indexes.size() < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "In order to find an object by its index, you must provide at least 2 index.\n";
        return;
    }
    LayerClass * Layer = nullptr;
    unsigned entityIndex = indexes[0];
    for(unsigned realIndex = 0; realIndex < SourceContext.Layers.size(); realIndex++){
        if(SourceContext.Layers[realIndex]->getIsDeleted()){
            entityIndex++;
        }
        else if(entityIndex == realIndex){
            Layer = SourceContext.Layers[entityIndex];
            break;
        }
    }
    
    if(Layer == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Index is out of layers vector's scope.\n";
        return;
    }

    if(Operation.Location.attribute == object_a){
        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Layer->Objects, object_inst, NewContext.Objects, NewContext.type, CurrentInstr);
    }
    else{
        if(indexes.size() < 3){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "In order to find a module instance by its index, you must provide at least 3 indexes.\n";
            return;
        }
        entityIndex = indexes[1];
        AncestorObject * Object = nullptr;
        for(unsigned realIndex = 0; realIndex < Layer->Objects.size(); realIndex++){
            if(Layer->Objects[realIndex].getIsDeleted()){
                entityIndex++;
            }
            else if(entityIndex == realIndex){
                Object = &Layer->Objects[entityIndex];
                break;
            }
        }
        if(Object == nullptr){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index is out of objects vector's scope.\n";
            return;
        }

        findInstanceInModuleVectors(Operation, Object, vector<unsigned>(indexes.begin()+2, indexes.end()), NewContext, CurrentInstr);
    }
}
void ProcessClass::getReferenceFromObject(const OperationClass & Operation, ContextClass & SourceContext, vector<unsigned> & indexes, ContextClass & NewContext){
    if(Operation.Location.attribute == object_a){
        findInstanceInVectorByIndex(indexes, SourceContext.Objects, object_inst, NewContext.Objects, NewContext.type, CurrentInstr);
        return;
    }

    if(indexes.size() < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "In order to find a module instance by its index, you must provide at least 2 indexes.\n";
        return;
    }
    unsigned entityIndex = indexes[0];
    AncestorObject * Object = nullptr;
    for(unsigned realIndex = 0; realIndex < SourceContext.Objects.size(); realIndex++){
        if(SourceContext.Objects[realIndex]->getIsDeleted()){
            entityIndex++;
        }
        else if(entityIndex == realIndex){
            Object = SourceContext.Objects[entityIndex];
            break;
        }
    }
    if(Object == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Index is out of objects vector's scope.\n";
        return;
    }

    findInstanceInModuleVectors(Operation, Object, vector<unsigned>(indexes.begin()+1, indexes.end()), NewContext, CurrentInstr);
}
void ProcessClass::getReferenceFromContextByIndex(OperationClass & Operation, ContextClass & SourceContext, vector<unsigned> & indexes, ContextClass & NewContext){
    DataType instType = instantiateEntityDataType(CurrentInstr, SourceContext.type);
    switch(SourceContext.type){
        case camera_inst:
        case camera_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Cameras, camera_inst, NewContext.Cameras, NewContext.type, CurrentInstr);
            break;
        case layer_inst:
        case layer_vec:
            getReferenceFromLayer(Operation, SourceContext, indexes, NewContext);
            break;
        case object_inst:
        case object_vec:
            getReferenceFromObject(Operation, SourceContext, indexes, NewContext);
            break;
        case super_text_mod:
        case super_text_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.SuperTexts, instType, NewContext.Modules.SuperTexts, NewContext.type, CurrentInstr);
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.SuperEditableTexts, instType, NewContext.Modules.SuperEditableTexts, NewContext.type, CurrentInstr);
            break;
        case image_mod:
        case image_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Images, instType, NewContext.Modules.Images, NewContext.type, CurrentInstr);
            break;
        case movement_mod:
        case movement_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Movements, instType, NewContext.Modules.Movements, NewContext.type, CurrentInstr);
            break;
        case collision_mod:
        case collision_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Collisions, instType, NewContext.Modules.Collisions, NewContext.type, CurrentInstr);
            break;
        case particles_mod:
        case particles_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Particles, instType, NewContext.Modules.Particles, NewContext.type, CurrentInstr);
            break;
        case event_mod:
        case event_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Events, instType, NewContext.Modules.Events, NewContext.type, CurrentInstr);
            break;
        case variable_mod:
        case variable_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Variables, instType, NewContext.Modules.Variables, NewContext.type, CurrentInstr);
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Scrollbars, instType, NewContext.Modules.Scrollbars, NewContext.type, CurrentInstr);
            break;
        case primitives_mod:
        case primitives_mod_vec:
            findInstanceInVectorByIndex(indexes, SourceContext.Modules.Primitives, instType, NewContext.Modules.Primitives, NewContext.type, CurrentInstr);
            break;
        case vector_mod:
        case vector_mod_vec: //Scary hacks for allowing the access to vector indexes
            if(indexes.size() == 1){
                findInstanceInVectorByIndex(indexes, SourceContext.Modules.Vectors, instType, NewContext.Modules.Vectors, NewContext.type, CurrentInstr);
            }
            else{
                std::vector<VectorModule*> SelectedVector;
                findInstanceInVectorByIndex(vector<unsigned>{indexes[0]}, SourceContext.Modules.Vectors, instType, SelectedVector, NewContext.type, CurrentInstr);
                indexes.erase(indexes.begin());
                NewContext.type = value_inst;
                if(indexes.size() > 1){
                    NewContext.type = vectorizeEntityDataType(CurrentInstr, NewContext.type);
                }
                if(SelectedVector.back()->getType() == 'b'){
                    findInstanceInVectorByIndexNoReference(indexes, SelectedVector.back()->getAllBoolsAsVariables(),
                        instType, NewContext.Values, CurrentInstr, true);
                }
                else if(SelectedVector.back()->getType() == 'i'){
                    findInstanceInVectorByIndexNoReference(indexes, SelectedVector.back()->getAllIntsAsVariables(),
                        instType, NewContext.Values, CurrentInstr, true);
                }
                else if(SelectedVector.back()->getType() == 'd'){
                    findInstanceInVectorByIndexNoReference(indexes, SelectedVector.back()->getAllDoublesAsVariables(),
                        instType, NewContext.Values, CurrentInstr, true);
                }
                else if(SelectedVector.back()->getType() == 's'){
                    findInstanceInVectorByIndexNoReference(indexes, SelectedVector.back()->getAllStringsAsVariables(),
                        instType, NewContext.Values, CurrentInstr, true);
                }
            }
            break;
        case pointer_inst:
        case pointer_vec:
            findInstanceInVectorByIndex(Operation.instruction, indexes, SourceContext.BasePointers, instType, NewContext.BasePointers, NewContext.type, CurrentInstr);
            break;
        case value_inst:
        case value_vec:
            findInstanceInVectorByIndex(Operation.instruction, indexes, SourceContext.Values, instType, NewContext.Values, NewContext.type, CurrentInstr);
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context of \'" << dataTypeToStr(SourceContext.type) << "\' type is invalid for this operation.\n";
            return;
    }
}
void ProcessClass::getReferenceByIndex(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    NewContext.clear();
    vector<unsigned> indexes;
    unsigned realIndex = 0, entityIndex = 0;

    if(Operation.Location.source != ValueSource::null_s && Operation.Location.source != ValueSource::context){
        getIndexes(ObjectMemory, Operation.Parameters, indexes, false);

        if(indexes.empty()){
            indexes.emplace_back(0);
        }

        if(printOutInstructions){
            if(Operation.Location.attribute != null_a){
                cout << "index " << sourceToStr(Operation.Location.source)
                    << " " << attributeToStr(Operation.Location.attribute) << " [";
            }
            else{
                cout << "index " << sourceToStr(Operation.Location.source) << " [";
            }
            
            for(const unsigned & i : indexes){
                cout << i << ", ";
            }
            cout << "] " << Operation.Output.variableID << "\n";
        }

        if(Operation.Location.source == ValueSource::camera){
            findInstanceInVectorByIndex(indexes, Cameras, camera_inst, NewContext.Cameras, NewContext.type, CurrentInstr);
        }
        else if(Operation.Location.source == ValueSource::layer){
            switch(Operation.Location.attribute){
                case null_a:
                case layer_a:
                    findInstanceInVectorByIndex(indexes, Layers, layer_inst, NewContext.Layers, NewContext.type, CurrentInstr);
                    break;
                case object_a:
                case super_text_a:
                case super_editable_text_a:
                case image_a:
                case movement_a:
                case collision_a:
                case particles_a:
                case event_a:
                case variable_a:
                case scrollbar_a:
                case primitives_a:
                case vector_a:{
                    if(indexes.size() < 2){
                        cerr << instructionError(CurrentInstr, __FUNCTION__)
                            << "In order to find an object by its index, you must provide at least 2 indexes.\n";
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
                        cerr << instructionError(CurrentInstr, __FUNCTION__)
                            << "Index is out of layers vector's scope.\n";
                        return;
                    }
                    if(Operation.Location.attribute == object_a){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()),
                            Layer->Objects, object_inst, NewContext.Objects, NewContext.type, CurrentInstr
                        );
                    }
                    else{
                        if(indexes.size() < 3){
                            cerr << instructionError(CurrentInstr, __FUNCTION__)
                                << "In order to find a module instance by its index, you must provide at least 3 indexes.\n";
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
                            cerr << instructionError(CurrentInstr, __FUNCTION__)
                                << "Index is out of objects vector's scope.\n";
                            return;
                        }

                        findInstanceInModuleVectors(Operation, Object, vector<unsigned>(indexes.begin()+2, indexes.end()), NewContext, CurrentInstr);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else{
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Source cannot have \'" << sourceToStr(Operation.Location.source) << "\' value.\n";
            return;
        }

        if(NewContext.type == null_dt){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index is out of vector's scope.\n";
            return;
        }
    }
    else{
        ContextClass SourceContext;
        bool negateAfterCopy = false; //ignore
        if(SourceContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
            CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
        )){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "No context found.\n";
            return;
        }

        getIndexes(ObjectMemory, Operation.Parameters, indexes, true);
        
        if(indexes.empty()){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "No indexes provided.\n";
            return;
        }

        if(printOutInstructions){
            if(Operation.Location.attribute != null_a){
                cout << "index " << dataTypeToStr(SourceContext.type) << " "
                    << attributeToStr(Operation.Location.attribute) << " [";
            }
            else{
                cout << "index " << dataTypeToStr(SourceContext.type) << " [";
            }
            
            for(const unsigned & i : indexes){
                cout << i << ", ";
            }
            cout << "] " << Operation.Output.variableID << "\n";
        }
        
        getReferenceFromContextByIndex(Operation, SourceContext, indexes, NewContext);
    }

    switch(NewContext.type){
        case null_dt:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed.\n";
            return;
        case value_inst:
        case value_vec:
        case pointer_inst:
        case pointer_vec:
        case variable_mod:
        case variable_mod_vec:
        case vector_mod:
        case vector_mod_vec:
            assignVariable(ObjectMemory, Operation.Output);
            return;
        default:
            assignEntities(ObjectMemory, NewContext, Operation.Output);
            return;
    }
}
void ProcessClass::getInstanceFromVector(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    ContextClass * SourceContext;
    if(getContextPointerFromTheParameter(SourceContext, ObjectMemory, LocalToGlobalTranslation, CurrentInstr, Operation.Parameters, 0, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 1.\n";
        return;
    }

    unsigned index = 0;
    if(getUnsignedFromTheParameterOptimized(ObjectMemory, LocalToGlobalTranslation, CurrentInstr, Operation.Parameters, 1, index, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get an unsigned value from the parameter 2.\n";
        return;
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << dataTypeToStr(SourceContext->type) << " "
            << index << " " << attributeToStr(Operation.Location.attribute)
            << " " << Operation.Output.variableID << "\n";
    }

    NewContext.clear();
    switch(SourceContext->type){
        case vector_mod:
        case vector_mod_vec:
            NewContext.type = value_inst;
            if(SourceContext->Modules.Vectors.back()->getType() == 'b'){
                if(index >= SourceContext->Modules.Vectors.back()->vBool.size()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Index is equal to " << index << ", while the size of the vector is "
                        << SourceContext->Modules.Vectors.back()->vBool.size() << ".\n";
                    return;
                }
                NewContext.Values.emplace_back(SourceContext->Modules.Vectors.back()->vBool[index].value);
            }
            else if(SourceContext->Modules.Vectors.back()->getType() == 'i'){
                if(index >= SourceContext->Modules.Vectors.back()->vInt.size()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Index is equal to " << index << ", while the size of the vector is "
                        << SourceContext->Modules.Vectors.back()->vInt.size() << ".\n";
                    return;
                }
                NewContext.Values.emplace_back(SourceContext->Modules.Vectors.back()->vInt[index]);
            }
            else if(SourceContext->Modules.Vectors.back()->getType() == 'd'){
                if(index >= SourceContext->Modules.Vectors.back()->vDouble.size()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Index is equal to " << index << ", while the size of the vector is " 
                        << SourceContext->Modules.Vectors.back()->vDouble.size() << ".\n";
                    return;
                }
                NewContext.Values.emplace_back(SourceContext->Modules.Vectors.back()->vDouble[index]);
            }
            else if(SourceContext->Modules.Vectors.back()->getType() == 's'){
                if(index >= SourceContext->Modules.Vectors.back()->vString.size()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Index is equal to " << index << ", while the size of the vector is "
                        << SourceContext->Modules.Vectors.back()->vString.size() << ".\n";
                    return;
                }
                NewContext.Values.emplace_back(SourceContext->Modules.Vectors.back()->vString[index]);
            }
            break;
        case value_vec:
            NewContext.type = value_inst;
            if(index >= SourceContext->Values.size()){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Index is equal to " << index << ", while the size of the vector is "
                    << SourceContext->Values.size() << ".\n";
                return;
            }
            NewContext.Values.push_back(SourceContext->Values[index]);
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Variable '" << SourceContext->ID << "' has invalid type: \'"
                << dataTypeToStr(SourceContext->type) << "\'.\n";
            return;
    }

    if(NewContext.type != null_dt){
        moveToVariable(ObjectMemory, Operation.Output);
        //assignVariable(ObjectMemory, Operation.Output);
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed.\n";
    }
}
void printStringVectorForInstruction(const vector<string> & values, int maxLengthOfValuesPrinting){
    if(values.size() == 1){
        cout << values.back() << " ";
    }
    else if(values.size() > 1){
        cout << " [";
        for(string text : values){
            cout << "\"" << shortenText(text, maxLengthOfValuesPrinting) << "\", ";
        }
        cout << "] ";
    }
}
void ProcessClass::bindFilesToObjects(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    ContextClass ObjectContext;

    bool negateAfterCopy = false; //ignore
    if(ObjectContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get any objects from the first parameter.\n";
        return;
    }

    if(ObjectContext.Objects.empty()){
        if(printOutInstructions){
            cout << instrToStr(Operation.instruction) << " <null>\n";
        }
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        ObjectContext.printOutObjects();
    }

    vector<string> scriptPaths;

    if(getStringVectorFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, scriptPaths, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get value from the parameter 2.\n";
        return;
    }

    if(scriptPaths.empty()){
        if(printOutInstructions){
            cout << "<null>\n";
        }
        return;
    }

    if(printOutInstructions){
        printStringVectorForInstruction(scriptPaths, maxLengthOfValuesPrinting);
        cout << "\n";
    }

    for(AncestorObject * Object : ObjectContext.Objects){
        for(string path : scriptPaths){
            Object->bindedScripts.emplace_back(EXE_PATH + workingDirectory + path);
        }
    }
}
void ProcessClass::removeBindedFilesFromObjects(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory
){
    ContextClass ObjectContext;
    bool negateAfterCopy = false; //ignore
    if(ObjectContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get any objects from the first parameter.\n";
        return;
    }

    if(ObjectContext.Objects.empty()){
        if(printOutInstructions){
            cout << instrToStr(Operation.instruction) << " <null>\n";
        }
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        ObjectContext.printOutObjects();
        cout << "\n";
    }

    for(AncestorObject * Object : ObjectContext.Objects){
        Object->bindedScripts.clear();
    }
}
inline LayerClass * findLayerWithId(vector<LayerClass> &Layers, const string &layerId){
    for(LayerClass & itLayer : Layers){
        if(itLayer.getID() == layerId){
            return &itLayer;
        }
    }
    return nullptr;
}
bool ProcessClass::buildEventsInObjects(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, AncestorObject * Owner,
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & MemoryStack, bool allowNotAscii
){
    ContextClass ObjectContext;
    bool negateAfterCopy = false; //ignore
    if(ObjectContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get any objects from the first parameter.\n";
        return false;
    }

    if(ObjectContext.Objects.empty()){
        if(printOutInstructions){
            cout << instrToStr(Operation.instruction) << " <null>\n";
        }
        return false;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        ObjectContext.printOutObjects();
    }
    
    bool canResetEvents = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 1, canResetEvents, false
    );

    bool canDeleteEventsOfItsOwner = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 2, canDeleteEventsOfItsOwner, false
    );

    if(printOutInstructions){
        if(canResetEvents){
            cout << " true\n";
        }
        else{
            cout << " false\n";
        }
    }

    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, MemoryStack,
        ActiveEditableText, CurrentInstr
    );

    bool myEventsAreDeleted = false;
    for(AncestorObject * Object : ObjectContext.Objects){
        if(canResetEvents && Object == Owner){
            if(!canDeleteEventsOfItsOwner){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Cannot delete events of the owner of the currently executed event.\n";
                continue;
            }
            myEventsAreDeleted = true;
        }
        ObjectMemoryStruct & CurrentMap = ProcessMemory[Object->getUniqueIndex()];
        if(canResetEvents){
            CurrentMap.clear();
        }
        LayerClass * ObjectsLayer = findLayerWithId(Layers, Object->getLayerID());
        if(ObjectsLayer == nullptr){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Layer '" << Object->getLayerID() << "' with an object '"
                << Object->getID() << "' does not exist.\n";
            return myEventsAreDeleted;
        }
        allocateBuiltInVariables(CurrentMap, *Object, *ObjectsLayer);
        allocatePredefinedMemberParameters(CurrentMap, *Object);
        Object->translateAllScripts(EXE_PATH, canResetEvents, allowNotAscii,
            CurrentMap.MemberVarsScope, CurrentMap.topAddress, topModuleUniqueIndex
        );
        allocateAllLocalVariables(CurrentMap, Object->EventContainer);
        Object->findIndexesOfEventChildren();
        detectRecursionInEvents(Object->EventContainer, CurrentInstr);
        wasAnyEventUpdated = true;
    }

    if(!myEventsAreDeleted){
        Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
            MemoryStack, ActiveEditableText, CurrentInstr
        );
    }

    return myEventsAreDeleted;
}
bool ProcessClass::customBuildEventsInObjects(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory,
    AncestorObject * Owner, vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & MemoryStack, const EngineInstr & mode, bool allowNotAscii
){
    ContextClass ObjectContext;
    bool negateAfterCopy = false; //object
    if(ObjectContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get any objects from the first parameter. "
            << "Instruction requires two parameters.\n";
        return false;
    }

    if(ObjectContext.Objects.empty()){
        if(printOutInstructions){
            cout << instrToStr(Operation.instruction) << " <null>\n";
        }
        return false;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        ObjectContext.printOutObjects();
    }

    vector<string> stringVector;
    if(getStringVectorFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, stringVector, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter 2. A vector of strings was expected.\n";
        return false;
    }

    if(stringVector.empty()){
        if(printOutInstructions){
            cout << "<null>\n";
        }
        return false;
    }

    bool canResetEvents = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, canResetEvents, false);

    bool canDeleteEventsOfItsOwner = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 3, canDeleteEventsOfItsOwner, false);

    if(printOutInstructions){
        printStringVectorForInstruction(stringVector, maxLengthOfValuesPrinting);
        cout << "\n";
    }

    //All indexes linked with events must be recalculated - after adding new events, some contexts of event type might be invalid.  
    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, MemoryStack, ActiveEditableText, CurrentInstr);

    bool myEventsAreDeleted = false;
    
    for(AncestorObject * Object : ObjectContext.Objects){
        if(canResetEvents && Object == Owner){
            if(!canDeleteEventsOfItsOwner){
                std::cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Cannot delete events of the owner of the currently executed event.\n";
                continue;
            }
            myEventsAreDeleted = true;
        }
        ObjectMemoryStruct & CurrentMap = ProcessMemory[Object->getUniqueIndex()];
        if(canResetEvents){
            CurrentMap.clear();
        }
        LayerClass * ObjectsLayer = findLayerWithId(Layers, Object->getLayerID());
        if(ObjectsLayer == nullptr){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Layer '" << Object->getLayerID() << "' with an object '"
                << Object->getID() << "' does not exist.\n";
            return myEventsAreDeleted;
        }
        allocateBuiltInVariables(CurrentMap, *Object, *ObjectsLayer);
        allocatePredefinedMemberParameters(CurrentMap, *Object);
        
        switch(mode){
            case load_build:
                for(string & path : stringVector){
                    path = EXE_PATH + workingDirectory + path;
                }
                Object->translateScriptsFromPaths(EXE_PATH, canResetEvents, stringVector,
                    allowNotAscii, CurrentMap.MemberVarsScope, CurrentMap.topAddress,
                    topModuleUniqueIndex
                );
                break;
            case build_subset:
                for(string & path : stringVector){
                    path = EXE_PATH + workingDirectory + path;
                }
                Object->translateSubsetBindedScripts(EXE_PATH, canResetEvents, stringVector,
                    allowNotAscii, CurrentMap.MemberVarsScope, CurrentMap.topAddress,
                    topModuleUniqueIndex
                );
                break;
            case inject_code:
                Object->injectCode(canResetEvents, stringVector, CurrentMap.MemberVarsScope,
                    CurrentMap.topAddress, topModuleUniqueIndex
                );
                break;
            case inject_instr:{
                bool isInsideStringSector = false;
                vector<string> preprocessed = {""};
                for(size_t i = 0; i < stringVector.size(); i++){
                    for(size_t j = 0; j < stringVector[i].size(); j++){
                        if(stringVector[i][j] == '\"' && (j == 0
                            || stringVector[i][j - 1] != '\\'
                        )){
                            isInsideStringSector = !isInsideStringSector;
                        }
                        if(!isInsideStringSector && stringVector[i][j] == '\n'){
                            preprocessed.emplace_back("");
                        }
                        else{
                            preprocessed.back() += stringVector[i][j];
                        }
                    }
                }
                if(isInsideStringSector){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "String section was not closed in lines:\n";
                    for(const string & line : stringVector){
                        cout << "\t" << line << "\n";
                    }
                }
                Object->injectInstructions(canResetEvents, preprocessed, CurrentMap.MemberVarsScope,
                    CurrentMap.topAddress, topModuleUniqueIndex
                );
                } break;
            default:
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "\'" << mode
                    << "\' mode does not exist."
                    << "Allowed modes: p (uses translateScriptsFromPaths), "
                    << "s (uses translateSubsetBindedScripts),"
                    << "c (uses injectCode), i (uses injectInstructions)\n";
                return false;
        }

        allocateAllLocalVariables(CurrentMap, Object->EventContainer);
        Object->findIndexesOfEventChildren();
        detectRecursionInEvents(Object->EventContainer, CurrentInstr);

        wasAnyEventUpdated = true;
    }
    if(!myEventsAreDeleted || wasAnyEventUpdated){
        Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
            MemoryStack, ActiveEditableText, CurrentInstr
        );
    }

    return myEventsAreDeleted;
}
void ProcessClass::clearEventsInObjects(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, AncestorObject * Owner
){
    ContextClass ObjectContext;
    bool negateAfterCopy = false; //ignore
    if(ObjectContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get any objects from the first parameter. "
            << "Instruction requires one parameter.\n";
        return;
    }

    if(ObjectContext.Objects.empty()){
        if(printOutInstructions){
            cout << instrToStr(Operation.instruction) << " <null>\n";
        }
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
        ObjectContext.printOutObjects();
    }

    for(AncestorObject * Object : ObjectContext.Objects){
        if(Object == Owner){
            cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                << "Cannot delete events from the owner of the currently executed event.\n";
            continue;
        }
        ProcessMemory[Object->getUniqueIndex()].clear();
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
void ProcessClass::executeFunctionForCameras(OperationClass & Operation, const vector<VariableModule> & Variables,
    vector<Camera2D*> CamerasFromContext, Camera2D *& SelectedCamera, string & focusedProcessID
){
    for(Camera2D * Camera : CamerasFromContext){
        if(Camera->getIsDeleted()){
            continue;
        }
        switch(Operation.Location.attribute){
            case set_id:
                if(Variables.empty()){
                    break;
                }
                Camera->setID(Variables[0].getStringUnsafe(), camerasIDs);
                break;
            case draw_one_frame:
                if(Variables.empty()){
                    break;
                }
                Camera->drawOneFrame = Variables[0].getBoolUnsafe();
                break;
            case clear_bitmap:
                if(Variables.empty()){
                    break;
                }
                Camera->clearBitmap = Variables[0].getBoolUnsafe();
                break;
            case set_active:
                if(Variables.empty()){
                    break;
                }
                Camera->setIsActive(Variables[0].getBoolUnsafe());
                break;
            case activate_a:
                Camera->activate();
                break;
            case deactivate_a:
                Camera->deactivate();
                break;
            case toggle:
                Camera->toggleIsActive();
                break;
            case set_pos:
                if(Variables.size() < 2){
                    break;
                }
                Camera->setPos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                updateTreeOfCamerasFromSelectedRoot(Camera);
                for(Camera2D & Pin : Cameras){
                    if(Pin.getID() == Camera->pinnedCameraID){
                        updateTreeOfCamerasFromSelectedRoot(&Pin);
                        break;
                    }
                }
                break;
            case set_relative_position:
                if(Variables.size() < 2){
                    break;
                }
                Camera->setRelativePos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                updateTreeOfCamerasFromSelectedRoot(Camera);
                for(Camera2D & Pin : Cameras){
                    if(Pin.getID() == Camera->pinnedCameraID){
                        updateTreeOfCamerasFromSelectedRoot(&Pin);
                        break;
                    }
                }
                break;
            case resize:
                if(Variables.size() < 2){
                    break;
                }
                Camera->setSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                break;
            case set_min_size:
                if(Variables.size() < 2){
                    break;
                }
                Camera->setMinSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                break;
            case set_zoom:
                if(Variables.empty()){
                    break;
                }
                Camera->zoom = Variables[0].getDoubleUnsafe();
                break;
            case set_zoom_increase:
                if(Variables.empty()){
                    break;
                }
                Camera->zoomIncrease = Variables[0].getDoubleUnsafe();
                break;
            case set_min_zoom:
                if(Variables.empty()){
                    break;
                }
                Camera->minZoom = Variables[0].getDoubleUnsafe();
                break;
            case set_max_zoom:
                if(Variables.empty()){
                    break;
                }
                Camera->maxZoom = Variables[0].getDoubleUnsafe();
                break;
            case set_speed:
                if(Variables.empty()){
                    break;
                }
                Camera->setSpeed(Variables[0].getDoubleUnsafe());
                break;
            case bind_keys:
                if(Variables.size() < 7){
                    break;
                }
                Camera->setKeyBinds(Variables[0].getIntUnsafe(), Variables[1].getIntUnsafe(), Variables[2].getIntUnsafe(),
                    Variables[3].getIntUnsafe(), Variables[4].getIntUnsafe(), Variables[5].getIntUnsafe(), Variables[6].getIntUnsafe());
                break;
            case set_key_zoom_in:
                if(Variables.empty()){
                    break;
                }
                Camera->zoomInKey = Variables[0].getIntUnsafe();
                break;
            case set_key_zoom_out:
                if(Variables.empty()){
                    break;
                }
                Camera->zoomOutKey = Variables[0].getIntUnsafe();
                break;
            case set_key_zoom_reset:
                if(Variables.empty()){
                    break;
                }
                Camera->zoomResetKey = Variables[0].getIntUnsafe();
                break;
            case set_key_up:
                if(Variables.empty()){
                    break;
                }
                Camera->upKey = Variables[0].getIntUnsafe();
                break;
            case set_key_right:
                if(Variables.empty()){
                    break;
                }
                Camera->rightKey = Variables[0].getIntUnsafe();
                break;
            case set_key_down:
                if(Variables.empty()){
                    break;
                }
                Camera->downKey = Variables[0].getIntUnsafe();
                break;
            case set_key_left:
                if(Variables.empty()){
                    break;
                }
                Camera->leftKey = Variables[0].getIntUnsafe();
                break;
            case pin_to_camera:
                if(Variables.empty()){
                    break;
                }
                Camera->pinnedCameraID = Variables[0].getStringUnsafe();
                break;
            case follow_layer:
                if(Variables.empty()){
                    break;
                }
                Camera->followedLayerID = Variables[0].getStringUnsafe();
                break;
            case follow_object:
                if(Variables.empty()){
                    break;
                }
                Camera->followedObjectID = Variables[0].getStringUnsafe();
                break;
            case follow_image:
                if(Variables.empty()){
                    break;
                }
                Camera->followedImageID = Variables[0].getStringUnsafe();
                break;
            case set_is_pinned_to_camera:
                if(Variables.empty()){
                    break;
                }
                Camera->isPinnedToCamera = Variables[0].getBoolUnsafe();
                break;
            case set_is_forcefully_pinned:
                if(Variables.empty()){
                    break;
                }
                Camera->setIsForcefullyPinned(Variables[0].getBoolUnsafe());
                break;
            case activate_pin:
                Camera->activatePin();
                break;
            case deactivate_pin:
                Camera->deactivatePin();
                break;
            case toggle_pin:
                Camera->togglePin();
                break;
            case set_is_following_object:
                if(Variables.empty()){
                    break;
                }
                Camera->isFollowingObject = Variables[0].getBoolUnsafe();
                break;
            case set_can_move_with_keyboard:
                if(Variables.empty()){
                    break;
                }
                Camera->isUsingKeyboardToMove = Variables[0].getBoolUnsafe();
                break;
            case set_can_zoom_with_keyboard:
                if(Variables.empty()){
                    break;
                }
                Camera->isUsingKeyboardToZoom = Variables[0].getBoolUnsafe();
                break;
            case set_is_vision_affected_by_mouse:
                if(Variables.empty()){
                    break;
                }
                Camera->isVisionAffectedByMouse = Variables[0].getBoolUnsafe();
                break;
            case set_can_zoom_with_mouse:
                if(Variables.empty()){
                    break;
                }
                Camera->canZoomWithMouse = Variables[0].getBoolUnsafe();
                break;
            case set_can_be_modified_by_mouse:
                if(Variables.empty()){
                    break;
                }
                Camera->canBeModifiedByMouse = Variables[0].getBoolUnsafe();
                break;
            case set_can_move_objects:
                if(Variables.empty()){
                    break;
                }
                Camera->setCanMoveObjects(Variables[0].getBoolUnsafe());
                break;
            case set_can_edit_text:
                if(Variables.empty()){
                    break;
                }
                Camera->setCanEditText(Variables[0].getBoolUnsafe());
                break;
            case add_visible_layer:
                if(Variables.empty()){
                    break;
                }
                Camera->addVisibleLayer(Variables[0].getStringUnsafe());
                break;
            case add_accessible_layer:
                if(Variables.empty()){
                    break;
                }
                Camera->addAccessibleLayer(Variables[0].getStringUnsafe());
                break;
            case remove_visible_layer:
                if(Variables.empty()){
                    break;
                }
                Camera->removeVisibleLayer(Variables[0].getStringUnsafe());
                break;
            case remove_accessible_layer:
                if(Variables.empty()){
                    break;
                }
                Camera->removeAccessibleLayer(Variables[0].getStringUnsafe());
                break;
            case clear_visible_layers:
                Camera->clearVisibleLayers();
                break;
            case clear_accessible_layers:
                Camera->clearAccessibleLayers();
                break;
            case set_tint:
                if(Variables.size() < 4){
                    break;
                }
                Camera->setTint(Variables[0].getIntUnsafe(), Variables[1].getIntUnsafe(), Variables[2].getIntUnsafe(), Variables[3].getIntUnsafe());
                break;
            case set_drawing_borders:
                if(Variables.empty()){
                    break;
                }
                Camera->allowsDrawingBorders = Variables[0].getBoolUnsafe();
                break;
            case allow_drawing_object_borders:
                Camera->canDrawObjectBorders = true;
                break;
            case forbid_drawing_object_borders:
                Camera->canDrawObjectBorders = false;
                break;
            case set_can_draw_object_borders:
                if(Variables.empty()){
                    break;
                }
                Camera->canDrawObjectBorders = Variables[0].getBoolUnsafe();
                break;
            case minimize:{
                Camera->minimize();
                unsigned cameraIndex = Camera - &Cameras[0];
                for(unsigned indexInOrder = 0; indexInOrder < camerasOrder.size(); indexInOrder++){
                    if(camerasOrder[indexInOrder] == cameraIndex){
                        auto it = camerasOrder.rbegin() + camerasOrder.size() - 1 - indexInOrder;
                        std::rotate(it, it + 1, camerasOrder.rend());
                        break;
                    }
                }
                break;
            }
            case bring_forward:{
                unsigned cameraIndex = Camera - &Cameras[0];
                for(unsigned indexInOrder = 0; indexInOrder < camerasOrder.size(); indexInOrder++){
                    if(camerasOrder[indexInOrder] == cameraIndex){
                        bringCameraForward(indexInOrder, Camera);
                        break;
                    }
                }
                break;
            }
            case set_can_be_grabbed:
                if(Variables.empty()){
                    break;
                }
                Camera->canBeGrabbed = Variables[0].getBoolUnsafe();
                break;
            case set_grabbing_area_position:
                if(Variables.size() < 2){
                    break;
                }
                Camera->setGrabbingAreaPos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                break;
            case set_grabbing_area_size:
                if(Variables.size() < 2){
                    break;
                }
                Camera->setGrabbingAreaSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                break;
            case set_can_draw_on_camera:
                if(Variables.empty()){
                    break;
                }
                Camera->canDrawOnCamera = Variables[0].getBoolUnsafe();
                break;
            case set_can_clear_bitmap:
                if(Variables.empty()){
                    break;
                }
                Camera->canClearBitmap = Variables[0].getBoolUnsafe();
                break;
            case set_keep_inside_screen:
                if(Variables.empty()){
                    break;
                }
                Camera->keepInsideScreen = Variables[0].getBoolUnsafe();
                break;
            case set_can_mouse_resize:
                if(Variables.empty()){
                    break;
                }
                Camera->canMouseResizeNow = Variables[0].getBoolUnsafe();
                break;
            case focus:
                focusCamera(Cameras, SelectedCamera, getID(), focusedProcessID, Camera);
                break;
            case unfocus:
                unfocusCameras(Cameras, SelectedCamera, getID(), focusedProcessID);
                break;
            default:
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Function " << attributeToStr(Operation.Location.attribute)
                    << "<" << Variables.size() << "> does not exist.\n";
                break;
        }
    }

    if(Operation.Location.attribute == pin_to_camera){
        updateWholeForestOfCameras();
    }
}
void ProcessClass::moveLayerInDrawingOrder(LayerClass * Layer, unsigned newIndex){
    if(newIndex >= layersOrder.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Index " << newIndex
            << " is out of scope of objects' drawing order of size " << layersOrder.size() << ".\n";  
        return;
    }
    unsigned indexInTheContainer = Layer - &Layers[0];
    unsigned oldIndex = 0;
    for(; oldIndex < layersOrder.size(); oldIndex++){
        if(layersOrder[oldIndex] == indexInTheContainer){
            break;
        }
    }
    if(oldIndex == layersOrder.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Layer '" << Layer->getID() << "' was not found in the drawing order.\n";   
        return;
    }
    if(newIndex == oldIndex){
        return;
    }

    //While swapping layers, skip deleted layers
    int distance = oldIndex - newIndex;
    if(distance > 0){
        for(; distance < 0; --distance){
            if(Layers[layersOrder[newIndex + distance]].getIsDeleted()){
                ++distance;
                continue;
            }
            std::swap(layersOrder[newIndex + distance], layersOrder[newIndex + distance - 1]);
        }
    }
    else if(distance < 0){
        for(; distance > 0; ++distance){
            if(Layers[layersOrder[newIndex + distance]].getIsDeleted()){
                --distance;
                continue;
            }
            std::swap(layersOrder[newIndex + distance], layersOrder[newIndex + distance + 1]);
        }
    }
}
void ProcessClass::minimizeLayerInDrawingOrder(LayerClass * Layer){
    unsigned indexInTheContainer = Layer - &Layers[0];
    unsigned oldIndex = 0;
    for(; oldIndex < layersOrder.size(); oldIndex++){
        if(layersOrder[oldIndex] == indexInTheContainer){
            break;
        }
    }
    if(oldIndex == layersOrder.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Layer '" << Layer->getID() << "' was not found in the drawing order.\n";   
        return;
    }
    for(; oldIndex > 0; --oldIndex){
        std::swap(layersOrder[oldIndex], layersOrder[oldIndex - 1]);
    }
}
void ProcessClass::bringForwardLayerInDrawingOrder(LayerClass * Layer){
    unsigned indexInTheContainer = Layer - &Layers[0];
    unsigned oldIndex = 0;
    for(; oldIndex < layersOrder.size(); oldIndex++){
        if(layersOrder[oldIndex] == indexInTheContainer){
            break;
        }
    }
    if(oldIndex == layersOrder.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Layer '" << Layer->getID() << "' was not found in the drawing order.\n";   
        return;
    }
    for(; oldIndex < layersOrder.size(); ++oldIndex){
        std::swap(layersOrder[oldIndex], layersOrder[oldIndex + 1]);
    }
}
void ProcessClass::executeFunctionForLayers(OperationClass & Operation, const vector<VariableModule> & Variables, vector<LayerClass*> & ContextLayers){
    for(LayerClass * Layer : ContextLayers){
        if(Layer->getIsDeleted()){
            continue;
        }
        switch(Operation.Location.attribute){
            case set_id:
                if(Variables.size() == 0){
                    break;
                }
                Layer->setID(Variables[0].getStringUnsafe(), layersIDs);
                break;
            case set_is_active:
                if(Variables.size() == 0){
                    break;
                }
                Layer->setIsActive(Variables[0].getBoolUnsafe());
                break;
            case activate_a:
                Layer->setIsActive(true);
                break;
            case deactivate_a:
                Layer->setIsActive(false);
                break;
            case toggle:
                Layer->setIsActive(!Layer->getIsActive());
                break;
            case add_group:
                if(Variables.size() == 0){
                    break;
                }
                Layer->addGroup(Variables[0].getStringUnsafe());
                break;
            case remove_group:
                if(Variables.size() == 0){
                    break;
                }
                Layer->removeGroup(Variables[0].getStringUnsafe());
                break;
            case set_pos:
                if(Variables.size() < 2){
                    break;
                }
                Layer->pos.x = Variables[0].getDoubleUnsafe();
                Layer->pos.y = Variables[1].getDoubleUnsafe();
                break;
            case set_size:
                if(Variables.size() < 2){
                    break;
                }
                Layer->size.x = Variables[0].getDoubleUnsafe();
                Layer->size.y = Variables[1].getDoubleUnsafe();
                break;
            case move_in_drawing_order:
                if(Variables.size() == 0){
                    break;
                }
                moveLayerInDrawingOrder(Layer, Variables[0].getIntUnsafe());
                break;
            case minimize:
                minimizeLayerInDrawingOrder(Layer);
                break;
            case bring_forward:
                minimizeLayerInDrawingOrder(Layer);
                break;
            default:
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Function "
                    << attributeToStr(Operation.Location.attribute)
                    << "<" << Variables.size() << "> does not exist.\n";
                break;
        }
    }
}
inline bool ProcessClass::getLayerOfTheObject(LayerClass *& ObjectLayer, AncestorObject * Object, string functionName){
    for(LayerClass & Layer : Layers){
        if(Layer.getID() == Object->getLayerID()){
            ObjectLayer = &Layer;
            break;
        }
    }
    if(ObjectLayer == nullptr){
        cerr << instructionError(CurrentInstr, functionName) << "Layer '" << Object->getID()
            << "' with an object '" << Object->getID() << "' was not found in the drawing order.\n";   
        return true;
    }
    if(ObjectLayer->getIsDeleted()){
        cerr << instructionError(CurrentInstr, functionName) << "Layer '" << ObjectLayer->getID()
            << "' with an object '" << Object->getID() << "' was deleted.\n";  
        return true;
    }
    return false;
}
inline bool ProcessClass::findCurrentIndexInObjectsDrawingOrder(LayerClass * ObjectLayer, AncestorObject * Object, string functionName, unsigned & currentIndex){
    unsigned indexInTheContainer = Object - &ObjectLayer->Objects[0];
    for(; currentIndex < ObjectLayer->objectsOrder.size(); currentIndex++){
        if(ObjectLayer->objectsOrder[currentIndex] == indexInTheContainer){
            break;
        }
    }
    if(currentIndex == ObjectLayer->objectsOrder.size()){
        cerr << instructionError(CurrentInstr, functionName) << "Layer '" << Object->getID()
            << "' with an object '" << Object->getID() << "' was not found in the drawing order.\n";   
        return true;
    }
    return false;
}
void ProcessClass::moveObjectInDrawingOrder(AncestorObject * Object, unsigned newIndex){    
    LayerClass * ObjectLayer = nullptr;
    if(getLayerOfTheObject(ObjectLayer, Object, __FUNCTION__)){
        return;
    }
    if(newIndex >= ObjectLayer->objectsOrder.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Index " << newIndex
            << " is out of scope of objects' drawing order of size " << ObjectLayer->objectsOrder.size() << ".\n";  
        return;
    }
    unsigned currentIndex = 0;
    if(findCurrentIndexInObjectsDrawingOrder(ObjectLayer, Object, __FUNCTION__, currentIndex)){
        return;
    }
    if(newIndex == currentIndex){
        return;
    }

    //While swapping objects, skip deleted objects
    int distance = currentIndex - newIndex;
    if(distance > 0){
        for(; distance < 0; --distance){
            if(ObjectLayer->Objects[ObjectLayer->objectsOrder[newIndex + distance]].getIsDeleted()){
                ++distance;
                continue;
            }
            std::swap(ObjectLayer->objectsOrder[newIndex + distance], ObjectLayer->objectsOrder[newIndex + distance - 1]);
        }
    }
    else if(distance < 0){
        for(; distance > 0; ++distance){
            if(ObjectLayer->Objects[ObjectLayer->objectsOrder[newIndex + distance]].getIsDeleted()){
                --distance;
                continue;
            }
            std::swap(ObjectLayer->objectsOrder[newIndex + distance], ObjectLayer->objectsOrder[newIndex + distance + 1]);
        }
    }
}
void ProcessClass::minimizeObjectInDrawingOrder(AncestorObject * Object){
    LayerClass * ObjectLayer = nullptr;
    if(getLayerOfTheObject(ObjectLayer, Object, __FUNCTION__)){
        return;
    }
    unsigned currentIndex = 0;
    if(findCurrentIndexInObjectsDrawingOrder(ObjectLayer, Object, __FUNCTION__, currentIndex)){
        return;
    }
    for(; currentIndex > 0; --currentIndex){
        std::swap(ObjectLayer->objectsOrder[currentIndex], ObjectLayer->objectsOrder[currentIndex - 1]);
    }
}
void ProcessClass::bringForwardObjectInDrawingOrder(AncestorObject * Object){
    LayerClass * ObjectLayer = nullptr;
    if(getLayerOfTheObject(ObjectLayer, Object, __FUNCTION__)){
        return;
    }
    unsigned currentIndex = 0;
    if(findCurrentIndexInObjectsDrawingOrder(ObjectLayer, Object, __FUNCTION__, currentIndex)){
        return;
    }
    for(; currentIndex < ObjectLayer->objectsOrder.size(); ++currentIndex){
        std::swap(ObjectLayer->objectsOrder[currentIndex], ObjectLayer->objectsOrder[currentIndex + 1]);
    }
}
void ProcessClass::executeFunctionForObjects(OperationClass & Operation, const vector<VariableModule> & Variables, vector<AncestorObject*> & Objects){
    if(Operation.Location.attribute == set_id && Variables.size() > 0){
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
        switch(Operation.Location.attribute){
            case set_active:
                if(Variables.size() == 0){
                    continue;
                }
                Object->setIsActive(Variables[0].getBoolUnsafe());
                continue;
            case activate_a:
                Object->activate();
                continue;
            case deactivate_a:
                Object->deactivate();
                continue;
            case toggle:
                Object->toggleIsActive();
                continue;
            case set_pos:
                if(Variables.size() < 2){
                    continue;
                }
                Object->setPos(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                continue;
            case set_size:
                if(Variables.size() < 2){
                    continue;
                }
                Object->setSize(Variables[0].getDoubleUnsafe(), Variables[1].getDoubleUnsafe());
                continue;
            case add_group:
                if(Variables.size() == 0){
                    continue;
                }
                Object->addGroup(Variables[0].getStringUnsafe());
                continue;
            case remove_group:
                if(Variables.size() == 0){
                    continue;
                }
                Object->removeGroup(Variables[0].getStringUnsafe());
                continue;
            case clear_groups:
                Object->clearGroups();
                continue;
            case disable_scrolling:
                Object->setIsScrollable(false);
                continue;
            case enable_scrolling:
                Object->setIsScrollable(true);
                continue;
            case set_is_scrollable:
                if(Variables.size() == 0){
                    continue;
                }
                Object->setIsScrollable(Variables[0].getBoolUnsafe());
                continue;
            case enable_selection_border:
                Object->canDrawSelectionBorder = true;
                continue;
            case disable_selection_border:
                Object->canDrawSelectionBorder = false;
                continue;
            case set_can_draw_selection_border:
                if(Variables.size() == 0){
                    continue;
                }
                Object->canDrawSelectionBorder = Variables[0].getBoolUnsafe();
                continue;
            case set_can_be_moved_with_mouse:
                if(Variables.size() == 0){
                    continue;
                }
                Object->canBeMovedWithMouse = Variables[0].getBoolUnsafe();
                continue;
            case update_scrollbars:
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
                continue;
            case move_in_drawing_order:
                if(Variables.size() == 0){
                    continue;
                }
                moveObjectInDrawingOrder(Object, Variables[0].getIntUnsafe());
                continue;
            case minimize:
                minimizeObjectInDrawingOrder(Object);
                continue;
            case bring_forward:
                bringForwardObjectInDrawingOrder(Object);
                continue;
            default:
                bool temp = false;
                if(Variables.size() > 0){
                    temp = Variables[0].getBoolUnsafe();
                }
                Object->control(Operation.Location.attribute, temp, Variables.size());
                continue;
        }
    }
}
void ProcessClass::executeFunction(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory,
    vector<EventModule>::iterator & Event, EngineClass & Engine
){
    ContextClass * Context = nullptr;

    if(getContextPointerFromTheParameter(Context, ObjectMemory, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Function requires at least one context.\n";
        return;
    }

    vector<VariableModule> functionArguments;

    //Get values from all parameters of this instruction
    for(unsigned index = 1; index < Operation.rootParametersSize; index++){
        if(getValuesFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext,
            CurrentInstr, Operation.Parameters, index, functionArguments, true
        )){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Failed to get value from the parameter " << index+1 << ".\n";
            return;
        }
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << Context->ID
            << "." << attributeToStr(Operation.Location.attribute) << "()";
        if(functionArguments.size() > 0){
            cout << "[";
            for(const VariableModule & Var : functionArguments){
                cout << shortenText(Var.getAnyValue(), maxLengthOfValuesPrinting) << ", ";
            }
            cout << "]<" << functionArguments.size() << ">";
        }
        cout << "\n";
    }

    AncestorObject * ModulesObject = nullptr;
    vector<string> emptyString;

    
    switch(Context->type){
        case camera_inst:
        case camera_vec:
            executeFunctionForCameras(Operation, functionArguments, Context->Cameras,
                SelectedCamera, Engine.focusedProcessID
            );
            break;
        case layer_inst:
        case layer_vec:
            executeFunctionForLayers(Operation, functionArguments, Context->Layers);
            break;
        case object_inst:
        case object_vec:
            executeFunctionForObjects(Operation, functionArguments, Context->Objects);
            break;
        case super_text_mod:
        case super_text_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(SuperTextModule * SuperText : Context->Modules.SuperTexts){
                    if(!findObjectForFunction(ModulesObject, Layers, SuperText->getObjectID(),
                        SuperText->getLayerID()
                    )){
                        continue;
                    }
                    Event->controlSuperText(SuperText, Operation.Location.attribute,
                        functionArguments, ModulesObject->superTextContainerIDs,
                        Engine.FontContainer, EXE_PATH, workingDirectory
                    );
                }
                return;
            }
            for(SuperTextModule * SuperText : Context->Modules.SuperTexts){
                Event->controlSuperText(SuperText, Operation.Location.attribute, functionArguments,
                    emptyString, Engine.FontContainer, EXE_PATH, workingDirectory
                );
            }
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(SuperEditableTextModule *& SuperEditableText : Context->Modules.SuperEditableTexts){
                    if(!findObjectForFunction(ModulesObject, Layers, SuperEditableText->getObjectID(), SuperEditableText->getLayerID())){
                        continue;
                    }
                    Event->controlSuperEditableText(SuperEditableText, Operation.Location.attribute, functionArguments,
                        ModulesObject->superEditableTextContainerIDs, Engine.FontContainer, ActiveEditableText, EXE_PATH, workingDirectory
                    );
                }
                return;
            }
            for(SuperEditableTextModule *& SuperEditableText : Context->Modules.SuperEditableTexts){
                Event->controlSuperEditableText(SuperEditableText, Operation.Location.attribute, functionArguments,
                    emptyString, Engine.FontContainer, ActiveEditableText, EXE_PATH, workingDirectory
                );
            }
            break;
        case image_mod:
        case image_mod_vec:
            switch(Operation.Location.attribute){
                case set_id:
                case set_pos:
                case set_size:
                case set_scale:
                case resize:
                case connect_bitmap:
                    for(ImageModule * Image : Context->Modules.Images){
                        if(!findObjectForFunction(ModulesObject, Layers, Image->getObjectID(), Image->getLayerID())){
                            continue;
                        }
                        Event->controlImage(Image, Operation.Location.attribute, functionArguments,
                            ModulesObject->imageContainerIDs, Engine.BitmapContainer, EXE_PATH + workingDirectory
                        );
                        if(Operation.Location.attribute != set_id){
                            ModulesObject->refreshPositionsAndSizesOfObjectAndItsImages();
                        }
                    }
                    return;
                default:
                    for(ImageModule * Image : Context->Modules.Images){
                        Event->controlImage(Image, Operation.Location.attribute, functionArguments, emptyString,
                            Engine.BitmapContainer, EXE_PATH + workingDirectory
                        );
                    }
                    return;
            }
            break;
        case movement_mod:
        case movement_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(MovementModule * Movement : Context->Modules.Movements){
                    if(!findObjectForFunction(ModulesObject, Layers, Movement->getObjectID(), Movement->getLayerID())){
                        continue;
                    }
                    Event->controlMovement(Movement, Operation.Location.attribute, functionArguments, ModulesObject->movementContainerIDs);
                }
                return;
            }
            for(MovementModule * Movement : Context->Modules.Movements){
                Event->controlMovement(Movement, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case collision_mod:
        case collision_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(CollisionModule * Collision : Context->Modules.Collisions){
                    if(!findObjectForFunction(ModulesObject, Layers, Collision->getObjectID(), Collision->getLayerID())){
                        continue;
                    }
                    Event->controlCollision(Collision, Operation.Location.attribute, functionArguments, ModulesObject->collisionContainerIDs);
                }
                return;
            }
            for(CollisionModule * Collision : Context->Modules.Collisions){
                Event->controlCollision(Collision, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case particles_mod:
        case particles_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(ParticleEffectModule * Particles : Context->Modules.Particles){
                    if(!findObjectForFunction(ModulesObject, Layers, Particles->getObjectID(), Particles->getLayerID())){
                        continue;
                    }
                    Event->controlParticles(Particles, Operation.Location.attribute, functionArguments, ModulesObject->particlesContainerIDs);
                }
                return;
            }
            for(ParticleEffectModule * Particles : Context->Modules.Particles){
                Event->controlParticles(Particles, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case event_mod:
        case event_mod_vec:
            /*
            if(Operation.Location.attribute == set_id){
                for(EventModule * Event : Context->Modules.Events){
                    if(!findObjectForFunction(ModuleObject, Layers, EditableText->getObjectID(), EditableText->getLayerID())){
                        continue;
                    }
                    Event->controlEvent(Event, Operation.Location.attribute, Variables);
                }
            }
            return;
            for(EventModule * Event : Context->Modules.Events){
                Event->controlEvent(Event, Operation.Location.attribute, Variables, emptyString);
            }*/
            break;
        case variable_mod:
        case variable_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(VariableModule * Variable : Context->Modules.Variables){
                    if(!findObjectForFunction(ModulesObject, Layers, Variable->getObjectID(), Variable->getLayerID())){
                        continue;
                    }
                    Event->controlVariables(Variable, Operation.Location.attribute, functionArguments, ModulesObject->variablesContainerIDs);
                }
                return;
            }
            for(VariableModule * Variable : Context->Modules.Variables){
                Event->controlVariables(Variable, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(ScrollbarModule * Scrollbar : Context->Modules.Scrollbars){
                    if(!findObjectForFunction(ModulesObject, Layers, Scrollbar->getObjectID(), Scrollbar->getLayerID())){
                        continue;
                    }
                    Event->controlScrollbar(Scrollbar, Operation.Location.attribute, functionArguments, ModulesObject->scrollbarContainerIDs);
                }
                return;
            }
            for(ScrollbarModule * Scrollbar : Context->Modules.Scrollbars){
                Event->controlScrollbar(Scrollbar, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case primitives_mod:
        case primitives_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(PrimitivesModule * Primitives : Context->Modules.Primitives){
                    if(!findObjectForFunction(ModulesObject, Layers, Primitives->getObjectID(), Primitives->getLayerID())){
                        continue;
                    }
                    Event->controlPrimitives(Primitives, Operation.Location.attribute, functionArguments, ModulesObject->primitivesContainerIDs);
                }
                return;
            }
            for(PrimitivesModule * Primitives : Context->Modules.Primitives){
                Event->controlPrimitives(Primitives, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            if(Operation.Location.attribute == set_id){
                for(VectorModule * Vector : Context->Modules.Vectors){
                    if(!findObjectForFunction(ModulesObject, Layers, Vector->getObjectID(), Vector->getLayerID())){
                        continue;
                    }
                    Event->controlVector(Vector, Operation.Location.attribute, functionArguments, ModulesObject->vectorContainerIDs);
                }
                return;
            }
            for(VectorModule * Vector : Context->Modules.Vectors){
                Event->controlVector(Vector, Operation.Location.attribute, functionArguments, emptyString);
            }
            break;
        case value_vec:
            if(Operation.Location.attribute == pop_back_a){
                if(Context->Values.size() == 0){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Vector of literals is empty. Cannot use 'pop_back' function.\n";
                    return;
                }
                Context->Values.pop_back();
            }
            else if(Operation.Location.attribute == push_back_a && functionArguments.size() > 0){
                Context->Values.reserve(Context->Values.size() + functionArguments.size());
                for(const VariableModule & Value : functionArguments){
                    Context->Values.push_back(Value);
                }
            }
            else if(Operation.Location.attribute == set_a && functionArguments.size() >= 2){
                if((size_t)functionArguments[0].getIntUnsafe() >= Context->Values.size() || 
                    functionArguments[0].getIntUnsafe() + (functionArguments.size()-1) > Context->Values.size()
                ){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In function " << attributeToStr(Operation.Location.attribute)
                        << " index " << functionArguments[0].getIntUnsafe() << " is out of scope.\n";
                    return;
                }
                
                for(size_t valIdx = 0; valIdx < functionArguments.size()-1; ++valIdx){
                    Context->Values[functionArguments[0].getIntUnsafe()+valIdx].copyValue(functionArguments[valIdx+1]);
                }
            }
            else if(Operation.Location.attribute == clear_a){
                Context->Values.clear();
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Function " << attributeToStr(Operation.Location.attribute)
                    << "<" << functionArguments.size() << "> does not exist.\n";
            }
            break;
        case value_inst:
            if(Context->Values.size() == 0){
                Context->Values.push_back(VariableModule());
            }
            if(Operation.Location.attribute == set_bool && functionArguments.size() > 0){
                if(!functionArguments[0].isNumeric()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Argument of '" << Operation.Location.attribute << "' function is not numeric.\n";
                    return;
                }
                Context->Values[0].setBool(functionArguments[0].getBoolUnsafe());
            }
            else if(Operation.Location.attribute == set_int && functionArguments.size() > 0){
                if(!functionArguments[0].isNumeric()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Argument of '" << Operation.Location.attribute << "' function is not numeric.\n";
                    return;
                }
                Context->Values[0].setInt(functionArguments[0].getIntUnsafe());
            }
            else if(Operation.Location.attribute == set_double && functionArguments.size() > 0){
                if(!functionArguments[0].isNumeric()){
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Argument of '" << Operation.Location.attribute << "' function is not numeric.\n";
                    return;
                }
                Context->Values[0].setDouble(functionArguments[0].getDoubleUnsafe());
            }
            else if(Operation.Location.attribute == set_string && functionArguments.size() > 0){
                Context->Values[0].setString(functionArguments[0].getStringUnsafe());
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Function " << attributeToStr(Operation.Location.attribute)
                    << "<" << functionArguments.size() << "> does not exist.\n";
            }
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Type \'"
                << dataTypeToStr(Context->type) << "\' does not exist.\n";
            break;
    }
}
void ProcessClass::changeEngineVariables(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, EngineClass & Engine){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 2 parameters.\n";
        return;
    }

    string strAttribute = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, strAttribute, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter 1.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << strAttribute << " ";
    }
    

    VariableModule FirstValue;
    if(getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, FirstValue, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter 2.\n";
        return;
    }
    if(FirstValue.getType() == 'n'){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 2 has a null value.\n";
        return;
    }
    if(printOutInstructions){
        cout << shortenText(FirstValue.getAnyValue(), maxLengthOfValuesPrinting) << " ";
    }

    VariableModule SecondValue; //sometimes optional
    getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, SecondValue, false);
    if(printOutInstructions){
        if(SecondValue.getType() != 'n'){
            cout << shortenText(SecondValue.getAnyValue(), maxLengthOfValuesPrinting) << "\n";
        }
        else{
            cout << "\n";
        }
    }

    AttributeType e_attribute = strToAttribute(strAttribute);

    switch(e_attribute){
        case window_title_a:
            if(Engine.display == nullptr){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Display was not created yet. To create it use the \"create_window\" instruction.\n";
                return;
            }
            if(Engine.windowTitle == FirstValue.getStringUnsafe()){
                return;
            }
            Engine.windowTitle = FirstValue.getStringUnsafe();
            al_set_window_title(Engine.display, Engine.windowTitle.c_str());
            return;
        case display_size_a:
            if(Engine.display == nullptr){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Display was not created yet. To create it use the \"create_window\" instruction.\n";
                return;
            }
            if(SecondValue.getType() == 'n'){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Changing the attribute '" << strAttribute << "' requires 2 values.\n";
                return;
            }
            if(!FirstValue.isNumeric() || !SecondValue.isNumeric()){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Changing the attribute '" << strAttribute << "' requires 2 last parameters to be of a numeric type.\n";
                return;
            }
            if(Engine.displaySize.isEqual(FirstValue.getInt(), SecondValue.getInt())){
                return;
            }
            Engine.displaySize.set(FirstValue.getInt(), SecondValue.getInt());
            if(!al_resize_display(Engine.display, Engine.displaySize.x, Engine.displaySize.y)){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "al_resize_display() failed to resize the display.\n";
                /*#if __WIN32__
                    cerr << instructionError(CurrentInstr, __FUNCTION__) << "al_resize_display() failed to resize the display.\n";
                #else
                    cout << "Warning: In " << EventIds.describe() << ": In " << __FUNCTION__ << ": al_resize_display() stops to work on Linux systems if the display resolution is set too high.\n";
                #endif*/
            }
            return;
        case fullscreen_a:
            if(Engine.display == nullptr){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Display was not created yet. To create it use the \"create_window\" instruction.\n";
                return;
            }
            if(FirstValue.getBool() == Engine.fullscreen){
                return;
            }
            Engine.fullscreen = FirstValue.getBool();
            al_set_display_flag(Engine.display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(Engine.display) & ALLEGRO_FULLSCREEN_WINDOW));
            #if __WIN32__
            al_set_display_flag(Engine.display, ALLEGRO_MAXIMIZED, !(al_get_display_flags(Engine.display) & ALLEGRO_MAXIMIZED));
            #endif
            //al_set_display_flag(window, ALLEGRO_NOFRAME, !(al_get_display_flags(window) & ALLEGRO_NOFRAME));
            return;
        case pixel_art_a:
            if(FirstValue.getBool() == Engine.isPixelArt){
                return;
            }
            Engine.isPixelArt = FirstValue.getBool();
            if(!Engine.isPixelArt){
                al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
            }
            else{
                al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
            }
            return;
        case can_afk_timeout_a:
            Engine.canTerminateWithTimeout = FirstValue.getBool();
            return;
        case afk_timeout_time_a:
            if(!FirstValue.isNumeric()){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Changing the attribute '" << strAttribute
                    << "' requires the 'time' parameter to be of a numeric type.\n";
                return;
            }
            Engine.timeoutTerminationTime = FirstValue.getInt();
            Engine.terminationTimer = Engine.timeoutTerminationTime;
            return;
        case can_exit_on_idle_a:
            Engine.canExitWhenNoEventIsTriggered = FirstValue.getBool();
            return;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '"
                << strAttribute << "' is not valid for this instruction.\n";
            return;
    }
}
void ProcessClass::changeProcessVariables(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, vector<string> & processIDs){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 parameter.\n";
        return;
    }

    string attribute = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, attribute, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter 1.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << attribute << " ";
    }

    if(attribute == "clear_layers"){
        wasDeleteExecuted = true;
        for(LayerClass & Layer : Layers){
            Layer.deleteLater();
        }
        printOutInstructions && cout << "\n";
        return;
    }
    else if(attribute == "clear_cameras"){
        wasDeleteExecuted = true;
        for(Camera2D & Camera : Cameras){
            Camera.deleteLater();
        }
        printOutInstructions && cout << "\n";
        return;
    }

    if(Operation.rootParametersSize < 2){
        printOutInstructions && cout << "\n";
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 2 parameters.\n";
        return;
    }

    VariableModule FirstValue;
    if(getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, FirstValue, true)){
        printOutInstructions && cout << "\n";
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter 2.\n";
        return;
    }
    if(FirstValue.getType() == 'n'){
        printOutInstructions && cout << "\n";
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 2 has a null value.\n";
        return;
    }
    if(printOutInstructions){
        cout << shortenText(FirstValue.getAnyValue(), maxLengthOfValuesPrinting) << " ";
    }

    VariableModule SecondValue; //sometimes optional
    getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, SecondValue, false);
    if(printOutInstructions){
        if(SecondValue.getType() != 'n'){
            cout << shortenText(SecondValue.getAnyValue(), maxLengthOfValuesPrinting) << " ";
        }
        if(attribute != "window_tint"){
            cout << "\n";
        }
    }
    
    if(attribute == "id"){
        if(FirstValue.getType() != 's'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter 2 must be of a string type.\n";
            return;
        }
        setID(FirstValue.getString(), processIDs); 
    }
    else if(attribute == "is_active"){
        isActive = FirstValue.getBool(); 
    }
    else if(attribute == "can_interact_with_user"){
        canUserInteract = FirstValue.getBool(); 
    }
    else if(attribute == "is_rendering"){
        isRendering = FirstValue.getBool(); 
    }
    else if(attribute == "draw_camera_borders"){
        drawCameraBorders = FirstValue.getBool(); 
    }
    else if(attribute == "draw_text_borders"){
        drawTextFieldBorders = FirstValue.getBool(); 
    }
    else if(attribute == "draw_hitboxes"){
        drawHitboxes = FirstValue.getBool(); 
    }
    else if(attribute == "ignore_distant"){
        ignoreDistantObjects = FirstValue.getBool(); 
    }
    else if(attribute == "draw_only_visible"){
        drawOnlyVisibleObjects = FirstValue.getBool(); 
    }
    else if(attribute == "bitmap_layers_number"){
        totalNumberOfBitmapLayers = FirstValue.getInt(); 
    }
    else if(attribute == "print_logical_evaluations"){
        printOutLogicalEvaluations = FirstValue.getBool(); 
    }
    else if(attribute == "print_instructions"){
        printOutInstructions = FirstValue.getBool();
        if(SecondValue.isNumeric()){
            maxLengthOfValuesPrinting = SecondValue.getIntUnsafe();
        }
    }
    else if(attribute == "auto_print_stack"){
        printOutStackAutomatically = FirstValue.getBool(); 
    }
    else if(attribute == "reservation_multiplier"){
        if(FirstValue.getType() != 'd'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter 2 must be of a double type.\n";
            return;
        }
        reservationMultiplier = FirstValue.getDouble(); 
    }
    else if(attribute == "window_pos"){
        if(SecondValue.getType() == 'n'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Changing the attribute \'" << attribute << "\' requires 2 values.\n";
            return;
        }
        if(!FirstValue.isNumeric() || !SecondValue.isNumeric()){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Changing the attribute '" << attribute << "' requires 2 last parameters to be of a numeric type.\n";
            return;
        }
        windowPos.set(FirstValue.getInt(), SecondValue.getInt());
    }
    else if(attribute == "window_size"){
        if(SecondValue.getType() == 'n'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Changing the attribute \'" << attribute << "\' requires 2 values.\n";
            return;
        }
        if(!FirstValue.isNumeric() || !SecondValue.isNumeric()){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Changing the attribute '" << attribute << "' requires 2 last parameters to be of a numeric type.\n";
            return;
        }
        windowSize.set(FirstValue.getInt(), SecondValue.getInt());
        resizeWindow(windowSize);
    }
    else if(attribute == "min_window_size"){
        if(SecondValue.getType() == 'n'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Changing the attribute \'" << attribute << "\' requires 2 values.\n";
            return;
        }
        if(!FirstValue.isNumeric() || !SecondValue.isNumeric()){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Changing the attribute '" << attribute << "' requires 2 last parameters to be of a numeric type.\n";
            return;
        }
        minWindowSize.set(FirstValue.getInt(), SecondValue.getInt());
    }
    else if(attribute == "window_tint"){

        //Please don't look below. This scope is terrible.

        if(Operation.rootParametersSize < 5){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Changing the attribute \'" << attribute << "\' requires 4 values.\n";
            return;
        }
        
        VariableModule ThirdValue;
        getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 3, ThirdValue, false);
        if(ThirdValue.getType() == 'n'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter 4 is of a null type. Changing the attribute \'" << attribute << "\' requires 4 non-null values.\n";
            return;
        }
        if(printOutInstructions){
            cout << shortenText(ThirdValue.getAnyValue(), maxLengthOfValuesPrinting) << " ";
        }

        VariableModule FourthValue;
        getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 4, FourthValue, false);
        if(FourthValue.getType() == 'n'){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Parameter 5 is of a null type. Changing the attribute \'" << attribute << "\' requires 4 non-null values.\n";
            return;
        }
        if(printOutInstructions){
            cout << shortenText(FourthValue.getAnyValue(), maxLengthOfValuesPrinting) << "\n";
        }
        
        if(!FirstValue.isNumeric() || !SecondValue.isNumeric()
            || !ThirdValue.isNumeric() || !FourthValue.isNumeric()
        ){
            printOutInstructions && cout << "\n";
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Changing the attribute \'" << attribute << "\' requires 4 last parameters to be of a numeric type.\n";
            return;
        }
        windowTint[0] = FirstValue.getDouble();
        windowTint[1] = SecondValue.getDouble();
        windowTint[2] = ThirdValue.getDouble();
        windowTint[3] = FourthValue.getDouble();
    }
    else{
        printOutInstructions && cout << "\n";
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute \'" << attribute << "\' is not valid.\n";
    }
}
void ProcessClass::loadBitmap(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, vector<SingleBitmap> & BitmapContainer){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 2 parameters.\n";
        return;
    }

    string pathToTheBitmap = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pathToTheBitmap, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }

    string nameForTheBitmap = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, nameForTheBitmap, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2.\n";
        return;
    }

    bool createLightBitmap = false, ignoreWarnings = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, createLightBitmap, false);
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 3, ignoreWarnings, false);
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << pathToTheBitmap << " " << nameForTheBitmap
            << " " << createLightBitmap << " " << ignoreWarnings << "\n";
    }

    BitmapContainer.emplace_back(SingleBitmap());
    BitmapContainer.back().loadBitmap(nameForTheBitmap, pathToTheBitmap, EXE_PATH + workingDirectory, createLightBitmap);
}
void ProcessClass::createDirectory(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 1 string parameter.\n";
        return;
    }

    string pathToTheDirectory = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pathToTheDirectory, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << pathToTheDirectory << "\n";
    }

    if(pathToTheDirectory == "" || pathToTheDirectory == "~/" || pathToTheDirectory[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + pathToTheDirectory << "\'.\n";
        return;
    }
    try{
        std::filesystem::create_directory(EXE_PATH + workingDirectory + pathToTheDirectory);
    }
    catch(std::filesystem::filesystem_error const& ex){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "No such directory \'" << EXE_PATH + workingDirectory + pathToTheDirectory << "\'.\n";
    }
}
void ProcessClass::removeFileOrDirectory(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 1 string parameter.\n";
        return;
    }

    string path = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, path, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << path << "\n";
    }

    if(path == "" || path == "~/" || path[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + path << "\'.\n";
        return;
    }
    try{
        std::filesystem::remove(EXE_PATH + workingDirectory + path);
    }
    catch(std::filesystem::filesystem_error const& ex){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "" << ex.what() << "\n";
    }
}
void ProcessClass::removeRecursivelyFileOrDirectory(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 1 string parameter.\n";
        return;
    }

    string path = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, path, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << path << "\n";
    }

    if(path == "" || path == "~/" || path[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + path << "\'.\n";
        return;
    }
    try{
        std::filesystem::remove_all(EXE_PATH + workingDirectory + path);
    }
    catch(std::filesystem::filesystem_error const& ex){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "" << ex.what() << "\n";
    }
}
void ProcessClass::renameFileOrDirectory(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 1 string parameter.\n";
        return;
    }

    string originalPath = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, originalPath, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    if(originalPath == "" || originalPath == "~/" || originalPath[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + originalPath << "\'.\n";
        return;
    }

    string newPath = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, newPath, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2.\n";
        return;
    }
    if(newPath == "" || newPath == "~/" || newPath[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + newPath << "\'.\n";
        return;
    }

    try{
        std::filesystem::rename(EXE_PATH + workingDirectory + originalPath, EXE_PATH + workingDirectory + newPath);
    }
    catch(std::filesystem::filesystem_error const& ex){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "" << ex.what() << "\n";
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
string catchQuotes(const string & input){
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
void ProcessClass::executePrint(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 string parameter.\n";
        return;
    }

    string delimeter = "";
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, delimeter, false);

    //Get contexts from all parameters of this instruction
    vector<ContextClass> ValueContexts;
    for(unsigned index = 1; index < Operation.rootParametersSize; index++){
        ValueContexts.emplace_back(ContextClass());
        bool negateAfterCopy = false; //ignore
        if(ValueContexts.back().copyFromTheParameter(ObjectMemory.MemoryMap,
            LocalToGlobalTranslation, CurrentInstr, Operation.Parameters,
            negateAfterCopy, index, true
        )){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Failed to get the context from the parameter " << index+2 << ".\n";
            return;
        }
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction)
            << " \"" << shortenText(delimeter, maxLengthOfValuesPrinting) << "\" ";
        if(Operation.Output.variableID != ""){
            cout << Operation.Output.variableID << " ";
        }
        if(ValueContexts.size() > 0){
            cout << "{";
            for(const ContextClass & Value : ValueContexts){
                if(Value.type == value_vec){
                    cout << "[";
                    for(const VariableModule & LiteralValues : Value.Values){
                        cout << shortenText(LiteralValues.getAnyValue(), maxLengthOfValuesPrinting) << ", ";
                    }
                    cout << "]";
                }
                else if(Value.type == value_inst && Value.Values.size() > 0){
                    cout << shortenText(Value.Values[0].getAnyValue(), maxLengthOfValuesPrinting) << ", ";
                }
                else{
                    cout << dataTypeToStr(Value.type);
                }
                cout << "<" << Value.getVectorSize() << ">, ";
            }
            
            cout << "}<" << ValueContexts.size() << ">";
        }
        cout << "\n";
    }

    string buffer = "";

    for(const ContextClass & Value : ValueContexts){
        switch(Value.type){
            case value_inst:
            case value_vec:
                for(const VariableModule & Variable : Value.Values){
                    buffer += catchQuotes(Variable.getStringUnsafe()) + delimeter;
                }
                break;
            case pointer_inst:
            case pointer_vec:
                for(const BasePointersStruct & Pointer : Value.BasePointers){
                    buffer += catchQuotes(Pointer.getString()) + delimeter;
                }
                break;
            case variable_mod:
            case variable_mod_vec:
                for(const VariableModule * Variable : Value.Modules.Variables){
                    buffer += catchQuotes(Variable->getStringUnsafe()) + delimeter;
                }
                break;
            case vector_mod:
            case vector_mod_vec:
                for(const VectorModule * Vector : Value.Modules.Vectors){
                    vector<string> stringsFromVector = Vector->getAllValuesAsStringVector();
                    for(string text : stringsFromVector){
                        buffer += catchQuotes(text) + delimeter;
                    }
                }
                break;
            case camera_inst:
            case camera_vec:
                buffer += getStringOfIDs(Value.Cameras, delimeter);
                break;
            case layer_inst:
            case layer_vec:
                buffer += getStringOfIDs(Value.Layers, delimeter);
                break;
            case object_inst:
            case object_vec:
                buffer += getStringOfIDs(Value.Objects, delimeter);
                break;
            case super_text_mod:
            case super_text_mod_vec:
                buffer += getStringOfIDs(Value.Modules.SuperTexts, delimeter);
                break;
            case super_editable_text_mod:
            case super_editable_text_mod_vec:
                buffer += getStringOfIDs(Value.Modules.SuperEditableTexts, delimeter);
                break;
            case image_mod:
            case image_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Images, delimeter);
                break;
            case movement_mod:
            case movement_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Movements, delimeter);
                break;
            case collision_mod:
            case collision_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Collisions, delimeter);
                break;
            case particles_mod:
            case particles_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Particles, delimeter);
                break;
            case event_mod:
            case event_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Events, delimeter);
                break;
            case scrollbar_mod:
            case scrollbar_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Scrollbars, delimeter);
                break;
            case primitives_mod:
            case primitives_mod_vec:
                buffer += getStringOfIDs(Value.Modules.Primitives, delimeter);
                break;
            case null_dt:
                break;
            default:
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "DataType \'" << dataTypeToStr(Value.type) << "\' is not valid for this operation.\n";
                break;
        }
    }

    if(buffer == ""){
        buffer += delimeter;
    }
    else{
        buffer = buffer.substr(0, buffer.size() - delimeter.size());
    }

    if(Operation.Output.type == null_dt){
        cout << buffer;
        cout.flush();
    }
    else{
        NewContext.clear();
        NewContext.type = value_inst;
        NewContext.Values.emplace_back(VariableModule::newString(buffer));
        assignVariable(ObjectMemory, Operation.Output);
    }
}
void ProcessClass::loadFileAsString(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 string parameter.\n";
        return;
    }

    string pathToTheFile = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pathToTheFile, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << pathToTheFile << " " << Operation.Output.variableID << "\n";
    }
    if(pathToTheFile == "" || pathToTheFile == "~/" || pathToTheFile[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: '" << EXE_PATH + workingDirectory + pathToTheFile << "'.\n";
        return;
    }

    string loadedText = "";

    std::ifstream File(EXE_PATH + workingDirectory + pathToTheFile);

	if(!File){
		cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Cannot open the file: " << EXE_PATH + workingDirectory + pathToTheFile << "\n";
        return;
    }
    for(string line; std::getline(File, line);){
        loadedText += line + "\n";
    }
    File.close();

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.push_back(VariableModule::newString(loadedText));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::saveStringAsFile(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 2 string parameters.\n";
        return;
    }

    string pathToTheFile = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pathToTheFile, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    if(pathToTheFile == "" || pathToTheFile == "~/" || pathToTheFile[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + pathToTheFile << "\'.\n"; 
        return;
    }

    vector<string> textToWrite;
    if(getStringVectorFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, textToWrite, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2.\n";
        return;
    }

    string delimeter = "";
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, delimeter, false);

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << pathToTheFile << " ";
        printStringVectorForInstruction(textToWrite, maxLengthOfValuesPrinting);
        cout << "\"" << delimeter << "\"\n";
    }

    string finalPath = EXE_PATH + workingDirectory + pathToTheFile;

    if(pathToTheFile.substr(0, 2) == "~/"){
        finalPath = EXE_PATH + pathToTheFile.substr(2, pathToTheFile.size()-2);
    }

    std::ofstream File(finalPath);

	if(!File){
		cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Cannot open the file: '" << finalPath << "'.\n";
        return;
    }
    for(const string & text : textToWrite){
        File << text << delimeter;
    }
    File.close();
}
void ProcessClass::listOutEntities(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, const vector<ProcessClass> & Processes, const EngineClass & Engine){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 string parameter.\n";
        return;
    }
    
    string source = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, source, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }

    bool printDetails = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, printDetails, false);
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << source << " " << printDetails << "\n";
    }

    if(source == "processes"){
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
    else if(source == "layers"){
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
    else if(source == "cameras"){
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
    else if(source == "bitmaps"){
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
    else if(source == "fonts"){
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
void ProcessClass::createNewProcess(OperationClass & Operation, vector<ProcessClass> & Processes, ObjectMemoryStruct & ObjectMemory,
    AncestorObject *& Owner, vector<AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack, EngineClass & Engine
){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 string parameter.\n";
        return;
    }
    
    string processID = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, processID, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    if(processID == ""){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 1 cannot have an empty string.\n";
        return;
    }
    
    string layerID = "", objectID = "", pathToTheScript = "";
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, layerID, false);
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, objectID, false);
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 3, pathToTheScript, false);

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << processID << " " << layerID << " " << objectID << " " << pathToTheScript << "\n";
    }

    if(pathToTheScript == "" || pathToTheScript == "~/" || pathToTheScript[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + pathToTheScript << "\'.\n"; 
        return;
    }
    
    if(Processes.size() + 1 <= Processes.capacity()){
        Processes.emplace_back(ProcessClass());
        Processes.back().create(Engine.EXE_PATH + workingDirectory, Engine.allowNotAscii, Engine.getDisplaySize(),
            pathToTheScript, processID, layerID, objectID, Engine.processIDs
        );
    }
    else{
        
    }
}
void ProcessClass::createNewOwnerVariable(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, AncestorObject * Owner, 
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & MemoryStack
){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction requires at least 1 parameter.\n";
        return;
    }

    VariableModule Value;
    if(getValueFromParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 0, Value, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a value from the parameter 1.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " "
            << shortenText(Value.getAnyValue(), maxLengthOfValuesPrinting)
            << " " << Operation.Output.variableID << "\n";
    }

    for(const VariableModule & Variable : Owner->VariablesContainer){
        if(Variable.getID() == Operation.Output.variableID){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Cannot create a variable with id \'"
                << Operation.Output.variableID
                << "\', because a variable with the same id already exists inside '"
                << Owner->getID() << "' object.\n";
            return;
        }
    }

    PointerRecalculator Recalculator;

    Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, MemoryStack, 
        ActiveEditableText, CurrentInstr
    );

    PrimaryData initData = {
        .topIndex = &topModuleUniqueIndex,
        .objectUniqueIndex = Owner->getUniqueIndex(),
        .layerUniqueIndex = Owner->getLayerUniqueIndex(),
        .newID = Operation.Output.variableID,
        .listOfIDs = &Owner->variablesContainerIDs,
        .newLayerID = Owner->getLayerID(),
        .newObjectID = Owner->getID()
    };
    
    if(Value.getType() == 'b'){
        Owner->VariablesContainer.emplace_back(VariableModule::newBool(Value.getBool(), initData));
    }
    else if(Value.getType() == 'i'){
        Owner->VariablesContainer.emplace_back(VariableModule::newInt(Value.getInt(), initData));
    }
    else if(Value.getType() == 'd'){
        Owner->VariablesContainer.emplace_back(VariableModule::newDouble(Value.getDouble(),
            initData
        ));
    }
    else if(Value.getType() == 's'){
        Owner->VariablesContainer.emplace_back(VariableModule::newString(Value.getString(),
            initData
        ));
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 1 has an invalid type: \'" << Value.getType() << "\'.\n";
        return;
    }

    Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
        MemoryStack, ActiveEditableText, CurrentInstr
    );

    NewContext.clear();
    if(Owner->VariablesContainer.size() > 0){
        NewContext.Modules.Variables.push_back(&Owner->VariablesContainer.back());
        NewContext.type = variable_mod;
        NewContext.isPointingToMember = true;
        NewContext.containerIndex = Owner->VariablesContainer.size() - 1;
        assignVariable(ObjectMemory, Operation.Output);
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction \'" << instrToStr(Operation.instruction) << "\' failed.\n";
    }
}
void ProcessClass::createNewOwnerVector(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory, AncestorObject * Owner,
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt, 
    vector<EventStackStruct> & MemoryStack
){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction requires 2 parameters.\n";
        return;
    }

    string vectorType = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
            Operation.Parameters, 0, vectorType, true
        )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }

    vector<VariableModule> Values;
    getValuesFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 1, Values, false
    );
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << vectorType << " [";
        for(const VariableModule & Value : Values){
            cout << shortenText(Value.getAnyValue(), maxLengthOfValuesPrinting) << ", ";
        }
        cout << "] " << Operation.Output.variableID << "\n";
    }

    for(const VectorModule & Vector : Owner->VectorContainer){
        if(Vector.getID() == Operation.Output.variableID){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Cannot create a vector with the id '"
                << Operation.Output.variableID
                << "', because a vector with the same id already exists.\n";
            return;
        }
    }

    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, ObjectMemory, startingEventIt, eventIt, MemoryStack,
        ActiveEditableText, CurrentInstr
    );
    
    PrimaryData initData = {
        .topIndex = &topModuleUniqueIndex,
        .objectUniqueIndex = Owner->getUniqueIndex(),
        .layerUniqueIndex = Owner->getLayerUniqueIndex(),
        .newID = Operation.Output.variableID,
        .listOfIDs = &Owner->vectorContainerIDs,
        .newLayerID = Owner->getLayerID(),
        .newObjectID = Owner->getID()
    };
    Owner->VectorContainer.emplace_back(initData, vectorType);

    if(Owner->VectorContainer.back().getType() == 'n' && vectorType != "null"){
        Owner->VectorContainer.pop_back();
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Type '" << vectorType << "' is not valid.\n";
        return;
    }

    if(vectorType == "bool"){
        Owner->VectorContainer.back().setType('b');
        for(const VariableModule & Value : Values){
            if(Value.getType() != 'b'){
                Owner->VectorContainer.pop_back();
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Literal (" << Value.getType() << ") is not of a " << vectorType
                    << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushBoolUnsafe(Value.getBool());
        }
    }
    else if(vectorType == "int"){
        Owner->VectorContainer.back().setType('i');
        for(const VariableModule & Value : Values){
            if(Value.getType() != 'i'){
                Owner->VectorContainer.pop_back();
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Literal (" << Value.getType() << ") is not of a " << vectorType
                    << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushIntUnsafe(Value.getInt());
        }
    }
    else if(vectorType == "double"){
        Owner->VectorContainer.back().setType('d');
        for(const VariableModule & Value : Values){
            if(Value.getType() != 'd'){
                Owner->VectorContainer.pop_back();
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Literal (" << Value.getType() << ") is not of a " << vectorType
                    << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushDoubleUnsafe(Value.getDouble());
        }
    }
    else if(vectorType == "string"){
        Owner->VectorContainer.back().setType('s');
        for(const VariableModule & Value : Values){
            if(Value.getType() != 's'){
                Owner->VectorContainer.pop_back();
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Literal ("
                    << Value.getType() << ") is not of a " << vectorType << " type.\n";
                return;
            }
            Owner->VectorContainer.back().pushStringUnsafe(Value.getString());
        }
    }
    else{
        Owner->VectorContainer.pop_back();
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 2 has an invalid value: \'" << vectorType << "\'.\n";
        return;
    }

    Recalculator.updatePointersToModules(Layers, ObjectMemory, startingEventIt, eventIt,
        MemoryStack, ActiveEditableText, CurrentInstr
    );

    NewContext.clear();
    if(Owner->VectorContainer.size() > 0){
        NewContext.Modules.Vectors.push_back(&Owner->VectorContainer.back());
        NewContext.type = vector_mod;
        NewContext.isPointingToMember = true;
        NewContext.containerIndex = Owner->VectorContainer.size() - 1;
        assignVariable(ObjectMemory, Operation.Output);
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction \'"
            << instrToStr(Operation.instruction) << "\' failed.\n";
    }
}
void ProcessClass::tokenizeStringFromContext(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 2 string parameters.\n";
        return;
    }

    string delimeter = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, delimeter, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    if(delimeter.size() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Delimeter cannot be empty.\n";
        return;
    }
    if(delimeter.size() > 1){
        cout << instructionWarning(CurrentInstr, __FUNCTION__)
            << "Instruction will use only the first character of the \"" << delimeter << "\" string as a delimeter.\n";
    }

    string text = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, text, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2.\n";
        return;
    }

    vector<string> tokenizedWords = tokenizeString(text, delimeter[0]);

    auto[Outputs, error] = getAllVariablesPointerFromParameters(
        ObjectMemory.MemoryMap, LocalToGlobalTranslation, CurrentInstr, Operation.Parameters, 2, true
    );

    if(error){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to gather output variables.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << delimeter << " \"" << shortenText(text, maxLengthOfValuesPrinting) << "\" [";
        for(const ContextClass * OutputVariable : Outputs){
            cout << OutputVariable->ID << " ";
        }
        cout << "]\n";
    }

    NewContext.clear();
    NewContext.type = value_vec;
    if(Outputs.empty()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction has no output variables. Nothing to do.\n";
        return;
    }

    if(Outputs.size() == 1){
        NewContext.type = value_vec;
        for(const string & word : tokenizedWords){
            NewContext.Values.emplace_back(VariableModule::newString(word));
        }

        assignVariable(ObjectMemory, Outputs[0]);

        return;
    }

    if(tokenizedWords.size() > Outputs.size()){
        cerr << instructionWarning(CurrentInstr, __FUNCTION__)
            << "Number of extracted tokens (" << tokenizedWords.size()
            << ") is not equal to the number of provided outputs (" << Outputs.size() << ").\n";
    }

    NewContext.Values.emplace_back(VariableModule::newString(""));
    for(unsigned index = 0; index < tokenizedWords.size() && index < Outputs.size(); index++){
        NewContext.Values.back().setString(tokenizedWords[index]);
        assignVariable(ObjectMemory, Outputs[index]); 
    }
}
void ProcessClass::printTree(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, vector<ProcessClass> & Processes){
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
                for(const EventModule & Event : Object.EventContainer){
                    if(Event.isFunction){
                        buffor += "\t\t\tFunction " + Event.getID();
                    }
                    else{
                        buffor += "\t\t\tEvent " + Event.getID();
                    }
                    if(!Event.getIsActive()){
                        buffor += " (disabled)";
                    }
                    buffor += "\n";
                    for(const ChildStruct & Child : Event.Children){
                        buffor += "\t\t\t\tEvent::Child " + Child.id;
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
                    buffor += "\t\t\tPrimitive::" + transPrimitiveTypeToString(Primitive.type) + " " + Primitive.getID();
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
        cout << instrToStr(Operation.instruction) << " " << Operation.Output.variableID << "\n";
    }
    if(Operation.Output.type == null_dt){
        cout << buffor;
        return;
    }

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newString(buffor));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::getStringSizeFromContext(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 string parameter.\n";
        return;
    }

    string text = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, text, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " \"" << shortenText(text, 100) << "\" " << Operation.Output.variableID << " " << "\n";
    }

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newInt(text.size()));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::getSizeOfContext(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    ContextClass * Context = nullptr;
    
    if(getContextPointerFromTheParameter(Context, ObjectMemory, LocalToGlobalTranslation, CurrentInstr, Operation.Parameters, 0, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 1.\n";
        return;
    }
    
    int size = 0;
    size = Context->getVectorSize();

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << Context->ID << "\n";
    }

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newInt(size));
    moveToVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::getSubStringFromContext(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 3){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 3 parameters.\n";
        return;
    }

    string text = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, text, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    unsigned beginning = 0;
    if(getUnsignedFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, LeftOperandProc, CurrentInstr, Operation.Parameters, 1, beginning, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get an unsigned value from the parameter 2.\n";
        return;
    }
    unsigned length = 0;
    if(getUnsignedFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, LeftOperandProc, CurrentInstr, Operation.Parameters, 2, length, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get an unsigned value from the parameter 3.\n";
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
        cout << instrToStr(Operation.instruction) << " \"" << shortenText(text, maxLengthOfValuesPrinting)
            << "\" " << beginning << " " << length << " " << Operation.Output.variableID << "\n";
    }

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newString(text.substr(beginning, length)));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::loadFontFromContext(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, EngineClass & Engine){
    if(Operation.rootParametersSize < 3){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 3 parameters.\n";
        return;
    }

    string pathToTheFont = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pathToTheFont, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }
    unsigned fontSize = 0;
    if(getUnsignedFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, LeftOperandProc, CurrentInstr, Operation.Parameters, 1, fontSize, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get an unsigned value from the parameter 2.\n";
        return;
    }
    string fontID = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, fontID, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 3.\n";
        return;
    }
    bool ignoreWarnings = false;
    if(getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 3, ignoreWarnings, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a bool value from the parameter 4.\n";
        return;
    }

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << pathToTheFont << " " << fontSize << " " << fontID << " " << ignoreWarnings << "\n";
    }

    if(pathToTheFont == "" || pathToTheFont == "~/" || pathToTheFont[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + pathToTheFont << "\'.\n"; 
        return;
    }

    for(const SingleFont & Font : Engine.FontContainer){
        if(Font.ID == fontID){
            if(!ignoreWarnings){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Loading failed. Font with the id \'" << Font.ID << "\' already exists.\n";
            }
            return;
        }
    }

    Engine.loadNewFont(pathToTheFont, fontSize, fontID);
}
void ProcessClass::findByIDInObjectMemory(OperationClass & Operation,
    ObjectMemoryStruct & ObjectMemory
){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction requires 2 parameters.\n";
        return;
    }

    ContextClass SourceContext;
    bool negateAfterCopy = false; //it's only searching
    if(SourceContext.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a context from the parameter 1.\n";
        return;
    }
    
    string entityID = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 1, entityID, true
    )){;
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2.\n";
        return;
    }

    if(entityID == ""){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter 2 (id) cannot be empty.\n";
        return;
    }

    NewContext.clear();
    NewContext.type = instantiateEntityDataType(CurrentInstr, SourceContext.type);

    if(printOutInstructions){
        cerr << instrToStr(Operation.instruction) << " " << SourceContext.ID << " " << entityID
            << " " << Operation.Output.variableID << "\n";
    }

    auto ifEmpty = [](bool isEmpty, const DataType type,
        const InstrDescription & CurrentInstr
    ){
        if(isEmpty){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "There are no instances of '" << dataTypeToStr(type)
                << "' type in the context.\n";
            return true;
        }
        return false;
    };
    
    switch(SourceContext.type){
        case variable_mod:
            if(ifEmpty(SourceContext.Modules.Variables.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Variables[0]->getID() == entityID){
                SourceContext.Modules.Variables.push_back(SourceContext.Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            for(VariableModule * Variable : SourceContext.Modules.Variables){
                if(Variable->getID() == entityID){
                    SourceContext.Modules.Variables.push_back(Variable);
                    break;
                }
            }
            break;
        case camera_inst:
            if(ifEmpty(SourceContext.Cameras.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Cameras[0]->getID() == entityID){
                NewContext.Cameras.push_back(SourceContext.Cameras[0]);
            }
            break;
        case camera_vec:
            for(Camera2D * Camera : SourceContext.Cameras){
                if(Camera->getID() == entityID){
                    NewContext.Cameras.push_back(Camera);
                    break;
                }
            }
            break;
        case layer_inst:
            if(ifEmpty(SourceContext.Layers.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Layers[0]->getID() == entityID){
                NewContext.Layers.push_back(SourceContext.Layers[0]);
            }
            break;
        case layer_vec:
            for(LayerClass * Layer : SourceContext.Layers){
                if(Layer->getID() == entityID){
                    NewContext.Layers.push_back(Layer);
                    break;
                }
            }
            break;
        case object_inst:
            if(ifEmpty(SourceContext.Objects.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Objects[0]->getID() == entityID){
                NewContext.Objects.push_back(SourceContext.Objects[0]);
            }
            break;
        case object_vec:
            for(AncestorObject * Object : SourceContext.Objects){
                if(Object->getID() == entityID){
                    NewContext.Objects.push_back(Object);
                    break;
                }
            }
            break;
        case super_text_mod:
            if(ifEmpty(SourceContext.Modules.SuperTexts.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.SuperTexts[0]->getID() == entityID){
                NewContext.Modules.SuperTexts.push_back(SourceContext.Modules.SuperTexts[0]);
            }
            break;
        case super_text_mod_vec:
            for(SuperTextModule * SuperText : SourceContext.Modules.SuperTexts){
                if(SuperText->getID() == entityID){
                    NewContext.Modules.SuperTexts.push_back(SuperText);
                    break;
                }
            }
            break;
        case super_editable_text_mod:
            if(ifEmpty(SourceContext.Modules.SuperEditableTexts.empty(), SourceContext.type,
                CurrentInstr
            )) return;
            if(SourceContext.Modules.SuperEditableTexts[0]->getID() == entityID){
                NewContext.Modules.SuperEditableTexts.push_back(SourceContext.Modules.SuperEditableTexts[0]);
            }
            break;
        case super_editable_text_mod_vec:
            for(SuperEditableTextModule * SuperEditableText : SourceContext.Modules.SuperEditableTexts){
                if(SuperEditableText->getID() == entityID){
                    NewContext.Modules.SuperEditableTexts.push_back(SuperEditableText);
                    break;
                }
            }
            break;
        case image_mod:
            if(ifEmpty(SourceContext.Modules.Images.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Images[0]->getID() == entityID){
                NewContext.Modules.Images.push_back(SourceContext.Modules.Images[0]);
            }
            break;
        case image_mod_vec:
            for(ImageModule * Image : SourceContext.Modules.Images){
                if(Image->getID() == entityID){
                    NewContext.Modules.Images.push_back(Image);
                    break;
                }
            }
            break;
        case movement_mod:
            if(ifEmpty(SourceContext.Modules.Movements.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Movements[0]->getID() == entityID){
                NewContext.Modules.Movements.push_back(SourceContext.Modules.Movements[0]);
            }
            break;
        case movement_mod_vec:
            for(MovementModule * Movement : SourceContext.Modules.Movements){
                if(Movement->getID() == entityID){
                    NewContext.Modules.Movements.push_back(Movement);
                    break;
                }
            }
            break;
        case collision_mod:
            if(ifEmpty(SourceContext.Modules.Collisions.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Collisions[0]->getID() == entityID){
                NewContext.Modules.Collisions.push_back(SourceContext.Modules.Collisions[0]);
            }
            break;
        case collision_mod_vec:
            for(CollisionModule * Collision : SourceContext.Modules.Collisions){
                if(Collision->getID() == entityID){
                    NewContext.Modules.Collisions.push_back(Collision);
                    break;
                }
            }
            break;
        case particles_mod:
            if(ifEmpty(SourceContext.Modules.Particles.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Particles[0]->getID() == entityID){
                NewContext.Modules.Particles.push_back(SourceContext.Modules.Particles[0]);
            }
            break;
        case particles_mod_vec:
            for(ParticleEffectModule * Particle : SourceContext.Modules.Particles){
                if(Particle->getID() == entityID){
                    NewContext.Modules.Particles.push_back(Particle);
                    break;
                }
            }
            break;
        case event_mod:
            if(ifEmpty(SourceContext.Modules.Events.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Events[0]->getID() == entityID){
                NewContext.Modules.Events.push_back(SourceContext.Modules.Events[0]);
            }
            break;
        case event_mod_vec:
            for(EventModule * Event : SourceContext.Modules.Events){
                if(Event->getID() == entityID){
                    NewContext.Modules.Events.push_back(Event);
                    break;
                }
            }
            break;
        case scrollbar_mod:
            if(ifEmpty(SourceContext.Modules.Scrollbars.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Scrollbars[0]->getID() == entityID){
                NewContext.Modules.Scrollbars.push_back(SourceContext.Modules.Scrollbars[0]);
            }
            break;
        case scrollbar_mod_vec:
            for(ScrollbarModule * Scrollbar : SourceContext.Modules.Scrollbars){
                if(Scrollbar->getID() == entityID){
                    NewContext.Modules.Scrollbars.push_back(Scrollbar);
                    break;
                }
            }
            break;
        case primitives_mod:
            if(ifEmpty(SourceContext.Modules.Primitives.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Primitives[0]->getID() == entityID){
                NewContext.Modules.Primitives.push_back(SourceContext.Modules.Primitives[0]);
            }
            break;
        case primitives_mod_vec:
            for(PrimitivesModule * Primitive : SourceContext.Modules.Primitives){
                if(Primitive->getID() == entityID){
                    NewContext.Modules.Primitives.push_back(Primitive);
                    break;
                }
            }
            break;
        case vector_mod:
            if(ifEmpty(SourceContext.Modules.Vectors.empty(), SourceContext.type, CurrentInstr))
                return;
            if(SourceContext.Modules.Vectors[0]->getID() == entityID){
                NewContext.Modules.Vectors.push_back(SourceContext.Modules.Vectors[0]);
            }
            break;
        case vector_mod_vec:
            for(VectorModule * Vector : SourceContext.Modules.Vectors){
                if(Vector->getID() == entityID){
                    NewContext.Modules.Vectors.push_back(Vector);
                    break;
                }
            }
            break;
        default:
            break;
    }
    
    if(NewContext.getVectorSize() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "There is no entity '" << entityID << "' of the '"
            << dataTypeToStr(SourceContext.type) << "' type. Cannot create a variable with id '"
            << Operation.Output.variableID << "'.\n";
        return;
    }
    assignVariable(ObjectMemory, Operation.Output);
}
vector<string> getAllFilesNamesWithinFolder(string directory, string workingDirectory, int depth, char mode, const InstrDescription & CurrentInstr){
    vector<string> names;
    if(!std::filesystem::exists(directory)){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Directory '" << directory
            << "' does not exist.\n";
        return names;
    }
    if(mode == 'f'){
        for(const auto & entry : std::filesystem::directory_iterator(directory)){
            names.emplace_back(entry.path().string());
        }
    }
    else if(mode == 'r'){
        for (auto entry = std::filesystem::recursive_directory_iterator(directory);
            entry != std::filesystem::recursive_directory_iterator();
            ++entry)
        {
            if(depth <= -1 || entry.depth() < depth){
                string temp = entry->path().string();
                temp = temp.substr(workingDirectory.size(), temp.size()-workingDirectory.size());
                names.emplace_back(temp);
            }
        }
    }
    else{
        for(const auto & entry : std::filesystem::directory_iterator(directory)){
            names.emplace_back(entry.path().filename().string());
        }
    }
    return names;
}
void ProcessClass::listOutFiles(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    string directory = "";
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, directory, false);

    if(directory.size() >= 256)[[unlikely]]{
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Path to a directory exceeds the length limit of 255 characters.\n";
        //TODO: Check the length of each directory name in the path, because there's a file name limit of 255.
        return;
    }

    if(directory == "~/" || directory[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + directory << "\'.\n";
        return;
    }

    bool recursiveSearch = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, recursiveSearch, false);

    int depth = -1;
    getIntFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, depth, false);

    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " " << directory
            << " " << Operation.Output.variableID << " " << recursiveSearch;
        if(depth > -1){
            cout << " " << depth;
        }
        cout << "\n";
    }

    vector<string> fileNames;
    #if _WIN32
        if(recursiveSearch){
            fileNames = getAllFilesNamesWithinFolder(EXE_PATH + workingDirectory + directory, EXE_PATH + workingDirectory, depth, 'r', CurrentInstr);
        }
        else{
            fileNames = getAllFilesNamesWithinFolder(EXE_PATH + workingDirectory + directory, EXE_PATH + workingDirectory, depth, 'n', CurrentInstr);
        }
    #elif __linux__
        if(recursiveSearch){
            fileNames = getAllFilesNamesWithinFolder(EXE_PATH + workingDirectory + directory, EXE_PATH + workingDirectory, depth, 'r', CurrentInstr);
        }
        else{
            fileNames = getAllFilesNamesWithinFolder(EXE_PATH + workingDirectory + directory, EXE_PATH + workingDirectory, depth, 'n', CurrentInstr);
        }
    #endif

    vector<string> directories, normalFiles;
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
    if(Operation.Output.type == null_dt){
        cout << buffer;
        return;
    }

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newString(buffer));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::changeWorkingDirectory(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    string newDirectory = "";
    getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, newDirectory, false);

    if(newDirectory == "~/" || newDirectory[0] == ' '){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Access denied to the path: \'" << EXE_PATH + workingDirectory + newDirectory << "\'.\n";
        return;
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
        string testPath = workingDirectory + newDirectory;
        if(newDirectory.back() != '/'){
            testPath += '/';
        }
        if(!std::filesystem::exists(EXE_PATH + testPath)){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Directory '" << EXE_PATH + testPath << "' does not exist.\n";
            return;
        }
        workingDirectory = testPath;
    }
    
}
void ProcessClass::printWorkingDirectory(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.Output.type == null_dt){
        cout << workingDirectory;
        return;
    }
    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newString(workingDirectory));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::findSimilarStrings(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 2 parameters.\n";
        return;
    }

    string pattern = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pattern, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1.\n";
        return;
    }

    vector<string> stringVector;
    if(getStringVectorFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, stringVector, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2.\n";
        return;
    }

    bool returnTheLongestCommonPart = false;
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 2, returnTheLongestCommonPart, false);

    if(printOutInstructions){
        cout << Operation.instruction << " " << shortenText(pattern, maxLengthOfValuesPrinting) << " ";
        printStringVectorForInstruction(stringVector, maxLengthOfValuesPrinting);
        cout << " " << returnTheLongestCommonPart << " " << Operation.Output.variableID;
    }
    
    NewContext.clear();
    NewContext.type = value_vec;
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
            NewContext.Values.emplace_back(VariableModule::newString(text));
        }
    }
    if(returnTheLongestCommonPart){
        if(pattern == "" || NewContext.Values.size() == 0){
            NewContext.Values.clear();
            NewContext.Values.emplace_back(VariableModule::newString(""));
            assignVariable(ObjectMemory, Operation.Output);
            return;
        }
        string commonPart = NewContext.Values[0].getString();
        for(const VariableModule & SimilarText : NewContext.Values){
            string text = SimilarText.getString();
            if(commonPart.size() > text.size()){
                commonPart = commonPart.substr(0, text.size());
            }
            if(text.substr(0, commonPart.size()) == commonPart){
                continue;
            }
            for(unsigned i = pattern.size(); i < commonPart.size(); ++i){
                if(commonPart[i] != text[i]){
                    commonPart = commonPart.substr(0, i);
                    break;
                }
            }
        }
        NewContext.Values.clear();
        NewContext.Values.emplace_back(VariableModule::newString(commonPart));
    }
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::countPatternOccurrences(OperationClass &Operation, ObjectMemoryStruct &ObjectMemory){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 2 parameters.\n";
        return;
    }

    string pattern = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, pattern, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1 (pattern).\n";
        return;
    }

    string text = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 1, text, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 2 (text).\n";
        return;
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << shortenText(pattern, maxLengthOfValuesPrinting)
            << " " << shortenText(text, maxLengthOfValuesPrinting) << " " <<  Operation.Output.variableID;
    }
    
    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newInt(0));
    if(pattern == ""){
        assignVariable(ObjectMemory, Operation.Output);
        return;
    }
    unsigned j = 0;
    for(unsigned i = 0; i < text.size(); ++i){
        if(text[i] != pattern[0]){
            continue;
        }
        if(text.size() - pattern.size() < 0){
            break;
        }
        for(j = 1; j < pattern.size(); ++j){
            if(text[i + j] != pattern[j]){
                break;
            }
        }
        if(j == pattern.size() && text[i + j - 1] == pattern[j - 1]){
            NewContext.Values.back().addInt(1);
        }
    }
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::getConsoleInput(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory,
    int & terminationTimer, const int & timeoutTerminationTime, ALLEGRO_EVENT_QUEUE * eventQueue
){
    string consoleInput = "";
    terminationTimer = timeoutTerminationTime;
    std::getline(std::cin, consoleInput);
    if(consoleInput == "start"){
        consoleInput = "";
        string temp = "";
        while(std::getline(std::cin, temp) && temp != "end"){
            consoleInput += temp + "\n";
        }
    }
    al_flush_event_queue(eventQueue);
    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newString(consoleInput));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::createDisplay(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory, EngineClass & Engine){
    if(getIntFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 0, Engine.displaySize.x, true)
    ){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to get an unsigned int.\n";
        return;
    }
    if(getIntFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 1, Engine.displaySize.y, true)
    ){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to get an unsigned int.\n";
        return;
    }
    if(getIntFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 2, Engine.backbufferSize.x, true)
    ){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to get an unsigned int.\n";
        return;
    }
    if(getIntFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 3, Engine.backbufferSize.y, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Failed to get an unsigned int.\n";
        return;
    }
    getBoolFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr,
        Operation.Parameters, 4, Engine.autoScaleBackbuffer, false
    );
    
    Engine.createDisplay();
}
bool compare(std::pair<EngineInstr, int64_t>& a, std::pair<EngineInstr, int64_t>& b) { 
    return a.second > b.second; 
}
void ProcessClass::printProfiler(){
    cout << "Profiler: \n";
    std::vector<std::pair<EngineInstr, int64_t>> InstructionsTimings/*(TimeSpentOnInstructions.begin(), TimeSpentOnInstructions.end())*/;
    std::sort(InstructionsTimings.begin(), InstructionsTimings.end(), compare);
    
    int64_t entireTime = 0;
    for(auto instruction : InstructionsTimings){
        entireTime += instruction.second;
        cout << instrToStr(instruction.first) << " : ";
        if(instrToStr(instruction.first).size() < 5){
            cout << "\t\t\t";
        }
        else if(instrToStr(instruction.first).size() <= 11){
            cout << "\t\t";
        }
        else{
            cout << "\t";
        }
        cout << instruction.second / 1000000.0f << "s\n";
    }

    cout << "\nTime sum: " << entireTime / 1000000.0f << "s\n";

    /*cout << "\nindex_vec: " << INDEX_TESTS[0] / 1000000.0f << " => " << INDEX_TESTS[1] / 1000000.0f
        << " + " << INDEX_TESTS[2] / 1000000.0f << " + " << INDEX_TESTS[3] / 1000000.0f << " + " << INDEX_TESTS[4] / 1000000.0f
        << " = " << (INDEX_TESTS[1]+INDEX_TESTS[2]+INDEX_TESTS[3]+INDEX_TESTS[4]) / 1000000.0f << "\n";

    cout << "\nif: " << IF_TESTS[0] / 1000000.0f << " => " << IF_TESTS[1] / 1000000.0f
        << " + " << IF_TESTS[2] / 1000000.0f << " + " << IF_TESTS[3] / 1000000.0f << " + " << IF_TESTS[4] / 1000000.0f
        << " = " << (IF_TESTS[1]+IF_TESTS[2]+IF_TESTS[3]+IF_TESTS[4]) / 1000000.0f << "\n";*/
}
void ProcessClass::startTimer(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 1 parameter.\n";
        return;
    }

    string userDefinedTimerName = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, userDefinedTimerName, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1 (name).\n";
        return;
    }
    
    userDefinedTimers[userDefinedTimerName] = std::chrono::steady_clock::now();
}
void ProcessClass::stopTimer(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires at least 1 parameter.\n";
        return;
    }

    string userDefinedTimerName = "";
    if(getStringFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, CurrentInstr, Operation.Parameters, 0, userDefinedTimerName, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get a string value from the parameter 1 (name).\n";
        return;
    }

    if(!userDefinedTimers.contains(userDefinedTimerName)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "There is no user defined timer with the name '" << userDefinedTimerName << "'.\n";
        return;
    }

    TimePoint currentTime = std::chrono::steady_clock::now();
    auto timeDifference = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - userDefinedTimers[userDefinedTimerName]).count();

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(VariableModule::newDouble(timeDifference / 1000000.0f));
    assignVariable(ObjectMemory, Operation.Output);
}
bool ProcessClass::assertValues(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 2){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Instruction requires 2 parameters.\n";
        return true;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
    }

    ContextClass LeftVariable;
    bool negateAfterCopy = false; //ignore
    LeftVariable.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 0, false
    );

    if(printOutInstructions){
        cout << Operation.Parameters[0].getVariableIdOrValue() << " ";
    }

    ContextClass RightVariable;
    RightVariable.copyFromTheParameter(ObjectMemory.MemoryMap, LocalToGlobalTranslation,
        CurrentInstr, Operation.Parameters, negateAfterCopy, 1, false
    );

    if(printOutInstructions){
        cout << Operation.Parameters[1].getVariableIdOrValue() << "\n";
    }
    
    if(LeftVariable.ID == "NULL" || RightVariable.ID == "NULL"){
        if((LeftVariable.type == null_dt && RightVariable.ID == "NULL")
            || (LeftVariable.ID == "NULL" && RightVariable.type == null_dt)
        ){
            return true;
        }
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: ";
        if(LeftVariable.ID == "NULL"){
            cerr << "NULL != ";
            if(RightVariable.getValue(RightOperandProc) != ReturnType::INVALID_TYPE){
                cerr << RightOperandProc.getAnyValue();
            }
            else{
                cerr << RightVariable.ID;
            }
        }
        else{
            if(LeftVariable.getValue(LeftOperandProc) != ReturnType::INVALID_TYPE){
                cerr << LeftOperandProc.getAnyValue();
            }
            else{
                cerr << LeftVariable.ID;
            }
            cerr << " != NULL";
        }
        cerr << "\n";
        return false;
    }
    else if(LeftVariable.type == null_dt && RightVariable.type == null_dt){
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: UNDEFINED != UNDEFINED.\n";
        return false;
    }
    else if(LeftVariable.type == null_dt){
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: UNDEFINED != ";
        if(RightVariable.getValue(RightOperandProc) != ReturnType::INVALID_TYPE){
            cerr << RightOperandProc.getAnyValue();
        }
        else{
            cerr << RightVariable.ID;
        }
        cerr << "\n";
        return false;
    }
    else if(RightVariable.type == null_dt){
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: ";
        if(LeftVariable.getValue(LeftOperandProc) != ReturnType::INVALID_TYPE){
            cerr << LeftOperandProc.getAnyValue();
        }
        else{
            cerr << LeftVariable.ID;
        }
        cerr << " != UNDEFINED\n";
        return false;
    }
    else if(LeftVariable.getVectorSize() == 0 && RightVariable.getVectorSize() == 0
        && LeftVariable.type == RightVariable.type
    ){
        return true;
    }
    else if(LeftVariable.getVectorSize() == 0 && RightVariable.getVectorSize() == 0){
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: Both operands are empty and of different types.\n";
        return false;
    }
    else if(LeftVariable.getVectorSize() == 0 || RightVariable.getVectorSize() == 0){
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: ["
            << dataTypeToStr(LeftVariable.type) << "]<" << LeftVariable.getVectorSize() << "> != ["
            << dataTypeToStr(RightVariable.type) << "]<" << RightVariable.getVectorSize() << ">\n";
        return false;
    }
    else if(LeftVariable.getValue(LeftOperandProc) != ReturnType::INVALID_TYPE
        && RightVariable.getValue(RightOperandProc) != ReturnType::INVALID_TYPE
    ){
        if(cmpVars(&LeftOperandProc, equal, &RightOperandProc, CurrentInstr)){
            return true;
        }
        cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
            << NEW_LINE_PADDING << "Assertion failed: " << LeftOperandProc.getAnyValue()
            << " != " << RightOperandProc.getAnyValue() << ".\n";
        return false;
    }
    cerr << "Error: In " + CurrentInstr.scriptName + ":" + uIntToStr(CurrentInstr.lineNumber) + ":\n"
        << NEW_LINE_PADDING << "Assertion failed: " << dataTypeToStr(LeftVariable.type) << ":"
        << LeftVariable.getValue(CurrentInstr, maxLengthOfValuesPrinting)
        << " != " << dataTypeToStr(RightVariable.type) << ":"
        << RightVariable.getValue(CurrentInstr, maxLengthOfValuesPrinting) << ".\n";
    return false;
}
void ProcessClass::getContextType(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1 || Operation.Output.type == null_dt){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 2 parameters.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
    }

    ContextClass * Variable = nullptr;
    if(getContextPointerFromTheParameter(Variable, ObjectMemory, LocalToGlobalTranslation, CurrentInstr, Operation.Parameters, 0, true)){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get context from the parameter 0.\n";
        return;
    }

    if(printOutInstructions){
        Operation.Location.print(Variable->ID);
        cout << "\n";
    }

    NewContext.clear();
    NewContext.type = value_inst;
    NewContext.Values.emplace_back(dataTypeToStr(Variable->type));
    assignVariable(ObjectMemory, Operation.Output);
}
void ProcessClass::loadVariableFromMemoryAddress(OperationClass & Operation, ObjectMemoryStruct & ObjectMemory){
    if(Operation.rootParametersSize < 1 || Operation.Output.type == null_dt){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Instruction requires 2 parameters.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << instrToStr(Operation.instruction) << " ";
    }

    unsigned memoryAddress = 0;
    if(getUnsignedFromTheParameter(ObjectMemory, LocalToGlobalTranslation, HelpContext, LeftOperandProc,
        CurrentInstr, Operation.Parameters, 0, memoryAddress, true
    )){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Failed to get an unsigned value from the 'address' parameter.\n";
        return;
    }

    if(printOutInstructions){
        cout << memoryAddress << "\n";
    }

    if(memoryAddress > ObjectMemory.MemoryMap.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Address " << memoryAddress <<" is not allocated.\n";
        return;
    }
    if(LocalToGlobalTranslation[Operation.Output.localAddress].dynamicAddress == memoryAddress){
        return;
    }
    NewContext = ObjectMemory.MemoryMap[memoryAddress];

    assignVariable(ObjectMemory, Operation.Output);
}
inline void setProgramCounter(unsigned & programCounter, bool & decrementProgramCounter, const unsigned & jumpToLine){
    programCounter = jumpToLine;
    if(programCounter > 0){
        --programCounter; 
    }
    else{
        decrementProgramCounter = true;
    }
}
string getFieldWithPadding(const string &text, const size_t & maxTabCount){
    const size_t & TAB_SIZE = 8; 
    size_t textLength = text.size();
    size_t usedTabs = (textLength / TAB_SIZE) + 1;
    string customPadding = "\t";

    if(maxTabCount - usedTabs + 1 < 0){
        cerr << "Error: Unsigned only condition is negative.\n";
        return "";
    }

    for (size_t i = 1; i < maxTabCount - usedTabs + 1; ++i){
        customPadding += "\t";
    }

    return text + customPadding;
}
inline void ProcessClass::dumpMemory(MemoryMapType & MemoryMap){
    const size_t & TAB_SIZE = 8; 
    auto normalize = [](const size_t & valueSize, const size_t & TAB_SIZE){
        return (valueSize / TAB_SIZE) + 1;
    };
    size_t maxFieldPadding[4] = {1, 1, 1, 1};
    for(unsigned address = 0; address < MemoryMap.size(); ++address){
        const ContextClass & Variable = MemoryMap[address];
        maxFieldPadding[0] = std::max(maxFieldPadding[0], normalize(std::to_string(address).size(), TAB_SIZE));
        maxFieldPadding[1] = std::max(maxFieldPadding[1], normalize(Variable.ID.size(), TAB_SIZE));
        maxFieldPadding[2] = std::max(maxFieldPadding[2], normalize(dataTypeToStr(Variable.type).size(), TAB_SIZE));
        maxFieldPadding[3] = std::max(maxFieldPadding[3], normalize(Variable.getValue(CurrentInstr, maxLengthOfValuesPrinting).size(), TAB_SIZE));
    }
    string buffor = "\n";
    buffor += getFieldWithPadding("ADDR", maxFieldPadding[0])
        + getFieldWithPadding("NAME", maxFieldPadding[1])
        + getFieldWithPadding("TYPE", maxFieldPadding[2])
        + getFieldWithPadding("VALUE", maxFieldPadding[3]) + "\n";
    for(unsigned address = 0; address < MemoryMap.size(); ++address){
        const ContextClass & Variable = MemoryMap[address];
        buffor += getFieldWithPadding(std::to_string(address), maxFieldPadding[0])
            + getFieldWithPadding(Variable.ID, maxFieldPadding[1])
            + getFieldWithPadding(dataTypeToStr(Variable.type), maxFieldPadding[2])
            + getFieldWithPadding(Variable.getValue(CurrentInstr, maxLengthOfValuesPrinting), maxFieldPadding[3]) + "\n";
    }
    buffor += "\n";
    printInColor(buffor, 11);
}
inline void ProcessClass::dumpLocalMemory(MemoryMapType & MemoryMap){
    if(printOutInstructions){
        cout << "dump_local_memory\n";
    }
    const size_t & TAB_SIZE = 8; 
    auto normalize = [](const size_t & valueSize, const size_t & TAB_SIZE){
        return (valueSize / TAB_SIZE) + 1;
    };
    const size_t labelNumber = 6;
    size_t maxFieldPadding[labelNumber] = {1, 1, 1, 1, 1, 1};
    string labels[labelNumber] = {"LOC_ADDR", "REAL_ADDR", "IDX", "NAME", "TYPE", "VALUE"};
    for(size_t labelIdx = 0; labelIdx < labelNumber; ++labelIdx){
        maxFieldPadding[labelIdx] = std::max(maxFieldPadding[labelIdx],
            normalize(labels[labelIdx].size(), TAB_SIZE)
        );
    }
    for(unsigned localAddress = 0; localAddress < LocalToGlobalTranslation.size(); ++localAddress){
        unsigned globalAddress =  LocalToGlobalTranslation[localAddress].dynamicAddress;
        const ContextClass & Variable = MemoryMap[globalAddress];
        maxFieldPadding[0] = std::max(maxFieldPadding[0], normalize(std::to_string(localAddress).size(), TAB_SIZE));
        maxFieldPadding[1] = std::max(maxFieldPadding[1], normalize(std::to_string(globalAddress).size(), TAB_SIZE));
        maxFieldPadding[2] = std::max(maxFieldPadding[2], normalize(Variable.getUniqueIndexes().size(), TAB_SIZE));
        maxFieldPadding[3] = std::max(maxFieldPadding[3], normalize(Variable.ID.size(), TAB_SIZE));
        maxFieldPadding[4] = std::max(maxFieldPadding[4], normalize(dataTypeToStr(Variable.type).size(), TAB_SIZE));
        maxFieldPadding[5] = std::max(maxFieldPadding[5], normalize(Variable.getValue(CurrentInstr, maxLengthOfValuesPrinting).size(), TAB_SIZE));
    }
    string buffor = "\n";
    for(size_t labelIdx = 0; labelIdx < labelNumber; ++labelIdx){
        buffor += getFieldWithPadding(labels[labelIdx], maxFieldPadding[labelIdx]);
    }
    buffor += "\n";
    for(unsigned localAddress = 0; localAddress < LocalToGlobalTranslation.size(); ++localAddress){
        unsigned globalAddress =  LocalToGlobalTranslation[localAddress].dynamicAddress;
        const ContextClass & Variable = MemoryMap[globalAddress];
        buffor += getFieldWithPadding(std::to_string(localAddress), maxFieldPadding[0])
            + getFieldWithPadding(std::to_string(globalAddress), maxFieldPadding[1])
            + getFieldWithPadding(Variable.getUniqueIndexes(), maxFieldPadding[2])
            + getFieldWithPadding(Variable.ID, maxFieldPadding[3])
            + getFieldWithPadding(dataTypeToStr(Variable.type), maxFieldPadding[4])
            + getFieldWithPadding(Variable.getValue(CurrentInstr, maxLengthOfValuesPrinting), maxFieldPadding[5]) + "\n";
    }
    buffor += "\n";
    printInColor(buffor, 11);
}
EngineInstr ProcessClass::executeInstructions(LayerClass *& OwnerLayer,
    AncestorObject *& Owner, ObjectMemoryStruct & ObjectMemory, vector<AncestorObject *> & TriggeredObjects,
    vector<ProcessClass> & Processes, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack, EngineClass & Engine,
    unsigned & runChildEventWithIndex
){
    for(; EventCallState.programCounter < eventIt->Operations.size(); ++EventCallState.programCounter){
        EventCallState.programCounter -= EventCallState.decrementProgramCounter;
        EventCallState.decrementProgramCounter = false;
        
        OperationClass & Operation = eventIt->Operations[EventCallState.programCounter];
        CurrentInstr.instruction = Operation.instruction;
        CurrentInstr.lineNumber = Operation.lineNumber;
        //TODO: Map script names to a vector and access it with an index
        CurrentInstr.scriptName = Operation.scriptName;

        switch(Operation.instruction){
            case return_i:
            case reboot:
            case exit_i:
                if(printOutInstructions){
                    cout << instrToStr(Operation.instruction) << "\n";
                }
                return Operation.instruction;
            case run:
                ++EventCallState.programCounter;
                runChildEventWithIndex = Operation.specialValue;
                return Operation.instruction;
            case if_i:
                EventCallState.goToEndOfIfStatement.push_back(0);
                EventCallState.conditionalStatus = evaluateConditionalChain(Operation.ConditionalChain,
                    Operation.resultStack, Owner, OwnerLayer, Engine, ObjectMemory
                );
                if(printOutInstructions){
                    cout << "if " << EventCallState.conditionalStatus << "\n";
                }
                if(EventCallState.conditionalStatus == 't'){
                    EventCallState.goToEndOfIfStatement.back() = 1;
                }
                else if(EventCallState.conditionalStatus == 'f'){
                    //Jump to next else_if, else or end_if.
                    setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.specialValue);
                }
                else{
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Conditional status is equal to '" << EventCallState.conditionalStatus << "'.\n";
                }
                break;
            case else_if:
                if(EventCallState.goToEndOfIfStatement.back() == 1){
                    //Jump to next end_if.
                    setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.jumpToLine);
                }
                EventCallState.conditionalStatus = evaluateConditionalChain(Operation.ConditionalChain,
                    Operation.resultStack, Owner, OwnerLayer, Engine, ObjectMemory
                );
                if(EventCallState.conditionalStatus == 't'){
                    EventCallState.goToEndOfIfStatement.back() = 1;
                }
                else if(EventCallState.conditionalStatus == 'f'){
                    //Jump to next else_if, else or end_if.
                    setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.specialValue);
                }
                else{
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Conditional status is equal to '" << EventCallState.conditionalStatus << "'.\n";
                }
                break;
            case else_i:
                if(EventCallState.goToEndOfIfStatement.back() == 1){
                    //Jump to next end_if.
                    setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.jumpToLine);
                }
                break;
            case end_if:
                EventCallState.goToEndOfIfStatement.pop_back();
                break;
            case while_i:
                EventCallState.conditionalStatus = evaluateConditionalChain(Operation.ConditionalChain, Operation.resultStack, Owner, OwnerLayer, Engine, ObjectMemory);
                if(EventCallState.conditionalStatus == 't'){
                    EventCallState.breakFromCurrentLoop = false;
                }
                else if(EventCallState.conditionalStatus == 'f'){
                    EventCallState.breakFromCurrentLoop = true;
                    //Jump to "end_while" label.
                    setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.jumpToLine);
                }
                else{
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Conditional status is equal to '" << EventCallState.conditionalStatus << "'.\n";
                }
                break;
            case end_while:
                if(EventCallState.breakFromCurrentLoop){
                    EventCallState.breakFromCurrentLoop = false;
                }
                else{
                    //Jump to "while_i" instruction.
                    setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.jumpToLine);
                }
                break;
            case break_i:
                EventCallState.breakFromCurrentLoop = true;
                //Jump to "end_while" label.
                setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.jumpToLine);
                break;
            case continue_i:
                //Jump to "while_i" instruction.
                setProgramCounter(EventCallState.programCounter, EventCallState.decrementProgramCounter, Operation.jumpToLine);
                break;
            case first: //Aggregate entities and push them on the Variables Stack.
            case last: //Aggregate entities and push them on the Variables Stack.
            case all: //Aggregate entities and push them on the Variables Stack.
            case random_i: //Aggregate entities and push them on the Variables Stack.
                aggregateEntities(Operation, ObjectMemory, Engine);
                break;
            case index_i:
                getReferenceByIndex(Operation, ObjectMemory);
                break;
            case index_vec:
                getInstanceFromVector(Operation, ObjectMemory);
                break;
            case sum: //Execute operations on sets.
            case intersection: //Execute operations on sets.
            case diff: //Execute operations on sets.
                aggregateTwoSets(Operation, ObjectMemory);
                break;
            case access_i: //Get only values from the environment.
                aggregateValues(Operation, ObjectMemory, OwnerLayer, Owner, Engine, &Processes);
                break;
            case bool_i: 
            case int_i: 
            case double_i: 
            case string_i:
            case bool_vec_i: 
            case int_vec_i: 
            case double_vec_i: 
            case string_vec_i:
                createLiteral(Operation, ObjectMemory); //Get literals prepared in the event.
                break;
            case rand_int: //Generate random int value between
                generateRandomValue(Operation, ObjectMemory);
                break;
            case rand_double: //Generate random int value between
                generateRandomValue(Operation, ObjectMemory, true);
                break;
            case find_by_id: //Aggregate context only by id.
                aggregateOnlyById(ObjectMemory, Operation, OwnerLayer, Owner);
                break;
            case clone_i:
                cloneEntities(Operation, ObjectMemory, Layers, Owner, TriggeredObjects, startingEventIt, eventIt, MemoryStack);
                break;
            case add:
            case sub:
            case mul:
            case div_i:
            case mod:
            case pow_i:
                executeArithmetics(Operation, ObjectMemory);
                break;
            case assign:
                cerr << "Assign instruction is not implemented yet. It's currently used only inside other instructions.\n";
                break;
            case next:
                incrementInteger(Operation, ObjectMemory);
                break;
            case inc:
            case dec:
            case move:
            case add_move:
            case sub_move:
            case mul_move:
            case div_move:
                moveValues(Operation, ObjectMemory);
                break;
            case in:
                checkIfVectorContainsVector(Operation, ObjectMemory);
                break;
            case new_i:
                createNewEntities(Operation, ObjectMemory, OwnerLayer, Owner,
                    TriggeredObjects, startingEventIt, eventIt, MemoryStack, Engine.focusedProcessID
                );
                break;
            case del:
                markEntitiesForDeletion(Operation, ObjectMemory, OwnerLayer, Owner, TriggeredObjects, Engine.focusedProcessID);
                if(OwnerLayer == nullptr || Owner == nullptr || eventIt->getIsDeleted()){
                    return null;
                }
                break;
            case delete_this_event:
                if(printOutInstructions){
                    cout << "delete_this_event\n";
                }
                eventIt->deleteLater();
                wasDeleteExecuted = true;
                return Operation.instruction;
            case bind_i:
                bindFilesToObjects(Operation, ObjectMemory);
                break;
            case rbind_i:
                removeBindedFilesFromObjects(Operation, ObjectMemory);
                break;
            case build:
                if(buildEventsInObjects(Operation, ObjectMemory, Owner, startingEventIt, eventIt, MemoryStack, Engine.allowNotAscii)){
                    return return_i;
                }
                break;
            case load_build:
            case build_subset:
            case inject_code:
            case inject_instr:
                if(customBuildEventsInObjects(Operation, ObjectMemory, Owner, startingEventIt, eventIt, MemoryStack, Operation.instruction, Engine.allowNotAscii)){
                    return return_i;
                }
                break;
            case demolish:
                clearEventsInObjects(Operation, ObjectMemory, Owner);
                break;
            case fun:
                executeFunction(Operation, ObjectMemory, eventIt, Engine);
                break;
            case env:
                changeEngineVariables(Operation, ObjectMemory, Engine);
                break;
            case edit_proc:
                changeProcessVariables(Operation, ObjectMemory, Engine.processIDs);
                break;
            case load_bitmap:
                loadBitmap(Operation, ObjectMemory, Engine.BitmapContainer);
                break;
            case mkdir_i:
                createDirectory(Operation, ObjectMemory);
                break;
            case rm:
                removeFileOrDirectory(Operation, ObjectMemory);
                break;
            case rmll:
                removeRecursivelyFileOrDirectory(Operation, ObjectMemory);
                break;
            case mv_i:
                renameFileOrDirectory(Operation, ObjectMemory);
                break;
            case print_v_i:
            case print_d_i:
            case print_i:
                executePrint(Operation, ObjectMemory);
                break;
            case load_text:
                loadFileAsString(Operation, ObjectMemory);
                break;
            case save_text:
                saveStringAsFile(Operation, ObjectMemory);
                break;
            case ls:
                listOutFiles(Operation, ObjectMemory);
                break;
            case lse:
                listOutEntities(Operation, ObjectMemory, Processes, Engine);
                break;
            case new_proc:
                createNewProcess(Operation, Processes, ObjectMemory, Owner, TriggeredObjects,
                    startingEventIt, eventIt, MemoryStack, Engine);
                break;
            case var:
                createNewOwnerVariable(Operation, ObjectMemory, Owner, startingEventIt, eventIt, MemoryStack);
                break;
            case vec:
               createNewOwnerVector(Operation, ObjectMemory, Owner, startingEventIt, eventIt, MemoryStack);
                break;
            case tokenize:
                tokenizeStringFromContext(Operation, ObjectMemory);
                break;
            case tree:
                printTree(Operation, ObjectMemory, Processes);
                break;
            case len:
                getStringSizeFromContext(Operation, ObjectMemory);
                break;
            case size_i:
                getSizeOfContext(Operation, ObjectMemory);
                break;
            case substr:
                getSubStringFromContext(Operation, ObjectMemory);
                break;
            case load_font:
                loadFontFromContext(Operation, ObjectMemory, Engine);
                break;
            case reset_keyboard: //TODO: Fix and add to the documentation
                Engine.pressedKeys.clear();
                Engine.releasedKeys.clear();
                Engine.firstPressedKeys.clear();
                break;
            case find_by_id_2:
                findByIDInObjectMemory(Operation, ObjectMemory);
                break;
            case dump_memory:
                dumpMemory(ObjectMemory.MemoryMap);
                break;
            case dump_local_memory:
                dumpLocalMemory(ObjectMemory.MemoryMap);
                break;
            case restart_drag:
                detectStartPosOfDraggingCamera(Engine.display, Engine.Mouse);
                break;
            case cd:
                changeWorkingDirectory(Operation, ObjectMemory);
                break;
            case pwd:
                printWorkingDirectory(Operation, ObjectMemory);
                break;
            case similar:
                findSimilarStrings(Operation, ObjectMemory);
                break;
            case count:
                countPatternOccurrences(Operation, ObjectMemory);
                break;
            case create_display:
                createDisplay(Operation, ObjectMemory, Engine);
                break;
            case console_input:
                getConsoleInput(Operation, ObjectMemory, Engine.terminationTimer,
                    Engine.timeoutTerminationTime, Engine.eventQueue
                );
                break;
            case start_timer:
                startTimer(Operation, ObjectMemory);
                break;
            case stop_timer:
                stopTimer(Operation, ObjectMemory);
                break;
            case breakpoint:
                cerr << "Warning: The 'breakpoint' instruction can be used only in the debugger.\n";
                raise(SIGINT);
                break;
            case assert:
                if(!assertValues(Operation, ObjectMemory)){
                    return Operation.instruction;
                }
                break;
            case type:
                getContextType(Operation, ObjectMemory);
                break;
            case load_i:
                loadVariableFromMemoryAddress(Operation, ObjectMemory);
                break;
            default:
                cerr << "Error: In " << OwnerLayer->getID() << "::" << Owner->getID() << "::" << eventIt->getID()
                    << "': In " << __FUNCTION__ << ": Instruction '" << instrToStr(Operation.instruction) << "' does not exist.\n";
                break;
        }
        
        if(printOutInstructions){
            if(printOutStackAutomatically && Operation.instruction != EngineInstr::dump_context_stack){
                string buffor = "\nStack: ";
                for(auto Context : ObjectMemory.MemoryMap){
                    buffor += Context.ID + ":" + dataTypeToStr(Context.type)
                        + ":" + Context.getValue(CurrentInstr, maxLengthOfValuesPrinting) + ", ";
                }
                buffor += "\n\n";
                printInColor(buffor, 11);
            }
            
            cout.flush();
        }
    }
    return null;
}
VariableModule ProcessClass::findNextValueInMovementModule(ConditionClass &Condition, AncestorObject *CurrentObject){
    if(CurrentObject == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    VariableModule NewValue;
    NewValue.setID(sourceToStr(Condition.Location.moduleType) + "_" 
        + attributeToStr(Condition.Location.attribute), nullptr
    );
    if(Condition.Location.attribute == is_moving){
        NewValue.setBool(false);
        for(const MovementModule & Movement : CurrentObject->MovementContainer){
            if(!Movement.getIsDeleted() && Movement.isMoving()){
                NewValue.setBool(true);
                break;
            }
        }
        return NewValue;
    }
    else if(Condition.Location.attribute == is_still){
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
        switch(Condition.Location.attribute){
            case allowed_jumps:
                NewValue.setInt(Movement.getAllowedJumps());
                return NewValue;
            case jumps_count:
                NewValue.setInt(Movement.getJumpsCount());
                return NewValue;
            case jump_cooldown:
                NewValue.setDouble(Movement.getJumpCooldown());
                return NewValue;
            case jump_cooldown_duration:
                NewValue.setDouble(Movement.getJumpCooldownDuration());
                return NewValue;
            case can_jump:
                NewValue.setBool(Movement.getCanJump());
                return NewValue;
            case body_mass:
                NewValue.setDouble(Movement.getBodyMass());
                return NewValue;
            case walking_speed:
                NewValue.setDouble(Movement.getWalkingSpeed());
                return NewValue;
            case running_speed:
                NewValue.setDouble(Movement.getRunningSpeed());
                return NewValue;
            case jump_speed:
                NewValue.setDouble(Movement.getJumpSpeed());
                return NewValue;
            case gravitation:
                NewValue.setDouble(Movement.getGravitation());
                return NewValue;
            case base_friction:
                NewValue.setDouble(Movement.getBaseFriction());
                return NewValue;
            case momentum_x:
                NewValue.setDouble(Movement.getMomentum().x);
                return NewValue;
            case momentum_y:
                NewValue.setDouble(Movement.getMomentum().y);
                return NewValue;
            case is_moving_up:
            case is_moving_right:
            case is_moving_down:
            case is_moving_left:
                NewValue.setBool(Movement.isMovingInThisDirection(Condition.Location.attribute));
                return NewValue;
            default:
                break;
        }
        break;
    }
    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '"
        << attributeToStr(Condition.Location.attribute) << "' is not valid.\n";
    NewValue.setBool(false);
    NewValue.setID("null", nullptr);
    return NewValue;
}
VariableModule ProcessClass::getValueFromObjectInCamera(AncestorObject * CurrentObject, const AttributeType & attribute, const string & cameraID){
    if(CurrentObject == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    bool newValue = false;
    if(attribute == visible){
        for(const Camera2D & Camera : Cameras){
            if(!Camera.getIsDeleted() && Camera.isObjectVisible(CurrentObject->getPos(), CurrentObject->getSize())){
                newValue = true;
                break;
            }
        }
    }
    else if(attribute == visible_specific){
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
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '" << attribute << "' is not valid.\n";
    }
    return VariableModule::newBool(newValue);
}
VariableModule ProcessClass::getValueFromMouseClickingObject(const MouseClass & Mouse, AncestorObject * CurrentObject,
    const AttributeType & attribute, const short & button
){
    if(CurrentObject == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    bool result = false;
    if(attribute == pressed){
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
    else if(attribute == pressing){
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
    else if(attribute == released){
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
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '" << attribute << "' is not valid.\n";
    }
    return VariableModule::newBool(result);
}
VariableModule ProcessClass::getValueFromObjectInCollision(ConditionClass &Condition, AncestorObject * CurrentObject, LayerClass * CurrentLayer){
    if(CurrentObject == nullptr || CurrentObject->getIsDeleted()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    switch(Condition.Location.attribute){
        case has_solid_hitbox:
            for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
                if(!Collision.getIsDeleted() && Collision.getIsSolid()){
                    return VariableModule::newBool(true);
                }
            }
            return VariableModule::newBool(false);
        case is_fully_solid:{
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
        case detected:
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
        case with_object:
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
        case with_hitbox:
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
        default:
            break;
    }
    if(CurrentLayer == nullptr || CurrentLayer->getIsDeleted()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Layer does not exist.\n";
        return VariableModule::newBool(false);
    }
    for(const CollisionModule & Collision : CurrentObject->CollisionContainer){
        if(Collision.getID() != Condition.Location.moduleID){
            continue;
        }
        if(Collision.getIsDeleted()){
            break;
        }
        switch(Condition.Location.attribute){
            case in_group:
                return VariableModule::newBool(Collision.isInAGroup(Condition.Literal.getStringUnsafe()));
            case hitbox_is_solid:
                return VariableModule::newBool(Collision.getIsSolid());
            case hitbox_can_penetrate:
                return VariableModule::newBool(Collision.getCanPenetrateSolids());
            case hitbox_ignores_object:
                return VariableModule::newBool(Collision.ignores("object", Condition.Literal.getStringUnsafe()));
            case hitbox_ignores_object_group:
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
            case hitbox_ignores_hitbox:
                return VariableModule::newBool(Collision.ignores("hitboxes", Condition.Literal.getStringUnsafe()));
            case hitbox_ignores_hitbox_group:
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
            case hitbox_detected:
                for(const DetectedCollision & Detected : Collision.Detected){
                    if(Detected.collisionType > 0){
                        return VariableModule::newBool(true);
                    }
                }
                return VariableModule::newBool(false);
            case hitbox_detected_hitbox:
                for(const DetectedCollision & Detected : Collision.Detected){
                    if(Detected.collisionType > 0
                        && Detected.solidID == Condition.Literal.getStringUnsafe()
                        && Detected.collisionID == Condition.Location.spareID
                    ){
                        return VariableModule::newBool(true);
                    }
                }
                return VariableModule::newBool(false);
            default:
                break;
        }
        break;
    }
    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '"
        << attributeToStr(Condition.Location.attribute) << "' is not valid.\n";
    return VariableModule::newBool(false, "null");
}
VariableModule ProcessClass::findNextValueAmongObjects(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, const MouseClass & Mouse){
    LayerClass * CurrentLayer = nullptr;
    AncestorObject * CurrentObject = nullptr;

    if(!findLayerAndObject(Condition.Location, Owner, OwnerLayer, CurrentLayer, CurrentObject)){
        return VariableModule::newBool(false, "null");
    }

    switch(Condition.Location.moduleType){
        case variable:
            for(const VariableModule & Variable : CurrentObject->VariablesContainer){
                if(Variable.getID() == Condition.Location.moduleID && Variable.getType() != 'n'){
                    if(Variable.getIsDeleted()){
                        break;
                    }
                    return Variable;
                }
            }
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable  '" << Condition.Location.moduleID << "' does not exist.\n";
            break;
        case ancestor:
        case null_s:
            return CurrentObject->getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
        case camera:
            return getValueFromObjectInCamera(CurrentObject, Condition.Location.attribute, Condition.Location.moduleID);
        case mouse:
            if(canUserInteract){
                return getValueFromMouseClickingObject(Mouse, CurrentObject, Condition.Location.attribute, Condition.Literal.getIntUnsafe());
            }
            else{
                return VariableModule::newBool(false, "null");
            }
            break;
        case movement:
            return findNextValueInMovementModule(Condition, CurrentObject);
        case super_text:
            for(const SuperTextModule & SuperText : CurrentObject->SuperTextContainer){
                if(SuperText.getID() == Condition.Location.moduleID){
                    if(SuperText.getIsDeleted()){
                        break;
                    }
                    return SuperText.getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe(), CurrentInstr);
                }
            }
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
            break;
        case super_editable_text:
            for(const SuperEditableTextModule & SuperEditableText : CurrentObject->SuperEditableTextContainer){
                if(SuperEditableText.getID() == Condition.Location.moduleID){
                    if(SuperEditableText.getIsDeleted()){
                        break;
                    }
                    return SuperEditableText.getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe(), CurrentInstr);
                }
            }
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
            break;
        case collision:
            return getValueFromObjectInCollision(Condition, CurrentObject, CurrentLayer);
        case scrollbar:
            for(const ScrollbarModule & Scrollbar : CurrentObject->ScrollbarContainer){
                if(Scrollbar.getID() == Condition.Location.moduleID){
                    if(Scrollbar.getIsDeleted()){
                        break;
                    }
                    return Scrollbar.getValue(Condition.Location.attribute, CurrentInstr);
                }
            }
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
            return VariableModule::newBool(false, "null");
        default:
            break;
    }
    
    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Value not found.\n";
    return VariableModule::newBool(false, "null");
}
inline bool getIntFromContext(const InstrDescription & CurrentInstr, string function, ObjectMemoryStruct & ObjectMemory,
    vector<DynamicVariableInfo> & EventLocalVariables, const unsigned & localAddress, const string & variableID, int & value
){
    ContextClass * Context = getVariableByAddress(
        CurrentInstr, ObjectMemory.MemoryMap, EventLocalVariables[localAddress], variableID, false
    );

    if(Context == nullptr){
        cerr << instructionError(CurrentInstr, function)
            << ": Variable with the name '" << variableID << "' does not exist.\n";
        return true;
    }
    if(Context->getIntOrAbort(value, CurrentInstr)){
        cerr << instructionError(CurrentInstr, function)
            << "Could not get an integer from the variable with name '" << variableID << "'.\n";
        return true;
    }
    return false;
}
void ProcessClass::getExistsOrIsDirectory(ObjectMemoryStruct & ObjectMemory, VariableModule & NewValue, const unsigned localVariableAddress,
    const string & variableId, const ValueSource & source
){
    ContextClass * Context = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        LocalToGlobalTranslation[localVariableAddress], variableId, true
    );

    if(Context == nullptr){
        NewValue.setBool(false);
    }
    else{
        string file = "";
        Context->getStringOrIgnore(file, EngineInstr::access_i);
        if(file == ""){
            NewValue.setBool(false);
        }
        else{
            file = EXE_PATH + workingDirectory + file;
            if(source == ValueSource::exists){
                NewValue.setBool(std::filesystem::exists(file));
            }
            else if(source == ValueSource::is_directory){
                NewValue.setBool(std::filesystem::is_directory(file));
            }
        }
    }
}
void ProcessClass::getScreenWidthOrHeigth(ObjectMemoryStruct & ObjectMemory, VariableModule & NewValue,
    const ConditionClass &Condition, const ValueSource & source
){
    int SCREEN_W = 0, SCREEN_H = 0;
    int tempInt = 0;

    if(Condition.Location.attribute == memory_address_a){
        if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation, Condition.localAddresses[0], "", tempInt)){
            NewValue.setBool(false);
            return;
        }
    }
    else if(Condition.Literal.getType() == 'i'){
        tempInt = Condition.Literal.vInt;
    }
    else{
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << ": Accessing value \'" << sourceToStr(source) << "\' requires a screen id of the integer type.\n";
        NewValue.setBool(false);
        return;
    }
    getDesktopResolution(tempInt, &SCREEN_W, &SCREEN_H);
    if(source == screen_w){
        NewValue.setInt(SCREEN_W);
    }
    else{
        NewValue.setInt(SCREEN_H);
    }
}
inline bool checkIfUserCanInteract(VariableModule & NewValue, const ValueSource & source, const AttributeType & attribute, const bool & canUserInteract){
    if(canUserInteract){
        return false;
    }
    NewValue.setID(source + "_" + attributeToStr(attribute) + "<user_cannot_interract>", nullptr);
    NewValue.setBool(0);
    return true;
}
bool ProcessClass::getProcess(VariableModule & NewValue, vector<ProcessClass> * Processes, ProcessClass *& Process, const string & processID){
    if(processID != "" && processID != ID && Processes != nullptr){
        Process = nullptr;
        for(ProcessClass & ProcessIter : *Processes){
            if(ProcessIter.ID == processID){
                Process = &ProcessIter;
                break;
            }
        }
        if(Process == nullptr){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << ": Process with id \'" << processID << "\' does not exist.\n";
            NewValue.setBool(false);
            return true;
        }
    }
    return false;
}
VariableModule ProcessClass::getValueFromVector(ConditionClass & Condition, ObjectMemoryStruct & ObjectMemory){
    VariableModule NewValue;

    ContextClass * Context = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        LocalToGlobalTranslation[Condition.localAddresses[0]], Condition.Location.moduleID, true
    );
    if(Context == nullptr){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "No context found.\n";
        NewValue.setBool(false);
        return NewValue;
    }
    if(Context->Modules.Vectors.size() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "There are no vectors in the context.\n";
        NewValue.setBool(false);
        return NewValue;
    }
    if(Context->Modules.Vectors.size() != 1){
        cout << instructionWarning(CurrentInstr, __FUNCTION__)
            << "There are several vectors in the context. Program will proceed with the last added vector.\n";
    }
    if(Condition.Literal.getType() == 'i'){
        return Context->Modules.Vectors.back()->getValue(value, Condition.Literal.getIntUnsafe());
    }
    else if(Condition.Literal.getType() == 's'){
        ContextClass * IndexContext = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
            LocalToGlobalTranslation[Condition.localAddresses[1]], Condition.Literal.getStringUnsafe(), true
        );
        unsigned vectorIdx = 0;
        if(IndexContext == nullptr || !IndexContext->getUnsignedOrAbort(vectorIdx, CurrentInstr)){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Cannot find a context for an index.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        return Context->Modules.Vectors.back()->getValue(value, vectorIdx);
    }
    else if(Condition.Location.attribute == size_a || Condition.Location.attribute == back_a){
        return Context->Modules.Vectors.back()->getValue(Condition.Location.attribute, 0);
    }
    cerr << instructionError(CurrentInstr, __FUNCTION__)
        << "Attribute '" << attributeToStr(Condition.Location.attribute)
        << "' is not valid for this value source.\n";
    NewValue.setBool(false);
    return NewValue;
}
void ProcessClass::getValueFromContext(ConditionClass & Condition, ObjectMemoryStruct & ObjectMemory,
    AncestorObject * Owner, LayerClass * OwnerLayer, const MouseClass & Mouse, VariableModule & NewValue,
    vector<VariableModule> * NewValueVec
){
    ContextClass * Context = nullptr;
    
    const string & variableID = Condition.Literal.vString;
    Context = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
        LocalToGlobalTranslation[Condition.localAddresses[0]], variableID, true
    );
    if(Context == nullptr){
        NewValue.setBool(false);
        return;
    }


    auto printTooManyInstancesWarning = [](const unsigned vecSize, const DataType type,
        const InstrDescription & CurrentInstr
    ){
        if(vecSize != 1){
            cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                << "There is more than one instance of '" << dataTypeToStr(type)
                << "' in the context. Only the first instance will be used.\n";
        }
    };
    auto abortIfEmpty = [](bool isEmpty, const DataType type,
        VariableModule & NewValue, const InstrDescription & CurrentInstr
    ){
        if(isEmpty){
            NewValue.setBool(false);
            return true;
        }
        return false;
    };
    auto printErrorIfEmpty = [](bool isEmpty, const DataType type,
        VariableModule & NewValue, const InstrDescription & CurrentInstr
    ){
        if(isEmpty){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "There are no instances of '" << dataTypeToStr(type)
                << "' type in the context.\n";
            NewValue.setBool(false);
            return true;
        }
        return false;
    };
    auto printIncorrectAttributeError = [](const AttributeType attribute, const DataType type,
        VariableModule & NewValue, const InstrDescription & CurrentInstr
    ){
        cerr << instructionError(CurrentInstr, __FUNCTION__) 
            << "Instances of '" << dataTypeToStr(type) << "' does not support the '"
            << attributeToStr(attribute) <<  "' attribute.\n";
        NewValue.setBool(false);
    };
    
    switch(Context->type){
        case value_vec:
            if(Condition.Location.attribute == size_a){
                NewValue.setInt(Context->Values.size());
                return;
            }
            if(abortIfEmpty(Context->Values.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            if(Condition.Location.attribute == back_a){
                NewValue.copyValue(Context->Values.back());
                return;
            }
            printTooManyInstancesWarning(Context->Values.size(), Context->type, CurrentInstr);
        case value_inst:
            if(printErrorIfEmpty(Context->Values.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            NewValue.copyValue(Context->Values[0]);
            return;
        case pointer_vec:
            if(Condition.Location.attribute == size_a){
                NewValue.setInt(Context->BasePointers.size());
                return;
            }
            if(abortIfEmpty(Context->BasePointers.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            if(Condition.Location.attribute == back_a){
                NewValue.setValueFromPointer(Context->BasePointers.back());
                return;
            }
            printTooManyInstancesWarning(Context->BasePointers.size(), Context->type, CurrentInstr);
        case pointer_inst:
            if(printErrorIfEmpty(Context->BasePointers.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            NewValue.setValueFromPointer(Context->BasePointers[0]);
            return;
        case variable_mod_vec:
            if(Condition.Location.attribute == size_a){
                NewValue.setInt(Context->Modules.Variables.size());
                return;
            }
            if(abortIfEmpty(Context->Modules.Variables.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            if(Condition.Location.attribute == back_a){
                NewValue.copyValue(Context->Modules.Variables.back());
                return;
            }
            printTooManyInstancesWarning(Context->Modules.Variables.size(), Context->type,
                CurrentInstr
            );
        case variable_mod:
            if(printErrorIfEmpty(Context->Modules.Variables.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            if(Condition.Location.attribute == size_a){
                NewValue.setInt(Context->Modules.Variables[0]->getString().size());
                return;
            }
            NewValue.copyValue(Context->Modules.Variables[0]);
            return;
        case collision_mod_vec:
            if(abortIfEmpty(Context->Modules.Collisions.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            printTooManyInstancesWarning(Context->Modules.Collisions.size(), Context->type,
                CurrentInstr
            );
        case collision_mod:
            if(printErrorIfEmpty(Context->Modules.Collisions.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            if(Condition.Location.attribute == detected){
                for(const DetectedCollision & Detected : Context->Modules.Collisions[0]->Detected){
                    if(Detected.collisionType == Condition.Literal.getIntUnsafe()){
                        NewValue.setBool(true);
                        return;
                    }
                }
                NewValue.setBool(false);
                return;
            }
            if(Condition.Location.attribute == release_on){
                NewValue.setBool(Context->Modules.Collisions[0]->getMouseCollision() == 'r');
                return;
            }
            printIncorrectAttributeError(Condition.Location.attribute, Context->type, NewValue,
                CurrentInstr
            );
            return;
        case primitives_mod_vec:
            if(abortIfEmpty(Context->Modules.Primitives.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            printTooManyInstancesWarning(Context->Modules.Primitives.size(), Context->type,
                CurrentInstr
            );
        case primitives_mod:
            if(printErrorIfEmpty(Context->Modules.Primitives.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            if(Condition.Location.attribute == pos_x){
                NewValue.setDouble(Context->Modules.Primitives[0]->getPos().x);
                return;
            }
            if(Condition.Location.attribute == pos_y){
                NewValue.setDouble(Context->Modules.Primitives[0]->getPos().y);
                return;
            }
            printIncorrectAttributeError(Condition.Location.attribute, Context->type, NewValue,
                CurrentInstr
            );
            return;
        case super_text_mod_vec:
            if(abortIfEmpty(Context->Modules.SuperTexts.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            printTooManyInstancesWarning(Context->Modules.SuperTexts.size(), Context->type,
                CurrentInstr
            );
        case super_text_mod:
            if(printErrorIfEmpty(Context->Modules.SuperTexts.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            if(Context->Modules.SuperTexts[0]->getComplexAttributeValue(
                Condition.Location.attribute, Condition.Location.spareID, CurrentInstr, NewValueVec
            )){
                return;
            }
            NewValue.copyValue(Context->Modules.SuperTexts[0]->getAttributeValue(
                Condition.Location.attribute, Condition.Location.spareID, CurrentInstr
            ));
            return;
        case super_editable_text_mod_vec:
            if(abortIfEmpty(Context->Modules.SuperEditableTexts.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            printTooManyInstancesWarning(Context->Modules.SuperEditableTexts.size(), Context->type,
                CurrentInstr
            );
        case super_editable_text_mod:
            if(printErrorIfEmpty(Context->Modules.SuperEditableTexts.empty(), Context->type,
                NewValue, CurrentInstr
            )){
                return;
            }
            if(Context->Modules.SuperEditableTexts[0]->getComplexAttributeValue(
                Condition.Location.attribute, Condition.Location.spareID, CurrentInstr, NewValueVec
            )){
                return;
            }
            NewValue.copyValue(Context->Modules.SuperEditableTexts[0]->getAttributeValue(
                Condition.Location.attribute, Condition.Location.spareID, CurrentInstr
            ));
            return;
        case vector_mod_vec:
            if(abortIfEmpty(Context->Modules.Vectors.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            printTooManyInstancesWarning(Context->Modules.Vectors.size(), Context->type,
                CurrentInstr
            );
        case vector_mod:
            if(printErrorIfEmpty(Context->Modules.Vectors.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            NewValue.copyValue(Context->Modules.Vectors.back()->getValue(
                Condition.Location.attribute, Context->Modules.Vectors.back()->getSize() - 1
            ));
            return;
        case object_vec:
            if(abortIfEmpty(Context->Objects.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            printTooManyInstancesWarning(Context->Objects.size(), Context->type, CurrentInstr);
        case object_inst:
            if(printErrorIfEmpty(Context->Objects.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            Condition.Location.layerID = Context->Objects[0]->getLayerID();
            Condition.Location.objectID = Context->Objects[0]->getID();
            NewValue.copyValue(findNextValueAmongObjects(Condition, Owner, OwnerLayer, Mouse));
            return;
        case layer_vec:
            if(abortIfEmpty(Context->Layers.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            printTooManyInstancesWarning(Context->Layers.size(), Context->type, CurrentInstr);
        case layer_inst:
            if(printErrorIfEmpty(Context->Layers.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            NewValue.copyValue(Context->Layers[0]->getValue(Condition.Location.attribute,
                Condition.Location.spareID
            ));
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return;
        case camera_vec:
            if(abortIfEmpty(Context->Cameras.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            printTooManyInstancesWarning(Context->Cameras.size(), Context->type, CurrentInstr);
        case camera_inst:
            if(printErrorIfEmpty(Context->Cameras.empty(), Context->type, NewValue, CurrentInstr)){
                return;
            }
            if(Condition.Location.attribute == is_selected){
                NewValue.setBool(SelectedCamera == Context->Cameras[0]);
            }
            else{
                NewValue.copyValue(Context->Cameras[0]->getValue(Condition.Location.attribute));
            }
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return;
        case scrollbar_mod_vec:
            if(abortIfEmpty(Context->Modules.Scrollbars.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            printTooManyInstancesWarning(Context->Modules.Scrollbars.size(), Context->type,
                CurrentInstr
            );
        case scrollbar_mod:
            if(printErrorIfEmpty(Context->Modules.Scrollbars.empty(), Context->type, NewValue,
                CurrentInstr
            )){
                return;
            }
            NewValue.copyValue(Context->Modules.Scrollbars[0]->getValue(
                Condition.Location.attribute, CurrentInstr
            ));
            return;
        default:
            break;
    }
    
    cerr << instructionError(CurrentInstr, __FUNCTION__)
        << "No value can be extracted from the context.\n";
    return;
}
void ProcessClass::findNextValue(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer,
    const EngineClass & Engine, vector<ProcessClass> * Processes, ObjectMemoryStruct & ObjectMemory,
    VariableModule & NewValue, vector<VariableModule> * NewValueVec
){
    ProcessClass * Process = this;
    NewValue.clear();
    //NewValue.setID(transSourceToStr(Condition.Location.source), nullptr);
    
    switch(Condition.Location.source){
        case fullscreen:{
            NewValue.setBool(Engine.fullscreen);
            return;}
        case second_passed:{
            NewValue.setBool(Engine.secondHasPassed());
            return;}
        case fps:{
            NewValue.setInt(Engine.fps.get());
            return;}
        case is_directory:{
            getExistsOrIsDirectory(ObjectMemory, NewValue, Condition.localAddresses[0], Condition.Literal.getString(), ValueSource::is_directory);
            return;}
        case exists:{
            getExistsOrIsDirectory(ObjectMemory, NewValue, Condition.localAddresses[0], Condition.Literal.getString(), ValueSource::exists);
            return;}
        case used_os:{
            #if __WIN32__
                NewValue.setString("windows");
            #elif __linux__
                NewValue.setString("linux");
            #elif __APPLE__
                NewValue.setString("orange");
            #else
                NewValue.setString("garlic_bread");
            #endif
            return;}
        case screen_w:{
            getScreenWidthOrHeigth(ObjectMemory, NewValue, Condition, ValueSource::screen_w);
            return;}
        case screen_h:{
            getScreenWidthOrHeigth(ObjectMemory, NewValue, Condition, ValueSource::screen_h);
            return;}
        case key_pressed:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            if(Condition.Literal.getType() == 's'){
                int tempInt = 0;
                if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation,
                    Condition.localAddresses[0], Condition.Literal.getString(), tempInt
                )){
                    NewValue.setBool(false);
                    return;
                }
                NewValue.setBool(isKeyFirstPressed(tempInt, Engine.firstPressedKeys));
            }
            else{
                NewValue.setBool(isKeyFirstPressed(Condition.Literal.getInt(), Engine.firstPressedKeys));
            }
            return;}
        case key_pressing:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            if(Condition.Literal.getType() == 's'){
                int tempInt = 0;
                if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation,
                    Condition.localAddresses[0], Condition.Literal.getString(), tempInt
                )){
                    NewValue.setBool(false);
                    return;
                }
                NewValue.setBool(isKeyPressed(tempInt, Engine.pressedKeys));
            }
            else{
                NewValue.setBool(isKeyPressed(Condition.Literal.getInt(), Engine.pressedKeys));
            }
            return;}
        case key_released:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            if(Condition.Literal.getType() == 's'){
                int tempInt = 0;
                if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation,
                    Condition.localAddresses[0], Condition.Literal.getString(), tempInt
                )){
                    NewValue.setBool(false);
                    return;
                }
                NewValue.setBool(isKeyReleased(tempInt, Engine.releasedKeys));
            }
            else{
                NewValue.setBool(isKeyReleased(Condition.Literal.getInt(), Engine.releasedKeys));
            }
            return;}
        case any_key_pressed:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            NewValue.setBool(Engine.firstPressedKeys.size() > 0);
            return;}
        case any_key_pressing:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            NewValue.setBool(Engine.pressedKeys.size() > 0);
            return;}
        case any_key_released:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            NewValue.setBool(Engine.releasedKeys.size() > 0);
            return;}
        case mouse_moved:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            NewValue.setBool(Engine.Mouse.didMouseMove);
            return;}
        case mouse_pressed:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            if(Condition.Literal.getType() == 's'){
                int tempInt = 0;
                if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation,
                    Condition.localAddresses[0], Condition.Literal.getString(), tempInt
                )){
                    NewValue.setBool(false);
                    return;
                }
                NewValue.setBool(Engine.Mouse.isFirstPressed(tempInt));
            }
            else{
                NewValue.setBool(Engine.Mouse.isFirstPressed(Condition.Literal.getInt()));
            }
            return;}
        case mouse_pressing:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            if(Condition.Literal.getType() == 's'){
                int tempInt = 0;
                if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation,
                    Condition.localAddresses[0], Condition.Literal.getString(), tempInt
                )){
                    NewValue.setBool(false);
                    return;
                }
                NewValue.setBool(Engine.Mouse.isPressed(tempInt));
            }
            else{
                NewValue.setBool(Engine.Mouse.isPressed(Condition.Literal.getInt()));
            }
            return;}
        case mouse_released:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            if(Condition.Literal.getType() == 's'){
                int tempInt = 0;
                if(getIntFromContext(CurrentInstr, __FUNCTION__, ObjectMemory, LocalToGlobalTranslation,
                    Condition.localAddresses[0], Condition.Literal.getString(), tempInt
                )){
                    NewValue.setBool(false);
                    return;
                }
                NewValue.setBool(Engine.Mouse.isReleased(tempInt));
            }
            else{
                NewValue.setBool(Engine.Mouse.isReleased(Condition.Literal.getInt()));
            }
            return;}
        case literal:
            NewValue.copyValue(Condition.Literal);
            return;
        case variable:{
            for(const VariableModule & Variable : Owner->VariablesContainer){
                if(Variable.getID() == Condition.Location.moduleID){
                    if(Variable.getIsDeleted()){
                        break;
                    }
                    NewValue.copyValue(Variable);
                    return;
                }
            }
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << Condition.Location.moduleID << "' does not exist.\n";
            return;}
        case camera:{
            if(getProcess(NewValue, Processes, Process, Condition.Location.process)){
                return;
            }
            //NewValue.setID(transSourceToStr(Condition.Location.source) + "_" + Condition.Location.attribute, nullptr);
            for(const Camera2D & Camera : Process->Cameras){
                if(Camera.getID() != Condition.Location.cameraID){
                    continue;
                }
                if(Camera.getIsDeleted()){
                    break;
                }
                if(Condition.Location.attribute == is_selected){
                    NewValue.setBool(SelectedCamera == &Camera);
                }
                else{
                    NewValue.copyValue(Camera.getValue(Condition.Location.attribute));
                }
                //NewValue.setID(transSourceToStr(Condition.Location.source) + "_" + Condition.Location.attribute, nullptr);
                return;
            }
            return;}
        case layer:{
            if(getProcess(NewValue, Processes, Process, Condition.Location.process)){
                return;
            }
            //NewValue.setID(transSourceToStr(Condition.Location.source) + "_" + Condition.Location.attribute, nullptr);
            for(const LayerClass & Layer : Process->Layers){
                if(Layer.getID() != Condition.Location.layerID){
                    continue;
                }
                if(Layer.getIsDeleted()){
                    break;
                }
                NewValue.copyValue(Layer.getValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe()));
                //NewValue.setID(transSourceToStr(Condition.Location.source) + "_" + Condition.Location.attribute, nullptr);
                return;
            }
            return;}
        case object:{
            if(getProcess(NewValue, Processes, Process, Condition.Location.process)){
                return;
            }
            NewValue.copyValue(Process->findNextValueAmongObjects(Condition, Owner, OwnerLayer, Engine.Mouse));
            return;}
        case display_resized:{
            NewValue.setBool(Engine.displayResized);
            return;}
        case vector_s:
            NewValue.copyValue(getValueFromVector(Condition, ObjectMemory));
            return;
        case mouse_x:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            NewValue.setDouble(Engine.Mouse.getPos().x);
            return;}
        case mouse_y:{
            if(checkIfUserCanInteract(NewValue, Condition.Location.source, Condition.Location.attribute, canUserInteract)){
                return;
            }
            NewValue.setDouble(Engine.Mouse.getPos().y);
            return;}
        case display_w:{
            NewValue.setInt(Engine.displaySize.x);
            return;}
        case display_h:{
            NewValue.setInt(Engine.displaySize.y);
            return;}
        case number_of_processes:{
            NewValue.setInt(Processes->size());
            return;}
        case number_of_cameras:{
            NewValue.setInt(0);
            for(const ProcessClass & ProcessIter : *Processes){
                NewValue.addInt(ProcessIter.Cameras.size());
            }
            return;}
        case number_of_layers:{
            NewValue.setInt(0);
            for(const ProcessClass & ProcessIter : *Processes){
                NewValue.addInt(ProcessIter.Layers.size());
            }
            return;}
        case number_of_objects:{
            NewValue.setInt(0);
            for(const ProcessClass & ProcessIter : *Processes){
                for(const LayerClass & Layer : ProcessIter.Layers){
                    NewValue.addInt(Layer.Objects.size());
                }
            }
            return;}
        case booting:{
            if(getProcess(NewValue, Processes, Process, Condition.Location.process)){
                return;
            }
            NewValue.setBool(Process->firstIteration);
            return;}
        case process:{
            if(getProcess(NewValue, Processes, Process, Condition.Location.process)){
                return;
            }
            if(Condition.Location.attribute == number_of_cameras_a){
                NewValue.setInt(Process->Cameras.size());
                return;
            }
            if(Condition.Location.attribute == number_of_layers_a){
                NewValue.setInt(Process->Layers.size());
                return;
            }
            if(Condition.Location.attribute == number_of_objects_a){
                NewValue.setInt(0);
                for(const LayerClass & Layer : Process->Layers){
                    NewValue.addInt(Layer.Objects.size());
                }
                return;
            }
            NewValue.setBool(false);
            return;}
        case context:
            getValueFromContext(Condition, ObjectMemory, Owner, OwnerLayer, Engine.Mouse, NewValue, NewValueVec);
            return;
        default:
            break;
    }
    
    cerr << instructionError(CurrentInstr, __FUNCTION__)
        << "Source \'" << sourceToStr(Condition.Location.source) << "\' is not valid.\n";
    NewValue.setBool(false);
}
char ProcessClass::evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, vector<VariableModule> & resultStack,
    AncestorObject * Owner, LayerClass * OwnerLayer, const EngineClass & Engine, ObjectMemoryStruct & ObjectMemory
){
    
    short ignoreFlagOr = 0, ignoreFlagAnd = 0;
    bool comparasion;
    int resultInt;
    double resultDouble;
    int stackSize = -1;
    
    for(ConditionClass & Condition : ConditionalChain){
        if(ignoreFlagOr == 0 && ignoreFlagAnd == 0){
            findNextValue(Condition, Owner, OwnerLayer, Engine, nullptr, ObjectMemory, resultStack[++stackSize], nullptr);
            //resultStack[++stackSize].copyValue(LeftOperandProc);
        }
        if(stackSize == -1){
            continue;
        }
        for(const EngineInstr & op : Condition.operators){
            if(ignoreFlagOr > 0 || ignoreFlagAnd > 0){
                if(op == EngineInstr::or_i && ignoreFlagOr > 0){
                    ignoreFlagOr--;
                }
                if(op == EngineInstr::and_i && ignoreFlagAnd > 0){
                    ignoreFlagAnd--;
                }
            }
            else if(op == EngineInstr::not_i){
                LeftOperandProc.copyValue(resultStack[stackSize]);
                LeftOperandProc.negate();
                if(printOutLogicalEvaluations){
                    cout << "!(" << resultStack[stackSize].getID() << ":" << resultStack[stackSize].getAnyValue()
                        << ") -> " << LeftOperandProc.getAnyValue() << "\n";
                }
                --stackSize;
                //LeftOperand.setID("!("+LeftOperand.getID()+")", nullptr);
                resultStack[++stackSize].copyValue(LeftOperandProc);
            }
            else if(op == EngineInstr::igT){
                if(resultStack[stackSize].isConditionMet<bool>(true, EngineInstr::equal, 'b')){
                    if(printOutLogicalEvaluations){
                        cout << resultStack[stackSize].getID() << ":" << "true || ... -> true\n";
                    }
                    ignoreFlagOr++;
                }
            }
            else if(op == EngineInstr::igF){
                if(resultStack[stackSize].isConditionMet<bool>(false, EngineInstr::equal, 'b')){
                    if(printOutLogicalEvaluations){
                        cout << resultStack[stackSize].getID() << ":" << "false && ... -> false\n";
                    }
                    ignoreFlagAnd++;
                }
            }
            else if(stackSize >= 1){
                RightOperandProc.copyValue(resultStack[stackSize]);
                --stackSize;
                LeftOperandProc.copyValue(resultStack[stackSize]);
                --stackSize;

                resultStack[++stackSize].clear();

                switch(op){
                    case and_i:
                    case or_i:
                    case equal:
                    case not_equal:
                    case more:
                    case less:
                    case more_equal:
                    case less_equal:
                        comparasion = cmpVars(&LeftOperandProc, op, &RightOperandProc, CurrentInstr);
                        
                        if(printOutLogicalEvaluations){
                            cout << LeftOperandProc.getID() << ":"  << shortenText(LeftOperandProc.getAnyValue(), maxLengthOfValuesPrinting) << " "
                                << instrToStr(op) << " " << RightOperandProc.getID() << ":"
                                    << shortenText(RightOperandProc.getAnyValue(), maxLengthOfValuesPrinting) << " -> " << comparasion << "\n";
                        }
                        
                        resultStack[stackSize].setBool(comparasion);
                        break;
                    case add:
                    case sub:
                    case mul:
                    case div_i:
                    case mod:
                    case pow_i:
                        if(LeftOperandProc.getType() == 'd' || RightOperandProc.getType() == 'd'){
                            resultDouble = LeftOperandProc.floatingOperation(op, &RightOperandProc);

                            if(printOutLogicalEvaluations){
                                cout << LeftOperandProc.getID() << ":"  << shortenText(LeftOperandProc.getAnyValue(), maxLengthOfValuesPrinting)
                                    << " " << instrToStr(op) << " " << RightOperandProc.getID() << ":"
                                    << shortenText(RightOperandProc.getAnyValue(), maxLengthOfValuesPrinting) << " -> " << resultDouble << "\n";
                            }

                            resultStack[stackSize].setDouble(resultDouble);
                        }
                        else{
                            resultInt = LeftOperandProc.intOperation(op, &RightOperandProc);

                            if(printOutLogicalEvaluations){
                                cout << LeftOperandProc.getID() << ":"  << shortenText(LeftOperandProc.getAnyValue(), maxLengthOfValuesPrinting) << " "
                                    << instrToStr(op) << " " << RightOperandProc.getID() << ":"
                                    << shortenText(RightOperandProc.getAnyValue(), maxLengthOfValuesPrinting) << " -> " << resultInt << "\n";
                            }

                            resultStack[stackSize].setInt(resultInt);
                        }
                        break;
                    default:
                        cerr << instructionError(CurrentInstr, __FUNCTION__)
                            << "Operator '" << instrToStr(op) << "' unrecognized in the if statement.\n";
                        break;
                }
            }
        }
    }

    if(stackSize == 0){
        if(resultStack[stackSize].getType() == 's'){
            if(isStringInGroup(resultStack[stackSize].vString, 3, "true", "t", "1")){
                return 't';
            }
            else if(isStringInGroup(resultStack[stackSize].vString, 3, "false", "f", "0")){
                return 'f';
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "String is not a valid booleon value.\n";
            }
        }
        if(resultStack[stackSize].vBool){
            return 't';
        }
        return 'f';
    }

    //al_draw_filled_circle(SCREEN_W/2, SCREEN_H/2, 10, al_map_rgb_f(1.0, 0.0, 0.0));
    return 't';
}

std::pair<vector<EventModule>::iterator, ChildStruct*> ProcessClass::findChildEventToRun(
    vector<EventModule> & EventContainer, vector<EventModule>::iterator & Event,
    const unsigned & runChildEventWithIndex
){
    if(runChildEventWithIndex >= Event->Children.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Index " << runChildEventWithIndex << " is out of scope of child events container ("
                << Event->Children.size() << ").\n";
        return {Event, nullptr};
    }

    ChildStruct* SelectedChild = &Event->Children[runChildEventWithIndex];

    if(SelectedChild->containerIndex >= EventContainer.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Index " << SelectedChild->containerIndex << " is out of scope of event container ("
            << EventContainer.size() << ").\n";
        return {Event, nullptr};
    }

    vector<EventModule>::iterator ChildEvent = EventContainer.begin()
        + SelectedChild->containerIndex;

    if(ChildEvent->getUniqueIndex() != SelectedChild->uniqueIndex){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Event '" << ChildEvent->getID() << "' was found in place of event '"
            << SelectedChild->id << "'. Index " << SelectedChild->containerIndex
            << " is incorrect. Check if the event '" << SelectedChild->id << "' was defined.\n";
        return {Event, nullptr};
    }

    if(ChildEvent->getIsDeleted() || !ChildEvent->getIsActive()){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Event '" << SelectedChild->id
            << "' does not exist.\n";
        return {Event, nullptr};
    }
    return {ChildEvent, SelectedChild};
}
template <class Module>
void deleteModuleInstance(vector<Module> & Container, vector<string> & IDs,
    bool & layersWereModified
){
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
template <class Module>
vector<size_t> deleteModuleInstanceAndReturnIndexes(vector<Module> & Container, vector<string> & IDs, bool & layersWereModified){
    vector<size_t> deletedIndexes;
    size_t deletionCount = 0; //Index normalisation
    for(auto moduleInstanceIt = Container.begin(); moduleInstanceIt != Container.end();){
        if(moduleInstanceIt->getIsDeleted()){
            removeFromVector(IDs, moduleInstanceIt->getID());
            layersWereModified = true;
            deletedIndexes.push_back(std::distance(Container.begin(), moduleInstanceIt) + deletionCount);
            moduleInstanceIt->clear();
            moduleInstanceIt = Container.erase(moduleInstanceIt);
            ++deletionCount;
        }
        else{
            ++moduleInstanceIt;
        }
    }
    return deletedIndexes;
}
bool deleteEventInstance(vector<EventModule> & Container, vector<string> & IDs, bool & layersWereModified, ObjectMemoryStruct & ContextMap){
    bool wereEventsDeleted = false;
    for(auto Instance = Container.begin(); Instance != Container.end();){
        if(Instance->getIsDeleted() || Instance->willBeDeleted){
            vector<unsigned> indexesForDeletion;
            for(size_t contextIdx = 0; contextIdx < ContextMap.MemoryMap.size(); ++contextIdx){
                // if(ContextMap.MemoryMap[contextIdx].definitionEventId == Instance->getID()){
                //     indexesForDeletion.push_back(contextIdx);
                // }
            }
            // for(const unsigned & key : indexesForDeletion){
            //     ContextMap.MemoryMap[key]; //???
            // }
            indexesForDeletion.clear();
            
            removeFromVector(IDs, Instance->getID());
            layersWereModified = true;
            wereEventsDeleted = true;
            Instance->clear();
            Instance = Container.erase(Instance);
        }
        else{
            ++Instance;
        }
    }
    return wereEventsDeleted;
}
void removeIndexFromOrder(const InstrDescription Description, vector<unsigned> & order, unsigned index){
    bool foundInOrder = false;
    unsigned camPosInOrder = 0;
    for(camPosInOrder = 0; camPosInOrder < order.size(); camPosInOrder++){
        if(order[camPosInOrder] == index){
            foundInOrder = true;
            break;
        }
    }
    if(!foundInOrder || order.size() == 0){
        cerr << instructionWarning(Description, __FUNCTION__)
            << ": Index " << index << " was not included in the order of size " << order.size() << ".\n";
        return;
    }
    for(; camPosInOrder < order.size()-1; camPosInOrder++){
        order[camPosInOrder] = order[camPosInOrder + 1];
    }
    order.pop_back();
    for(camPosInOrder = 0; camPosInOrder < order.size(); camPosInOrder++){
        if(order[camPosInOrder] > index){
            order[camPosInOrder]--;
        }
    }
}
inline void fixIndexesAfterDeletion(MemoryMapType & objectMemory, const vector<size_t> &deletedIndexes, DataType variableType){
    if(!deletedIndexes.empty()){
        for(ContextClass & memVarIt : objectMemory){
            if(!memVarIt.isPointingToMember || memVarIt.type != variableType){ continue; }
            for(auto indexIt = deletedIndexes.rbegin(); indexIt < deletedIndexes.rend(); ++indexIt){
                if(memVarIt.containerIndex >= *indexIt){ --memVarIt.containerIndex; }
            }
        }
    }
}
void activateGarbageCollector(vector<LayerClass> &layers){
    for(LayerClass & layerIt : layers){
        for(AncestorObject & objectIt : layerIt.Objects){
            objectIt.executeGarbageCollector = true;
        }
    }
}
bool ProcessClass::deleteEntities(){
    bool wereLayersModified = false;
    unsigned entityIndex = 0;
    for(auto cameraIt = Cameras.begin(); cameraIt != Cameras.end(); entityIndex++){
        if(cameraIt->getIsDeleted()){
            removeFromVector(camerasIDs, cameraIt->getID());
            cameraIt->clear();
            cameraIt = Cameras.erase(cameraIt);
            removeIndexFromOrder(CurrentInstr, camerasOrder, entityIndex);
        }
        else{
            ++cameraIt;
        }
    }
    entityIndex = 0;
    //When using erase, iterators and references to the elements at or after the point of the erase
    //are invalidated. ~ https://en.cppreference.com/w/cpp/container/vector/erase.html
    bool invalidateFollowingLayers = false;
    for(auto Layer = Layers.begin(); Layer != Layers.end(); entityIndex++){
        if(Layer->getIsDeleted()){
            for(const AncestorObject & objectIt : Layer->Objects){
                ProcessMemory[objectIt.getUniqueIndex()].clear();
                ProcessMemory.erase(objectIt.getUniqueIndex());
            }
            removeFromVector(layersIDs, Layer->getID());
            wereLayersModified = true;
            Layer->clear();
            Layer = Layers.erase(Layer);
            removeIndexFromOrder(CurrentInstr, layersOrder, entityIndex);
            invalidateFollowingLayers = true;
        }
        else{
            unsigned objectIndex = 0;
            bool invalidateFollowingObjects = false;
            for(vector<AncestorObject>::iterator objectIt = Layer->Objects.begin(); 
                objectIt != Layer->Objects.end(); objectIndex++
            ){
                if(objectIt->getIsDeleted()){
                    ProcessMemory[objectIt->getUniqueIndex()].clear();
                    ProcessMemory.erase(objectIt->getUniqueIndex());
                    removeFromVector(Layer->objectsIDs, objectIt->getID());
                    wereLayersModified = true;
                    objectIt->clear();
                    objectIt = Layer->Objects.erase(objectIt);
                    removeIndexFromOrder(CurrentInstr, Layer->objectsOrder, objectIndex);
                    invalidateFollowingObjects = true;
                }
                else{
                    if(invalidateFollowingLayers || invalidateFollowingObjects){
                        objectIt->hasInvalidatedMemory = true;
                    }
                    deleteModuleInstance(objectIt->SuperTextContainer, 
                        objectIt->superTextContainerIDs, wereLayersModified
                    );
                    deleteModuleInstance(objectIt->SuperEditableTextContainer, 
                        objectIt->superEditableTextContainerIDs, wereLayersModified
                    );
                    deleteModuleInstance(objectIt->ImageContainer, objectIt->imageContainerIDs, 
                        wereLayersModified
                    );
                    deleteModuleInstance(objectIt->MovementContainer, 
                        objectIt->movementContainerIDs, wereLayersModified
                    );
                    deleteModuleInstance(objectIt->CollisionContainer, 
                        objectIt->collisionContainerIDs, wereLayersModified
                    );
                    deleteModuleInstance(objectIt->ParticlesContainer, 
                        objectIt->particlesContainerIDs, wereLayersModified
                    );
                    if(deleteEventInstance(objectIt->EventContainer, objectIt->eventContainerIDs,
                        wereLayersModified, ProcessMemory[objectIt->getUniqueIndex()]
                    )){
                        objectIt->findIndexesOfEventChildren(true);
                        detectRecursionInEvents(objectIt->EventContainer, CurrentInstr);
                    }
                    deleteModuleInstance(objectIt->ScrollbarContainer, 
                        objectIt->scrollbarContainerIDs, wereLayersModified
                    );
                    deleteModuleInstance(objectIt->PrimitivesContainer, 
                        objectIt->primitivesContainerIDs, wereLayersModified
                    );
                    vector<size_t> deletedVariableIndexes = deleteModuleInstanceAndReturnIndexes(
                        objectIt->VariablesContainer, objectIt->variablesContainerIDs, 
                        wereLayersModified
                    );
                    fixIndexesAfterDeletion(ProcessMemory[objectIt->getUniqueIndex()].MemoryMap,
                        deletedVariableIndexes, DataType::variable_mod
                    );
                    vector<size_t> deletedVectorIndexes = deleteModuleInstanceAndReturnIndexes(
                        objectIt->VectorContainer, objectIt->vectorContainerIDs, wereLayersModified
                    );
                    fixIndexesAfterDeletion(ProcessMemory[objectIt->getUniqueIndex()].MemoryMap, 
                        deletedVectorIndexes, DataType::vector_mod
                    );
                    if(!deletedVariableIndexes.empty() || !deletedVectorIndexes.empty()){
                        objectIt->hasInvalidatedMemory = true;
                    }
                    ++objectIt;
                }
            }
            ++Layer;
        }
    }
    if(wereLayersModified){
        activateGarbageCollector(Layers);
    }
    return wereLayersModified;
}
void removeOnInitTrigger(vector<TriggerType> & primaryTriggerTypes){
    for(auto primaryTrigger = primaryTriggerTypes.begin(); primaryTrigger != primaryTriggerTypes.end();){
        if((*primaryTrigger) == on_init){
            primaryTrigger = primaryTriggerTypes.erase(primaryTrigger);
        }
        else{
            ++primaryTrigger;
        }
    }
}
inline bool isEventTriggered(const Triggers & CurrentTriggers, const std::vector<EventModule>::iterator & Event){
    if(Event->getIsDeleted() || !Event->getIsActive() || Event->primaryTriggerTypes.size() == 0){
        return false;
    }
    for(const TriggerType & eventTrigger : Event->primaryTriggerTypes){
        switch(eventTrigger){
            case by_movement:
                if(CurrentTriggers.movingObjects.contains(Event->getObjectID())){
                    return true;
                }
                break;
            case by_stillness:
                if(CurrentTriggers.stillObjects.contains(Event->getObjectID())){
                    return true;
                }
                break;
            default:
                if(CurrentTriggers.active.contains(eventTrigger) || eventTrigger == on_init){
                    return true;
                }
                break;
        }
    }
    return false;
}
inline bool areTypesCompatible(const DataType & leftOperand, const DataType & rightOperand){
    if(rightOperand == any_dt){
        return true;
    }
    switch (leftOperand){
        case bool_inst:
        case int_inst:
        case double_inst:
            switch(rightOperand){
                case bool_inst:
                case int_inst:
                case double_inst:
                case value_inst:
                case pointer_inst:
                case variable_mod:
                    return true;
                default:
                    return false;
            }
            return false;
        case value_inst:
        case pointer_inst:
        case string_inst:
            switch(rightOperand){
                case bool_inst:
                case int_inst:
                case double_inst:
                case string_inst:
                case value_inst:
                case pointer_inst:
                case variable_mod:
                    return true;
                default:
                    return false;
            }
            return false;
        case int_vec:
        case bool_vec:
        case double_vec:
            switch(rightOperand){
                case bool_vec:
                case int_vec:
                case double_vec:
                case value_vec:
                case pointer_vec:
                case variable_mod_vec:
                    return true;
                default:
                    return false;
            }
            return false;
        case string_vec:
        case value_vec:
        case pointer_vec:
            switch(rightOperand){
                case bool_vec:
                case int_vec:
                case double_vec:
                case string_vec:
                case value_vec:
                case pointer_vec:
                case variable_mod_vec:
                    return true;
                default:
                    return false;
            }
            return false;
        case camera_inst:
        case layer_inst:
        case object_inst:
        case variable_mod:
        case vector_mod:
        case super_text_mod:
        case super_editable_text_mod:
        case image_mod:
        case movement_mod:
        case collision_mod:
        case particles_mod:
        case event_mod:
        case scrollbar_mod:
        case primitives_mod:
        case camera_vec:
        case layer_vec:
        case object_vec:
        case variable_mod_vec:
        case vector_mod_vec:
        case super_text_mod_vec:
        case super_editable_text_mod_vec:
        case image_mod_vec:
        case movement_mod_vec:
        case collision_mod_vec:
        case particles_mod_vec:
        case event_mod_vec:
        case scrollbar_mod_vec:
        case primitives_mod_vec:
            if(leftOperand == rightOperand){
                return true;
            }
            return false;
        case any_dt:
            return true;
        default:
            return false;
    }
}
inline bool areTypesCompatibleWithReference(const DataType & leftOperand, const DataType & rightOperand){
    if(rightOperand == any_dt){
        return true;
    }
    //Reference skips moveRightToLeft function - it uses the same context under other name.
    switch(leftOperand){
        case bool_inst:
        case int_inst:
        case double_inst:
            switch(rightOperand){
                case bool_inst:
                case int_inst:
                case double_inst:
                case value_inst:
                    return true;
                default:
                    return false;
            }
            return false;
        case string_inst:
        case value_inst:
            switch(rightOperand){
                case bool_inst:
                case int_inst:
                case double_inst:
                case string_inst:
                case value_inst:
                    return true;
                default:
                    return false;
            }
            return false;
        case int_vec:
        case bool_vec:
        case double_vec:
            switch(rightOperand){
                case bool_vec:
                case int_vec:
                case double_vec:
                case value_vec:
                    return true;
                default:
                    return false;
            }
            return false;
        case string_vec:
        case value_vec:
            switch(rightOperand){
                case bool_vec:
                case int_vec:
                case double_vec:
                case string_vec:
                case value_vec:
                    return true;
                default:
                    return false;
            }
            return false;
        case pointer_inst:
        case pointer_vec:
        case camera_inst:
        case layer_inst:
        case object_inst:
        case variable_mod:
        case vector_mod:
        case super_text_mod:
        case super_editable_text_mod:
        case image_mod:
        case movement_mod:
        case collision_mod:
        case particles_mod:
        case event_mod:
        case scrollbar_mod:
        case primitives_mod:
        case camera_vec:
        case layer_vec:
        case object_vec:
        case variable_mod_vec:
        case vector_mod_vec:
        case super_text_mod_vec:
        case super_editable_text_mod_vec:
        case image_mod_vec:
        case movement_mod_vec:
        case collision_mod_vec:
        case particles_mod_vec:
        case event_mod_vec:
        case scrollbar_mod_vec:
        case primitives_mod_vec:
            if(leftOperand == rightOperand){
                return true;
            }
            return false;
        case any_dt:
            return true;
        default:
            return false;
    }
}
//Return true if there's no need to allocate new memory.
inline bool findFreeDynamicMemory(ObjectMemoryStruct & ObjectMemory, vector<DynamicVariableInfo> & NewLocalVariables, const bool & isReference){
    for(unsigned memIdx = ObjectMemory.topFreeDynamicAddress; memIdx < ObjectMemory.DynamicMemory.size(); ++memIdx){
        DynamicMemoryStruct &dynamicVarIt = ObjectMemory.DynamicMemory[memIdx];
        if(dynamicVarIt.inUse){
            continue;
        }
        dynamicVarIt.inUse = true;
        ObjectMemory.topFreeDynamicAddress = memIdx+1;
        NewLocalVariables.emplace_back(DynamicVariableInfo(dynamicVarIt.address, memIdx, isReference));
        return true;
    }
    return false;
}
void ProcessClass::allocateNewDynamicMemory(ObjectMemoryStruct & ObjectMemory, const VariableInfo & NewLocalVar){
    NewContext.clear();
    NewContext.ID = NewLocalVar.name;
    NewContext.type = NewLocalVar.type;
    unsigned newDynamicAddress = ObjectMemory.topAddress++;

    if(newDynamicAddress < ObjectMemory.MemoryMap.size()){
        cerr << "Error: Address " << newDynamicAddress << " is already allocated.\n";
        return;
    }
    else if(newDynamicAddress > ObjectMemory.MemoryMap.size()){
        if(newDynamicAddress == 0 || newDynamicAddress - ObjectMemory.MemoryMap.size() > 100){
            cerr << "Error: You cannot allocate more than 100 new memory addresses.\n";
            return;
        }
        cerr << "Warning: Allocating empty memory block between addresses: "
            << ObjectMemory.MemoryMap.size() << " and " << newDynamicAddress-1 << ".\n";
        ObjectMemory.MemoryMap.resize(newDynamicAddress-1);
    }
    
    ObjectMemory.MemoryMap.push_back(NewContext);
    ObjectMemory.DynamicMemory.emplace_back(DynamicMemoryStruct(newDynamicAddress, true));
    ObjectMemory.topFreeDynamicAddress = ObjectMemory.DynamicMemory.size();
}
void ProcessClass::allocateMemoryForDynamicVariables(const vector<EventModule>::iterator & newEventIt, ObjectMemoryStruct & ObjectMemory){
    //If the event was called for the first time in the current stack, use pre-allocated memory found under the default address. 
    if(!EventCallState.isCurrentCallRecursive){
        for(const VariableInfo & localVarIt : newEventIt->LocalVariables){
            LocalToGlobalTranslation.emplace_back(DynamicVariableInfo(localVarIt.defaultAddress, 0, localVarIt.isReference));
		}
        return;
	}
	//If it's a recurrent call, find already allocated memory that is not currently in use or allocate new memory.
    for(VariableInfo & localVarIt : newEventIt->LocalVariables){
        if(localVarIt.isReference){
            LocalToGlobalTranslation.emplace_back(DynamicVariableInfo(localVarIt.defaultAddress, 0, true));
            continue;
        }		
        
        if(!findFreeDynamicMemory(ObjectMemory, LocalToGlobalTranslation, localVarIt.isReference)){
            allocateNewDynamicMemory(ObjectMemory, localVarIt);
            LocalToGlobalTranslation.emplace_back(DynamicVariableInfo(
                ObjectMemory.DynamicMemory.back().address, ObjectMemory.DynamicMemory.size()-1, localVarIt.isReference
            ));
        }
    }
}
void ProcessClass::deallocateDynamicallyAllocatedMemory(vector<DynamicMemoryStruct> & DynamicMemory, unsigned & topDynamicAddress){
    for(DynamicVariableInfo & localVarIt : LocalToGlobalTranslation){
        if(localVarIt.isReference){
            continue;
        }
        if(localVarIt.dynamicMemoryAddress >= DynamicMemory.size()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Dynamic memory at address " << localVarIt.dynamicMemoryAddress
                << " has already been deallocated.\n";
            continue;
        }
        DynamicMemory[localVarIt.dynamicMemoryAddress].inUse = false;
        topDynamicAddress = localVarIt.dynamicMemoryAddress;
    }
}
bool ProcessClass::passVariablesToTheChild(const vector<PassingVariableInfo> & ParentEventArguments,
    const vector<PassingVariableInfo> & CurrentEventParameters,
    vector<EventModule>::iterator & ParentEvent, vector<EventModule>::iterator & CurrentEvent,
    vector<DynamicVariableInfo> & ParentEventLocalVariables,
    vector<DynamicVariableInfo> & CurrentEventLocalVariables,
    ObjectMemoryStruct & ObjectMemory
){
    if(CurrentEventParameters.size() != ParentEventArguments.size()){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
			<< "Number of passed arguments (" << ParentEventArguments.size() << ")"
			<<" is not equal to the number of event parameters (" << CurrentEventParameters.size() << ").\n";
		return true;
    }
    for(size_t argIdx = 0; argIdx < CurrentEventParameters.size(); argIdx++){
        const PassingVariableInfo & Argument = ParentEventArguments[argIdx];
		const PassingVariableInfo & Parameter = CurrentEventParameters[argIdx];
        
        if(Parameter.isReference){
			if(!areTypesCompatibleWithReference(Parameter.type, Argument.type)){
				cerr << instructionError(CurrentInstr, __FUNCTION__)
					<< "Cannot pass an argument of '" << dataTypeToStr(Argument.type)
					<< "' type to a parameter of '" << dataTypeToStr(Parameter.type) << "' type.\n";
				return true;
			}
            //When passing by reference you only need to copy the dynamic address of the variable instead of copying the whole its state.
            CurrentEventLocalVariables[Parameter.localAddress].dynamicAddress = ParentEventLocalVariables[Argument.localAddress].dynamicAddress;

			continue;		
		}
        if(!areTypesCompatible(Parameter.type, Argument.type)){
			cerr << instructionError(CurrentInstr, __FUNCTION__)
				<< "Cannot pass an argument of '" << dataTypeToStr(Argument.type)
				<< "' type to a parameter of '" << dataTypeToStr(Parameter.type) << "' type.\n";
			return true;
		}

        ContextClass * ArgumentVariable = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
            ParentEventLocalVariables[Argument.localAddress], Argument.name, true
        );
        if(ArgumentVariable == nullptr){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Variable '" << Argument.name << "' does not exist.\n";
            continue;
        }
		ContextClass * ParameterVariable = getVariableByAddress(CurrentInstr, ObjectMemory.MemoryMap,
            CurrentEventLocalVariables[Parameter.localAddress], Parameter.name, true
        );
        if(ParameterVariable == nullptr){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Variable '" << Parameter.name << "' does not exist.\n";
            continue;
        }

        if(Parameter.isReference){
			if(!areTypesCompatibleWithReference(ParameterVariable->type, ArgumentVariable->type)){
				cerr << instructionError(CurrentInstr, __FUNCTION__)
					<< "Cannot pass an argument of '" << dataTypeToStr(ArgumentVariable->type)
					<< "' type to a parameter of '" << dataTypeToStr(ParameterVariable->type) << "' type.\n";
				return true;
			}
			continue;		
		}
        if(!areTypesCompatible(ParameterVariable->type, ArgumentVariable->type)){
			cerr << instructionError(CurrentInstr, __FUNCTION__)
				<< "Cannot pass an argument of '" << dataTypeToStr(ArgumentVariable->type)
				<< "' type to a parameter of '" << dataTypeToStr(ParameterVariable->type) << "' type.\n";
			return true;
		}

        moveRightToLeft(CurrentInstr, EngineInstr::assign, ParameterVariable, *ArgumentVariable);
    }
    return false;
}
EventControlFlow ProcessClass::prepareChildEvent(ObjectMemoryStruct & ObjectMemory, vector<EventStackStruct> & EventStack,
    vector<EventModule>::iterator & eventIt, ChildStruct * SelectedChild
){
    CurrentInstr.lineNumber = SelectedChild->lineNumber;
    CurrentInstr.scriptName = SelectedChild->callingScript;
    
    CurrentInstr.eventID = eventIt->getID();
    CurrentInstr.instruction = EngineInstr::run;

    if(printOutInstructions){
        cout << "run " << eventIt->getID() << "(";
        if(SelectedChild != nullptr){
            for(auto variable : SelectedChild->Arguments){
                cout << variable.name << ", ";
            }
        }
        cout << ")\n";
    }

    EventCallState.clear();
    LocalToGlobalTranslation.clear();
    EventCallState.isCurrentCallRecursive = SelectedChild->isRecursiveCall;
    allocateMemoryForDynamicVariables(eventIt, ObjectMemory);
    if(passVariablesToTheChild(SelectedChild->Arguments, eventIt->Parameters, EventStack.back().Event,
        eventIt, EventStack.back().LocalVariables, LocalToGlobalTranslation, ObjectMemory
    )){
        return flow_abort;
    }
    return flow_run_function;
}
EventControlFlow ProcessClass::executeSingleEvent(EngineClass & Engine, vector<ProcessClass> & Processes,
    vector<EventModule>::iterator & startingEventIt, vector<EventModule>::iterator & eventIt,
    vector<EventStackStruct> & EventStack, ObjectMemoryStruct & ObjectMemory, vector<AncestorObject*> & TriggeredObjects,
    LayerClass *& TriggeredLayer, AncestorObject *& Triggered
){
    CurrentInstr.eventID = eventIt->getID();
    removeOnInitTrigger(eventIt->primaryTriggerTypes);
    if(printOutInstructions){
        printInColor("\n---Current event: " + TriggeredLayer->getID() + "::" + Triggered->getID() + "::" + eventIt->getID() + "\n", 14);
        if(interruptInstruction == EngineInstr::break_i){
            printInColor("---break\n", 14);
        }
        else if(interruptInstruction == EngineInstr::return_i){
            printInColor("---return\n", 14);
        }
    }

    //Execute all instructions bound to an event.
    if(interruptInstruction != EngineInstr::break_i && interruptInstruction != EngineInstr::return_i){
        unsigned runChildEventWithIndex = 0;
        if(EventCallState.programCounter < eventIt->Operations.size()){
            interruptInstruction = executeInstructions(
                TriggeredLayer, Triggered, ObjectMemory, TriggeredObjects,
                Processes, startingEventIt, eventIt, EventStack, Engine, runChildEventWithIndex
            );

            if(interruptInstruction == EngineInstr::exit_i
                || interruptInstruction == EngineInstr::assert
            ){
                Engine.closeProgram = true;
                return flow_abort;
            }
            else if(interruptInstruction == EngineInstr::reboot){
                Engine.reboot = true;
                return flow_abort;
            }
            if(TriggeredLayer == nullptr || Triggered == nullptr){
                return flow_self_deletion;
            }
        }
        if(interruptInstruction == EngineInstr::run){
            interruptInstruction = EngineInstr::null;
            EventStack.emplace_back(eventIt, LocalToGlobalTranslation, EventCallState);

            ChildStruct * SelectedChild = nullptr;

            std::tie(eventIt, SelectedChild) = findChildEventToRun(Triggered->EventContainer,
                eventIt, runChildEventWithIndex
            );
            
            if(SelectedChild == nullptr){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Pointer to the child event instance is null. Aborting.\n";
                return flow_abort;
            }

            return prepareChildEvent(ObjectMemory, EventStack, eventIt, SelectedChild);
        }
    }

    //jump back in event stack
    if(!EventStack.empty()){
        if(printOutInstructions){
            printInColor("---go_back\n", 14);
        }

        if(!eventIt->isInline){
            interruptInstruction = EngineInstr::null;
        }

        if(EventCallState.isCurrentCallRecursive){
            deallocateDynamicallyAllocatedMemory(ObjectMemory.DynamicMemory, ObjectMemory.topFreeDynamicAddress);
        }
        
        eventIt = EventStack.back().Event;
        LocalToGlobalTranslation = EventStack.back().LocalVariables;
        EventCallState = EventStack.back().EventCallState;
        EventStack.pop_back();
        return flow_jump_back;
    }

    interruptInstruction = EngineInstr::null;

    return flow_next_event;
}
inline bool findNextEvent(const Triggers & CurrentTriggers, vector<EventModule>::iterator & eventIt,
    vector<EventModule>::iterator & startingEventIt, AncestorObject *& TriggeredObject
){
    auto end = TriggeredObject->EventContainer.end();

    while(++eventIt != end && ++startingEventIt != end){
        if(isEventTriggered(CurrentTriggers, eventIt)){
            return true;
        }
    }

    return false;
}
//Clear module pointers from different objects when their memory layout is modified (deletions, additions, etc.)
void collectGarbage(AncestorObject *& TriggeredObject, ObjectMemoryStruct & ObjectMemory, vector<AncestorObject*> &TriggeredObjects){
    if(TriggeredObject->executeGarbageCollector){
        TriggeredObject->executeGarbageCollector = false;
        for(size_t varIdx = builtInVarAddr::MAX_bv; varIdx < ObjectMemory.MemoryMap.size(); ++varIdx){
            if(ObjectMemory.MemoryMap[varIdx].isPointingToMember){ continue; }
            ObjectMemory.MemoryMap[varIdx].clearPointers();
        }
    }
}
bool ProcessClass::executeEventLoop(EngineClass & Engine, vector<ProcessClass> & Processes,
    const Triggers & CurrentTriggers, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, ObjectMemoryStruct & ObjectMemory,
    vector<AncestorObject*> & TriggeredObjects, LayerClass *& TriggeredLayer,
    AncestorObject *& TriggeredObject
){
    vector<EventStackStruct> EventStack;

    EventCallState.clear();
    LocalToGlobalTranslation.clear();
    allocateMemoryForDynamicVariables(eventIt, ObjectMemory);

    interruptInstruction = EngineInstr::null;

    while(eventIt != TriggeredObject->EventContainer.end()){
        collectGarbage(TriggeredObject, ObjectMemory, TriggeredObjects);    

        EventControlFlow eEventControl = executeSingleEvent(
            Engine, Processes, startingEventIt, eventIt, EventStack,
            ObjectMemory, TriggeredObjects, TriggeredLayer, TriggeredObject
        );

        switch(eEventControl){
            case flow_next_event:
                if(findNextEvent(CurrentTriggers, eventIt, startingEventIt, TriggeredObject)){
                    EventCallState.clear();
                    LocalToGlobalTranslation.clear();
                    allocateMemoryForDynamicVariables(eventIt, ObjectMemory);
                }
                break;
            case flow_self_deletion:
                return false;
            case flow_abort:
                return true;
            case flow_run_function:
            case flow_jump_back:
            default:
                break;
        }
    }
    return false;
}
template<class Entity>
inline bool isEntityInaccessible(Entity * t_Entity){
    return t_Entity == nullptr || t_Entity->getIsDeleted() || !t_Entity->getIsActive();
}
inline bool findTriggeredLayer(vector<LayerClass> & AllLayers, AncestorObject * TriggeredObject, LayerClass *& TriggeredLayer){
    TriggeredLayer = nullptr;
    for(LayerClass & layerIt : AllLayers){
        if(TriggeredObject->getLayerID() == layerIt.getID()){
            TriggeredLayer = &layerIt;
            break;
        }
    }
    if(isEntityInaccessible(TriggeredLayer)){
        return true;
    }
    return false;
}
inline bool findFirstTriggeredEvent(AncestorObject * TriggeredObject, Triggers & CurrentTriggers, vector<EventModule>::iterator & eventIt){
    eventIt = TriggeredObject->EventContainer.begin();
    for(; eventIt < TriggeredObject->EventContainer.end(); eventIt++){
        if(isEventTriggered(CurrentTriggers, eventIt)){
            return false;
        }
    }
    return true;
}
inline void updatePersistentPointers(AncestorObject *& currentObj, MemoryMapType & memory, const InstrDescription &CurrentInstr){
    if(!currentObj->hasInvalidatedMemory){
        return;
    }
    currentObj->hasInvalidatedMemory = false;
    for(ContextClass & contextVar : memory){
        if(!contextVar.isPointingToMember){ continue; }
        switch(contextVar.type){
            case variable_mod:
                if(currentObj->VariablesContainer.empty()){
                    continue;
                }
                contextVar.Modules.Variables[0] = &currentObj->VariablesContainer[contextVar.containerIndex];
                break;
            case vector_mod:
                if(currentObj->VectorContainer.empty()){
                    continue;
                }
                contextVar.Modules.Vectors[0] = &currentObj->VectorContainer[contextVar.containerIndex];
                break;
            default:
                break;
        }
    }
}
bool ProcessClass::executeTriggeredEvents(EngineClass & Engine, vector<ProcessClass> & Processes,
    vector<AncestorObject*> & TriggeredObjects, Triggers & CurrentTriggers
){
    vector<EventModule>::iterator startingEventIt, eventIt;
    LayerClass * triggeredLayer = nullptr;

    for(AncestorObject * triggeredObjectIt : TriggeredObjects){
        if(isEntityInaccessible(triggeredObjectIt)
            || findFirstTriggeredEvent(triggeredObjectIt, CurrentTriggers, eventIt)
            || findTriggeredLayer(Layers, triggeredObjectIt, triggeredLayer)
        ){ continue; }
        
        startingEventIt = eventIt;

        if(!ProcessMemory.contains(triggeredObjectIt->getUniqueIndex())){
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Object '" 
                << triggeredObjectIt->getID() << "' doesn't have memory.\n";
            continue;
        }
        ObjectMemoryStruct & ObjectMemory = ProcessMemory[triggeredObjectIt->getUniqueIndex()];

        CurrentInstr.layerID = triggeredLayer->getID();
        CurrentInstr.objectID = triggeredObjectIt->getID();

        updatePersistentPointers(triggeredObjectIt, ObjectMemory.MemoryMap, CurrentInstr);

        ObjectMemory.MemoryMap[1].Objects[0] = triggeredObjectIt;
        ObjectMemory.MemoryMap[2].Layers[0] = triggeredLayer;

        if(executeEventLoop(Engine, Processes, CurrentTriggers,
            startingEventIt, eventIt, ObjectMemory, TriggeredObjects,
            triggeredLayer, triggeredObjectIt
        )){ return true; }

        if(wasNewExecuted || wasAnyEventUpdated){
            updateBaseOfTriggerableObjects(&CurrentTriggers);
            wasNewExecuted = false;
            wasAnyEventUpdated = false;
        }
    }
    return false;
}
void ProcessClass::detectAndExecuteTriggeredEvents(EngineClass & Engine, vector<ProcessClass> & Processes){
    //Only events from TriggeredObjects can be executed in the current iteration - events of newly created objects 
    //must wait with execution for the next iteration, unless run() command will be used.
    if(wasDeleteExecuted && deleteEntities()){
        updateBaseOfTriggerableObjects(nullptr);
        wasDeleteExecuted = false;
    }
    if(wasNewExecuted || wasAnyEventUpdated){
        updateBaseOfTriggerableObjects(nullptr);
        wasNewExecuted = false;
        wasAnyEventUpdated = false;
    }
    
    vector<AncestorObject*> TriggeredObjects;
    Triggers CurrentTriggers;
    detectTriggeredEvents(Engine, TriggeredObjects, CurrentTriggers);

    if(TriggeredObjects.empty()){
        Engine.isIdle = true;
        detectTriggeredByIdleEvents(Engine, TriggeredObjects, CurrentTriggers);
        if(TriggeredObjects.empty()){
            Engine.closeProgram = Engine.canExitWhenNoEventIsTriggered;
            return;
        }
    }
    else{
        Engine.isIdle = false;
    }

    //Remember to delete pointers to destroyed objects during the iteration
    
    if(executeTriggeredEvents(Engine, Processes, TriggeredObjects, CurrentTriggers)){
        return;
    }

    TriggeredObjects.clear();

    if(wasDeleteExecuted && deleteEntities()){
        updateBaseOfTriggerableObjects(nullptr);
        wasDeleteExecuted = false;
    }
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
            if(Engine.display != nullptr){
                al_set_system_mouse_cursor(Engine.display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
            }
            break;
    }
}
bool isALeaf(const string & leafID, const string & rootID, const vector<Camera2D> & Cameras){
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
    
    for(unsigned orderIndex = 0; orderIndex < index;){
        if(camerasOrder[orderIndex] >= Cameras.size()){
            cerr << "Error: Camera index <" << camerasOrder[orderIndex]
                << "> in the cameras' order is out of scope of camera's Container<"
                << Cameras.size() << ">.\n";
            orderIndex++;
            continue;
        }
        if(Cameras[camerasOrder[orderIndex]].getIsActive() && orderIndex >= treshold
            && isALeaf(Cameras[camerasOrder[orderIndex]].getID(), ChosenCamera->getID(), Cameras)
        ){
            Cameras[camerasOrder[orderIndex]].bringBack();
            index--;
            auto it = camerasOrder.begin() + orderIndex;
            std::rotate(it, it + 1, camerasOrder.end());
            orderIndex = 0;
            continue;
        }
        orderIndex++;
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
            cerr << "Error: Camera index <" << camerasOrder[activeCameraIndex] << "> in the cameras' order is out of scope of camera's Container<"
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
                cerr << "Error: Camera index <" << camerasOrder[j] << "> in the cameras' order is out of scope of camera's Container<"
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
            cerr << "Error: Camera index <" << camerasOrder[i] << "> in the cameras' order is out of scope of camera's Container<"
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
bool ProcessClass::isKeyFirstPressed(short key, vector<short> firstPressedKeys){
    for(short firstPressed : firstPressedKeys){
        if(firstPressed == key){
            return true;
        }
    }
    return false;
}
bool ProcessClass::isKeyPressed(short key, vector<short> pressedKeys){
    for(short pressed : pressedKeys){
        if(pressed == key){
            return true;
        }
    }
    return false;
}
bool ProcessClass::isKeyReleased(short key, vector<short> releasedKeys){
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
    if(display == nullptr){
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
    if(display == nullptr){
        return;
    }
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
void ProcessClass::detectBackgroundCollisions(LayerClass & Layer, AncestorObject & Object,
    vec2d momentum
){
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
bool ProcessClass::shouldCheckOverlapingOnly(CollisionModule & Collision,
    AncestorObject & SolidObject, CollisionModule & SolidCollision
){
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
void ProcessClass::detectRealCollisions(LayerClass & Layer, AncestorObject & Object,
    MovementModule & Movement
){
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

                if(ignoreDistantObjects && !SolidCollision.isCloseEnough(SolidObject.getPos(),
                    SolidObject.getID(), Object.getPos(), Movement.getMomentum(), &Collision
                )){
                    continue;
                }

                if(shouldCheckOverlapingOnly(Collision, SolidObject, SolidCollision)){
                    Collision.detectOverlaping(
                        SolidObject.getID(), SolidCollision.getID(),
                        SolidObject.getPos() + SolidCollision.getPos(),
                        SolidCollision.getSize(), Object.getPos(), Movement.getMomentum()
                    );
                    continue;
                }

                Collision.detectCollision(
                    SolidObject.getID(), SolidCollision.getID(),
                    SolidObject.getPos() + SolidCollision.getPos(),
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
            //Update momentum on the X axis. Third type of collisions is collision with corners -
            //you could either adjust X or Y axis, but I chose X axis.
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
        vec2d newPos(-FollowedByCamera->getPos().x + Camera.size.x/2,
            -FollowedByCamera->getPos().y + Camera.size.y/2
        );
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
                Movement.updateStatesAndVectors(canUserInteract, pressedKeys, pressedMouse,
                    releasedMouse, Mouse.getZoomedPos(SelectedCamera), Object.getPos()
                );
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
                if(!Camera.getIsActive() || Camera.getIsMinimized()
                    || !Camera.isLayerVisible(Layer.getID())
                ){
                    continue;
                }
                if(Camera.isFollowingObject && Camera.followedLayerID == Layer.getID()
                    && Camera.followedObjectID == Object.getID()
                ){
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
            SelectedObject->setPos(
                Mouse.getZoomedPos(SelectedCamera) - SelectedCamera->visionShift-dragStartingPos
            );
        }
        else{
            SelectedObject->setPos(Mouse.getPos() - dragStartingPos);
        }
    }
}
void ProcessClass::drawEverything(EngineClass & Engine){
    //al_set_clipping_rectangle(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
    
    size_t numberOfDrawnObjects;
    Camera2D * Camera;

    for(const unsigned & cameraIndex : camerasOrder){
        if(cameraIndex >= Cameras.size()){
            cerr << "Error: Camera index <" << cameraIndex
                << "> in the cameras' order is out of scope of camera's container<"
                << Cameras.size() << ">.\n";
            continue;
        }
        Camera = &Cameras[cameraIndex];
        if(!Camera->getIsActive() || Camera->getIsMinimized()
            || (!Camera->canDrawOnCamera && !Camera->drawOneFrame)
        ){
            continue;
        }
        Camera->drawOneFrame = false;
        al_set_clipping_rectangle(Camera->pos.x, Camera->pos.y, Camera->size.x, Camera->size.y);
        for(const unsigned & layerIndex : layersOrder){
            if(!Layers[layerIndex].getIsActive() || !Camera->isLayerVisible(Layers[layerIndex].getID())){
                continue;
            }
            
            numberOfDrawnObjects = 0;

            for(const unsigned & objectIndex : Layers[layerIndex].objectsOrder){
                if(!Layers[layerIndex].Objects[objectIndex].getIsActive()){
                    continue;
                }
                drawModules(Layers[layerIndex].Objects[objectIndex], *Camera, Engine.FontContainer, numberOfDrawnObjects, Engine.displaySize);
            }
            if(SelectedLayer == &Layers[layerIndex]){
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
void ProcessClass::drawModules(const AncestorObject & Object, Camera2D & Camera,
    vector<SingleFont> & FontContainer, size_t & numberOfDrawnObjects, vec2i displaySize
){
    vec2d newPos;
    vec2d objectSize;
    vec2d scaledObjectSize;

    for(const PrimitivesModule & Primitives : Object.PrimitivesContainer){
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

    for(const ImageModule & Image : Object.ImageContainer){
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
        Image.draw(Object.getPos()+Camera.pos, Camera, false);
        numberOfDrawnObjects++;
    }

    for(const SuperTextModule & SuperText : Object.SuperTextContainer){
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

    for(const SuperEditableTextModule & SuperEditableText : Object.SuperEditableTextContainer){
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

    for(const ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
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
        for(const CollisionModule & Hitbox : Object.CollisionContainer){
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

    for(const ParticleEffectModule & ParticleEffect : Object.ParticlesContainer){
        if(!ParticleEffect.getIsActive()){
            continue;
        }
        ParticleEffect.draw(Object.ImageContainer, displaySize, Camera);
        numberOfDrawnObjects++;
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
            SelectedObject->refreshPositionsAndSizesOfObjectAndItsImages();
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

AncestorObject *AncestorIndex::object(vector<LayerClass> &Layers) const{
    if(Layers.size() <= layerIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex(" << layerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
        return nullptr;
    }
    if(Layers[layerIndex].Objects.size() <= objectIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": objectIndex(" << objectIndex << ") goes out of scope of Layers[].Objects<" << Layers[layerIndex].Objects.size() << ">.\n";
        return nullptr;
    }
    return &Layers[layerIndex].Objects[objectIndex];
}
template <class Module>
Module * ModuleIndex::getModulePointer(vector<LayerClass> &Layers){
    if(Layers.size() <= layerIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex goes out of scope of Layers.\n";
        return nullptr;
    }
    if(Layers[layerIndex].Objects.size() <= objectIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": objectIndex goes out of scope of Layers[].Objects.\n";
        return nullptr;
    }
    if constexpr (std::is_same<Module, SuperTextModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].SuperTextContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].SuperTextContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, SuperEditableTextModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].SuperEditableTextContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].SuperEditableTextContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, ImageModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].ImageContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].ImageContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, MovementModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].MovementContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].MovementContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, CollisionModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].CollisionContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].CollisionContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, ParticleEffectModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].ParticlesContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].ParticlesContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, EventModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].EventContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].EventContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, VariableModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].VariablesContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].VariablesContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, PrimitivesModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].PrimitivesContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].PrimitivesContainer[moduleIndex];
    }
    else if constexpr (std::is_same<Module, VectorModule>::value){
        if(Layers[layerIndex].Objects[objectIndex].VectorContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
            return nullptr;
        }
        return &Layers[layerIndex].Objects[objectIndex].VectorContainer[moduleIndex];
    }
    else{
        if(Layers[layerIndex].Objects[objectIndex].ScrollbarContainer.size() <= moduleIndex){
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex goes out of scope of Layers[].Objects.<Module>\n";
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
vector<EventModule>::iterator ModuleIndex::module(vector<LayerClass> &Layers){
    if(Layers.size() <= layerIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex(" << layerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
        return vector<EventModule>::iterator();
    }
    if(Layers[layerIndex].Objects.size() <= objectIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": objectIndex(" << objectIndex << ") goes out of scope of Layers["
            << layerIndex << "].Objects<" << Layers[layerIndex].Objects.size() << ">.\n";
        return vector<EventModule>::iterator();
    }
    if(Layers[layerIndex].Objects[objectIndex].EventContainer.size() <= moduleIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": moduleIndex(" << moduleIndex
            << ") goes out of scope of Layers[" << layerIndex << "].Objects[" << objectIndex
            << "].EventContainer<" << Layers[layerIndex].Objects[objectIndex].EventContainer.size() << ">\n";
        return vector<EventModule>::iterator();
    }
    return Layers[layerIndex].Objects[objectIndex].EventContainer.begin() + moduleIndex;
}

void PointerRecalculator::clear(){
    ObjectIndexes.clear();
    CameraIndexes.clear();
    TriggeredObjectIndexes.clear();
    didActiveEditableTextExist = false;
    currentEventUniqueIndex = 0;
}
void PointerRecalculator::findIndexesForCameras(vector<Camera2D> &Cameras, ObjectMemoryStruct & ObjectMemory, Camera2D *& SelectedCamera){ 
    for(size_t contextIdx = 0; contextIdx < ObjectMemory.MemoryMap.size(); ++contextIdx){
        for(Camera2D * Camera : ObjectMemory.MemoryMap[contextIdx].Cameras){
            CameraIndexes[contextIdx].push_back(Camera - &Cameras[0]);
        }
    }
    if(SelectedCamera != nullptr){
        selectedCameraIndex = SelectedCamera - &Cameras[0];
    }
}
void PointerRecalculator::findIndexesForLayers(vector<LayerClass> &Layers, ObjectMemoryStruct & ObjectMemory, LayerClass *& OwnerLayer){
    for(size_t contextIdx = 0; contextIdx < ObjectMemory.MemoryMap.size(); ++contextIdx){
        for(LayerClass * Layer : ObjectMemory.MemoryMap[contextIdx].Layers){
            LayerIndexes[contextIdx].push_back(Layer - &Layers[0]);
        }
    }
    if(OwnerLayer != nullptr){
        for(LayerClass & Layer : Layers){
            if(Layer.getUniqueIndex() == OwnerLayer->getUniqueIndex()){
                eventOwnerLayerIndex = &Layer - &Layers[0];
                break;
            }
        }
    }
}
void PointerRecalculator::findIndexesForObjects(vector<LayerClass> &Layers, ObjectMemoryStruct & ObjectMemory, AncestorObject *& Owner,
    vector<AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject
){
    unsigned layerIndex;   
    for(size_t contextIdx = 0; contextIdx < ObjectMemory.MemoryMap.size(); ++contextIdx){
        for(AncestorObject * Object : ObjectMemory.MemoryMap[contextIdx].Objects){
            for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
                if(Layers[layerIndex].getUniqueIndex() == Object->getLayerUniqueIndex()){
                    ObjectIndexes[contextIdx].push_back(AncestorIndex(layerIndex, Object - &Layers[layerIndex].Objects[0]));
                    break;
                }
            }
        }
    }
    for(AncestorObject * Triggered : TriggeredObjects){
        TriggeredObjectIndexes.emplace_back(AncestorIndex(0, 0));
        for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
            if(Layers[layerIndex].getUniqueIndex() == Triggered->getLayerUniqueIndex()){
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
            if(Layers[layerIndex].getUniqueIndex() == Owner->getLayerUniqueIndex()){
                EventOwnerIndex = AncestorIndex(layerIndex, Owner - &Layers[layerIndex].Objects[0]);
                break;
            }
        }
    }
    if(SelectedObject != nullptr){
        for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
            if(Layers[layerIndex].getUniqueIndex() == SelectedObject->getLayerUniqueIndex()){
                SelectedObjectIndex = AncestorIndex(layerIndex, SelectedObject - &Layers[layerIndex].Objects[0]);
                break;
            }
        }
    }
}
void PointerRecalculator::findIndexesForObjectsInObjectMemory(vector<LayerClass> &Layers, MemoryMapType &MemoryMap){
    unsigned layerIndex;   
    for(size_t contextIdx = 0; contextIdx < MemoryMap.size(); ++contextIdx){
        for(AncestorObject * Object : MemoryMap[contextIdx].Objects){
            for(layerIndex = 0; layerIndex < Layers.size(); layerIndex++){
                if(Layers[layerIndex].getUniqueIndex() == Object->getLayerUniqueIndex()){
                    ObjectIndexes[contextIdx].push_back(AncestorIndex(layerIndex, Object - &Layers[layerIndex].Objects[0]));
                    break;
                }
            }
        }
    }
}
template <class Module>
ModuleIndex PointerRecalculator::getIndex(Module *& Instance, vector<LayerClass> & Layers,
    const InstrDescription & CurrentInstr
){
    if(Instance->getLayerUniqueIndex() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Module instance does not belong to any layer.\n";
    }
    if(Instance->getObjectUniqueIndex() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Module instance does not belong to any object.\n";
    }
    if(Instance->getLayerUniqueIndex() == 0 || Instance->getObjectUniqueIndex() == 0){
        return ModuleIndex(0, 0, 0);
    }
    unsigned layer, object;
    for(layer = 0; layer < Layers.size(); layer++){
        if(Layers[layer].getUniqueIndex() != Instance->getLayerUniqueIndex()){
            continue;
        }
        for(object = 0; object < Layers[layer].Objects.size(); object++){
            if(Layers[layer].Objects[object].getUniqueIndex() != Instance->getObjectUniqueIndex()){
                continue;
            }
            if constexpr (std::is_same<Module, SuperTextModule>::value){
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
            else if constexpr (std::is_same<Module, EventModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].EventContainer[0]);
            }
            else if constexpr (std::is_same<Module, VariableModule>::value){
                Layers[layer].Objects[object].hasInvalidatedMemory = true;
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].VariablesContainer[0]);
            }
            else if constexpr (std::is_same<Module, ScrollbarModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].ScrollbarContainer[0]);
            }
            else if constexpr (std::is_same<Module, PrimitivesModule>::value){
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].PrimitivesContainer[0]);
            }
            else if constexpr (std::is_same<Module, VectorModule>::value){
                Layers[layer].Objects[object].hasInvalidatedMemory = true;
                return ModuleIndex(layer, object, Instance - &Layers[layer].Objects[object].VectorContainer[0]);
            }
            break;
        }
        break;
    }
    return ModuleIndex(0, 0, 0);
}
template <class Module>
inline void PointerRecalculator::findIndexesInModule(vector<Module*> Instances,
    vector<LayerClass> & Layers, const InstrDescription & CurrentInstr, const unsigned & address
){
    for(Module * Instance : Instances){
        ModuleIndexes[address].push_back(getIndex(Instance, Layers, CurrentInstr));
    }
}
ModuleIndex PointerRecalculator::getIndex(vector<EventModule>::iterator & Instance,
    vector<LayerClass> & Layers, const InstrDescription & CurrentInstr
){
    if(Instance->getLayerUniqueIndex() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Event '" << Instance->getID()
            << "' does not belong to any layer.\n";
    }
    if(Instance->getObjectUniqueIndex() == 0){
        cerr << instructionError(CurrentInstr, __FUNCTION__) << "Event '" << Instance->getID()
            << "' does not belong to any object.\n";
    }
    if(Instance->getLayerUniqueIndex() == 0 || Instance->getObjectUniqueIndex() == 0){
        return ModuleIndex(0, 0, 0);
    }
    unsigned layer, object;
    for(layer = 0; layer < Layers.size(); layer++){
        if(Layers[layer].getUniqueIndex() != Instance->getLayerUniqueIndex()){
            continue;
        }
        for(object = 0; object < Layers[layer].Objects.size(); object++){
            if(Layers[layer].Objects[object].getUniqueIndex() == Instance->getObjectUniqueIndex()){
                return ModuleIndex(layer, object,
                    Instance - Layers[layer].Objects[object].EventContainer.begin()
                );
            }   
        }
        break;
    }
    return ModuleIndex(0, 0, 0);
}
void PointerRecalculator::findIndexesForModules(vector<LayerClass> & Layers,
    ObjectMemoryStruct & ObjectMemory, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack,
    SuperEditableTextModule *& ActiveEditableText, const InstrDescription & CurrentInstr
){
    currentEventUniqueIndex = eventIt->getUniqueIndex();

    //Invalidate all pointers that reference other modules' instances.
    for(LayerClass & Layer : Layers){
        for(AncestorObject & Object : Layer.Objects){
            for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
                Scrollbar.TrackImage = nullptr;
                Scrollbar.ThumbImage = nullptr;
            }
        }
    }

    startingEventIndex = getIndex(startingEventIt, Layers, CurrentInstr);
    eventIndex = getIndex(eventIt, Layers, CurrentInstr);
    for(EventStackStruct & Memory : MemoryStack){
        PastEvents.emplace_back(getIndex(Memory.Event, Layers, CurrentInstr));
    }
    for(size_t contextIdx = 0; contextIdx < ObjectMemory.MemoryMap.size(); ++contextIdx){
        ContextClass & Context = ObjectMemory.MemoryMap[contextIdx];
        switch(Context.type){
            case super_text_mod:
            case super_text_mod_vec:
                findIndexesInModule(Context.Modules.SuperTexts, Layers, CurrentInstr, contextIdx);
                break;
            case super_editable_text_mod:
            case super_editable_text_mod_vec:
                findIndexesInModule(Context.Modules.SuperEditableTexts, Layers, CurrentInstr, contextIdx);
                break;
            case image_mod:
            case image_mod_vec:
                findIndexesInModule(Context.Modules.Images, Layers, CurrentInstr, contextIdx);
                break;
            case movement_mod:
            case movement_mod_vec:
                findIndexesInModule(Context.Modules.Movements, Layers, CurrentInstr, contextIdx);
                break;
            case collision_mod:
            case collision_mod_vec:
                findIndexesInModule(Context.Modules.Collisions, Layers, CurrentInstr, contextIdx);
                break;
            case particles_mod:
            case particles_mod_vec:
                findIndexesInModule(Context.Modules.Particles, Layers, CurrentInstr, contextIdx);
                break;
            case event_mod:
            case event_mod_vec:
                findIndexesInModule(Context.Modules.Events, Layers, CurrentInstr, contextIdx);
                break;
            case variable_mod:
            case variable_mod_vec:
                findIndexesInModule(Context.Modules.Variables, Layers, CurrentInstr, contextIdx);
                break;
            case scrollbar_mod:
            case scrollbar_mod_vec:
                findIndexesInModule(Context.Modules.Scrollbars, Layers, CurrentInstr, contextIdx);
                break;
            case primitives_mod:
            case primitives_mod_vec:
                findIndexesInModule(Context.Modules.Primitives, Layers, CurrentInstr, contextIdx);
                break;
            case vector_mod:
            case vector_mod_vec:
                findIndexesInModule(Context.Modules.Vectors, Layers, CurrentInstr, contextIdx);
                break;
            default:
                break;
        }
    }
    didActiveEditableTextExist = false;
    if(ActiveEditableText != nullptr){
        ActiveEditableTextIndex = getIndex(ActiveEditableText, Layers, CurrentInstr);
        didActiveEditableTextExist = true;
    }
}
void PointerRecalculator::updatePointersToCameras(vector<Camera2D> &Cameras,
    ObjectMemoryStruct & ObjectMemory, Camera2D *& SelectedCamera, string processID,
    string & focusedProcessID, const InstrDescription & CurrentInstr
){
    for(const auto & IndexPair : CameraIndexes){
        ContextClass & CurrentContext = ObjectMemory.MemoryMap[IndexPair.first];
        for(size_t varCamIdx = 0; varCamIdx < IndexPair.second.size(); ++varCamIdx){
            unsigned realCamIdx = IndexPair.second[varCamIdx];
            if(realCamIdx >= Cameras.size()){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "CameraIndexes["
                    << IndexPair.first << "][" << realCamIdx
                    << "] goes out of scope of Cameras<" << Cameras.size() << ">.\n";
                CurrentContext.Cameras[varCamIdx] = nullptr;
                continue;
            }
            CurrentContext.Cameras[varCamIdx] = &Cameras[realCamIdx];
        }
    }
    if(SelectedCamera != nullptr){
        if(Cameras.size() <= selectedCameraIndex){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "selectedCameraIndex goes out of scope of Cameras.\n";
            unfocusCameras(Cameras, SelectedCamera, processID, focusedProcessID);
            return;
        }
        focusCamera(Cameras, SelectedCamera, processID, focusedProcessID, &Cameras[selectedCameraIndex]);
    }
}
void PointerRecalculator::updatePointersToLayers(vector<LayerClass> &Layers,
    ObjectMemoryStruct & ObjectMemory, LayerClass *& OwnerLayer,
    const InstrDescription & CurrentInstr
){
    for(const auto & IndexPair : LayerIndexes){
        ContextClass & CurrentContext = ObjectMemory.MemoryMap[IndexPair.first];
        for(size_t varLayerIdx = 0; varLayerIdx < IndexPair.second.size(); ++varLayerIdx){
            unsigned realLayerIdx = IndexPair.second[varLayerIdx];
            if(realLayerIdx >= Layers.size()){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "LayerIndexes[\""
                    << IndexPair.first << "\"][" << realLayerIdx << "]"
                    << " goes out of scope of Layers<" << Layers.size()  << ">.\n";
                CurrentContext.Layers[varLayerIdx] = nullptr;
                continue;
            }
            CurrentContext.Layers[varLayerIdx] = &Layers[realLayerIdx];
        }
    }
    if(OwnerLayer != nullptr){
        OwnerLayer = getOwnerLayer(Layers);
    }
}
void PointerRecalculator::updatePointersToObjects(vector<LayerClass> &Layers,
    ObjectMemoryStruct & ObjectMemory, AncestorObject *&Owner,
    vector<AncestorObject *> &TriggeredObjects, LayerClass *&SelectedLayer,
    AncestorObject *&SelectedObject, const InstrDescription & CurrentInstr
){
    unsigned i;
    for(const auto & IndexPair : ObjectIndexes){
        ContextClass & CurrentContext = ObjectMemory.MemoryMap[IndexPair.first];
        CurrentContext.Objects.clear();
        for(const AncestorIndex & Index : IndexPair.second){
            CurrentContext.Objects.push_back(Index.object(Layers));
        }
    }
    for(i = 0; i < TriggeredObjects.size(); i++){
        TriggeredObjects[i] = TriggeredObjectIndexes[i].object(Layers);
    }
    if(SelectedLayer != nullptr){
        if(Layers.size() <= selectedLayerIndex){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << ": selectedLayerIndex(" << selectedLayerIndex
                << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
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
void PointerRecalculator::updatePointersToObjectsInObjectMemory(
    vector<LayerClass> &Layers, MemoryMapType &MemoryMap, const InstrDescription & CurrentInstr
){
    for(const auto & IndexPair : ObjectIndexes){
        ContextClass & CurrentContext = MemoryMap[IndexPair.first];
        CurrentContext.Objects.clear();
        for(const AncestorIndex & Index : IndexPair.second){
            CurrentContext.Objects.push_back(Index.object(Layers));
        }
    }
}
void PointerRecalculator::updatePointersToModules(vector<LayerClass> & Layers,
    ObjectMemoryStruct & ObjectMemory, vector<EventModule>::iterator & startingEventIt,
    vector<EventModule>::iterator & eventIt, vector<EventStackStruct> & MemoryStack,
    SuperEditableTextModule *& ActiveEditableText, const InstrDescription & CurrentInstr
){
    startingEventIt = startingEventIndex.module(Layers);
    eventIt = eventIndex.module(Layers);

    if(currentEventUniqueIndex != eventIt->getUniqueIndex()){
        cerr << "CRITICAL ERROR: In " << __FUNCTION__ << ": Current event has been overwritten: "
            << currentEventUniqueIndex << " -> " << eventIt->getID() << "\n"
            << NEW_LINE_PADDING << "This may cause undefined behavior. Aborting.\n";
        abort();
    }

    for(unsigned memory = 0; memory < MemoryStack.size(); memory++){
        MemoryStack[memory].Event = PastEvents[memory].module(Layers);
    }
    ModuleIndex Index(0, 0, 0);
    AncestorObject * Object;
    for(const auto & IndexPair : ModuleIndexes){
        for(unsigned module = 0; module < IndexPair.second.size(); ++module){
            Index = IndexPair.second[module];
            Object = Index.object(Layers);

            if(Object == nullptr){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Object pointer is a null value.\n";
                continue;
            }

            ContextClass & CurrentContext = ObjectMemory.MemoryMap[IndexPair.first];

            switch(CurrentContext.type){
                case super_text_mod:
                case super_text_mod_vec:
                    CurrentContext.Modules.SuperTexts[module] = &(*Object).SuperTextContainer[Index.moduleIndex];
                    break;
                case super_editable_text_mod:
                case super_editable_text_mod_vec:
                    CurrentContext.Modules.SuperEditableTexts[module] = &(*Object).SuperEditableTextContainer[Index.moduleIndex];
                    break;
                case image_mod:
                case image_mod_vec:
                    CurrentContext.Modules.Images[module] = &(*Object).ImageContainer[Index.moduleIndex];
                    break;
                case movement_mod:
                case movement_mod_vec:
                    CurrentContext.Modules.Movements[module] = &(*Object).MovementContainer[Index.moduleIndex];
                    break;
                case collision_mod:
                case collision_mod_vec:
                    CurrentContext.Modules.Collisions[module] = &(*Object).CollisionContainer[Index.moduleIndex];
                    break;
                case particles_mod:
                case particles_mod_vec:
                    CurrentContext.Modules.Particles[module] = &(*Object).ParticlesContainer[Index.moduleIndex];
                    break;
                case event_mod:
                case event_mod_vec:
                    CurrentContext.Modules.Events[module] = &(*Object).EventContainer[Index.moduleIndex];
                    break;
                case variable_mod:
                case variable_mod_vec:
                    CurrentContext.Modules.Variables[module] = &(*Object).VariablesContainer[Index.moduleIndex];
                    break;
                case scrollbar_mod:
                case scrollbar_mod_vec:
                    CurrentContext.Modules.Scrollbars[module] = &(*Object).ScrollbarContainer[Index.moduleIndex];
                    break;
                case primitives_mod:
                case primitives_mod_vec:
                    CurrentContext.Modules.Primitives[module] = &(*Object).PrimitivesContainer[Index.moduleIndex];
                    break;
                case vector_mod:
                case vector_mod_vec:
                    CurrentContext.Modules.Vectors[module] = &(*Object).VectorContainer[Index.moduleIndex];
                    break;
                default:
                    break;
            }
        }
    }
    if(didActiveEditableTextExist){
        ActiveEditableText = ActiveEditableTextIndex.getModulePointer<SuperEditableTextModule>(Layers);
    }
}
LayerClass * PointerRecalculator::getOwnerLayer(vector<LayerClass> & Layers){
    if(Layers.size() <= eventOwnerLayerIndex){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": layerIndex(" << eventOwnerLayerIndex
            << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
        return nullptr;
    }
    return &Layers[eventOwnerLayerIndex];
}

void Triggers::clear(){
    active.clear();
    movingObjects.clear();
    stillObjects.clear();
}

void ObjectMemoryStruct::clear(){
    for(ContextClass & Context : MemoryMap){
        Context.clear();
    }
    MemoryMap.clear();
    DynamicMemory.clear();
    MemberVarsScope.clear();
    topAddress = 0;
    topFreeDynamicAddress = 0;
}

void EventCallStateStruct::clear(){
    isCurrentCallRecursive = false;
    programCounter = 0;
    decrementProgramCounter = false;
    conditionalStatus = 'n';
    breakFromCurrentLoop = false;
    goToEndOfIfStatement.clear();
}
