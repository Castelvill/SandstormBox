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
    string moduleID; //ancestor, text, image, movement, collision, particles, variables, scrollbar
};

class TriggerClass{
public:
    bool isIsolated;
    vector <ObjectIDs> leftObjects;
    vector <ObjectIDs> rightObjects;
    VariableModule Variable;
    string triggerName; //time, variable, collision, mouse, layer, camera, keyboard
    char resultType; //v - variable/if; f - first object; a - all objects; forall
    vector <string> conjunctions; //!, ==, !=, <=, <, >=, >, &&, ||, (, ), +(sum of objects), *(product of objects), c(cartesian product)
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
	bool finished;
	string ID;
};

struct ConditionStruct{
    vector <TriggerClass> Triggers; //If empty, it's just else.
	vector <OperaClass> DependentOperations;
};

class EveModule: public PrimaryModule{
public:
	vector <ConditionStruct> ConditionalChain; //If one condition is fullfield, others are ignored - otherwise next condition is checked.
	vector <OperaClass> PostOperations;
	vector <ChildStruct> Children;
	string parentID;
	bool parentStatus;
	bool werePostOperationsExecuted;

    EveModule(unsigned int textModuleID);
    EveModule(string textModuleID);
    void setUpNewInstance();
    void clearModule();
	void resetStatus();
	bool areAllConditionsFulfilled();
	void executeDependentOperations();
	void executePostOperations();
	bool allChildrenFinished();

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
