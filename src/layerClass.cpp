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

    for(AncestorObject & Object : Objects){
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
            for(TriggerClass & Trigger : Event.ConditionalChain){
                if(Trigger.Location.layerID == oldID){
                    Trigger.Location.layerID = getID();
                }
            }
            for(OperaClass & Operation : Event.DependentOperations){
                for(TriggerClass & Trigger : Operation.ConditionalChain){
                    if(Trigger.Location.layerID == oldID){
                        Trigger.Location.layerID = getID();
                    }
                }
            }
            for(OperaClass & Operation : Event.PostOperations){
                for(TriggerClass & Trigger : Operation.ConditionalChain){
                    if(Trigger.Location.layerID == oldID){
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
    }
}