#include "textModule.h"

SingleFont * findFontByID(vector <SingleFont> & FontContainer, string fontID){
    for(SingleFont & font : FontContainer){
        if(fontID == font.ID){
            if(!font.font){
                cout << "Error: In " << __FUNCTION__ << ": Font '" << fontID << "' does not exist.\n";
                return nullptr;
            }
            return &font;
        }
    }
    cout << "Error: In " << __FUNCTION__ << ": Font '" << fontID << "' does not exist.\n";
    return nullptr;
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
    tabLength = 4;
    cursorPos = 0;
    secondCursorPos = 0;
    ignoreSize = false;
    ignoreVerticalArrows = false;
}
TextModule::TextModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
TextModule::TextModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
TextModule::TextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
TextModule::~TextModule(){

}
void TextModule::clone(const TextModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool &changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}
void TextModule::adjustCursorPos(){
    if(cursorPos < 0){
        cursorPos = 0;
    }
    if(secondCursorPos < 0){
        secondCursorPos = 0;
    }
    if(content.size() == 0 || currentTextIdx > content.size()){
        cursorPos = 0;
        secondCursorPos = cursorPos;
        return;
    }
    if(cursorPos > content[currentTextIdx].size()){
        cursorPos = content[currentTextIdx].size();
    }
    if(secondCursorPos > content[currentTextIdx].size()){
        secondCursorPos = content[currentTextIdx].size();
    }
}
void TextModule::addNewContent(string newContent){
    content.push_back(newContent);
    adjustCursorPos();
}
void TextModule::fitSizeToText(vector <SingleFont> FontContainer){
    SingleFont * Font = findFontByID(FontContainer, fontID);
    if(Font == nullptr){
        return;
    }
    if(Font->sizeF < 0 || Font->sizeF > 1000){
        cout << "Error: In " << __FUNCTION__ << ": Font size equal to " << Font->sizeF << " is not a valid size.\n";
    }
    vec2d newSize(0.0, al_get_font_line_height(Font->font));
    for(string text : content){
        if(newSize.x < al_get_text_width(Font->font, text.c_str())){
            newSize.x = al_get_text_width(Font->font, text.c_str());
        }
    }
    size.set(newSize);
    adjustCursorPos();
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
    adjustCursorPos();
}
void TextModule::addTextToContent(unsigned int textNumber, string newText){
    if(!checkSize(textNumber)){
        return;
    }
    content[textNumber] += newText;
    adjustCursorPos();
}
void TextModule::modifyContent(unsigned int textNumber, string modifiedContent){
    if(!checkSize(textNumber)){
        return;
    }
    content[textNumber] = modifiedContent;
    adjustCursorPos();
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
    adjustCursorPos();
}
void TextModule::cutCurrentContent(size_t newSize){
    content[currentTextIdx] = content[currentTextIdx].substr(0, newSize);
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
void TextModule::setCursorPos(unsigned int newCursorPos){
    cursorPos = newCursorPos;
    secondCursorPos = newCursorPos;
    adjustCursorPos();
}
void TextModule::setSecondCursorPos(unsigned int newCursorPos){
    secondCursorPos = newCursorPos;
    adjustCursorPos();
}
void TextModule::setCursorPos(vec2d finalPos, vec2d finalSize, const vector<SingleFont> & FontContainer, const MouseClass & Mouse, const Camera2D & Camera){
    cursorPos = 0;
    unsigned columns = 1;
    string text = getCurrentContent();
    for(cursorPos = 0; cursorPos < text.size(); cursorPos++){
        if(text[cursorPos] == '\n'){
            columns++;
        }
    }
    const ALLEGRO_FONT * font = nullptr;
    for(const SingleFont & checkFont : FontContainer){
        if(getFontID() == checkFont.ID){
            font = checkFont.font;
            break;
        }
    }
    if(font == nullptr){
        cursorPos = text.size();
        return;
    }
    if(finalPos.y + al_get_font_line_height(font) * columns < Mouse.getPos().y){
        cursorPos = text.size();
        return;
    }
    columns = (Mouse.getPos().y - finalPos.y) / double(al_get_font_line_height(font));
    if(columns < 0){
        cursorPos = text.size();
        return;
    }
    for(cursorPos = 0; columns > 0 && cursorPos < text.size(); cursorPos++){
        if(text[cursorPos] == '\n'){
            columns--;
        }
    }
    unsigned rowLength = 0, tabCounter = 0, i, rowCursor = cursorPos, onlyTabs = 0, realWidth = 0;
    for(; rowCursor < text.size(); rowCursor++, tabCounter++, rowLength++){
        if(text[rowCursor] == '\t'){
            for(i = 0; i < tabLength - tabCounter % tabLength; i++){
                realWidth += al_get_text_width(font, string(" ").c_str());
            }
            rowLength += getCurrentTabLength(tabCounter);
            onlyTabs += getCurrentTabLength(tabCounter);
            if(tabLength > 0){
                tabCounter += getCurrentTabLength(tabCounter);
            }
        }
        else{
            realWidth += al_get_text_width(font, text.substr(rowCursor, 1).c_str());
        }
        if(text[rowCursor] == '\n'){
            break;
        }
    }
    if(Mouse.getPos().x < finalPos.x){
        return;
    }
    if(rowCursor == text.size()){
        realWidth += al_get_text_width(font, string(" ").c_str());
    }
    rowLength++;
    
    unsigned place = 0;
    if(finalPos.x + realWidth <= Mouse.getPos().x){
        place = rowLength - 1;
    }
    else{
        place = (double(rowLength) * (Mouse.getPos().x - finalPos.x)) / double(realWidth);
    }

    tabCounter = 0;
    for(; place > 0; place--, tabCounter++){
        if(text[cursorPos] == '\t'){
            if(place <= getCurrentTabLength(tabCounter)){
                break;
            }
            else{
                place -= getCurrentTabLength(tabCounter);
                tabCounter += getCurrentTabLength(tabCounter);
            }
        }
        cursorPos++;
    }
}
void TextModule::drawText(vec2d base, ALLEGRO_FONT * font, bool drawBorders, Camera2D Camera, unsigned cursorPos, unsigned secondCursorPos, bool editingIsActive){
    if(!getIsActive()){
        return;
    }

    vector <string> textLines;
    textLines.push_back(string());
    string temp, text;
    int currentLength = 0, currentHeight = 0, realHeight = 0;
    int fontHeight = al_get_font_line_height(font);
    int charLength = 0, smartCharLength = 0;
    unsigned letter = 0, lettersCountForTabs = 0;
    if(currentTextIdx < content.size()){
        text = content[currentTextIdx];
    }
    else{
        return;
    }

    for(; letter < text.size(); letter++, lettersCountForTabs++){
        temp.clear();
        temp = text.substr(letter, 1);
        if(temp == "\n"){ //enter handling
            currentLength = 0;
            currentHeight += fontHeight;
            if(!ignoreSize && currentHeight + fontHeight > size.y){
                break;
            }
            textLines.back() += '\n';
            lettersCountForTabs = -1;
            textLines.push_back(string());
            continue;
        }
        if((temp == " " || temp == "\t") && wrapped == 2){ //smart text wrapping - keeping words intact
            int futureLenght = currentLength;
            string newTemp;
            bool success = false;

            for(unsigned i = letter, j = lettersCountForTabs; i < text.size(); i++, j++){
                newTemp.clear();
                newTemp = text.substr(i, 1);

                if(i != letter && (newTemp == " " || newTemp == "\n" || newTemp == "\t")){
                    break;
                }
                smartCharLength = al_get_text_width(font, newTemp.c_str());
                if(newTemp == "\t"){
                    smartCharLength = al_get_text_width(font, string(" ").c_str()) * (tabLength - j % tabLength);
                    if(tabLength > 0){
                        j += tabLength - j % tabLength - 1;
                    }
                }
                futureLenght += smartCharLength;
                if(!ignoreSize && futureLenght > size.x){
                    currentLength = 0;
                    currentHeight += fontHeight;
                    textLines.back() += '\n';
                    lettersCountForTabs = -1;
                    textLines.push_back(string());
                    success = true;
                    break;
                }
            }
            if(success){
                continue;
            }
        }
        charLength = al_get_text_width(font, temp.c_str());
        if(temp == "\t"){
            charLength = al_get_text_width(font, string(" ").c_str()) * (tabLength - lettersCountForTabs % tabLength);
            if(tabLength > 0){
                lettersCountForTabs += tabLength - lettersCountForTabs % tabLength - 1;
            }
        }
        //cropping text according to its dimensions and wrapping text when possible
        if(!ignoreSize && currentLength + charLength > size.x){
            if(wrapped == 0){
                break;
            }
            currentLength = 0;
            currentHeight += fontHeight;
            lettersCountForTabs = -1;
            textLines.push_back(string());
            if(!ignoreSize && currentHeight + fontHeight > size.y){
                break;
            }
        }
        textLines[textLines.size()-1] = textLines[textLines.size()-1] + temp;
        currentLength += charLength;
        
        realHeight = currentHeight;
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

    unsigned cursorLine = 0;

    unsigned cursorStart = 0, cursorEnd = 0;
    if(editingIsActive){
        cursorStart = std::min(cursorPos, secondCursorPos);
        cursorEnd = std::max(cursorPos, secondCursorPos);
    }

    if(editingIsActive){
        if(cursorPos > text.size()){
            cursorPos = 0;
        }
        else{
            unsigned countedChars = 0;
            for(cursorLine = 0; cursorLine < textLines.size(); cursorLine++){
                for(letter = 0, lettersCountForTabs = 0; letter < textLines[cursorLine].size()
                    && countedChars < cursorPos; letter++, countedChars++, lettersCountForTabs++
                ){
                    temp.clear();
                    temp = textLines[cursorLine].substr(letter, 1);
                    if(temp != "\t"){
                        cursorRealPos.x += al_get_text_width(font, temp.c_str());
                    }
                    else{
                        cursorRealPos.x += al_get_text_width(font, string(" ").c_str()) * (tabLength - lettersCountForTabs % tabLength);
                        if(tabLength > 0){
                            lettersCountForTabs += tabLength - lettersCountForTabs % tabLength - 1;
                        }
                    }
                }
                if(countedChars >= cursorPos){
                    if(textLines[cursorLine].size() > 0 && ((letter == textLines[cursorLine].size()-1
                        && textLines[cursorLine][letter] == '\n' && cursorRealPos.x + al_get_text_width(font, string(" ").c_str()) >= size.x)
                        || (letter == textLines[cursorLine].size() && (textLines[cursorLine][letter-1] == '\n'
                        /*|| al_get_text_width(font, (textLines[cursorLine]+" ").c_str()) > size.x*/))))
                    {
                        cursorRealPos.x = finalPos.x;
                        cursorRealPos.y += fontHeight;
                        cursorLine++;
                        letter = 0;
                    }
                    if(horizontalAlign == 2 && letter == textLines[cursorLine].size()){
                        if(cursorLine == textLines.size()-1 && letter > 0){
                            letter--;
                            cursorRealPos.x -= al_get_text_width(font, textLines[cursorLine].substr(letter, 1).c_str());
                        }
                        else{
                            cursorRealPos.x -= al_get_text_width(font, string(" ").c_str());
                        }
                    }
                    if(cursorLine < textLines.size() && letter < textLines[cursorLine].size()){
                        letterOnCursor = textLines[cursorLine].substr(letter, 1);
                        if(letterOnCursor == "\t" || letterOnCursor == "\n"){
                            letterOnCursor = " ";
                        }
                    }
                    break;
                }
                cursorRealPos.x = finalPos.x;
                cursorRealPos.y += fontHeight;
                lettersCountForTabs = -1;
            }
        }
    }

    vector <string> finalTextLines;
    vector <string> selectionBackgroundLines;
    vector <string> selectionForegroundLines;
    unsigned tmpCursor = 0;
    for(string & line : textLines){
        finalTextLines.push_back("");
        if(editingIsActive && cursorStart != cursorEnd){
            selectionBackgroundLines.push_back("");
            selectionForegroundLines.push_back("");
        }
        for(letter = 0, lettersCountForTabs = 0; letter < line.size(); letter++, lettersCountForTabs++, tmpCursor++){
            if(line[letter] == '\n'){
                continue;
            }
            if(line[letter] == '\t'){
                for(unsigned tabN = 0; tabN < (tabLength - lettersCountForTabs % tabLength); tabN++){
                    finalTextLines.back() += " ";
                    if(editingIsActive && cursorStart != cursorEnd){
                        selectionForegroundLines.back() += " ";
                        if(tmpCursor < cursorStart || tmpCursor > cursorEnd){
                            selectionBackgroundLines.back() += " ";
                        }
                        else{
                            selectionBackgroundLines.back() += "█";
                        }
                    }
                }
                if(tabLength > 0){
                    lettersCountForTabs += tabLength - lettersCountForTabs % tabLength - 1;
                }
                continue;
            }
            finalTextLines.back() += line[letter];
            if(editingIsActive && cursorStart != cursorEnd){
                if(tmpCursor < cursorStart || tmpCursor > cursorEnd){
                    selectionBackgroundLines.back() += " ";
                    selectionForegroundLines.back() += " ";
                }
                else{
                    selectionBackgroundLines.back() += "█";
                    selectionForegroundLines.back() += line[letter];
                }
            }
        }
    }

    ALLEGRO_COLOR inversedColor = al_map_rgba_f(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);

    if(horizontalAlign == 0){
        if(editingIsActive){
            if(cursorStart == cursorEnd){
                al_draw_text(font, color, cursorRealPos.x, alignHeight + cursorRealPos.y, 0, "█");
            }
            else{
                for(letter = 0; letter < selectionBackgroundLines.size(); letter++){
                    al_draw_text(font, color, finalPos.x, finalPos.y + alignHeight + letter * fontHeight, 0, selectionBackgroundLines[letter].c_str());
                }
            }
        }
        for(letter = 0; letter < finalTextLines.size(); letter++){
            al_draw_text(font, color, finalPos.x, finalPos.y + alignHeight + letter * fontHeight, 0, finalTextLines[letter].c_str());
        }
        if(editingIsActive){
            if(cursorStart == cursorEnd){
                al_draw_text(font, inversedColor, cursorRealPos.x, alignHeight + cursorRealPos.y, 0, letterOnCursor.c_str());
            }
            else{
                for(letter = 0; letter < selectionForegroundLines.size(); letter++){
                    al_draw_text(font, inversedColor, finalPos.x, finalPos.y + alignHeight + letter * fontHeight, 0, selectionForegroundLines[letter].c_str());
                }
            }
        }
    }
    else if(horizontalAlign == 1){
        if(editingIsActive){
            if(cursorLine < finalTextLines.size()){
                al_draw_text(font, color, size.x / 2 + cursorRealPos.x - al_get_text_width(font, (finalTextLines[cursorLine]).c_str()) / 2, alignHeight + cursorRealPos.y, 0, "█");
            }
            else{
                al_draw_text(font, color, size.x / 2 + cursorRealPos.x - al_get_text_width(font, string(" ").c_str()) / 2, alignHeight + cursorRealPos.y, 0, "█");
            }
        }
        for(letter = 0; letter < finalTextLines.size(); letter++){
            al_draw_text(font, color, finalPos.x + size.x / 2, finalPos.y + alignHeight + letter * fontHeight, ALLEGRO_ALIGN_CENTRE, finalTextLines[letter].c_str());
        }
        if(editingIsActive && cursorLine < finalTextLines.size()){
            al_draw_text(font, inversedColor, size.x / 2 + cursorRealPos.x - al_get_text_width(font, (finalTextLines[cursorLine]).c_str()) / 2, alignHeight + cursorRealPos.y, 0, letterOnCursor.c_str());
        }
    }
    else if(horizontalAlign == 2){
        if(editingIsActive){
            if(cursorLine < finalTextLines.size()){
                al_draw_text(font, color, size.x + cursorRealPos.x - al_get_text_width(font, (finalTextLines[cursorLine]).c_str()),
                    alignHeight + cursorRealPos.y, 0, "█");
            }
            else{
                al_draw_text(font, color, size.x + cursorRealPos.x - al_get_text_width(font, string(" ").c_str()), alignHeight + cursorRealPos.y, 0, "█");
            }
        }
        for(letter = 0; letter < finalTextLines.size(); letter++){
            al_draw_text(font, color, finalPos.x + size.x, finalPos.y + alignHeight + letter * fontHeight, ALLEGRO_ALIGN_RIGHT, finalTextLines[letter].c_str());
        }
        if(editingIsActive && cursorLine < finalTextLines.size()){
            al_draw_text(font, inversedColor, size.x + cursorRealPos.x - al_get_text_width(font, (finalTextLines[cursorLine]).c_str()), alignHeight + cursorRealPos.y, 0, letterOnCursor.c_str());
        }
    }

    if(drawBorders){
        al_draw_rectangle(finalPos.x, finalPos.y, finalPos.x+size.x, finalPos.y+size.y, color, 1);
    }

    textLines.clear();
    finalTextLines.clear();

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
unsigned TextModule::getCurrentTabLength(const unsigned & tabCounter){
    return tabLength - tabCounter % tabLength - 1;
}

void EditableTextModule::setUpNewInstance(){
    TextModule::setUpNewInstance();
    canBeEdited = true;
    editingIsActive = false;
    canUseSpace = false;
    canUseEnter = false;
    isNumerical = false;
    hasFloatingPoint = false;
    updateConnectedVariable = false;
    canClearContentAfterSuccess = false;
    enterAcceptsChanges = false;
    useTabs = false;
    cursorPos = 0;
    secondCursorPos = 0;
    minContentSize = 0;
    maxContentSize = 20;
    inputDelay = 1.0;
    repetitionDelay = 0.5;
    currentInputDelay = 0.0;
    lastInputedKey = -1;
    protectedArea = 0;
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
void EditableTextModule::setCanUseEnter(bool newCanUseEnter){
    canUseEnter = newCanUseEnter;
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
void EditableTextModule::setUseTabs(bool newValue){
    useTabs = newValue;
}
void EditableTextModule::setCanClearContentAfterSuccess(bool newValue){
    canClearContentAfterSuccess = newValue;
}
void EditableTextModule::setCanEnterAcceptChanges(bool newValue){
    enterAcceptsChanges = newValue;
}
void EditableTextModule::setProtectedArea(unsigned cursor){
    protectedArea = cursor;
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
bool EditableTextModule::getCanUseEnter(){
    return canUseEnter;
}
unsigned int EditableTextModule::getCursorPos()
{
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
string EditableTextModule::getConnectedObjectID(){
    return connectedObject;
}
void EditableTextModule::clearContentAfterSuccess(bool success){
    if(!canClearContentAfterSuccess || !success){
        return;
    }
    modifyContent(currentTextIdx, "");
    setCursorPos(0);
}
char toChara(short a){
    return 'a'+a-1;
}
void EditableTextModule::moveCursorUp(const string & text, bool shift){
    //From cursor go back to the beginning of the current line.
    //If cursor is in the first line, do nothing.
    //If not, count the distance from the beginning of previous line to the cursor.
    //Count the length of the new line.
    //Add the smaller length to the cursor position at the beginning of the new line.
    unsigned newCursor = cursorPos;
    bool cursorOnFirstLine = true;
    for(; newCursor > 0; newCursor--){
        if(text[newCursor] == '\n' && newCursor != cursorPos){
            cursorOnFirstLine = false;
            break;
        }
    }
    if(cursorOnFirstLine){
        return;
    }
    unsigned tempCursor = newCursor + 1, preLineLength = 0, newLineLength = 0, tabCounter = 0;
    for(; tempCursor < cursorPos; tempCursor++, tabCounter++){
        if(text[tempCursor] == '\t'){
            preLineLength += tabLength - tabCounter % tabLength;
            if(tabLength > 0){
                tabCounter += getCurrentTabLength(tabCounter);
            }
        }
        else{
            preLineLength++;
        }
    }

    newCursor--;
    for(; newCursor > 0; newCursor--){
        if(text[newCursor] == '\n'){
            newCursor++;
            break;
        }
    }

    if(newCursor < protectedArea && preLineLength < protectedArea - newCursor){
        cursorPos = protectedArea;
        if(!shift){
            secondCursorPos = cursorPos;
        }
        return;
    }
    
    unsigned newLineTabsLength = 0;
    for(tempCursor = newCursor, tabCounter = 0; text[tempCursor] != '\n'; tempCursor++, tabCounter++){
        if(text[tempCursor] == '\t'){
            newLineLength += tabLength - tabCounter % tabLength;
            newLineTabsLength += getCurrentTabLength(tabCounter);
            if(tabLength > 0){
                tabCounter += getCurrentTabLength(tabCounter);
            }
        }
        else{
            newLineLength++;
        }
    }

    if(preLineLength == 0){
        cursorPos = newCursor;
    }
    else if(newLineLength > preLineLength){
        if(newLineTabsLength > 0){
            cursorPos = newCursor;
            //Find a local minimum on the new line.
            tabCounter = 0;
            for(; preLineLength > 0; preLineLength--, tabCounter++){
                if(text[cursorPos] == '\t'){
                    if(preLineLength < tabLength - tabCounter % tabLength){
                        break;
                    }
                    else{
                        preLineLength -= getCurrentTabLength(tabCounter);
                        tabCounter += getCurrentTabLength(tabCounter);
                    }
                }
                cursorPos++;
            }
        }
        else{
            cursorPos = newCursor + preLineLength;
        }
    }
    else{
        cursorPos = newCursor + newLineLength - newLineTabsLength;
    }
    if(!shift){
        secondCursorPos = cursorPos;
    }
}
void EditableTextModule::moveCursorDown(const string & text, bool shift){
    unsigned newCursor;
    
    for(newCursor = cursorPos; newCursor > 0; newCursor--){
        if(text[newCursor] == '\n' && newCursor != cursorPos){
            break;
        }
    }
    
    unsigned tempCursor, preLineLength = 0, newLineLength = 0, tabCounter = 0;
    for(tempCursor = newCursor + 1; tempCursor < cursorPos; tempCursor++, tabCounter++){
        if(text[tempCursor] == '\t'){
            preLineLength += tabLength - tabCounter % tabLength;
            if(tabLength > 0){
                tabCounter += getCurrentTabLength(tabCounter);
            }
        }
        else{
            preLineLength++;
        }
    }

    bool cursorOnLastLine = true;
    for(newCursor = cursorPos; newCursor < text.size(); newCursor++){
        if(text[newCursor] == '\n'){
            cursorOnLastLine = false;
            newCursor++;
            break;
        }
    }
    if(cursorOnLastLine){
        return;
    }
    
    unsigned newLineTabsLength = 0;
    for(tempCursor = newCursor, tabCounter = 0; text[tempCursor] != '\n' && tempCursor < text.size(); tempCursor++, tabCounter++){
        if(text[tempCursor] == '\t'){
            newLineLength += tabLength - tabCounter % tabLength;
            newLineTabsLength += getCurrentTabLength(tabCounter);
            if(tabLength > 0){
                tabCounter += getCurrentTabLength(tabCounter);
            }
        }
        else{
            newLineLength++;
        }
    }

    if(preLineLength == 0){
        cursorPos = newCursor;
    }
    else if(newLineLength > preLineLength){
        if(newLineTabsLength > 0){
            cursorPos = newCursor;
            //Find a local minimum on the new line.
            tabCounter = 0;
            for(; preLineLength > 0; preLineLength--, tabCounter++){
                if(text[cursorPos] == '\t'){
                    if(preLineLength < tabLength - tabCounter % tabLength){
                        break;
                    }
                    else{
                        preLineLength -= getCurrentTabLength(tabCounter);
                        tabCounter += getCurrentTabLength(tabCounter);
                    }
                }
                cursorPos++;
            }
        }
        else{
            cursorPos = newCursor + preLineLength;
        }
    }
    else{
        cursorPos = newCursor + newLineLength - newLineTabsLength;
    }
    if(!shift){
        secondCursorPos = cursorPos;
    }
}
bool EditableTextModule::prepareEditing(const vector <short> & releasedKeys, vector <short> & pressedKeys, bool & shift, bool & control){
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(pressedKeys[i] == ALLEGRO_KEY_LSHIFT || pressedKeys[i] == ALLEGRO_KEY_RSHIFT){
            shift = true;
            pressedKeys.erase(pressedKeys.begin() + i);
            i--;
            continue;
        }
        if(pressedKeys[i] == ALLEGRO_KEY_LCTRL || pressedKeys[i] == ALLEGRO_KEY_RCTRL){
            control = true;
            pressedKeys.erase(pressedKeys.begin() + i);
            i--;
            continue;
        }
        if(pressedKeys[i] == ALLEGRO_KEY_ENTER && getCanUseEnter() && enterAcceptsChanges){
            editingIsActive = false;
            setUpdateConnectedVariable(true);
            return false;
        }
    }

    //remove released keys from blocked keys
    for(char rKey : releasedKeys){
        for(unsigned int j = 0; j < blockedKeys.size(); j++){
            if(rKey == blockedKeys[j]){
                blockedKeys.erase(blockedKeys.begin() + j);
                currentInputDelay = inputDelay;
                break;
            }
        }
    }

    bool ignoreLast = false;

    if(pressedKeys.size() > blockedKeys.size()){
        lastInputedKey = -1;
        currentInputDelay = 0.0;
        ignoreLast = true;
    }

    //removing blocked keys from pressed keys
    for(char bKey : blockedKeys){
        for(unsigned int j = 0; j < pressedKeys.size(); j++){
            if(bKey == pressedKeys[j] && pressedKeys[j] != lastInputedKey){
                pressedKeys.erase(pressedKeys.begin() + j);
                break;
            }
        }
    }

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

    //block all pressed keys
    bool found;
    for(char pKey : pressedKeys){
        found = false;
        for(unsigned int j = 0; j < blockedKeys.size(); j++){
            if(pKey == blockedKeys[j]){
                found = true;
                break;
            }
        }
        if(!found){
            blockedKeys.push_back(pKey);
        }
    }

    if(pressedKeys.size() == 0){
        currentInputDelay = 0.0;
        lastInputedKey = -1;
        return false;
    }

    if(pressedKeys.size() > 1){
        currentInputDelay = 0.0;
    }

    if(currentInputDelay > 0.0){
        return false;
    }

    //delaying input and delaying repetition of the last pressed key
    if(pressedKeys.back() != lastInputedKey || pressedKeys.size() > 1 || ignoreLast){
        currentInputDelay = inputDelay;
    }
    else{
        currentInputDelay = repetitionDelay;
    }

    return true;
}
void EditableTextModule::getNumbers(char pKey, char & character, bool shift){
    if(pKey < 27 || pKey > 36){
        return;
    }
    if(!shift){
        character = pKey+21;
        return;
    }
    switch (pKey){
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
void EditableTextModule::getLetters(char pKey, char & character, bool shift){
    if(character != '\0' || getIsNumerical()){
        return;
    }
    if(pKey >= 1 && pKey <= 26){
        if(shift){
            character = 'A'+pKey-1;
        }
        else{
            character = 'a'+pKey-1;
        }
    }
    else if(pKey == ALLEGRO_KEY_MINUS && shift){
        character = '_';
    }
    else if(pKey == ALLEGRO_KEY_MINUS){
        character = '-';
    }
    else if(pKey == ALLEGRO_KEY_EQUALS && shift){
        character = '+';
    }
    else if(pKey == ALLEGRO_KEY_EQUALS){
        character = '=';
    }
    else if(pKey == ALLEGRO_KEY_OPENBRACE && shift){
        character = '{';
    }
    else if(pKey == ALLEGRO_KEY_OPENBRACE){
        character = '[';
    }
    else if(pKey == ALLEGRO_KEY_CLOSEBRACE && shift){
        character = '}';
    }
    else if(pKey == ALLEGRO_KEY_CLOSEBRACE){
        character = ']';
    }
    else if(pKey == ALLEGRO_KEY_COMMA && shift){
        character = '<';
    }
    else if(pKey == ALLEGRO_KEY_COMMA){
        character = ',';
    }
    else if(pKey == ALLEGRO_KEY_FULLSTOP && shift){
        character = '>';
    }
    else if(pKey == ALLEGRO_KEY_FULLSTOP){
        character = '.';
    }
    else if(pKey == ALLEGRO_KEY_SLASH && shift){
        character = '?';
    }
    else if(pKey == ALLEGRO_KEY_SLASH){
        character = '/';
    }
    else if(pKey == ALLEGRO_KEY_SEMICOLON && shift){
        character = ':';
    }
    else if(pKey == ALLEGRO_KEY_SEMICOLON){
        character = ';';
    }
    else if(pKey == ALLEGRO_KEY_QUOTE && shift){
        character = '\"';
    }
    else if(pKey == ALLEGRO_KEY_QUOTE){
        character = '\'';
    }
    else if(pKey == ALLEGRO_KEY_BACKSLASH && shift){
        character = '|';
    }
    else if(pKey == ALLEGRO_KEY_BACKSLASH){
        character = '\\';
    }
    else if(pKey == ALLEGRO_KEY_SPACE && getCanUseSpace()){
        character = ' ';
    }
    else if(pKey == ALLEGRO_KEY_TAB){
        character = '\t';
    }
    else if(pKey == ALLEGRO_KEY_ENTER && getCanUseEnter()){
        character = '\n';
    }
    else{
        character = '\0';
    }
}
bool EditableTextModule::deleteFromText(char pKey, char character, string text, bool & control, ALLEGRO_DISPLAY * window){
    if(pKey != ALLEGRO_KEY_BACKSPACE && pKey != ALLEGRO_KEY_DELETE){
        return false;
    }
    if(cursorPos <= protectedArea || secondCursorPos <= protectedArea){
        return true;
    }
    if(pKey == ALLEGRO_KEY_BACKSPACE || (pKey == ALLEGRO_KEY_X && control && cursorPos != secondCursorPos)){
        if(text.size() <= getMinContentSize()){
            return true;
        }
        string newContent = "";
        if(cursorPos != secondCursorPos){
            unsigned selectionStart = std::min(cursorPos, secondCursorPos);
            unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
            if(selectionEnd >= text.size()){
                selectionEnd--;
            }

            if(pKey == ALLEGRO_KEY_X){
                string clipboard = "";
                if(cursorPos != secondCursorPos){
                    clipboard = text.substr(selectionStart, selectionEnd - selectionStart + 1);
                }
                al_set_clipboard_text(window, clipboard.c_str());
            }

            newContent = text.substr(0, selectionStart) + text.substr(selectionEnd + 1, text.size()-selectionEnd);
            cursorPos = selectionStart;
        }
        else{
            if(cursorPos == 0){
                return true;
            }
            newContent = text.substr(0, cursorPos-1) + text.substr(cursorPos, text.size()-cursorPos);
            cursorPos--;
        }
        modifyContent(currentTextIdx, newContent);
        secondCursorPos = cursorPos;
        return true;
    }
    else if(pKey == ALLEGRO_KEY_DELETE){
        if(text.size() <= getMinContentSize()){
            return true;
        }
        string newContent = "";
        if(cursorPos != secondCursorPos){
            unsigned selectionStart = std::min(cursorPos, secondCursorPos);
            unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
            if(selectionEnd >= text.size()){
                selectionEnd--;
            }
            newContent = text.substr(0, selectionStart) + text.substr(selectionEnd + 1, text.size()-selectionEnd);
            cursorPos = selectionStart;
        }
        else{
            if(cursorPos >= text.size()){
                return true;
            }
            newContent = text.substr(0, cursorPos) + text.substr(cursorPos+1, text.size()-cursorPos);
        }
        modifyContent(currentTextIdx, newContent);
        secondCursorPos = cursorPos;
        return true;
    }
    return false;
}
void EditableTextModule::addFloatingPoint(char pKey, char & character, string text){
    if(character != '\0'){
        return;
    }
    if(pKey == 73 && (getHasFloatingPoint() || getIsNumerical())){
        bool isTherePoint = false;
        if(getIsNumerical()){
            for(char p : text){
                if(p == '.'){
                    isTherePoint = true;
                    break;
                }
            }
        }
        if(isTherePoint){
            character = '.';
        }
    }
}
bool EditableTextModule::addMinus(char pKey, char & character, string text){
    if(pKey != ALLEGRO_KEY_MINUS || !getIsNumerical()){
        return false;
    }
    if(cursorPos > 0){
        return true;
    }
    for(char p : text){
        if(p == '-'){
            return true;
        }
    }
    return false;
}
void EditableTextModule::editText(vector <short> releasedKeys, vector <short> pressedKeys, vector <SingleFont> & FontContainer, ALLEGRO_DISPLAY * window){
    if(!getIsActive()){
        return;
    }

    bool shift = false;
    bool control = false;
    
    if(!prepareEditing(releasedKeys, pressedKeys, shift, control)){
        return;
    }

    char character = '\0';
    string text = "";

    for(char pKey : pressedKeys){
        text = getContent(currentTextIdx);

        if(!ignoreVerticalArrows && pKey == ALLEGRO_KEY_UP){
            if(cursorPos == 0){
                continue;
            }
            moveCursorUp(text, shift);
            continue;
        }
        else if(pKey == ALLEGRO_KEY_RIGHT){
            if(cursorPos >= text.size()){
                continue;
            }
            cursorPos += 1;
            if(!shift){
                secondCursorPos = cursorPos;
            }
            continue;
        }
        else if(!ignoreVerticalArrows && pKey == ALLEGRO_KEY_DOWN){
            if(cursorPos >= text.size()){
                continue;
            }
            moveCursorDown(text, shift);
            continue;
        }
        else if(pKey == ALLEGRO_KEY_LEFT){
            if(cursorPos == 0 || cursorPos <= protectedArea){
                continue;
            }
            cursorPos -= 1;
            if(!shift){
                secondCursorPos = cursorPos;
            }
            continue;
        }

        if(deleteFromText(pKey, character, text, control, window)){
            continue;
        }

        if(control){
            if(pKey == ALLEGRO_KEY_A){
                cursorPos = 0;
                secondCursorPos = text.size();
                continue;
            }
            else if(text.size() > 0 && pKey == ALLEGRO_KEY_C){
                string clipboard = "";
                if(cursorPos != secondCursorPos){
                    unsigned selectionStart = std::min(cursorPos, secondCursorPos);
                    unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
                    clipboard = text.substr(selectionStart, selectionEnd - selectionStart + 1);
                }
                al_set_clipboard_text(window, clipboard.c_str());
                continue;
            }
            else if(pKey == ALLEGRO_KEY_V){
                if(cursorPos < protectedArea || secondCursorPos < protectedArea){
                    continue;
                }
                string newContent = text.substr(0, std::min(cursorPos, secondCursorPos));
                if(!al_clipboard_has_text(window)){
                    continue;
                }
                string clipboard = al_get_clipboard_text(window);
                if(clipboard.size() == 0){
                    continue;
                }
                newContent += clipboard;
                if(cursorPos != secondCursorPos){
                    unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
                    if(selectionEnd >= text.size()){
                        selectionEnd--;
                    }
                    newContent += text.substr(selectionEnd + 1, text.size()-selectionEnd);
                }
                else{
                    newContent += text.substr(cursorPos, text.size()-cursorPos);
                }
                cursorPos = std::min(cursorPos, secondCursorPos);
                secondCursorPos = cursorPos + clipboard.size() - 1;
                modifyContent(currentTextIdx, newContent);
            }
            continue;
        }

        if(cursorPos < protectedArea || secondCursorPos < protectedArea){
            continue;
        }

        if(getIsNumerical() && (pKey < 27 || pKey > 36)){
            continue;
        }

        getNumbers(pKey, character, shift);

        addFloatingPoint(pKey, character, text);
        
        if(addMinus(pKey, character, text)){
            continue;
        }

        getLetters(pKey, character, shift);

        if(character == '\0'){
            continue;
        }
        
        unsigned selectionStart = std::min(cursorPos, secondCursorPos);
        string newContent = text.substr(0, selectionStart);
        if(character != '\t' || useTabs){
            newContent += character;
            if(cursorPos != secondCursorPos){
                unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
                if(selectionEnd >= text.size()){
                    selectionEnd--;
                }
                newContent += text.substr(selectionEnd + 1, text.size()-selectionEnd);
            }
            else{
                newContent += text.substr(cursorPos, text.size()-cursorPos);
                selectionStart++;
            }
            cursorPos = selectionStart;
            secondCursorPos = cursorPos;
        }
        else{
            unsigned tmpCursor = 0, tabCounter = 0;
            for(; tmpCursor < selectionStart; tmpCursor++, tabCounter++){
                if(text[tmpCursor] == '\t'){
                    tabCounter += getCurrentTabLength(tabCounter);
                }
            }
            for(unsigned i = 0; i < getCurrentTabLength(tabCounter) + 1; i++){
                newContent += " ";
            }
            if(cursorPos != secondCursorPos){
                unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
                if(selectionEnd >= text.size()){
                    selectionEnd--;
                }
                newContent += text.substr(selectionEnd + 1, text.size()-selectionEnd);
            }
            else{
                newContent += text.substr(cursorPos, text.size()-cursorPos);
                selectionStart += getCurrentTabLength(tabCounter) + 1;
            }
            cursorPos = selectionStart;
            secondCursorPos = cursorPos;
        }

        modifyContent(currentTextIdx, newContent);
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
unsigned EditableTextModule::getProtectedArea() const{
    return protectedArea;
}
bool EditableTextModule::tryUpdatingID(vector<string> &listOfIDs, string &currentID, string newID)
{
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

    string cContent = getContent(currentTextIdx);

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
    double dValue = cstod(getContent(currentTextIdx));

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

    string cContent = getContent(currentTextIdx);

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
    double dValue = cstod(cContent);

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
bool EditableTextModule::controlImage(ImageModule & Image, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfIDs, string workingDirectory){
    bool success = false;

    string cContent = getContent(currentTextIdx);
    if(connectedVariable == "connect_bitmap_via_path"){
        Image.connectBitmap(BitmapContainer, cContent, "", workingDirectory);
        success = true;
    }
    else if(connectedVariable == "connect_bitmap_via_alias"){
        Image.connectBitmap(BitmapContainer, "", cContent, workingDirectory);
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
    double dValue = cstod(cContent);

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
    string cContent = getContent(currentTextIdx);

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
    double dValue = cstod(cContent);
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

    string cContent = getContent(currentTextIdx);

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
    string cContent = getContent(currentTextIdx);

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
    double dValue = cstod(cContent);


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
    string cContent = getContent(currentTextIdx);

    if(connectedVariable == "string"){
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
        if(cContent == "true" || cContent == "1"){
            bValue = true;
        }
        if(cContent == "false" || cContent == "0"){
            bValue = false;
        }
        if(connectedVariable == "bool"){
            return Variable.setBool(bValue);
        }
    }

    if(!canConvertContentToNumber()){
        printNotNumericalWarning();
        return false;
    }

    double dValue = cstod(cContent);
    int iValue = int(dValue);

    if(connectedVariable == "int"){
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
    else if(attribute == "can_use_enter"){
        BasePointers.back().setPointer(&canUseEnter);
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
    else if(attribute == "protected_area"){
        BasePointers.back().setPointer(&protectedArea);
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
    else if(attribute == "protected_area"){
        return VariableModule::newInt(protectedArea);
    }

    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    return VariableModule::newBool(false);
}