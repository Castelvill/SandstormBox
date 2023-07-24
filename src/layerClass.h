#ifndef LAYERCLASS_H_INCLUDED
#define LAYERCLASS_H_INCLUDED

#include "ancestorObject.h"

class LayerClass{
private:
    string ID;
    bool isActive;
public:
    vector <AncestorObject> Objects;
    //Uniques of IDs is checked only for objects existing in the same layer - user should be able to copy entire layers
    //without worring about providing unique IDs for every object, ex. testing or branching.
    vector <string> listOfUniqueIDs;
    vec2d pos, size;
    LayerClass(string layerID, bool activate, vec2d bufferPos, vec2d bufferSize);
    void clear();
    bool createListOfUniqueIDs();

    void setID(string newID);
    void setIsActive(bool newIsActive);
    string getID();
    bool getIsActive();
    void clone(const LayerClass& Orginal);
};


#endif // LAYERCLASS_H_INCLUDED
