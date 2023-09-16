#include "movementModule.h"


RandomAction::RandomAction(){
    areRandomActionsEnabled = true;
    timeWhenActionPersists = 0.0;
    minTimeOfAction = 0.0;
    maxTimeOfAction = 0.0;
    minMoveDistance = 0.0;
    maxMoveDistance = 0.0;
    savedAction = -1;
    savedDirection.set(0.0, 0.0);
    areBreaksAllowed = true;
    chanceForBreak = 0.5;
}
void RandomAction::changeRandomActionsSettings(bool newAreRandomActionsEnabled, double newMinTimeOfAction, double newMaxTimeOfAction, double newMinMoveDistance, double newMaxMoveDistance, bool newAreBreaksAllowed, double newChanceForBreak){
    areRandomActionsEnabled = newAreRandomActionsEnabled;
    timeWhenActionPersists = 0.0;
    minTimeOfAction = newMinTimeOfAction;
    if(minTimeOfAction < 0.0)
        minTimeOfAction = 0.1;
    maxTimeOfAction = newMaxTimeOfAction;
    minMoveDistance = newMinMoveDistance;
    maxMoveDistance = newMaxMoveDistance;
    savedAction = -1;
    savedDirection.set(0.0, 0.0);
    areBreaksAllowed = newAreBreaksAllowed;
    chanceForBreak = newChanceForBreak;
}
short RandomAction::chooseRandomKey(short actionType){
    if(timeWhenActionPersists > 0.0){
        timeWhenActionPersists -= 1/FPS;
        if(timeWhenActionPersists < 0.0)
            timeWhenActionPersists = 0.0;
        return savedAction;
    }

    if(actionType == 0)
        savedAction = rand()%6;
    if(actionType == 1)
        savedAction = rand()%9;
    timeWhenActionPersists = randomDouble(minTimeOfAction, maxTimeOfAction);
    return savedAction;
}
vec2d RandomAction::chooseRandomDirection(vec2d objectPos, vec2d & objectDestination){
    //Continue previous action if possible
    if(timeWhenActionPersists > 0.0){
        timeWhenActionPersists -= 1/FPS;
        if(timeWhenActionPersists < 0.0)
            timeWhenActionPersists = 0.0;
    }
    if(timeWhenActionPersists > 0.0){
        if(savedAction == 0){ //when object doesn't move
            return vec2d(0.0, 0.0);
        }
        else if(savedAction == 1){ //when object is moving about
            if(!objectPos.isEqual(objectDestination)){
                savedDirection.set(objectPos.x, objectPos.y, objectDestination.x, objectDestination.y);
                savedDirection.normalize();
                return savedDirection;
            }
        }
    }

    //Generate new random action
    timeWhenActionPersists = randomDouble(minTimeOfAction, maxTimeOfAction);
    if(areBreaksAllowed && randomDouble(0.0, 1.0) <= chanceForBreak){
        savedAction = 0;
        return vec2d(0.0, 0.0);
    }
    else{
        savedAction = 1;
        savedDirection.set(1.0, 0.0);
        savedDirection.rotate(randomDouble(0.0, 359.9));
        savedDirection.normalize();
        savedDirection.setLength(randomDouble(minMoveDistance, maxMoveDistance));
        objectDestination.set(objectPos);
        objectDestination.translate(savedDirection.x * savedDirection.length, savedDirection.y * savedDirection.length);
        return savedDirection;
    }
}

void ChainMovement::clearChain(){
    posChain.clear();
}

MovementModule::MovementModule(){
}
MovementModule::MovementModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, *listOfIDs, newLayerID, newObjectID);
    movementType = 0;
    allowedJumps = 0;
    jumpsCount = 0;
    jumpCooldown = 0.0;
    jumpCooldownDuration = 0.0;
    isMovePlanned = false;
    canJump = false;
    bodyMass = 0.0;
    walkingSpeed = 0.0;
    runningSpeed = 0.0;
    jumpSpeed = 0.0;
    gravitation = 0.0;
    baseFriction = 0.0;
    momentum.set(0.0, 0.0);
    maxMomentumX = 0.0;
    minMomentumY = 0.0;
    maxMomentumY = 0.0;
    moveCooldown = 0.0;
    moveCooldownDuration = 0.0;
    canMoveDiagonally = true;
    resetMomentumWhenJumping = true;

    upKey = ALLEGRO_KEY_UP;
    rightKey = ALLEGRO_KEY_RIGHT;
    downKey = ALLEGRO_KEY_DOWN;
    leftKey = ALLEGRO_KEY_LEFT;
    jumpKey = ALLEGRO_KEY_UP;
    runningKey = ALLEGRO_KEY_LSHIFT;

    clearStates();

    directionOfMove.set(0.0, 0.0);
    mouseButton = 0;
    moveOnMouseRelease = true;
}
MovementModule::MovementModule(unsigned int newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    MovementModule(intToStr(newID), listOfIDs, newLayerID, newObjectID);
}
void MovementModule::clone(const MovementModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID);
}

void MovementModule::translateNumericalActionToState(short action){
    if(action == 0)
        return;
    if(movementType == 0){
        if(action == 1)
            setNextMove(true, false, false, false, false, false, false);
        if(action == 2)
            setNextMove(true, true, false, false, false, false, false);
        if(action == 3)
            setNextMove(false, true, false, false, false, false, false);
        if(action == 4)
            setNextMove(false, false, false, true, true, false, false);
        if(action == 5)
            setNextMove(true, false, false, true, false, false, false);
    }
    if(movementType == 1){
        if(action == 1)
            setNextMove(true, false, false, false, false, false, false);
        if(action == 2)
            setNextMove(true, true, false, false, false, false, false);
        if(action == 3)
            setNextMove(false, true, false, false, false, false, false);
        if(action == 4)
            setNextMove(false, true, true, false, false, false, false);
        if(action == 5)
            setNextMove(false, false, true, false, false, false, false);
        if(action == 6)
            setNextMove(false, false, true, true, false, false, false);
        if(action == 7)
            setNextMove(false, false, false, true, false, false, false);
        if(action == 8)
            setNextMove(true, false, false, true, false, false, false);
    }
}
void MovementModule::movingWithJump(){
    double finalGravitation = gravitation * bodyMass;
    if(bodyMass <= 0)
        finalGravitation = 0.0;
    if(bodyMass >= 1)
        finalGravitation = gravitation;

    momentum.y += finalGravitation;
    if(momentum.y > maxMomentumY)
        momentum.y = maxMomentumY;

    if(jumpCooldown > 0.0)
        jumpCooldown -= 1/FPS;
    if(jumpCooldown <= 0.0)
        jumpCooldown = 0.0;
    if(canJump)
        jumpsCount = 0;
    if(isJumping && momentum.y >= 0.0){
        isJumping = false;
        if(momentum.y > 0.0)
            isFalling = true;
    }
    if(isFalling && momentum.y <= 0.0){
        isFalling = false;
    }
    if((canJump || jumpsCount < allowedJumps) && jumpCooldown == 0.0){
        if(isMovingUp){
            if(momentum.y > 0.0 && resetMomentumWhenJumping)
                momentum.y = 0.0;
            momentum.y -= jumpSpeed;
            jumpsCount++;
            jumpCooldown = jumpCooldownDuration;
            canJump = false;
            isJumping = true;
        }
    }
    if(!isJumping && momentum.y > finalGravitation)
        isFalling = true;
    else
        isFalling = false;

    double finalFriction = baseFriction * (1-bodyMass);
    if(bodyMass <= 0)
        finalFriction = baseFriction;
    if(bodyMass >= 1)
        finalFriction = 0.001;

    if(momentum.x > 0){
        momentum.x -= finalFriction;
        if(momentum.x < 0)
            momentum.x = 0.0;
    }
    if(momentum.x < 0){
        momentum.x += finalFriction;
        if(momentum.x > 0)
            momentum.x = 0.0;
    }

    if(isMovingLeft){
        if(isRunning)
            momentum.x -= runningSpeed;
        else
            momentum.x -= walkingSpeed;
        isMovingLeft = true;
    }
    else
        isMovingLeft = false;

    if(isMovingRight){
        if(isRunning)
            momentum.x += runningSpeed;
        else
            momentum.x += walkingSpeed;
        isMovingRight = true;
    }
    else
        isMovingRight = false;

    if(momentum.x > maxMomentumX)
        momentum.x = maxMomentumX;
    if(momentum.x < -maxMomentumX)
        momentum.x = -maxMomentumX;
}
void MovementModule::resetJump(){
    canJump = true;
}
void MovementModule::movingIn8Directions(){
    double finalFriction = baseFriction * (1-bodyMass);
    if(bodyMass <= 0)
        finalFriction = baseFriction;
    if(bodyMass >= 1)
        finalFriction = 0.001;

    if(momentum.x > 0){
        momentum.x -= finalFriction;
        if(momentum.x < 0)
            momentum.x = 0.0;
    }
    if(momentum.x < 0){
        momentum.x += finalFriction;
        if(momentum.x > 0)
            momentum.x = 0.0;
    }
    if(momentum.y > 0){
        momentum.y -= finalFriction;
        if(momentum.y < 0)
            momentum.y = 0.0;
    }
    if(momentum.y < 0){
        momentum.y += finalFriction;
        if(momentum.y > 0)
            momentum.y = 0.0;
    }

    if(moveCooldown == 0.0){
        if(isMovingUp && ((!isMovingRight && !isMovingLeft) || canMoveDiagonally)){
            if(isRunning)
                momentum.y -= runningSpeed;
            else
                momentum.y -= walkingSpeed;
            moveCooldown = moveCooldownDuration;
        }
        else
            isMovingUp = false;
        if(isMovingDown && ((!isMovingRight && !isMovingLeft) || canMoveDiagonally)){
            if(isRunning)
                momentum.y += runningSpeed;
            else
                momentum.y += walkingSpeed;
            moveCooldown = moveCooldownDuration;
        }
        else
            isMovingDown = false;
        if(isMovingLeft && ((!isMovingUp && !isMovingDown) || canMoveDiagonally)){
            if(isRunning)
                momentum.x -= runningSpeed;
            else
                momentum.x -= walkingSpeed;
            moveCooldown = moveCooldownDuration;
        }
        else
            isMovingLeft = false;
        if(isMovingRight && ((!isMovingUp && !isMovingDown) || canMoveDiagonally)){
            if(isRunning)
                momentum.x += runningSpeed;
            else
                momentum.x += walkingSpeed;
            moveCooldown = moveCooldownDuration;
        }
        else
            isMovingRight = false;
    }
    else{
        if(moveCooldown > 0.0)
            moveCooldown -= 1/FPS;
        if(moveCooldown < 0.0)
            moveCooldown = 0.0;
    }

    if(momentum.x > maxMomentumX)
        momentum.x = maxMomentumX;
    if(momentum.x < -maxMomentumX)
        momentum.x = -maxMomentumX;
    if(momentum.y > maxMomentumX)
        momentum.y = maxMomentumX;
    if(momentum.y < -maxMomentumX)
        momentum.y = -maxMomentumX;
}
/*void MovementModule::translateVectorsToJumpMovement(){
    Function translates vectors (mouse and random positions) and chains of positions (pre-programmed path)
    to input for jump based movement.

    Checking if the next position in chain is reached.


    Counting
}*/
void MovementModule::movingWithVectors(vec2d objectPos){
    //Function uses vectors to move the object. There are two types of input: mouse, random vectors and chains of positions (pre-programmed path).
    /*double finalFriction = baseFriction * (1-bodyMass);
    if(bodyMass <= 0)
        finalFriction = baseFriction;
    if(bodyMass >= 1)
        finalFriction = 0.001;

    if(momentum.x > 0){
        momentum.x -= finalFriction;
        if(momentum.x < 0)
            momentum.x = 0.0;
    }
    if(momentum.x < 0){
        momentum.x += finalFriction;
        if(momentum.x > 0)
            momentum.x = 0.0;
    }
    if(momentum.y > 0){
        momentum.y -= finalFriction;
        if(momentum.y < 0)
            momentum.y = 0.0;
    }
    if(momentum.y < 0){
        momentum.y += finalFriction;
        if(momentum.y > 0)
            momentum.y = 0.0;
    }*/

    if(moveCooldown == 0.0){
        if(isRunning){
            momentum.x = runningSpeed * directionOfMove.x;
            momentum.y = runningSpeed * directionOfMove.y;
        }
        else{
            momentum.x = walkingSpeed * directionOfMove.x;
            momentum.y = walkingSpeed * directionOfMove.y;
        }
        vec2d distance1(objectPos.x, objectPos.y, objectPos.x+momentum.x, objectPos.y+momentum.y);
        vec2d distance2(objectPos.x, objectPos.y, destination.x, destination.y);

        if(distance1.length > distance2.length){ //When object will miss destination
            momentum.set(distance2);
        }

        //Figure out states of movement
        if(momentum.getAngle() >= 280.0 || momentum.getAngle() < 80.0)
            isMovingUp = true;
        if(momentum.getAngle() >= 10.0 || momentum.getAngle() < 170.0)
            isMovingRight = true;
        if(momentum.getAngle() >= 100.0 || momentum.getAngle() < 260.0)
            isMovingDown = true;
        if(momentum.getAngle() >= 190.0 || momentum.getAngle() < 350.0)
            isMovingLeft = true;
    }
    else{
        if(moveCooldown > 0.0)
            moveCooldown -= 1/FPS;
        if(moveCooldown < 0.0)
            moveCooldown = 0.0;
    }

    if(momentum.x > maxMomentumX)
        momentum.x = maxMomentumX;
    if(momentum.x < -maxMomentumX)
        momentum.x = -maxMomentumX;
    if(momentum.y > maxMomentumX)
        momentum.y = maxMomentumX;
    if(momentum.y < -maxMomentumX)
        momentum.y = -maxMomentumX;
}
void MovementModule::movingWithPathFinding(){

}
void MovementModule::updateMomentum(vec2d objectPos){
    if(movementType == 0){
        movingWithJump();
    }
    if(movementType == 1){
        movingIn8Directions();
    }
    if(movementType == 2){
        movingWithVectors(objectPos);
    }
}
void MovementModule::stopMovementAfterCollision(short collisionType){
    if(resetDirectionAfterCollision){
        directionOfMove.set(0.0, 0.0);
    }
    if(collisionType == 2){
        momentum.y = 0.0;
    }
    if(collisionType == 1 || collisionType == 3){
        momentum.x = 0.0;
    }
}
void MovementModule::changeJumpParameters(double newJumpSpeed, short newAllowedJumps, double newJumpCooldownDuration, double newGravitation, double newMinMomentumY, double newMaxMomentumY, bool newResetMomentumWhenJumping){
    jumpSpeed = newJumpSpeed;
    allowedJumps = newAllowedJumps;
    jumpCooldownDuration = newJumpCooldownDuration;
    gravitation = newGravitation;
    minMomentumY = newMinMomentumY;
    maxMomentumY = newMaxMomentumY;
    resetMomentumWhenJumping = newResetMomentumWhenJumping;
}
void MovementModule::changeMoveParameters(short newMovementType, short newInputType, double newBodyMass, double newWalkingSpeed, double newRunningSpeed,
                                          double newBaseFriction, double newMaxMomentumX, double newMoveCooldown, bool newCanMoveDiagonally){
    movementType = newMovementType;
    inputType = newInputType;
    bodyMass = newBodyMass;
    walkingSpeed = newWalkingSpeed;
    runningSpeed = newRunningSpeed;
    baseFriction = newBaseFriction;
    maxMomentumX = newMaxMomentumX;
    moveCooldownDuration = newMoveCooldown;
    canMoveDiagonally = newCanMoveDiagonally;
}
void MovementModule::changeVectorMoveParameters(short newMouseButton, bool newMoveOnMouseRelease, bool newResetDirectionAfterCollision){
    mouseButton = newMouseButton;
    moveOnMouseRelease = newMoveOnMouseRelease;
    resetDirectionAfterCollision = newResetDirectionAfterCollision;
}
void MovementModule::setNextMove(bool up, bool right, bool down, bool left, bool jump, bool fall, bool run){
    isMovingUp = up;
    isMovingRight = right;
    isMovingDown = down;
    isMovingLeft = left;
    isJumping = jump;
    isFalling = fall;
    isRunning = run;
    isMovePlanned = true;
}
void MovementModule::addMomentum(vec2d newMomentum){
    momentum.x += newMomentum.x;
    momentum.y += newMomentum.y;
}
void MovementModule::clearStates(){
    isMovingUp = false;
    isMovingRight = false;
    isMovingDown = false;
    isMovingLeft = false;
    isJumping = false;
    isFalling = false;
    isRunning = false;
}
void MovementModule::translateKeyboardToState(vector <short> pressedKeys){
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(pressedKeys[i] == upKey)
            isMovingUp = true;
        if(pressedKeys[i] == rightKey)
            isMovingRight = true;
        if(pressedKeys[i] == downKey)
            isMovingDown = true;
        if(pressedKeys[i] == leftKey)
            isMovingLeft = true;
        if(pressedKeys[i] == jumpKey)
            isJumping = true;
        if(pressedKeys[i] == runningKey)
            isRunning = true;
    }
}
void MovementModule::translateMouseToVector(bool pressedMouse[5], bool releasedMouse[5], vec2d mousePos, vec2d objectPos){
    if((moveOnMouseRelease && releasedMouse[mouseButton]) || (!moveOnMouseRelease && pressedMouse[mouseButton])){
        destination.set(mousePos.x, mousePos.y);
        directionOfMove.set(objectPos.x, objectPos.y, mousePos.x, mousePos.y);
        directionOfMove.normalize();
    }
    else if(objectPos.isEqual(destination)){
        directionOfMove.set(0.0, 0.0);
        destination.set(0.0, 0.0);
    }
    else if(!directionOfMove.isEqual(vec2d(0.0, 0.0))){
        directionOfMove.set(objectPos.x, objectPos.y, destination.x, destination.y);
        directionOfMove.normalize();
    }
}
void MovementModule::updateStatesAndVectors(vector <short> pressedKeys, bool pressedMouse[5], bool releasedMouse[5], vec2d mousePos, vec2d objectPos){
    if(isMovePlanned || inputType == 0){
        isMovePlanned = false;
        return;
    }
    clearStates();

    if(inputType == 1){
        if(areRandomActionsEnabled){
            if(movementType == 0 || movementType == 1)
                translateNumericalActionToState(chooseRandomKey(movementType));
            else if(movementType == 2){
                directionOfMove = chooseRandomDirection(objectPos, destination);
            }
        }
    }
    if(inputType == 2){
        translateKeyboardToState(pressedKeys);
    }
    if(inputType == 3){
        if(areRandomActionsEnabled){
            translateNumericalActionToState(chooseRandomKey(movementType));
        }
        translateKeyboardToState(pressedKeys);
    }
    if(inputType == 4){
        translateMouseToVector(pressedMouse, releasedMouse, mousePos, objectPos);
    }
    if(inputType == 5){
        directionOfMove = chooseRandomDirection(objectPos, destination);
        vec2d temp(directionOfMove);
        translateMouseToVector(pressedMouse, releasedMouse, mousePos, objectPos);
        directionOfMove.translate(temp);
        directionOfMove.normalize();
        if(!temp.isEqual(0.0, 0.0))
            destination.setLength(destination.length/2.0);
    }

}
void MovementModule::bindKeys(short newUpKey, short newRightKey, short newDownKey, short newLeftKey, short newJumpKey, short newRunningKey){
    upKey = newUpKey;
    rightKey = newRightKey;
    downKey = newDownKey;
    leftKey = newLeftKey;
    jumpKey = newJumpKey;
    runningKey = newRunningKey;
}
void MovementModule::bindMouseButton(short newMouseButton){
    mouseButton = newMouseButton;
}
void MovementModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "movement_type"){
        BasePointers.back().setPointer(&movementType);
    }
    else if(attribute == "input_type"){
        BasePointers.back().setPointer(&inputType);
    }
    else if(attribute == "allowed_jumps"){
        BasePointers.back().setPointer(&allowedJumps);
    }
    else if(attribute == "jumps_count"){
        BasePointers.back().setPointer(&jumpsCount);
    }
    else if(attribute == "jump_cooldown"){
        BasePointers.back().setPointer(&jumpCooldown);
    }
    else if(attribute == "jump_cooldown_duration"){
        BasePointers.back().setPointer(&jumpCooldownDuration);
    }
    else if(attribute == "move_cooldown"){
        BasePointers.back().setPointer(&moveCooldown);
    }
    else if(attribute == "move_cooldown_duration"){
        BasePointers.back().setPointer(&moveCooldownDuration);
    }
    else if(attribute == "can_move_diagonally"){
        BasePointers.back().setPointer(&canMoveDiagonally);
    }
    else if(attribute == "reset_momentum_when_jumping"){
        BasePointers.back().setPointer(&resetMomentumWhenJumping);
    }
    else if(attribute == "is_move_planned"){
        BasePointers.back().setPointer(&isMovePlanned);
    }
    else if(attribute == "can_jump"){
        BasePointers.back().setPointer(&canJump);
    }
    else if(attribute == "body_mass"){
        BasePointers.back().setPointer(&bodyMass);
    }
    else if(attribute == "walking_speed"){
        BasePointers.back().setPointer(&walkingSpeed);
    }
    else if(attribute == "running_speed"){
        BasePointers.back().setPointer(&runningSpeed);
    }
    else if(attribute == "jump_speed"){
        BasePointers.back().setPointer(&jumpSpeed);
    }
    else if(attribute == "gravitation"){
        BasePointers.back().setPointer(&gravitation);
    }
    else if(attribute == "base_friction"){
        BasePointers.back().setPointer(&baseFriction);
    }
    else if(attribute == "momentum_x"){
        BasePointers.back().setPointer(&momentum.x);
    }
    else if(attribute == "momentum_y"){
        BasePointers.back().setPointer(&momentum.y);
    }
    else if(attribute == "max_momentum_x"){
        BasePointers.back().setPointer(&maxMomentumX);
    }
    else if(attribute == "min_momentum_y"){
        BasePointers.back().setPointer(&minMomentumY);
    }
    else if(attribute == "max_momentum_y"){
        BasePointers.back().setPointer(&maxMomentumY);
    }
    else if(attribute == "destination_x"){
        BasePointers.back().setPointer(&destination.x);
    }
    else if(attribute == "destination_y"){
        BasePointers.back().setPointer(&destination.y);
    }
    else if(attribute == "direction_of_move_x"){
        BasePointers.back().setPointer(&directionOfMove.x);
    }
    else if(attribute == "direction_of_move_y"){
        BasePointers.back().setPointer(&directionOfMove.y);
    }
    else if(attribute == "mouse_button"){
        BasePointers.back().setPointer(&mouseButton);
    }
    else if(attribute == "move_on_mouse_release"){
        BasePointers.back().setPointer(&moveOnMouseRelease);
    }
    else if(attribute == "reset_direction_after_collision"){
        BasePointers.back().setPointer(&resetDirectionAfterCollision);
    }
    else if(attribute == "is_moving_up"){
        BasePointers.back().setPointer(&isMovingUp);
    }
    else if(attribute == "is_moving_right"){
        BasePointers.back().setPointer(&isMovingRight);
    }
    else if(attribute == "is_moving_down"){
        BasePointers.back().setPointer(&isMovingDown);
    }
    else if(attribute == "is_moving_left"){
        BasePointers.back().setPointer(&isMovingLeft);
    }
    else if(attribute == "is_jumping"){
        BasePointers.back().setPointer(&isJumping);
    }
    else if(attribute == "is_falling"){
        BasePointers.back().setPointer(&isFalling);
    }
    else if(attribute == "is_running"){
        BasePointers.back().setPointer(&isRunning);
    }
    else if(attribute == "up_key"){
        BasePointers.back().setPointer(&upKey);
    }
    else if(attribute == "right_key"){
        BasePointers.back().setPointer(&rightKey);
    }
    else if(attribute == "down_key"){
        BasePointers.back().setPointer(&downKey);
    }
    else if(attribute == "left_key"){
        BasePointers.back().setPointer(&leftKey);
    }
    else if(attribute == "jump_key"){
        BasePointers.back().setPointer(&jumpKey);
    }
    else if(attribute == "running_key"){
        BasePointers.back().setPointer(&runningKey);
    }
    else if(attribute == "are_random_actions_enabled"){
        BasePointers.back().setPointer(&areRandomActionsEnabled);
    }
    else if(attribute == "time_when_action_persists"){
        BasePointers.back().setPointer(&timeWhenActionPersists);
    }
    else if(attribute == "min_time_of_action"){
        BasePointers.back().setPointer(&minTimeOfAction);
    }
    else if(attribute == "max_time_of_action"){
        BasePointers.back().setPointer(&maxTimeOfAction);
    }
    else if(attribute == "min_move_distance"){
        BasePointers.back().setPointer(&minMoveDistance);
    }
    else if(attribute == "max_move_distance"){
        BasePointers.back().setPointer(&maxMoveDistance);
    }
    else if(attribute == "saved_action"){
        BasePointers.back().setPointer(&savedAction);
    }
    else if(attribute == "saved_direction_x"){
        BasePointers.back().setPointer(&savedDirection.x);
    }
    else if(attribute == "saved_direction_y"){
        BasePointers.back().setPointer(&savedDirection.y);
    }
    else if(attribute == "chance_for_break"){
        BasePointers.back().setPointer(&chanceForBreak);
    }
    else if(attribute == "current_direction_x"){
        BasePointers.back().setPointer(&currentDirection.x);
    }
    else if(attribute == "current_direction_y"){
        BasePointers.back().setPointer(&currentDirection.y);
    }
    else if(attribute == "current_pos"){
        BasePointers.back().setPointer(&currentPos);
    }
    else if(attribute == "loop_movement"){
        BasePointers.back().setPointer(&loopMovement);
    }
    else{
        BasePointers.pop_back();
        bindPrimaryToVariable(attribute, BasePointers);
    }
}

short MovementModule::getMovementType(){
    return movementType;
}
short MovementModule::getInputType(){
    return inputType;
}
short MovementModule::getAllowedJumps(){
    return allowedJumps;
}
short MovementModule::getJumpsCount(){
    return jumpsCount;
}
double MovementModule::getJumpCooldown(){
    return jumpCooldown;
}
double MovementModule::getJumpCooldownDuration(){
    return jumpCooldownDuration;
}
double MovementModule::getMoveCooldown(){
    return moveCooldown;
}
double MovementModule::getMoveCooldownDuration(){
    return moveCooldownDuration;
}
bool MovementModule::getCanMoveDiagonally(){
    return canMoveDiagonally;
}
bool MovementModule::getResetMomentumWhenJumping(){
    return resetMomentumWhenJumping;
}
bool MovementModule::getIsMovePlanned(){
    return isMovePlanned;
}
bool MovementModule::getCanJump(){
    return canJump;
}
double MovementModule::getBodyMass(){
    return bodyMass;
}
double MovementModule::getWalkingSpeed(){
    return walkingSpeed;
}
double MovementModule::getRunningSpeed(){
    return runningSpeed;
}
double MovementModule::getJumpSpeed(){
    return jumpSpeed;
}
double MovementModule::getGravitation(){
    return gravitation;
}
double MovementModule::getBaseFriction(){
    return baseFriction;
}
vec2d MovementModule::getMomentum(){
    return momentum;
}
bool MovementModule::isMoving() const{
    if(momentum.x != 0 || momentum.y != 0){
        return true;
    }
    return false;
}
bool MovementModule::isMovingInThisDirection(string direction){
    if(direction == "is_moving_up"){
        return isMovingUp;
    }
    if(direction == "is_moving_right"){
        return isMovingRight;
    }
    if(direction == "is_moving_down"){
        return isMovingDown;
    }
    if(direction == "is_moving_left"){
        return isMovingLeft;
    }
    return false;
}
double MovementModule::getMaxMomentumX(){
    return maxMomentumX;
}
double MovementModule::getMinMomentumY(){
    return minMomentumY;
}
double MovementModule::getMaxMomentumY(){
    return maxMomentumY;
}
vec2d MovementModule::getDestination(){
    return destination;
}
vec2d MovementModule::getDirectionOfMove(){
    return directionOfMove;
}
short MovementModule::getMouseButton(){
    return mouseButton;
}
bool MovementModule::getMoveOnMouseRelease(){
    return moveOnMouseRelease;
}
bool MovementModule::getResetDirectionAfterCollision(){
    return resetDirectionAfterCollision;
}
short MovementModule::getUpKey(){
    return upKey;
}
short MovementModule::getRightKey(){
    return rightKey;
}
short MovementModule::getDownKey(){
    return downKey;
}
short MovementModule::getLeftKey(){
    return leftKey;
}
short MovementModule::getJumpKey(){
    return jumpKey;
}
short MovementModule::getRunningKey(){
    return runningKey;
}

void MovementModule::setMovementType(short newValue){
    movementType = newValue;
}
void MovementModule::setInputType(short newValue){
    inputType = newValue;
}
void MovementModule::setAllowedJumps(short newValue){
    allowedJumps = newValue;
}
void MovementModule::setJumpsCount(short newValue){
    jumpsCount = newValue;
}
void MovementModule::setJumpCooldown(double newValue){
    jumpCooldown = newValue;
}
void MovementModule::setJumpCooldownDuration(double newValue){
    jumpCooldownDuration = newValue;
}
void MovementModule::setMoveCooldown(double newValue){
    moveCooldown = newValue;
}
void MovementModule::setMoveCooldownDuration(double newValue){
    moveCooldownDuration = newValue;
}
void MovementModule::setCanMoveDiagonally(bool newValue){
    canMoveDiagonally = newValue;
}
void MovementModule::setResetMomentumWhenJumping(bool newValue){
    resetMomentumWhenJumping = newValue;
}
void MovementModule::setIsMovePlanned(bool newValue){
    isMovePlanned = newValue;
}
void MovementModule::setCanJump(bool newValue){
    canJump = newValue;
}
void MovementModule::setBodyMass(double newValue){
    bodyMass = newValue;
}
void MovementModule::setWalkingSpeed(double newValue){
    walkingSpeed = newValue;
}
void MovementModule::setRunningSpeed(double newValue){
    runningSpeed = newValue;
}
void MovementModule::setJumpSpeed(double newValue){
    jumpSpeed = newValue;
}
void MovementModule::setGravitation(double newValue){
    gravitation = newValue;
}
void MovementModule::setBaseFriction(double newValue){
    baseFriction = newValue;
}
void MovementModule::setMomentum(vec2d newValue){
    momentum.set(newValue);
}
void MovementModule::setMomentum(double momentumX, double momentumY){
    momentum.x = momentumX;
    momentum.y = momentumY;
}
void MovementModule::setMaxMomentumX(double newValue){
    maxMomentumX = newValue;
}
void MovementModule::setMinMomentumY(double newValue){
    minMomentumY = newValue;
}
void MovementModule::setMaxMomentumY(double newValue){
    maxMomentumY = newValue;
}
void MovementModule::setDestination(vec2d newValue){
    destination.set(newValue);
}
void MovementModule::setDirectionOfMove(vec2d newValue){
    directionOfMove.set(newValue);
}
void MovementModule::setMouseButton(short newValue){
    mouseButton = newValue;
}
void MovementModule::setMoveOnMouseRelease(bool newValue){
    moveOnMouseRelease = newValue;
}
void MovementModule::setResetDirectionAfterCollision(bool newValue){
    resetDirectionAfterCollision = newValue;
}
void MovementModule::setUpKey(char newValue){
    upKey = translateToAllegroKey(newValue);
}
void MovementModule::setRightKey(char newValue){
    rightKey = translateToAllegroKey(newValue);
}
void MovementModule::setDownKey(char newValue){
    downKey = translateToAllegroKey(newValue);
}
void MovementModule::setLeftKey(char newValue){
    leftKey = translateToAllegroKey(newValue);
}
void MovementModule::setJumpKey(char newValue){
    jumpKey = translateToAllegroKey(newValue);
}
void MovementModule::setRunningKey(char newValue){
    runningKey = translateToAllegroKey(newValue);
}
