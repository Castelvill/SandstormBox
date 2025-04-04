#include "ancestorObject.h"
#include <regex>
#include <queue>

vector<string> removeComments(const vector<string> & lines){
    vector<string> newLines;
    unsigned cursor;
    bool commentSection = false;
    unsigned lineNumber = 1;
    for(const string & line : lines){
        newLines.emplace_back("");
        for(cursor = 0; cursor < line.size(); cursor++){
            if(line[cursor] == '/' && cursor < line.size() + 2){
                if(line[cursor + 1] == '/'){
                    break;
                }
                else if(line[cursor + 1] == '*'){
                    commentSection = true;
                    cursor++;
                    continue;
                }
            }
            if(line[cursor] == '*' && cursor < line.size() + 2 && line[cursor + 1] == '/'){
                if(!commentSection){
                    cout << "Syntax error. (in line: " << lineNumber << ")\n";
                    return newLines;
                }
                commentSection = false;
                cursor++;
                continue;
            }
            if(commentSection){
               continue;
            }
            newLines.back() += line[cursor];
        }
        lineNumber++;
    }
    
    return newLines;
}
inline vector<string> builtInScriptInterpreter = {
    "start interpreter",
    "triggers each_iteration",
    "pwd path",
    "print _ _ path \">> \"",
    "console_input input",
    "inject_instr me input",
    "end"
};
inline vector<string> builtInScriptHello = {
    "start interpreter",
    "triggers on_init",
    "print \"Hello, World!\n\"",
    "exit",
    "end"
};
vector<string> readLines(const string& filename, bool allowNotAscii) {
	if(filename.size() == 0){
        cerr << "Error: In " << __FUNCTION__ << ": The file name cannot be empty.\n";
        return vector<string>();
    }
    if(filename[0] == '?'){
        if(filename == "?interpreter"){
            return builtInScriptInterpreter;
        }
        if(filename == "?hello"){
            return builtInScriptHello;
        }
        cerr << "Error: In " << __FUNCTION__ << ": There is no built-in script with the name: \""
            << filename << "\".\n";
        return vector<string>();
    }
    vector<string> lines;

    std::ifstream File(filename);

	if(!File){
		std::cerr << "Cannot open file: " << filename << "\n";
        return lines;
	}
    if(allowNotAscii){
        for(string line; std::getline(File, line);){
            lines.push_back(line);
        }
    }
    else{
        for(string line; std::getline(File, line);){
            lines.emplace_back(removeNotAscii(line));
        }
    }
    
    File.close();

    lines = removeComments(lines);

	return lines;
}

AncestorObject::AncestorObject(){
    deleted = false;
    isActive = false;
    canBeMovedWithMouse = true;
    canDrawSelectionBorder = false;
    //blank object
    //cout << "Warning: You are creating a blank object - it doesn't have an ID nor layerID.\n";
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
void AncestorObject::clone(const AncestorObject &Original, vector<string> &listOfUniqueIDs, string newLayerID, const bool & changeOldID){
    if(isStringInVector(reservedIDs, Original.ID)){
        cerr << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << Original.ID << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    clearVectorsOfIDs();
    clear();
    PrimaryModule::clone(Original, listOfUniqueIDs, newLayerID, "", changeOldID);
    for(const TextModule & Text : Original.TextContainer){
        TextContainer.emplace_back(TextModule());
        TextContainer.back().clone(Text, textContainerIDs, newLayerID, getID(), true);
    }
    for(const EditableTextModule & Editable : Original.EditableTextContainer){
        EditableTextContainer.emplace_back(EditableTextModule());
        EditableTextContainer.back().clone(Editable, editableTextContainerIDs, newLayerID, getID(), true);
    }
    for(const SuperTextModule & SuperText : Original.SuperTextContainer){
        SuperTextContainer.emplace_back(SuperTextModule());
        SuperTextContainer.back().clone(SuperText, superTextContainerIDs, newLayerID, getID(), true);
    }
    for(const SuperEditableTextModule & SuperEditableText : Original.SuperEditableTextContainer){
        SuperEditableTextContainer.emplace_back(SuperEditableTextModule());
        SuperEditableTextContainer.back().clone(SuperEditableText, superEditableTextContainerIDs, newLayerID, getID(), true);
    }
    for(const ImageModule & Image : Original.ImageContainer){
        ImageContainer.emplace_back(ImageModule());
        ImageContainer.back().clone(Image, imageContainerIDs, newLayerID, getID(), true);
    }
    for(const MovementModule & Movement : Original.MovementContainer){
        MovementContainer.emplace_back(MovementModule());
        MovementContainer.back().clone(Movement, movementContainerIDs, newLayerID, getID(), true);
    }
    for(const CollisionModule & Collision : Original.CollisionContainer){
        CollisionContainer.emplace_back(CollisionModule());
        CollisionContainer.back().clone(Collision, collisionContainerIDs, newLayerID, getID(), true);
    }
    for(const ParticleEffectModule & Particle : Original.ParticlesContainer){
        ParticlesContainer.emplace_back(ParticleEffectModule());
        ParticlesContainer.back().clone(Particle, particlesContainerIDs, newLayerID, getID(), true);
    }
    for(const EventModule & Event : Original.EventContainer){
        EventContainer.emplace_back(EventModule());
        EventContainer.back().clone(Event, EventContainerIDs, newLayerID, getID(), true);
    }
    for(const VariableModule & Variable : Original.VariablesContainer){
        VariablesContainer.emplace_back(VariableModule());
        VariablesContainer.back().clone(Variable, variablesContainerIDs, newLayerID, getID(), true);
    }
    for(const ScrollbarModule & Scrollbar : Original.ScrollbarContainer){
        ScrollbarContainer.emplace_back(ScrollbarModule());
        ScrollbarContainer.back().clone(Scrollbar, scrollbarContainerIDs, newLayerID, getID(), true);
    }
    for(const PrimitivesModule & Primitives : Original.PrimitivesContainer){
        PrimitivesContainer.emplace_back(PrimitivesModule());
        PrimitivesContainer.back().clone(Primitives, primitivesContainerIDs, newLayerID, getID(), true);
    }
    for(const VectorModule & Vector : Original.VectorContainer){
        VectorContainer.emplace_back(VectorModule());
        VectorContainer.back().clone(Vector, vectorContainerIDs, newLayerID, getID(), true);
    }

    bindedScripts.insert(bindedScripts.end(), Original.bindedScripts.begin(), Original.bindedScripts.end());
    canBeMovedWithMouse = Original.canBeMovedWithMouse;
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
    eventsContainerIDs.clear();
    EventContainerIDs.clear();
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
void AncestorObject::operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects,
    vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs, string workingDirectory
){
    for(AncestorObject & Object : Objects){
        if(EditableText.connectedObject == Object.getID()
           || Object.isInAGroup(EditableText.connectedGroup)){
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
void AncestorObject::refreshCoordinates(){
    cerr << "Error: Method 'refreshCoordinates' is currently deprecated.\n";
    return;
    
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

    for(auto Image : ImageContainer){
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
        EventContainerIDs.emplace_back(content.getID());
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

void AncestorObject::setID(string newID, vector<string> &listOfIDs){
    PrimaryModule::setID(newID, listOfIDs);
    objectLookupID = ID + layerID;
}

void AncestorObject::primaryConstructor(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    PrimaryModule::primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    objectLookupID = ID + layerID;
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
                ": Attribute '" << attributeToStr(attribute) << "' is not valid.\n";
            NewValue.setBool(false);
            break;
    }
    return NewValue;
}

string findAndUseSpecialCharacters(string input){
    //Translate special letter pairs into special characters.
    string output = "";
    for(unsigned i = 0; i < input.size(); i++){
        if(input[i] == '\\' && i + 1 < input.size()){
            if(input[i + 1] == 'n'){
                output += '\n';
                i++;
                continue;
            }
            else if(input[i + 1] == 't'){
                output += '\t';
                i++;
                continue;
            }
        }
        output += input[i];
    }

    return output;
}

bool canStringBeDouble(string text){
    if(text[0] == '.'){
        return false;
    }
    bool hasPoint = false;
    for(char a : text){
        if(int(a) < 48 || int(a) > 57){
            if(a == '.' && !hasPoint){
                hasPoint = true;
                continue;
            }
            return false;
        }
    }
    return hasPoint;
}

vector<WordStruct> tokenizeCode(string input){
    std::regex word_regex("([\\w+\\.*]*\\w+)|;|:|\\,|\\.|==|=|>=|<=|>|<|-=|\\+=|/=|\\*=|/=|\\*\\*|\\+\\+|\\-\\-|\\+|-|\\*|/|%|\\[|\\]|\\(|\\\\\\\"|\\)|\"|!=|!|\\|\\||&&|\n|\t|&|@|#", std::regex_constants::icase);
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

    vector<string> stringSectors;
    bool isInsideStringSector = false;

    for(size_t i = 0; i < input.size(); i++){
        if(input[i] == '\"' && (i == 0 || input[i - 1] != '\\')){
            if(!isInsideStringSector){
                stringSectors.emplace_back("");
            }
            isInsideStringSector = !isInsideStringSector;
            continue;
        }
        if(isInsideStringSector){
            stringSectors.back() += input[i];
        }
    }

    vector <WordStruct> mergedOutput;
    isInsideStringSector = false;
    unsigned sector = 0;
    string error;

    //If a word is not a part of a string, add it as a single word, otherwise add the whole string as a single word and ignore its parts. Don't add " to the words.
    for(unsigned i = 0; i < output.size(); i++){
        if(output[i][0] == '\"' && !isInsideStringSector){
            isInsideStringSector = true;
            mergedOutput.emplace_back(WordStruct('s', stringSectors[sector], false));
            sector++;
            continue;
        }
        if(isInsideStringSector){
            if(output[i][0] == '\"'){
                isInsideStringSector = false;
            }
            continue;
        }
        if(output[i] == "\t"){
            continue;
        }
        if(i == 0){
            mergedOutput.emplace_back(WordStruct('o', output[i], false)); //operation
            continue;
        }
        if(output[i] == "_"){
            mergedOutput.emplace_back(WordStruct('e', output[i], false)); //empty / null
            continue;
        }
        if(output[i] == "true"){
            mergedOutput.emplace_back(WordStruct('b', "1", false)); //bool
            continue;
        }
        if(output[i] == "false"){
            mergedOutput.emplace_back(WordStruct('b', "0", false)); //bool
            continue;
        }
        if(canStringBeDouble(output[i])){
            cstod(output[i], error);
            if(mergedOutput.size() > 0 && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct('d', "-" + output[i], false)); //double
            }
            else{
                mergedOutput.emplace_back(WordStruct('d', output[i], false)); //double
            }
            continue;
        }
        cstoi(output[i], error);
        if(error == ""){
            if(mergedOutput.size() > 0 && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct('i', "-" + output[i], false)); //int
            }
            else{
                mergedOutput.emplace_back(WordStruct('i', output[i], false)); //int
            }
            
            continue;
        }
        if(mergedOutput.size() > 0 && mergedOutput.back().value == "-"){
            mergedOutput.pop_back();
            mergedOutput.emplace_back(WordStruct('c', output[i], true)); //context / variable
        }
        else{
            mergedOutput.emplace_back(WordStruct('c', output[i], false)); //context / variable
        }
    }

    return mergedOutput;
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
                /*if(i > 0 && buffor.size() > 0 && code[i-1][0] != '\\' &&
                    code[i-1][0] != '/' && code[i][0] != '\\' && code[i][0] != '/'
                    && code[i][0] != ',' && code[i][0] != '.' && code[i-1].back() != '\n'
                    && code[i-1].back() != '\t' &&  code[i-1].back() != '@' && code[i-1].back() != '%'
                    && code[i-1].back() != '*' && code[i-1].back() != '-'  && code[i-1].back() != '+'
                    && code[i-1].back() != ':' && code[i-1].back() != '='
                    && code[i-1].back() != '#' && code[i-1].back() != '.'
                ){
                    buffor += " ";
                }*/
                if(i > 0 && buffor.size() > 0 && code[i-1][0] != '\\' &&
                    code[i-1][0] != '/' && code[i][0] != '\\' && code[i][0] != '/'
                    && code[i-1].back() != '\n' && code[i-1].back() != '\t'
                ){
                    buffor += " ";
                }
                buffor += code[i];
            }
        }
        merged.push_back(buffor);
    }
    return merged;
}
inline string errorSpacing(){return "\t";}; 
bool prepareNewInstruction(vector<WordStruct> words, EventModule & NewEvent, OperationClass *& Operation,
    bool postOperations, unsigned minLength, unsigned lineNumber, string scriptName
){
    if(words.size() < minLength){
        if(minLength == 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least 1 parameter.\n";
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least " << minLength-1 << " parameters.\n";
        }
        return false;
    }
    if(!postOperations){
        NewEvent.DependentOperations.emplace_back(OperationClass());
        NewEvent.DependentOperations.back().instruction = strToInstr(words[0].value);
        NewEvent.DependentOperations.back().scriptName = scriptName;
        NewEvent.DependentOperations.back().lineNumber = lineNumber;
        Operation = &NewEvent.DependentOperations.back();
    }
    else{
        NewEvent.PostOperations.emplace_back(OperationClass());
        NewEvent.PostOperations.back().instruction = strToInstr(words[0].value);
        NewEvent.PostOperations.back().scriptName = scriptName;
        NewEvent.PostOperations.back().lineNumber = lineNumber;
        Operation = &NewEvent.PostOperations.back();
    }
    
    return true;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, string & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        variable = words[cursor].value;
    }
    cursor++;
    return false;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, AttributeType & attribute){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        attribute = strToAttribute(words[cursor].value);
    }
    cursor++;
    return false;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, ValueSource & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        variable = transSource(words[cursor].value);
    }
    cursor++;
    return false;
}
inline string localContextID(const string & eventID, const string & newID){
    return eventID + /*":" +*/ newID;
}
bool optionalOutput(string scriptName, unsigned lineNumber, string & error, const vector<WordStruct> & words,
    unsigned & cursor, vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, const vector<string> & allAvailableEventIDs,
    const DataType & variableType, string & outputVariableID, bool & isOutputReference, bool isGlobal,
    bool canCreateNewsVariable, bool inAfterSection
){
    error = "";
    if(cursor + 1 > words.size()){
        return true;
    }
    if(words[cursor].type != 'e' && words[cursor].type != 'c'){
        error = "Parameter 'output' (" + intToStr(cursor);
        error += ") must be a context.";
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return true;
    }
    if(words[cursor].type == 'c'){
        //Check if the output variable is global and if it exists.
        if(words[cursor].value == "NULL" || words[cursor].value == "me" || words[cursor].value == "my_layer"){
            isGlobal = true;
        }
        bool variableExists = false;
        for(auto & Variable : NewVariablesForLookupTable){
            if(Variable.id == words[cursor].value){
                variableExists = true;
                isGlobal = Variable.eventID.size() == 0;
                break;
            }
        }

        //Add eventID to the output variable if it is not global.
        string usedEventID;
        if(isGlobal){
            outputVariableID = words[cursor].value;
        }
        else{
            outputVariableID = findExistingVariableOrCreateNew(
                NewVariablesForLookupTable, allAvailableEventIDs, words[cursor].value,
                usedEventID, canCreateNewsVariable, scriptName, lineNumber, inAfterSection);
        }
        
        //Check if the output variable is a reference.
        isOutputReference = false;
        for(const StartingVariableStruct & Variable : PassedVariables){
            if(Variable.id == outputVariableID){
                isOutputReference = Variable.isReference;
            }
        }

        //Add this variable to the lookup table if it doesn't already exist.
        if(!variableExists){
            NewVariablesForLookupTable.emplace_back(variableType, outputVariableID, words[cursor].value, 0, usedEventID, isOutputReference);
        }
    }
    cursor++;
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, string & variable, string scriptName, unsigned lineNumber){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e' && words[cursor].value != "]"){
        variable = words[cursor].value;
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, AttributeType & attribute, string scriptName, unsigned lineNumber){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e' && words[cursor].value != "]"){
        attribute = strToAttribute(words[cursor].value);
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, ValueSource & Variable, string scriptName, unsigned lineNumber){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e' && words[cursor].value != "]"){
        Variable = transSource(words[cursor].value);
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, VariableModule & Variable,
    const char & type, string scriptName, unsigned lineNumber, vector<string> & allAvailableEventIDs,
    const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewsVariable, bool inAfterSection
){
    string error = "";
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        if(type == 'b'){
            Variable.setBool(cstoi(words[cursor].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": " << error << "\n";
            }
        }
        else if(type == 'i'){
            Variable.setInt(cstoi(words[cursor].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": " << error << "\n";
            }
        }
        else if(type == 'd'){
            Variable.setDouble(cstod(words[cursor].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": " << error << "\n";
            }
        }
        else if(type == 's'){
            if(words[cursor].type == 's'){
                Variable.setString(words[cursor].value);
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Parameter nr." << cursor << " is not a string.\n";
            }
        }
        else if(type == 'c'){
            if(words[cursor].type == 'c'){
                Variable.setString(createCustomOutput(
                    NewVariablesForLookupTable, PassedVariables, allAvailableEventIDs,
                    words[cursor].value, true, canCreateNewsVariable, scriptName, lineNumber, inAfterSection
                ));
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Parameter nr." << cursor << " is not a context.\n";
            }
        }
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, int & variable, string scriptName, unsigned lineNumber){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        string error;
        variable = cstoi(words[cursor].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": " << error << "\n";
        }
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, double & variable, string scriptName, unsigned lineNumber){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        string error;
        variable = cstod(words[cursor].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": " << error << "\n";
        }
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool createExpression(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    vector<VariableModule> & resultStack, unsigned lineNumber, string scriptName, bool isConditionalExpression,
    vector<string> & allAvailableEventIDs, const vector<StartingVariableStruct> & NewVariablesForLookupTable,
    const vector<StartingVariableStruct> & PassedVariables, bool canCreateNewsVariable, bool inAfterSection
){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return true;
    }

    string endingChar = ")";
    if(isConditionalExpression){
        if(words[cursor].value != "("){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Every expression must begin with parentheses.\n";
            return false;
        }
    }
    else{
        endingChar = "]";
        if(words[cursor].value != "["){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Parameter " << cursor << " must begin with square brackets.\n";
            return false;
        }
    }
    
    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    bool inCondition = false;
    WordStruct firstWord;
    
    while(words[cursor].value != endingChar || inCondition){
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
        if(words[cursor].value == "["){
            Expression.emplace_back(ConditionClass(""));
            inCondition = true;
            cursor++;
        }
        else if(words[cursor].value == "]"){
            inCondition = false;
            cursor++;
        }
        else if(!inCondition){
            if(!isConditionalExpression){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Each value source must be enclosed in seperate square brackets. Correct syntax: [[source_0] [source_1] ...]\n";
                return false;
            }
            if(Expression.size() == 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Operator '" << words[cursor].value << "' cannot be added to an empty expression.\n";
                return false;
            }
            Expression.back().operators.emplace_back(strToInstr(words[cursor].value));
            cursor++;
        }
        else{
            if(words.size() < cursor + 1){ continue; };
            if(words[cursor].type != 'e' && words[cursor].value != "]"){
                firstWord = words[cursor];
            }
            /*if(words[cursor].value != "]"){
                cursor++;
            }*/

            Expression.back().Location.source = transSource(firstWord.value);

            if(firstWord.type == 'b'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 'b', scriptName, lineNumber, allAvailableEventIDs,
                    NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                )){ continue; };
            }
            else if(firstWord.type == 'i'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber, allAvailableEventIDs,
                    NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                )){ continue; };
            }
            else if(firstWord.type == 'd'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 'd', scriptName, lineNumber, allAvailableEventIDs,
                    NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                )){ continue; };
            }
            else if(firstWord.type == 's'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber, allAvailableEventIDs,
                    NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                )){ continue; };
            }
            else if(firstWord.type == 'c'){
                if(words[cursor].value != "]"){
                    cursor++;
                }
                if(isStringInGroup(firstWord.value, 8, "key_pressed", "key_pressing",
                    "key_released", "mouse_pressed", "mouse_pressing", "mouse_released", "screen_w", "screen_h")
                ){
                    if(words.size() < cursor + 1){
                        continue;
                    }
                    if(words[cursor].value == "]"){
                        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                            << errorSpacing() << "In " << __FUNCTION__ << ": Source '" << firstWord.value
                            << "' requires one integer literal or a variable.\n";
                        return false;
                    }
                    if(words[cursor].type == 'c'){
                        if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber, allAvailableEventIDs,
                            NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                        )){ continue; };
                    }
                    else if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber, allAvailableEventIDs,
                        NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                    )){ continue; };
                }
                else if(firstWord.value == "exists" || firstWord.value == "is_directory"){
                    if(words[cursor].value == "]"){
                        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                            << errorSpacing() << "In " << __FUNCTION__ << ": Source '" << firstWord.value
                            << "' requires one string literal.\n";
                        return false;
                    }
                    if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber, allAvailableEventIDs,
                        NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                    )){ continue; };
                }
                else if(isStringInGroup(firstWord.value, 18, "booting", "second_passed", "fps", "any_key_pressed",
                    "any_key_pressing", "any_key_released", "mouse_x", "mouse_y", "mouse_moved",
                    "display_w", "display_h", "fullscreen", "display_resized", "used_os",
                    "number_of_processes", "number_of_cameras", "number_of_layers", "number_of_objects")
                ){
                    continue;
                }
                else if(firstWord.value == "vector"){
                    if(nextCond(words, cursor, Expression.back().Location.moduleID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    switch(Expression.back().Location.attribute){
                        case index_a:
                            if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber, allAvailableEventIDs,
                                NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                            )){ continue; };
                            Expression.back().Location.attribute = value;
                            break;
                        case context_a:
                            if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber, allAvailableEventIDs,
                                NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                            )){ continue; };
                            Expression.back().Location.attribute = value;
                            break;
                        case size_a:
                            Expression.back().Location.attribute = size_a;
                            break;
                        case back_a:
                            Expression.back().Location.attribute = back_a;
                            break;
                        default:
                            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                                << errorSpacing() << "In " << __FUNCTION__ << ": Invalid attribute '"
                                << attributeToStr(Expression.back().Location.attribute) << "'.\n";
                            return false;
                    }
                }
                else if(firstWord.value == "camera"){
                    if(nextCond(words, cursor, Expression.back().Location.cameraID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                }
                else if(firstWord.value == "layer"){
                    if(nextCond(words, cursor, Expression.back().Location.layerID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    if(Expression.back().Location.attribute == in_group){
                        if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber, allAvailableEventIDs,
                            NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                        )){ continue; };
                    }
                }
                else if(firstWord.value == "object"){
                    if(nextCond(words, cursor, Expression.back().Location.layerID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.objectID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.moduleType, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.moduleID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    if(Expression.back().Location.moduleType == ancestor
                        || Expression.back().Location.moduleType == text
                        || Expression.back().Location.moduleType == editable_text
                    ){
                        if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber, allAvailableEventIDs,
                            NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                        )){ continue; };
                    }
                    else if(Expression.back().Location.moduleType == mouse){
                        if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber, allAvailableEventIDs,
                            NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                        )){ continue; };
                    }
                    else if(Expression.back().Location.moduleType == collision){
                        if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber, allAvailableEventIDs,
                            NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                        )){ continue; };
                        if(nextCond(words, cursor, Expression.back().Location.spareID, scriptName, lineNumber)){ continue; };
                    }
                }
                else if(firstWord.value == "variable"){
                    if(nextCond(words, cursor, Expression.back().Location.moduleID, scriptName, lineNumber)){ continue; };
                }
                else{
                    cursor--;
                    Expression.back().Location.source = ValueSource::context;
                    if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber, allAvailableEventIDs,
                        NewVariablesForLookupTable, PassedVariables, canCreateNewsVariable, inAfterSection
                    )){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.spareID, scriptName, lineNumber)){ continue; };
                }
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Word of the '" << firstWord.type << "' type is not valid.\n";
                return false;
            }
        }
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    resultStack.reserve(Expression.size());
    for(unsigned i = 0; i < Expression.size(); ++i){
        resultStack.push_back(VariableModule());
    }
    return true;
}
bool createEvent(const string & scriptName, const unsigned & lineNumber, const string & layerID,
    const string & objectID, vector <EventModule> & EventContainer, vector <string> & EventContainerIDs,
    EventModule & NewEvent, const vector<WordStruct> & words,
    vector<StartingVariableStruct> & NewVariablesForLookupTable
){
    string eventType = "";
    string eventID = "";
    
    if(words[0].value == "override"){
        if(words.size() < 3){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 2 parameters.\n";
            return true;
        }
        if(words[1].type != 'c' && words[1].type != 'e'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": In the '" << words[0].value
                << "' instruction: The second parameter (event_type) is not a context.\n";
            return true;
        }
        if(words[2].type != 'c' && words[2].type != 'e'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": In the '" << words[0].value
                << "' instruction: The third parameter (id) is not a context.\n";
            return true;
        }
        eventType = words[1].value;
        eventID = words[2].value;
    }
    else{
        if(words.size() < 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 1 parameter.\n";
            return true;
        }
        if(words[1].type != 'c' && words[1].type != 'e'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": In the '" << words[0].value
                << "' instruction: The second parameter (id) is not a context.\n";
            return true;
        }
        eventType = words[0].value;
        eventID = words[1].value;
    }
    
    if(eventID[0] != '_' && isStringInVector(EventContainerIDs, eventID)){
        if(words[0].value == "override"){
            removeFromStringVector(EventContainerIDs, eventID);
            removeModuleInstanceByID(EventContainer, eventID);
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Event with id \'" << eventID << "\' already exists.\n";
            return true;
        }
    }
    NewEvent = EventModule(eventID, &EventContainerIDs, layerID, objectID);
    if(eventType == "loop"){
        NewEvent.loop = true;
    }

    unsigned cursor = 2;
    if(NewEvent.getPassedVariables(words, cursor, lineNumber, scriptName, NewVariablesForLookupTable)){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return true;
    }
    return false;
}
bool findCallingEvent(const string & scriptName, const unsigned & lineNumber,
    vector <EventModule> & EventContainer, EventModule & NewEvent,
    vector<string> & allAvailableEventIDs, int eventIdx
){
    short parentOccurrences = 0;
    EventModule * PossibleEventSource = nullptr;
    for(int possibleParentIdx = eventIdx; possibleParentIdx >= 0; --possibleParentIdx){
        for(const ChildStruct & Child : EventContainer[possibleParentIdx].Children){
            if(Child.ID == allAvailableEventIDs.back()){
                PossibleEventSource = &EventContainer[possibleParentIdx];
                ++parentOccurrences;
                if(parentOccurrences > 1){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << errorSpacing() << "In " << __FUNCTION__
                        << ": Inline function '" << allAvailableEventIDs.back() << "' called in '"
                        << EventContainer[possibleParentIdx].getID()
                        << "' event. Inline functions can only be called from one event\n";
                }
            }
        }
    }
    if(parentOccurrences > 1){
        return true;
    }
    if(parentOccurrences == 1){
        allAvailableEventIDs.push_back(PossibleEventSource->getID());
        NewEvent.PassedVariables = PossibleEventSource->PassedVariables;
    }
    return false;
}
bool createCallingStackOfInlineEvents(const string & scriptName, const unsigned & lineNumber,
    vector <EventModule> & EventContainer, EventModule & NewEvent, vector<string> & allAvailableEventIDs
){
    allAvailableEventIDs.clear();
    allAvailableEventIDs.push_back(NewEvent.getID());
    for(int eventIdx = EventContainer.size(); eventIdx >= 0; --eventIdx){
        short parentOccurrences = 0;
        EventModule * PossibleEventSource = nullptr;
        for(int possibleParentIdx = eventIdx - 1; possibleParentIdx >= 0; --possibleParentIdx){
            for(const ChildStruct & Child : EventContainer[possibleParentIdx].Children){
                if(Child.ID == allAvailableEventIDs.back()){
                    PossibleEventSource = &EventContainer[possibleParentIdx];
                    ++parentOccurrences;
                    if(parentOccurrences > 1){
                        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                            << errorSpacing() << "In " << __FUNCTION__
                            << ": Inline function '" << allAvailableEventIDs.back() << "' called in '"
                            << EventContainer[possibleParentIdx].getID()
                            << "' event. Inline functions can only be called from one event\n";
                    }
                }
            }
        }
        if(parentOccurrences > 1){
            return true;
        }
        if(parentOccurrences == 1){
            allAvailableEventIDs.push_back(PossibleEventSource->getID());
            NewEvent.PassedVariables = PossibleEventSource->PassedVariables;
            if(!PossibleEventSource->isInline){
                break;
            }
        }
    }
    return false;
}
bool createInlineEvent(const string & scriptName, const unsigned & lineNumber, const string & layerID,
    const string & objectID, vector <EventModule> & EventContainer, vector <string> & EventContainerIDs,
    EventModule & NewEvent, const vector<WordStruct> & words, vector<string> & allAvailableEventIDs
){
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 1 parameter.\n";
        return true;
    }
    if(words[1].type != 'c' && words[1].type != 'e'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": In the '" << words[0].value
            << "' instruction: The second parameter (id) is not a context.\n";
        return true;
    }
    string eventType = words[0].value;
    string eventID = words[1].value;
    
    if(isStringInVector(EventContainerIDs, eventID)){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__ << ": Event with id \'" << eventID << "\' already exists.\n";
        return true;
    }

    NewEvent = EventModule(eventID, &EventContainerIDs, layerID, objectID);
    if(eventType == "inline_loop"){
        NewEvent.loop = true;
    }

    NewEvent.isInline = true;

    // if(NewEvent.getID() == "checkItems"){
    //     raise(SIGINT);
    // }

    if(createCallingStackOfInlineEvents(scriptName, lineNumber, EventContainer, NewEvent, allAvailableEventIDs)){
        return true;
    }

    return false;
}
inline DataType attributeToInstDataType(const InstrDescription & CurrentInstr, const AttributeType &attribute){
    switch(attribute){
        case camera_a:
            return camera_inst;
        case layer_a:
            return layer_inst;
        case object_a:
            return object_inst;
        case text_a:
            return text_mod;
        case editable_text_a:
            return editable_text_mod;
        case super_text_a:
            return super_text_mod;
        case super_editable_text_a:
            return super_editable_text_mod;
        case image_a:
            return image_mod;
        case movement_a:
            return movement_mod;
        case collision_a:
            return collision_mod;
        case particles_a:
            return particles_mod;
        case event_a:
            return event_mod;
        case variable_a:
            return variable_mod;
        case scrollbar_a:
            return scrollbar_mod;
        case primitives_a:
            return primitives_mod;
        case vector_a:
            return vector_mod;
        default:
            return null_dt;
    }
}
bool setupFirstLastAllRandomInstr(const vector<WordStruct> & words, EventModule & NewEvent, OperationClass * Operation,
    bool inAfterSection, unsigned lineNumber, string scriptName, string & error, unsigned & cursor,
    vector <string> & allAvailableEventIDs, vector<StartingVariableStruct> & NewVariablesForLookupTable
){
    if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
        return true;
    }
    if(words[1].type != 'c'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return true;
    }

    if(words[1].value == "Layers"){
        Operation->Location.source = ValueSource::layer;
    }
    else if(words[1].value == "Cameras"){
        Operation->Location.source = ValueSource::camera;
    }
    else if(words[1].value != ""){
        Operation->Location.source = ValueSource::context;
        if(Operation->addParameter(scriptName, lineNumber, error,
            words, cursor, 'c', "context", false, allAvailableEventIDs,
            NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
        )){ return true; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return true;
    }

    DataType outputType = null_dt;
    
    cursor = 2;
    if(Operation->Location.source == ValueSource::camera){
        if(optional(words, cursor, Operation->Location.cameraID)){ return false; }
        if(optional(words, cursor, Operation->Location.attribute)){ return false; }
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, allAvailableEventIDs,
            NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return true;
        }
        outputType = camera_inst;
        if(Operation->Location.attribute != null_a && Operation->Location.attribute != camera_a){
            outputType = pointer_inst;
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
            NewEvent.PassedVariables, allAvailableEventIDs, outputType,
            Operation->outputVariableID, Operation->isOutputReference, false, true, inAfterSection
        )){
            if(error.size() == 0){ return false; }
            return true;
        }
    }
    else if(Operation->Location.source == ValueSource::layer){
        if(optional(words, cursor, Operation->Location.layerID)){ return false; }
        if(optional(words, cursor, Operation->Location.objectID)){ return false; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return false; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return false; }
        if(optional(words, cursor, Operation->Location.attribute)){ return false; }
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, allAvailableEventIDs,
            NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return true;
        }
        outputType = layer_inst;
        if(Operation->Location.attribute != object_a && Operation->Location.objectID == ""
            && Operation->Location.moduleType == null_s && Operation->Location.moduleID == ""
        ){
            outputType = pointer_inst;
        }
        else{
            if(Operation->Location.moduleType != null_s){
                switch(Operation->Location.moduleType){
                    case text:
                        outputType = text_mod;
                        break;
                    case editable_text:
                        outputType = editable_text_mod;
                        break;
                    case super_text:
                        outputType = super_text_mod;
                        break;
                    case super_editable_text:
                        outputType = super_editable_text_mod;
                        break;
                    case image:
                        outputType = image_mod;
                        break;
                    case movement:
                        outputType = image_mod;
                        break;
                    case collision:
                        outputType = collision_mod;
                        break;
                    case particles:
                        outputType = particles_mod;
                        break;
                    case event:
                        outputType = event_mod;
                        break;
                    case variable:
                        outputType = variable_mod;
                        break;
                    case scrollbar:
                        outputType = scrollbar_mod;
                        break;
                    case primitives:
                        outputType = primitives_mod;
                        break;
                    case vector_s:
                        outputType = vector_mod;
                        break;
                    default:
                        break;
                }
            }
            else if(Operation->Location.attribute == object_a || Operation->Location.attribute == null_a){
                outputType = object_inst;
            }
            else{
                outputType = pointer_inst;
            }
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
            NewEvent.PassedVariables, allAvailableEventIDs, outputType, Operation->outputVariableID,
            Operation->isOutputReference, false, true, inAfterSection
        )){
            if(error.size() == 0){ return false; }
            return true;
        }
    }
    else if(Operation->Location.source == ValueSource::context){
        if(optional(words, cursor, Operation->Location.layerID)){ return false; }
        if(optional(words, cursor, Operation->Location.objectID)){ return false; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return false; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return false; }
        if(optional(words, cursor, Operation->Location.attribute)){ return false; }
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, allAvailableEventIDs,
            NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return true;
        }
        outputType = attributeToInstDataType(InstrDescription(), Operation->Location.attribute);
        if(outputType == null_dt){
            outputType = any_dt;
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
            NewEvent.PassedVariables, allAvailableEventIDs, outputType, Operation->outputVariableID,
            Operation->isOutputReference, false, true, inAfterSection
        )){
            if(error.size() == 0){ return false; }
            return true;
        }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": Source '" << words[1].value << "' does not exist.\n";
        return true;
    }
    return false;
}
bool setupIndexInstr(const vector<WordStruct> & words, EventModule & NewEvent, OperationClass * Operation,
    bool inAfterSection, unsigned lineNumber, string scriptName, string & error, unsigned & cursor,
    vector <string> & allAvailableEventIDs, vector<StartingVariableStruct> & NewVariablesForLookupTable
){
    if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
        return true;
    }
    if(words[1].type != 'c'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
        return true;
    }

    if(words[1].value == "Processes"){
        Operation->Location.source = ValueSource::process;
    }
    else if(words[1].value == "Layers"){
        Operation->Location.source = ValueSource::layer;
    }
    else if(words[1].value == "Cameras"){
        Operation->Location.source = ValueSource::camera;
    }
    else if(words[1].value != ""){
        Operation->Location.source = ValueSource::context;
        if(Operation->addParameter(scriptName, lineNumber, error,
            words, 1, 'c', "source", false, allAvailableEventIDs,
            NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
        )){ return true; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << errorSpacing() << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return true;
    }
    
    cursor = 2;
    if(Operation->addVectorOrVariableToParameters(scriptName,
        lineNumber, error, words, cursor, 'i', "indexes", false, allAvailableEventIDs,
        NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
    )){ return true; }
    if(optional(words, cursor, Operation->Location.attribute)){ return false; }
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
        NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
        Operation->isOutputReference, false, true, inAfterSection
    )){
        if(error.size() == 0){ return false; }
        return true;
    }
    return false;
}
void AncestorObject::assembleEvents(vector<string> code, string scriptName, vector<StartingVariableStruct> & NewVariablesForLookupTable){
    vector<WordStruct> words;
    EventModule NewEvent = EventModule();
    unsigned cursor = 0, lineNumber = 0;
    OperationClass * Operation;
    bool inAfterSection = false;

    //merge string sections
    vector<string> code2 = {""};
    bool stringSection = false;
    unsigned emptyLinesCount = 0;
    for(unsigned line = 0; line < code.size(); line++){
        for(unsigned ch = 0; ch < code[line].size(); ch++){
            if(code[line][ch] == '\"'){
                if(ch != 0 && code[line][ch-1] == '\\'){
                    continue;
                }
                stringSection = !stringSection;
            }
        }
        code2.back() += code[line];
        if(!stringSection){
            code2.emplace_back("");
            for(; emptyLinesCount > 0; emptyLinesCount--){
                code2.emplace_back("");
            }
        }
        else{
            code2.back() += '\n';
            emptyLinesCount++;
        }
    }
    code = code2;

    string error;
    bool triggerBreakpoint = false;

    vector <string> allAvailableEventIDs;
    
    for(string line : code){
        lineNumber++;
        words.clear();
        line = findAndUseSpecialCharacters(line);
        words = tokenizeCode(line);
        //words = mergeStrings(words);
        if(words.size() == 0){
            continue;
        }
        cursor = 1;

        if(triggerBreakpoint){
            raise(SIGINT);
            triggerBreakpoint = false;
        }
        if(words[0].value == "import"){
            //Ignore
        }
        else if(words[0].value == "compiler_breakpoint"){
            triggerBreakpoint = true;
            cerr << "Warning: The 'compiler_breakpoint' instruction can be used only in the debugger.\n";
        }
        else if(words[0].value == "start" || words[0].value == "loop" ||  words[0].value == "override"){
            if(createEvent(scriptName, lineNumber, layerID, ID, EventContainer, EventContainerIDs, NewEvent, words, NewVariablesForLookupTable)){
                return;
            }
            allAvailableEventIDs.clear();
            allAvailableEventIDs.push_back(NewEvent.getID());
        }
        else if(words[0].value == "inline" || words[0].value == "inline_loop"){
            if(createInlineEvent(scriptName, lineNumber, layerID, ID, EventContainer, EventContainerIDs, NewEvent, words, allAvailableEventIDs)){
                return;
            }
        }
        else if(words[0].value == "end"){
            if(NewEvent.isInline){
                NewEvent.PassedVariables.clear();
            }
            EventContainer.push_back(NewEvent);
            NewEvent = EventModule();
            inAfterSection = false;
        }
        else if(words[0].value == "after"){
            inAfterSection = true;
        }
        else if(words[0].value == "triggers"){
            if(words.size() < 2){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires at least 2 parameters.\n";
                return;
            }
            while(cursor < words.size()){
                if(words[cursor].type != 'c'){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << errorSpacing() << "In " << __FUNCTION__
                        << ": In the '" << words[0].value << "' instruction: Parameter " << cursor << " is not a context.\n";
                    return;
                }
                if(words[cursor].type != 'e'){
                    TriggerType NewTrigger = transStringToTrigger(words[cursor].value);
                    if(NewTrigger == null_t){
                        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                            << errorSpacing() << "In " << __FUNCTION__
                            << ": In the '" << words[0].value << "' instruction: Parameter '"
                            << words[cursor].value << "' is not a valid trigger.\n";
                        return;
                    }
                    NewEvent.primaryTriggerTypes.push_back(NewTrigger);
                    NewEvent.isFunction = false;
                }
                cursor++;
            }
            /*if(!gatherStringVector(words, cursor, NewEvent.primaryTriggerTypes, lineNumber, scriptName)){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Context gather failed.\n";
                return;
            }*/
        }
        else if(words[0].value == "run"){
            if(inAfterSection){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Cannot run another events in the after section.\n";
                return;
            }
            if(words.size() < 2){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires 2 parameters.\n";
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
                return;
            }
            NewEvent.Children.emplace_back(ChildStruct(words[1].value, false, vector<string>(), 0, scriptName, lineNumber));
            cursor = 2;
            if(NewEvent.getPassingVariables(NewEvent.Children.back().passingVariables,
                words, cursor, lineNumber, scriptName, NewVariablesForLookupTable,
                allAvailableEventIDs
            )){
                return;
            }
        }
        else if(words[0].value == "if"){
            if(inAfterSection){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Cannot create an if statement in the after section.\n";
                return;
            }
            if(!createExpression(words, cursor, NewEvent.ConditionalChain, NewEvent.resultStack, lineNumber, scriptName, true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Expression creation failed.\n";
                return;
            }
        }
        else if(words[0].value == "else"){
            if(inAfterSection){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Cannot run another event in the after section.\n";
                return;
            }
            if(words.size() < 2){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires one parameter of a string type.\n";
                return;
            }
            if(words[1].type == 'c'){
                NewEvent.elseChildID = words[1].value;
            }
            else if(words[1].type != 'e'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires one parameter of a string type.\n";
                return;
            }
            NewEvent.passingVariablesForElseEvent.clear();
            cursor = 2;
            if(NewEvent.getPassingVariables(NewEvent.passingVariablesForElseEvent,
                words, cursor, lineNumber, scriptName, NewVariablesForLookupTable,
                allAvailableEventIDs
            )){
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 10, "continue", "break", "return", "reboot", "exit",
            "delete_this_event", "reset_keyboard", "dump_context_stack", "restart_drag", "breakpoint")
        ){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 1, lineNumber, scriptName)){
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 4, "first", "last", "all", "random")){
            if(setupFirstLastAllRandomInstr(words, NewEvent, Operation, inAfterSection, lineNumber, scriptName, error,
                cursor, allAvailableEventIDs, NewVariablesForLookupTable
            )){
                return;
            }
        }
        else if(words[0].value == "index"){
            if(setupIndexInstr(words, NewEvent, Operation, inAfterSection, lineNumber, scriptName, error,
                cursor, allAvailableEventIDs, NewVariablesForLookupTable
            )){
                return;
            }
        }
        else if(words[0].value == "index_vec"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 4, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }

            if(words[1].value != ""){
                Operation->Location.source = ValueSource::context;
                if(Operation->addParameter(scriptName, lineNumber, error,
                    words, 1, 'c', "source", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": First parameter cannot be an empty context.\n";
                return;
            }
            
            if(Operation->addParameter(scriptName,
                lineNumber, error, words, 2, 'i', "index", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 3;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 7, "+", "-", "*", "/", "**", "random_int", "assert")){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(words[0].value == "+" || words[0].value == "assert"){
                if(Operation->addParameter(scriptName, lineNumber, error,
                    words, 1, 'a', "left", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection, words[0].value == "assert"
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'a', "right", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection, words[0].value == "assert"
                )){ return; }
            }
            else{
                if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'n', "left", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'n', "right", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            cursor = 3;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 5, "=", "+=", "-=", "*=", "/=")){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "left", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(words[0].value == "=" || words[0].value == "+="){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'a', "right", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else{
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'n', "right", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            cursor = 3;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "find_by_id_2"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "source", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "id", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 3;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 4, "sum", "intersection", "difference", "in")){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "left", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'c', "right", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 3;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "++" || words[0].value == "--" || words[0].value == "delete"
            || words[0].value == "demolish" || words[0].value == "rbind" || words[0].value == "type"
        ){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            string parameterName = "context";
            if(words[0].value == "demolish" || words[0].value == "rbind"){
                parameterName = "objects";
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', parameterName, false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "next"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'i', "number", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "access"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() > 0){ return; }
            }
            if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
                lineNumber, scriptName, false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Expression creation failed.\n";
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 4, "bool", "int", "double", "string")){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }

            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() > 0){ return; }
            }
            
            if(words[0].value == "bool"){
                if(Operation->addLiteralOrVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'b', "values", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[0].value == "int"){
                if(Operation->addLiteralOrVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'i', "values", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[0].value == "double"){
                if(Operation->addLiteralOrVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'd', "values", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[0].value == "string"){
                if(Operation->addLiteralOrVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 's', "values", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": Literal type is required.\n";
                return;
            }
        }
        else if(words[0].value == "find_by_id"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Location.source = transSource(words[1].value);
            cursor = 2;
            if(words[1].value == "camera"){
                if(words.size() < 4){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << errorSpacing() << "In " << __FUNCTION__
                        << ": \'find_by_id " << words[1].value << "\' requires at least 2 additional parameters.\n";
                    continue;
                }
                if(optional(words, cursor, Operation->Location.cameraID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                    NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                    Operation->isOutputReference, false, true, inAfterSection
                )){
                    if(error.size() == 0){ continue; }
                    return;
                }
            }
            else if(words[1].value == "layer"){
                if(words.size() < 7){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << errorSpacing() << "In " << __FUNCTION__
                        << ": \'find_by_id " << words[1].value << "\' requires at least 5 additional parameters.\n";
                    continue;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                    NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                    Operation->isOutputReference, false, true, inAfterSection
                )){
                    if(error.size() == 0){ continue; }
                    return;
                }
            }
            else if(words[1].value == "context" || words[1].value == "c" || words[1].type == 'e'){
                if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'c', "scripts", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(words.size() < cursor + 5){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << errorSpacing() << "In " << __FUNCTION__
                        << ": \'find_by_id " << words[1].value << " [context_list]\' requires at least 5 additional parameters.\n";
                    continue;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                    NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                    Operation->isOutputReference, false, true, inAfterSection
                )){
                    if(error.size() == 0){ continue; }
                    return;
                }
            }
        }
        else if(words[0].value == "let"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->outputVariableID = words[1].value;
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'c', "old_variable", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "clone"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 4, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "left", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'c', "right", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'b', "changeOldID", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "new"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Location.source = transSource(words[1].value);
            
            //If the destination is provided as a variable, skip one parameter in the instruction.
            if(words[2].type == 'c'){
                Operation->addLiteralParameter(VariableModule::newString("variable"));
            }
            else{
                Operation->addLiteralParameter(VariableModule::newString("location"));
            }
            
            cursor = 2;
            if(words[cursor].type == 'c'){
                if(Operation->addParameter(scriptName, lineNumber, error, words, cursor, 'c', "destination", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                cursor++;
            }
            else{
                if(Operation->Location.source == ValueSource::object){
                    if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                }
                else if(Operation->Location.source != ValueSource::camera && Operation->Location.source != ValueSource::layer){
                    if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                    if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                }
            }
            
            if(Operation->addParameter(scriptName, lineNumber, error, words, cursor, 'i', "quantity", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            cursor++;
            if(Operation->addLiteralOrVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 's', "new_ids", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, any_dt, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "bind"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "objects", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 's', "scripts", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "build"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "objects", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'b', "reset", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'b', "do_not_preserve", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "load_build" || words[0].value == "build_subset"
            || words[0].value == "inject_code" || words[0].value == "inject_instr"
        ){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'c', "objects", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            string parameterName = "paths";
            if(words[0].value == "inject_code"){
                parameterName = "code";
            }
            else if(words[0].value == "inject_instr"){
                parameterName = "instructions";
            }
            if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 's', parameterName, false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, cursor, 'b', "reset", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, cursor + 1, 'b', "do_not_preserve", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "fun"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "objects", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            Operation->Location.attribute = strToAttribute(words[2].value);
            cursor = 3;
            while(words.size() > cursor){
                if(Operation->addParameter(scriptName, lineNumber, error, words, cursor, 'a', "value", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                cursor++;
            }
        }
        else if(words[0].value == "env"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
                return;
            }
            Operation->addLiteralParameter(VariableModule::newString(words[1].value));
            if(words[1].value == "window_title"){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "title", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "display_size"){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "width", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'i', "height", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else{
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "value", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
        }
        else if(words[0].value == "edit_proc"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
                return;
            }
            for(cursor = 2; cursor < words.size(); cursor++){
                if(words[cursor].type != 'c'){
                    continue;
                }
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: Parameter " << cursor << " must be a literal.\n";
                return;
            }
            Operation->addLiteralParameter(VariableModule::newString(words[1].value));
            if(isStringInGroup(words[1].value, 2, "clear_layers", "clear_cameras")){
                continue;
            }
            
            if(words[1].value == "id"){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "new_id", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "reservation_multiplier"){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'd', "multiplier", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "window_pos" || words[1].value == "window_size" || words[1].value == "min_window_size"){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "x", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'i', "y", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "window_tint"){
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'd', "red", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'd', "green", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 4, 'd', "blue", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                if(Operation->addParameter(scriptName, lineNumber, error, words, 5, 'd', "alpha", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else{
                if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "value", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
        }
        else if(words[0].value == "load_bitmap"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "name", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'b', "light", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 4, 'b', "ignore_warnings", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "mkdir" || words[0].value == "rm" || words[0].value == "rmll"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "mv"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "new_path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "print"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "delimeter", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            while(cursor < words.size()){
                if(Operation->addParameter(scriptName, lineNumber, error, words, cursor, 'a', "value", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
                cursor++;
            }
        }
        else if(words[0].value == "load_text"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "save_text"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "text", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 's', "delimeter", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "ls"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 1, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'b', "recursive", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 4, 'i', "max_depth", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "lse"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
                return;
            }
            Operation->addLiteralParameter(VariableModule::newString(words[1].value));
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'b', "detail", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "new_proc"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "name", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "layer", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 's', "object", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 4, 's', "script", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "var"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, variable_mod, Operation->outputVariableID,
                Operation->isOutputReference, true, true, inAfterSection
            )){
                if(error.size() > 0){ return; }
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'a', "value", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "vec"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
                return;
            }
            Operation->addLiteralParameter(VariableModule::newString(words[1].value));
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, vector_mod, Operation->outputVariableID,
                Operation->isOutputReference, true, true, inAfterSection
            )){
                if(error.size() > 0){ return; }
            }
            if(words.size() < 4){
                continue;
            }
            if(words[1].value == "bool"){
                if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'b', "scripts", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "int"){
                if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'i', "scripts", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "double"){
                if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 'd', "scripts", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else if(words[1].value == "string"){
                if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 's', "scripts", false, allAvailableEventIDs,
                    NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
                )){ return; }
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << errorSpacing() << "In " << __FUNCTION__ << ": In the instruction '" << words[0].value
                    << "': The type \'" << words[1].value << "\' does not exist.\n";
                continue;
            }
        }
        else if(words[0].value == "tokenize"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "delimeter", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "text", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 3;
            while(cursor < words.size()){
                if(words[cursor].type != 'c'){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << errorSpacing() << "In " << __FUNCTION__
                        << ": In the '" << words[0].value << "' instruction: Parameter '" << words[cursor].value
                        << "' (" << cursor << ") must be a variable.\n";
                    return;
                }
                Operation->addLiteralParameter(VariableModule::newString(words[cursor].value));
                cursor++;
            }
        }
        else if(words[0].value == "tree" || words[0].value == "pwd" || words[0].value == "console_input"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 1, lineNumber, scriptName)){
                return;
            }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "len"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "text", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "size"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'c', "text", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "substr"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 4, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "text", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "begin", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'i', "length", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 4;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "load_font"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 4, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "size", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 's', "name", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 4, 'b', "ignore_warnings", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "cd"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 1, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "path", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "similar"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "pattern", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, cursor, 's', "vector", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, cursor, 'b', "longest_common_part", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
            cursor++;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "count"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 3, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "pattern", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 's', "text", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 3;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "create_display"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 5, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 'i', "display_width", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 2, 'i', "display_height", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 3, 'i', "backbuffer_width", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 4, 'i', "backbuffer_height", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 5, 'b', "auto_scale_backbuffer", true, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else if(words[0].value == "start_timer"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 2, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "name", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
        }
        else if(words[0].value == "stop_timer"){
            if(!prepareNewInstruction(words, NewEvent, Operation, inAfterSection, 1, lineNumber, scriptName)){
                return;
            }
            if(Operation->addParameter(scriptName, lineNumber, error, words, 1, 's', "name", false, allAvailableEventIDs,
                NewVariablesForLookupTable, NewEvent.PassedVariables, false, inAfterSection
            )){ return; }
            cursor = 2;
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, NewVariablesForLookupTable,
                NewEvent.PassedVariables, allAvailableEventIDs, value_inst, Operation->outputVariableID,
                Operation->isOutputReference, false, true, inAfterSection
            )){
                if(error.size() == 0){ continue; }
                return;
            }
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' does not exist.\n";
        }
    }
    if(words.size() > 0){
        if(words[0].value != "end"){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << errorSpacing() << "In " << __FUNCTION__ << ": Every event must end with \"end\" instruction.\n";
        }
    }

    // for(EventModule & InlineEvent : EventContainer){
    //     if(!InlineEvent.isInline){
    //         continue;
    //     }
    //     InlineEvent.PassedVariables.clear();
    // }
}
void AncestorObject::clearAllEvents(){
    for(auto & Event : EventContainer){
        Event.clear();
    }
    EventContainer.clear();
    EventContainerIDs.clear();
}
inline void printEmptyFileWarning(string scriptName, string functionName){
    cout << "Warning: In " << functionName
        << ": Script '" << scriptName << "' is empty or cannot be opened.\n";
}
inline void printBasicFile(){
    cout << "Basic \"Hello World\" program:\n\n"
        << "start helloWorld\n"
        << "\ttriggers on_init\n"
        << "\tprint \"Hello, World!\\n\"\n"
        << "\texit\n"
        << "end\n\n";
}
//Return true if a script is trying to import itself.
ReturnType gatherImportsFromScript(const string & scriptName, vector<string> & allImports){
    allImports.clear();
    vector<string> scriptLines = readLines(scriptName, false);
    if(scriptLines.size() == 0){
        return ReturnType::EMPTY;
    }
    vector<WordStruct> words;
    for(const string & line : scriptLines){
        words = tokenizeCode(line); //TODO: Use less expensive way to extract imports from lines
        if(words.size() == 0){
            continue;
        }
        if(words[0].value == "import"){
            if(words.size() == 1){
                cerr << "Error: Import requires at least one parameter.\n";
                continue;
            }
            if(words[1].value.size() == 0){
                cerr << "Error: First parameter of the import statement cannot be empty.\n";
                continue;
            }
            if(words[1].value == scriptName){
                allImports.clear();
                return ReturnType::ERROR_INF;
            }
            allImports.push_back(words[1].value);
            continue;
        }
    }
    return ReturnType::OK;
}
void removeStringDuplicatesFromVector(vector<string> & stringVec){
    std::unordered_set<string> encounteredStrings;
    for(auto iter = stringVec.begin(); iter != stringVec.end();){
        if(encounteredStrings.find(*iter) != encounteredStrings.end()){
            iter = stringVec.erase(iter);
        }
        else{
            encounteredStrings.insert(*iter);
            ++iter;
        }
    }
}
//Return a vector of scripts and their imports in left-to-right, depth-first order.
ReturnType addImportsToBindedScripts(const vector<string> & bindedScripts, vector<string> & allScriptsToAssemble){
    if(bindedScripts.size() == 0){
        return OK;
    }
    
    vector<string> parentScripts;
    vector<std::queue<string>> scriptsToAnalyze;
    vector<string> subsequentImports;
    string currentScript;
    ReturnType importResult;

    scriptsToAnalyze.emplace_back(std::queue<string>());
    for(const string & script : bindedScripts){
        scriptsToAnalyze.back().push(script);
    }

    while(!scriptsToAnalyze.empty()){
        currentScript = scriptsToAnalyze.back().front();
        scriptsToAnalyze.back().pop();
        importResult = gatherImportsFromScript(currentScript, subsequentImports);
        if(importResult == ERROR_INF){
            return ReturnType::ERROR_INF;
        }
        if(subsequentImports.empty()){
            if(importResult != EMPTY){
                allScriptsToAssemble.push_back(currentScript);
            }
            if(scriptsToAnalyze.back().empty()){ //All children have been added to the final vector.
                scriptsToAnalyze.pop_back();
                if(!parentScripts.empty()){
                    allScriptsToAssemble.push_back(parentScripts.back());
                    parentScripts.pop_back();
                }
            }
        }
        else{
            if(scriptsToAnalyze.back().empty()){
                scriptsToAnalyze.pop_back();
            }
            //Check if imported scripts are not importing any parent scripts - if they are, return error to avoid a cycle.
            for(const string & parent : parentScripts){
                for(const string & importedScript : subsequentImports){
                    if(parent == importedScript){
                        return ReturnType::ERROR_INF;
                    }
                }
            }
            scriptsToAnalyze.emplace_back(std::queue<string>());
            for(const string & script : subsequentImports){
                scriptsToAnalyze.back().push(script);
            }
            parentScripts.push_back(currentScript);
        }
    }

    removeStringDuplicatesFromVector(allScriptsToAssemble);

    return ReturnType::OK;
}
void AncestorObject::translateAllScripts(bool clearEvents, bool allowNotAscii, vector<StartingVariableStruct> & NewVariablesForLookupTable){
    if(clearEvents){
        clearAllEvents();
    }

    if(bindedScripts.size() == 0){
        printBasicFile();
        return;
    }

    removeStringDuplicatesFromVector(bindedScripts);
    vector<string> allScriptsToAssemble;
    ReturnType status = addImportsToBindedScripts(bindedScripts, allScriptsToAssemble);
    if(status == ERROR_INF){
        cerr << "Error: Assembler preprocessor detected a cycle in the imports.\n";
        return;
    }
    
    vector<string> code;
    bool somethingWasAssembled = false;
    
    for(const string & scriptName : allScriptsToAssemble){
        code = readLines(scriptName, allowNotAscii);
        if(code.size() > 0){
            somethingWasAssembled = true;
            assembleEvents(code, scriptName, NewVariablesForLookupTable);
            code.clear();
        }
        else{
            printEmptyFileWarning(scriptName, __FUNCTION__);
        }
    }

    if(!somethingWasAssembled){
        printBasicFile();
    }
}
void AncestorObject::translateScriptsFromPaths(bool clearEvents, vector<string> scriptsPaths, bool allowNotAscii,
    vector<StartingVariableStruct> & NewVariablesForLookupTable
){
    if(clearEvents){
        clearAllEvents();
    }

    removeStringDuplicatesFromVector(scriptsPaths);
    vector<string> allScriptsToAssemble;
    ReturnType status = addImportsToBindedScripts(scriptsPaths, allScriptsToAssemble);
    if(status == ERROR_INF){
        cerr << "Error: Assembler preprocessor detected a cycle in the imports.\n";
        return;
    }
    vector<string> code;
    bool somethingWasAssembled = false;
    
    for(const string & scriptName : allScriptsToAssemble){
        code = readLines(scriptName, allowNotAscii);
        if(code.size() > 0){
            somethingWasAssembled = true;
            assembleEvents(code, scriptName, NewVariablesForLookupTable);
            code.clear();
        }
        else{
            printEmptyFileWarning(scriptName, __FUNCTION__);
        }
    }

    if(!somethingWasAssembled){
        printBasicFile();
    }
}
void AncestorObject::translateSubsetBindedScripts(bool clearEvents, vector<string> scripts, bool allowNotAscii,
    vector<StartingVariableStruct> & NewVariablesForLookupTable
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

    vector<string> allScriptsToAssemble;
    ReturnType status = addImportsToBindedScripts(selectedBindedScripts, allScriptsToAssemble);
    if(status == ERROR_INF){
        cerr << "Error: Assembler preprocessor detected a cycle in the imports.\n";
        return;
    }

    vector<string> code;
    bool somethingWasAssembled = false;
    
    for(const string & scriptName : allScriptsToAssemble){
        code = readLines(scriptName, allowNotAscii);
        if(code.size() > 0){
            somethingWasAssembled = true;
            assembleEvents(code, scriptName, NewVariablesForLookupTable);
            code.clear();
        }
        else{
            printEmptyFileWarning(scriptName, __FUNCTION__);
        }
    }

    if(!somethingWasAssembled){
        printBasicFile();
    }
}
void AncestorObject::injectCode(bool clearEvents, vector<string> code,
    vector<StartingVariableStruct> & NewVariablesForLookupTable
){
    if(clearEvents){
        clearAllEvents();
    }

    code = removeComments(code);
    
    if(code.size() > 0){
        assembleEvents(code, "<injection>", NewVariablesForLookupTable);
    }
}
void AncestorObject::injectInstructions(bool clearEvents, vector<string> instructions,
    vector<StartingVariableStruct> & NewVariablesForLookupTable){
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
    preprocessed.insert(preprocessed.begin(), "triggers each_iteration");
    preprocessed.insert(preprocessed.begin(), "start _ false");
    preprocessed.emplace_back("delete_this_event");
    preprocessed.emplace_back("end");
    assembleEvents(preprocessed, "<injection>", NewVariablesForLookupTable);
}

void AncestorObject::propagateLayerID(){
    for(TextModule & Text : TextContainer){
        Text.setLayerID(layerID);
    }
    for(EditableTextModule & EditableText : EditableTextContainer){
        EditableText.setLayerID(layerID);
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.setLayerID(layerID);
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.setLayerID(layerID);
    }
    for(ImageModule & Image : ImageContainer){
        Image.setLayerID(layerID);
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.setLayerID(layerID);
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.setLayerID(layerID);
    }
    for(ParticleEffectModule & Particles : ParticlesContainer){
        Particles.setLayerID(layerID);
    }
    for(EventModule & Event : EventContainer){
        Event.setLayerID(layerID);
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.setLayerID(layerID);
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.setLayerID(layerID);
    }
    for(PrimitivesModule & Primitives : PrimitivesContainer){
        Primitives.setLayerID(layerID);
    }
    for(VectorModule & Vector : VectorContainer){
        Vector.setLayerID(layerID);
    }
}
void AncestorObject::propagateObjectID(){
    for(TextModule & Text : TextContainer){
        Text.setObjectID(layerID);
    }
    for(EditableTextModule & EditableText : EditableTextContainer){
        EditableText.setObjectID(layerID);
    }
    for(SuperTextModule & SuperText : SuperTextContainer){
        SuperText.setObjectID(layerID);
    }
    for(SuperEditableTextModule & SuperEditableText : SuperEditableTextContainer){
        SuperEditableText.setObjectID(layerID);
    }
    for(ImageModule & Image : ImageContainer){
        Image.setObjectID(layerID);
    }
    for(MovementModule & Movement : MovementContainer){
        Movement.setObjectID(layerID);
    }
    for(CollisionModule & Collision : CollisionContainer){
        Collision.setObjectID(layerID);
    }
    for(ParticleEffectModule & Particles : ParticlesContainer){
        Particles.setObjectID(layerID);
    }
    for(EventModule & Event : EventContainer){
        Event.setObjectID(layerID);
    }
    for(VariableModule & Variable : VariablesContainer){
        Variable.setObjectID(layerID);
    }
    for(ScrollbarModule & Scrollbar : ScrollbarContainer){
        Scrollbar.setObjectID(layerID);
    }
    for(PrimitivesModule & Primitives : PrimitivesContainer){
        Primitives.setObjectID(layerID);
    }
    for(VectorModule & Vector : VectorContainer){
        Vector.setObjectID(layerID);
    }
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


DataType vectorizeEntityDataType(const InstrDescription & CurrentInstr, const DataType & oldType){
    switch(oldType){
        case camera_inst:
            return camera_vec;
        case layer_inst:
            return layer_vec;
        case object_inst:
            return object_vec;
        case text_mod:
            return text_mod_vec;
        case editable_text_mod:
            return editable_text_mod_vec;
        case super_text_mod:
            return super_text_mod_vec;
        case super_editable_text_mod:
            return super_editable_text_mod_vec;
        case image_mod:
            return image_mod_vec;
        case movement_mod:
            return movement_mod_vec;
        case collision_mod:
            return collision_mod_vec;
        case particles_mod:
            return particles_mod_vec;
        case event_mod:
            return event_mod_vec;
        case variable_mod:
            return variable_mod_vec;
        case scrollbar_mod:
            return scrollbar_mod_vec;
        case primitives_mod:
            return primitives_mod_vec;
        case vector_mod:
            return vector_mod_vec;
        case any_dt:
            return any_dt;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << dataTypeToStr(oldType) << "\' is not valid for this operation.\n";
            return null_dt;
    }
}