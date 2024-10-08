#include "eventModule.h"

ConditionClass::ConditionClass(unsigned int newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass(string newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass() : Literal(){}

OperationClass::OperationClass(){
    instruction = EngineInstr::null;
}
inline string errorSpacing(){return "\t";}; 
bool OperationClass::addParameter(string scriptName, unsigned lineNumber, string & error, vector<WordStruct> words,
    unsigned index, char type, string name, bool optional
){
    auto printError = [](string scriptName, unsigned lineNumber, string instruction, std::string error){
        cerr << "Error in: " << scriptName << ":" << lineNumber << ":\n"
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
        Parameters.back().variableID = words[index].value;
        Parameters.back().negateVariable = words[index].negateVariable;
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
    vector<WordStruct> words, unsigned &index, char type, string name, bool optional
){
    auto printError = [](string scriptName, unsigned lineNumber, string instruction, std::string error){
        cerr << "Error in: " << scriptName << ":" << lineNumber << ":\n"
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
    return addVectorOrVariableToParameters(scriptName, lineNumber, error, words, index, type, name, optional);
}
bool OperationClass::addVectorOrVariableToParameters(string scriptName, unsigned lineNumber, string &error,
    vector<WordStruct> words, unsigned &index, char type, string name, bool optional)
{
    auto printError = [](string scriptName, unsigned lineNumber, string instruction, std::string error){
        cerr << "Error in: " << scriptName << ":" << lineNumber << ":\n"
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
        Parameters.back().variableID = words[index].value;
        Parameters.back().negateVariable = words[index].negateVariable;
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
            Parameters.back().variableID = words[index].value;
            Parameters.back().negateVariable = words[index].negateVariable;
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
    areDependentOperationsDone = false;
    elseChildFinished = false;
    loop = false;
    willBeDeleted = false;
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
void EventModule::resetStatus(){
	for(ChildStruct & Child : Children){
		Child.finished = false;
	}
}
bool EventModule::checkIfAllChildrenFinished(){
    for(ChildStruct Child : Children){
        if(!Child.finished){
            return false;
        }
    }
    return true;
}

void EventModule::controlText(TextModule * Text, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer){
    if(attribute == "set_id" && Values.size() > 0){
        Text->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        Text->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Text->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        Text->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_scale" && Values.size() >= 2){
        Text->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        Text->setColors(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "set_color_r" && Values.size() >= 1){
        Text->color.r = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_g" && Values.size() >= 1){
        Text->color.g = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_b" && Values.size() >= 1){
        Text->color.b = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_a" && Values.size() >= 1){
        Text->color.a = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_random_color"){
        Text->setRandomColors();
    }
    else if(attribute == "increment_random_color"){
        Text->incrementRandomColor();
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        Text->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_random_change_speed" && Values.size() >= 1){
        Text->randomChangeSpeed = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_min_color" && Values.size() >= 1){
        Text->minColorValue = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_color" && Values.size() >= 1){
        Text->maxColorValue = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "rotate" && Values.size() >= 1){
        Text->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_font" && Values.size() > 0){
        Text->setFontID(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_new_content" && Values.size() > 0){
        Text->addNewContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_new_and_update" && Values.size() > 0){
        Text->addNewContentAndResize(Values[0].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "update_size"){
        Text->fitSizeToText(FontContainer);
    }
    else if(attribute == "select_content" && Values.size() > 0){
        Text->chooseContent(Values[0].getIntUnsafe());
    }
    else if(attribute == "delete" && Values.size() > 0){
        Text->deleteContent(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_text" && Values.size() > 1){
        Text->addTextToContent(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "set_text" && Values.size() > 1){
        Text->modifyContent(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "set_resize_text" && Values.size() > 1){
        Text->modifyContentAndResize(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "set_wrapping" && Values.size() > 0){
        Text->setWrapping(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_horizontal_align" && Values.size() > 0){
        Text->setHorizontalAlign(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_vertical_align" && Values.size() > 0){
        Text->setVerticalAlign(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_cursor_pos" && Values.size() > 0){
        Text->setCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_second_cursor_pos" && Values.size() > 0){
        Text->setSecondCursorPos(Values[0].getIntUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Text->control(attribute, temp, Values.size());
    }
}
void EventModule::controlEditableText(EditableTextModule *EditableText, string attribute, const vector<VariableModule> &Values, vector<string> &IDs, const vector<SingleFont> &FontContainer){
    if(attribute == "set_editable" && Values.size() > 0){
        EditableText->setCanBeEdited(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_space_use" && Values.size() > 0){
        EditableText->setCanUseSpace(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_enter_use" && Values.size() > 0){
        EditableText->setCanUseEnter(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_can_enter_accept" && Values.size() > 0){
        EditableText->setCanEnterAcceptChanges(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_numerical" && Values.size() > 0){
        EditableText->setIsNumerical(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_floating_point" && Values.size() > 0){
        EditableText->setHasFloatingPoint(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_can_update_variable" && Values.size() > 0){
        EditableText->setUpdateConnectedVariable(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_auto_cleaning" && Values.size() > 0){
        EditableText->setCanClearContentAfterSuccess(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_use_tabs" && Values.size() > 0){
        EditableText->setUseTabs(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_min_content_length" && Values.size() > 0){
        EditableText->setMinContentSize(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_max_content_length" && Values.size() > 0){
        EditableText->setMaxContentSize(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_input_delay" && Values.size() > 0){
        EditableText->setInputDelay(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_repetition_delay" && Values.size() > 0){
        EditableText->setRepetitionDelay(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "connect_object" && Values.size() > 0){
        EditableText->connectedObject = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_group" && Values.size() > 0){
        EditableText->connectedGroup = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_module" && Values.size() > 0){
        EditableText->connectedModule = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_module_id" && Values.size() > 0){
        EditableText->connectedModuleID = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_variable" && Values.size() > 0){
        EditableText->connectedVariable = Values[0].getStringUnsafe();
    }
    else if(attribute == "set_protected_area" && Values.size() > 0){
        EditableText->setProtectedArea(Values[0].getIntUnsafe());
    }
    else if(attribute == "cut_unprotected_area"){
        EditableText->cutCurrentContent(EditableText->getProtectedArea());
    }
    else if(attribute == "stop_editing"){
        EditableText->setEditingIsActive(false);
    }
    else if(attribute == "start_editing"){
        EditableText->setEditingIsActive(true);
    }
    else if(attribute == "set_ignore_vertical_arrows" && Values.size() > 0){
        EditableText->ignoreVerticalArrows = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_auto_resize_axis_y" && Values.size() > 0){
        EditableText->setAutoResizeAxisY(Values[0].getBoolUnsafe());
    }
    else{
        controlText(EditableText, attribute, Values, IDs, FontContainer);
    }
}
void EventModule::controlSuperText(SuperTextModule * SuperText, string attribute, const vector<VariableModule> & Values,
    vector <string> & IDs, vector<SingleFont> & FontContainer, string EXE_PATH, string workingDirectory
){
    if(attribute == "set_id" && Values.size() > 0){
        SuperText->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        SuperText->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        SuperText->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        SuperText->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_scale" && Values.size() >= 2){
        SuperText->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_cursor_pos" && Values.size() > 0){
        SuperText->setCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_second_cursor_pos" && Values.size() > 0){
        SuperText->setSecondCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "update"){
        SuperText->update();
    }
    else if(attribute == "crop_size_to_text"){
        SuperText->cropSizeToText();
    }
    else if(attribute == "crop_width_to_text"){
        SuperText->cropWidthToText();
    }
    else if(attribute == "crop_height_to_text"){
        SuperText->cropHeightToText();
    }
    else if(attribute == "set_content" && Values.size() >= 1){
        SuperText->setContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_content" && Values.size() >= 1){
        SuperText->addContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_new_text_line" && Values.size() >= 1){
        SuperText->addNewTextLine(Values[0].getStringUnsafe());
    }
    else if(attribute == "set_text_line" && Values.size() >= 2){
        SuperText->setTextLine(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "add_to_text_line" && Values.size() >= 2){
        SuperText->addToTextLine(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if((attribute == "save_to_file" || attribute == "load_from_file") && Values.size() >= 1){
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
        if(attribute == "save_to_file"){
            SuperText->saveFormattedTextToTheFile(finalPath);
        }
        else{
            SuperText->loadFormattedTextFromTheFile(finalPath, FontContainer);
        }
    }
    else if(attribute == "add_format" && Values.size() >= 13){
        SuperText->addFormat(al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
            al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()),
            Values[8].getStringUnsafe(), FontContainer, Values[9].getIntUnsafe(), Values[10].getIntUnsafe(), Values[11].getIntUnsafe(),
            Values[12].getIntUnsafe()
        );
    }
    else if(attribute == "add_format" && Values.size() >= 6){
        SuperText->addFormat(al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
            al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
            Values[4].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[5].getIntUnsafe()
        );
    }
    else if(attribute == "add_format" && Values.size() >= 2){
        SuperText->addFormat(al_map_rgba_f(0.0, 0.0, 0.0, 1.0), al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
            Values[0].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[1].getIntUnsafe()
        );
    }
    else if(attribute == "modify_format" && Values.size() >= 14){
        SuperText->modifyFormat(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(),
            Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()), al_map_rgba_f(Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(),
            Values[7].getDoubleUnsafe(), Values[8].getDoubleUnsafe()), Values[9].getStringUnsafe(), FontContainer, Values[10].getIntUnsafe(),
            Values[11].getIntUnsafe(), Values[12].getBoolUnsafe(), Values[13].getIntUnsafe()
        );
    }
    else if(attribute == "modify_last_format" && Values.size() >= 13){
        SuperText->modifyFormat(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(),
            Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()), al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(),
            Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()), Values[8].getStringUnsafe(), FontContainer, Values[9].getIntUnsafe(),
            Values[10].getIntUnsafe(), Values[11].getBoolUnsafe(), Values[12].getIntUnsafe()
        );
    }
    else if(attribute == "modify_last_format" && Values.size() >= 10){
        SuperText->modifyFormat(SuperText->Formatting.size() - 1,
            al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
            al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()),
            Values[8].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[9].getIntUnsafe()
        );
    }
    else if(attribute == "inject_format" && Values.size() >= 14){
        SuperText->injectFormat(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), al_map_rgba_f(Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(),
            Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe()), al_map_rgba_f(Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe(),
            Values[8].getDoubleUnsafe(), Values[9].getDoubleUnsafe()), Values[10].getStringUnsafe(), FontContainer, Values[11].getIntUnsafe(),
            Values[12].getIntUnsafe(), Values[13].getBoolUnsafe()
        );
    }
    else if(attribute == "inject_format" && Values.size() >= 11){
        SuperText->injectFormat(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(),
            al_map_rgba_f(Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe()),
            al_map_rgba_f(Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe(), Values[8].getDoubleUnsafe(), Values[9].getDoubleUnsafe()),
            Values[10].getStringUnsafe(), FontContainer, 0.0, 0.0, false
        );
    }
    else if(attribute == "delete_format" && Values.size() >= 1){
        SuperText->deleteFormat(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 5){
        SuperText->setColor(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(),
            Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_last_color" && Values.size() >= 4){
        SuperText->setColor(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(),
            Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_accent_color" && Values.size() >= 5){
        SuperText->setAccentColor(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(),
            Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_last_accent_color" && Values.size() >= 4){
        SuperText->setAccentColor(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(),
            Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_font" && Values.size() >= 2){
        SuperText->setFont(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "set_offset" && Values.size() >= 3){
        SuperText->setOffset(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "add_offset" && Values.size() >= 3){
        SuperText->addOffset(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "set_offset_x" && Values.size() >= 2){
        SuperText->setOffsetX(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "add_offset_x" && Values.size() >= 2){
        SuperText->addOffsetX(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_offset_y" && Values.size() >= 2){
        SuperText->setOffsetY(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "add_offset_y" && Values.size() >= 2){
        SuperText->addOffsetY(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_selected" && Values.size() >= 2){
        SuperText->setSelected(Values[0].getIntUnsafe(), Values[1].getBoolUnsafe());
    }
    else if(attribute == "set_limit" && Values.size() >= 2){
        SuperText->setLimit(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_wrapping" && Values.size() >= 1){
        if(Values[0].getStringUnsafe().size() > 0){
            SuperText->setWrapping(Values[0].getStringUnsafe()[0]);
        }
        else{
            std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
        }
    }
    else if(attribute == "set_horizontal_align" && Values.size() >= 1){
        if(Values[0].getStringUnsafe().size() > 0){
            SuperText->setHorizontalAlign(Values[0].getStringUnsafe()[0]);
        }
        else{
            std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
        }
    }
    else if(attribute == "set_vertical_align" && Values.size() >= 1){
        if(Values[0].getStringUnsafe().size() > 0){
            SuperText->setVerticalAlign(Values[0].getStringUnsafe()[0]);
        }
        else{
            std::cerr << "Error: In " << __FUNCTION__ << ": In the event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
        }
    }
    else if(attribute == "set_padding_between_lines" && Values.size() >= 1){
        SuperText->setPaddingBetweenLines(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        SuperText->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "add_rotation" && Values.size() >= 1){
        SuperText->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_tab_length" && Values.size() >= 1){
        SuperText->setTabLength(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_draw_selection_first" && Values.size() >= 1){
        SuperText->drawSelectionFirst = Values[0].getBoolUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        SuperText->control(attribute, temp, Values.size());
    }
}
void EventModule::controlSuperEditableText(SuperEditableTextModule *& SuperEditableText, string attribute, const vector<VariableModule> & Values,
    vector <string> & IDs, vector<SingleFont> & FontContainer, SuperEditableTextModule *& ActiveEditableText, string EXE_PATH, string workingDirectory
){
    if(attribute == "set_can_be_edited" && Values.size() > 0){
        SuperEditableText->canBeEdited = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_use_space" && Values.size() > 0){
        SuperEditableText->canUseSpace = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_use_enter" && Values.size() > 0){
        SuperEditableText->canUseEnter = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_use_tabs" && Values.size() > 0){
        SuperEditableText->canUseTabs = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_is_numerical" && Values.size() > 0){
        SuperEditableText->isNumerical = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_has_floating_point" && Values.size() > 0){
        SuperEditableText->hasFloatingPoint = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_ignore_vertical_arrows" && Values.size() > 0){
        SuperEditableText->ignoreVerticalArrows = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_ignore_content_restriction" && Values.size() > 0){
        SuperEditableText->ignoreContentRestriction = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_is_storing_history" && Values.size() > 0){
        SuperEditableText->isStoringHistory = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_copy_format" && Values.size() > 0){
        SuperEditableText->canCopyFormat = Values[0].getBoolUnsafe();
    }
    else if(attribute == "start_editing"){
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
    }
    else if(attribute == "stop_editing"){
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
    }
    else if(attribute == "set_min_content_length" && Values.size() > 0){
        SuperEditableText->minContentLength = Values[0].getIntUnsafe();
    }
    else if(attribute == "set_max_content_length" && Values.size() > 0){
        SuperEditableText->maxContentLength = Values[0].getIntUnsafe();
    }
    else if(attribute == "set_input_delay" && Values.size() > 0){
        SuperEditableText->inputDelay = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_repetition_delay" && Values.size() > 0){
        SuperEditableText->repetitionDelay = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_protected_area" && Values.size() > 0){
        SuperEditableText->protectedArea = Values[0].getIntUnsafe();
    }
    else if(attribute == "cut_unprotected_area"){
        SuperEditableText->cutContent(SuperEditableText->protectedArea);
    }
    else if(attribute == "set_cursor_pos" && Values.size() > 0){
        SuperEditableText->setCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_second_cursor_pos" && Values.size() > 0){
        SuperEditableText->setSecondCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "deactivate"){
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
    }
    else{
        controlSuperText(SuperEditableText, attribute, Values, IDs, FontContainer, EXE_PATH, workingDirectory);
    }
}
void EventModule::controlImage(ImageModule *Image, string attribute, const vector<VariableModule> &Values, vector<string> &IDs, vector<SingleBitmap> &BitmapContainer, string workingDirectory){
    if(attribute == "set_id" && Values.size() > 0){
        Image->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        Image->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Image->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        Image->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "resize" && Values.size() >= 2){
        Image->resize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        Image->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "rotate" && Values.size() >= 1){
        Image->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_mirror" && Values.size() >= 2){
        Image->setMirror(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        float arr[] = {float(Values[0].getDoubleUnsafe()), float(Values[1].getDoubleUnsafe()), float(Values[2].getDoubleUnsafe()), float(Values[3].getDoubleUnsafe())};
        Image->setImageColor(arr);
    }
    else if(attribute == "set_color_r" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'r');
    }
    else if(attribute == "set_color_g" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'g');
    }
    else if(attribute == "set_color_b" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'b');
    }
    else if(attribute == "set_color_a" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'a');
    }
    else if(attribute == "set_light" && Values.size() >= 4){
        Image->setLightColor(vec3d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe()), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "connect_bitmap" && Values.size() > 0){
        if(Values.size() == 1){
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", workingDirectory);
        }
        else{
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), Values[1].getStringUnsafe(), workingDirectory);
        }   
    }
    else if(attribute == "connect_bitmap_via_path" && Values.size() > 0){
        Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", workingDirectory);  
    }
    else if(attribute == "connect_bitmap_via_alias" && Values.size() > 0){
        Image->connectBitmap(BitmapContainer, "", Values[0].getStringUnsafe(), workingDirectory);  
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Image->control(attribute, temp, Values.size());
    }
}
void EventModule::controlMovement(MovementModule * Movement, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "move" && Values.size() >= 2){
        Movement->addMomentum(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "move_up"){
        Movement->setNextMove(true, false, false, false, false, false, false);
    }
    else if(attribute == "move_right"){
        Movement->setNextMove(false, true, false, false, false, false, false);
    }
    else if(attribute == "move_down"){
        Movement->setNextMove(false, false, true, false, false, false, false);
    }
    else if(attribute == "move_left"){
        Movement->setNextMove(false, false, false, true, false, false, false);
    }
    else if(attribute == "move_jump"){
        Movement->setNextMove(false, false, false, false, true, false, false);
    }
    else if(attribute == "move_fall"){
        Movement->setNextMove(false, false, false, false, false, true, false);
    }
    else if(attribute == "move_run"){
        Movement->setNextMove(false, false, false, false, false, false, true);
    }
    else if(attribute == "set_id" && Values.size() > 0){
        Movement->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_movement_type" && Values.size() > 0){
        Movement->setMovementType(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_input_type" && Values.size() > 0){
        Movement->setInputType(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_allowed_jumps" && Values.size() > 0){
        Movement->setAllowedJumps(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_jump_cooldown" && Values.size() > 0){
        Movement->setJumpCooldownDuration(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_move_cooldown" && Values.size() > 0){
        Movement->setMoveCooldownDuration(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_diagonal_movement" && Values.size() > 0){
        Movement->setCanMoveDiagonally(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_reset_momentum_when_jumping" && Values.size() > 0){
        Movement->setResetMomentumWhenJumping(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_mass" && Values.size() > 0){
        Movement->setBodyMass(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_walking_speed" && Values.size() > 0){
        Movement->setWalkingSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_running_speed" && Values.size() > 0){
        Movement->setRunningSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_jump_speed" && Values.size() > 0){
        Movement->setJumpSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_gravity" && Values.size() > 0){
        Movement->setGravitation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_friction" && Values.size() > 0){
        Movement->setBaseFriction(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_max_momentum_x" && Values.size() > 0){
        Movement->setMaxMomentumX(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_momentum_y" && Values.size() > 1){
        Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
        Movement->setMaxMomentumY(Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_min_momentum_y" && Values.size() > 0){
        Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_max_momentum_y" && Values.size() > 0){
        Movement->setMaxMomentumY(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_mouse_movement_button" && Values.size() > 0){
        Movement->setMouseButton(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_move_on_mouse_release" && Values.size() > 0){
        Movement->setMoveOnMouseRelease(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_direction_reset_after_collision" && Values.size() > 0){
        Movement->setResetDirectionAfterCollision(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_moving_keys" && Values.size() > 5){
        Movement->bindKeys(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe(), Values[4].getIntUnsafe(), Values[5].getIntUnsafe());
    }
    else if(attribute == "set_key_up" && Values.size() > 0){
        Movement->setUpKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_right" && Values.size() > 0){
        Movement->setRightKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_down" && Values.size() > 0){
        Movement->setDownKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_left" && Values.size() > 0){
        Movement->setLeftKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_jump" && Values.size() > 0){
        Movement->setJumpKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_running" && Values.size() > 0){
        Movement->setRunningKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_random_actions" && Values.size() > 0){
        Movement->areRandomActionsEnabled = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_action_time" && Values.size() > 1){
        Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
        Movement->maxTimeOfAction = Values[1].getDoubleUnsafe();
    }
    else if(attribute == "set_min_action_time" && Values.size() > 0){
        Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_action_time" && Values.size() > 0){
        Movement->maxTimeOfAction = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_move_distance" && Values.size() > 1){
        Movement->minMoveDistance = Values[0].getDoubleUnsafe();
        Movement->maxMoveDistance = Values[1].getDoubleUnsafe();
    }
    else if(attribute == "set_min_move_distance" && Values.size() > 0){
        Movement->minMoveDistance = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_move_distance" && Values.size() > 0){
        Movement->maxMoveDistance = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_breaks_allowed" && Values.size() > 0){
        Movement->areBreaksAllowed = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_break_chance" && Values.size() > 0){
        Movement->chanceForBreak = Values[0].getDoubleUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Movement->control(attribute, temp, Values.size());
    }
}
void EventModule::controlCollision(CollisionModule * Collision, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Collision->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        Collision->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Collision->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_solid" && Values.size() > 0){
        Collision->setIsSolid(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_solid"){
        Collision->switchSolid();
    }
    else if(attribute == "set_penetration" && Values.size() > 0){
        Collision->setCanPenetrateSolids(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_penetration"){
        Collision->switchSolidPenetration();
    }
    else if(attribute == "ignore" && Values.size() > 1){
        Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "stop_ignoring" && Values.size() > 1){
        Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "clear_ignoring" && Values.size() > 0){
        Collision->clearIgnoreList(Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_object" && Values.size() > 0){
        Collision->addToIgnoreList("objects", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_group_of_objects" && Values.size() > 0){
        Collision->addToIgnoreList("groups_of_objects", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_hitbox" && Values.size() > 0){
        Collision->addToIgnoreList("hitboxes", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_group_of_hitboxes" && Values.size() > 0){
        Collision->addToIgnoreList("groups_of_hitboxes", Values[0].getStringUnsafe());
    }
    else if(attribute == "set_is_circle" && Values.size() > 0){
        Collision->setIsCircle(Values[0].getBoolUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Collision->control(attribute, temp, Values.size());
    }
}
void EventModule::controlParticles(ParticleEffectModule * Particles, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Particles->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        Particles->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Particles->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_environment" && Values.size() >= 2){
        Particles->setEnvironment(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_environment_speed" && Values.size() >= 2){
        Particles->setEnvironmentSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_speed" && Values.size() > 1){
        Particles->setSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_basic_speed" && Values.size() > 1){
        Particles->setBasicSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_acceleration" && Values.size() > 1){
        Particles->setAcceleration(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_mass" && Values.size() > 1){
        Particles->setParticleMass(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_directional_degree" && Values.size() > 1){
        Particles->setDirectionDegree(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation_speed" && Values.size() > 1){
        Particles->setRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_time_to_negate_rotation" && Values.size() > 1){
        Particles->setTimeToNegateRotation(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_are_particles_moving" && Values.size() > 0){
        Particles->setAreParticlesMoving(Values[0].getBoolUnsafe());
    }
    else if(attribute == "allow_movement"){
        Particles->switchAreParticlesMoving();
    }
    else if(attribute == "set_is_env_sync" && Values.size() > 0){
        Particles->setIsEnvironmentSynchronized(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_radius" && Values.size() > 1){
        Particles->setParticleRadius(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_time_to_death" && Values.size() > 1){
        Particles->setTimeToDeath(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_shape_rotation_speed" && Values.size() > 1){
        Particles->setShapeRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color_intensity" && Values.size() > 1){
        Particles->setColorIntensity(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_color_interval" && Values.size() > 3){
        Particles->addColorInterval(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe());
    }
    else if(attribute == "add_color_interval_in_hex" && Values.size() > 0){
        Particles->addColorIntervalInHex(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_color_interval" && Values.size() > 0){
        Particles->removeColorInterval(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_color" && Values.size() > 2){
        Particles->addColor(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "add_color_in_hex" && Values.size() > 0){
        Particles->addColorInHex(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_color" && Values.size() > 0){
        Particles->removeColor(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_image" && Values.size() > 0){
        Particles->addImage(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_image" && Values.size() > 0){
        Particles->removeImage(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_image_by_index" && Values.size() > 0){
        Particles->removeImage(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_shape" && Values.size() > 0){
        Particles->setParticlesShape(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_use_images" && Values.size() > 0){
        Particles->setUseImageAsParticles(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_source"){
        Particles->switchUseImageAsParticles();
    }
    else if(attribute == "set_use_random_colors" && Values.size() > 0){
        Particles->setUseRandomColors(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_random_colors"){
        Particles->switchUseRandomColors();
    }
    else if(attribute == "set_is_static" && Values.size() > 0){
        Particles->setIsModuleStatic(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_drawing_details" && Values.size() > 0){
        Particles->setIsDrawingWithDetails(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_block_spawning" && Values.size() > 0){
        Particles->setBlockParticlesSpawn(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_spawning" && Values.size() > 0){
        Particles->setCanParticlesSpawn(Values[0].getBoolUnsafe());
    }
    else if(attribute == "allow_spawning"){
        Particles->allowSpawning();
    }
    else if(attribute == "set_spawn_rate" && Values.size() > 1){
        Particles->setParticlesPerSpawn(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_spawn_cap" && Values.size() > 0){
        Particles->setMaxParticlesCount(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_max_time_to_spawn" && Values.size() > 0){
        Particles->setMaxTimeToSpawn(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "bind_key" && Values.size() > 0){
        Particles->setSpawnKeyBind(Values[0].getStringUnsafe()[0]);
    }
    else if(attribute == "bind_key_by_int" && Values.size() > 0){
        Particles->setSpawnKeyBindShort(Values[0].getIntUnsafe());
    }
    else if(attribute == "spawn_on_key_release" && Values.size() > 0){
        Particles->setSpawnOnKeyRelease(Values[0].getBoolUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Particles->control(attribute, temp, Values.size());
    }
}
void EventModule::controlVariables(VariableModule * Variable, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Variable->setID(Values[0].getStringUnsafe(), &IDs);
    }
    else if(attribute == "toggle_bool"){
        Variable->toggleBool();
    }
    else if(attribute == "set_bool_random"){
        if(rand() % 2 == 0)
        Variable->toggleBool();
    }
    else if(attribute == "set_bool_probability" && Values.size() >= 3){
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
    }
    else if(attribute == "set_bool_probability" && Values.size() >= 2){
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
    else if(attribute == "set_bool" && Values.size() >= 1){
        if(Values[0].getDoubleUnsafe() == 0){
            Variable->setBool(false);
        }
        if(Values[0].getDoubleUnsafe() == 1){
            Variable->setBool(true);
        }
    }
    else if(attribute == "set_int" && Values.size() >= 1){
        Variable->setInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_int_interval" && Values.size() >= 2){
        Variable->setInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "set_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "set_double" && Values.size() >= 1){
        Variable->setDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "set_double_interval" && Values.size() >= 2){
        Variable->setDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "set_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setDouble(Values[dice].getDoubleUnsafe());
    }
    else if(attribute == "set_string" && Values.size() >= 1){
        Variable->setString(Values[0].getString());
    }
    else if(attribute == "set_string_random" && Values.size() >= 1){
        cout << "hello ";
        unsigned int dice = rand() % Values.size();
        cout << Variable->setString(Values[dice].getString()) << " ";
        cout << Variable->getString() << "\n";
    }
    else if(attribute == "add_int" && Values.size() >= 1){
        Variable->addInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_int_interval" && Values.size() >= 2){
        Variable->addInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "add_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "add_double" && Values.size() >= 1){
        Variable->addDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "add_double_interval" && Values.size() >= 2){
        Variable->addDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "add_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addDouble(Values[dice].getDoubleUnsafe());
    }
    else{
        cerr << "Error: In " << __FUNCTION__ << ": function " << attribute << "<" << Values.size() << "> does not exist.\n";
    }
}
void EventModule::controlScrollbar(ScrollbarModule * Scrollbar, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Scrollbar->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        Scrollbar->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "resize" && Values.size() >= 2){
        Scrollbar->setSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_thumb_position" && Values.size() >= 2){
        Scrollbar->setThumbPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_thumb_size" && Values.size() >= 2){
        Scrollbar->setThumbSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_scrolling_area" && Values.size() >= 2){
        Scrollbar->setScrollingArea(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_real_scrolling_area" && Values.size() >= 2){
        Scrollbar->setRealScrollingArea(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "add_real_scrolling_area" && Values.size() >= 2){
        Scrollbar->addRealScrollingArea(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_thumb_image_id" && Values.size() >= 1){
        Scrollbar->setThumbImageID(Values[0].getString());
    }
    else if(attribute == "set_track_image_id" && Values.size() >= 1){
        Scrollbar->setTrackImageID(Values[0].getString());
    }
    else if(attribute == "set_can_be_drawn" && Values.size() >= 1){
        Scrollbar->canBeDrawn = Values[0].getBool();
    }
    else if(attribute == "set_mouse_wheel_speed" && Values.size() >= 1){
        Scrollbar->mouseWheelSpeed = Values[0].getDouble();
    }
    else if(attribute == "scroll_to_the_beginning"){
        Scrollbar->scrollToTheBeginning();
    }
    else if(attribute == "scroll_to_the_end"){
        Scrollbar->scrollToTheEnd();
    }
    else if(attribute == "scroll_by_distance" && Values.size() >= 2){
        Scrollbar->scrollByDistance(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "add_camera" && Values.size() >= 1){
        addUniqueToStringVector(Scrollbar->cameraIDs, Values[0].getString());
    }
    else if(attribute == "remove_camera" && Values.size() >= 1){
        removeFromStringVector(Scrollbar->cameraIDs, Values[0].getString());
    }
    else if(attribute == "clear_cameras" && Values.size() >= 1){
        Scrollbar->cameraIDs.clear();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Scrollbar->control(attribute, temp, Values.size());
    }
}
void EventModule::controlPrimitives(PrimitivesModule * Primitives, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "update_with_size"){
        Primitives->updateWithSize();
    }
    else if(attribute == "set_id" && Values.size() > 0){
        Primitives->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_pos" && Values.size() >= 2){
        Primitives->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Primitives->setSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_type" && Values.size() >= 1){
        Primitives->type = getPrimitiveType(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_point" && Values.size() >= 2){
        Primitives->points.push_back(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        Primitives->color = al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "set_thickness" && Values.size() >= 1){
        Primitives->thickness = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_radius" && Values.size() >= 1){
        Primitives->radius = Values[0].getDoubleUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Primitives->control(attribute, temp, Values.size());
    }
}
void EventModule::controlVector(VectorModule * Vector, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Vector->setID(Values[0].getStringUnsafe(), &IDs);
    }
    else if(attribute == "push_back" && Values.size() >= 1){
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
    }
    else if(attribute == "pop_back"){
        Vector->popBack();
    }
    else if(attribute == "clear"){
        Vector->clear();
    }
    else if(attribute == "remove" && Values.size() >= 1){
        Vector->removeIndex(Values[0].getIntUnsafe());
    }
    else if(attribute == "set" && Values.size() >= 2){
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
    }
    else{
        cerr << "Error: In " << __FUNCTION__ << ": Function " << attribute << "<" << Values.size() << "> does not exist.\n";
    }
}

void EventModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    if(attribute == "else_child_id"){
        BasePointers.push_back(BasePointersStruct());
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
    if(source != "" && source != "_"){
        if(dynamicID != ""){
            cout << ".";
        }
        cout << source;
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
    if(moduleType != "" && moduleType != "_"){
        cout << "." << moduleType;
    }
    if(moduleID != "" && moduleID != "_"){
        cout << "." << moduleID;
    }
    if(attribute != "" && attribute != "_"){
        cout << "." << attribute;
    }
    if(spareID != "" && spareID != "_"){
        cout << "." << spareID;
    }
    cout << " ";
}