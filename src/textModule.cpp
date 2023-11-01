#include "textModule.h"

SingleFont * findFontByID(vector <SingleFont> FontContainer, string fontID){
    for(SingleFont & font : FontContainer){
        if(fontID == font.ID){
            return &font;
        }
    }
    return &FontContainer[0];
}

double getFontHeight(vector <SingleFont> FontContainer, string fontID){
    return al_get_font_line_height(findFontByID(FontContainer, fontID)->font);
}

void TextModule::setUpNewInstance(){
    color = al_map_rgba_f(0.0, 0.0, 0.0, 1.0);
    fontID = -1;
    wrapped = false;
    horizontalAlign = 0;
    verticalAlign = 0;
    rotation = 0.0;
    scale.set(1.0, 1.0);
    currentTextIdx = 0;
    usedBitmapLayer = 0;
    randomChangeSpeed = 0.001;
    minColorValue = 0.0f;
    maxColorValue = 1.0f;
}
TextModule::TextModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
TextModule::TextModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
void TextModule::clone(const TextModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}
TextModule::TextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
void TextModule::addNewContent(string newContent){
    content.push_back(newContent);
}
void TextModule::fitSizeToText(vector <SingleFont> FontContainer){
    ALLEGRO_FONT * font = findFontByID(FontContainer, fontID)->font;
    vec2d newSize(0.0, al_get_font_line_height(font));
    for(string text : content){
        if(newSize.x < al_get_text_width(font, text.c_str())){
            newSize.x = al_get_text_width(font, text.c_str());
        }
    }
    size.set(newSize);
}
void TextModule::addNewContentAndResize(string newContent, vector <SingleFont> FontContainer){
    content.push_back(newContent);
    fitSizeToText(FontContainer);
}
bool TextModule::checkSize(unsigned int textID) const {
    if(content.size() == 0){
        cout << "Error: In " << __FUNCTION__ << ": No text content in the text field: \"" << ID << "\".\n";
        return false;
    }
    if(textID >= content.size()){
        cout << "Error: In " << __FUNCTION__ << ": Bad index (" << textID << "/" << content.size() << ") in the text field: \"" << ID << "\".\n";
        return false;
    }
    return true;
}
void TextModule::chooseContent(unsigned int textNumber){
    if(!checkSize(textNumber)){
        return;
    }
    currentTextIdx = textNumber;
}
void TextModule::modifyContent(unsigned int textNumber, string modifiedContent){
    if(!checkSize(textNumber)){
        return;
    }
    content[textNumber] = modifiedContent;
}
void TextModule::modifyContentAndResize(unsigned int textNumber, string modifiedContent, vector <SingleFont> FontContainer){
    if(!checkSize(textNumber)){
        return;
    }
    content[textNumber] = modifiedContent;
    fitSizeToText(FontContainer);
}
void TextModule::deleteContent(unsigned int textNumber){
    if(!checkSize(textNumber)){
        return;
    }
    content.erase(content.begin()+textNumber);
    if(currentTextIdx >= textNumber && currentTextIdx > 0){
        currentTextIdx--;
    }
}
void TextModule::clear(){
    content.clear();
    currentTextIdx = 0;
}
void TextModule::setColors(float r, float g, float b, float a){
    color = al_map_rgba_f(r, g, b, a);
}
void TextModule::setRandomColors(){
    color = al_map_rgba_f(
        randomFloat(0.0, 1.0),
        randomFloat(0.0, 1.0),
        randomFloat(0.0, 1.0),
        color.a
    );
}
void TextModule::incrementRandomColor(){
    char choice = rand() % 6;
    switch(choice){
        case 0:
            color.r = std::min(color.r + randomChangeSpeed, maxColorValue);
            break;
        case 1:
            color.r = std::max(color.r - randomChangeSpeed, minColorValue);
            break;
        case 2:
            color.g = std::min(color.g + randomChangeSpeed, maxColorValue);
            break;
        case 3:
            color.g = std::max(color.g - randomChangeSpeed, minColorValue);
            break;
        case 4:
            color.b = std::min(color.b + randomChangeSpeed, maxColorValue);
            break;
        case 5:
            color.b = std::max(color.b - randomChangeSpeed, minColorValue);
            break;
    }
}
void TextModule::setRotation(double newAngle){
    rotation = newAngle;
}
void TextModule::setUsedBitmapLayer(int newLayer){
    usedBitmapLayer = newLayer;
}
void TextModule::addRotation(double newAngle){
    rotation += newAngle;
}
void TextModule::setFontID(string newValue){
    fontID = newValue;
}
void TextModule::setWrapping(short newValue){
    wrapped = newValue;
}
void TextModule::setHorizontalAlign(short newValue){
    horizontalAlign = newValue;
}
void TextModule::setVerticalAlign(short newValue){
    verticalAlign = newValue;
}
void TextModule::setAlpha(float newValue){
    color.a = newValue;
}
void TextModule::changeParameters(string newID, vector<string> & listOfIDs, vec4d posSize, vec3d fontColor, string newFontID, vec2d newScale,
                          double newRotateAngle, short newWrapped, int newHorizontalAlign, int newVerticalAlign){
    setID(newID, listOfIDs);
    setPos(posSize.val[0], posSize.val[1]);
    setSize(posSize.val[2], posSize.val[3]);
    setScale(newScale);
    color = al_map_rgba_f(fontColor.val[0], fontColor.val[1], fontColor.val[2], 1.0);
    fontID = newFontID;
    rotation = newRotateAngle;
    wrapped = newWrapped;
    horizontalAlign = newHorizontalAlign;
    verticalAlign = newVerticalAlign;
}
void TextModule::drawText(vec2d base, ALLEGRO_FONT * font, bool drawBorders, Camera2D Camera, unsigned int cursorPos, bool editingIsActive){
    if(!getIsActive()){
        return;
    }

    vector <string> textLines;
    textLines.push_back(string());
    string temp, text;
    int currentLength = 0, currentHeight = 0, realHeight = 0;
    int fontHeight = al_get_font_line_height(font);
    if(currentTextIdx < content.size()){
        text = content[currentTextIdx];
    }
    else{
        return;
    }

    for(unsigned int letter = 0; letter < text.size(); letter++){
        temp.clear();
        temp = text.substr(letter, 1);
        if(temp == "\n"){ //enter handling
            currentLength = 0;
            currentHeight += fontHeight;
            if(currentHeight + fontHeight > size.y){
                break;
            }
            textLines.back() += '\n';
            textLines.push_back(string());
            continue;
        }
        if(temp == " " && wrapped == 2){ //smart text wrapping - keeping words intact
            int futureLenght = currentLength;
            string newTemp;
            bool success = false;
            for(unsigned int i = letter; i < text.size(); i++){
                newTemp.clear();
                newTemp = text.substr(i, 1);

                if(i != letter && newTemp == " "){
                    break;
                }
                futureLenght += al_get_text_width(font, newTemp.c_str());
                if(futureLenght > size.x){
                    currentLength = 0;
                    currentHeight += fontHeight;
                    textLines.push_back(string());
                    success = true;
                    break;
                }
            }
            if(success){
                continue;
            }
        }
        //cropping text according to its dimensions and wrapping text when possible
        if(currentLength + al_get_text_width(font, temp.c_str()) > size.x){
            if(wrapped == 0){
                break;
            }
            currentLength = 0;
            currentHeight += fontHeight;
            textLines.push_back(string());
            if(currentHeight + fontHeight > size.y){
                break;
            }
        }
        if(temp != " " || currentLength != 0){
            textLines[textLines.size()-1] = textLines[textLines.size()-1] + temp;
            currentLength += al_get_text_width(font, temp.c_str());
            realHeight = currentHeight;
        }
    }
    realHeight += fontHeight;
    int alignHeight = 0;
    if(verticalAlign == 1){
        alignHeight = size.y/2-realHeight/2;
    }
    else if(verticalAlign == 2){
        alignHeight = size.y-realHeight;
    }   

    vec2d newScale(scale);
    vec2d finalPos(base);
    finalPos.translate(pos);
    if(!isAttachedToCamera){
        vec2d diff(Camera.size.x/2-base.x-pos.x, Camera.size.y/2-base.y-pos.y);
        finalPos.set(Camera.size.x/2-diff.x*(Camera.zoom), Camera.size.y/2-diff.y*(Camera.zoom));
        finalPos.translate(Camera.visionShift.x*Camera.zoom, Camera.visionShift.y*Camera.zoom);
        newScale.x *= Camera.zoom;
        newScale.y *= Camera.zoom;
        if(isScaledFromCenter){
            finalPos.translate(-((scale.x-1)*size.x*Camera.zoom)/2, -((scale.y-1)*size.y*Camera.zoom)/2);
        }
    }

    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_translate_transform(&t, -finalPos.x, -finalPos.y);
    al_rotate_transform(&t, (rotation*M_PI)/180.0);
    al_scale_transform(&t, newScale.x, newScale.y);
    al_translate_transform(&t, finalPos.x, finalPos.y);
    al_use_transform(&t);

    vec2d cursorRealPos(finalPos);

    string letterOnCursor = "";

    if(editingIsActive){
        if(cursorPos > text.size()){
            cursorPos = 0;
        }
        else{
            unsigned countedChars = 0, line, letter;
            for(line = 0; line < textLines.size(); line++){
                for(letter = 0; letter < textLines[line].size() && countedChars < cursorPos; letter++, countedChars++){
                    temp.clear();
                    temp = text.substr(letter, 1);
                    if(temp != "\n"){
                        cursorRealPos.x += al_get_text_width(font, temp.c_str());
                    }
                    else{
                        cursorRealPos.x += al_get_text_width(font, " ");
                    }
                }
                if(countedChars >= cursorPos){
                    if(textLines[line].size() > 0 && letter == textLines[line].size() && (textLines[line][letter-1] == '\n' || al_get_text_width(font, (textLines[line]+" ").c_str()) > size.x)){
                        cursorRealPos.x = finalPos.x;
                        cursorRealPos.y += fontHeight;
                        line++;
                        letter = 0;
                    }
                    if(line < textLines.size() && letter < textLines[line].size()){
                        letterOnCursor = textLines[line].substr(letter, 1);
                    }
                    break;
                }
                cursorRealPos.x = finalPos.x;
                cursorRealPos.y += fontHeight;
            }
        }
    }

    if(letterOnCursor == "\n"){
        letterOnCursor = "";
    }

    for(string & line : textLines){
        if(line.size() > 0 && line.back() == '\n'){
            line.pop_back();
        }
    }

    ALLEGRO_COLOR inversedColor = al_map_rgba_f(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);

    if(horizontalAlign == 0){
        if(editingIsActive){
            al_draw_text(font, color, cursorRealPos.x, alignHeight + cursorRealPos.y, 0, "█");
        }
        for(unsigned int i = 0; i < textLines.size(); i++){
            al_draw_text(font, color, finalPos.x, finalPos.y + alignHeight + i * fontHeight, 0, textLines[i].c_str());
        }
        if(editingIsActive){
            al_draw_text(font, inversedColor, cursorRealPos.x, alignHeight + cursorRealPos.y, 0, letterOnCursor.c_str());
        }
    }
    else if(horizontalAlign == 1){
        if(editingIsActive){
            al_draw_text(font, color, size.x / 2 + cursorRealPos.x, alignHeight + cursorRealPos.y, ALLEGRO_ALIGN_CENTRE, "█");
        }
        for(unsigned int i = 0; i < textLines.size(); i++){
            al_draw_text(font, color, finalPos.x + size.x / 2, finalPos.y + alignHeight + i * fontHeight, ALLEGRO_ALIGN_CENTRE, textLines[i].c_str());
        }
        if(editingIsActive){
            al_draw_text(font, inversedColor, size.x / 2 + cursorRealPos.x, alignHeight + cursorRealPos.y, ALLEGRO_ALIGN_CENTRE, letterOnCursor.c_str());
        }
    }
    else if(horizontalAlign == 2){
        if(editingIsActive){
            al_draw_text(font, color, size.x + cursorRealPos.x, alignHeight + cursorRealPos.y, ALLEGRO_ALIGN_RIGHT, "█");
        }
        for(unsigned int i = 0; i < textLines.size(); i++){
            al_draw_text(font, color, finalPos.x + size.x, finalPos.y + alignHeight + i * fontHeight, ALLEGRO_ALIGN_RIGHT, textLines[i].c_str());
        }
        if(editingIsActive){
            al_draw_text(font, inversedColor, size.x + cursorRealPos.x, alignHeight + cursorRealPos.y, ALLEGRO_ALIGN_RIGHT, letterOnCursor.c_str());
        }
    }

    if(drawBorders){
        al_draw_rectangle(finalPos.x, finalPos.y, finalPos.x+size.x, finalPos.y+size.y, color, 1);
    }

    textLines.clear();

    al_identity_transform(&t);
    al_use_transform(&t);
}
void TextModule::drawTextByLetters(ALLEGRO_FONT * font){
    if(!getIsActive()){
        return;
    }

    string temp, text;
    int currentLength = 0, currentHigh = 0;
    if(currentTextIdx < content.size()){
        text = content[currentTextIdx];
    }
    else{
        text = "error";
    }

    for(unsigned int letter = 0; letter < text.size(); letter++){
        temp.clear();
        temp = text.substr(letter, 1);

        if(temp == "\n"){ //enter handling
            currentLength = 0;
            currentHigh += al_get_font_line_height(font);
            if(currentHigh + al_get_font_line_height(font) > size.y)
                return;
            continue;
        }
        if(temp == " " && wrapped == 2){ //smart text wrapping - keeping words intact
            int futureLenght = currentLength;
            string newTemp;
            bool success = false;
            for(unsigned int i = letter; i < text.size(); i++){
                newTemp.clear();
                newTemp = text.substr(i, 1);

                if(i != letter && newTemp == " "){
                    break;
                }
                futureLenght += al_get_text_width(font, newTemp.c_str());
                if(futureLenght > size.x){
                    currentLength = 0;
                    currentHigh += al_get_font_line_height(font);
                    success = true;
                    break;
                }
            }
            if(success)
                continue;
        }
        if(currentLength + al_get_text_width(font, temp.c_str()) > size.x){ //cropping text according to its dimensions and wrapping text when possible
            if(wrapped == 0)
                return;
            currentLength = 0;
            currentHigh += al_get_font_line_height(font);
            if(currentHigh + al_get_font_line_height(font) > size.y)
                return;
        }

        al_draw_text(font, color, pos.x+currentLength, pos.y+currentHigh, 0, temp.c_str());
        currentLength += al_get_text_width(font, temp.c_str());
    }
}
void TextModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "content"){
        BasePointers.back().setPointer(&content[currentTextIdx]);
    }
    else if(attribute == "current_text_id"){
        BasePointers.back().setPointer(&currentTextIdx);
    }
    else if(attribute == "font_id"){
        BasePointers.back().setPointer(&fontID);
    }
    else if(attribute == "text_color_r"){
        BasePointers.back().setPointer(&color.r);
    }
    else if(attribute == "text_color_g"){
        BasePointers.back().setPointer(&color.g);
    }
    else if(attribute == "text_color_b"){
        BasePointers.back().setPointer(&color.b);
    }
    else if(attribute == "text_color_a"){
        BasePointers.back().setPointer(&color.a);
    }
    else if(attribute == "wrapped"){
        BasePointers.back().setPointer(&wrapped);
    }
    else if(attribute == "horizontal_align"){
        BasePointers.back().setPointer(&horizontalAlign);
    }
    else if(attribute == "vertical_align"){
        BasePointers.back().setPointer(&verticalAlign);
    }
    else if(attribute == "rotate_angle"){
        BasePointers.back().setPointer(&rotation);
    }
    else if(attribute == "used_bitmap_layer"){
        BasePointers.back().setPointer(&usedBitmapLayer);
    }
    else{
        getPrimaryContext(attribute, BasePointers);
    }
}
string TextModule::getFontID(){
    return fontID;
}
string TextModule::getContent(unsigned int textNumber) const{
    if(!checkSize(textNumber)){
        return "";
    }
    return content[textNumber];
}
float TextModule::getColor(char whichColor){
    if(whichColor == 'r'){
        return color.r;
    }
    if(whichColor == 'g'){
        return color.g;
    }
    if(whichColor == 'b'){
        return color.b;
    }
    if(whichColor == 'a'){
        return color.b;
    }
    return 0.0;
}
unsigned int TextModule::getCurrentTextIdx() const{
    return currentTextIdx;
}
string TextModule::getCurrentContent() const{
    return getContent(getCurrentTextIdx());
}
VariableModule TextModule::getAttributeValue(const string &attribute, const string &detail) const{
    if(attribute == "in_group"){
        return VariableModule::newBool(isInAGroup(detail));
    }
    else if(attribute == "content"){
        return VariableModule::newString(getCurrentContent());
    }
    else if(attribute == "current_text_id"){
        return VariableModule::newInt(currentTextIdx);
    }
    else if(attribute == "font_id"){
        return VariableModule::newString(getCurrentContent());
    }
    else if(attribute == "color_r"){
        return VariableModule::newDouble(color.r);
    }
    else if(attribute == "color_g"){
        return VariableModule::newDouble(color.g);
    }
    else if(attribute == "color_b"){
        return VariableModule::newDouble(color.b);
    }
    else if(attribute == "color_a"){
        return VariableModule::newDouble(color.a);
    }
    else if(attribute == "wrapped"){
        return VariableModule::newInt(wrapped);
    }
    else if(attribute == "horizontal_align"){
        return VariableModule::newInt(horizontalAlign);
    }
    else if(attribute == "vertical_align"){
        return VariableModule::newInt(verticalAlign);
    }
    else if(attribute == "rotation"){
        return VariableModule::newDouble(rotation);
    }
    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    return VariableModule::newBool(false);
}

void EditableTextModule::setUpNewInstance(){
    TextModule::setUpNewInstance();
    isAttachedToCamera = true;
    canBeEdited = true;
    editingIsActive = false;
    canUseSpace = false;
    isNumerical = false;
    hasFloatingPoint = false;
    updateConnectedVariable = false;
    canClearContentAfterSuccess = false;
    useArrowsAsChar = false;
    enterAcceptsChanges = false;
    cursorPos = 0;
    minContentSize = 0;
    maxContentSize = 20;
    inputDelay = 1.0;
    repetitionDelay = 0.5;
    currentInputDelay = 0.0;
    lastInputedKey = -1;
}
EditableTextModule::EditableTextModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
EditableTextModule::EditableTextModule(unsigned newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
EditableTextModule::EditableTextModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
void EditableTextModule::clone(const EditableTextModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}
void EditableTextModule::setCanBeEdited(bool newCanBeEdited){
    canBeEdited = newCanBeEdited;
}
void EditableTextModule::setEditingIsActive(bool newEditingIsActive){
    editingIsActive = newEditingIsActive;
}
void EditableTextModule::setCanUseSpace(bool newCanUseSpace){
    canUseSpace = newCanUseSpace;
}
void EditableTextModule::setIsNumerical(bool newIsNumerical){
    isNumerical = newIsNumerical;
}
void EditableTextModule::setHasFloatingPoint(bool newHasFloatingPoint){
    hasFloatingPoint = newHasFloatingPoint;
}
void EditableTextModule::setUpdateConnectedVariable(bool newUpdateConnectedVariable){
    updateConnectedVariable = newUpdateConnectedVariable;
}
void EditableTextModule::setUseArrowsAsChar(bool newValue){
    useArrowsAsChar = newValue;
}
void EditableTextModule::setCanClearContentAfterSuccess(bool newValue){
    canClearContentAfterSuccess = newValue;
}
void EditableTextModule::setCursorPos(unsigned int newCursorPos){
    cursorPos = newCursorPos;
}
void EditableTextModule::setMinContentSize(unsigned int newMinContentSize){
    minContentSize = newMinContentSize;
}
void EditableTextModule::setMaxContentSize(unsigned int newMaxContentSize){
    maxContentSize = newMaxContentSize;
}
void EditableTextModule::setInputDelay(float newValue){
    inputDelay = newValue;
}
void EditableTextModule::setRepetitionDelay(float newValue){
    repetitionDelay = newValue;
}
void EditableTextModule::setConnectedObjectID(string newValue){
    connectedObject = newValue;
}
void EditableTextModule::setConnectedObject(string objectID, string moduleType, string moduleID, string variableName){
    setConnectedObjectID(objectID);
    connectedModule = moduleType;
    connectedModuleID = moduleID;
    connectedVariable = variableName;
}
void EditableTextModule::setConnectedGroup(string groupName, string moduleType, string moduleID, string variableName){
    connectedGroup = groupName;
    connectedModule = moduleType;
    connectedModuleID = moduleID;
    connectedVariable = variableName;
}
bool EditableTextModule::getCanBeEdited() const{
    return canBeEdited;
}
bool EditableTextModule::getEditingIsActive() const{
    return editingIsActive;
}
bool EditableTextModule::getCanUseSpace(){
    return canUseSpace;
}
unsigned int EditableTextModule::getCursorPos(){
    return cursorPos;
}
unsigned int EditableTextModule::getMinContentSize(){
    return minContentSize;
}
unsigned int EditableTextModule::getMaxContentSize(){
    return maxContentSize;
}
bool EditableTextModule::getIsNumerical(){
    return isNumerical;
}
bool EditableTextModule::getHasFloatingPoint(){
    return hasFloatingPoint;
}
bool EditableTextModule::getUpdateConnectedVariable(){
    return updateConnectedVariable;
}
bool EditableTextModule::getUseArrowsAsChar(){
    return useArrowsAsChar;
}
string EditableTextModule::getConnectedObjectID(){
    return connectedObject;
}
void EditableTextModule::clearContentAfterSuccess(bool success){
    if(!canClearContentAfterSuccess || !success){
        return;
    }
    modifyContent(0, "");
    setCursorPos(0);
}
char toChara(short a){
    return 'a'+a-1;
}
void EditableTextModule::editText(vector <short> releasedKeys, vector <short> pressedKeys, vector <SingleFont> & FontContainer){
    if(!getIsActive()){
        return;
    }

    // cout << "Pressed: [";
    // for(short a : pressedKeys){
    //     cout << toChara(a) << ", ";
    // }
    // cout << "] Released: [";
    // for(short a : releasedKeys){
    //     cout << toChara(a) << ", ";
    // }
    // cout << "] Blocked: [";
    // for(short a : blockedKeys){
    //     cout << toChara(a) << ", ";
    // }
    // cout << "] Last: " << toChara(lastInputedKey) << "\n";

    bool shift = false;
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(pressedKeys[i] == ALLEGRO_KEY_LSHIFT || pressedKeys[i] == ALLEGRO_KEY_RSHIFT){
            shift = true;
            pressedKeys.erase(pressedKeys.begin() + i);
            i--;
            continue;
        }
        if(pressedKeys[i] == ALLEGRO_KEY_ENTER && enterAcceptsChanges){
            editingIsActive = false;
            setUpdateConnectedVariable(true);
            return;
        }
    }

    //remove released keys from blocked keys
    for(unsigned int i = 0; i < releasedKeys.size(); i++){
        for(unsigned int j = 0; j < blockedKeys.size(); j++){
            if(releasedKeys[i] == blockedKeys[j]){
                blockedKeys.erase(blockedKeys.begin() + j);
                currentInputDelay = inputDelay;
                break;
            }
        }
    }
    // cout << "Blocked after releasing: [";
    // for(short a : blockedKeys){
    //     cout << toChara(a) << ", ";
    // }
    // cout << "]\n";

    bool ignoreLast = false;

    if(pressedKeys.size() > blockedKeys.size()){
        lastInputedKey = -1;
        currentInputDelay = 0.0;
        ignoreLast = true;
    }

    //removing blocked keys from pressed keys
    for(unsigned int i = 0; i < blockedKeys.size(); i++){
        for(unsigned int j = 0; j < pressedKeys.size(); j++){
            if(blockedKeys[i] == pressedKeys[j] && pressedKeys[j] != lastInputedKey){
                pressedKeys.erase(pressedKeys.begin() + j);
                break;
            }
        }
    }
    // cout << "Pressed after removing: [";
    // for(short a : pressedKeys){
    //     cout << toChara(a) << ", ";
    // }
    // cout << "]\n";


    //find last pressed key, dont include the last 
    if(pressedKeys.size() > 0){
        vector<short> newKeys = pressedKeys;
        if(lastInputedKey != -1 && pressedKeys.size() > 1){
            for(unsigned int i = 0; i < newKeys.size(); i++){
                if(lastInputedKey == newKeys[i]){
                    newKeys.erase(newKeys.begin() + i);
                    break;
                }
            }
        }
        lastInputedKey = newKeys.back();
    }
    //cout << "New last key: " << toChara(lastInputedKey) << "\n";

    //block all pressed keys
    bool found;
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        found = false;
        for(unsigned int j = 0; j < blockedKeys.size(); j++){
            if(pressedKeys[i] == blockedKeys[j]){
                found = true;
                break;
            }
        }
        if(!found){
            blockedKeys.push_back(pressedKeys[i]);
        }
    }
    // cout << "New blocked: [";
    // for(short a : blockedKeys){
    //     cout << toChara(a) << ", ";
    // }
    // cout << "]\n";

    if(pressedKeys.size() == 0){
        currentInputDelay = 0.0;
        lastInputedKey = -1;
        //cout << "Nothing pressed!\n\n";
        return;
    }

    if(pressedKeys.size() > 1){
        currentInputDelay = 0.0;
    }

    if(currentInputDelay > 0.0){
        //cout << "Waiting!\n\n";
        return;
    }

    //delaying input and delaying repetition of the last pressed key
    if(pressedKeys.back() != lastInputedKey || pressedKeys.size() > 1 || ignoreLast){
        currentInputDelay = inputDelay;
        //cout << "Delayed input: " << inputDelay << "!\n";
    }
    else{
        currentInputDelay = repetitionDelay;
        //cout << "Delayed repeat: " << repetitionDelay << "!\n";
    }

    //cout << "\n";

    char character = '\0';

    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(!useArrowsAsChar){
            if(pressedKeys[i] == ALLEGRO_KEY_RIGHT && cursorPos < content[currentTextIdx].size()){
                cursorPos += 1;
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_LEFT && cursorPos > 0){
                cursorPos -= 1;
            }
        }
        else{
            if(pressedKeys[i] == ALLEGRO_KEY_UP){
                character = 24;
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_RIGHT){
                character = 26;
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_DOWN){
                character = 25;
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_LEFT){
                character = 27;
            }
        }


        if(pressedKeys[i] >= 27 && pressedKeys[i] <= 36){
            if(getIsNumerical() || !shift){
                character = pressedKeys[i]+21;
            }
            else{
                switch (pressedKeys[i]){
                case ALLEGRO_KEY_1:
                    character = '!';
                    break;
                case ALLEGRO_KEY_2:
                    character = '@';
                    break;
                case ALLEGRO_KEY_3:
                    character = '#';
                    break;
                case ALLEGRO_KEY_4:
                    character = '$';
                    break;
                case ALLEGRO_KEY_5:
                    character = '%';
                    break;
                case ALLEGRO_KEY_6:
                    character = '^';
                    break;
                case ALLEGRO_KEY_7:
                    character = '&';
                    break;
                case ALLEGRO_KEY_8:
                    character = '*';
                    break;
                case ALLEGRO_KEY_9:
                    character = '(';
                    break;
                case ALLEGRO_KEY_0:
                    character = ')';
                    break;
                default:
                    break;
                }
            }
        }
        if(pressedKeys[i] == 73 && (getHasFloatingPoint() || getIsNumerical())){
            string text = getContent(0);
            bool isTherePoint = false;
            if(getIsNumerical()){
                for(unsigned int i = 0; i < text.size(); i++){
                    if(text[i] == '.'){
                        isTherePoint = true;
                        break;
                    }
                }
            }
            if(isTherePoint){
                character = '.';
            }
        }
        if(pressedKeys[i] == ALLEGRO_KEY_MINUS){
            if(getIsNumerical()){
                if(cursorPos > 0)
                    continue;
                string text = getContent(0);
                bool isThereMinus = false;
                for(unsigned int i = 0; i < text.size(); i++){
                    if(text[i] == '-'){
                        isThereMinus = true;
                        break;
                    }
                }
                if(isThereMinus)
                    continue;
            }
            character = '-';
        }


        if(!getIsNumerical()){
            if(pressedKeys[i] >= 1 && pressedKeys[i] <= 26){
                if(shift){
                    character = 'A'+pressedKeys[i]-1;
                }
                else{
                    character = 'a'+pressedKeys[i]-1;
                }
            }
            else if(pressedKeys[i] == 61 && shift){
                character = '_';
            }
            else if(pressedKeys[i] == 61){
                character = '-';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_EQUALS && shift){
                character = '+';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_EQUALS){
                character = '=';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_OPENBRACE && shift){
                character = '{';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_OPENBRACE){
                character = '[';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_CLOSEBRACE && shift){
                character = '}';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_CLOSEBRACE){
                character = ']';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_COMMA && shift){
                character = '<';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_COMMA){
                character = ',';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_FULLSTOP && shift){
                character = '>';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_FULLSTOP){
                character = '.';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_SLASH && shift){
                character = '?';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_SLASH){
                character = '/';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_SEMICOLON && shift){
                character = ':';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_SEMICOLON){
                character = ';';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_QUOTE && shift){
                character = '\"';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_QUOTE){
                character = '\'';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_BACKSLASH && shift){
                character = '|';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_BACKSLASH){
                character = '\\';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_SPACE && getCanUseSpace()){
                character = ' ';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_TAB){
                character = '\t';
            }
            else if(pressedKeys[i] == ALLEGRO_KEY_ENTER){
                character = '\n';
            }
        }


        if(character == '\0' || getContent(0).size() >= getMaxContentSize()){
            if(pressedKeys[i] == ALLEGRO_KEY_BACKSPACE && getContent(0).size() > getMinContentSize()){
                if(cursorPos == 0)
                    continue;
                string old_content = getContent(0);
                string new_content = old_content.substr(0, cursorPos-1) + old_content.substr(cursorPos, old_content.size()-cursorPos);
                modifyContent(0, new_content);
                cursorPos -= 1;
            }
            continue;
        }

        cout << "Character: \'" << character << "\'\n";

        string old_content = getContent(0);
        string new_content = old_content.substr(0, cursorPos);
        new_content = new_content + character + old_content.substr(cursorPos, old_content.size()-cursorPos);

        cout << "New content: \"" << new_content << "\"\n\n";

        modifyContent(0, new_content);
        cursorPos += 1;

        //cout << int(character) << " ";
        //cout << character << " (" << int(character) << ") " << " ";
    }
}
bool EditableTextModule::canConvertContentToNumber(){
    if(getIsNumerical())
        return false;
    string text = getContent(0);
    bool isTherePoint = false;

    for(unsigned int i = 0; i < text.size(); i++){
        if(text[i] == '.' || text[i] == ' ' || text[i] == '_'){
            if(text[i] == '.' && isTherePoint){
                isTherePoint = true;
                continue;
            }
            return false;
        }
    }
    return true;
}
bool EditableTextModule::tryUpdatingID(vector <string> & listOfIDs, string & currentID, string newID){
    if(currentID == newID)
        return false;
    //if the algorithm finds the same ID, function returns false
    if(std::find(listOfIDs.begin(), listOfIDs.end(), newID) != listOfIDs.end()){
        return false;
    }
    auto it = std::find(listOfIDs.begin(), listOfIDs.end(), currentID);
    int posOfCurrentObject = std::distance(listOfIDs.begin(), it);
    listOfIDs[posOfCurrentObject] = newID;
    currentID = newID;
    return true;
}
bool EditableTextModule::controlAncestor(PrimaryModule & Primary, vector <string> & listOfIDs){
    bool success = true;

    if(connectedVariable == "activate"){
        Primary.activate();
    }
    else if(connectedVariable == "deactivate"){
        Primary.deactivate();
    }
    else if(connectedVariable == "toggle_is_active"){
        Primary.toggleIsActive();
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }

    string cContent = getContent(0);

    if(cContent.size() == 0){
        return false;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Primary.getIDAddr(), cContent)){
            connectedObject = cContent;
            return true;
        }
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        success = true;
        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(connectedVariable == "is_active"){
            Primary.setIsActive(bValue);
        }
        else if(connectedVariable == "is_scaled_from_center"){
            Primary.setIsScaledFromCenter(bValue);
        }
        else if(connectedVariable == "is_attached_to_camera"){
            Primary.setIsAttachedToCamera(bValue);
        }
        else{
            success = false;
        }
    }

    if(success){
        return success;
    }

    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }
    double dValue = std::stod(getContent(0));

    success = true;

    if(connectedVariable == "position_x"){
        Primary.setPos(dValue, Primary.getPos(false).y);
    }
    else if(connectedVariable == "position_y"){
        Primary.setPos(Primary.getPos(false).x, dValue);
    }
    else if(connectedVariable == "size_x"){
        Primary.setSize(dValue, Primary.getSize().y);
    }
    else if(connectedVariable == "size_y"){
        Primary.setSize(Primary.getSize().x, dValue);
    }
    else{
        success = false;
    }

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlText(TextModule & Text, vector <string> & listOfIDs){
    bool success = false;

    if(connectedVariable == "activate"){
        Text.activate();
        return true;
    }
    if(connectedVariable == "deactivate"){
        Text.deactivate();
        return true;
    }

    string cContent = getContent(0);

    if(connectedVariable == "content"){
        Text.modifyContent(Text.getCurrentTextIdx(), cContent);
        success = true;
    }

    if(cContent.size() == 0){
        return success;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Text.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
    }

    if(connectedVariable == "font"){
        setFontID(cContent);
        return true;
    }

    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }

    success = true;
    double dValue = std::stod(cContent);

    if(connectedVariable == "position_x"){
        Text.setPos(dValue, Text.getPos(false).y);
    }
    else if(connectedVariable == "position_y"){
        Text.setPos(Text.getPos(false).x, dValue);
    }
    else if(connectedVariable == "size_x"){
        Text.setSize(dValue, Text.getSize().y);
    }
    else if(connectedVariable == "size_y"){
        Text.setSize(Text.getSize().x, dValue);
    }
    else if(connectedVariable == "scale_x"){
        Text.setScale(dValue, Text.getSize().y);
    }
    else if(connectedVariable == "scale_y"){
        Text.setScale(Text.getSize().x, dValue);
    }
    else if(connectedVariable == "resize_x"){
        Text.resizeX(dValue);
    }
    else if(connectedVariable == "resize_y"){
        Text.resizeY(dValue);
    }
    else if(connectedVariable == "rotation"){
        Text.setRotation(dValue);
    }
    if(connectedVariable == "color_r"){
        Text.setColors(dValue, Text.color.g, Text.color.b, Text.color.a);
    }
    else if(connectedVariable == "color_g"){
        Text.setColors(Text.color.r, dValue, Text.color.b, Text.color.a);
    }
    else if(connectedVariable == "color_b"){
        Text.setColors(Text.color.r, Text.color.g, dValue, Text.color.a);
    }
    else if(connectedVariable == "color_a"){
        Text.setColors(Text.color.r, Text.color.g, Text.color.b, dValue);
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }

    if(dValue >= 0){
        unsigned int uiValue = static_cast<unsigned int>(dValue);
        if(connectedVariable == "font_id"){
            Text.fontID = uiValue;
            success = true;
        }
        if(success){
            return success;
        }
    }

    success = true;
    short shValue = short(dValue);

    if(connectedVariable == "wrapped"){
        Text.wrapped = shValue;
    }
    else if(connectedVariable == "horizontal_align"){
        Text.horizontalAlign = shValue;
    }
    else if(connectedVariable == "vertical_align"){
        Text.verticalAlign = shValue;
    }
    else if(connectedVariable == "bitmap_layer"){
        Text.usedBitmapLayer = shValue;
    }
    else{
        success = false;
    }

    if(success)
        return success;
    success = false;

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlImage(ImageModule & Image, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfIDs, string EXE_PATH){
    bool success = false;

    string cContent = getContent(0);
    if(connectedVariable == "connect_bitmap_via_path"){
        Image.connectBitmap(BitmapContainer, cContent, "", EXE_PATH);
        success = true;
    }
    else if(connectedVariable == "connect_bitmap_via_alias"){
        Image.connectBitmap(BitmapContainer, "", cContent, EXE_PATH);
        success = true;
    }

    if(success || cContent == ""){
        return success;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Image.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        success = true;
        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(connectedVariable == "mirror_x"){
            Image.setMirrorX(bValue);
        }
        else if(connectedVariable == "mirror_y"){
            Image.setMirrorX(bValue);
        }
        else if(connectedVariable == "drawing"){
            Image.setIsActive(bValue);
        }
        else{
            success = false;
        }
    }

    if(success){
        return success;
    }
    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }
    success = true;
    double dValue = std::stod(cContent);

    if(connectedVariable == "position_x"){
        Image.setPos(dValue, Image.getPos(false).y);
    }
    else if(connectedVariable == "position_y"){
        Image.setPos(Image.getPos(false).x, dValue);
    }
    else if(connectedVariable == "size_x"){
        Image.setSize(dValue, Image.getSize().y);
    }
    else if(connectedVariable == "size_y"){
        Image.setSize(Image.getSize().x, dValue);
    }
    else if(connectedVariable == "scale_x"){
        Image.setScale(dValue, Image.getScale().y);
    }
    else if(connectedVariable == "scale_y"){
        Image.setScale(Image.getScale().x, dValue);
    }
    else if(connectedVariable == "resize_x"){
        Image.resizeX(dValue);
    }
    else if(connectedVariable == "resize_y"){
        Image.resizeY(dValue);
    }
    else if(connectedVariable == "layer"){
        Image.setUsedBitmapLayer(int(dValue));
    }
    else if(connectedVariable == "center_x"){
        Image.setCenterX(dValue);
    }
    else if(connectedVariable == "center_y"){
        Image.setCenterY(dValue);
    }
    else if(connectedVariable == "start_x"){
        Image.setStartX(dValue);
    }
    else if(connectedVariable == "start_y"){
        Image.setStartY(dValue);
    }
    else if(connectedVariable == "frame_size_x"){
        Image.setFrameSizeX(dValue);
    }
    else if(connectedVariable == "frame_size_y"){
        Image.setFrameSizeY(dValue);
    }
    else if(connectedVariable == "current_frame_x"){
        Image.setCurrentFrameX(int(dValue));
    }
    else if(connectedVariable == "current_frame_y"){
        Image.setCurrentFrameY(int(dValue));
    }
    else if(connectedVariable == "rotation"){
        Image.setRotation(dValue);
    }
    else if(connectedVariable == "image_color_r"){
        Image.setImageColor(float(dValue), 'r');
    }
    else if(connectedVariable == "image_color_g"){
        Image.setImageColor(float(dValue), 'g');
    }
    else if(connectedVariable == "image_color_b"){
        Image.setImageColor(float(dValue), 'b');
    }
    else if(connectedVariable == "image_color_a"){
        Image.setImageColor(float(dValue), 'a');
    }
    else if(connectedVariable == "light_r"){
        Image.setLightColor(float(dValue), 'r');
    }
    else if(connectedVariable == "light_g"){
        Image.setLightColor(float(dValue), 'g');
    }
    else if(connectedVariable == "light_b"){
        Image.setLightColor(float(dValue), 'b');
    }
    else if(connectedVariable == "light_l"){
        Image.setLightColor(float(dValue), 'l');
    }
    else{
        success = false;
    }

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlMovement(MovementModule & Movement, vector <string> & listOfIDs){
    bool success = false;
    string cContent = getContent(0);

    if(cContent == ""){
        return false;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Movement.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        success = true;
        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(connectedVariable == "can_move_diagonally"){
            Movement.setCanMoveDiagonally(bValue);
        }
        else if(connectedVariable == "reset_momentum_when_jumping"){
            Movement.setResetMomentumWhenJumping(bValue);
        }
        else if(connectedVariable == "move_on_mouse_release"){
            Movement.setMoveOnMouseRelease(bValue);
        }
        else if(connectedVariable == "reset_direction_after_collision"){
            Movement.setResetDirectionAfterCollision(bValue);
        }
        else{
            success = false;
        }
    }

    if(success){
        return success;
    }

    if(cContent.size() == 1){
        success = true;
        char chValue = cContent[0];
        if(connectedVariable == "up_key"){
            Movement.setUpKey(chValue);
        }
        else if(connectedVariable == "right_key"){
            Movement.setRightKey(chValue);
        }
        else if(connectedVariable == "down_key"){
            Movement.setDownKey(chValue);
        }
        else if(connectedVariable == "left_key"){
            Movement.setLeftKey(chValue);
        }
        else if(connectedVariable == "jump_key"){
            Movement.setJumpKey(chValue);
        }
        else if(connectedVariable == "running_key"){
            Movement.setRunningKey(chValue);
        }
        else{
            success = false;
        }
    }

    if(success){
        return success;
    }
    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }

    success = true;
    double dValue = std::stod(cContent);
    short shValue = short(dValue);

    if(connectedVariable == "movement_type"){
        Movement.setMovementType(shValue);
    }
    else if(connectedVariable == "input_type"){
        Movement.setInputType(shValue);
    }
    else if(connectedVariable == "allowed_jumps"){
        Movement.setAllowedJumps(shValue);
    }
    else if(connectedVariable == "mouse_button"){
        Movement.setMouseButton(shValue);
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }

    success = true;

    if(connectedVariable == "jump_ cooldown_duration"){
        Movement.setJumpCooldownDuration(dValue);
    }
    else if(connectedVariable == "move_cooldown_duration"){
        Movement.setMoveCooldownDuration(dValue);
    }
    else if(connectedVariable == "body_mass"){
        Movement.setBodyMass(dValue);
    }
    else if(connectedVariable == "walking_speed"){
        Movement.setWalkingSpeed(dValue);
    }
    else if(connectedVariable == "running_speed"){
        Movement.setRunningSpeed(dValue);
    }
    else if(connectedVariable == "jump_speed"){
        Movement.setJumpSpeed(dValue);
    }
    else if(connectedVariable == "gravitation"){
        Movement.setGravitation(dValue);
    }
    else if(connectedVariable == "base_friction"){
        Movement.setBaseFriction(dValue);
    }
    else if(connectedVariable == "max_momentum_x"){
        Movement.setMaxMomentumX(dValue);
    }
    else if(connectedVariable == "min_momentum_y"){
        Movement.setMinMomentumY(dValue);
    }
    else if(connectedVariable == "max_momentum_y"){
        Movement.setMaxMomentumY(dValue);
    }
    else{
        success = false;
    }

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlCollision(CollisionModule & Collision, vector <string> & listOfIDs){
    bool success = true;

    string cContent = getContent(0);

    if(connectedVariable == "add_to_ignored_objects"){
        Collision.addToIgnoreList("objects", cContent);
    }
    else if(connectedVariable == "add_to_ignored_groups_of_objects"){
        Collision.addToIgnoreList("groups_of_objects", cContent);
    }
    else if(connectedVariable == "add_to_ignored_hitboxes"){
        Collision.addToIgnoreList("hitboxes", cContent);
    }
    else if(connectedVariable == "add_to_ignored_groups_of_hitboxes"){
        Collision.addToIgnoreList("groups_of_hitboxes", cContent);
    }
    else if(connectedVariable == "remove_from_ignored_objects"){
        Collision.removeFromIgnoreList("objects", cContent);
    }
    else if(connectedVariable == "remove_from_ignored_groups_of_objects"){
        Collision.removeFromIgnoreList("groups_of_objects", cContent);
    }
    else if(connectedVariable == "remove_from_ignored_hitboxes"){
        Collision.removeFromIgnoreList("hitboxes", cContent);
    }
    else if(connectedVariable == "remove_from_ignored_groups_of_hitboxes"){
        Collision.removeFromIgnoreList("groups_of_hitboxes", cContent);
    }
    else{
        success = false;
    }

    if(success || cContent == ""){
        return success;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Collision.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        success = true;
        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(connectedVariable == "is_solid"){
            Collision.setIsSolid(bValue);
        }
        else if(connectedVariable == "can_penetrate_solids"){
            Collision.setCanPenetrateSolids(bValue);
        }
        else{
            success = false;
        }
    }

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlParticles(ParticleEffectModule & Particles, vector <string> & listOfIDs){
    bool success = true;
    string cContent = getContent(0);

    if(connectedVariable == "spawn_key_bind"){
        if(cContent.size() != 1)
            return false;
        Particles.setSpawnKeyBind(cContent[0]);
        return true;
    }

    if(connectedVariable == "add_color_in_hex"){
        Particles.addColorInHex(cContent);
    }
    else if(connectedVariable == "add_color_interval_in_hex"){
        Particles.addColorIntervalInHex(cContent);
    }
    else if(connectedVariable == "add_image"){
        Particles.addImage(cContent);
    }
    else if(connectedVariable == "remove_image"){
        Particles.removeImage(cContent);
    }
    else{
        success = false;
    }

    if(success || cContent == ""){
        return success;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Particles.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        success = true;

        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(connectedVariable == "particles_moving"){
            Particles.setAreParticlesMoving(bValue);
        }
        else if(connectedVariable == "environment_synchronized"){
            Particles.setIsEnvironmentSynchronized(bValue);
        }
        else if(connectedVariable == "use_image_as_particles"){
            Particles.setUseImageAsParticles(bValue);
        }
        else if(connectedVariable == "use_random_colors"){
            Particles.setUseRandomColors(bValue);
        }
        else if(connectedVariable == "module_static"){
            Particles.setIsModuleStatic(bValue);
        }
        else if(connectedVariable == "drawing_with_details"){
            Particles.setIsDrawingWithDetails(bValue);
        }
        else if(connectedVariable == "block_particles_spawn"){
            Particles.setBlockParticlesSpawn(bValue);
        }
        else if(connectedVariable == "spawn_on_key_release"){
            Particles.setSpawnOnKeyRelease(bValue);
        }
        else{
            success = false;
        }
    }

    if(success){
        return success;
    }
    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }
    success = true;
    double dValue = std::stod(cContent);


    if(connectedVariable == "environment_x"){
        Particles.setEnvironmentX(dValue);
    }
    else if(connectedVariable == "environment_y"){
        Particles.setEnvironmentY(dValue);
    }
    else if(connectedVariable == "environment_speed_x"){
        Particles.setEnvironmentSpeedX(dValue);
    }
    else if(connectedVariable == "environment_speed_y"){
        Particles.setEnvironmentSpeedY(dValue);
    }
    else if(connectedVariable == "min_speed"){
        Particles.setMinSpeed(dValue);
    }
    else if(connectedVariable == "max_speed"){
        Particles.setMaxSpeed(dValue);
    }
    else if(connectedVariable == "min_basic_speed"){
        Particles.setMinBasicSpeed(dValue);
    }
    else if(connectedVariable == "max_basic_speed"){
        Particles.setMaxBasicSpeed(dValue);
    }
    else if(connectedVariable == "min_acceleration"){
        Particles.setMinAcceleration(dValue);
    }
    else if(connectedVariable == "max_acceleration"){
        Particles.setMaxAcceleration(dValue);
    }
    else if(connectedVariable == "min_particle_mass"){
        Particles.setMinParticleMass(dValue);
    }
    else if(connectedVariable == "max_particle_mass"){
        Particles.setMaxParticleMass(dValue);
    }
    else if(connectedVariable == "min_direction_degree"){
        Particles.setMinDirectionDegree(dValue);
    }
    else if(connectedVariable == "max_direction_degree"){
        Particles.setMaxDirectionDegree(dValue);
    }
    else if(connectedVariable == "min_rotation_speed"){
        Particles.setMinRotationSpeed(dValue);
    }
    else if(connectedVariable == "max_rotation_speed"){
        Particles.setMaxRotationSpeed(dValue);
    }
    else if(connectedVariable == "min_time_to_negate_rotation"){
        Particles.setMinTimeToNegateRotation(dValue);
    }
    else if(connectedVariable == "max_time_to_negate_rotation"){
        Particles.setMaxTimeToNegateRotation(dValue);
    }
    else if(connectedVariable == "min_particle_radius"){
        Particles.setMinParticleRadius(dValue);
    }
    else if(connectedVariable == "max_particle_radius"){
        Particles.setMaxParticleRadius(dValue);
    }
    else if(connectedVariable == "min_time_to_death"){
        Particles.setMinTimeToDeath(dValue);
    }
    else if(connectedVariable == "max_time_to_death"){
        Particles.setMaxTimeToDeath(dValue);
    }
    else if(connectedVariable == "min_shape_rotation_speed"){
        Particles.setMinShapeRotationSpeed(dValue);
    }
    else if(connectedVariable == "max_shape_rotation_speed"){
        Particles.setMaxShapeRotationSpeed(dValue);
    }
    else if(connectedVariable == "min_color_intensity"){
        Particles.setMinColorIntensity(dValue);
    }
    else if(connectedVariable == "max_color_intensity"){
        Particles.setMaxColorIntensity(dValue);
    }
    else if(connectedVariable == "max_time_to_spawn"){
        Particles.setMaxTimeToSpawn(dValue);
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }
    success = true;

    short shValue = short(dValue);
    if(connectedVariable == "particle_shape"){
        Particles.setParticlesShape(shValue);
    }
    else if(connectedVariable == "spawn_key_bind_short"){
        Particles.setSpawnKeyBindShort(shValue);
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }

    if(dValue >= 0){
        success = true;
        unsigned int uiValue = static_cast<unsigned int>(dValue);

        if(connectedVariable == "min_particles_per_spawn"){
            Particles.setMinParticlesPerSpawn(uiValue);
        }
        else if(connectedVariable == "max_particles_per_spawn"){
            Particles.setMaxParticlesPerSpawn(uiValue);
        }
        else if(connectedVariable == "max_particles_count"){
            Particles.setMaxParticlesCount(uiValue);
        }
        else if(connectedVariable == "remove_color"){
            Particles.removeColor(uiValue);
        }
        else if(connectedVariable == "remove_color_interval"){
            Particles.removeColorInterval(uiValue);
        }
        else if(connectedVariable == "remove_image"){
            Particles.removeImage(uiValue);
        }
        else{
            success = false;
        }
    }

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlVariable(VariableModule & Variable, vector <string> & listOfIDs){
    string cContent = getContent(0);

    if(connectedVariable == "default_string"){
        return Variable.setDefaultString(cContent);
    }
    else if(connectedVariable == "string"){
        return Variable.setString(cContent);
    }

    if(cContent == ""){
        return false;
    }

    if(connectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Variable.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
        return false;
    }

    if(connectedVariable == "type"){
        return Variable.setType(cContent[0]);
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(connectedVariable == "dafault_bool"){
            return Variable.setDefaultBool(bValue);
        }
        else if(connectedVariable == "bool"){
            return Variable.setBool(bValue);
        }
    }


    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }

    double dValue = std::stod(cContent);
    int iValue = int(dValue);

    if(connectedVariable == "default_int"){
        return Variable.setDefaultInt(iValue);
    }
    else if(connectedVariable == "default_double"){
        return Variable.setDefaultDouble(dValue);
    }
    else if(connectedVariable == "int"){
        return Variable.setInt(iValue);
    }
    else if(connectedVariable == "double"){
        return Variable.setDouble(dValue);
    }
    return false;
}
void printNotNumericalWarning(){
    cout << "Warning: You can't use text to change numerical variable!\n";
}
void printCommandDoesNotExistWarning(){
    cout << "Warning: Command doesn't exist!\n";
}
void EditableTextModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "content"){
        BasePointers.back().setPointer(&content[currentTextIdx]);
    }
    else if(attribute == "current_text_id"){
        BasePointers.back().setPointer(&currentTextIdx);
    }
    else if(attribute == "font_id"){
        BasePointers.back().setPointer(&fontID);
    }
    else if(attribute == "color_r"){
        BasePointers.back().setPointer(&color.r);
    }
    else if(attribute == "color_g"){
        BasePointers.back().setPointer(&color.g);
    }
    else if(attribute == "color_b"){
        BasePointers.back().setPointer(&color.b);
    }
    else if(attribute == "color_a"){
        BasePointers.back().setPointer(&color.a);
    }
    else if(attribute == "wrapped"){
        BasePointers.back().setPointer(&wrapped);
    }
    else if(attribute == "horizontal_align"){
        BasePointers.back().setPointer(&horizontalAlign);
    }
    else if(attribute == "vertical_align"){
        BasePointers.back().setPointer(&verticalAlign);
    }
    else if(attribute == "rotation"){
        BasePointers.back().setPointer(&rotation);
    }
    else if(attribute == "used_bitmap_layer"){
        BasePointers.back().setPointer(&usedBitmapLayer);
    }
    else if(attribute == "can_be_edited"){
        BasePointers.back().setPointer(&canBeEdited);
    }
    else if(attribute == "editing_is_active"){
        BasePointers.back().setPointer(&editingIsActive);
    }
    else if(attribute == "can_use_space"){
        BasePointers.back().setPointer(&canUseSpace);
    }
    else if(attribute == "is_numerical"){
        BasePointers.back().setPointer(&isNumerical);
    }
    else if(attribute == "has_floating_point"){
        BasePointers.back().setPointer(&hasFloatingPoint);
    }
    else if(attribute == "can_clear_content_after_success"){
        BasePointers.back().setPointer(&canClearContentAfterSuccess);
    }
    else if(attribute == "use_arrows_as_char"){
        BasePointers.back().setPointer(&useArrowsAsChar);
    }
    else if(attribute == "min_content_size"){
        BasePointers.back().setPointer(&minContentSize);
    }
    else if(attribute == "max_content_size"){
        BasePointers.back().setPointer(&maxContentSize);
    }
    else if(attribute == "connected_object"){
        BasePointers.back().setPointer(&connectedObject);
    }
    else if(attribute == "connected_group"){
        BasePointers.back().setPointer(&connectedGroup);
    }
    else if(attribute == "affected_module"){
        BasePointers.back().setPointer(&connectedModule);
    }
    else if(attribute == "connected_module_id"){
        BasePointers.back().setPointer(&connectedModuleID);
    }
    else if(attribute == "affected_variable"){
        BasePointers.back().setPointer(&connectedVariable);
    }
    else{
        getPrimaryContext(attribute, BasePointers);
    }
}
VariableModule EditableTextModule::getAttributeValue(const string &attribute, const string &detail) const{
    if(attribute == "in_group"){
        return VariableModule::newBool(isInAGroup(detail));
    }
    else if(attribute == "content"){
        return VariableModule::newString(getCurrentContent());
    }
    else if(attribute == "current_text_id"){
        return VariableModule::newInt(currentTextIdx);
    }
    else if(attribute == "font_id"){
        return VariableModule::newString(getCurrentContent());
    }
    else if(attribute == "color_r"){
        return VariableModule::newDouble(color.r);
    }
    else if(attribute == "color_g"){
        return VariableModule::newDouble(color.g);
    }
    else if(attribute == "color_b"){
        return VariableModule::newDouble(color.b);
    }
    else if(attribute == "color_a"){
        return VariableModule::newDouble(color.a);
    }
    else if(attribute == "wrapped"){
        return VariableModule::newInt(wrapped);
    }
    else if(attribute == "horizontal_align"){
        return VariableModule::newInt(horizontalAlign);
    }
    else if(attribute == "vertical_align"){
        return VariableModule::newInt(verticalAlign);
    }
    else if(attribute == "rotation"){
        return VariableModule::newDouble(rotation);
    }
    else if(attribute == "can_be_edited"){
        return VariableModule::newBool(getCanBeEdited());
    }
    else if(attribute == "editing"){
        return VariableModule::newBool(getEditingIsActive());
    }

    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    return VariableModule::newBool(false);
}