#ifndef MOVEMENTMODULE_H_INCLUDED
#define MOVEMENTMODULE_H_INCLUDED
#include "collisionModule.h"
#include <allegro5/allegro_primitives.h>

class RandomAction{
public:
    bool areRandomActionsEnabled;
    double timeWhenActionPersists;
    double minTimeOfAction;
    double maxTimeOfAction;
    double minMoveDistance;
    double maxMoveDistance;
    short savedAction;
    vec2d savedDirection;
    bool areBreaksAllowed;
    double chanceForBreak;
    RandomAction();
    short chooseRandomKey(short actionType);
    void changeRandomActionsSettings(bool newAreRandomActionsEnabled, double newMinTimeOfAction, double newMaxTimeOfAction, double newMinMoveDistance, double newMaxMoveDistance, bool newAreBreaksAllowed, double newChanceForBreak);
    vec2d chooseRandomDirection(vec2d objectPos, vec2d & objectDestination);
};


class ChainMovement{
public:
    //Before changing direction, it counts new direction vector
    vector <vec2d> posChain; //chain of positions
    vec2d currentDirection;
    int currentPos;
    bool loopMovement;
    void clearChain();
};


class MovementModule: public PrimaryModule, public RandomAction, public ChainMovement{
private:
    short movementType; //0-two directions and jump, 1-eight directions, 2-vector based
    short inputType; //0-none, 1-random, 2-keyboard, 3-keyboard+random, 4-mouse (vector), 5-mouse+random (vector), 6-chain move (vector), 7-mouse+path finding algorithm (vector)
    short allowedJumps;
    short jumpsCount;
    double jumpCooldown; //You cannot press jump button until cooldown falls to zero.
    double jumpCooldownDuration; //Constant, it's a default value of jumpCooldown.
    double moveCooldown;
    double moveCooldownDuration; //Constant, it's a default value of moveCooldown.
    bool canMoveDiagonally;
    bool resetMomentumWhenJumping;

    bool isMovePlanned;
    bool canJump; //If momentum speed reaches max level, hits something or jump button is released, canJump turns to false. You cannot jump if canJump is false except when there are more jumps available.

    double bodyMass;
    double walkingSpeed, runningSpeed; //add this to momentumX every second of move button press till maxMomentumX is reached
    double jumpSpeed; //add this to momentumY every second of button press till maxMomentumY is reached
    double gravitation; //substract this from momentumY every second till minMomentumX is reached
    double baseFriction; //bring momentumY to zero with this value every second till 0 is reached
    vec2d momentum; //variable, add this to x and y, they cannot exceed maxMomentumX, minMomentumY and maxMomentumY
    double maxMomentumX; //absolute max speed object can move in both directions
    double minMomentumY, maxMomentumY; //max speed object can move in Y axis

    vec2d destination;
    vec2d directionOfMove;
    short mouseButton;
    bool moveOnMouseRelease;
    bool resetDirectionAfterCollision;

    bool isMovingUp, isMovingRight, isMovingDown, isMovingLeft, isJumping, isFalling, isRunning; //Next action
    short upKey, rightKey, downKey, leftKey, jumpKey, runningKey;

public:
    void setUpNewInstance(const string & newID, vector<string> * listOfIDs, const string & newLayerID, const string & newObjectID);
    MovementModule();
    MovementModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    MovementModule(unsigned int newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~MovementModule();
    void clone(const MovementModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clear();

    void changeJumpParameters(double newJumpSpeed, short newAllowedJumps, double newJumpCooldownDuration, double newGravitation, double newMinMomentumY, double newMaxMomentumY, bool newResetMomentumWhenJumping);
    void changeMoveParameters(short newMovementType, short newInputType, double newBodyMass, double newWalkingSpeed, double newRunningSpeed, double newBaseFriction, double newMaxMomentumX, double newMoveCooldown, bool newCanMoveDiagonally);
    void changeVectorMoveParameters(short newMouseButton, bool newMoveOnMouseRelease, bool newResetDirectionAfterCollision);
    void setNextMove(bool up, bool right, bool down, bool left, bool jump, bool fall, bool run);
    void addMomentum(vec2d newMomentum);
    void addMomentum(double x, double y);
    void updateMomentum(vec2d objectPos);
    void stopMovementAfterCollision(short collisionType);
    void updateMomentumAfterCollisionBetter(vec2d pos, vector <CollisionModule> movingCollisions);

    void movingWithJump();
    void movingIn8Directions();
    void movingWithVectors(vec2d objectPos);
    void movingWithPathFinding();
    void clearStates();
    void translateKeyboardToState(vector <short> pressedKeys);
    void translateNumericalActionToState(short action);
    void translateMouseToVector(bool canUserInteract, bool pressedMouse[5], bool releasedMouse[5], vec2d mousePos, vec2d objectPos);
    void updateStatesAndVectors(bool canUserInteract, vector <short> pressedKeys, bool pressedMouse[5], bool releasedMouse[5], vec2d mousePos, vec2d objectPos);
    void resetJump();
    void bindKeys(short newUpKey, short newRightKey, short newDownKey, short newLeftKey, short newJumpKey, short newRunningKey);
    void bindMouseButton(short newMouseButton);
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);

    short getMovementType() const;
    short getInputType() const;
    short getAllowedJumps() const;
    short getJumpsCount() const;
    double getJumpCooldown() const;
    double getJumpCooldownDuration() const;
    double getMoveCooldown() const;
    double getMoveCooldownDuration() const;
    bool getCanMoveDiagonally() const;
    bool getResetMomentumWhenJumping() const;
    bool getIsMovePlanned() const;
    bool getCanJump() const;
    double getBodyMass() const;
    double getWalkingSpeed() const;
    double getRunningSpeed() const;
    double getJumpSpeed() const;
    double getGravitation() const;
    double getBaseFriction() const;
    vec2d getMomentum() const;
    bool isMoving() const;
    bool isMovingInThisDirection(string direction) const;
    double getMaxMomentumX() const;
    double getMinMomentumY() const;
    double getMaxMomentumY() const;
    vec2d getDestination() const;
    vec2d getDirectionOfMove() const;
    short getMouseButton() const;
    bool getMoveOnMouseRelease() const;
    bool getResetDirectionAfterCollision() const;
    short getUpKey() const;
    short getRightKey() const;
    short getDownKey() const;
    short getLeftKey() const;
    short getJumpKey() const;
    short getRunningKey() const;

    void setMovementType(short);
    void setInputType(short);
    void setAllowedJumps(short);
    void setJumpsCount(short);
    void setJumpCooldown(double);
    void setJumpCooldownDuration(double);
    void setMoveCooldown(double);
    void setMoveCooldownDuration(double);
    void setCanMoveDiagonally(bool);
    void setResetMomentumWhenJumping(bool);
    void setIsMovePlanned(bool);
    void setCanJump(bool);
    void setBodyMass(double);
    void setWalkingSpeed(double);
    void setRunningSpeed(double);
    void setJumpSpeed(double);
    void setGravitation(double);
    void setBaseFriction(double);
    void setMomentum(vec2d);
    void setMomentum(double, double);
    void setMaxMomentumX(double);
    void setMinMomentumY(double);
    void setMaxMomentumY(double);
    void setDestination(vec2d);
    void setDirectionOfMove(vec2d);
    void setMouseButton(short);
    void setMoveOnMouseRelease(bool);
    void setResetDirectionAfterCollision(bool);
    void setUpKey(char);
    void setRightKey(char);
    void setDownKey(char);
    void setLeftKey(char);
    void setJumpKey(char);
    void setRunningKey(char);
};


#endif // MOVEMENTMODULE_H_INCLUDED
