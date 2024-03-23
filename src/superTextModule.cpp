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
    firstLineWithSelection = 0;
    lastLineWithSelection = 0;
    drawSelectionFirst = false;
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
    lineLengths.clear();
    Formatting.clear();
}

void SuperTextModule::update(){
    textLines.clear();
    textLines.push_back("");
    lineWidths.clear();
    lineWidths.push_back(0);
    lineLengths.clear();
    lineLengths.push_back(0);
    realTextSize.set(0.0, 0.0);
    firstLineWithSelection = 0;
    lastLineWithSelection = 0;
    bool isFirstSelectionLineFound = false;

    if(Formatting.size() == 0){
        cout << "Warning: In " << __FUNCTION__ << ": In SuperText '" << ID << "': Text does not have any formatting.\n";
        return;
    }
    
    char letter;
    size_t letterIdx = 0, formatLimit = 0, lineLength = 0;
    float letterWidth = 0.0, wordWidth = 0.0, wordWidthBehind = 0.0, lineWidth = 0.0;
    
    //Merge the same formatting.
    for(unsigned formatIdx = 0; formatIdx < Formatting.size() - 1;){
        if(Formatting[formatIdx].isTheSame(Formatting[formatIdx + 1])){
            Formatting[formatIdx].limit += Formatting[formatIdx + 1].limit;
            Formatting.erase(Formatting.begin() + formatIdx + 1);
        }
        else{
            formatIdx++;
        }
    }

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
    if(Format->selected){
        isFirstSelectionLineFound = true;
    }

    bool ignoreLine = false; //Works when wrapped=='c'. Ignores the part of the text in the line that is out of the scope of text field.

    for(; letterIdx < content.size(); letterIdx++){
        letter = content.substr(letterIdx, 1)[0];
        if(letter == '\n'){
            Format->drawingLimit++;
            lineWidth += al_get_text_width(Format->Font->font, string(" ").c_str());
            if(wrapped == 'c' && lineWidth > size.x){
                ignoreLine = true;
            }
            if(!ignoreLine){
                lineLength++;
            }

            if(Format->selected){
                if(!isFirstSelectionLineFound){
                    firstLineWithSelection = textLines.size() - 1;
                    isFirstSelectionLineFound = true;
                }
                lastLineWithSelection = textLines.size() - 1;
            }

            textLines.back() += ' ';
            textLines.push_back("");
            
            realTextSize.x = std::max(realTextSize.x, lineWidth);
            lineWidths.back() = lineWidth;
            lineWidths.push_back(0);
            lineLengths.back() = lineLength;
            lineLengths.push_back(0);
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
            
            if(wrapped == 'c' && realTextSize.y + Format->Font->height + paddingBetweenLines > size.y){
                break;
            }
        }
        else if(letter == '\t'){
            unsigned currentTabLength = tabLength - textLines.back().size() % tabLength;
            letterWidth = al_get_text_width(Format->Font->font, string(" ").c_str()); //It's the only way, if you don't want undefined behaviour. 
            for(unsigned i = 0; i < currentTabLength; i++){
                if(lineWidth + letterWidth > size.x){
                    if(wrapped == 'c'){
                        ignoreLine = true;
                    }
                    else if(wrapped == 'l' || wrapped == 'w'){
                        if(i > 0){
                            break;
                        }
                        currentTabLength = tabLength;
                        textLines.push_back("");
                        realTextSize.x = std::max(realTextSize.x, lineWidth);
                        lineWidths.back() = lineWidth;
                        lineWidths.push_back(0);
                        lineLengths.back() = lineLength;
                        lineLengths.push_back(0);
                        lineWidth = 0;
                        lineLength = 0;
                        realTextSize.y += Format->Font->height;
                        if(textLines.size() > 1){
                            realTextSize.y += paddingBetweenLines;
                        }
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
                if(Format->selected){
                    if(!isFirstSelectionLineFound){
                        firstLineWithSelection = textLines.size() - 1;
                        isFirstSelectionLineFound = true;
                    }
                    lastLineWithSelection = textLines.size() - 1;
                }
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
        }
        else{
            Format->drawingLimit++;
            letterWidth = al_get_text_width(Format->Font->font, string(1, letter).c_str()); //It's the only way, if you don't want undefined behaviour. 
            wordWidth = 0.0;
            wordWidthBehind = 0.0;
            if(wrapped == 'w' && letter != ' '){
                char wordLetter;
                if(letterIdx > 0){
                    for(unsigned i = letterIdx - 1; i >= 0; i--){
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
                for(unsigned i = letterIdx + 1; i < content.size(); i++){
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
                    lineLengths.back() = lineLength;
                    lineLengths.push_back(0);
                    lineWidth = 0;
                    lineLength = 0;
                    realTextSize.y += Format->Font->height;
                    if(textLines.size() > 1){
                        realTextSize.y += paddingBetweenLines;
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
                if(Format->selected){
                    if(!isFirstSelectionLineFound){
                        firstLineWithSelection = textLines.size() - 1;
                        isFirstSelectionLineFound = true;
                    }
                    lastLineWithSelection = textLines.size() - 1;
                }
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
        }
    }
    
    if(Format != Formatting.end()){
        if(Format->selected){
            if(!isFirstSelectionLineFound){
                firstLineWithSelection = textLines.size() - 1;
                isFirstSelectionLineFound = true;
            }
            lastLineWithSelection = textLines.size() - 1;
        }
        realTextSize.y += Format->Font->height; //add the last line
    }
    cout << "Selected lines: " << firstLineWithSelection << " to " << lastLineWithSelection << "\n";
    if(textLines.size() > 1){
        realTextSize.y += paddingBetweenLines;
    }
    realTextSize.x = std::max(realTextSize.x, lineWidth);
    lineWidths.back() = lineWidth;
    lineLengths.back() = lineLength;
    for(float & width : lineWidths){
        width = std::min(width, float(size.x));
    }
}

void SuperTextModule::cropSizeToText(){
    size.set(realTextSize.x, realTextSize.y);
}

VariableModule SuperTextModule::getAttributeValue(const string &attribute, const string &detail) const{
    if(attribute == "in_group"){
        return VariableModule::newBool(isInAGroup(detail));
    }
    else if(attribute == "content"){
        return VariableModule::newString(content);
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

void SuperTextModule::drawFormattedString(string text, vec2d finalPos, size_t lineIdx, vector<FormatClass>::iterator Format, bool drawSelection){
    if(Format->selected){
        if(drawSelection){
            string background;
            for(size_t i = 0; i < text.size(); i++){
                background += "█";
            }
            al_draw_text(Format->Font->font, Format->color, finalPos.x + Format->offset.x,
            finalPos.y + Format->offset.y + lineIdx * Format->Font->height + paddingBetweenLines * (lineIdx > 0), 0, background.c_str());
        }
        if(!drawSelectionFirst || !drawSelection){
            al_draw_text(Format->Font->font, Format->accentColor, finalPos.x + Format->offset.x,
                finalPos.y + Format->offset.y + lineIdx * Format->Font->height + paddingBetweenLines * (lineIdx > 0), 0, text.c_str());
        }
    }
    else{
        al_draw_text(Format->Font->font, Format->color, finalPos.x + Format->offset.x,
            finalPos.y + Format->offset.y + lineIdx * Format->Font->height + paddingBetweenLines * (lineIdx > 0), 0, text.c_str());
    }
}
void SuperTextModule::drawAllLines(vec2d finalPos, bool drawSelection){
    size_t letterIdx = 0;
    size_t lineIdx = 0;
    size_t formatLimit = 0;
    size_t longestFragment = 0;
    string currentFragment;
    vec2d linePos(finalPos);
    float previousLength;
    vector<FormatClass>::iterator Format = Formatting.begin();
    for(lineIdx = 0; lineIdx < textLines.size(); lineIdx++){
        previousLength = 0.0;
        linePos.set(finalPos);
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
            drawFormattedString(currentFragment, linePos, lineIdx, Format, drawSelection);

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
void SuperTextModule::draw(vec2d base, bool drawBorders, Camera2D Camera, unsigned cursorPos, unsigned secondCursorPos, bool editingIsActive){
    vec2d newScale(scale);
    vec2d unformatedPos(base + pos);
    if(!isAttachedToCamera){
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


    drawAllLines(finalPos, true);
    if(drawSelectionFirst){
        drawAllLines(finalPos, false);
    }
    
    al_identity_transform(&trans);
    al_use_transform(&trans);

    if(drawBorders){
        al_draw_rectangle(unformatedPos.x, unformatedPos.y, unformatedPos.x+size.x, unformatedPos.y+size.y, Formatting.back().color, 1);
    }
}

void SuperTextModule::setContent(string newContent){
    content = newContent;
}
void SuperTextModule::addContent(string newContent){
    content += newContent;
}
void SuperTextModule::addNewTextLine(string newLine){
    textLines.push_back(newLine);
}
void SuperTextModule::setTextLine(size_t index, string newLine){
    if(index > textLines.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of textLines vector.\n";
        return;
    }
    textLines[index] = newLine;
}
void SuperTextModule::addToTextLine(size_t index, string newLine){
    if(index > textLines.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of textLines vector.\n";
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
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].color = newColor;
    Formatting[index].accentColor = newAccentColor;
    Formatting[index].Font = findFontByID(FontContainer, fontID);
    Formatting[index].offset.set(offsetX, offsetY);
    Formatting[index].selected = isSelected;
    Formatting[index].limit = newLimit;
}
void SuperTextModule::deleteFormat(size_t index){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting.erase(Formatting.begin() + index);
}
void SuperTextModule::setColor(size_t index, ALLEGRO_COLOR newColor){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].color = newColor;
}
void SuperTextModule::setAccentColor(size_t index, ALLEGRO_COLOR newAccentColor){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].accentColor = newAccentColor;
}
void SuperTextModule::setFont(size_t index, string newFontID, vector <SingleFont> & FontContainer){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].Font = findFontByID(FontContainer, newFontID);
}
void SuperTextModule::setOffset(size_t index, float offsetX, float offsetY){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.set(offsetX, offsetY);
}
void SuperTextModule::addOffset(size_t index, float offsetX, float offsetY){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.translate(offsetX, offsetY);
}
void SuperTextModule::setOffsetX(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.x = newOffset;
}
void SuperTextModule::addOffsetX(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.x += newOffset;
}
void SuperTextModule::setOffsetY(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.y = newOffset;
}
void SuperTextModule::addOffsetY(size_t index, float newOffset){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].offset.y += newOffset;
}
void SuperTextModule::setSelected(size_t index, bool isSelected){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
        return;
    }
    Formatting[index].selected = isSelected;
}
void SuperTextModule::setLimit(size_t index, size_t newLimit){
    if(index > Formatting.size()){
        std::cout << "Error: In " << __FUNCTION__ << ": In SuperTextModule '" << ID << "': Index is out of scope of Formatting vector.\n";
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
    minContentLength = 0;
    maxContentLength = 512;
    inputDelay = 0.3;
    repetitionDelay = 0.05;
    protectedArea = 0; //User cannot edit the text placed before the protection cursor.
    isEditingActive = false;
    blockedKeys.clear();
    lastInputedKey = -1;
    currentInputDelay = 0.0;
    cursorPos = 0;
    secondCursorPos = 0;
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
    SuperTextModule::clear();
    previousContent.clear();
    futureContent.clear();
    previousFormatting.clear();
    futureFormatting.clear();
    blockedKeys.clear();
}
VariableModule SuperEditableTextModule::getAttributeValue(const string &attribute, const string &detail) const{
    if(attribute == "original_content"){
        if(previousContent.size() > 0){
            return VariableModule::newString(*previousContent.begin());
        }
        return VariableModule::newString(content);
    }
    else if(attribute == "can_be_edited"){
        return VariableModule::newBool(canBeEdited);
    }
    else if(attribute == "can_use_space"){
        return VariableModule::newBool(canUseSpace);
    }
    else if(attribute == "can_use_enter"){
        return VariableModule::newBool(canUseEnter);
    }
    else if(attribute == "enter_ends_editing"){
        return VariableModule::newBool(enterEndsEditing);
    }
    else if(attribute == "can_use_tabs"){
        return VariableModule::newBool(canUseTabs);
    }
    else if(attribute == "is_numerical"){
        return VariableModule::newBool(isNumerical);
    }
    else if(attribute == "has_floating_point"){
        return VariableModule::newBool(hasFloatingPoint);
    }
    else if(attribute == "ignore_vertical_arrows"){
        return VariableModule::newBool(ignoreVerticalArrows);
    }
    else if(attribute == "ignore_content_restriction"){
        return VariableModule::newBool(ignoreContentRestriction);
    }
    else if(attribute == "is_storing_history"){
        return VariableModule::newBool(isStoringHistory);
    }
    else if(attribute == "min_content_length"){
        return VariableModule::newInt(minContentLength);
    }
    else if(attribute == "max_content_length"){
        return VariableModule::newInt(maxContentLength);
    }
    else if(attribute == "input_delay"){
        return VariableModule::newDouble(inputDelay);
    }
    else if(attribute == "repetition_delay"){
        return VariableModule::newDouble(repetitionDelay);
    }
    else if(attribute == "protected_area"){
        return VariableModule::newInt(protectedArea);
    }
    else if(attribute == "is_editing_active"){
        return VariableModule::newBool(isEditingActive);
    }
    else if(attribute == "cursor_pos"){
        return VariableModule::newInt(cursorPos);
    }
    else if(attribute == "second_cursor_pos"){
        return VariableModule::newInt(secondCursorPos);
    }
    else if(attribute == "min_cursor_pos"){
        return VariableModule::newInt(std::min(cursorPos, secondCursorPos));
    }
    else if(attribute == "max_cursor_pos"){
        return VariableModule::newInt(std::max(cursorPos, secondCursorPos));
    }
    else{
        return SuperTextModule::getAttributeValue(attribute, detail);
    }
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
bool SuperEditableTextModule::deleteFromText(char pKey, string text, bool & control,
    ALLEGRO_DISPLAY * window, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx,
    bool ENABLE_al_set_clipboard_text, string & internalClipboard, vector<FormatClass> & CopiedFormatting
){
    if(pKey != ALLEGRO_KEY_BACKSPACE && pKey != ALLEGRO_KEY_DELETE && (pKey != ALLEGRO_KEY_X || !control)){
        return false;
    }
    if(cursorPos < protectedArea || secondCursorPos < protectedArea){
        return true;
    }
    if(pKey == ALLEGRO_KEY_BACKSPACE || (pKey == ALLEGRO_KEY_X && control && cursorPos != secondCursorPos)){
        if(text.size() <= minContentLength){
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
                internalClipboard = clipboard;

                if(ENABLE_al_set_clipboard_text){
                    al_set_clipboard_text(window, clipboard.c_str());
                }
                else{
                    std::ofstream File("clipboard.txt", std::ios::trunc | std::ios::out);
                    if(File){
                        File << internalClipboard;
                    }
                    File.close();
                }

                CopiedFormatting.clear();
                CopiedFormatting.insert(CopiedFormatting.begin(), Formatting.begin() + leftCursorOnFormatIdx,
                    Formatting.begin() + rightCursorOnFormatIdx + 1);
                for(FormatClass & Format : CopiedFormatting){
                    Format.selected = false;
                }
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
            if(cursorPos == 0){
                return true;
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
        return true;
    }
    else if(pKey == ALLEGRO_KEY_DELETE){
        if(text.size() <= minContentLength){
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
                return true;
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

            /*cout << "DEL: ";
            for(unsigned i = 0; i < Formatting.size(); i++){
                cout << i << ":" << Formatting[i].limit << "; ";
            }
            cout << "\n";
            cout.flush();*/
        }
        content = newContent;
        secondCursorPos = cursorPos;
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
void SuperEditableTextModule::divideFormattingByCursor(){
    unsigned cursorBegin = std::min(cursorPos, secondCursorPos);
    unsigned cursorEnd = std::max(cursorPos, secondCursorPos) + 1;
    unsigned letterIdx = 0;
    unsigned formatIdx = 0;

    /*cout << "STA: ";
    for(unsigned i = 0; i < Formatting.size(); i++){
        cout << i << ":" << Formatting[i].limit << "; ";
    }
    cout << "\n";
    cout.flush();*/

    for(formatIdx = 0; formatIdx < Formatting.size(); formatIdx++){
        Formatting[formatIdx].selected = false;
    }

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
                    Formatting[formatIdx].limit = cursorBegin - letterIdx;
                }
                else{ //Format with both cursors.
                    Formatting.insert(Formatting.begin()+formatIdx+1, FormatClass());
                    Formatting[formatIdx + 2] = Formatting[formatIdx];
                    Formatting[formatIdx + 2].limit = letterIdx + Formatting[formatIdx].limit - cursorEnd;
                    Formatting[formatIdx + 1].limit = cursorEnd - cursorBegin;
                    Formatting[formatIdx].limit = cursorBegin - letterIdx;
                }
            }
            else{
                Formatting.insert(Formatting.begin(), FormatClass());
                Formatting[0] = Formatting[1];
                Formatting[0].limit = cursorEnd;
                Formatting[0].selected = true;
                Formatting[1].limit -= cursorEnd;
            }
        }
        else if(cursorEnd > letterIdx && cursorEnd < letterIdx + Formatting[formatIdx].limit){ //Format with only right cursor inside.
            Formatting.insert(Formatting.begin()+formatIdx+1, FormatClass());
            Formatting[formatIdx + 1] = Formatting[formatIdx];
            Formatting[formatIdx + 1].limit = letterIdx + Formatting[formatIdx].limit - cursorEnd;
            Formatting[formatIdx].limit = cursorEnd - letterIdx;
            Formatting[formatIdx].selected = true;
        }

        letterIdx += Formatting[formatIdx].limit;
    }

    update();

    /*cout << "DIV: ";
    for(unsigned i = 0; i < Formatting.size(); i++){
        cout << i << ":" << Formatting[i].limit << "; ";
    }
    cout << "\n";
    cout.flush();*/
}
void SuperEditableTextModule::moveCursorDown(const string & text, bool shift){
    //Find the line where the cursor is located.
    //unsigned currentLine = 0;

    /*
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
    }*/
}
void SuperEditableTextModule::moveCursorToLeft(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
    if(!shift){
        cursorPos = std::min(cursorPos, secondCursorPos);
        if(cursorPos > 0){
            cursorPos--;
        }
        cursorPos = std::max(cursorPos, protectedArea);
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
void SuperEditableTextModule::moveCursorToRight(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx){
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
                Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FormatClass());
                Formatting[leftCursorOnFormatIdx] = Formatting[leftCursorOnFormatIdx + 1];
                Formatting[leftCursorOnFormatIdx].limit = 1;
                Formatting[leftCursorOnFormatIdx].selected = false;
            }
            else{
                Formatting[leftCursorOnFormatIdx].selected = false;
                leftCursorOnFormatIdx++;
            }
        }

        if(cursorPos < content.size()){
            cursorPos++;
        }
    }
}
void SuperEditableTextModule::edit(vector <short> releasedKeys, vector <short> pressedKeys, ALLEGRO_DISPLAY * window,
    bool ENABLE_al_set_clipboard_text, string & internalClipboard, vector<FormatClass> & CopiedFormatting
){
    if(!getIsActive() || !isEditingActive){
        return;
    }
    
    /*cout << "INI: ";
    for(unsigned i = 0; i < Formatting.size(); i++){
        cout << i << ":" << Formatting[i].limit << "; ";
    }
    cout << "\n";
    cout.flush();*/
    
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

        if(!ignoreVerticalArrows && pKey == ALLEGRO_KEY_UP){
            if(cursorPos == 0){
                continue;
            }
            //moveCursorUp(text, shift);
            continue;
        }
        else if(pKey == ALLEGRO_KEY_RIGHT){
            moveCursorToRight(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
            
            /*if(cursorPos == text.size() || secondCursorPos == text.size()){
                continue;
            }
            //Format right letter.
            if(rightCursorOnFormatIdx < Formatting.size() - 1 && Formatting[rightCursorOnFormatIdx + 1].limit != 1){
                Formatting[rightCursorOnFormatIdx + 1].limit--;
                if(Formatting[rightCursorOnFormatIdx].isTheSame(Formatting[rightCursorOnFormatIdx + 1])){
                    Formatting[rightCursorOnFormatIdx].limit++;
                }
                else{
                    rightCursorOnFormatIdx++;
                    Formatting.insert(Formatting.begin() + rightCursorOnFormatIdx, FormatClass());
                    Formatting[rightCursorOnFormatIdx] = Formatting[rightCursorOnFormatIdx + 1];
                    Formatting[rightCursorOnFormatIdx].limit = 1;
                }
            }
            else if(Formatting[rightCursorOnFormatIdx + 1].limit == 1){
                rightCursorOnFormatIdx++;
            }
            
            if(shift){
                cursorPos++;
            }
            else{
                cursorPos = std::max(cursorPos, secondCursorPos) + 1;
                secondCursorPos = cursorPos;
                for(FormatClass & Format : Formatting){
                    Format.selected = false;
                }
                leftCursorOnFormatIdx = rightCursorOnFormatIdx;
            }
            Formatting[rightCursorOnFormatIdx].selected = true;*/
            
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
            moveCursorToLeft(shift, leftCursorOnFormatIdx, rightCursorOnFormatIdx);
            continue;
        }

        if(deleteFromText(pKey, text, control, window, leftCursorOnFormatIdx, rightCursorOnFormatIdx,
            ENABLE_al_set_clipboard_text, internalClipboard, CopiedFormatting))
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
                    al_set_clipboard_text(window, clipboard.c_str());
                }
                else{
                    std::ofstream File("clipboard.txt", std::ios::trunc | std::ios::out);
                    if(File){
                        File << internalClipboard;
                    }
                    File.close();
                }

                CopiedFormatting.clear();
                CopiedFormatting.insert(CopiedFormatting.begin(), Formatting.begin() + leftCursorOnFormatIdx,
                    Formatting.begin() + rightCursorOnFormatIdx + 1);
                for(FormatClass & Format : CopiedFormatting){
                    Format.selected = false;
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
                    if(!al_clipboard_has_text(window)){
                        continue;
                    }
                    clipboard = al_get_clipboard_text(window);
                }
                else{
                    clipboard = internalClipboard;
                }
                
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
                //cursorPos = std::min(cursorPos, secondCursorPos);
                //secondCursorPos = cursorPos + clipboard.size() - 1;
                cursorPos += clipboard.size();
                secondCursorPos = cursorPos;
                content = newContent;

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

                if(leftCursorOnFormatIdx < rightCursorOnFormatIdx){ //If more than one letter is selected.
                    Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx, Formatting.begin() + rightCursorOnFormatIdx + 1);
                    if(leftCursorOnFormatIdx >= Formatting.size()){
                        Formatting.insert(Formatting.end(), FinalFormatting.begin(), FinalFormatting.end());
                        leftCursorOnFormatIdx = Formatting.size();
                        Formatting.push_back(*Formatting.end());
                        Formatting.back().limit++;
                    }
                    else{
                        Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FinalFormatting.begin(), FinalFormatting.end());
                        leftCursorOnFormatIdx += FinalFormatting.size();
                    }
                }
                else{
                    Formatting.insert(Formatting.begin() + leftCursorOnFormatIdx, FinalFormatting.begin(), FinalFormatting.end());
                    leftCursorOnFormatIdx += FinalFormatting.size();
                }
                rightCursorOnFormatIdx = leftCursorOnFormatIdx;
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
            selectionStart++;
        }
        cursorPos = selectionStart;
        secondCursorPos = cursorPos;

        content = newContent;

        //Remove selected formatting.
        if(leftCursorOnFormatIdx < rightCursorOnFormatIdx){
            Formatting.erase(Formatting.begin() + leftCursorOnFormatIdx + 1, Formatting.begin() + rightCursorOnFormatIdx + 1);
            Formatting[leftCursorOnFormatIdx].limit = 1; //Add one letter to formatting.
        }
        else{
            if(leftCursorOnFormatIdx > 0){
                Formatting[leftCursorOnFormatIdx - 1].limit++; //Add one letter to formatting.
            }
            else{
                Formatting.insert(Formatting.begin(), FormatClass());
                Formatting[0] = Formatting[1];
                Formatting[0].selected = false;
                Formatting[0].limit = 1; //Add one letter to formatting.
            }
        }
        rightCursorOnFormatIdx = leftCursorOnFormatIdx;
    }
    
    /*cout << "PRE: ";
    for(unsigned i = 0; i < Formatting.size(); i++){
        cout << i << ":" << Formatting[i].limit << "; ";
    }
    cout << "\n";
    cout.flush();*/

    update();
    textLines.back() += ' ';
    Formatting.back().drawingLimit++;
    lineLengths.back()++;
    
    /*cout << "END: ";
    for(unsigned i = 0; i < Formatting.size(); i++){
        cout << i << ":" << Formatting[i].limit << "; ";
    }
    cout << "\n";
    cout.flush();*/
}
