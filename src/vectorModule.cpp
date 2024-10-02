#include "vectorModule.h"

VectorModule::VectorModule(){
    deleted = false;
    ID = "";
    clear();
}
VectorModule::VectorModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    deleted = false;
    setAllIDs(newID, listOfIDs, newLayerID, newObjectID, true);
    clear();
}
VectorModule::VectorModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, string newType){
    deleted = false;
    setAllIDs(newID, listOfIDs, newLayerID, newObjectID, true);
    clear();
    if(newType == "null"){
        type = 'n';
    }
    else if(newType == "bool"){
        type = 'b';
    }
    else if(newType == "int"){
        type = 'i';
    }
    else if(newType == "double"){
        type = 'd';
    }
    else if(newType == "string"){
        type = 's';
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Type '" << newType << "' is not valid.\n";
    }
}
VectorModule::VectorModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, char newType){
    deleted = false;
    setAllIDs(newID, listOfIDs, newLayerID, newObjectID, true);
    clear();
    if(newType == 'n' || newType == 'b' || newType == 'i' || newType == 'd' || newType == 's'){
        type = newType;
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Type '" << newType << "' is not valid.\n";
    }
}
void VectorModule::clear(){
    type = 'n';
    vBool.clear();
    vInt.clear();
    vDouble.clear();
    vString.clear();
}
void VectorModule::setAllIDs(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, const bool &changeOldID){
    if(changeOldID){
        setID(newID, listOfIDs);
    }
    setLayerID(newLayerID);
    setObjectID(newObjectID);
}
void VectorModule::clone(const VectorModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool &changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), &listOfIDs, newLayerID, newObjectID, changeOldID);
}
void VectorModule::deleteLater(){
    deleted = true;
}
void VectorModule::setID(string newID, vector<string> *listOfIDs){
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
void VectorModule::setLayerID(string newID){
    layerID = newID;
}
void VectorModule::setObjectID(string newID){
    objectID = newID;
}
bool VectorModule::setType(char newType){
    if(type != 'n' && type != newType){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": You can't change the type of already initialized vector without clearing it first.\n";
        return false;
    }
    if(!isCharInGroup(newType, 4, 'b', 'i', 'd', 's')){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Variable type '" << newType << "' does not exist.\n";
        return false;
    }
    type = newType;
    return true;
}
bool VectorModule::pushBool(bool newValue){
    if(type == 'b' || type == 'n'){
        vBool.push_back(stupidBool(newValue));
        type = 'b';
        return true;
    }
    else if(type == 'i'){
        vInt.push_back(newValue);
        return true;
    }
    else if(type == 'd'){
        vDouble.push_back(newValue);
        return true;
    }
    else if(type == 's'){
        vString.push_back(intToStr(newValue));
        return true;
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'bool' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushBoolUnsafe(bool newValue){
    vBool.push_back(stupidBool(newValue));
}
bool VectorModule::pushInt(int newValue){
    if(type == 'b'){
        vBool.push_back(stupidBool(newValue));
        return true;
    }
    else if(type == 'i' || type == 'n'){
        vInt.push_back(newValue);
        type = 'i';
        return true;
    }
    else if(type == 'd'){
        vDouble.push_back(newValue);
        return true;
    }
    else if(type == 's'){
        vString.push_back(intToStr(newValue));
        return true;
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'int' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushIntUnsafe(int newValue){
    vInt.push_back(newValue);
}
bool VectorModule::pushDouble(double newValue){
    if(type == 'b'){
        vBool.push_back(stupidBool(newValue));
        return true;
    }
    else if(type == 'i'){
        vInt.push_back(newValue);
        return true;
    }
    else if(type == 'd' || type == 'n'){
        vDouble.push_back(newValue);
        type = 'd';
        return true;
    }
    else if(type == 's'){
        vString.push_back(doubleToStr(newValue));
        return true;
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'double' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushDoubleUnsafe(double newValue){
    vDouble.push_back(newValue);
}
bool VectorModule::pushString(string newValue){
    if(type == 's' || type == 'n'){
        vString.push_back(newValue);
        type = 's';
        return true;
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'string' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushStringUnsafe(string newValue){
    vString.push_back(newValue);
}
bool VectorModule::popBack(){
    if(type == 'b' && vBool.size() > 0){
        vBool.pop_back();
        return true;
    }
    if(type == 'i' && vInt.size() > 0){
        vInt.pop_back();
        return true;
    }
    if(type == 'd' && vDouble.size() > 0){
        vDouble.pop_back();
        return true;
    }
    if(type == 's' && vString.size() > 0){
        vString.pop_back();
        return true;
    }
    cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Cannot pop back an empty vector.\n";
    return false;
}
bool VectorModule::removeIndex(size_t index){
    if(type == 'b' && index < vBool.size()){
        vBool.erase(vBool.begin()+index);
        return true;
    }
    if(type == 'i' && index < vInt.size()){
        vInt.erase(vInt.begin()+index);
        return true;
    }
    if(type == 'd' && index < vDouble.size()){
        vDouble.erase(vDouble.begin()+index);
        return true;
    }
    if(type == 's' && index < vString.size()){
        vString.erase(vString.begin()+index);
        return true;
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index (" << index << ") is out of scope of vector '" << ID << "'.\n";
    return false;
}
bool VectorModule::setBool(size_t index, bool newValue){
    if(type == 'b' && index < vBool.size()){
        vBool[index].value = newValue;
    }
    else if(type == 'i' && index < vInt.size()){
        vInt[index] = newValue;
    }
    else if(type == 'd' && index < vDouble.size()){
        vDouble[index] = newValue;
    }
    else if(type == 's' && index < vString.size()){
        vString[index] = newValue;
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index (" << index
            << ") is out of scope of the vector " << ID << " of size " << vBool.size() << ".\n";
        return false;
    }
    return true;
}
bool VectorModule::setInt(size_t index, int newValue){
    if(type == 'b' && index < vBool.size()){
        vBool[index].value = newValue;
    }
    else if(type == 'i' && index < vInt.size()){
        vInt[index] = newValue;
    }
    else if(type == 'd' && index < vDouble.size()){
        vDouble[index] = newValue;
    }
    else if(type == 's' && index < vString.size()){
        vString[index] = newValue;
    }
    return true;
}
bool VectorModule::setDouble(size_t index, double newValue){
    if(type == 'b' && index < vBool.size()){
        vBool[index].value = newValue;
    }
    else if(type == 'i' && index < vInt.size()){
        vInt[index] = newValue;
    }
    else if(type == 'd' && index < vDouble.size()){
        vDouble[index] = newValue;
    }
    else if(type == 's' && index < vString.size()){
        vString[index] = newValue;
    }
    return true;
}
bool VectorModule::setString(size_t index, string newValue){
    if(index >= vString.size()){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index (" << index
            << ") is out of scope of the vector " << ID << " of size " << vBool.size() << ".\n";
        return false;
    }
    if(type != 's'){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot assign a string value to a vector of '" << type << "' type.\n";
        return false;
    }
    vString[index] = newValue;
    return true;
}
void VectorModule::reserve(unsigned newSize){
    if(type == 'b'){
        vBool.reserve(newSize);
    }
    else if(type == 'i'){
        vInt.reserve(newSize);
    }
    else if(type == 'd'){
        vDouble.reserve(newSize);
    }
    else if(type == 's'){
        vString.reserve(newSize);
    }
    else{
        cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Cannot reserve memory for a vector of '" << type << "' type.\n";
    }
}
void VectorModule::move(vector<VariableModule*> Variables, const InstrDescription & CurrentInstr){
    cout << instructionError(CurrentInstr, __FUNCTION__)
        << "': Not yet implemented!\n";
}

bool VectorModule::getIsDeleted() const{
    return deleted;
}
string VectorModule::getID() const{
    return ID;
}
string & VectorModule::getIDAddr(){
    return ID;
}
string VectorModule::getLayerID() const{
    return layerID;
}
string VectorModule::getObjectID() const{
    return objectID;
}
char VectorModule::getType() const{
    return type;
}
string VectorModule::getFullTypeName() const{
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
VariableModule VectorModule::getValue(string attribute, size_t index) const{
    if(attribute == "back" || attribute == "b"){
        if(type == 'b'){
            return VariableModule::newBool(getLastBool());
        }
        if(type == 'i'){
            return VariableModule::newInt(getLastInt());
        }
        if(type == 'd'){
            return VariableModule::newDouble(getLastDouble());
        }
        if(type == 's'){
            return VariableModule::newString(getLastString());
        }
        cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Type '" << type << "' is not valid for last value extraction.\n";
    }
    else if(attribute == "value" || attribute == "v"){
        if(type == 'b'){
            return VariableModule::newBool(getBool(index));
        }
        if(type == 'i'){
            return VariableModule::newInt(getInt(index));
        }
        if(type == 'd'){
            return VariableModule::newDouble(getDouble(index));
        }
        if(type == 's'){
            return VariableModule::newString(getString(index));
        }
        cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Type '" << type << "' is not valid for value extraction.\n";
    }
    else if(attribute == "size"){
        return VariableModule::newInt(getSize());
    }
    else{
        cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Attribute '" << attribute << "' does not exist in the context of Vector module.\n";
    }
    return VariableModule::newBool(false, "null");
}
vector<VariableModule> VectorModule::getValues() const{
    vector<VariableModule> Vector;
    if(type == 'b'){
        for(stupidBool value : vBool){
            Vector.push_back(VariableModule::newBool(value.value));
        }
        return Vector;
    }
    if(type == 'i'){
        for(int value : vInt){
            Vector.push_back(VariableModule::newInt(value));
        }
        return Vector;
    }
    if(type == 'd'){
        for(double value : vDouble){
            Vector.push_back(VariableModule::newDouble(value));
        }
        return Vector;
    }
    if(type == 's'){
        for(string value : vString){
            Vector.push_back(VariableModule::newString(value));
        }
        return Vector;
    }
    if(type == 'n'){
        return Vector;
    }
    cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Type '" << type << "' is not valid for last value extraction.\n";
    return Vector;
}
string VectorModule::getAnyStringValue(size_t index) const{
    if(type == 'b' && index < vBool.size()){
        if(vBool[index].value){
            return "true";
        }
        return "false";
    }
    if(type == 'i' && index < vInt.size()){
        return intToStr(vInt[index]);
    }
    if(type == 'd' && index < vDouble.size()){
        return doubleToStr(vDouble[index]);
    }
    if(type == 's' && index < vString.size()){
        return vString[index];
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return "";
}
string VectorModule::getAnyStringValueUnsafe(long index) const{
    if(type == 'b'){
        if(getBoolUnsafe(index)){
            return "true";
        }
        return "false";
    }
    if(type == 'i'){
        return intToStr(getIntUnsafe(index));
    }
    if(type == 'd'){
        return doubleToStr(getDoubleUnsafe(index));
    }
    if(type == 's'){
        return getStringUnsafe(index);
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return "";
}
vector <string> VectorModule::getAllValuesAsStringVector() const{
    vector<string> newStringVector;
    if(type == 'b'){
        for(unsigned index = 0; index < vBool.size(); index++){
            if(vBool[index].value){
                newStringVector.push_back("true");
            }
            newStringVector.push_back("false");
        }
    }
    else if(type == 'i'){
        for(unsigned index = 0; index < vInt.size(); index++){
            newStringVector.push_back(intToStr(vInt[index]));
        }
    }
    else if(type == 'd'){
        for(unsigned index = 0; index < vInt.size(); index++){
            newStringVector.push_back(doubleToStr(vDouble[index]));
        }
    }
    else if(type == 's'){
        return vString;
    }
    else{
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    }
    return newStringVector;
}
bool VectorModule::getBool(size_t index) const{
    if(index >= vBool.size()){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vBool.size() << ".\n";
        return false;
    }
    return vBool[index].value;
}
bool VectorModule::getBoolUnsafe(long index) const{
    if(vBool.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    
    if(index >= 0){
        index %= vBool.size();
    }
    else{
        index = vBool.size() - ((-index) % vBool.size());
    }

    return vBool[index].value;
}
vector<stupidBool> VectorModule::getAllBools() const{
    return vBool;
}
vector<VariableModule> VectorModule::getAllBoolsAsVariables(){
    vector<VariableModule> Values;
    for(stupidBool stupidValue : vBool){
        Values.push_back(VariableModule::newBool(stupidValue.value));
    }
    return Values;
}
int VectorModule::getInt(size_t index) const{
    if(index >= vInt.size()){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vInt.size() << ".\n";
        return 0;
    }
    return vInt[index];
}
int VectorModule::getIntUnsafe(long index) const{
    if(vInt.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0;
    }
    
    if(index >= 0){
        index %= vInt.size();
    }
    else{
        index = vInt.size() - ((-index) % vInt.size());
    }
    
    return vInt[index];
}
vector<int> VectorModule::getAllInts() const{
    return vInt;
}
vector<VariableModule> VectorModule::getAllIntsAsVariables(){
    vector<VariableModule> Values;
    for(int value : vInt){
        Values.emplace_back(VariableModule::newInt(value));
    }
    return Values;
}
double VectorModule::getDouble(size_t index) const{
    if(index >= vDouble.size()){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vDouble.size() << ".\n";
        return 0.0;
    }
    return vDouble[index];
}
double VectorModule::getDoubleUnsafe(long index) const{
    if(vDouble.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0.0;
    }
    
    if(index >= 0){
        index %= vDouble.size();
    }
    else{
        index = vDouble.size() - ((-index) % vDouble.size());
    }
    
    return vDouble[index];
}
vector<double> VectorModule::getAllDoubles() const{
    return vDouble;
}
vector<VariableModule> VectorModule::getAllDoublesAsVariables(){
    vector<VariableModule> Values;
    for(double value : vDouble){
        Values.emplace_back(VariableModule::newDouble(value));
    }
    return Values;
}
string VectorModule::getString(size_t index) const{
    if(index >= vString.size()){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vString.size() << ".\n";
        return "";
    }
    return vString[index];
}
string VectorModule::getStringUnsafe(long index) const{
    if(vString.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    
    if(index >= 0){
        index %= vString.size();
    }
    else{
        index = vString.size() - ((-index) % vString.size());
    }
    
    return vString[index];
}
string VectorModule::getLastValue() const{
    if(type == 'b' && vBool.size() > 0){
        if(vBool.back().value){
            return "true";
        }
        return "false";
    }
    if(type == 'i' && vInt.size() > 0){
        return intToStr(vInt.back());
    }
    if(type == 'd' && vDouble.size() > 0){
        return doubleToStr(vDouble.back());
    }
    if(type == 's' && vString.size() > 0){
        return vString.back();
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return "";
}
vector<string> VectorModule::getAllStrings() const{
    return vString;
}
vector<VariableModule> VectorModule::getAllStringsAsVariables(){
    vector<VariableModule> Values;
    for(string value : vString){
        Values.emplace_back(VariableModule::newString(value));
    }
    return Values;
}
bool VectorModule::getLastBool() const
{
    if(vBool.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vBool.back().value;
}
int VectorModule::getLastInt() const{
    if(vInt.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vInt.back();
}
double VectorModule::getLastDouble() const{
    if(vDouble.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vDouble.back();
}
string VectorModule::getLastString() const{
    if(vString.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    return vString.back();
}
bool VectorModule::getFirstBool() const{
    if(vBool.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vBool[0].value;
}
int VectorModule::getFirstInt() const{
    if(vInt.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0;
    }
    return vInt[0];
}
double VectorModule::getFirstDouble() const{
    if(vDouble.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0.0;
    }
    return vDouble[0];
}
string VectorModule::getFirstString() const{
    if(vString.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    return vString[0];
}
size_t VectorModule::getSize() const{
    if(type == 'b'){
        return vBool.size();
    }
    if(type == 'i'){
        return vInt.size();
    }
    if(type == 'd'){
        return vDouble.size();
    }
    if(type == 's'){
        return vString.size();
    }
    return 0;
}

void VectorModule::getContext(string attribute, vector<BasePointersStruct> &BasePointers){
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
    else if(getSize() > 0){
        if(attribute == "last_bool"){
            BasePointers.back().setPointer(&vBool.back().value);
        }
        else if(attribute == "last_int"){
            BasePointers.back().setPointer(&vInt.back());
        }
        else if(attribute == "last_double"){
            BasePointers.back().setPointer(&vDouble.back());
        }
        else if(attribute == "last_string"){
            BasePointers.back().setPointer(&vString.back());
        }
        else if(attribute == "first_bool"){
            BasePointers.back().setPointer(&vBool[0].value);
        }
        else if(attribute == "first_int"){
            BasePointers.back().setPointer(&vInt[0]);
        }
        else if(attribute == "first_double"){
            BasePointers.back().setPointer(&vDouble[0]);
        }
        else if(attribute == "first_string"){
            BasePointers.back().setPointer(&vString[0]);
        }
        else if(attribute == "random_bool"){
            BasePointers.back().setPointer(&vBool[rand() % vBool.size()].value);
        }
        else if(attribute == "random_int"){
            BasePointers.back().setPointer(&vInt[rand() % vInt.size()]);
        }
        else if(attribute == "random_double"){
            BasePointers.back().setPointer(&vDouble[rand() % vDouble.size()]);
        }
        else if(attribute == "random_string"){
            BasePointers.back().setPointer(&vString[rand() % vString.size()]);
        }
        else{
            BasePointers.pop_back();
            cout << "Error: In " << __FUNCTION__ << ": Attribute '" << attribute << "' is not valid.\n";
        }
    }
    else{
        BasePointers.pop_back();
        if(getSize() == 0){
            cout << "Error: In " << __FUNCTION__ << ": Vector '" << ID << "' is empty.\n";
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Attribute '" << attribute << "' is not valid.\n";
        }
    }
}

bool VectorModule::contains(const VariableModule &Variable){
    if(type != Variable.getType()){
        return false;
    }
    if(type == 'b'){
        if(std::find(vBool.begin(), vBool.end(), Variable.getBool()) != vBool.end()){
            return true;
        }
    }
    else if(type == 'i'){
        if(std::find(vInt.begin(), vInt.end(), Variable.getInt()) != vInt.end()){
            return true;
        }
    }
    else if(type == 'd'){
        if(std::find(vDouble.begin(), vDouble.end(), Variable.getDouble()) != vDouble.end()){
            return true;
        }
    }
    else if(type == 's'){
        if(std::find(vString.begin(), vString.end(), Variable.getString()) != vString.end()){
            return true;
        }
    }
    return false;
}
bool VectorModule::contains(const BasePointersStruct &Pointer){
    if(type == 'b'){
        if(std::find(vBool.begin(), vBool.end(), Pointer.getBool()) != vBool.end()){
            return true;
        }
    }
    else if(type == 'i'){
        if(std::find(vInt.begin(), vInt.end(), Pointer.getInt()) != vInt.end()){
            return true;
        }
    }
    else if(type == 'd'){
        if(std::find(vDouble.begin(), vDouble.end(), Pointer.getDouble()) != vDouble.end()){
            return true;
        }
    }
    else if(type == 's'){
        if(std::find(vString.begin(), vString.end(), Pointer.getString()) != vString.end()){
            return true;
        }
    }
    return false;
}

bool operator==(stupidBool sb, const bool b){
    return sb.value == b;
}
