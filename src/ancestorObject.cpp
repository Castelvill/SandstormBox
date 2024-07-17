#include "ancestorObject.h"
#include <regex>

vector<string> splitSpringReturnVector(const string & str, const string & delimiter, bool reAdd){
    vector<string> strings;
    string::size_type pos = 0;
    string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }
    if(reAdd){
        strings.push_back(delimiter + str.substr(prev));
    }
    else{
        strings.push_back(str.substr(prev));
    }
    return strings;
}
auto splitStringReturnPair(const string & str, const string & delimiter, bool reAdd){
    struct result{
        string left;
        string right;
    };
    string::size_type pos = 0;
    string::size_type prev = 0;
    string left, right;
    while ((pos = str.find(delimiter, prev)) != string::npos){
        left += str.substr(prev, pos - prev);
        prev = pos + delimiter.size();
    }
    if(reAdd && prev != 0){
        right = delimiter + str.substr(prev);
    }
    else{
        right = str.substr(prev);
    }
    return result{left, right};
}
vector<string> removeComments(const vector<string> & lines){
    vector<string> newLines;
    unsigned cursor;
    bool commentSection = false;
    unsigned lineNumber = 1;
    for(const string & line : lines){
        newLines.push_back("");
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
vector<string> readLines(const string& filename, bool allowNotAscii) {
	std::ifstream File(filename);
	vector<string> lines;

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
            lines.push_back(removeNotAscii(line));
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
    for(EveModule & Event : EveContainer){
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
        cout << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << Original.ID << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": Object with a reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    clearVectorsOfIDs();
    clear();
    PrimaryModule::clone(Original, listOfUniqueIDs, newLayerID, "", changeOldID);
    for(const TextModule & Text : Original.TextContainer){
        TextContainer.push_back(TextModule());
        TextContainer.back().clone(Text, textContainerIDs, newLayerID, getID(), true);
    }
    for(const EditableTextModule & Editable : Original.EditableTextContainer){
        EditableTextContainer.push_back(EditableTextModule());
        EditableTextContainer.back().clone(Editable, editableTextContainerIDs, newLayerID, getID(), true);
    }
    for(const SuperTextModule & SuperText : Original.SuperTextContainer){
        SuperTextContainer.push_back(SuperTextModule());
        SuperTextContainer.back().clone(SuperText, superTextContainerIDs, newLayerID, getID(), true);
    }
    for(const SuperEditableTextModule & SuperEditableText : Original.SuperEditableTextContainer){
        SuperEditableTextContainer.push_back(SuperEditableTextModule());
        SuperEditableTextContainer.back().clone(SuperEditableText, superEditableTextContainerIDs, newLayerID, getID(), true);
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
    for(const VariableModule & Variable : Original.VariablesContainer){
        VariablesContainer.push_back(VariableModule());
        VariablesContainer.back().clone(Variable, variablesContainerIDs, newLayerID, getID(), true);
    }
    for(const ScrollbarModule & Scrollbar : Original.ScrollbarContainer){
        ScrollbarContainer.push_back(ScrollbarModule());
        ScrollbarContainer.back().clone(Scrollbar, scrollbarContainerIDs, newLayerID, getID(), true);
    }
    for(const PrimitivesModule & Primitives : Original.PrimitivesContainer){
        PrimitivesContainer.push_back(PrimitivesModule());
        PrimitivesContainer.back().clone(Primitives, primitivesContainerIDs, newLayerID, getID(), true);
    }
    for(const VectorModule & Vector : Original.VectorContainer){
        VectorContainer.push_back(VectorModule());
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
    eveContainerIDs.clear();
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
    for(EveModule & Event : EveContainer){
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
        textContainerIDs.push_back(content.getID());
    }
    for(const EditableTextModule & content : EditableTextContainer){
        editableTextContainerIDs.push_back(content.getID());
    }
    for(const SuperTextModule & content : SuperTextContainer){
        superTextContainerIDs.push_back(content.getID());
    }
    for(const SuperEditableTextModule & content : SuperEditableTextContainer){
        superEditableTextContainerIDs.push_back(content.getID());
    }
    for(const ImageModule & content : ImageContainer){
        imageContainerIDs.push_back(content.getID());
    }
    for(const MovementModule & content : MovementContainer){
        movementContainerIDs.push_back(content.getID());
    }
    for(const CollisionModule & content : CollisionContainer){
        collisionContainerIDs.push_back(content.getID());
    }
    for(const ParticleEffectModule & content : ParticlesContainer){
        particlesContainerIDs.push_back(content.getID());
    }
    for(const EveModule & content : EveContainer){
        eveContainerIDs.push_back(content.getID());
    }
    for(const VariableModule & content : VariablesContainer){
        variablesContainerIDs.push_back(content.getID());
    }
    for(const ScrollbarModule & content : ScrollbarContainer){
        scrollbarContainerIDs.push_back(content.getID());
    }
    for(const PrimitivesModule & content : PrimitivesContainer){
        primitivesContainerIDs.push_back(content.getID());
    }
    for(const VectorModule & content : VectorContainer){
        vectorContainerIDs.push_back(content.getID());
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

VariableModule AncestorObject::getAttributeValue(const string &attribute, const string & detail){
    VariableModule NewValue;

    if(attribute == "is_active"){
        NewValue.setBool(getIsActive());
    }
    else if(attribute == "id"){
        NewValue.setString(getID());
    }
    else if(attribute == "layer_id"){
        NewValue.setString(getLayerID());
    }
    else if(attribute == "in_group"){
        NewValue.setBool(isInAGroup(detail));
    }
    else if(attribute == "pos_x"){
        NewValue.setDouble(getPos().x);
    }
    else if(attribute == "pos_y"){
        NewValue.setDouble(getPos().y);
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
    else if(attribute == "can_draw_selection_border"){
        NewValue.setBool(canDrawSelectionBorder);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Attribute '" << attribute << "' is not valid.\n";
        NewValue.setBool(false);
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

#include <signal.h>

vector<WordStruct> tokenizeCode(string input){
    std::regex word_regex("([\\w+\\.*]*\\w+)|;|:|\\,|\\.|==|=|>=|<=|>|<|-=|\\+=|/=|\\*=|/=|\\*\\*|\\+\\+|\\-\\-|\\+|-|\\*|/|%|\\[|\\]|\\(|\\\\\\\"|\\)|\"|!=|!|\\|\\||&&|\n|\t|@|#", std::regex_constants::icase);
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
                stringSectors.push_back("");
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
            mergedOutput.push_back(WordStruct('s', stringSectors[sector]));
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
            mergedOutput.push_back(WordStruct('o', output[i])); //operation
            continue;
        }
        if(output[i] == "_"){
            mergedOutput.push_back(WordStruct('e', output[i])); //empty / null
            continue;
        }
        if(canStringBeDouble(output[i])){
            cstod(output[i], error);
            if(mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.push_back(WordStruct('d', "-" + output[i])); //double
            }
            else{
                mergedOutput.push_back(WordStruct('d', output[i])); //double
            }
            continue;
        }
        cstoi(output[i], error);
        if(error == ""){
            if(mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.push_back(WordStruct('i', "-" + output[i])); //int
            }
            else{
                mergedOutput.push_back(WordStruct('i', output[i])); //int
            }
            
            continue;
        }
        mergedOutput.push_back(WordStruct('c', output[i]));
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
bool prepareNewInstruction(vector<WordStruct> words, EveModule & NewEvent, OperaClass *& Operation, bool postOperations, unsigned minLength, unsigned lineNumber, string scriptName){
    if(words.size() < minLength){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least \'" << minLength << "\' parameters.\n";
        return false;
    }
    if(!postOperations){
        NewEvent.DependentOperations.push_back(OperaClass());
        NewEvent.DependentOperations.back().instruction = transInstr(words[0].value);
        Operation = &NewEvent.DependentOperations.back();
    }
    else{
        NewEvent.PostOperations.push_back(OperaClass());
        NewEvent.PostOperations.back().instruction = transInstr(words[0].value);
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
bool optional(const vector<WordStruct> & words, unsigned & cursor, vector<string> & strVec){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        strVec.push_back(words[cursor].value);
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
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, VariableModule & variable, const char & type, string scriptName, unsigned lineNumber){
    string error = "";
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != 'e'){
        if(type == 'b'){
            variable.setBool(cstoi(words[cursor].value, error));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
            }
        }
        else if(type == 'i'){
            variable.setInt(cstoi(words[cursor].value, error));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
            }
        }
        else if(type == 'd'){
            variable.setDouble(cstod(words[cursor].value, error));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
            }
        }
        else if(type == 's'){
            if(words[cursor].type == 's'){
                variable.setString(words[cursor].value);
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Parameter nr." << cursor+1 << " is not a string.\n";
            }
        }
        else if(type == 'c'){
            if(words[cursor].type == 'c'){
                variable.setString(words[cursor].value);
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Parameter nr." << cursor+1 << " is not a context.\n";
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
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
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
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
        }
    }
    if(words[cursor].value != "]"){
        cursor++;
    }
    return false;
}
bool createExpression(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression, unsigned lineNumber, string scriptName){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return true;
    }
    if(words[cursor].value != "("){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Every expression must begin with a parentheses.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    bool inCondition = false;
    WordStruct firstWord;
    while(words[cursor].value != ")"){
        if(cursor >= words.size()){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
        if(words[cursor].value == "["){
            Expression.push_back(ConditionClass(""));
            inCondition = true;
            cursor++;
        }
        else if(words[cursor].value == "]"){
            inCondition = false;
            cursor++;
        }
        else if(!inCondition){
            if(Expression.size() == 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Operator cannot be added to an empty expression.\n";
                return false;
            }
            Expression.back().operators.push_back(transInstr(words[cursor].value));
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

            Expression.back().Location.source = firstWord.value;

            if(firstWord.type == 'b'){
                Expression.back().Location.source = "literal";
                if(nextCond(words, cursor, Expression.back().Literal, 'b', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 'i'){
                Expression.back().Location.source = "literal";
                if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 'd'){
                Expression.back().Location.source = "literal";
                if(nextCond(words, cursor, Expression.back().Literal, 'd', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 's'){
                Expression.back().Location.source = "literal";
                if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber)){ continue; };
            }
            else if(firstWord.type == 'c'){
                if(words[cursor].value != "]"){
                    cursor++;
                }
                if(isStringInGroup(firstWord.value, 6, "key_pressed", "key_pressing",
                    "key_released", "mouse_pressed", "mouse_pressing", "mouse_released")
                ){
                    if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ continue; };
                }
                else if(firstWord.value == "exists" || firstWord.value == "is_directory"){
                    if(words[cursor].value == "]"){
                        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber
                            << ": In " << __FUNCTION__ << ": Source '" << firstWord.value
                            << "' requires one string literal.\n";
                        return false;
                    }
                    if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber)){ continue; };
                }
                else if(isStringInGroup(firstWord.value, 19, "on_boot", "second_passed", "any_key_pressed",
                    "any_key_pressing", "any_key_released", "mouse_x", "mouse_y", "mouse_moved", "screen_w",
                    "screen_h", "window_w", "window_h", "fullscreen", "on_display_resize", "used_os",
                    "number_of_processes", "number_of_cameras", "number_of_layers", "number_of_objects")
                ){
                    continue;
                }
                else if(firstWord.value == "vector" || firstWord.value == "v"){
                    if(nextCond(words, cursor, Expression.back().Location.moduleID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    if(Expression.back().Location.attribute == "i" || Expression.back().Location.attribute == "index"){
                        if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ continue; };
                        Expression.back().Location.attribute = "value";
                    }
                    else if(Expression.back().Location.attribute == "c" || Expression.back().Location.attribute == "context"){
                        if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber)){ continue; };
                        Expression.back().Location.attribute = "value";
                    }
                    else if(Expression.back().Location.attribute == "s" || Expression.back().Location.attribute == "size"){
                        Expression.back().Location.attribute = "size";
                    }
                    else if(Expression.back().Location.attribute == "b" || Expression.back().Location.attribute == "back"){
                        Expression.back().Location.attribute = "back";
                    }
                    else{
                        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber
                            << ": In " << __FUNCTION__ << ": Invalid attribute.\n";
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
                    if(Expression.back().Location.attribute == "in_group"){
                        if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber)){ continue; };
                    }
                }
                else if(firstWord.value == "object"){
                    if(nextCond(words, cursor, Expression.back().Location.layerID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.objectID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.moduleType, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.moduleID, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    if(isStringInGroup(Expression.back().Location.moduleType, 3, "ancestor", "text", "editable_text")){
                        if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber)){ continue; };
                    }
                    else if(Expression.back().Location.moduleType == "mouse"){
                        if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ continue; };
                    }
                    else if(Expression.back().Location.moduleType == "collision"){
                        if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ continue; };
                        if(nextCond(words, cursor, Expression.back().Location.spareID, scriptName, lineNumber)){ continue; };
                    }
                }
                else if(firstWord.value == "variable"){
                    if(nextCond(words, cursor, Expression.back().Location.moduleID, scriptName, lineNumber)){ continue; };
                }
                else{
                    cursor--;
                    Expression.back().Location.source = "context";
                    if(nextCond(words, cursor, Expression.back().Literal, 'c', scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.attribute, scriptName, lineNumber)){ continue; };
                    if(nextCond(words, cursor, Expression.back().Location.spareID, scriptName, lineNumber)){ continue; };
                }
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber
                    << ": In " << __FUNCTION__ << ": Word of the '" << firstWord.type << "' type is not valid.\n";
                return false;
            }
        }
        if(cursor >= words.size()){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherContextVector(const vector<WordStruct> & words, unsigned & cursor, vector<string> & stringVector, unsigned lineNumber, string scriptName){
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return true;
    }
    if(words[cursor].value != "["){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Every list of strings must begin with a square bracket.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    while(words[cursor].value != "]"){
        if(words[cursor].type != 'c'){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Parameter nr." << cursor+1 << " is not a string.\n";
            return false;
        }
        stringVector.push_back(words[cursor].value);
        cursor++;
        if(cursor >= words.size()){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherArgumentVector(const vector<WordStruct> & words, unsigned & cursor, vector<string> & Variables, vector<VariableModule> & Literals, unsigned lineNumber, string scriptName){
    bool negate = false;
    string error;
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return true;
    }
    if(words[cursor].value != "["){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Every list of strings must begin with a square bracket.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    while(words[cursor].value != "]"){
        if(words[cursor].value == "-"){
            negate = true;
        }
        else if(words[cursor].type == 'c'){
            Variables.push_back(words[cursor].value);
        }
        else if(words[cursor].type == 'b'){
            Literals.push_back(VariableModule::newBool(cstoi(words[cursor].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                return false;
            }
            if(negate){
                negate = false;
                Literals.back().toggleBool();
            }
        }
        else if(words[cursor].type == 'i'){
            Literals.push_back(VariableModule::newInt(cstoi(words[cursor].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                return false;
            }
            if(negate){
                negate = false;
                Literals.back().negate();
            }
        }
        else if(words[cursor].type == 'd'){
            Literals.push_back(VariableModule::newDouble(cstod(words[cursor].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                return false;
            }
            if(negate){
                negate = false;
                Literals.back().negate();
            }
        }
        else if(words[cursor].type == 's'){
            Literals.push_back(VariableModule::newString(words[cursor].value));
        }
        else{
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                << ": Parameter nr." << cursor+1 << " is not of a valid type: '" << words[cursor].type << "'.\n";
            return false;
        }
        
        cursor++;
        if(cursor >= words.size()){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherLiterals(const vector<WordStruct> & words, unsigned & cursor, vector<VariableModule> & Literals, char type, unsigned lineNumber, string scriptName){
    bool negate = false;
    string error;
    if(cursor >= words.size()){
        return true;
    }
    if(words[cursor].type == 'e'){
        cursor++;
        return true;
    }
    if(words[cursor].value != "["){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Every list of literals must begin with a square bracket.\n";
        return false;
    }
    cursor++;
    if(cursor >= words.size()){
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
        return false;
    }
    while(words[cursor].value != "]"){
        if(words[cursor].value == "-" && type != 's'){
            negate = true;
        }
        else if(type == 'b'){
            Literals.push_back(VariableModule::newBool(cstoi(words[cursor].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                return false;
            }
            if(negate){
                negate = false;
                Literals.back().toggleBool();
            }
        }
        else if(type == 'i'){
            Literals.push_back(VariableModule::newInt(cstoi(words[cursor].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                return false;
            }
            if(negate){
                negate = false;
                Literals.back().negate();
            }
        }
        else if(type == 'd'){
            Literals.push_back(VariableModule::newDouble(cstod(words[cursor].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                return false;
            }
            if(negate){
                negate = false;
                Literals.back().negate();
            }
        }
        else{
            if(words[cursor].type == 's'){
                Literals.push_back(VariableModule::newString(words[cursor].value));
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": Parameter '" << words[cursor].value << "' is not a string.\n";
                return false;
            }
            if(negate){
                negate = false;
            }
        }
        cursor++;
        if(cursor >= words.size()){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Command is too short.\n";
            return false;
        }
    }
    cursor++;
    return true;
}
bool gatherLiterals(const vector<WordStruct> & words, unsigned & cursor, vector<VariableModule> & Literals, const string & type, unsigned lineNumber, string scriptName){
    if(type == "bool" || type == "b"){
        if(!gatherLiterals(words, cursor, Literals, 'b', lineNumber, scriptName)){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else if(type == "int" || type == "i"){
        if(!gatherLiterals(words, cursor, Literals, 'i', lineNumber, scriptName)){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else if(type == "double" || type == "d"){
        if(!gatherLiterals(words, cursor, Literals, 'd', lineNumber, scriptName)){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else if(type == "string" || type == "s"){
        if(!gatherLiterals(words, cursor, Literals, 's', lineNumber, scriptName)){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": \'" << type << "\' literal creation failed.\n";
            return false;
        }
    }
    else{
        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": \'" << type << "\' type does not exist.\n";
        return false;
    }
    return true;
}
void AncestorObject::eventAssembler(vector<string> code, string scriptName){
    vector<WordStruct> words;
    EveModule NewEvent = EveModule();
    unsigned cursor = 0, lineNumber = 0;
    OperaClass * Operation;
    bool postOperations = false;

    bool stringSection = false;

    vector<string> code2 = {""};

    //merge string sections
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
            code2.push_back("");
        }
        else{
            code2.back() += '\n';
        }
    }

    code = code2;

    string error;
    bool triggerBreakpoint = false;
    
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
        if(words[0].value == "breakpoint"){
            triggerBreakpoint = true;
            cout << "Warning: The 'breakpoint' instruction can be used only in the debugger.\n";
        }
        else if(words[0].value == "start"){
            if(words.size() < 2){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber <<
                    ": In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires one parameter.\n";
                return;
            }

            bool overrideEvent = false;

            if(words.size() >= 4){
                if(words[3].type == 'i'){
                    overrideEvent = cstoi(words[3].value, error);
                }
                else{
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                }
            }

            if(words[1].type != 'c' && words[1].type != 'e'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": In the 'start' instruction: The second parameter is not a context.\n";
                return;
            }

            if(words[1].value[0] != '_' && isStringInVector(eveContainerIDs, words[1].value)){
                if(overrideEvent){
                    removeFromStringVector(eveContainerIDs, words[1].value);
                    removeModuleInstanceByID(EveContainer, words[1].value);
                }
                else{
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber <<
                        ": In " << __FUNCTION__ << ": Event with id \'" << words[1].value << "\' already exists.\n";
                    return;
                }
            }
            NewEvent = EveModule(words[1].value, &eveContainerIDs, getLayerID(), getID());

            if(words.size() == 2 || words[2].type == '_'){
                continue;
            }

            if(words[2].type != 'i' && words[2].type != 'e'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": The second parameter is not an int.\n";
                return;
            }
            if(words[2].type != 'e'){
                NewEvent.loop = cstoi(words[2].value, error);
            }
        }
        else if(words[0].value == "end"){
            EveContainer.push_back(NewEvent);
            NewEvent = EveModule();
            postOperations = false;
        }
        else if(words[0].value == "post"){
            postOperations = true;
        }
        else if(words[0].value == "triggers"){
            if(words.size() < 2){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires at least 2 parameters.\n";
                return;
            }
            cursor = 1;
            while(cursor < words.size()){
                if(words[cursor].type != 'c'){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: Parameter nr. " << cursor+1 << " is not a context.\n";
                    return;
                }
                if(words[cursor].type != 'e'){
                    NewEvent.primaryTriggerTypes.push_back(words[cursor].value);
                }
                cursor++;
            }
            /*if(!gatherStringVector(words, cursor, NewEvent.primaryTriggerTypes, lineNumber, scriptName)){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Context gather failed.\n";
                return;
            }*/
        }
        else if(words[0].value == "run"){
            if(words.size() < 2){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires 2 parameters.\n";
                return;
            }
            cursor = 1;
            while(cursor < words.size()){
                if(words[cursor].type != 'c'){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: Parameter nr. " << cursor+1 << " is not a context.\n";
                    return;
                }
                if(words[cursor].type != 'e'){
                    NewEvent.Children.push_back(ChildStruct(words[cursor].value, false));
                }
                cursor++;
            }
        }
        else if(words[0].value == "if"){
            cursor = 1;
            if(!createExpression(words, cursor, NewEvent.ConditionalChain, lineNumber, scriptName)){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Expression creation failed.\n";
                return;
            }
        }
        else if(words[0].value == "else"){
            if(words.size() < 2){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires one parameter of a string type.\n";
                return;
            }
            if(words[1].type == 'c'){
                NewEvent.elseChildID = words[1].value;
            }
            else if(words[1].type != 'e'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": Instruction \'" << words[0].value << "\' requires one parameter of a string type.\n";
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 9, "continue", "break", "return", "reboot", "power_off", "delete_this_event", "reset_keyboard", "dump_context_stack", "restart_drag")){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1, lineNumber, scriptName)){
                return;
            }
        }
        else if(isStringInGroup(words[0].value, 4, "first", "last", "all", "random")){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' && words[1].type != 'e'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            if(words[1].type != 'e'){
                Operation->Location.source = words[1].value;
            }
            
            cursor = 2;
            if(words[1].value == "camera"){
                if(optional(words, cursor, Operation->Location.cameraID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(!createExpression(words, cursor, Operation->ConditionalChain, lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Expression creation failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1].value == "layer"){
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(!createExpression(words, cursor, Operation->ConditionalChain, lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Expression creation failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1].value == "context" || words[1].value == "c" || words[1].type == 'e'){
                if(!gatherContextVector(words, cursor, Operation->dynamicIDs, lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Context gather failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(!createExpression(words, cursor, Operation->ConditionalChain, lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Expression creation failed.\n";
                    return;
                }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": Source '" << words[1].value << "' does not exist.\n";
                return;
            }
        }
        else if(words[0].value == "index"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }
            if(words[1].value == "Processes"){
                Operation->Location.source = "process";
            }
            else if(words[1].value == "Layers"){
                Operation->Location.source = "layer";
            }
            else if(words[1].value == "Cameras"){
                Operation->Location.source = "camera";
            }
            else{
                Operation->Location.source = "context";
                Operation->dynamicIDs.push_back(words[1].value);
            }
            cursor = 2;
            if(!gatherArgumentVector(words, cursor, Operation->dynamicIDs, Operation->Literals, lineNumber, scriptName)){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Creation of an argument vector failed.\n";
                return;
            }
            if(optional(words, cursor, Operation->Location.attribute)){ continue; }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(isStringInGroup(words[0].value, 15, "sum", "intersection", "difference", "+", "-", "*", "/", "**", "=", "+=", "-=", "*=", "/=", "in", "find_by_id_2")){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->dynamicIDs.push_back(words[2].value);
            cursor = 3;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "++" || words[0].value == "--" || words[0].value == "delete" || words[0].value == "demolish"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1, lineNumber, scriptName)){
                return;
            }
            if(optional(words, cursor, Operation->dynamicIDs)){ continue; }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "value"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            cursor = 1;
            if(!createExpression(words, cursor, Operation->ConditionalChain, lineNumber, scriptName)){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Expression creation failed.\n";
                return;
            }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(isStringInGroup(words[0].value, 4, "bool", "int", "double", "string")){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            cursor = 1;
            if(words[0].value == "bool"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'b', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[0].value == "int"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'i', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[0].value == "double"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'd', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[0].value == "string"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 's', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal type is required.\n";
                return;
            }
            
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "random_int"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type == 'i' && words[2].type == 'i'){
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[1].value, error)));
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[2].value, error)));
            }
            else if(words[1].type == 'c' && words[2].type == 'c'){
                Operation->dynamicIDs.push_back(words[1].value);
                Operation->dynamicIDs.push_back(words[2].value);
            }
            else{
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of an int or context type.\n";
                return;
            }
            cursor = 3;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "find_by_id"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Location.source = words[1].value;
            cursor = 2;
            if(words[1].value == "camera"){
                if(words.size() < 4){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": \'find_by_id " << words[1].value << "\' requires at least 2 additional parameters.\n";
                    continue;
                }
                if(optional(words, cursor, Operation->Location.cameraID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1].value == "layer"){
                if(words.size() < 7){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": \'find_by_id " << words[1].value << "\' requires at least 5 additional parameters.\n";
                    continue;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
            else if(words[1].value == "context" || words[1].value == "c" || words[1].type == 'e'){
                if(!gatherContextVector(words, cursor, Operation->dynamicIDs, lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Context gather failed.\n";
                    return;
                }
                if(words.size() < cursor + 5){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": \'find_by_id " << words[1].value << " [context_list]\' requires at least 5 additional parameters.\n";
                    continue;
                }
                if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleType)){ continue; }
                if(optional(words, cursor, Operation->Location.moduleID)){ continue; }
                if(optional(words, cursor, Operation->Location.attribute)){ continue; }
                if(optional(words, cursor, Operation->newContextID)){ continue; }
            }
        }
        else if(words[0].value == "let"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->newContextID = words[1].value;
            cursor = 2;
            if(optional(words, cursor, Operation->dynamicIDs)){ continue; }
        }
        else if(words[0].value == "clone"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->dynamicIDs.push_back(words[2].value);
            if(words.size() >= 4){
                Operation->Literals.push_back(VariableModule::newBool(cstoi(words[3].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }
        }
        else if(words[0].value == "new"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Location.source = words[1].value;
            
            if(words[2].type == 'c'){
                Operation->Literals.push_back(VariableModule::newString("context"));
            }
            else{
                Operation->Literals.push_back(VariableModule::newString("location"));
            }
            
            cursor = 2;
            if(words[2].type == 'c'){
                Operation->dynamicIDs.push_back(words[2].value);
                cursor++;
            }
            else{
                if(Operation->Location.source == "object"){
                    if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                }
                else if(Operation->Location.source != "camera" && Operation->Location.source != "layer"){
                    if(optional(words, cursor, Operation->Location.layerID)){ continue; }
                    if(optional(words, cursor, Operation->Location.objectID)){ continue; }
                }
            }
            
            if(words.size() <= cursor){
                continue;
            }
            if(words[cursor].type != 'e'){
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[cursor].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                }
            }
            cursor++;
            if(optional(words, cursor, Operation->dynamicIDs)){ continue; }
            if(optional(words, cursor, Operation->dynamicIDs)){ continue; }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "bind"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->Literals.push_back(VariableModule::newString(words[2].value));
            if(isStringInGroup(words[2].value, 5, "literal", "l", "remove_literal", "rliteral", "rl")){
                cursor = 3;
                if(!gatherLiterals(words, cursor, Operation->Literals, 's', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(isStringInGroup(words[2].value, 5, "context", "c", "remove_context", "rcontext", "rc")){
                if(words.size() > 3){
                    if(words[3].type != 'c'){
                        cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                            << ": In '" << words[0].value << "' instruction: The third parameter is not of a context type.\n";
                        return;
                    }
                    Operation->dynamicIDs.push_back(words[3].value);
                }
                else{
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": No context provided.\n";
                }
            }
            else if(words[2].value != "reset" && words[2].value != "r"){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": In bind instruction, type must be equal to one of these values: "
                    "\"literal\", \"l\", \"remove_literal\", \"rliteral\", \"rl\", \"context\", \"c\", \"remove_context\", \"rcontext\", \"rc\", \"reset\", \"r\".\n";
                return;
            }
        }
        else if(words[0].value == "build"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            if(words.size() > 2 && words[2].type != 'e'){
                Operation->Literals.push_back(VariableModule::newBool(cstoi(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }     
        }
        else if(words[0].value == "load_build" || words[0].value == "build_subset" || words[0].value == "inject_code" || words[0].value == "inject_instr"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            cursor = 2;
            if(optional(words, cursor, Operation->dynamicIDs)){ continue; }
            if(!gatherLiterals(words, cursor, Operation->Literals, 's', lineNumber, scriptName)){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                return;
            }
        }
        else if(words[0].value == "fun"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->Location.attribute = words[2].value;
            cursor = 3;
            while(words.size() > cursor){
                if(words[cursor].type == 'c'){
                    Operation->dynamicIDs.push_back(words[cursor].value);
                }
                else if(words[cursor].type == 'b'){
                    Operation->Literals.push_back(VariableModule::newBool(cstoi(words[cursor].value, error)));
                }
                else if(words[cursor].type == 'i'){
                    Operation->Literals.push_back(VariableModule::newInt(cstoi(words[cursor].value, error)));
                }
                else if(words[cursor].type == 'd'){
                    Operation->Literals.push_back(VariableModule::newDouble(cstod(words[cursor].value, error)));
                }
                else if(words[cursor].type == 's'){
                    Operation->Literals.push_back(VariableModule::newString(words[cursor].value));
                }
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
                cursor++;
            }
        }
        else if(words[0].value == "env"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            if(words[1].value == "window_title"){
                if(words[1].type != 's'){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: The second parameter is not of a string type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newString(words[2].value));
            }
            else if(words[1].value == "window_size"){
                if(words.size() < 4){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Instruction \'" << Operation->instruction <<
                        "\' with \'" << Operation->Literals[0].getString() << "\' attribute requires two literals of a numeric type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                }
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[3].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                }
            }
            else{
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                }
            }
        }
        else if(words[0].value == "edit_proc"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            if(isStringInGroup(words[1].value, 2, "clear_layers", "clear_cameras")){
                continue;
            }
            if(words.size() < 3){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Instruction \'" << Operation->instruction <<
                        "\' with \'" << Operation->Literals[0].getString() << "\' attribute requires two literals of a numeric type.\n";
                return;
            }
            if(words[1].value == "id"){
                if(words[1].type != 's'){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: The second parameter is not of a string type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newString(words[2].value));
            }
            else if(words[1].value == "reservation_multiplier"){
                Operation->Literals.push_back(VariableModule::newDouble(cstod(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                }
            }
            else if(words[1].value == "window_pos" || words[1].value == "window_size" || words[1].value == "min_window_size"){
                if(words.size() < 4){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Instruction \'" << Operation->instruction <<
                        "\' with \'" << Operation->Literals[0].getString() << "\' attribute requires two literals of a numeric type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[3].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }
            else if(words[1].value == "window_tint"){
                if(words.size() < 6){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Instruction \'" << Operation->instruction <<
                        "\' with \'" << Operation->Literals[0].getString() << "\' attribute requires four literals of a double type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newDouble(cstoi(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newDouble(cstoi(words[3].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newDouble(cstoi(words[4].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newDouble(cstoi(words[5].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }
            else{
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[2].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }
        }
        else if(words[0].value == "load_bitmap"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 's' || words[2].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a string type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            Operation->Literals.push_back(VariableModule::newString(words[2].value));
            if(words.size() > 3){
                Operation->Literals.push_back(VariableModule::newBool(cstoi(words[3].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }
            if(words.size() > 4){
                Operation->Literals.push_back(VariableModule::newBool(cstoi(words[4].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
            }
        }
        else if(words[0].value == "mkdir" || words[0].value == "rm" || words[0].value == "rmll"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a string type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
        }
        else if(words[0].value == "rename"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 's' || words[2].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a string type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            Operation->Literals.push_back(VariableModule::newString(words[2].value));
        }
        else if(words[0].value == "print"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type == 'e'){
                Operation->Literals.push_back(VariableModule::newString(""));
            }
            else if(words[1].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a string type.\n";
                return;
            }
            else{
                Operation->Literals.push_back(VariableModule::newString(words[1].value));
            }

            if(words.size() < 3){
                continue;
            }

            if(words[2].type != 'c' && words[2].type != 'e'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The second parameter is not of a context type.\n";
                return;
            }
            else if(words[2].type != 'e'){
                Operation->newContextID = words[2].value;
            }
            
            cursor = 3;
            while(words.size() > cursor){
                if(words[cursor].type == 'c'){
                    Operation->dynamicIDs.push_back(words[cursor].value);
                }
                else if(words[cursor].type == 'b'){
                    Operation->Literals.push_back(VariableModule::newBool(cstoi(words[cursor].value, error)));
                }
                else if(words[cursor].type == 'i'){
                    Operation->Literals.push_back(VariableModule::newInt(cstoi(words[cursor].value, error)));
                }
                else if(words[cursor].type == 'd'){
                    Operation->Literals.push_back(VariableModule::newDouble(cstod(words[cursor].value, error)));
                }
                else if(words[cursor].type == 's'){
                    Operation->Literals.push_back(VariableModule::newString(words[cursor].value));
                }
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
                    return;
                }
                cursor++;
            }
        }
        else if(words[0].value == "load_text"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            cursor = 2;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "save_text"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->dynamicIDs.push_back(words[2].value);
        }
        else if(words[0].value == "ls"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1, lineNumber, scriptName)){
                return;
            }
            if(optional(words, cursor, Operation->dynamicIDs)){ continue; }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "lse"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a string type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            if(words.size() == 2){
                continue;
            }
            Operation->Literals.push_back(VariableModule::newBool(cstoi(words[2].value, error)));
            if(error.size() > 0){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ":\n" << error << "\n";
            }
        }
        else if(words[0].value == "new_proc"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a string type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            for(unsigned int i = 2; i < 5; i++){
                if(words.size() <= i){
                    break;
                }
                if(words[i].type != 's'){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: The parameter nr " << i+1 << ". is not of a string type.\n";
                    return;
                }
                Operation->Literals.push_back(VariableModule::newString(words[i].value));
            }
        }
        else if(words[0].value == "var"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].value == "true" || words[1].value == "false"){
                Operation->Literals.push_back(VariableModule::newBool(cstoi(words[1].value, error)));
            }
            else if(words[1].type == 'i'){
                Operation->Literals.push_back(VariableModule::newInt(cstoi(words[1].value, error)));
            }
            else if(words[1].type == 'd'){
                Operation->Literals.push_back(VariableModule::newDouble(cstod(words[1].value, error)));
            }
            else if(words[1].type == 's'){
                Operation->Literals.push_back(VariableModule::newString(words[1].value));
            }
            else{
                cout << "Error: In script: " << scriptName << ":\n\tIn line " << lineNumber << ": In "
                    << __FUNCTION__ << ": In instruction \'" << words[0].value << "\' the value \'" << words[1].value << "\' is not of a literal type.\n";
                    return;
                continue;
            }
            if(words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The second parameter is not of a context type.\n";
                return;
            }
            Operation->newContextID = words[2].value;
        }
        else if(words[0].value == "vec"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 4, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            cursor = 2;
            if(words[1].value == "bool" || words[1].value == "b"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'b', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[1].value == "int" || words[1].value == "i"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'i', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[1].value == "double" || words[1].value == "d"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 'd', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else if(words[1].value == "string" || words[1].value == "s"){
                if(!gatherLiterals(words, cursor, Operation->Literals, 's', lineNumber, scriptName)){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Literal creation failed.\n";
                    return;
                }
            }
            else{
                cout << "Error: In script: " << scriptName << ":\n\tIn line " << lineNumber << ": In "
                    << __FUNCTION__ << ": In instruction \'" << words[0].value << "\' the type \'" << words[1].value << "\' does not exist.\n";
                continue;
            }
            if(words[cursor].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The last parameter is not of a context type.\n";
                return;
            }
            Operation->newContextID = words[cursor].value;
        }
        else if(words[0].value == "tokenize"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 's'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a string type.\n";
                return;
            }
            Operation->Literals.push_back(VariableModule::newString(words[1].value));
            cursor = 2;
            while(cursor < words.size()){
                if(words[cursor].type != 'c'){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: Parameter nr " << cursor+1 << ". is not of a context type.\n";
                    return;
                }
                Operation->dynamicIDs.push_back(words[cursor].value);
                cursor++;
            }
        }
        else if(words[0].value == "tree" || words[0].value == "pwd"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1, lineNumber, scriptName)){
                return;
            }
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "len" || words[0].value == "size"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 2, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            cursor++;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "substr"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 4, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c' || words[3].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: All parameters must be of the context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->dynamicIDs.push_back(words[2].value);
            Operation->dynamicIDs.push_back(words[3].value);
            cursor = 4;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else if(words[0].value == "load_font"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 4, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c' || words[3].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: All parameters must be of the context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->dynamicIDs.push_back(words[2].value);
            Operation->dynamicIDs.push_back(words[3].value);
            if(words.size() > 4){
                Operation->Literals.push_back(VariableModule::newBool(cstoi(words[4].value, error)));
                if(error.size() > 0){
                    cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                        << ": In '" << words[0].value << "' instruction: The fourth parameter is not of bool type.\n";
                    return;
                }
            }
        }
        else if(words[0].value == "cd"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 1, lineNumber, scriptName)){
                return;
            }
            if(words.size() == 1){
                continue;
            }
            if(words[1].type == 's'){
                Operation->Literals.push_back(VariableModule::newString(words[1].value));
            }
            else if(words[1].type == 'c'){
                Operation->dynamicIDs.push_back(words[1].value);
            }
            else {
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: The first parameter is not of string or context type.\n";
                return;
            }
        }
        else if(words[0].value == "similar"){
            if(!prepareNewInstruction(words, NewEvent, Operation, postOperations, 3, lineNumber, scriptName)){
                return;
            }
            if(words[1].type != 'c' || words[2].type != 'c'){
                cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__
                    << ": In '" << words[0].value << "' instruction: All parameters must be of the context type.\n";
                return;
            }
            Operation->dynamicIDs.push_back(words[1].value);
            Operation->dynamicIDs.push_back(words[2].value);
            cursor = 3;
            if(optional(words, cursor, Operation->newContextID)){ continue; }
        }
        else{
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' does not exist.\n";
        }
    }
    if(words.size() > 0){
        if(words[0].value != "end"){
            cout << "Error: In script: " << scriptName << ":\nIn line " << lineNumber << ": In " << __FUNCTION__ << ": Every event must end with \"end\" instruction.\n";
        }
    }
}
void AncestorObject::clearAllEvents(){
    for(auto & Event : EveContainer){
        Event.clear();
    }
    EveContainer.clear();
    eveContainerIDs.clear();
}
void AncestorObject::translateAllScripts(bool clearEvents, bool allowNotAscii){
    if(clearEvents){
        clearAllEvents();
    }

    vector<string> code;
    
    for(string scriptName : bindedScripts){
        code = readLines(scriptName, allowNotAscii);
        if(code.size() > 0){
            eventAssembler(code, scriptName);
            code.clear();
        }
        else{
            cout << "Warning: In " << __FUNCTION__ << ": script \'" << scriptName << "\' is empty or cannot be opened.\n";
        }
    }
}
void AncestorObject::translateScriptsFromPaths(vector<string> scriptsPaths, bool allowNotAscii){
    vector<string> code;
    
    for(string scriptName : scriptsPaths){
        code = readLines(scriptName, allowNotAscii);
        if(code.size() > 0){
            eventAssembler(code, scriptName);
            code.clear();
        }
        else{
            cout << "Warning: In " << __FUNCTION__ << ": script \'" << scriptName << "\' is empty or cannot be opened.\n";
        }
    }
}
void AncestorObject::translateSubsetBindedScripts(vector<string> scripts, bool allowNotAscii){
    vector<string> code;
    
    for(string scriptName : bindedScripts){
        if(!isStringInVector(scripts, scriptName)){
            continue;
        }
        code = readLines(scriptName, allowNotAscii);
        if(code.size() > 0){
            eventAssembler(code, scriptName);
            code.clear();
        }
        else{
            cout << "Warning: In " << __FUNCTION__ << ": script \'" << scriptName << "\' is empty or cannot be opened.\n";
        }
    }
}
void AncestorObject::injectCode(vector<string> code){
    code = removeComments(code);
    
    if(code.size() > 0){
        eventAssembler(code, "<injection>");
    }
}
void AncestorObject::injectInstructions(vector<string> instructions){
    vector<string> preprocessed;
    for(string line : instructions){
        string output;
        for(size_t i = 0; i < line.size(); i++){
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
    preprocessed.push_back("delete_this_event");
    preprocessed.push_back("end");
    eventAssembler(preprocessed, "<injection>");
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
    for(EveModule & Event : EveContainer){
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
    for(EveModule & Event : EveContainer){
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