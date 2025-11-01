#include "scrollbarModule.h"

void ScrollbarModule::clear(){
    thumbPos.set(0.0, 0.0);
    thumbSize.set(0.0, 0.0);
    scrollingArea.set(0.0, 0.0);
    realScrollingArea.set(0.0, 0.0);
    dragStartingPos.set(0.0, 0.0);
    scrollShift.set(0.0, 0.0);
    cameraIDs.clear();
    thumbImageID = "";
    trackImageID = "";
    FocusedCamera = nullptr;
    TrackImage = nullptr;
    ThumbImage = nullptr;
    canBeDrawn = true;
    mousePressed = false;
    mouseWheelSpeed = 5.0;
}
ScrollbarModule::ScrollbarModule(size_t & topModuleUniqueIndex){
    primaryConstructor(topModuleUniqueIndex);
    clear();
}
ScrollbarModule::ScrollbarModule(PrimaryData & initData){
    primaryConstructor(initData);
    clear();
}
ScrollbarModule::~ScrollbarModule(){

}

void ScrollbarModule::clone(const ScrollbarModule &Original, PrimaryData & initData,
    bool changeOldID
){
    size_t oldIndex = getUniqueIndex();
    string oldID = ID;
    *this = Original;
    setUniqueIndex(oldIndex);
    TrackImage = nullptr;
    ThumbImage = nullptr;
    ID = oldID;

    setObjectUniqueIndex(initData.objectUniqueIndex);
    setLayerUniqueIndex(initData.layerUniqueIndex);
    
    initData.newID = Original.getID(); 
    setAllIDs(initData, changeOldID);
}

void ScrollbarModule::draw(vec2d basePos, const vector <ImageModule> & ImageContainer,
    Camera2D Camera
) const {
    vec2d newPos(basePos+pos);
    if(TrackImage != nullptr && TrackImage->getID() == trackImageID){
        TrackImage->draw(newPos, Camera, true);
    }
    else{
        for(const ImageModule & Image : ImageContainer){
            if(Image.getID() == trackImageID){
                TrackImage = const_cast<ImageModule*>(&Image);
                TrackImage->draw(newPos, Camera, true);
                break;
            }
        }
    }
    if(ThumbImage != nullptr && ThumbImage->getID() == thumbImageID){
        ThumbImage->resize(thumbSize);
        ThumbImage->draw(newPos+thumbPos, Camera, true);
    }
    else{
        for(const ImageModule & Image : ImageContainer){
            if(Image.getID() == thumbImageID){
                ThumbImage = const_cast<ImageModule*>(&Image);
                ThumbImage->resize(thumbSize);
                ThumbImage->draw(newPos+thumbPos, Camera, true);
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
    if(isScrollable){
        if(Mouse.inRectangle(realThumbPos, thumbSize, true, Camera)){
            mousePressed = true;
            dragStartingPos.set(Mouse.getZoomedPos(Camera)-realThumbPos-Camera->visionShift);
            FocusedCamera = Camera;
            return true;
        }
    }
    else{
        if(Mouse.inRectangle(realThumbPos, thumbSize, false, nullptr)){
            mousePressed = true;
            dragStartingPos.set(Mouse.getPos()-realThumbPos);
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
    if(isScrollable){
        thumbPos.set(
            Mouse.getZoomedPos(FocusedCamera) - FocusedCamera->visionShift - basePos - pos
            - dragStartingPos
        );
        correctThumbPosition();
    }
    else{
        thumbPos.set(Mouse.getPos()-basePos-pos-dragStartingPos);
        correctThumbPosition();
    }

    return true;
}
vec2d ScrollbarModule::countScrollShift(){
    scrollShift.set(0.0, 0.0);
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
vec2d ScrollbarModule::dragThumbWithMouseWheel(const MouseClass &Mouse){
    if(scrollingArea.y-thumbSize.y == 0){
        return scrollShift;
    }
    scrollShift.y -= (Mouse.scrollPos - Mouse.lastScrollPos) * mouseWheelSpeed;
    if(scrollShift.y < 0){
        scrollShift.y = 0;
    }
    if(scrollShift.y > realScrollingArea.y){
        scrollShift.y = realScrollingArea.y;
    }
    thumbPos.y = (scrollShift.y / realScrollingArea.y)*(scrollingArea.y-thumbSize.y);
    return scrollShift;
}
void ScrollbarModule::getContext(AttributeType attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.emplace_back(BasePointersStruct());
    switch(attribute){
        case thumb_pos_x:
            BasePointers.back().setPointer(&thumbPos.x);
            return;
        case thumb_pos_y:
            BasePointers.back().setPointer(&thumbPos.y);
            return;
        case thumb_size_x:
            BasePointers.back().setPointer(&thumbSize.x);
            return;
        case thumb_size_y:
            BasePointers.back().setPointer(&thumbSize.y);
            return;
        case scrolling_area_x:
            BasePointers.back().setPointer(&scrollingArea.x);
            return;
        case scrolling_area_y:
            BasePointers.back().setPointer(&scrollingArea.y);
            return;
        case real_scrolling_area_x:
            BasePointers.back().setPointer(&realScrollingArea.x);
            return;
        case real_scrolling_area_y:
            BasePointers.back().setPointer(&realScrollingArea.y);
            return;
        case drag_starting_pos_x:
            BasePointers.back().setPointer(&dragStartingPos.x);
            return;
        case drag_starting_pos_y:
            BasePointers.back().setPointer(&dragStartingPos.y);
            return;
        case AttributeType::thumb_image_id:
            BasePointers.back().setPointer(&thumbImageID);
            return;
        case AttributeType::track_image_id:
            BasePointers.back().setPointer(&trackImageID);
            return;
        default:
            BasePointers.pop_back();
            getPrimaryContext(attribute, BasePointers);
            return;
    }
}
VariableModule ScrollbarModule::getValue(const AttributeType &attribute,
    const InstrDescription & CurrentInstr
) const {
    switch(attribute){
        case scroll_shift_x:
            return VariableModule::newDouble(scrollShift.x);
        case scroll_shift_y:
            return VariableModule::newDouble(scrollShift.y);
        case pos_x:
            return VariableModule::newDouble(pos.x);
        case pos_y:
            return VariableModule::newDouble(pos.y);
        case size_x:
            return VariableModule::newDouble(size.x);
        case size_y:
            return VariableModule::newDouble(size.y);
        case thumb_pos_x:
            return VariableModule::newDouble(thumbPos.x);
        case thumb_pos_y:
            return VariableModule::newDouble(thumbPos.y);
        case thumb_size_x:
            return VariableModule::newDouble(thumbSize.x);
        case thumb_size_y:
            return VariableModule::newDouble(thumbSize.y);
        case scrolling_area_x:
            return VariableModule::newDouble(scrollingArea.x);
        case scrolling_area_y:
            return VariableModule::newDouble(scrollingArea.y);
        case real_scrolling_area_x:
            return VariableModule::newDouble(realScrollingArea.x);
        case real_scrolling_area_y:
            return VariableModule::newDouble(realScrollingArea.y);
        case drag_starting_pos_x:
            return VariableModule::newDouble(dragStartingPos.x);
        case drag_starting_pos_y:
            return VariableModule::newDouble(dragStartingPos.y);
        case can_be_drawn:
            return VariableModule::newBool(canBeDrawn);
        case mouse_pressed_a:
            return VariableModule::newBool(mousePressed);
        case mouse_wheel_speed:
            return VariableModule::newDouble(mouseWheelSpeed);
        default:
            break;
    }
    
    cerr << instructionError(CurrentInstr, __FUNCTION__) << "Attribute '" << attribute
        << "' is not valid.\n";
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
void ScrollbarModule::setThumbImageID(const string & newValue){
    thumbImageID = newValue;
}
void ScrollbarModule::setTrackImageID(const string & newValue){
    trackImageID = newValue;
}
void ScrollbarModule::nullifyFocusedCameraPointer(){
    FocusedCamera = nullptr;
}
