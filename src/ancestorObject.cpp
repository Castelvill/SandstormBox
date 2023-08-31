#include "ancestorObject.h"
#include <ctype.h>

AncestorObject::AncestorObject(){
    //blank object
    //std::cout << "Warning: You are creating a blank object - it doesn't have an ID nor layerID.\n";
}
AncestorObject::AncestorObject(string newID, vector<string> &listOfIDs, string newLayerID){
    primaryConstructor(newID, listOfIDs, newLayerID, "");
}
AncestorObject::AncestorObject(unsigned newID, vector<string> &listOfIDs, string newLayerID){
    primaryConstructor(newID, listOfIDs, newLayerID, "");
}
void AncestorObject::clone(const AncestorObject& Original, vector <string> & listOfUniqueIDs, string newLayerID){
    clearVectorsOfIDs();
    clearContainers();
    PrimaryModule::clone(Original, listOfUniqueIDs, layerID, "");
    for(const TextModule & Text : Original.TextContainer){
        TextContainer.push_back(TextModule());
        TextContainer.back().clone(Text, textContainerIDs, newLayerID, getID());
    }
    for(const EditableTextModule & Editable : Original.EditableTextContainer){
        EditableTextContainer.push_back(EditableTextModule());
        EditableTextContainer.back().clone(Editable, editableTextContainerIDs, newLayerID, getID());
    }
    for(const ImageModule & Image : Original.ImageContainer){
        ImageContainer.push_back(ImageModule());
        ImageContainer.back().clone(Image, imageContainerIDs, newLayerID, getID());
    }
    for(const MovementModule & Movement : Original.MovementContainer){
        MovementContainer.push_back(MovementModule());
        MovementContainer.back().clone(Movement, movementContainerIDs, newLayerID, getID());
    }
    for(const CollisionModule & Collision : Original.CollisionContainer){
        CollisionContainer.push_back(CollisionModule());
        CollisionContainer.back().clone(Collision, collisionContainerIDs, newLayerID, getID());
    }
    for(const ParticleEffectModule & Particle : Original.ParticlesContainer){
        ParticlesContainer.push_back(ParticleEffectModule());
        ParticlesContainer.back().clone(Particle, particlesContainerIDs, newLayerID, getID());
    }
    for(const EveModule & Event : Original.EveContainer){
        EveContainer.push_back(EveModule());
        EveContainer.back().clone(Event, eveContainerIDs, newLayerID, getID());
    }
    for(const VariableModule & Collision : Original.VariablesContainer){
        VariablesContainer.push_back(VariableModule());
        VariablesContainer.back().clone(Collision, variablesContainerIDs, newLayerID, getID());
    }
    for(const ScrollbarModule & Scrollbar : Original.ScrollbarContainer){
        ScrollbarContainer.push_back(ScrollbarModule());
        ScrollbarContainer.back().clone(Scrollbar, scrollbarContainerIDs, newLayerID, getID());
    }
}
void AncestorObject::clearVectorsOfIDs(){
    textContainerIDs.clear();
    editableTextContainerIDs.clear();
    imageContainerIDs.clear();
    movementContainerIDs.clear();
    collisionContainerIDs.clear();
    particlesContainerIDs.clear();
    eventsContainerIDs.clear();
    eveContainerIDs.clear();
    variablesContainerIDs.clear();
    scrollbarContainerIDs.clear();
}
void AncestorObject::clearContainers(){
    for(auto & Text : TextContainer){
        Text.clearAllContent();
    }
    for(auto & Editable : EditableTextContainer){
        Editable.clearAllContent();
    }
    for(auto & Image : ImageContainer){
        Image.destroyBitmap();
    }
    for(auto & Movement : MovementContainer){
        Movement.clearChain();
    }
    for(auto & Collision : CollisionContainer){
        Collision.clearModule();
    }
    for(auto & Particle : ParticlesContainer){
        Particle.clearModule();
    }
    for(auto & Event : EventsContainer){
        Event.clearModule();
    }
    for(auto & Event : EveContainer){
        Event.clearModule();
    }

    clearVectorsOfIDs();
    groups.clear();
    TextContainer.clear();
    EditableTextContainer.clear();
    ImageContainer.clear();
    MovementContainer.clear();
    CollisionContainer.clear();
    ParticlesContainer.clear();
    EventsContainer.clear();
    VariablesContainer.clear();
    ScrollbarContainer.clear();
}
/*void AncestorObject::createButton(string bID, vec2d bPos, vec2d bSize, vec2d bImageSize, vec2d bImageScale, string bImageID, vector <SingleBitmap> & BitmapContainer, bool bIsScaledFromCenter){
    setID(bID);
    setPos(bPos);
    setSize(bSize);
    ImageContainer.push_back(ImageModule(ImageContainer.size()));
    ImageContainer.back().connectBitmap(BitmapContainer, bImageID);
    ImageContainer.back().changeParameters("0", vec6d(0.0, 0.0, bImageSize.x, bImageSize.y, 0.0, 0.0), 0.0, bImageScale, false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    ImageContainer.back().setIsScaledFromCenter(bIsScaledFromCenter);
    ImageContainer.back().setIsAttachedToCamera(true);
    setIsAttachedToCamera(true);
}*/
void AncestorObject::operateEvent(int sourceID, int event, int operationID, vector <AncestorObject> & Objects){
    unsigned int objIndex = EventsContainer[event].Operations[operationID].affectedObjectIndex;

    if(!EventsContainer[event].Operations[operationID].isTriggeringItself){
        if(EventsContainer[event].Operations[operationID].affectedObjectID != Objects[objIndex].getID()){
            bool success = false;
            for(unsigned int i = 0 ; i < Objects.size(); i++){
                if(EventsContainer[event].Operations[operationID].affectedObjectID == Objects[i].getID()){
                    success = true;
                    EventsContainer[event].Operations[operationID].affectedObjectIndex = i;
                    objIndex = i;
                }
            }
            if(!success){
                return;
            }
        }
    }
    else{
        objIndex = sourceID;
    }


    if(EventsContainer[event].Operations[operationID].affectedModule == "ancestor"){
        EventsContainer[event].controlAncestor(operationID, Objects[objIndex].getPosAddr(), Objects[objIndex].getSizeAddr());
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "text"){
        for(auto & Text : Objects[objIndex].TextContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Text.getID()){
                EventsContainer[event].controlText(operationID, Text);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "image"){
        for(auto & Image : Objects[objIndex].ImageContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Image.getID()){
                EventsContainer[event].controlImage(operationID, Image);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "movement"){
        for(auto & Movement : Objects[objIndex].MovementContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Movement.getID()){
                EventsContainer[event].controlMovement(operationID, Movement);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "collision"){
        for(auto & Collision : Objects[objIndex].CollisionContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Collision.getID()){
                EventsContainer[event].controlCollision(operationID, Collision);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "particles"){
        for(auto & Particles : Objects[objIndex].ParticlesContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Particles.getID()){
                EventsContainer[event].controlParticles(operationID, Particles);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "variables"){
        for(auto & Variable : Objects[objIndex].VariablesContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Variable.getID()){
                EventsContainer[event].controlVariables(operationID, Variable);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "scrollbar"){
        for(auto & Scrollbar : Objects[objIndex].ScrollbarContainer){
            if(EventsContainer[event].Operations[operationID].affectedModuleID == Scrollbar.getID()){
                EventsContainer[event].controlScrollbar(operationID, Scrollbar);
            }
        }
    }
    else if(EventsContainer[event].Operations[operationID].affectedModule == "function"){
        if(EventsContainer[event].Operations[operationID].affectedVariable == "deactivate_all"){
            deactivateAllVectorsInEditorWindow(&Objects[objIndex]);
        }
        else if(EventsContainer[event].Operations[operationID].affectedVariable == "activate_based_on_id"){
            activateBasedOnId(&Objects[objIndex], EventsContainer[event].Operations[operationID].affectedModuleID);
        }
        else if(EventsContainer[event].Operations[operationID].affectedVariable == "activate_based_on_first_char"){
            activateBasedOnFirstChar(&Objects[objIndex], EventsContainer[event].Operations[operationID].affectedModuleID[0]);
            deactivateWrapped(std::stoi(EventsContainer[event].Operations[operationID].affectedModuleID), &Objects[objIndex]);
        }
        else if(EventsContainer[event].Operations[operationID].affectedVariable == "manage_wrap"){
            int categoryIndex = (int)EventsContainer[event].Operations[operationID].choosenDoubles[0];
            int wrapperIndex = (int)EventsContainer[event].Operations[operationID].choosenDoubles[1];
            double containerHeight = EventsContainer[event].Operations[operationID].choosenDoubles[2];
            manageWrapper(categoryIndex, wrapperIndex, &Objects[objIndex], containerHeight);
        }
    }
}
void AncestorObject::operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs){
    for(AncestorObject & Object : Objects){
        if(EditableText.connectedObject == Object.getID()
           || Object.isInAGroup(EditableText.connectedGroup)){
            bool success = false;
            if(EditableText.affectedModule == "ancestor"){
                success = EditableText.controlAncestor(Object, listOfAncestorIDs);
            }
            else if(EditableText.affectedModule == "text"){
                for(auto & Text : Object.TextContainer){
                    if(EditableText.connectedModuleID == Text.getID()){
                        success = EditableText.controlText(Text, Object.textContainerIDs);
                    }
                }
            }
            else if(EditableText.affectedModule == "image"){
                for(auto & Image : Object.ImageContainer){
                    if(EditableText.connectedModuleID == Image.getID()){
                        success = EditableText.controlImage(Image, BitmapContainer, Object.imageContainerIDs);
                    }
                }
            }
            else if(EditableText.affectedModule == "movement"){
                for(auto & Movement : Object.MovementContainer){
                    if(EditableText.connectedModuleID == Movement.getID()){
                        success = EditableText.controlMovement(Movement, Object.movementContainerIDs);
                    }
                }
            }
            else if(EditableText.affectedModule == "collision"){
                for(auto & Collision : Object.CollisionContainer){
                    if(EditableText.connectedModuleID == Collision.getID()){
                        success = EditableText.controlCollision(Collision, Object.collisionContainerIDs);
                    }
                }
            }
            else if(EditableText.affectedModule == "particles"){
                for(auto & Particles : Object.ParticlesContainer){
                    if(EditableText.connectedModuleID == Particles.getID()){
                        success = EditableText.controlParticles(Particles, Object.particlesContainerIDs);
                    }
                }
            }
            else if(EditableText.affectedModule == "variable"){
                for(auto & Variable : Object.VariablesContainer){
                    if(EditableText.connectedModuleID == Variable.getID()){
                        success = EditableText.controlVariable(Variable, Object.variablesContainerIDs);
                    }
                }
            }
            EditableText.clearContentAfterSuccess(success);
        }
    }
}
void AncestorObject::refreshCoordinates(){
    if(ImageContainer.size() == 0){
        return;
    }
    vec2d minPos, maxPos;
    if(!ImageContainer[0].getIsScaledFromCenter()){
        minPos.set(pos + ImageContainer[0].getPos(false));
        maxPos.set(pos + ImageContainer[0].getPos(false) + ImageContainer[0].getScaledSize());
    }
    else{
        minPos.set(pos + ImageContainer[0].getPos(false) + ImageContainer[0].getSize()/2 - ImageContainer[0].getScaledSize()/2);
        maxPos.set(pos + ImageContainer[0].getPos(false) + ImageContainer[0].getSize()/2 + ImageContainer[0].getScaledSize()/2);
    }

    for(auto Image : ImageContainer){
        if(!Image.getIsScaledFromCenter()){
            minPos.set(min(minPos, pos+Image.getPos(false)));
            maxPos.set(max(maxPos, pos+Image.getPos(false) + Image.getScaledSize()));
        }
        else{
            minPos.set(min(minPos, pos + Image.getPos(false) + Image.getSize()/2 - Image.getScaledSize()/2));
            maxPos.set(max(maxPos, pos + Image.getPos(false) + Image.getSize()/2 + Image.getScaledSize()/2));
        }
    }
    for(auto & Image : ImageContainer){
        Image.setPos(Image.getPos(false)-minPos+pos);
    }

    pos.set(minPos);
    size.set(maxPos-minPos);
}
void AncestorObject::createVectorsOfIds(){
    clearVectorsOfIDs();
    for(TextModule content : TextContainer){
        textContainerIDs.push_back(content.getID());
    }
    for(EditableTextModule content : EditableTextContainer){
        editableTextContainerIDs.push_back(content.getID());
    }
    for(ImageModule content : ImageContainer){
        imageContainerIDs.push_back(content.getID());
    }
    for(MovementModule content : MovementContainer){
        movementContainerIDs.push_back(content.getID());
    }
    for(CollisionModule content : CollisionContainer){
        collisionContainerIDs.push_back(content.getID());
    }
    for(ParticleEffectModule content : ParticlesContainer){
        particlesContainerIDs.push_back(content.getID());
    }
    for(EventModule content : EventsContainer){
        eventsContainerIDs.push_back(content.getID());
    }
    for(EveModule content : EveContainer){
        eveContainerIDs.push_back(content.getID());
    }
    for(VariableModule content : VariablesContainer){
        variablesContainerIDs.push_back(content.getID());
    }
    for(ScrollbarModule content : ScrollbarContainer){
        scrollbarContainerIDs.push_back(content.getID());
    }
}
vec2d AncestorObject::getPosOnCamera(Camera2D * SelectedCamera){
    vec2d finalPos(getPos(false));
    if(!getIsAttachedToCamera())
        finalPos.translate(SelectedCamera->pos/SelectedCamera->zoom + SelectedCamera->visionShift);
    else
        finalPos.translate(SelectedCamera->pos);
    return finalPos;
}
string SuccessInstanceAdded(string module, string ID){
    return "Instance of " + module + "Module with ID: \'" + ID + "\' has been added.\n";
}
string AncestorObject::addModuleInstance(string module, string newID){
    if(module == "text"){
        TextContainer.push_back(TextModule(newID, textContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, TextContainer.back().getID());
    }
    if(module == "editable_text"){
        EditableTextContainer.push_back(EditableTextModule(newID, editableTextContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, EditableTextContainer.back().getID());
    }
    if(module == "image"){
        ImageContainer.push_back(ImageModule(newID, imageContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, ImageContainer.back().getID());
    }
    if(module == "movement"){
        MovementContainer.push_back(MovementModule(newID, movementContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, MovementContainer.back().getID());
    }
    if(module == "collision"){
        CollisionContainer.push_back(CollisionModule(newID, collisionContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, CollisionContainer.back().getID());
    }
    if(module == "particles"){
        ParticlesContainer.push_back(ParticleEffectModule(newID, particlesContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, ParticlesContainer.back().getID());
    }
    if(module == "event"){
        EveContainer.push_back(EveModule(newID, eveContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, EveContainer.back().getID());
    }
    if(module == "variable"){
        VariablesContainer.push_back(VariableModule(newID, &variablesContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, VariablesContainer.back().getID());
    }
    if(module == "scrollbar"){
        ScrollbarContainer.push_back(ScrollbarModule(newID, scrollbarContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, ScrollbarContainer.back().getID());
    }
    return "Error: Module \'" + module + "\' does not exist!\n";
}
string ErrorNoInstance(string module, string ID){
    return "Error: There is no instance of " + module + "Module with ID: \'" + ID + "\'.\n";
}
string SuccessInstanceDestroyed(string module, string ID){
    return "Instance of " + module + "Module with ID: \'" + ID + "\' has been destroyed.\n";
}
string AncestorObject::destroyModuleInstance(string module, string destroyID){
    if(module == "text"){
        return tryRemovingModuleInstance(module, TextContainer, textContainerIDs, destroyID);
    }
    if(module == "editableText"){
        return tryRemovingModuleInstance(module, EditableTextContainer, editableTextContainerIDs, destroyID);
    }
    if(module == "image"){
        return tryRemovingModuleInstance(module, ImageContainer, imageContainerIDs, destroyID);
    }
    if(module == "movement"){
        return tryRemovingModuleInstance(module, MovementContainer, movementContainerIDs, destroyID);
    }
    if(module == "collision"){
        return tryRemovingModuleInstance(module, CollisionContainer, collisionContainerIDs, destroyID);
    }
    if(module == "particles"){
        return tryRemovingModuleInstance(module, ParticlesContainer, particlesContainerIDs, destroyID);
    }
    if(module == "event"){
        return tryRemovingModuleInstance(module, EveContainer, eveContainerIDs, destroyID);
    }
    if(module == "variable"){
        return tryRemovingModuleInstance(module, VariablesContainer, variablesContainerIDs, destroyID);
    }
    if(module == "scrollbar"){
        return tryRemovingModuleInstance(module, ScrollbarContainer, scrollbarContainerIDs, destroyID);
    }
    return "Error: " + module + "Module does not exist!\n";
}

bool ModulesPointers::hasInstanceOfAnyModule(){
    return Texts.size() > 0 || EditableTexts.size() > 0 || Images.size() > 0
        || Movements.size() > 0 || Collisions.size() > 0 || Particles.size() > 0
        || Events.size() > 0 || Variables.size() > 0 || Scrollbars.size() > 0;
}

void deactivateAllVectorsInEditorWindow(AncestorObject * EditorWindow){
    for_each_if(EditorWindow->TextContainer.begin(), EditorWindow->TextContainer.end(), isStringInGroupModule<TextModule>(), deactivateModule<TextModule>());
    for_each_if(EditorWindow->ImageContainer.begin(), EditorWindow->ImageContainer.end(), isStringInGroupModule<ImageModule>(), deactivateModule<ImageModule>());
    for_each_if(EditorWindow->CollisionContainer.begin(), EditorWindow->CollisionContainer.end(), isStringInGroupModule<CollisionModule>(), deactivateModule<CollisionModule>());
    for_each_if(EditorWindow->EditableTextContainer.begin(), EditorWindow->EditableTextContainer.end(), isStringInGroupModule<EditableTextModule>(), deactivateModule<EditableTextModule>());
    for_each_if(EditorWindow->EventsContainer.begin(), EditorWindow->EventsContainer.end(), isStringInGroupModule<EventModule>(), deactivateModule<EventModule>());
    for_each_if(EditorWindow->EveContainer.begin(), EditorWindow->EveContainer.end(), isStringInGroupModule<EveModule>(), deactivateModule<EveModule>());
    for_each_if(EditorWindow->ScrollbarContainer.begin(), EditorWindow->ScrollbarContainer.end(), isStringInGroupModule<ScrollbarModule>(), deactivateModule<ScrollbarModule>());
}
void activateBasedOnId(AncestorObject * EditorWindow, string activateID){
    for_each_if(EditorWindow->TextContainer.begin(), EditorWindow->TextContainer.end(), isIDEqualString<TextModule>(activateID), activateModule<TextModule>());
    for_each_if(EditorWindow->ImageContainer.begin(), EditorWindow->ImageContainer.end(), isIDEqualString<ImageModule>(activateID), activateModule<ImageModule>());
    for_each_if(EditorWindow->CollisionContainer.begin(), EditorWindow->CollisionContainer.end(), isIDEqualString<CollisionModule>(activateID), activateModule<CollisionModule>());
    for_each_if(EditorWindow->EditableTextContainer.begin(), EditorWindow->EditableTextContainer.end(), isIDEqualString<EditableTextModule>(activateID), activateModule<EditableTextModule>());
    for_each_if(EditorWindow->EventsContainer.begin(), EditorWindow->EventsContainer.end(), isIDEqualString<EventModule>(activateID), activateModule<EventModule>());
    for_each_if(EditorWindow->ScrollbarContainer.begin(), EditorWindow->ScrollbarContainer.end(), isIDEqualString<ScrollbarModule>(activateID), activateModule<ScrollbarModule>());
}
void activateBasedOnFirstChar(AncestorObject * EditorWindow, char activateID){
    for_each_if(EditorWindow->TextContainer.begin(), EditorWindow->TextContainer.end(), isIDEqualChar<TextModule>(activateID), activateModule<TextModule>());
    for_each_if(EditorWindow->ImageContainer.begin(), EditorWindow->ImageContainer.end(), isIDEqualChar<ImageModule>(activateID), activateModule<ImageModule>());
    for_each_if(EditorWindow->CollisionContainer.begin(), EditorWindow->CollisionContainer.end(), isIDEqualChar<CollisionModule>(activateID), activateModule<CollisionModule>());
    for_each_if(EditorWindow->EditableTextContainer.begin(), EditorWindow->EditableTextContainer.end(), isIDEqualChar<EditableTextModule>(activateID), activateModule<EditableTextModule>());
    for_each_if(EditorWindow->EventsContainer.begin(), EditorWindow->EventsContainer.end(), isIDEqualChar<EventModule>(activateID), activateModule<EventModule>());
    for_each_if(EditorWindow->ScrollbarContainer.begin(), EditorWindow->ScrollbarContainer.end(), isIDEqualChar<ScrollbarModule>(activateID), activateModule<ScrollbarModule>());
}

void deactivateWrapped(int categoryIndex, AncestorObject * EditorWindow){
    vec2d newPos;
    bool wrapContent = false;
    unsigned int index = 0;
    string wrapperVariableID;
    int wrapperIndex = 0;
    for(VariableModule Variable : EditorWindow->VariablesContainer){
        wrapperVariableID = std::to_string(categoryIndex) + "_wrapper_" + std::to_string(wrapperIndex+1);
        if(Variable.getID() == wrapperVariableID){
            wrapContent = Variable.getBool();
            if(wrapContent == false){
                for(index = 0; index < EditorWindow->TextContainer.size(); index++){
                    if(EditorWindow->TextContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
                        index+=1;
                        index += wrapperIndex * (3) + 1;
                        for(unsigned int i = 0; i < 2; i++){
                            EditorWindow->TextContainer[index].deactivate();
                            index++;
                        }
                        break;
                    }
                }

                for(index = 0; index < EditorWindow->ImageContainer.size(); index++){
                    if(EditorWindow->ImageContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
                        index+=3;
                        index += wrapperIndex * (5) + 1;
                        for(unsigned int i = 0; i < 2; i++){
                            EditorWindow->ImageContainer[index].deactivate();
                            index++;
                        }
                        break;
                    }
                }

                for(index = 0; index < EditorWindow->EditableTextContainer.size(); index++){
                    if(EditorWindow->EditableTextContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
                        index+=0;
                        index += wrapperIndex * (2);
                        for(unsigned int i = 0; i < 2; i++){
                            EditorWindow->EditableTextContainer[index].deactivate();
                            index++;
                        }
                        break;
                    }
                }
            }
            wrapperIndex++;
        }
    }
}

void manageWrapper(int categoryIndex, int wrapperIndex, AncestorObject * EditorWindow, double containerHeight){
    vec2d newPos;
    bool wrapContent = false;
    unsigned int index = 0;
    string wrapperVariableID = std::to_string(categoryIndex) + "_wrapper_" + std::to_string(wrapperIndex+1);
    for(VariableModule Variable : EditorWindow->VariablesContainer){
        if(Variable.getID() == wrapperVariableID){
            wrapContent = Variable.getBool();
            break;
        }
    }

    for(index = 0; index < EditorWindow->TextContainer.size(); index++){
        if(EditorWindow->TextContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
            index+=1;
            index += wrapperIndex * (3) + 1;
            for(unsigned int i = 0; i < 2; i++){
                EditorWindow->TextContainer[index].toggleIsActive();
                index++;
            }

            for(; index < EditorWindow->TextContainer.size(); index++){
                if(EditorWindow->TextContainer[index].getID()[0] != std::to_string(categoryIndex)[0])
                    break;
                newPos = EditorWindow->TextContainer[index].getPos(false);
                if(wrapContent){
                    newPos.y += containerHeight*4;
                }
                else{
                    newPos.y -= containerHeight*4;
                }
                EditorWindow->TextContainer[index].setPos(newPos);
            }
            break;
        }
    }

    for(index = 0; index < EditorWindow->ImageContainer.size(); index++){
        if(EditorWindow->ImageContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
            index+=3;
            index += wrapperIndex * (5) + 1;
            for(unsigned int i = 0; i < 2; i++){
                EditorWindow->ImageContainer[index].toggleIsActive();
                index++;
            }
            for(; index < EditorWindow->ImageContainer.size(); index++){
                if(EditorWindow->ImageContainer[index].getID()[0] != std::to_string(categoryIndex)[0])
                    break;
                newPos = EditorWindow->ImageContainer[index].getPos(false);
                if(wrapContent){
                    newPos.y += containerHeight*4;
                }
                else{
                    newPos.y -= containerHeight*4;
                }
                EditorWindow->ImageContainer[index].setPos(newPos);
            }
            break;
        }
    }

    for(index = 0; index < EditorWindow->EditableTextContainer.size(); index++){
        if(EditorWindow->EditableTextContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
            index+=0;
            index += wrapperIndex * (2);
            for(unsigned int i = 0; i < 2; i++){
                EditorWindow->EditableTextContainer[index].toggleIsActive();
                index++;
            }
            for(; index < EditorWindow->EditableTextContainer.size(); index++){
                if(EditorWindow->EditableTextContainer[index].getID()[0] != std::to_string(categoryIndex)[0])
                    break;
                newPos = EditorWindow->EditableTextContainer[index].getPos(false);
                if(wrapContent){
                    newPos.y += containerHeight*4;
                }
                else{
                    newPos.y -= containerHeight*4;
                }
                EditorWindow->EditableTextContainer[index].setPos(newPos);
            }
            break;
        }
    }

    for(index = 0; index < EditorWindow->EventsContainer.size(); index++){
        if(EditorWindow->EventsContainer[index].getID()[0] == std::to_string(categoryIndex)[0]){
            index += wrapperIndex+1;
            for(; index < EditorWindow->EventsContainer.size(); index++){
                if(EditorWindow->EventsContainer[index].getID()[0] != std::to_string(categoryIndex)[0])
                    break;
                newPos = EditorWindow->EventsContainer[index].getPos(false);
                if(wrapContent){
                    newPos.y += containerHeight*4;
                }
                else{
                    newPos.y -= containerHeight*4;
                }
                EditorWindow->EventsContainer[index].setPos(newPos);
            }
            break;
        }
    }
}