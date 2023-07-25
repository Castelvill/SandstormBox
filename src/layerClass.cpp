#include "layerClass.h"

LayerClass::LayerClass(string layerID, bool activate, vec2d bufferPos, vec2d bufferSize){
    ID = layerID;
    isActive = activate;
    pos.set(bufferPos);
    size.set(bufferSize);
}
void LayerClass::clear(){
    Objects.clear();
    listOfUniqueIDs.clear();
}
//Returns true if uniqueness is violeted. 
bool LayerClass::createListOfUniqueIDs(){
    listOfUniqueIDs.clear();
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Objects.size(); i++){
        for(j = 0; j < Objects.size(); j++){
            if(i == j){
                continue;
            }
            if(Objects[i].getID() == Objects[j].getID()){
                if(!violated){
                    std::cout << "\n\n";
                }
                std::cout << "Uniqueness has been violated in: Layer[" << ID << "]=" << ID
                    << " by Object[" << j << "]=" << Objects[j].getID() << "\n";
                violated = true;
            }
        }
        if(!violated){
            listOfUniqueIDs.push_back(Objects[i].getID());
        }
    }

    if(violated){
        std::cout << "\n";
    }
    return violated;
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
void LayerClass::clone(const LayerClass& Orginal, vector <string> & layersIDs){
    *this = Orginal;
    ID = findRightID(layersIDs, getID());
    layersIDs.push_back(ID);
}