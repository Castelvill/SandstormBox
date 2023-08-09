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
    vector <string> objectsIDs;
    vec2d pos, size;
    LayerClass();
    LayerClass(string newID, vector <string> & layersIDs, bool activate, vec2d bufferPos, vec2d bufferSize);
    void clear();
    bool isObjectsUniquenessViolated();

    void setID(string newID, vector <string> & layersIDs);
    void setIsActive(bool newIsActive);
    string getID() const;
    string* getIDAddr();
    bool getIsActive();
    bool* getIsActiveAddr();
    void clone(const LayerClass& Orginal, vector <string> & layersIDs);
};


#endif // LAYERCLASS_H_INCLUDED
