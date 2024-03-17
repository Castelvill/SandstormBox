#include "superTextModule.h"

void SuperTextModule::setUpNewInstance(){
    content = "";
    tabLength = 4;
    wrapped = 'n';
    horizontalAlign = 'l';
    verticalAlign = 'i';
    paddingBetweenLines = 0.0;
    rotation = 0.0;
    scale.set(1.0, 1.0);
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

    if(Formatting.size() == 0){
        cout << "Warning: In " << __FUNCTION__ << ": In SuperText '" << ID << "': Text does not have any formatting.\n";
        return;
    }
    
    char letter;
    size_t letterIdx = 0, formatLimit = 0, lineLength = 0;
    float lineWidth = 0, newHeight = 0;
    vector<FormatClass>::iterator Format = Formatting.begin();
    bool ignoreLine = false; //Works when wrapped='c'. Ignores the part of the text in the line that is out of the scope of text field.

    for(; letterIdx < content.size(); letterIdx++){
        letter = content.substr(letterIdx, 1)[0];
        if(letter == '\n'){
            lineWidth += al_get_text_width(Format->Font->font, string(" ").c_str());
            if(wrapped == 'c' && lineWidth > size.x){
                ignoreLine = true;
            }
            if(!ignoreLine){
                lineLength++;
            }
            textLines.back() += ' ';
            textLines.push_back("");
            lineWidths.back() = lineWidth;
            lineWidths.push_back(0);
            lineLengths.back() = lineLength;
            lineLengths.push_back(0);
            lineWidth = 0;
            lineLength = 0;
            newHeight += Format->Font->height;
            ignoreLine = false;
            formatLimit++;
            if(formatLimit == Format->limit){
                Format++;
                formatLimit = 0;
            }
            if(Format == Formatting.end()){
                break;
            }
            if(textLines.size() > 1){
                newHeight += paddingBetweenLines;
            }
            if(wrapped == 'c' && newHeight + Format->Font->height + paddingBetweenLines > size.y){
                break;
            }
        }
        else if(letter == '\t'){
            unsigned currentTabLength = tabLength - textLines.back().size() % tabLength;
            for(unsigned i = 0; i < currentTabLength; i++){
                lineWidth += al_get_text_width(Format->Font->font, string(" ").c_str()); //It's the only way, if you don't want undefined behaviour. 
                if(wrapped == 'c' && lineWidth > size.x){
                    ignoreLine = true;
                }
                if(!ignoreLine){
                    lineLength++;
                }
                textLines.back() += ' ';
                formatLimit++;
                if(formatLimit == Format->limit){
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
        else{
            lineWidth += al_get_text_width(Format->Font->font, string(1, letter).c_str()); //It's the only way, if you don't want undefined behaviour. 
            if(wrapped == 'c' && lineWidth > size.x){
                ignoreLine = true;
            }
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
        }
    }
    lineWidths.back() = lineWidth;
    lineLengths.back() = lineLength;
    for(float & width : lineWidths){
        width = std::min(width, float(size.x));
    }
    for(unsigned i = 0; i < textLines.size(); i++){
        cout << textLines[i].size() << " " << lineLengths[i] << "\n";
    }   
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

void SuperTextModule::drawFormattedString(string text, vec2d finalPos, size_t lineIdx, vector<FormatClass>::iterator Format){
    if(Format->selected){
        string background;
        for(size_t i = 0; i < text.size(); i++){
            background += "█";
        }
        al_draw_text(Format->Font->font, Format->accentColor, finalPos.x + Format->offset.x,
            finalPos.y + Format->offset.y + lineIdx * Format->Font->height + paddingBetweenLines, 0, background.c_str());
        al_draw_text(Format->Font->font, Format->color, finalPos.x + Format->offset.x,
            finalPos.y + Format->offset.y + lineIdx * Format->Font->height + paddingBetweenLines, 0, text.c_str());
    }
    else{
        al_draw_text(Format->Font->font, Format->color, finalPos.x + Format->offset.x,
            finalPos.y + Format->offset.y + lineIdx * Format->Font->height + paddingBetweenLines, 0, text.c_str());
    }
}
void SuperTextModule::draw(vec2d base, bool drawBorders, Camera2D Camera, unsigned cursorPos, unsigned secondCursorPos, bool editingIsActive){
    vec2d newScale(scale);
    vec2d finalPos(base + pos);
    if(!isAttachedToCamera){
        vec2d diff(Camera.size.x/2-base.x-pos.x, Camera.size.y/2-base.y-pos.y);
        finalPos.set(Camera.size.x/2-diff.x*(Camera.zoom), Camera.size.y/2-diff.y*(Camera.zoom));
        finalPos.translate(Camera.visionShift.x*Camera.zoom, Camera.visionShift.y*Camera.zoom);
        newScale.multiply(Camera.zoom);
        if(isScaledFromCenter){
            finalPos.translate(-((scale.x-1)*size.x*Camera.zoom)/2, -((scale.y-1)*size.y*Camera.zoom)/2);
        }
    }

    if(verticalAlign == 'c'){
        finalPos.y += size.y / 2 - realTextHeight / 2;
    }
    else if(verticalAlign == 'r'){
        finalPos.y += size.y - realTextHeight;
    }

    if(Formatting.size() == 0){
        cout << "Warning: In " << __FUNCTION__ << ": In SuperText '" << ID << "': Text does not have any formatting.\n";
        if(drawBorders){
            al_draw_rectangle(finalPos.x, finalPos.y, finalPos.x+size.x, finalPos.y+size.y, Formatting.back().color, 1);
        }
        return;
    }

    ALLEGRO_TRANSFORM trans;
    al_identity_transform(&trans);
    al_translate_transform(&trans, -finalPos.x, -finalPos.y);
    al_rotate_transform(&trans, (rotation*M_PI)/180.0);
    al_scale_transform(&trans, newScale.x, newScale.y);
    al_translate_transform(&trans, finalPos.x, finalPos.y);
    al_use_transform(&trans);

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
        }
        else if(horizontalAlign == 'r'){
            linePos.x += size.x - lineWidths[lineIdx];
        }
        for(letterIdx = 0; letterIdx < lineLengths[lineIdx];){
            longestFragment = Format->limit - formatLimit;
            longestFragment = std::min(longestFragment, lineLengths[lineIdx] - letterIdx);
            currentFragment = textLines[lineIdx].substr(letterIdx, longestFragment);
            linePos.x += previousLength;
            letterIdx += longestFragment;
            formatLimit += longestFragment;
            cout << longestFragment << " ";
            if(Format->Font != nullptr){
                previousLength = al_get_text_width(Format->Font->font, currentFragment.c_str());
                drawFormattedString(currentFragment, linePos, lineIdx, Format);
            }
            if(formatLimit >= Format->limit){
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
            longestFragment = Format->limit - formatLimit;
            cout << "(pre)" << longestFragment << " ";
            longestFragment = std::min(longestFragment, textLines[lineIdx].size() - letterIdx);
            letterIdx += longestFragment;
            formatLimit += longestFragment;
            cout << "(hide)" << longestFragment << " ";
            if(formatLimit >= Format->limit){
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
    cout << "\n";
    
    if(drawBorders){
        al_draw_rectangle(finalPos.x, finalPos.y, finalPos.x+size.x, finalPos.y+size.y, Formatting.back().color, 1);
    }

    al_identity_transform(&trans);
    al_use_transform(&trans);
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
