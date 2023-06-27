#include "eventModule.h"

TriggerClass::TriggerClass(unsigned int newID) : LeftLiteral(0), RightLiteral(1){
}

EveModule::EveModule(unsigned int eventModuleID){
    primaryConstructor(eventModuleID);
    setUpNewInstance();
}
EveModule::EveModule(string eventModuleID){
    primaryConstructor(eventModuleID);
    setUpNewInstance();
}
void EveModule::setUpNewInstance(){
}
void EveModule::clearModule(){
    ConditionalChain.clear();
    PostOperations.clear();
    Children.clear();
}
void EveModule::resetStatus(){
	parentStatus = false;
	for(ChildStruct & Child : Children){
		Child.finished = false;
	}
	werePostOperationsExecuted = false;
}
bool EveModule::areAllConditionsFulfilled(){
    return false;
}
void EveModule::executeDependentOperations(){

}
void EveModule::executePostOperations(){
	//...
	werePostOperationsExecuted = true;
}
bool EveModule::allChildrenFinished(){
    for(ChildStruct Child : Children){
        if(!Child.finished){
            return false;
        }
    }
    return true;
}
void EveModule::controlAncestor(OperaClass & Operation, vec2d & objectPos, vec2d & objectSize){
    if(Operation.affectedVariable == "position" && Operation.choosenDoubles.size() >= 2){
        objectPos.set(Operation.choosenDoubles[0], Operation.choosenDoubles[1]);
    }
    if(Operation.affectedVariable == "size" && Operation.choosenDoubles.size() >= 2){
        objectSize.set(Operation.choosenDoubles[0], Operation.choosenDoubles[1]);
    }
}
void EveModule::controlText(OperaClass & Operation, TextModule & Text){
    if(Operation.affectedVariable == "activate"){
        Text.activate();
    }
    else if(Operation.affectedVariable == "deactivate"){
        Text.deactivate();
    }
    else if(Operation.affectedVariable == "position" && Operation.choosenDoubles.size() >= 2){
        Text.setPos(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "size" && Operation.choosenDoubles.size() >= 2){
        Text.setSize(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "set_scale" && Operation.choosenDoubles.size() >= 2){
        Text.setScale(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "add_scale" && Operation.choosenDoubles.size() >= 2){
        Text.addScale(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "color" && Operation.choosenDoubles.size() >= 3){
        Text.setColors(Operation.choosenDoubles[0], Operation.choosenDoubles[1], Operation.choosenDoubles[2]);
    }
    else if(Operation.affectedVariable == "random_color"){
        Text.setRandomColors();
    }
    else if(Operation.affectedVariable == "set_rotation" && Operation.choosenDoubles.size() >= 1){
        Text.setRotation(Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "rotate" && Operation.choosenDoubles.size() >= 1){
        Text.addRotation(Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "visibility" && Operation.choosenDoubles.size() >= 1){
        Text.setVisibility(Operation.choosenDoubles[0]);
    }
}
void EveModule::controlImage(OperaClass & Operation, ImageModule & Image){
    if(Operation.affectedVariable == "position" && Operation.choosenDoubles.size() >= 2){
        Image.setPos(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "size" && Operation.choosenDoubles.size() >= 2){
        Image.setSize(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "scale" && Operation.choosenDoubles.size() >= 2){
        Image.setScale(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "resize" && Operation.choosenDoubles.size() >= 2){
        Image.resize(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "set_rotation" && Operation.choosenDoubles.size() >= 1){
        Image.setRotation(Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "rotate" && Operation.choosenDoubles.size() >= 1){
        Image.addRotation(Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "mirror" && Operation.choosenDoubles.size() >= 2){
        Image.setMirror(Operation.choosenDoubles[0], Operation.choosenDoubles[1]);
    }
    else if(Operation.affectedVariable == "image_color" && Operation.choosenDoubles.size() >= 4){
        float arr[] = {float(Operation.choosenDoubles[0]), float(Operation.choosenDoubles[1]), float(Operation.choosenDoubles[2]), float(Operation.choosenDoubles[3])};
        Image.setImageColor(arr);
    }
    else if(Operation.affectedVariable == "light" && Operation.choosenDoubles.size() >= 4){
        Image.setLightColor(vec3d(Operation.choosenDoubles[0], Operation.choosenDoubles[1], Operation.choosenDoubles[2]), Operation.choosenDoubles[3]);
    }
}
void EveModule::controlMovement(OperaClass & Operation, MovementModule & Movement){
    if(Operation.affectedVariable == "move_up"){
        Movement.setNextMove(true, false, false, false, false, false, false);
    }
    else if(Operation.affectedVariable == "move_right"){
        Movement.setNextMove(false, true, false, false, false, false, false);
    }
    else if(Operation.affectedVariable == "move_down"){
        Movement.setNextMove(false, false, true, false, false, false, false);
    }
    else if(Operation.affectedVariable == "move_left"){
        Movement.setNextMove(false, false, false, true, false, false, false);
    }
    else if(Operation.affectedVariable == "move_jump"){
        Movement.setNextMove(false, false, false, false, true, false, false);
    }
    else if(Operation.affectedVariable == "move_fall"){
        Movement.setNextMove(false, false, false, false, false, true, false);
    }
    else if(Operation.affectedVariable == "move_run"){
        Movement.setNextMove(false, false, false, false, false, false, true);
    }
    else if(Operation.affectedVariable == "move" && Operation.choosenDoubles.size() >= 2){
        Movement.addMomentum(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
}
void EveModule::controlCollision(OperaClass & Operation, CollisionModule & Collision){
    if(Operation.affectedVariable == "solid"){
        Collision.switchSolid();
    }
    if(Operation.affectedVariable == "penetrate"){
        Collision.switchSolidPenetration();
    }
}
void EveModule::controlParticles(OperaClass & Operation, ParticleEffectModule & Particles){
    if(Operation.affectedVariable == "environment" && Operation.choosenDoubles.size() >= 2){
        Particles.setEnvironment(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "environment_speed" && Operation.choosenDoubles.size() >= 2){
        Particles.setEnvironmentSpeed(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "shape" && Operation.choosenDoubles.size() >= 1){
        Particles.setParticlesShape(Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "image"){
        Particles.switchUseImageAsParticles();
    }
    else if(Operation.affectedVariable == "spawn"){
        Particles.activeSpawn();
    }
    else if(Operation.affectedVariable == "random_colors"){
        Particles.switchUseRandomColors();
    }
    else if(Operation.affectedVariable == "move"){
        Particles.switchAreParticlesMoving();
    }
}
void EveModule::controlVariables(OperaClass & Operation, VariableModule & Variable){
    if(Operation.affectedVariable == "toggle_bool"){
        Variable.toggleBool();
    }
    else if(Operation.affectedVariable == "set_default_bool_random"){
        if(rand() % 2 == 0){
            Variable.toggleDefaultBool();
        }
    }
    else if(Operation.affectedVariable == "set_default_bool_probability" && Operation.choosenDoubles.size() >= 3){
        bool setBool;
        if(Operation.choosenDoubles[0] == 0){
            setBool = false;
        }
        else if(Operation.choosenDoubles[0] == 1){
            setBool = true;
        }
        else{
            return;
        }

        if(rand() % (int)Operation.choosenDoubles[2] <= (int)Operation.choosenDoubles[1]){
            Variable.setDefaultBool(setBool);
        }
    }
    else if(Operation.affectedVariable == "set_default_bool_probability" && Operation.choosenDoubles.size() >= 2){
        bool setBool;
        if(Operation.choosenDoubles[0] == 0){
            setBool = false;
        }
        else if(Operation.choosenDoubles[0] == 1){
            setBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Operation.choosenDoubles[1]){
            Variable.setDefaultBool(setBool);
        }
    }
    else if(Operation.affectedVariable == "set_default_bool" && Operation.choosenDoubles.size() >= 1){
        if(Operation.choosenDoubles[0] == 0){
            Variable.setDefaultBool(false);
        }
        if(Operation.choosenDoubles[0] == 1){
            Variable.setDefaultBool(true);
        }
    }
    else if(Operation.affectedVariable == "set_default_int" && Operation.choosenDoubles.size() >= 1){
        Variable.setDefaultInt((int)Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "set_default_int_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.setDefaultInt(randomInt((int)Operation.choosenDoubles[0], (int)Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "set_default_int_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.setDefaultInt((int)Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "set_default_double" && Operation.choosenDoubles.size() >= 1){
        Variable.setDefaultDouble(Operation.choosenDoubles[0]);;
    }
    else if(Operation.affectedVariable == "set_default_double_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.setDefaultDouble(randomDouble(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));;
    }
    else if(Operation.affectedVariable == "set_default_double_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.setDefaultDouble(Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "set_default_string" && Operation.choosenStrings.size() >= 1){
        Variable.setDefaultString(Operation.choosenStrings[0]);
    }
    else if(Operation.affectedVariable == "set_default_string_random" && Operation.choosenStrings.size() >= 1){
        unsigned int dice = rand() % Operation.choosenStrings.size();
        Variable.setDefaultString(Operation.choosenStrings[dice]);
    }
    else if(Operation.affectedVariable == "set_bool_random"){
        if(rand() % 2 == 0)
        Variable.toggleBool();
    }
    else if(Operation.affectedVariable == "set_bool_probability" && Operation.choosenDoubles.size() >= 3){
        bool settingBool;
        if(Operation.choosenDoubles[0] == 0){
            settingBool = false;
        }
        else if(Operation.choosenDoubles[0] == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % (int)Operation.choosenDoubles[2] <= (int)Operation.choosenDoubles[1]){
            Variable.setBool(settingBool);
        }
    }
    else if(Operation.affectedVariable == "set_bool_probability" && Operation.choosenDoubles.size() >= 2){
        bool settingBool;
        if(Operation.choosenDoubles[0] == 0){
            settingBool = false;
        }
        else if(Operation.choosenDoubles[0] == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Operation.choosenDoubles[1]){
            Variable.setBool(settingBool);
        }
    }
    else if(Operation.affectedVariable == "set_bool" && Operation.choosenDoubles.size() >= 1){
        if(Operation.choosenDoubles[0] == 0){
            Variable.setBool(false);
        }
        if(Operation.choosenDoubles[0] == 1){
            Variable.setBool(true);
        }
    }
    else if(Operation.affectedVariable == "set_int" && Operation.choosenDoubles.size() >= 1){
        Variable.setInt((int)Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "set_int_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.setInt(randomInt((int)Operation.choosenDoubles[0], (int)Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "set_int_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.setInt((int)Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "set_double" && Operation.choosenDoubles.size() >= 1){
        Variable.setDouble(Operation.choosenDoubles[0]);;
    }
    else if(Operation.affectedVariable == "set_double_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.setDouble(randomDouble(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));;
    }
    else if(Operation.affectedVariable == "set_double_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.setDouble(Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "set_string" && Operation.choosenStrings.size() >= 1){
        Variable.setString(Operation.choosenStrings[0]);
    }
    else if(Operation.affectedVariable == "set_string_random" && Operation.choosenStrings.size() >= 1){
        std::cout << "hello ";
        unsigned int dice = rand() % Operation.choosenStrings.size();
        std::cout << Variable.setString(Operation.choosenStrings[dice]) << " ";
        std::cout << Variable.getString() << "\n";
    }
    else if(Operation.affectedVariable == "set_to_default"){
        Variable.resetValue();
    }
    else if(Operation.affectedVariable == "add_default_int" && Operation.choosenDoubles.size() >= 1){
        Variable.addDefaultInt((int)Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "add_default_int_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.addDefaultInt(randomInt((int)Operation.choosenDoubles[0], (int)Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "add_default_int_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.addDefaultInt((int)Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "add_default_double" && Operation.choosenDoubles.size() >= 1){
        Variable.addDefaultDouble(Operation.choosenDoubles[0]);;
    }
    else if(Operation.affectedVariable == "add_default_double_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.addDefaultDouble(randomDouble(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));;
    }
    else if(Operation.affectedVariable == "add_default_double_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.addDefaultDouble(Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "add_int" && Operation.choosenDoubles.size() >= 1){
        Variable.addInt((int)Operation.choosenDoubles[0]);
    }
    else if(Operation.affectedVariable == "add_int_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.addInt(randomInt((int)Operation.choosenDoubles[0], (int)Operation.choosenDoubles[1]));
    }
    else if(Operation.affectedVariable == "add_int_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.addInt((int)Operation.choosenDoubles[dice]);
    }
    else if(Operation.affectedVariable == "add_double" && Operation.choosenDoubles.size() >= 1){
        Variable.addDouble(Operation.choosenDoubles[0]);;
    }
    else if(Operation.affectedVariable == "add_double_interval" && Operation.choosenDoubles.size() >= 2){
        Variable.addDouble(randomDouble(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));;
    }
    else if(Operation.affectedVariable == "add_double_random" && Operation.choosenDoubles.size() >= 1){
        unsigned int dice = rand() % Operation.choosenDoubles.size();
        Variable.addDouble(Operation.choosenDoubles[dice]);
    }
}
void EveModule::controlScrollbar(OperaClass & Operation, ScrollbarModule & Scrollbar){
    if(Operation.affectedVariable == "add_real_area" && Operation.choosenDoubles.size() >= 2){
        Scrollbar.addRealScrollingArea(vec2d(Operation.choosenDoubles[0], Operation.choosenDoubles[1]));
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

EventModule::EventModule(int moduleID){
    primaryConstructor(moduleID);
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
void EventModule::clearModule(){
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
            std::cout << getID() << " ";
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
        bool saa = EventVariable->isConditionMet(ObjectVariable, Operations[operationID].typeOfTrigger);
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
        Text.setPos(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "size" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.setSize(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "set_scale" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.setScale(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "add_scale" && Operations[operationID].choosenDoubles.size() >= 2){
        Text.addScale(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "color" && Operations[operationID].choosenDoubles.size() >= 3){
        Text.setColors(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1], Operations[operationID].choosenDoubles[2]);
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
    else if(Operations[operationID].affectedVariable == "visibility" && Operations[operationID].choosenDoubles.size() >= 1){
        Text.setVisibility(Operations[operationID].choosenDoubles[0]);
    }
}
void EventModule::controlImage(int operationID, ImageModule & Image){
    if(Operations[operationID].affectedVariable == "position" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setPos(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "size" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setSize(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "scale" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.setScale(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "resize" && Operations[operationID].choosenDoubles.size() >= 2){
        Image.resize(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
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
        Movement.addMomentum(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
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
        Particles.setEnvironment(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "environment_speed" && Operations[operationID].choosenDoubles.size() >= 2){
        Particles.setEnvironmentSpeed(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
    else if(Operations[operationID].affectedVariable == "shape" && Operations[operationID].choosenDoubles.size() >= 1){
        Particles.setParticlesShape(Operations[operationID].choosenDoubles[0]);
    }
    else if(Operations[operationID].affectedVariable == "image"){
        Particles.switchUseImageAsParticles();
    }
    else if(Operations[operationID].affectedVariable == "spawn"){
        Particles.activeSpawn();
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
        std::cout << "hello ";
        unsigned int dice = rand() % Operations[operationID].choosenStrings.size();
        std::cout << Variable.setString(Operations[operationID].choosenStrings[dice]) << " ";
        std::cout << Variable.getString() << "\n";
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
        Scrollbar.addRealScrollingArea(vec2d(Operations[operationID].choosenDoubles[0], Operations[operationID].choosenDoubles[1]));
    }
}
