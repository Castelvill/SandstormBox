#include "primaryModule.h"


void PrimaryModule::primaryConstructor(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID){
    setAllIDs(newID, listOfIDs, newLayerID, newObjectID);
    
    pos.set(0.0, 0.0);
    scrollShift.set(0.0, 0.0);
    size.set(100.0, 100.0);
    scale.set(0.0, 0.0);

    isActive = true;
    isScaledFromCenter = false;
    isAttachedToCamera = false;
    canBeSelected = true;
    isScrollable = false;
}
void PrimaryModule::primaryConstructor(unsigned newID, vector<string> & listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(intToStr4(newID), listOfIDs, newLayerID, newObjectID);
}
void PrimaryModule::clone(const PrimaryModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID);
}

void PrimaryModule::setID(string newID, vector<string> & listOfIDs){
    removeFromStringVector(listOfIDs, ID);
    ID = findNewUniqueID(listOfIDs, newID);
    listOfIDs.push_back(ID);
}

void PrimaryModule::setLayerID(string newLayerID){
    layerID = newLayerID;
}

void PrimaryModule::setObjectID(string newOwnerID){
    objectID = newOwnerID;
}

void PrimaryModule::setAllIDs(string newID, vector<string> &listOfIDs, string newLayerID, string newObjectID){
    setID(newID, listOfIDs);
    setLayerID(newLayerID);
    setObjectID(newObjectID);
}

void PrimaryModule::addGroup(string newGroup){
    addUniqueToStringVector(groups, newGroup);
}
void PrimaryModule::removeGroup(string selectedGroup){
    removeFromStringVector(groups, selectedGroup);
}
void PrimaryModule::clearGroups(){
    groups.clear();
}
bool PrimaryModule::isInAGroup(string findGroup){
    return isStringInVector(groups, findGroup);
}
vector <string> PrimaryModule::getGroups(){
    return groups;
}


void PrimaryModule::setPos(vec2d newPos){
    pos.set(newPos);
}
void PrimaryModule::setPos(double x, double y){
    pos.set(x, y);
}
void PrimaryModule::translatePos(vec2d newPos){
    pos.translate(newPos);
}
void PrimaryModule::setScrollShift(vec2d newValue){
    scrollShift = newValue;
}
void PrimaryModule::setSize(vec2d newSize){
    size.set(newSize);
}
void PrimaryModule::setSize(double x, double y){
    size.set(x, y);
}
void PrimaryModule::setScale(vec2d newScale){
    scale.set(newScale);
}
void PrimaryModule::setScale(double x, double y){
    scale.set(x, y);
}
void PrimaryModule::addScale(vec2d newScale){
    scale.translate(newScale);
}
void PrimaryModule::resize(vec2d newSize){
    scale.set(newSize.x/size.x, newSize.y/size.y);
}
void PrimaryModule::resize(double newSizeX, double newSizeY){
    scale.set(newSizeX/size.x, newSizeY/size.y);
}
void PrimaryModule::resizeX(double newSizeX){
    scale.x = newSizeX/size.x;
}
void PrimaryModule::resizeY(double newSizeY){
    scale.y = newSizeY/size.y;
}
void PrimaryModule::setIsActive(bool newValue){
    isActive = newValue;
}
void PrimaryModule::activate(){
    setIsActive(true);
}
void PrimaryModule::deactivate(){
    setIsActive(false);
}
void PrimaryModule::toggleIsActive(){
    setIsActive(!getIsActive());
}
void PrimaryModule::setIsScaledFromCenter(bool newIsScaledFromCenter){
    isScaledFromCenter = newIsScaledFromCenter;
}
void PrimaryModule::setIsAttachedToCamera(bool newIsAttachedToCamera){
    isAttachedToCamera = newIsAttachedToCamera;
}
void PrimaryModule::setCanBeSelected(bool newValue){
    canBeSelected = newValue;
}
void PrimaryModule::setIsScrollable(bool newValue){
    isScrollable = newValue;
}
string PrimaryModule::getID() const{
    return ID;
}
string PrimaryModule::getLayerID() const{
    return layerID;
}
string &PrimaryModule::getLayerIDAddr(){
    return layerID;
}
string &PrimaryModule::getIDAddr(){
    return ID;
}
string PrimaryModule::getObjectID(){
    return objectID;
}
vec2d PrimaryModule::getPos(bool useScrollshift){
    if(!useScrollshift){
        return pos;
    }
    return pos-scrollShift;
}
vec2d& PrimaryModule::getPosAddr(){
    return pos;
}
vec2d PrimaryModule::getSize(){
    return size;
}
vec2d& PrimaryModule::getSizeAddr(){
    return size;
}
vec2d PrimaryModule::getScale(){
    return scale;
}
vec2d PrimaryModule::getScaledSize(){
    vec2d scaledSize(size);
    scaledSize.multiply(scale);
    return scaledSize;
}
bool PrimaryModule::getIsActive(){
    return isActive;
}
bool PrimaryModule::getIsScaledFromCenter(){
    return isScaledFromCenter;
}
bool PrimaryModule::getIsAttachedToCamera(){
    return isAttachedToCamera;
}
bool PrimaryModule::getCanBeSelected(){
    return canBeSelected;
}
bool PrimaryModule::getIsScrollable(){
    return isScrollable;
}
void PrimaryModule::bindPrimaryToVariable(string attribute, BasePointersStruct & BasePointer){
    if(attribute == "id"){
        BasePointer.setPointer(&ID);
    }
    else if(attribute == "pos_x"){
        BasePointer.setPointer(&pos.x);
    }
    else if(attribute == "pos_y"){
        BasePointer.setPointer(&pos.y);
    }
    else if(attribute == "size_x"){
        BasePointer.setPointer(&size.x);
    }
    else if(attribute == "size_y"){
        BasePointer.setPointer(&size.y);
    }
    else if(attribute == "scale_x"){
        BasePointer.setPointer(&scale.x);
    }
    else if(attribute == "scale_y"){
        BasePointer.setPointer(&scale.y);
    }
    else if(attribute == "is_active"){
        BasePointer.setPointer(&isActive);
    }
    else if(attribute == "is_scaled_from_center"){
        BasePointer.setPointer(&isScaledFromCenter);
    }
    else if(attribute == "is_attached_to_camera"){
        BasePointer.setPointer(&isAttachedToCamera);
    }
    else if(attribute == "can_be_selected"){
        BasePointer.setPointer(&canBeSelected);
    }
    else if(attribute == "is_scrollable"){
        BasePointer.setPointer(&isScrollable);
    }
}
BasePointersStruct::BasePointersStruct(){
    type = "";
}
void BasePointersStruct::clear(){
    type = "";
    pBool = nullptr;
    pShort = nullptr;
    pUShort = nullptr;
    pInt = nullptr;
    pUInt = nullptr;
    pDouble = nullptr;
    pString = nullptr;
}
template<typename T>
void BasePointersStruct::tryToSet(T * LeftOperand, const T * RightOperand){
    if(LeftOperand == nullptr){
        std::cout << "Error: In BasePointersStruct::tryToSet(): Left operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(RightOperand == nullptr){
        std::cout << "Error: In BasePointersStruct::tryToSet(): Right operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    *LeftOperand = *RightOperand;
}
void BasePointersStruct::clone(const BasePointersStruct &RightOperand){
    if(type == "bool"){
        tryToSet(pBool, RightOperand.pBool);
    }
    else if(type == "char"){
        tryToSet(pChar, RightOperand.pChar);
    }
    else if(type == "short"){
        tryToSet(pShort, RightOperand.pShort);
    }
    else if(type == "unsigned_short"){
        tryToSet(pUShort, RightOperand.pUShort);
    }
    else if(type == "int"){
        tryToSet(pInt, RightOperand.pInt);
    }
    else if(type == "unsigned_int"){
        tryToSet(pUInt, RightOperand.pUInt);
    }
    else if(type == "float"){
        tryToSet(pFloat, RightOperand.pFloat);
    }
    else if(type == "double"){
        tryToSet(pDouble, RightOperand.pDouble);
    }
    else if(type == "string"){
        tryToSet(pString, RightOperand.pString);
    }
    else{
        std::cout << "Error: In BasePointersStruct::clone(): \'" << type << "\' is not a valid type.\n"; 
    }
}
void BasePointersStruct::setPointer(bool *pointer){
    pBool = pointer;
    type = "bool";
}
void BasePointersStruct::setPointer(char * pointer){
    pChar = pointer;
    type = "char";
}
void BasePointersStruct::setPointer(short * pointer){
    pShort = pointer;
    type = "short";
}
void BasePointersStruct::setPointer(unsigned short * pointer){
    pUShort = pointer;
    type = "unsigned_short";
}
void BasePointersStruct::setPointer(int * pointer){
    pInt = pointer;
    type = "int";
}
void BasePointersStruct::setPointer(unsigned int * pointer){
    pUInt = pointer;
    type = "unsigned_int";
}
void BasePointersStruct::setPointer(float * pointer){
    pFloat = pointer;
    type = "float";
}
void BasePointersStruct::setPointer(double * pointer){
    pDouble = pointer;
    type = "double";
}
void BasePointersStruct::setPointer(string * pointer){
    pString = pointer;
    type = "string";
}

bool BasePointersStruct::areEqual(BasePointersStruct *OtherVariable){
    if(type != OtherVariable->type){
        std::cout << "Error: Pointers of built-in types have different types.\n";
        return false;
    }
    if(type == "bool"){
        return *pBool == *OtherVariable->pBool;
    }
    else if(type == "char"){
        return *pChar == *OtherVariable->pChar;
    }
    else if(type == "short"){
        return *pShort == *OtherVariable->pShort;
    }
    else if(type == "unsigned_short"){
        return *pUShort == *OtherVariable->pUShort;
    }
    else if(type == "int"){
        return *pInt == *OtherVariable->pInt;
    }
    else if(type == "unsigned_int"){
        return *pUInt == *OtherVariable->pUInt;
    }
    else if(type == "float"){
        return *pFloat == *OtherVariable->pFloat;
    }
    else if(type == "double"){
        return *pDouble == *OtherVariable->pDouble;
    }
    else if(type == "string"){
        return *pString == *OtherVariable->pString;
    }
    else{
        std::cout << "Error: In BasePointersStruct::areEqual(): \'" << type << "\' is not a valid type.\n"; 
    }
    return false;
}
