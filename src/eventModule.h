#ifndef EVENTMODULE_H_INCLUDED
#define EVENTMODULE_H_INCLUDED
#include "textModule.h"
#include "movementModule.h"
#include "particles.h"
#include "mouse.h"
#include "variableModule.h"
#include "scrollbarModule.h"

struct ObjectIDs{
    string cameraID;
    string layerID;
    string objectID;
    string module; //ancestor, text, editable, image, movement, collision, particles, variables, scrollbar
    string elementID; 
};

class TriggerClass{
public:
    string source; //second_passed, key_down, key_pressed, key_released, mouse_down, mouse_pressed, mouse_released, literal, camera, layer, owner, object, pointer
    string dynamicName;
    VariableModule Literal;
    ObjectIDs Object;
    vector <string> pre_conjunctions; //!, (
    vector <string> post_conjunctions; //==, !=, <=, <, >=, >, &&, ||, )
    TriggerClass(unsigned int newID);
};

class OperaClass{
public:
    vector <ObjectIDs> leftObjects;
    vector <ObjectIDs> rightObjects;
    string affectedVariable;
    string functionID; //also break and return
    vector <double> choosenDoubles;
    vector <string> choosenStrings;
};

struct ChildStruct{
	bool finished; //If true, ignore the child.
	string ID;
};

class EveModule: public PrimaryModule{
public:
	vector <TriggerClass> ConditionalChain;
    vector <OperaClass> DependentOperations;
	vector <OperaClass> PostOperations;
	vector <ChildStruct> Children;
    vector <string> primaryTriggerTypes; //Types of triggers checked first in the conditional chain hierarchy. Without them event can be executed by other events and a direct use of run() command.
	char directive; //n - normal, a - atomic, u - unique, r - random
    char conditionalStatus; //n-null, t-true, f-false
    string parentID;
    string elseChildID; //Ignore if empty.
    bool areDependentOperationsDone;
	bool parentStatus;
	bool werePostOperationsExecuted;

    EveModule(unsigned int textModuleID);
    EveModule(string textModuleID);
    void setUpNewInstance();
    void clearModule();
	void resetStatus();
	bool checkIfAllChildrenFinished();

    void controlAncestor(OperaClass & Operation, vec2d & objectPos, vec2d & objectSize);
    void controlText(OperaClass & Operation, TextModule & Text);
    void controlImage(OperaClass & Operation, ImageModule & Image);
    void controlMovement(OperaClass & Operation, MovementModule & Movement);
    void controlCollision(OperaClass & Operation, CollisionModule & Collision);
    void controlParticles(OperaClass & Operation, ParticleEffectModule & Particles);
    void controlVariables(OperaClass & Operation, VariableModule & Variable);
    void controlScrollbar(OperaClass & Operation, ScrollbarModule & Scrollbar);
};







class OperationClass{
public:
    string eventVariableID;
    string typeOfTrigger; //mouse_press, mouse_no_press, mouse_release, mouse_in_area, mouse_not_in_area
    string bindedToTrigger;

    string affectedObjectID;
    unsigned int affectedObjectIndex;
    bool isTriggeringItself;
    string affectedModule;
    string affectedModuleID;
    string affectedVariable;
    vector <double> choosenDoubles;
    vector <string> choosenStrings;

    void clearOperation();
    short getBindedMouseButton();
};


class EventModule: public PrimaryModule{
private:
    bool waitToActivate;
    double timeToActivate;
    double maxTimeToActivate;
    bool isButton;
public:
    vector <OperationClass> Operations;
    EventModule(int moduleID);
    string getTypeOfTrigger(short operationID);
    void setUpButton(vec2d newPos, vec2d newSize);
    unsigned int countOperations();
    void addOperation(string newItsBinderToTrigger, string newTypeOfTrigger, string newBindedToTrigger, string newConnectedObject, bool triggerItself, string newConnectedModule, string newConnectedModuleName,
                        string newConnectedVariable, vecXd newChoosenDoubles, vecXs newChoosenStrings);
    void resetOperations();
    void clearModule();
    bool isEventTriggered(int operationID, MouseClass mouse, vec2d objPos, VariableModule * EventVariable, VariableModule * ObjectVariable);
    void controlAncestor(int operationID, vec2d & objectPos, vec2d & objectSize);
    void controlText(int operationID, TextModule & Text);
    void controlImage(int operationID, ImageModule & Image);
    void controlMovement(int operationID, MovementModule & Movement);
    void controlCollision(int operationID, CollisionModule & Collision);
    void controlParticles(int operationID, ParticleEffectModule & Particles);
    void controlVariables(int operationID, VariableModule & Variable);
    void controlScrollbar(int operationID, ScrollbarModule & Scrollbar);
};

#endif // EVENTMODULE_H_INCLUDED
