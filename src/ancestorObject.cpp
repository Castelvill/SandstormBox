#include "ancestorObject.h"

AncestorObject::AncestorObject(){
    deleted = false;
    isActive = true;
    canBeMovedWithMouse = false;
    canDrawSelectionBorder = false;
    hasInvalidatedMemory = true;
}
AncestorObject::AncestorObject(size_t & topObjectUniqueIndex){
    setUniqueIndex(topObjectUniqueIndex++);
    deleted = false;
    isActive = true;
    canBeMovedWithMouse = false;
    canDrawSelectionBorder = false;
    hasInvalidatedMemory = true;
}
void AncestorObject::deleteLater(){
    deleted = true;
    deactivate();
    for(TextModule & Text : TextContainer){
        Text.deleteLater();
    }
    for(EditableTextModule & EditableText : EditableTextContainer){
        EditableText.deleteLater();
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.deleteLater();
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.deleteLater();
    }
    for(ImageModule & Image : ImageContainer){
        Image.deleteLater();
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.deleteLater();
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.deleteLater();
    }
    for(ParticleEffectModule & Particles : ParticlesContainer){
        Particles.deleteLater();
    }
    for(EventModule & Event : EventContainer){
        Event.deleteLater();
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.deleteLater();
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.deleteLater();
    }
    for(PrimitivesModule & Primitives : PrimitivesContainer){
        Primitives.deleteLater();
    }
    for(VectorModule & Vector : VectorContainer){
        Vector.deleteLater();
    }
}
void AncestorObject::findIndexesOfEventChildren(bool postDelete){
    for(EventModule & ParentEvent : EventContainer){
        for(ChildStruct & Child : ParentEvent.Children){
            unsigned childEventIdx = 0;
            for(; childEventIdx < EventContainer.size(); childEventIdx++){
                if(Child.id == EventContainer[childEventIdx].getID()){
                    Child.uniqueIndex = EventContainer[childEventIdx].getUniqueIndex();
                    Child.containerIndex = childEventIdx;
                    break;
                }
            }
            if(childEventIdx == EventContainer.size()){
                if(!postDelete){
                    printLogMessage("Error", __FILE__, __LINE__, __FUNCTION__,
                        "Child '" + Child.id + "' of the event '" + ParentEvent.getID()
                        + "' does not exist in the event container.\n"
                    );
                }
                else{
                    printLogMessage("Warning", __FILE__, __LINE__, __FUNCTION__,
                        "Child '" + Child.id + "' of the event '" + ParentEvent.getID()
                        + "' has been deleted.\n"
                    );
                }  
            }
        }
    }
}
void AncestorObject::clone(const AncestorObject &Original, vector<string> &listOfUniqueIDs, 
    size_t layerUniqueIndex, const string & newLayerID, bool changeOldID,
    size_t & topModuleUniqueIndex
){
    if(isStringInVector(reservedIDs, Original.ID)){
        cerr << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << Original.ID 
            << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << ID 
            << "\' cannot be changed.\n";
        return;
    }
    clearVectorsOfIDs();
    clear();
    
    PrimaryData initData = {
        .objectUniqueIndex = getUniqueIndex(),
        .layerUniqueIndex = layerUniqueIndex,
        .listOfIDs = &listOfUniqueIDs,
        .newLayerID = newLayerID
    };
    PrimaryModule::clone(Original, initData, changeOldID);

    initData.newObjectID = ID;

    hasInvalidatedMemory = true;
    for(const TextModule & Text : Original.TextContainer){
        TextContainer.emplace_back(TextModule(topModuleUniqueIndex));
        initData.listOfIDs = &textContainerIDs;
        TextContainer.back().clone(Text, initData, true);
    }
    for(const EditableTextModule & Editable : Original.EditableTextContainer){
        EditableTextContainer.emplace_back(EditableTextModule(topModuleUniqueIndex));
        initData.listOfIDs = &editableTextContainerIDs;
        EditableTextContainer.back().clone(Editable, initData, true);
    }
    for(const SuperTextModule & SuperText : Original.SuperTextContainer){
        SuperTextContainer.emplace_back(SuperTextModule(topModuleUniqueIndex));
        initData.listOfIDs = &superTextContainerIDs;
        SuperTextContainer.back().clone(SuperText, initData, true);
    }
    for(const SuperEditableTextModule & SuperEditableText : Original.SuperEditableTextContainer){
        SuperEditableTextContainer.emplace_back(SuperEditableTextModule(topModuleUniqueIndex));
        initData.listOfIDs = &superEditableTextContainerIDs;
        SuperEditableTextContainer.back().clone(SuperEditableText, initData, true);
    }
    for(const ImageModule & Image : Original.ImageContainer){
        ImageContainer.emplace_back(ImageModule(topModuleUniqueIndex));
        initData.listOfIDs = &imageContainerIDs;
        ImageContainer.back().clone(Image, initData, true);
    }
    for(const MovementModule & Movement : Original.MovementContainer){
        MovementContainer.emplace_back(MovementModule(topModuleUniqueIndex));
        initData.listOfIDs = &movementContainerIDs;
        MovementContainer.back().clone(Movement, initData, true);
    }
    for(const CollisionModule & Collision : Original.CollisionContainer){
        CollisionContainer.emplace_back(CollisionModule(topModuleUniqueIndex));
        initData.listOfIDs = &collisionContainerIDs;
        CollisionContainer.back().clone(Collision, initData, true);
    }
    for(const ParticleEffectModule & Particle : Original.ParticlesContainer){
        ParticlesContainer.emplace_back(ParticleEffectModule(topModuleUniqueIndex));
        initData.listOfIDs = &particlesContainerIDs;
        ParticlesContainer.back().clone(Particle, initData, true);
    }
    for(const EventModule & Event : Original.EventContainer){
        EventContainer.emplace_back(EventModule(topModuleUniqueIndex));
        initData.listOfIDs = &eventContainerIDs;
        EventContainer.back().clone(Event, initData, true);
    }
    for(const VariableModule & Variable : Original.VariablesContainer){
        VariablesContainer.emplace_back(VariableModule(topModuleUniqueIndex));
        initData.listOfIDs = &variablesContainerIDs;
        VariablesContainer.back().clone(Variable, initData, true);
    }
    for(const ScrollbarModule & Scrollbar : Original.ScrollbarContainer){
        ScrollbarContainer.emplace_back(ScrollbarModule(topModuleUniqueIndex));
        initData.listOfIDs = &scrollbarContainerIDs;
        ScrollbarContainer.back().clone(Scrollbar, initData, true);
    }
    for(const PrimitivesModule & Primitives : Original.PrimitivesContainer){
        PrimitivesContainer.emplace_back(PrimitivesModule(topModuleUniqueIndex));
        initData.listOfIDs = &primitivesContainerIDs;
        PrimitivesContainer.back().clone(Primitives, initData, true);
    }
    for(const VectorModule & Vector : Original.VectorContainer){
        VectorContainer.emplace_back(VectorModule(topModuleUniqueIndex));
        initData.listOfIDs = &vectorContainerIDs;
        VectorContainer.back().clone(Vector, initData, true);
    }

    bindedScripts.insert(bindedScripts.end(), Original.bindedScripts.begin(), 
        Original.bindedScripts.end()
    );
    canBeMovedWithMouse = Original.canBeMovedWithMouse;

    findIndexesOfEventChildren();
}
void AncestorObject::clearVectorsOfIDs(){
    textContainerIDs.clear();
    editableTextContainerIDs.clear();
    superTextContainerIDs.clear();
    superEditableTextContainerIDs.clear();
    imageContainerIDs.clear();
    movementContainerIDs.clear();
    collisionContainerIDs.clear();
    particlesContainerIDs.clear();
    eventContainerIDs.clear();
    variablesContainerIDs.clear();
    scrollbarContainerIDs.clear();
    primitivesContainerIDs.clear();
    vectorContainerIDs.clear();
}
void AncestorObject::clear(){
    for(TextModule & Text : TextContainer){
        Text.clear();
    }
    for(EditableTextModule & Editable : EditableTextContainer){
        Editable.clear();
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.clear();
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.clear();
    }
    for(ImageModule & Image : ImageContainer){
        Image.clear();
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.clear();
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.clear();
    }
    for(ParticleEffectModule & Particle : ParticlesContainer){
        Particle.clear();
    }
    for(EventModule & Event : EventContainer){
        Event.clear();
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.clear();
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.clear();
    }
    for(PrimitivesModule & Primitive : PrimitivesContainer){
        Primitive.clear();
    }
    for(VectorModule & Vector : VectorContainer){
        Vector.clear();
    }

    clearVectorsOfIDs();
    groups.clear();
    TextContainer.clear();
    EditableTextContainer.clear();
    SuperTextContainer.clear();
    SuperEditableTextContainer.clear();
    ImageContainer.clear();
    MovementContainer.clear();
    CollisionContainer.clear();
    ParticlesContainer.clear();
    VariablesContainer.clear();
    ScrollbarContainer.clear();
    PrimitivesContainer.clear();
    VectorContainer.clear();
    bindedScripts.clear();
}
void AncestorObject::operateTextFieldUpdate(EditableTextModule & EditableText,
    vector<AncestorObject> & Objects, vector<SingleBitmap> & BitmapContainer,
    vector<string> & listOfAncestorIDs, string workingDirectory
){
    for(AncestorObject & Object : Objects){
        if(EditableText.connectedObject == Object.getID()
           || Object.isInAGroup(EditableText.connectedGroup)
        ){
            bool success = false;
            if(EditableText.connectedModule == "ancestor"){
                if(EditableText.connectedVariable == "is_scrollable"){
                    Object.setIsScrollable(stringToBool(EditableText.getCurrentContent()));
                    success = true;
                }
                else{
                    success = EditableText.controlAncestor(Object, listOfAncestorIDs);
                }
            }
            else if(EditableText.connectedModule == "text"){
                for(auto & Text : Object.TextContainer){
                    if(EditableText.connectedModuleID == Text.getID()){
                        success = EditableText.controlText(Text, Object.textContainerIDs);
                    }
                }
            }
            else if(EditableText.connectedModule == "image"){
                for(auto & Image : Object.ImageContainer){
                    if(EditableText.connectedModuleID == Image.getID()){
                        success = EditableText.controlImage(Image, BitmapContainer, Object.imageContainerIDs, workingDirectory);
                    }
                }
            }
            else if(EditableText.connectedModule == "movement"){
                for(auto & Movement : Object.MovementContainer){
                    if(EditableText.connectedModuleID == Movement.getID()){
                        success = EditableText.controlMovement(Movement, Object.movementContainerIDs);
                    }
                }
            }
            else if(EditableText.connectedModule == "collision"){
                for(auto & Collision : Object.CollisionContainer){
                    if(EditableText.connectedModuleID == Collision.getID()){
                        success = EditableText.controlCollision(Collision, Object.collisionContainerIDs);
                    }
                }
            }
            else if(EditableText.connectedModule == "particles"){
                for(auto & Particles : Object.ParticlesContainer){
                    if(EditableText.connectedModuleID == Particles.getID()){
                        success = EditableText.controlParticles(Particles, Object.particlesContainerIDs);
                    }
                }
            }
            else if(EditableText.connectedModule == "variable"){
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
void AncestorObject::refreshPositionsAndSizesOfObjectAndItsImages(){
    // cerr << "Error: Method is currently deprecated.\n"; // Idk what was the reason of this "deprecation". For now will ignore it.
    // return;
    
    if(ImageContainer.size() == 0){
        return;
    }
    vec2d minPos, maxPos;
    if(!ImageContainer[0].getIsScaledFromCenter()){
        minPos.set(pos + ImageContainer[0].getPos());
        maxPos.set(pos + ImageContainer[0].getPos() + ImageContainer[0].getScaledSize());
    }
    else{
        minPos.set(pos + ImageContainer[0].getPos() + ImageContainer[0].getSize()/2 - ImageContainer[0].getScaledSize()/2);
        maxPos.set(pos + ImageContainer[0].getPos() + ImageContainer[0].getSize()/2 + ImageContainer[0].getScaledSize()/2);
    }

    for(const ImageModule & Image : ImageContainer){
        if(!Image.getIsScaledFromCenter()){
            minPos.set(min(minPos, pos+Image.getPos()));
            maxPos.set(max(maxPos, pos+Image.getPos() + Image.getScaledSize()));
        }
        else{
            minPos.set(min(minPos, pos + Image.getPos() + Image.getSize()/2 - Image.getScaledSize()/2));
            maxPos.set(max(maxPos, pos + Image.getPos() + Image.getSize()/2 + Image.getScaledSize()/2));
        }
    }
    for(auto & Image : ImageContainer){
        Image.setPos(Image.getPos()-minPos+pos);
    }

    pos.set(minPos);
    size.set(maxPos-minPos);
}
void AncestorObject::createVectorsOfIds(){
    clearVectorsOfIDs();
    for(const TextModule & content : TextContainer){
        textContainerIDs.emplace_back(content.getID());
    }
    for(const EditableTextModule & content : EditableTextContainer){
        editableTextContainerIDs.emplace_back(content.getID());
    }
    for(const SuperTextModule & content : SuperTextContainer){
        superTextContainerIDs.emplace_back(content.getID());
    }
    for(const SuperEditableTextModule & content : SuperEditableTextContainer){
        superEditableTextContainerIDs.emplace_back(content.getID());
    }
    for(const ImageModule & content : ImageContainer){
        imageContainerIDs.emplace_back(content.getID());
    }
    for(const MovementModule & content : MovementContainer){
        movementContainerIDs.emplace_back(content.getID());
    }
    for(const CollisionModule & content : CollisionContainer){
        collisionContainerIDs.emplace_back(content.getID());
    }
    for(const ParticleEffectModule & content : ParticlesContainer){
        particlesContainerIDs.emplace_back(content.getID());
    }
    for(const EventModule & content : EventContainer){
        eventContainerIDs.emplace_back(content.getID());
    }
    for(const VariableModule & content : VariablesContainer){
        variablesContainerIDs.emplace_back(content.getID());
    }
    for(const ScrollbarModule & content : ScrollbarContainer){
        scrollbarContainerIDs.emplace_back(content.getID());
    }
    for(const PrimitivesModule & content : PrimitivesContainer){
        primitivesContainerIDs.emplace_back(content.getID());
    }
    for(const VectorModule & content : VectorContainer){
        vectorContainerIDs.emplace_back(content.getID());
    }
}
vec2d AncestorObject::getPosOnCamera(Camera2D * SelectedCamera){
    vec2d finalPos(getPos());
    if(getIsScrollable()){
        finalPos.translate(SelectedCamera->pos);
    }
    else{
        finalPos.translate(SelectedCamera->pos/SelectedCamera->zoom + SelectedCamera->visionShift);
    }
    return finalPos;
}

void AncestorObject::setID(const string & newID, vector<string> &listOfIDs){
    PrimaryModule::setID(newID, listOfIDs);
}

void AncestorObject::primaryConstructor(PrimaryData & data){
    PrimaryModule::primaryConstructor(data);
    hasInvalidatedMemory = true;
}

void AncestorObject::setIsScrollable(bool newValue){
    isScrollable = newValue;
    for(TextModule & Text : TextContainer){
        Text.setIsScrollable(isScrollable);
    }
    for(EditableTextModule & Editable : EditableTextContainer){
        Editable.setIsScrollable(isScrollable);
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.setIsScrollable(isScrollable);
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.setIsScrollable(isScrollable);
    }
    for(ImageModule & Image : ImageContainer){
        Image.setIsScrollable(isScrollable);
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.setIsScrollable(isScrollable);
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.setIsScrollable(isScrollable);
    }
    for(ParticleEffectModule & Particle : ParticlesContainer){
        Particle.setIsScrollable(isScrollable);
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.setIsScrollable(isScrollable);
    }
    for(PrimitivesModule & Primitives : PrimitivesContainer){
        Primitives.setIsScrollable(isScrollable);
    }
}

VariableModule AncestorObject::getAttributeValue(const AttributeType &attribute, const string & detail){
    VariableModule NewValue;
    switch(attribute){
        case is_active:
            NewValue.setBool(getIsActive());
            break;
        case id:
            NewValue.setString(getID());
            break;
        case layer_id:
            NewValue.setString(getLayerID());
            break;
        case in_group:
            NewValue.setBool(isInAGroup(detail));
            break;
        case pos_x:
            NewValue.setDouble(getPos().x);
            break;
        case pos_y:
            NewValue.setDouble(getPos().y);
            break;
        case size_x:
            NewValue.setDouble(getSize().x);
            break;
        case size_y:
            NewValue.setDouble(getSize().y);
            break;
        case scale_x:
            NewValue.setDouble(getSize().x);
            break;
        case scale_y:
            NewValue.setDouble(getSize().y);
            break;
        case can_draw_selection_border:
            NewValue.setBool(canDrawSelectionBorder);
            break;
        default:
            cerr << "Error: In " << __FUNCTION__ <<
                ":\n" << NEW_LINE_PADDING << "Attribute '" << attributeToStr(attribute) << "' is not valid.\n";
            NewValue.setBool(false);
            break;
    }
    return NewValue;
}
vector<vector<WordStruct>> CodeGenerator::preprocessTokens(const vector<WordStruct> & inputTokens){
    if(inputTokens.size() == 0){
        return vector<vector<WordStruct>>{};
    }

    vector<vector<WordStruct>> generatedCode;

    vector<WordStruct> modifiedInput;

    generatedCode.push_back(inputTokens);

    return generatedCode;
}
void AncestorObject::clearAllEvents(){
    for(auto & Event : EventContainer){
        Event.clear();
    }
    EventContainer.clear();
    eventContainerIDs.clear();
}
inline void printEmptyFileWarning(string scriptName, string functionName){
    cout << "Warning: In " << functionName
        << ": Script '" << scriptName << "' is empty or cannot be opened.\n";
}
inline void printBasicFile(){
    cout << "Basic \"Hello World\" program:\n\n"
        << "@trigger on_init\n"
        << "start helloWorld\n"
        << "\tprint \"Hello, World!\\n\"\n"
        << "\texit\n"
        << "end\n\n";
}

void AncestorObject::translateAllScripts(const string & exePath, bool clearEvents,
    bool allowNotAscii, vector<VariableLocationStruct> & globalScope, unsigned & topMemoryAddress,
    size_t & topModuleUniqueIndex
){
    if(clearEvents){
        clearAllEvents();
    }

    if(bindedScripts.empty()){
        printBasicFile();
        return;
    }

    removeStringDuplicatesFromVector(bindedScripts);

    auto [status, anyAssembledEvents] = compile (exePath, bindedScripts, allowNotAscii,
        EventContainer, eventContainerIDs, globalScope, topMemoryAddress, getLayerUniqueIndex(),
        layerID, getUniqueIndex(), ID, topModuleUniqueIndex
    );

    if(!anyAssembledEvents){
        printBasicFile();
    }
}
void AncestorObject::translateScriptsFromPaths(const string & exePath, bool clearEvents,
    vector<string> scriptsPaths, bool allowNotAscii, vector<VariableLocationStruct> & globalScope,
    unsigned & topMemoryAddress, size_t & topModuleUniqueIndex
){
    if(clearEvents){
        clearAllEvents();
    }

    removeStringDuplicatesFromVector(scriptsPaths);

    auto [status, anyAssembledEvents] = compile (exePath, scriptsPaths, allowNotAscii,
        EventContainer, eventContainerIDs, globalScope, topMemoryAddress, getLayerUniqueIndex(),
        layerID, getUniqueIndex(), ID, topModuleUniqueIndex
    );

    if(!anyAssembledEvents){
        printBasicFile();
    }
}
void AncestorObject::translateSubsetBindedScripts(const string & exePath, bool clearEvents,
    vector<string> scripts, bool allowNotAscii, vector<VariableLocationStruct> & globalScope,
    unsigned & topMemoryAddress, size_t & topModuleUniqueIndex
){
    if(clearEvents){
        clearAllEvents();
    }

    removeStringDuplicatesFromVector(bindedScripts);

    vector<string> selectedBindedScripts;
    //Filter out not selected scripts
    for(const string & scriptName : bindedScripts){
        if(isStringInVector(scripts, scriptName)){
            selectedBindedScripts.push_back(scriptName);
        }
    }

    auto [status, anyAssembledEvents] = compile (exePath, selectedBindedScripts, allowNotAscii,
        EventContainer, eventContainerIDs, globalScope, topMemoryAddress, getLayerUniqueIndex(),
        layerID, getUniqueIndex(), ID, topModuleUniqueIndex
    );

    if(!anyAssembledEvents){
        printBasicFile();
    }
}
void AncestorObject::injectCode(bool clearEvents, vector<string> code,
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress,
    size_t & topModuleUniqueIndex
){
    if(clearEvents){
        clearAllEvents();
    }

    code = removeComments(code);

    code = divideStringVectorIntoLines(code);
    
    if(!code.empty()){
        const size_t preAssemblyEventCount = EventContainer.size();
        const size_t preAssemblyModuleUniqueIndex = topModuleUniqueIndex;
        ReturnType assemblyStatus = assembleEvents(EventContainer, eventContainerIDs,
            getLayerUniqueIndex(), layerID, getUniqueIndex(), ID, code,
            "<injection>", GlobalScope, topMemoryAddress, topModuleUniqueIndex
        );
        if(assemblyStatus == ReturnType::ERROR){
            EventContainer.resize(preAssemblyEventCount);
            topModuleUniqueIndex = preAssemblyModuleUniqueIndex;
            return;
        }
    }
}
void AncestorObject::injectInstructions(bool clearEvents, const vector<string> & instructions,
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress,
    size_t & topModuleUniqueIndex
){
    if(clearEvents){
        clearAllEvents();
    }
    
    vector<string> preprocessed;
    string output;
    for(const string & line : instructions){
        output = "";
        for(size_t i = 0; i < line.size(); ++i){
            if(line[i] == '\\' && i + 1 != line.size() && line[i+1] == '\"'){
                output += '\"';
                i++;
                continue;
            }
            output += line[i];
        }
        preprocessed.push_back(output);
    }
    
    preprocessed = removeComments(preprocessed);
    preprocessed.insert(preprocessed.begin(), "start _");
    preprocessed.insert(preprocessed.begin(), "@trigger each_iteration");
    preprocessed.emplace_back("delete_this_event");
    preprocessed.emplace_back("end");
    size_t preAssemblyEventCount = EventContainer.size();
    size_t preAssemblyModuleUniqueIndex = topModuleUniqueIndex;
    ReturnType assemblyStatus = assembleEvents(EventContainer, eventContainerIDs,
        getLayerUniqueIndex(), layerID, getUniqueIndex(), ID, preprocessed, "<injection>",
        GlobalScope, topMemoryAddress, topModuleUniqueIndex
    );
    if(assemblyStatus == ReturnType::ERROR){
        EventContainer.resize(preAssemblyEventCount);
        topModuleUniqueIndex = preAssemblyModuleUniqueIndex;
        return;
    }
}

void AncestorObject::propagateLayer(){
    for(TextModule & Text : TextContainer){
        Text.setLayerUniqueIndex(getLayerUniqueIndex());
        Text.setLayerID(layerID);
    }
    for(EditableTextModule & EditableText : EditableTextContainer){
        EditableText.setLayerUniqueIndex(getLayerUniqueIndex());
        EditableText.setLayerID(layerID);
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.setLayerUniqueIndex(getLayerUniqueIndex());
        SuperText.setLayerID(layerID);
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.setLayerUniqueIndex(getLayerUniqueIndex());
        SuperEditableText.setLayerID(layerID);
    }
    for(ImageModule & Image : ImageContainer){
        Image.setLayerUniqueIndex(getLayerUniqueIndex());
        Image.setLayerID(layerID);
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.setLayerUniqueIndex(getLayerUniqueIndex());
        Movement.setLayerID(layerID);
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.setLayerUniqueIndex(getLayerUniqueIndex());
        Collision.setLayerID(layerID);
    }
    for(ParticleEffectModule & Particles : ParticlesContainer){
        Particles.setLayerUniqueIndex(getLayerUniqueIndex());
        Particles.setLayerID(layerID);
    }
    for(EventModule & Event : EventContainer){
        Event.setLayerUniqueIndex(getLayerUniqueIndex());
        Event.setLayerID(layerID);
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.setLayerUniqueIndex(getLayerUniqueIndex());
        Variable.setLayerID(layerID);
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.setLayerUniqueIndex(getLayerUniqueIndex());
        Scrollbar.setLayerID(layerID);
    }
    for(PrimitivesModule & Primitives : PrimitivesContainer){
        Primitives.setLayerUniqueIndex(getLayerUniqueIndex());
        Primitives.setLayerID(layerID);
    }
    for(VectorModule & Vector : VectorContainer){
        Vector.setLayerUniqueIndex(getLayerUniqueIndex());
        Vector.setLayerID(layerID);
    }
}
void AncestorObject::propagateObjectID(){
    for(TextModule & Text : TextContainer){
        Text.setObjectID(ID);
    }
    for(EditableTextModule & EditableText : EditableTextContainer){
        EditableText.setObjectID(ID);
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.setObjectID(ID);
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.setObjectID(ID);
    }
    for(ImageModule & Image : ImageContainer){
        Image.setObjectID(ID);
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.setObjectID(ID);
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.setObjectID(ID);
    }
    for(ParticleEffectModule & Particles : ParticlesContainer){
        Particles.setObjectID(ID);
    }
    for(EventModule & Event : EventContainer){
        Event.setObjectID(ID);
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.setObjectID(ID);
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.setObjectID(ID);
    }
    for(PrimitivesModule & Primitives : PrimitivesContainer){
        Primitives.setObjectID(ID);
    }
    for(VectorModule & Vector : VectorContainer){
        Vector.setObjectID(ID);
    }
}
string AncestorObject::getObjectID(){
    return ID;
}

bool ModulesPointers::hasInstanceOfAnyModule() const{
    return Texts.size() > 0 || EditableTexts.size() > 0 || SuperTexts.size() > 0
        || SuperEditableTexts.size() > 0 || Images.size() > 0
        || Movements.size() > 0 || Collisions.size() > 0 || Particles.size() > 0
        || Events.size() > 0 || Variables.size() > 0 || Scrollbars.size() > 0
        || Primitives.size() > 0 || Vectors.size() > 0;
}
unsigned ModulesPointers::size() const{
    return Texts.size() + EditableTexts.size() + SuperTexts.size()
        + SuperEditableTexts.size()  + Images.size() +
        Movements.size() + Collisions.size() + Particles.size() +
        Events.size() + Variables.size() + Scrollbars.size()
        + Primitives.size() + Vectors.size();
}

bool ModulesPointers::empty() const{
    return Texts.empty() && EditableTexts.empty() && SuperTexts.empty()
        && SuperEditableTexts.empty()  && Images.empty() &&
        Movements.empty() && Collisions.empty() && Particles.empty() &&
        Events.empty() && Variables.empty() && Scrollbars.empty()
        && Primitives.empty() && Vectors.empty();
}