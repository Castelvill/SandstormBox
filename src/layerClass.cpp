#include "layerClass.h"

LayerClass::LayerClass(){
    ID = "";
    isActive = false;
}
LayerClass::LayerClass(string newID, vector <string> & layersIDs, bool activate, vec2d bufferPos, vec2d bufferSize){
    setID(newID, layersIDs);
    isActive = activate;
    pos.set(bufferPos);
    size.set(bufferSize);
}
void LayerClass::clear(){
    Objects.clear();
    objectsIDs.clear();
}
//Returns true if uniqueness is violeted. 
bool LayerClass::isObjectsUniquenessViolated(){
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Objects.size(); i++){
        for(j = 0; j < Objects.size(); j++){
            if(i == j || Objects[i].getID() != Objects[j].getID()){
                continue;
            }
            if(!violated){
                std::cout << "\n\n";
            }
            std::cout << "Uniqueness has been violated in: Layer[" << ID << "]=" << ID
                << " by Object[" << j << "]=" << Objects[j].getID() << "\n";
            violated = true;
        }
    }

    if(violated){
        std::cout << "\n";
    }
    return violated;
}

void LayerClass::setID(string newID, vector <string> & layersIDs){
    removeFromStringVector(layersIDs, ID);
    ID = findNewUniqueID(layersIDs, newID);
    layersIDs.push_back(ID);
}
void LayerClass::setIsActive(bool newIsActive){
    isActive = newIsActive;
}
string LayerClass::getID() const{
    return ID;
}
string* LayerClass::getIDAddr(){
    return &ID;
}
bool LayerClass::getIsActive(){
    return isActive;
}
bool* LayerClass::getIsActiveAddr(){
    return &isActive;
}
void LayerClass::clone(const LayerClass& Original, vector <string> & layersIDs){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setID(Original.getID(), layersIDs);
}