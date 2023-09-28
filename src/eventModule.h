#ifndef EVENTMODULE_H_INCLUDED
#define EVENTMODULE_H_INCLUDED
#include "textModule.h"

struct ValueLocation{
    string source; //on_boot, second_passed, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, literal, variable, camera, layer, owner, object, pointer
    string cameraID;
    string layerID;
    string objectID;
    string moduleType; //ancestor, text, editable, image, movement, collision, particles, variables, scrollbar
    string moduleID;
    string attribute;
    string spareID; //used in detecting collision of two specific hitboxes
    void print(string dynamicID);
};

class ConditionClass{
public:
    VariableModule Literal;
    ValueLocation Location;
    vector <string> operators; //!, ==, !=, <=, <, >=, >, &&, ||, igT (ignore the rest if true), igF (ignore the rest if false)
    ConditionClass(unsigned int newID);
    ConditionClass(string newID);
    ConditionClass();
};

class OperaClass{
public:
    vector <ConditionClass> ConditionalChain;
    vector <VariableModule> Literals;
    ValueLocation Location;
    string instruction; //first, last, all, random, let, assigment, class method, run(), break, return
    vector <string> dynamicIDs;
    string newContextID; //Assign an id to the result of operation.
    OperaClass();
};

struct ChildStruct{
	string ID;
    bool finished; //If true, ignore the child.
};

class EveModule: public PrimaryModule{
public:
	vector <ConditionClass> ConditionalChain;
    vector <OperaClass> DependentOperations;
	vector <OperaClass> PostOperations;
	vector <ChildStruct> Children;
    //Types of triggers checked first in the conditional chain hierarchy. Without them event can be executed by other events and a direct use of run() command.
    //Types: each_iteration, second_passed, key_pressed, key_pressing, key_released, mouse_moved, mouse_not_moved, mouse_pressed, mouse_pressing, mouse_released, objects, variables, collision, editables, movement, stillness.
    vector <string> primaryTriggerTypes;
    char conditionalStatus; //n-null, t-true, f-false
    string elseChildID; //Ignore if empty.
    bool areDependentOperationsDone;
    bool elseChildFinished;
    bool loop;
    EveModule();
    EveModule(unsigned int textModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    EveModule(string textModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    void clone(const EveModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);

    void setUpNewInstance();
    void clear();
	void resetStatus();
	bool checkIfAllChildrenFinished();

    void controlText(TextModule * Text, string attribute, const vector<VariableModule> & Values);
    void controlImage(ImageModule * Image, string attribute, const vector<VariableModule> & Values);
    void controlMovement(MovementModule * Movement, string attribute, const vector<VariableModule> & Values);
    void controlCollision(CollisionModule * Collision, string attribute, const vector<VariableModule> & Values);
    void controlParticles(ParticleEffectModule * Particles, string attribute, const vector<VariableModule> & Values);
    void controlVariables(VariableModule * Variable, string attribute, const vector<VariableModule> & Values);
    void controlScrollbar(ScrollbarModule * Scrollbar, string attribute, const vector<VariableModule> & Values);

    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
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
    EventModule();
    EventModule(int moduleID);
    string getTypeOfTrigger(short operationID);
    void setUpButton(vec2d newPos, vec2d newSize);
    unsigned int countOperations();
    void addOperation(string newItsBinderToTrigger, string newTypeOfTrigger, string newBindedToTrigger, string newConnectedObject, bool triggerItself, string newConnectedModule, string newConnectedModuleName,
                        string newConnectedVariable, vecXd newChoosenDoubles, vecXs newChoosenStrings);
    void resetOperations();
    void clear();
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
