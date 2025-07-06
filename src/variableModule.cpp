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

VariableModule::VariableModule(){}
VariableModule::VariableModule(bool value){
    type = 'b';
    vBool = value;
}
VariableModule::VariableModule(int value){
    type = 'i';
    vInt = value;
}
VariableModule::VariableModule(double value){
    type = 'd';
    vDouble = value;
}
VariableModule::VariableModule(string value){
    type = 's';
    vString = value;
}
VariableModule::VariableModule(const VariableModule &Original){
    deleted = Original.deleted;
    ID = Original.ID;
    layerID = Original.layerID;
    objectID = Original.objectID;
    type = Original.type;
    if(deleted){
        return;
    }
    if(type == 'b'){
        vBool = Original.vBool;
    }
    else if(type == 'i'){
        vInt = Original.vInt;
    }
    else if(type == 'd'){
        vDouble = Original.vDouble;
    }
    else if(type == 's'){
        vString = Original.vString;
    }
}
VariableModule & VariableModule::operator=(const VariableModule& Original){
    deleted = Original.deleted;
    ID = Original.ID;
    layerID = Original.layerID;
    objectID = Original.objectID;
    type = Original.type;
    if(deleted){
        return *this;
    }
    switch(type){
        case 'b':
            vBool = Original.vBool;
            break;
        case 'i':
            vInt = Original.vInt;
            break;
        case 'd':
            vDouble = Original.vDouble;
            break;
        case 's':
            vString = Original.vString;
            break;
        default:
            break;
    }
    return *this;
}
void VariableModule::copyValue(const VariableModule& Original){
    type = Original.type;
    switch(type){
        case 'b':
            vBool = Original.vBool;
            break;
        case 'i':
            vInt = Original.vInt;
            break;
        case 'd':
            vDouble = Original.vDouble;
            break;
        case 's':
            vString = Original.vString;
            break;
        default:
            break;
    }
}
void VariableModule::copyValue(const VariableModule * Original){
    type = Original->type;
    switch(type){
        case 'b':
            vBool = Original->vBool;
            break;
        case 'i':
            vInt = Original->vInt;
            break;
        case 'd':
            vDouble = Original->vDouble;
            break;
        case 's':
            vString = Original->vString;
            break;
        default:
            break;
    }
}
VariableModule::~VariableModule(){

}
void VariableModule::deleteLater(){
    deleted = true;
}
bool VariableModule::getIsDeleted() const{
    return deleted;
}
void VariableModule::clear(){
    switch(type){
        case 'b':
            vBool = false;
            break;
        case 'i':
            vInt = 0;
            break;
        case 'd':
            vDouble = 0.0;
            break;
        case 's':
            vString = "";
            break;
        default:
            break;
    }
    type = 'n';
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
string &VariableModule::getIDAddr(){
    return ID;
}
string VariableModule::getAnyValue() const{
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
    return "<invalid_type>";
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
    return "null";
}
bool VariableModule::getBool() const{
    if(type == 'i'){
        return vInt > 0;
    }
    else if(type == 'd'){
        return vDouble > 0;
    }
    else if(type != 'b'){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ":\n\t You can't access boolean variable.\n";
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
    if(type == 'b'){
        return vBool;
    }
    else if(type == 'd'){
        //cout << "Warning [VariableModule]: floating point ignored.\n";
        return vDouble;
    }
    else if(type != 'i'){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ":\n\t You can't access int variable.\n";
        return 0;
    }
    return vInt;
}
int VariableModule::getIntUnsafe() const{
    if(type == 'b'){
        return vBool;
    }
    else if(type == 'd'){
        return vDouble;
    }
    else if(type != 'i'){
        return 0;
    }
    return vInt;
}
double VariableModule::getDouble() const{
    if(type == 'i'){
        //cout << "Warning [VariableModule]: No floating point.\n";
        return vInt;
    }
    else if(type != 'd'){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ":\n\t You can't access double variable.\n";
        return 0.0;
    }
    return vDouble;
}
double VariableModule::getDoubleUnsafe() const{
    if(type == 'i'){
        return vInt;
    }
    else if(type == 's'){
        string error;
        double temp = cstod(vString, error);
        if(error.size() > 0){
            cerr << "Error: In VariableModule '" << ID << "': In " << __FUNCTION__ << ":\n" << error << "\n";
        }
        return temp;
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
    
    cerr << "Error: In " << __PRETTY_FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
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
void VariableModule::setID(string newID, vector<string> *listOfIDs){
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
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
void VariableModule::setIdUnsafe(string newID){
    ID = newID;
}
void VariableModule::setLayerID(string newID){
    layerID = newID;
}
void VariableModule::setObjectID(string newID){
    objectID = newID;
}
bool VariableModule::setType(char newType){
    if(isCharInGroup(newType, 4, 'b', 'i', 'd', 's')){
        type = newType;
        return true;
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Variable type '" << newType << "' does not exist.\n";
    return false;
}
bool VariableModule::tryToSetType(char newType){
    if(type != 'n' && type != newType){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": You can't change the type of already initialized variable.\n";
        return false;
    }
    if(!isCharInGroup(newType, 4, 'b', 'i', 'd', 's')){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Variable type '" << newType << "' does not exist.\n";
        return false;
    }
    type = newType;
    return true;
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
bool VariableModule::addInt(int newValue){
    return setInt(getInt()+newValue);
}
bool VariableModule::addDouble(double newValue){
    return setDouble(getDouble()+newValue);
}
bool VariableModule::addString(string newValue){
    return setString(getString()+newValue);
}
void VariableModule::addStringUnsafe(string newValue){
    vString += newValue;
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
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ":\n\t You can't negate a string.\n";
    }
    else{
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ":\n\t You can't negate the value of already not-initialized variable.\n";
    }
}
void VariableModule::getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.emplace_back(BasePointersStruct());
    switch(attribute){
        case id:
            if(isStringInVector(reservedIDs, ID)){
                cerr << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << ID << "\' address was denied.\n";
                BasePointers.pop_back();
                return;
            }
            BasePointers.back().setPointer(&ID);
            BasePointers.back().readOnly = true;
            return;
        case type_a:
            BasePointers.back().setPointer(&type);
            return;
        case bool_a:
            BasePointers.back().setPointer(&vBool);
            return;
        case int_a:
            BasePointers.back().setPointer(&vInt);
            return;
        case double_a:
            BasePointers.back().setPointer(&vDouble);
            return;
        case string_a:
            BasePointers.back().setPointer(&vString);
            return;
        default:
            BasePointers.pop_back();
            cerr << "Error: In " << __FUNCTION__ << ": Attribute '"
                << attributeToStr(attribute) << "' is not valid.\n";
            return;
    }
}

template <typename condValueType>
bool VariableModule::isConditionMet(condValueType condVal, EngineInstr operatorType, char valType){
    if(operatorType == EngineInstr::or_i){
        if(valType == 'b'){
            return getBoolUnsafe() || condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() || condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() || condVal;
        }
    }
    else if(operatorType == EngineInstr::and_i){
        if(valType == 'b'){
            return getBoolUnsafe() && condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() && condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() && condVal;
        }
    }
    else if(operatorType == EngineInstr::equal){
        if(valType == 'b'){
            return getBoolUnsafe() == condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() == condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() == condVal;
        }
    }
    else if(operatorType == EngineInstr::not_equal){
        if(valType == 'b'){
            return getBoolUnsafe() != condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() != condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() != condVal;
        }
    }
    else if(operatorType == EngineInstr::more){
        if(valType == 'b'){
            return getBoolUnsafe() > condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() > condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() > condVal;
        }
    }
    else if(operatorType == EngineInstr::less){
        if(valType == 'b'){
            return getBoolUnsafe() < condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() < condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() < condVal;
        }
    }
    else if(operatorType == EngineInstr::more_equal){
        if(valType == 'b'){
            return getBoolUnsafe() >= condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() >= condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() >= condVal;
        }
    }
    else if(operatorType == EngineInstr::less_equal){
        if(valType == 'b'){
            return getBoolUnsafe() <= condVal;
        }
        else if(valType == 'i'){
            return getIntUnsafe() <= condVal;
        }
        else if(valType == 'd'){
            return getDoubleUnsafe() <= condVal;
        }
    }
    return false;
}
bool VariableModule::isConditionMet(const string & condVal, EngineInstr operatorType, char valType){
    if(type != valType || valType != 's'){
        cerr << "Error: In " << __FUNCTION__ << ": Comparison of two different variable types.\n";
        return false;
    }
    switch(operatorType){
        case equal:
            return vString == condVal;
        case not_equal:
            return vString != condVal;
        case more:
            return vString > condVal;
        case less:
            return vString < condVal;
        case more_equal:
            return vString >= condVal;
        case less_equal:
            return vString <= condVal;
        default:
            break;
    }
    return false;
}
bool VariableModule::isConditionMet(EngineInstr operatorType, VariableModule * OtherVariable){
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
    
    cerr << "Error: In " << __FUNCTION__ << ": Invalid comparison: " << ID << ":" << type << ":" << getAnyValue()
        << " " << instrToStr(operatorType) << " " << OtherVariable->getID() << ":" << OtherVariable->getType() << ":" << OtherVariable->getAnyValue() << "\n";
    
    return false;
}
bool VariableModule::isConditionMet(EngineInstr operatorType, const BasePointersStruct & OtherVariable){
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
    else if(type == 's' && (OtherVariable.type == string_bt || OtherVariable.type == char_bt)){
        return isConditionMet(OtherVariable.getString(), operatorType, type);
    }
    
    cerr << "Error: " << __FUNCTION__ << ": Invalid types.\n";
    
    return false;
}
double VariableModule::floatingOperation(EngineInstr operatorType, VariableModule * OtherVariable){
    if(type == 's' || OtherVariable->getType() == 's'){
        cerr << "Error: " << __PRETTY_FUNCTION__ << ": Cannot use string variable in arithmetic operation.\n";
        return false;
    }
    if(operatorType == EngineInstr::add){
        return getDouble() + OtherVariable->getDouble();
    }
    else if(operatorType == EngineInstr::sub){
        return getDouble() - OtherVariable->getDouble();
    }
    else if(operatorType == EngineInstr::mul){
        return getDouble() * OtherVariable->getDouble();
    }
    else if(operatorType == EngineInstr::div_i){
        return getDouble() / OtherVariable->getDouble();
    }
    else if(operatorType == EngineInstr::mod){
        return int(getDouble()) % int(OtherVariable->getDouble());
    }
    else if(operatorType == EngineInstr::pow_i){
        return pow(getDouble(), OtherVariable->getDouble());
    }
    cerr << "Error: " << __PRETTY_FUNCTION__ << ": Operator '" << instrToStr(operatorType) << "' is not valid.\n";
    return 0.0;
}
double VariableModule::floatingOperation(EngineInstr operatorType, BasePointersStruct * RightOperand){
    if(type == 's' || RightOperand->type == string_bt){
        cerr << "Error: " << __PRETTY_FUNCTION__ << ": Cannot use string variable in the arithmetic operation.\n";
        return false;
    }
    if(operatorType == EngineInstr::add){
        return getDouble() + RightOperand->getDouble();
    }
    else if(operatorType == EngineInstr::sub){
        return getDouble() - RightOperand->getDouble();
    }
    else if(operatorType == EngineInstr::mul){
        return getDouble() * RightOperand->getDouble();
    }
    else if(operatorType == EngineInstr::div_i){
        return getDouble() / RightOperand->getDouble();
    }
    else if(operatorType == EngineInstr::mod){
        return int(getDouble()) % int(RightOperand->getDouble());
    }
    else if(operatorType == EngineInstr::pow_i){
        return pow(getDouble(), RightOperand->getDouble());
    }
    cerr << "Error: " << __PRETTY_FUNCTION__ << ": Operator '" << instrToStr(operatorType) << "' is not valid.\n";
    return 0.0;
}
int VariableModule::intOperation(EngineInstr operatorType, VariableModule * OtherVariable){
    if(type == 's' || OtherVariable->getType() == 's'){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot use string variable in arithmetic operation.\n";
        return false;
    }
    if(operatorType == EngineInstr::add){
        return getInt() + OtherVariable->getInt();
    }
    else if(operatorType == EngineInstr::sub){
        return getInt() - OtherVariable->getInt();
    }
    else if(operatorType == EngineInstr::mul){
        return getInt() * OtherVariable->getInt();
    }
    else if(operatorType == EngineInstr::div_i){
        return getInt() / OtherVariable->getInt();
    }
    else if(operatorType == EngineInstr::mod){
        return int(getInt()) % int(OtherVariable->getInt());
    }
    else if(operatorType == EngineInstr::pow_i){
        return pow(getInt(), OtherVariable->getInt());
    }
    cerr << "Error: " << __PRETTY_FUNCTION__ << ": Operator '" << instrToStr(operatorType) << "' is not valid.\n";
    return 0;
}
int VariableModule::intOperation(EngineInstr operatorType, BasePointersStruct * RightOperand){
    if(type == 's' || RightOperand->type == string_bt){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot use string variable in the arithmetic operation.\n";
        return false;
    }
    if(operatorType == EngineInstr::add){
        return getInt() + RightOperand->getInt();
    }
    else if(operatorType == EngineInstr::sub){
        return getInt() - RightOperand->getInt();
    }
    else if(operatorType == EngineInstr::mul){
        return getInt() * RightOperand->getInt();
    }
    else if(operatorType == EngineInstr::div_i){
        return getInt() / RightOperand->getInt();
    }
    else if(operatorType == EngineInstr::mod){
        return int(getInt()) % int(RightOperand->getInt());
    }
    else if(operatorType == EngineInstr::pow_i){
        return pow(getInt(), RightOperand->getInt());
    }
    cerr << "Error: " << __PRETTY_FUNCTION__ << ": Operator '" << instrToStr(operatorType) << "' is not valid.\n";
    return 0;
}
string VariableModule::stringOperation(EngineInstr operatorType, VariableModule * OtherVariable){
    if(type != 's' || OtherVariable->getType() != 's'){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Two variables must be of string type.\n";
        return "";
    }
    if(operatorType == EngineInstr::add){
        return getString() + OtherVariable->getString();
    }
    cerr << "Error: " << __PRETTY_FUNCTION__ << ": Operator '" << instrToStr(operatorType) << "' is not valid.\n";
    return 0;
}
string VariableModule::stringOperation(EngineInstr operatorType, BasePointersStruct * RightOperand){
    if(type != 's' || RightOperand->type != string_bt){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Two variables must be of string type.\n";
        return "";
    }
    if(operatorType == EngineInstr::add){
        return getString() + RightOperand->getString();
    }
    cerr << "Error: " << __PRETTY_FUNCTION__ << ": Operator '" << instrToStr(operatorType) << "' is not valid.\n";
    return 0;
}
BaseVariableStruct VariableModule::getBaseVariableStruct() const{
    BaseVariableStruct Structure;

    switch (type) {
        case 'b':
            Structure.type = bool_bt;
            Structure.vBool = vBool;
            break;
        case 'i':
            Structure.type = int_bt;
            Structure.vInt = vInt;
            break;
        case 'd':
            Structure.type = double_bt;
            Structure.vDouble = vDouble;
            break;
        case 's':
            Structure.type = string_bt;
            Structure.vString = vString;
            break;
        default:
            Structure.type = null_bt;
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": type \'" << type << "\' is not a valid.\n";
            break;
    }

    return Structure;
}
BasePointersStruct VariableModule::getBasePointersStruct(){
    BasePointersStruct Structure;

    switch (type) {
        case 'b':
            Structure.type = bool_bt;
            Structure.pBool = &vBool;
            break;
        case 'i':
            Structure.type = int_bt;
            Structure.pInt = &vInt;
            break;
        case 'd':
            Structure.type = double_bt;
            Structure.pDouble = &vDouble;
            break;
        case 's':
            Structure.type = string_bt;
            Structure.pString = &vString;
            break;
        default:
            Structure.type = null_bt;
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Type \'" << type << "\' is not a valid.\n";
            break;
    }

    return Structure;
}
template<typename LeftType, typename RightType>
void executeMoveTypeInstruction(LeftType * LeftOperand, RightType * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo){
    if(LeftOperand == nullptr){
        cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
            << "Left operand is null.\n";
        return;
    }
    if(instruction == inc){
        (*LeftOperand)++;
        return;
    } 
    if(instruction == dec){
        (*LeftOperand)--;
        return;
    }
    if(RightOperand == nullptr){
        cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
            << "Right operand is null.\n";
        return;
    }
    switch(instruction){
        case move:
        case clone_i:
            *LeftOperand = *RightOperand;
            return;
        case add_move:
            *LeftOperand += *RightOperand;
            return;
        case sub_move:
            *LeftOperand -= *RightOperand;
            return;
        case mul_move:
            *LeftOperand *= *RightOperand;
            return;
        case div_move:
            if(*RightOperand != 0) [[likely]]{
                *LeftOperand /= *RightOperand;
            }
            else{
                cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                    << "Cannot divide by zero.\n";
            }
            return;
        default:
            cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                << "Instruction \'" << instrToStr(instruction) << "\' is not valid.\n";
            return;
    }
}
template<typename RightType>
void VariableModule::moveFromTemp(RightType * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo){
    if(type == 'b'){
        short temp = vBool;
        executeMoveTypeInstruction(&temp, RightOperand, instruction, CurrentInstrInfo);
        vBool = temp > 0;
    }
    else if(type == 'i'){
        executeMoveTypeInstruction(&vInt, RightOperand, instruction, CurrentInstrInfo);
    }
    else if(type == 'd'){
        executeMoveTypeInstruction(&vDouble, RightOperand, instruction, CurrentInstrInfo);
    }
    else{
        cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
            << "About the left operand: \'" << type << "\' type is not valid.\n";
    }
}
void VariableModule::move(VariableModule *RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo){
    if(type == 's' || RightOperand->type == 's'){
        if(type == 's'){
            if(instruction == EngineInstr::move){
                vString = RightOperand->getString();
            }
            else if(instruction == EngineInstr::add_move){
                vString += RightOperand->getString();
            }
            else{
                cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                    << "In variable '" << ID
                    << "': Cannot execute \'" << instrToStr(instruction)
                    << "\' instruction on string type values.\n";
            }
        }
        else{
            cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                << "Cannot execute any instructions if only the right operand is of a string type.\n";
        }
    }
    else if(RightOperand->type == 'b'){
        int temp = RightOperand->vBool;
        moveFromTemp(&temp, instruction, CurrentInstrInfo);
    }
    else if(RightOperand->type == 'i'){
        moveFromTemp(&RightOperand->vInt, instruction, CurrentInstrInfo);
    }
    else if(RightOperand->type == 'd'){
        moveFromTemp(&RightOperand->vDouble, instruction, CurrentInstrInfo);
    }
    else if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveFromTemp((int*)nullptr, instruction, CurrentInstrInfo);
    }
    else{
        cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
            << "In variable '" << ID
            << "': About the right operand: \'" << RightOperand->type << "\' type is not valid.\n";
    }
}
void VariableModule::move(const BasePointersStruct *RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo){
    if(type == 'n'){
        return;
    }
    switch (RightOperand->type) {
        case bool_bt:{
            char temp = *RightOperand->pBool;
            moveFromTemp(&temp, instruction, CurrentInstrInfo);}
            break;
        case char_bt:
            moveFromTemp(RightOperand->pChar, instruction, CurrentInstrInfo);
            break;
        case short_bt:
            moveFromTemp(RightOperand->pShort, instruction, CurrentInstrInfo);
            break;
        case u_short_bt:
            moveFromTemp(RightOperand->pUShort, instruction, CurrentInstrInfo);
            break;
        case int_bt:
            moveFromTemp(RightOperand->pInt, instruction, CurrentInstrInfo);
            break;
        case u_int_bt:
            moveFromTemp(RightOperand->pUInt, instruction, CurrentInstrInfo);
            break;
        case float_bt:
            moveFromTemp(RightOperand->pFloat, instruction, CurrentInstrInfo);
            break;
        case double_bt:
            moveFromTemp(RightOperand->pDouble, instruction, CurrentInstrInfo);
            break;
        case string_bt:
            if(type == 's'){
                if(instruction == EngineInstr::move){
                    vString = RightOperand->getString();
                }
                else if(instruction == EngineInstr::add_move){
                    vString += RightOperand->getString();
                }
                else{
                    cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                        << "In variable '" << ID
                        << "': Cannot execute \'" << instrToStr(instruction)
                        << "\' instruction on string type values.\n";
                }
            }
            else{
                cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                    << "In variable '" << ID
                    << "': Cannot execute any instructions if only the right operand is of a string type.\n";
            }
            break;
        default:
            if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
                moveFromTemp((int*)nullptr, instruction, CurrentInstrInfo);
            }
            else{
                cerr << instructionError(CurrentInstrInfo, __FUNCTION__)
                << "In variable '" << ID
                << "': About the right operand: \'" << RightOperand->type << "\' type is not valid.\n";
            }
            break;
    }
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
        cerr << "Error: In " << __FUNCTION__ << ": \'" << newType << "\' type does not exist.\n";
    }
}
void VariableModule::setValueFromPointer(const BasePointersStruct &BasePointer){
    switch (BasePointer.type) {
        case bool_bt:
            tryToSetFromPointer(*BasePointer.pBool, 'b');
            break;
        case char_bt:
            tryToSetFromPointer(*BasePointer.pChar, 'i');
            break;
        case short_bt:
            tryToSetFromPointer(*BasePointer.pShort, 'i');
            break;
        case u_short_bt:
            tryToSetFromPointer(*BasePointer.pUShort, 'i');
            break;
        case int_bt:
            tryToSetFromPointer(*BasePointer.pInt, 'i');
            break;
        case u_int_bt:
            tryToSetFromPointer(*BasePointer.pUInt, 'i');
            break;
        case float_bt:
            tryToSetFromPointer(*BasePointer.pFloat, 'd');
            break;
        case double_bt:
            tryToSetFromPointer(*BasePointer.pDouble, 'd');
            break;
        case string_bt:
            if(type == 's' || type == 'n'){
                setString(BasePointer.getString());
            }
            else{
                cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot assign string type value to a non-string variable.\n";
            }
            break;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": About the right operand: Type \'"
                << baseTypeToStr(BasePointer.type) << "\' does not exist.\n";
            break;
    }
}

void VariableModule::set(const BaseVariableStruct &BaseVariable){
    switch (BaseVariable.type) {
        case bool_bt:
            setBool(BaseVariable.vBool);
            break;
        case char_bt:
            setString(BaseVariable.vChar);
            break;
        case short_bt:
            setInt(BaseVariable.vShort);
            break;
        case u_short_bt:
            setInt(BaseVariable.vUShort);
            break;
        case int_bt:
            setInt(BaseVariable.vInt);
            break;
        case u_int_bt:
            setInt(BaseVariable.vUInt);
            break;
        case float_bt:
            setDouble(BaseVariable.vFloat);
            break;
        case double_bt:
            setDouble(BaseVariable.vDouble);
            break;
        case string_bt:
            setString(BaseVariable.vString);
            break;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": About the right operand: Type \'"
                << baseTypeToStr(BaseVariable.type) << "\' does not exist.\n";
            break;
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
VariableModule VariableModule::newBool(bool val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    VariableModule newVariable(newID, listOfIDs, newLayerID, newObjectID);
    newVariable.setBool(val);
    return newVariable;
}
VariableModule VariableModule::newInt(int val)
{
    VariableModule newVariable;
    newVariable.setInt(val);
    return newVariable;
}
VariableModule VariableModule::newInt(int val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    VariableModule newVariable(newID, listOfIDs, newLayerID, newObjectID);
    newVariable.setInt(val);
    return newVariable;
}
VariableModule VariableModule::newDouble(double val){
    VariableModule newVariable;
    newVariable.setDouble(val);
    return newVariable;
}
VariableModule VariableModule::newDouble(double val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    VariableModule newVariable(newID, listOfIDs, newLayerID, newObjectID);
    newVariable.setDouble(val);
    return newVariable;
}
VariableModule VariableModule::newString(const string & val){
    VariableModule newVariable;
    newVariable.setString(val);
    return newVariable;
}

VariableModule VariableModule::newString(const string & val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    VariableModule newVariable(newID, listOfIDs, newLayerID, newObjectID);
    newVariable.setString(val);
    return newVariable;
}

bool VariableModule::isNumeric() const{
    return type != 'n' && type != 's';
}
