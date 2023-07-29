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
    textColor[0] = 255;
    textColor[1] = 255;
    textColor[2] = 255;
    fontID = -1;
    wrapped = false;
    horizontalAlign = 0;
    verticalAlign = 0;
    rotateAngle = 0.0;
    scale.set(1.0, 1.0);
    currentTextID = 0;
    visibility = 1.0;
    usedBitmapLayer = 0;
}
TextModule::TextModule(string textModuleID){
    primaryConstructor(textModuleID);
    setUpNewInstance();
}
TextModule::TextModule(unsigned int textModuleID){
    primaryConstructor(textModuleID);
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
void TextModule::chooseContent(unsigned int textID){
    if(textID < content.size())
        currentTextID = textID;
    else
        std::cout << "Bad index in textModule \"" << ID << "\".\n";
}
void TextModule::modifyContent(unsigned int textID, string modifiedContent){
    if(textID < content.size()){
        content[textID] = modifiedContent;
    }
    else
        std::cout << "Bad index in textModule \"" << ID << "\".\n";
}
void TextModule::modifyContentAndResize(unsigned int textID, string modifiedContent, vector <SingleFont> FontContainer){
    if(textID < content.size()){
        content[textID] = modifiedContent;
        fitSizeToText(FontContainer);
    }
    else
        std::cout << "Bad index in textModule \"" << ID << "\".\n";
}
void TextModule::deleteContent(unsigned int textID){
    if(textID < content.size()){
        content.erase(content.begin()+textID);
        if(currentTextID >= textID && currentTextID > 0)
            currentTextID--;
    }
    else
        std::cout << "Bad index in textModule \"" << ID << "\".\n";
}
void TextModule::clearAllContent(){
    content.clear();
    currentTextID = 0;
}
void TextModule::setColors(unsigned short red, unsigned short green, unsigned short blue){
    textColor[0] = red;
    textColor[1] = green;
    textColor[2] = blue;
}
void TextModule::setRandomColors(){
    textColor[0] = rand()%255;
    textColor[1] = rand()%255;
    textColor[2] = rand()%255;
}
void TextModule::setRotation(double newAngle){
    rotateAngle = newAngle;
}
void TextModule::setUsedBitmapLayer(int newLayer){
    usedBitmapLayer = newLayer;
}
void TextModule::addRotation(double newAngle){
    rotateAngle += newAngle;
}
void TextModule::setFontID(string newValue){
    fontID = newValue;
}
void TextModule::setVisibility(double newVisibility){
    visibility = newVisibility;
}
void TextModule::TextModule::changeParameters(string newID, vec4d posSize, vec3d fontColor, string newFontID, vec2d newScale,
                          double newRotateAngle, short newWrapped, int newHorizontalAlign, int newVerticalAlign){
    setID(newID);
    setPos(vec2d(posSize.val[0], posSize.val[1]));
    setSize(vec2d(posSize.val[2], posSize.val[3]));
    setScale(newScale);
    textColor[0] = (unsigned short)fontColor.val[0];
    textColor[1] = (unsigned short)fontColor.val[1];
    textColor[2] = (unsigned short)fontColor.val[2];
    fontID = newFontID;
    rotateAngle = newRotateAngle;
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
    int currentLength = 0, currentHeight = 0, realHeight = 0, cursorRealPos = 0;
    if(currentTextID < content.size()){
        text = content[currentTextID];
    }
    else{
        //text = "error";
        return;
    }

    if(cursorPos > text.size()){
        cursorPos = 0;
    }

    for(unsigned int letter = 0; letter < cursorPos; letter++){
        temp.clear();
        temp = text.substr(letter, 1);
        cursorRealPos += al_get_text_width(font, temp.c_str());
    }


    for(unsigned int letter = 0; letter < text.size(); letter++){
        temp.clear();
        temp = text.substr(letter, 1);
        if(temp == "\n"){ //enter handling
            currentLength = 0;
            currentHeight += al_get_font_line_height(font);
            if(currentHeight + al_get_font_line_height(font) > size.y)
                break;
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
                    currentHeight += al_get_font_line_height(font);
                    textLines.push_back(string());
                    success = true;
                    break;
                }
            }
            if(success)
                continue;
        }
        if(currentLength + al_get_text_width(font, temp.c_str()) > size.x){ //cropping text according to its dimensions and wrapping text when possible
            if(wrapped == 0)
                break;
            currentLength = 0;
            currentHeight += al_get_font_line_height(font);
            textLines.push_back(string());
            if(currentHeight + al_get_font_line_height(font) > size.y)
                break;
        }
        if(temp != " " || currentLength != 0){
            textLines[textLines.size()-1] = textLines[textLines.size()-1] + temp;
            currentLength += al_get_text_width(font, temp.c_str());
            realHeight = currentHeight;
        }
    }
    realHeight += al_get_font_line_height(font);
    int alignHeight = 0;
    if(verticalAlign == 1)
        alignHeight = size.y/2-realHeight/2;
    else if(verticalAlign == 2)
        alignHeight = size.y-realHeight;

    if(visibility < 0.0)
        visibility = 0.0;
    if(visibility > 1.0)
        visibility = 1.0;

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
    al_rotate_transform(&t, (rotateAngle*M_PI)/180.0);
    al_scale_transform(&t, newScale.x, newScale.y);
    al_translate_transform(&t, finalPos.x, finalPos.y);
    al_use_transform(&t);

    for(unsigned int i = 0; i < textLines.size(); i++){
        if(horizontalAlign == 0){
            al_draw_textf(font, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility),
                          finalPos.x, finalPos.y+alignHeight+i*al_get_font_line_height(font), 0, textLines[i].c_str());
            if(editingIsActive)
                al_draw_textf(font, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility),
                              finalPos.x+cursorRealPos, finalPos.y+alignHeight+i*al_get_font_line_height(font), 0, "|");
        }
        else if(horizontalAlign == 1){
            al_draw_textf(font, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility),
                          finalPos.x+size.x/2, finalPos.y+alignHeight+i*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, textLines[i].c_str());
            if(editingIsActive)
                al_draw_textf(font, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility),
                              finalPos.x+size.x/2+cursorRealPos, finalPos.y+alignHeight+i*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "|");
        }
        else if(horizontalAlign == 2){
            al_draw_textf(font, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility),
                          finalPos.x+size.x, finalPos.y+alignHeight+i*al_get_font_line_height(font), ALLEGRO_ALIGN_RIGHT, textLines[i].c_str());
            if(editingIsActive)
                al_draw_textf(font, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility),
                          finalPos.x+size.x+cursorRealPos, finalPos.y+alignHeight+i*al_get_font_line_height(font), ALLEGRO_ALIGN_RIGHT, "|");
        }

    }

    if(drawBorders){
        al_draw_rectangle(finalPos.x, finalPos.y, finalPos.x+size.x, finalPos.y+size.y, al_map_rgba(textColor[0]*visibility, textColor[1]*visibility, textColor[2]*visibility, 255*visibility), 1);
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
    if(currentTextID < content.size()){
        text = content[currentTextID];
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

        al_draw_textf(font, al_map_rgb(textColor[0], textColor[1], textColor[2]), pos.x+currentLength, pos.y+currentHigh, 0, temp.c_str());
        currentLength += al_get_text_width(font, temp.c_str());
    }
}
string TextModule::getFontID(){
    return fontID;
}
string TextModule::getContent(unsigned int textID){
    if(textID < content.size())
        return content[textID];
    else{
        std::cout << "Bad index in textModule \"" << ID << "\".\n";
        return "no";
    }
}
unsigned short TextModule::getColor(char whichColor){
    if(whichColor == 'r')
        return textColor[0];
    if(whichColor == 'g')
        return textColor[1];
    if(whichColor == 'b')
        return textColor[2];
    return 0;
}
unsigned int TextModule::getCurrentTextID(){
    return currentTextID;
}
string TextModule::getCurrentContent(){
    return getContent(getCurrentTextID());
}


void EditableTextModule::setUpEditableInstance(){
    isAttachedToCamera = true;
    canBeEdited = true;
    editingIsActive = false;
    canUseSpace = false;
    isNumerical = true;
    hasFloatingPoint = true;
    updateConnectedVariable = false;
    canClearContentAfterSuccess = false;
    useArrowsAsChar = false;
    cursorPos = 0;
    minContentSize = 0;
    maxContentSize = 20;
}
EditableTextModule::EditableTextModule(unsigned int EditableTextModuleID) : TextModule(EditableTextModuleID){
    setUpEditableInstance();
}
EditableTextModule::EditableTextModule(string EditableTextModuleID) : TextModule(EditableTextModuleID){
    setUpEditableInstance();
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
void EditableTextModule::setConnectedObjectID(string newValue){
    connectedObject = newValue;
}
void EditableTextModule::setConnectedObject(string objectID, string moduleType, string moduleID, string variableName){
    setConnectedObjectID(objectID);
    affectedModule = moduleType;
    connectedModuleID = moduleID;
    affectedVariable = variableName;
}
void EditableTextModule::setConnectedGroup(string groupName, string moduleType, string moduleID, string variableName){
    connectedGroup = groupName;
    affectedModule = moduleType;
    connectedModuleID = moduleID;
    affectedVariable = variableName;
}
bool EditableTextModule::getCanBeEdited(){
    return canBeEdited;
}
bool EditableTextModule::getEditingIsActive(){
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
void EditableTextModule::editText(vector <short> releasedKeys, vector <short> pressedKeys){
    if(!getIsActive()){
        return;
    }

    short special_key = 0;
    char character = '\0';
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(pressedKeys[i] == ALLEGRO_KEY_RSHIFT)
            special_key = pressedKeys[i];
        if(pressedKeys[i] == ALLEGRO_KEY_ENTER){
            editingIsActive = false;
            setUpdateConnectedVariable(true);
            return;
        }
    }
    for(unsigned int i = 0; i < releasedKeys.size(); i++){
        if(!useArrowsAsChar){
            if(releasedKeys[i] == ALLEGRO_KEY_RIGHT && cursorPos < content[currentTextID].size())
                cursorPos += 1;
            if(releasedKeys[i] == ALLEGRO_KEY_LEFT && cursorPos > 0)
                cursorPos -= 1;
        }
        else{
            if(releasedKeys[i] == ALLEGRO_KEY_UP)
                character = 24;
            if(releasedKeys[i] == ALLEGRO_KEY_RIGHT)
                character = 26;
            if(releasedKeys[i] == ALLEGRO_KEY_DOWN)
                character = 25;
            if(releasedKeys[i] == ALLEGRO_KEY_LEFT)
                character = 27;
        }


        if(releasedKeys[i] >= 27 && releasedKeys[i] <= 36)
            character = releasedKeys[i]+21;
        if(releasedKeys[i] == 73 && (getHasFloatingPoint() == true || getIsNumerical() == false)){
            string text = getContent(0);
            bool isTherePoint = false;
            if(getIsNumerical() == true){
                for(unsigned int i = 0; i < text.size(); i++){
                    if(text[i] == '.'){
                        isTherePoint = true;
                        break;
                    }
                }
            }
            if(isTherePoint == false)
                character = '.';
        }
        if(releasedKeys[i] == ALLEGRO_KEY_MINUS){
            if(getIsNumerical() == true){
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
                if(isThereMinus == true)
                    continue;
            }
            character = '-';
        }


        if(getIsNumerical() == false){
            if(releasedKeys[i] >= 1 && releasedKeys[i] <= 26 && special_key != ALLEGRO_KEY_RSHIFT)
                character = 'a'+releasedKeys[i]-1;
            else if(releasedKeys[i] >= 1 && releasedKeys[i] <= 26)
                character = 'A'+releasedKeys[i]-1;
            if(releasedKeys[i] == 61 && special_key == ALLEGRO_KEY_RSHIFT)
                character = '_';
            if(releasedKeys[i] == 72)
                character = ',';
            if(releasedKeys[i] == 75 && getCanUseSpace() == true)
                character = ' ';
        }


        if(character == '\0' || getContent(0).size() >= getMaxContentSize()){
            if(releasedKeys[i] == ALLEGRO_KEY_BACKSPACE && getContent(0).size() > getMinContentSize()){
                if(cursorPos == 0)
                    continue;
                string old_content = getContent(0);
                string new_content = old_content.substr(0, cursorPos-1) + old_content.substr(cursorPos, old_content.size()-cursorPos);
                modifyContent(0, new_content);
                cursorPos -= 1;
            }
            continue;
        }

        string old_content = getContent(0);
        string new_content = old_content.substr(0, cursorPos);
        new_content = new_content + character + old_content.substr(cursorPos, old_content.size()-cursorPos);


        modifyContent(0, new_content);
        cursorPos += 1;

        //std::cout << int(character) << " ";
        //std::cout << character << " (" << int(character) << ") " << " ";
    }
}
bool EditableTextModule::canConvertContentToNumber(){
    if(getIsNumerical() == false)
        return false;
    string text = getContent(0);
    bool isTherePoint = false;

    for(unsigned int i = 0; i < text.size(); i++){
        if(text[i] == '.' || text[i] == ' ' || text[i] == '_'){
            if(text[i] == '.' && isTherePoint == false){
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

    if(affectedVariable == "activate"){
        Primary.activate();
    }
    else if(affectedVariable == "deactivate"){
        Primary.deactivate();
    }
    else if(affectedVariable == "toggle_is_active"){
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

    if(affectedVariable == "id"){
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

        if(affectedVariable == "is_active"){
            Primary.setIsActive(bValue);
        }
        else if(affectedVariable == "is_scaled_from_center"){
            Primary.setIsScaledFromCenter(bValue);
        }
        else if(affectedVariable == "is_attached_to_camera"){
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

    if(affectedVariable == "position_x"){
        Primary.setPos(dValue, Primary.getPos(false).y);
    }
    else if(affectedVariable == "position_y"){
        Primary.setPos(Primary.getPos(false).x, dValue);
    }
    else if(affectedVariable == "size_x"){
        Primary.setSize(dValue, Primary.getSize().y);
    }
    else if(affectedVariable == "size_y"){
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

    if(affectedVariable == "activate"){
        Text.activate();
        return true;
    }
    if(affectedVariable == "deactivate"){
        Text.deactivate();
        return true;
    }

    string cContent = getContent(0);

    if(affectedVariable == "content"){
        Text.modifyContent(Text.getCurrentTextID(), cContent);
        success = true;
    }

    if(cContent.size() == 0){
        return success;
    }

    if(affectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Text.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
    }

    if(affectedVariable == "font"){
        setFontID(cContent);
        return true;
    }

    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }

    success = true;
    double dValue = std::stod(cContent);

    if(affectedVariable == "position_x"){
        Text.setPos(vec2d(dValue, Text.getPos(false).y));
    }
    else if(affectedVariable == "position_y"){
        Text.setPos(vec2d(Text.getPos(false).x, dValue));
    }
    else if(affectedVariable == "size_x"){
        Text.setSize(vec2d(dValue, Text.getSize().y));
    }
    else if(affectedVariable == "size_y"){
        Text.setSize(vec2d(Text.getSize().x, dValue));
    }
    else if(affectedVariable == "scale_x"){
        Text.setScale(vec2d(dValue, Text.getSize().y));
    }
    else if(affectedVariable == "scale_y"){
        Text.setScale(vec2d(Text.getSize().x, dValue));
    }
    else if(affectedVariable == "resize_x"){
        Text.resizeX(dValue);
    }
    else if(affectedVariable == "resize_y"){
        Text.resizeY(dValue);
    }
    else if(affectedVariable == "rotation"){
        Text.setRotation(dValue);
    }
    else if(affectedVariable == "visibility"){
        Text.setVisibility(dValue);
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }

    if(dValue >= 0){
        unsigned int uiValue = static_cast<unsigned int>(dValue);
        if(affectedVariable == "font_id"){
            Text.fontID = uiValue;
            success = true;
        }
        if(success){
            return success;
        }
    }

    success = true;
    short shValue = short(dValue);

    if(affectedVariable == "wrapped"){
        Text.wrapped = shValue;
    }
    else if(affectedVariable == "horizontal_align"){
        Text.horizontalAlign = shValue;
    }
    else if(affectedVariable == "vertical_align"){
        Text.verticalAlign = shValue;
    }
    else if(affectedVariable == "bitmap_layer"){
        Text.usedBitmapLayer = shValue;
    }
    else{
        success = false;
    }

    if(success || shValue > 255 || shValue < 0)
        return success;
    success = true;

    if(affectedVariable == "color_r"){
        Text.setColors(shValue, Text.getColor('g'), Text.getColor('b'));
    }
    else if(affectedVariable == "color_g"){
        Text.setColors(Text.getColor('r'), shValue, Text.getColor('b'));
    }
    else if(affectedVariable == "color_b"){
        Text.setColors(Text.getColor('r'), Text.getColor('g'), shValue);
    }
    else{
        success = false;
    }

    if(!success){
        printCommandDoesNotExistWarning();
    }

    return success;
}
bool EditableTextModule::controlImage(ImageModule & Image, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfIDs){
    bool success = false;

    string cContent = getContent(0);
    if(affectedVariable == "connect_bitmap"){
        Image.connectBitmap(BitmapContainer, cContent);
        success = true;
    }

    if(success || cContent == ""){
        return success;
    }

    if(affectedVariable == "id"){
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

        if(affectedVariable == "mirror_x"){
            Image.setMirrorX(bValue);
        }
        else if(affectedVariable == "mirror_y"){
            Image.setMirrorX(bValue);
        }
        else if(affectedVariable == "drawing"){
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

    if(affectedVariable == "position_x"){
        Image.setPos(vec2d(dValue, Image.getPos(false).y));
    }
    else if(affectedVariable == "position_y"){
        Image.setPos(vec2d(Image.getPos(false).x, dValue));
    }
    else if(affectedVariable == "size_x"){
        Image.setSize(vec2d(dValue, Image.getSize().y));
    }
    else if(affectedVariable == "size_y"){
        Image.setSize(vec2d(Image.getSize().x, dValue));
    }
    else if(affectedVariable == "scale_x"){
        Image.setScale(vec2d(dValue, Image.getScale().y));
    }
    else if(affectedVariable == "scale_y"){
        Image.setScale(vec2d(Image.getScale().x, dValue));
    }
    else if(affectedVariable == "resize_x"){
        Image.resizeX(dValue);
    }
    else if(affectedVariable == "resize_y"){
        Image.resizeY(dValue);
    }
    else if(affectedVariable == "layer"){
        Image.setUsedBitmapLayer(int(dValue));
    }
    else if(affectedVariable == "center_x"){
        Image.setCenterX(dValue);
    }
    else if(affectedVariable == "center_y"){
        Image.setCenterY(dValue);
    }
    else if(affectedVariable == "start_x"){
        Image.setStartX(dValue);
    }
    else if(affectedVariable == "start_y"){
        Image.setStartY(dValue);
    }
    else if(affectedVariable == "frame_size_x"){
        Image.setFrameSizeX(dValue);
    }
    else if(affectedVariable == "frame_size_y"){
        Image.setFrameSizeY(dValue);
    }
    else if(affectedVariable == "current_frame_x"){
        Image.setCurrentFrameX(int(dValue));
    }
    else if(affectedVariable == "current_frame_y"){
        Image.setCurrentFrameY(int(dValue));
    }
    else if(affectedVariable == "rotation"){
        Image.setRotation(dValue);
    }
    else if(affectedVariable == "image_color_r"){
        Image.setImageColor(float(dValue), 'r');
    }
    else if(affectedVariable == "image_color_g"){
        Image.setImageColor(float(dValue), 'g');
    }
    else if(affectedVariable == "image_color_b"){
        Image.setImageColor(float(dValue), 'b');
    }
    else if(affectedVariable == "image_color_a"){
        Image.setImageColor(float(dValue), 'a');
    }
    else if(affectedVariable == "light_r"){
        Image.setLightColor(float(dValue), 'r');
    }
    else if(affectedVariable == "light_g"){
        Image.setLightColor(float(dValue), 'g');
    }
    else if(affectedVariable == "light_b"){
        Image.setLightColor(float(dValue), 'b');
    }
    else if(affectedVariable == "light_l"){
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

    if(affectedVariable == "id"){
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

        if(affectedVariable == "can_move_diagonally"){
            Movement.setCanMoveDiagonally(bValue);
        }
        else if(affectedVariable == "reset_momentum_when_jumping"){
            Movement.setResetMomentumWhenJumping(bValue);
        }
        else if(affectedVariable == "move_on_mouse_release"){
            Movement.setMoveOnMouseRelease(bValue);
        }
        else if(affectedVariable == "reset_direction_after_collision"){
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
        if(affectedVariable == "up_key"){
            Movement.setUpKey(chValue);
        }
        else if(affectedVariable == "right_key"){
            Movement.setRightKey(chValue);
        }
        else if(affectedVariable == "down_key"){
            Movement.setDownKey(chValue);
        }
        else if(affectedVariable == "left_key"){
            Movement.setLeftKey(chValue);
        }
        else if(affectedVariable == "jump_key"){
            Movement.setJumpKey(chValue);
        }
        else if(affectedVariable == "running_key"){
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

    if(affectedVariable == "movement_type"){
        Movement.setMovementType(shValue);
    }
    else if(affectedVariable == "input_type"){
        Movement.setInputType(shValue);
    }
    else if(affectedVariable == "allowed_jumps"){
        Movement.setAllowedJumps(shValue);
    }
    else if(affectedVariable == "mouse_button"){
        Movement.setMouseButton(shValue);
    }
    else{
        success = false;
    }

    if(success){
        return success;
    }

    success = true;

    if(affectedVariable == "jump_ cooldown_duration"){
        Movement.setJumpCooldownDuration(dValue);
    }
    else if(affectedVariable == "move_cooldown_duration"){
        Movement.setMoveCooldownDuration(dValue);
    }
    else if(affectedVariable == "body_mass"){
        Movement.setBodyMass(dValue);
    }
    else if(affectedVariable == "walking_speed"){
        Movement.setWalkingSpeed(dValue);
    }
    else if(affectedVariable == "running_speed"){
        Movement.setRunningSpeed(dValue);
    }
    else if(affectedVariable == "jump_speed"){
        Movement.setJumpSpeed(dValue);
    }
    else if(affectedVariable == "gravitation"){
        Movement.setGravitation(dValue);
    }
    else if(affectedVariable == "base_friction"){
        Movement.setBaseFriction(dValue);
    }
    else if(affectedVariable == "max_momentum_x"){
        Movement.setMaxMomentumX(dValue);
    }
    else if(affectedVariable == "min_momentum_y"){
        Movement.setMinMomentumY(dValue);
    }
    else if(affectedVariable == "max_momentum_y"){
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

    if(affectedVariable == "add_to_ignored_objects"){
        Collision.addToIgnoreList("objects", cContent);
    }
    else if(affectedVariable == "add_to_ignored_groups_of_objects"){
        Collision.addToIgnoreList("groups_of_objects", cContent);
    }
    else if(affectedVariable == "add_to_ignored_hitboxes"){
        Collision.addToIgnoreList("hitboxes", cContent);
    }
    else if(affectedVariable == "add_to_ignored_groups_of_hitboxes"){
        Collision.addToIgnoreList("groups_of_hitboxes", cContent);
    }
    else if(affectedVariable == "remove_from_ignored_objects"){
        Collision.removeFromIgnoreList("objects", cContent);
    }
    else if(affectedVariable == "remove_from_ignored_groups_of_objects"){
        Collision.removeFromIgnoreList("groups_of_objects", cContent);
    }
    else if(affectedVariable == "remove_from_ignored_hitboxes"){
        Collision.removeFromIgnoreList("hitboxes", cContent);
    }
    else if(affectedVariable == "remove_from_ignored_groups_of_hitboxes"){
        Collision.removeFromIgnoreList("groups_of_hitboxes", cContent);
    }
    else{
        success = false;
    }

    if(success || cContent == ""){
        return success;
    }

    if(affectedVariable == "id"){
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

        if(affectedVariable == "is_solid"){
            Collision.setIsSolid(bValue);
        }
        else if(affectedVariable == "can_penetrate_solids"){
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

    if(affectedVariable == "spawn_key_bind"){
        if(cContent.size() != 1)
            return false;
        Particles.setSpawnKeyBind(cContent[0]);
        return true;
    }

    if(affectedVariable == "add_color_in_hex"){
        Particles.addColorInHex(cContent);
    }
    else if(affectedVariable == "add_color_interval_in_hex"){
        Particles.addColorIntervalInHex(cContent);
    }
    else if(affectedVariable == "add_image"){
        Particles.addImage(cContent);
    }
    else if(affectedVariable == "remove_image"){
        Particles.removeImage(cContent);
    }
    else{
        success = false;
    }

    if(success || cContent == ""){
        return success;
    }

    if(affectedVariable == "id"){
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

        if(affectedVariable == "particles_moving"){
            Particles.setAreParticlesMoving(bValue);
        }
        else if(affectedVariable == "environment_synchronized"){
            Particles.setIsEnvironmentSynchronized(bValue);
        }
        else if(affectedVariable == "use_image_as_particles"){
            Particles.setUseImageAsParticles(bValue);
        }
        else if(affectedVariable == "use_random_colors"){
            Particles.setUseRandomColors(bValue);
        }
        else if(affectedVariable == "module_static"){
            Particles.setIsModuleStatic(bValue);
        }
        else if(affectedVariable == "drawing_with_details"){
            Particles.setIsDrawingWithDetails(bValue);
        }
        else if(affectedVariable == "block_particles_spawn"){
            Particles.setBlockParticlesSpawn(bValue);
        }
        else if(affectedVariable == "spawn_on_key_release"){
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


    if(affectedVariable == "environment_x"){
        Particles.setEnvironmentX(dValue);
    }
    else if(affectedVariable == "environment_y"){
        Particles.setEnvironmentY(dValue);
    }
    else if(affectedVariable == "environment_speed_x"){
        Particles.setEnvironmentSpeedX(dValue);
    }
    else if(affectedVariable == "environment_speed_y"){
        Particles.setEnvironmentSpeedY(dValue);
    }
    else if(affectedVariable == "min_speed"){
        Particles.setMinSpeed(dValue);
    }
    else if(affectedVariable == "max_speed"){
        Particles.setMaxSpeed(dValue);
    }
    else if(affectedVariable == "min_basic_speed"){
        Particles.setMinBasicSpeed(dValue);
    }
    else if(affectedVariable == "max_basic_speed"){
        Particles.setMaxBasicSpeed(dValue);
    }
    else if(affectedVariable == "min_acceleration"){
        Particles.setMinAcceleration(dValue);
    }
    else if(affectedVariable == "max_acceleration"){
        Particles.setMaxAcceleration(dValue);
    }
    else if(affectedVariable == "min_particle_mass"){
        Particles.setMinParticleMass(dValue);
    }
    else if(affectedVariable == "max_particle_mass"){
        Particles.setMaxParticleMass(dValue);
    }
    else if(affectedVariable == "min_direction_degree"){
        Particles.setMinDirectionDegree(dValue);
    }
    else if(affectedVariable == "max_direction_degree"){
        Particles.setMaxDirectionDegree(dValue);
    }
    else if(affectedVariable == "min_rotation_speed"){
        Particles.setMinRotationSpeed(dValue);
    }
    else if(affectedVariable == "max_rotation_speed"){
        Particles.setMaxRotationSpeed(dValue);
    }
    else if(affectedVariable == "min_time_to_negate_rotation"){
        Particles.setMinTimeToNegateRotation(dValue);
    }
    else if(affectedVariable == "max_time_to_negate_rotation"){
        Particles.setMaxTimeToNegateRotation(dValue);
    }
    else if(affectedVariable == "min_particle_radius"){
        Particles.setMinParticleRadius(dValue);
    }
    else if(affectedVariable == "max_particle_radius"){
        Particles.setMaxParticleRadius(dValue);
    }
    else if(affectedVariable == "min_time_to_death"){
        Particles.setMinTimeToDeath(dValue);
    }
    else if(affectedVariable == "max_time_to_death"){
        Particles.setMaxTimeToDeath(dValue);
    }
    else if(affectedVariable == "min_shape_rotation_speed"){
        Particles.setMinShapeRotationSpeed(dValue);
    }
    else if(affectedVariable == "max_shape_rotation_speed"){
        Particles.setMaxShapeRotationSpeed(dValue);
    }
    else if(affectedVariable == "min_color_intensity"){
        Particles.setMinColorIntensity(dValue);
    }
    else if(affectedVariable == "max_color_intensity"){
        Particles.setMaxColorIntensity(dValue);
    }
    else if(affectedVariable == "max_time_to_spawn"){
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
    if(affectedVariable == "particle_shape"){
        Particles.setParticlesShape(shValue);
    }
    else if(affectedVariable == "spawn_key_bind_short"){
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

        if(affectedVariable == "min_particles_per_spawn"){
            Particles.setMinParticlesPerSpawn(uiValue);
        }
        else if(affectedVariable == "max_particles_per_spawn"){
            Particles.setMaxParticlesPerSpawn(uiValue);
        }
        else if(affectedVariable == "max_particles_count"){
            Particles.setMaxParticlesCount(uiValue);
        }
        else if(affectedVariable == "remove_color"){
            Particles.removeColor(uiValue);
        }
        else if(affectedVariable == "remove_color_interval"){
            Particles.removeColorInterval(uiValue);
        }
        else if(affectedVariable == "remove_image"){
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

    if(affectedVariable == "default_string"){
        return Variable.setDefaultString(cContent);
    }
    else if(affectedVariable == "string"){
        return Variable.setString(cContent);
    }

    if(cContent == ""){
        return false;
    }

    if(affectedVariable == "id"){
        if(tryUpdatingID(listOfIDs, Variable.getIDAddr(), cContent)){
            connectedModuleID = cContent;
            return true;
        }
        return false;
    }

    if(affectedVariable == "type"){
        return Variable.setType(cContent[0]);
    }

    if(cContent == "true" || cContent == "1" || cContent == "false" || cContent == "0"){
        bool bValue;
        if(cContent == "true" || cContent == "1")
            bValue = true;
        if(cContent == "false" || cContent == "0")
            bValue = false;

        if(affectedVariable == "dafault_bool"){
            return Variable.setDefaultBool(bValue);
        }
        else if(affectedVariable == "bool"){
            return Variable.setBool(bValue);
        }
    }


    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }

    double dValue = std::stod(cContent);
    int iValue = int(dValue);

    if(affectedVariable == "default_int"){
        return Variable.setDefaultInt(iValue);
    }
    else if(affectedVariable == "default_double"){
        return Variable.setDefaultDouble(dValue);
    }
    else if(affectedVariable == "int"){
        return Variable.setInt(iValue);
    }
    else if(affectedVariable == "double"){
        return Variable.setDouble(dValue);
    }
    return false;
}
void printNotNumericalWarning(){
    std::cout << "Warning: You can't use text to change numerical variable!\n";
}
void printCommandDoesNotExistWarning(){
    std::cout << "Warning: Command doesn't exist!\n";
}

void EditableTextModule::bindEditableToVariable(string attribute, BasePointersStruct & UniversalVariable){
    if(attribute == "content"){
        UniversalVariable.getPointer(&content[currentTextID]);
    }
    else if(attribute == "current_text_id"){
        UniversalVariable.getPointer(&currentTextID);
    }
    else if(attribute == "font_id"){
        UniversalVariable.getPointer(&fontID);
    }
    else if(attribute == "text_color_r"){
        UniversalVariable.getPointer(&textColor[0]);
    }
    else if(attribute == "text_color_g"){
        UniversalVariable.getPointer(&textColor[1]);
    }
    else if(attribute == "text_color_b"){
        UniversalVariable.getPointer(&textColor[2]);
    }
    else if(attribute == "wrapped"){
        UniversalVariable.getPointer(&wrapped);
    }
    else if(attribute == "horizontal_align"){
        UniversalVariable.getPointer(&horizontalAlign);
    }
    else if(attribute == "vertical_align"){
        UniversalVariable.getPointer(&verticalAlign);
    }
    else if(attribute == "rotate_angle"){
        UniversalVariable.getPointer(&rotateAngle);
    }
    else if(attribute == "visibility"){
        UniversalVariable.getPointer(&visibility);
    }
    else if(attribute == "used_bitmap_layer"){
        UniversalVariable.getPointer(&usedBitmapLayer);
    }
    else if(attribute == "can_be_edited"){
        UniversalVariable.getPointer(&canBeEdited);
    }
    else if(attribute == "editing_is_active"){
        UniversalVariable.getPointer(&editingIsActive);
    }
    else if(attribute == "can_use_space"){
        UniversalVariable.getPointer(&canUseSpace);
    }
    else if(attribute == "is_numerical"){
        UniversalVariable.getPointer(&isNumerical);
    }
    else if(attribute == "has_floating_point"){
        UniversalVariable.getPointer(&hasFloatingPoint);
    }
    else if(attribute == "can_clear_content_after_success"){
        UniversalVariable.getPointer(&canClearContentAfterSuccess);
    }
    else if(attribute == "use_arrows_as_char"){
        UniversalVariable.getPointer(&useArrowsAsChar);
    }
    else if(attribute == "min_content_size"){
        UniversalVariable.getPointer(&minContentSize);
    }
    else if(attribute == "max_content_size"){
        UniversalVariable.getPointer(&maxContentSize);
    }
    else if(attribute == "connected_object"){
        UniversalVariable.getPointer(&connectedObject);
    }
    else if(attribute == "connected_group"){
        UniversalVariable.getPointer(&connectedGroup);
    }
    else if(attribute == "affected_module"){
        UniversalVariable.getPointer(&affectedModule);
    }
    else if(attribute == "connected_module_id"){
        UniversalVariable.getPointer(&connectedModuleID);
    }
    else if(attribute == "affected_variable"){
        UniversalVariable.getPointer(&affectedVariable);
    }
    else{
        bindPrimaryToVariable(attribute, UniversalVariable);
    }
}