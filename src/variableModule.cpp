#include "variableModule.h"

VariableModule::VariableModule(unsigned int newID){
    ID = std::to_string(newID);
    type = 'n';
    vInt = 0;
    defaultInt = 0;
    vDouble = 0.0;
    defaultDouble = 0.0;
    vString = "";
    defaultString = "";
}
string VariableModule::getID(){
    return ID;
}
string & VariableModule::getIDAddr(){
    return ID;
}
char VariableModule::getType(){
    return type;
}
bool VariableModule::getDefaultBool(){
    return defaultBool;
}
int VariableModule::getDefaultInt(){
    return defaultInt;
}
double VariableModule::getDefaultDouble(){
    return defaultDouble;
}
string VariableModule::getDefaultString(){
    return defaultString;
}
bool VariableModule::getBool(){
    return vBool;
}
int VariableModule::getInt(){
    return vInt;
}
double VariableModule::getDouble(){
    return vDouble;
}
string VariableModule::getString(){
    return vString;
}
void VariableModule::setID(string newID){
    ID = newID;
}
bool VariableModule::setType(char newType){
    if(isCharInGroup(newType, 4, 'b', 'i', 'd', 's')){
        type = newType;
        return true;
    }
    return false;
}
bool VariableModule::tryToSetType(char newType){
    if(type != 'n' && type != newType){
        std::cout << "Error [VariableModule]: You can't change the type of already initialized variable.\n";
        return false;
    }
    type = newType;
    return true;
}
bool VariableModule::setDefaultBool(bool newValue){
    if(tryToSetType('b')){
        defaultBool = newValue;
        vBool = defaultBool;
        return true;
    }
    return false;
}
bool VariableModule::toggleDefaultBool(){
    if(tryToSetType('b')){
        defaultBool = !defaultBool;
        vBool = defaultBool;
        return true;
    }
    return false;
}
bool VariableModule::setDefaultInt(int newValue){
    if(tryToSetType('i')){
        defaultInt = newValue;
        vInt = defaultInt;
        return true;
    }
    return false;
}
bool VariableModule::setDefaultDouble(double newValue){
    if(tryToSetType('d')){
        defaultDouble = newValue;
        vDouble = newValue;
        return true;
    }
    return false;
}
bool VariableModule::setDefaultString(string newValue){
    if(tryToSetType('s')){
        defaultString = newValue;
        vString = newValue;
        return true;
    }
    return false;
}
bool VariableModule::setBool(bool newValue){
    if(tryToSetType('b')){
        vBool = newValue;
        return true;
    }
    return false;
}
bool VariableModule::toggleBool(){
    if(tryToSetType('b')){
        vBool = !vBool;
        return true;
    }
    return false;
}
bool VariableModule::setInt(int newValue){
    if(tryToSetType('i')){
        vInt = newValue;
        return true;
    }
    return false;
}
bool VariableModule::setDouble(double newValue){
    if(tryToSetType('d')){
        vDouble = newValue;
        return true;
    }
    return false;
}
bool VariableModule::setString(string newValue){
    if(tryToSetType('s')){
        vString = newValue;
        return true;
    }
    return false;
}
bool VariableModule::addDefaultInt(int newValue){
    return setDefaultInt(getDefaultInt()+newValue);
}
bool VariableModule::addDefaultDouble(double newValue){
    return setDefaultDouble(getDefaultDouble()+newValue);
}
bool VariableModule::addInt(int newValue){
    return setInt(getInt()+newValue);
}
bool VariableModule::addDouble(double newValue){
    return setDouble(getDouble()+newValue);
}
void VariableModule::resetValue(){
    if(type == 'b'){
        vBool = defaultBool;
    }
    else if(type == 'i'){
        vInt = defaultInt;
    }
    else if(type == 'd'){
        vDouble = defaultDouble;
    }
    else if(type == 's'){
        vString = defaultString;
    }
}

template <typename condValueType>
bool VariableModule::isConditionMet(condValueType condVal, string operatorType, char valType){
    if(type != valType){
        std::cout << "Error [VariableModule]: Comparison of two different variable types.\n";
        return false;
    }
    if(operatorType == "=="){
        if(valType == 'b'){
            return vBool == condVal;
        }
        else if(valType == 'i'){
            return vInt == condVal;
        }
        else if(valType == 'd'){
            return vDouble == condVal;
        }
    }
    else if(operatorType == "!="){
        if(valType == 'b'){
            return vBool != condVal;
        }
        else if(valType == 'i'){
            return vInt != condVal;
        }
        else if(valType == 'd'){
            return vDouble != condVal;
        }
    }
    else if(operatorType == ">"){
        if(valType == 'b'){
            return vBool > condVal;
        }
        else if(valType == 'i'){
            return vInt > condVal;
        }
        else if(valType == 'd'){
            return vDouble > condVal;
        }
    }
    else if(operatorType == "<"){
        if(valType == 'b'){
            return vBool < condVal;
        }
        else if(valType == 'i'){
            return vInt < condVal;
        }
        else if(valType == 'd'){
            return vDouble < condVal;
        }
    }
    else if(operatorType == ">="){
        if(valType == 'b'){
            return vBool >= condVal;
        }
        else if(valType == 'i'){
            return vInt >= condVal;
        }
        else if(valType == 'd'){
            return vDouble >= condVal;
        }
    }
    else if(operatorType == "<="){
        if(valType == 'b'){
            return vBool <= condVal;
        }
        else if(valType == 'i'){
            return vInt <= condVal;
        }
        else if(valType == 'd'){
            return vDouble <= condVal;
        }
    }
    return false;
}
bool VariableModule::isConditionMet(string condVal, string operatorType, char valType){
    if(type != valType || valType != 's'){
        std::cout << "Error [VariableModule]: Comparison of two different variable types.\n";
        return false;
    }
    if(operatorType == "=="){
        return vString == condVal;
    }
    else if(operatorType == "!="){
        return vString != condVal;
    }
    else if(operatorType == ">"){
        return vString > condVal;
    }
    else if(operatorType == "<"){
        return vString < condVal;
    }
    else if(operatorType == ">="){
        return vString >= condVal;
    }
    else if(operatorType == "<="){
        return vString <= condVal;
    }
    return false;
}

bool VariableModule::isConditionMet(VariableModule * OtherVariable, string operatorType){
    if(type != OtherVariable->getType()){
        std::cout << "Error [VariableModule]: Comparison of two different variable types.\n";
        return false;
    }
    if(OtherVariable->getType() == 'b'){
        return isConditionMet(OtherVariable->getBool(), operatorType, OtherVariable->getType());
    }
    else if(OtherVariable->getType() == 'i'){
        return isConditionMet(OtherVariable->getInt(), operatorType, OtherVariable->getType());
    }
    else if(OtherVariable->getType() == 'd'){
        return isConditionMet(OtherVariable->getDouble(), operatorType, OtherVariable->getType());
    }
    else if(OtherVariable->getType() == 's'){
        return isConditionMet(OtherVariable->getString(), operatorType, OtherVariable->getType());
    }

    return false;
}

VariableModule & VariableModule::operator=(const VariableModule& original){
    ID = original.ID;
    type = original.type;
    if(type == 'b'){
        vBool = original.vBool;
    }
    else if(type == 'i'){
        vInt = original.vInt;
    }
    else if(type == 'd'){
        vDouble = original.vDouble;
    }
    else if(type == 's'){
        vString = original.vString;
    }
    return *this;
}