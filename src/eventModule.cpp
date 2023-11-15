#include "eventModule.h"

ConditionClass::ConditionClass(unsigned int newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass(string newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass() : Literal(){}

OperaClass::OperaClass(){
    instruction = "";
}

void EveModule::clone(const EveModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}

void EveModule::setUpNewInstance(){
    conditionalStatus = 'n';
    elseChildID = "";
    areDependentOperationsDone = false;
    elseChildFinished = false;
    loop = false;
    willBeDeleted = false;
}
EveModule::EveModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
EveModule::EveModule(unsigned int eventModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(eventModuleID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
EveModule::EveModule(string eventModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(eventModuleID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
void EveModule::clear(){
    ConditionalChain.clear();
    PostOperations.clear();
    Children.clear();
}
void EveModule::resetStatus(){
	for(ChildStruct & Child : Children){
		Child.finished = false;
	}
}
bool EveModule::checkIfAllChildrenFinished(){
    for(ChildStruct Child : Children){
        if(!Child.finished){
            return false;
        }
    }
    return true;
}

void EveModule::controlText(TextModule * Text, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer){
    if(attribute == "set_id" && Values.size() > 0){
        Text->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Text->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Text->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        Text->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_scale" && Values.size() >= 2){
        Text->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        Text->setColors(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "set_color_r" && Values.size() >= 1){
        Text->color.r = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_g" && Values.size() >= 1){
        Text->color.g = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_b" && Values.size() >= 1){
        Text->color.b = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_a" && Values.size() >= 1){
        Text->color.a = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_random_color"){
        Text->setRandomColors();
    }
    else if(attribute == "increment_random_color"){
        Text->incrementRandomColor();
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        Text->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_random_change_speed" && Values.size() >= 1){
        Text->randomChangeSpeed = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_min_color" && Values.size() >= 1){
        Text->minColorValue = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_color" && Values.size() >= 1){
        Text->maxColorValue = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "rotate" && Values.size() >= 1){
        Text->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_font" && Values.size() > 0){
        Text->setFontID(Values[0].getStringUnsafe());
    }
    else if(attribute == "add" && Values.size() > 0){
        Text->addNewContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_and_update" && Values.size() > 0){
        Text->addNewContentAndResize(Values[0].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "update_size" && Values.size() > 0){
        Text->fitSizeToText(FontContainer);
    }
    else if(attribute == "select_content" && Values.size() > 0){
        Text->chooseContent(Values[0].getIntUnsafe());
    }
    else if(attribute == "delete" && Values.size() > 0){
        Text->deleteContent(Values[0].getIntUnsafe());
    }
    else if(attribute == "update_text" && Values.size() > 1){
        Text->modifyContentAndResize(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "select_layer" && Values.size() > 0){
        Text->setUsedBitmapLayer(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_wrapping" && Values.size() > 0){
        Text->setWrapping(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_horizontal_align" && Values.size() > 0){
        Text->setHorizontalAlign(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_vertical_align" && Values.size() > 0){
        Text->setVerticalAlign(Values[0].getIntUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Text->control(attribute, temp, Values.size());
    }
}
void EveModule::controlEditableText(EditableTextModule *EditableText, string attribute, const vector<VariableModule> &Values, vector<string> &IDs, const vector<SingleFont> &FontContainer){
    if(attribute == "set_editable" && Values.size() > 0){
        EditableText->setCanBeEdited(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_space_use" && Values.size() > 0){
        EditableText->setCanUseSpace(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_numerical" && Values.size() > 0){
        EditableText->setIsNumerical(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_floating_point" && Values.size() > 0){
        EditableText->setHasFloatingPoint(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_can_update_variable" && Values.size() > 0){
        EditableText->setUpdateConnectedVariable(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_auto_cleaning" && Values.size() > 0){
        EditableText->setCanClearContentAfterSuccess(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_use_tabs" && Values.size() > 0){
        EditableText->setUseTabs(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_min_content_length" && Values.size() > 0){
        EditableText->setMinContentSize(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_max_content_length" && Values.size() > 0){
        EditableText->setMaxContentSize(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_input_delay" && Values.size() > 0){
        EditableText->setInputDelay(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_repetition_delay" && Values.size() > 0){
        EditableText->setRepetitionDelay(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "connect_object" && Values.size() > 0){
        EditableText->connectedObject = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_group" && Values.size() > 0){
        EditableText->connectedGroup = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_module" && Values.size() > 0){
        EditableText->connectedModule = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_module_id" && Values.size() > 0){
        EditableText->connectedModuleID = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_variable" && Values.size() > 0){
        EditableText->connectedVariable = Values[0].getStringUnsafe();
    }
    else{
        controlText(EditableText, attribute, Values, IDs, FontContainer);
    }
}
void EveModule::controlImage(ImageModule *Image, string attribute, const vector<VariableModule> &Values, vector<string> &IDs, vector<SingleBitmap> &BitmapContainer, string EXE_PATH){
    if(attribute == "set_id" && Values.size() > 0){
        Image->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Image->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Image->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        Image->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "resize" && Values.size() >= 2){
        Image->resize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        Image->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "rotate" && Values.size() >= 1){
        Image->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_mirror" && Values.size() >= 2){
        Image->setMirror(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        float arr[] = {float(Values[0].getDoubleUnsafe()), float(Values[1].getDoubleUnsafe()), float(Values[2].getDoubleUnsafe()), float(Values[3].getDoubleUnsafe())};
        Image->setImageColor(arr);
    }
    else if(attribute == "set_light" && Values.size() >= 4){
        Image->setLightColor(vec3d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe()), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "connect_bitmap" && Values.size() > 0){
        if(Values.size() == 1){
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", EXE_PATH);
        }
        else{
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), Values[1].getStringUnsafe(), EXE_PATH);
        }   
    }
    else if(attribute == "connect_bitmap_via_path" && Values.size() > 0){
        Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", EXE_PATH);  
    }
    else if(attribute == "connect_bitmap_via_alias" && Values.size() > 0){
        Image->connectBitmap(BitmapContainer, "", Values[0].getStringUnsafe(), EXE_PATH);  
    }
    else if(attribute == "select_layer" && Values.size() > 0){
        Image->setUsedBitmapLayer(Values[0].getIntUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Image->control(attribute, temp, Values.size());
    }
}
void EveModule::controlMovement(MovementModule * Movement, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "move" && Values.size() >= 2){
        Movement->addMomentum(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "move_up"){
        Movement->setNextMove(true, false, false, false, false, false, false);
    }
    else if(attribute == "move_right"){
        Movement->setNextMove(false, true, false, false, false, false, false);
    }
    else if(attribute == "move_down"){
        Movement->setNextMove(false, false, true, false, false, false, false);
    }
    else if(attribute == "move_left"){
        Movement->setNextMove(false, false, false, true, false, false, false);
    }
    else if(attribute == "move_jump"){
        Movement->setNextMove(false, false, false, false, true, false, false);
    }
    else if(attribute == "move_fall"){
        Movement->setNextMove(false, false, false, false, false, true, false);
    }
    else if(attribute == "move_run"){
        Movement->setNextMove(false, false, false, false, false, false, true);
    }
    else if(attribute == "set_id" && Values.size() > 0){
        Movement->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_movement_type" && Values.size() > 0){
        Movement->setMovementType(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_input_type" && Values.size() > 0){
        Movement->setInputType(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_allowed_jumps" && Values.size() > 0){
        Movement->setAllowedJumps(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_jump_cooldown" && Values.size() > 0){
        Movement->setJumpCooldownDuration(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_move_cooldown" && Values.size() > 0){
        Movement->setMoveCooldownDuration(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_diagonal_movement" && Values.size() > 0){
        Movement->setCanMoveDiagonally(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_reset_momentum_when_jumping" && Values.size() > 0){
        Movement->setResetMomentumWhenJumping(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_mass" && Values.size() > 0){
        Movement->setBodyMass(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_walking_speed" && Values.size() > 0){
        Movement->setWalkingSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_running_speed" && Values.size() > 0){
        Movement->setRunningSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_jump_speed" && Values.size() > 0){
        Movement->setJumpSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_gravity" && Values.size() > 0){
        Movement->setGravitation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_friction" && Values.size() > 0){
        Movement->setBaseFriction(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_max_momentum_x" && Values.size() > 0){
        Movement->setMaxMomentumX(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_momentum_y" && Values.size() > 1){
        Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
        Movement->setMaxMomentumY(Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_min_momentum_y" && Values.size() > 0){
        Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_max_momentum_y" && Values.size() > 0){
        Movement->setMaxMomentumY(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_mouse_movement_button" && Values.size() > 0){
        Movement->setMouseButton(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_move_on_mouse_release" && Values.size() > 0){
        Movement->setMoveOnMouseRelease(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_direction_reset_after_collision" && Values.size() > 0){
        Movement->setResetDirectionAfterCollision(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_moving_keys" && Values.size() > 5){
        Movement->bindKeys(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe(), Values[4].getIntUnsafe(), Values[5].getIntUnsafe());
    }
    else if(attribute == "set_key_up" && Values.size() > 0){
        Movement->setUpKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_right" && Values.size() > 0){
        Movement->setRightKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_down" && Values.size() > 0){
        Movement->setDownKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_left" && Values.size() > 0){
        Movement->setLeftKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_jump" && Values.size() > 0){
        Movement->setJumpKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_running" && Values.size() > 0){
        Movement->setRunningKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_random_actions" && Values.size() > 0){
        Movement->areRandomActionsEnabled = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_action_time" && Values.size() > 1){
        Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
        Movement->maxTimeOfAction = Values[1].getDoubleUnsafe();
    }
    else if(attribute == "set_min_action_time" && Values.size() > 0){
        Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_action_time" && Values.size() > 0){
        Movement->maxTimeOfAction = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_move_distance" && Values.size() > 1){
        Movement->minMoveDistance = Values[0].getDoubleUnsafe();
        Movement->maxMoveDistance = Values[1].getDoubleUnsafe();
    }
    else if(attribute == "set_min_move_distance" && Values.size() > 0){
        Movement->minMoveDistance = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_move_distance" && Values.size() > 0){
        Movement->maxMoveDistance = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_breaks_allowed" && Values.size() > 0){
        Movement->areBreaksAllowed = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_break_chance" && Values.size() > 0){
        Movement->chanceForBreak = Values[0].getDoubleUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Movement->control(attribute, temp, Values.size());
    }
}
void EveModule::controlCollision(CollisionModule * Collision, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Collision->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Collision->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Collision->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_solid" && Values.size() > 0){
        Collision->setIsSolid(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_solid"){
        Collision->switchSolid();
    }
    else if(attribute == "set_penetration" && Values.size() > 0){
        Collision->setCanPenetrateSolids(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_penetration"){
        Collision->switchSolidPenetration();
    }
    else if(attribute == "ignore" && Values.size() > 1){
        Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "stop_ignoring" && Values.size() > 1){
        Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "clear_ignoring" && Values.size() > 0){
        Collision->clearIgnoreList(Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_object" && Values.size() > 0){
        Collision->addToIgnoreList("objects", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_group_of_objects" && Values.size() > 0){
        Collision->addToIgnoreList("groups_of_objects", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_hitbox" && Values.size() > 0){
        Collision->addToIgnoreList("hitboxes", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_group_of_hitboxes" && Values.size() > 0){
        Collision->addToIgnoreList("groups_of_hitboxes", Values[0].getStringUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Collision->control(attribute, temp, Values.size());
    }
}
void EveModule::controlParticles(ParticleEffectModule * Particles, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Particles->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_environment" && Values.size() >= 2){
        Particles->setEnvironment(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_environment_speed" && Values.size() >= 2){
        Particles->setEnvironmentSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_speed" && Values.size() > 1){
        Particles->setSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_basic_speed" && Values.size() > 1){
        Particles->setBasicSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_acceleration" && Values.size() > 1){
        Particles->setAcceleration(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_mass" && Values.size() > 1){
        Particles->setParticleMass(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_directional_degree" && Values.size() > 1){
        Particles->setDirectionDegree(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation_speed" && Values.size() > 1){
        Particles->setRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_time_to_negate_rotation" && Values.size() > 1){
        Particles->setTimeToNegateRotation(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_are_particles_moving" && Values.size() > 0){
        Particles->setAreParticlesMoving(Values[0].getBoolUnsafe());
    }
    else if(attribute == "allow_movement"){
        Particles->switchAreParticlesMoving();
    }
    else if(attribute == "set_is_env_sync" && Values.size() > 0){
        Particles->setIsEnvironmentSynchronized(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_radius" && Values.size() > 1){
        Particles->setParticleRadius(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_time_to_death" && Values.size() > 1){
        Particles->setTimeToDeath(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_shape_rotation_speed" && Values.size() > 1){
        Particles->setShapeRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color_intensity" && Values.size() > 1){
        Particles->setColorIntensity(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_color_interval" && Values.size() > 3){
        Particles->addColorInterval(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe());
    }
    else if(attribute == "add_color_interval_in_hex" && Values.size() > 0){
        Particles->addColorIntervalInHex(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_color_interval" && Values.size() > 0){
        Particles->removeColorInterval(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_color" && Values.size() > 2){
        Particles->addColor(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "add_color_in_hex" && Values.size() > 0){
        Particles->addColorInHex(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_color" && Values.size() > 0){
        Particles->removeColor(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_image" && Values.size() > 0){
        Particles->addImage(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_image" && Values.size() > 0){
        Particles->removeImage(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_image_by_index" && Values.size() > 0){
        Particles->removeImage(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_shape" && Values.size() > 0){
        Particles->setParticlesShape(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_use_images" && Values.size() > 0){
        Particles->setUseImageAsParticles(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_source"){
        Particles->switchUseImageAsParticles();
    }
    else if(attribute == "set_use_random_colors" && Values.size() > 0){
        Particles->setUseRandomColors(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_random_colors"){
        Particles->switchUseRandomColors();
    }
    else if(attribute == "set_is_static" && Values.size() > 0){
        Particles->setIsModuleStatic(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_drawing_details" && Values.size() > 0){
        Particles->setIsDrawingWithDetails(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_block_spawning" && Values.size() > 0){
        Particles->setBlockParticlesSpawn(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_spawning" && Values.size() > 0){
        Particles->setCanParticlesSpawn(Values[0].getBoolUnsafe());
    }
    else if(attribute == "allow_spawning"){
        Particles->allowSpawning();
    }
    else if(attribute == "set_spawn_rate" && Values.size() > 1){
        Particles->setParticlesPerSpawn(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_spawn_cap" && Values.size() > 0){
        Particles->setMaxParticlesCount(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_max_time_to_spawn" && Values.size() > 0){
        Particles->setMaxTimeToSpawn(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "bind_key" && Values.size() > 0){
        Particles->setSpawnKeyBind(Values[0].getStringUnsafe()[0]);
    }
    else if(attribute == "bind_key_by_int" && Values.size() > 0){
        Particles->setSpawnKeyBindShort(Values[0].getIntUnsafe());
    }
    else if(attribute == "spawn_on_key_release" && Values.size() > 0){
        Particles->setSpawnOnKeyRelease(Values[0].getBoolUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Particles->control(attribute, temp, Values.size());
    }
}
void EveModule::controlVariables(VariableModule * Variable, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Variable->setID(Values[0].getStringUnsafe(), &IDs);
    }
    else if(attribute == "toggle_bool"){
        Variable->toggleBool();
    }
    else if(attribute == "set_default_bool_random"){
        if(rand() % 2 == 0){
            Variable->toggleDefaultBool();
        }
    }
    else if(attribute == "set_default_bool_probability" && Values.size() >= 3){
        bool setBool;
        if(Values[0].getDoubleUnsafe() == 0){
            setBool = false;
        }
        else if(Values[0].getDoubleUnsafe() == 1){
            setBool = true;
        }
        else{
            return;
        }

        if(rand() % Values[2].getIntUnsafe() <= Values[1].getIntUnsafe()){
            Variable->setDefaultBool(setBool);
        }
    }
    else if(attribute == "set_default_bool_probability" && Values.size() >= 2){
        bool setBool;
        if(Values[0].getDoubleUnsafe() == 0){
            setBool = false;
        }
        else if(Values[0].getDoubleUnsafe() == 1){
            setBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Values[1].getDoubleUnsafe()){
            Variable->setDefaultBool(setBool);
        }
    }
    else if(attribute == "set_default_bool" && Values.size() >= 1){
        if(Values[0].getDoubleUnsafe() == 0){
            Variable->setDefaultBool(false);
        }
        if(Values[0].getDoubleUnsafe() == 1){
            Variable->setDefaultBool(true);
        }
    }
    else if(attribute == "set_default_int" && Values.size() >= 1){
        Variable->setDefaultInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_default_int_interval" && Values.size() >= 2){
        Variable->setDefaultInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "set_default_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setDefaultInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "set_default_double" && Values.size() >= 1){
        Variable->setDefaultDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "set_default_double_interval" && Values.size() >= 2){
        Variable->setDefaultDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "set_default_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setDefaultDouble(Values[dice].getDoubleUnsafe());
    }
    else if(attribute == "set_default_string" && Values.size() >= 1){
        Variable->setDefaultString(Values[0].getString());
    }
    else if(attribute == "set_default_string_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setDefaultString(Values[dice].getString());
    }
    else if(attribute == "set_bool_random"){
        if(rand() % 2 == 0)
        Variable->toggleBool();
    }
    else if(attribute == "set_bool_probability" && Values.size() >= 3){
        bool settingBool;
        if(Values[0].getDoubleUnsafe() == 0){
            settingBool = false;
        }
        else if(Values[0].getDoubleUnsafe() == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % Values[2].getIntUnsafe() <= Values[1].getIntUnsafe()){
            Variable->setBool(settingBool);
        }
    }
    else if(attribute == "set_bool_probability" && Values.size() >= 2){
        bool settingBool;
        if(Values[0].getDoubleUnsafe() == 0){
            settingBool = false;
        }
        else if(Values[0].getDoubleUnsafe() == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Values[1].getDoubleUnsafe()){
            Variable->setBool(settingBool);
        }
    }
    else if(attribute == "set_bool" && Values.size() >= 1){
        if(Values[0].getDoubleUnsafe() == 0){
            Variable->setBool(false);
        }
        if(Values[0].getDoubleUnsafe() == 1){
            Variable->setBool(true);
        }
    }
    else if(attribute == "set_int" && Values.size() >= 1){
        Variable->setInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_int_interval" && Values.size() >= 2){
        Variable->setInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "set_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "set_double" && Values.size() >= 1){
        Variable->setDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "set_double_interval" && Values.size() >= 2){
        Variable->setDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "set_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setDouble(Values[dice].getDoubleUnsafe());
    }
    else if(attribute == "set_string" && Values.size() >= 1){
        Variable->setString(Values[0].getString());
    }
    else if(attribute == "set_string_random" && Values.size() >= 1){
        cout << "hello ";
        unsigned int dice = rand() % Values.size();
        cout << Variable->setString(Values[dice].getString()) << " ";
        cout << Variable->getString() << "\n";
    }
    else if(attribute == "set_to_default"){
        Variable->resetValue();
    }
    else if(attribute == "add_default_int" && Values.size() >= 1){
        Variable->addDefaultInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_default_int_interval" && Values.size() >= 2){
        Variable->addDefaultInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "add_default_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addDefaultInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "add_default_double" && Values.size() >= 1){
        Variable->addDefaultDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "add_default_double_interval" && Values.size() >= 2){
        Variable->addDefaultDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "add_default_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addDefaultDouble(Values[dice].getDoubleUnsafe());
    }
    else if(attribute == "add_int" && Values.size() >= 1){
        Variable->addInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_int_interval" && Values.size() >= 2){
        Variable->addInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "add_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "add_double" && Values.size() >= 1){
        Variable->addDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "add_double_interval" && Values.size() >= 2){
        Variable->addDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "add_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addDouble(Values[dice].getDoubleUnsafe());
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": function " << attribute << "<" << Values.size() << "> does not exist.\n";
    }
}
void EveModule::controlScrollbar(ScrollbarModule * Scrollbar, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Scrollbar->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "add_real_area" && Values.size() >= 2){
        Scrollbar->addRealScrollingArea(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Scrollbar->control(attribute, temp, Values.size());
    }
}
void EveModule::controlPrimitives(PrimitivesModule * Primitives, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "update_with_size"){
        Primitives->updateWithSize();
    }
    else if(attribute == "set_id" && Values.size() > 0){
        Primitives->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Primitives->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Primitives->setSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_type" && Values.size() >= 1){
        Primitives->type = getPrimitiveType(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_point" && Values.size() >= 2){
        Primitives->points.push_back(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        Primitives->color = al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "set_thickness" && Values.size() >= 1){
        Primitives->thickness = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_radius" && Values.size() >= 1){
        Primitives->radius = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_samples" && Values.size() >= 1){
        Primitives->samples = Values[0].getIntUnsafe();
    }
    else if(attribute == "select_layer" && Values.size() >= 1){
        Primitives->usedBitmapLayer = Values[0].getIntUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Primitives->control(attribute, temp, Values.size());
    }
}

void EveModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    if(attribute == "else_child_id"){
        BasePointers.push_back(BasePointersStruct());
        BasePointers.back().setPointer(&elseChildID);
    }
    else{
        getPrimaryContext(attribute, BasePointers);
    }
}



void OperationClass::clearOperation(){
    choosenDoubles.clear();
    choosenStrings.clear();
}
short OperationClass::getBindedMouseButton(){
    if(bindedToTrigger == "left"){
        return 0;
    }
    if(bindedToTrigger == "right"){
        return 1;
    }
    if(bindedToTrigger == "middle"){
        return 2;
    }
    if(bindedToTrigger == "button_3"){
        return 3;
    }
    if(bindedToTrigger == "button_4"){
        return 4;
    }
    return 0;
}

EventModule::EventModule(){
}

EventModule::EventModule(int moduleID)
{
    vector <string> temp;
    primaryConstructor(moduleID, &temp, "", "");
    /*
    Here, fill it with all variables. Go ahead.
    */
}
string EventModule::getTypeOfTrigger(short operationID){
    return Operations[operationID].typeOfTrigger;
}
void EventModule::setUpButton(vec2d newPos, vec2d newSize){
    isButton = true;
    pos.set(newPos);
    size.set(newSize);
}
unsigned int EventModule::countOperations(){
    return Operations.size();
}
void EventModule::addOperation(string newEventVariableID, string newTypeOfTrigger, string newBindedToTrigger, string newConnectedObjectID, bool triggerItself, string newConnectedModule, string newConnectedModuleID,
        string newConnectedVariable, vecXd newChoosenDoubles, vecXs newChoosenStrings){
    Operations.push_back(OperationClass());
    Operations.back().eventVariableID = newEventVariableID;
    Operations.back().typeOfTrigger = newTypeOfTrigger;
    Operations.back().bindedToTrigger = newBindedToTrigger;
    Operations.back().affectedObjectID = newConnectedObjectID;
    Operations.back().isTriggeringItself = triggerItself;
    Operations.back().affectedObjectIndex = 0;
    Operations.back().affectedModule = newConnectedModule;
    Operations.back().affectedModuleID = newConnectedModuleID;
    Operations.back().affectedVariable = newConnectedVariable;

    Operations.back().choosenDoubles.reserve(newChoosenDoubles.val.size());
    for(unsigned int i = 0; i < newChoosenDoubles.val.size(); i++){
        Operations.back().choosenDoubles.push_back(newChoosenDoubles.val[i]);
    }


    Operations.back().choosenStrings.reserve(newChoosenStrings.val.size());
    for(unsigned int i = 0; i < newChoosenStrings.val.size(); i++)
        Operations.back().choosenStrings.push_back(newChoosenStrings.val[i]);
}
void EventModule::resetOperations(){
    for(OperationClass & Operation : Operations){
        Operation.clearOperation();
    }
    Operations.clear();
}
void EventModule::clear(){
    resetOperations();
}
bool EventModule::isEventTriggered(int operationID, MouseClass mouse, vec2d objPos, VariableModule * EventVariable, VariableModule * ObjectVariable){
    if(!isActive)
        return false;
    if(Operations[operationID].typeOfTrigger == "mouse_press"){
        if(mouse.pressedInRectangle(pos+objPos, size, Operations[operationID].getBindedMouseButton(), isAttachedToCamera))
            return true;
    }
    else if(Operations[operationID].typeOfTrigger == "mouse_no_press"){
        if(!mouse.pressedInRectangle(pos+objPos, size, Operations[operationID].getBindedMouseButton(), isAttachedToCamera))
            return true;
    }
    else if(Operations[operationID].typeOfTrigger == "mouse_release"){
        if(mouse.releasedInRectangle(pos+objPos, size, Operations[operationID].getBindedMouseButton(), isAttachedToCamera)){
            cout << getID() << " ";
            return true;
        }
    }
    else if(Operations[operationID].typeOfTrigger == "mouse_in_area"){
        if(mouse.inRectangle(pos+objPos, size, isAttachedToCamera))
            return true;
    }
    else if(Operations[operationID].typeOfTrigger == "mouse_not_in_area"){
        if(!mouse.inRectangle(pos+objPos, size, isAttachedToCamera))
            return true;
    }
    else if(isStringInGroup(Operations[operationID].typeOfTrigger, 6, "==", "!=", ">", "<", ">=", "<=")){
        if(EventVariable == nullptr || ObjectVariable == nullptr){
            return false;
        }
        bool saa = EventVariable->isConditionMet(Operations[operationID].typeOfTrigger, ObjectVariable);
        return saa;
    }

    return false;
}
void EventModule::controlAncestor(int operationID, vec2d & objectPos, vec2d & objectSize){
    if(Operations[operationID].affectedVariable == "position" && Operations[operationID].choosenDoubles.size() >= 2){
        objectPos.set(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    if(Operations[operationID].affectedVariable == "size" && Operations[operationID].choosenDoubles.size() >= 2){
        objectSize.set(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
}
void EventModule::controlText(int operationID, TextModule & Text){
    if(Operations[operationID].affectedVariable == "activate"){
        Text.activate();
    }
    else if(Operations[operationID].affectedVariable == "deactivate"){
        Text.deactivate();
    }
    else if(Operations[operationID].affectedVariable == "position" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.setPos(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "size" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.setSize(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "set_scale" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.setScale(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "add_scale" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.addScale(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "color" && Operations[operationID].choosenDoubles.size() >= 4){
        Text.setColors(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1],
            Operations[operationID].choosenDoubles[2], Operations[operationID].choosenDoubles[3]
        );
    }
    else if(Operations[operationID].affectedVariable == "random_color"){
        Text.setRandomColors();
    }
    else if(Operations[operationID].affectedVariable == "set_rotation" && Operations[operationID].choosenDoubles.size() >= 1){
        Text.setRotation(Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "rotate" && Operations[operationID].choosenDoubles.size() >= 1){
        Text.addRotation(Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "alpha" && Operations[operationID].choosenDoubles.size() >= 1){
        Text.setAlpha(Operations[operationID].choosenDoubles[0]);
    }
}
void EventModule::controlImage(int operationID, ImageModule & Image){
    if(Operations[operationID].affectedVariable == "position" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setPos(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "size" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setSize(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "scale" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setScale(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "resize" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.resize(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "set_rotation" && Operations[operationID].choosenDoubles.size() >= 1){
        Image.setRotation(Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "rotate" && Operations[operationID].choosenDoubles.size() >= 1){
        Image.addRotation(Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "mirror" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setMirror(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "image_color" && Operations[operationID].choosenDoubles.size() >= 4){
        float arr[] = {float(Operations[operationID].choosenDoubles[0]), float(Operations[operationID].choosenDoubles[1]), float(Operations[operationID].choosenDoubles[2]), float(Operations[operationID].choosenDoubles[3])};
        Image.setImageColor(arr);
    }
    else if(Operations[operationID].affectedVariable == "light" && Operations[operationID].choosenDoubles.size() >= 4){
        Image.setLightColor(vec3d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1], Operations[operationID].choosenDoubles[2]), Operations[operationID].choosenDoubles[3]);
    }
}
void EventModule::controlMovement(int operationID, MovementModule & Movement){
    if(Operations[operationID].affectedVariable == "move_up"){
        Movement.setNextMove(true, false, false, false, false, false, false);
    }
    else if(Operations[operationID].affectedVariable == "move_right"){
        Movement.setNextMove(false, true, false, false, false, false, false);
    }
    else if(Operations[operationID].affectedVariable == "move_down"){
        Movement.setNextMove(false, false, true, false, false, false, false);
    }
    else if(Operations[operationID].affectedVariable == "move_left"){
        Movement.setNextMove(false, false, false, true, false, false, false);
    }
    else if(Operations[operationID].affectedVariable == "move_jump"){
        Movement.setNextMove(false, false, false, false, true, false, false);
    }
    else if(Operations[operationID].affectedVariable == "move_fall"){
        Movement.setNextMove(false, false, false, false, false, true, false);
    }
    else if(Operations[operationID].affectedVariable == "move_run"){
        Movement.setNextMove(false, false, false, false, false, false, true);
    }
    else if(Operations[operationID].affectedVariable == "move" && Operations[operationID].choosenDoubles.size() >= 2){
        Movement.addMomentum(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
}
void EventModule::controlCollision(int operationID, CollisionModule & Collision){
    if(Operations[operationID].affectedVariable == "solid"){
        Collision.switchSolid();
    }
    if(Operations[operationID].affectedVariable == "penetrate"){
        Collision.switchSolidPenetration();
    }
}
void EventModule::controlParticles(int operationID, ParticleEffectModule & Particles){
    if(Operations[operationID].affectedVariable == "environment" && Operations[operationID].choosenDoubles.size() >= 2){
        Particles.setEnvironment(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "environment_speed" && Operations[operationID].choosenDoubles.size() >= 2){
        Particles.setEnvironmentSpeed(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
    else if(Operations[operationID].affectedVariable == "shape" && Operations[operationID].choosenDoubles.size() >= 1){
        Particles.setParticlesShape(Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "image"){
        Particles.switchUseImageAsParticles();
    }
    else if(Operations[operationID].affectedVariable == "spawn"){
        Particles.allowSpawning();
    }
    else if(Operations[operationID].affectedVariable == "random_colors"){
        Particles.switchUseRandomColors();
    }
    else if(Operations[operationID].affectedVariable == "move"){
        Particles.switchAreParticlesMoving();
    }
}
void EventModule::controlVariables(int operationID, VariableModule & Variable){
    if(Operations[operationID].affectedVariable == "toggle_bool"){
        Variable.toggleBool();
    }
    else if(Operations[operationID].affectedVariable == "set_default_bool_random"){
        if(rand() % 2 == 0){
            Variable.toggleDefaultBool();
        }
    }
    else if(Operations[operationID].affectedVariable == "set_default_bool_probability" && Operations[operationID].choosenDoubles.size() >= 3){
        bool setBool;
        if(Operations[operationID].choosenDoubles[0] == 0){
            setBool = false;
        }
        else if(Operations[operationID].choosenDoubles[0] == 1){
            setBool = true;
        }
        else{
            return;
        }

        if(rand() % (int)Operations[operationID].choosenDoubles[2] <= (int)Operations[operationID].choosenDoubles[1]){
            Variable.setDefaultBool(setBool);
        }
    }
    else if(Operations[operationID].affectedVariable == "set_default_bool_probability" && Operations[operationID].choosenDoubles.size() >= 2){
        bool setBool;
        if(Operations[operationID].choosenDoubles[0] == 0){
            setBool = false;
        }
        else if(Operations[operationID].choosenDoubles[0] == 1){
            setBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Operations[operationID].choosenDoubles[1]){
            Variable.setDefaultBool(setBool);
        }
    }
    else if(Operations[operationID].affectedVariable == "set_default_bool" && Operations[operationID].choosenDoubles.size() >= 1){
        if(Operations[operationID].choosenDoubles[0] == 0){
            Variable.setDefaultBool(false);
        }
        if(Operations[operationID].choosenDoubles[0] == 1){
            Variable.setDefaultBool(true);
        }
    }
    else if(Operations[operationID].affectedVariable == "set_default_int" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.setDefaultInt((int)Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "set_default_int_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.setDefaultInt(randomInt((int)Operations[operationID].choosenDoubles[0], (int)Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "set_default_int_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.setDefaultInt((int)Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "set_default_double" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.setDefaultDouble(Operations[operationID].choosenDoubles[0]);;
    }
    else if(Operations[operationID].affectedVariable == "set_default_double_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.setDefaultDouble(randomDouble(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));;
    }
    else if(Operations[operationID].affectedVariable == "set_default_double_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.setDefaultDouble(Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "set_default_string" && Operations[operationID].choosenStrings.size() >= 1){
        Variable.setDefaultString(Operations[operationID].choosenStrings[0]);
    }
    else if(Operations[operationID].affectedVariable == "set_default_string_random" && Operations[operationID].choosenStrings.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenStrings.size();
        Variable.setDefaultString(Operations[operationID].choosenStrings[dice]);
    }
    else if(Operations[operationID].affectedVariable == "set_bool_random"){
        if(rand() % 2 == 0)
        Variable.toggleBool();
    }
    else if(Operations[operationID].affectedVariable == "set_bool_probability" && Operations[operationID].choosenDoubles.size() >= 3){
        bool settingBool;
        if(Operations[operationID].choosenDoubles[0] == 0){
            settingBool = false;
        }
        else if(Operations[operationID].choosenDoubles[0] == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % (int)Operations[operationID].choosenDoubles[2] <= (int)Operations[operationID].choosenDoubles[1]){
            Variable.setBool(settingBool);
        }
    }
    else if(Operations[operationID].affectedVariable == "set_bool_probability" && Operations[operationID].choosenDoubles.size() >= 2){
        bool settingBool;
        if(Operations[operationID].choosenDoubles[0] == 0){
            settingBool = false;
        }
        else if(Operations[operationID].choosenDoubles[0] == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Operations[operationID].choosenDoubles[1]){
            Variable.setBool(settingBool);
        }
    }
    else if(Operations[operationID].affectedVariable == "set_bool" && Operations[operationID].choosenDoubles.size() >= 1){
        if(Operations[operationID].choosenDoubles[0] == 0){
            Variable.setBool(false);
        }
        if(Operations[operationID].choosenDoubles[0] == 1){
            Variable.setBool(true);
        }
    }
    else if(Operations[operationID].affectedVariable == "set_int" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.setInt((int)Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "set_int_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.setInt(randomInt((int)Operations[operationID].choosenDoubles[0], (int)Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "set_int_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.setInt((int)Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "set_double" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.setDouble(Operations[operationID].choosenDoubles[0]);;
    }
    else if(Operations[operationID].affectedVariable == "set_double_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.setDouble(randomDouble(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));;
    }
    else if(Operations[operationID].affectedVariable == "set_double_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.setDouble(Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "set_string" && Operations[operationID].choosenStrings.size() >= 1){
        Variable.setString(Operations[operationID].choosenStrings[0]);
    }
    else if(Operations[operationID].affectedVariable == "set_string_random" && Operations[operationID].choosenStrings.size() >= 1){
        cout << "hello ";
        unsigned int dice = rand() % Operations[operationID].choosenStrings.size();
        cout << Variable.setString(Operations[operationID].choosenStrings[dice]) << " ";
        cout << Variable.getString() << "\n";
    }
    else if(Operations[operationID].affectedVariable == "set_to_default"){
        Variable.resetValue();
    }
    else if(Operations[operationID].affectedVariable == "add_default_int" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.addDefaultInt((int)Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "add_default_int_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.addDefaultInt(randomInt((int)Operations[operationID].choosenDoubles[0], (int)Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "add_default_int_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.addDefaultInt((int)Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "add_default_double" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.addDefaultDouble(Operations[operationID].choosenDoubles[0]);;
    }
    else if(Operations[operationID].affectedVariable == "add_default_double_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.addDefaultDouble(randomDouble(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));;
    }
    else if(Operations[operationID].affectedVariable == "add_default_double_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.addDefaultDouble(Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "add_int" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.addInt((int)Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "add_int_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.addInt(randomInt((int)Operations[operationID].choosenDoubles[0], (int)Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "add_int_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.addInt((int)Operations[operationID].choosenDoubles[dice]);
    }
    else if(Operations[operationID].affectedVariable == "add_double" && Operations[operationID].choosenDoubles.size() >= 1){
        Variable.addDouble(Operations[operationID].choosenDoubles[0]);;
    }
    else if(Operations[operationID].affectedVariable == "add_double_interval" && Operations[operationID].choosenDoubles.size() >= 2){
        Variable.addDouble(randomDouble(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));;
    }
    else if(Operations[operationID].affectedVariable == "add_double_random" && Operations[operationID].choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operations[operationID].choosenDoubles.size();
        Variable.addDouble(Operations[operationID].choosenDoubles[dice]);
    }
}
void EventModule::controlScrollbar(int operationID, ScrollbarModule & Scrollbar){
    if(Operations[operationID].affectedVariable == "add_real_area" && Operations[operationID].choosenDoubles.size() >= 2){
        Scrollbar.addRealScrollingArea(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]);
    }
}

void ValueLocation::print(string dynamicID){
    if(dynamicID != ""){
        cout << dynamicID;
    }
    if(source != "" && source != "_"){
        if(dynamicID != ""){
            cout << ".";
        }
        cout << source;
    }
    if(cameraID != "" && cameraID != "_"){
        cout << "." << cameraID;
    }
    if(layerID != "" && layerID != "_"){
        cout << "." << layerID;
    }
    if(objectID != "" && objectID != "_"){
        cout << "." << objectID;
    }
    if(moduleType != "" && moduleType != "_"){
        cout << "." << moduleType;
    }
    if(moduleID != "" && moduleID != "_"){
        cout << "." << moduleID;
    }
    if(attribute != "" && attribute != "_"){
        cout << "." << attribute;
    }
    if(spareID != "" && spareID != "_"){
        cout << "." << spareID;
    }
    cout << " ";
}
