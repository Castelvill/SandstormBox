#include "scrollbarModule.h"

void ScrollbarModule::initScrollbar(){
    thumbPos.set(0.0, 0.0);
    thumbSize.set(0.0, 0.0);
    scrollingArea.set(0.0, 0.0);
    dragStartingPos.set(0.0, 0.0);
    thumbImageID = "";
    usedBitmapLayer = 0;
    FocusedCamera = nullptr;
}
ScrollbarModule::ScrollbarModule(unsigned int moduleID){
    initScrollbar();
    primaryConstructor(moduleID);
}
ScrollbarModule::ScrollbarModule(string moduleID){
    initScrollbar();
    primaryConstructor(moduleID);
}

void ScrollbarModule::draw(vec2d basePos ,vector <ImageModule> & ImageContainer, Camera2D Camera){
    vec2d newPos(basePos+pos);
    for(ImageModule & Image : ImageContainer){
        if(Image.getID() == trackImageID){
            Image.drawImage(newPos, Camera, true);
            break;
        }
    }
    for(ImageModule & Image : ImageContainer){
        if(Image.getID() == thumbImageID){
            Image.drawImage(newPos+thumbPos, Camera, true);
            break;
        }
    }
}
bool ScrollbarModule::startDragging(vec2d basePos, MouseClass Mouse, Camera2D * Camera){
    mousePressed = false;

    vec2d realThumbPos(basePos+pos+thumbPos);
    if(isAttachedToCamera){
        if(Mouse.inRectangle(realThumbPos, thumbSize, true)){
            mousePressed = true;
            dragStartingPos.set(Mouse.getPos()-realThumbPos);
            FocusedCamera = Camera;
            return true;
        }
    }
    else{
        if(Mouse.inRectangle(realThumbPos+Camera->visionShift, thumbSize, false)){
            mousePressed = true;
            dragStartingPos.set(Mouse.getZoomPos()-realThumbPos-Camera->visionShift);
            FocusedCamera = Camera;
            return true;
        }
    }

    return false;
}
bool ScrollbarModule::dragThumb(vec2d basePos, MouseClass Mouse){
    if(FocusedCamera == nullptr){
        mousePressed = false;
        return false;
    }
    Mouse.updateZoomPos(*FocusedCamera);
    if(!mousePressed || !Mouse.pressedInRectangle(FocusedCamera->pos, FocusedCamera->size, 0, false)){
        mousePressed = false;
        return false;
    }
    if(isAttachedToCamera){
        thumbPos.set(Mouse.getPos()-basePos-pos-dragStartingPos);
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
    else{
        thumbPos.set(Mouse.getZoomPos()-FocusedCamera->visionShift-basePos-pos-dragStartingPos);
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

    return true;
}
vec2d ScrollbarModule::countScrollShift(){
    vec2d scrollShift(0.0, 0.0);
    if(scrollingArea.x-thumbSize.x != 0){
        scrollShift.x = (thumbPos.x/(scrollingArea.x-thumbSize.x))*realScrollingArea.x;
    }
    if(scrollingArea.y-thumbSize.y != 0){
        scrollShift.y = (thumbPos.y/(scrollingArea.y-thumbSize.y))*realScrollingArea.y;
    }
    std::cout << scrollShift.x << " " << scrollShift.y << "\n";
    return scrollShift;
}

void ScrollbarModule::setThumbPos(vec2d newValue){
    thumbPos.set(newValue);
}
void ScrollbarModule::setThumbSize(vec2d newValue){
    thumbSize.set(newValue);
}
void ScrollbarModule::setScrollingArea(vec2d newValue){
    scrollingArea.set(newValue);
}
void ScrollbarModule::setRealScrollingArea(vec2d newValue){
    realScrollingArea = newValue;
}
void ScrollbarModule::addRealScrollingArea(vec2d newValue){
    realScrollingArea.translate(newValue);
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
void ScrollbarModule::setUsedBitmapLayer(int newValue){
    usedBitmapLayer = newValue;
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
int ScrollbarModule::getUsedBitmapLayer(){
    return usedBitmapLayer;
}
