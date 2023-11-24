#ifndef LAYERCLASS_H_INCLUDED
#define LAYERCLASS_H_INCLUDED

#include "ancestorObject.h"

class LayerClass{
private:
    string ID;
    vector <string> groups;
    bool isActive;
    bool deleted;
public:
    vector <AncestorObject> Objects;
    //Uniques of IDs is checked only for objects existing in the same layer - user should be able to copy entire layers
    //without worring about providing unique IDs for every object, ex. testing or branching.
    vector <string> objectsIDs;
    vec2d pos, size;
    void setUpNewInstance(const string & newID, vector <string> & layersIDs, const bool & activate, const vec2d & bufferPos, const vec2i & bufferSize);
    LayerClass();
    LayerClass(string newID, vector <string> & layersIDs, bool activate, vec2d bufferPos, vec2i bufferSize);
    LayerClass(string newID, vector <string> & layersIDs);
    ~LayerClass();
    void clear();
    bool isObjectsUniquenessViolated();

    void setID(string newID, vector <string> & layersIDs);
    void setIsActive(bool newIsActive);
    void deleteLater();
    string getID() const;
    string* getIDAddr();
    bool getIsActive() const;
    bool getIsDeleted() const;
    bool* getIsActiveAddr();
    void clone(const LayerClass& Orginal, vector <string> & layersIDs, const bool & changeOldID);

    void addGroup(string newGroup);
    void removeGroup(string selectedGroup);
    void clearGroups();
    bool isInAGroup(string findGroup) const;
    vector <string> getGroups();
    vector <string> & getGroupsAddr();
    void nullifyAllPointers();
};


#endif // LAYERCLASS_H_INCLUDED
