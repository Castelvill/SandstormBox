#ifndef EVENTMODULE_H_INCLUDED
#define EVENTMODULE_H_INCLUDED
#include "superTextModule.h"
#include "vectorModule.h"

struct ValueLocation{
    string process;
    string source; //on_boot, second_passed, fps, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, literal, variable, camera, layer, owner, object, pointer
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
    vector <EngineInstr> operators; //!, ==, !=, <=, <, >=, >, &&, ||, igT (ignore the rest if true), igF (ignore the rest if false)
    ConditionClass(unsigned int newID);
    ConditionClass(string newID);
    ConditionClass();
};

struct ParameterStruct{
    char type; //e - empty, l - literals, c - variable, v - vector
    vector<VariableModule> Literals;
    vector<string> variableIDs;
    vector<stupidBool> negateVariable;
    ParameterStruct();
};

struct WordStruct{
    char type; //o - operation, b - bool, i - int, d - double, s - string, e - empty, c - context
    string value;
    bool negateVariable;
};

class OperationClass{
public:
    vector <ConditionClass> ConditionalChain;
    vector <ParameterStruct> Parameters;
    ValueLocation Location;
    EngineInstr instruction; //first, last, all, random, let, assigment, class method, run(), break, return
    string newContextID; //Assign an id to the result of operation.
    OperationClass();
    
    //Add literal or context. The type will be checked only if the provided word is a literal.
    //Available types: a - anything, v - variable, n - number, b - bool, i - int, d - double, s - string.
    bool addParameter(string scriptName, unsigned lineNumber, string & error, vector<WordStruct> words, unsigned index, char type, string name, bool optional);
    bool addLiteralOrVectorOrVariableToParameters(string scriptName, unsigned lineNumber, string & error, vector<WordStruct> words, unsigned & index, char type, string name, bool optional);
    bool addVectorOrVariableToParameters(string scriptName, unsigned lineNumber, string & error, vector<WordStruct> words, unsigned & index, char type, string name, bool optional);
    void addLiteralParameter(const VariableModule & Variable);
};

struct ChildStruct{
	string ID;
    bool finished; //If true, ignore the child.
};

class EveModule: public PrimaryModule{
public:
	vector <ConditionClass> ConditionalChain;
    vector <OperationClass> DependentOperations;
	vector <OperationClass> PostOperations; //Post operations can be executed ONLY if ConditionalChain returns true - otherwise only the else scope will be called and executed.
	vector <ChildStruct> Children;
    //Types of triggers checked first in the conditional chain hierarchy. Without them event can be executed only by the other events with the use of "run" and "else" commands.
    //Types: on_boot, on_init, each_iteration, second_passed, key_pressed, key_pressing, key_released, mouse_moved, mouse_not_moved, mouse_pressed, mouse_pressing, mouse_released, objects, variables, collision, editables, movement, stillness, on_display_resize.
    vector <string> primaryTriggerTypes;
    char conditionalStatus; //n-null, t-true, f-false
    string elseChildID; //Ignore if empty. "Else if" statements can be created by adding conditions to the "else child" Event. Each Event can only have one else statement ("else child") - it's a normal thing in branching. It's the optimal way. Do not argue, stupid.
    bool areDependentOperationsDone;
    bool elseChildFinished;
    bool loop;
    bool willBeDeleted; //Event will be deleted as soon as possible, but it still can be executed.
    EveModule();
    EveModule(unsigned int textModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    EveModule(string textModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    ~EveModule();
    void clone(const EveModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);

    void setUpNewInstance();
    void clear();
	void resetStatus();
	bool checkIfAllChildrenFinished();

    void controlText(TextModule * Text, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer);
    void controlEditableText(EditableTextModule * Text, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer);
    void controlSuperText(SuperTextModule * SuperText, string attribute, const vector<VariableModule> & Values,
        vector <string> & IDs, vector<SingleFont> & FontContainer, string EXE_PATH, string workingDirectory
    );
    void controlSuperEditableText(SuperEditableTextModule *& SuperEditableText, string attribute, const vector<VariableModule> & Values,
        vector <string> & IDs, vector<SingleFont> & FontContainer, SuperEditableTextModule *& ActiveEditableText, string EXE_PATH, string workingDirectory
    );
    void controlImage(ImageModule * Image, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, vector<SingleBitmap> & BitmapContainer, string workingDirectory);
    void controlMovement(MovementModule * Movement, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlCollision(CollisionModule * Collision, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlParticles(ParticleEffectModule * Particles, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlVariables(VariableModule * Variable, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlScrollbar(ScrollbarModule * Scrollbar, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlPrimitives(PrimitivesModule * Primitives, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlVector(VectorModule * Vector, string attribute, const vector<VariableModule> & Values, vector <string> & IDs);

    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
};

#endif // EVENTMODULE_H_INCLUDED
