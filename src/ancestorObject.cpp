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
    hasInvalidatedMemory = true;
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
        EventContainer.back().clone(Event, eventContainerIDs, newLayerID, getID(), true);
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

void AncestorObject::setID(string newID, vector<string> &listOfIDs){
    PrimaryModule::setID(newID, listOfIDs);
    objectLookupID = layerID + ID;
}

void AncestorObject::primaryConstructor(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    PrimaryModule::primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    objectLookupID = layerID + ID;
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

std::pair<vector<WordStruct>, bool> tokenizeCode(const string & input){
    std::regex word_regex("([\\w+\\.*]*\\w+)|;|:|\\,|\\.|==|=|>=|<=|>|<|-=|\\+=|\\*=|/=|\\*\\*|\\+\\+|\\-\\-|\\+|-|\\*|/|%|\\[|\\]|\\(|\\\\\\\"|\\)|\"|!=|!|\\|\\||&&|\n|\t|&|@|#", std::regex_constants::icase);
    auto words_begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
    auto words_end = std::sregex_iterator();

    bool triggerPreprocessor = false;

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
    bool keywordAquired = false;

    //If a word is not a part of a string, add it as a single word, otherwise add the whole string as a single word and ignore its parts. Don't add " to the words.
    for(unsigned i = 0; i < output.size(); i++){
        if(output[i][0] == '\"' && !isInsideStringSector){
            isInsideStringSector = true;
            mergedOutput.emplace_back(WordStruct(TokenType::string_tk, stringSectors[sector], false));
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
        if(!keywordAquired){
            mergedOutput.emplace_back(WordStruct(TokenType::keyword_tk, output[i], false, strToInstr(output[i], false)));
            if(mergedOutput.back().instruction == null){
                mergedOutput.back().type = TokenType::identifier_tk;
            }
            keywordAquired = true;
            continue;
        }
        if(isStringInGroup(output[i], 9, "!", "==", "!=", "<", "<=", ">", ">=", "||", "&&")){
            mergedOutput.emplace_back(WordStruct(TokenType::keyword_tk, output[i], false, strToInstr(output[i], false)));
            continue;
        }
        if(output[i] == "_"){
            mergedOutput.emplace_back(WordStruct(TokenType::empty_tk, output[i], false));
            continue;
        }
        if(output[i] == "("){
            triggerPreprocessor = false;
            mergedOutput.emplace_back(WordStruct(TokenType::start_expr_tk, output[i], false));
            continue;
        }
        if(output[i] == ")"){
            mergedOutput.emplace_back(WordStruct(TokenType::end_expr_tk, output[i], false));
            continue;
        }
        if(output[i] == "{"){
            mergedOutput.emplace_back(WordStruct(TokenType::start_scope_tk, output[i], false));
            continue;
        }
        if(output[i] == "}"){
            mergedOutput.emplace_back(WordStruct(TokenType::end_scope_tk, output[i], false));
            continue;
        }
        if(output[i] == "["){
            mergedOutput.emplace_back(WordStruct(TokenType::open_brackets_tk, output[i], false));
            continue;
        }
        if(output[i] == "]"){
            mergedOutput.emplace_back(WordStruct(TokenType::close_brackets_tk, output[i], false));
            continue;
        }
        if(output[i] == "true"){
            mergedOutput.emplace_back(WordStruct(TokenType::bool_tk, "1", false));
            continue;
        }
        if(output[i] == "false"){
            mergedOutput.emplace_back(WordStruct(TokenType::bool_tk, "0", false));
            continue;
        }
        if(canStringBeDouble(output[i])){
            cstod(output[i], error);
            if(mergedOutput.size() > 0 && mergedOutput.back().type == TokenType::identifier_tk && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct(TokenType::double_tk, "-" + output[i], false));
            }
            else{
                mergedOutput.emplace_back(WordStruct(TokenType::double_tk, output[i], false));
            }
            continue;
        }
        cstoi(output[i], error);
        if(error == ""){
            if(mergedOutput.size() > 0 && mergedOutput.back().type == TokenType::identifier_tk && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct(TokenType::int_tk, "-" + output[i], false));
            }
            else{
                mergedOutput.emplace_back(WordStruct(TokenType::int_tk, output[i], false));
            }
            
            continue;
        }
        if(mergedOutput.size() > 0 && mergedOutput.back().type == TokenType::identifier_tk && mergedOutput.back().value == "-"){
            mergedOutput.pop_back();
            mergedOutput.emplace_back(WordStruct(TokenType::identifier_tk, output[i], true));
        }
        else{
            mergedOutput.emplace_back(WordStruct(TokenType::identifier_tk, output[i], false));
        }
    }

    return {mergedOutput, triggerPreprocessor};
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
    if(words[cursor].type != TokenType::empty_tk){
        variable = words[cursor].value;
    }
    cursor++;
    return false;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, AttributeType & attribute){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
        attribute = strToAttribute(words[cursor].value);
    }
    cursor++;
    return false;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, ValueSource & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
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
    unsigned & cursor, const DataType & outputType, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, OutputParameterStruct & OutputParameter,
    bool makeOutputGlobal = false, bool forceNewDeclaration = false
){
    error = "";
    if(cursor + 1 > words.size()){
        return true;
    }
    const WordStruct & instructionOutput = words[cursor];
    
    if(instructionOutput.type != TokenType::empty_tk && instructionOutput.type != TokenType::identifier_tk){
        error = "Parameter 'output' (" + intToStr(cursor);
        error += ") must be a context.";
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return true;
    }

    if(instructionOutput.type == TokenType::identifier_tk){
        OutputParameter.variableID = instructionOutput.value;
        OutputParameter.type = outputType;

        ReturnType result = ReturnType::OK;
        std::tie(OutputParameter.localAddress, result) = getLocalAddress(
            OutputParameter.variableID, outputType, Scopes, NewLocalVariables,
            topAddress, true, makeOutputGlobal, false, forceNewDeclaration
        );
        if(result == ReturnType::OUT_OF_SCOPE){
            error = + "Index (" + std::to_string(OutputParameter.localAddress)
                + ") of the local variable is out of scope ("
                + std::to_string(NewLocalVariables.size()) + ").\n";
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            OutputParameter.localAddress = 0;
            return true;
        }
        else if(result == ReturnType::UNDEFINED){
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
    if(words[cursor].type != TokenType::empty_tk){
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
                if(words[cursor].type == TokenType::string_tk){
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
    if(words[cursor].type != TokenType::close_brackets_tk){
        cursor++;
    }
    return false;
}
inline void setOptionalAttributeValueInCond(const size_t & index, const char & type,
    const vector<WordStruct> & words, VariableModule & Variable, const string & scriptName,
    const unsigned & lineNumber
){
    if(index >= words.size() || words[index].type == TokenType::empty_tk){
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
            if(words[index].type == TokenType::string_tk){
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
    ScopeType &Scopes, vector<VariableInfo> &NewLocalVariables,
    unsigned &topAddress, const string &scriptName, const unsigned &lineNumber
){
    ReturnType result = ReturnType::OK;
    std::tie(localAddress, result) = getLocalAddress(
        variableId, any_dt, Scopes, NewLocalVariables, topAddress, false
    );
    switch(result){
        case ReturnType::OUT_OF_SCOPE:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
                << "Index (" << localAddress
                << ") of a variable '" << variableId
                << "' is out of scope (" << NewLocalVariables.size() << ").\n";
            localAddress = 0;
            return true;
        case ReturnType::UNDEFINED:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
                << "Index (" << localAddress
                << ") of a variable '" << variableId
                << "' is out of scope (" << NewLocalVariables.size() << ").\n";
            localAddress = 0;
            return true;
        default:
            return false;
    }
}
inline void setOptionalAddressInCond(const size_t & index, const vector<WordStruct> & words, unsigned & localAddress,
    const string & scriptName, unsigned lineNumber, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable
){
    if(index >= words.size() || words[index].type == TokenType::empty_tk){
        return;
    }
    if(words[index].type == TokenType::identifier_tk){
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
    if(words[cursor].type != TokenType::empty_tk){
        string error;
        variable = cstoi(words[cursor].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
        }
    }
    if(words[cursor].type!= TokenType::close_brackets_tk){
        cursor++;
    }
    return false;
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, double & variable, string scriptName, unsigned lineNumber){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
        string error;
        variable = cstod(words[cursor].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
        }
    }
    if(words[cursor].type != TokenType::close_brackets_tk){
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
    ScopeType & Scopes, vector<VariableInfo> & NewLocalVariables, unsigned & topAddress,
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
    ScopeType &Scopes, vector<VariableInfo> &NewLocalVariables,
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
    const string & scriptName, ConditionClass & Expression, ScopeType & Scopes,
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
            if(attributeArgs.size() > 0 && attributeArgs[0].type == TokenType::identifier_tk){
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
            if(attributeArgs.size() > 0 && attributeArgs[0].type == TokenType::identifier_tk){
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
inline bool isLogicInstr(EngineInstr instruction){
    switch(instruction){
        case EngineInstr::not_i:
        case EngineInstr::equal:
        case EngineInstr::not_equal:
        case EngineInstr::less:
        case EngineInstr::less_equal:
        case EngineInstr::more:
        case EngineInstr::more_equal:
        case EngineInstr::or_i:
        case EngineInstr::and_i:
            return true;
        default:
            return false;
    }
}
ReturnType parseUserDefinedIdentifiers(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    unsigned lineNumber, const string & scriptName, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable, WordStruct &firstWord
){
    if(words[cursor].type != TokenType::close_brackets_tk){
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
    if(words[cursor].type == TokenType::start_expr_tk){
        if(cursor == words.size() - 1){
            return ReturnType::CONTINUE;
        }
        ++cursor;
        while(cursor < words.size() - 1 && words[cursor].type != TokenType::end_expr_tk){
            if(words[cursor].type == TokenType::close_brackets_tk){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parenthesis were not closed.\n";
                return ReturnType::ERROR;
            }
            locationAttributesArgs.push_back(words[cursor]);
            ++cursor;
        }
        if(words[cursor].type != TokenType::end_expr_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parenthesis were not closed.\n";
            return ReturnType::ERROR;
        }
        if(cursor == words.size() - 1){
            return ReturnType::CONTINUE;
        }
        ++cursor;
    }

    if(setComplexDataAccessors(valueLocationAttributes, locationAttributesArgs,
        lineNumber, scriptName, Expression.back(), Scopes, NewLocalVariables, topAddress,
        canCreateNewVariable
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType parseExpression(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    unsigned lineNumber, const string & scriptName, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable, WordStruct &firstWord
){
    if(words.size() < cursor + 1){ return ReturnType::CONTINUE; };

    if(words[cursor].type != TokenType::empty_tk && words[cursor].type != TokenType::close_brackets_tk){
        firstWord = words[cursor];
    }

    Expression.push_back(ConditionClass(""));

    string error;
    Expression.back().Location.source = strToSource(firstWord.value, error);

    switch(firstWord.type){
        case TokenType::bool_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 'b', scriptName, lineNumber)){
                return ReturnType::CONTINUE;
            };
            return ReturnType::OK;
        case TokenType::int_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ return ReturnType::CONTINUE; };
            return ReturnType::OK;
        case TokenType::double_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 'd', scriptName, lineNumber)){ return ReturnType::CONTINUE; };
            return ReturnType::OK;
        case TokenType::string_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber)){ return ReturnType::CONTINUE; };
            return ReturnType::OK;
        case TokenType::identifier_tk:
            return parseUserDefinedIdentifiers(words, cursor, Expression, lineNumber, scriptName, Scopes,
                NewLocalVariables, topAddress, canCreateNewVariable, firstWord
            );    
        default:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Word of the '"
                << tokenToStr(firstWord.type) << "' type is not valid.\n";
            return ReturnType::ERROR;
    }
}
ReturnType createExpression(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    vector<VariableModule> & resultStack, unsigned lineNumber, string scriptName, bool isConditionalExpression,
    ScopeType & Scopes, vector<VariableInfo> & NewLocalVariables, unsigned & topAddress,
    bool canCreateNewVariable
){
    if(cursor >= words.size()){
        return ReturnType::OK;
    }
    if(words[cursor].type == TokenType::empty_tk){
        cursor++;
        return ReturnType::OK;
    }

    TokenType scopeType = TokenType::end_expr_tk;
    if(isConditionalExpression){
        if(words[cursor].type != TokenType::start_expr_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Every expression must begin with parentheses.\n";
            return ReturnType::ERROR;
        }
    }
    else{
        scopeType = TokenType::close_brackets_tk;
        if(words[cursor].type != TokenType::open_brackets_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter " << cursor << " must begin with square brackets.\n";
            return ReturnType::ERROR;
        }
    }
    
    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
        return ReturnType::ERROR;
    }
    WordStruct firstWord;
    
    while(words[cursor].type != scopeType){
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
            return ReturnType::ERROR;
        }
        if(words[cursor].type == TokenType::keyword_tk && isLogicInstr(words[cursor].instruction)){
            if(!isConditionalExpression){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": Cannot use operators inside not conditional expression. Correct syntax: [[source_0] [source_1] ...]\n";
                return ReturnType::ERROR;
            }
            if(Expression.size() == 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": Operator '" << words[cursor].value << "' cannot be added to an empty expression.\n";
                return ReturnType::ERROR;
            }
            Expression.back().operators.emplace_back(words[cursor].instruction);
            cursor++;
        }
        else{
            ReturnType status = parseExpression(words, cursor, Expression, lineNumber, scriptName, Scopes,
                NewLocalVariables, topAddress, canCreateNewVariable, firstWord
            );
            if(status == ReturnType::CONTINUE){
                continue;
            }
            if(status == ReturnType::ERROR){
                return ReturnType::ERROR;
            }
        }
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
            return ReturnType::ERROR;
        }
    }
    cursor++;
    resultStack.reserve(Expression.size());
    for(unsigned i = 0; i < Expression.size(); ++i){
        resultStack.push_back(VariableModule());
    }
    return ReturnType::OK;
}
bool createEvent(const string & scriptName, const unsigned & lineNumber, const string & layerID,
    const string & objectID, vector <EventModule> & EventContainer, vector <string> & EventContainerIDs,
    EventModule & NewEvent, const vector<WordStruct> & words,
    ScopeType & Scopes, unsigned & topAddress
){
    string eventID = "";
    
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 1 parameter.\n";
        return true;
    }
    if(words[1].type != TokenType::identifier_tk && words[1].type != TokenType::empty_tk){
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
    ScopeType & Scopes
){
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires 1 parameter.\n";
        return true;
    }
    if(words[1].type != TokenType::identifier_tk && words[1].type != TokenType::empty_tk){
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
DataType instrToDataType(EngineInstr instruction){
    switch(instruction){
        case Val:
            return DataType::value_inst;
        case ValVec:
            return DataType::value_vec;
        case Pointer:
            return DataType::pointer_inst;
        case PointerVec:
            return DataType::pointer_vec;
        case Camera:
            return DataType::camera_inst;
        case CameraVec:
            return DataType::camera_vec;
        case Layer:
            return DataType::layer_inst;
        case LayerVec:
            return DataType::layer_vec;
        case Object:
            return DataType::object_inst;
        case ObjectVec:
            return DataType::object_vec;
        case Var:
            return DataType::variable_mod;
        case VarVec:
            return DataType::variable_mod_vec;
        case Vec:
            return DataType::vector_mod;
        case VecVec:
            return DataType::vector_mod_vec;
        case Text:
            return DataType::text_mod;
        case TextVec:
            return DataType::text_mod_vec;
        case EditText:
            return DataType::editable_text_mod;
        case EditTextVec:
            return DataType::editable_text_mod_vec;
        case SText:
            return DataType::super_text_mod;
        case STextVec:
            return DataType::super_text_mod_vec;
        case SEditText:
            return DataType::super_editable_text_mod;
        case SEditTextVec:
            return DataType::super_editable_text_mod_vec;
        case Image:
            return DataType::image_mod;
        case ImageVec:
            return DataType::image_mod_vec;
        case Movement:
            return DataType::movement_mod;
        case MovementVec:
            return DataType::movement_mod_vec;
        case Collision:
            return DataType::collision_mod;
        case CollisionVec:
            return DataType::collision_mod_vec;
        case Particles:
            return DataType::particles_mod;
        case Event:
            return DataType::event_mod;
        case EventVec:
            return DataType::event_mod_vec;
        case Scrollbar:
            return DataType::scrollbar_mod;
        case ScrollbarVec:
            return DataType::scrollbar_mod_vec;
        case Primitive:
            return DataType::primitives_mod;
        case PrimitiveVec:
            return DataType::primitives_mod_vec;
        case any:
            return DataType::any_dt;
        default:
            return DataType::null_dt;
    }
}

ReturnType InstrParser::parseCompilerBreakpoint(bool & triggerBreakpoint){
    triggerBreakpoint = true;
    cerr << "Warning: The 'compiler_breakpoint' instruction can be used only in the debugger.\n";
    return ReturnType::OK;
}
ReturnType InstrParser::parseStartAndOverride(vector<string> & allAvailableEventIDs, const string & layerId, const string & objectId,
    vector<EventModule> &eventContainer, vector<string> &eventContainerIds
){
    if(createEvent(scriptName, lineNumber, layerId, objectId, eventContainer, eventContainerIds, NewEvent, words, Scopes, topAddress))
        return ReturnType::ERROR;
    allAvailableEventIDs.clear();
    allAvailableEventIDs.push_back(NewEvent.getID());
    return ReturnType::OK;
}
ReturnType InstrParser::parseEnd(vector<EventModule> &eventContainer){
    if(NewEvent.isInline){
        NewEvent.Parameters.clear();
    }
    Scopes.pop_back(); //Remove the scope of the last event
    eventContainer.push_back(NewEvent);
    NewEvent = EventModule();
    return ReturnType::OK;

}
ReturnType InstrParser::parseTriggers(){
    size_t cursor = 1;
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Instruction \'" << words[0].value << "\' requires at least 2 parameters.\n";
        return ReturnType::ERROR;
    }
    while(cursor < words.size()){
        if(words[cursor].type != TokenType::identifier_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: Parameter " << cursor << " is not a context.\n";
            return ReturnType::ERROR;
        }
        if(words[cursor].type != TokenType::empty_tk){
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
    return ReturnType::OK;
}
ReturnType InstrParser::parseEmpty(){
    OperationClass * operation = nullptr;
    if(!prepareNewInstruction(words, NewEvent, operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    return ReturnType::OK;
}
ReturnType InstrParser::parseIf(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    Scopes.emplace_back(vector<VariableLocationStruct>());
    if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
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
    return ReturnType::OK;
}
ReturnType InstrParser::parseElseIf(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    Scopes.pop_back();
    Scopes.emplace_back(vector<VariableLocationStruct>());

    if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseElse(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseEndIf(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseWhile(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    Scopes.emplace_back(vector<VariableLocationStruct>());
    if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
        lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
    )){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return ReturnType::ERROR;
    }

    BranchingStack.whileStartStack.push_back(NewEvent.Operations.size()-1);
    BranchingStack.whileEndStack.push_back(vector<unsigned>());
    BranchingStack.whileEndStack.back().push_back(NewEvent.Operations.size()-1);
    return ReturnType::OK;
}
ReturnType InstrParser::parseEndWhile(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseContinue(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(BranchingStack.whileStartStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside a loop.\n";
        return ReturnType::ERROR;
    }

    NewEvent.Operations.back().jumpToLine = BranchingStack.whileStartStack.back();

    return ReturnType::OK;
}
ReturnType InstrParser::parseBreak(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(BranchingStack.whileEndStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside a loop.\n";
        return ReturnType::ERROR;
    }
    BranchingStack.whileEndStack.back().push_back(NewEvent.Operations.size() - 1);
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseFirstLastAllRandom(vector <string> & allAvailableEventIDs){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){
        return ReturnType::ERROR;
    }
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";

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
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return ReturnType::ERROR;
    }

    DataType outputType = null_dt;
    
    cursor = 2;
    if(Operation->Location.source == ValueSource::camera){
        if(optional(words, cursor, Operation->Location.cameraID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
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
            if(error.size() == 0){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(Operation->Location.source == ValueSource::layer){
        if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
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
            if(error.size() == 0){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(Operation->Location.source == ValueSource::context){
        if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
            lineNumber, scriptName, true, Scopes, NewEvent.LocalVariables, topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
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
            if(error.size() == 0){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Source '" << words[1].value << "' does not exist.\n";
        return ReturnType::ERROR;
    }
    return ReturnType::OK;
}
ReturnType InstrParser::parseIndex(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value
            << "' instruction: The first two parameters are not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";

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
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return ReturnType::ERROR;
    }
    
    cursor = 2;
    
    if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'i', "indexes", false, false
    )){ return ReturnType::ERROR; }

    if(optional(words, cursor, Operation->Location.attribute))
        return ReturnType::OK;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseIndexVec(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";

    if(words[1].value != ""){
        Operation->Location.source = ValueSource::context;
        if(Operation->addParameter(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
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
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseAddSubMulDivModPowRand(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(words[0].instruction == add){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'a', "left", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'a', "right", false, false, false
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
    return ReturnType::OK;
}
ReturnType InstrParser::parseAssert(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'a', "left", false, false, true
    )){ return ReturnType::ERROR; }
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'a', "right", false, false, true
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoad(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

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
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseMove(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "left", false, false, false
    )){ return ReturnType::ERROR; }

    if(words[0].instruction == move || words[0].instruction == add_move){
        if(Operation->addParameter(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
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
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseFindById2(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
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
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSumIntersecDiffIn(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

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
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseIncDecDelDemolishRbindType(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string parameterName = "context";
    if(words[0].instruction == demolish || words[0].instruction == rbind_i){
        parameterName = "objects";
    }

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', parameterName, false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;
    
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseNext(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'i', "number", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseAccess(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.size() > 0)
            return ReturnType::ERROR;
    }

    if(ReturnType::OK != createExpression(words, cursor, Operation->ConditionalChain, Operation->resultStack,
        lineNumber, scriptName, false, Scopes, NewEvent.LocalVariables, topAddress, false
    )){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseBoolIntDoubleStringAndTheirVectors(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    DataType newVariableType = value_inst;
    
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

    bool makeOutputGlobal = false;
    string error = "";

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, newVariableType,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, makeOutputGlobal, true
    )){
        if(error.size() > 0)
            return ReturnType::ERROR;
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseFindById(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";
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
    else if(words[1].value == "context" || words[1].value == "c" || words[1].type == TokenType::empty_tk){
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseClone(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseNew(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";
    Operation->Location.source = strToSource(words[1].value, error);
    
    if(error.size() > 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return ReturnType::ERROR;
    }
    
    //If the destination is provided as a variable, skip one parameter in the instruction.
    if(words[2].type == TokenType::identifier_tk){
        Operation->addLiteralParameter(VariableModule::newString("variable"));
    }
    else{
        Operation->addLiteralParameter(VariableModule::newString("location"));
    }
    
    cursor = 2;

    if(words[cursor].type == TokenType::identifier_tk){
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
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseBind(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "objects", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;
    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 's', "scripts", false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseBuild(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

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
    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadBuildInject(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'c', "objects", false, false
    )){ return ReturnType::ERROR; }

    string parameterName = "paths";
    if(words[0].instruction == inject_code){
        parameterName = "code";
    }
    else if(words[0].instruction == inject_instr){
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseFunction(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseEnv(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseEditProc(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    for(cursor = 2; cursor < words.size(); cursor++){
        if(words[cursor].type != TokenType::identifier_tk){
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

    string error = "";
    
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadBitmap(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseMkdirRmRmll(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR; 

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseMv(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "new_path", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parsePrint(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    switch(words[0].instruction){
        case print_v_i:
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
            break;
        case print_d_i:
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                1, 's', "delimeter", false, false, false
            )){ return ReturnType::ERROR; }
            cursor = 2;
            break;
        default:
            Operation->addEmptyParameter();
            break;
    }

    while(cursor < words.size()){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'a', "value", false, false, false
        )){ return ReturnType::ERROR; }
        cursor++;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadText(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseSaveText(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseLs(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseLse(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'b', "detail", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseNewProc(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseVar(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, variable_mod,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, true
    )){
        if(error.size() > 0){ return ReturnType::ERROR; }
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'a', "value", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseVec(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    cursor = 2;
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseTokenize(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseTreePwdConsoleInput(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLen(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseSize(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseSubstr(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadFont(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseCd(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSimilar(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseCount(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseCreateDisplay(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 5, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseStartTimer(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "name", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseStopTimer(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

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

    return ReturnType::OK;
}
ReturnType InstrParser::parseVarDefinition(){
    if(!prepareNewVariableDeclaration(words, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    auto[localAddress, result] = getLocalAddress(
        words[1].value, instrToDataType(words[0].instruction), Scopes, NewEvent.LocalVariables,
        topAddress, true, false, false, true
    );

    if(result != ReturnType::OK){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Variable declaration failed.\n";
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseRun(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    if(words[1].type != TokenType::identifier_tk){
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

    return ReturnType::OK;
}
ReturnType InstrParser::parseAutoRun(){
    if(words.size() >= 3 && words[1].type == TokenType::start_expr_tk && words.back().type == TokenType::end_expr_tk){
        const vector<WordStruct> runToken = {WordStruct(TokenType::keyword_tk, "run", false)};
        if(!prepareNewInstruction(runToken, NewEvent, Operation, 1, lineNumber, scriptName))
            return ReturnType::ERROR;
        
        if(words[0].type != TokenType::identifier_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the 'auto_run' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        Operation->specialValue = NewEvent.Children.size();
        NewEvent.Children.emplace_back(ChildStruct(words[0].value, vector<PassingVariableInfo>(), 0, scriptName, lineNumber));
        cursor = 1;

        if(NewEvent.getPassingVariables(NewEvent.Children.back().Arguments,
            words, cursor, lineNumber, scriptName, Scopes, topAddress
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' does not exist.\n";
    }
    return ReturnType::OK;
}

ReturnType AncestorObject::parseTokensAndAssembleEvents(const vector<WordStruct> & words, const string & scriptName,
    unsigned lineNumber, ScopeType & Scopes, unsigned & topAddress, bool & triggerBreakpoint,
    EventModule & NewEvent, vector<string> & allAvailableEventIDs, BranchingStackStruct & BranchingStack
){
    /*unsigned cursor = 1;
    string error;
    OperationClass * Operation = nullptr;
    const vector<WordStruct> runToken = {WordStruct(TokenType::keyword_tk, "run", false)};*/

    switch(words[0].instruction){
        case EngineInstr::import:
        case EngineInstr::start:
        case EngineInstr::override:
            break;
        default:
            if(Scopes.size() < 2){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ":\n"
                    << NEW_LINE_PADDING << "Instruction \'" << words[0].value << "\' cannot be used outside of an event scope.\n";
                return ReturnType::ERROR;
            }
            break;
    }

    InstrParser instrParser(words, scriptName, lineNumber, NewEvent, Scopes, topAddress);

    switch(words[0].instruction){
        case import:
            return ReturnType::OK;
        case compiler_breakpoint:
            return instrParser.parseCompilerBreakpoint(triggerBreakpoint);
        case start:
        case override:
            return instrParser.parseStartAndOverride(allAvailableEventIDs, layerID, ID, EventContainer, eventContainerIDs);
        case end_i:
            return instrParser.parseEnd(EventContainer);
        case triggers:
            return instrParser.parseTriggers();
        case if_i:
            return instrParser.parseIf(BranchingStack);
        case else_if:
            return instrParser.parseElseIf(BranchingStack);
        case else_i:
            return instrParser.parseElse(BranchingStack);
        case end_if:
            return instrParser.parseEndIf(BranchingStack);
        case while_i:
            return instrParser.parseWhile(BranchingStack);
        case end_while:
            return instrParser.parseEndWhile(BranchingStack);
        case continue_i:
            return instrParser.parseContinue(BranchingStack);
        case break_i:
            return instrParser.parseBreak(BranchingStack);
        case return_i:
        case reboot:
        case exit_i:
        case delete_this_event:
        case reset_keyboard:
        case dump_context_stack:
        case dump_memory:
        case dump_local_memory:
        case restart_drag:
        case breakpoint:
            return instrParser.parseEmpty();
        case first:
        case last:
        case all:
        case random_i:
            return instrParser.parseFirstLastAllRandom(allAvailableEventIDs);
        case index_i:
            return instrParser.parseIndex();
        case index_vec:
            return instrParser.parseIndexVec();
        case add:
        case sub:
        case mul:
        case div_i:
        case mod:
        case pow_i:
        case rand_int:
            return instrParser.parseAddSubMulDivModPowRand();
        case assert:
            return instrParser.parseAssert();
        case load_i:
            return instrParser.parseLoad();
        case move:
        case add_move:
        case sub_move:
        case mul_move:
        case div_move:
            return instrParser.parseMove();
        case find_by_id_2:
            return instrParser.parseFindById2();
        case sum:
        case intersection:
        case diff:
        case in:
            return instrParser.parseSumIntersecDiffIn();
        case inc:
        case dec:
        case del:
        case demolish:
        case rbind_i:
        case type:
            return instrParser.parseIncDecDelDemolishRbindType();
        case next:
            return instrParser.parseNext();
        case access_i:
            return instrParser.parseAccess();
        case bool_i:
        case int_i:
        case double_i:
        case string_i:
        case bool_vec_i:
        case int_vec_i:
        case double_vec_i:
        case string_vec_i:
            return instrParser.parseBoolIntDoubleStringAndTheirVectors();
        case find_by_id:
            return instrParser.parseFindById();
        case clone_i:
            return instrParser.parseClone();
        case new_i:
            return instrParser.parseNew();
        case bind_i:
            return instrParser.parseBind();
        case build:
            return instrParser.parseBuild();
        case load_build:
        case build_subset:
        case inject_code:
        case inject_instr:
            return instrParser.parseLoadBuildInject();
        case fun:
            return instrParser.parseFunction();
        case env:
            return instrParser.parseEnv();
        case edit_proc:
            return instrParser.parseEditProc();
        case load_bitmap:
            return instrParser.parseLoadBitmap();
        case mkdir_i:
        case rm:
        case rmll:
            return instrParser.parseMkdirRmRmll();
        case mv_i:
            return instrParser.parseMv();
        case print_v_i:
        case print_d_i:
        case print_i:
            return instrParser.parsePrint();
        case load_text:
            return instrParser.parseLoadText();
        case save_text:
            return instrParser.parseSaveText();
        case ls:
            return instrParser.parseLs();
        case lse:
            return instrParser.parseLse();
        case new_proc:
            return instrParser.parseNewProc();
        case var:
            return instrParser.parseVar();
        case vec:
            return instrParser.parseVec();
        case tokenize:
            return instrParser.parseTokenize();
        case tree:
        case pwd:
        case console_input:
            return instrParser.parseTreePwdConsoleInput();
        case len:
            return instrParser.parseLen();
        case size_i:
            return instrParser.parseSize();
        case substr:
            return instrParser.parseSubstr();
        case load_font:
            return instrParser.parseLoadFont();
        case cd:
            return instrParser.parseCd();
        case similar:
            return instrParser.parseSimilar();
        case count:
            return instrParser.parseCount();
        case create_display:
            return instrParser.parseCreateDisplay();
        case start_timer:
            return instrParser.parseStartTimer();
        case stop_timer:
            return instrParser.parseStopTimer();
        case Val:
        case ValVec:
        case Pointer:
        case PointerVec:
        case Camera:
        case CameraVec:
        case Layer:
        case LayerVec:
        case Object:
        case ObjectVec:
        case Var:
        case VarVec:
        case Vec:
        case VecVec:
        case Text:
        case TextVec:
        case EditText:
        case EditTextVec:
        case SText:
        case STextVec:
        case SEditText:
        case SEditTextVec:
        case Image:
        case ImageVec:
        case Movement:
        case MovementVec:
        case Collision:
        case CollisionVec:
        case Particles:
        case Event:
        case EventVec:
        case Scrollbar:
        case ScrollbarVec:
        case Primitive:
        case PrimitiveVec:
        case any:
            return instrParser.parseVarDefinition();
        case run:
            return instrParser.parseRun();
        default:
            return instrParser.parseAutoRun();
    }
}
vector<vector<WordStruct>> CodeGenerator::preprocessTokens(const vector<WordStruct> & inputTokens){
    if(inputTokens.size() == 0){
        return vector<vector<WordStruct>>{};
    }

    vector<vector<WordStruct>> generatedCode;

    vector<WordStruct> modifiedInput;

    /*size_t cursor = 0;
    //Find start_expr_tk
    for(cursor = 0; cursor < inputTokens.size(); ++cursor){

    }

    switch(inputTokens[0].instruction){

        default:
            generatedCode.push_back(inputTokens);
            return generatedCode;
    }*/

    generatedCode.push_back(inputTokens);

    return generatedCode;
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

    vector<WordStruct> words;
    EventModule NewEvent = EventModule();
    unsigned lineNumber = 0;
    bool triggerBreakpoint = false;
    bool assembleEvents = true;
    bool printTokens = false;

    vector<string> allAvailableEventIDs;

    ScopeType Scopes;
    Scopes.push_back(GlobalScope);

    BranchingStackStruct BranchingStack;
    CodeGenerator codeGenerator;

    for(string line : code){
        lineNumber++;
        words.clear();
        line = findAndUseSpecialCharacters(line);

        bool triggerPreprocessor = false;
        
        std::tie(words, triggerPreprocessor) = tokenizeCode(line);

        if(words.size() == 0){
            continue;
        }

        if(triggerBreakpoint){
            raise(SIGINT);
            triggerBreakpoint = false;
        }

        //Compilation-time instructions
        bool resumeLoop = false;
        switch(words[0].instruction){
            case EngineInstr::resume_assembling:
                assembleEvents = true;
                break;
            case EngineInstr::stop_assembling:
                assembleEvents = false;
                break;
            case EngineInstr::start_printing_tokens:
                printTokens = true;
                break;
            case EngineInstr::stop_printing_tokens:
                printTokens = false;
                break;
            default:
                resumeLoop = true;
                break;
        }

        if(printTokens){
            for(const WordStruct & word : words){
                if(word.negateVariable){
                    cout << "neg ";
                }
                cout << "[" << tokenToStr(word.type) << ", " << instrToStr(word.instruction) << ", \"" << word.value << "\"] ";
            }
            cout << "\n";
            cout.flush();
        }

        if(!assembleEvents || !resumeLoop){
            continue;
        }

        if(triggerPreprocessor){
            vector<vector<WordStruct>> preprocessedWords = codeGenerator.preprocessTokens(words);
            for(auto finalWords : preprocessedWords){
                ReturnType result = parseTokensAndAssembleEvents(
                    finalWords, scriptName, lineNumber, Scopes, topMemoryAddress, triggerBreakpoint,
                    NewEvent, allAvailableEventIDs, BranchingStack
                );
                if(result == ReturnType::ERROR){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Compilation aborted due to previous errors.\n";
                    return ReturnType::ERROR;
                }
            }
        }
        else{
            ReturnType result = parseTokensAndAssembleEvents(
                words, scriptName, lineNumber, Scopes, topMemoryAddress, triggerBreakpoint,
                NewEvent, allAvailableEventIDs, BranchingStack
            );
            if(result == ReturnType::ERROR){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Compilation aborted due to previous errors.\n";
                return ReturnType::ERROR;
            }
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
    eventContainerIDs.clear();
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
ReturnType gatherImportsFromScript(const string & exePath, const string & scriptName, vector<string> & allImports){
    allImports.clear();
    vector<string> scriptLines = readLines(scriptName, false);
    if(scriptLines.size() == 0){
        return ReturnType::EMPTY;
    }
    vector<WordStruct> words;
    for(const string & line : scriptLines){
        bool triggerPreprocessor = false;
        std::tie(words, triggerPreprocessor) = tokenizeCode(line); //TODO: Use less expensive way to extract imports from lines
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
            allImports.push_back(exePath + words[1].value);
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
ReturnType addImportsToBindedScripts(const string & exePath, const vector<string> & bindedScripts, vector<string> & allScriptsToAssemble){
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
        importResult = gatherImportsFromScript(exePath, currentScript, subsequentImports);
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
void AncestorObject::translateAllScripts(const string & exePath, bool clearEvents, bool allowNotAscii, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress){
    if(clearEvents){
        clearAllEvents();
    }

    if(bindedScripts.size() == 0){
        printBasicFile();
        return;
    }

    removeStringDuplicatesFromVector(bindedScripts);
    vector<string> allScriptsToAssemble;
    ReturnType status = addImportsToBindedScripts(exePath, bindedScripts, allScriptsToAssemble);
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
void AncestorObject::translateScriptsFromPaths(const string & exePath, bool clearEvents, vector<string> scriptsPaths, bool allowNotAscii,
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
){
    if(clearEvents){
        clearAllEvents();
    }

    removeStringDuplicatesFromVector(scriptsPaths);
    vector<string> allScriptsToAssemble;
    ReturnType status = addImportsToBindedScripts(exePath, scriptsPaths, allScriptsToAssemble);
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
void AncestorObject::translateSubsetBindedScripts(const string & exePath, bool clearEvents, vector<string> scripts, bool allowNotAscii,
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
    ReturnType status = addImportsToBindedScripts(exePath, selectedBindedScripts, allScriptsToAssemble);
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