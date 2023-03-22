#include "layerClass.h"

LayerClass::LayerClass(string layerID, bool activate, vec2d bufferPos, vec2d bufferSize){
    ID = layerID;
    isActive = activate;
    pos.set(bufferPos);
    size.set(bufferSize);
}
void LayerClass::clear(){
    
}
void LayerClass::setID(string newID){
    ID = newID;
}
void LayerClass::setIsActive(bool newIsActive){
    isActive = newIsActive;
}
string LayerClass::getID(){
    return ID;
}
bool LayerClass::getIsActive(){
    return isActive;
}
