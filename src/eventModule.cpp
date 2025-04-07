#include "eventModule.h"

ValueSource transSource(const string & source, string & error){
    error = "";
    if(source == "fullscreen"){
        return ValueSource::fullscreen;
    }
    if(source == "second_passed"){
        return ValueSource::second_passed;
    }
    if(source == "fps"){
        return ValueSource::fps;
    }
    if(source == "is_directory"){
        return ValueSource::is_directory;
    }
    if(source == "used_os"){
        return ValueSource::used_os;
    }
    if(source == "screen_w"){
        return ValueSource::screen_w;
    }
    if(source == "screen_h"){
        return ValueSource::screen_h;
    }
    if(source == "key_pressed"){
        return ValueSource::key_pressed;
    }
    if(source == "key_pressing"){
        return ValueSource::key_pressing;
    }
    if(source == "key_released"){
        return ValueSource::key_released;
    }
    if(source == "any_key_pressed"){
        return ValueSource::any_key_pressed;
    }
    if(source == "any_key_pressing"){
        return ValueSource::any_key_pressing;
    }
    if(source == "any_key_released"){
        return ValueSource::any_key_released;
    }
    if(source == "mouse"){
        return ValueSource::mouse;
    }
    if(source == "mouse_moved"){
        return ValueSource::mouse_moved;
    }
    if(source == "mouse_pressed"){
        return ValueSource::mouse_pressed;
    }
    if(source == "mouse_pressing"){
        return ValueSource::mouse_pressing;
    }
    if(source == "mouse_released"){
        return ValueSource::mouse_released;
    }
    if(source == "literal"){
        return ValueSource::literal;
    }
    if(source == "variable"){
        return ValueSource::variable;
    }
    if(source == "camera"){
        return ValueSource::camera;
    }
    if(source == "layer"){
        return ValueSource::layer;
    }
    if(source == "owner"){
        return ValueSource::owner;
    }
    if(source == "ancestor"){
        return ValueSource::ancestor;
    }
    if(source == "object"){
        return ValueSource::object;
    }
    if(source == "pointer"){
        return ValueSource::pointer;
    }
    if(source == "display_resized"){
        return ValueSource::display_resized;
    }
    if(source == "vector_s"){
        return ValueSource::vector_s;
    }
    if(source == "mouse_x"){
        return ValueSource::mouse_x;
    }
    if(source == "mouse_y"){
        return ValueSource::mouse_y;
    }
    if(source == "display_w"){
        return ValueSource::display_w;
    }
    if(source == "display_h"){
        return ValueSource::display_h;
    }
    if(source == "number_of_processes"){
        return ValueSource::number_of_processes;
    }
    if(source == "number_of_cameras"){
        return ValueSource::number_of_cameras;
    }
    if(source == "number_of_layers"){
        return ValueSource::number_of_layers;
    }
    if(source == "number_of_objects"){
        return ValueSource::number_of_objects;
    }
    if(source == "booting"){
        return ValueSource::booting;
    }
    if(source == "process"){
        return ValueSource::process;
    }
    if(source == "context"){
        return ValueSource::context;
    }
    if(source == "text"){
        return ValueSource::text;
    }
    if(source == "editable_text"){
        return ValueSource::editable_text;
    }
    if(source == "super_text"){
        return ValueSource::super_text;
    }
    if(source == "super_editable_text"){
        return ValueSource::super_editable_text;
    }
    if(source == "image"){
        return ValueSource::image;
    }
    if(source == "movement"){
        return ValueSource::movement;
    }
    if(source == "collision"){
        return ValueSource::collision;
    }
    if(source == "particles"){
        return ValueSource::particles;
    }
    if(source == "event"){
        return ValueSource::event;
    }
    if(source == "scrollbar"){
        return ValueSource::scrollbar;
    }
    if(source == "primitives"){
        return ValueSource::primitives;
    }
    if(source == "exists"){
        return ValueSource::exists;
    }
    error = "ValueSource '" + source + "' is undefined.";
    return ValueSource::null_s;
}

string sourceToStr(ValueSource source){
    switch(source){
        case null_s:
            return "null";
        case fullscreen:
            return "fullscreen";
        case second_passed:
            return "second_passed";
        case fps:
            return "fps";
        case is_directory:
            return "is_directory";
        case used_os:
            return "used_os";
        case screen_w:
            return "screen_w";
        case screen_h:
            return "screen_h";
        case key_pressing:
            return "key_pressing";
        case key_pressed:
            return "key_pressed";
        case key_released:
            return "key_released";
        case any_key_pressed:
            return "any_key_pressed";
        case any_key_pressing:
            return "any_key_pressing";
        case any_key_released:
            return "any_key_released";
        case mouse:
            return "mouse";
        case mouse_moved:
            return "mouse_moved";
        case mouse_pressed:
            return "mouse_pressed";
        case mouse_pressing:
            return "mouse_pressing";
        case mouse_released:
            return "mouse_released";
        case literal:
            return "literal";
        case variable:
            return "variable";
        case camera:
            return "camera";
        case layer:
            return "layer";
        case owner:
            return "owner";
        case ancestor:
            return "ancestor";
        case object:
            return "object";
        case pointer:
            return "pointer";
        case display_resized:
            return "display_resized";
        case vector_s:
            return "vector_s";
        case mouse_x:
            return "mouse_x";
        case mouse_y:
            return "mouse_y";
        case display_w:
            return "display_w";
        case display_h:
            return "display_h";
        case number_of_processes:
            return "number_of_processes";
        case number_of_cameras:
            return "number_of_cameras";
        case number_of_layers:
            return "number_of_layers";
        case number_of_objects:
            return "number_of_objects";
        case booting:
            return "booting";
        case process:
            return "process";
        case context:
            return "context";
        case text:
            return "text";
        case editable_text:
            return "editable_text";
        case super_text:
            return "super_text";
        case super_editable_text:
            return "super_editable_text";
        case image:
            return "image";
        case movement:
            return "movement";
        case collision:
            return "collision";
        case particles:
            return "particles";
        case event:
            return "event";
        case scrollbar:
            return "scrollbar";
        case primitives:
            return "primitives";
        case exists:
            return "exists";
    }
    cerr << "Error: In " << __FUNCTION__ << ": ValueSource with code: '" << source << "' is undefined.\n"; 
    return "undefined";
}

DataType strToDataType(string dataType){
    if(dataType == "Null"){
        return null_dt;
    }
    else if(dataType == "bool"){
        return bool_inst;
    }
    else if(dataType == "boolVec"){
        return bool_vec;
    }
    else if(dataType == "int"){
        return int_inst;
    }
    else if(dataType == "intVec"){
        return int_vec;
    }
    else if(dataType == "double"){
        return double_inst;
    }
    else if(dataType == "doubleVec"){
        return double_vec;
    }
    else if(dataType == "string"){
        return string_inst;
    }
    else if(dataType == "stringVec"){
        return string_vec;
    }
    else if(dataType == "Val"){
        return value_inst;
    }
    else if(dataType == "ValVec"){
        return value_vec;
    }
    else if(dataType == "Pointer"){
        return pointer_inst;
    }
    else if(dataType == "PointerVec"){
        return pointer_vec;
    }
    else if(dataType == "Camera"){
        return camera_inst;
    }
    else if(dataType == "CameraVec"){
        return camera_vec;
    }
    else if(dataType == "Layer"){
        return layer_inst;
    }
    else if(dataType == "LayerVec"){
        return layer_vec;
    }
    else if(dataType == "Object"){
        return object_inst;
    }
    else if(dataType == "ObjectVec"){
        return object_vec;
    }
    else if(dataType == "Var"){
        return variable_mod;
    }
    else if(dataType == "VarVec"){
        return variable_mod_vec;
    }
    else if(dataType == "Vec"){
        return vector_mod;
    }
    else if(dataType == "VecVec"){
        return vector_mod_vec;
    }
    else if(dataType == "Text"){
        return text_mod;
    }
    else if(dataType == "TextVec"){
        return text_mod_vec;
    }
    else if(dataType == "EditText"){
        return editable_text_mod;
    }
    else if(dataType == "EditTextVec"){
        return editable_text_mod_vec;
    }
    else if(dataType == "SText"){
        return super_text_mod;
    }
    else if(dataType == "STextVec"){
        return super_text_mod_vec;
    }
    else if(dataType == "SEditText"){
        return super_editable_text_mod;
    }
    else if(dataType == "SEditTextVec"){
        return super_editable_text_mod_vec;
    }
    else if(dataType == "Image"){
        return image_mod;
    }
    else if(dataType == "ImageVec"){
        return image_mod_vec;
    }
    else if(dataType == "Movement"){
        return movement_mod;
    }
    else if(dataType == "MovementVec"){
        return movement_mod_vec;
    }
    else if(dataType == "Collision"){
        return collision_mod;
    }
    else if(dataType == "CollisionVec"){
        return collision_mod_vec;
    }
    else if(dataType == "Particles"){
        return particles_mod;
    }
    else if(dataType == "ParticlesVec"){
        return particles_mod_vec;
    }
    else if(dataType == "Event"){
        return event_mod;
    }
    else if(dataType == "EventVec"){
        return event_mod_vec;
    }
    else if(dataType == "Scrollbar"){
        return scrollbar_mod;
    }
    else if(dataType == "ScrollbarVec"){
        return scrollbar_mod_vec;
    }
    else if(dataType == "Primitive"){
        return primitives_mod;
    }
    else if(dataType == "PrimitiveVec"){
        return primitives_mod_vec;
    }
    else if(dataType == "Any"){
        return any_dt;
    }
    cerr << "Error: In " << __FUNCTION__ << ": DataType '" << dataType << "' is undefined.\n";
    return null_dt;
}

string dataTypeToStr(DataType dataType){
    switch (dataType){
        case null_dt:
            return "Null";
        case bool_inst:
            return "bool";
        case bool_vec:
            return "boolVec";
        case int_inst:
            return "int";
        case int_vec:
            return "intVec";
        case double_inst:
            return "double";
        case double_vec:
            return "doubleVec";
        case string_inst:
            return "string";
        case string_vec:
            return "stringVec";
        case value_inst:
            return "Val";
        case value_vec:
            return "ValVec";
        case pointer_inst:
            return "Pointer";
        case pointer_vec:
            return "PointerVec";
        case camera_inst:
            return "Camera";
        case camera_vec:
            return "CameraVec";
        case layer_inst:
            return "Layer";
        case layer_vec:
            return "LayerVec";
        case object_inst:
            return "Object";
        case object_vec:
            return "ObjectVec";
        case variable_mod:
            return "Var";
        case variable_mod_vec:
            return "VarVec";
        case vector_mod:
            return "Vec";
        case vector_mod_vec:
            return "VecVec";
        case text_mod:
            return "Text";
        case text_mod_vec:
            return "TextVec";
        case editable_text_mod:
            return "EditText";
        case editable_text_mod_vec:
            return "EditTextVec";
        case super_text_mod:
            return "SText";
        case super_text_mod_vec:
            return "STextVec";
        case super_editable_text_mod:
            return "SEditText";
        case super_editable_text_mod_vec:
            return "SEditTextVec";
        case image_mod:
            return "Image";
        case image_mod_vec:
            return "ImageVec";
        case movement_mod:
            return "Movement";
        case movement_mod_vec:
            return "MovementVec";
        case collision_mod:
            return "Collision";
        case collision_mod_vec:
            return "CollisionVec";
        case particles_mod:
            return "Particles";
        case particles_mod_vec:
            return "ParticlesVec";
        case event_mod:
            return "Event";
        case event_mod_vec:
            return "EventVec";
        case scrollbar_mod:
            return "Scrollbar";
        case scrollbar_mod_vec:
            return "ScrollbarVec";
        case primitives_mod:
            return "Primitive";
        case primitives_mod_vec:
            return "PrimitiveVec";
        case any_dt:
            return "Any";
        default:
            break;
    }
    cerr << "Error: In " << __FUNCTION__ << ": DataType with code: '" << dataType << "' is undefined.\n"; 
    return "undefined";
}

ConditionClass::ConditionClass(unsigned int newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass(string newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass() : Literal(){}

OperationClass::OperationClass(){}
inline string errorSpacing(){return "\t";}; 
inline string localContextID(const string & eventID, const string & newID){
    return eventID + /*":" +*/ newID;
}
bool checkIfVariableIsReference(const vector<StartingVariableStruct> & PassedVariables, const string & variableID){
    for(const StartingVariableStruct & Variable : PassedVariables){
        if(localContextID(Variable.eventID, Variable.id) == variableID){
            return Variable.isReference;
        }
    }
    return false;
}
bool isVariableGlobal(const vector<StartingVariableStruct> & NewVariablesForLookupTable, const string & variableID){
    if(variableID == "NULL" || variableID == "me" || variableID == "my_layer"){
        return true;
    }
    for(auto & Variable : NewVariablesForLookupTable){
        if(Variable.id == variableID){
            return Variable.eventID.size() == 0;
        }
    }
    return false;
}
string ParameterStruct::getVariableIdOrValue(){
    if(variableID != ""){
        return variableID;
    }
    switch (type){
        case 'e':
            return "NULL";
        case 'l':
            return Literal.getAnyValue();
        case 'c':
            return "(context)";
        case 'v':
            return "(vector)";
        default:
            return "(bad type)";
    }
}
bool OperationClass::addParameter(string scriptName, unsigned lineNumber, string & error, vector<WordStruct> words,
    unsigned index, char type, string name, bool optional, const vector<string> & allAvailableEventIDs,
    const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewsVariable, bool inAfterSection,
    bool ignoreUndefinedVariable
){
    auto printError = [](string scriptName, unsigned lineNumber, string instruction, std::string error){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": In the '" << instruction << "' instruction: " << error << ".\n";
    };
    error = "";
    if(index >= words.size()){
        if(optional){
            return true;
        }
        error = "For parameter '" + name + "': ";
        error += "Index " + intToStr(index+0);
        error += " is out of scope (" + intToStr(words.size());
        error += ").";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }
    if(words[index].type == 'e'){
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'e';
        ++rootParametersSize;
        return false;
    }
    if(words[index].type == 'c'){
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'c';
        if(isVariableGlobal(NewVariablesForLookupTable, words[index].value)){
            Parameters.back().variableID = words[index].value;
        }
        else{
            string temp;
            Parameters.back().variableID = findExistingVariableOrCreateNew(
                NewVariablesForLookupTable, allAvailableEventIDs, words[index].value, temp,
                canCreateNewsVariable, scriptName, lineNumber, inAfterSection, ignoreUndefinedVariable
            );
        }
        Parameters.back().negateVariable = words[index].negateVariable;
        Parameters.back().isReference = checkIfVariableIsReference(PassedVariables, Parameters.back().variableID);
        ++rootParametersSize;
        return false;
    }
    if(type == 'c'){
        error = "Parameter '" + name + "' (";
        error += intToStr(index+0) + ") must be a variable.";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }
    if(words[index].type == 's'){
        if(type != 'a' && type != 's'){
            error = "Parameter '" + name + "' (";
            error += intToStr(index+0) + ") cannot be a string.";
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'l';
        Parameters.back().Literal.setString(words[index].value);
        ++rootParametersSize;
        return false;
    }
    if(type == 's'){
        error = "Parameter '" + name + "' (";
        error += intToStr(index+0) + ") must be a string.";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }
    if(words[index].type == 'd'){
        if(type == 'i' || type == 'b'){
            error = "Parameter '" + name + "' (";
            error += intToStr(index+0) + ") cannot have a floating point.";
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'l';
        Parameters.back().Literal.setDouble(cstod(words[index].value, error));
        ++rootParametersSize;
        if(error.size() > 0){
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        return false;
    }
    if(words[index].type == 'i'){
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'l';
        Parameters.back().Literal.setInt(cstoi(words[index].value, error));
        ++rootParametersSize;
        if(error.size() > 0){
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        return false;
    }
    if(words[index].type == 'b'){
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'l';
        Parameters.back().Literal.setBool(cstoi(words[index].value, error));
        ++rootParametersSize;
        if(error.size() > 0){
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        return false;
    }
    error = "Parameter '" + name + "' (";
    error += intToStr(index+0) + ") cannot be of the type '";
    error += words[index].type + "'.";
    printError(scriptName, lineNumber, words[0].value, error);
    return true;
}
bool OperationClass::addLiteralOrVectorOrVariableToParameters(string scriptName, unsigned lineNumber, string &error,
    vector<WordStruct> words, unsigned &index, char type, string name, bool optional, const vector<string> & allAvailableEventIDs,
    const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewsVariable, bool inAfterSection
){
    auto printError = [](string scriptName, unsigned lineNumber, string instruction, std::string error){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": In the '" << instruction << "' instruction: " << error << "\n";
    };
    error = "";
    if(index >= words.size()){
        if(optional){
            return true;
        }
        error = "For parameter '" + name + "': ";
        error += "Index " + intToStr(index+0);
        error += " is out of scope (" + intToStr(words.size());
        error += ").";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }
    if(words[index].type == 'b' || words[index].type == 'i' || words[index].type == 'd'){
        if(type == 'b' || (words[index].type == 'b' && type == 'a')){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 0;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setBool(cstoi(words[index].value, error));
            ++rootParametersSize;
        }
        else if(type == 'i' || (words[index].type == 'i' && type == 'a')){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 0;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setInt(cstoi(words[index].value, error));
            ++rootParametersSize;
        }
        else if(type == 'd' || (words[index].type == 'd' && type == 'a')){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 0;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setDouble(cstod(words[index].value, error));
            ++rootParametersSize;
        }
        else{
            error = "Parameter '" + name + "' (";
            error += intToStr(index+0) + ") of the type '" + type;
            error += "' cannot be created from the value of the '" + words[index].type;
            error += "' type.";
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        if(error.size() > 0){
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        index++;
        return false;
    }
    if(words[index].type == 's'){
        if(type != 's' && type != 'a'){
            error = "Parameter '" + name + "' (";
            error += intToStr(index+0) + ") of the type '" + type;
            error += "' cannot be created from the value of the '" + words[index].type;
            error += "' type.";
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'l';
        Parameters.back().Literal.setString(words[index].value);
        ++rootParametersSize;
        index++;
        return false;
    }
    return addVectorOrVariableToParameters(scriptName, lineNumber, error, words, index,
        type, name, optional, allAvailableEventIDs, NewVariablesForLookupTable, PassedVariables,
        canCreateNewsVariable, inAfterSection
    );
}
bool OperationClass::addVectorOrVariableToParameters(string scriptName, unsigned lineNumber, string &error,
    vector<WordStruct> words, unsigned &index, char type, string name, bool optional,
    const vector<string> & allAvailableEventIDs,
    const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewsVariable, bool inAfterSection
){
    auto printError = [](string scriptName, unsigned lineNumber, string instruction, std::string error){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": In the '" << instruction << "' instruction: " << error << "\n";
    };
    error = "";
    if(index >= words.size()){
        if(optional){
            return true;
        }
        error = "For parameter '" + name + "': ";
        error += "Index " + intToStr(index+0);
        error += " is out of scope (" + intToStr(words.size());
        error += ").";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }
    if(words[index].type == 'e'){
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'e';
        ++rootParametersSize;
        index++;
        return false;
    }
    if(words[index].type != 'c'){
        error = "Parameter '" + name + "' (";
        error += intToStr(index+0) + ") must be a vector or a context. Vectors must begin and end with square brackets.";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }
    if(words[index].value != "["){
        Parameters.emplace_back(ParameterStruct());
        Parameters.back().treeLevel = 0;
        Parameters.back().type = 'c';
        if(isVariableGlobal(NewVariablesForLookupTable, words[index].value)){
            Parameters.back().variableID = words[index].value;
        }
        else{
            string temp;
            Parameters.back().variableID = findExistingVariableOrCreateNew(
                NewVariablesForLookupTable, allAvailableEventIDs, words[index].value, temp,
                canCreateNewsVariable, scriptName, lineNumber, inAfterSection
            );
        }
        Parameters.back().negateVariable = words[index].negateVariable;
        Parameters.back().isReference = checkIfVariableIsReference(PassedVariables, Parameters.back().variableID);
        ++rootParametersSize;
        index++;
        return false;
    }

    //Gather a vector
    index++;
    if(index >= words.size()){
        error = "Failed to build a vector for the parameter '" + name + "' (";
        error += intToStr(index+0) + ").";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }

    Parameters.emplace_back(ParameterStruct());
    Parameters.back().treeLevel = 0;
    Parameters.back().type = 'v';
    ++rootParametersSize;
    while(index < words.size() && words[index].value != "]"){
        if(words[index].type == 'c'){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 1;
            Parameters.back().type = 'c';
            if(isVariableGlobal(NewVariablesForLookupTable, words[index].value)){
                Parameters.back().variableID = words[index].value;
            }
            else{
                string temp;
                Parameters.back().variableID = findExistingVariableOrCreateNew(
                    NewVariablesForLookupTable, allAvailableEventIDs, words[index].value, temp,
                    canCreateNewsVariable, scriptName, lineNumber, inAfterSection
                );
            }
            Parameters.back().negateVariable = words[index].negateVariable;
            Parameters.back().isReference = checkIfVariableIsReference(PassedVariables, Parameters.back().variableID);
            index++;
            continue;
        }
        if(type == 'c'){
            error = "Parameter '" + name + "' (";
            error += intToStr(index+0) + ") must be a variable.";
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        if(words[index].type == 's'){
            if(type != 'a' && type != 's'){
                error = "Parameter '" + name + "' (";
                error += intToStr(index+0) + ") cannot be a string.";
                printError(scriptName, lineNumber, words[0].value, error);
                return true;
            }
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 1;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setString(words[index].value);
            index++;
            continue;
        }
        if(type == 's'){
            error = "Parameter '" + name + "' (";
            error += intToStr(index+0) + ") must be a string.";
            printError(scriptName, lineNumber, words[0].value, error);
            return true;
        }
        if(words[index].type == 'd'){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 1;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setDouble(cstod(words[index].value, error));
            if(error.size() > 0){
                printError(scriptName, lineNumber, words[0].value, error);
                return true;
            }
            index++;
            continue;
        }
        if(words[index].type == 'i'){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 1;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setInt(cstoi(words[index].value, error));
            if(error.size() > 0){
                printError(scriptName, lineNumber, words[0].value, error);
                return true;
            }
            index++;
            continue;
        }
        if(words[index].type == 'b'){
            Parameters.emplace_back(ParameterStruct());
            Parameters.back().treeLevel = 1;
            Parameters.back().type = 'l';
            Parameters.back().Literal.setBool(cstoi(words[index].value, error));
            if(error.size() > 0){
                printError(scriptName, lineNumber, words[0].value, error);
                return true;
            }
            index++;
            continue;
        }
        error = "Parameter '" + name + "' (";
        error += intToStr(index+0) + ") cannot be of the type '";
        error += words[index].type + "'.";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }

    if(index >= words.size()){
        error = "There is no closing square bracket in the parameter '" + name + "' (";
        error += intToStr(index+0) + ").";
        printError(scriptName, lineNumber, words[0].value, error);
        return true;
    }

    index++;
    return false;
}
void OperationClass::addLiteralParameter(const VariableModule & Variable){
    Parameters.emplace_back(ParameterStruct());
    Parameters.back().treeLevel = 0;
    Parameters.back().type = 'l';
    Parameters.back().Literal = Variable;
    ++rootParametersSize;
}

void EventModule::clone(const EventModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}

void EventModule::setUpNewInstance(){
    conditionalStatus = 'n';
    elseChildID = "";
    programCounter = 0;
    elseChildFinished = false;
    loop = false;
    willBeDeleted = false;

    goToEndOfIfStatement.clear();
    conditionalStatus = 'n';
    breakFromCurrentLoop = false;
    decrementProgramCounter = false;
}
EventModule::EventModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
EventModule::EventModule(unsigned int eventModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(eventModuleID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
EventModule::EventModule(string eventModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(eventModuleID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
EventModule::~EventModule(){

}
void EventModule::clear(){
    ConditionalChain.clear();
    PostOperations.clear();
    Children.clear();
}
void EventModule::resetStateVariables(){
    programCounter = 0;
    goToEndOfIfStatement.clear();
    conditionalStatus = 'n';
    breakFromCurrentLoop = false;
    decrementProgramCounter = false;
}
DataType strToDataTypeWithoutPrimaryTypes(string dataType){
    if(dataType == "bool" || dataType == "int" || dataType == "double" || dataType == "string"){
        return value_inst;
    }
    if(dataType == "boolVec" || dataType == "intVec" || dataType == "doubleVec" || dataType == "stringVec"){
        return value_vec;
    }
    return strToDataType(dataType);
}
bool EventModule::getPassedVariables(const vector<WordStruct> & words, unsigned & cursor,
    const unsigned & lineNumber, const string & scriptName, vector<StartingVariableStruct> & NewVariablesForLookupTable
){
    if(cursor >= words.size()){
        return false;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return false;
    }

    if(words[cursor].value != "("){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Passed parameters must be enclosed in parentheses.\n";
        return true;
    }
    
    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
        return true;
    }
    
    short variableIndex = 0;

    while(words[cursor].value != ")"){
        // [')'], [type, name, ')'], [type, '&', name, ')'], [type, name, ','], [type, '&', name, ',']
        if(cursor >= words.size()){ //[')'], [type]
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
            return true;
        }
        if(cursor + 1 >= words.size()){ //[name], ['&']
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Passed variable requires a name.\n";
            return true;
        }
        if(cursor + 2 >= words.size()){ //[')'], [name], [',']
            if(words[cursor + 1].type == 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
            }
            else if(words[cursor + 1].value == "&"){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Passed by reference variable requires a name.\n";
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Token '"
                    << words[cursor + 1].value << "' of the type '" << words[cursor + 1].type
                    << "' is not valid.\n";
            }
            return true;
        }
        
        bool isReference = false;
        if(words[cursor + 1].value == "&"){
            isReference = true;
        }

        if(words[cursor + 2 + isReference].value != ")"){ //[')'], [',']
            if(words[cursor + 2 + isReference].value != ","){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Variables must be divided by commas.\n";
                return true;
            }
            if(cursor + 3 + isReference >= words.size()){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
                return true;
            }
        }
        
        DataType variableType = strToDataTypeWithoutPrimaryTypes(words[cursor].value);
        string variableID = words[cursor + 1 + isReference].value;
        cursor += 2 + isReference;

        if(words[cursor].value == ","){
            cursor++;
        }

        PassedVariables.emplace_back(variableType, variableID, variableID, variableIndex, ID, isReference);
        NewVariablesForLookupTable.emplace_back(variableType, localContextID(ID, variableID), variableID, variableIndex, ID, isReference);
        ++variableIndex;
    }
    cursor++;
    return false;
}
string findExistingVariableOrCreateNew(const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<string> & allAvailableEventIDs, const string & variableID, string & usedEventID,
    bool canCreateNewsVariable, const string & scriptName, const unsigned &lineNumber, bool inAfterSection,
    bool ignoreUndefinedVariable
){
    unsigned eventIdx = inAfterSection && allAvailableEventIDs.size() > 1;
    for(; eventIdx < allAvailableEventIDs.size(); ++eventIdx){
        string localName = localContextID(allAvailableEventIDs[eventIdx], variableID);
        for(const StartingVariableStruct & ExistingVariable : NewVariablesForLookupTable){
            if(localName == ExistingVariable.id/*localContextID(ExistingVariable.eventID, variableID)*/){
                usedEventID = ExistingVariable.eventID;
                return localName/*localContextID(ExistingVariable.eventID, variableID)*/;
            }
        }
    }
    if(!canCreateNewsVariable){
        !ignoreUndefinedVariable && cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Variable '" << variableID << "' is undefined.\n";
        return variableID;
    }
    usedEventID = allAvailableEventIDs[0];
    return localContextID(allAvailableEventIDs[0], variableID);
}
//Passing variables to the event and creating expressions require a non-standard variable syntax (adding "&" to the beginning of referenced variables).
string createCustomOutput(const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, const vector<string> & allAvailableEventIDs,
    const string & variableID, bool canBeReferenced, bool canCreateNewsVariable,
    const string & scriptName, const unsigned &lineNumber, bool inAfterSection
){
    string customID = "";
    if(isVariableGlobal(NewVariablesForLookupTable, variableID)){ //If the current variable is global, use it.
        customID = variableID;
    }
    else{ //If the variable is local, first check if it exists in the available scopes. If not, create a new one for the current scope.
        string temp;
        customID = findExistingVariableOrCreateNew(
            NewVariablesForLookupTable, allAvailableEventIDs, variableID, temp,
            canCreateNewsVariable, scriptName, lineNumber, inAfterSection
        );
    }
    if(canBeReferenced && checkIfVariableIsReference(PassedVariables, customID)){
        customID = "&" + customID;
    }
    return customID;
}
TriggerType transStringToTrigger(const string &trigger){
    if(trigger == "on_boot") return on_boot;
    else if(trigger == "on_init") return on_init;
    else if(trigger == "each_iteration") return each_iteration;
    else if(trigger == "each_second") return each_second;
    else if(trigger == "on_key_press") return on_key_press;
    else if(trigger == "on_key_pressing") return on_key_pressing;
    else if(trigger == "on_key_release") return on_key_release;
    else if(trigger == "on_mouse_move") return on_mouse_move;
    else if(trigger == "when_mouse_still") return when_mouse_still;
    else if(trigger == "on_mouse_press") return on_mouse_press;
    else if(trigger == "on_mouse_pressing") return on_mouse_pressing;
    else if(trigger == "on_mouse_release") return on_mouse_release;
    else if(trigger == "by_objects") return by_objects;
    else if(trigger == "by_variables") return by_variables;
    else if(trigger == "by_collision") return by_collision;
    else if(trigger == "by_editables") return by_editables;
    else if(trigger == "by_movement") return by_movement;
    else if(trigger == "when_stillness") return by_stillness;
    else if(trigger == "on_display_resize") return on_display_resize;
    return null_t;
}
string transTriggerToString(const TriggerType &trigger){
    switch(trigger){
        case null_t: return "null";
        case on_boot: return "on_boot";
        case on_init: return "on_init";
        case each_iteration: return "each_iteration";
        case each_second: return "each_second";
        case on_key_press: return "on_key_press";
        case on_key_pressing: return "on_key_pressing";
        case on_key_release: return "on_key_release";
        case on_mouse_move: return "on_mouse_move";
        case when_mouse_still: return "when_mouse_still";
        case on_mouse_press: return "on_mouse_press";
        case on_mouse_pressing: return "on_mouse_pressing";
        case on_mouse_release: return "on_mouse_release";
        case by_objects: return "by_objects";
        case by_variables: return "by_variables";
        case by_collision: return "by_collision";
        case by_editables: return "by_editables";
        case by_movement: return "by_movement";
        case by_stillness: return "when_stillness";
        case on_display_resize: return "on_display_resize";
        default:
            cerr << "Error: In " << __FUNCTION__
                << ": TriggerType with code: '" << trigger << "' is undefined.\n"; 
            return "undefined";
    }
}
bool EventModule::getPassingVariables(vector<string> &passingVariables,
    const vector<WordStruct> &words, unsigned &cursor,
    const unsigned &lineNumber, const string &scriptName,
    const vector<StartingVariableStruct> &NewVariablesForLookupTable,
    const vector<string> &allAvailableEventIDs
){
    if(cursor >= words.size()){
        return false;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return false;
    }

    if(words[cursor].value != "("){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Passing parameters must be enclosed in parentheses.\n";
        return true;
    }
    
    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
        return true;
    }
    
    while(words[cursor].value != ")"){
        // [')'], [name, ')'], [name, ',']
        // [')'], [name]
        if(words[cursor].type != 'c'){ //[name]
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Parameter " << cursor+1 << " must be a context.\n";
            return true;
        }
        if(cursor + 1 >= words.size()){ //[')'], [',']
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
            return true;
        }
        if(words[cursor + 1].value != ")"){
            if(words[cursor + 1].value != ","){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Variables must be divided by commas.\n";
                return true;
            }
            if(cursor + 2 >= words.size()){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Parentheses were not closed.\n";
                return true;
            }
        }
        passingVariables.push_back(createCustomOutput(
            NewVariablesForLookupTable, PassedVariables, allAvailableEventIDs,
            words[cursor].value, true, false, scriptName, lineNumber, false
        ));
        if(words[cursor + 1].value == ","){
            cursor += 2;
        }
        else{
            cursor++;
        }
    }
    cursor++;
    return false;
}

void EventModule::controlText(TextModule *Text, AttributeType attribute, const vector<VariableModule> &Values, vector<string> &IDs, const vector<SingleFont> &FontContainer){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                break;
            }
            Text->setID(Values[0].getStringUnsafe(), IDs);
            break;
        case set_pos:
            if(Values.size() < 2){
                break;
            }
            Text->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            break;
        case set_size:
            if(Values.size() < 2){
                break;
            }
            Text->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            break;
        case set_scale:
            if(Values.size() < 2){
                break;
            }
            Text->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            break;
        case add_scale:
            if(Values.size() < 2){
                break;
            }
            Text->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            break;
        case set_color:
            if(Values.size() < 4){
                break;
            }
            Text->setColors(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
            break;
        case set_color_r:
            if(Values.size() < 1){
                break;
            }
            Text->color.r = Values[0].getDoubleUnsafe();
            break;
        case set_color_g:
            if(Values.size() < 1){
                break;
            }
            Text->color.g = Values[0].getDoubleUnsafe();
            break;
        case set_color_b:
            if(Values.size() < 1){
                break;
            }
            Text->color.b = Values[0].getDoubleUnsafe();
            break;
        case set_color_a:
            if(Values.size() < 1){
                break;
            }
            Text->color.a = Values[0].getDoubleUnsafe();
            break;
        case set_random_color:
            Text->setRandomColors();
            break;
        case increment_random_color:
            Text->incrementRandomColor();
            break;
        case set_rotation:
            if(Values.size() < 1){
                break;
            }
            Text->setRotation(Values[0].getDoubleUnsafe());
            break;
        case set_random_change_speed:
            if(Values.size() < 1){
                break;
            }
            Text->randomChangeSpeed = Values[0].getDoubleUnsafe();
            break;
        case set_min_color:
            if(Values.size() < 1){
                break;
            }
            Text->minColorValue = Values[0].getDoubleUnsafe();
            break;
        case set_max_color:
            if(Values.size() < 1){
                break;
            }
            Text->maxColorValue = Values[0].getDoubleUnsafe();
            break;
        case rotate:
            if(Values.size() < 1){
                break;
            }
            Text->addRotation(Values[0].getDoubleUnsafe());
            break;
        case set_font:
            if(Values.size() == 0){
                break;
            }
            Text->setFontID(Values[0].getStringUnsafe());
            break;
        case add_new_content:
            if(Values.size() == 0){
                break;
            }
            Text->addNewContent(Values[0].getStringUnsafe());
            break;
        case add_new_and_update:
            if(Values.size() == 0){
                break;
            }
            Text->addNewContentAndResize(Values[0].getStringUnsafe(), FontContainer);
            break;
        case update_size:
            Text->fitSizeToText(FontContainer);
            break;
        case select_content:
            if(Values.size() == 0){
                break;
            }
            Text->chooseContent(Values[0].getIntUnsafe());
            break;
        case delete_a:
            if(Values.size() == 0){
                break;
            }
            Text->deleteContent(Values[0].getIntUnsafe());
            break;
        case add_text:
            if(Values.size() < 2){
                break;
            }
            Text->addTextToContent(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
            break;
        case set_text:
            if(Values.size() < 2){
                break;
            }
            Text->modifyContent(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
            break;
        case set_resize_text:
            if(Values.size() < 2){
                break;
            }
            Text->modifyContentAndResize(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
            break;
        case set_wrapping:
            if(Values.size() == 0){
                break;
            }
            Text->setWrapping(Values[0].getIntUnsafe());
            break;
        case set_horizontal_align:
            if(Values.size() == 0){
                break;
            }
            Text->setHorizontalAlign(Values[0].getIntUnsafe());
            break;
        case set_vertical_align:
            if(Values.size() == 0){
                break;
            }
            Text->setVerticalAlign(Values[0].getIntUnsafe());
            break;
        case set_cursor_pos:
            if(Values.size() == 0){
                break;
            }
            Text->setCursorPos(Values[0].getIntUnsafe());
            break;
        case set_second_cursor_pos:
            if(Values.size() == 0){
                break;
            }
            Text->setSecondCursorPos(Values[0].getIntUnsafe());
            break;
        default:
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Text->control(attribute, temp, Values.size());
            return;
    }
}
void EventModule::controlEditableText(EditableTextModule *EditableText, AttributeType attribute, const vector<VariableModule> &Values, vector<string> &IDs, const vector<SingleFont> &FontContainer){
    switch(attribute){
        case set_editable:
            if(Values.size() == 0){
                return;
            }
            EditableText->setCanBeEdited(Values[0].getBoolUnsafe());
            return;
        case set_space_use:
            if(Values.size() == 0){
                return;
            }
            EditableText->setCanUseSpace(Values[0].getBoolUnsafe());
            return;
        case set_enter_use:
            if(Values.size() == 0){
                return;
            }
            EditableText->setCanUseEnter(Values[0].getBoolUnsafe());
            return;
        case set_can_enter_accept:
            if(Values.size() == 0){
                return;
            }
            EditableText->setCanEnterAcceptChanges(Values[0].getBoolUnsafe());
            return;
        case set_numerical:
            if(Values.size() == 0){
                return;
            }
            EditableText->setIsNumerical(Values[0].getBoolUnsafe());
            return;
        case set_floating_point:
            if(Values.size() == 0){
                return;
            }
            EditableText->setHasFloatingPoint(Values[0].getBoolUnsafe());
            return;
        case set_can_update_variable:
            if(Values.size() == 0){
                return;
            }
            EditableText->setUpdateConnectedVariable(Values[0].getBoolUnsafe());
            return;
        case set_auto_cleaning:
            if(Values.size() == 0){
                return;
            }
            EditableText->setCanClearContentAfterSuccess(Values[0].getBoolUnsafe());
            return;
        case set_use_tabs:
            if(Values.size() == 0){
                return;
            }
            EditableText->setUseTabs(Values[0].getBoolUnsafe());
            return;
        case set_min_content_length:
            if(Values.size() == 0){
                return;
            }
            EditableText->setMinContentSize(Values[0].getIntUnsafe());
            return;
        case set_max_content_length:
            if(Values.size() == 0){
                return;
            }
            EditableText->setMaxContentSize(Values[0].getIntUnsafe());
            return;
        case set_input_delay:
            if(Values.size() == 0){
                return;
            }
            EditableText->setInputDelay(Values[0].getDoubleUnsafe());
            return;
        case set_repetition_delay:
            if(Values.size() == 0){
                return;
            }
            EditableText->setRepetitionDelay(Values[0].getDoubleUnsafe());
            return;
        case connect_object:
            if(Values.size() == 0){
                return;
            }
            EditableText->connectedObject = Values[0].getStringUnsafe();
            return;
        case connect_group:
            if(Values.size() == 0){
                return;
            }
            EditableText->connectedGroup = Values[0].getStringUnsafe();
            return;
        case connect_module:
            if(Values.size() == 0){
                return;
            }
            EditableText->connectedModule = Values[0].getStringUnsafe();
            return;
        case connect_module_id:
            if(Values.size() == 0){
                return;
            }
            EditableText->connectedModuleID = Values[0].getStringUnsafe();
            return;
        case connect_variable:
            if(Values.size() == 0){
                return;
            }
            EditableText->connectedVariable = Values[0].getStringUnsafe();
            return;
        case set_protected_area:
            if(Values.size() == 0){
                return;
            }
            EditableText->setProtectedArea(Values[0].getIntUnsafe());
            return;
        case cut_unprotected_area:
            EditableText->cutCurrentContent(EditableText->getProtectedArea());
            return;
        case stop_editing:
            EditableText->setEditingIsActive(false);
            return;
        case start_editing:
            EditableText->setEditingIsActive(true);
            return;
        case set_ignore_vertical_arrows:
            if(Values.size() == 0){
                return;
            }
            EditableText->ignoreVerticalArrows = Values[0].getBoolUnsafe();
            return;
        case set_can_auto_resize_axis_y:
            if(Values.size() == 0){
                return;
            }
            EditableText->setAutoResizeAxisY(Values[0].getBoolUnsafe());
            return;
        default:
            controlText(EditableText, attribute, Values, IDs, FontContainer);
            return;
    }
}
void EventModule::controlSuperText(SuperTextModule * SuperText, AttributeType attribute, const vector<VariableModule> & Values,
    vector <string> & IDs, vector<SingleFont> & FontContainer, string EXE_PATH, string workingDirectory
){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            }
            SuperText->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_pos:
            if(Values.size() < 2){
                return;
            }
            SuperText->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_size:
            if(Values.size() < 2){
                return;
            }
            SuperText->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_scale:
            if(Values.size() < 2){
                return;
            }
            SuperText->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case add_scale:
            if(Values.size() < 2){
                return;
            }
            SuperText->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_cursor_pos:
            if(Values.size() == 0){
                return;
            }
            SuperText->setCursorPos(Values[0].getIntUnsafe());
            return;
        case set_second_cursor_pos:
            if(Values.size() == 0){
                return;
            }
            SuperText->setSecondCursorPos(Values[0].getIntUnsafe());
            return;
        case update:
            SuperText->update();
            return;
        case crop_size_to_text:
            SuperText->cropSizeToText();
            return;
        case crop_width_to_text:
            SuperText->cropWidthToText();
            return;
        case crop_height_to_text:
            SuperText->cropHeightToText();
            return;
        case set_content:
            if(Values.size() < 1){
                return;
            }
            SuperText->setContent(Values[0].getStringUnsafe());
            return;
        case add_content:
            if(Values.size() < 1){
                return;
            }
            SuperText->addContent(Values[0].getStringUnsafe());
            return;
        case add_new_text_line:
            if(Values.size() < 1){
                return;
            }
            SuperText->addNewTextLine(Values[0].getStringUnsafe());
            return;
        case set_text_line:
            if(Values.size() < 2){
                return;
            }
            SuperText->setTextLine(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
            return;
        case add_to_text_line:
            if(Values.size() < 2){
                return;
            }
            SuperText->addToTextLine(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
            return;
        case save_to_file:
        case load_from_file:{
            if(Values.size() < 1){
                return;
            }
            string fileName = Values[0].getStringUnsafe();
            if(fileName == ""){
                std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': File name is empty.\n";
                return;
            }
            string finalPath = "";
            if(fileName.substr(0, 2) == "~/"){
                finalPath = EXE_PATH + fileName.substr(2, fileName.size()-2);
            }
            else{
                finalPath = EXE_PATH + workingDirectory + fileName;
            }
            if(attribute == save_to_file){
                SuperText->saveFormattedTextToTheFile(finalPath);
            }
            else{
                SuperText->loadFormattedTextFromTheFile(finalPath, FontContainer);
            }
            return;
        }
        case add_format:
            if(Values.size() >= 13){
                SuperText->addFormat(al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
                    al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()),
                    Values[8].getStringUnsafe(), FontContainer, Values[9].getIntUnsafe(), Values[10].getIntUnsafe(), Values[11].getIntUnsafe(),
                    Values[12].getIntUnsafe()
                );
                return;
            }
            if(Values.size() >= 6){
                SuperText->addFormat(al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
                    al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
                    Values[4].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[5].getIntUnsafe()
                );
                return;
            }
            if(Values.size() >= 2){
                SuperText->addFormat(al_map_rgba_f(0.0, 0.0, 0.0, 1.0), al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
                    Values[0].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[1].getIntUnsafe()
                );
                return;
            }
            return;
        case modify_format:
            if(Values.size() < 14){
                return;
            }
            SuperText->modifyFormat(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(),
                Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()), al_map_rgba_f(Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(),
                Values[7].getDoubleUnsafe(), Values[8].getDoubleUnsafe()), Values[9].getStringUnsafe(), FontContainer, Values[10].getIntUnsafe(),
                Values[11].getIntUnsafe(), Values[12].getBoolUnsafe(), Values[13].getIntUnsafe()
            );
            return;
        case modify_last_format:
            if(Values.size() >= 13){
                SuperText->modifyFormat(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(),
                    Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()), al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(),
                    Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()), Values[8].getStringUnsafe(), FontContainer, Values[9].getIntUnsafe(),
                    Values[10].getIntUnsafe(), Values[11].getBoolUnsafe(), Values[12].getIntUnsafe()
                );
                return;
            }
            if(Values.size() >= 10){
                SuperText->modifyFormat(SuperText->Formatting.size() - 1,
                    al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
                    al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()),
                    Values[8].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[9].getIntUnsafe()
                );
                return;
            }
            return;
        case inject_format:
            if(Values.size() >= 14){
                SuperText->injectFormat(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), al_map_rgba_f(Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(),
                    Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe()), al_map_rgba_f(Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe(),
                    Values[8].getDoubleUnsafe(), Values[9].getDoubleUnsafe()), Values[10].getStringUnsafe(), FontContainer, Values[11].getIntUnsafe(),
                    Values[12].getIntUnsafe(), Values[13].getBoolUnsafe()
                );
                return;
            }
            if(Values.size() >= 11){
                SuperText->injectFormat(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(),
                    al_map_rgba_f(Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe()),
                    al_map_rgba_f(Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe(), Values[8].getDoubleUnsafe(), Values[9].getDoubleUnsafe()),
                    Values[10].getStringUnsafe(), FontContainer, 0.0, 0.0, false
                );
            }
            return;
        case delete_format:
            if(Values.size() < 1){
                return;
            }
            SuperText->deleteFormat(Values[0].getIntUnsafe());
            return;
        case set_color:
            if(Values.size() < 5){
                return;
            }
            SuperText->setColor(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(),
                Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()
            ));
            return;
        case set_last_color:
            if(Values.size() < 4){
                return;
            }
            SuperText->setColor(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(),
                Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()
            ));
            return;
        case set_accent_color:
            if(Values.size() < 5){
                return;
            }
            SuperText->setAccentColor(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(),
                Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()
            ));
            return;
        case set_last_accent_color:
            if(Values.size() < 4){
                return;
            }
            SuperText->setAccentColor(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(),
                Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()
            ));
            return;
        case set_font:
            if(Values.size() < 2){
                return;
            }
            SuperText->setFont(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
            return;
        case set_offset:
            if(Values.size() < 3){
                return;
            }
            SuperText->setOffset(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
            return;
        case add_offset:
            if(Values.size() < 3){
                return;
            }
            SuperText->addOffset(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
            return;
        case set_offset_x:
            if(Values.size() < 2){
                return;
            }
            SuperText->setOffsetX(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            return;
        case add_offset_x:
            if(Values.size() < 2){
                return;
            }
            SuperText->addOffsetX(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            return;
        case set_offset_y:
            if(Values.size() < 2){
                return;
            }
            SuperText->setOffsetY(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            return;
        case add_offset_y:
            if(Values.size() < 2){
                return;
            }
            SuperText->addOffsetY(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            return;
        case set_selected:
            if(Values.size() < 2){
                return;
            }
            SuperText->setSelected(Values[0].getIntUnsafe(), Values[1].getBoolUnsafe());
            return;
        case set_limit:
            if(Values.size() < 2){
                return;
            }
            SuperText->setLimit(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            return;
        case set_wrapping:
            if(Values.size() < 1){
                return;
            }
            if(Values[0].getStringUnsafe().size() > 0){
                SuperText->setWrapping(Values[0].getStringUnsafe()[0]);
            }
            else{
                std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
            }
            return;
        case set_horizontal_align:
            if(Values.size() < 1){
                return;
            }
            if(Values[0].getStringUnsafe().size() > 0){
                SuperText->setHorizontalAlign(Values[0].getStringUnsafe()[0]);
            }
            else{
                std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
            }
            return;
        case set_vertical_align:
            if(Values.size() < 1){
                return;
            }
            if(Values[0].getStringUnsafe().size() > 0){
                SuperText->setVerticalAlign(Values[0].getStringUnsafe()[0]);
            }
            else{
                std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
            }
            return;
        case set_padding_between_lines:
            if(Values.size() < 1){
                return;
            }
            SuperText->setPaddingBetweenLines(Values[0].getDoubleUnsafe());
            return;
        case set_rotation:
            if(Values.size() < 1){
                return;
            }
            SuperText->setRotation(Values[0].getDoubleUnsafe());
            return;
        case add_rotation:
            if(Values.size() < 1){
                return;
            }
            SuperText->addRotation(Values[0].getDoubleUnsafe());
            return;
        case set_tab_length:
            if(Values.size() < 1){
                return;
            }
            SuperText->setTabLength(Values[0].getIntUnsafe());
            return;
        case set_draw_selection_first:
            if(Values.size() < 1){
                return;
            }
            SuperText->drawSelectionFirst = Values[0].getBoolUnsafe();
            return;
        default:{
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            SuperText->control(attribute, temp, Values.size());
            return;
        }
    }
}
void EventModule::controlSuperEditableText(SuperEditableTextModule *& SuperEditableText, AttributeType attribute, const vector<VariableModule> & Values,
    vector <string> & IDs, vector<SingleFont> & FontContainer, SuperEditableTextModule *& ActiveEditableText, string EXE_PATH, string workingDirectory
){
    switch(attribute){
        case set_can_be_edited:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->canBeEdited = Values[0].getBoolUnsafe();
            return;
        case set_can_use_space:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->canUseSpace = Values[0].getBoolUnsafe();
            return;
        case set_can_use_enter:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->canUseEnter = Values[0].getBoolUnsafe();
            return;
        case set_can_use_tabs:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->canUseTabs = Values[0].getBoolUnsafe();
            return;
        case set_is_numerical:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->isNumerical = Values[0].getBoolUnsafe();
            return;
        case set_has_floating_point:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->hasFloatingPoint = Values[0].getBoolUnsafe();
            return;
        case set_ignore_vertical_arrows:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->ignoreVerticalArrows = Values[0].getBoolUnsafe();
            return;
        case set_ignore_content_restriction:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->ignoreContentRestriction = Values[0].getBoolUnsafe();
            return;
        case set_is_storing_history:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->isStoringHistory = Values[0].getBoolUnsafe();
            return;
        case set_can_copy_format:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->canCopyFormat = Values[0].getBoolUnsafe();
            return;
        case start_editing:
            if(ActiveEditableText != nullptr){
                ActiveEditableText->isEditingActive = false;
                for(FormatClass & Format : ActiveEditableText->Formatting){
                    Format.selected = false;
                }
                ActiveEditableText->update();
            }
            ActiveEditableText = SuperEditableText;
            SuperEditableText->isEditingActive = true;
            SuperEditableText->divideFormattingByCursor();
            return;
        case stop_editing:
            SuperEditableText->isEditingActive = false;
            for(FormatClass & Format : SuperEditableText->Formatting){
                Format.selected = false;
            }
            SuperEditableText->update();

            if(ActiveEditableText == nullptr){
                return;
            }
            if(ActiveEditableText == SuperEditableText){
                ActiveEditableText = nullptr;
            }
            return;
        case set_min_content_length:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->minContentLength = Values[0].getIntUnsafe();
            return;
        case set_max_content_length:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->maxContentLength = Values[0].getIntUnsafe();
            return;
        case set_input_delay:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->inputDelay = Values[0].getDoubleUnsafe();
            return;
        case set_repetition_delay:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->repetitionDelay = Values[0].getDoubleUnsafe();
            return;
        case set_protected_area:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->protectedArea = Values[0].getIntUnsafe();
            return;
        case cut_unprotected_area:
            SuperEditableText->cutContent(SuperEditableText->protectedArea);
            return;
        case set_cursor_pos:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->setCursorPos(Values[0].getIntUnsafe());
            return;
        case set_second_cursor_pos:
            if(Values.size() == 0){
                return;
            }
            SuperEditableText->setSecondCursorPos(Values[0].getIntUnsafe());
            return;
        case deactivate_a:
            SuperEditableText->isEditingActive = false;
            for(FormatClass & Format : SuperEditableText->Formatting){
                Format.selected = false;
            }
            SuperEditableText->update();
            SuperEditableText->deactivate();

            if(ActiveEditableText == nullptr){
                return;
            }
            if(ActiveEditableText == SuperEditableText){
                ActiveEditableText = nullptr;
            }
            return;
        default:
            controlSuperText(SuperEditableText, attribute, Values, IDs, FontContainer, EXE_PATH, workingDirectory);
            return;
    }
}
void EventModule::controlImage(ImageModule *Image, AttributeType attribute, const vector<VariableModule> &Values,
    vector<string> &IDs, vector<SingleBitmap> &BitmapContainer, string workingDirectory
){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Image->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_pos:
            if(Values.size() < 2){
                return;
            }
            Image->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_size:
            if(Values.size() < 2){
                return;
            }
            Image->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_scale:
            if(Values.size() < 2){
                return;
            }
            Image->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case AttributeType::resize:
            if(Values.size() < 2){
                return;
            }
            Image->resize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_rotation:
            if(Values.size() < 1){
                return;
            }
            Image->setRotation(Values[0].getDoubleUnsafe());
            return;
        case rotate:
            if(Values.size() < 1){
                return;
            }
            Image->addRotation(Values[0].getDoubleUnsafe());
            return;
        case set_mirror:
            if(Values.size() < 2){
                return;
            }
            Image->setMirror(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_color:{
            if(Values.size() < 4){
                return;
            }
            float arr[] = {float(Values[0].getDoubleUnsafe()), float(Values[1].getDoubleUnsafe()), float(Values[2].getDoubleUnsafe()), float(Values[3].getDoubleUnsafe())};
            Image->setImageColor(arr);
            return;
        }
        case set_color_r:
            if(Values.size() < 1){
                return;
            }
            Image->setImageColor(Values[0].getDoubleUnsafe(), 'r');
            return;
        case set_color_g:
            if(Values.size() < 1){
                return;
            }
            Image->setImageColor(Values[0].getDoubleUnsafe(), 'g');
            return;
        case set_color_b:
            if(Values.size() < 1){
                return;
            }
            Image->setImageColor(Values[0].getDoubleUnsafe(), 'b');
            return;
        case set_color_a:
            if(Values.size() < 1){
                return;
            }
            Image->setImageColor(Values[0].getDoubleUnsafe(), 'a');
            return;
        case set_light:
            if(Values.size() < 4){
                return;
            }
            Image->setLightColor(vec3d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe()), Values[3].getDoubleUnsafe());
            return;
        case connect_bitmap:
            if(Values.size() == 0){
                return;
            }
            if(Values.size() == 1){
                Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", workingDirectory);
            }
            else{
                Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), Values[1].getStringUnsafe(), workingDirectory);
            }   
            return;
        case connect_bitmap_via_path:
            if(Values.size() == 0){
                return;
            }
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", workingDirectory);  
            return;
        case connect_bitmap_via_alias:
            if(Values.size() == 0){
                return;
            }
            Image->connectBitmap(BitmapContainer, "", Values[0].getStringUnsafe(), workingDirectory);  
            return;
        default:{
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Image->control(attribute, temp, Values.size());
            return;
        }
    }
}
void EventModule::controlMovement(MovementModule * Movement, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case move_a:
            if(Values.size() < 2){
                return;
            }
            Movement->addMomentum(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case move_up:
            Movement->setNextMove(true, false, false, false, false, false, false);
            return;
        case move_right:
            Movement->setNextMove(false, true, false, false, false, false, false);
            return;
        case move_down:
            Movement->setNextMove(false, false, true, false, false, false, false);
            return;
        case move_left:
            Movement->setNextMove(false, false, false, true, false, false, false);
            return;
        case move_jump:
            Movement->setNextMove(false, false, false, false, true, false, false);
            return;
        case move_fall:
            Movement->setNextMove(false, false, false, false, false, true, false);
            return;
        case move_run:
            Movement->setNextMove(false, false, false, false, false, false, true);
            return;
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Movement->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_movement_type:
            if(Values.size() == 0){
                return;
            }
            Movement->setMovementType(Values[0].getIntUnsafe());
            return;
        case set_input_type:
            if(Values.size() == 0){
                return;
            }
            Movement->setInputType(Values[0].getIntUnsafe());
            return;
        case set_allowed_jumps:
            if(Values.size() == 0){
                return;
            }
            Movement->setAllowedJumps(Values[0].getIntUnsafe());
            return;
        case set_jump_cooldown:
            if(Values.size() == 0){
                return;
            }
            Movement->setJumpCooldownDuration(Values[0].getDoubleUnsafe());
            return;
        case set_move_cooldown:
            if(Values.size() == 0){
                return;
            }
            Movement->setMoveCooldownDuration(Values[0].getDoubleUnsafe());
            return;
        case set_diagonal_movement:
            if(Values.size() == 0){
                return;
            }
            Movement->setCanMoveDiagonally(Values[0].getBoolUnsafe());
            return;
        case set_reset_momentum_when_jumping:
            if(Values.size() == 0){
                return;
            }
            Movement->setResetMomentumWhenJumping(Values[0].getBoolUnsafe());
            return;
        case set_mass:
            if(Values.size() == 0){
                return;
            }
            Movement->setBodyMass(Values[0].getDoubleUnsafe());
            return;
        case set_walking_speed:
            if(Values.size() == 0){
                return;
            }
            Movement->setWalkingSpeed(Values[0].getDoubleUnsafe());
            return;
        case set_running_speed:
            if(Values.size() == 0){
                return;
            }
            Movement->setRunningSpeed(Values[0].getDoubleUnsafe());
            return;
        case set_jump_speed:
            if(Values.size() == 0){
                return;
            }
            Movement->setJumpSpeed(Values[0].getDoubleUnsafe());
            return;
        case set_gravity:
            if(Values.size() == 0){
                return;
            }
            Movement->setGravitation(Values[0].getDoubleUnsafe());
            return;
        case set_friction:
            if(Values.size() == 0){
                return;
            }
            Movement->setBaseFriction(Values[0].getDoubleUnsafe());
            return;
        case set_max_momentum_x:
            if(Values.size() == 0){
                return;
            }
            Movement->setMaxMomentumX(Values[0].getDoubleUnsafe());
            return;
        case set_momentum_y:
            if(Values.size() < 2){
                return;
            }
            Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
            Movement->setMaxMomentumY(Values[1].getDoubleUnsafe());
            return;
        case set_min_momentum_y:
            if(Values.size() == 0){
                return;
            }
            Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
            return;
        case set_max_momentum_y:
            if(Values.size() == 0){
                return;
            }
            Movement->setMaxMomentumY(Values[0].getDoubleUnsafe());
            return;
        case set_mouse_movement_button:
            if(Values.size() == 0){
                return;
            }
            Movement->setMouseButton(Values[0].getIntUnsafe());
            return;
        case set_move_on_mouse_release:
            if(Values.size() == 0){
                return;
            }
            Movement->setMoveOnMouseRelease(Values[0].getBoolUnsafe());
            return;
        case set_direction_reset_after_collision:
            if(Values.size() == 0){
                return;
            }
            Movement->setResetDirectionAfterCollision(Values[0].getBoolUnsafe());
            return;
        case set_moving_keys:
            if(Values.size() < 6){
                return;
            }
            Movement->bindKeys(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(),
                Values[3].getIntUnsafe(), Values[4].getIntUnsafe(), Values[5].getIntUnsafe()
            );
            return;
        case set_key_up:
            if(Values.size() == 0){
                return;
            }
            Movement->setUpKey(Values[0].getIntUnsafe());
            return;
        case set_key_right:
            if(Values.size() == 0){
                return;
            }
            Movement->setRightKey(Values[0].getIntUnsafe());
            return;
        case set_key_down:
            if(Values.size() == 0){
                return;
            }
            Movement->setDownKey(Values[0].getIntUnsafe());
            return;
        case set_key_left:
            if(Values.size() == 0){
                return;
            }
            Movement->setLeftKey(Values[0].getIntUnsafe());
            return;
        case set_key_jump:
            if(Values.size() == 0){
                return;
            }
            Movement->setJumpKey(Values[0].getIntUnsafe());
            return;
        case set_key_running:
            if(Values.size() == 0){
                return;
            }
            Movement->setRunningKey(Values[0].getIntUnsafe());
            return;
        case set_random_actions:
            if(Values.size() == 0){
                return;
            }
            Movement->areRandomActionsEnabled = Values[0].getBoolUnsafe();
            return;
        case set_action_time:
            if(Values.size() < 2){
                return;
            }
            Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
            Movement->maxTimeOfAction = Values[1].getDoubleUnsafe();
            return;
        case set_min_action_time:
            if(Values.size() == 0){
                return;
            }
            Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
            return;
        case set_max_action_time:
            if(Values.size() == 0){
                return;
            }
            Movement->maxTimeOfAction = Values[0].getDoubleUnsafe();
            return;
        case set_move_distance:
            if(Values.size() < 2){
                return;
            }
            Movement->minMoveDistance = Values[0].getDoubleUnsafe();
            Movement->maxMoveDistance = Values[1].getDoubleUnsafe();
            return;
        case set_min_move_distance:
            if(Values.size() == 0){
                return;
            }
            Movement->minMoveDistance = Values[0].getDoubleUnsafe();
            return;
        case set_max_move_distance:
            if(Values.size() == 0){
                return;
            }
            Movement->maxMoveDistance = Values[0].getDoubleUnsafe();
            return;
        case set_breaks_allowed:
            if(Values.size() == 0){
                return;
            }
            Movement->areBreaksAllowed = Values[0].getBoolUnsafe();
            return;
        case set_break_chance:
            if(Values.size() == 0){
                return;
            }
            Movement->chanceForBreak = Values[0].getDoubleUnsafe();
            return;
        default:
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Movement->control(attribute, temp, Values.size());
            return;
    }
}
void EventModule::controlCollision(CollisionModule * Collision, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Collision->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_pos:
            if(Values.size() < 2){
                return;
            }
            Collision->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_size:
            if(Values.size() < 2){
                return;
            }
            Collision->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_solid:
            if(Values.size() == 0){
                return;
            }
            Collision->setIsSolid(Values[0].getBoolUnsafe());
            return;
        case toggle_solid:
            Collision->switchSolid();
            return;
        case set_penetration:
            if(Values.size() == 0){
                return;
            }
            Collision->setCanPenetrateSolids(Values[0].getBoolUnsafe());
            return;
        case toggle_penetration:
            Collision->switchSolidPenetration();
            return;
        case ignore:
            if(Values.size() < 2){
                return;
            }
            Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
            return;
        case stop_ignoring:
            if(Values.size() < 2){
                return;
            }
            Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
            return;
        case clear_ignoring:
            if(Values.size() == 0){
                return;
            }
            Collision->clearIgnoreList(Values[0].getStringUnsafe());
            return;
        case ignore_object:
            if(Values.size() == 0){
                return;
            }
            Collision->addToIgnoreList("objects", Values[0].getStringUnsafe());
            return;
        case ignore_group_of_objects:
            if(Values.size() == 0){
                return;
            }
            Collision->addToIgnoreList("groups_of_objects", Values[0].getStringUnsafe());
            return;
        case ignore_hitbox:
            if(Values.size() == 0){
                return;
            }
            Collision->addToIgnoreList("hitboxes", Values[0].getStringUnsafe());
            return;
        case ignore_group_of_hitboxes:
            if(Values.size() == 0){
                return;
            }
            Collision->addToIgnoreList("groups_of_hitboxes", Values[0].getStringUnsafe());
            return;
        case set_is_circle:
            if(Values.size() == 0){
                return;
            }
            Collision->setIsCircle(Values[0].getBoolUnsafe());
            return;
        default:
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Collision->control(attribute, temp, Values.size());
            return;
    }
}
void EventModule::controlParticles(ParticleEffectModule * Particles, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Particles->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_pos:
            if(Values.size() < 2){
                return;
            }
            Particles->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_size:
            if(Values.size() < 2){
                return;
            }
            Particles->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_environment:
            if(Values.size() < 2){
                return;
            }
            Particles->setEnvironment(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_environment_speed:
            if(Values.size() < 2){
                return;
            }
            Particles->setEnvironmentSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_speed:
            if(Values.size() < 2){
                return;
            }
            Particles->setSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_basic_speed:
            if(Values.size() < 2){
                return;
            }
            Particles->setBasicSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_acceleration:
            if(Values.size() < 2){
                return;
            }
            Particles->setAcceleration(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_mass:
            if(Values.size() < 2){
                return;
            }
            Particles->setParticleMass(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_directional_degree:
            if(Values.size() < 2){
                return;
            }
            Particles->setDirectionDegree(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_rotation_speed:
            if(Values.size() < 2){
                return;
            }
            Particles->setRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_time_to_negate_rotation:
            if(Values.size() < 2){
                return;
            }
            Particles->setTimeToNegateRotation(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_are_particles_moving:
            if(Values.size() == 0){
                return;
            }
            Particles->setAreParticlesMoving(Values[0].getBoolUnsafe());
            return;
        case allow_movement:
            Particles->switchAreParticlesMoving();
            return;
        case set_is_env_sync:
            if(Values.size() == 0){
                return;
            }
            Particles->setIsEnvironmentSynchronized(Values[0].getBoolUnsafe());
            return;
        case set_radius:
            if(Values.size() < 2){
                return;
            }
            Particles->setParticleRadius(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_time_to_death:
            if(Values.size() < 2){
                return;
            }
            Particles->setTimeToDeath(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_shape_rotation_speed:
            if(Values.size() < 2){
                return;
            }
            Particles->setShapeRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_color_intensity:
            if(Values.size() < 2){
                return;
            }
            Particles->setColorIntensity(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case add_color_interval:
            if(Values.size() < 4){
                return;
            }
            Particles->addColorInterval(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe());
            return;
        case add_color_interval_in_hex:
            if(Values.size() == 0){
                return;
            }
            Particles->addColorIntervalInHex(Values[0].getStringUnsafe());
            return;
        case remove_color_interval:
            if(Values.size() == 0){
                return;
            }
            Particles->removeColorInterval(Values[0].getIntUnsafe());
            return;
        case add_color:
            if(Values.size() < 3){
                return;
            }
            Particles->addColor(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
            return;
        case add_color_in_hex:
            if(Values.size() == 0){
                return;
            }
            Particles->addColorInHex(Values[0].getStringUnsafe());
            return;
        case remove_color:
            if(Values.size() == 0){
                return;
            }
            Particles->removeColor(Values[0].getIntUnsafe());
            return;
        case add_image:
            if(Values.size() == 0){
                return;
            }
            Particles->addImage(Values[0].getStringUnsafe());
            return;
        case remove_image:
            if(Values.size() == 0){
                return;
            }
            Particles->removeImage(Values[0].getStringUnsafe());
            return;
        case remove_image_by_index:
            if(Values.size() == 0){
                return;
            }
            Particles->removeImage(Values[0].getIntUnsafe());
            return;
        case set_shape:
            if(Values.size() == 0){
                return;
            }
            Particles->setParticlesShape(Values[0].getIntUnsafe());
            return;
        case set_use_images:
            if(Values.size() == 0){
                return;
            }
            Particles->setUseImageAsParticles(Values[0].getBoolUnsafe());
            return;
        case toggle_source:
            Particles->switchUseImageAsParticles();
            return;
        case set_use_random_colors:
            if(Values.size() == 0){
                return;
            }
            Particles->setUseRandomColors(Values[0].getBoolUnsafe());
            return;
        case toggle_random_colors:
            Particles->switchUseRandomColors();
            return;
        case set_is_static:
            if(Values.size() == 0){
                return;
            }
            Particles->setIsModuleStatic(Values[0].getBoolUnsafe());
            return;
        case set_drawing_details:
            if(Values.size() == 0){
                return;
            }
            Particles->setIsDrawingWithDetails(Values[0].getBoolUnsafe());
            return;
        case set_block_spawning:
            if(Values.size() == 0){
                return;
            }
            Particles->setBlockParticlesSpawn(Values[0].getBoolUnsafe());
            return;
        case set_spawning:
            if(Values.size() == 0){
                return;
            }
            Particles->setCanParticlesSpawn(Values[0].getBoolUnsafe());
            return;
        case allow_spawning:
            Particles->allowSpawning();
            return;
        case set_spawn_rate:
            if(Values.size() < 2){
                return;
            }
            Particles->setParticlesPerSpawn(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            return;
        case set_spawn_cap:
            if(Values.size() == 0){
                return;
            }
            Particles->setMaxParticlesCount(Values[0].getIntUnsafe());
            return;
        case set_max_time_to_spawn:
            if(Values.size() == 0){
                return;
            }
            Particles->setMaxTimeToSpawn(Values[0].getDoubleUnsafe());
            return;
        case bind_key:
            if(Values.size() == 0){
                return;
            }
            Particles->setSpawnKeyBind(Values[0].getStringUnsafe()[0]);
            return;
        case bind_key_by_int:
            if(Values.size() == 0){
                return;
            }
            Particles->setSpawnKeyBindShort(Values[0].getIntUnsafe());
            return;
        case spawn_on_key_release:
            if(Values.size() == 0){
                return;
            }
            Particles->setSpawnOnKeyRelease(Values[0].getBoolUnsafe());
            return;
        default:
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Particles->control(attribute, temp, Values.size());
            return;
    }
}
void EventModule::controlVariables(VariableModule * Variable, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            } 
            Variable->setID(Values[0].getStringUnsafe(), &IDs);
            return;
        case toggle_bool:
            Variable->toggleBool();
            return;
        case set_bool_random:
            if(rand() % 2 == 0)
            Variable->toggleBool();
            return;
        case set_bool_probability:
            if(Values.size() >= 3){
                bool settingBool;
                if(Values[0].getDoubleUnsafe() == 0){
                    settingBool = false;
                }
                else if(Values[0].getDoubleUnsafe() == 1){
                    settingBool = true;
                }
                else{
                    return;
                }
                if(rand() % Values[2].getIntUnsafe() <= Values[1].getIntUnsafe()){
                    Variable->setBool(settingBool);
                }
                return;
            }
            else if(Values.size() >= 2){
                bool settingBool;
                if(Values[0].getDoubleUnsafe() == 0){
                    settingBool = false;
                }
                else if(Values[0].getDoubleUnsafe() == 1){
                    settingBool = true;
                }
                else{
                    return;
                }
                if(rand() % 100 <= Values[1].getDoubleUnsafe()){
                    Variable->setBool(settingBool);
                }
            } 
            return;
        case set_bool:
            if(Values.size() < 1){
                return;
            } 
            if(Values[0].getDoubleUnsafe() == 0){
                Variable->setBool(false);
            }
            if(Values[0].getDoubleUnsafe() == 1){
                Variable->setBool(true);
            }
            return;
        case set_int:
            if(Values.size() < 1){
                return;
            } 
            Variable->setInt(Values[0].getIntUnsafe());
            return;
        case set_int_interval:
            if(Values.size() < 2){
                return;
            } 
            Variable->setInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
            return;
        case set_int_random:{
            if(Values.size() < 1){
                return;
            } 
            unsigned int dice = rand() % Values.size();
            Variable->setInt(Values[dice].getIntUnsafe());
            return;
        }
        case set_double:
            if(Values.size() < 1){
                return;
            } 
            Variable->setDouble(Values[0].getDoubleUnsafe());;
            return;
        case set_double_interval:
            if(Values.size() < 2){
                return;
            } 
            Variable->setDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
            return;
        case set_double_random:{
            if(Values.size() < 1){
                return;
            } 
            unsigned int dice = rand() % Values.size();
            Variable->setDouble(Values[dice].getDoubleUnsafe());
            return;
        }
        case set_string:
            if(Values.size() < 1){
                return;
            } 
            Variable->setString(Values[0].getString());
            return;
        case set_string_random:{
            if(Values.size() < 1){
                return;
            } 
            cout << "hello ";
            unsigned int dice = rand() % Values.size();
            cout << Variable->setString(Values[dice].getString()) << " ";
            cout << Variable->getString() << "\n";
            return;
        }
        case add_int:
            if(Values.size() < 1){
                return;
            } 
            Variable->addInt(Values[0].getIntUnsafe());
            return;
        case add_int_interval:
            if(Values.size() < 2){
                return;
            } 
            Variable->addInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
            return;
        case add_int_random:{
            if(Values.size() < 1){
                return;
            } 
            unsigned int dice = rand() % Values.size();
            Variable->addInt(Values[dice].getIntUnsafe());
            return;
        }
        case add_double:
            if(Values.size() < 1){
                return;
            } 
            Variable->addDouble(Values[0].getDoubleUnsafe());;
            return;
        case add_double_interval:
            if(Values.size() < 2){
                return;
            } 
            Variable->addDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
            return;
        case add_double_random:{
            if(Values.size() < 1){
                return;
            } 
            unsigned dice = rand() % Values.size();
            Variable->addDouble(Values[dice].getDoubleUnsafe());
            return;
        }
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Function "
                << attributeToStr(attribute) << "<" << Values.size() << "> does not exist.\n";
            return;
    }
}
void EventModule::controlScrollbar(ScrollbarModule * Scrollbar, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Scrollbar->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_pos:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case AttributeType::resize:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->setSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_thumb_position:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->setThumbPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_thumb_size:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->setThumbSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_scrolling_area:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->setScrollingArea(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_real_scrolling_area:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->setRealScrollingArea(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case add_real_scrolling_area:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->addRealScrollingArea(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
            return;
        case set_thumb_image_id:
            if(Values.size() < 1){
                return;
            }
            Scrollbar->setThumbImageID(Values[0].getString());
            return;
        case set_track_image_id:
            if(Values.size() < 1){
                return;
            }
            Scrollbar->setTrackImageID(Values[0].getString());
            return;
        case set_can_be_drawn:
            if(Values.size() < 1){
                return;
            }
            Scrollbar->canBeDrawn = Values[0].getBool();
            return;
        case set_mouse_wheel_speed:
            if(Values.size() < 1){
                return;
            }
            Scrollbar->mouseWheelSpeed = Values[0].getDouble();
            return;
        case scroll_to_the_beginning:
            Scrollbar->scrollToTheBeginning();
            return;
        case scroll_to_the_end:
            Scrollbar->scrollToTheEnd();
            return;
        case scroll_by_distance:
            if(Values.size() < 2){
                return;
            }
            Scrollbar->scrollByDistance(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case add_camera:
            if(Values.size() < 1){
                return;
            }
            addUniqueToStringVector(Scrollbar->cameraIDs, Values[0].getString());
            return;
        case remove_camera:
            if(Values.size() < 1){
                return;
            }
            removeFromStringVector(Scrollbar->cameraIDs, Values[0].getString());
            return;
        case clear_cameras:
            if(Values.size() < 1){
                return;
            }
            Scrollbar->cameraIDs.clear();
            return;
        default:{
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Scrollbar->control(attribute, temp, Values.size());
            return;
        }
    }
}
void EventModule::controlPrimitives(PrimitivesModule * Primitives, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case update_with_size:
            Primitives->updateWithSize();
            return;
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Primitives->setID(Values[0].getStringUnsafe(), IDs);
            return;
        case set_pos:
            if(Values.size() < 2){
                return;
            }
            Primitives->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_size:
            if(Values.size() < 2){
                return;
            }
            Primitives->setSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_type:
            if(Values.size() < 1){
                return;
            }
            Primitives->type = getPrimitiveType(Values[0].getStringUnsafe());
            return;
        case add_point:
            if(Values.size() < 2){
                return;
            }
            Primitives->points.emplace_back(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
            return;
        case set_color:
            if(Values.size() < 4){
                return;
            }
            Primitives->color = al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
            return;
        case set_thickness:
            if(Values.size() < 1){
                return;
            }
            Primitives->thickness = Values[0].getDoubleUnsafe();
            return;
        case set_radius:
            if(Values.size() < 1){
                return;
            }
            Primitives->radius = Values[0].getDoubleUnsafe();
            return;
        default:{
            bool temp = false;
            if(Values.size() > 0){
                temp = Values[0].getBoolUnsafe();
            }
            Primitives->control(attribute, temp, Values.size());
            return;
        }
    }
}
void EventModule::controlVector(VectorModule * Vector, AttributeType attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    switch(attribute){
        case set_id:
            if(Values.size() == 0){
                return;
            }
            Vector->setID(Values[0].getStringUnsafe(), &IDs);
            return;
        case push_back_a:
            if(Values.size() < 1){
                return;
            }
            if(Vector->getType() != 'n'){
                Vector->reserve(Vector->getSize() + Values.size());
            }
            for(const VariableModule & Value : Values){
                if(Value.getType() == 'b'){
                    Vector->pushBool(Value.getBoolUnsafe());
                }
                else if(Value.getType() == 'i'){
                    Vector->pushInt(Value.getIntUnsafe());
                }
                else if(Value.getType() == 'd'){
                    Vector->pushDouble(Value.getDoubleUnsafe());
                }
                else if(Value.getType() == 's'){
                    Vector->pushString(Value.getStringUnsafe());
                }
                else{
                    cerr << "Error: In " << __FUNCTION__ << ": Value of '" << Value.getType()
                        << "' type cannot be pushed back into the vector '" << Vector->getID()
                        << "' of '" << Vector->getType() << "' type.\n";
                }
            }
            return;
        case pop_back_a:
            Vector->popBack();
            return;
        case clear_a:
            Vector->clear();
            return;
        case remove_a:
            if(Values.size() < 1){
                return;
            }
            Vector->removeIndex(Values[0].getIntUnsafe());
            return;
        case set_a:
            if(Values.size() < 2){
                return;
            }
            if(Values[1].getType() == 'b'){
                Vector->setBool(Values[0].getIntUnsafe(), Values[1].getBoolUnsafe());
            }
            else if(Values[1].getType() == 'i'){
                Vector->setInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
            }
            else if(Values[1].getType() == 'd'){
                Vector->setDouble(Values[0].getIntUnsafe(), Values[1].getDoubleUnsafe());
            }
            else if(Values[1].getType() == 's'){
                Vector->setString(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
            }
            else{
                cerr << "Error: In " << __FUNCTION__ << ": Value of '" << Values[0].getType()
                    << "' type cannot be assign to the vector '" << Vector->getID()
                    << "' of '" << Vector->getType() << "' type.\n";
            }
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Function "
                << attributeToStr(attribute) << "<" << Values.size() << "> does not exist.\n";
            return;
    }
}

void EventModule::getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers){
    if(attribute == else_child_id){
        BasePointers.emplace_back(BasePointersStruct());
        BasePointers.back().setPointer(&elseChildID);
    }
    else{
        getPrimaryContext(attribute, BasePointers);
    }
}


void ValueLocation::print(string dynamicID){
    if(dynamicID != ""){
        cout << dynamicID;
    }
    if(source != null_s){
        if(dynamicID != ""){
            cout << ".";
        }
        cout << sourceToStr(source);
    }
    if(cameraID != "" && cameraID != "_"){
        cout << "." << cameraID;
    }
    if(layerID != "" && layerID != "_"){
        cout << "." << layerID;
    }
    if(objectID != "" && objectID != "_"){
        cout << "." << objectID;
    }
    if(moduleType != null_s){
        cout << "." << sourceToStr(moduleType);
    }
    if(moduleID != "" && moduleID != "_"){
        cout << "." << moduleID;
    }
    if(attribute != null_a){
        cout << "." << attributeToStr(attribute);
    }
    if(spareID != "" && spareID != "_"){
        cout << "." << spareID;
    }
    cout << " ";
}