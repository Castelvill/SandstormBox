#include "ancestorObject.h"
#include <regex>

vector<string> split_string(const string & str, const string & delimiter){
    vector<string> strings;

    string::size_type pos = 0;
    string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}
vector<string> readLines(const string& filename) {
	std::ifstream File(filename);
	vector<string> lines;

	if(!File){
		std::cerr << "Cannot open file: " << filename << "\n";
	}
    else{
	    for(string line; std::getline(File, line);){
			lines.push_back(line);
		}
	}

	return lines;
}

AncestorObject::AncestorObject(){
    deleted = false;
    isActive = false;
    //blank object
    //std::cout << "Warning: You are creating a blank object - it doesn't have an ID nor layerID.\n";
}
AncestorObject::AncestorObject(string newID, vector<string> &listOfIDs, string newLayerID){
    primaryConstructor(newID, listOfIDs, newLayerID, "");
}
AncestorObject::AncestorObject(unsigned newID, vector<string> &listOfIDs, string newLayerID){
    primaryConstructor(newID, listOfIDs, newLayerID, "");
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
    for(EveModule & Event : EveContainer){
        Event.deleteLater();
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.deleteLater();
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.deleteLater();
    }
}
void AncestorObject::clone(const AncestorObject &Original, vector<string> &listOfUniqueIDs, string newLayerID, const bool & changeOldID){
    if(isStringInVector(reservedIDs, Original.ID)){
        std::cout << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << Original.ID << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        std::cout << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    clearVectorsOfIDs();
    clearContainers();
    PrimaryModule::clone(Original, listOfUniqueIDs, layerID, "", changeOldID);
    for(const TextModule & Text : Original.TextContainer){
        TextContainer.push_back(TextModule());
        TextContainer.back().clone(Text, textContainerIDs, newLayerID, getID(), true);
    }
    for(const EditableTextModule & Editable : Original.EditableTextContainer){
        EditableTextContainer.push_back(EditableTextModule());
        EditableTextContainer.back().clone(Editable, editableTextContainerIDs, newLayerID, getID(), true);
    }
    for(const ImageModule & Image : Original.ImageContainer){
        ImageContainer.push_back(ImageModule());
        ImageContainer.back().clone(Image, imageContainerIDs, newLayerID, getID(), true);
    }
    for(const MovementModule & Movement : Original.MovementContainer){
        MovementContainer.push_back(MovementModule());
        MovementContainer.back().clone(Movement, movementContainerIDs, newLayerID, getID(), true);
    }
    for(const CollisionModule & Collision : Original.CollisionContainer){
        CollisionContainer.push_back(CollisionModule());
        CollisionContainer.back().clone(Collision, collisionContainerIDs, newLayerID, getID(), true);
    }
    for(const ParticleEffectModule & Particle : Original.ParticlesContainer){
        ParticlesContainer.push_back(ParticleEffectModule());
        ParticlesContainer.back().clone(Particle, particlesContainerIDs, newLayerID, getID(), true);
    }
    for(const EveModule & Event : Original.EveContainer){
        EveContainer.push_back(EveModule());
        EveContainer.back().clone(Event, eveContainerIDs, newLayerID, getID(), true);
    }
    for(const VariableModule & Collision : Original.VariablesContainer){
        VariablesContainer.push_back(VariableModule());
        VariablesContainer.back().clone(Collision, variablesContainerIDs, newLayerID, getID(), true);
    }
    for(const ScrollbarModule & Scrollbar : Original.ScrollbarContainer){
        ScrollbarContainer.push_back(ScrollbarModule());
        ScrollbarContainer.back().clone(Scrollbar, scrollbarContainerIDs, newLayerID, getID(), true);
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
        Text.clear();
    }
    for(auto & Editable : EditableTextContainer){
        Editable.clear();
    }
    for(auto & Image : ImageContainer){
        Image.clear();
    }
    for(auto & Movement : MovementContainer){
        Movement.clear();
    }
    for(auto & Collision : CollisionContainer){
        Collision.clear();
    }
    for(auto & Particle : ParticlesContainer){
        Particle.clear();
    }
    for(auto & Event : EventsContainer){
        Event.clear();
    }
    for(auto & Event : EveContainer){
        Event.clear();
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
        TextContainer.push_back(TextModule(newID, &textContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, TextContainer.back().getID());
    }
    if(module == "editable_text"){
        EditableTextContainer.push_back(EditableTextModule(newID, &editableTextContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, EditableTextContainer.back().getID());
    }
    if(module == "image"){
        ImageContainer.push_back(ImageModule(newID, &imageContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, ImageContainer.back().getID());
    }
    if(module == "movement"){
        MovementContainer.push_back(MovementModule(newID, &movementContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, MovementContainer.back().getID());
    }
    if(module == "collision"){
        CollisionContainer.push_back(CollisionModule(newID, &collisionContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, CollisionContainer.back().getID());
    }
    if(module == "particles"){
        ParticlesContainer.push_back(ParticleEffectModule(newID, &particlesContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, ParticlesContainer.back().getID());
    }
    if(module == "event"){
        EveContainer.push_back(EveModule(newID, &eveContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, EveContainer.back().getID());
    }
    if(module == "variable"){
        VariablesContainer.push_back(VariableModule(newID, &variablesContainerIDs, getLayerID(), getID()));
        return SuccessInstanceAdded(module, VariablesContainer.back().getID());
    }
    if(module == "scrollbar"){
        ScrollbarContainer.push_back(ScrollbarModule(newID, &scrollbarContainerIDs, getLayerID(), getID()));
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

VariableModule AncestorObject::getAttributeValue(const string &attribute, const string & detail){
    VariableModule NewValue;

    if(attribute == "is_active"){
        NewValue.setBool(getIsActive());
    }
    else if(attribute == "id"){
        NewValue.setString(getID());
    }
    else if(attribute == "in_group"){
        NewValue.setBool(isInAGroup(detail));
    }
    else if(attribute == "pos_x"){
        NewValue.setDouble(getPos(false).x);
    }
    else if(attribute == "pos_y"){
        NewValue.setDouble(getPos(false).y);
    }
    else if(attribute == "size_x"){
        NewValue.setDouble(getSize().x);
    }
    else if(attribute == "size_y"){
        NewValue.setDouble(getSize().y);
    }
    else if(attribute == "scale_x"){
        NewValue.setDouble(getSize().x);
    }
    else if(attribute == "scale_y"){
        NewValue.setDouble(getSize().y);
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
        NewValue.setBool(false);
    }
    return NewValue;
}

vector <string> changeCodeIntoWords(string input){
    std::regex word_regex("([\\w+\\.*]*\\w+)|;|==|=|>|<|>=|<=|-=|\\+=|/=|\\+\\+|\\-\\-|\\+|-|\\*|/|%|\\[|\\]|\\(|\"|\\)|\"|!", std::regex_constants::icase);
    auto words_begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
    auto words_end = std::sregex_iterator();

    vector <string> output;
    string match_str;

    for(std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        match_str = match.str();
        if (match_str.size() > 0){
            output.push_back(match_str);
        }
    }
    return output;
}
vector <string> mergeStrings(vector <string> code){
    vector <string> merged;
    string buffor = "";
    for(unsigned i = 0; i < code.size(); i++){
        buffor = code[i];
        if(code[i][0] == '\"'){
            buffor = "";
            i++;
            for(; i < code.size(); i++){
                if(code[i][0] == '\"'){
                    break;
                }
                if(i > 0 && buffor.size() > 0 && code[i-1][0] != '\\'
                    && code[i-1][0] != '/' && code[i][0] != '\\' && code[i][0] != '/'){
                    buffor += " ";
                }
                buffor += code[i];
            }
        }
        merged.push_back(buffor);
    }
    return merged;
}
bool prepareNewInstruction(vector<string> words, EveModule & NewEvent, OperaClass *& Operation, bool postOperations, unsigned minLength){
    if(words.size() < minLength){
        std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << words[0] << "\' requires at least \'" << minLength << "\' parameters.\n";
        return false;
    }
    if(!postOperations){
        NewEvent.DependentOperations.push_back(OperaClass());
        NewEvent.DependentOperations.back().instruction = words[0];
        Operation = &NewEvent.DependentOperations.back();
    }
    else{
        NewEvent.PostOperations.push_back(OperaClass());
        NewEvent.PostOperations.back().instruction = words[0];
        Operation = &NewEvent.PostOperations.back();
    }
    
    return true;
}
string optional(const string & word){
    if(word != "_"){
        return word;
    }
    return "";
}
bool optional(const vector<string> & words, unsigned & cursor, string & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor] != "_"){
        variable = words[cursor];
    }
    cursor++;
    return false;
}
bool nextCond(const vector<string> & words, unsigned & cursor, string & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor] != "_"){
        variable = words[cursor];
    }
    if(words[cursor] != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<string> & words, unsigned & cursor, VariableModule & variable, const char & type){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor] != "_"){
        if(type == 'b'){
            if(words[cursor] == "true"){
                variable.setBool(true);
            }
            else if(words[cursor] == "false"){
                variable.setBool(false);
            }
            else{
                variable.setBool(stoi(words[cursor]));
            }
        }
        else if(type == 'i'){
            variable.setInt(stoi(words[cursor]));
        }
        else if(type == 'd'){
            variable.setInt(stod(words[cursor]));
        }
        else if(type == 's'){
            variable.setString(words[cursor]);
        }
    }
    if(words[cursor] != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<string> & words, unsigned & cursor, int & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor] != "_"){
        variable = stoi(words[cursor]);
    }
    if(words[cursor] != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<string> & words, unsigned & cursor, double & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor] != "_"){
        variable = stod(words[cursor]);
    }
    if(words[cursor] != "]"){
        cursor++;
    }
    return false;
}
bool createExpression(const vector<string> & words, unsigned & cursor, vector<ConditionClass> & Expression){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor] == "_"){
        cursor++;
        return true;
    }
    if(words[cursor] != "("){
        std::cout << "Error: In " << __FUNCTION__ << ": Every expression must begin with a parentheses.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    bool inCondition = false;
    while(words[cursor] != ")"){
        if(cursor >= words.size()){
            std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
        if(words[cursor] == "["){
            Expression.push_back(ConditionClass(""));
            inCondition = true;
            cursor++;
        }
        else if(words[cursor] == "]"){
            inCondition = false;
            cursor++;
        }
        else if(!inCondition){
            if(Expression.size() == 0){
                std::cout << "Error: In " << __FUNCTION__ << ": Operator cannot be added to an empty expression.\n";
                return false;
            }
            Expression.back().operators.push_back(words[cursor]);
            cursor++;
        }
        else{
            if(nextCond(words, cursor, Expression.back().Location.source)){ continue; };
            if(Expression.back().Location.source == "bool"){
                if(Expression.back().Location.source == "bool"){
                    Expression.back().Location.source = "literal";
                }
                if(nextCond(words, cursor, Expression.back().Literal, 'b')){ continue; };
            }
            else if(isStringInGroup(Expression.back().Location.source, 7, "int", "key_pressed", "key_pressing", "key_released", "mouse_pressed", "mouse_pressing", "mouse_released")){
                if(Expression.back().Location.source == "int"){
                    Expression.back().Location.source = "literal";
                }
                if(nextCond(words, cursor, Expression.back().Literal, 'i')){ continue; };
            }
            else if(Expression.back().Location.source == "double"){
                if(Expression.back().Location.source == "double"){
                    Expression.back().Location.source = "literal";
                }
                if(nextCond(words, cursor, Expression.back().Literal, 'd')){ continue; };
            }
            else if(Expression.back().Location.source == "string" || Expression.back().Location.source == "context"){
                if(Expression.back().Location.source == "string"){
                    Expression.back().Location.source = "literal";
                }
                if(nextCond(words, cursor, Expression.back().Literal, 's')){ continue; };
            }
            else if(Expression.back().Location.source == "camera"){
                if(nextCond(words, cursor, Expression.back().Location.cameraID)){ continue; };
                if(nextCond(words, cursor, Expression.back().Location.attribute)){ continue; };
            }
            else if(Expression.back().Location.source == "layer"){
                if(nextCond(words, cursor, Expression.back().Location.layerID)){ continue; };
                if(nextCond(words, cursor, Expression.back().Location.attribute)){ continue; };
                if(Expression.back().Location.attribute == "in_group"){
                    if(nextCond(words, cursor, Expression.back().Literal, 's')){ continue; };
                }
            }
            else if(Expression.back().Location.source == "object"){
                if(nextCond(words, cursor, Expression.back().Location.layerID)){ continue; };
                if(nextCond(words, cursor, Expression.back().Location.objectID)){ continue; };
                if(nextCond(words, cursor, Expression.back().Location.moduleType)){ continue; };
                if(nextCond(words, cursor, Expression.back().Location.moduleID)){ continue; };
                if(nextCond(words, cursor, Expression.back().Location.attribute)){ continue; };
                if(isStringInGroup(Expression.back().Location.moduleType, 3, "ancestor", "text", "editable_text")){
                    if(nextCond(words, cursor, Expression.back().Literal, 's')){ continue; };
                }
                else if(Expression.back().Location.moduleType == "mouse"){
                    if(nextCond(words, cursor, Expression.back().Literal, 'i')){ continue; };
                }
                else if(Expression.back().Location.moduleType == "collision"){
                    if(nextCond(words, cursor, Expression.back().Literal, 'i')){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.spareID)){ continue; };
                }
            }
            else if(Expression.back().Location.source == "variable"){
                if(nextCond(words, cursor, Expression.back().Location.moduleID)){ continue; };
            }
        }
        if(cursor >= words.size()){
            std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherStringVector(const vector<string> & words, unsigned & cursor, vector<string> & stringVector){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor] == "_"){
        cursor++;
        return true;
    }
    if(words[cursor] != "["){
        std::cout << "Error: In " << __FUNCTION__ << ": Every context list must begin with a square bracket.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    while(words[cursor] != "]"){
        stringVector.push_back(words[cursor]);
        cursor++;
        if(cursor >= words.size()){
            std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherChildEvents(const vector<string> & words, unsigned & cursor, vector<ChildStruct> & Children){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor] == "_"){
        cursor++;
        return true;
    }
    if(words[cursor] != "["){
        std::cout << "Error: In " << __FUNCTION__ << ": Every context list must begin with a square bracket.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    while(words[cursor] != "]"){
        Children.push_back(ChildStruct(words[cursor]));
        cursor++;
        if(cursor >= words.size()){
            std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherLiterals(const vector<string> & words, unsigned & cursor, vector<VariableModule> & Literals, char type){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor] == "_"){
        cursor++;
        return true;
    }
    if(words[cursor] != "["){
        std::cout << "Error: In " << __FUNCTION__ << ": Every context list must begin with a square bracket.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    while(words[cursor] != "]"){
        if(type == 'b'){
            if(words[cursor] == "true"){
                Literals.push_back(VariableModule::newBool(true));
            }
            else if(words[cursor] == "false"){
                Literals.push_back(VariableModule::newBool(false));
            }
            else{
                Literals.push_back(VariableModule::newBool(stoi(words[cursor])));
            }
        }
        else if(type == 'i'){
            Literals.push_back(VariableModule::newInt(stoi(words[cursor])));
        }
        else if(type == 'd'){
            Literals.push_back(VariableModule::newDouble(stod(words[cursor])));
        }
        else{
            Literals.push_back(VariableModule::newString(words[cursor]));
        }
        cursor++;
        if(cursor >= words.size()){
            std::cout << "Error: In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherLiterals(const vector<string> & words, unsigned & cursor, vector<VariableModule> & Literals, const string & type){
    if(type == "bool"){
        if(!gatherLiterals(words, cursor, Literals, 'b')){
            std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else if(type == "int"){
        if(!gatherLiterals(words, cursor, Literals, 'i')){
            std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else if(type == "double"){
        if(!gatherLiterals(words, cursor, Literals, 'd')){
            std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else if(type == "string"){
        if(!gatherLiterals(words, cursor, Literals, 's')){
            std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' type does not exist.\n";
        return false;
    }
    return true;
}
void AncestorObject::eventAssembler(vector<string> code){
    vector<string> words;
    EveModule NewEvent = EveModule();
    unsigned cursor = 0;
    OperaClass * Operation;
    bool postOperations = false;
    for(const string & line : code){
        words.clear();
        words = changeCodeIntoWords(line);
        words = mergeStrings(words);
        if(words.size() == 0){
            continue;
        }
        cursor = 1;
        if(words[0] == "start"){
            if(words.size() < 3){
                std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << words[0] << "\' requires 2 parameters.\n";
                return;
            }
            NewEvent = EveModule(words[1], &eveContainerIDs, getLayerID(), getID());
            if(words[2] == "true"){
                NewEvent.loop = true;
            }
            else if(words[2] == "false"){
                NewEvent.loop = false;
            }
            else{
                NewEvent.loop = stoi(words[2]);
            }
        }
        else if(words[0] == "end"){
            EveContainer.push_back(NewEvent);
            NewEvent = EveModule();
            postOperations = false;
        }
        else if(words[0] == "post"){
            postOperations = true;
        }
        else if(words[0] == "triggers"){
            if(words.size() < 2){
                std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << words[0] << "\' requires at least 2 parameters.\n";
                return;
            }
            cursor = 1;
            if(!gatherStringVector(words, cursor, NewEvent.primaryTriggerTypes)){
                std::cout << "Error: In " << __FUNCTION__ << ": Context gather failed.\n";
                return;
            }
        }
        else if(words[0] == "run"){
            if(words.size() < 2){
                std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << words[0] << "\' requires 2 parameters.\n";
                return;
            }
            cursor = 1;
            if(!gatherChildEvents(words, cursor, NewEvent.Children)){
                std::cout << "Error: In " << __FUNCTION__ << ": Context gather failed.\n";
                return;
            }
        }
        else if(words[0] == "if"){
            cursor = 1;
            if(!createExpression(words, cursor, NewEvent.ConditionalChain)){
                std::cout << "Error: In " << __FUNCTION__ << ": Expression creation failed.\n";
                return;
            }
        }
        else if(words[0] == "else"){
            if(words.size() < 2){
                std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << words[0] << "\' requires 2 parameters.\n";
                return;
            }
            NewEvent.elseChildID = words[1];
        }
        else if(words[0] == "break" || words[0] == "return"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1)){
                return;
            }
        }
        else if(isStringInGroup(words[0], 4, "first", "last", "all", "random")){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2)){
                return;
            }
            
            Operation->Location.source = words[1];
            cursor = 2;
            if(words[1] == "camera"){
                if(optional(words, cursor, Operation->Location.cameraID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(!createExpression(words, cursor, Operation->ConditionalChain)){
                    std::cout << "Error: In " << __FUNCTION__ << ": Expression creation failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1] == "layer"){
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(!createExpression(words, cursor, Operation->ConditionalChain)){
                    std::cout << "Error: In " << __FUNCTION__ << ": Expression creation failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1] == "context" || words[1] == "_"){
                if(!gatherStringVector(words, cursor, Operation->dynamicIDs)){
                    std::cout << "Error: In " << __FUNCTION__ << ": Context gather failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(!createExpression(words, cursor, Operation->ConditionalChain)){
                    std::cout << "Error: In " << __FUNCTION__ << ": Expression creation failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
        }
        else if(words[0] == "index"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2)){
                return;
            }
            Operation->Location.source = words[1];
            cursor = 2;
            if(!gatherLiterals(words, cursor, Operation->Literals, 'i')){
                std::cout << "Error: In " << __FUNCTION__ << ": Literal creation failed.\n";
                return;
            }
            if(!gatherStringVector(words, cursor, Operation->dynamicIDs)){
                std::cout << "Error: In " << __FUNCTION__ << ": Context gather failed.\n";
                return;
            }
            if(optional(words, cursor, Operation->Location.attribute)){ continue; }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(isStringInGroup(words[0], 13, "sum", "intersection", "difference", "+", "-", "*", "/", "=", "+=", "-=", "*=", "/=", "in")){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3)){
                return;
            }
            Operation->dynamicIDs.push_back(words[1]);
            Operation->dynamicIDs.push_back(words[2]);
            cursor = 3;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0] == "++" || words[0] == "--" || words[0] == "delete"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1)){
                return;
            }
            if(words.size() <= 1){
                continue;
            }
            Operation->dynamicIDs.push_back(words[1]);
            cursor = 2;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0] == "value"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2)){
                return;
            }
            cursor = 1;
            if(!createExpression(words, cursor, Operation->ConditionalChain)){
                std::cout << "Error: In " << __FUNCTION__ << ": Expression creation failed.\n";
                return;
            }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0] == "literal"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3)){
                return;
            }
            cursor = 2;
            if(words[1] == "bool"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'b')){
                    std::cout << "Error: In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[1] == "int"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'i')){
                    std::cout << "Error: In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[1] == "double"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'd')){
                    std::cout << "Error: In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[1] == "string"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 's')){
                    std::cout << "Error: In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else{
                std::cout << "Error: In " << __FUNCTION__ << ": Literal type is required.\n";
                return;
            }
            
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0] == "random_int"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 4)){
                return;
            }
            if(words[1] == "literal"){
                Operation->Literals.push_back(VariableModule::newInt(stoi(words[2])));
                Operation->Literals.push_back(VariableModule::newInt(stoi(words[3])));
            }
            else{
                Operation->dynamicIDs.push_back(words[2]);
                Operation->dynamicIDs.push_back(words[3]);
            }
            cursor = 4;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0] == "find_by_id"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2)){
                return;
            }
            Operation->Location.source = words[1];
            cursor = 2;
            if(words[1] == "camera"){
                if(optional(words, cursor, Operation->Location.cameraID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1] == "layer"){
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1] == "context" || words[1] == "_"){
                if(!gatherStringVector(words, cursor, Operation->dynamicIDs)){
                    std::cout << "Error: In " << __FUNCTION__ << ": Context gather failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
        }
        else if(words[0] == "let"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3)){
                return;
            }
            Operation->dynamicIDs.push_back(words[1]);
            Operation->newContextID = words[2];
        }
        else if(words[0] == "clone"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3)){
                return;
            }
            Operation->dynamicIDs.push_back(words[1]);
            Operation->dynamicIDs.push_back(words[2]);
            if(words.size() >= 4){
                if(words[3] == "true"){
                    Operation->Literals.push_back(VariableModule::newBool(true));
                }
                else if(words[3] == "false"){
                    Operation->Literals.push_back(VariableModule::newBool(false));
                }
                else{
                    Operation->Literals.push_back(VariableModule::newBool(stoi(words[3])));
                }
            }
        }
        else if(words[0] == "new"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2)){
                return;
            }
            Operation->Location.source = words[1];
            cursor = 2;
            if(Operation->Location.source == "object"){
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
            }
            else if(Operation->Location.source != "camera" && Operation->Location.source != "layer"){
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
            }
            if(words.size() <= cursor){
                continue;
            }
            if(words[cursor] != "_"){
                Operation->Literals.push_back(VariableModule::newInt(stoi(words[cursor])));
            }
            cursor++;
            if(words.size() <= cursor){
                continue;
            }
            if(words[cursor] != "_"){
                Operation->dynamicIDs.push_back(words[cursor]);
            }
            cursor++;
            if(words.size() <= cursor){
                continue;
            }
            if(words[cursor] != "_"){
                Operation->dynamicIDs.push_back(words[cursor]);
            }
            cursor++;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0] == "bind"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 4)){
                return;
            }
            Operation->dynamicIDs.push_back(words[1]);
            Operation->Literals.push_back(VariableModule::newString(words[2]));
            if(isStringInGroup(words[2], 5, "literal", "l", "remove_literal", "rliteral", "rl")){
                cursor = 3;
                if(!gatherLiterals(words, cursor, Operation->Literals, 's')){
                    std::cout << "Error: In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(isStringInGroup(words[2], 5, "context", "c", "remove_context", "rcontext", "rc")){
                Operation->dynamicIDs.push_back(words[3]);
            }
            else if(words[2] != "reset" && words[2] != "r"){
                std::cout << "Error: In " << __FUNCTION__ << ": In bind instruction, type must be equal to one of these values: "
                    "\"literal\", \"l\", \"remove_literal\", \"rliteral\", \"rl\", \"context\", \"c\", \"remove_context\", \"rcontext\", \"rc\", \"reset\", \"r\".\n";
                return;
            }
        }
        else if(words[0] == "build"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2)){
                return;
            }
            Operation->dynamicIDs.push_back(words[1]);
            if(words.size() > 2 && words[2] != "_"){
                if(words[2] == "true"){
                    Operation->Literals.push_back(VariableModule::newBool(true));
                }
                else if(words[2] == "false"){
                    Operation->Literals.push_back(VariableModule::newBool(false));
                }
                else{
                    Operation->Literals.push_back(VariableModule::newBool(stoi(words[2])));
                }
            }     
        }
        else if(words[0] == "fun"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3)){
                return;
            }
            Operation->dynamicIDs.push_back(words[1]);
            Operation->Location.attribute = words[2];
            if(words.size() < 5){
                continue;
            }
            if(words[3] == "context" || words[3] == "c"){
                Operation->dynamicIDs.push_back(words[4]);
            }
            else{
                cursor = 4;
                if(!gatherLiterals(words, cursor, Operation->Literals, words[3])){
                    return;
                }
            }
        }
        else if(words[0] == "env"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3)){
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1]));
            if(words[1] == "window_title"){
                Operation->Literals.push_back(VariableModule::newString(words[2]));
            }
            else if(words[1] == "reservation_multiplier"){
                Operation->Literals.push_back(VariableModule::newDouble(stod(words[2])));
            }
            else if(words[1] == "window_size"){
                if(words.size() < 4){
                    std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << Operation->instruction <<
                        "\' with \'" << Operation->Literals[0].getString() << "\' attribute requires two literals of numeric type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newInt(stoi(words[2])));
                Operation->Literals.push_back(VariableModule::newInt(stoi(words[3])));
            }
            else{
                Operation->Literals.push_back(VariableModule::newInt(stoi(words[2])));
            }
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": Instruction \'" << words[0] << "\' does not exist.\n";
        }
    }
    if(words.size() > 0){
        if(words[0] != "end"){
            std::cout << "Error: In " << __FUNCTION__ << ": Every event must end with \"end\" instruction.\n";
        }
    }
}
void AncestorObject::translateAllScripts(bool clearEvents){
    if(clearEvents){
        for(auto & Event : EveContainer){
            Event.clear();
        }
        EveContainer.clear();
        eveContainerIDs.clear();
    }

    vector<string> code;
    
    for(string fileName : bindedScripts){
        code = readLines(fileName);
        eventAssembler(code);
        code.clear();
    }
}

bool ModulesPointers::hasInstanceOfAnyModule() const{
    return Texts.size() > 0 || EditableTexts.size() > 0 || Images.size() > 0
        || Movements.size() > 0 || Collisions.size() > 0 || Particles.size() > 0
        || Events.size() > 0 || Variables.size() > 0 || Scrollbars.size() > 0;
}
unsigned ModulesPointers::size() const{
    return Texts.size() + EditableTexts.size() + Images.size() +
        Movements.size() + Collisions.size() + Particles.size() +
        Events.size() + Variables.size() + Scrollbars.size();
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