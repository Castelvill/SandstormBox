#include "layerClass.h"

void LayerClass::setUpNewInstance(const string &newID, vector<string> &layersIDs, bool activate,
    const vec2d &bufferPos, const vec2i &bufferSize, size_t & topLayerIndex
){
    uniqueIndex = topLayerIndex++;
    setID(newID, layersIDs);
    isActive = activate;
    deleted = false;
    pos.set(bufferPos);
    size.x = bufferSize.x;
    size.y = bufferSize.y;
}
LayerClass::LayerClass(){
    isActive = false;
    deleted = false;
    pos.set(0.0, 0.0);
    size.set(0.0, 0.0);
}
LayerClass::LayerClass(const string & newID, vector<string> & layersIDs, size_t & topLayerIndex,
    bool activate, vec2d bufferPos, vec2i bufferSize
){
    setUpNewInstance(newID, layersIDs, activate, bufferPos, bufferSize, topLayerIndex);
}
LayerClass::LayerClass(const string & newID, vector<string> & layersIDs, size_t & topLayerIndex){
    setUpNewInstance(newID, layersIDs, false, vec2d(0.0, 0.0), vec2i(0, 0), topLayerIndex);
}
LayerClass::~LayerClass(){

}
void LayerClass::clear(){
    for(AncestorObject & Object : Objects){
        Object.clear();
    }
    Objects.clear();
    objectsIDs.clear();
}
//Returns true if uniqueness is violeted. 
bool LayerClass::isObjectsUniquenessViolated(){
    unsigned i, j;
    bool violated = false;
    for(i = 0; i < Objects.size(); i++){
        for(j = 0; j < Objects.size(); j++){
            if(i == j || Objects[i].getID() != Objects[j].getID()
                || Objects[i].getUniqueIndex()!= Objects[j].getUniqueIndex()
            ){
                continue;
            }
            if(!violated){
                cout << "\n\n";
            }
            cout << "Uniqueness has been violated In Layer[" << ID << "]=" << ID
                << " by Object[" << j << "]=" << Objects[j].getID() << "\n";
            violated = true;
        }
    }

    if(violated){
        cout << "\n";
    }
    return violated;
}

void LayerClass::setUniqueIndex(size_t value){
    uniqueIndex = value;
}
size_t LayerClass::getUniqueIndex() const{
    return uniqueIndex;
}

void LayerClass::setID(const string & newID, vector<string> & layersIDs){
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID
            << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(layersIDs, ID);
    ID = findNewUniqueID(layersIDs, newID);
    for(AncestorObject & Object : Objects){
        Object.setLayerID(ID);
        Object.propagateLayer();
    }
    layersIDs.push_back(ID);
}
void LayerClass::setIsActive(bool newIsActive){
    isActive = newIsActive;
}
void LayerClass::deleteLater(){
    deleted = true;
    isActive = false;
    for(AncestorObject & Object : Objects){
        Object.deleteLater();
    }
}
string LayerClass::getID() const{
    return ID;
}
string* LayerClass::getIDAddr(){
    return &ID;
}
bool LayerClass::getIsActive() const{
    return isActive;
}
bool LayerClass::getIsDeleted() const
{
    return deleted;
}
bool *LayerClass::getIsActiveAddr()
{
    return &isActive;
}
VariableModule LayerClass::getValue(AttributeType attribute, string option) const{
    VariableModule NewValue;
    switch(attribute){
        case id:
            NewValue.setString(getID());
            return NewValue;
        case in_group:
            NewValue.setBool(isInAGroup(option));
            return NewValue;
        case number_of_objects_a:
            NewValue.setInt(Objects.size());
            return NewValue;
        case is_active:
            NewValue.setDouble(getIsActive());
            return NewValue;
        case pos_x:
            NewValue.setDouble(pos.x);
            return NewValue;
        case pos_y:
            NewValue.setDouble(pos.y);
            return NewValue;
        case size_x:
            NewValue.setDouble(size.x);
            return NewValue;
        case size_y:
            NewValue.setDouble(size.y);
            return NewValue;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Attribute '"
                << attributeToStr(attribute) << "' is not valid.\n";
            return VariableModule();
    }
}
void LayerClass::clone(const LayerClass &Original, vector<string> &layersIDs, bool changeOldID,
    size_t & topObjectUniqueIndex, size_t & topModuleUniqueIndex
){
    if(isStringInVector(reservedIDs, Original.ID)){
        cerr << "Error: In " << __FUNCTION__ << ": Layer with a reserved ID \'" << Original.ID
            << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": Layer with a reserved ID \'" << ID
            << "\' cannot be changed.\n";
        return;
    }
    string oldID = ID;
    clear();
    ID = oldID;
    if(changeOldID){
        setID(Original.getID(), layersIDs);
    }

    groups = Original.groups;
    isActive = Original.isActive;
    deleted = Original.deleted;
    pos.set(Original.pos);
    size.set(Original.size);

    for(const AncestorObject & Object : Original.Objects){
        Objects.emplace_back(AncestorObject(topObjectUniqueIndex));
        Objects.back().clone(Object, objectsIDs, getUniqueIndex(), ID, true, topModuleUniqueIndex);
    }

    objectsOrder = Original.objectsOrder;

    if(Objects.size() != objectsOrder.size()){
        cerr << "Error: In " << __FUNCTION__ << ": Inside the layer '" << ID
            << "' number of objects (" << Objects.size()
            << ") is not equal to the number of objects inside their drawing order ("
            << objectsOrder.size() << ").\n";
    }
}

void LayerClass::addGroup(string newGroup){
    addUniqueToStringVector(groups, newGroup);
}
void LayerClass::removeGroup(string selectedGroup){
    removeFromStringVector(groups, selectedGroup);
}
void LayerClass::clearGroups(){
    groups.clear();
}
bool LayerClass::isInAGroup(string findGroup) const{
    return isStringInVector(groups, findGroup);
}
vector<string> LayerClass::getGroups(){
    return groups;
}
vector<string> &LayerClass::getGroupsAddr(){
    return groups;
}

void LayerClass::nullifyAllPointers(){
    for(AncestorObject & Object : Objects){
        for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
            Scrollbar.nullifyFocusedCameraPointer();
        }
    }
}
