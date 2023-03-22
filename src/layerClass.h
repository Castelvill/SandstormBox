#ifndef LAYERCLASS_H_INCLUDED
#define LAYERCLASS_H_INCLUDED

#include "ancestorObject.h"

class LayerClass{
private:
    string ID;
    bool isActive;
public:
    vector <AncestorObject> Objects;
    vec2d pos, size;
    LayerClass(string layerID, bool activate, vec2d bufferPos, vec2d bufferSize);
    void clear();

    void setID(string newID);
    void setIsActive(bool newIsActive);
    string getID();
    bool getIsActive();
};


#endif // LAYERCLASS_H_INCLUDED
