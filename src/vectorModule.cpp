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
    if(type == 'b'){
        vBool.push_back(newValue);
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
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'bool' type to a '" << type << "' vector.\n";
    return false;
}
bool VectorModule::pushInt(int newValue){
    if(type == 'b'){
        vBool.push_back(newValue);
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
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'int' type to a '" << type << "' vector.\n";
    return false;
}
bool VectorModule::pushDouble(double newValue){
    if(type == 'b'){
        vBool.push_back(newValue);
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
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'double' type to a '" << type << "' vector.\n";
    return false;
}
bool VectorModule::pushString(string newValue){
    if(type == 's'){
        vString.push_back(newValue);
        return true;
    }
    cout << "Error: In " << __PRETTY_FUNCTION__ << ": Cannot push back a value of 'string' type to a '" << type << "' vector.\n";
    return false;
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
string VectorModule::getValue(size_t index) const{
    if(type == 'b' && index < vBool.size()){
        if(vBool[index]){
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
string VectorModule::getValueUnsafe(long index) const{
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
bool VectorModule::getBool(size_t index) const{
    if(index >= vBool.size()){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vBool.size() << ".\n";
        return false;
    }
    return vBool[index];
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

    return vBool[index];
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
        if(vBool.back()){
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
bool VectorModule::getLastBool() const{
    if(vBool.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vBool.back();
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
    if(vBool.size() == 0){
        cout << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    return vString.back();
}