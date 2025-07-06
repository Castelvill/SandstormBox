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
    "print path \">> \"",
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
    objectLookupID = layerID + ID;
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
    objectLookupID = layerID + ID;
}

void AncestorObject::primaryConstructor(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    PrimaryModule::primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    objectLookupID = layerID + ID;
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
            cerr << "Error: In " << __PRETTY_FUNCTION__ <<
                ":\n" << NEW_LINE_PADDING << "Attribute '" << attributeToStr(attribute) << "' is not valid.\n";
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
    std::regex word_regex("([\\w+\\.*]*\\w+)|;|:|\\,|\\.|==|=|>=|<=|>|<|-=|\\+=|\\*=|/=|\\*\\*|\\+\\+|\\-\\-|\\+|-|\\*|/|%|\\[|\\]|\\(|\\\\\\\"|\\)|\"|!=|!|\\|\\||&&|\n|\t|&|@|#", std::regex_constants::icase);
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
            if(mergedOutput.size() > 0 && mergedOutput.back().type == 'c' && mergedOutput.back().value == "-"){
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
            if(mergedOutput.size() > 0 && mergedOutput.back().type == 'c' && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct('i', "-" + output[i], false)); //int
            }
            else{
                mergedOutput.emplace_back(WordStruct('i', output[i], false)); //int
            }
            
            continue;
        }
        if(mergedOutput.size() > 0 && mergedOutput.back().type == 'c' && mergedOutput.back().value == "-"){
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
bool prepareNewInstruction(const vector<WordStruct> &words, EventModule &NewEvent, OperationClass *&Operation,
    unsigned minLength, unsigned lineNumber, const string &scriptName
){
    if(words.size() < minLength){
        if(minLength == 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least 1 parameter.\n";
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least " << minLength-1 << " parameters.\n";
        }
        return false;
    }

    NewEvent.Operations.emplace_back(OperationClass());
    NewEvent.Operations.back().instruction = strToInstr(words[0].value);
    NewEvent.Operations.back().scriptName = scriptName;
    NewEvent.Operations.back().lineNumber = lineNumber;
    Operation = &NewEvent.Operations.back();
    
    return true;
}
bool prepareNewVariableDeclaration(const vector<WordStruct> & words, unsigned minLength, unsigned lineNumber, const string & scriptName){
    if(words.size() < minLength){
        if(minLength == 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least 1 parameter.\n";
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least " << minLength-1 << " parameters.\n";
        }
        return false;
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
        string error;
        variable = strToSource(words[cursor].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << __FUNCTION__ << ": " << error << "\n";
            return true;
        }
    }
    cursor++;
    return false;
}
bool optionalOutput(const string & scriptName, const unsigned & lineNumber, string & error, const vector<WordStruct> & words,
    unsigned & cursor, const DataType & outputType, vector<vector<VariableLocationStruct>> & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, OutputParameterStruct & OutputParameter,
    bool makeOutputGlobal = false, bool forceNewDeclaration = false
){
    error = "";
    if(cursor + 1 > words.size()){
        return true;
    }
    const WordStruct & instructionOutput = words[cursor];
    
    if(instructionOutput.type != 'e' && instructionOutput.type != 'c'){
        error = "Parameter 'output' (" + intToStr(cursor);
        error += ") must be a context.";
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return true;
    }

    if(instructionOutput.type == 'c'){
        OutputParameter.variableID = instructionOutput.value;
        OutputParameter.type = outputType;

        ReturnType e_Result = ReturnType::OK;
        std::tie(OutputParameter.localAddress, e_Result) = getLocalAddress(
            OutputParameter.variableID, outputType, Scopes, NewLocalVariables,
            topAddress, true, makeOutputGlobal, false, forceNewDeclaration
        );
        if(e_Result == ReturnType::OUT_OF_SCOPE){
            error = + "Index (" + std::to_string(OutputParameter.localAddress)
                + ") of the local variable is out of scope ("
                + std::to_string(NewLocalVariables.size()) + ").\n";
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            OutputParameter.localAddress = 0;
            return true;
        }
        else if(e_Result == ReturnType::UNDEFINED){
            error = + "Variable '" + OutputParameter.variableID + "' is undefined.\n";
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            return true;
        }
    }

    cursor++;
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, VariableModule & Variable,
    const char & type, string scriptName, unsigned lineNumber
){
    string error = "";
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        switch(type){   
            case 'b':
                Variable.setBool(cstoi(words[cursor].value, error));
                if(error.size() > 0){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
                }
                break;
            case 'i':
                Variable.setInt(cstoi(words[cursor].value, error));
                if(error.size() > 0){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
                }
                break;
            case 'd':
                Variable.setDouble(cstod(words[cursor].value, error));
                if(error.size() > 0){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
                }
                break;
            case 's':
                if(words[cursor].type == 's'){
                    Variable.setString(words[cursor].value);
                }
                else{
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter nr." << cursor << " is not a string.\n";
                }
                break;
            default:
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter nr."
                    << cursor << " has invalid type: '" << type << "'.\n";
                break;
        }
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
inline void setOptionalAttributeValueInCond(const size_t & index, const char & type,
    const vector<WordStruct> & words, VariableModule & Variable, const string & scriptName,
    const unsigned & lineNumber
){
    if(index >= words.size() || words[index].type == 'e'){
        return;
    }
    string error = "";
    switch(type){
        case 'b':
            Variable.setBool(cstoi(words[index].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
            }
            return;
        case 'i':
            Variable.setInt(cstoi(words[index].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
            }
            return;
        case 'd':
            Variable.setDouble(cstod(words[index].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
            }
            return;
        case 's':
            if(words[index].type == 's'){
                Variable.setString(words[index].value);
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter '" << words[index].value << "' is not a string.\n";
            }
            return;
        default:
            return;
    }
}
inline bool getLocalAddressWithError(unsigned & localAddress, const string & variableId,
    vector<vector<VariableLocationStruct>> &Scopes, vector<VariableInfo> &NewLocalVariables,
    unsigned &topAddress, const string &scriptName, const unsigned &lineNumber
){
    ReturnType e_Result = ReturnType::OK;
    std::tie(localAddress, e_Result) = getLocalAddress(
        variableId, any_dt, Scopes, NewLocalVariables, topAddress, false
    );
    if(e_Result == ReturnType::OUT_OF_SCOPE){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
             << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
             << "Index (" << localAddress
             << ") of a variable '" << variableId
             << "' is out of scope (" << NewLocalVariables.size() << ").\n";
        localAddress = 0;
        return true;
    }
    else if(e_Result == ReturnType::UNDEFINED){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
             << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
             << "Variable '" << variableId << "' is undefined.\n";
        return true;
    }
    return false;
}
inline void setOptionalAddressInCond(const size_t & index, const vector<WordStruct> & words, unsigned & localAddress,
    const string & scriptName, unsigned lineNumber, vector<vector<VariableLocationStruct>> & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable
){
    if(index >= words.size() || words[index].type == 'e'){
        return;
    }
    if(words[index].type == 'c'){
        getLocalAddressWithError(localAddress, words[index].value,
            Scopes, NewLocalVariables, topAddress, scriptName, lineNumber
        );
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter '" << words[index].value << "' is not a context.\n";
    }
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
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
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
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
        }
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
inline string getStringByIndex(const vector<string> & strVec, const size_t & index) noexcept {
    if(index >= strVec.size()){
        return "";
    }
    return strVec[index];
}
inline bool prepareVectorSource(const vector<string> & attributes, const vector<WordStruct> & attributeArgs,
    const unsigned & lineNumber, const string & scriptName, ConditionClass & Expression,
    vector<vector<VariableLocationStruct>> & Scopes, vector<VariableInfo> & NewLocalVariables, unsigned & topAddress,
    const bool & canCreateNewVariable
){
    Expression.Location.moduleID = getStringByIndex(attributes, 1);
    getLocalAddressWithError(Expression.localAddresses[0], Expression.Location.moduleID,
        Scopes, NewLocalVariables, topAddress, scriptName, lineNumber
    );
    Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 2));
    switch(Expression.Location.attribute){
        case index_a:
            setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            Expression.Location.attribute = value;
            break;
        case context_a:
            if(!attributeArgs.empty()){ //Set variableId
                Expression.Literal.setString(attributeArgs[0].value);
            }
            setOptionalAddressInCond(0, attributeArgs, Expression.localAddresses[1],
                scriptName, lineNumber, Scopes, NewLocalVariables, topAddress, canCreateNewVariable
            );
            Expression.Location.attribute = value;
            break;
        case size_a:
            Expression.Location.attribute = size_a;
            break;
        case back_a:
            Expression.Location.attribute = back_a;
            break;
        case null_a:
            break;
        default:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Invalid attribute '"
                << attributeToStr(Expression.Location.attribute) << "'.\n";
            return true;
    }
    return false;
}
inline void prepareObjectSource(const vector<string> & attributes, const vector<WordStruct> & attributeArgs,
    const unsigned & lineNumber, const string & scriptName, ConditionClass & Expression
){
    Expression.Location.layerID = getStringByIndex(attributes, 1);
    Expression.Location.objectID = getStringByIndex(attributes, 2);
    Expression.Location.moduleType = strToSource(getStringByIndex(attributes, 3));
    Expression.Location.moduleID = getStringByIndex(attributes, 4);
    Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 5));

    switch(Expression.Location.moduleType){
        case ancestor:
        case text:
        case editable_text:
            setOptionalAttributeValueInCond(0, 's', attributeArgs, Expression.Literal, scriptName, lineNumber);
            return;
        case mouse:
            setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            return;
        case collision:
            setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            if(attributeArgs.size() > 1){
                Expression.Location.spareID = attributeArgs[1].value;
            }
            return;
        default:
            return;
    }
    return;
}
inline bool setLocalAddressForExpression(ConditionClass &Expression, const vector<string> &attributes,
    vector<vector<VariableLocationStruct>> &Scopes, vector<VariableInfo> &NewLocalVariables,
    unsigned &topAddress, const string &scriptName, const unsigned &lineNumber
){
    if(getLocalAddressWithError(Expression.localAddresses[0], getStringByIndex(attributes, 0),
        Scopes, NewLocalVariables, topAddress, scriptName, lineNumber
    )){ return true; }

    Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 1));
    Expression.Location.spareID = getStringByIndex(attributes, 2);

    return false;
}
//Return true on failure.
bool setComplexDataAccessors(const vector<string> & attributes, const vector<WordStruct> & attributeArgs, unsigned lineNumber,
    const string & scriptName, ConditionClass & Expression, vector<vector<VariableLocationStruct>> & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, const bool & canCreateNewVariable
){
    string valueSource = attributes[0];
    Expression.Location.source = strToSource(valueSource);
    
    switch(Expression.Location.source){
        case key_pressed:
        case key_pressing:
        case key_released:
        case mouse_pressed:
        case mouse_pressing:
        case mouse_released:
        case screen_w:
        case screen_h:
            if(attributeArgs.size() > 0 && attributeArgs[0].type == 'c'){
                Expression.Literal.setString(attributeArgs[0].value); //Set variableId
                setOptionalAddressInCond(0, attributeArgs, Expression.localAddresses[0],
                    scriptName, lineNumber, Scopes, NewLocalVariables, topAddress, canCreateNewVariable
                );
            }
            else{
                setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            }
            return false;
        case exists:
        case is_directory:
            if(attributeArgs.size() > 0 && attributeArgs[0].type == 'c'){
                Expression.Literal.setString(attributeArgs[0].value);
                setOptionalAddressInCond(0, attributeArgs, Expression.localAddresses[0],
                    scriptName, lineNumber, Scopes, NewLocalVariables, topAddress, canCreateNewVariable
                );
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ":\n"
                    << NEW_LINE_PADDING << "Function " << valueSource << " requires one argument.\n";
            }
            return false;
        case booting:
        case second_passed:
        case fps:
        case any_key_pressed:
        case any_key_pressing:
        case any_key_released:
        case mouse_x:
        case mouse_y:
        case mouse_moved:
        case display_w:
        case display_h:
        case fullscreen:
        case display_resized:
        case used_os:
        case number_of_processes:
        case number_of_cameras:
        case number_of_layers:
        case number_of_objects:
            return false;
        case vector_s:
            return prepareVectorSource(attributes, attributeArgs, lineNumber, scriptName, Expression,
                Scopes, NewLocalVariables, topAddress, canCreateNewVariable
            );
        case camera:
            Expression.Location.cameraID = getStringByIndex(attributes, 1);
            Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 2));
            return false;
        case layer:
            Expression.Location.layerID = getStringByIndex(attributes, 1);
            Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 2));
            if(Expression.Location.attribute == in_group){
                setOptionalAttributeValueInCond(0, 's', attributeArgs, Expression.Literal, scriptName, lineNumber);
            }
        case object:
            prepareObjectSource(attributes, attributeArgs, lineNumber, scriptName, Expression);
            return false;
        case variable:
            Expression.Location.moduleID = getStringByIndex(attributes, 1);
            return false;
        case process:{
            string error;
            Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 1), error);
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
                    << error;
                return true;
            }
            return false;}
        default:
            Expression.Location.source = ValueSource::context;
            return setLocalAddressForExpression(Expression, attributes, Scopes, NewLocalVariables, topAddress, scriptName, lineNumber);
    }
    return false;
}
bool createExpression(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    vector<VariableModule> & resultStack, unsigned lineNumber, string scriptName, bool isConditionalExpression,
    vector<vector<VariableLocationStruct>> & Scopes, vector<VariableInfo> & NewLocalVariables, unsigned & topAddress,
    bool canCreateNewVariable
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
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Every expression must begin with parentheses.\n";
            return false;
        }
    }
    else{
        endingChar = "]";
        if(words[cursor].value != "["){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter " << cursor << " must begin with square brackets.\n";
            return false;
        }
    }
    
    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    WordStruct firstWord;
    
    while(words[cursor].value != endingChar){
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
        if(isStringInGroup(words[cursor].value, 9, "!", "==", "!=", "<", "<=", ">", ">=", "||", "&&")){
            if(!isConditionalExpression){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": Cannot use operators inside not conditional expression. Correct syntax: [[source_0] [source_1] ...]\n";
                return false;
            }
            if(Expression.size() == 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
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

            Expression.push_back(ConditionClass(""));

            string error;
            Expression.back().Location.source = strToSource(firstWord.value, error);

            if(firstWord.type == 'b'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 'b', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 'i'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 'd'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 'd', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 's'){
                Expression.back().Location.source = ValueSource::literal;
                if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 'c'){
                if(words[cursor].value != "]"){
                    cursor++;
                }

                vector<string> valueLocationAttributes;
                string currentAttribute = "";
                for(const char & letter : firstWord.value){
                    if(letter != '.'){
                        currentAttribute += letter;
                    }
                    else{
                        valueLocationAttributes.push_back(currentAttribute);
                        currentAttribute = "";
                    }
                }
                valueLocationAttributes.push_back(currentAttribute);

                vector<WordStruct> locationAttributesArgs; //Value in optional parenthesis
                if(words[cursor].value == "("){
                    if(cursor == words.size() - 1){
                        continue;
                    }
                    ++cursor;
                    while(cursor < words.size() - 1 && words[cursor].value != ")"){
                        if(words[cursor].value == "]"){
                            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parenthesis were not closed.\n";
                            return false;
                        }
                        locationAttributesArgs.push_back(words[cursor]);
                        ++cursor;
                    }
                    if(words[cursor].value != ")"){
                        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parenthesis were not closed.\n";
                        return false;
                    }
                    if(cursor == words.size() - 1){
                        continue;
                    }
                    ++cursor;
                }

                if(setComplexDataAccessors(valueLocationAttributes, locationAttributesArgs,
                    lineNumber, scriptName, Expression.back(), Scopes, NewLocalVariables, topAddress,
                    canCreateNewVariable
                )){ return false; }
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Word of the '" << firstWord.type << "' type is not valid.\n";
                return false;
            }
        }
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
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
    vector<vector<VariableLocationStruct>> & Scopes, unsigned & topAddress
){
    string eventID = "";
    
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 1 parameter.\n";
        return true;
    }
    if(words[1].type != 'c' && words[1].type != 'e'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": In the '" << words[0].value
            << "' instruction: The first parameter (id) is not a context.\n";
        return true;
    }

    eventID = words[1].value;
    
    if(eventID[0] != '_' && isStringInVector(EventContainerIDs, eventID)){
        if(words[0].value == "override"){
            removeFromStringVector(EventContainerIDs, eventID);
            removeModuleInstanceByID(EventContainer, eventID);
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Event with id \'" << eventID << "\' already exists.\n";
            return true;
        }
    }
    NewEvent = EventModule(eventID, &EventContainerIDs, layerID, objectID);
    
    Scopes.emplace_back(vector<VariableLocationStruct>()); //Create a new scope

    unsigned cursor = 2;
    
    if(NewEvent.getPassedVariables(words, cursor, lineNumber, scriptName, Scopes, topAddress)){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return true;
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
                            << NEW_LINE_PADDING << "In " << __FUNCTION__
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
            NewEvent.Parameters = PossibleEventSource->Parameters;
            if(!PossibleEventSource->isInline){
                break;
            }
        }
    }
    return false;
}
bool createInlineEvent(const string & scriptName, const unsigned & lineNumber, const string & layerID,
    const string & objectID, vector <EventModule> & EventContainer, vector <string> & EventContainerIDs,
    EventModule & NewEvent, const vector<WordStruct> & words, vector<string> & allAvailableEventIDs,
    vector<vector<VariableLocationStruct>> & Scopes
){
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 1 parameter.\n";
        return true;
    }
    if(words[1].type != 'c' && words[1].type != 'e'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": In the '" << words[0].value
            << "' instruction: The first parameter (id) is not a context.\n";
        return true;
    }
    string eventID = words[1].value;
    
    if(isStringInVector(EventContainerIDs, eventID)){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Event with id \'" << eventID << "\' already exists.\n";
        return true;
    }

    NewEvent = EventModule(eventID, &EventContainerIDs, layerID, objectID);

    Scopes.emplace_back(vector<VariableLocationStruct>()); //Create a new scope

    NewEvent.isInline = true;

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
bool setupFirstLastAllRandomInstr(const vector<WordStruct> & words, EventModule & NewEvent,
    OperationClass * Operation, unsigned lineNumber, string scriptName, string & error, unsigned & cursor,
    vector <string> & allAvailableEventIDs, vector<vector<VariableLocationStruct>> & Scopes, unsigned & topAddress
){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){
        return true;
    }
    if(words[1].type != 'c'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
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
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'c', "context", false, false, false
        )){ return true; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return true;
    }

    DataType outputType = null_dt;
    
    cursor = 2;
    if(Operation->Location.source == ValueSource::camera){
        if(optional(words, cursor, Operation->Location.cameraID)){ return false; }
        if(optional(words, cursor, Operation->Location.attribute)){ return false; }
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return true;
        }
        outputType = camera_inst;
        if(Operation->Location.attribute != null_a && Operation->Location.attribute != camera_a){
            outputType = pointer_inst;
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor,
            outputType, Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
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
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return true;
        }
        if(Operation->Location.attribute == null_a && Operation->Location.objectID.empty()
            && Operation->Location.moduleType == null_s && Operation->Location.moduleID.empty()
        ){
            outputType = layer_inst;
        }
        else if(Operation->Location.attribute != object_a && Operation->Location.objectID.empty()
            && Operation->Location.moduleType == null_s && Operation->Location.moduleID.empty()
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
        if(optionalOutput(scriptName, lineNumber, error, words, cursor,
            outputType, Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
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
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return true;
        }
        outputType = attributeToInstDataType(InstrDescription(), Operation->Location.attribute);
        if(outputType == null_dt){
            outputType = any_dt;
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor,
            outputType, Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.size() == 0){ return false; }
            return true;
        }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Source '" << words[1].value << "' does not exist.\n";
        return true;
    }
    return false;
}
bool setupIndexInstr(const vector<WordStruct> & words, EventModule & NewEvent, OperationClass * Operation,
    unsigned lineNumber, string scriptName, string & error, unsigned & cursor,
    vector<vector<VariableLocationStruct>> & Scopes, unsigned & topAddress
){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){
        return true;
    }
    if(words[1].type != 'c'){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value
            << "' instruction: The first two parameters are not of a context type.\n";
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
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "source", false, false, false
        )){ return true; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return true;
    }
    
    cursor = 2;
    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'i', "indexes", false, false
    )){ return true; }
    if(optional(words, cursor, Operation->Location.attribute)){ return false; }
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.size() == 0){ return false; }
        return true;
    }
    return false;
}
inline bool isStringAnInstanceDeclaration(const string & instruction){
    return isStringInGroup(instruction, 36, "Val", "ValVec", "Pointer", "PointerVec", "Camera", "CameraVec", "Layer", "LayerVec", //8
        "Object", "ObjectVec", "Var", "VarVec", "Vec", "VecVec", "Text", "TextVec", "EditText", "EditTextVec", "SText", "STextVec", //12
        "SEditText", "SEditTextVec", "Image", "ImageVec", "Movement", "MovementVec", "Collision", "CollisionVec", "Particles", //9
        "Event", "EventVec", "Scrollbar", "ScrollbarVec", "Primitive", "PrimitiveVec", "any" //7
    );
} 
ReturnType AncestorObject::translateTokensIntoEngineInstruction(
    const vector<WordStruct> & words, const string & scriptName, const unsigned & lineNumber,
    vector<vector<VariableLocationStruct>> & Scopes, unsigned & topAddress,
    bool & triggerBreakpoint, EventModule & NewEvent, vector<string> & allAvailableEventIDs,
    BranchingStackStruct & BranchingStack
){
    unsigned cursor = 1;
    string error;
    OperationClass * Operation = nullptr;

    if(words[0].value == "import"){
        return ReturnType::OK;
    }
    if(words[0].value == "start" ||  words[0].value == "override"){
        if(createEvent(scriptName, lineNumber, layerID, ID, EventContainer,
            EventContainerIDs, NewEvent, words, Scopes, topAddress
        )){ return ReturnType::ERROR; }
        allAvailableEventIDs.clear();
        allAvailableEventIDs.push_back(NewEvent.getID());
    }
    else if(Scopes.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ":\n"
            << NEW_LINE_PADDING << "Instruction \'" << words[0].value << "\' cannot be used outside of an event scope.\n";
        return ReturnType::ERROR;
    }
    else if(words[0].value == "compiler_breakpoint"){
        triggerBreakpoint = true;
        cerr << "Warning: The 'compiler_breakpoint' instruction can be used only in the debugger.\n";
    }
    else if(words[0].value == "end"){
        if(NewEvent.isInline){
            NewEvent.Parameters.clear();
        }
        Scopes.pop_back(); //Remove the scope of the last event
        EventContainer.push_back(NewEvent);
        NewEvent = EventModule();
    }
    else if(words[0].value == "triggers"){
        if(words.size() < 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": Instruction \'" << words[0].value << "\' requires at least 2 parameters.\n";
            return ReturnType::ERROR;
        }
        while(cursor < words.size()){
            if(words[cursor].type != 'c'){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: Parameter " << cursor << " is not a context.\n";
                return ReturnType::ERROR;
            }
            if(words[cursor].type != 'e'){
                TriggerType NewTrigger = strToTrigger(words[cursor].value);
                if(NewTrigger == null_t){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__
                        << ": In the '" << words[0].value << "' instruction: Parameter '"
                        << words[cursor].value << "' is not a valid trigger.\n";
                    return ReturnType::ERROR;
                }
                NewEvent.primaryTriggerTypes.push_back(NewTrigger);
                NewEvent.isFunction = false;
            }
            cursor++;
        }
        /*if(!gatherStringVector(words, cursor, NewEvent.primaryTriggerTypes, lineNumber, scriptName)){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << ERROR_SPACING << "In " << __FUNCTION__ << ": Context gather failed.\n";
            return ReturnType::ERROR;
        }*/
    }
    else if(words[0].value == "run"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        Operation->specialValue = NewEvent.Children.size();
        NewEvent.Children.emplace_back(ChildStruct(words[1].value, vector<PassingVariableInfo>(), 0, scriptName, lineNumber));
        cursor = 2;

        if(NewEvent.getPassingVariables(NewEvent.Children.back().Arguments,
            words, cursor, lineNumber, scriptName, Scopes, topAddress
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "if"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        Scopes.emplace_back(vector<VariableLocationStruct>());
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
        BranchingStack.ifElseJumpStack.push_back(NewEvent.Operations.size()-1);
        BranchingStack.usedElseStatements.push_back(0);
        BranchingStack.ifEndJumpStack.push_back(vector<unsigned>());
        BranchingStack.ifEndJumpStack.back().push_back(NewEvent.Operations.size()-1);
    }
    else if(words[0].value == "else_if"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        Scopes.pop_back();
        Scopes.emplace_back(vector<VariableLocationStruct>());
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
        if(BranchingStack.ifElseJumpStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        NewEvent.Operations[BranchingStack.ifElseJumpStack.back()].specialValue = NewEvent.Operations.size()-1;
        BranchingStack.ifElseJumpStack.pop_back();
        BranchingStack.ifElseJumpStack.push_back(NewEvent.Operations.size()-1);
        if(BranchingStack.ifEndJumpStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        BranchingStack.ifEndJumpStack.back().push_back(NewEvent.Operations.size()-1);
    }
    else if(words[0].value == "else"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        Scopes.pop_back();
        Scopes.emplace_back(vector<VariableLocationStruct>());
        if(BranchingStack.ifElseJumpStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        NewEvent.Operations[BranchingStack.ifElseJumpStack.back()].specialValue = NewEvent.Operations.size()-1;
        BranchingStack.ifElseJumpStack.pop_back();
        if(BranchingStack.usedElseStatements.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        BranchingStack.usedElseStatements.back() = 1;
        if(BranchingStack.ifEndJumpStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        BranchingStack.ifEndJumpStack.back().push_back(NewEvent.Operations.size()-1);
    }
    else if(words[0].value == "end_if"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        Scopes.pop_back();
        if(BranchingStack.ifEndJumpStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        if(BranchingStack.usedElseStatements.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        if(BranchingStack.usedElseStatements.back() == 0){
            if(BranchingStack.ifElseJumpStack.size() == 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                    << words[0].value << "' outside the if statement scope.\n";
                return ReturnType::ERROR;
            }
            NewEvent.Operations[BranchingStack.ifElseJumpStack.back()].specialValue = NewEvent.Operations.size()-1;
            BranchingStack.ifElseJumpStack.pop_back();
        }
        BranchingStack.usedElseStatements.pop_back();
        if(BranchingStack.ifEndJumpStack.size() > 0){
            for(unsigned lineNumberIdx : BranchingStack.ifEndJumpStack.back()){
                NewEvent.Operations[lineNumberIdx].jumpToLine = NewEvent.Operations.size()-1;
            }
            BranchingStack.ifEndJumpStack.back().clear();
            BranchingStack.ifEndJumpStack.pop_back();
        }

    }
    else if(words[0].value == "while"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        Scopes.emplace_back(vector<VariableLocationStruct>());
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
        BranchingStack.whileStartStack.push_back(NewEvent.Operations.size()-1);
        BranchingStack.whileEndStack.push_back(vector<unsigned>());
        BranchingStack.whileEndStack.back().push_back(NewEvent.Operations.size()-1);
    }
    else if(words[0].value == "end_while"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        Scopes.pop_back();
        if(BranchingStack.whileStartStack.size() == 0 || BranchingStack.whileEndStack.size() == 0 || BranchingStack.whileEndStack.back().size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' before any loop.\n";
            return ReturnType::ERROR;
        }
        NewEvent.Operations.back().jumpToLine = BranchingStack.whileStartStack.back();
        BranchingStack.whileStartStack.pop_back();
        for(unsigned & index : BranchingStack.whileEndStack.back()){
            NewEvent.Operations[index].jumpToLine = NewEvent.Operations.size()-1;
        }
        BranchingStack.whileEndStack.back().clear();
        BranchingStack.whileEndStack.pop_back();
    }
    else if(words[0].value == "continue"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(BranchingStack.whileStartStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside a loop.\n";
            return ReturnType::ERROR;
        }
        NewEvent.Operations.back().jumpToLine = BranchingStack.whileStartStack.back();
    }
    else if(words[0].value == "break"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(BranchingStack.whileEndStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside a loop.\n";
            return ReturnType::ERROR;
        }
        BranchingStack.whileEndStack.back().push_back(NewEvent.Operations.size()-1);
    }
    else if(isStringInGroup(words[0].value, 10, "return", "reboot", "exit", "delete_this_event",
        "reset_keyboard", "dump_context_stack", "dump_memory", "dump_local_memory", "restart_drag", "breakpoint"
    )){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
    }
    else if(isStringInGroup(words[0].value, 4, "first", "last", "all", "random")){
        if(setupFirstLastAllRandomInstr(words, NewEvent,
            Operation, lineNumber, scriptName, error, cursor,
            allAvailableEventIDs, Scopes, topAddress
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "index"){
        if(setupIndexInstr(words, NewEvent, Operation, lineNumber,
            scriptName, error, cursor, Scopes, topAddress
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "index_vec"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
            return ReturnType::ERROR;
        }

        if(words[1].value != ""){
            Operation->Location.source = ValueSource::context;
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                1, 'c', "vector", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": First parameter cannot be an empty context.\n";
            return ReturnType::ERROR;
        }
        
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "index", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 3;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(isStringInGroup(words[0].value, 8, "+", "-", "*", "/", "%", "**", "random_int", "assert")){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[0].value == "+" || words[0].value == "assert"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                1, 'a', "left", false, false, words[0].value == "assert"
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'a', "right", false, false, words[0].value == "assert"
            )){ return ReturnType::ERROR; }
        }
        else{
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                1, 'n', "left", false, false, false
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'n', "right", false, false, false
            )){ return ReturnType::ERROR; }
        }
        cursor = 3;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "load"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'a', "address", false, true, true
        )){ return ReturnType::ERROR; }
    }
    else if(isStringInGroup(words[0].value, 5, "=", "+=", "-=", "*=", "/=")){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "left", false, false, false
        )){ return ReturnType::ERROR; }
        if(words[0].value == "=" || words[0].value == "+="){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'a', "right", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else{
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'n', "right", false, false, false
            )){ return ReturnType::ERROR; }
        }
        cursor = 3;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "find_by_id_2"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "source", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "id", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 3;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(isStringInGroup(words[0].value, 4, "sum", "intersection", "difference", "in")){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "left", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'c', "right", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 3;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "++" || words[0].value == "--" || words[0].value == "delete"
        || words[0].value == "demolish" || words[0].value == "rbind" || words[0].value == "type"
    ){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        string parameterName = "context";
        if(words[0].value == "demolish" || words[0].value == "rbind"){
            parameterName = "objects";
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', parameterName, false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "next"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'i', "number", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "access"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.size() > 0){ return ReturnType::ERROR; }
        }
        if(!createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, false, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
    }
    else if(isStringInGroup(words[0].value, 8, "bool", "int", "double", "string", "bool_vec", "int_vec", "double_vec", "string_vec")){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        
        DataType newVariableType = value_inst;
        bool makeOutputGlobal = false;
        switch(Operation->instruction){
            case bool_vec_i:
            case int_vec_i:
            case double_vec_i:
            case string_vec_i:
                newVariableType = value_vec;
                break;
            default:
                break;
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, newVariableType,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, makeOutputGlobal, true
        )){
            if(error.size() > 0){ return ReturnType::ERROR; }
        }
        
        switch(Operation->instruction){
            case bool_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 'b', "value", false, false, true
                )){ return ReturnType::ERROR; }
                break;
            case bool_vec_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 'b', "values", false, false, false
                )){ return ReturnType::ERROR; }
                break;
            case int_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 'i', "value", false, false, true
                )){ return ReturnType::ERROR; }
                break;
            case int_vec_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 'i', "values", false, false, false
                )){ return ReturnType::ERROR; }
                break;
            case double_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 'd', "value", false, false, true
                )){ return ReturnType::ERROR; }
                break;
            case double_vec_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 'd', "values", false, false, false
                )){ return ReturnType::ERROR; }
                break;
            case string_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 's', "value", false, false, true
                )){ return ReturnType::ERROR; }
                break;
            case string_vec_i:
                if(Operation->addLiteralOrVectorOrVariableToParameters(
                    scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                    cursor, 's', "values", false, false, false
                )){ return ReturnType::ERROR; }
                break;
            default:
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Literal type is required.\n";
                return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "find_by_id"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
            return ReturnType::ERROR;
        }
        Operation->Location.source = strToSource(words[1].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            return ReturnType::ERROR;
        }
        cursor = 2;
        if(words[1].value == "Cameras"){
            if(words.size() < 4){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": \'find_by_id " << words[1].value << "\' requires at least 2 additional parameters.\n";
                return ReturnType::ERROR;
            }
            if(optional(words, cursor, Operation->Location.cameraID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
                Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
            )){
                if(error.empty()){ return ReturnType::OK; }
                return ReturnType::ERROR;
            }
        }
        else if(words[1].value == "Layers"){
            if(words.size() < 7){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": \'find_by_id " << words[1].value << "\' requires at least 5 additional parameters.\n";
                return ReturnType::ERROR;
            }
            if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
                Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
            )){
                if(error.empty()){ return ReturnType::OK; }
                return ReturnType::ERROR;
            }
        }
        else if(words[1].value == "context" || words[1].value == "c" || words[1].type == 'e'){
            if(Operation->addVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'c', "scripts", false, false
            )){ return ReturnType::ERROR; }
            if(words.size() < cursor + 5){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": \'find_by_id " << words[1].value << " [context_list]\' requires at least 5 additional parameters.\n";
                return ReturnType::ERROR;
            }
            if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
                Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
            )){
                if(error.empty()){ return ReturnType::OK; }
                return ReturnType::ERROR;
            }
        }
    }
    else if(words[0].value == "clone"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "left", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'c', "right", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'b', "changeOldID", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "new"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
            return ReturnType::ERROR;
        }
        Operation->Location.source = strToSource(words[1].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            return ReturnType::ERROR;
        }
        
        //If the destination is provided as a variable, skip one parameter in the instruction.
        if(words[2].type == 'c'){
            Operation->addLiteralParameter(VariableModule::newString("variable"));
        }
        else{
            Operation->addLiteralParameter(VariableModule::newString("location"));
        }
        
        cursor = 2;
        if(words[cursor].type == 'c'){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'c', "destination", false, false, false
            )){ return ReturnType::ERROR; }
            cursor++;
        }
        else{
            if(Operation->Location.source == ValueSource::object){
                if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
            }
            else if(Operation->Location.source != ValueSource::camera && Operation->Location.source != ValueSource::layer){
                if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
                if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
            }
        }
        
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'i', "quantity", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        cursor++;
        if(Operation->addLiteralOrVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 's', "new_ids", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        DataType typeForNewEntity = sourceToEntityType(InstrDescription(), Operation->Location.source);
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, typeForNewEntity,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "bind"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "objects", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 's', "scripts", false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "build"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "objects", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'b', "reset", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'b', "do_not_preserve", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "load_build" || words[0].value == "build_subset"
        || words[0].value == "inject_code" || words[0].value == "inject_instr"
    ){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'c', "objects", false, false
        )){ return ReturnType::ERROR; }
        string parameterName = "paths";
        if(words[0].value == "inject_code"){
            parameterName = "code";
        }
        else if(words[0].value == "inject_instr"){
            parameterName = "instructions";
        }
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 's', parameterName, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'b', "reset", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor + 1, 'b', "do_not_preserve", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "fun"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "objects", false, false, false
        )){ return ReturnType::ERROR; }
        Operation->Location.attribute = strToAttribute(words[2].value);
        cursor = 3;
        while(words.size() > cursor){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'a', "value", false, false, false
            )){ return ReturnType::ERROR; }
            cursor++;
        }
    }
    else if(words[0].value == "env"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        Operation->addLiteralParameter(VariableModule::newString(words[1].value));
        if(words[1].value == "window_title"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 's', "title", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "display_size"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'i', "width", false, false, false
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                3, 'i', "height", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else{
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'i', "value", false, false, false
            )){ return ReturnType::ERROR; }
        }
    }
    else if(words[0].value == "edit_proc"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        for(cursor = 2; cursor < words.size(); cursor++){
            if(words[cursor].type != 'c'){
                continue;
            }
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: Parameter " << cursor << " must be a literal.\n";
            return ReturnType::ERROR;
        }
        Operation->addLiteralParameter(VariableModule::newString(words[1].value));
        if(isStringInGroup(words[1].value, 2, "clear_layers", "clear_cameras")){
            return ReturnType::OK;
        }
        
        if(words[1].value == "id"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 's', "new_id", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "reservation_multiplier"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'd', "multiplier", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "window_pos" || words[1].value == "window_size" || words[1].value == "min_window_size"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'i', "x", false, false, false
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                3, 'i', "y", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "window_tint"){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'd', "red", false, false, false
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                3, 'd', "green", false, false, false
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                4, 'd', "blue", false, false, false
            )){ return ReturnType::ERROR; }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                5, 'd', "alpha", false, false, false
            )){ return ReturnType::ERROR; }
        }
        else{
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 'i', "value", false, false, false
            )){ return ReturnType::ERROR; }
        }
    }
    else if(words[0].value == "load_bitmap"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "name", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'b', "light", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            4, 'b', "ignore_warnings", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "mkdir" || words[0].value == "rm" || words[0].value == "rmll"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "mv"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "new_path", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "print_v" || words[0].value == "print_d" || words[0].value == "print"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[0].value == "print_v"){ //output
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
                Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
            )){
                if(error.empty()){ return ReturnType::OK; }
                return ReturnType::ERROR;
            }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 's', "delimeter", false, false, false
            )){ return ReturnType::ERROR; }
            cursor = 3;
        }
        else if(words[0].value == "print_d"){ //delimeter
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                1, 's', "delimeter", false, false, false
            )){ return ReturnType::ERROR; }
            cursor = 2;
        }
        else{
            Operation->addEmptyParameter();
        }
        while(cursor < words.size()){
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'a', "value", false, false, false
            )){ return ReturnType::ERROR; }
            cursor++;
        }
    }
    else if(words[0].value == "load_text"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "save_text"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "text", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 's', "delimeter", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "ls"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'b', "recursive", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            4, 'i', "max_depth", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "lse"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        Operation->addLiteralParameter(VariableModule::newString(words[1].value));
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'b', "detail", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "new_proc"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "name", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "layer", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 's', "object", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            4, 's', "script", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "var"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, variable_mod,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, true
        )){
            if(error.size() > 0){ return ReturnType::ERROR; }
        }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'a', "value", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "vec"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(words[1].type != 'c'){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        Operation->addLiteralParameter(VariableModule::newString(words[1].value));
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, vector_mod,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, true
        )){
            if(error.size() > 0){ return ReturnType::ERROR; }
        }
        if(words.size() < 4){
            return ReturnType::OK;
        }
        if(words[1].value == "bool"){
            if(Operation->addVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'b', "scripts", false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "int"){
            if(Operation->addVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'i', "scripts", false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "double"){
            if(Operation->addVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'd', "scripts", false, false
            )){ return ReturnType::ERROR; }
        }
        else if(words[1].value == "string"){
            if(Operation->addVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 's', "scripts", false, false
            )){ return ReturnType::ERROR; }
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": In the instruction '" << words[0].value
                << "': The type \'" << words[1].value << "\' does not exist.\n";
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "tokenize"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "delimeter", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "text", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 3;
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'c', "outputs", false, true
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "tree" || words[0].value == "pwd" || words[0].value == "console_input"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "len"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "text", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "size"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "text", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "substr"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "text", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "begin", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'i', "length", false, false, false
        )){ return ReturnType::ERROR; }

        cursor = 4;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "load_font"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "size", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 's', "name", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            4, 'b', "ignore_warnings", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "cd"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "path", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "similar"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "pattern", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 's', "vector", false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'b', "longest_common_part", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
        cursor++;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "count"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "pattern", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "text", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 3;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "create_display"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 5, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'i', "display_width", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "display_height", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'i', "backbuffer_width", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            4, 'i', "backbuffer_height", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            5, 'b', "auto_scale_backbuffer", true, false, false
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[0].value == "start_timer"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "name", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[0].value == "stop_timer"){
        if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName)){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 's', "name", false, false, false
        )){ return ReturnType::ERROR; }
        cursor = 2;
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(isStringAnInstanceDeclaration(words[0].value)){
        if(!prepareNewVariableDeclaration(words, 2, lineNumber, scriptName)){ return ReturnType::ERROR; }
        auto[localAddress, e_Result] = getLocalAddress(
            words[1].value, strToDataType(words[0].value), Scopes, NewEvent.LocalVariables,
            topAddress, true, false, false, true
        );
        if(e_Result != ReturnType::OK){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Variable declaration failed.\n";
            return ReturnType::ERROR;
        }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' does not exist.\n";
    }
    return ReturnType::OK;
}
ReturnType AncestorObject::assembleEvents(vector<string> & code, const string & scriptName,
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
){
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

    // Pre-define built-in global variables.

    vector<WordStruct> words;
    EventModule NewEvent = EventModule();
    unsigned lineNumber = 0;
    bool triggerBreakpoint = false;

    vector <string> allAvailableEventIDs;

    vector<vector<VariableLocationStruct>> Scopes;
    Scopes.push_back(GlobalScope);

    BranchingStackStruct BranchingStack;

    for(string line : code){
        lineNumber++;
        words.clear();
        line = findAndUseSpecialCharacters(line);
        words = tokenizeCode(line);
        //words = mergeStrings(words);
        if(words.size() == 0){
            continue;
        }

        if(triggerBreakpoint){
            raise(SIGINT);
            triggerBreakpoint = false;
        }
        
        ReturnType result = translateTokensIntoEngineInstruction(
            words, scriptName, lineNumber, Scopes, topMemoryAddress, triggerBreakpoint,
            NewEvent, allAvailableEventIDs, BranchingStack
        );
        if(result == ReturnType::ERROR){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Compilation aborted due to previous errors.\n";
            return ReturnType::ERROR;
        }
    }
    if(words.size() > 0){
        if(words[0].value != "end"){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Every event must end with 'end' instruction.\n";
        }
    }

    if(BranchingStack.ifElseJumpStack.size() > 0 || BranchingStack.ifEndJumpStack.size() > 0 || BranchingStack.usedElseStatements.size() > 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Every if statement must end with '"
            << instrToStr(end_if) << "' label.\n";
    }

    if(BranchingStack.whileStartStack.size() > 0 || BranchingStack.whileEndStack.size() > 0 ){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Every while loop must end with '"
            << instrToStr(end_while) << "' label.\n";
    }

    if(Scopes.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Global scope has been destroyed.\n";
        return ReturnType::ERROR;
    }
    if(Scopes[0].size() < GlobalScope.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Global variables has been deleted.\n";
        return ReturnType::ERROR;
    }

    //If new global variables were defined, add them to the global scope.
    if(Scopes[0].size() > GlobalScope.size()){
        GlobalScope = Scopes[0];
    }

    return ReturnType::OK;

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
void AncestorObject::translateAllScripts(bool clearEvents, bool allowNotAscii, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress){
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
            size_t preAssemblyEventCount = EventContainer.size();
            ReturnType assemblyStatus = assembleEvents(code, scriptName, GlobalScope, topMemoryAddress);
            if(assemblyStatus == ReturnType::ERROR){
                EventContainer.resize(preAssemblyEventCount);
                return;
            }
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
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
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

            size_t preAssemblyEventCount = EventContainer.size();
            ReturnType assemblyStatus = assembleEvents(code, scriptName, GlobalScope, topMemoryAddress);
            if(assemblyStatus == ReturnType::ERROR){
                EventContainer.resize(preAssemblyEventCount);
                return;
            }

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
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
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

            size_t preAssemblyEventCount = EventContainer.size();
            ReturnType assemblyStatus = assembleEvents(code, scriptName, GlobalScope, topMemoryAddress);
            if(assemblyStatus == ReturnType::ERROR){
                EventContainer.resize(preAssemblyEventCount);
                return;
            }

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
void AncestorObject::injectCode(bool clearEvents, vector<string> code, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress){
    if(clearEvents){
        clearAllEvents();
    }

    code = removeComments(code);
    
    if(code.size() > 0){
        size_t preAssemblyEventCount = EventContainer.size();
        ReturnType assemblyStatus = assembleEvents(code, "<injection>", GlobalScope, topMemoryAddress);
        if(assemblyStatus == ReturnType::ERROR){
            EventContainer.resize(preAssemblyEventCount);
            return;
        }
    }
}
void AncestorObject::injectInstructions(bool clearEvents, vector<string> instructions, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress){
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
    preprocessed.insert(preprocessed.begin(), "start _");
    preprocessed.emplace_back("delete_this_event");
    preprocessed.emplace_back("end");
    size_t preAssemblyEventCount = EventContainer.size();
    ReturnType assemblyStatus = assembleEvents(preprocessed, "<injection>", GlobalScope, topMemoryAddress);
    if(assemblyStatus == ReturnType::ERROR){
        EventContainer.resize(preAssemblyEventCount);
        return;
    }
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

bool ModulesPointers::empty() const{
    return Texts.empty() && EditableTexts.empty() && SuperTexts.empty()
        && SuperEditableTexts.empty()  && Images.empty() &&
        Movements.empty() && Collisions.empty() && Particles.empty() &&
        Events.empty() && Variables.empty() && Scrollbars.empty()
        && Primitives.empty() && Vectors.empty();
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
        case pointer_inst:
            return pointer_vec;
        case any_dt:
            return any_dt;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << dataTypeToStr(oldType) << "\' is not valid for this operation.\n";
            return null_dt;
    }
}