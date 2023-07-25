#include "engineLoop.h"

#if __linux__
    unsigned long long GetTickCount()
    {
        struct timeval tv;
        if (gettimeofday(&tv, NULL) != 0) {
            // Handle error
        }
        return (unsigned long long) tv.tv_sec * 1000 + (unsigned long long) tv.tv_usec / 1000;
    }
#endif

inline unsigned int Interval::value() const{
    return GetTickCount()-initial_;
}


void Fps::update(){
    // increase the counter by one
    m_fpscount++;

    // one second elapsed? (= 1000 milliseconds)
    if (m_fpsinterval.value() > 1000)
    {
        // save the current counter value to m_fps
        m_fps = m_fpscount;

        // reset the counter and the interval
        m_fpscount = -1;
        m_fpsinterval = Interval();
    }
}
unsigned int Fps::get() const{
    return m_fps;
}

vector <short> getPressedKeys(unsigned char key[]){
    vector <short> pressedKeys;
    for(short i = 1; i < ALLEGRO_KEY_MAX; i++){
        if(key[i]){
            pressedKeys.push_back(i);
        }
    }
    return pressedKeys;
}

vector <short> getReleasedKeys(unsigned char key[], vector <short> pressedKeys){
    vector <short> releasedKeys;
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(!key[pressedKeys[i]]){
            releasedKeys.push_back(pressedKeys[i]);
        }
    }
    return releasedKeys;
}

void EventsLookupTable::clearLookupTable(){
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
}

EngineLoop::EngineLoop(std::string title){
    mainBuffer = NULL;
    cursorBitmap = NULL;
    windowTitle = title;
    bootGame = false;
    closeGame = false;
    closeEditor = false;
    isGameActive = false;
    drawTextFieldBorders = false;
    drawHitboxes = true;
    isPixelArt = true;
    ignoreDistantObjects = false;
    drawOnlyVisibleObjects = false;
    wasMousePressedInSelectedObject = false;
    timeToInterruptMovement = 1;
    timeToInterruptParticles = 1;
    totalNumberOfBitmapLayers = 3;
    selectedObjectLayerID = "";
    selectedObjectID = "";
}
void EngineLoop::initAllegro(){
    al_init();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 2, ALLEGRO_SUGGEST); //antialias stuff
    al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST); //antialias stuff
    //al_set_new_bitmap_samples(4);

    window = al_create_display(SCREEN_W, SCREEN_H);
    al_set_window_title(window, windowTitle.c_str());



    mainBuffer = al_create_bitmap(BUFFER_W, BUFFER_H);

    //cursorBitmap = al_load_bitmap("images/cursor.png");
    if(cursorBitmap){
        mouseCursor = al_create_mouse_cursor(cursorBitmap, 0, 0);
        al_set_mouse_cursor(window, mouseCursor);
    }
    else{
        al_set_system_mouse_cursor(window, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }


    eventQueue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);

    std::cout << "Timer speed: "<< al_get_timer_speed(timer)*60 << "s\n";

    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(window));
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    al_register_event_source(eventQueue, al_get_mouse_event_source());

    memset(key, 0, sizeof(key));

    //Use it only on images that are not pixel arts
    if(!isPixelArt)
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    else
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);

    //Find out how much buttons the user has.
    Mouse.setUp();

    SelectedCamera = nullptr;
    SelectedLayer = nullptr;
    SelectedObject = nullptr;
}
void EngineLoop::startTimer(){
    al_start_timer(timer);
}
void EngineLoop::exitAllegro(){
    al_destroy_bitmap(mainBuffer);
    al_destroy_display(window);
    if(cursorBitmap){
        al_destroy_bitmap(cursorBitmap);
        al_destroy_mouse_cursor(mouseCursor);
    }
    al_destroy_timer(timer);
    al_destroy_event_queue(eventQueue);
    releasedKeys.clear();
    firstPressedKeys.clear();
    pressedKeys.clear();
    foregroundOfObjects.clear();
    BaseOfTriggerableObjects.clearLookupTable();
}
bool EngineLoop::createListOfUniqueIDsOfLayers(vector <LayerClass> & Layers, vector <string> & layersIDs){
    layersIDs.clear();
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Layers.size(); i++){
        for(j = 0; j < Layers.size(); j++){
            if(i == j){
                continue;
            }
            if(Layers[i].getID() == Layers[j].getID()){
                if(!violated){
                    std::cout << "\n\n";
                }
                std::cout << "Uniqueness has been violated by \'" << Layers[j].getID() << "\' layer.\n";
                violated = true;
            }
        }
        if(!violated){
            layersIDs.push_back(Layers[i].getID());
        }
    }

    if(violated){
        std::cout << "\n";
    }
    return violated;
}
bool EngineLoop::createListOfUniqueIDsOfCameras(vector <Camera2D> & Cameras, vector <string> & camerasIDs){
    camerasIDs.clear();
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Cameras.size(); i++){
        for(j = 0; j < Cameras.size(); j++){
            if(i == j){
                continue;
            }
            if(Cameras[i].getID() == Cameras[j].getID()){
                if(!violated){
                    std::cout << "\n\n";
                }
                std::cout << "Uniqueness has been violated by \'" << Cameras[j].getID() << "\' layer.\n";
                violated = true;
            }
        }
        if(!violated){
            camerasIDs.push_back(Cameras[i].getID());
        }
    }

    if(violated){
        std::cout << "\n";
    }
    return violated;
}

void EngineLoop::windowLoop(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer, Fps & fps, vector <SingleBitmap> & BitmapContainer){
    bool redraw = false;
    do {
        al_wait_for_event(eventQueue , &event);
        switch(event.type){
            case ALLEGRO_EVENT_TIMER:
                //Keyboard input is gathered here
                releasedKeys.clear();
                releasedKeys = getReleasedKeys(key, pressedKeys);
                pressedKeys.clear();
                pressedKeys = getPressedKeys(key);

                updateCamerasPositions(Cameras);
                
                moveObjects(Layers, Cameras);
                
                moveParticles(Layers);

                if(SelectedCamera != nullptr && SelectedCamera->getIsActive()){
                    SelectedCamera->update(pressedKeys);
                }

                //START EVENT CONTROL SECTION
                
                triggerEve(Layers, Cameras);

                //END EVENT CONTROL SECTION

                firstPressedKeys.clear();

                Mouse.didMouseMove = false;

                Mouse.resetFirstPressed();
                
                if(key[ALLEGRO_KEY_ESCAPE])
                    closeEditor = true;
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;
                redraw = true;
                Mouse.resetReleased();
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                Mouse.updateAxes(event);
                if(SelectedCamera != nullptr && SelectedCamera->getIsActive()){
                    Mouse.updateZoomPos(*SelectedCamera);
                }

                moveSelectedObject(Layers[0].Objects[0].EditableTextContainer);
                dragScrollbars(Layers);

                if(SelectedCamera != nullptr && SelectedCamera->getIsActive()){
                    if(Mouse.firstPositionInRectangle(SelectedCamera->pos, SelectedCamera->size, 2, true)){
                        SelectedCamera->visionShift = Mouse.getZoomPos() - dragCameraStaringPos;
                    }
                }

                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                Mouse.updateButtonsPressed(event);

                focusOnCamera(Cameras);

                selectObject(Layers, BitmapContainer, FontContainer);

                detectStartPosOfDraggingObjects();

                startScrollbarDragging(Layers);
                if(SelectedCamera != nullptr && SelectedCamera->getIsActive()){
                    dragCameraStaringPos.set(Mouse.getZoomPos()-SelectedCamera->visionShift);
                }

                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                Mouse.updateButtonsReleased(event);
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                firstPressedKeys = getPressedKeys(key);
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                closeEditor = true;
                break;
        }

        if(Mouse.isPressed() || releasedKeys.size() != 0 || pressedKeys.size() != 0){
            for(unsigned int i = 0; i < releasedKeys.size(); i++){
                if(releasedKeys[i] >= ALLEGRO_KEY_0 && releasedKeys[i] <= ALLEGRO_KEY_9 && int(Layers.size()) >= releasedKeys[i]-26){
                    Layers[releasedKeys[i]-27].setIsActive(!Layers[releasedKeys[i]-27].getIsActive());
                    if(!Layers[releasedKeys[i]-27].getIsActive()){
                        unselectObject();
                    }
                }
            }
            updateEditableTextFields(Layers, BitmapContainer);
        }

        if(Mouse.didMouseMove && SelectedCamera != nullptr && SelectedCamera->getIsActive()){
            if(Mouse.inRectangle(SelectedCamera->pos, SelectedCamera->size, true)){
                Mouse.updateZoomForCamera(SelectedCamera);
                Mouse.updateZoomPos(*SelectedCamera);
            }
        }
    } while (!al_is_event_queue_empty(eventQueue));

    if(redraw){
        timeToInterruptMovement = 20000;

        timeToInterruptParticles = 500;

        al_set_target_bitmap(al_get_backbuffer(window));
        drawObjects(Layers, Cameras, FontContainer);

        //al_draw_filled_circle(SCREEN_W/2, SCREEN_H/2, 10, al_map_rgb_f(1.0, 0.0, 0.0));

        al_flip_display();
        al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));

        redraw = false;

        fps.update();
        string updatedFpsLabel = "FPS: " + intToStr4(fps.get());

        if(Layers[0].Objects.size() >= 2)
            if(Layers[0].Objects[1].TextContainer.size() > 0)
                Layers[0].Objects[1].TextContainer[0].modifyContent(0, updatedFpsLabel);
    }
}
void EngineLoop::updateBaseOfTriggerableObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras){
    BaseOfTriggerableObjects.clearLookupTable();
    
    for(LayerClass & Layer : Layers){
        for(AncestorObject & Object : Layer.Objects){
            for(EveModule Event : Object.EveContainer){
                for(string type : Event.primaryTriggerTypes){
                    if(type == "each_iteration"){
                        BaseOfTriggerableObjects.IterationTriggered.push_back(&Object);
                    }
                    else if(type == "second_passed"){
                        BaseOfTriggerableObjects.TimeTriggered.push_back(&Object);
                    }
                    else if(type == "key_pressed"){
                        BaseOfTriggerableObjects.KeyPressedTriggered.push_back(&Object);
                    }
                    else if(type == "key_pressing"){
                        BaseOfTriggerableObjects.KeyPressingTriggered.push_back(&Object);
                    }
                    else if(type == "key_released"){
                        BaseOfTriggerableObjects.KeyReleasedTriggered.push_back(&Object);
                    }
                    else if(type == "mouse_moved"){
                        BaseOfTriggerableObjects.MouseMovedTriggered.push_back(&Object);
                    }
                    else if(type == "mouse_not_moved"){
                        BaseOfTriggerableObjects.MouseNotMovedTriggered.push_back(&Object);
                    }
                    else if(type == "mouse_pressed"){
                        BaseOfTriggerableObjects.MousePressedTriggered.push_back(&Object);
                    }
                    else if(type == "mouse_pressing"){
                        BaseOfTriggerableObjects.MousePressingTriggered.push_back(&Object);
                    }
                    else if(type == "mouse_released"){
                        BaseOfTriggerableObjects.MouseReleasedTriggered.push_back(&Object);
                    }
                    else if(type == "objects"){
                        BaseOfTriggerableObjects.ObjectsTriggered.push_back(&Object);
                    }
                    else if(type == "variables"){
                        BaseOfTriggerableObjects.VariablesTriggered.push_back(&Object);
                    }
                    else if(type == "collision"){
                        BaseOfTriggerableObjects.CollisionTriggered.push_back(&Object);
                    }
                    else if(type == "editables"){
                        BaseOfTriggerableObjects.EditableTextTriggered.push_back(&Object);
                    }
                    else if(type == "movement"){
                        BaseOfTriggerableObjects.MovementTriggered.push_back(&Object);
                    }
                    else if(type == "stillness"){
                        BaseOfTriggerableObjects.StillnessTriggered.push_back(&Object);
                    }
                }
            }
        }
    }
}
void EngineLoop::detectTriggeredEvents(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <AncestorObject*> & TriggeredObjects){
    for(AncestorObject * Object : BaseOfTriggerableObjects.IterationTriggered){
        TriggeredObjects.push_back(Object);
    }
    if(secondHasPassed()){ 
        for(AncestorObject * Object : BaseOfTriggerableObjects.TimeTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(firstPressedKeys.size() > 0){
        for(AncestorObject * Object : BaseOfTriggerableObjects.KeyPressedTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(pressedKeys.size() > 0){
        for(AncestorObject * Object : BaseOfTriggerableObjects.KeyPressingTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(releasedKeys.size() > 0){
        for(AncestorObject * Object : BaseOfTriggerableObjects.KeyReleasedTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(Mouse.didMouseMove){
        for(AncestorObject * Object : BaseOfTriggerableObjects.MouseMovedTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(!Mouse.didMouseMove){
        for(AncestorObject * Object : BaseOfTriggerableObjects.MouseNotMovedTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(Mouse.isFirstPressed()){
        for(AncestorObject * Object : BaseOfTriggerableObjects.MousePressedTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(Mouse.isPressed()){
        for(AncestorObject * Object : BaseOfTriggerableObjects.MousePressingTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    if(Mouse.isReleased()){
        for(AncestorObject * Object : BaseOfTriggerableObjects.MouseReleasedTriggered){
            TriggeredObjects.push_back(Object);
        }
    }
    for(AncestorObject * Object : BaseOfTriggerableObjects.MovementTriggered){
        for(MovementModule Movement : Object->MovementContainer){
            if(!Movement.getIsActive()){
                continue;
            }
            if(Movement.isMoving()){
                TriggeredObjects.push_back(Object);
                break;
            }
        }
    }
    bool triggered;
    for(AncestorObject * Object : BaseOfTriggerableObjects.StillnessTriggered){
        triggered = true;
        for(MovementModule Movement : Object->MovementContainer){
            if(Movement.getIsActive() && Movement.isMoving()){
                triggered = false;
                break;
            }
        }
        if(triggered){
            TriggeredObjects.push_back(Object);
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
}
void EngineLoop::executeDependentOperations(LayerClass * OwnerLayer, AncestorObject * Owner, EveModule & Event, vector <LayerClass> & Layers, vector <Camera2D> & Cameras){

}
void EngineLoop::executePostOperations(AncestorObject * Owner, EveModule & Event, vector <LayerClass> & Layers, vector <Camera2D> & Cameras){

}
VariableModule EngineLoop::findNextValueInMovementModule(TriggerClass & Condition, AncestorObject * CurrentObject){
    VariableModule NewValue(Condition.Location.moduleType + "_" + Condition.Location.attribute);
    if(Condition.Location.attribute == "is_moving"){
        NewValue.setBool(false);
        for(MovementModule Movement : CurrentObject->MovementContainer){
            if(Movement.getIsActive() && Movement.isMoving()){
                NewValue.setBool(true);
                break;
            }
        }
        return NewValue;
    }
    if(Condition.Location.attribute == "is_still"){
        NewValue.setBool(true);
        for(MovementModule Movement : CurrentObject->MovementContainer){
            if(Movement.getIsActive() && Movement.isMoving()){
                NewValue.setBool(false);
                break;
            }
        }
        return NewValue;
    }
    for(MovementModule Movement : CurrentObject->MovementContainer){
        if(Movement.getID() != Condition.Location.moduleID){
            continue;
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
    NewValue.setBool(false);
    NewValue.setID("null");
    return NewValue;
}
VariableModule EngineLoop::findNextValueAmongObjects(TriggerClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras){
    VariableModule NewValue("null");
    LayerClass * CurrentLayer = nullptr;
    AncestorObject * CurrentObject = nullptr;
    if(Condition.Location.layerID == Owner->layerID){
        CurrentLayer = OwnerLayer;
    }
    else{
        for(LayerClass & Layer : Layers){
            if(Layer.getIsActive() && Layer.getID() == Condition.Location.layerID){
                CurrentLayer = &Layer;
            }
        }
    }
    
    if(CurrentLayer == nullptr){
        NewValue.setBool(false);
        return NewValue;
    }
    
    if(CurrentLayer == OwnerLayer && Condition.Location.objectID == Owner->getID()){
        CurrentObject = Owner;
    }
    else{
        for(AncestorObject & Object : CurrentLayer->Objects){
            if(Object.getIsActive() && Object.getID() == Condition.Location.objectID){
                CurrentObject = &Object;
                break;
            }
        }
    }
    
    if(CurrentObject == nullptr){
        NewValue.setBool(false);
        return NewValue;
    }

    NewValue.setID(Condition.Location.moduleType + "_" + Condition.Location.attribute);

    if(Condition.Location.moduleType == "variable"){
        for(VariableModule Variable : CurrentObject->VariablesContainer){
            if(Variable.getID() == Condition.Location.moduleID){
                return Variable;
            }
        }
    }
    else if(Condition.Location.moduleType == "ancestor"){
        if(Condition.Location.attribute == "is_active"){
            NewValue.setBool(CurrentObject->getIsActive());
        }
        else if(Condition.Location.attribute == "in_group"){
            NewValue.setBool(CurrentObject->isInAGroup(Condition.Literal.getString()));
        }
        else if(Condition.Location.attribute == "pos_x"){
            NewValue.setDouble(CurrentObject->getPos(false).x);
        }
        else if(Condition.Location.attribute == "pos_y"){
            NewValue.setDouble(CurrentObject->getPos(false).y);
        }
        else if(Condition.Location.attribute == "size_x"){
            NewValue.setDouble(CurrentObject->getSize().x);
        }
        else if(Condition.Location.attribute == "size_y"){
            NewValue.setDouble(CurrentObject->getSize().y);
        }
        else if(Condition.Location.attribute == "scale_x"){
            NewValue.setDouble(CurrentObject->getSize().x);
        }
        else if(Condition.Location.attribute == "scale_y"){
            NewValue.setDouble(CurrentObject->getSize().y);
        }
        else{
            NewValue.setBool(false);
        }
        return NewValue;
    }
    else if(Condition.Location.moduleType == "camera"){
        if(Condition.Location.attribute == "visible"){
            NewValue.setBool(false);
            for(Camera2D Camera : Cameras){
                if(Camera.isObjectVisible(CurrentObject->getPos(false), CurrentObject->getSize())){
                    NewValue.setBool(true);
                    break;
                }
            }
            return NewValue;
        }
        else if(Condition.Location.attribute == "visible_specific"){
            NewValue.setBool(false);
            for(Camera2D Camera : Cameras){
                if(Camera.getID() == Condition.Location.moduleID
                    && Camera.isObjectVisible(CurrentObject->getPos(false), CurrentObject->getSize())
                ){
                    NewValue.setBool(true);
                    break;
                }
            }
            return NewValue;
        }
    }
    else if(Condition.Location.moduleType == "mouse"){
        NewValue.setBool(false);
        if(Condition.Location.attribute == "pressed"){
            if(SelectedCamera != nullptr && Mouse.firstPressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true)){
                NewValue.setBool(
                    Mouse.firstPressedInRectangle(
                        CurrentObject->getPosOnCamera(SelectedCamera),
                        CurrentObject->getSize(),
                        Condition.Literal.getInt(),
                        CurrentObject->getIsAttachedToCamera()
                    )
                );
            }
            return NewValue;
        }
        else if(Condition.Location.attribute == "pressing"){
            if(SelectedCamera != nullptr && Mouse.pressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true)){
                NewValue.setBool(
                    Mouse.pressedInRectangle(
                        CurrentObject->getPosOnCamera(SelectedCamera),
                        CurrentObject->getSize(),
                        Condition.Literal.getInt(),
                        CurrentObject->getIsAttachedToCamera()
                    )
                );
            }
            return NewValue;
        }
        else if(Condition.Location.attribute == "released"){
            if(SelectedCamera != nullptr && Mouse.releasedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true)){
                NewValue.setBool(
                    Mouse.releasedInRectangle(
                        CurrentObject->getPosOnCamera(SelectedCamera),
                        CurrentObject->getSize(),
                        Condition.Literal.getInt(),
                        CurrentObject->getIsAttachedToCamera()
                    )
                );
            }
            return NewValue;
        }
    }
    else if(Condition.Location.moduleType == "movement"){
        return findNextValueInMovementModule(Condition, CurrentObject);
    }
    else if(Condition.Location.moduleType == "text"){
        for(TextModule Text : CurrentObject->TextContainer){
            if(Text.getID() != Condition.Location.moduleID){
                continue;
            }
            if(Condition.Location.attribute == "content"){
                NewValue.setString(Text.getCurrentContent());
                return NewValue;
            }
            if(Condition.Location.attribute == "rotation_angle"){
                NewValue.setDouble(Text.rotateAngle);
                return NewValue;
            }
            if(Condition.Location.attribute == "visibility"){
                NewValue.setDouble(Text.visibility);
                return NewValue;
            }
            break;
        }
    }
    else if(Condition.Location.moduleType == "editable_text"){
        for(EditableTextModule Text : CurrentObject->EditableTextContainer){
            if(Text.getID() != Condition.Location.moduleID){
                continue;
            }
            if(Condition.Location.attribute == "content"){
                NewValue.setString(Text.getCurrentContent());
                return NewValue;
            }
            if(Condition.Location.attribute == "rotation_angle"){
                NewValue.setDouble(Text.rotateAngle);
                return NewValue;
            }
            if(Condition.Location.attribute == "visibility"){
                NewValue.setDouble(Text.visibility);
                return NewValue;
            }
            if(Condition.Location.attribute == "can_be_edited"){
                NewValue.setBool(Text.getCanBeEdited());
                return NewValue;
            }
            if(Condition.Location.attribute == "editing"){
                NewValue.setBool(Text.getEditingIsActive());
                return NewValue;
            }
            break;
        }
    }
    else if(Condition.Location.moduleType == "collision"){
        NewValue.setBool(false);
        if(Condition.Location.attribute == "has_solid_hitbox"){
            for(CollisionModule Collision : CurrentObject->CollisionContainer){
                if(Collision.getIsSolid()){
                    NewValue.setBool(true);
                    return NewValue;
                }
            }
            return NewValue;
        }
        if(Condition.Location.attribute == "is_fully_solid"){
            NewValue.setBool(true);
            for(CollisionModule Collision : CurrentObject->CollisionContainer){
                if(!Collision.getIsSolid()){
                    NewValue.setBool(false);
                    return NewValue;
                }
            }
            return NewValue;
        }
        if(Condition.Location.attribute == "detected"){
            for(CollisionModule Collision : CurrentObject->CollisionContainer){
                for(DetectedCollision Detected : Collision.Detected){
                    if(Detected.collisionType > 0){
                        NewValue.setBool(true);
                        return NewValue;
                    }
                }
            }
            return NewValue;
        }
        if(Condition.Location.attribute == "with_object"){
            for(CollisionModule Collision : CurrentObject->CollisionContainer){
                for(DetectedCollision Detected : Collision.Detected){
                    if(Detected.collisionType > 0 && Detected.solidID == Condition.Literal.getString()){
                        NewValue.setBool(true);
                        return NewValue;
                    }
                }
            }
            return NewValue;
        }
        if(Condition.Location.attribute == "with_hitbox"){
            for(CollisionModule Collision : CurrentObject->CollisionContainer){
                for(DetectedCollision Detected : Collision.Detected){
                    if(Detected.collisionType > 0
                        && Detected.solidID == Condition.Literal.getString()
                        && Detected.collisionID == Condition.Location.spareID
                    ){
                        NewValue.setBool(true);
                        return NewValue;
                    }
                }
            }
            return NewValue;
        }
        for(CollisionModule Collision : CurrentObject->CollisionContainer){
            if(Collision.getID() != Condition.Location.moduleID){
                continue;
            }
            if(Condition.Location.attribute == "hitbox_is_solid"){
                NewValue.setBool(Collision.getIsSolid());
                return NewValue;
            }
            if(Condition.Location.attribute == "hitbox_can_penetrate"){
                NewValue.setBool(Collision.getCanPenetrateSolids());
                return NewValue;
            }
            if(Condition.Location.attribute == "hitbox_ignores_object"){
                NewValue.setBool(Collision.ignores("object", Condition.Location.spareID));
                return NewValue;
            }
            if(Condition.Location.attribute == "hitbox_ignores_object_group"){
                for(PrimaryModule Primary : CurrentLayer->Objects){
                    if(Primary.getID() != Condition.Location.spareID){
                        continue;
                    }
                    for(string group : Primary.getGroups()){
                        NewValue.setBool(Collision.ignores("groups_of_objects", group));
                        return NewValue;
                    }
                    return NewValue;
                }
                return NewValue;
            }
            if(Condition.Location.attribute == "hitbox_ignores_hitbox"){
                NewValue.setBool(Collision.ignores("hitboxes", Condition.Location.spareID));
                return NewValue;
            }
            if(Condition.Location.attribute == "hitbox_ignores_hitbox_group"){
                for(AncestorObject NextObject : CurrentLayer->Objects){
                    if(NextObject.getID() != Condition.Location.spareID){
                        continue;
                    }
                    for(PrimaryModule PrimaryCollision : NextObject.CollisionContainer){
                        if(PrimaryCollision.getID() != Condition.Literal.getString()){
                            continue;
                        }
                        for(string group : PrimaryCollision.getGroups()){
                            NewValue.setBool(Collision.ignores("groups_of_hitboxes", group));
                            return NewValue;
                        }
                        return NewValue;
                    }
                    return NewValue;
                }
                return NewValue;
            }
            if(Condition.Location.attribute == "detected_by_hitbox"){
                for(DetectedCollision Detected : Collision.Detected){
                    if(Detected.collisionType > 0){
                        NewValue.setBool(true);
                        return NewValue;
                    }
                }
                return NewValue;
            }
            if(Condition.Location.attribute == "between_hitboxes"){
                for(CollisionModule Collision : CurrentObject->CollisionContainer){
                    if(Collision.getID() != Condition.Location.moduleID){
                        continue;
                    }
                    for(DetectedCollision Detected : Collision.Detected){
                        if(Detected.collisionType > 0
                            && Detected.solidID == Condition.Literal.getString()
                            && Detected.collisionID == Condition.Location.spareID
                        ){
                            NewValue.setBool(true);
                            return NewValue;
                        }
                    }
                }
                return NewValue;
            }
            return NewValue;
        }
    }
    
    NewValue.setID("null");
    NewValue.setBool(false);
    return NewValue;
}
VariableModule EngineLoop::findNextValue(TriggerClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras){
    VariableModule NewValue(Condition.source);
    
    if(Condition.source == "object"){
        return findNextValueAmongObjects(Condition, Owner, OwnerLayer, Layers, Cameras);
    }
    if(Condition.source == "second_passed"){
        NewValue.setBool(secondHasPassed());
        return NewValue;
    }
    if(Condition.source == "literal"){
        return Condition.Literal;
    }
    if(Condition.source == "key_pressed"){
        NewValue.setBool(isKeyFirstPressed(Condition.Literal.getInt()));
        return NewValue;
    }
    if(Condition.source == "key_pressing"){
        NewValue.setBool(isKeyPressed(Condition.Literal.getInt()));
        return NewValue;
    }
    if(Condition.source == "key_released"){
        NewValue.setBool(isKeyReleased(Condition.Literal.getInt()));
        return NewValue;
    }
    if(Condition.source == "any_key_pressed"){
        NewValue.setBool(firstPressedKeys.size() > 0);
        return NewValue;
    }
    if(Condition.source == "any_key_pressing"){
        NewValue.setBool(pressedKeys.size() > 0);
        return NewValue;
    }
    if(Condition.source == "any_key_released"){
        NewValue.setBool(releasedKeys.size() > 0);
        return NewValue;
    }
    if(Condition.source == "mouse_moved"){
        NewValue.setBool(Mouse.didMouseMove);
        return NewValue;
    }
    if(Condition.source == "mouse_pressed"){
        NewValue.setBool(Mouse.isFirstPressed(Condition.Literal.getInt()));
        return NewValue;
    }
    if(Condition.source == "mouse_pressing"){
        NewValue.setBool(Mouse.isPressed(Condition.Literal.getInt()));
        return NewValue;
    }
    if(Condition.source == "mouse_released"){
        NewValue.setBool(Mouse.isReleased(Condition.Literal.getInt()));
        return NewValue;
    }
    if(Condition.source == "variable"){
        for(VariableModule Variable : Owner->VariablesContainer){
            if(Variable.getID() == Condition.Literal.getString()){
                return Variable;
            }
        }
    }
    if(Condition.source == "camera"){
        NewValue.setID(Condition.source + "_" + Condition.Location.attribute);
        for(Camera2D Camera : Cameras){
            if(Camera.getID() == Condition.Literal.getString()){
                if(Condition.Location.attribute == "pos_x"){
                    NewValue.setDouble(Camera.pos.x);
                    return NewValue;
                }
                if(Condition.Location.attribute == "pos_y"){
                    NewValue.setDouble(Camera.pos.y);
                    return NewValue;
                }
                if(Condition.Location.attribute == "size_x"){
                    NewValue.setDouble(Camera.size.x);
                    return NewValue;
                }
                if(Condition.Location.attribute == "size_y"){
                    NewValue.setDouble(Camera.size.y);
                    return NewValue;
                }
                if(Condition.Location.attribute == "zoom"){
                    NewValue.setDouble(Camera.zoom);
                    return NewValue;
                }
                break;
            }
        }
    }
    NewValue.setBool(false);
    return NewValue;
}
char EngineLoop::evaluateConditionalChain(AncestorObject * Owner, EveModule & Event, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras){
    short ignoreFlagOr = 0, ignoreFlagAnd = 0;
    bool comparasion;
    int resultInt;
    double resultDouble;
    VariableModule leftOperand, rightOperand;
    vector<VariableModule> resultStack;
    string newID;
    
    for(TriggerClass & Condition : Event.ConditionalChain){
        //check source
        if(ignoreFlagOr == 0 && ignoreFlagAnd == 0){
            resultStack.push_back(findNextValue(Condition, Owner, OwnerLayer, Layers, Cameras));
        }
        if(resultStack.size() == 0){
            continue;
        }
        //check conjunctions
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
                std::cout << "!(" << resultStack.back().getID() << ":" << resultStack.back().getAnyValue() << ") -> " << leftOperand.getAnyValue() << "\n";
                resultStack.pop_back();
                leftOperand.setID("!("+leftOperand.getID()+")");
                resultStack.push_back(leftOperand);
            }
            else if(op == "igT"){
                if(resultStack.back().isConditionMet(true, "==", 'b')){
                    std::cout << resultStack.back().getID() << ":" << "true || ... -> true\n";
                    ignoreFlagOr++;
                }
            }
            else if(op == "igF"){
                if(resultStack.back().isConditionMet(false, "==", 'b')){
                    std::cout << resultStack.back().getID() << ":" << "false && ... -> false\n";
                    ignoreFlagAnd++;
                }
            }
            else if(resultStack.size() >= 2){

                if(!isStringInGroup(op, 14, "&&", "||", "==", "!=", ">", "<", ">=", "<=", "+", "-", "*", "/", "%", "**")){
                    std::cout << "Error [Engine]: Unrecognized operator in the if statement.\n";
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

                resultStack.push_back(VariableModule(newID));
                
                if (isStringInGroup(op, 8, "&&", "||", "==", "!=", ">", "<", ">=", "<=")){
                    comparasion = leftOperand.isConditionMet(op, &rightOperand);

                    std::cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << op << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << comparasion << "\n";
                    
                    resultStack.back().setBool(comparasion);
                }
                else if(isStringInGroup(op, 6, "+", "-", "*", "/", "%", "**")){
                    if(leftOperand.getType() == 'd' || rightOperand.getType() == 'd'){
                        resultDouble = leftOperand.floatingOperation(op, &rightOperand);

                        std::cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << op << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << resultDouble << "\n";

                        resultStack.back().setDouble(resultDouble);
                    }
                    else{
                        resultInt = leftOperand.intOperation(op, &rightOperand);

                        std::cout << leftOperand.getID() << ":"  << leftOperand.getAnyValue() << " " << op << " " << rightOperand.getID() << ":" << rightOperand.getAnyValue() << " -> " << resultInt << "\n";

                        resultStack.back().setInt(resultInt);
                    }
                }
            }
        }
    }
    if(resultStack.size() == 1){
        if(resultStack.back().getBool()){
            return 't';
        }
        return 'f';
    }

    //al_draw_filled_circle(SCREEN_W/2, SCREEN_H/2, 10, al_map_rgb_f(1.0, 0.0, 0.0));
    
    return 'n';
}
vector<EveModule>::iterator EngineLoop::FindUnfinishedEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event){
    vector<EveModule>::iterator Unfinished;
    for(ChildStruct & Child : Event->Children){
        if(!Child.finished){
            for(Unfinished = Triggered->EveContainer.begin(); Unfinished < Triggered->EveContainer.end(); Unfinished++){
                if(Unfinished->getID() == Child.ID){
                    Child.finished = true;
                    return Unfinished;
                }
            }
            Child.finished = true;
        }
    }
    return Event;
}
vector<EveModule>::iterator EngineLoop::FindElseEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event){
    vector<EveModule>::iterator ElseEvent;
    for(ElseEvent = Triggered->EveContainer.begin(); ElseEvent < Triggered->EveContainer.end(); ElseEvent++){
        if(ElseEvent->getID() == Event->elseChildID){
            Event->elseChildFinished = true;
            return ElseEvent;
        }
    }
    return Event;
}
void EngineLoop::triggerEve(vector <LayerClass> & Layers, vector <Camera2D> & Cameras){
    //Only events from TriggeredObjects can be executed in the current iteration - events of newly created objects 
    //must wait with execution for the next iteration, unless run() command will be used.
    vector <AncestorObject*> TriggeredObjects;

    detectTriggeredEvents(Layers, Cameras, TriggeredObjects);

    if(TriggeredObjects.size() > 0){
        std::cout << "\nTriggered "<< TriggeredObjects.size() << " objects:\n";
    }

    //Remember to delete pointers to destroyed objects during the iteration
    
    vector<EveModule>::iterator StartingEvent, Event;
    vector<vector<EveModule>::iterator> MemoryStack;

    bool noTriggerableEvents;

    LayerClass * TriggeredLayer;

    for(AncestorObject * Triggered : TriggeredObjects){
        for(EveModule & Eve : Triggered->EveContainer){
            Eve.conditionalStatus = 'n';
            Eve.areDependentOperationsDone = false;
            Eve.elseChildFinished = false;
            for(ChildStruct & Unfinished : Eve.Children){
                Unfinished.finished = true;
            }
        }

        //Find the first triggerable event.
        noTriggerableEvents = true;
        for(EveModule & Eve : Triggered->EveContainer){
            if(Eve.primaryTriggerTypes.size() > 0){
                noTriggerableEvents = false;
                Event = Triggered->EveContainer.begin();
                break;
            }
        }

        if(noTriggerableEvents){
            continue;
        }

        for(LayerClass & Layer : Layers){
            if(Triggered->layerID == Layer.getID()){
                TriggeredLayer = &Layer;
                break;
            }
        }
        
        StartingEvent = Event;
        MemoryStack.clear();
        do{
            if(Event->conditionalStatus == 'n'){
                Event->conditionalStatus = evaluateConditionalChain(Triggered, *Event, TriggeredLayer, Layers, Cameras);
                std::cout << "Result: " << Event->conditionalStatus << "\n\n";
            }
            if(Event->conditionalStatus == 't'){
                if(!Event->areDependentOperationsDone){
                    executeDependentOperations(TriggeredLayer, Triggered, *Event, Layers, Cameras);
                    Event->areDependentOperationsDone = true;
                }
                if(!Event->checkIfAllChildrenFinished()){
                    MemoryStack.push_back(Event);
                    Event = FindUnfinishedEvent(Triggered, Event);
                    continue;
                }
            }
            else if(Event->conditionalStatus == 'f' && Event->elseChildID != "" && !Event->elseChildFinished){
                MemoryStack.push_back(Event);
                Event = FindElseEvent(Triggered, Event);
                if(Event->elseChildFinished){ //True if else event has been found.
                    continue;
                }
                MemoryStack.pop_back();
            }
            executePostOperations(Triggered, *Event, Layers, Cameras);
            if(StartingEvent != Event){
                Event = MemoryStack.back();
                MemoryStack.pop_back();
                continue;
            }
            MemoryStack.clear();
            
            do{
                Event++;
                StartingEvent++;
            }while(Event->primaryTriggerTypes.size() == 0 && Event < Triggered->EveContainer.end());
            
        }while(Event < Triggered->EveContainer.end());
    }

    TriggeredObjects.clear();
}
void EngineLoop::updateTreeOfCamerasFromSelectedRoot(vector <Camera2D> & Cameras, Camera2D * Selected){
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
                    if(!Cameras[j].isActive || i == j || tokensOfChanges[j] > 0){
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
void EngineLoop::updateAllForestOfCameras(vector <Camera2D> & Cameras){
    //Find all roots of Cameras' graph forest and update each tree.
    for(Camera2D & Camera : Cameras){
        if((Camera.isActive && !Camera.isPinnedToCamera) || (Camera.isPinnedToCamera && Camera.pinnedCameraID == "")){
            updateTreeOfCamerasFromSelectedRoot(Cameras, &Camera);
        }
    }
}
void EngineLoop::updateCamerasPositions(vector <Camera2D> & Cameras){
    if(SelectedCamera != nullptr && SelectedCamera->getIsActive() && isKeyPressed(ALLEGRO_KEY_LSHIFT) && Mouse.isPressed(0)){
        SelectedCamera->relativePos = Mouse.getPos();
        SelectedCamera->pos = SelectedCamera->relativePos;
        for(Camera2D & Camera : Cameras){
            if(Camera.isActive && SelectedCamera->pinnedCameraID == Camera.ID){
                SelectedCamera->relativePos -= Camera.pos;
                break;
            }
        }
        updateTreeOfCamerasFromSelectedRoot(Cameras, SelectedCamera);
    }
}
void EngineLoop::focusOnCamera(vector <Camera2D> & Cameras){
    for(Camera2D & Camera : Cameras){
        if(!Camera.getIsActive()){
            continue;
        }
        if(Mouse.inRectangle(Camera.pos, Camera.size, true)){
            SelectedCamera = &Camera;
            Mouse.updateZoomPos(*SelectedCamera);
            break;
        }
    }
}
bool EngineLoop::isKeyFirstPressed(short key){
    for(short firstPressed : firstPressedKeys){
        if(firstPressed == key){
            return true;
        }
    }
    return false;
}
bool EngineLoop::isKeyPressed(short key){
    for(short pressed : pressedKeys){
        if(pressed == key){
            return true;
        }
    }
    return false;
}
bool EngineLoop::isKeyReleased(short key){
    for(short released : releasedKeys){
        if(released == key){
            return true;
        }
    }
    return false;
}
void EngineLoop::detectStartPosOfDraggingObjects(){
    wasMousePressedInSelectedObject = false;

    if(
        SelectedCamera != nullptr && SelectedCamera->getIsActive()
        && SelectedLayer != nullptr && SelectedObject != nullptr
        && Mouse.inRectangle(SelectedCamera->pos, SelectedCamera->size, true)
        && SelectedCamera->isLayerVisible(SelectedLayer->getID())
        && SelectedCamera->isLayerAccessible(SelectedLayer->getID())
    ){
        if(SelectedObject->getIsAttachedToCamera()){
            if(Mouse.inRectangle(SelectedCamera->pos + SelectedObject->getPos(false)+SelectedLayer->pos, SelectedObject->getSize(), true)){
                wasMousePressedInSelectedObject = true;
                dragStartingPos.set(Mouse.getPos()-SelectedObject->getPos(false));
            }
        }
        else{
            if(Mouse.inRectangle(SelectedCamera->pos / SelectedCamera->zoom + SelectedObject->getPos(false) + SelectedCamera->visionShift, SelectedObject->getSize(), false)){
                wasMousePressedInSelectedObject = true;
                dragStartingPos.set(Mouse.getZoomPos()-SelectedObject->getPos(false)-SelectedCamera->visionShift);
            }
        }
    }
}
void EngineLoop::startScrollbarDragging(vector <LayerClass> & Layers){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()){
        return;
    }
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive() || !SelectedCamera->isLayerVisible(Layer.getID()) || !SelectedCamera->isLayerAccessible(Layer.getID())){
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
void EngineLoop::dragScrollbars(vector <LayerClass> & Layers){
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
void EngineLoop::moveParticles(vector <LayerClass> & Layers){
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
void EngineLoop::detectBackgroundCollisions(LayerClass & Layer, AncestorObject & Object, vec2d momentum){
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
bool EngineLoop::shouldCheckOverlapingOnly(CollisionModule & Collision, AncestorObject & SolidObject, CollisionModule & SolidCollision){
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
void EngineLoop::detectRealCollisions(LayerClass & Layer, AncestorObject & Object, MovementModule & Movement){
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
void EngineLoop::adjustAndStopMomentum(AncestorObject & Object, MovementModule & Movement){
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
void EngineLoop::updateCameraPosition(Camera2D & Camera, AncestorObject * FollowedByCamera){
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
void EngineLoop::moveObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras){
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
                Movement.updateStatesAndVectors(pressedKeys, Mouse.getPressed(), Mouse.getReleased(), Mouse.getZoomPos(), Object.getPos(false));
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
void EngineLoop::moveSelectedObject(vector <EditableTextModule> & EditableTextContainer){
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
                SelectedObject->setPos(Mouse.getZoomPos()-SelectedCamera->visionShift-dragStartingPos);
            }
        }
        updateEditorWindowOnAxisChange(EditableTextContainer);
    }
}
void EngineLoop::triggerEvents(vector <LayerClass> & Layers, short eventsType){
    VariableModule * EventVariable, * ObjectVariable;
    bool isScrollable;
    unsigned int objIndex = 0, eventIndex = 0,  operationIndex = 0;

    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive()){
            continue;
        }
        objIndex = 0;
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getIsActive()){
                objIndex++;
                continue;
            }
            eventIndex = 0;
            for(EventModule & Event : Object.EventsContainer){
                if(!Event.getIsActive()){
                    eventIndex++;
                    continue;
                }
                operationIndex = 0;
                for(OperationClass & Operation : Event.Operations){
                    EventVariable = nullptr;
                    ObjectVariable = nullptr;
                    if(eventsType == 0){
                        if(Event.getTypeOfTrigger(operationIndex) != "mouse_release"){
                            if(isStringInGroup(Event.getTypeOfTrigger(operationIndex),
                                           6, "==", "!=", ">", "<", ">=", "<=")){
                                string itsSearchForID = Operation.eventVariableID;
                                EventVariable = findByIDAndReturnPointer(Object.VariablesContainer, itsSearchForID);
                                if(EventVariable == nullptr){
                                    continue;
                                }
                                string connectedObjID = Operation.affectedObjectID;
                                AncestorObject * ConnectedObj;
                                ConnectedObj = findByIDAndReturnPointer(Layer.Objects, connectedObjID);
                                if(ConnectedObj == nullptr){
                                    continue;
                                }
                                string searchForID = Operation.bindedToTrigger;
                                ObjectVariable = findByIDAndReturnPointer(ConnectedObj->VariablesContainer, searchForID);
                                if(ObjectVariable == nullptr){
                                    continue;
                                }
                            }
                            isScrollable = Event.getIsScrollable();
                            if(Event.isEventTriggered(operationIndex, Mouse, Object.getPos(isScrollable), EventVariable, ObjectVariable)){
                                Object.operateEvent(objIndex, eventIndex, operationIndex, Layer.Objects);
                            }
                        }
                    }
                    else if(eventsType == 1){
                        if(Event.getTypeOfTrigger(operationIndex) == "mouse_release"){
                            isScrollable = Event.getIsScrollable();
                            if(Event.isEventTriggered(operationIndex, Mouse, Object.getPos(isScrollable), EventVariable, ObjectVariable)){
                                Object.operateEvent(objIndex, eventIndex, operationIndex, Layer.Objects);
                            }
                        }
                    }
                    operationIndex++;
                }
                eventIndex++;
            }
            objIndex++;
        }
    }
}
void EngineLoop::drawObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer){
    int numberOfDrawnObjects;
    unsigned int i;

    for(Camera2D & Camera : Cameras){
        al_set_target_bitmap(Camera.bitmapBuffer);
        al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
        for(LayerClass & Layer : Layers){
            if(!Layer.getIsActive() || !Camera.isLayerVisible(Layer.getID())){
                continue;
            }
            
            numberOfDrawnObjects = 0;
            foregroundOfObjects.clear();

            for(int currentlyDrawnLayer = 0; currentlyDrawnLayer < totalNumberOfBitmapLayers; currentlyDrawnLayer++){
                for(i = 0; i < Layer.Objects.size(); i++){
                    if(Layer.Objects[i].getIsActive())
                        drawModules(Layer.Objects[i], i, Camera, FontContainer, currentlyDrawnLayer, numberOfDrawnObjects, foregroundOfObjects, false);
                }
            }
            for(i = 0; i < foregroundOfObjects.size(); i++){
                if(Layer.Objects[foregroundOfObjects[i]].getIsActive())
                    drawModules(Layer.Objects[foregroundOfObjects[i]], i, Camera, FontContainer, -1, numberOfDrawnObjects, foregroundOfObjects, true);
            }
            if(SelectedLayer == &Layer)
                drawSelectionBorder(Camera);
        }
        al_set_target_bitmap(al_get_backbuffer(window));
        al_draw_bitmap(Camera.bitmapBuffer, Camera.pos.x, Camera.pos.y, 0);

        foregroundOfObjects.clear();
        //vector<unsigned int>().swap(foregroundOfObjects);
        //foregroundOfObjects.shrink_to_fit();
        if(SelectedCamera != &Camera)
            al_draw_rectangle(Camera.pos.x, Camera.pos.y, Camera.pos.x + Camera.size.x, Camera.pos.y + Camera.size.y, al_map_rgb(0, 155, 145), 4);
        else
            al_draw_rectangle(Camera.pos.x, Camera.pos.y, Camera.pos.x + Camera.size.x, Camera.pos.y + Camera.size.y, al_map_rgb(62, 249, 239), 4);
    }
}
void EngineLoop::drawModules(AncestorObject & Object, unsigned int iteration, Camera2D & Camera, vector <SingleFont> & FontContainer, int currentlyDrawnLayer, int & numberOfDrawnObjects,
                             vector <unsigned int> & foregroundOfObjects, bool isTimeForForeground){
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
                Text.drawText(Object.getPos(isScrollable), font.font, drawTextFieldBorders, Camera, 0, false);
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
                    drawTextFieldBorders, Camera, Editable.getCursorPos(), Editable.getEditingIsActive());
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
        
        ParticleEffect.drawParticles(Object.ImageContainer, vec2i(SCREEN_W, SCREEN_H), Camera);
        numberOfDrawnObjects++;
    }
}
void EngineLoop::updateEditableTextFields(vector <LayerClass> & Layers, vector <SingleBitmap> & BitmapContainer){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()){
        return;
    }
    if(Mouse.isPressed() == true){
        vec2d finalPos, finalSize;
        bool isScrollable;
        for(LayerClass & Layer : Layers){
            if(!Layer.getIsActive() || !SelectedCamera->isLayerVisible(Layer.getID()) || !SelectedCamera->isLayerAccessible(Layer.getID())){
                continue;
            }
            for(AncestorObject & Object : Layer.Objects){
                for(EditableTextModule & TextField : Object.EditableTextContainer){
                    if(!TextField.getIsActive()){
                        continue;
                    }
                    if(TextField.getCanBeEdited() == true){
                        
                        isScrollable = TextField.getIsScrollable();
                        finalPos.set(Object.getPos(isScrollable));
                        finalPos.translate(TextField.getPos(false));
                        if(!TextField.getIsAttachedToCamera()){
                            finalPos.translate(SelectedCamera->pos/SelectedCamera->zoom + SelectedCamera->visionShift);
                        }
                        else{
                            finalPos.translate(SelectedCamera->pos);
                        }
                        finalSize.set(TextField.getSize());
                        finalSize.multiply(TextField.getScale());
                        if(Mouse.firstPositionInRectangle(finalPos, finalSize, 0, TextField.getIsAttachedToCamera())){
                            TextField.setEditingIsActive(true);
                            TextField.setCursorPos(TextField.getContent(0).size());
                            continue;
                        }
                    }
                    if(TextField.getEditingIsActive() == true){
                        Object.operateTextFieldUpdate(TextField, Layer.Objects, BitmapContainer, Layer.listOfUniqueIDs);
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
                for(EditableTextModule & TextField : Object.EditableTextContainer){
                    if(TextField.getCanBeEdited() == true){
                        if(TextField.getEditingIsActive() == true){

                            TextField.editText(releasedKeys, pressedKeys);

                            if(TextField.getUpdateConnectedVariable() == true){
                                Object.operateTextFieldUpdate(TextField, Layer.Objects, BitmapContainer, Layer.listOfUniqueIDs);
                                TextField.setUpdateConnectedVariable(false);

                                if(SelectedLayer != nullptr && SelectedObject != nullptr && Object.getID() == "editor_window"){
                                    Object.EditableTextContainer[0].modifyContent(0, SelectedObject->getID());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
void EngineLoop::selectObject(vector <LayerClass> & Layers, vector <SingleBitmap> & BitmapContainer, vector <SingleFont> & FontContainer){
    if(SelectedCamera == nullptr || !SelectedCamera->getIsActive()){
        return;
    }
    
    for(LayerClass & Layer : Layers){
        if(!Layer.getIsActive() || !SelectedCamera->isLayerVisible(Layer.getID()) || !SelectedCamera->isLayerAccessible(Layer.getID())){
            continue;
        }
        
        //Check if mouse cursor was in the camera box while mouse button was pressed.  
        if(!Mouse.pressedInRectangle(SelectedCamera->pos, SelectedCamera->size, 0, true)){
            continue;
        }
        for(AncestorObject & Object : Layer.Objects){
            if(!Object.getCanBeSelected()){
                continue;
            }
            if(Mouse.pressedInRectangle(Object.getPosOnCamera(SelectedCamera), Object.getSize(), 0, Object.getIsAttachedToCamera())){
                if(&Layer == SelectedLayer && &Object == SelectedObject){
                    return;
                }
                selectedObjectLayerID = Layer.getID();
                selectedObjectID = Object.getID();
                SelectedLayer = &Layer;
                SelectedObject = &Object;
                SelectedObject->refreshCoordinates();
                //std::cout << "Selected: " << SelectedObject->getID() << "\n";

                /*EditorObject = &Layers[0].Objects[0];
                THERE IS A MEMORY LEAK HERE
                char lastActiveCategory = getActiveEditorWindowCategory(EditorObject);
                //std::cout << "Last active category: " << lastActiveCategory << "\n";

                removeListsInEditorWindow(EditorObject);

                EditorWindowArrangement Arr;
                Arr.labelHeight = getFontHeight(FontContainer, Arr.labelFontID);
                Arr.attributeSize.y = getFontHeight(FontContainer, Arr.attributeFontID);
                prepareEditorWindowImage(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowText(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowMovement(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowCollisions(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowEvents(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowParticles(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowVariables(EditorObject, FontContainer, BitmapContainer, Arr);
                prepareEditorWindowEditable(EditorObject, FontContainer, BitmapContainer, Arr);

                activateBasedOnFirstChar(EditorObject, lastActiveCategory);
                deactivateWrapped((int)(lastActiveCategory - '0'), EditorObject);

                updateEditorWindowOnSelection(EditorObject->EditableTextContainer);*/

                return;
            }
        }
    }
}
void EngineLoop::unselectObject(){
    selectedObjectLayerID = "";
    selectedObjectID = "";
    SelectedLayer = nullptr;
    SelectedObject = nullptr;
}
void EngineLoop::updateEditorWindowOnSelection(vector <EditableTextModule> & EditableTextContainer){
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
void EngineLoop::updateEditorWindowOnAxisChange(vector <EditableTextModule> & EditableTextContainer){
    if(!EditableTextContainer[0].getIsActive()){
        return;
    }

    EditableTextContainer[1].modifyContent(0, std::to_string(SelectedObject->getPos(false).x));
    EditableTextContainer[2].modifyContent(0, std::to_string(SelectedObject->getPos(false).y));
}

bool EngineLoop::secondHasPassed(){
    return al_get_timer_count(timer) % (int)FPS == 0;
}

void EngineLoop::drawSelectionBorder(Camera2D Camera){
    if(SelectedObject == nullptr){
        return;
    }
    vec2d borderPos(SelectedObject->getPos(false));
    vec2d borderSize(SelectedObject->getSize());
    if(!SelectedObject->getIsAttachedToCamera()){
        borderSize.multiply(Camera.zoom);
        borderPos.set(Camera.translateWithZoom(borderPos));
        al_draw_rectangle(borderPos.x, borderPos.y, borderPos.x + borderSize.x, borderPos.y + borderSize.y, al_map_rgb(80, 154, 213), 4*Camera.zoom);
    }
    else{
        al_draw_rectangle(borderPos.x, borderPos.y, borderPos.x + borderSize.x, borderPos.y + borderSize.y, al_map_rgb(216, 78, 213), 4);
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
            EditorWindow->TextContainer.resize(i, TextModule(0));
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->ImageContainer.size(); i++){
        if(isCharInGroup(EditorWindow->ImageContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->ImageContainer.resize(i, ImageModule(0));
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->EditableTextContainer.size(); i++){
        if(isCharInGroup(EditorWindow->EditableTextContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->EditableTextContainer.resize(i, EditableTextModule(0));
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->CollisionContainer.size(); i++){
        if(isCharInGroup(EditorWindow->CollisionContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->CollisionContainer.resize(i, CollisionModule(0));
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->EventsContainer.size(); i++){
        if(isCharInGroup(EditorWindow->EventsContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->EventsContainer.resize(i, EventModule(0));
            break;
        }
    }
    for(unsigned int i = 0; i < EditorWindow->VariablesContainer.size(); i++){
        if(isCharInGroup(EditorWindow->VariablesContainer[i].getID()[0], 8, "2", "3", "4", "5", "6", "7", "8", "9")){
            EditorWindow->VariablesContainer.resize(i, VariableModule(0));
            break;
        }
    }
}

void prepareEditorWindow(vector <AncestorObject> & Objects, string layerID, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer){
    EditorWindowArrangement Arr;
    Arr.labelHeight = getFontHeight(FontContainer, Arr.labelFontID);
    Arr.attributeSize.y = getFontHeight(FontContainer, Arr.attributeFontID);

    Objects.push_back(AncestorObject(Objects.size(), layerID));
    AncestorObject * EditorWindow = &Objects.back();

    EditorWindow->setID("editor_window");
    EditorWindow->setPos(SCREEN_W-Arr.windowWidth, 0.0);
    EditorWindow->setCanBeSelected(false);
    EditorWindow->setIsAttachedToCamera(true);

    EditorWindow->ScrollbarContainer.push_back(0);
    EditorWindow->ScrollbarContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ScrollbarContainer.back().setPos(vec2d(Arr.windowWidth-45.0, Arr.buttonSize.y+Arr.margin.y+Arr.buttonMargin));
    EditorWindow->ScrollbarContainer.back().setThumbPos(vec2d(0.0, 0.0));
    EditorWindow->ScrollbarContainer.back().setThumbSize(Arr.thumbSize);
    EditorWindow->ScrollbarContainer.back().setScrollingArea(vec2d(Arr.thumbSize.x, SCREEN_H-Arr.scrollbarMargin.y-Arr.margin.y-Arr.buttonSize.y));
    EditorWindow->ScrollbarContainer.back().setRealScrollingArea(vec2d(0.0, 0.0));

    EditorWindow->ScrollbarContainer.back().setThumbImageID("thumb_1");
    EditorWindow->ScrollbarContainer.back().setTrackImageID("thumb_track_1");
    EditorWindow->ScrollbarContainer.back().setUsedBitmapLayer(-1);



    EditorWindow->VariablesContainer.push_back(0);
    EditorWindow->VariablesContainer.back().setID("true");
    EditorWindow->VariablesContainer.back().setDefaultBool(true);

    EditorWindow->ImageContainer.push_back(0);
    EditorWindow->ImageContainer.back().setID("background");
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "gui_background");
    EditorWindow->ImageContainer.back().setPos(0.0, 0.0);
    EditorWindow->ImageContainer.back().resize(vec2d(Arr.windowWidth, SCREEN_H));
    EditorWindow->TextContainer.push_back(0);
    EditorWindow->EventsContainer.push_back(0);
    for(unsigned int i = 1; i <= 9; i++){
        EditorWindow->ImageContainer.push_back(i);
        EditorWindow->ImageContainer.back().setID("icon");
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x + (i-1) * (Arr.buttonSize.x+Arr.buttonMargin), Arr.margin.y);

        EditorWindow->TextContainer.push_back(i);
        EditorWindow->TextContainer.back().setID("l_" + std::to_string(i));
        EditorWindow->TextContainer.back().setIsAttachedToCamera(true);
        EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->TextContainer.back().setPos(Arr.margin.x, Arr.margin.y+70.0);
        EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
        EditorWindow->TextContainer.back().deactivate();

        EditorWindow->EventsContainer.push_back(i);
        EditorWindow->EventsContainer.back().setID("c_" + std::to_string(i));
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

    EditorWindow->ImageContainer.push_back(0);
    EditorWindow->ImageContainer.back().setID("thumb_1");
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "thumb_1");
    EditorWindow->ImageContainer.back().resize(EditorWindow->ScrollbarContainer.back().getThumbSize());
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().deactivate();

    EditorWindow->ImageContainer.push_back(0);
    EditorWindow->ImageContainer.back().setID("thumb_track_1");
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "thumb_track_1");
    EditorWindow->ImageContainer.back().resize(EditorWindow->ScrollbarContainer.back().getScrollingArea());
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().deactivate();

    std::cout << "Editor Window:\n-Text: " << EditorWindow->TextContainer.size() << "\n-Image: " << EditorWindow->ImageContainer.size() << "\n-Editable: " << EditorWindow->EditableTextContainer.size() << "\n-Events: " << EditorWindow->EventsContainer.size() << "\n";
}
void prepareEditorWindowGeneral(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    unsigned int labelIndex = EditorWindow->TextContainer.size();
    unsigned int attrIndex = EditorWindow->EditableTextContainer.size();
    for(unsigned int attr = 0; attr < 7; attr++){
        EditorWindow->TextContainer.push_back(1);
        EditorWindow->TextContainer.back().setIsAttachedToCamera(true);;
        EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->TextContainer.back().setPos(Arr.margin.x, Arr.attributeStart + attr*(Arr.labelHeight + Arr.labelMargin+Arr.attributeSize.y+Arr.attributeMargin));
        EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
        EditorWindow->TextContainer.back().setIsScrollable(true);

        EditorWindow->EditableTextContainer.push_back(1);
        EditorWindow->EditableTextContainer.back().setFontID(Arr.attributeFontID);
        EditorWindow->EditableTextContainer.back().setPos(Arr.margin.x+Arr.editablePadding.x, Arr.attributeStart + Arr.labelHeight + Arr.labelMargin + Arr.editablePadding.y + attr*(Arr.labelHeight + Arr.labelMargin+Arr.attributeSize.y+Arr.attributeMargin));
        EditorWindow->EditableTextContainer.back().setSize(Arr.attributeSize);
        EditorWindow->EditableTextContainer.back().addNewContent("");
        EditorWindow->EditableTextContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->EditableTextContainer.back().setVisibility(1.0);
        EditorWindow->EditableTextContainer.back().setIsScrollable(true);

        EditorWindow->ImageContainer.push_back(1);
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "text_field_1");
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x, Arr.attributeStart + Arr.labelHeight + Arr.labelMargin + attr*(Arr.labelHeight + Arr.labelMargin+Arr.attributeSize.y+Arr.attributeMargin)-Arr.attributePadding.y);
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

void EngineLoop::prepareEditorWindowObjectsList(int categoryIndex, AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){

    unsigned int containerSize = 0;

    EditorWindow->TextContainer.push_back(categoryIndex);
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


    EditorWindow->ImageContainer.push_back(categoryIndex);
    EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
    EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
    EditorWindow->ImageContainer.back().setPos(Arr.margin.x, Arr.attributeStart-Arr.wraperSize.y/2);
    EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "plus_1");
    EditorWindow->ImageContainer.back().resize(Arr.wraperSize);
    EditorWindow->ImageContainer.back().setIsScrollable(true);
    EditorWindow->ImageContainer.back().deactivate();

    unsigned int objNumber = 1;

    for(unsigned int vectorNumber = objNumber; vectorNumber <= containerSize; vectorNumber++){
        EditorWindow->TextContainer.push_back(std::to_string(categoryIndex));
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

        EditorWindow->ImageContainer.push_back(wrapperID);
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

        EditorWindow->VariablesContainer.push_back(0);
        EditorWindow->VariablesContainer.back().setID(wrapperID);
        EditorWindow->VariablesContainer.back().setDefaultBool(false);


        EditorWindow->ImageContainer.push_back(categoryIndex);
        EditorWindow->ImageContainer.back().setIsAttachedToCamera(true);
        EditorWindow->ImageContainer.back().setUsedBitmapLayer(-1);
        EditorWindow->ImageContainer.back().setPos(Arr.margin.x+Arr.wraperSize.x, Arr.attributeStart + objNumber * Arr.containerHeight);
        EditorWindow->ImageContainer.back().connectBitmap(BitmapContainer, "text_field_2");
        EditorWindow->ImageContainer.back().resize(Arr.windowWidth-Arr.wraperSize.x*3-Arr.scrollbarMargin.x, Arr.wraperSize.y);
        EditorWindow->ImageContainer.back().setIsScrollable(true);
        EditorWindow->ImageContainer.back().deactivate();

        EditorWindow->ImageContainer.push_back(categoryIndex);
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
            EditorWindow->TextContainer.push_back(categoryIndex);
            EditorWindow->TextContainer.back().setIsAttachedToCamera(true);;
            EditorWindow->TextContainer.back().setUsedBitmapLayer(-1);
            EditorWindow->TextContainer.back().setPos(Arr.margin.x + Arr.indentation, Arr.attributeStart + Arr.labelMoveAhead + attr * Arr.containerHeight);
            EditorWindow->TextContainer.back().setFontID(Arr.labelFontID);
            EditorWindow->TextContainer.back().setIsScrollable(true);
            EditorWindow->TextContainer.back().deactivate();

            EditorWindow->EditableTextContainer.push_back(categoryIndex);
            EditorWindow->EditableTextContainer.back().setFontID(Arr.attributeFontID);
            EditorWindow->EditableTextContainer.back().setPos(Arr.margin.x + Arr.indentation + Arr.editablePadding.x, Arr.attributeStart + Arr.editablePadding.y + (attr+1) * Arr.containerHeight);
            EditorWindow->EditableTextContainer.back().setSize(Arr.attributeSize.x-Arr.scrollbarMargin.x, Arr.attributeSize.y);
            EditorWindow->EditableTextContainer.back().addNewContent("");
            EditorWindow->EditableTextContainer.back().setUsedBitmapLayer(-1);
            EditorWindow->EditableTextContainer.back().setVisibility(1.0);
            EditorWindow->EditableTextContainer.back().setIsScrollable(true);
            EditorWindow->EditableTextContainer.back().deactivate();

            EditorWindow->ImageContainer.push_back(categoryIndex);
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
        /*EditorWindow->TextContainer[labelIndex+2].addNewContentAndResize("Position Y:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+2].setConnectedObject("", "ancestor", "", "position_y");
        EditorWindow->TextContainer[labelIndex+3].addNewContentAndResize("Size X:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+3].setConnectedObject("", "ancestor", "", "size_x");
        EditorWindow->TextContainer[labelIndex+4].addNewContentAndResize("Size Y:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+4].setConnectedObject("", "ancestor", "", "size_y");
        EditorWindow->TextContainer[labelIndex+5].addNewContentAndResize("Scaled from center:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+5].setConnectedObject("", "ancestor", "", "is_scaled_from_center");
        EditorWindow->TextContainer[labelIndex+6].addNewContentAndResize("Attached to camera:", FontContainer);
        EditorWindow->EditableTextContainer[attrIndex+6].setConnectedObject("", "ancestor", "", "is_attached_to_camera");*/
    }
}
void EngineLoop::prepareEditorWindowImage(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(2, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowText(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(3, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowMovement(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(4, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowCollisions(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(5, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowEvents(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(6, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowParticles(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(7, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowVariables(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(8, EditorWindow, FontContainer, BitmapContainer, Arr);
}
void EngineLoop::prepareEditorWindowEditable(AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr){
    prepareEditorWindowObjectsList(9, EditorWindow, FontContainer, BitmapContainer, Arr);
}

