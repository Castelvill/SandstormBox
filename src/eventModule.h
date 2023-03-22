#ifndef EVENTMODULE_H_INCLUDED
#define EVENTMODULE_H_INCLUDED
#include "textModule.h"
#include "movementModule.h"
#include "particles.h"
#include "mouse.h"
#include "variableModule.h"
#include "scrollbarModule.h"

struct VariableStateStruct{
    string layerID;
    string objectID;
    string moduleID;
    string variableID;
    char type;
    bool vBool;
    int vInt;
	double vDouble;
	string vString;
};

struct ConditionStruct{
    bool negation;
    VariableStateStruct Variable;
    string operation; //==, !=, <=, <, >=, >
};

struct OperaStruct{
    string layerID;
    string objectID;
    string moduleID;
    string functionID; //also break and return
    string affectedVariable;
    vector <double> choosenDoubles;
    vector <string> choosenStrings;
};

struct ChildStruct{
	bool finished;
	string ID;
};

class EveModule: public PrimaryModule{
public:
	vector <ConditionStruct> Conditions;
	vector <OperaStruct> DependentOperations;
	vector <OperaStruct> PostOperations;
	vector <ChildStruct> Children;
	string parentID;
	bool parentStatus;
	bool werePostOperationsExecuted;

    void clearModule();
	void resetStatus();
	bool areAllConditionsFulfilled();
	void executeDependentOperations();
	void executePostOperations();
	bool allChildrenFinished();

    void controlAncestor(OperaStruct & Operation, vec2d & objectPos, vec2d & objectSize);
    void controlText(OperaStruct & Operation, TextModule & Text);
    void controlImage(OperaStruct & Operation, ImageModule & Image);
    void controlMovement(OperaStruct & Operation, MovementModule & Movement);
    void controlCollision(OperaStruct & Operation, CollisionModule & Collision);
    void controlParticles(OperaStruct & Operation, ParticleEffectModule & Particles);
    void controlVariables(OperaStruct & Operation, VariableModule & Variable);
    void controlScrollbar(OperaStruct & Operation, ScrollbarModule & Scrollbar);
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
