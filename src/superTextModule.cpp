#include "superTextModule.h"

bool FormatClass::isTheSame(const FormatClass &Compare){
    return color.r == Compare.color.r && color.g == Compare.color.g
        && color.b == Compare.color.b && color.a == Compare.color.a
        && accentColor.r == Compare.accentColor.r && accentColor.g == Compare.accentColor.g
        && accentColor.b == Compare.accentColor.b && accentColor.a == Compare.accentColor.a
        && Font == Compare.Font && offset.x == Compare.offset.x
        && offset.y == Compare.offset.y && selected == Compare.selected;
}

void SuperTextModule::setUpNewInstance(){
    content = "";
    tabLength = 4;
    wrapped = 'n';
    horizontalAlign = 'l';
    verticalAlign = 'i';
    paddingBetweenLines = 0.0;
    rotation = 0.0;
    scale.set(1.0, 1.0);
    drawSelectionFirst = false;
    cursorPos = 0;
    secondCursorPos = 0;
    localCursorPos = 0;
    lineWithCursorIdx = 0;
    lineWidthToCursor = 0.0;
    localSecondCursorPos = 0;
    lineWithSecondCursorIdx = 0;
    lineWidthToSecondCursor = 0.0;
    updated = false;
}
SuperTextModule::SuperTextModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
SuperTextModule::SuperTextModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
SuperTextModule::SuperTextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
SuperTextModule::~SuperTextModule(){

}
void SuperTextModule::clone(const SuperTextModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool &changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}
void SuperTextModule::clear(){
    setUpNewInstance();
    textLines.clear();
    lineWidths.clear();
    lineHeights.clear();
    lineLengths.clear();
    lineStarts.clear();
    floatingNewLine.clear();
    Formatting.clear();
}

void SuperTextModule::mergeFormatting(){
    for(unsigned formatIdx = 0; formatIdx < Formatting.size() - 1;){
        if(Formatting[formatIdx].isTheSame(Formatting[formatIdx + 1])){
            Formatting[formatIdx].limit += Formatting[formatIdx + 1].limit;
            Formatting.erase(Formatting.begin() + formatIdx + 1);
        }
        else{
            formatIdx++;
        }
    }
}
void SuperTextModule::update(){
    //cout << "Update!";
    //cout.flush();
    
    textLines.clear();
    textLines.push_back("");
    lineWidths.clear();
    lineWidths.push_back(0);
    lineHeights.clear();
    lineHeights.push_back(0);
    lineLengths.clear();
    lineLengths.push_back(0);
    lineStarts.clear();
    lineStarts.push_back(0);
    floatingNewLine.clear();
    floatingNewLine.push_back(false);
    realTextSize.set(0.0, 0.0);
    localCursorPos = 0;
    lineWithCursorIdx = 0;
    lineWidthToCursor = 0.0;
    localSecondCursorPos = 0;
    lineWithSecondCursorIdx = 0;
    lineWidthToSecondCursor = 0.0;

    if(Formatting.size() == 0){
        cout << "Warning: In " << __FUNCTION__ << ": In SuperText '" << ID << "': Text does not have any formatting.\n";
        return;
    }

    if(Formatting[0].Font == nullptr){
        cerr << "Error: The first font was not loaded. Text update failed.\n";
        return;
    }

    if(Formatting[0].Font->font == nullptr){
        cerr << "Error: Font " << Formatting[0].Font->ID << " was not loaded. Text update failed.\n";
        return;
    }

    lineHeights.back() = al_get_font_line_height(Formatting[0].Font->font);
    
    mergeFormatting();

    //Expand the last formatting to match full text
    unsigned letterLimitSum = 0;
    for(const FormatClass & Format : Formatting){
        letterLimitSum += Format.limit;
    }
    if(letterLimitSum < content.size()){
        letterLimitSum -= Formatting.back().limit;
        Formatting.back().limit = content.size() - letterLimitSum + 1;
    }

    for(FormatClass & Format : Formatting){
        Format.drawingLimit = 0;
    }

    vector<FormatClass>::iterator Format = Formatting.begin();

    char letter;
    unsigned letterIdx = 0, formatLimit = 0, lineLength = 0;
    float letterWidth = 0.0, wordWidth = 0.0, wordWidthBehind = 0.0, lineWidth = 0.0;
    bool ignoreLine = false; //Used when wrapped=='c'. Ignores the part of the text in the line that is out of the scope of text field.
    unsigned currentTabLength, i;
    char wordLetter;

    for(; letterIdx < content.size(); letterIdx++){
        if(letterIdx == cursorPos){
            localCursorPos = textLines.back().size();
            lineWithCursorIdx = textLines.size() - 1;
            lineWidthToCursor = lineWidth;
        }
        if(letterIdx == secondCursorPos){
            localSecondCursorPos = textLines.back().size();
            lineWithSecondCursorIdx = textLines.size() - 1;
            lineWidthToSecondCursor = lineWidth;
        }
        letter = content.substr(letterIdx, 1)[0];
        if(letter == '\n'){
            Format->drawingLimit++;
            lineWidth += al_get_text_width(Format->Font->font, string(" ").c_str());
            if(lineWidth > size.x){
                if(wrapped == 'c'){
                    ignoreLine = true;
                }
                if(wrapped != 'n')
                    floatingNewLine.back() = true;
            }
            if(!ignoreLine){
                lineLength++;
            }

            textLines.back() += ' ';
            textLines.push_back("");
            
            realTextSize.x = std::max(realTextSize.x, lineWidth);
            lineWidths.back() = lineWidth;
            lineWidths.push_back(0);
            if(Format != Formatting.end() && Format->Font != nullptr
                && Format->Font->height > lineHeights.back()
            ){
                lineHeights.back() = Format->Font->height;
            }
            lineHeights.push_back(0);
            lineLengths.back() = lineLength;
            lineLengths.push_back(0);
            lineStarts.push_back(letterIdx + 1);
            floatingNewLine.push_back(false);
            lineWidth = 0;
            lineLength = 0;
            realTextSize.y += Format->Font->height;
            if(textLines.size() > 1){
                realTextSize.y += paddingBetweenLines;
            }
            ignoreLine = false;
            formatLimit++;
            if(formatLimit == Format->limit){
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
            else if(Format->Font != nullptr && Format->Font->height > lineHeights.back()){
                lineHeights.back() = Format->Font->height;
            }
            
            if(wrapped == 'c' && realTextSize.y + Format->Font->height + paddingBetweenLines > size.y){
                break;
            }
        }
        else if(letter == '\t'){
            currentTabLength = tabLength - textLines.back().size() % tabLength;
            letterWidth = al_get_text_width(Format->Font->font, string(" ").c_str()); //It's the only way, if you don't want undefined behaviour. 
            if(lineWidth + letterWidth > size.x && (wrapped == 'l' || wrapped == 'w')){
                currentTabLength = tabLength;
                textLines.push_back("");
                realTextSize.x = std::max(realTextSize.x, lineWidth);
                lineWidths.back() = lineWidth;
                lineWidths.push_back(0);
                if(Format != Formatting.end() && Format->Font != nullptr
                    && Format->Font->height > lineHeights.back()
                ){
                    lineHeights.back() = Format->Font->height;
                }
                lineHeights.push_back(0);
                lineLengths.back() = lineLength;
                lineLengths.push_back(0);
                lineStarts.push_back(letterIdx);
                floatingNewLine.push_back(false);
                lineWidth = 0;
                lineLength = 0;
                realTextSize.y += Format->Font->height;
            }
            if(letterIdx == cursorPos){
                localCursorPos = textLines.back().size();
                lineWithCursorIdx = textLines.size() - 1;
                lineWidthToCursor = lineWidth;
            }
            if(letterIdx == secondCursorPos){
                localSecondCursorPos = textLines.back().size();
                lineWithSecondCursorIdx = textLines.size() - 1;
                lineWidthToSecondCursor = lineWidth;
            }
            for(i = 0; i < currentTabLength; i++){
                if(lineWidth + letterWidth > size.x){
                    if(wrapped == 'c'){
                        ignoreLine = true;
                    }
                    else if(wrapped == 'l' || wrapped == 'w'){
                        // if(i > 0){
                        //     break;
                        // }
                        //currentTabLength = tabLength;
                        textLines.push_back("");
                        realTextSize.x = std::max(realTextSize.x, lineWidth);
                        lineWidths.back() = lineWidth;
                        lineWidths.push_back(0);
                        if(Format != Formatting.end() && Format->Font != nullptr
                            && Format->Font->height > lineHeights.back()
                        ){
                            lineHeights.back() = Format->Font->height;
                        }
                        lineHeights.push_back(0);
                        lineLengths.back() = lineLength;
                        lineLengths.push_back(0);
                        lineStarts.push_back(letterIdx + 1);
                        floatingNewLine.push_back(false);
                        lineWidth = 0;
                        lineLength = 0;
                        realTextSize.y += Format->Font->height;
                        if(textLines.size() > 1){
                            realTextSize.y += paddingBetweenLines;
                        }
                        break;
                    }
                }
                lineWidth += letterWidth;
                if(!ignoreLine){
                    lineLength++;
                }
                textLines.back() += ' ';
                Format->drawingLimit++;
            }
            formatLimit++;
            if(formatLimit == Format->limit){
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
            else if(Format->Font != nullptr && Format->Font->height > lineHeights.back()){
                lineHeights.back() = Format->Font->height;
            }
        }
        else{
            Format->drawingLimit++;
            letterWidth = al_get_text_width(Format->Font->font, string(1, letter).c_str()); //It's the only way, if you don't want undefined behaviour. 
            wordWidth = 0.0;
            wordWidthBehind = 0.0;
            if(wrapped == 'w' && letter != ' '){
                if(letterIdx > 0){
                    for(i = letterIdx - 1; i >= 0; i--){
                        wordLetter = content.substr(i, 1)[0];
                        if(wordLetter == ' ' || wordLetter == '\t' || wordLetter == '\n'){
                            break;
                        }
                        wordWidthBehind += al_get_text_width(Format->Font->font, string(1, wordLetter).c_str()); //It's the only way, if you don't want undefined behaviour.
                        if(i == 0){
                            break;
                        }
                    }
                }
                for(i = letterIdx + 1; i < content.size(); i++){
                    wordLetter = content.substr(i, 1)[0];
                    if(wordLetter == ' ' || wordLetter == '\t' || wordLetter == '\n'){
                        break;
                    }
                    wordWidth += al_get_text_width(Format->Font->font, string(1, wordLetter).c_str()); //It's the only way, if you don't want undefined behaviour. 
                }
                if(letterWidth + wordWidthBehind + wordWidth > size.x){
                    wordWidth = 0.0;
                    wordWidthBehind = 0.0;
                }
            }
            if(lineWidth - wordWidthBehind + letterWidth + wordWidth > size.x){
                if(wrapped == 'c'){
                    ignoreLine = true;
                }
                else if(wrapped == 'l' || wrapped == 'w'){
                    textLines.push_back("");
                    realTextSize.x = std::max(realTextSize.x, lineWidth);
                    lineWidths.back() = lineWidth;
                    lineWidths.push_back(0);
                    if(Format != Formatting.end() && Format->Font != nullptr
                        && Format->Font->height > lineHeights.back()
                    ){
                        lineHeights.back() = Format->Font->height;
                    }
                    lineHeights.push_back(0);
                    lineLengths.back() = lineLength;
                    lineLengths.push_back(0);
                    lineStarts.push_back(letterIdx);
                    floatingNewLine.push_back(false);
                    lineWidth = 0;
                    lineLength = 0;
                    realTextSize.y += Format->Font->height;
                    if(textLines.size() > 1){
                        realTextSize.y += paddingBetweenLines;
                    }
                    if(letterIdx == cursorPos){
                        localCursorPos = textLines.back().size();
                        lineWithCursorIdx = textLines.size() - 1;
                        lineWidthToCursor = lineWidth;
                    }
                    if(letterIdx == secondCursorPos){
                        localSecondCursorPos = textLines.back().size();
                        lineWithSecondCursorIdx = textLines.size() - 1;
                        lineWidthToSecondCursor = lineWidth;
                    }
                }
            }
            lineWidth += letterWidth;
            if(!ignoreLine){
                lineLength++;
            }
            textLines.back() += letter;
            formatLimit++;
            if(formatLimit == Format->limit){
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
            else if(Format->Font != nullptr && Format->Font->height > lineHeights.back()){
                lineHeights.back() = Format->Font->height;
            }
        }
    }

    if(cursorPos > 0 && lineWithCursorIdx == 0 && localCursorPos == 0){
        localCursorPos = textLines.back().size();
        lineWithCursorIdx = textLines.size() - 1;
        lineWidthToCursor = lineWidth;
    }
    if(secondCursorPos > 0 && lineWithSecondCursorIdx == 0 && localSecondCursorPos == 0){
        localSecondCursorPos = textLines.back().size();
        lineWithSecondCursorIdx = textLines.size() - 1;
        lineWidthToSecondCursor = lineWidth;
    }
    
    if(Formatting.back().Font->height > lineHeights.back()){
        lineHeights.back() = Formatting.back().Font->height;
    }
    realTextSize.y += lineHeights.back(); //add the last line
    if(textLines.size() > 1){
        realTextSize.y += paddingBetweenLines;
    }
    realTextSize.x = std::max(realTextSize.x, lineWidth);
    lineWidths.back() = lineWidth;
    lineLengths.back() = lineLength;
    if(wrapped != 'n'){
        for(float & width : lineWidths){
            width = std::min(width, float(size.x));
        }
    }

    updated = true;
}

void SuperTextModule::cropSizeToText(){
    size.set(realTextSize.x, realTextSize.y);
}
void SuperTextModule::cropWidthToText(){
    size.x = realTextSize.x;
}
void SuperTextModule::cropHeightToText(){
    size.y = realTextSize.y;
}
VariableModule SuperTextModule::getAttributeValue(const string &attribute, const string &detail, EventDescription EventIds) const{
    if(attribute == "pos_x"){
        return VariableModule::newDouble(pos.x);
    }
    if(attribute == "pos_y"){
        return VariableModule::newDouble(pos.y);
    }
    if(attribute == "size_x"){
        return VariableModule::newDouble(size.x);
    }
    if(attribute == "size_y"){
        return VariableModule::newDouble(size.y);
    }
    if(attribute == "in_group"){
        return VariableModule::newBool(isInAGroup(detail));
    }
    if(attribute == "in_group"){
        return VariableModule::newBool(isInAGroup(detail));
    }
    if(attribute == "content"){
        return VariableModule::newString(content);
    }
    if(attribute == "wrapped"){
        return VariableModule::newInt(wrapped);
    }
    if(attribute == "horizontal_align"){
        return VariableModule::newInt(horizontalAlign);
    }
    if(attribute == "vertical_align"){
        return VariableModule::newInt(verticalAlign);
    }
    if(attribute == "rotation"){
        return VariableModule::newDouble(rotation);
    }
    if(attribute == "cursor_pos_x"){
        return VariableModule::newDouble(lineWidthToCursor);
    }
    if(attribute == "cursor_pos_y"){
        double cursorPixelPosY = 0;
        for(unsigned lineIdx = 0; lineIdx < lineWithCursorIdx; lineIdx++){
            cursorPixelPosY += lineHeights[lineIdx];
        }
        return VariableModule::newDouble(cursorPixelPosY);
    }
    
    cerr << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__
        << ": Attribute '" << attribute << "' is not valid.\n";
    return VariableModule::newBool(false);
}
void SuperTextModule::getContext(string attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "content"){
        BasePointers.back().setPointer(&content);
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
    else{
        getPrimaryContext(attribute, BasePointers);
    }
}

void SuperTextModule::drawFormattedString(string text, vec2d finalPos, size_t lineIdx, vector<FormatClass>::iterator Format, bool drawSelection) const{
    if(Format->selected){
        if(drawSelection){
            string background;
            for(size_t i = 0; i < text.size(); i++){
                background += "█";
            }
            al_draw_text(Format->Font->font, Format->color, finalPos.x + Format->offset.x,
                finalPos.y,
                0, background.c_str()
            );
        }
        if(!drawSelectionFirst || !drawSelection){
            al_draw_text(Format->Font->font, Format->accentColor, finalPos.x + Format->offset.x,
                finalPos.y,
                0, text.c_str()
            );
        }
    }
    else{
        al_draw_text(Format->Font->font, Format->color, finalPos.x + Format->offset.x,
            finalPos.y,
            0, text.c_str()
        );
    }
}
void SuperTextModule::drawAllLines(vec2d finalPos, bool drawSelection, vec2i displaySize) const{
    size_t letterIdx = 0;
    size_t lineIdx = 0;
    size_t formatLimit = 0;
    size_t longestFragment = 0;
    string currentFragment;
    vec2d linePos(finalPos);
    float previousLength;
    vector<FormatClass>::iterator Format = (const_cast<vector<FormatClass> &>(Formatting)).begin();

    for(lineIdx = 0; lineIdx < textLines.size(); lineIdx++){
        previousLength = 0.0;
        linePos.set(finalPos);
        if(Format->Font == nullptr){
            cerr << "Error: Font was not loaded. Text update failed.\n";
            return;
        }
        if(Format->Font->font == nullptr){
            cerr << "Error: Font " << Format->Font->ID << " was not loaded. Text update failed.\n";
            return;
        }
        linePos.y = finalPos.y + Format->offset.y + lineIdx * (Format->Font->height + paddingBetweenLines);
        if(rotation == 0 && linePos.y > displaySize.y){
            break;
        }
        
        if(horizontalAlign == 'c'){
            linePos.x += (size.x / 2) - (lineWidths[lineIdx] / 2);
            if(textLines[lineIdx].back() == ' '){
                linePos.x += al_get_text_width(Format->Font->font, string(" ").c_str()) / 2;
            }
        }
        else if(horizontalAlign == 'r'){
            linePos.x += size.x - lineWidths[lineIdx];
            if(textLines[lineIdx].back() == ' '){
                linePos.x += al_get_text_width(Format->Font->font, string(" ").c_str()) / 2;
            }
        }
        for(letterIdx = 0; letterIdx < lineLengths[lineIdx];){
            longestFragment = Format->drawingLimit - formatLimit;
            longestFragment = std::min(longestFragment, lineLengths[lineIdx] - letterIdx);
            currentFragment = textLines[lineIdx].substr(letterIdx, longestFragment);
            linePos.x += previousLength;
            letterIdx += longestFragment;
            formatLimit += longestFragment;
            
            previousLength = al_get_text_width(Format->Font->font, currentFragment.c_str());
            if(rotation != 0 || linePos.y + Format->Font->height >= 0){
                drawFormattedString(currentFragment, linePos, lineIdx, Format, drawSelection);
            }

            if(formatLimit >= Format->drawingLimit){
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
        }
        if(Format == Formatting.end()){
            break;
        }
        for(;letterIdx < textLines[lineIdx].size();){
            longestFragment = Format->drawingLimit - formatLimit;
            longestFragment = std::min(longestFragment, textLines[lineIdx].size() - letterIdx);
            letterIdx += longestFragment;
            formatLimit += longestFragment;
            if(formatLimit >= Format->drawingLimit){
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
        }
        if(Format == Formatting.end()){
            break;
        }   
    }
}
void SuperTextModule::draw(vec2d base, bool drawBorders, Camera2D Camera, unsigned cursorPos, unsigned secondCursorPos, bool editingIsActive, vec2i displaySize) const{
    vec2d newScale(scale);
    vec2d unformatedPos(base + pos);
    if(isScrollable){
        vec2d diff(Camera.size.x/2-base.x-pos.x, Camera.size.y/2-base.y-pos.y);
        unformatedPos.set(Camera.size.x/2-diff.x*(Camera.zoom), Camera.size.y/2-diff.y*(Camera.zoom));
        unformatedPos.translate(Camera.visionShift.x*Camera.zoom, Camera.visionShift.y*Camera.zoom);
        newScale.multiply(Camera.zoom);
        if(isScaledFromCenter){
            unformatedPos.translate(-((scale.x-1)*size.x*Camera.zoom)/2, -((scale.y-1)*size.y*Camera.zoom)/2);
        }
    }
    vec2d finalPos(unformatedPos);

    if(verticalAlign == 'c'){
        finalPos.y += size.y / 2 - realTextSize.y / 2;
    }
    else if(verticalAlign == 'd'){
        finalPos.y += size.y - realTextSize.y;
    }

    if(Formatting.size() == 0){
        cout << "Warning: In " << __FUNCTION__ << ": In SuperText '" << ID << "': Text does not have any formatting.\n";
        if(drawBorders){
            al_draw_rectangle(unformatedPos.x, unformatedPos.y, unformatedPos.x+size.x, unformatedPos.y+size.y, Formatting.back().color, 1);
        }
        return;
    }

    ALLEGRO_TRANSFORM trans;
    al_identity_transform(&trans);
    al_translate_transform(&trans, -unformatedPos.x - size.x / 2, -unformatedPos.y - size.y / 2);
    al_rotate_transform(&trans, (rotation*M_PI)/180.0);
    al_scale_transform(&trans, newScale.x, newScale.y);
    al_translate_transform(&trans, unformatedPos.x + size.x / 2, unformatedPos.y + size.y / 2);
    al_use_transform(&trans);


    drawAllLines(finalPos, true, displaySize);
    if(drawSelectionFirst){
        drawAllLines(finalPos, false, displaySize);
    }
    
    al_identity_transform(&trans);
    al_use_transform(&trans);

    if(drawBorders){
        al_draw_rectangle(unformatedPos.x, unformatedPos.y, unformatedPos.x+size.x, unformatedPos.y+size.y, Formatting.back().color, 1);
    }
}

void SuperTextModule::fitFormattingToContent(){
    unsigned contentSize = content.size();
    if(contentSize == 0){
        Formatting.erase(Formatting.begin() + 1, Formatting.end());
        Formatting[0].limit = 1;
        Formatting[0].drawingLimit = 1;
        //update();
        return;
    }
    unsigned limitSum = 0;
    unsigned formatIdx;
    for(formatIdx = 0; formatIdx < Formatting.size(); formatIdx++){
        limitSum += Formatting[formatIdx].limit;
        if(contentSize <= limitSum){
            break;
        }
    }
    if(formatIdx == Formatting.size()){
        Formatting.back().limit += contentSize - limitSum + 1;
        //update();
        return;
    }
    Formatting[formatIdx].limit = (contentSize - (limitSum - Formatting[formatIdx].limit)) + 1;
    if(formatIdx == Formatting.size() - 1){
        //update();
        return;
    }
    Formatting.erase(Formatting.begin() + formatIdx + 1, Formatting.end());
}
void SuperTextModule::setContent(string newContent){
    content = newContent;
    fitFormattingToContent();
    //update();
}
void SuperTextModule::addContent(string newContent){
    content += newContent;
    Formatting.back().limit += newContent.size();

    //update();
}
void SuperTextModule::addNewTextLine(string newLine){
    textLines.push_back(newLine);
}
void SuperTextModule::setTextLine(size_t index, string newLine){
    if(index > textLines.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of textLines vector.\n";
        return;
    }
    textLines[index] = newLine;
}
void SuperTextModule::addToTextLine(size_t index, string newLine){
    if(index > textLines.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of textLines vector.\n";
        return;
    }
    textLines[index] += newLine;
}
void SuperTextModule::addFormat(ALLEGRO_COLOR newColor, ALLEGRO_COLOR newAccentColor, string fontID,
                                vector<SingleFont> & FontContainer, float offsetX, float offsetY, bool isSelected, size_t newLimit)
{
    Formatting.push_back(FormatClass());
    Formatting.back().color = newColor;
    Formatting.back().accentColor = newAccentColor;
    Formatting.back().Font = findFontByID(FontContainer, fontID);
    Formatting.back().offset.set(offsetX, offsetY);
    Formatting.back().selected = isSelected;
    Formatting.back().limit = newLimit;
}
void SuperTextModule::modifyFormat(size_t index, ALLEGRO_COLOR newColor, ALLEGRO_COLOR newAccentColor, string fontID,
    vector <SingleFont> & FontContainer, float offsetX, float offsetY, bool isSelected, size_t newLimit
){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].color = newColor;
    Formatting[index].accentColor = newAccentColor;
    Formatting[index].Font = findFontByID(FontContainer, fontID);
    Formatting[index].offset.set(offsetX, offsetY);
    Formatting[index].selected = isSelected;
    Formatting[index].limit = newLimit;
}
void SuperTextModule::injectFormat(unsigned fragmentStart, unsigned fragmentEnd, ALLEGRO_COLOR newColor, ALLEGRO_COLOR newAccentColor,
    string fontID, vector <SingleFont> & FontContainer, float offsetX, float offsetY, bool isSelected
){
    if(fragmentStart > content.size()){
        cerr << "Error: In " << __FUNCTION__ << ": Cannot inject format beyond the text content (start-index:" << fragmentStart << " >= size:" << content.size() << ").\n";
        return;
    }
    if(fragmentEnd > content.size()){
        cerr << "Error: In " << __FUNCTION__ << ": Cannot inject format beyond the text content (end-index:" << fragmentEnd << " >= size:" << content.size() << ").\n";
        return;
    }
    if(fragmentEnd < fragmentStart){
        cerr << "Error: In " << __FUNCTION__ << ": Start index (" << fragmentStart << ") cannot be bigger than the end index (" << fragmentEnd << ").\n";
        return;
    }

    unsigned startErase = 0;
    unsigned endErase = 0;
    unsigned leftLimitSaved = 0;
    unsigned rightLimitSaved = 0;

    //Chose formatting to be erased.
    unsigned letterIdx = 0;
    unsigned formatIdx = 0;
    for(; formatIdx < Formatting.size(); formatIdx++){
        if(letterIdx <= fragmentStart && letterIdx + Formatting[formatIdx].limit > fragmentStart){ //Format with left cursor inside.
            startErase = formatIdx;
            assert(fragmentStart >= letterIdx);
            leftLimitSaved = fragmentStart - letterIdx;
        }
        if(letterIdx + Formatting[formatIdx].limit > fragmentEnd){ //Format with right cursor inside
            endErase = formatIdx;
            assert(letterIdx + Formatting[formatIdx].limit >= fragmentEnd);
            rightLimitSaved = ((letterIdx + Formatting[formatIdx].limit) - fragmentEnd) - 1;
            break;
        }
        letterIdx += Formatting[formatIdx].limit;
    }

    //Erase selected formatting.
    if(endErase - startErase > 1){ //If more than three formats exist, erase all formats between the first and last selected format - not including those two.
        Formatting.erase(Formatting.begin() + startErase + 1, Formatting.begin() + endErase);
        endErase = startErase + 1;
        if(endErase >= Formatting.size()){
            endErase--;
        }
    }
    if(leftLimitSaved == 0 && rightLimitSaved == 0){ //Erase all selected formats if they are fully selected. 
        Formatting.erase(Formatting.begin() + startErase, Formatting.begin() + endErase + 1);
    }
    else if(endErase - startErase == 1){ //If two formats are selected, cut selected letters out of them and or erase one fully selected format. 
        if(leftLimitSaved > 0 && rightLimitSaved > 0){
            Formatting[startErase].limit = leftLimitSaved;
            Formatting[startErase].drawingLimit = Formatting[startErase].limit;
            Formatting[startErase + 1].limit = rightLimitSaved;
            Formatting[startErase + 1].drawingLimit = Formatting[startErase + 1].limit;
            startErase++;
        }
        else if(leftLimitSaved > 0){
            Formatting[startErase].limit = leftLimitSaved;
            Formatting[startErase].drawingLimit = Formatting[startErase].limit;
            Formatting.erase(Formatting.begin() + startErase + 1);
            startErase++;
        }
        else{
            Formatting[startErase + 1].limit = rightLimitSaved;
            Formatting[startErase + 1].drawingLimit = Formatting[startErase + 1].limit;
            Formatting.erase(Formatting.begin() + startErase);
        }
    }
    else if(leftLimitSaved > 0 && rightLimitSaved > 0){ //If a part inside a single format is selected, divide it to two and make place for the new injected format between the new pair.
        Formatting.insert(Formatting.begin() + startErase, FormatClass());
        Formatting[startErase] = Formatting[startErase + 1];
        Formatting[startErase].limit = leftLimitSaved;
        Formatting[startErase].drawingLimit = Formatting[startErase].limit;
        Formatting[startErase + 1].limit = rightLimitSaved - 1;
        Formatting[startErase + 1].drawingLimit = Formatting[startErase + 1].limit;
        startErase++;
    }
    else if(leftLimitSaved > 0){
        Formatting[startErase].limit = leftLimitSaved;
        Formatting[startErase].drawingLimit = Formatting[startErase].limit;
        startErase++;
    }
    else if(rightLimitSaved > 0){
        Formatting[startErase].limit = rightLimitSaved;
        Formatting[startErase].drawingLimit = Formatting[startErase].limit;
    }

    //Insert one new format in the place of erased ones.
    Formatting.insert(Formatting.begin() + startErase, FormatClass());
    Formatting[startErase].color = newColor;
    Formatting[startErase].accentColor = newAccentColor;
    Formatting[startErase].Font = findFontByID(FontContainer, fontID);
    Formatting[startErase].offset.set(offsetX, offsetY);
    Formatting[startErase].selected = isSelected;
    Formatting[startErase].limit = (fragmentEnd - fragmentStart) + 1;
    Formatting[startErase].drawingLimit = Formatting[startErase].limit;
}
void SuperTextModule::deleteFormat(size_t index)
{
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting.erase(Formatting.begin() + index);
}
void SuperTextModule::setColor(size_t index, ALLEGRO_COLOR newColor){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].color = newColor;
}
void SuperTextModule::setAccentColor(size_t index, ALLEGRO_COLOR newAccentColor){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].accentColor = newAccentColor;
}
void SuperTextModule::setFont(size_t index, string newFontID, vector <SingleFont> & FontContainer){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].Font = findFontByID(FontContainer, newFontID);
}
void SuperTextModule::setOffset(size_t index, float offsetX, float offsetY){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.set(offsetX, offsetY);
}
void SuperTextModule::addOffset(size_t index, float offsetX, float offsetY){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.translate(offsetX, offsetY);
}
void SuperTextModule::setOffsetX(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.x = newOffset;
}
void SuperTextModule::addOffsetX(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.x += newOffset;
}
void SuperTextModule::setOffsetY(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.y = newOffset;
}
void SuperTextModule::addOffsetY(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.y += newOffset;
}
void SuperTextModule::setSelected(size_t index, bool isSelected){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].selected = isSelected;
}
void SuperTextModule::setLimit(size_t index, size_t newLimit){
    if(index > Formatting.size()){
        std::cerr << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].limit += newLimit;
}
void SuperTextModule::setWrapping(char newWrapping){
    wrapped = newWrapping;
}
void SuperTextModule::setHorizontalAlign(char newHorizontalAlign){
    horizontalAlign = newHorizontalAlign;
}
void SuperTextModule::setVerticalAlign(char newVerticalAlign){
    verticalAlign = newVerticalAlign;
}
void SuperTextModule::setPaddingBetweenLines(float newPadding){
    paddingBetweenLines = newPadding;
}
void SuperTextModule::setRotation(float newRotation){
    rotation = newRotation;
}
void SuperTextModule::addRotation(float newRotation){
    rotation += newRotation;
}
void SuperTextModule::setTabLength(unsigned short newTabLength){
    tabLength = newTabLength;
}
void SuperTextModule::divideFormattingByCursor(){
    unsigned formatIdx = 0;
    for(formatIdx = 0; formatIdx < Formatting.size(); formatIdx++){
        Formatting[formatIdx].selected = false;
    }
    unsigned cursorBegin = std::min(cursorPos, secondCursorPos);
    unsigned cursorEnd = std::max(cursorPos, secondCursorPos) + 1;
    unsigned letterIdx = 0;

    //Divide formatting by cursors' postitions.
    for(formatIdx = 0; formatIdx < Formatting.size(); formatIdx++){
        if(letterIdx >= cursorBegin && letterIdx + Formatting[formatIdx].limit <= cursorEnd){ //Inside of the cursor selection.
            Formatting[formatIdx].selected = true;
        }
        else if(letterIdx <= cursorBegin && letterIdx + Formatting[formatIdx].limit > cursorBegin){ //Format with left cursor inside.
            if(cursorBegin > 0){
                Formatting.insert(Formatting.begin()+formatIdx+1, FormatClass());
                Formatting[formatIdx + 1] = Formatting[formatIdx];
                Formatting[formatIdx + 1].selected = true;
                if(letterIdx + Formatting[formatIdx].limit < cursorEnd){ //Format without the right cursor.
                    Formatting[formatIdx + 1].limit = letterIdx + Formatting[formatIdx].limit - cursorBegin;
                    Formatting[formatIdx + 1].drawingLimit = Formatting[formatIdx + 1].limit;
                    Formatting[formatIdx].limit = cursorBegin - letterIdx;
                    Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
                }
                else if(letterIdx == cursorBegin){ ////Format with both cursors, when cursor is on the start of the format
                    Formatting[formatIdx].selected = true;
                    Formatting[formatIdx].limit = cursorEnd - cursorBegin;
                    Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
                    Formatting[formatIdx + 1].selected = false;
                    Formatting[formatIdx + 1].limit -= Formatting[formatIdx].limit;
                    Formatting[formatIdx + 1].drawingLimit = Formatting[formatIdx + 1].limit;
                    break;
                }
                else if(letterIdx + Formatting[formatIdx].limit == cursorEnd){ ////Format with both cursors, when cursor is on the end of the format
                    Formatting[formatIdx + 1].selected = true;
                    Formatting[formatIdx + 1].limit = cursorEnd - cursorBegin;
                    Formatting[formatIdx + 1].drawingLimit = Formatting[formatIdx + 1].limit;
                    Formatting[formatIdx].selected = false;
                    Formatting[formatIdx].limit -= Formatting[formatIdx + 1].limit;
                    Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
                    break;
                }
                else{ //Format with both cursors.
                    Formatting.insert(Formatting.begin()+formatIdx+2, FormatClass());
                    Formatting[formatIdx + 2] = Formatting[formatIdx];
                    Formatting[formatIdx + 2].limit = letterIdx + Formatting[formatIdx].limit - cursorEnd;
                    Formatting[formatIdx + 2].drawingLimit = Formatting[formatIdx + 2].limit;
                    Formatting[formatIdx + 1].limit = cursorEnd - cursorBegin;
                    Formatting[formatIdx + 1].drawingLimit = Formatting[formatIdx + 1].limit;
                    Formatting[formatIdx].limit = cursorBegin - letterIdx;
                    Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
                    break;
                }
            }
            else{
                Formatting.insert(Formatting.begin(), FormatClass());
                Formatting[0] = Formatting[1];
                Formatting[0].limit = cursorEnd;
                Formatting[0].selected = true;
                Formatting[1].limit -= cursorEnd;
                Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
            }
        }
        else if(cursorEnd > letterIdx && cursorEnd < letterIdx + Formatting[formatIdx].limit){ //Format with only right cursor inside.
            Formatting.insert(Formatting.begin()+formatIdx+1, FormatClass());
            Formatting[formatIdx + 1] = Formatting[formatIdx];
            Formatting[formatIdx + 1].limit = letterIdx + Formatting[formatIdx].limit - cursorEnd;
            Formatting[formatIdx + 1].drawingLimit = Formatting[formatIdx + 1].limit;
            Formatting[formatIdx].limit = cursorEnd - letterIdx;
            Formatting[formatIdx].selected = true;
            Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
            break;
        }

        letterIdx += Formatting[formatIdx].limit;
    }

    //Add all lost tabs to the formatting.
    formatIdx = 0;
    Formatting[0].drawingLimit = Formatting[0].limit;
    unsigned line = 0;
    unsigned currentLineLength = 0, currentTabLength;
    unsigned limit = 0;
    float lineWidth = 0.0;
    for(letterIdx = 0; letterIdx < content.size(); letterIdx++, currentLineLength++){
        if(line < textLines.size() && letterIdx == lineStarts[line + 1]){
            line++;
            currentLineLength = 0;
            lineWidth = 0;
        }
        if(content[letterIdx] == '\t'){
            currentTabLength = tabLength - currentLineLength % tabLength - 1;
            float letterWidth = al_get_text_width(Formatting[formatIdx].Font->font, string(" ").c_str());
            lineWidth += letterWidth;
            for(unsigned i = 0; i < currentTabLength; i++){
                if(lineWidth + letterWidth > size.x && (wrapped == 'l' || wrapped == 'w')){
                    break;
                }
                Formatting[formatIdx].drawingLimit++;
                currentLineLength++;
                lineWidth += letterWidth;
            }
        }
        else{
            lineWidth += al_get_text_width(Formatting[formatIdx].Font->font, string(1, content[letterIdx]).c_str());
        }
        limit++;
        if(limit >= Formatting[formatIdx].limit){
            formatIdx++;
            Formatting[formatIdx].drawingLimit = Formatting[formatIdx].limit;
            limit = 0;
        }
    }

    if(updated){
        textLines.back() += ' ';
        lineLengths.back()++;
        updated = false;
    }
    if(Formatting.back().drawingLimit < Formatting.back().limit){
        Formatting.back().drawingLimit++;
    }
}
void SuperTextModule::setCursorPos(int newPos){
    if(newPos < 0){
        cursorPos = 0;
    }
    else if(newPos > int(content.size())){
        cursorPos = content.size();
    }
    else{
        cursorPos = newPos;
    }
    secondCursorPos = cursorPos;
    divideFormattingByCursor();
}
void SuperTextModule::setSecondCursorPos(int newPos){
    if(newPos < 0){
        secondCursorPos = 0;
    }
    else if(newPos > int(content.size())){
        secondCursorPos = content.size();
    }
    else{
        secondCursorPos = newPos;
    }
    divideFormattingByCursor();
}
void SuperTextModule::cutContent(size_t newSize){
    content = content.substr(0, newSize);
    fitFormattingToContent();
    //update();
}
void SuperTextModule::saveFormattedTextToTheFile(string filePath){
    std::ofstream File(filePath);

	if(!File){
		std::cerr << "Error: In " << __FUNCTION__ << ": Cannot open file: '" << filePath << "'.\n";
        return;
    }

    mergeFormatting();

    File << Formatting.size() << " ";

    for(const FormatClass & Format : Formatting){
        File << Format.color.r << " " << Format.color.g << " " << Format.color.b << " " << Format.color.a << " ";
        File << Format.accentColor.r << " " << Format.accentColor.g << " " << Format.accentColor.b << " " << Format.accentColor.a << " ";
        File << Format.Font->ID << " ";
        File << Format.offset.x << " " << Format.offset.y << " ";
        File << Format.selected << " ";
        File << Format.limit << " ";
        File << Format.drawingLimit << " ";
    }

	File << content;
    File.close();
}
void SuperTextModule::loadFormattedTextFromTheFile(string filePath, vector<SingleFont> & FontContainer){
    std::ifstream File(filePath);

	if(!File){
		std::cerr << "Error: In " << __FUNCTION__ << ": Cannot open file: '" << filePath << "'.\n";
        return;
    }

    unsigned formattingSize = 0;
    string temp, error;
    File >> temp;
    formattingSize = cstoi(temp, error);

    if(formattingSize > 0){
        vector<string> buffer;
        
        while(buffer.size() < formattingSize * 14 && File >> temp){
            buffer.push_back(temp);
        }
        char nextSpace;
        File.get(nextSpace);
        if(nextSpace != ' '){
            std::cerr << "Error: In " << __FUNCTION__ << ": Expected a space, but got: '" << nextSpace << "'.\n";
            return;
        }
        if(buffer.size() < formattingSize * 14){
            std::cerr << "Error: In " << __FUNCTION__ << ": Formatting in the file: '" << filePath << "' is corrupted.\n";
            return;
        }

        vector<FormatClass> FormattingBuffer;
        FormattingBuffer.reserve(formattingSize);
        
        for(unsigned formatIdx = 0; formatIdx < formattingSize; formatIdx++){
            FormattingBuffer.push_back(FormatClass());

            FormattingBuffer.back().color.r = cstof(buffer[formatIdx * 14], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().color.g = cstof(buffer[1 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().color.b = cstof(buffer[2 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().color.a = cstof(buffer[3 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};

            FormattingBuffer.back().accentColor.r = cstof(buffer[4 + formatIdx * 14], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().accentColor.g = cstof(buffer[5 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().accentColor.b = cstof(buffer[6 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().accentColor.a = cstof(buffer[7 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};

            FormattingBuffer.back().Font = findFontByID(FontContainer, buffer[8 + (formatIdx * 14)]);

            FormattingBuffer.back().offset.x = cstof(buffer[9 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().offset.y = cstof(buffer[10 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};

            FormattingBuffer.back().selected = cstoi(buffer[11 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};

            FormattingBuffer.back().limit = cstoi(buffer[12 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
            FormattingBuffer.back().drawingLimit = cstoi(buffer[13 + (formatIdx * 14)], error);
            if(error.size() > 0){break;};
        }
        if(error.size() > 0){
            std::cerr << "Error: In " << __FUNCTION__ << ": Formatting in the file: '" << filePath << "' is corrupted.\n";
            std::cerr << error << "\n";
            return;
        }
        Formatting = FormattingBuffer;
    }
    else{
        std::cerr << "Warning: In " << __FUNCTION__ << ": File: '" << filePath << "' does not have any format.\n";
        
        if(error.size() == 0){ //If the first word was a number (length of the saved formatting), remove a next space to fix the loaded content.
            char nextSpace;
            File.get(nextSpace);
            if(nextSpace != ' '){
                std::cerr << "Error: In " << __FUNCTION__ << ": Expected a space, but got: '" << nextSpace << "'.\n";
                return;
            }
        }
    }
    content.clear();
    std::streampos currentPos = File.tellg();
    File.seekg(0, std::ios::end);
    std::streampos fileSize = File.tellg();
    File.seekg(currentPos, std::ios::beg);
    content.resize(fileSize - currentPos);
    File.read(&content[0], fileSize - currentPos);
    File.close();
}

void SuperEditableTextModule::setUpNewInstance(){
    SuperTextModule::setUpNewInstance();
    previousContent.clear();
    futureContent.clear();
    previousFormatting.clear();
    futureFormatting.clear();

    canBeEdited = true;
    canUseSpace = true;
    canUseEnter = true;
    enterEndsEditing = false;
    canUseTabs = true;
    isNumerical = false;
    hasFloatingPoint = false;
    ignoreVerticalArrows = false;
    ignoreContentRestriction = true;
    isStoringHistory = true;
    canCopyFormat = true;
    minContentLength = 0;
    maxContentLength = 512;
    inputDelay = 0.3;
    repetitionDelay = 0.05;
    protectedArea = 0; //User cannot edit the text placed before the protection cursor.
    isEditingActive = false;
    blockedKeys.clear();
    lastInputedKey = -1;
    currentInputDelay = 0.0;
}
SuperEditableTextModule::SuperEditableTextModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
SuperEditableTextModule::SuperEditableTextModule(unsigned newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
SuperEditableTextModule::SuperEditableTextModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
SuperEditableTextModule::~SuperEditableTextModule(){

}
void SuperEditableTextModule::clone(const SuperEditableTextModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool &changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}
void SuperEditableTextModule::clear(){
    textLines.clear();
    lineWidths.clear();
    lineHeights.clear();
    lineLengths.clear();
    lineStarts.clear();
    floatingNewLine.clear();
    Formatting.clear();
    previousContent.clear();
    futureContent.clear();
    previousFormatting.clear();
    futureFormatting.clear();
    blockedKeys.clear();
}
VariableModule SuperEditableTextModule::getAttributeValue(const string &attribute, const string &detail, EventDescription EventIds) const{
    if(attribute == "original_content"){
        if(previousContent.size() > 0){
            return VariableModule::newString(*previousContent.begin());
        }
        return VariableModule::newString(content);
    }
    if(attribute == "can_be_edited"){
        return VariableModule::newBool(canBeEdited);
    }
    if(attribute == "can_use_space"){
        return VariableModule::newBool(canUseSpace);
    }
    if(attribute == "can_use_enter"){
        return VariableModule::newBool(canUseEnter);
    }
    if(attribute == "enter_ends_editing"){
        return VariableModule::newBool(enterEndsEditing);
    }
    if(attribute == "can_use_tabs"){
        return VariableModule::newBool(canUseTabs);
    }
    if(attribute == "is_numerical"){
        return VariableModule::newBool(isNumerical);
    }
    if(attribute == "has_floating_point"){
        return VariableModule::newBool(hasFloatingPoint);
    }
    if(attribute == "ignore_vertical_arrows"){
        return VariableModule::newBool(ignoreVerticalArrows);
    }
    if(attribute == "ignore_content_restriction"){
        return VariableModule::newBool(ignoreContentRestriction);
    }
    if(attribute == "is_storing_history"){
        return VariableModule::newBool(isStoringHistory);
    }
    if(attribute == "min_content_length"){
        return VariableModule::newInt(minContentLength);
    }
    if(attribute == "max_content_length"){
        return VariableModule::newInt(maxContentLength);
    }
    if(attribute == "input_delay"){
        return VariableModule::newDouble(inputDelay);
    }
    if(attribute == "repetition_delay"){
        return VariableModule::newDouble(repetitionDelay);
    }
    if(attribute == "protected_area"){
        return VariableModule::newInt(protectedArea);
    }
    if(attribute == "is_editing_active"){
        return VariableModule::newBool(isEditingActive);
    }
    if(attribute == "cursor_pos"){
        return VariableModule::newInt(cursorPos);
    }
    if(attribute == "second_cursor_pos"){
        return VariableModule::newInt(secondCursorPos);
    }
    if(attribute == "min_cursor_pos"){
        return VariableModule::newInt(std::min(cursorPos, secondCursorPos));
    }
    if(attribute == "max_cursor_pos"){
        return VariableModule::newInt(std::max(cursorPos, secondCursorPos));
    }
    return SuperTextModule::getAttributeValue(attribute, detail, EventIds);
}
void SuperEditableTextModule::getContext(string attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "test"){
        BasePointers.back().setPointer(&content);
    }
    if(attribute == "original_content"){
        if(previousContent.size() > 0){
            BasePointers.back().setPointer(&previousContent[0]);
        }
        else{
            BasePointers.back().setPointer(&content);
        }
        BasePointers.back().readOnly = true;
    }
    else if(attribute == "can_be_edited"){
        BasePointers.back().setPointer(&canBeEdited);
    }
    else if(attribute == "can_use_space"){
        BasePointers.back().setPointer(&canUseSpace);
    }
    else if(attribute == "can_use_enter"){
        BasePointers.back().setPointer(&canUseEnter);
    }
    else if(attribute == "enter_ends_editing"){
        BasePointers.back().setPointer(&enterEndsEditing);
    }
    else if(attribute == "can_use_tabs"){
        BasePointers.back().setPointer(&canUseTabs);
    }
    else if(attribute == "is_numerical"){
        BasePointers.back().setPointer(&isNumerical);
    }
    else if(attribute == "has_floating_point"){
        BasePointers.back().setPointer(&hasFloatingPoint);
    }
    else if(attribute == "ignore_vertical_arrows"){
        BasePointers.back().setPointer(&ignoreVerticalArrows);
    }
    else if(attribute == "ignore_content_restriction"){
        BasePointers.back().setPointer(&ignoreContentRestriction);
    }
    else if(attribute == "is_storing_history"){
        BasePointers.back().setPointer(&isStoringHistory);
    }
    else if(attribute == "min_content_length"){
        BasePointers.back().setPointer(&minContentLength);
    }
    else if(attribute == "max_content_length"){
        BasePointers.back().setPointer(&maxContentLength);
    }
    else if(attribute == "input_delay"){
        BasePointers.back().setPointer(&inputDelay);
    }
    else if(attribute == "repetition_delay"){
        BasePointers.back().setPointer(&repetitionDelay);
    }
    else if(attribute == "protected_area"){
        BasePointers.back().setPointer(&protectedArea);
    }
    else if(attribute == "is_editing_active"){
        BasePointers.back().setPointer(&isEditingActive);
        BasePointers.back().readOnly = true;
    }
    else if(attribute == "cursor_pos"){
        BasePointers.back().setPointer(&cursorPos);
        BasePointers.back().readOnly = true;
    }
    else if(attribute == "second_cursor_pos"){
        BasePointers.back().setPointer(&secondCursorPos);
        BasePointers.back().readOnly = true;
    }
    else{
        BasePointers.pop_back();
        SuperTextModule::getContext(attribute, BasePointers);
    }
}
bool SuperEditableTextModule::prepareEditing(const vector <short> & releasedKeys, vector <short> & pressedKeys, bool & shift, bool & control){
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
        if(pressedKeys[i] == ALLEGRO_KEY_ENTER && canUseEnter && enterEndsEditing){
            isEditingActive = false;
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
void SuperEditableTextModule::executeOneBackspaceOrCtrlX(char pKey, string text, ALLEGRO_DISPLAY * display, unsigned & leftCursorOnFormatIdx,
    unsigned & rightCursorOnFormatIdx, bool ENABLE_al_set_clipboard_text, string & internalClipboard,
    vector<FormatClass> & CopiedFormatting, string EXE_PATH
){
    if(text.size() <= minContentLength){
        return;
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
            internalClipboard = clipboard;

            if(ENABLE_al_set_clipboard_text){
                if(display != nullptr){
                    al_set_clipboard_text(display, clipboard.c_str());
                }
            }
            else{
                std::ofstream File(EXE_PATH + "clipboard.txt", std::ios::trunc | std::ios::out);
                if(File){
                    File << internalClipboard;
                }
                File.close();
            }

            CopiedFormatting.clear();
            CopiedFormatting.insert(CopiedFormatting.begin(), Formatting.begin() + leftCursorOnFormatIdx,
                Formatting.begin() + rightCursorOnFormatIdx + 1);
        }

        newContent = text.substr(0, selectionStart) + text.substr(selectionEnd + 1, text.size()-selectionEnd);
        cursorPos = selectionStart;

        Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1, Formatting.begin() + rightCursorOnFormatIdx + 1);
        if(Formatting.size() > leftCursorOnFormatIdx + 1){
            Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
            Formatting[leftCursorOnFormatIdx + 1].limit--;
            if(Formatting[leftCursorOnFormatIdx + 1].limit == 0){
                Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1);
            }
        }
        Formatting[leftCursorOnFormatIdx].limit = 1;
        Formatting[leftCursorOnFormatIdx].selected = true;
        rightCursorOnFormatIdx = leftCursorOnFormatIdx;
    }
    else{
        if(cursorPos == protectedArea || secondCursorPos == protectedArea || cursorPos == 0){
            return;
        }
        newContent = text.substr(0, cursorPos-1) + text.substr(cursorPos, text.size()-cursorPos);
        cursorPos--;
        
        if(Formatting[leftCursorOnFormatIdx - 1].limit == 1){
            Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx - 1);
            leftCursorOnFormatIdx--;
        }
        else{
            Formatting[leftCursorOnFormatIdx - 1].limit--;
        }
    }
    content = newContent;
    secondCursorPos = cursorPos;
}
void SuperEditableTextModule::executeOneDeletion(string text, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(text.size() <= minContentLength){
        return;
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

        Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1, Formatting.begin() + rightCursorOnFormatIdx + 1);
        if(Formatting.size() > leftCursorOnFormatIdx + 1){
            Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
            Formatting[leftCursorOnFormatIdx + 1].limit--;
            if(Formatting[leftCursorOnFormatIdx + 1].limit == 0){
                Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1);
            }
        }
        Formatting[leftCursorOnFormatIdx].limit = 1;
        rightCursorOnFormatIdx = leftCursorOnFormatIdx;
    }
    else{
        if(cursorPos >= text.size()){
            return;
        }
        newContent = text.substr(0, cursorPos) + text.substr(cursorPos+1, text.size()-cursorPos);
        
        if(Formatting[leftCursorOnFormatIdx].limit == 1){
            Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
            Formatting[leftCursorOnFormatIdx].limit = 1;
            Formatting[leftCursorOnFormatIdx].selected = true;
            Formatting[leftCursorOnFormatIdx + 1].limit--;
            if(Formatting[leftCursorOnFormatIdx + 1].limit == 0){
                Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1);
            }
        }
        else{
            Formatting[leftCursorOnFormatIdx].limit--;
        }
    }
    content = newContent;
    secondCursorPos = cursorPos;
}
bool SuperEditableTextModule::deleteFromText(char pKey, string text, bool & control,
    ALLEGRO_DISPLAY * display, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx,
    bool ENABLE_al_set_clipboard_text, string & internalClipboard, vector<FormatClass> & CopiedFormatting,
    string EXE_PATH
){
    if(pKey != ALLEGRO_KEY_BACKSPACE && pKey != ALLEGRO_KEY_DELETE && (pKey != ALLEGRO_KEY_X || !control)){
        return false;
    }
    if(cursorPos < protectedArea || secondCursorPos < protectedArea){
        return true;
    }
    if(pKey == ALLEGRO_KEY_BACKSPACE || (pKey == ALLEGRO_KEY_X && control && cursorPos != secondCursorPos)){
        
        //If ctrl key is pressed, find the distance to the beginning of the current word. Later delete all letters from the cursor to the end of this word - letter by letter.
        unsigned letterShift = cursorPos;
        if(control && letterShift > protectedArea && letterShift > 0){
            letterShift--;
            for(; letterShift > 0; letterShift--){
                if(content[letterShift] == ' ' || content[letterShift] == '\t' || content[letterShift] == '\n'){
                    break;
                }
            }
            if(letterShift == 0 && content[0] != ' ' && content[0] != '\t' && content[0] != '\n'){
                letterShift = cursorPos - letterShift;
            }
            else{
                letterShift = cursorPos - letterShift - 1;
            }
            if(letterShift == 0){
                letterShift = 1;
            }
        }
        else{
            letterShift = 1;
        }

        for(; letterShift > 0; letterShift--){
            text = content;
            executeOneBackspaceOrCtrlX(pKey, text, display, leftCursorOnFormatIdx, rightCursorOnFormatIdx,
                ENABLE_al_set_clipboard_text, internalClipboard, CopiedFormatting, EXE_PATH
            );
        }
        return true;
    }
    else if(pKey == ALLEGRO_KEY_DELETE){
        //If ctrl key is pressed, find the distance to the end of the current word. Later delete all letters from the cursor to the start of this word - letter by letter.
        unsigned letterShift = cursorPos + 1;
        if(control){
            for(; letterShift < content.size(); letterShift++){
                if(content[letterShift] == ' ' || content[letterShift] == '\t' || content[letterShift] == '\n'){
                    break;
                }
            }
        }
        letterShift -= cursorPos;

        for(; letterShift > 0; letterShift--){
            text = content;
            executeOneDeletion(text, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
        }
        
        return true;
    }
    return false;
}
void SuperEditableTextModule::getNumbers(char pKey, char & character, bool shift){
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
void SuperEditableTextModule::addFloatingPoint(char pKey, char & character, string text){
    if(character != '\0'){
        return;
    }
    if(pKey == 73 && (hasFloatingPoint || isNumerical)){
        bool isTherePoint = false;
        if(isNumerical){
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
bool SuperEditableTextModule::addMinus(char pKey, char & character, string text){
    if(pKey != ALLEGRO_KEY_MINUS || !isNumerical){
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
void SuperEditableTextModule::getLetters(char pKey, char & character, bool shift){
    if(character != '\0' || isNumerical){
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
    else if(pKey == ALLEGRO_KEY_SPACE && canUseSpace){
        character = ' ';
    }
    else if(pKey == ALLEGRO_KEY_TAB && canUseTabs){
        character = '\t';
    }
    else if(pKey == ALLEGRO_KEY_ENTER && canUseEnter){
        character = '\n';
    }
    else{
        character = '\0';
    }
}
void SuperEditableTextModule::prepareToMoveCursorUp(bool shift, float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength,
    unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx
){
    //Prepare data about the current position.
    if(shift){
        currentWidth = lineWidthToCursor;
        letterIdx = cursorPos;
        currentLineLength = localCursorPos;
        if(cursorPos <= secondCursorPos){
            formatIdx = leftCursorOnFormatIdx;
            limit = 0;
        }
        else{
            formatIdx = rightCursorOnFormatIdx;
            limit = Formatting[rightCursorOnFormatIdx].limit - 1;
        }
    }
    else{
        if(cursorPos <= secondCursorPos){
            currentWidth = lineWidthToCursor;
            letterIdx = cursorPos;
            currentLineLength = localCursorPos;
        }
        else{
            currentWidth = lineWidthToSecondCursor;
            letterIdx = secondCursorPos;
            currentLineLength = localSecondCursorPos;
            lineWithCursorIdx = lineWithSecondCursorIdx;
        }
        formatIdx = leftCursorOnFormatIdx;
        limit = 0;
    }
}
bool SuperEditableTextModule::isCursorOnTheFirstLine(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    //Move cursors (text and format) to the beginning of the first line (like in most text editors).
    if(lineWithCursorIdx == 0){
        if(shift){
            return true;
        }
        lineWithSecondCursorIdx = 0;
        cursorPos = 0;
        secondCursorPos = 0;
        for(unsigned i = leftCursorOnFormatIdx; i <= rightCursorOnFormatIdx; i++){
            Formatting[i].selected = false;
        }
        if(Formatting[0].limit == 1){
            Formatting[0].selected = true;
        }
        else{
            Formatting.insert(Formatting.begin(), FormatClass());
            Formatting[0] = Formatting[1];
            Formatting[0].limit = 1;
            Formatting[0].selected = true;
            Formatting[1].limit--;
        }
        
        leftCursorOnFormatIdx = 0;
        rightCursorOnFormatIdx = 0;
        return true;
    }
    return false;
}
void SuperEditableTextModule::takeCareOfLastLineIfEmpty(float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit){
    //Ignore the empty last line. 
    if(letterIdx == content.size() && textLines.back() != " "){
        letterIdx--;
        currentLineLength--;
        currentWidth -= al_get_text_width(Formatting[formatIdx].Font->font, string(" ").c_str());
        if(limit == 0){
            formatIdx--;
            limit = Formatting[formatIdx].limit - 1;
        }
        else{
            limit--;
        }
    }
}
void SuperEditableTextModule::moveCursorToTheStartOfTheCurrentTextLine(unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit){
    //Move cursors to the start of the current line.
    bool onlySpacesLeft = false;
    for(; currentLineLength > 0; currentLineLength--, letterIdx--){
        if(content[letterIdx] == '\t'){
            for(unsigned i = currentLineLength;;i--){
                if(textLines[lineWithCursorIdx][i] != ' '){
                    break;
                }
                if(i == 0){
                    onlySpacesLeft = true;
                    break;
                }
            }
            if(onlySpacesLeft){
                break;
            }
        }
        while(content[letterIdx] != '\n' && content[letterIdx] != textLines[lineWithCursorIdx][currentLineLength]
            && !(content[letterIdx] == '\t' && textLines[lineWithCursorIdx][currentLineLength] == ' '))
        {
            currentLineLength--;
            if(currentLineLength == 0){
                break;
            }
        }
        if(currentLineLength == 0){
            break;
        }
        if(limit == 0){
            formatIdx--;
            limit = Formatting[formatIdx].limit - 1;
            continue;
        }
        limit--;
    }
    if(onlySpacesLeft /*&& letterIdx - lineStarts[lineWithCursorIdx] > 0*/){
        unsigned realCharacters = letterIdx - lineStarts[lineWithCursorIdx];
        for(; realCharacters > 0; realCharacters--, letterIdx--){
            if(currentLineLength == 0){
                break;
            }
            if(limit == 0){
                formatIdx--;
                limit = Formatting[formatIdx].limit - 1;
                continue;
            }
            limit--;
        }
        currentLineLength = 0;
        if(lineWithCursorIdx == 0){
            letterIdx--;
            if(limit == 0){
                formatIdx--;
                limit = Formatting[formatIdx].limit - 1;
            }
            limit--;
        }
    }
    if(letterIdx != lineStarts[lineWithCursorIdx]){
        cerr << "Error: In: " << __FUNCTION__ << ": Cursor moved passed the current line (cursorPos: "
            << letterIdx << "; line start: " << lineStarts[lineWithCursorIdx] << ").\n";
    }
}
void SuperEditableTextModule::moveCursorToTheStartOfThePreviousTextLine(bool shift, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit){
    //Move cursors to the start of the previous line to ensure tabs are measured correctly.
    if(limit == 0){
        formatIdx--;
        limit = Formatting[formatIdx].limit;
    }
    limit--;
    letterIdx--;
    lineWithCursorIdx--;
    if(!shift){
        lineWithSecondCursorIdx = lineWithCursorIdx;
    }
    currentLineLength = lineLengths[lineWithCursorIdx] - 1;
    bool onlySpacesLeft = false;
    for(; currentLineLength > 0 && letterIdx > 0; currentLineLength--, letterIdx--){
        if(content[letterIdx] == '\t'){
            for(unsigned i = currentLineLength;;i--){
                if(textLines[lineWithCursorIdx][i] != ' '){
                    break;
                }
                if(i == 0){
                    onlySpacesLeft = true;
                    break;
                }
            }
            if(onlySpacesLeft){
                break;
            }
        }
        while(content[letterIdx] != '\n' && content[letterIdx] != textLines[lineWithCursorIdx][currentLineLength]
            && !(content[letterIdx] == '\t' && textLines[lineWithCursorIdx][currentLineLength] == ' '))
        {
            currentLineLength--;
            if(currentLineLength == 0){
                break;
            }
        }
        if(currentLineLength == 0){
            break;
        }
        if(limit == 0){
            formatIdx--;
            limit = Formatting[formatIdx].limit - 1;
            continue;
        }
        limit--;
    }

    if(onlySpacesLeft /*&& letterIdx - lineStarts[lineWithCursorIdx] > 0*/){
        unsigned realCharacters = letterIdx - lineStarts[lineWithCursorIdx];
        for(; realCharacters > 0; realCharacters--, letterIdx--){
            if(currentLineLength == 0){
                break;
            }
            if(limit == 0){
                formatIdx--;
                limit = Formatting[formatIdx].limit - 1;
                continue;
            }
            limit--;
        }
        currentLineLength = 0;
        if(lineWithCursorIdx == 0){
            letterIdx--;
            if(limit == 0){
                formatIdx--;
                limit = Formatting[formatIdx].limit - 1;
            }
            limit--;
        }
    }

    if(letterIdx == 0){
        currentLineLength = 0;
    }
}
void SuperEditableTextModule::moveCursorToTheSavedWidth(float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength,
    unsigned & formatIdx, unsigned & limit, float selectedWidth, unsigned & currentTabLength){
    currentWidth = 0.0;
    currentTabLength = 0;
    char letter;
    float spaceWidth = al_get_text_width(Formatting[formatIdx].Font->font, string(" ").c_str());
    for(; currentWidth < selectedWidth && letterIdx < content.size(); currentLineLength++, letterIdx++){
        if(content[letterIdx] == '\t'){
            currentTabLength = tabLength - currentLineLength % tabLength;
            currentWidth += spaceWidth * currentTabLength;
            currentLineLength += currentTabLength - 1;
            if(currentWidth > selectedWidth){
                break;
            }
        }
        else{
            letter = content.substr(letterIdx, 1)[0];
            currentWidth += al_get_text_width(Formatting[formatIdx].Font->font, string(1, letter).c_str());
            if(currentWidth > selectedWidth){
                break;
            }
        }
        if(currentWidth >= lineWidths[lineWithCursorIdx]){
            break;
        }
        limit++;
        if(limit >= Formatting[formatIdx].limit){
            formatIdx++;
            limit = 0;
        }
    }
}
void SuperEditableTextModule::updateFormattingForUpMoveWithoutShift(unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    for(unsigned i = leftCursorOnFormatIdx; i <= rightCursorOnFormatIdx; i++){
        Formatting[i].selected = false;
    }
    if(Formatting[formatIdx].limit == 1){
        Formatting[formatIdx].selected = true;
        leftCursorOnFormatIdx = formatIdx;
    }
    else if(limit == 0){
        Formatting[formatIdx].limit--;
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting[formatIdx] = Formatting[formatIdx + 1];
        Formatting[formatIdx].limit = 1;
        Formatting[formatIdx].selected = true;
        leftCursorOnFormatIdx = formatIdx;
    }
    else if(limit == Formatting[formatIdx].limit - 1){
        Formatting[formatIdx].limit--;
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting[formatIdx] = Formatting[formatIdx + 1];
        Formatting[formatIdx + 1].limit = 1;
        Formatting[formatIdx + 1].selected = true;
        leftCursorOnFormatIdx = formatIdx + 1;
    }
    else{
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting[formatIdx] = Formatting[formatIdx + 2];
        Formatting[formatIdx + 1] = Formatting[formatIdx + 2];
        Formatting[formatIdx].limit = limit;
        Formatting[formatIdx + 1].limit = 1;
        Formatting[formatIdx + 1].selected = true;
        Formatting[formatIdx + 2].limit -= limit + 1;
        leftCursorOnFormatIdx = formatIdx + 1;
    }
    rightCursorOnFormatIdx = leftCursorOnFormatIdx;
}
void SuperEditableTextModule::updateFormattingForUpMoveWithShift(unsigned & letterIdx, unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    unsigned i;
    if(cursorPos > secondCursorPos && letterIdx <= secondCursorPos){
        for(i = rightCursorOnFormatIdx; i > leftCursorOnFormatIdx; i--){
            Formatting[i].selected = false;
        }
        if(Formatting[leftCursorOnFormatIdx].limit > 1){
            Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FormatClass());
            Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
            Formatting[leftCursorOnFormatIdx].limit = 1;
            Formatting[leftCursorOnFormatIdx + 1].limit--;
            Formatting[leftCursorOnFormatIdx + 1].selected = false;
        }
        rightCursorOnFormatIdx = leftCursorOnFormatIdx;
        for(i = rightCursorOnFormatIdx - 1; i > formatIdx; i--){
            Formatting[i].selected = true;
        }
        if(limit == 0){
            Formatting[formatIdx].selected = true;
            leftCursorOnFormatIdx = formatIdx;
        }
        else{
            Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
            Formatting[formatIdx] = Formatting[formatIdx + 1];
            Formatting[formatIdx].limit = limit;
            Formatting[formatIdx + 1].limit -= limit;
            Formatting[formatIdx + 1].selected = true;
            leftCursorOnFormatIdx = formatIdx + 1;
        }
    }
    else if(cursorPos <= secondCursorPos){
        for(i = leftCursorOnFormatIdx; i > formatIdx; i--){
            Formatting[i].selected = true;
        }
        for(i = leftCursorOnFormatIdx; i <= formatIdx; i++){
            Formatting[i].selected = false;
        }
        if(limit == 0){
            Formatting[formatIdx].selected = true;
            leftCursorOnFormatIdx = formatIdx;
        }
        else{
            Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
            Formatting[formatIdx] = Formatting[formatIdx + 1];
            Formatting[formatIdx].limit = limit;
            Formatting[formatIdx + 1].limit -= limit;
            Formatting[formatIdx + 1].selected = true;
            leftCursorOnFormatIdx = formatIdx + 1;
        }
    }
    else{
        for(i = rightCursorOnFormatIdx; i >= formatIdx; i--){
            Formatting[i].selected = false;
        }
        if(limit == Formatting[formatIdx].limit - 1){
            Formatting[formatIdx].selected = true;
        }
        else{
            Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
            Formatting[formatIdx] = Formatting[formatIdx + 1];
            Formatting[formatIdx].limit = limit + 1;
            Formatting[formatIdx].selected = true;
            Formatting[formatIdx + 1].limit -= limit + 1;
        }
        rightCursorOnFormatIdx = formatIdx;
    }
}
void SuperEditableTextModule::moveCursorUp(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(cursorPos == 0){
        return;
    }
    float currentWidth = 0;
    unsigned letterIdx = 0;
    unsigned currentLineLength = 0;
    unsigned formatIdx = 0;
    unsigned limit = 0;

    prepareToMoveCursorUp(shift, currentWidth, letterIdx, currentLineLength,
        formatIdx, limit, leftCursorOnFormatIdx, rightCursorOnFormatIdx
    );
    
    if(isCursorOnTheFirstLine(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx)){
        return;
    }

    takeCareOfLastLineIfEmpty(currentWidth, letterIdx, currentLineLength, formatIdx, limit);

    moveCursorToTheStartOfTheCurrentTextLine(letterIdx, currentLineLength, formatIdx, limit);

    moveCursorToTheStartOfThePreviousTextLine(shift, letterIdx, currentLineLength, formatIdx, limit);

    unsigned currentTabLength; //This is needed for selecting text with the mouse.
    moveCursorToTheSavedWidth(currentWidth, letterIdx, currentLineLength, formatIdx, limit, lineWidthToCursor, currentTabLength);
    
    if(!shift){
        cursorPos = letterIdx;
        secondCursorPos = cursorPos;
        updateFormattingForUpMoveWithoutShift(formatIdx, limit, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
    }
    else{
        updateFormattingForUpMoveWithShift(letterIdx, formatIdx, limit, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
        cursorPos = letterIdx;
    }
}
void SuperEditableTextModule::prepareToMoveCursorDown(bool shift, float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength,
    unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx
){
    //Prepare data about the current position.
    if(shift){
        currentWidth = lineWidthToCursor;
        letterIdx = cursorPos;
        currentLineLength = localCursorPos;
        if(cursorPos <= secondCursorPos){
            formatIdx = leftCursorOnFormatIdx;
            limit = 0;
        }
        else{
            formatIdx = rightCursorOnFormatIdx;
            limit = Formatting[rightCursorOnFormatIdx].limit - 1;
        }
    }
    else{
        if(cursorPos >= secondCursorPos){
            currentWidth = lineWidthToCursor;
            letterIdx = cursorPos;
            currentLineLength = localCursorPos;
        }
        else{
            currentWidth = lineWidthToSecondCursor;
            letterIdx = secondCursorPos;
            currentLineLength = localSecondCursorPos;
            lineWithCursorIdx = lineWithSecondCursorIdx;
        }
        formatIdx = rightCursorOnFormatIdx;
        limit = Formatting[rightCursorOnFormatIdx].limit - 1;
    }
}
bool SuperEditableTextModule::isCursorOnTheLastLine(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    //Move cursors (text and format) to the beginning of the first line (like in most text editors).
    if(lineWithCursorIdx >= textLines.size() - 1){
        if(shift){
            return true;
        }
        lineWithSecondCursorIdx = textLines.size() - 1;
        cursorPos = content.size();
        secondCursorPos = cursorPos;
        for(unsigned i = leftCursorOnFormatIdx; i <= rightCursorOnFormatIdx; i++){
            Formatting[i].selected = false;
        }
        if(Formatting.back().limit == 1){
            Formatting.back().selected = true;
        }
        else{
            Formatting.push_back(FormatClass());
            Formatting.back() = Formatting[Formatting.size() - 2];
            Formatting.back().limit = 1;
            Formatting.back().selected = true;
            Formatting[Formatting.size() - 2].limit--;
        }
        
        leftCursorOnFormatIdx = Formatting.size() - 1;
        rightCursorOnFormatIdx = Formatting.size() - 1;
        return true;
    }
    return false;
}
void SuperEditableTextModule::moveToTheEndOfTheLine(float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit){
    unsigned currentTabLength = 0;
    char letter;
    float spaceWidth = al_get_text_width(Formatting[formatIdx].Font->font, string(" ").c_str());
    for(; currentWidth < lineWidths[lineWithCursorIdx]; currentLineLength++, letterIdx++){
        if(content[letterIdx] == '\t'){
            currentTabLength = tabLength - currentLineLength % tabLength;
            currentWidth += spaceWidth * currentTabLength;
            currentLineLength += currentTabLength - 1;
        }
        else{
            letter = content.substr(letterIdx, 1)[0];
            currentWidth += al_get_text_width(Formatting[formatIdx].Font->font, string(1, letter).c_str());
        }
        limit++;
        if(limit >= Formatting[formatIdx].limit){
            formatIdx++;
            limit = 0;
        }
    }
    if(floatingNewLine[lineWithCursorIdx]){
        currentLineLength++;
        letterIdx++;
        limit++;
        if(limit >= Formatting[formatIdx].limit){
            formatIdx++;
            limit = 0;
        }
    }
}
void SuperEditableTextModule::updateFormattingForDownMoveWithoutShift(unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    for(unsigned i = leftCursorOnFormatIdx; i <= rightCursorOnFormatIdx; i++){
        Formatting[i].selected = false;
    }
    if(Formatting[formatIdx].limit == 1){
        Formatting[formatIdx].selected = true;
        rightCursorOnFormatIdx = formatIdx;
    }
    else if(limit == 0){
        Formatting[formatIdx].limit--;
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting[formatIdx] = Formatting[formatIdx + 1];
        Formatting[formatIdx].limit = 1;
        Formatting[formatIdx].selected = true;
        rightCursorOnFormatIdx = formatIdx;
    }
    else if(limit == Formatting[formatIdx].limit - 1){
        Formatting[formatIdx].limit--;
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting[formatIdx] = Formatting[formatIdx + 1];
        Formatting[formatIdx + 1].limit = 1;
        Formatting[formatIdx + 1].selected = true;
        rightCursorOnFormatIdx = formatIdx + 1;
    }
    else{
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
        Formatting[formatIdx] = Formatting[formatIdx + 2];
        Formatting[formatIdx + 1] = Formatting[formatIdx + 2];
        Formatting[formatIdx].limit = limit;
        Formatting[formatIdx + 1].limit = 1;
        Formatting[formatIdx + 1].selected = true;
        Formatting[formatIdx + 2].limit -= limit + 1;
        rightCursorOnFormatIdx = formatIdx + 1;
    }
    leftCursorOnFormatIdx = rightCursorOnFormatIdx;
}
void SuperEditableTextModule::updateFormattingForDownMoveWithShift(unsigned & letterIdx, unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    unsigned i;
    if(cursorPos < secondCursorPos && letterIdx >= secondCursorPos){
        for(i = leftCursorOnFormatIdx; i < rightCursorOnFormatIdx; i++){
            Formatting[i].selected = false;
        }
        if(Formatting[rightCursorOnFormatIdx].limit > 1){
            Formatting.insert(Formatting.begin() + rightCursorOnFormatIdx, FormatClass());
            Formatting[rightCursorOnFormatIdx] = Formatting[rightCursorOnFormatIdx + 1];
            Formatting[rightCursorOnFormatIdx].limit--;
            Formatting[rightCursorOnFormatIdx].selected = false;
            Formatting[rightCursorOnFormatIdx + 1].limit = 1;
            rightCursorOnFormatIdx++;
            formatIdx++;
        }
        leftCursorOnFormatIdx = rightCursorOnFormatIdx;
        for(i = rightCursorOnFormatIdx; i < formatIdx; i++){
            Formatting[i].selected = true;
        }
        if(limit == Formatting[formatIdx].limit - 1){
            Formatting[formatIdx].selected = true;
        }
        else{
            Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
            Formatting[formatIdx] = Formatting[formatIdx + 1];
            Formatting[formatIdx].limit = limit + 1;
            Formatting[formatIdx].selected = true;
            Formatting[formatIdx + 1].limit -= limit + 1;
        }
        rightCursorOnFormatIdx = formatIdx;
    }
    else if(cursorPos >= secondCursorPos){
        for(i = rightCursorOnFormatIdx; i < formatIdx; i++){
            Formatting[i].selected = true;
        }
        for(i = rightCursorOnFormatIdx; i >= formatIdx; i--){
            Formatting[i].selected = false;
        }
        if(limit == Formatting[formatIdx].limit - 1){
            Formatting[formatIdx].selected = true;
        }
        else{
            Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
            Formatting[formatIdx] = Formatting[formatIdx + 1];
            Formatting[formatIdx].limit = limit + 1;
            Formatting[formatIdx].selected = true;
            Formatting[formatIdx + 1].limit -= limit + 1;
        }
        rightCursorOnFormatIdx = formatIdx;
    }
    else{
        for(i = leftCursorOnFormatIdx; i <= formatIdx; i++){
            Formatting[i].selected = false;
        }
        if(limit == 0){
            Formatting[formatIdx].selected = true;
            leftCursorOnFormatIdx = formatIdx;
        }
        else{
            Formatting.insert(Formatting.begin() + formatIdx, FormatClass());
            Formatting[formatIdx] = Formatting[formatIdx + 1];
            Formatting[formatIdx].limit = limit;
            Formatting[formatIdx + 1].limit -= limit;
            Formatting[formatIdx + 1].selected = true;
            leftCursorOnFormatIdx = formatIdx + 1;
        }
    }
}
void SuperEditableTextModule::moveCursorDown(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    float currentWidth = 0;
    unsigned letterIdx = 0;
    unsigned currentLineLength = 0;
    unsigned formatIdx = 0;
    unsigned limit = 0;

    prepareToMoveCursorDown(shift, currentWidth, letterIdx, currentLineLength,
        formatIdx, limit, leftCursorOnFormatIdx, rightCursorOnFormatIdx
    );

    if(isCursorOnTheLastLine(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx)){
        return;
    }

    moveToTheEndOfTheLine(currentWidth, letterIdx, currentLineLength, formatIdx, limit);

    //Move to the saved width on the next line
    currentLineLength = 0;
    lineWithCursorIdx++;
    if(!shift){
        lineWithSecondCursorIdx = lineWithCursorIdx;
    }
    unsigned currentTabLength; //This is needed for selecting text with the mouse.
    moveCursorToTheSavedWidth(currentWidth, letterIdx, currentLineLength, formatIdx, limit, lineWidthToCursor, currentTabLength);
    if(lineWithCursorIdx == textLines.size() - 1 && textLines.back() != " "
        && (currentWidth < lineWidthToCursor || (currentWidth == lineWidthToCursor
        && currentWidth == lineWidths[lineWithCursorIdx]))
    ){
        letterIdx++;
        limit++;
        if(limit >= Formatting[formatIdx].limit){
            formatIdx++;
            limit = 0;
        }
    }
    
    if(!shift){
        cursorPos = letterIdx;
        secondCursorPos = cursorPos;
        updateFormattingForDownMoveWithoutShift(formatIdx, limit, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
    }
    else{
        updateFormattingForDownMoveWithShift(letterIdx, formatIdx, limit, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
        cursorPos = letterIdx;
    }
}
void SuperEditableTextModule::moveCursorToLeftByOne(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(!shift){
        cursorPos = std::min(cursorPos, secondCursorPos);
        if(cursorPos > 0){
            cursorPos--;
        }
        if(cursorPos < protectedArea){
            cursorPos = protectedArea;
            secondCursorPos = cursorPos;
            for(unsigned formatIdx = leftCursorOnFormatIdx + 1; formatIdx < rightCursorOnFormatIdx; formatIdx++){
                Formatting[formatIdx].selected = false;
            }
            rightCursorOnFormatIdx = leftCursorOnFormatIdx;
            if(Formatting[leftCursorOnFormatIdx].limit > 1){
                Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FormatClass());
                Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
                Formatting[leftCursorOnFormatIdx].limit = 1;
                Formatting[leftCursorOnFormatIdx + 1].limit--;
                Formatting[leftCursorOnFormatIdx + 1].selected = false;
            }
            return;
        }
        secondCursorPos = cursorPos;

        //Seperate the format of the letter on the left if possible.
        if(leftCursorOnFormatIdx > 0){
            if(Formatting[leftCursorOnFormatIdx - 1].limit > 1){
                Formatting[leftCursorOnFormatIdx - 1].limit--;
                Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FormatClass());
                Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx - 1];
                Formatting[leftCursorOnFormatIdx].limit = 1;
                rightCursorOnFormatIdx++;
            }
            else{
                leftCursorOnFormatIdx--;
            }
        }
        else{
            if(Formatting[0].limit > 1){
                Formatting.insert(Formatting.begin(), FormatClass());
                Formatting[0] = Formatting[1];
                Formatting[0].limit = 1;
                Formatting[1].limit--;
                rightCursorOnFormatIdx++;
            }
        }

        Formatting[leftCursorOnFormatIdx].selected = true;

        //Unselect all formatting on the right.
        for(unsigned formatIdx = leftCursorOnFormatIdx + 1; formatIdx <= rightCursorOnFormatIdx; formatIdx++){
            Formatting[formatIdx].selected = false;
        }

        rightCursorOnFormatIdx = leftCursorOnFormatIdx;
    }
    else{
        if(cursorPos == protectedArea){
            return;
        }
        if(cursorPos <= secondCursorPos){
            if(leftCursorOnFormatIdx > 0){
                if(Formatting[leftCursorOnFormatIdx - 1].limit > 1){
                    Formatting[leftCursorOnFormatIdx - 1].limit--;
                    Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FormatClass());
                    Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx - 1];
                    Formatting[leftCursorOnFormatIdx].limit = 1;
                    rightCursorOnFormatIdx++;
                }
                else{
                    leftCursorOnFormatIdx--;
                }
                Formatting[leftCursorOnFormatIdx].selected = true;
            }
        }
        else{
            if(Formatting[rightCursorOnFormatIdx].limit > 1){
                if(rightCursorOnFormatIdx + 1 < Formatting.size()){
                    Formatting[rightCursorOnFormatIdx].limit--;
                    Formatting.insert(Formatting.begin() + rightCursorOnFormatIdx + 1, FormatClass());
                    Formatting[rightCursorOnFormatIdx + 1] = Formatting[rightCursorOnFormatIdx];
                    Formatting[rightCursorOnFormatIdx + 1].limit = 1;
                    Formatting[rightCursorOnFormatIdx + 1].selected = false;
                }
                else{
                    Formatting.push_back(FormatClass());
                    Formatting[rightCursorOnFormatIdx + 1] = Formatting[rightCursorOnFormatIdx];
                    Formatting[rightCursorOnFormatIdx + 1].limit = 1;
                    Formatting[rightCursorOnFormatIdx + 1].selected = false;
                }
            }
            else{
                Formatting[rightCursorOnFormatIdx].selected = false;
                rightCursorOnFormatIdx--;
            }
        }

        if(cursorPos > 0){
            cursorPos--;
        }
    }
}
void SuperEditableTextModule::moveCursorToLeft(bool shift, bool control, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(cursorPos < protectedArea || secondCursorPos < protectedArea){
        return;
    }

    //If ctrl key is pressed, find the distance to the beginning of the current word. Later move there letter by letter.
    unsigned letterShift = cursorPos;
    if(control && letterShift > protectedArea && letterShift > 0){
        letterShift--;
        for(; letterShift > 0; letterShift--){
            if(content[letterShift] == ' ' || content[letterShift] == '\t' || content[letterShift] == '\n'){
                break;
            }
        }
        if(letterShift == 0 && content[0] != ' ' && content[0] != '\t' && content[0] != '\n'){
            letterShift = cursorPos - letterShift;
        }
        else{
            letterShift = cursorPos - letterShift - 1;
        }
        if(letterShift == 0){
            letterShift = 1;
        }
    }
    else{
        letterShift = 1;
    }

    for(; letterShift > 0; letterShift--){
        moveCursorToLeftByOne(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
    }
}
void SuperEditableTextModule::moveCursorToRightByOne(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(!shift){
        cursorPos = std::max(cursorPos, secondCursorPos);
        if(cursorPos < content.size()){
            cursorPos++;
        }
        secondCursorPos = cursorPos;

        //Seperate the format of the letter on the right if possible.
        if(rightCursorOnFormatIdx + 1 < Formatting.size()){
            if(Formatting[rightCursorOnFormatIdx + 1].limit > 1){
                Formatting[rightCursorOnFormatIdx + 1].limit--;
                Formatting.insert(Formatting.begin() + rightCursorOnFormatIdx + 1, FormatClass());
                Formatting[rightCursorOnFormatIdx + 1] = Formatting[rightCursorOnFormatIdx + 2];
                Formatting[rightCursorOnFormatIdx + 1].limit = 1;
            }
            rightCursorOnFormatIdx++;
        }
        else{
            if(Formatting.back().limit > 1){
                Formatting.push_back(FormatClass());
                Formatting.back() = Formatting[rightCursorOnFormatIdx];
                Formatting.back().limit = 1;
                Formatting[rightCursorOnFormatIdx].limit--;
                rightCursorOnFormatIdx++;
            }
        }

        Formatting[rightCursorOnFormatIdx].selected = true;

        //Unselect all formatting on the right.
        for(unsigned formatIdx = rightCursorOnFormatIdx - 1; formatIdx >= 0; formatIdx--){
            Formatting[formatIdx].selected = false;
            if(formatIdx == 0){
                break;
            }
        }
        leftCursorOnFormatIdx = rightCursorOnFormatIdx;
    }
    else{
        if(cursorPos >= secondCursorPos){
            if(rightCursorOnFormatIdx + 1 < Formatting.size()){
                if(Formatting[rightCursorOnFormatIdx + 1].limit > 1){
                    Formatting[rightCursorOnFormatIdx + 1].limit--;
                    Formatting.insert(Formatting.begin() + rightCursorOnFormatIdx + 1, FormatClass());
                    Formatting[rightCursorOnFormatIdx + 1] = Formatting[rightCursorOnFormatIdx + 2];
                    Formatting[rightCursorOnFormatIdx + 1].limit = 1;
                }
                rightCursorOnFormatIdx++;
                Formatting[rightCursorOnFormatIdx].selected = true;
            }
        }
        else{
            if(Formatting[leftCursorOnFormatIdx].limit > 1){
                Formatting[leftCursorOnFormatIdx].limit--;
                Formatting[leftCursorOnFormatIdx].drawingLimit--;
                Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FormatClass());
                Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
                Formatting[leftCursorOnFormatIdx].limit = 1;
                Formatting[leftCursorOnFormatIdx].drawingLimit = 1;
                Formatting[leftCursorOnFormatIdx].selected = false;
                rightCursorOnFormatIdx++;
            }
            else{
                Formatting[leftCursorOnFormatIdx].selected = false;
            }
            leftCursorOnFormatIdx++;
            
        }

        if(cursorPos < content.size()){
            cursorPos++;
        }
    }
}
void SuperEditableTextModule::moveCursorToRight(bool shift, bool control, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(cursorPos < protectedArea || secondCursorPos < protectedArea){
        return;
    }

    //If ctrl key is pressed, find the distance to the end of the current word. Later move there letter by letter.
    unsigned letterShift = cursorPos + 1;
    if(control){
        for(; letterShift < content.size(); letterShift++){
            if(content[letterShift] == ' ' || content[letterShift] == '\t' || content[letterShift] == '\n'){
                break;
            }
        }
    }
    letterShift -= cursorPos;

    for(; letterShift > 0; letterShift--){
        moveCursorToRightByOne(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
    }
}
void SuperEditableTextModule::edit(vector <short> releasedKeys, vector <short> pressedKeys, ALLEGRO_DISPLAY * display,
    bool ENABLE_al_set_clipboard_text, string & internalClipboard, vector<FormatClass> & CopiedFormatting, string EXE_PATH, bool allowNotAscii
){
    if(!getIsActive() || !isEditingActive){
        return;
    }
    
    unsigned leftCursorOnFormatIdx = 0;
    unsigned rightCursorOnFormatIdx = 0;

    bool firstFound = false;
    //Find first and last selected formatting.
    for(unsigned formatIdx = 0; formatIdx < Formatting.size(); formatIdx++){
        if(Formatting[formatIdx].selected){
            if(!firstFound){
                leftCursorOnFormatIdx = formatIdx;
            }
            rightCursorOnFormatIdx = formatIdx;
            firstFound = true;
        }
        else if(firstFound){
            break;
        }
    }

    bool shift = false;
    bool control = false;
    
    if(!prepareEditing(releasedKeys, pressedKeys, shift, control)){
        return;
    }

    char character = '\0';
    string text = "";

    for(char pKey : pressedKeys){
        text = content;

        if(pKey == ALLEGRO_KEY_UP && !ignoreVerticalArrows){
            moveCursorUp(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
            continue;
        }
        else if(pKey == ALLEGRO_KEY_RIGHT){
            moveCursorToRight(shift, control, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
            continue;
        }
        else if(pKey == ALLEGRO_KEY_DOWN && !ignoreVerticalArrows){
            moveCursorDown(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
            continue;
        }
        else if(pKey == ALLEGRO_KEY_LEFT){
            moveCursorToLeft(shift, control, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
            continue;
        }

        if(deleteFromText(pKey, text, control, display, leftCursorOnFormatIdx, rightCursorOnFormatIdx,
            ENABLE_al_set_clipboard_text, internalClipboard, CopiedFormatting, EXE_PATH))
        {
            continue;
        }

        if(control){
            if(pKey == ALLEGRO_KEY_A){
                cursorPos = 0;
                secondCursorPos = text.size();
                for(FormatClass & Format : Formatting){
                    Format.selected = true;
                }
                leftCursorOnFormatIdx = 0;
                rightCursorOnFormatIdx = Formatting.size() - 1;
                continue;
            }
            else if(text.size() > 0 && pKey == ALLEGRO_KEY_C){
                string clipboard = "";
                unsigned selectionStart = std::min(cursorPos, secondCursorPos);
                unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
                clipboard = text.substr(selectionStart, selectionEnd - selectionStart + 1);
                internalClipboard = clipboard;

                if(ENABLE_al_set_clipboard_text){
                    if(display != nullptr){
                        al_set_clipboard_text(display, clipboard.c_str());
                    }
                }
                else{
                    std::ofstream File(EXE_PATH + "clipboard.txt", std::ios::trunc | std::ios::out);
                    if(File){
                        File << internalClipboard;
                    }
                    File.close();
                }

                CopiedFormatting.clear();

                if(canCopyFormat){
                    CopiedFormatting.insert(CopiedFormatting.begin(), Formatting.begin() + leftCursorOnFormatIdx,
                        Formatting.begin() + rightCursorOnFormatIdx + 1);
                }
                
                continue;
            }
            else if(pKey == ALLEGRO_KEY_V){
                if(cursorPos < protectedArea || secondCursorPos < protectedArea){
                    continue;
                }
                string newContent = text.substr(0, std::min(cursorPos, secondCursorPos));
                
                string clipboard = "";

                if(ENABLE_al_set_clipboard_text || shift){
                    if(display == nullptr){
                        continue;
                    }
                    if(!al_clipboard_has_text(display)){
                        continue;
                    }
                    if(allowNotAscii){
                        clipboard = al_get_clipboard_text(display);
                    }
                    else{
                        clipboard = removeNotAscii(al_get_clipboard_text(display));
                    }
                }
                else{
                    clipboard = internalClipboard;
                }
                
                if(clipboard.size() == 0){
                    continue;
                }


                std::vector<FormatClass> FinalFormatting;
                unsigned sumCheck = 0;
                for(const FormatClass & Format : CopiedFormatting){
                    sumCheck += Format.limit;
                }
                if(clipboard != internalClipboard || CopiedFormatting.size() == 0 || internalClipboard.size() != sumCheck){
                    FinalFormatting.push_back(Formatting[leftCursorOnFormatIdx]);
                    FinalFormatting.back().limit = clipboard.size();
                }
                else{
                    FinalFormatting.insert(FinalFormatting.begin(), CopiedFormatting.begin(), CopiedFormatting.end());
                }

                newContent += clipboard;
                if(cursorPos != secondCursorPos){ //If more than one letter is selected.
                    unsigned selectionEnd = std::max(cursorPos, secondCursorPos);
                    if(selectionEnd >= text.size()){
                        selectionEnd--;
                    }
                    newContent += text.substr(selectionEnd + 1, text.size()-selectionEnd);
                    secondCursorPos = std::min(cursorPos, secondCursorPos);
                    cursorPos = secondCursorPos + clipboard.size() - 1;
                    Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx, Formatting.begin() + rightCursorOnFormatIdx + 1);
                    if(leftCursorOnFormatIdx >= Formatting.size()){
                        Formatting.insert(Formatting.end(), FinalFormatting.begin(), FinalFormatting.end());
                        rightCursorOnFormatIdx = Formatting.size();
                        Formatting.push_back(*Formatting.end());
                        Formatting.back().limit++;
                    }
                    else{
                        Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FinalFormatting.begin(), FinalFormatting.end());
                        rightCursorOnFormatIdx += FinalFormatting.size();
                    }
                }
                else{
                    newContent += text.substr(cursorPos, text.size()-cursorPos);
                    cursorPos += clipboard.size() - 1;
                    if(rightCursorOnFormatIdx == Formatting.size() - 1){
                        Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx);
                        Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FinalFormatting.begin(), FinalFormatting.end());
                        rightCursorOnFormatIdx += FinalFormatting.size() - 1;
                        Formatting.push_back(FinalFormatting.back());
                        Formatting.back().limit = 1;
                        Formatting.back().drawingLimit = 0;
                        Formatting.back().selected = false;
                    }
                    else{
                        Formatting[leftCursorOnFormatIdx].selected = false;
                        Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FinalFormatting.begin(), FinalFormatting.end());
                        rightCursorOnFormatIdx += FinalFormatting.size() - 1;
                    }
                }
                
                content = newContent;
            }
            continue;
        }

        if(cursorPos < protectedArea || secondCursorPos < protectedArea){
            continue;
        }

        if(isNumerical && (pKey < 27 || pKey > 36)){
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
        }
        cursorPos = selectionStart + 1;
        secondCursorPos = cursorPos;

        content = newContent;

        //Remove selected formatting.
        if(leftCursorOnFormatIdx < rightCursorOnFormatIdx || Formatting[leftCursorOnFormatIdx].limit > 1){
            Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1, Formatting.begin() + rightCursorOnFormatIdx + 1);
        }

        //Don't touch it.
        Formatting[leftCursorOnFormatIdx].limit = 1;
        Formatting[leftCursorOnFormatIdx].drawingLimit = 1;

        if(leftCursorOnFormatIdx > 0){ //Use the previous text format for the new inputed character.
            Formatting[leftCursorOnFormatIdx - 1].limit++; //Add one letter to formatting.
        }
        else{ //Use the next text format for the new inputed character.
            Formatting.insert(Formatting.begin(), FormatClass());
            if(Formatting.size() > 1){
                Formatting[0] = Formatting[1];
            }
            else{
                cout << "Warning: In " << __FUNCTION__ << ": There is no formatting to copy from.\n";
            }
            Formatting[0].selected = false;
            Formatting[0].limit = 1; //Add one letter to formatting.
        }
        rightCursorOnFormatIdx = leftCursorOnFormatIdx;
    }

    if(Formatting.back().limit == 1 && Formatting.size() > 1){
        Formatting.back() = Formatting[Formatting.size() - 2];
        Formatting.back().selected = cursorPos == content.size();
        Formatting.back().limit = 1;
        Formatting.back().drawingLimit = 1;
    }
    update();
    textLines.back() += ' ';
    lineLengths.back()++;
    Formatting.back().drawingLimit++;
}
void SuperEditableTextModule::setCursorPos(int newPos){
    if(newPos < 0){
        cursorPos = 0;
    }
    else if(newPos > int(content.size())){
        cursorPos = content.size();
    }
    else{
        cursorPos = newPos;
    }
    secondCursorPos = cursorPos;
    if(isEditingActive){
        divideFormattingByCursor();
    }
}
void SuperEditableTextModule::setSecondCursorPos(int newPos){
    if(newPos < 0){
        secondCursorPos = 0;
    }
    else if(newPos > int(content.size())){
        secondCursorPos = content.size();
    }
    else{
        secondCursorPos = newPos;
    }
    if(isEditingActive){
        divideFormattingByCursor();
    }
}
float SuperEditableTextModule::getWidthOfLetterInTheText(unsigned currentCursorPos){
    unsigned formatIdx = 0;
    for(unsigned limit = 0, letterIdx = 0; letterIdx < currentCursorPos; letterIdx++){
        limit++;
        if(limit == Formatting[formatIdx].limit){
            formatIdx++;
            limit = 0;
        }
    }
    char letter = content.substr(currentCursorPos, 1)[0];
    return al_get_text_width(Formatting[formatIdx].Font->font, string(1, letter).c_str());
}
void SuperEditableTextModule::setCursorsWithMouse(vec2d basePos, const MouseClass & Mouse, const Camera2D * Camera){
    if(isScrollable){
        basePos.translate(Camera->visionShift);
    }
    
    cursorPos = 0;
    unsigned lineIdx;

    if(Formatting.size() == 0){
        cursorPos = content.size();
        localCursorPos = lineLengths.back();
        lineWidthToCursor = lineWidths.back();
        return;
    }

    vec2d finalPos(basePos + getPos());
    
    double currentY = finalPos.y;
    for(lineIdx = 0; lineIdx < lineHeights.size(); lineIdx++){
        currentY += lineHeights[lineIdx];
        if(Mouse.getPos().y <= currentY){
            break;
        }
    }

    lineWithCursorIdx = lineIdx;

    if(lineIdx == lineHeights.size()){
        cursorPos = content.size();
        localCursorPos = lineLengths.back();
        lineWidthToCursor = lineWidths.back();
        return;
    }

    if(Mouse.getPos().x - finalPos.x > lineWidths[lineIdx]){
        if(lineIdx < lineHeights.size() - 1){
            cursorPos = lineStarts[lineIdx + 1] - 1;
            localCursorPos = lineLengths[lineIdx] - 1;
            lineWidthToCursor = lineWidths[lineIdx] - getWidthOfLetterInTheText(cursorPos);
        }
        else{
            cursorPos = content.size();
            localCursorPos = lineLengths.back();
            lineWidthToCursor = lineWidths.back();
        }
        return;
    }

    //Find the format that starts the current line
    unsigned formatIdx = 0, limit = 0;
    for(; cursorPos < lineStarts[lineIdx] && cursorPos < content.size(); cursorPos++){
        limit++;
        if(limit >= Formatting[formatIdx].limit){
            formatIdx++;
            limit = 0;
        }
    }

    //Find a letter that mouse is pointing to in the current line
    float currentWidth = 0;
    unsigned currentLineLength = 0;
    unsigned currentTabLength;
    moveCursorToTheSavedWidth(currentWidth, cursorPos, currentLineLength, formatIdx, limit, Mouse.getPos().x - finalPos.x, currentTabLength);

    if(currentWidth > Mouse.getPos().x - finalPos.x){
        if(content[cursorPos] == '\t'){
            currentWidth -= currentTabLength * al_get_text_width(Formatting[formatIdx].Font->font, string(" ").c_str());
            currentLineLength -= currentTabLength - 1;
        }
        else{
            char letter = content.substr(cursorPos, 1)[0];
            currentWidth -= al_get_text_width(Formatting[formatIdx].Font->font, string(1, letter).c_str());
        }
    }

    localCursorPos = currentLineLength;
    lineWidthToCursor = currentWidth;
}