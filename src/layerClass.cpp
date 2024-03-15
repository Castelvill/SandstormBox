#include "layerClass.h"

void LayerClass::setUpNewInstance(const string &newID, vector<string> &layersIDs, const bool &activate, const vec2d &bufferPos, const vec2i &bufferSize){
    setID(newID, layersIDs);
    isActive = activate;
    deleted = false;
    pos.set(bufferPos);
    size.x = bufferSize.x;
    size.y = bufferSize.y;
}
LayerClass::LayerClass(){
    ID = "";
    isActive = false;
    deleted = false;
    pos.set(0.0, 0.0);
    size.set(0.0, 0.0);
}
LayerClass::LayerClass(string newID, vector <string> & layersIDs, bool activate, vec2d bufferPos, vec2i bufferSize){
    setUpNewInstance(newID, layersIDs, activate, bufferPos, bufferSize);
}
LayerClass::LayerClass(string newID, vector <string> & layersIDs){
    setUpNewInstance(newID, layersIDs, false, vec2d(0.0, 0.0), vec2i(0, 0));
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
            if(i == j || Objects[i].getID() != Objects[j].getID()){
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

void LayerClass::setID(string newID, vector <string> & layersIDs){
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(layersIDs, ID);
    ID = findNewUniqueID(layersIDs, newID);
    for(AncestorObject & Object : Objects){
        Object.setLayerID(ID);
        Object.propagateLayerID();
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
VariableModule LayerClass::getValue(string attribute, string option) const{
    VariableModule NewValue;
    if(attribute == "id"){
        NewValue.setString(getID());
        return NewValue;
    }
    if(attribute == "in_group"){
        NewValue.setBool(isInAGroup(option));
        return NewValue;
    }
    if(attribute == "objects_count"){
        NewValue.setInt(Objects.size());
        return NewValue;
    }
    if(attribute == "is_active"){
        NewValue.setDouble(getIsActive());
        return NewValue;
    }
    if(attribute == "pos_x"){
        NewValue.setDouble(pos.x);
        return NewValue;
    }
    if(attribute == "pos_y"){
        NewValue.setDouble(pos.y);
        return NewValue;
    }
    if(attribute == "size_x"){
        NewValue.setDouble(size.x);
        return NewValue;
    }
    if(attribute == "size_y"){
        NewValue.setDouble(size.y);
        return NewValue;
    }
    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    return VariableModule();
}
void LayerClass::clone(const LayerClass &Original, vector<string> &layersIDs, const bool &changeOldID){
    if(isStringInVector(reservedIDs, Original.ID)){
        cout << "Error: In " << __FUNCTION__ << ": Layer with a reserved ID \'" << Original.ID << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": Layer with a reserved ID \'" << ID << "\' cannot be changed.\n";
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
        Objects.push_back(AncestorObject());
        Objects.back().clone(Object, objectsIDs, ID, true);
    }

    /*for(AncestorObject & Object : Objects){
        Object.setLayerID(getID());
        for(TextModule & Text : Object.TextContainer){
            Text.setLayerID(getID());
        }
        for(EditableTextModule & EditableText : Object.EditableTextContainer){
            EditableText.setLayerID(getID());
        }
        for(ImageModule & Image : Object.ImageContainer){
            Image.setLayerID(getID());
        }
        for(MovementModule & Movement : Object.MovementContainer){
            Movement.setLayerID(getID());
        }
        for(CollisionModule & Collision : Object.CollisionContainer){
            Collision.setLayerID(getID());
        }
        for(ParticleEffectModule & Particle : Object.ParticlesContainer){
            Particle.setLayerID(getID());
        }
        for(EveModule & Event : Object.EveContainer){
            Event.setLayerID(getID());
            for(ConditionClass & Trigger : Event.ConditionalChain){
                if(Trigger.Location.layerID == Original.getID()){
                    Trigger.Location.layerID = getID();
                }
            }
            for(OperaClass & Operation : Event.DependentOperations){
                for(ConditionClass & Trigger : Operation.ConditionalChain){
                    if(Trigger.Location.layerID == Original.getID()){
                        Trigger.Location.layerID = getID();
                    }
                }
            }
            for(OperaClass & Operation : Event.PostOperations){
                for(ConditionClass & Trigger : Operation.ConditionalChain){
                    if(Trigger.Location.layerID == Original.getID()){
                        Trigger.Location.layerID = getID();
                    }
                }
            }
        }
        for(VariableModule & Variable : Object.VariablesContainer){
            Variable.setLayerID(getID());
        }
        for(ScrollbarModule & Scrollbar : Object.ScrollbarContainer){
            Scrollbar.setLayerID(getID());
        }
    }*/
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
vector <string> LayerClass::getGroups(){
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
