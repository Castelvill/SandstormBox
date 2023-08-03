#include "primaryModule.h"


void PrimaryModule::primaryConstructor(string moduleID){
    ID = moduleID;
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
void PrimaryModule::primaryConstructor(unsigned int moduleID){
    primaryConstructor(intToStr4(moduleID));
}
void PrimaryModule::clone(const PrimaryModule & Original){
    *this = Original;
}

void PrimaryModule::setID(string newID){
    ID = newID;
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
    return inStringVector(groups, findGroup);
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
string & PrimaryModule::getIDAddr(){
    return ID;
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
void PrimaryModule::bindPrimaryToVariable(string attribute, BasePointersStruct & UniversalVariable){
    if(attribute == "id"){
        UniversalVariable.getPointer(&ID);
    }
    else if(attribute == "pos_x"){
        UniversalVariable.getPointer(&pos.x);
    }
    else if(attribute == "pos_y"){
        UniversalVariable.getPointer(&pos.y);
    }
    else if(attribute == "size_x"){
        UniversalVariable.getPointer(&size.x);
    }
    else if(attribute == "size_y"){
        UniversalVariable.getPointer(&size.y);
    }
    else if(attribute == "scale_x"){
        UniversalVariable.getPointer(&scale.x);
    }
    else if(attribute == "scale_y"){
        UniversalVariable.getPointer(&scale.y);
    }
    else if(attribute == "is_active"){
        UniversalVariable.getPointer(&isActive);
    }
    else if(attribute == "is_scaled_from_center"){
        UniversalVariable.getPointer(&isScaledFromCenter);
    }
    else if(attribute == "is_attached_to_camera"){
        UniversalVariable.getPointer(&isAttachedToCamera);
    }
    else if(attribute == "can_be_selected"){
        UniversalVariable.getPointer(&canBeSelected);
    }
    else if(attribute == "is_scrollable"){
        UniversalVariable.getPointer(&isScrollable);
    }
}
BasePointersStruct::BasePointersStruct(){
    type = "";
}
void BasePointersStruct::clear(){
    type = "";
    vBool = nullptr;
    vShort = nullptr;
    vUShort = nullptr;
    vInt = nullptr;
    vUInt = nullptr;
    vDouble = nullptr;
    vString = nullptr;
}
void BasePointersStruct::getPointer(bool * pointer){
    vBool = pointer;
    type = "bool";
}
void BasePointersStruct::getPointer(char * pointer){
    vChar = pointer;
    type = "char";
}
void BasePointersStruct::getPointer(short * pointer){
    vShort = pointer;
    type = "short";
}
void BasePointersStruct::getPointer(unsigned short * pointer){
    vUShort = pointer;
    type = "unsigned_short";
}
void BasePointersStruct::getPointer(int * pointer){
    vInt = pointer;
    type = "int";
}
void BasePointersStruct::getPointer(unsigned int * pointer){
    vUInt = pointer;
    type = "unsigned_int";
}
void BasePointersStruct::getPointer(float * pointer){
    vFloat = pointer;
    type = "float";
}
void BasePointersStruct::getPointer(double * pointer){
    vDouble = pointer;
    type = "double";
}
void BasePointersStruct::getPointer(string * pointer){
    vString = pointer;
    type = "string";
}
