#ifndef EVENTMODULE_H_INCLUDED
#define EVENTMODULE_H_INCLUDED
#include "superTextModule.h"
#include "vectorModule.h"

enum ValueSource: char{
    null_s, fullscreen, second_passed, fps, is_directory, used_os, screen_w,
    screen_h, key_pressed, key_pressing, key_released, any_key_pressed,
    any_key_pressing, any_key_released, mouse, mouse_moved, mouse_pressed,
    mouse_pressing, mouse_released, literal, camera, layer,
    ancestor, object, display_resized, vector_s, mouse_x, mouse_y,
    display_w, display_h, number_of_processes, number_of_cameras,
    number_of_layers, number_of_objects, booting, process, context,
    text, editable_text, super_text, super_editable_text, image, movement,
    collision, particles, event, scrollbar, primitives, variable, exists
};
ValueSource strToSource(const string & source, string & error);
ValueSource strToSource(const string & source);
string sourceToStr(ValueSource instruction);
enum DataType: char{
    //Basic data type
    null_dt, bool_inst, bool_vec, int_inst, int_vec, double_inst, double_vec, string_inst,
    string_vec, value_inst, value_vec, pointer_inst, pointer_vec,
    //Engine container
    camera_inst, camera_vec, layer_inst, layer_vec, object_inst, object_vec,
    //Object's module container
    variable_mod, variable_mod_vec, vector_mod, vector_mod_vec, text_mod, text_mod_vec,
    editable_text_mod, editable_text_mod_vec, super_text_mod, super_text_mod_vec,
    super_editable_text_mod, super_editable_text_mod_vec, image_mod, image_mod_vec,
    movement_mod, movement_mod_vec, collision_mod, collision_mod_vec, particles_mod,
    particles_mod_vec, event_mod, event_mod_vec, scrollbar_mod, scrollbar_mod_vec,
    primitives_mod, primitives_mod_vec,
    //Compile-time inference not implemented yet 
    any_dt
};
DataType strToDataType(string dataType);
string dataTypeToStr(DataType dataType);
DataType sourceToEntityType(const InstrDescription & CurrentInstr, const ValueSource & source);

struct ValueLocation{
    string process;
    // fullscreen, second_passed, fps, is_directory, used_os, screen_w,
    // screen_h, key_pressed, key_released, any_key_pressed,
    // any_key_pressing, any_key_released, mouse_moved, mouse_pressed,
    // mouse_pressing, mouse_released, literal, variable, camera, layer,
    // owner, object, pointer, display_resized, vector, mouse_x, mouse_y,
    // display_w, display_h, number_of_processes, number_of_cameras,
    // number_of_layers, number_of_objects, booting, process, context
    ValueSource source = ValueSource::null_s;
    string cameraID;
    string layerID;
    string objectID;
    ValueSource moduleType = ValueSource::null_s; //ancestor, text, editable, image, movement, collision, particles, variables, scrollbar
    string moduleID;
    AttributeType attribute = AttributeType::null_a;
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
    unsigned treeLevel = 0;
    char type = 'e'; //e - empty, l - literal, c - variable, v - vector
    VariableModule Literal;
    string variableID = "";
    bool negateVariable = false;
    bool isReference = false;
    string getVariableIdOrValue();
};

struct WordStruct{
    char type = 'e'; //o - operation, b - bool, i - int, d - double, s - string, e - empty, c - context
    string value;
    bool negateVariable;
};

struct StartingVariableStruct{
    DataType type = null_dt;
    string id = "";
    string rawID = ""; //ID without the eventID
    short index = 0;
    string eventID = ""; //If empty, variable is global
    bool isReference = false;
};
bool isVariableGlobal(const vector<StartingVariableStruct> & NewVariablesForLookupTable, const string & variableID);
bool checkIfVariableIsReference(const vector<StartingVariableStruct> & PassedVariables, const string & variableID);
string findExistingVariableOrCreateNew(const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<string> & allAvailableEventIDs, const string & variableID, string & usedEventID,
    bool canCreateNewVariable, const string & scriptName, const unsigned &lineNumber,
    bool ignoreUndefinedVariable = false 
);
string createCustomOutput(const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, const vector<string> & allAvailableEventIDs,
    const string & variableID, bool isReferenceNeeded, bool canCreateNewVariable,
    const string & scriptName, const unsigned &lineNumber
);

class OperationClass{
public:
    vector <ConditionClass> ConditionalChain;
    vector<VariableModule> resultStack;
    vector <ParameterStruct> Parameters;
    unsigned rootParametersSize = 0;
    ValueLocation Location;
    EngineInstr instruction = null; //first, last, all, random, let, assigment, class method, run(), break, return
    string scriptName;
    unsigned lineNumber = 0;
    string outputVariableID;
    bool isOutputReference = false;
    unsigned jumpToLine = 0; //Line number of end_if label in if statements.
    unsigned specialValue = 0; //Line number of a next else or else_if statement.
    OperationClass();
    
    //Add literal or context. The type will be checked only if the provided word is a literal.
    //Available types: a - anything, v - variable, n - number, b - bool, i - int, d - double, s - string.
    bool addParameter(const string & scriptName, const unsigned & lineNumber, string & error, vector<WordStruct> words,
        unsigned index, char type, string name, bool optional, const vector<string> & allAvailableEventIDs,
        const vector<StartingVariableStruct> & NewVariablesForLookupTable,
        const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewVariable,
        bool ignoreUndefinedVariable = false
    );
    void addEmptyParameter();
    bool addLiteralOrVectorOrVariableToParameters(const string & scriptName, const unsigned & lineNumber, string & error,
        vector<WordStruct> words, unsigned & index, char type, string name, bool optional, const vector<string> & allAvailableEventIDs,
        const vector<StartingVariableStruct> & NewVariablesForLookupTable,
        const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewVariable, const bool & forbidVectors = false
    );
    bool addVectorOrVariableToParameters(const string & scriptName, const unsigned & lineNumber, string & error,
        vector<WordStruct> words, unsigned & index, char type, string name, bool optional, const vector<string> & allAvailableEventIDs,
        const vector<StartingVariableStruct> & NewVariablesForLookupTable,
        const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewVariable,
        const bool & forbidVectors = false
    );
    void addLiteralParameter(const VariableModule & Variable);
};

struct ChildStruct{
	string ID;
    vector<string> passingVariables;
    unsigned containerIndex = 0;
    string callingScript;
    unsigned lineNumber = 0;
};

enum TriggerType: char{
    null_t, on_boot, on_init, each_iteration, on_idle, each_second, on_key_press, on_key_pressing, on_key_release,
    on_mouse_move, when_mouse_still, on_mouse_press, on_mouse_pressing, on_mouse_release,
    by_objects, by_variables, by_collision, by_editables, by_movement, by_stillness, on_display_resize
};
TriggerType strToTrigger(const string & trigger);
string triggerToStr(const TriggerType & trigger);

class EventModule: public PrimaryModule{
public:
    vector<VariableModule> resultStack;
    vector<OperationClass> DependentOperations;
	vector<ChildStruct> Children;
    //Types of triggers checked first in the conditional chain hierarchy. Without them event can be executed only by the other events with the use of "run" and "else" commands.
    vector <TriggerType> primaryTriggerTypes;
    vector <StartingVariableStruct> PassedVariables;
    bool isInline = false;
    string callingEventID = "";
    string callingType = "";
    bool willBeDeleted = false; //Event will be deleted as soon as possible, but it still can be executed.
    bool isFunction = true; //True if the event has not been connected to any trigger. It will stay false even if on_init trigger is removed. Currently it's only used in the "tree" instruction.

    unsigned programCounter = 0;
    char conditionalStatus = 'n'; //n-null, t-true, f-false
    bool breakFromCurrentLoop = false;
    bool decrementProgramCounter = false;
    vector<char> goToEndOfIfStatement;

    EventModule();
    EventModule(unsigned int textModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    EventModule(string textModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    ~EventModule();
    void clone(const EventModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);

    void setUpNewInstance();
    void clear();
    void resetStateVariables();

    bool getPassedVariables(const vector<WordStruct> & words, unsigned & cursor,
        const unsigned & lineNumber, const string & scriptName,
        vector<StartingVariableStruct> & NewVariablesForLookupTable
    );
    bool getPassingVariables(vector<string> & passingVariables,
        const vector<WordStruct> & words, unsigned & cursor,
        const unsigned & lineNumber, const string & scriptName,
        const vector<StartingVariableStruct> & NewVariablesForLookupTable,
        const vector<string> & allAvailableEventIDs
    );
    void controlText(TextModule * Text, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer);
    void controlEditableText(EditableTextModule * Text, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer);
    void controlSuperText(SuperTextModule * SuperText, AttributeType attribute, const vector<VariableModule> & Values,
        vector <string> & IDs, vector<SingleFont> & FontContainer, string EXE_PATH, string workingDirectory
    );
    void controlSuperEditableText(SuperEditableTextModule *& SuperEditableText, AttributeType attribute, const vector<VariableModule> & Values,
        vector <string> & IDs, vector<SingleFont> & FontContainer, SuperEditableTextModule *& ActiveEditableText, string EXE_PATH, string workingDirectory
    );
    void controlImage(ImageModule * Image, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs, vector<SingleBitmap> & BitmapContainer, string workingDirectory);
    void controlMovement(MovementModule * Movement, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlCollision(CollisionModule * Collision, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlParticles(ParticleEffectModule * Particles, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlVariables(VariableModule * Variable, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlScrollbar(ScrollbarModule * Scrollbar, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlPrimitives(PrimitivesModule * Primitives, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);
    void controlVector(VectorModule * Vector, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs);

    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
};

#endif // EVENTMODULE_H_INCLUDED
