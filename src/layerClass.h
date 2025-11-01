#ifndef LAYERCLASS_H_INCLUDED
#define LAYERCLASS_H_INCLUDED

#include "ancestorObject.h"

class LayerClass{
private:
    size_t uniqueIndex;
    string ID;
    vector <string> groups;
    bool isActive;
    bool deleted;
public:
    vector <AncestorObject> Objects;
    vector <unsigned> objectsOrder;
    //Uniques of IDs is checked only for objects existing in the same layer - user should be able to
    //copy entire layers without worring about providing unique IDs for every object,
    //ex. testing or branching.
    vector <string> objectsIDs;
    vec2d pos, size;
    void setUpNewInstance(const string &newID, vector<string> &layersIDs, bool activate,
        const vec2d &bufferPos, const vec2i &bufferSize, size_t & topLayerIndex);
    LayerClass();
    LayerClass(const string & newID, vector<string> & layersIDs, size_t & topLayerIndex,
        bool activate, vec2d bufferPos, vec2i bufferSize);
    LayerClass(const string & newID, vector<string> & layersIDs, size_t & topLayerIndex);
    ~LayerClass();
    void clear();
    bool isObjectsUniquenessViolated();

    void setUniqueIndex(size_t value);
    size_t getUniqueIndex() const;
    void setID(const string & newID, vector<string> & layersIDs);
    void setIsActive(bool newIsActive);
    void deleteLater();
    string getID() const;
    string* getIDAddr();
    bool getIsActive() const;
    bool getIsDeleted() const;
    bool* getIsActiveAddr();
    VariableModule getValue(AttributeType attribute, string option) const;
    void clone(const LayerClass& Original, vector<string> & layersIDs, bool changeOldID,
        size_t & topObjectUniqueIndex, size_t & topModuleUniqueIndex);

    void addGroup(string newGroup);
    void removeGroup(string selectedGroup);
    void clearGroups();
    bool isInAGroup(string findGroup) const;
    vector <string> getGroups();
    vector <string> & getGroupsAddr();
    void nullifyAllPointers();
};


#endif // LAYERCLASS_H_INCLUDED
