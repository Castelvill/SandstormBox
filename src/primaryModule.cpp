#include "primaryModule.h"

void PrimaryModule::primaryConstructor(unsigned int moduleID){
    ID = intToStr4(moduleID);
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
void PrimaryModule::primaryConstructor(std::string moduleID){
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
void PrimaryModule::setID(std::string newID){
    ID = newID;
}


void PrimaryModule::addGroup(std::string newGroup){
    addNewStringToVector(groups, newGroup);
}
void PrimaryModule::removeGroup(std::string selectedGroup){
    removeStringFromVector(groups, selectedGroup);
}
void PrimaryModule::clearGroups(){
    groups.clear();
}
bool PrimaryModule::isInAGroup(std::string findGroup){
    return isInAStringVector(groups, findGroup);
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
string PrimaryModule::getID(){
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
