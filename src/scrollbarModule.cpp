#include "scrollbarModule.h"

void ScrollbarModule::clear(){
    thumbPos.set(0.0, 0.0);
    thumbSize.set(0.0, 0.0);
    scrollingArea.set(0.0, 0.0);
    realScrollingArea.set(0.0, 0.0);
    dragStartingPos.set(0.0, 0.0);
    thumbImageID = "";
    trackImageID = "";
    FocusedCamera = nullptr;
    TrackImage = nullptr;
    ThumbImage = nullptr;
    canBeDrawn = true;
    mousePressed = false;
    mouseWheelSpeed = 5.0;
}
ScrollbarModule::ScrollbarModule(){
    clear();
}
ScrollbarModule::ScrollbarModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    clear();
}
ScrollbarModule::ScrollbarModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    clear();
}
ScrollbarModule::~ScrollbarModule(){

}

void ScrollbarModule::clone(const ScrollbarModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    TrackImage = nullptr;
    ThumbImage = nullptr;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}

void ScrollbarModule::draw(vec2d basePos, vector <ImageModule> & ImageContainer, Camera2D Camera){
    vec2d newPos(basePos+pos);
    if(TrackImage != nullptr && TrackImage->getID() == trackImageID){
        TrackImage->drawImage(newPos, Camera, true);
    }
    else{
        for(ImageModule & Image : ImageContainer){
            if(Image.getID() == trackImageID){
                TrackImage = &Image;
                TrackImage->drawImage(newPos, Camera, true);
                break;
            }
        }
    }
    if(ThumbImage != nullptr && ThumbImage->getID() == thumbImageID){
        ThumbImage->resize(thumbSize);
        ThumbImage->drawImage(newPos+thumbPos, Camera, true);
    }
    else{
        for(ImageModule & Image : ImageContainer){
            if(Image.getID() == thumbImageID){
                ThumbImage = &Image;
                ThumbImage->resize(thumbSize);
                ThumbImage->drawImage(newPos+thumbPos, Camera, true);
                break;
            }
        }
    }
}
bool ScrollbarModule::startDragging(vec2d basePos, const MouseClass & Mouse, Camera2D * Camera){
    if(!canBeDrawn){
        return false;
    }
    if(mousePressed){
        return true;
    }

    vec2d realThumbPos(basePos+pos+thumbPos);
    if(isPartOfInterface){
        if(Mouse.inRectangle(realThumbPos, thumbSize, true, Camera)){
            mousePressed = true;
            dragStartingPos.set(Mouse.getPos()-realThumbPos);
            FocusedCamera = Camera;
            return true;
        }
    }
    else{
        if(Mouse.inRectangle(realThumbPos+Camera->visionShift, thumbSize, false, nullptr)){
            mousePressed = true;
            dragStartingPos.set(Mouse.getZoomedPos(Camera)-realThumbPos-Camera->visionShift);
            FocusedCamera = Camera;
            return true;
        }
    }

    return false;
}
void ScrollbarModule::stopDragging(){
    mousePressed = false;
    FocusedCamera = nullptr;
}
void ScrollbarModule::correctThumbPosition(){
    if(thumbPos.x < 0){
        thumbPos.x = 0;
    }
    if(thumbPos.y < 0){
        thumbPos.y = 0;
    }
    if(thumbPos.x+thumbSize.x > scrollingArea.x){
        thumbPos.x = scrollingArea.x-thumbSize.x;
    }
    if(thumbPos.y+thumbSize.y > scrollingArea.y){
        thumbPos.y = scrollingArea.y-thumbSize.y;
    }
}
void ScrollbarModule::scrollToTheBeginning(){
    thumbPos.x = 0;
    thumbPos.y = 0;
}
void ScrollbarModule::scrollToTheEnd(){
    thumbPos.x = scrollingArea.x-thumbSize.x;
    thumbPos.y = scrollingArea.y-thumbSize.y;
}
void ScrollbarModule::scrollByDistance(vec2d distance){
    if(distance.x != 0 && realScrollingArea.x != 0){
        thumbPos.x += (distance.x / realScrollingArea.x) * (scrollingArea.x-thumbSize.x);
    }
    if(distance.y != 0 && realScrollingArea.y != 0){
        thumbPos.y += (distance.y / realScrollingArea.y) * (scrollingArea.y-thumbSize.y);
    }
    correctThumbPosition();
}
bool ScrollbarModule::dragThumb(vec2d basePos, const MouseClass &Mouse){
    if(FocusedCamera == nullptr || !mousePressed){
        return false;
    }
    if(isPartOfInterface){
        thumbPos.set(Mouse.getPos()-basePos-pos-dragStartingPos);
        correctThumbPosition();
    }
    else{
        thumbPos.set(Mouse.getZoomedPos(FocusedCamera)-FocusedCamera->visionShift-basePos-pos-dragStartingPos);
        correctThumbPosition();
    }

    return true;
}
vec2d ScrollbarModule::countScrollShift(){
    vec2d scrollShift(0.0, 0.0);
    if(thumbSize.x == 0 || thumbSize.y == 0){
        return scrollShift;
    }
    if(scrollingArea.x-thumbSize.x != 0){
        scrollShift.x = (thumbPos.x/(scrollingArea.x-thumbSize.x))*realScrollingArea.x;
    }
    if(scrollingArea.y-thumbSize.y != 0){
        scrollShift.y = (thumbPos.y/(scrollingArea.y-thumbSize.y))*realScrollingArea.y;
    }
    return scrollShift;
}
void ScrollbarModule::dragThumbWithMouseWheel(vec2d & objectScrollShift, const MouseClass &Mouse){
    if(scrollingArea.y-thumbSize.y == 0){
        return;
    }
    objectScrollShift.y -= (Mouse.scrollPos - Mouse.lastScrollPos) * mouseWheelSpeed;
    if(objectScrollShift.y < 0){
        objectScrollShift.y = 0;
    }
    if(objectScrollShift.y > realScrollingArea.y){
        objectScrollShift.y = realScrollingArea.y;
    }
    thumbPos.y = (objectScrollShift.y / realScrollingArea.y)*(scrollingArea.y-thumbSize.y);
}
void ScrollbarModule::getContext(string attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "thumbPos_x"){
        BasePointers.back().setPointer(&thumbPos.x);
    }
    else if(attribute == "thumbPos_y"){
        BasePointers.back().setPointer(&thumbPos.y);
    }
    else if(attribute == "thumbSize_x"){
        BasePointers.back().setPointer(&thumbSize.x);
    }
    else if(attribute == "thumbSize_y"){
        BasePointers.back().setPointer(&thumbSize.y);
    }
    else if(attribute == "scrollingArea_x"){
        BasePointers.back().setPointer(&scrollingArea.x);
    }
    else if(attribute == "scrollingArea_y"){
        BasePointers.back().setPointer(&scrollingArea.y);
    }
    else if(attribute == "realScrollingArea_x"){
        BasePointers.back().setPointer(&realScrollingArea.x);
    }
    else if(attribute == "realScrollingArea_y"){
        BasePointers.back().setPointer(&realScrollingArea.y);
    }
    else if(attribute == "dragStartingPos_x"){
        BasePointers.back().setPointer(&dragStartingPos.x);
    }
    else if(attribute == "dragStartingPos_y"){
        BasePointers.back().setPointer(&dragStartingPos.y);
    }
    else if(attribute == "thumbImageID"){
        BasePointers.back().setPointer(&thumbImageID);
    }
    else if(attribute == "trackImageID"){
        BasePointers.back().setPointer(&trackImageID);
    }
    else{
        BasePointers.pop_back();
        getPrimaryContext(attribute, BasePointers);
    }
}
VariableModule ScrollbarModule::getValue(const string &attribute, EventDescription EventIds) const{
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
    if(attribute == "thumb_pos_x"){
        return VariableModule::newDouble(thumbPos.x);
    }
    if(attribute == "thumb_pos_y"){
        return VariableModule::newDouble(thumbPos.y);
    }
    if(attribute == "thumb_size_x"){
        return VariableModule::newDouble(thumbSize.x);
    }
    if(attribute == "thumb_size_y"){
        return VariableModule::newDouble(thumbSize.y);
    }
    if(attribute == "scrolling_area_x"){
        return VariableModule::newDouble(scrollingArea.x);
    }
    if(attribute == "scrolling_area_y"){
        return VariableModule::newDouble(scrollingArea.y);
    }
    if(attribute == "real_scrolling_area_x"){
        return VariableModule::newDouble(realScrollingArea.x);
    }
    if(attribute == "real_scrolling_area_y"){
        return VariableModule::newDouble(realScrollingArea.y);
    }
    if(attribute == "drag_starting_pos_x"){
        return VariableModule::newDouble(dragStartingPos.x);
    }
    if(attribute == "drag_starting_pos_y"){
        return VariableModule::newDouble(dragStartingPos.y);
    }
    if(attribute == "can_be_drawn"){
        return VariableModule::newBool(canBeDrawn);
    }
    if(attribute == "mouse_pressed"){
        return VariableModule::newBool(mousePressed);
    }
    if(attribute == "mouse_wheel_speed"){
        return VariableModule::newDouble(mouseWheelSpeed);
    }
    
    cout << "Error: In " << EventIds.describe() << ": In " << __FUNCTION__
        << ": Attribute '" << attribute << "' is not valid.\n";
    return VariableModule::newBool(false);
}
vec2d ScrollbarModule::getThumbPos(){
    return thumbPos;
}
vec2d ScrollbarModule::getThumbSize(){
    return thumbSize;
}
vec2d ScrollbarModule::getScrollingArea(){
    return scrollingArea;
}
vec2d ScrollbarModule::getRealScrollingArea(){
    return realScrollingArea;
}
vec2d ScrollbarModule::getDragStartingPos(){
    return dragStartingPos;
}
bool ScrollbarModule::getMousePressed(){
    return mousePressed;
}
string ScrollbarModule::getThumbImageID(){
    return thumbImageID;
}
string ScrollbarModule::getTrackImageID(){
    return trackImageID;
}


void ScrollbarModule::setThumbPos(vec2d newValue){
    thumbPos.set(newValue);
    correctThumbPosition();
}
void ScrollbarModule::setThumbSize(vec2d newValue){
    thumbSize.set(newValue);
    if(thumbSize.x > scrollingArea.x){
        thumbSize.x = scrollingArea.x;
    }
    if(thumbSize.y > scrollingArea.y){
        thumbSize.y = scrollingArea.y;
    }
    correctThumbPosition();
}
void ScrollbarModule::setScrollingArea(vec2d newValue){
    scrollingArea.set(newValue);
}
void ScrollbarModule::setRealScrollingArea(vec2d newValue){
    realScrollingArea = newValue;
    realScrollingArea.x = std::max(realScrollingArea.x, 0.0);
    realScrollingArea.y = std::max(realScrollingArea.y, 0.0);

}
void ScrollbarModule::addRealScrollingArea(vec2d newValue){
    realScrollingArea.translate(newValue);
    realScrollingArea.x = std::max(realScrollingArea.x, 0.0);
    realScrollingArea.y = std::max(realScrollingArea.y, 0.0);
}
void ScrollbarModule::addRealScrollingArea(double x, double y){
    realScrollingArea.translate(x, y);
    realScrollingArea.x = std::max(realScrollingArea.x, 0.0);
    realScrollingArea.y = std::max(realScrollingArea.y, 0.0);
}
void ScrollbarModule::setDragStaringPos(vec2d newValue){
    dragStartingPos = newValue;
}
void ScrollbarModule::setMousePressed(bool newValue){
    mousePressed = newValue;
}
void ScrollbarModule::setThumbImageID(string newValue){
    thumbImageID = newValue;
}
void ScrollbarModule::setTrackImageID(string newValue){
    trackImageID = newValue;
}
void ScrollbarModule::nullifyFocusedCameraPointer(){
    FocusedCamera = nullptr;
}
