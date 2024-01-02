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

void ProcessClass::loadInitProcess(string layerID, string objectID, vec2i screenSize, string initFilePath){
    Layers.push_back(LayerClass(layerID, layersIDs, true, vec2d(0.0, 0.0), screenSize));
    Layers.back().Objects.push_back(AncestorObject(objectID, Layers.back().objectsIDs, Layers.back().getID()));
    if(initFilePath != ""){
        Layers.back().Objects.back().bindedScripts.push_back(EXE_PATH + initFilePath);
        Layers.back().Objects.back().translateAllScripts(true);
    }
    
    if(isLayersUniquenessViolated()){
        cout << "Error: In " << __FUNCTION__ << ": Layers id uniqueness has been violeted on the start of the process!\n";
    }

    for(LayerClass & Layer : Layers){
        for(AncestorObject & obj : Layer.Objects){
            obj.createVectorsOfIds();
        }
        if(Layer.isObjectsUniquenessViolated()){
            cout << "Error: In " << __FUNCTION__ << ": Layers id uniqueness has been violeted on the start of the process!\n";
        }
    }

    updateBaseOfTriggerableObjects();
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
ProcessClass::ProcessClass(string EXE_PATH_FROM_ENGINE, vec2i screenSize, string initFilePath, string newID, string newLayerID, string newObjectID, vector<string> &listOfIDs){
    setID(newID, listOfIDs);
    isActive = true;
    canInteractWithUser = true;
    
    EXE_PATH = EXE_PATH_FROM_ENGINE;
    firstIteration = true;
    rebooted = false;
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
        cout << "Error: RESERVATION_MULTIPLIER is lower than 1.\n";
    }

    SelectedCamera = nullptr;
    SelectedLayer = nullptr;
    SelectedObject = nullptr;

    isRendering = true;
    windowPos.set(0.0, 0.0);
    windowSize.set(screenSize.x, screenSize.y);
    minWindowSize.set(50, 50);
    std::fill_n(windowTint, 4, 1);

    WindowBuffer = al_create_bitmap(windowSize.x, windowSize.y);

    loadInitProcess(newLayerID, newObjectID, screenSize, initFilePath);
}
ProcessClass::~ProcessClass(){
    foregroundOfObjects.clear();
    BaseOfTriggerableObjects.clear();
    
    for(LayerClass & Layer : Layers){
        for(AncestorObject & obj : Layer.Objects){
            obj.clearContainers();
        }
        Layer.Objects.clear();
        Layer.clear();
    }
    Layers.clear();
    for(Camera2D & Camera : Cameras){
        Camera.clear();
    }
    Cameras.clear();
}
void ProcessClass::clear(){
    al_destroy_bitmap(WindowBuffer);
}
void ProcessClass::resizeWindow(vec2d newSize){
    windowSize.set(newSize);
    if(windowSize.x < minWindowSize.x){
        windowSize.x = minWindowSize.x;
    }
    if(windowSize.y < minWindowSize.y){
        windowSize.y = minWindowSize.y;
    }
    al_destroy_bitmap(WindowBuffer);
    WindowBuffer = al_create_bitmap(windowSize.x, windowSize.y);
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
            selectCamera(true, Engine.Mouse, Engine.pressedKeys, Engine.releasedKeys);
            updateCamerasPositions(Engine);
            moveObjects(Engine.pressedKeys, Engine.Mouse);
            moveParticles(Engine.pressedKeys, Engine.releasedKeys);
            if(SelectedCamera != nullptr && SelectedCamera->getIsActive()){
                SelectedCamera->update(Engine.pressedKeys);
            }
            
            triggerEve(Engine, Processes);

            if(Engine.closeProgram){
                return;
            }
            firstIteration = false;
            if(rebooted){
                rebooted = false;
                firstIteration = true;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            changeCursor(Engine.display, Engine.Mouse);
            moveSelectedObject(Engine.Mouse);
            dragScrollbars(Engine.Mouse);
            if(SelectedCamera != nullptr && SelectedCamera->getIsActive() && SelectedCamera->canMoveWithMouse
                && Engine.Mouse.firstPositionInRectangle(SelectedCamera->pos, SelectedCamera->size, 2, true, SelectedCamera)
            ){
                SelectedCamera->visionShift = Engine.Mouse.getZoomedPos(SelectedCamera) - dragCameraStaringPos;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            selectCamera(false, Engine.Mouse, Engine.pressedKeys, Engine.releasedKeys);
            selectObject(Engine.Mouse, Engine.BitmapContainer, Engine.FontContainer);
            detectStartPosOfDraggingCamera(Engine.display, Engine.Mouse);
            detectStartPosOfDraggingObjects(Engine.Mouse);
            startScrollbarDragging(Engine.Mouse);
            if(SelectedCamera != nullptr && SelectedCamera->getIsActive()){
                dragCameraStaringPos.set(Engine.Mouse.getZoomedPos(SelectedCamera)-SelectedCamera->visionShift);
            }
            break;
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

    if(Engine.Mouse.didMouseMove && SelectedCamera != nullptr && SelectedCamera->getIsActive()
        && SelectedCamera->canZoomWithMouse
        && Engine.Mouse.inRectangle(SelectedCamera->pos, SelectedCamera->size, true, SelectedCamera)
    ){
        Engine.Mouse.updateZoomForCamera(SelectedCamera);
    }
}
void ProcessClass::renderOnDisplay(EngineClass & Engine){
    if(!isActive || !isRendering){
        return;
    }
    
    timeToInterruptMovement = 20000;

    timeToInterruptParticles = 500;

    //al_set_target_bitmap(al_get_backbuffer(window));
    
    drawEverything(Engine);

    string updatedFpsLabel = "FPS: " + intToStr(Engine.fps.get());

    if(Layers[0].Objects.size() > 0){
        if(Layers[0].Objects[0].TextContainer.size() > 0){
            Layers[0].Objects[0].TextContainer[0].modifyContentAndResize(0, updatedFpsLabel, Engine.FontContainer);
        }
    }
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
    AncestorObject * tempObject = nullptr;
    if(firstIteration){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.BootTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    for(AncestorIndex & Index : BaseOfTriggerableObjects.InitTriggered){
        tempObject = Index.object(Layers);
        if(tempObject != nullptr && tempObject->getIsActive()){
            TriggeredObjects.push_back(&(*tempObject));
        }
    }
    BaseOfTriggerableObjects.InitTriggered.clear();
    for(AncestorIndex & Index : BaseOfTriggerableObjects.IterationTriggered){
        tempObject = Index.object(Layers);
        if(tempObject != nullptr && tempObject->getIsActive()){
            TriggeredObjects.push_back(&(*tempObject));
        }
    }
    if(Engine.secondHasPassed()){ 
        for(AncestorIndex & Index : BaseOfTriggerableObjects.TimeTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.firstPressedKeys.size() > 0){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.KeyPressedTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.pressedKeys.size() > 0){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.KeyPressingTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.releasedKeys.size() > 0){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.KeyReleasedTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.Mouse.didMouseMove){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.MouseMovedTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(!Engine.Mouse.didMouseMove){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.MouseNotMovedTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.Mouse.isFirstPressed()){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.MousePressedTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.Mouse.isPressed()){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.MousePressingTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.Mouse.isReleased()){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.MouseReleasedTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
            }
        }
    }
    if(Engine.displayResized){
        for(AncestorIndex & Index : BaseOfTriggerableObjects.ResizeTriggered){
            tempObject = Index.object(Layers);
            if(tempObject != nullptr && tempObject->getIsActive()){
                TriggeredObjects.push_back(&(*tempObject));
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
        }
    }

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
string ContextClass::getValue(){
    string buffer = "";
    if(type == ""){
        cout << "Error: In " << __FUNCTION__ << ": Context does not have type.\n";
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
                cout << "Error: In " << __FUNCTION__ << ": In the context \'" << ID << "\' the pointer to the variable has a nullptr value.\n";
                continue;
            }
            buffer += Variable->getStringUnsafe();
            buffer += ", ";
        }
        buffer += "]<";
        buffer += uIntToStr(Modules.Variables.size());
        buffer += ">";
    }
    else{
        buffer += "<";
        if(type == "camera"){
            buffer += uIntToStr(Cameras.size());
        }
        else if(type == "layer"){
            buffer += uIntToStr(Layers.size());
        }
        else if(type == "object"){
            buffer += uIntToStr(Objects.size());
        }
        else if(type == "text"){
            buffer += uIntToStr(Modules.Texts.size());
        }
        else if(type == "editable_text"){
            buffer += uIntToStr(Modules.EditableTexts.size());
        }
        else if(type == "image"){
            buffer += uIntToStr(Modules.Images.size());
        }
        else if(type == "movement"){
            buffer += uIntToStr(Modules.Movements.size());
        }
        else if(type == "collision"){
            buffer += uIntToStr(Modules.Collisions.size());
        }
        else if(type == "particles"){
            buffer += uIntToStr(Modules.Particles.size());
        }
        else if(type == "event"){
            buffer += uIntToStr(Modules.Events.size());
        }
        else if(type == "scrollbar"){
            buffer += uIntToStr(Modules.Scrollbars.size());
        }
        else if(type == "primitives"){
            buffer += uIntToStr(Modules.Primitives.size());
        }
        
        buffer += ">";
    }
    if(readOnly){
        buffer += "(R)";
    }
    return buffer;
}
ContextClass::ContextClass(){
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
void ContextClass::addModule(ImageModule * Module){
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
void ContextClass::setFirstModule(ImageModule * Module){
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
    else{
        cout << "Error: In " << __FUNCTION__ << ": Type \'" << NewContext.type << "\' does not exist.\n";
        return false;
    }
    return true;
}

void ProcessClass::aggregateCameras(OperaClass &Operation, ContextClass &NewContext, vector<Camera2D*> AggregatedCameras, const EngineClass & Engine, vector<ContextClass> &EventContext){
    Camera2D * Camera = nullptr;
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == "last"){
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
            if(Operation.instruction != "last"){
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
        if(Operation.Location.cameraID != "" || (Operation.instruction == "first" && (NewContext.BasePointers.size() == 1 || NewContext.Cameras.size() == 1))){
            break;
        }
        i++;
    }

    delete TempObject;
    delete TempLayer;
    if(Operation.instruction == "random"){
        if(NewContext.type == "camera"){
            chooseRandomEntity(NewContext.Cameras);
        }
        else{
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregateLayers(OperaClass & Operation, ContextClass & NewContext, vector <LayerClass*> AggregatedLayers, const EngineClass & Engine, vector<ContextClass> &EventContext){
    LayerClass * Layer = nullptr;
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == "last"){
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
            if(Operation.instruction != "last"){
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
        if(Operation.Location.layerID != "" || (Operation.instruction == "first" && (NewContext.BasePointers.size() == 1 || NewContext.Layers.size() == 1))){
            break;
        }
        i++;
    }

    delete TempObject;
    if(Operation.instruction == "random"){
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
void ProcessClass::aggregateObjects(OperaClass & Operation, ContextClass & NewContext, vector <AncestorObject*> AggregatedObjects, const EngineClass & Engine, vector<ContextClass> &EventContext){
    if(AggregatedObjects.size() == 0){
        return;
    }
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == "last"){
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
            if(Operation.instruction != "last"){
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
        if(Operation.instruction == "first" && (NewContext.BasePointers.size() == 1 || NewContext.Objects.size() == 1)){
            break;
        }
    }

    if(Operation.instruction == "random"){
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
            if(Operation.instruction != "last"){
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
        if(Operation.instruction == "first" && NewContext.Modules.hasInstanceOfAnyModule()){
            return;
        }
    }
}
template<class ModuleClass>
void ProcessClass::findContextInModule(string type, string attribute, ContextClass & NewContext, ModuleClass * Module){
    if(Module == nullptr){
        return;
    }
    if(attribute == "" || isStringInGroup(attribute, 10, "text", "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", "primitives")){
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
            cout << "Error: In " << __FUNCTION__ << ": Context not found.\n";
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
                cout << "Error: In " << __FUNCTION__ << ": No type.\n";
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
void ProcessClass::aggregateModules(OperaClass & Operation, ContextClass & NewContext, ContextClass * OldContext, vector<ContextClass> &EventContext, const EngineClass & Engine){
    ModulesPointers * AggregatedModules = &OldContext->Modules;
    if(Operation.ConditionalChain.size() == 0 && Operation.instruction == "last"){
        if(OldContext->type == "text" && AggregatedModules->Texts.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->Texts, Operation.Location.moduleID));
        }
        else if(OldContext->type == "editable_text" && AggregatedModules->EditableTexts.size() > 0){
            findContextInModule(OldContext->type, Operation.Location.attribute, NewContext, findLastModule(AggregatedModules->EditableTexts, Operation.Location.moduleID));
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
        return;
    }

    AncestorObject * EmptyObject = new AncestorObject();
    if(OldContext->type == "text"){
        aggregateModuleContextFromVectors(AggregatedModules->Texts, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
    }
    else if(OldContext->type == "editable_text"){
        aggregateModuleContextFromVectors(AggregatedModules->EditableTexts, OldContext->type, Operation, NewContext, EmptyObject, Engine, EventContext);
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
    else{
        cout << "Error: In " << __FUNCTION__ << ": Module type \'" << OldContext->type << "\' does not exist.\n";
        delete EmptyObject;
        return;
    }
    delete EmptyObject;

    if(Operation.instruction == "random"){
        if(!chooseRandomModule(NewContext)){
            NewContext.leaveOneRandomBasePointer();
        }
    }
}
void ProcessClass::aggregatePointers(string instruction, ContextClass & NewContext, vector <BasePointersStruct> & AggregatedPointers){
    if(instruction == "first"){
        NewContext.BasePointers.push_back(AggregatedPointers.front());
    }
    else if(instruction == "last"){
        NewContext.BasePointers.push_back(AggregatedPointers.back());
    }
    else if(instruction == "all"){
        NewContext.BasePointers.insert(NewContext.BasePointers.end(), AggregatedPointers.begin(), AggregatedPointers.end());
    }
    else if(instruction == "random" && AggregatedPointers.size() > 0){
        NewContext.BasePointers.push_back(AggregatedPointers[rand() % AggregatedPointers.size()]);
    }
    if(NewContext.BasePointers.size() > 0){
        NewContext.type = "pointer";
    }
}
void ProcessClass::aggregateVariables(string instruction, ContextClass & NewContext, vector <VariableModule> & AggregatedVariables){
    if(instruction == "first"){
        NewContext.Variables.push_back(AggregatedVariables.front());
    }
    else if(instruction == "last"){
        NewContext.Variables.push_back(AggregatedVariables.back());
    }
    else if(instruction == "all"){
        NewContext.Variables.insert(NewContext.Variables.end(), AggregatedVariables.begin(), AggregatedVariables.end());
    }
    else if(instruction == "random" && AggregatedVariables.size() > 0){
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
            cout << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << Camera->ID << "\' address was denied.\n";
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
    else if(attribute == "is_using_cursor_position_to_move"){
        NewContext.addBasePointer(&Camera->canInteractWithMouse);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
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
            cout << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << Layer->getID() << "\' address was denied.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": Attribute \'" << Location.attribute << "\' does not exist.\n";
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
            cout << "Error: In " << __FUNCTION__ << ": Context not found.\n";
        }
    }
}
bool ProcessClass::findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject){
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
        cout << "Error: In " << __FUNCTION__ << ": Layer \'" << Location.layerID << "\' does not exist.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": Object does not exist.\n";
        return false;
    }

    return true;
}
template <class T>
void getPointersFromVector(vector <T*> & Left, vector<T> & Right){
    for(T & Instance : Right){
        Left.push_back(&Instance);
    }
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
void extractPointersFromModules(ModulesPointers &ContextModules, AncestorObject *Object, string moduleType){
    if(moduleType == "text"){
        getPointersFromVector(ContextModules.Texts, Object->TextContainer);
    }
    else if(moduleType == "editable_text"){
        getPointersFromVector(ContextModules.EditableTexts, Object->EditableTextContainer);
    }
    else if(moduleType == "image"){
        getPointersFromVector(ContextModules.Images, Object->ImageContainer);
    }
    else if(moduleType == "movement"){
        getPointersFromVector(ContextModules.Movements, Object->MovementContainer);
    }
    else if(moduleType == "collision"){
        getPointersFromVector(ContextModules.Collisions, Object->CollisionContainer);
    }
    else if(moduleType == "particles"){
        getPointersFromVector(ContextModules.Particles, Object->ParticlesContainer);
    }
    else if(moduleType == "event"){
        getPointersFromVector(ContextModules.Events, Object->EveContainer);
    }
    else if(moduleType == "variable"){
        getPointersFromVector(ContextModules.Variables, Object->VariablesContainer);
    }
    else if(moduleType == "scrollbar"){
        getPointersFromVector(ContextModules.Scrollbars, Object->ScrollbarContainer);
    }
    else if(moduleType == "primitives"){
        getPointersFromVector(ContextModules.Primitives, Object->PrimitivesContainer);
    }
}
void ProcessClass::aggregateCamerasAndLayersById(ValueLocation & Location, ContextClass & NewContext, AncestorObject * Owner, LayerClass * OwnerLayer){
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
        cout << "Error: In " << __FUNCTION__ << ": No valid source provided.\n";
    }
}
void ProcessClass::aggregateModulesById(string moduleType, string moduleID, string attribute, ContextClass & NewContext, ModulesPointers & AggregatedModules){
    if(moduleType == "text" && AggregatedModules.Texts.size() > 0){
        getContextFromModuleVectorById<TextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Texts);
    }
    else if(moduleType == "editable_text" && AggregatedModules.EditableTexts.size() > 0){
        getContextFromModuleVectorById<EditableTextModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.EditableTexts);
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
    else if(moduleType == "scrollbar" && AggregatedModules.Primitives.size() > 0){
        getContextFromModuleVectorById<PrimitivesModule>(moduleType, moduleID, attribute, NewContext, AggregatedModules.Primitives);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": There are no instances of the \'" << moduleType << "\' module.\n";
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
    else if(isStringInGroup(OldContext->type, 10, "text", "editable_text" , "image", "movement", "collision", "particles", "event", "variable", "scrollbar", "primitives")){
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
        cout << "Error: In " << __FUNCTION__ << ": No valid source provided.\n";
    }
}
ContextClass * getContextByID(vector<ContextClass> & AllContexts, string contextID, bool warning){
    for(ContextClass & Context : AllContexts){
        if(Context.ID == contextID){
            return &Context;
        }
    }
    if(warning){
        cout << "Warning: In " << __FUNCTION__ << ": Dynamic variable with id \'" << contextID << "\' does not exist.\n";
    }
    return nullptr;
}
bool ProcessClass::getPairOfContexts(ContextClass *& LeftOperand, ContextClass *& RightOperand, vector<ContextClass> & AllContexts, vector<string> contextIDs){
    if(AllContexts.size() < 2){
        cout << "Error: In " << __FUNCTION__ << ": There are no contexts to choose from.\n";
    }
    if(contextIDs.size() == 0){
        LeftOperand = &AllContexts[AllContexts.size()-2];
        RightOperand = &AllContexts.back();
    }
    else if(contextIDs.size() == 1 && contextIDs[0] != ""){
        LeftOperand = getContextByID(AllContexts, contextIDs[0], true);
        RightOperand = &AllContexts.back();
    }
    else if(contextIDs.size() > 1 && contextIDs[0] == "" && contextIDs[1] != ""){
        LeftOperand = &AllContexts.back();
        RightOperand = getContextByID(AllContexts, contextIDs[1], true);
    }
    else if(contextIDs.size() > 1 && contextIDs[0] != "" && contextIDs[1] != ""){
        LeftOperand = getContextByID(AllContexts, contextIDs[0], true);
        RightOperand = getContextByID(AllContexts, contextIDs[1], true);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Incorrect input.\n";
        return false;
    }

    if(LeftOperand == nullptr){
        if(contextIDs.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": Left/First operand does not exist.\n";
        }
        return false;
    }
    if(RightOperand == nullptr){
        if(contextIDs.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": Right/Second operand does not exist.\n";
        }
        return false;
    }

    return true;
}
bool ProcessClass::getOneContext(ContextClass *& SelectedContext, vector<ContextClass> & AllContexts, vector<string> contextIDs){
    if(AllContexts.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": There are no contexts to choose from.\n";
    }

    if(contextIDs.size() == 0){
        SelectedContext = &AllContexts[AllContexts.size()-1];
    }
    else{
        SelectedContext = getContextByID(AllContexts, contextIDs[0], true);
    }

    if(SelectedContext == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Left operand does not exist.\n";
        return false;
    }
    
    return true;
}
bool ProcessClass::getAllSelectedContexts(vector<ContextClass*> & SelectedContexts, vector<ContextClass> & AllContexts, const vector<string> & contextIDs){
    if(contextIDs.size() == 0){
        return false;
    }
    
    if(AllContexts.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": There are no contexts to choose from.\n";
    }

    for(const string & ID : contextIDs){
        SelectedContexts.push_back(getContextByID(AllContexts, ID, true));
        if(SelectedContexts.back() == nullptr){
            SelectedContexts.pop_back();
        }
    }

    if(SelectedContexts.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": No contexts found.\n";
        return false;
    }
    
    return true;
}
bool ProcessClass::checkDefaultCondition(VariableModule * Left, VariableModule * Right){
    return Left->isConditionMet("==", Right);
}
bool ProcessClass::checkDefaultCondition(BasePointersStruct * Left, BasePointersStruct * Right){
    return Left->areEqual(Right);
}
template<class Entity>
void ProcessClass::executeOperationsOnSets(string instruction, vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand){
    if(instruction == "sum"){
        NewContext = LeftOperand;
        NewContext.insert(NewContext.end(), RightOperand.begin(), RightOperand.end());
    }
    else if(instruction == "intersection"){
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
    else if(instruction == "difference"){
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
void ProcessClass::executeOperationsOnSets(string instruction, vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand){
    if(instruction == "sum"){
        NewContext = LeftOperand;
        NewContext.insert(NewContext.end(), RightOperand.begin(), RightOperand.end());
    }
    else if(instruction == "intersection"){
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
    else if(instruction == "difference"){
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
    EventContext.back().setID(EventContext, newID, printOutInstructions);
}
void ProcessClass::aggregateTwoSets(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass NewContext;
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;

    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        return;
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue() << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue() << "\n";
    }

    if(LeftOperand->type != RightOperand->type){
        cout << "Error: In " << __FUNCTION__ << ": Right operand has a different type than the left operand.\n";
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
    else{
        cout << "Error: In " << __FUNCTION__ << ": type \'" << LeftOperand->type << "\' does not exist.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
        return;
    }

    addNewContext(EventContext, NewContext, LeftOperand->type, Operation.newContextID);
}
void ProcessClass::aggregateEntities(OperaClass & Operation, vector<ContextClass> & EventContext, const EngineClass & Engine){
    ContextClass NewContext;

    if(Operation.Location.source == "layer" || Operation.Location.source == "camera"){
        if(printOutInstructions){
            cout << Operation.instruction << " ";
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
            cout << "Error: In " << __FUNCTION__ << ": Source type \'" << Operation.Location.source << "\' does not exist in this context.\n";
        }
    }
    else if(Operation.dynamicIDs.size() > 0){
        for(const string & contextID : Operation.dynamicIDs){
            ContextClass * SourceContext = getContextByID(EventContext, contextID, true);
            if(SourceContext == nullptr){
                cout << "Error: In " << __FUNCTION__ << ": Context \'" << contextID << "\' does not exist.\n";
                continue;
            }
            if(printOutInstructions){
                cout << Operation.instruction << " ";
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
            else if(isStringInGroup(SourceContext->type, 10, "text", "editable_text" , "image", "movement", "collision", "particles", "event", "variable", "scrollbar", "primitives")){
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
                cout << "Error: In " << __FUNCTION__ << ": Context type \'" << SourceContext->type << "\' does not exist.\n";
            }
        }
    }

    if(NewContext.type != ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void moveRightToLeft(string instruction, ContextClass * LeftOperand, ContextClass * RightOperand){
    if(LeftOperand->type != "pointer" && LeftOperand->type != "value" && LeftOperand->type != "variable"){
        cout << "Error: In " << __FUNCTION__ << ": In \'" << instruction << "\' instruction: Left operand has an invalid type: \'" << LeftOperand->type << "\'.\n";
        return;
    }
    else if(RightOperand->type != "pointer" && RightOperand->type != "value" && RightOperand->type != "variable"){
        cout << "Error: In " << __FUNCTION__ << ": In \'" << instruction << "\' instruction: Right operand has an invalid type: \'" << RightOperand->type << "\'.\n";
        return;
    }

    if(LeftOperand->readOnly){
        cout << "Error: In " << __FUNCTION__ << ": In \'" << instruction << "\' instruction: Left operand is read-only. Instruction \'" << instruction << "\' failed.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": In \'" << instruction << "\' instruction: Failed to fetch a variable.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": In \'" << instruction << "\' instruction: You cannot move a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
    }
}
void ProcessClass::moveOrRename(vector<ContextClass> & EventContext, ContextClass * NewContext, string newContextID){
    ContextClass * OldContext = getContextByID(EventContext, newContextID, false);
    if(OldContext != nullptr){
        moveRightToLeft("=", OldContext, NewContext);
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

    if(printOutInstructions){
        cout << Operation.instruction << " " << NewContext.getValue() << " " << Operation.newContextID << "\n";
    }

    NewContext.type = "value";
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::aggregateOnlyById(vector<ContextClass> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner){
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
            LeftOperand = getContextByID(EventContext, dynamicID, true);
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
            cout << "found: " << NewContext.getValue() << "\n";
        }
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ContextClass::setID(vector<ContextClass> &EventContext, string newID, const bool & printOutInstructions){
    if(newID == ""){
        return;
    }
    
    ContextClass * OldVariable = getContextByID(EventContext, newID, false);

    if(OldVariable != nullptr){
        if(printOutInstructions){
            cout << "delete "<< OldVariable->ID << ":" << OldVariable->type << ":" << OldVariable->getValue() << "\n";
            cout << "let " << newID << " " << ID << ":" << type << ":" << getValue() << "\n";
        }
        *OldVariable = *this;
        OldVariable->ID = newID;
        EventContext.pop_back();
    }
    else{
        if(printOutInstructions){
            cout << "let " << newID << " " << ID << ":" << type << ":" << getValue() << "\n";
        }
        ID = newID;
    }
}
void ProcessClass::nameVariable(vector<ContextClass> & EventContext, OperaClass & Operation){
    ContextClass * LeftOperand = nullptr;

    if(!getOneContext(LeftOperand, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": Variable \'" << Operation.newContextID << "\' cannot be created.\n";
        return;
    }

    LeftOperand->setID(EventContext, Operation.newContextID, printOutInstructions);
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
                    cout << "Error: In " << __FUNCTION__ << ": Module with a reserved ID \'" << RightOperand[j]->getID() << "\' cannot be cloned.\n";
                    return;
                }
                if(isStringInVector(reservedIDs, LeftOperand[i]->getID())){
                    cout << "Error: In " << __FUNCTION__ << ": Module with a reserved ID \'" << LeftOperand[i]->getID() << "\' cannot be changed.\n";
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

    if(Operation.instruction == "++" || Operation.instruction == "--"){
        if(!getOneContext(LeftOperand, EventContext, Operation.dynamicIDs)){
            return;
        }
        if(LeftOperand->type != "pointer" && LeftOperand->type != "value" && LeftOperand->type != "variable"){
            cout << "Error: In " << __FUNCTION__ << ": Left operand has an invalid type: \'" << LeftOperand->type << "\'.\n";
        }
        RightOperand = LeftOperand; //Cloning the left operand allows to reuse this function for incrementing and decrementing.
    }
    else if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        return;
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue()
            << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue() << "\n";
    }
    
    moveRightToLeft(Operation.instruction, LeftOperand, RightOperand);
}
void ProcessClass::cloneEntities(vector<string> dynamicIDs, bool changeOldID, vector<ContextClass> &EventContext, vector<LayerClass> &Layers){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;
    
    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, dynamicIDs)){
        return;
    }

    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(printOutInstructions){
        cout << "clone " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue() << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue() << "\n";
    }

    if(LeftOperand->readOnly){
        cout << "Error: In " << __FUNCTION__ << ": Left operand is read-only. Instruction \'clone\' failed.\n";
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
                        cout << "Error: In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
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
                        cout << "Error: In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
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
            cout << "Error: In " << __FUNCTION__ << ": You cannot assign a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
        }
    }
    else{
        if(LeftOperand->type == "pointer"){
            if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
                return;
            }
            for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
                LeftOperand->BasePointers[i].move(RightOperand->BasePointers[j], "clone");
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
        }
        else if(LeftOperand->type == "text"){
            cloneRightToLeft(LeftOperand->Modules.Texts, RightOperand->Modules.Texts, Layers, changeOldID);
        }
        else if(LeftOperand->type == "editable_text"){
            cloneRightToLeft(LeftOperand->Modules.EditableTexts, RightOperand->Modules.EditableTexts, Layers, changeOldID);
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
    }
}
void ProcessClass::executeArithmetics(OperaClass & Operation, vector<ContextClass> &EventContext){
    ContextClass * LeftOperand = nullptr;
    ContextClass * RightOperand = nullptr;

    if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)){
        return;
    }

    ContextClass NewContext;
    BaseVariableStruct result;
    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(printOutInstructions){
        cout << Operation.instruction << " " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue() << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue() << "\n";
    }

    if(LeftOperand->type == "pointer" && RightOperand->type == "pointer"){
        if(!checkForVectorSize(LeftOperand->BasePointers.size(), RightOperand->BasePointers.size(), sameSize, __FUNCTION__)){
            return;
        }
        for(; i < LeftOperand->BasePointers.size(); i++, j+=sameSize){
            result = LeftOperand->BasePointers[i].executeArithmetics(RightOperand->BasePointers[j], Operation.instruction);
            if(result.type == ""){
                cout << "Error: In " << __FUNCTION__ << ": Failed to execute arithmetic equation.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                    if(!sameSize){
                        return;
                    }
                    continue;
                }
            }
            
            result = LeftOperand->BasePointers[i].executeArithmetics(RightVariable, Operation.instruction);

            if(result.type == ""){
                cout << "Error: In " << __FUNCTION__ << ": Failed to execute arithmetic equation.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
                    if(!sameSize){
                        return;
                    }
                    continue;
                }
            }
            
            result = LeftOperand->BasePointers[i].executeArithmetics(RightVariable, Operation.instruction);

            if(result.type == ""){
                cout << "Error: In " << __FUNCTION__ << ": Failed to execute arithmetic equation.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": You cannot execute arithmetic equation on variables of types: \'" << RightOperand->type << "\' and \'" << LeftOperand->type << "\'.\n";
    }

    NewContext.type = "value";
    moveOrRename(EventContext, &NewContext, Operation.newContextID);
}
void ProcessClass::createLiteral(vector<ContextClass> &EventContext, const OperaClass & Operation){
    ContextClass NewContext;
    if(printOutInstructions){
        cout << Operation.instruction << " {";
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
    else if(!getPairOfContexts(LeftOperand, RightOperand, EventContext, Operation.dynamicIDs)
        || (LeftOperand->type != "pointer" && LeftOperand->type != "value")
        || (RightOperand->type != "pointer" && RightOperand->type != "value")
    ){
        return;
    }

    ContextClass NewContext;
    VariableModule Result;
    unsigned i = 0, j = 0;
    bool sameSize = false;

    if(printOutInstructions){
        cout << Operation.instruction << " " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue() << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue() << "\n";
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
        cout << "Error: In " << __FUNCTION__ << ": You cannot move a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << NewContext.getValue() << " " << Operation.newContextID << "\n";
    }

    NewContext.type = "value";
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
        return;
    }

    bool result = false;
    unsigned i = 0, j = 0;

    if(printOutInstructions){
        cout << "in " << LeftOperand->ID << ":" << LeftOperand->type << ":" << LeftOperand->getValue() << " " << RightOperand->ID << ":" << RightOperand->type << ":" << RightOperand->getValue() << "\n";
    }

    if(LeftOperand->type != RightOperand->type){
        if(LeftOperand->type == "pointer" && RightOperand->type == "value"){
            BaseVariableStruct RightVariable;
            for(i = 0; i < LeftOperand->BasePointers.size(); i++){
                for(j = 0; j < RightOperand->Variables.size(); j++){
                    RightVariable = RightOperand->Variables[j].getVariableStruct();
                    if(RightVariable.type == ""){
                        cout << "Error: In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
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
                        cout << "Error: In " << __FUNCTION__ << ": Failed to fetch a variable.\n";
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
                    if(LeftOperand->Variables[i].isConditionMet("==", RightOperand->BasePointers[j])){
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
                    if(LeftOperand->Modules.Variables[i]->isConditionMet("==", RightOperand->BasePointers[j])){
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
            cout << "Error: In " << __FUNCTION__ << ": You cannot assign a value of \'" << RightOperand->type << "\' type to a variable of \'" << LeftOperand->type << "\' type.\n";
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
                    if(LeftOperand->Variables[i].isConditionMet("==", &RightOperand->Variables[j])){
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
        else{
            cout << "Error: In " << __FUNCTION__ << ": \'" << LeftOperand->type << "\' type does not exist.\n";
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
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, double reservationMultiplier
){
    if(Container.size() + newVectorSize > Container.capacity()){
        PointerRecalculator Recalculator;
        Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
        Container.reserve((Container.size() + newVectorSize) * reservationMultiplier);
        Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
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
        cout << "Error: In " << __FUNCTION__ << ": \'new\' instruction requires destination type (the first string literal of the operation).\n";
        return false;
    }
    
    ContextClass * SizeContext = nullptr;
    ContextClass * IdContext = nullptr;
    if(Literals.size() > 1 && Literals.back().getType() == 'i'){
        newVectorSize = Literals.back().getInt();
        if(dynamicIDs.size() > 0){
            if(!getOneContext(IdContext, EventContext, dynamicIDs)){
                cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
                return false;
            }
        }
    }
    else if(dynamicIDs.size() == 1){
        if(!getOneContext(SizeContext, EventContext, dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return false;
        }
    }
    else if(dynamicIDs.size() >= 2){
        if(!getPairOfContexts(SizeContext, IdContext, EventContext, dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return false;
        }
    }
    
    if(SizeContext != nullptr){
        if(SizeContext->type == "value" && SizeContext->Variables.size() == 1){
            newVectorSize = SizeContext->Variables.back().getIntUnsafe();
        }
        else if(SizeContext->type == "pointer" && SizeContext->BasePointers.size() == 1){
            newVectorSize = SizeContext->BasePointers.back().getInt();
        }
        else if(SizeContext->type == "variable" && SizeContext->Modules.Variables.size() == 1){
            newVectorSize = SizeContext->Modules.Variables.back()->getIntUnsafe();
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Context type is invalid for the 'new' instruction.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": \'new\' instruction requires destination type (the first string literal of the operation).\n";
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
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return false;
        }
        if(Context->type == "layer" && Operation.Location.source == "object"){
            if(Context->Layers.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": Context has no layers.\n";
                return false;
            }
            CurrentLayer = Context->Layers.back();
            layerID = CurrentLayer->getID();
            return true;
        }
        else if(Context->type == "object" && Operation.Location.source != "object"){
            if(Context->Objects.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": Context has no objects.\n";
                return false;
            }
            CurrentObject = Context->Objects.back();
            objectID = CurrentObject->getID();
            layerID = CurrentObject->getLayerID();
            return true;
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": You cannot create an entity of \'" << Operation.Location.source << "\' type in a context of \'" << Context->type << "\' type.\n";
            return false;
        }
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": wrong destination type in the \'new\' instruction.\n";
        return false;
    }

    if(CurrentLayer == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Layer \'" << Operation.Location.layerID << "\' does not exist.\n";
        return false;
    }

    if(CurrentObject == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Object \'" << Operation.Location.objectID << "\' does not exist.\n";
        return false;
    }

    return true;
}
void ProcessClass::createNewEntities(OperaClass & Operation, vector<ContextClass> & EventContext,
    AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
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
        cout << "Warning: In " << __FUNCTION__ << ": No new entity will be created - the count of new entities is zero.\n";
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
            Recalculator.updatePointersToCameras(Cameras, EventContext, SelectedCamera);
        }
        for(unsigned i = 0; i < newVectorSize; i++){
            if(i < newIDs.size()){
                ID = newIDs[i];
            }
            Cameras.push_back(Camera2D(ID, camerasIDs));
            NewContext.Cameras.push_back(&Cameras.back());
            camerasOrder.push_back(camerasOrder.size());
        }
    }
    else if(Operation.Location.source == "layer"){
        if(Layers.size() + newVectorSize > Layers.capacity()){
            PointerRecalculator Recalculator;
            Recalculator.findIndexesForLayers(Layers, EventContext);
            Recalculator.findIndexesForObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject);
            Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
            Layers.reserve((Layers.size() + newVectorSize) * reservationMultiplier);
            Recalculator.updatePointersToLayers(Layers, EventContext);
            Recalculator.updatePointersToObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject);
            Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
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
            Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
            CurrentLayer->Objects.reserve((CurrentLayer->Objects.size() + newVectorSize) * reservationMultiplier);
            Recalculator.updatePointersToObjects(Layers, EventContext, Owner, TriggeredObjects, SelectedLayer, SelectedObject);
            Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
        }
        for(unsigned i = 0; i < newVectorSize; i++){
            if(i < newIDs.size()){
                ID = newIDs[i];
            }
            CurrentLayer->Objects.push_back(AncestorObject(ID, CurrentLayer->objectsIDs, layerID));
            NewContext.Objects.push_back(&CurrentLayer->Objects.back());
        }
    }
    else if(Operation.Location.source == "text"){
        createNewModule(CurrentObject->TextContainer, CurrentObject->textContainerIDs, NewContext.Modules.Texts,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "editable_text"){
        createNewModule(CurrentObject->EditableTextContainer, CurrentObject->editableTextContainerIDs, NewContext.Modules.EditableTexts,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "image"){
        createNewModule(CurrentObject->ImageContainer, CurrentObject->imageContainerIDs, NewContext.Modules.Images,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "movement"){
        createNewModule(CurrentObject->MovementContainer, CurrentObject->movementContainerIDs, NewContext.Modules.Movements,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "collision"){
        createNewModule(CurrentObject->CollisionContainer, CurrentObject->collisionContainerIDs, NewContext.Modules.Collisions,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "particles"){
        createNewModule(CurrentObject->ParticlesContainer, CurrentObject->particlesContainerIDs, NewContext.Modules.Particles,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "event"){
        createNewModule(CurrentObject->EveContainer, CurrentObject->eveContainerIDs, NewContext.Modules.Events,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "variable"){
        createNewModule(CurrentObject->VariablesContainer, CurrentObject->variablesContainerIDs, NewContext.Modules.Variables,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "scrollbar"){
        createNewModule(CurrentObject->ScrollbarContainer, CurrentObject->scrollbarContainerIDs, NewContext.Modules.Scrollbars,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else if(Operation.Location.source == "primitives"){
        createNewModule(CurrentObject->PrimitivesContainer, CurrentObject->primitivesContainerIDs, NewContext.Modules.Primitives,
            newVectorSize, newIDs, layerID, objectID, Layers, EventContext, StartingEvent, Event, MemoryStack, reservationMultiplier
        );
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Entity type \'" << Operation.instruction << "\' does not exist.\n";
    }

    if(NewContext.type != ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
        wasNewExecuted = true;
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' failed.\n";
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
    AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects
){
    ContextClass * DeletedContext = nullptr;
    if(!getOneContext(DeletedContext, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
        return;
    }

    if(DeletedContext->size() == 0 || DeletedContext->type == ""){
        cout << "Warning: In " << __FUNCTION__ << ": Nothing to delete from selected context.\n";
        return;
    }

    if(printOutInstructions){
        cout << "delete " << DeletedContext->getValue() << "\n";
    }

    wasDeleteExecuted = true;

    if(DeletedContext->type == "camera"){
        for(Camera2D * Camera : DeletedContext->Cameras){
            if(Camera != nullptr){
                Camera->deleteLater();
            }
        }
        if(SelectedCamera != nullptr && SelectedCamera->getIsDeleted()){
            SelectedCamera = nullptr;
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

    if(DeletedContext->type == "layer" || DeletedContext->type == "object"){
        if(Owner != nullptr && Owner->getIsDeleted()){
            Owner = nullptr;
        }
        if(SelectedObject != nullptr && SelectedObject->getIsDeleted()){
            SelectedObject = nullptr;
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

    if(isStringInGroup(DeletedContext->type, 12, "layer", "object", "text", "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", "primitives")){
        for(ContextClass & Context : EventContext){
            if(Context.type == "text"){
                clearDeletedPointersFromVector(Context.Modules.Texts);
            }
            else if(Context.type == "editable_text"){
                clearDeletedPointersFromVector(Context.Modules.EditableTexts);
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
void findInstanceInVectorByIndex(vector<unsigned> indexes, vector<Entity> & Aggregated, string type, vector<Entity> & NewVector, string & newType){
    if(type != "pointer" && type != "value"){
        cout << "Error: In " << __FUNCTION__ << ": This function allows only entities of \'pointer\' and \'value\' types.\n";
        return;
    }
    for(unsigned index : indexes){
        if(index < Aggregated.size()){
            NewVector.push_back(Aggregated[index]);
            newType = type;
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Index is out of vector's scope.\n";
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
            cout << "Warning: In " << __FUNCTION__ << ": Each literal should be of integer type.\n";
        }
    }

    if(dynamicIDs.size() == 0){
        return;
    }

    vector<ContextClass*> IndexContexts;

    if(getAllSelectedContexts(IndexContexts, EventContext, dynamicIDs)){
        for(ContextClass * Index : IndexContexts){
            if(Index->type == "value"){
                for(const VariableModule & Variable : Index->Variables){
                    if(Variable.getType() == 'i'){
                        indexes.push_back(Variable.getInt());
                    }
                    else{
                        indexes.push_back(0);
                        cout << "Warning: In " << __FUNCTION__ << ": Each variable should be of integer type.\n";
                    }
                }
            }
            else if(Index->type == "pointer"){
                for(const BasePointersStruct & BasePointer : Index->BasePointers){
                    if(BasePointer.isInteger()){
                        indexes.push_back(BasePointer.getInt());
                    }
                    else{
                        indexes.push_back(0);
                        cout << "Warning: In " << __FUNCTION__ << ": Each pointer should be of integer type.\n";
                    }
                }
            }
            else if(Index->type == "variable"){
                for(const VariableModule * Variable : Index->Modules.Variables){
                    if(Variable->getType() == 'i'){
                        indexes.push_back(Variable->getInt());
                    }
                    else{
                        indexes.push_back(0);
                        cout << "Warning: In " << __FUNCTION__ << ": Each variable should be of integer type.\n";
                    }
                }
            }
            else{
                cout << "Warning: In " << __FUNCTION__ << ": Context is not of numerical type.\n";
            }
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
            else if(isStringInGroup(Operation.Location.attribute, 11, "object", "text", "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", "primitives")){
                if(indexes.size() < 2){
                    cout << "Error: In " << __FUNCTION__ << ": In order to find an object by its index, you must provide at least 2 indexes.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": Index is out of layers vector's scope.\n";
                    return;
                }
                if(Operation.Location.attribute == "object"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Layer->Objects, "object", NewContext.Objects, NewContext.type);
                }
                else{
                    if(indexes.size() < 3){
                        cout << "Error: In " << __FUNCTION__ << ": In order to find a module instance by its index, you must provide at least 3 indexes.\n";
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
                        cout << "Error: In " << __FUNCTION__ << ": Index is out of objects vector's scope.\n";
                        return;
                    }
                    if(Operation.Location.attribute == "text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->TextContainer, Operation.Location.attribute, NewContext.Modules.Texts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "editable_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->EditableTextContainer, Operation.Location.attribute, NewContext.Modules.EditableTexts, NewContext.type);
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
                    else{
                        cout << "Error: In " << __FUNCTION__ << ": Module type \'" << Operation.Location.attribute << "\' does not exist.\n";
                        return;
                    }
                }
            }
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Source cannot have \'" << Operation.Location.source << "\' value.\n";
            return;
        }

        if(NewContext.type == ""){
            cout << "Error: In " << __FUNCTION__ << ": Index is out of vector's scope.\n";
            return;
        }
    }
    else{
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": In order to find an object by its index, you must provide at least 2 index.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": Index is out of layers vector's scope.\n";
                    return;
                }

                if(Operation.Location.attribute == "object"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Layer->Objects, "object", NewContext.Objects, NewContext.type);
                }
                else{
                    if(indexes.size() < 3){
                        cout << "Error: In " << __FUNCTION__ << ": In order to find a module instance by its index, you must provide at least 3 indexes.\n";
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
                        cout << "Error: In " << __FUNCTION__ << ": Index is out of objects vector's scope.\n";
                        return;
                    }

                    if(Operation.Location.attribute == "text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->TextContainer, Operation.Location.attribute, NewContext.Modules.Texts, NewContext.type);
                    }
                    else if(Operation.Location.attribute == "editable_text"){
                        findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+2, indexes.end()), Object->EditableTextContainer, Operation.Location.attribute, NewContext.Modules.EditableTexts, NewContext.type);
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
                    else{
                        cout << "Error: In " << __FUNCTION__ << ": Module type \'" << Operation.Location.attribute << "\' does not exist.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": In order to find a module instance by its index, you must provide at least 2 indexes.\n";
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
                    cout << "Error: In " << __FUNCTION__ << ": Index is out of objects vector's scope.\n";
                    return;
                }
                
                if(Operation.Location.attribute == "text"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->TextContainer, Operation.Location.attribute, NewContext.Modules.Texts, NewContext.type);
                }
                else if(Operation.Location.attribute == "editable_text"){
                    findInstanceInVectorByIndex(vector<unsigned>(indexes.begin()+1, indexes.end()), Object->EditableTextContainer, Operation.Location.attribute, NewContext.Modules.EditableTexts, NewContext.type);
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
                else{
                    cout << "Error: In " << __FUNCTION__ << ": Module type \'" << Operation.Location.attribute << "\' does not exist.\n";
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
        else if(Context->type == "pointer"){
            findInstanceInVectorByIndex(indexes, Context->BasePointers, Context->type, NewContext.BasePointers, NewContext.type);
        }
        else if(Context->type == "value"){
            findInstanceInVectorByIndex(indexes, Context->Variables, Context->type, NewContext.Variables, NewContext.type);
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Context of \'" << Context->type << "\' type is invalid.\n";
            return;
        }
    }

    if(NewContext.type != ""){
        addNewContext(EventContext, NewContext, NewContext.type, Operation.newContextID);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ProcessClass::bindFilesToObjects(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": Bind requires at least one context.\n";
        return;
    }
    if(Operation.Literals.size() == 0 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Bind requires at least one literal of string type.\n";
        return;
    }
    if(isStringInGroup(Operation.Literals[0].getString(), 5, "literal", "l", "remove_literal", "rliteral", "rl")){
        if(Operation.Literals.size() == 1){
            cout << "Error: In " << __FUNCTION__ << ": Bind requires at least two literals of string type.\n";
            return;
        }
        ContextClass * ContextA;
        if(!getOneContext(ContextA, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(ContextA->Objects.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one object.\n";
            return;
        }
        if(printOutInstructions){
            cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
        }
        for(AncestorObject * Object : ContextA->Objects){
            for(const VariableModule & Variable : vector<VariableModule>(Operation.Literals.begin()+1, Operation.Literals.end())){
                if(Variable.getType() != 's'){
                    cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only literals of \'string'; type.\n";
                    continue;
                }
                if(printOutInstructions){
                    cout << "\"" << Variable.getString() << "\", ";
                }
                if(isStringInGroup(Operation.Literals[0].getString(), 2, "literal", "l")){
                    Object->bindedScripts.push_back(EXE_PATH + Variable.getString());
                }
                else{
                    removeFromStringVector(Object->bindedScripts, EXE_PATH + Variable.getString());
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
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(ContextObject->Objects.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one object.\n";
            return;
        }
        if(ContextFiles->type == "pointer"){
            if(ContextFiles->BasePointers.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one pointer.\n";
                return;
            }
            if(printOutInstructions){
                cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
            }
            for(AncestorObject * Object : ContextObject->Objects){
                for(const BasePointersStruct & Pointer : ContextFiles->BasePointers){
                    if(Pointer.type != "string"){
                        cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only pointers of \'string'; type.\n";
                        continue;
                    }
                    if(printOutInstructions){
                        cout << "\"" << Pointer.getString() << "\", ";
                    }
                    if(isStringInGroup(Operation.Literals[0].getString(), 2, "context", "c")){
                        Object->bindedScripts.push_back(EXE_PATH + Pointer.getString());
                    }
                    else{
                        removeFromStringVector(Object->bindedScripts, EXE_PATH + Pointer.getString());
                    }
                }
            }
            if(printOutInstructions){
                cout << "]\n";
            }
        }
        else if(ContextFiles->type == "value"){
            if(ContextFiles->Variables.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one pointer.\n";
                return;
            }
            if(printOutInstructions){
                cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
            }
            for(AncestorObject * Object : ContextObject->Objects){
                for(const VariableModule & Variable : ContextFiles->Variables){
                    if(Variable.getType() != 's'){
                        cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only literals of \'string'; type.\n";
                        continue;
                    }
                    if(printOutInstructions){
                        cout << "\"" << Variable.getString() << "\", ";
                    }
                    if(isStringInGroup(Operation.Literals[0].getString(), 2, "context", "c")){
                        Object->bindedScripts.push_back(EXE_PATH + Variable.getString());
                    }
                    else{
                        removeFromStringVector(Object->bindedScripts, EXE_PATH + Variable.getString());
                    }
                }
            }
            if(printOutInstructions){
                cout << "]\n";
            }
        }
        else if(ContextFiles->type == "variable"){
            if(ContextFiles->Modules.Variables.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one pointer.\n";
                return;
            }
            if(printOutInstructions){
                cout << "bind " << Operation.dynamicIDs[0] << " " << Operation.Literals[0].getString() << " [";
            }
            for(AncestorObject * Object : ContextObject->Objects){
                for(const VariableModule * Variable : ContextFiles->Modules.Variables){
                    if(Variable->getType() != 's'){
                        cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " accepts only literals of \'string'; type.\n";
                        continue;
                    }
                    if(printOutInstructions){
                        cout << "\"" << Variable->getString() << "\", ";
                    }
                    if(isStringInGroup(Operation.Literals[0].getString(), 2, "context", "c")){
                        Object->bindedScripts.push_back(EXE_PATH + Variable->getString());
                    }
                    else{
                        removeFromStringVector(Object->bindedScripts, EXE_PATH + Variable->getString());
                    }
                }
            }
            if(printOutInstructions){
                cout << "]\n";
            }
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " only accepts file input of types: \'pointer\' and \'value\'.\n";
            return;
        }
    }
    else if(Operation.Literals[0].getString() == "reset" || Operation.Literals[0].getString() == "r"){
        ContextClass * ContextObject;
        if(!getOneContext(ContextObject, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
        if(ContextObject->Objects.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": Bind " << Operation.Literals[0].getString() << " requires at least one object.\n";
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
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": Build requires at least one context.\n";
        return;
    }
    ContextClass * ContextA;
    if(!getOneContext(ContextA, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(ContextA->Objects.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": Build requires at least one object.\n";
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
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
    
    for(AncestorObject * Object : ContextA->Objects){
        if(canResetEvents && Object == Owner){
            std::cout << "Error: In " << __FUNCTION__ << ": Cannot delete events of the owner of the currently executed event.\n";
            continue;
        }
        Object->translateAllScripts(canResetEvents);
        wasAnyEventUpdated = true;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
}
void ProcessClass::customBuildEventsInObjects(OperaClass & Operation, vector<ContextClass> & EventContext, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, char mode
){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " instruction requires at least one context.\n";
        return;
    }
    ContextClass * ObjectContext = nullptr;
    if(!getOneContext(ObjectContext, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(ObjectContext->Objects.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " requires at least one object.\n";
        return;
    }

    vector <string> stringVec;

    if(Operation.dynamicIDs.size() > 1){
        ContextClass * StringVecContext = getContextByID(EventContext, Operation.dynamicIDs[1], false);
        if(StringVecContext == nullptr){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
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
            cout << "Warning: In " << __FUNCTION__ << ": Cannot extract string value from context of \'" << StringVecContext->type << "\' type.\n";
        }
    }

    for(const VariableModule & Variable : Operation.Literals){
        stringVec.push_back(Variable.getString());
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.dynamicIDs[0] << " " << Operation.dynamicIDs[1] << " ";
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
        cout << "Warning: In " << __FUNCTION__ << ": No strings were provided.\n";
        return;
    }

    //All indexes linked with events must be recalculated - after adding new events, some contexts of event type might be invalid.  
    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
    
    for(AncestorObject * Object : ObjectContext->Objects){
        if(mode == 'p'){
            Object->translateScriptsFromPaths(stringVec);
        }
        else if(mode == 's'){
            Object->translateSubsetBindedScripts(stringVec);
        }
        else if(mode == 'c'){
            Object->injectCode(stringVec);
        }
        else if(mode == 'i'){
            Object->injectInstructions(stringVec);
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": \'" << mode << "\' mode does not exist."
                << "Allowed modes: p (uses translateScriptsFromPaths), s (uses translateSubsetBindedScripts),"
                << "c (uses injectCode), i (uses injectInstructions)\n";
            return;
        }
        
        wasAnyEventUpdated = true;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
}
void ProcessClass::clearEventsInObjects(OperaClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " instruction requires at least one context.\n";
        return;
    }
    ContextClass * ContextA;
    if(!getOneContext(ContextA, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(ContextA->Objects.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " requires at least one object.\n";
        return;
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.dynamicIDs[0] << "\n";
    }

    for(AncestorObject * Object : ContextA->Objects){
        if(Object == Owner){
            std::cout << "Error: In " << __FUNCTION__ << ": Cannot delete events of the owner of the currently executed event.\n";
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
void ProcessClass::executeFunctionForCameras(OperaClass & Operation, vector <VariableModule> & Variables, vector<Camera2D*> CamerasFromContext){
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
        else if(Operation.Location.attribute == "set_is_using_keyboard" && Variables.size() > 0){
            Camera->isUsingKeyboardToMove = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_move_with_mouse" && Variables.size() > 0){
            Camera->canMoveWithMouse = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_zoom_with_mouse" && Variables.size() > 0){
            Camera->canZoomWithMouse = Variables[0].getBoolUnsafe();
        }
        else if(Operation.Location.attribute == "set_can_interact_with_mouse" && Variables.size() > 0){
            Camera->canInteractWithMouse = Variables[0].getBoolUnsafe();
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
        else if(Operation.Location.attribute == "minimize"){
            Camera->deactivate();
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
            Camera->activate();
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
        else{
            cout << "Error: In " << __FUNCTION__ << ": function " << Operation.Location.attribute << "<" << Variables.size() << "> does not exist.\n";
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
            cout << "Error: In " << __FUNCTION__ << ": function " << Operation.Location.attribute << "<" << Variables.size() << "> does not exist.\n";
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
        else{
            bool temp = false;
            if(Variables.size() > 0){
                temp = Variables[0].getBoolUnsafe();
            }
            Object->control(Operation.Location.attribute, temp, Variables.size());
        }
    }
}
void ProcessClass::executeFunction(OperaClass Operation, vector<ContextClass> & EventContext, vector<EveModule>::iterator & Event, EngineClass & Engine){
    if(Operation.dynamicIDs.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": Function requires at least one context.\n";
        return;
    }

    vector <VariableModule> & Variables = Operation.Literals;
    ContextClass * Context = nullptr;
    
    if(Operation.Literals.size() > 0 || Operation.dynamicIDs.size() == 1){
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return;
        }
    }
    else if(Operation.dynamicIDs.size() > 1){
        if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            return;
        }

        ContextClass * ContextValues = nullptr;

        for(unsigned i = 1; i < Operation.dynamicIDs.size(); i++){
            ContextValues = getContextByID(EventContext, Operation.dynamicIDs[i], true);
            if(ContextValues == nullptr){
                cout << "Error: In " << __FUNCTION__ << ": There is no context with id: \'" << Operation.dynamicIDs[i] << "\'.\n";
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
                cout << "Error: In " << __FUNCTION__ << ": Second context must be of type: pointer or value.\n";
            }
        }
    }

    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.dynamicIDs[0] << "." << Operation.Location.attribute << "()";
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
        executeFunctionForCameras(Operation, Variables, Context->Cameras);
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
    else if(Context->type == "image"){
        if(isStringInGroup(Operation.Location.attribute, 6, "set_id", "set_position", "set_size", "set_scale", "resize", "connect_bitmap")){
            for(ImageModule * Image : Context->Modules.Images){
                if(!findObjectForFunction(ModulesObject, Layers, Image->getObjectID(), Image->getLayerID())){
                    continue;
                }
                Event->controlImage(Image, Operation.Location.attribute, Variables, ModulesObject->imageContainerIDs, Engine.BitmapContainer, EXE_PATH);
                if(Operation.Location.attribute != "set_id"){
                    ModulesObject->refreshCoordinates();
                }
            }
            return;
        }
        for(ImageModule * Image : Context->Modules.Images){
            Event->controlImage(Image, Operation.Location.attribute, Variables, emptyString, Engine.BitmapContainer, EXE_PATH);
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
    else{
        cout << "Error: In " << __FUNCTION__ << ": type \'" << Context->type << "\' does not exist.\n";
    }
}
void ProcessClass::changeEngineVariables(OperaClass & Operation, EngineClass & Engine){
    if(Operation.Literals.size() < 2){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires at least two literals.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires first literal being of a string type.\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << " ";
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
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        Engine.displaySize.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
        al_resize_display(Engine.display, Engine.displaySize.x, Engine.displaySize.y);
    }
    else if(Operation.Literals[0].getString() == "fullscreen"){
        if(Operation.Literals[1].getBool() == Engine.fullscreen){
            return;
        }
        Engine.fullscreen = Operation.Literals[1].getBool();
        al_set_display_flag(Engine.display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(Engine.display) & ALLEGRO_FULLSCREEN_WINDOW));
        al_set_display_flag(Engine.display, ALLEGRO_MAXIMIZED, !(al_get_display_flags(Engine.display) & ALLEGRO_MAXIMIZED));
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
        cout << "Error: In " << __FUNCTION__ << ": Attribute \'" << Operation.Literals[0].getString() << "\' does not exist.\n";
    }
}
void ProcessClass::changeProcessVariables(OperaClass & Operation, vector <string> & processIDs){
    if(Operation.Literals.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires at least one literal.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires first literal being of a string type.\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << " ";
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
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires at least two literals.\n";
        return;
    }
    if(Operation.Literals[0].getString() == "id"){
        setID(Operation.Literals[1].getString(), processIDs); 
    }
    else if(Operation.Literals[0].getString() == "is_active"){
        isActive = Operation.Literals[1].getBool(); 
    }
    else if(Operation.Literals[0].getString() == "can_interact_with_user"){
        canInteractWithUser = Operation.Literals[1].getBool(); 
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
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        windowPos.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
    }
    else if(Operation.Literals[0].getString() == "window_size"){
        if(Operation.Literals.size() < 3){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        windowSize.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
        resizeWindow(windowSize);
    }
    else if(Operation.Literals[0].getString() == "min_window_size"){
        if(Operation.Literals.size() < 3){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        minWindowSize.set(Operation.Literals[1].getInt(), Operation.Literals[2].getInt());
    }
    else if(Operation.Literals[0].getString() == "window_tint"){
        if(Operation.Literals.size() < 5){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires three literals.\n";
            return;
        }
        if(!Operation.Literals[1].isNumeric() || !Operation.Literals[2].isNumeric()
            || !Operation.Literals[3].isNumeric() || !Operation.Literals[4].isNumeric()
        ){
            cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction <<
                "\' with \'" << Operation.Literals[0].getString() << "\' attribute requires two last parameters to be of a numeric type.\n";
            return;
        }
        windowTint[0] = Operation.Literals[1].getDouble();
        windowTint[1] = Operation.Literals[2].getDouble();
        windowTint[2] = Operation.Literals[3].getDouble();
        windowTint[3] = Operation.Literals[4].getDouble();
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Attribute \'" << Operation.Literals[0].getString() << "\' does not exist.\n";
    }
}
void ProcessClass::loadBitmap(OperaClass & Operation, vector<SingleBitmap> & BitmapContainer){
    if(Operation.Literals.size() < 2 || Operation.Literals[0].getType() != 's' || Operation.Literals[1].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires two strings.\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << " " << Operation.Literals[1].getString() << "\n";
    }
    for(const SingleBitmap & Bitmap : BitmapContainer){
        if(Bitmap.ID == Operation.Literals[1].getString()){
            cout << "Warning: In " << __FUNCTION__ << ": In \'" << Operation.instruction << "\' instruction: "
                << "Loading failed. Bitmap with the id \'" << Bitmap.ID << "\' already exists.\n";
            return;
        }
    }
    BitmapContainer.push_back(SingleBitmap());
    BitmapContainer.back().loadBitmap(Operation.Literals[1].getString(), Operation.Literals[0].getString(), EXE_PATH);
}
void ProcessClass::createDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires one string (a path to the new directory).\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::create_directory(EXE_PATH + Operation.Literals[0].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": No such directory \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n";
    }
}
void ProcessClass::removeFileOrDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires one string (a path to the new directory).\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::remove(EXE_PATH + Operation.Literals[0].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": " << ex.what() << "\n";
    }
}
void ProcessClass::removeRecursivelyFileOrDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires one string (a path to the new directory).\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::remove_all(EXE_PATH + Operation.Literals[0].getString());
    }
    catch(std::filesystem::filesystem_error const& ex){
        cout << "In " << __FUNCTION__ << ": " << ex.what() << "\n";
    }
}
void ProcessClass::renameFileOrDirectory(OperaClass & Operation){
    if(Operation.Literals.size() < 2 || Operation.Literals[0].getType() != 's' || Operation.Literals[1].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires two strings (two paths to files or directories).\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << " " << Operation.Literals[1].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n"; 
    }
    if(Operation.Literals[1].getString() == "" || Operation.Literals[1].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[1].getString() << "\'.\n"; 
    }
    try{
        std::filesystem::rename(EXE_PATH + Operation.Literals[0].getString(), EXE_PATH + Operation.Literals[1].getString());
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
        ContextValues = getContextByID(EventContext, contextID, true);
        if(ContextValues == nullptr){
            cout << "Error: In " << __FUNCTION__ << ": There is no context with id: \'" << contextID << "\'.\n";
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
            cout << "Error: In " << __FUNCTION__ << ": Invalid context type \'" << ContextValues->type << "\'.\n";
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
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires one string (a path to a text file).\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n"; 
    }

    string loadedText = "";

    std::ifstream File(EXE_PATH + Operation.Literals[0].getString());

	if(!File){
		std::cerr << "Error: In " << __FUNCTION__ << ": Cannot open file: " << Operation.Literals[0].getString() << "\n";
        return;
    }
    for(string line; std::getline(File, line);){
        loadedText += line + "\n";
    }
    File.close();

    ContextClass * Context;
    if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(Context->type == "value"){
        if(Context->Variables.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": There are no literals in the context.\n";
            return;
        }
        if(Context->Variables.size() != 1){
            cout << "Warning: In " << __FUNCTION__ << ": There are several literals in the context. Program will proceed with the last added literal.\n";
        }
        if(Context->Variables.back().getType() != 's'){
            cout << "Error: In " << __FUNCTION__ << ": String variable required, but got \'" << Context->Variables.back().getType() << "\'.\n";
            return;
        }
        Context->Variables.back().setString(loadedText);
    }
    else if(Context->type == "pointer"){
        if(Context->BasePointers.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": There are no pointers in the context.\n";
            return;
        }
        if(Context->BasePointers.size() != 1){
            cout << "Warning: In " << __FUNCTION__ << ": There are several pointers in the context. Program will proceed with the last added pointer.\n";
        }
        if(Context->BasePointers.back().type != "string"){
            cout << "Error: In " << __FUNCTION__ << ": String pointer required, but got \'" << Context->Variables.back().getType() << "\'.\n";
            return;
        }
        *Context->BasePointers.back().pString = loadedText;
    }
    else if(Context->type == "variable"){
        if(Context->Modules.Variables.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": There are no variables in the context.\n";
            return;
        }
        if(Context->Modules.Variables.size() != 1){
            cout << "Warning: In " << __FUNCTION__ << ": There are several variables in the context. Program will proceed with the last added literal.\n";
        }
        if(Context->Modules.Variables.back()->getType() != 's'){
            cout << "Error: In " << __FUNCTION__ << ": String variable required, but got \'" << Context->Variables.back().getType() << "\'.\n";
            return;
        }
        Context->Modules.Variables.back()->setString(loadedText);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": No value can be extracted from the context.\n";
    }
}
void ProcessClass::saveStringAsFile(OperaClass & Operation, vector<ContextClass> & EventContext){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires one string (a path to a text file).\n";
        return;
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << "\n";
    }
    if(Operation.Literals[0].getString() == "" || Operation.Literals[0].getString()[0] == ' '){
        cout << "In " << __FUNCTION__ << ": Access denied to the path: \'" << EXE_PATH + Operation.Literals[0].getString() << "\'.\n"; 
    }

    string textFromContext = "";

    ContextClass * Context;
    if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
        cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
        return;
    }
    if(Context->type == "value"){
        if(Context->Variables.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": There are no literals in the context.\n";
            return;
        }
        if(Context->Variables.size() != 1){
            cout << "Warning: In " << __FUNCTION__ << ": There are several literals in the context. Program will proceed with the last added literal.\n";
        }
        if(Context->Variables.back().getType() != 's'){
            cout << "Error: In " << __FUNCTION__ << ": String variable required, but got \'" << Context->Variables.back().getType() << "\'.\n";
            return;
        }
        textFromContext = Context->Variables.back().getString();
    }
    else if(Context->type == "pointer"){
        if(Context->BasePointers.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": There are no pointers in the context.\n";
            return;
        }
        if(Context->BasePointers.size() != 1){
            cout << "Warning: In " << __FUNCTION__ << ": There are several pointers in the context. Program will proceed with the last added pointer.\n";
        }
        if(Context->BasePointers.back().type != "string"){
            cout << "Error: In " << __FUNCTION__ << ": String pointer required, but got \'" << Context->Variables.back().getType() << "\'.\n";
            return;
        }
        textFromContext = *Context->BasePointers.back().pString;
    }
    else if(Context->type == "variable"){
        if(Context->Modules.Variables.size() == 0){
            cout << "Error: In " << __FUNCTION__ << ": There are no variables in the context.\n";
            return;
        }
        if(Context->Modules.Variables.size() != 1){
            cout << "Warning: In " << __FUNCTION__ << ": There are several variables in the context. Program will proceed with the last added literal.\n";
        }
        if(Context->Modules.Variables.back()->getType() != 's'){
            cout << "Error: In " << __FUNCTION__ << ": String variable required, but got \'" << Context->Variables.back().getType() << "\'.\n";
            return;
        }
        textFromContext = Context->Modules.Variables.back()->getString();
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": No value can be extracted from the context.\n";
        return;
    }

    std::ofstream File(EXE_PATH + Operation.Literals[0].getString());

	if(!File){
		std::cerr << "Error: In " << __FUNCTION__ << ": Cannot open file: " << Operation.Literals[0].getString() << "\n";
        return;
    }
	File << textFromContext;
    File.close();
}
void ProcessClass::listOutEntities(OperaClass & Operation, const vector<ProcessClass> & Processes, const EngineClass & Engine){
    if(Operation.Literals.size() < 1 || Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' requires at least one string.\n";
        return;
    }
    bool printDetails = false;
    if(Operation.Literals.size() >= 2){
        printDetails = Operation.Literals[1].getBoolUnsafe();
    }
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString() << " " << printDetails << "\n";
    }
    if(Operation.Literals[0].getString() == "processes"){
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
    else if(Operation.Literals[0].getString() == "layers"){
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
    else if(Operation.Literals[0].getString() == "cameras"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tProcess\tActive\tPinned\n";
            for(const ProcessClass & Process : Processes){
                for(const Camera2D & Camera : Process.Cameras){
                    cout << i
                    << "\t" << Camera.getID()
                    << "\t" << Process.getID()
                    << "\t" << Camera.getIsActive()
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
    else if(Operation.Literals[0].getString() == "bitmaps"){
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
    else if(Operation.Literals[0].getString() == "fonts"){
        if(printDetails){
            int i = 0;
            cout << "Nr\tID\tSize\n";
            for(const SingleFont & Font : Engine.FontContainer){
                cout << i << "\t" << Font.ID
                    << "\t" << Font.sizeF << "\n";
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
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " requires at least one string literal.\n";
        return;
    }

    string layerID = "", objectID = "", script = "";

    if(Operation.Literals.size() > 1 && Operation.Literals[1].getType() == 's'){
        layerID = Operation.Literals[1].getString();
    }
    if(Operation.Literals.size() > 2 && Operation.Literals[2].getType() == 's'){
        objectID = Operation.Literals[2].getString();
    }
    if(Operation.Literals.size() > 3 && Operation.Literals[3].getType() == 's'){
        script = Operation.Literals[3].getString();
    }
    
    if(Processes.size() + 1 <= Processes.capacity()){
        Processes.push_back(ProcessClass(Engine.EXE_PATH, Engine.getDisplaySize(),
            script, Operation.Literals[0].getString(), layerID, objectID, Engine.processIDs
        ));
    }
    else{
        
    }
}
void ProcessClass::createNewOwnerVariable(OperaClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
    vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
){
    if(Operation.Literals.size() < 2){
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " requires two literals.\n";
        return;
    }
    if(Operation.Literals[0].getType() != 's'){
        cout << "Error: In " << __FUNCTION__ << ": " << Operation.instruction << " requires the first literal to be of a string type.\n";
        return;
    }
    
    if(printOutInstructions){
        cout << Operation.instruction << " " << Operation.Literals[0].getString()
            << " " << Operation.Literals[1].getStringUnsafe() << Operation.newContextID << "\n";
    }

    for(const VariableModule & Variable : Owner->VariablesContainer){
        if(Variable.getID() == Operation.newContextID){
            cout << "Error: In \'" << Operation.instruction << "\' instruction: Cannot create a variable with id \'"
                << Operation.newContextID << "\', because a variable with the same id already exists.\n";
            return;
        }
    }

    PointerRecalculator Recalculator;
    Recalculator.findIndexesForModules(Layers, EventContext, StartingEvent, Event, MemoryStack);
    
    if(Operation.Literals[0].getString() == "bool" || Operation.Literals[0].getString() == "b"){
        if(Operation.Literals[1].getType() != 'b'){
            cout << "Error: In " << __FUNCTION__ << ": In " << Operation.instruction << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newBool(
            Operation.Literals[1].getBool(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else if(Operation.Literals[0].getString() == "int" || Operation.Literals[0].getString() == "i"){
        if(Operation.Literals[1].getType() != 'i'){
            cout << "Error: In " << __FUNCTION__ << ": In " << Operation.instruction << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newInt(
            Operation.Literals[1].getInt(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else if(Operation.Literals[0].getString() == "double" || Operation.Literals[0].getString() == "d"){
        if(Operation.Literals[1].getType() != 'd'){
            cout << "Error: In " << __FUNCTION__ << ": In " << Operation.instruction << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newDouble(
            Operation.Literals[1].getDouble(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else if(Operation.Literals[0].getString() == "string" || Operation.Literals[0].getString() == "s"){
        if(Operation.Literals[1].getType() != 's'){
            cout << "Error: In " << __FUNCTION__ << ": In " << Operation.instruction << " instruction the second literal is not of a "
                << Operation.Literals[0].getString() << " type.\n";
            return;
        }
        Owner->VariablesContainer.push_back(VariableModule::newString(
            Operation.Literals[1].getString(), Operation.newContextID, &Owner->variablesContainerIDs, Owner->getLayerID(), Owner->getID()
        ));
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": In " << Operation.instruction
            << " instruction the first parameter has incorrect value: \'" << Operation.Literals[0].getString() << "\'.\n";
        return;
    }

    Recalculator.updatePointersToModules(Layers, EventContext, StartingEvent, Event, MemoryStack);

    ContextClass NewContext;

    if(Owner->VariablesContainer.size() > 0){
        NewContext.Modules.Variables.push_back(&Owner->VariablesContainer.back());
        addNewContext(EventContext, NewContext, "variable", Operation.newContextID);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation.instruction << "\' failed.\n";
        addNewContext(EventContext, NewContext, "null", Operation.newContextID);
    }
}
void ProcessClass::tokenizeString(OperaClass & Operation, vector<ContextClass> & EventContext){
    ContextClass * Context = nullptr;
    if(!getOneContext(Context, EventContext, Operation.dynamicIDs)){
        cout << "Error: In" << __FUNCTION__ << ": \'" << Operation.instruction << "\' requires at least one context.\n";
        return;
    }
    string input = "";
    if(Context->type == "value"){
        if(Context->Variables.size() == 0){
            cout << "Warning: In" << __FUNCTION__ << ": In \'" << Operation.instruction << "\': Context is empty.\n";
            return;
        }
        input = Context->Variables[0].getString();
    }
    else if(Context->type == "variable"){
        if(Context->Modules.Variables.size() == 0){
            cout << "Warning: In" << __FUNCTION__ << ": In \'" << Operation.instruction << "\': Context is empty.\n";
            return;
        }
        input = Context->Modules.Variables[0]->getString();
    }
    else if(Context->type == "pointer"){
        if(Context->BasePointers.size() == 0){
            cout << "Warning: In" << __FUNCTION__ << ": In \'" << Operation.instruction << "\': Context is empty.\n";
            return;
        }
        input = Context->BasePointers[0].getString();
    }
    else{
        cout << "Error: In" << __FUNCTION__ << ": In \'" << Operation.instruction << "\': invalid context type \'" << Context->type << "\'.\n";
        return;
    }
    vector <string> words = tokenizeCode(input);
    ContextClass NewContext;
    NewContext.type = "value";
    if(Operation.dynamicIDs.size() == 1){
        for(string word : words){
            NewContext.Variables.push_back(VariableModule::newString(word));
        }
        addNewContext(EventContext, NewContext, "value", "");
    }
    else if(Operation.dynamicIDs.size() == 2){
        for(string word : words){
            NewContext.Variables.push_back(VariableModule::newString(word));
        }
        addNewContext(EventContext, NewContext, "value", Operation.dynamicIDs[1]);
    }
    else{
        NewContext.Variables.push_back(VariableModule::newString(""));
        for(size_t i = 0; i < words.size() && i+1 < Operation.dynamicIDs.size(); i++){
            NewContext.Variables.back().setString(words[i]);
            moveOrRename(EventContext, &NewContext, Operation.dynamicIDs[i+1]);
        }
    }
}
OperaClass ProcessClass::executeInstructions(vector<OperaClass> Operations, LayerClass *& OwnerLayer,
    AncestorObject *& Owner, vector<ContextClass> & EventContext, vector<AncestorObject*> & TriggeredObjects,
    vector<ProcessClass> & Processes, vector<EveModule>::iterator & StartingEvent,
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, EngineClass & Engine
){
    for(OperaClass & Operation : Operations){
        if(isStringInGroup(Operation.instruction, 5, "continue", "break", "return", "reboot", "power_off")){
            if(printOutInstructions){
                cout << Operation.instruction << "\n";
            }
            return Operation;
        }
        //Aggregate entities and push them on the Variables Stack.
        else if(isStringInGroup(Operation.instruction, 4, "first", "last", "all", "random")){
            aggregateEntities(Operation, EventContext, Engine);
        }
        else if(Operation.instruction == "index"){
            getReferenceByIndex(Operation, EventContext);
        }
        //Execute operations on sets.
        else if(isStringInGroup(Operation.instruction, 3, "sum", "intersection", "difference")){
            aggregateTwoSets(Operation, EventContext);
        }
        //Get only values from the environment.
        else if(Operation.instruction == "value"){
            aggregateValues(EventContext, Operation, OwnerLayer, Owner, Engine, &Processes);
        }
        //Get literals prepared in the event.
        else if(isStringInGroup(Operation.instruction, 4, "bool", "int", "double", "string")){
            createLiteral(EventContext, Operation);
        }
        //Generate random int value between  
        else if(Operation.instruction == "random_int"){
            generateRandomVariable(EventContext, Operation);
        }
        //Aggregate context only by id.
        else if(Operation.instruction == "find_by_id"){
            aggregateOnlyById(EventContext, Operation, OwnerLayer, Owner);
        }
        //Assign a name to the previously aggregated entities.
        else if(Operation.instruction == "let"){
            nameVariable(EventContext, Operation);
        }
        else if(Operation.instruction == "clone"){
            if(Operation.Literals.size() > 0){
                cloneEntities(Operation.dynamicIDs, Operation.Literals.back().getBoolUnsafe(true), EventContext, Layers);
            }
            else{
                cloneEntities(Operation.dynamicIDs, true, EventContext, Layers);
            }
        }
        else if(isStringInGroup(Operation.instruction, 4, "+", "-", "*", "/")){
            executeArithmetics(Operation, EventContext);
        }
        else if(isStringInGroup(Operation.instruction, 7, "++", "--", "=", "+=", "-=", "*=", "/=")){
            moveValues(Operation, EventContext);
        }
        else if(Operation.instruction == "in"){
            checkIfVectorContainsVector(Operation, EventContext);
        }
        else if(Operation.instruction == "new"){
            createNewEntities(Operation, EventContext, Owner, TriggeredObjects, StartingEvent, Event, MemoryStack);
        }
        else if(Operation.instruction == "delete"){
            markEntitiesForDeletion(Operation, EventContext, OwnerLayer, Owner, TriggeredObjects);
            if(OwnerLayer == nullptr || Owner == nullptr || Event->getIsDeleted()){
                return OperaClass();
            }
        }
        else if(Operation.instruction == "delete_this_event"){
            Event->deleteLater();
            wasDeleteExecuted = true;
            if(printOutInstructions){
                cout << "delete_this_event\n";
            }
            return Operation;
        }
        else if(Operation.instruction == "bind"){
            bindFilesToObjects(Operation, EventContext);
        }
        else if(Operation.instruction == "build"){
            buildEventsInObjects(Operation, EventContext, Owner, StartingEvent, Event, MemoryStack);
        }
        else if(Operation.instruction == "load_build"){
            customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 'p');
        }
        else if(Operation.instruction == "build_subset"){
            customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 's');
        }
        else if(Operation.instruction == "inject_code"){
            customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 'c');
        }
        else if(Operation.instruction == "inject_instr"){
            customBuildEventsInObjects(Operation, EventContext, StartingEvent, Event, MemoryStack, 'i');
        }
        else if(Operation.instruction == "demolish"){
            clearEventsInObjects(Operation, EventContext, Owner);
        }
        else if(Operation.instruction == "fun"){
            executeFunction(Operation, EventContext, Event, Engine);
        }
        else if(Operation.instruction == "env"){
            changeEngineVariables(Operation, Engine);
        }
        else if(Operation.instruction == "edit_proc"){
            changeProcessVariables(Operation, Engine.processIDs);
        }
        else if(Operation.instruction == "load_bitmap"){
            loadBitmap(Operation, Engine.BitmapContainer);
        }
        else if(Operation.instruction == "mkdir"){
            createDirectory(Operation);
        }
        else if(Operation.instruction == "rm"){
            removeFileOrDirectory(Operation);
        }
        else if(Operation.instruction == "rmll"){
            removeRecursivelyFileOrDirectory(Operation);
        }
        else if(Operation.instruction == "rename"){
            renameFileOrDirectory(Operation);
        }
        else if(Operation.instruction == "print"){
            executePrint(Operation, EventContext);
        }
        else if(Operation.instruction == "load_text"){
            loadFileAsString(Operation, EventContext);
        }
        else if(Operation.instruction == "save_text"){
            saveStringAsFile(Operation, EventContext);
        }
        else if(Operation.instruction == "ls"){
            listOutEntities(Operation, Processes, Engine);
        }
        else if(Operation.instruction == "new_proc"){
            createNewProcess(Operation, Processes, EventContext, Owner, TriggeredObjects,
                StartingEvent, Event, MemoryStack, Engine);
        }
        else if(Operation.instruction == "var"){
            createNewOwnerVariable(Operation, EventContext, Owner, StartingEvent, Event, MemoryStack);
        }
        else if(Operation.instruction == "tokenize"){
            tokenizeString(Operation, EventContext);
        }
        if(printOutInstructions && printOutStackAutomatically){
            string buffor = "Stack: ";
            for(auto context : EventContext){
                buffor += context.ID + ":" + context.type + ":" + context.getValue() + ", ";
            }
            buffor += "\n";
            printInColor(buffor, 11);
        }
    }
    return OperaClass();
}
VariableModule ProcessClass::findNextValueInMovementModule(ConditionClass &Condition, AncestorObject *CurrentObject){
    if(CurrentObject == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Object does not exist.\n";
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
    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    NewValue.setBool(false);
    NewValue.setID("null", nullptr);
    return NewValue;
}
VariableModule ProcessClass::getValueFromObjectInCamera(AncestorObject * CurrentObject, const string & attribute, const string & cameraID){
    if(CurrentObject == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    bool newValue = false;
    if(attribute == "visible"){
        for(const Camera2D & Camera : Cameras){
            if(!Camera.getIsDeleted() && Camera.isObjectVisible(CurrentObject->getPos(false), CurrentObject->getSize())){
                newValue = true;
                break;
            }
        }
    }
    else if(attribute == "visible_specific"){
        for(const Camera2D & Camera : Cameras){
            if(Camera.getID() == cameraID){
                if(!Camera.getIsDeleted() && Camera.isObjectVisible(CurrentObject->getPos(false), CurrentObject->getSize())){
                    newValue = true;
                }
                break;
            }
        }
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    }
    return VariableModule::newBool(newValue);
}
VariableModule ProcessClass::getValueFromMouseClickingObject(const MouseClass & Mouse, AncestorObject * CurrentObject, const string & attribute, const short & button){
    if(CurrentObject == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Object does not exist.\n";
        return VariableModule::newBool(false);
    }
    bool result = false;
    if(attribute == "pressed"){
        if(SelectedCamera != nullptr && Mouse.firstPressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            result = Mouse.firstPressedInRectangle(
                CurrentObject->getPosOnCamera(SelectedCamera),
                CurrentObject->getSize(),
                button,
                CurrentObject->getIsAttachedToCamera(),
                SelectedCamera
            );
        }
    }
    else if(attribute == "pressing"){
        if(SelectedCamera != nullptr && Mouse.pressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            result = Mouse.pressedInRectangle(
                CurrentObject->getPosOnCamera(SelectedCamera),
                CurrentObject->getSize(),
                button,
                CurrentObject->getIsAttachedToCamera(),
                SelectedCamera
            );
        }
    }
    else if(attribute == "released"){
        if(SelectedCamera != nullptr && Mouse.releasedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true, SelectedCamera)){
            result = Mouse.releasedInRectangle(
                CurrentObject->getPosOnCamera(SelectedCamera),
                CurrentObject->getSize(),
                button,
                CurrentObject->getIsAttachedToCamera(),
                SelectedCamera
            );
        }
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    }
    return VariableModule::newBool(result);
}
VariableModule ProcessClass::getValueFromObjectInCollision(ConditionClass &Condition, AncestorObject * CurrentObject, LayerClass * CurrentLayer){
    if(CurrentObject == nullptr || CurrentObject->getIsDeleted()){
        cout << "Error: In " << __FUNCTION__ << ": Object does not exist.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": Layer does not exist.\n";
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
    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": There is no variable with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "ancestor"){
        return CurrentObject->getAttributeValue(Condition.Location.attribute, Condition.Literal.getStringUnsafe());
    }
    else if(Condition.Location.moduleType == "camera"){
        return getValueFromObjectInCamera(CurrentObject, Condition.Location.attribute, Condition.Location.moduleID);
    }
    else if(Condition.Location.moduleType == "mouse"){
        return getValueFromMouseClickingObject(Mouse, CurrentObject, Condition.Location.attribute, Condition.Literal.getIntUnsafe());
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
        cout << "Error: In " << __FUNCTION__ << ": There is no text with id: \'" << Condition.Location.moduleID << "\'.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": There is no editable text with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else if(Condition.Location.moduleType == "collision"){
        return getValueFromObjectInCollision(Condition, CurrentObject, CurrentLayer);
    }
    
    cout << "Error: In " << __FUNCTION__ << ": Value not found.\n";
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
    else if(Condition.Location.source == "on_display_resize"){
        NewValue.setBool(Engine.displayResized);
        return NewValue;
    }
    else if(Condition.Location.source == "second_passed"){
        NewValue.setBool(Engine.secondHasPassed());
        return NewValue;
    }
    else if(Condition.Location.source == "key_pressed"){
        NewValue.setBool(isKeyFirstPressed(Condition.Literal.getInt(), Engine.firstPressedKeys));
        return NewValue;
    }
    else if(Condition.Location.source == "key_pressing"){
        NewValue.setBool(isKeyPressed(Condition.Literal.getInt(), Engine.pressedKeys));
        return NewValue;
    }
    else if(Condition.Location.source == "key_released"){
        NewValue.setBool(isKeyReleased(Condition.Literal.getInt(), Engine.releasedKeys));
        return NewValue;
    }
    else if(Condition.Location.source == "any_key_pressed"){
        NewValue.setBool(Engine.firstPressedKeys.size() > 0);
        return NewValue;
    }
    else if(Condition.Location.source == "any_key_pressing"){
        NewValue.setBool(Engine.pressedKeys.size() > 0);
        return NewValue;
    }
    else if(Condition.Location.source == "any_key_released"){
        NewValue.setBool(Engine.releasedKeys.size() > 0);
        return NewValue;
    }
    else if(Condition.Location.source == "mouse_moved"){
        NewValue.setBool(Engine.Mouse.didMouseMove);
        return NewValue;
    }
    else if(Condition.Location.source == "mouse_pressed"){
        NewValue.setBool(Engine.Mouse.isFirstPressed(Condition.Literal.getInt()));
        return NewValue;
    }
    else if(Condition.Location.source == "mouse_pressing"){
        NewValue.setBool(Engine.Mouse.isPressed(Condition.Literal.getInt()));
        return NewValue;
    }
    else if(Condition.Location.source == "mouse_released"){
        NewValue.setBool(Engine.Mouse.isReleased(Condition.Literal.getInt()));
        return NewValue;
    }
    else if(Condition.Location.source == "window_w"){
        NewValue.setInt(Engine.displaySize.x);
        return NewValue;
    }
    else if(Condition.Location.source == "window_h"){
        NewValue.setInt(Engine.displaySize.y);
        return NewValue;
    }
    else if(Condition.Location.source == "number_of_processes"){
        NewValue.setInt(Processes->size());
        return NewValue;
    }
    else if(Condition.Location.source == "number_of_cameras"){
        NewValue.setInt(0);
        for(const ProcessClass & ProcessIter : *Processes){
            NewValue.addInt(ProcessIter.Cameras.size());
        }
        return NewValue;
    }
    else if(Condition.Location.source == "number_of_layers"){
        NewValue.setInt(0);
        for(const ProcessClass & ProcessIter : *Processes){
            NewValue.addInt(ProcessIter.Layers.size());
        }
        return NewValue;
    }
    else if(Condition.Location.source == "number_of_objects"){
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
            cout << "Error: In " << __FUNCTION__ << ": Process with id \'" << Condition.Location.process << "\' does not exist.\n";
            return VariableModule::newBool(false);
        }
    }
    if(Condition.Location.source == "on_boot"){
        NewValue.setBool(Process->firstIteration);
        return NewValue;
    }
    else if(Condition.Location.source == "process"){
        if(Condition.Location.attribute == "number_of_cameras"){
            NewValue.setInt(Process->Cameras.size());
            return NewValue;
        }
        else if(Condition.Location.attribute == "number_of_layers"){
            NewValue.setInt(Process->Layers.size());
            return NewValue;
        }
        else if(Condition.Location.source == "number_of_objects"){
            NewValue.setInt(0);
            for(const LayerClass & Layer : Process->Layers){
                NewValue.addInt(Layer.Objects.size());
            }
            return NewValue;
        }
    }
    else if(Condition.Location.source == "camera"){
        NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
        for(const Camera2D & Camera : Process->Cameras){
            if(Camera.getID() != Condition.Location.cameraID){
                continue;
            }
            if(Camera.getIsDeleted()){
                break;
            }
            NewValue = Camera.getValue(Condition.Location.attribute);
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
    }
    else if(Condition.Location.source == "layer"){
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
    }
    else if(Condition.Location.source == "object"){
        return Process->findNextValueAmongObjects(Condition, Owner, OwnerLayer, Engine.Mouse);
    }
    else if(Condition.Location.source == "context" || Condition.Location.source == "c"){
        ContextClass * Context;
        vector<string> dynamicIDs = {Condition.Literal.getStringUnsafe()};
        if(!getOneContext(Context, EventContext, dynamicIDs)){
            cout << "Error: In " << __FUNCTION__ << ": No context found.\n";
            NewValue.setBool(false);
            return NewValue;
        }
        if(Context->type == "value"){
            if(Context->Variables.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": There are no literals in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Variables.size() != 1){
                cout << "Warning: In " << __FUNCTION__ << ": There are several literals in the context. Program will proceed with the last added literal.\n";
            }
            return Context->Variables.back();
        }
        if(Context->type == "pointer"){
            if(Context->BasePointers.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": There are no pointers in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->BasePointers.size() != 1){
                cout << "Warning: In " << __FUNCTION__ << ": There are several pointers in the context. Program will proceed with the last added pointer.\n";
            }
            NewValue.setValueFromPointer(Context->BasePointers.back());
            return NewValue;
        }
        if(Context->type == "variable"){
            if(Context->Modules.Variables.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": There are no variables in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Modules.Variables.size() != 1){
                cout << "Warning: In " << __FUNCTION__ << ": There are several variables in the context. Program will proceed with the last added literal.\n";
            }
            return *Context->Modules.Variables.back();
        }
        if(Context->type == "object"){
            if(Context->Objects.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": There are no objects in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Objects.size() != 1){
                cout << "Warning: In " << __FUNCTION__ << ": There are several objects in the context. Program will proceed with the last added object.\n";
            }
            Condition.Location.layerID = Context->Objects.back()->getLayerID();
            Condition.Location.objectID = Context->Objects.back()->getObjectID();
            return findNextValueAmongObjects(Condition, Owner, OwnerLayer, Engine.Mouse);
        }
        if(Context->type == "layer"){
            if(Context->Layers.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": There are no layers in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Layers.size() != 1){
                cout << "Warning: In " << __FUNCTION__ << ": There are several layers in the context. Program will proceed with the last added layer.\n";
            }
            NewValue = Context->Layers.back()->getValue(Condition.Location.attribute, Condition.Location.spareID);
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
        if(Context->type == "camera"){
            if(Context->Cameras.size() == 0){
                cout << "Error: In " << __FUNCTION__ << ": There are no cameras in the context.\n";
                NewValue.setBool(false);
                return NewValue;
            }
            if(Context->Cameras.size() != 1){
                cout << "Warning: In " << __FUNCTION__ << ": There are several cameras in the context. Program will proceed with the last added camera.\n";
            }
            NewValue = Context->Cameras.back()->getValue(Condition.Location.attribute);
            NewValue.setID(Condition.Location.source + "_" + Condition.Location.attribute, nullptr);
            return NewValue;
        }
        cout << "Error: In " << __FUNCTION__ << ": No value can be extracted from the context.\n";
    }
    else if(Condition.Location.source == "literal"){
        return Condition.Literal;
    }
    else if(Condition.Location.source == "variable"){
        for(const VariableModule & Variable : Owner->VariablesContainer){
            if(Variable.getID() == Condition.Location.moduleID){
                if(Variable.getIsDeleted()){
                    break;
                }
                return Variable;
            }
        }
        cout << "Error: In " << __FUNCTION__ << ": There is no variable with id: \'" << Condition.Location.moduleID << "\'.\n";
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": No valid source provided.\n";
    }
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
        for(string op : Condition.operators){
            if(ignoreFlagOr > 0 || ignoreFlagAnd > 0){
                if(op == "||" && ignoreFlagOr > 0){
                    ignoreFlagOr--;
                }
                if(op == "&&" && ignoreFlagAnd > 0){
                    ignoreFlagAnd--;
                }
            }
            else if(op == "!"){
                leftOperand = resultStack.back();
                leftOperand.negate();
                if(printOutLogicalEvaluations){
                    cout << "!(" << resultStack.back().getID() << ":" << resultStack.back().getAnyValue() << ") -> " << leftOperand.getAnyValue() << "\n";
                }
                resultStack.pop_back();
                leftOperand.setID("!("+leftOperand.getID()+")", nullptr);
                resultStack.push_back(leftOperand);
            }
            else if(op == "igT"){
                if(resultStack.back().isConditionMet<bool>(true, "==", 'b')){
                    if(printOutLogicalEvaluations){
                        cout << resultStack.back().getID() << ":" << "true || ... -> true\n";
                    }
                    ignoreFlagOr++;
                }
            }
            else if(op == "igF"){
                if(resultStack.back().isConditionMet<bool>(false, "==", 'b')){
                    if(printOutLogicalEvaluations){
                        cout << resultStack.back().getID() << ":" << "false && ... -> false\n";
                    }
                    ignoreFlagAnd++;
                }
            }
            else if(resultStack.size() >= 2){

                if(!isStringInGroup(op, 14, "&&", "||", "==", "!=", ">", "<", ">=", "<=", "+", "-", "*", "/", "%", "**")){
                    cout << "Error: In " << __FUNCTION__ << ": Unrecognized operator in the if statement.\n";
                    continue;
                }

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
                
                if (isStringInGroup(op, 8, "&&", "||", "==", "!=", ">", "<", ">=", "<=")){
                    comparasion = leftOperand.isConditionMet(op, &rightOperand);
                    
                    if(printOutLogicalEvaluations){
                        cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << op << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << comparasion << "\n";
                    }
                    
                    resultStack.back().setBool(comparasion);
                }
                else if(isStringInGroup(op, 6, "+", "-", "*", "/", "%", "**")){
                    if(leftOperand.getType() == 'd' || rightOperand.getType() == 'd'){
                        resultDouble = leftOperand.floatingOperation(op, &rightOperand);

                        if(printOutLogicalEvaluations){
                            cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << op << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << resultDouble << "\n";
                        }

                        resultStack.back().setDouble(resultDouble);
                    }
                    else{
                        resultInt = leftOperand.intOperation(op, &rightOperand);

                        if(printOutLogicalEvaluations){
                            cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << op << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << resultInt << "\n";
                        }

                        resultStack.back().setInt(resultInt);
                    }
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
                cout << "Error: In " << __FUNCTION__ << ": String is not a valid booleon value.\n";
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
    for(ChildStruct & Child : Event->Children){
        if(!Child.finished){
            for(Unfinished = Triggered->EveContainer.begin(); Unfinished != Triggered->EveContainer.end(); Unfinished++){
                if(!Unfinished->getIsDeleted() && Unfinished->getIsActive() && Unfinished->getID() == Child.ID){
                    Child.finished = true;
                    return Unfinished;
                }
            }
            Child.finished = true;
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
    bool layersWereModified = false;
    unsigned cameraIndex = 0;
    for(auto Camera = Cameras.begin(); Camera != Cameras.end(); cameraIndex++){
        if(Camera->getIsDeleted()){
            removeFromVector(camerasIDs, Camera->getID());
            Camera->clear();
            Camera = Cameras.erase(Camera);
            removeFromVector(camerasOrder, cameraIndex);
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
                    Object->clearContainers();
                    Object = Layer->Objects.erase(Object);
                }
                else{
                    deleteModuleInstance(Object->TextContainer, Object->textContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->EditableTextContainer, Object->editableTextContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->ImageContainer, Object->imageContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->MovementContainer, Object->movementContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->CollisionContainer, Object->collisionContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->ParticlesContainer, Object->particlesContainerIDs, layersWereModified);
                    deleteEventInstance(Object->EveContainer, Object->eveContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->VariablesContainer, Object->variablesContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->ScrollbarContainer, Object->scrollbarContainerIDs, layersWereModified);
                    deleteModuleInstance(Object->PrimitivesContainer, Object->primitivesContainerIDs, layersWereModified);
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
void ProcessClass::triggerEve(EngineClass & Engine, vector<ProcessClass> & Processes){
    if(wasDeleteExecuted && deleteEntities()){
        updateBaseOfTriggerableObjects();
        wasDeleteExecuted = false;
    }
    if(wasNewExecuted || wasAnyEventUpdated){
        updateBaseOfTriggerableObjects();
        wasNewExecuted = false;
        wasAnyEventUpdated = false;
    }
    
    //Only events from TriggeredObjects can be executed in the current iteration - events of newly created objects 
    //must wait with execution for the next iteration, unless run() command will be used.
    vector <AncestorObject*> TriggeredObjects;
    detectTriggeredEvents(Engine, TriggeredObjects);

    if(TriggeredObjects.size() == 0){
        return;
    }

    if(printOutInstructions){
        cout << "\nTriggered "<< TriggeredObjects.size() << " objects:\n";
    }

    //Remember to delete pointers to destroyed objects during the iteration
    
    vector<EveModule>::iterator StartingEvent, Event;
    vector<MemoryStackStruct> MemoryStack;
    vector<ContextClass> Context; //All dynamic context created from instructions. It's inherited by the children of an event.

    bool noTriggerableEvents = true;
    

    unsigned i = 0;

    unsigned preGeneratedContextSize = 0;

    LayerClass * TriggeredLayer;
    OperaClass Interrupt;

    for(AncestorObject * Triggered : TriggeredObjects){
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

        for(auto trigger : Event->primaryTriggerTypes){
            if(trigger == "on_init"){
                Event->willBeDeleted = true;
                wasDeleteExecuted = true;
            }
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
        for(VariableModule & Variable : Triggered->VariablesContainer){
            Context.push_back(ContextClass());
            Context.back().type = "variable";
            Context.back().Modules.Variables.push_back(&Variable);
            Context.back().ID = Variable.getID();
        }
        preGeneratedContextSize = Context.size();

        do{
            if(printOutInstructions){
                printInColor("Current event: " + Event->getID() + "\n", 14);
            }
            if(Event->conditionalStatus == 'n' && Interrupt.instruction != "break"){
                Event->conditionalStatus = evaluateConditionalChain(Event->ConditionalChain, Triggered, TriggeredLayer, Engine, Context);
            }
            if(Event->conditionalStatus == 't' && Interrupt.instruction != "break"){ //if true
                if(!Event->areDependentOperationsDone){
                    Interrupt = executeInstructions(Event->DependentOperations, TriggeredLayer, Triggered, Context, TriggeredObjects,
                        Processes, StartingEvent, Event, MemoryStack, Engine
                    );
                    if(Interrupt.instruction == "power_off"){
                        Engine.closeProgram = true;
                        return;
                    }
                    else if(Interrupt.instruction == "reboot"){
                        rebooted = true;
                        return;
                    }
                    else if(Interrupt.instruction == "return"){
                        Interrupt.instruction = "";
                        break;
                    }
                    if(TriggeredLayer == nullptr || Triggered == nullptr){
                        cout << "Aborting! The owner of the event has been deleted.\n";
                        break;
                    }
                    Event->areDependentOperationsDone = true;
                }
                if(!Event->checkIfAllChildrenFinished() && Interrupt.instruction != "break"){
                    MemoryStack.push_back(MemoryStackStruct(Event, Context.size()));
                    Event = FindUnfinishedEvent(Triggered, Event);
                    continue;
                }
            }
            else if(Event->conditionalStatus == 'f' && Interrupt.instruction != "break" && Event->elseChildID != "" && !Event->elseChildFinished){ //else
                MemoryStack.push_back(MemoryStackStruct(Event, Context.size()));
                Event = FindElseEvent(Triggered, Event);
                if(MemoryStack.back().Event->elseChildFinished){ //True if else event has been found.
                    continue;
                }
                MemoryStack.pop_back();
            }

            if(MemoryStack.size() > 0){
                if(MemoryStack.size() > 0 && MemoryStack.back().contextSize <= Context.size()){
                    Context.erase(Context.begin() + MemoryStack.back().contextSize, Context.end());
                }
                else{
                    cout << "THIS SHOULD NOT HAPPENED\n";
                }
            }

            if(Event->loop && Event->conditionalStatus != 'f' && Interrupt.instruction != "break"){ //loop back
                Event->conditionalStatus = 'n';
                Event->areDependentOperationsDone = false;
                Event->elseChildFinished = false;
                resetChildren(Event, Triggered);
                continue;
            }
            if(Event->loop){
                Interrupt.instruction = "";
            }
            if(Interrupt.instruction != "break"){ //operations after loop/if
                Interrupt = executeInstructions(Event->PostOperations, TriggeredLayer, Triggered, Context, TriggeredObjects,
                    Processes, StartingEvent, Event, MemoryStack, Engine
                );
                if(Interrupt.instruction == "power_off"){
                    Engine.closeProgram = true;
                    return;
                }
                else if(Interrupt.instruction == "reboot"){
                    rebooted = true;
                    return;
                }
                else if(Interrupt.instruction == "return"){
                    Interrupt.instruction = "";
                    break;
                }
                if(TriggeredLayer == nullptr || Triggered == nullptr){
                    cout << "Aborting! The owner of the event has been deleted.\n";
                    break;
                }
            }
            if(StartingEvent != Event){ //jump back in event stack
                Event->conditionalStatus = 'n';
                Event->areDependentOperationsDone = false;
                Event->elseChildFinished = false;
                resetChildren(Event, Triggered);

                Event = MemoryStack.back().Event;

                if(MemoryStack.back().contextSize <= Context.size()){
                    Context.erase(Context.begin() + MemoryStack.back().contextSize, Context.end());
                }
                else{
                    cout << "THIS SHOULD NOT HAPPENED\n";
                }
                
                MemoryStack.pop_back();
                continue;
            }
            for(i = preGeneratedContextSize; i < Context.size(); i++){
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

        if(wasDeleteExecuted && deleteEntities()){
            updateBaseOfTriggerableObjects();
            wasDeleteExecuted = false;
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
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive() || !SelectedCamera->canInteractWithMouse){
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
    auto it = camerasOrder.begin() + index;
    std::rotate(it, it + 1, camerasOrder.end());

    if(index == camerasOrder.size() - 1){
        return;
    }

    unsigned treshold = index;

    index = camerasOrder.size() - 1;
    
    for(unsigned j = 0; j < index;){
        if(camerasOrder[j] >= Cameras.size()){
            cout << "Error: Index of camera is out of scope!\n";
            j++;
            continue;
        }
        if(j >= treshold && isALeaf(Cameras[camerasOrder[j]].getID(), ChosenCamera->getID(), Cameras)){
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
                for(int l = k; l < j - 1; l++){
                    std::swap(camerasOrder[l], camerasOrder[l + 1]);
                }
                break;
            }
        }
    }
}
void ProcessClass::selectCamera(bool fromAltTab, const MouseClass & Mouse, const vector<short> & pressedKeys, const vector<short> & releasedKeys){
    if(Cameras.size() == 0){
        return;
    }
    if(isKeyPressed(ALLEGRO_KEY_LCTRL, pressedKeys) && isKeyReleased(ALLEGRO_KEY_TAB, releasedKeys) && fromAltTab){
        if(camerasOrder[0] >= Cameras.size()){
            cout << "Error: Index of camera is out of scope!\n";
            return;
        }
        SelectedCamera = &Cameras[camerasOrder[0]];
        auto it = camerasOrder.begin();
        std::rotate(it, it + 1, camerasOrder.end());

        SelectedCamera->activate();

        int i = camerasOrder.size() - 1;
            
        for(int j = 0; j < i; j++){
            if(camerasOrder[j] >= Cameras.size()){
                cout << "Error: Index of camera is out of scope!\n";
                continue;
            }
            if(Cameras[camerasOrder[j]].isForcefullyPinned &&
                Cameras[camerasOrder[j]].pinnedCameraID == SelectedCamera->getID()){
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
            cout << "Error: Index of camera is out of scope!\n";
            continue;
        }
        Camera = &Cameras[camerasOrder[i]];
        if(!Camera->getIsActive()){
            continue;
        }
        if(Mouse.inRectangle(Camera->pos, Camera->size, true, SelectedCamera)){
            SelectedCamera = Camera;

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
        && SelectedLayer != nullptr && SelectedObject != nullptr
        && Mouse.inRectangle(SelectedCamera->pos, SelectedCamera->size, true, SelectedCamera)
        && SelectedCamera->isLayerVisible(SelectedLayer->getID())
        && SelectedCamera->isLayerAccessible(SelectedLayer->getID())
    ){
        if(SelectedObject->getIsAttachedToCamera()){
            if(Mouse.inRectangle(SelectedCamera->pos + SelectedObject->getPos(false)+SelectedLayer->pos, SelectedObject->getSize(), true, SelectedCamera)){
                wasMousePressedInSelectedObject = true;
                dragStartingPos.set(Mouse.getPos()-SelectedObject->getPos(false));
            }
        }
        else{
            if(Mouse.inRectangle(SelectedCamera->pos / SelectedCamera->zoom + SelectedObject->getPos(false) + SelectedCamera->visionShift,
                SelectedObject->getSize(), false, SelectedCamera)
            ){
                wasMousePressedInSelectedObject = true;
                dragStartingPos.set(Mouse.getZoomedPos(SelectedCamera)-SelectedObject->getPos(false)-SelectedCamera->visionShift);
            }
        }
    }
}
void ProcessClass::changeCursor(ALLEGRO_DISPLAY *display, const MouseClass & Mouse){
    if(Mouse.isPressed(0)){
        return;
    }
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive() || !SelectedCamera->canInteractWithMouse){
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
}
void ProcessClass::detectStartPosOfDraggingCamera(ALLEGRO_DISPLAY *display, const MouseClass & Mouse){
    activeCameraMoveType = NONE;
    if(!Mouse.isPressed(0) || SelectedCamera == nullptr || !SelectedCamera->getIsActive() || !SelectedCamera->canInteractWithMouse){
        return;
    }
    if(Mouse.inRectangle(SelectedCamera->pos + vec2d(5.0, 0.0), vec2d(SelectedCamera->size.x - 10.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N);
        activeCameraMoveType = CAMERA_N;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos() - SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(SelectedCamera->size.x - 5.0, 0.0), vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE);
        activeCameraMoveType = CAMERA_NE;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x - SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos()-SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(SelectedCamera->size.x - 5.0, 5.0), vec2d(5.0, SelectedCamera->size.y - 10.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E);
        activeCameraMoveType = CAMERA_E;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x - SelectedCamera->size.x, Mouse.getPos().y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + SelectedCamera->size - vec2d(5.0, 5.0), vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE);
        activeCameraMoveType = CAMERA_SE;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x - SelectedCamera->size.x, Mouse.getPos().y - SelectedCamera->size.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(5.0, SelectedCamera->size.y - 5.0), vec2d(SelectedCamera->size.x - 10.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S);
        activeCameraMoveType = CAMERA_S;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x, Mouse.getPos().y-SelectedCamera->size.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(0.0, SelectedCamera->size.y - 5.0), vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW);
        activeCameraMoveType = CAMERA_SW;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y - SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos()-SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + vec2d(0.0, 5.0), vec2d(5.0, SelectedCamera->size.y - 10.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W);
        activeCameraMoveType = CAMERA_W;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos() - SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos, vec2d(5.0, 5.0), true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW);
        activeCameraMoveType = CAMERA_NW;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos().x + SelectedCamera->size.x, Mouse.getPos().y + SelectedCamera->size.y);
        dragStartingPos2.set(Mouse.getPos()-SelectedCamera->pos);
        dragLimit.set(Mouse.getPos().x + SelectedCamera->size.x - SelectedCamera->minSize.x, Mouse.getPos().y + SelectedCamera->size.y - SelectedCamera->minSize.y);
    }
    else if(Mouse.inRectangle(SelectedCamera->pos + SelectedCamera->grabbingAreaPos, SelectedCamera->grabbingAreaSize, true, SelectedCamera)){
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
        activeCameraMoveType = CAMERA_FULL;
        wasMousePressedInSelectedObject = false;
        dragStartingPos.set(Mouse.getPos()-SelectedCamera->pos);
    }
}
void ProcessClass::startScrollbarDragging(const MouseClass & Mouse){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()){
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
                if(Scrollbar.startDragging(Object.getPos(false), Mouse, SelectedCamera)){
                    return;
                }
            }
        }
    }
}
void ProcessClass::dragScrollbars(const MouseClass & Mouse){
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive()){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
                if(Scrollbar.dragThumb(Object.getPos(false), Mouse)){
                    Object.setScrollShift(Scrollbar.countScrollShift());
                }
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
                bool isScrollable = ParticleEffect.getIsScrollable();
                if(ParticleEffect.getSpawnOnKeyRelease())
                    ParticleEffect.spawnParticles(Object.getPos(isScrollable), releasedKeys);
                else
                    ParticleEffect.spawnParticles(Object.getPos(isScrollable), pressedKeys);
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
                    if(!SolidCollision.isCloseEnough(SolidObject.getPos(false), SolidObject.getID(),
                            Object.getPos(false), momentum, &Collision)){
                        continue;
                    }
                }

                Collision.detectOverlaping(
                    SolidObject.getID(),
                    SolidCollision.getID(),
                    SolidObject.getPos(false) + SolidCollision.getPos(false),
                    SolidCollision.getSize(),
                    Object.getPos(false),
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

                if(ignoreDistantObjects && !SolidCollision.isCloseEnough(SolidObject.getPos(false), SolidObject.getID(), Object.getPos(false), Movement.getMomentum(), &Collision)){
                    continue;
                }

                if(shouldCheckOverlapingOnly(Collision, SolidObject, SolidCollision)){
                    Collision.detectOverlaping(
                        SolidObject.getID(), SolidCollision.getID(), SolidObject.getPos(false) + SolidCollision.getPos(false),
                        SolidCollision.getSize(), Object.getPos(false), Movement.getMomentum()
                    );
                    continue;
                }

                Collision.detectCollision(
                    SolidObject.getID(), SolidCollision.getID(), SolidObject.getPos(false)+SolidCollision.getPos(false),
                    SolidCollision.getSize(), Object.getPos(false), Movement.getMomentum()
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
        vec2d newPos(-FollowedByCamera->getPos(false).x+Camera.size.x/2, -FollowedByCamera->getPos(false).y+Camera.size.y/2);
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
                bool pressedMouse[5];
                bool releasedMouse[5];
                Mouse.getPressed(pressedMouse);
                Mouse.getReleased(releasedMouse);
                Movement.updateStatesAndVectors(pressedKeys, pressedMouse, releasedMouse, Mouse.getZoomedPos(SelectedCamera), Object.getPos(false));
                Movement.updateMomentum(Object.getPos(false));

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
                if(!Camera.getIsActive() || !Camera.isLayerVisible(Layer.getID()))
                    continue;
                if(Camera.isFollowingObject && Camera.followedLayerID == Layer.getID() && Camera.followedObjectID == Object.getID()){
                    updateCameraPosition(Camera, &Object);
                }
            }
        }
    }
}
void ProcessClass::moveSelectedObject(const MouseClass & Mouse){
    if(SelectedLayer != nullptr && SelectedObject != nullptr && Mouse.isPressed(0) && wasMousePressedInSelectedObject){
        if(
            SelectedCamera != nullptr && SelectedCamera->getIsActive()
            && SelectedCamera->isLayerVisible(SelectedLayer->getID())
            && SelectedCamera->isLayerAccessible(SelectedLayer->getID())
        ){
            if(SelectedObject->getIsAttachedToCamera()){
                SelectedObject->setPos(Mouse.getPos()-dragStartingPos);
            }
            else{
                SelectedObject->setPos(Mouse.getZoomedPos(SelectedCamera)-SelectedCamera->visionShift-dragStartingPos);
            }
        }
    }
}
void ProcessClass::drawEverything(EngineClass & Engine){
    al_set_clipping_rectangle(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
    
    int numberOfDrawnObjects;
    unsigned int i;
    int currentlyDrawnLayer;
    Camera2D * Camera;

    for(const unsigned & cameraIndex : camerasOrder){
        if(cameraIndex >= Cameras.size()){
            cout << "Error: Index of camera is out of scope!\n";
            continue;
        }
        Camera = &Cameras[cameraIndex];
        if(!Camera->getIsActive() || (!Camera->canDrawOnCamera && !Camera->drawOneFrame)){
            continue;
        }
        Camera->drawOneFrame = false;
        al_set_target_bitmap(Camera->bitmapBuffer);
        if(Camera->canClearBitmap || Camera->clearBitmap){
            al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
        }
        for(LayerClass & Layer : Layers){
            if(!Layer.getIsActive() || !Camera->isLayerVisible(Layer.getID())){
                continue;
            }
            
            numberOfDrawnObjects = 0;
            foregroundOfObjects.clear();

            for(currentlyDrawnLayer = 0; currentlyDrawnLayer < totalNumberOfBitmapLayers; currentlyDrawnLayer++){
                for(i = 0; i < Layer.Objects.size(); i++){
                    if(Layer.Objects[i].getIsActive()){
                        drawModules(Layer.Objects[i], i, *Camera, Engine.FontContainer, currentlyDrawnLayer, numberOfDrawnObjects, foregroundOfObjects, false, Engine.displaySize);
                    }
                }
            }
            for(i = 0; i < foregroundOfObjects.size(); i++){
                if(Layer.Objects[foregroundOfObjects[i]].getIsActive()){
                    drawModules(Layer.Objects[foregroundOfObjects[i]], i, *Camera, Engine.FontContainer, -1, numberOfDrawnObjects, foregroundOfObjects, true, Engine.displaySize);
                }
            }
            if(SelectedLayer == &Layer){
                drawSelectionBorder(*Camera);
            }
        }
        al_set_target_backbuffer(Engine.display);
        //al_draw_bitmap(Camera->bitmapBuffer, Camera->pos.x, Camera->pos.y, 0);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_tinted_bitmap(Camera->bitmapBuffer, al_map_rgba_f(Camera->tint[0], Camera->tint[1], Camera->tint[2], Camera->tint[3]),
            windowPos.x + Camera->pos.x, windowPos.y + Camera->pos.y, 0);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        
        foregroundOfObjects.clear();
        //vector<unsigned int>().swap(foregroundOfObjects);
        //foregroundOfObjects.shrink_to_fit();
        if(drawCameraBorders && Camera->allowsDrawingBorders){
            if(SelectedCamera != Camera){
                al_draw_rectangle(Camera->pos.x, Camera->pos.y, Camera->pos.x + Camera->size.x, Camera->pos.y + Camera->size.y, al_map_rgb(0, 155, 145), 6);
            }
            else{
                al_draw_rectangle(Camera->pos.x, Camera->pos.y, Camera->pos.x + Camera->size.x, Camera->pos.y + Camera->size.y, al_map_rgb(62, 249, 239), 6);
            }
        }
        if(Engine.Mouse.isPressed(0)){
            //al_draw_rectangle(Engine.Mouse.getPressedPos().x, Engine.Mouse.getPressedPos().y,
            //    Engine.Mouse.getPos().x, Engine.Mouse.getPos().y, al_map_rgb(200, 200, 200), 1);
        }
    }

    al_reset_clipping_rectangle();
}
void ProcessClass::drawModules(AncestorObject & Object, unsigned int iteration, Camera2D & Camera, vector <SingleFont> & FontContainer, int currentlyDrawnLayer, int & numberOfDrawnObjects,
                             vector <unsigned int> & foregroundOfObjects, bool isTimeForForeground, vec2i displaySize){
    vec2d newPos;
    vec2d objectSize;
    vec2d scaledObjectSize;
    bool isScrollable;
    bool wasForegroundUpdated = false; //If any module is already drawn in the foreground, don't add its object again.


    for(ImageModule & Image : Object.ImageContainer){
        if(!Image.getIsActive()){
            continue;
        }
        if(Image.usedBitmapLayer == -1 && !isTimeForForeground && currentlyDrawnLayer == 0){
            if(wasForegroundUpdated)
                continue;
            foregroundOfObjects.push_back(iteration);
            wasForegroundUpdated = true;
            continue;
        }
        if(Image.usedBitmapLayer != currentlyDrawnLayer)
            continue;
        isScrollable = Image.getIsScrollable();
        //Image.drawFrame(x, y);
        if(drawOnlyVisibleObjects && !Image.getIsAttachedToCamera()){
            newPos.set(Object.getPos(isScrollable));
            newPos.translate(Image.getPos(false));
            objectSize.set(Image.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Image.getScale());
            if(!Image.getIsScaledFromCenter())
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            else
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize)){
                continue;
            }
                
        }
        Image.drawImage(Object.getPos(isScrollable), Camera, false);
        numberOfDrawnObjects++;
    }
    for(TextModule & Text : Object.TextContainer){
        if(!Text.getIsActive()){
            continue;
        }
        if(Text.usedBitmapLayer == -1 && !isTimeForForeground && currentlyDrawnLayer == 0){
            if(wasForegroundUpdated)
                continue;
            foregroundOfObjects.push_back(iteration);
            wasForegroundUpdated = true;
            continue;
        }

        if(Text.usedBitmapLayer != currentlyDrawnLayer)
            continue;

        isScrollable = Text.getIsScrollable();
        if(drawOnlyVisibleObjects && !Text.getIsAttachedToCamera()){
            newPos.set(Object.getPos(isScrollable));
            newPos.translate(Text.getPos(false));
            objectSize.set(Text.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Text.getScale());
            if(!Text.getIsScaledFromCenter())
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            else
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize))
                continue;
        }

        //If a font exists in FontContainer, draw the text on screen.
        for(SingleFont font : FontContainer){
            if(Text.getFontID() == font.ID){
                Text.drawText(Object.getPos(isScrollable), font.font, drawTextFieldBorders, Camera, 0, 0, false);
                break;
            }
        }

        numberOfDrawnObjects++;
    }

    for(EditableTextModule & Editable : Object.EditableTextContainer){
        if(!Editable.getIsActive()){
            continue;
        }
        if(Editable.usedBitmapLayer == -1 && !isTimeForForeground && currentlyDrawnLayer == 0){
            if(wasForegroundUpdated)
                continue;
            foregroundOfObjects.push_back(iteration);
            wasForegroundUpdated = true;
            continue;
        }


        if(Editable.usedBitmapLayer != currentlyDrawnLayer)
            continue;

        isScrollable = Editable.getIsScrollable();
        if(drawOnlyVisibleObjects && !Editable.getIsAttachedToCamera()){
            newPos.set(Object.getPos(isScrollable));
            newPos.translate(Editable.getPos(false));
            objectSize.set(Editable.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Editable.getScale());
            if(!Editable.getIsScaledFromCenter())
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            else
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize))
                continue;
        }

        //If a font exists in FontContainer, draw the text on screen.
        for(auto font : FontContainer){
            if(Editable.getFontID() == font.ID){
                Editable.drawText(Object.getPos(isScrollable), font.font,
                    drawTextFieldBorders, Camera, Editable.getCursorPos(), Editable.secondCursorPos, Editable.getEditingIsActive());
            }
        }
        numberOfDrawnObjects++;
    }

    for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
        if(!Scrollbar.getIsActive()){
            continue;
        }
        if(Scrollbar.getUsedBitmapLayer() == -1 && !isTimeForForeground && currentlyDrawnLayer == 0){
            if(wasForegroundUpdated)
                continue;
            foregroundOfObjects.push_back(iteration);
            wasForegroundUpdated = true;
            continue;
        }
        if(Scrollbar.getUsedBitmapLayer() != currentlyDrawnLayer)
            continue;

        isScrollable = Scrollbar.getIsScrollable();
        if(drawOnlyVisibleObjects && !Scrollbar.getIsAttachedToCamera()){
            newPos.set(Object.getPos(isScrollable));
            newPos.translate(Scrollbar.getPos(false));
            objectSize.set(Scrollbar.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Scrollbar.getScale());
            if(!Scrollbar.getIsScaledFromCenter())
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            else
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize))
                continue;
        }
        Scrollbar.draw(Object.getPos(isScrollable), Object.ImageContainer, Camera);
        numberOfDrawnObjects++;
    }

    for(PrimitivesModule & Primitives : Object.PrimitivesContainer){
        if(!Primitives.getIsActive()){
            continue;
        }
        if(Primitives.getUsedBitmapLayer() == -1 && !isTimeForForeground && currentlyDrawnLayer == 0){
            if(wasForegroundUpdated)
                continue;
            foregroundOfObjects.push_back(iteration);
            wasForegroundUpdated = true;
            continue;
        }
        if(Primitives.getUsedBitmapLayer() != currentlyDrawnLayer)
            continue;

        isScrollable = Primitives.getIsScrollable();
        if(drawOnlyVisibleObjects && !Primitives.getIsAttachedToCamera()){
            newPos.set(Object.getPos(isScrollable));
            newPos.translate(Primitives.getPos(false));
            objectSize.set(Primitives.getSize());
            scaledObjectSize.set(objectSize);
            scaledObjectSize.multiply(Primitives.getScale());
            if(!Primitives.getIsScaledFromCenter())
                newPos.set(newPos.x+scaledObjectSize.x/2, newPos.y+scaledObjectSize.y/2);
            else
                newPos.set(newPos.x+objectSize.x/2, newPos.y+objectSize.y/2);

            if(!Camera.isOnScreenWithRadius(newPos, scaledObjectSize))
                continue;
        }
        Primitives.draw(Object.getPos(isScrollable), Camera, false);
        numberOfDrawnObjects++;
    }

    if(drawHitboxes){
        vec2d hitboxSize;
        for(CollisionModule & Hitbox : Object.CollisionContainer){
            if(!Hitbox.getIsActive()){
                continue;
            }
            newPos = Object.getPos(false) + Hitbox.getPos(false);

           
            newPos.set(Camera.translateWithZoom(newPos));
            hitboxSize = Hitbox.getSize();
            hitboxSize.multiply(Camera.zoom);
            
            vec4d borderColor(0, 0, 255, 127);
            for(DetectedCollision Detection : Hitbox.Detected){
                if(Detection.collisionType > 0){
                    borderColor.val[1] = 255;
                    borderColor.val[2] = 0;
                    break;
                }
            }
            al_draw_rectangle(newPos.x, newPos.y, newPos.x+hitboxSize.x, newPos.y+hitboxSize.y, al_map_rgba(borderColor.val[0], borderColor.val[1], borderColor.val[2], borderColor.val[3]), 2);
        }
    }

    if(isTimeForForeground)
        return;

    for(ParticleEffectModule & ParticleEffect : Object.ParticlesContainer){
        if(!ParticleEffect.getIsActive()){
            continue;
        }
        if(ParticleEffect.usedBitmapLayer != currentlyDrawnLayer)
            continue;
        
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
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()){
        return;
    }
    if(Engine.Mouse.isPressed()){
        vec2d finalPos, finalSize;
        bool isScrollable;
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
                        isScrollable = TextField.getIsScrollable();
                        finalPos.set(Object.getPos(isScrollable));
                        finalPos.translate(TextField.getPos(false));
                        if(!TextField.getIsAttachedToCamera()){
                            finalPos.translate(SelectedCamera->pos / SelectedCamera->zoom + SelectedCamera->visionShift);
                        }
                        else{
                            finalPos.translate(SelectedCamera->pos);
                        }
                        finalSize.set(TextField.getSize());
                        finalSize.multiply(TextField.getScale());
                        if(Engine.Mouse.firstPositionInRectangle(finalPos, finalSize, 0, TextField.getIsAttachedToCamera(), SelectedCamera)){
                            TextField.setEditingIsActive(true);
                            TextField.setCursorPos(finalPos, finalSize, Engine.FontContainer, Engine.Mouse, *SelectedCamera);
                            if(Engine.Mouse.firstPressedInRectangle(finalPos, finalSize, 0, TextField.getIsAttachedToCamera(), SelectedCamera)){
                                TextField.secondCursorPos = TextField.cursorPos;
                            }
                            
                            //TextField.setCursorPos(TextField.getCurrentContent().size());
                            continue;
                        }
                    }
                    if(TextField.getEditingIsActive()){
                        Object.operateTextFieldUpdate(TextField, Layer.Objects, Engine.BitmapContainer, Layer.objectsIDs, EXE_PATH);
                    }
                    TextField.setEditingIsActive(false);
                    if(SelectedLayer != nullptr && SelectedObject != nullptr && Object.getID() == "editor_window"){
                        Object.EditableTextContainer[0].modifyContent(0, SelectedObject->getID());
                    }
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
                    Object.operateTextFieldUpdate(TextField, Layer.Objects, Engine.BitmapContainer, Layer.objectsIDs, EXE_PATH);
                    TextField.setUpdateConnectedVariable(false);

                    if(SelectedLayer != nullptr && SelectedObject != nullptr && Object.getID() == "editor_window"){
                        Object.EditableTextContainer[0].modifyContent(0, SelectedObject->getID());
                    }
                }
            }
        }
    }
}
void ProcessClass::selectObject(const MouseClass & Mouse, vector <SingleBitmap> & BitmapContainer, vector <SingleFont> & FontContainer){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()){
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
            if(!Object.getCanBeSelected()){
                continue;
            }
            if(Mouse.pressedInRectangle(Object.getPosOnCamera(SelectedCamera), Object.getSize(), 0, Object.getIsAttachedToCamera(), SelectedCamera)){
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
    }
}
void ProcessClass::unselectObject(){
    selectedObjectLayerID = "";
    selectedObjectID = "";
    SelectedLayer = nullptr;
    SelectedObject = nullptr;
}

void ProcessClass::updateEditorWindowOnSelection(vector <EditableTextModule> & EditableTextContainer){
    for(EditableTextModule & Text : EditableTextContainer){
        Text.setConnectedObjectID(SelectedObject->getID());
    }
    EditableTextContainer[0].modifyContent(0, SelectedObject->getID());
    EditableTextContainer[1].modifyContent(0, std::to_string(SelectedObject->getPos(false).x));
    EditableTextContainer[2].modifyContent(0, std::to_string(SelectedObject->getPos(false).y));
    EditableTextContainer[3].modifyContent(0, std::to_string(SelectedObject->getSize().x));
    EditableTextContainer[4].modifyContent(0, std::to_string(SelectedObject->getSize().y));
    EditableTextContainer[5].modifyContent(0, std::to_string(SelectedObject->getIsScaledFromCenter()));
    EditableTextContainer[6].modifyContent(0, std::to_string(SelectedObject->getIsAttachedToCamera()));
}
//Updates coordinates of the selected object in the editor's text fields. 
void ProcessClass::updateEditorWindowOnAxisChange(vector <EditableTextModule> & EditableTextContainer){
    if(EditableTextContainer.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": Editable text container is empty.\n";
        return;
    }
    if(!EditableTextContainer[0].getIsActive()){
        return;
    }

    EditableTextContainer[1].modifyContent(0, std::to_string(SelectedObject->getPos(false).x));
    EditableTextContainer[2].modifyContent(0, std::to_string(SelectedObject->getPos(false).y));
}

void ProcessClass::drawSelectionBorder(Camera2D Camera){
    if(SelectedObject == nullptr){
        return;
    }
    vec2d borderPos(SelectedObject->getPos(false));
    vec2d borderSize(SelectedObject->getSize());
    if(!SelectedObject->getIsAttachedToCamera()){
        borderSize.multiply(Camera.zoom);
        borderPos.set(Camera.translateWithZoom(borderPos));
        al_draw_rectangle(borderPos.x, borderPos.y, borderPos.x + borderSize.x, borderPos.y + borderSize.y, al_map_rgb(80, 154, 213), 6*Camera.zoom);
    }
    else{
        al_draw_rectangle(borderPos.x, borderPos.y, borderPos.x + borderSize.x, borderPos.y + borderSize.y, al_map_rgb(216, 78, 213), 6);
    }
}

EditorWindowArrangement::EditorWindowArrangement(){
    windowWidth = 500.0;
    buttonSize.set(42.0, 42.0);
    margin.set(20.0, 20.0);
    buttonMargin = 10.0;
    labelFontID = "Minecraft24";
    attributeFontID = "Minecraft24";
    labelHeight = 0;
    attributeStart = 150.0;
    labelMargin = 10.0;
    attributeMargin = 30.0;
    attributePadding.set(0.0, 8.0);
    attributeSize.set(windowWidth-margin.x*2.0, 0);
    editablePadding.set(8.0, 2.0);
    containerHeight = 45;
    wraperSize.set(40.0, 40.0);
    wraperMargin = 3.0;

    labelMoveAhead = 6.0;
    attributeMoveBack = 8.0;
    indentation = 0.0;

    scrollbarMargin.set(30.0, 30.0);

    thumbSize.set(32.0, 32.0);
}

char getActiveEditorWindowCategory(AncestorObject * EditorWindow){
    for(TextModule & Text : EditorWindow->TextContainer){
        if(Text.getIsActive() && isCharInGroup(Text.getID()[0], 9, "1", "2", "3", "4", "5", "6", "7", "8", "9")){
            return Text.getID()[0];
        }
    }
    return '0';
}
void removeListsInEditorWindow(AncestorObject * EditorWindow){
    for(unsigned int i = 0; i < EditorWindow->TextContainer.size(); i++){
        if(isCharInGroup(EditorWindow->TextContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->TextContainer.resize(i, TextModule());
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->ImageContainer.size(); i++){
        if(isCharInGroup(EditorWindow->ImageContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->ImageContainer.resize(i, ImageModule());
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->EditableTextContainer.size(); i++){
        if(isCharInGroup(EditorWindow->EditableTextContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->EditableTextContainer.resize(i, EditableTextModule());
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->CollisionContainer.size(); i++){
        if(isCharInGroup(EditorWindow->CollisionContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->CollisionContainer.resize(i, CollisionModule());
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->VariablesContainer.size(); i++){
        if(isCharInGroup(EditorWindow->VariablesContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->VariablesContainer.resize(i, VariableModule());
            break;
        }
    }
}

/*
void prepareEditorWindow(vector <AncestorObject> & Objects, string layerID, vector<string> &listOfIDs, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer){
    int SCREEN_W = 1600, SCREEN_H = 900;
    EditorWindowArrangement Arr;
    Arr.labelHeight = getFontHeight(FontContainer, Arr.labelFontID);
    Arr.attributeSize.y = getFontHeight(FontContainer, Arr.attributeFontID);

    Objects.push_back(AncestorObject(Objects.size(), listOfIDs, layerID));
    AncestorObject * EditorWindow = &Objects.back();

    EditorWindow->setID("editor_window", listOfIDs);
    EditorWindow->setPos(SCREEN_W-Arr.windowWidth, 0.0);
    EditorWindow->setCanBeSelected(false);
    EditorWindow->setIsAttachedToCamera(true);

    EditorWindow->ScrollbarContainer.push_back(ScrollbarModule(0, &EditorWindow->scrollbarContainerIDs, layerID, EditorWindow->getID()));
    EditorWindow->ScrollbarContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ScrollbarContainer.back().setPos(vec2d(Arr.windowWidth-45.0, Arr.buttonSize.y+Arr.margin.y+Arr.buttonMargin));
    EditorWindow->ScrollbarContainer.back().setThumbPos(vec2d(0.0, 0.0));
    EditorWindow->ScrollbarContainer.back().setThumbSize(Arr.thumbSize);
    EditorWindow->ScrollbarContainer.back().setScrollingArea(vec2d(Arr.thumbSize.x, SCREEN_H-Arr.scrollbarMargin.y-Arr.margin.y-Arr.buttonSize.y));
    EditorWindow->ScrollbarContainer.back().setRealScrollingArea(vec2d(0.0, 0.0));

    EditorWindow->ScrollbarContainer.back().setThumbImageID("thumb_1");
    EditorWindow->ScrollbarContainer.back().setTrackImageID("thumb_track_1");
    EditorWindow->ScrollbarContainer.back().setUsedBitmapLayer(-1);


    EditorWindow->VariablesContainer.push_back(VariableModule("true", &EditorWindow->variablesContainerIDs, layerID, EditorWindow->getID()));
    EditorWindow->VariablesContainer.back().setDefaultBool(true);

    EditorWindow->ImageContainer.push_back(ImageModule("background", &EditorWindow->imageContainerIDs, layerID, EditorWindow->getID()));
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "gui_background");
    EditorWindow->ImageContainer.back().setPos(0.0, 0.0);
    EditorWindow->ImageContainer.back().resize(vec2d(Arr.windowWidth, SCREEN_H));
    EditorWindow->TextContainer.push_back(TextModule(0, &EditorWindow->textContainerIDs, layerID, EditorWindow->getID()));
    EditorWindow->EventsContainer.push_back(0);
    for(unsigned int i = 1; i <= 9; i++){
        EditorWindow->ImageContainer.push_back(ImageModule("icon", &EditorWindow->imageContainerIDs, layerID, EditorWindow->getID()));
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x + (i-1) * (Arr.buttonSize.x+Arr.buttonMargin), Arr.margin.y);

        EditorWindow->TextContainer.push_back(TextModule("l_" + std::to_string(i), &EditorWindow->textContainerIDs, layerID, EditorWindow->getID()));
        EditorWindow->TextContainer.back().setIsAttachedToCamera(true);
        EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->TextContainer.back().setPos(Arr.margin.x, Arr.margin.y+70.0);
        EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
        EditorWindow->TextContainer.back().deactivate();

        EditorWindow->EventsContainer.push_back(i);
        EditorWindow->EventsContainer.back().setID("c_" + std::to_string(i), EditorWindow->eventsContainerIDs);
        EditorWindow->EventsContainer.back().setIsAttachedToCamera(true);
        EditorWindow->EventsContainer.back().setUpButton(vec2d(Arr.margin.x + (i-1) * (Arr.buttonSize.x+Arr.buttonMargin), Arr.margin.y), Arr.buttonSize);
        EditorWindow->EventsContainer.back().addOperation("", "mouse_release", "left", "editor_window", true, "function", "", "deactivate_all", vecXd(), vecXs());
        EditorWindow->EventsContainer.back().addOperation("", "mouse_release", "left", "editor_window", true, "function", std::to_string(i), "activate_based_on_first_char", vecXd(), vecXs());
        EditorWindow->EventsContainer.back().addOperation("", "mouse_release", "left", "editor_window", true, "function", "l_" + std::to_string(i), "activate_based_on_id", vecXd(), vecXs());
    }

    EditorWindow->ImageContainer[1].connectBitmap(BitmapContainer, "icon_general");
    EditorWindow->TextContainer[1].addNewContentAndResize("General", FontContainer);
    EditorWindow->TextContainer[1].activate();

    prepareEditorWindowGeneral(EditorWindow, FontContainer, BitmapContainer, Arr);

    EditorWindow->ImageContainer[2].connectBitmap(BitmapContainer, "icon_image");
    EditorWindow->TextContainer[2].addNewContentAndResize("Images", FontContainer);

    EditorWindow->ImageContainer[3].connectBitmap(BitmapContainer, "icon_text");
    EditorWindow->TextContainer[3].addNewContentAndResize("Text", FontContainer);
    EditorWindow->ImageContainer[4].connectBitmap(BitmapContainer, "icon_movement");
    EditorWindow->TextContainer[4].addNewContentAndResize("Movements", FontContainer);
    EditorWindow->ImageContainer[5].connectBitmap(BitmapContainer, "icon_collision");
    EditorWindow->TextContainer[5].addNewContentAndResize("Collisions", FontContainer);
    EditorWindow->ImageContainer[6].connectBitmap(BitmapContainer, "icon_event");
    EditorWindow->TextContainer[6].addNewContentAndResize("Events", FontContainer);
    EditorWindow->ImageContainer[7].connectBitmap(BitmapContainer, "icon_particles");
    EditorWindow->TextContainer[7].addNewContentAndResize("Particles", FontContainer);
    EditorWindow->ImageContainer[8].connectBitmap(BitmapContainer, "icon_variables");
    EditorWindow->TextContainer[8].addNewContentAndResize("Variables", FontContainer);
    EditorWindow->ImageContainer[9].connectBitmap(BitmapContainer, "icon_editable_text");
    EditorWindow->TextContainer[9].addNewContentAndResize("Editable Text", FontContainer);

    for(unsigned int i = 1; i <= 9; i++){
        EditorWindow->ImageContainer[i].resize(Arr.buttonSize);
    }

    EditorWindow->ImageContainer.push_back(ImageModule("thumb_1", &EditorWindow->imageContainerIDs, layerID, EditorWindow->getID()));
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "thumb_1");
    EditorWindow->ImageContainer.back().resize(EditorWindow->ScrollbarContainer.back().getThumbSize());
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().deactivate();

    EditorWindow->ImageContainer.push_back(ImageModule("thumb_track_1", &EditorWindow->imageContainerIDs, layerID, EditorWindow->getID()));
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "thumb_track_1");
    EditorWindow->ImageContainer.back().resize(EditorWindow->ScrollbarContainer.back().getScrollingArea());
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().deactivate();

    cout << "Editor Window:\n-Text: " << EditorWindow->TextContainer.size() << "\n-Image: " << EditorWindow->ImageContainer.size() << "\n-Editable: " << EditorWindow->EditableTextContainer.size() << "\n-Events: " << EditorWindow->EventsContainer.size() << "\n";
}

void prepareEditorWindowGeneral(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    unsigned int labelIndex = EditorWindow->TextContainer.size();
    unsigned int attrIndex = EditorWindow->EditableTextContainer.size();
    for(unsigned int attr = 0; attr < 7; attr++){
        EditorWindow->TextContainer.push_back(TextModule(1, &EditorWindow->textContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->TextContainer.back().setIsAttachedToCamera(true);;
        EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->TextContainer.back().setPos(Arr.margin.x, Arr.attributeStart + attr*(Arr.labelHeight + Arr.labelMargin+Arr.attributeSize.y+Arr.attributeMargin));
        EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
        EditorWindow->TextContainer.back().setIsScrollable(true);

        EditorWindow->EditableTextContainer.push_back(EditableTextModule(1, &EditorWindow->editableTextContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->EditableTextContainer.back().setFontID(Arr.attributeFontID);
        EditorWindow->EditableTextContainer.back().setPos(
            Arr.margin.x+Arr.editablePadding.x,
            Arr.attributeStart + Arr.labelHeight + Arr.labelMargin + Arr.editablePadding.y + attr*(Arr.labelHeight + Arr.labelMargin+Arr.attributeSize.y+Arr.attributeMargin)
        );
        EditorWindow->EditableTextContainer.back().setSize(Arr.attributeSize);
        EditorWindow->EditableTextContainer.back().addNewContent("");
        EditorWindow->EditableTextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->EditableTextContainer.back().setVisibility(1.0);
        EditorWindow->EditableTextContainer.back().setIsScrollable(true);

        EditorWindow->ImageContainer.push_back(ImageModule(1, &EditorWindow->imageContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "text_field_1");
        EditorWindow->ImageContainer.back().setPos(
            Arr.margin.x,
            Arr.attributeStart + Arr.labelHeight + Arr.labelMargin + attr*(Arr.labelHeight + Arr.labelMargin+Arr.attributeSize.y+Arr.attributeMargin)-Arr.attributePadding.y
        );
        EditorWindow->ImageContainer.back().resize(Arr.attributeSize+Arr.attributePadding*2.0);
        EditorWindow->ImageContainer.back().setIsScrollable(true);
    }
    EditorWindow->TextContainer[labelIndex].addNewContentAndResize("Id:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex].setIsNumerical(false);
    EditorWindow->EditableTextContainer[attrIndex].setCanUseSpace(true);
    EditorWindow->EditableTextContainer[attrIndex].setConnectedObject("", "ancestor", "", "id");
    EditorWindow->TextContainer[labelIndex+1].addNewContentAndResize("Position X:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex+1].setConnectedObject("", "ancestor", "", "position_x");
    EditorWindow->TextContainer[labelIndex+2].addNewContentAndResize("Position Y:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex+2].setConnectedObject("", "ancestor", "", "position_y");
    EditorWindow->TextContainer[labelIndex+3].addNewContentAndResize("Size X:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex+3].setConnectedObject("", "ancestor", "", "size_x");
    EditorWindow->TextContainer[labelIndex+4].addNewContentAndResize("Size Y:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex+4].setConnectedObject("", "ancestor", "", "size_y");
    EditorWindow->TextContainer[labelIndex+5].addNewContentAndResize("Scaled from center:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex+5].setConnectedObject("", "ancestor", "", "is_scaled_from_center");
    EditorWindow->TextContainer[labelIndex+6].addNewContentAndResize("Attached to camera:", FontContainer);
    EditorWindow->EditableTextContainer[attrIndex+6].setConnectedObject("", "ancestor", "", "is_attached_to_camera");
}

void ProcessClass::prepareEditorWindowObjectsList(int categoryIndex, AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){

    unsigned int containerSize = 0;

    EditorWindow->TextContainer.push_back(TextModule(categoryIndex, &EditorWindow->textContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
    EditorWindow->TextContainer.back().setIsAttachedToCamera(true);;
    EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
    EditorWindow->TextContainer.back().setPos(Arr.margin.x+Arr.wraperSize.x+Arr.wraperMargin, Arr.attributeStart + Arr.wraperSize.y/2 - Arr.labelHeight/2 + Arr.editablePadding.y - Arr.wraperSize.y/2);
    EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
    if(categoryIndex == 2){
        containerSize = SelectedObject->ImageContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add image", FontContainer);
    }
    else if(categoryIndex == 3){
        containerSize = SelectedObject->TextContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add text", FontContainer);
    }
    else if(categoryIndex == 4){
        containerSize = SelectedObject->MovementContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add movement", FontContainer);
    }
    else if(categoryIndex == 5){
        containerSize = SelectedObject->CollisionContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add collision", FontContainer);
    }
    else if(categoryIndex == 6){
        containerSize = SelectedObject->EventsContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add event", FontContainer);
    }
    else if(categoryIndex == 7){
        containerSize = SelectedObject->ParticlesContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add particle", FontContainer);
    }
    else if(categoryIndex == 8){
        containerSize = SelectedObject->VariablesContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add variable", FontContainer);
    }
    else if(categoryIndex == 9){
        containerSize = SelectedObject->EditableTextContainer.size();
        EditorWindow->TextContainer.back().addNewContentAndResize("Add editable text", FontContainer);
    }
    EditorWindow->TextContainer.back().setIsScrollable(true);
    EditorWindow->TextContainer.back().deactivate();


    EditorWindow->ImageContainer.push_back(ImageModule(categoryIndex, &EditorWindow->imageContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
    EditorWindow->ImageContainer.back().setPos(Arr.margin.x, Arr.attributeStart-Arr.wraperSize.y/2);
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "plus_1");
    EditorWindow->ImageContainer.back().resize(Arr.wraperSize);
    EditorWindow->ImageContainer.back().setIsScrollable(true);
    EditorWindow->ImageContainer.back().deactivate();

    unsigned int objNumber = 1;

    for(unsigned int vectorNumber = objNumber; vectorNumber <= containerSize; vectorNumber++){
        EditorWindow->TextContainer.push_back(TextModule(categoryIndex, &EditorWindow->textContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->TextContainer.back().setIsAttachedToCamera(true);;
        EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->TextContainer.back().setPos(Arr.margin.x+Arr.wraperSize.x+Arr.wraperMargin+Arr.editablePadding.x, Arr.attributeStart + objNumber * Arr.containerHeight + Arr.wraperSize.y/2 - Arr.labelHeight/2 + Arr.editablePadding.y);
        EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
        if(categoryIndex == 2){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->ImageContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 3){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->TextContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 4){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->MovementContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 5){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->CollisionContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 6){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->EventsContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 7){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->ParticlesContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 8){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->VariablesContainer[vectorNumber-1].getID(), FontContainer);
        }
        else if(categoryIndex == 9){
            EditorWindow->TextContainer.back().addNewContentAndResize(SelectedObject->EditableTextContainer[vectorNumber-1].getID(), FontContainer);
        }
        EditorWindow->TextContainer.back().setIsScrollable(true);
        EditorWindow->TextContainer.back().deactivate();

        string wrapperID = std::to_string(categoryIndex) + "_wrapper_" + std::to_string(vectorNumber);

        EditorWindow->ImageContainer.push_back(ImageModule(wrapperID, &EditorWindow->imageContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x, Arr.attributeStart + objNumber * Arr.containerHeight);
        EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "arrow_1");
        EditorWindow->ImageContainer.back().resize(Arr.wraperSize);
        EditorWindow->ImageContainer.back().setIsScrollable(true);
        EditorWindow->ImageContainer.back().deactivate();


        EditorWindow->EventsContainer.push_back(categoryIndex);
        EditorWindow->EventsContainer.back().setIsAttachedToCamera(true);
        EditorWindow->EventsContainer.back().setUpButton(vec2d(Arr.margin.x, Arr.attributeStart + objNumber * Arr.containerHeight), Arr.wraperSize);
        EditorWindow->EventsContainer.back().addOperation("", "mouse_release", "left", "editor_window", true, "variables", wrapperID, "toggle_bool", vecXd(), vecXs());
        EditorWindow->EventsContainer.back().addOperation("", "mouse_release", "left", "editor_window", true, "function", "", "manage_wrap", vecXd(3, (double)categoryIndex, (double)(vectorNumber-1), (double)Arr.containerHeight), vecXs());
        EditorWindow->EventsContainer.back().addOperation("true", "==", wrapperID, "editor_window", true, "image", EditorWindow->ImageContainer.back().getID(), "set_rotation", vecXd(1, 90.0), vecXs());
        EditorWindow->EventsContainer.back().addOperation("true", "!=", wrapperID, "editor_window", true, "image", EditorWindow->ImageContainer.back().getID(), "set_rotation", vecXd(1, 0.0), vecXs());


        EditorWindow->EventsContainer.back().addOperation("true", "==", wrapperID, "editor_window", true, "scrollbar", EditorWindow->ScrollbarContainer[0].getID(), "add_real_area", vecXd(2, 0.0, -Arr.containerHeight), vecXs());
        EditorWindow->EventsContainer.back().addOperation("true", "!=", wrapperID, "editor_window", true, "scrollbar", EditorWindow->ScrollbarContainer[0].getID(), "add_real_area", vecXd(2, 0.0, Arr.containerHeight), vecXs());


        EditorWindow->EventsContainer.back().setIsScrollable(true);
        EditorWindow->EventsContainer.back().deactivate();

        EditorWindow->VariablesContainer.push_back(VariableModule(wrapperID, &EditorWindow->variablesContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->VariablesContainer.back().setDefaultBool(false);


        EditorWindow->ImageContainer.push_back(ImageModule(categoryIndex, &EditorWindow->imageContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x+Arr.wraperSize.x, Arr.attributeStart + objNumber * Arr.containerHeight);
        EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "text_field_2");
        EditorWindow->ImageContainer.back().resize(Arr.windowWidth-Arr.wraperSize.x*3-Arr.scrollbarMargin.x, Arr.wraperSize.y);
        EditorWindow->ImageContainer.back().setIsScrollable(true);
        EditorWindow->ImageContainer.back().deactivate();

        EditorWindow->ImageContainer.push_back(ImageModule(categoryIndex, &EditorWindow->imageContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x+Arr.wraperSize.x+Arr.windowWidth-Arr.wraperSize.x*3-Arr.scrollbarMargin.x, Arr.attributeStart + objNumber * Arr.containerHeight);
        EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "minus_1");
        EditorWindow->ImageContainer.back().resize(Arr.wraperSize.x, Arr.wraperSize.y);
        EditorWindow->ImageContainer.back().setIsScrollable(true);
        EditorWindow->ImageContainer.back().deactivate();

        objNumber++;

        unsigned int attributesCount = 2*2;

        unsigned int attrIndex = EditorWindow->EditableTextContainer.size();

        unsigned int labelIndex = EditorWindow->TextContainer.size();

        for(unsigned int attr = objNumber; attr < objNumber+attributesCount; attr+=2){
            EditorWindow->TextContainer.push_back(TextModule(categoryIndex, &EditorWindow->textContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
            EditorWindow->TextContainer.back().setIsAttachedToCamera(true);;
            EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
            EditorWindow->TextContainer.back().setPos(Arr.margin.x + Arr.indentation, Arr.attributeStart + Arr.labelMoveAhead + attr * Arr.containerHeight);
            EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
            EditorWindow->TextContainer.back().setIsScrollable(true);
            EditorWindow->TextContainer.back().deactivate();

            EditorWindow->EditableTextContainer.push_back(EditableTextModule(categoryIndex, &EditorWindow->editableTextContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
            EditorWindow->EditableTextContainer.back().setFontID(Arr.attributeFontID);
            EditorWindow->EditableTextContainer.back().setPos(Arr.margin.x + Arr.indentation + Arr.editablePadding.x, Arr.attributeStart + Arr.editablePadding.y + (attr+1) * Arr.containerHeight);
            EditorWindow->EditableTextContainer.back().setSize(Arr.attributeSize.x-Arr.scrollbarMargin.x, Arr.attributeSize.y);
            EditorWindow->EditableTextContainer.back().addNewContent("");
            EditorWindow->EditableTextContainer.back().setUsedBitmapLayer(-1);
            EditorWindow->EditableTextContainer.back().setVisibility(1.0);
            EditorWindow->EditableTextContainer.back().setIsScrollable(true);
            EditorWindow->EditableTextContainer.back().deactivate();

            EditorWindow->ImageContainer.push_back(ImageModule(categoryIndex, &EditorWindow->imageContainerIDs, EditorWindow->getLayerID(), EditorWindow->getID()));
            EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
            EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
            EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "text_field_1");
            EditorWindow->ImageContainer.back().setPos(Arr.margin.x + Arr.indentation, Arr.attributeStart - Arr.attributeMoveBack + (attr+1) * Arr.containerHeight);
            EditorWindow->ImageContainer.back().resize(Arr.attributeSize.x+Arr.attributePadding.x*2.0-Arr.wraperSize.x-Arr.scrollbarMargin.x, Arr.attributeSize.y+Arr.attributePadding.y*2.0);
            EditorWindow->ImageContainer.back().setIsScrollable(true);
            EditorWindow->ImageContainer.back().deactivate();
        }

        //objNumber += attributesCount-1;
        //objNumber++;

        EditorWindow->TextContainer[labelIndex].addNewContentAndResize("Id:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex].setIsNumerical(false);
        EditorWindow->EditableTextContainer[attrIndex].setCanUseSpace(true);
        EditorWindow->EditableTextContainer[attrIndex].setConnectedObject("", "ancestor", "", "id");
        EditorWindow->TextContainer[labelIndex+1].addNewContentAndResize("Position X:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+1].setConnectedObject("", "ancestor", "", "position_x");
        EditorWindow->TextContainer[labelIndex+2].addNewContentAndResize("Position Y:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+2].setConnectedObject("", "ancestor", "", "position_y");
        EditorWindow->TextContainer[labelIndex+3].addNewContentAndResize("Size X:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+3].setConnectedObject("", "ancestor", "", "size_x");
        EditorWindow->TextContainer[labelIndex+4].addNewContentAndResize("Size Y:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+4].setConnectedObject("", "ancestor", "", "size_y");
        EditorWindow->TextContainer[labelIndex+5].addNewContentAndResize("Scaled from center:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+5].setConnectedObject("", "ancestor", "", "is_scaled_from_center");
        EditorWindow->TextContainer[labelIndex+6].addNewContentAndResize("Attached to camera:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+6].setConnectedObject("", "ancestor", "", "is_attached_to_camera");
    }
}*/
/*
void ProcessClass::prepareEditorWindowImage(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(2, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowText(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(3, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowMovement(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(4, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowCollisions(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(5, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowEvents(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(6, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowParticles(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(7, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowVariables(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(8, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void ProcessClass::prepareEditorWindowEditable(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(9, EditorWindow, FontContainer, BitmapContainer, Arr);
}
*/

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
Module * ModuleIndex::module(vector<LayerClass> &Layers){
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
void PointerRecalculator::findIndexesForLayers(vector<LayerClass> &Layers, vector<ContextClass> &EventContext){
    for(ContextClass & Context : EventContext){
        LayerIndexes.push_back(vector<unsigned>());
        for(LayerClass * Layer : Context.Layers){
            LayerIndexes.back().push_back(Layer - &Layers[0]);
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
ModuleIndex PointerRecalculator::getIndex(Module *& Instance, vector<LayerClass> & Layers){
    if(Instance->getLayerID() == ""){
        cout << "Error: In " << __FUNCTION__ << ": Module instance does not belong to any layer.\n";
    }
    if(Instance->getObjectID() == ""){
        cout << "Error: In " << __FUNCTION__ << ": Module instance does not belong to any object.\n";
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
            break;
        }
        break;
    }
    return ModuleIndex(0, 0, 0);
}
template <class Module>
void PointerRecalculator::findIndexesInModule(vector<Module*> Instances, vector<LayerClass> & Layers){
    for(Module * Instance : Instances){
        ModuleIndexes.back().push_back(getIndex(Instance, Layers));
    }
}
ModuleIndex PointerRecalculator::getIndex(vector<EveModule>::iterator & Instance, vector<LayerClass> & Layers){
    if(Instance->getLayerID() == ""){
        cout << "Error: In " << __FUNCTION__ << ": Event instance does not belong to any layer.\n";
    }
    if(Instance->getObjectID() == ""){
        cout << "Error: In " << __FUNCTION__ << ": Event instance does not belong to any object.\n";
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
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
){
    startingEventIndex = getIndex(StartingEvent, Layers);
    eventIndex = getIndex(Event, Layers);
    for(MemoryStackStruct & Memory : MemoryStack){
        PastEvents.push_back(getIndex(Memory.Event, Layers));
    }
    for(ContextClass & Context : EventContext){
        ModuleIndexes.push_back(vector<ModuleIndex>());
        if(Context.type == "text"){
            findIndexesInModule(Context.Modules.Texts, Layers);
        }
        else if(Context.type == "editable_text"){
            findIndexesInModule(Context.Modules.EditableTexts, Layers);
        }
        else if(Context.type == "image"){
            findIndexesInModule(Context.Modules.Images, Layers);
        }
        else if(Context.type == "movement"){
            findIndexesInModule(Context.Modules.Movements, Layers);
        }
        else if(Context.type == "collision"){
            findIndexesInModule(Context.Modules.Collisions, Layers);
        }
        else if(Context.type == "particles"){
            findIndexesInModule(Context.Modules.Particles, Layers);
        }
        else if(Context.type == "event"){
            findIndexesInModule(Context.Modules.Events, Layers);
        }
        else if(Context.type == "variable"){
            findIndexesInModule(Context.Modules.Variables, Layers);
        }
        else if(Context.type == "scrollbar"){
            findIndexesInModule(Context.Modules.Scrollbars, Layers);
        }
        else if(Context.type == "primitives"){
            findIndexesInModule(Context.Modules.Primitives, Layers);
        }
    }
}
void PointerRecalculator::updatePointersToCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext, Camera2D *& SelectedCamera){
    unsigned context, camera;
    for(context = 0; context < CameraIndexes.size(); context++){
        for(camera = 0; camera < CameraIndexes[context].size(); camera++){
            if(Cameras.size() <= CameraIndexes[context][camera]){
                cout << "Error: In " << __FUNCTION__ << ": CameraIndexes[context][camera] goes out of scope of Cameras.\n";
                EventContext[context].Cameras[camera] = nullptr;
                continue;
            }
            EventContext[context].Cameras[camera] = &Cameras[CameraIndexes[context][camera]];
        }
    }
    if(SelectedCamera != nullptr){
        if(Cameras.size() <= selectedCameraIndex){
            cout << "Error: In " << __FUNCTION__ << ": selectedCameraIndex goes out of scope of Cameras.\n";
            SelectedCamera = nullptr;
            return;
        }
        SelectedCamera = &Cameras[selectedCameraIndex];   
    }
}
void PointerRecalculator::updatePointersToLayers(vector<LayerClass> &Layers, vector<ContextClass> &EventContext){
    unsigned context, layer;
    for(context = 0; context < LayerIndexes.size(); context++){
        for(layer = 0; layer < LayerIndexes[context].size(); layer++){
            if(Layers.size() <= LayerIndexes[context][layer]){
                cout << "Error: In " << __FUNCTION__ << ": LayerIndexes[" << context
                    << "][" << layer << "]=" << LayerIndexes[context][layer]
                    << " goes out of scope of Layers<" << Layers.size()  << ">.\n";
                EventContext[context].Layers[layer] = nullptr;
                continue;
            }
            EventContext[context].Layers[layer] = &Layers[LayerIndexes[context][layer]];
        }
    }
}
void PointerRecalculator::updatePointersToObjects(vector<LayerClass> &Layers, vector<ContextClass> &EventContext, AncestorObject *&Owner,
    vector<AncestorObject *> &TriggeredObjects, LayerClass *&SelectedLayer, AncestorObject *&SelectedObject)
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
            cout << "Error: In " << __FUNCTION__ << ": selectedLayerIndex(" << selectedLayerIndex << ") goes out of scope of Layers<" << Layers.size() << ">.\n";
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
    vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
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
                cout << "Error: In " << __FUNCTION__ << ": Object pointer is a null value.\n";
                continue;
            }

            if(EventContext[context].type == "text"){
                EventContext[context].Modules.Texts[module] = &(*Object).TextContainer[Index.moduleIndex];
            }
            else if(EventContext[context].type == "editable_text"){
                EventContext[context].Modules.EditableTexts[module] = &(*Object).EditableTextContainer[Index.moduleIndex];
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
        }
    }
}