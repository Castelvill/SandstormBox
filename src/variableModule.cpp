#include "variableModule.h"

VariableModule::VariableModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    deleted = false;
    setAllIDs(newID, listOfIDs, newLayerID, newObjectID, true);
    clear();
}
VariableModule::VariableModule(unsigned newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    deleted = false;
    setAllIDs(std::to_string(newID), listOfIDs, newLayerID, newObjectID, true);
    clear();
}
VariableModule::VariableModule(){
    deleted = false;
    ID = "";
    clear();
}
void VariableModule::deleteLater(){
    deleted = true;
}
bool VariableModule::getIsDeleted() const{
    return deleted;
}
void VariableModule::clear(){
    type = 'n';
    vInt = 0;
    defaultInt = 0;
    vDouble = 0.0;
    defaultDouble = 0.0;
    vString = "";
    defaultString = "";
}
void VariableModule::clone(const VariableModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), &listOfIDs, newLayerID, newObjectID, changeOldID);
}
void VariableModule::setAllIDs(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    if(changeOldID){
        setID(newID, listOfIDs);
    }
    setLayerID(newLayerID);
    setObjectID(newObjectID);
}
string VariableModule::getID() const{
    return ID;
}
string VariableModule::getLayerID() const{
    return layerID;
}
string VariableModule::getObjectID() const{
    return objectID;
}
string &VariableModule::getIDAddr()
{
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
        return intToStr(vInt);
    }
    if(type == 'd'){
        return doubleToStr(vDouble);
    }
    if(type == 's'){
        return "\"" + vString + "\"";
    }
    return "";
}
char VariableModule::getType() const{
    return type;
}
string VariableModule::getFullTypeName(){
    if(type == 'b'){
        return "bool";
    }
    if(type == 'i'){
        return "int";
    }
    if(type == 'd'){
        return "double";
    }
    if(type == 's'){
        return "string";
    }
    return "";
}
bool VariableModule::getDefaultBool()
{
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
bool VariableModule::getBool() const{
    if(type == 'i'){
        return vInt > 0;
    }
    else if(type == 'd'){
        return vDouble > 0;
    }
    else if(type != 'b'){
        cout << "Error [VariableModule]: You can't access boolean variable.\n";
        return false;
    }
    return vBool;
}
bool VariableModule::getBoolUnsafe() const{
    if(type == 'i'){
        return vInt > 0;
    }
    else if(type == 'd'){
        return vDouble > 0;
    }
    else if(type != 'b'){
        cout << "Error [VariableModule]: You can't access boolean variable.\n";
        return false;
    }
    return vBool;
}
bool VariableModule::getBoolUnsafe(const bool & defaultValue) const{
    if(type == 'i'){
        return vInt > 0;
    }
    else if(type == 'd'){
        return vDouble > 0;
    }
    else if(type != 'b'){
        return defaultValue;
    }
    return vBool;
}
int VariableModule::getInt() const{
    if(type == 'd'){
        //cout << "Warning [VariableModule]: floating point ignored.\n";
        return vDouble;
    }
    else if(type != 'i'){
        cout << "Error [VariableModule]: You can't access int variable.\n";
        return 0;
    }
    return vInt;
}
int VariableModule::getIntUnsafe() const{
    if(type == 'd'){
        return vDouble;
    }
    else if(type != 'i'){
        return 0;
    }
    return vInt;
}
double VariableModule::getDouble() const{
    if(type == 'i'){
        cout << "Warning [VariableModule]: no floating point.\n";
        return vInt;
    }
    else if(type != 'd'){
        cout << "Error [VariableModule]: You can't access double variable.\n";
        return 0.0;
    }
    return vDouble;
}
double VariableModule::getDoubleUnsafe() const{
    if(type == 'i'){
        return vInt;
    }
    else if(type != 'd'){
        return 0.0;
    }
    return vDouble;
}
string VariableModule::getString() const{
    if(type == 's'){
        return vString;
    }
    else if(type == 'b'){
        if(vBool){
            return "true";
        }
        return "false";
    }
    else if(type == 'i'){
        return intToStr(vInt);
    }
    else if(type == 'd'){
        return doubleToStr(vDouble);
    }
    
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
    return "[invalid type]";
}
string VariableModule::getStringUnsafe() const{
    if(type == 's'){
        return vString;
    }
    else if(type == 'b'){
        if(vBool){
            return "true";
        }
        return "false";
    }
    else if(type == 'i'){
        return intToStr(vInt);
    }
    else if(type == 'd'){
        return doubleToStr(vDouble);
    }
    
    return "";
}
void VariableModule::setID(string newID, vector<string> *listOfIDs)
{
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    if(listOfIDs != nullptr){
        removeFromStringVector(*listOfIDs, ID);
        ID = findNewUniqueID(*listOfIDs, newID);
        listOfIDs->push_back(ID);
    }
    else{
        ID = newID;
    }
}
void VariableModule::setLayerID(string newID){
    layerID = newID;
}
void VariableModule::setObjectID(string newID){
    objectID = newID;
}
bool VariableModule::setType(char newType)
{
    if(isCharInGroup(newType, 4, 'b', 'i', 'd', 's')){
        type = newType;
        return true;
    }
    return false;
}
bool VariableModule::tryToSetType(char newType){
    if(type != 'n' && type != newType){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": You can't change the type of already initialized variable.\n";
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
bool VariableModule::setString(char newValue){
    if(tryToSetType('s')){
        vString[0] = newValue;
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
        cout << "Error [VariableModule]: You can't negate a string.\n";
    }
    else{
        cout << "Error [VariableModule]: You can't negate the value of already not-initialized variable.\n";
    }
}
void VariableModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "id"){
        if(isStringInVector(reservedIDs, ID)){
            cout << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << ID << "\' address was denied.\n";
            BasePointers.pop_back();
            return;
        }
        BasePointers.back().setPointer(&ID);
        BasePointers.back().readOnly = true;
    }
    else if(attribute == "type"){
        BasePointers.back().setPointer(&type);
    }
    else if(attribute == "bool"){
        BasePointers.back().setPointer(&vBool);
    }
    else if(attribute == "default_bool"){
        BasePointers.back().setPointer(&defaultBool);
    }
    else if(attribute == "int"){
        BasePointers.back().setPointer(&vInt);
    }
    else if(attribute == "default_int"){
        BasePointers.back().setPointer(&defaultInt);
    }
    else if(attribute == "double"){
        BasePointers.back().setPointer(&vDouble);
    }
    else if(attribute == "default_double"){
        BasePointers.back().setPointer(&defaultDouble);
    }
    else if(attribute == "string"){
        BasePointers.back().setPointer(&vString);
    }
    else if(attribute == "default_string"){
        BasePointers.back().setPointer(&defaultString);
    }
    else{
        BasePointers.pop_back();
        cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    }
}

template <typename condValueType>
bool VariableModule::isConditionMet(condValueType condVal, string operatorType, char valType){
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
        cout << "Error [VariableModule]: Comparison of two different variable types.\n";
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
    if(isNumeric() && OtherVariable->isNumeric()){
        if(OtherVariable->getType() == 'b'){
            return isConditionMet(OtherVariable->getBool(), operatorType, OtherVariable->getType());
        }
        else if(OtherVariable->getType() == 'i'){
            return isConditionMet(OtherVariable->getInt(), operatorType, OtherVariable->getType());
        }
        else if(OtherVariable->getType() == 'd'){
            return isConditionMet(OtherVariable->getDouble(), operatorType, OtherVariable->getType());
        }
    }
    else if(OtherVariable->getType() == 's' && type == 's'){
        return isConditionMet(OtherVariable->getString(), operatorType, OtherVariable->getType());
    }
    
    cout << "Error [VariableModule]: Invalid types.\n";
    
    return false;
}
bool VariableModule::isConditionMet(string operatorType, const BasePointersStruct & OtherVariable){
    if(OtherVariable.isNumeric() && isNumeric()){
        if(type == 'b'){
            return isConditionMet(OtherVariable.getBool(), operatorType, type);
        }
        else if(type == 'i'){
            return isConditionMet(OtherVariable.getInt(), operatorType, type);
        }
        else{
            return isConditionMet(OtherVariable.getDouble(), operatorType, type);
        }
    }
    else if(type == 's' && (OtherVariable.type == "string" || OtherVariable.type == "char")){
        return isConditionMet(OtherVariable.getString(), operatorType, type);
    }
    
    cout << "Error: " << __FUNCTION__ << ": Invalid types.\n";
    
    return false;
}
double VariableModule::floatingOperation(string operatorType, VariableModule * OtherVariable){
    if(type == 's' || OtherVariable->getType() == 's'){
        cout << "Error: " << __PRETTY_FUNCTION__ << ": You cannot use string variable in arithmetic operation.\n";
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
    cout << "Error: " << __PRETTY_FUNCTION__ << ": Unrecognized operator.\n";
    return 0.0;
}
double VariableModule::floatingOperation(string operatorType, BasePointersStruct * RightOperand){
    if(type == 's' || RightOperand->type == "string"){
        cout << "Error: " << __PRETTY_FUNCTION__ << ": You cannot use string variable in the arithmetic operation.\n";
        return false;
    }
    if(operatorType == "+"){
        return getDouble() + RightOperand->getDouble();
    }
    else if(operatorType == "-"){
        return getDouble() - RightOperand->getDouble();
    }
    else if(operatorType == "*"){
        return getDouble() * RightOperand->getDouble();
    }
    else if(operatorType == "/"){
        return getDouble() / RightOperand->getDouble();
    }
    else if(operatorType == "%"){
        return int(getDouble()) % int(RightOperand->getDouble());
    }
    else if(operatorType == "**"){
        return pow(getDouble(), RightOperand->getDouble());
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Unrecognized operator.\n";
    return 0.0;
}
int VariableModule::intOperation(string operatorType, VariableModule * OtherVariable){
    if(type == 's' || OtherVariable->getType() == 's'){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot use string variable in arithmetic operation.\n";
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
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Unrecognized operator.\n";
    return 0;
}
int VariableModule::intOperation(string operatorType, BasePointersStruct * RightOperand){
    if(type == 's' || RightOperand->type == "string"){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot use string variable in the arithmetic operation.\n";
        return false;
    }
    if(operatorType == "+"){
        return getInt() + RightOperand->getInt();
    }
    else if(operatorType == "-"){
        return getInt() - RightOperand->getInt();
    }
    else if(operatorType == "*"){
        return getInt() * RightOperand->getInt();
    }
    else if(operatorType == "/"){
        return getInt() / RightOperand->getInt();
    }
    else if(operatorType == "%"){
        return int(getInt()) % int(RightOperand->getInt());
    }
    else if(operatorType == "**"){
        return pow(getInt(), RightOperand->getInt());
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Unrecognized operator.\n";
    return 0;
}
string VariableModule::stringOperation(string operatorType, VariableModule * OtherVariable){
    if(type != 's' || OtherVariable->getType() != 's'){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Two variables must be of string type.\n";
        return "";
    }
    if(operatorType == "+"){
        return getString() + OtherVariable->getString();
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Unrecognized operator.\n";
    return 0;
}
string VariableModule::stringOperation(string operatorType, BasePointersStruct * RightOperand){
    if(type != 's' || RightOperand->type != "string"){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Two variables must be of string type.\n";
        return "";
    }
    if(operatorType == "+"){
        return getString() + RightOperand->getString();
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Unrecognized operator.\n";
    return 0;
}
BaseVariableStruct VariableModule::getVariableStruct() const{
    BaseVariableStruct Structure;

    if(type == 'b'){
        Structure.type = "bool";
        Structure.vBool = getBool();
    }
    else if(type == 'i'){
        Structure.type = "int";
        Structure.vInt = getInt();
    }
    else if(type == 'd'){
        Structure.type = "double";
        Structure.vDouble = getDouble();
    }
    else if(type == 's'){
        Structure.type = "string";
        Structure.vString = getString();
    }
    else{
        Structure.type = "";
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": \'" << type << "\' is not a valid type.\n";
    }

    return Structure;
}
template<typename LeftType, typename RightType>
void VariableModule::executeMoveTypeInstruction(LeftType * LeftOperand, RightType * RightOperand, string instruction){
    if(instruction == "++"){
        (*LeftOperand)++;
        return;
    }
    else if(instruction == "--"){
       (*LeftOperand)--;
        return;
    }
    if(LeftOperand == nullptr){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Left operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(RightOperand == nullptr){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Right operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(instruction == "=" || instruction == "clone"){
        *LeftOperand = *RightOperand;
    }
    else if(instruction == "+="){
        *LeftOperand += *RightOperand;
    }
    else if(instruction == "-="){
        *LeftOperand -= *RightOperand;
    }
    else if(instruction == "*="){
        *LeftOperand *= *RightOperand;
    }
    else if(instruction == "/="){
        if(*RightOperand != 0){
            *LeftOperand /= *RightOperand;
        }
        else{
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot divide by zero.\n";
        }
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": \'" << instruction << "\' is not a valid instruction.\n";
    }
}
template<typename RightType>
void VariableModule::moveFromTemp(RightType * RightOperand, string instruction){
    if(type == 'b'){
        short temp = vBool;
        executeMoveTypeInstruction(&temp, RightOperand, instruction);
        vBool = temp > 0;
    }
    else if(type == 'i'){
        executeMoveTypeInstruction(&vInt, RightOperand, instruction);
    }
    else if(type == 'd'){
        executeMoveTypeInstruction(&vDouble, RightOperand, instruction);
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": About the left operand: \'" << type << "\' type is not valid.\n";
    }
}
void VariableModule::move(VariableModule *RightOperand, string instruction){
    if(type == 's' || RightOperand->type == 's'){
        if(type == 's'){
            if(instruction == "="){
                vString = RightOperand->getString();
            }
            else if(instruction == "+="){
                vString += RightOperand->getString();
            }
            else{
                cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot execute \'" << instruction << "\' instruction on string type values.\n";
            }
        }
        else{
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot execute any instructions if only the right operand is of a string type.\n";
        }
    }
    else if(RightOperand->type == 'b'){
        int temp = RightOperand->vBool;
        moveFromTemp(&temp, instruction);
    }
    else if(RightOperand->type == 'i'){
        moveFromTemp(&RightOperand->vInt, instruction);
    }
    else if(RightOperand->type == 'd'){
        moveFromTemp(&RightOperand->vDouble, instruction);
    }
    else if(instruction == "++" || instruction == "--"){
        moveFromTemp((int*)nullptr, instruction);
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": About the right operand: \'" << RightOperand->type << "\' type is not valid.\n";
    }
}
void VariableModule::move(const BasePointersStruct *RightOperand, string instruction){
    if(type == 's' || RightOperand->type == "string"){
        if(type == 's'){
            if(instruction == "="){
                vString = RightOperand->getString();
            }
            else if(instruction == "+="){
                vString += RightOperand->getString();
            }
            else{
                cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot execute \'" << instruction << "\' instruction on string type values.\n";
            }
        }
        else{
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot execute any instructions if only the right operand is of a string type.\n";
        }
    }
    else if(RightOperand->type == "bool"){
        int temp = *RightOperand->pBool;
        moveFromTemp(&temp, instruction);
    }
    else if(RightOperand->type == "char" || RightOperand->type == "short" || RightOperand->type == "unsigned_short" || RightOperand->type == "int" || RightOperand->type == "unsigned_int"){
        int temp = RightOperand->getInt();
        moveFromTemp(&temp, instruction);
    }
    else if(RightOperand->type == "float" || RightOperand->type == "double"){
        double temp = RightOperand->getDouble();
        moveFromTemp(&temp, instruction);
    }
    else if(instruction == "++" || instruction == "--"){
        moveFromTemp((int*)nullptr, instruction);
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": About the right operand: \'" << RightOperand->type << "\' type is not valid.\n";
    }
}
VariableModule &VariableModule::operator=(const VariableModule &original){
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

template <typename T>
void VariableModule::tryToSetFromPointer(const T & value, char newType){
    if(newType == 'b'){
        setBool(value);
    }
    else if(newType == 'i'){
        setInt(value);
    }
    else if(newType == 'd'){
        setDouble(value);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": \'" << newType << "\' type does not exist.\n";
    }
}
void VariableModule::setValueFromPointer(const BasePointersStruct &BasePointer){
    if(type == 's' || BasePointer.type == "string"){
        if(type == 's'){
            setString(BasePointer.getString());
        }
        else{
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": You cannot assign string type value to a non-string variable.\n";
        }
    }
    else if(BasePointer.type == "bool"){
        tryToSetFromPointer(*BasePointer.pBool, 'b');
    }
    else if(BasePointer.type == "char"){
        tryToSetFromPointer(*BasePointer.pChar, 'i');
    }
    else if(BasePointer.type == "short"){
        tryToSetFromPointer(*BasePointer.pShort, 'i');
    }
    else if(BasePointer.type == "unsigned_short"){
        tryToSetFromPointer(*BasePointer.pUShort, 'i');
    }
    else if(BasePointer.type == "int"){
        tryToSetFromPointer(*BasePointer.pInt, 'i');
    }
    else if(BasePointer.type == "unsigned_int"){
        tryToSetFromPointer(*BasePointer.pUInt, 'i');
    }
    else if(BasePointer.type == "float"){
        tryToSetFromPointer(*BasePointer.pFloat, 'd');
    }
    else if(BasePointer.type == "double"){
        tryToSetFromPointer(*BasePointer.pDouble, 'd');
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": About the right operand: \'" << BasePointer.type << "\' type does not exist.\n";
    }
}

void VariableModule::set(const BaseVariableStruct &BaseVariable){
    if(BaseVariable.type == "bool"){
        setBool(BaseVariable.vBool);
    }
    else if(BaseVariable.type == "char"){
        setString(BaseVariable.vChar);
    }
    else if(BaseVariable.type == "short"){
        setInt(BaseVariable.vShort);
    }
    else if(BaseVariable.type == "unsigned_short"){
        setInt(BaseVariable.vUShort);
    }
    else if(BaseVariable.type == "int"){
        setInt(BaseVariable.vInt);
    }
    else if(BaseVariable.type == "unsigned_int"){
        setInt(BaseVariable.vUInt);
    }
    else if(BaseVariable.type == "float"){
        setDouble(BaseVariable.vFloat);
    }
    else if(BaseVariable.type == "double"){
        setDouble(BaseVariable.vDouble);
    }
    else if(BaseVariable.type == "string"){
        setString(BaseVariable.vString);
    }
}

VariableModule VariableModule::newBool(bool val){
    VariableModule newVariable;
    newVariable.setBool(val);
    return newVariable;
}
VariableModule VariableModule::newBool(bool val, string newID){
    VariableModule newVariable;
    newVariable.ID = newID;
    newVariable.setBool(val);
    return newVariable;
}
VariableModule VariableModule::newInt(int val){
    VariableModule newVariable;
    newVariable.setInt(val);
    return newVariable;
}
VariableModule VariableModule::newDouble(double val){
    VariableModule newVariable;
    newVariable.setDouble(val);
    return newVariable;
}
VariableModule VariableModule::newString(string val){
    VariableModule newVariable;
    newVariable.setString(val);
    return newVariable;
}

bool VariableModule::isNumeric() const{
    return type != 's';
}
