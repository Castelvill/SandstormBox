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
    super_text, super_editable_text, image, movement,
    collision, particles, event, scrollbar, primitives, variable, exists
};
ValueSource strToSource(const string & source, string & error);
ValueSource strToSource(const string & source);
ValueSource instrToSource(EngineInstr instruction);
string sourceToStr(ValueSource instruction);
enum DataType: char{
    null_dt,
    //Basic data types
    bool_inst, bool_vec, int_inst, int_vec, double_inst, double_vec, string_inst,
    string_vec, value_inst, value_vec, pointer_inst, pointer_vec,
    //Engine container
    camera_inst, camera_vec, layer_inst, layer_vec, object_inst, object_vec,
    //Object's module container
    variable_mod, variable_mod_vec, vector_mod, vector_mod_vec, super_text_mod, super_text_mod_vec,
    super_editable_text_mod, super_editable_text_mod_vec, image_mod, image_mod_vec,
    movement_mod, movement_mod_vec, collision_mod, collision_mod_vec, particles_mod,
    particles_mod_vec, event_mod, event_mod_vec, scrollbar_mod, scrollbar_mod_vec,
    primitives_mod, primitives_mod_vec,
    //Compile-time inference not implemented yet 
    any_dt
};
DataType strToDataType(const string & dataType);
string dataTypeToStr(DataType dataType);
DataType sourceToEntityType(const InstrDescription & CurrentInstr, const ValueSource & source);
DataType instrToDataType(EngineInstr instruction, bool singleInstance);

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
    unsigned localAddresses[2] = {0, 0};
    vector<EngineInstr> operators; //!, ==, !=, <=, <, >=, >, &&, ||, igT (ignore the rest if true), igF (ignore the rest if false)
    ConditionClass(unsigned int newID);
    ConditionClass(string newID);
    ConditionClass();
};

struct ParameterStruct{
    unsigned treeLevel = 0;
    char type = 'e'; //e - empty, l - literal, c - variable, v - vector
    VariableModule Literal;
    unsigned localAddress = 0;
    string variableID = "";
    bool negateVariable = false;
    string getVariableIdOrValue();
};

struct OutputParameterStruct{
    unsigned localAddress = 0;
    string variableID = "";
    DataType type = null_dt;
};

enum class TokenType {
    empty_tk, keyword_tk,
    identifier_tk, //user defined name/keyword
    start_scope_tk, end_scope_tk,
    start_expr_tk, end_expr_tk,
    open_brackets_tk, close_brackets_tk,
    bool_tk, int_tk, double_tk, string_tk
};

string tokenToStr(TokenType type);

struct WordStruct{
    TokenType type = TokenType::empty_tk;
    string value = "";
    bool negateVariable = false;
    EngineInstr instruction = null;
};

struct VariableInfo{
    string name; //Only for debugging.
    DataType type = null_dt;
    bool isReference = false; //If true, when passing arguments to an event, copy the address of the variable instead of copying its value.  
    unsigned defaultAddress = 0;
};

struct VariableLocationStruct{
    string name;
    DataType type = null_dt;
    bool isLocal = true; //False if the variable is global AND not yet referenced locally.
    bool isReference = false; //True if it's global or a reference
    unsigned index = 0; //Index of local or global variable.
    unsigned defaultAddress = 0; //Required for the global variables - if isLocal is false.
};

struct DynamicVariableInfo{
    unsigned dynamicAddress = 0; //It's set to defaultAddress if the function was called for the first time in the stack, otherwise you have to use spare addresses or allocate new variables. If you do that, set Context type of new variables to "Any".
    unsigned dynamicMemoryAddress = 0; //If the variable is dynamically allocated, use this address to "deallocate its memory" after exiting from the current event.
    bool isReference = false;
};

struct PassingVariableInfo{
    bool isReference = false;
    DataType type = null_dt;
    unsigned localAddress = 0;
    string name = "";
    string parameterName = "";
    UniversalVariable literal;
};


std::pair<unsigned, ReturnType> getLocalAddress(const string & variableId, const DataType & variableType,
    vector<vector<VariableLocationStruct>> & Scopes, vector<VariableInfo> & NewLocalVariables,
    unsigned & topAddress, bool canAllocateNewVariable = true, bool makeVariableGlobal = false,
    bool makeVariableReference = false, bool forceNewDeclaration = false
);
std::pair<unsigned, ReturnType> findExistingVariableOrCreateNew(const string & scriptName,
    const unsigned lineNumber, const string & variableID, const DataType & variableType,
    vector<vector<VariableLocationStruct>> & Scopes, vector<VariableInfo> & NewLocalVariables,
    unsigned & topAddress, bool canCreateNewVariable, bool ignoreUndefinedVariable
);

class OperationClass{
public:
    vector<ConditionClass> ConditionalChain;
    vector<VariableModule> resultStack;
    vector<ParameterStruct> Parameters;
    unsigned rootParametersSize = 0;
    ValueLocation Location;
    EngineInstr instruction = null; //first, last, all, random, let, assigment, class method, run(), break, return
    string scriptName;
    unsigned lineNumber = 0;

    OutputParameterStruct Output;

    unsigned jumpToLine = 0; //Line number of end_if label in if statements.
    unsigned specialValue = 0; //Line number of a next else or else_if statement.
    OperationClass();
    
    //Add literal or context. The type will be checked only if the provided word is a literal.
    //Available types: a - anything, v - variable, n - number, b - bool, i - int, d - double, s - string.
    bool addParameter(const string & scriptName, const unsigned lineNumber,
        string & error, vector<WordStruct> words, vector<vector<VariableLocationStruct>> & Scopes,
        vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, unsigned index, char type,
        const string & parameterName, bool optional, bool canCreateNewVariable, bool ignoreUndefinedVariable
    );
    void addEmptyParameter();
    bool addLiteralOrVectorOrVariableToParameters(
        const string &scriptName, const unsigned lineNumber, string & error,
        vector<WordStruct> words, vector<vector<VariableLocationStruct>> & Scopes,
        vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, unsigned & index,
        char type, const string & name, bool optional, bool canCreateNewVariable, bool forbidVectors
    );
    bool addVectorOrVariableToParameters(
        const string & scriptName, const unsigned lineNumber, string &error,
        vector<WordStruct> words, vector<vector<VariableLocationStruct>> & Scopes,
        vector<VariableInfo> & NewLocalVariables, unsigned & topAddress,
        unsigned &index, char type, string name, bool optional, bool canCreateNewVariable,
        const bool & forbidVectors = false
    );
    bool addVectorToParameters(const string & scriptName, const unsigned lineNumber, string &error,
        vector<WordStruct> words, vector<vector<VariableLocationStruct>> & Scopes, vector<VariableInfo> & NewLocalVariables,
        unsigned & topAddress, unsigned &index, char type, const string & name, bool canCreateNewVariable,
        const bool & forbidVectors
    );
    void addLiteralParameter(const VariableModule & Variable);
};

struct ChildStruct{
    size_t uniqueIndex = 0;
    string id;
    vector<PassingVariableInfo> originalArguments;
    unsigned containerIndex = 0;
    string callingScript;
    unsigned lineNumber = 0;
    bool isRecursiveCall = false;
    size_t idxOfFirstNamedArg = 0;
    vector<PassingVariableInfo> arrangedArguments;
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
    vector<OperationClass> Operations;
	vector<ChildStruct> Children;
    //Types of triggers checked first in the conditional chain hierarchy. Without them event can be executed only by the other events.
    vector<TriggerType> primaryTriggerTypes;
    vector<VariableInfo> LocalVariables;
    vector<PassingVariableInfo> Parameters;

    string callingEventID = "";
    string callingType = "";
    bool willBeDeleted = false; //Event will be deleted as soon as possible, but it still can be executed.
    bool isFunction = true; //True if the event has not been connected to any trigger. It will stay false even if on_init trigger is removed. Currently it's only used in the "tree" instruction.

    EventModule();
    EventModule(size_t & topModuleUniqueIndex);
    EventModule(PrimaryData & initData);
    ~EventModule();
    void clone(const EventModule &Original, PrimaryData & initData, bool changeOldID);

    void setUpNewInstance();
    void clear();

    bool getPassedVariables(const vector<WordStruct> & words, unsigned & cursor, const unsigned & lineNumber,
        const string & scriptName, vector<vector<VariableLocationStruct>> & Scopes, unsigned & topAddress
    );
    bool getPassingVariables(vector<PassingVariableInfo> &Arguments,
        const vector<WordStruct> &words, unsigned &cursor, const unsigned lineNumber,
        const string &scriptName, vector<vector<VariableLocationStruct>> & Scopes,
        unsigned & topAddress, size_t & namedArgumentsStartAt
    );
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
