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
VariableModule::VariableModule(string newID){
    ID = newID;
    type = 'n';
    vInt = 0;
    defaultInt = 0;
    vDouble = 0.0;
    defaultDouble = 0.0;
    vString = "";
    defaultString = "";
}
VariableModule::VariableModule(){
    ID = "";
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
string VariableModule::getAnyValue(){
    if(type == 'b'){
        if(vBool){
            return "true";
        }
        return "false";
    }
    if(type == 'i'){
        return intToStr8(vInt);
    }
    if(type == 'd'){
        return doubleToStr8(vDouble);
    }
    if(type == 's'){
        return "\"" + vString + "\"";
    }
    return "";
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
    if(type == 'i'){
        return vInt > 0;
    }
    else if(type == 'd'){
        return vDouble > 0;
    }
    else if(type != 'b'){
        std::cout << "Error [VariableModule]: You can't access boolean variable.\n";
        return false;
    }
    return vBool;
}
int VariableModule::getInt(){
    if(type == 'd'){
        std::cout << "Warning [VariableModule]: floating point ignored.\n";
        return vDouble;
    }
    else if(type != 'i'){
        std::cout << "Error [VariableModule]: You can't access int variable.\n";
        return 0;
    }
    return vInt;
}
double VariableModule::getDouble(){
    if(type == 'i'){
        std::cout << "Warning [VariableModule]: no floating point.\n";
        return vInt;
    }
    else if(type != 'd'){
        std::cout << "Error [VariableModule]: You can't access double variable.\n";
        return 0;
    }
    return vDouble;
}
string VariableModule::getString(){
    if(type != 's'){
        std::cout << "Error [VariableModule]: You can't access string variable.\n";
        return "";
    }
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
void VariableModule::negate(){
    if(type == 'b'){
        toggleBool();
    }
    else if(type == 'i'){
        vInt = -vInt;
    }
    else if(type == 'd'){
        vDouble = -vDouble;
    }
    else if(type == 's'){
        std::cout << "Error [VariableModule]: You can't negate a string.\n";
    }
    else{
        std::cout << "Error [VariableModule]: You can't negate the value of already not-initialized variable.\n";
    }
}

template <typename condValueType>
bool VariableModule::isConditionMet(condValueType condVal, string operatorType, char valType){
    if(type != valType){
        std::cout << "Error [VariableModule]: Comparison of two different variable types.\n";
        return false;
    }
    if(operatorType == "||"){
        if(valType == 'b'){
            return vBool || condVal;
        }
        else if(valType == 'i'){
            return vInt || condVal;
        }
        else if(valType == 'd'){
            return vDouble || condVal;
        }
    }
    else if(operatorType == "&&"){
        if(valType == 'b'){
            return vBool && condVal;
        }
        else if(valType == 'i'){
            return vInt && condVal;
        }
        else if(valType == 'd'){
            return vDouble && condVal;
        }
    }
    else if(operatorType == "=="){
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

bool VariableModule::isConditionMet(string operatorType, VariableModule * OtherVariable){
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
double VariableModule::floatingOperation(string operatorType, VariableModule * OtherVariable){
    if(type == 's' || OtherVariable->getType() == 's'){
        std::cout << "Error [VariableModule]: You cannot use string variable in arithmetic operation.\n";
        return false;
    }
    if(operatorType == "+"){
        return getDouble() + OtherVariable->getDouble();
    }
    else if(operatorType == "-"){
        return getDouble() - OtherVariable->getDouble();
    }
    else if(operatorType == "*"){
        return getDouble() * OtherVariable->getDouble();
    }
    else if(operatorType == "/"){
        return getDouble() / OtherVariable->getDouble();
    }
    else if(operatorType == "%"){
        return int(getDouble()) % int(OtherVariable->getDouble());
    }
    else if(operatorType == "**"){
        return pow(getDouble(), OtherVariable->getDouble());
    }
    std::cout << "Error [VariableModule]: Unrecognized operator.\n";
    return 0.0;
}
int VariableModule::intOperation(string operatorType, VariableModule * OtherVariable){
    if(type == 's' || OtherVariable->getType() == 's'){
        std::cout << "Error [VariableModule]: You cannot use string variable in arithmetic operation.\n";
        return false;
    }
    if(operatorType == "+"){
        return getInt() + OtherVariable->getInt();
    }
    else if(operatorType == "-"){
        return getInt() - OtherVariable->getInt();
    }
    else if(operatorType == "*"){
        return getInt() * OtherVariable->getInt();
    }
    else if(operatorType == "/"){
        return getInt() / OtherVariable->getInt();
    }
    else if(operatorType == "%"){
        return int(getInt()) % int(OtherVariable->getInt());
    }
    else if(operatorType == "**"){
        return pow(getInt(), OtherVariable->getInt());
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