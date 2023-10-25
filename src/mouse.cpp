#include "mouse.h"

MouseClass::MouseClass(){
    scrollPos = 0;
    lastScrollPos = 0;
}
void MouseClass::reset(){
    for(int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        firstPressed[i] = false;
        pressed[i] = false;
        released[i] = false;
    }
}
void MouseClass::resetReleased(){
    for(int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        released[i] = false;
    }
}
void MouseClass::resetFirstPressed(){
    for(int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        firstPressed[i] = false;
    }
}
void MouseClass::setUp(){
    reset();
}
bool * MouseClass::getPressed(){
    return pressed;
}
bool * MouseClass::getReleased(){
    return released;
}
vec2d MouseClass::getPos(){
    return pos;
}
vec2d MouseClass::getZoomPos(){
    return zoomPos;
}
vec2d MouseClass::getPressedPos(){
    return pressedPos;
}
vec2d MouseClass::getZoomPressedPos(){
    return zoomPressedPos;
}
void MouseClass::updateZoomPos(Camera2D Camera){
    zoomPos.set(Camera.size.x/2-pos.x, Camera.size.y/2-pos.y);
    zoomPos.divide(Camera.zoom);
    zoomPos.set(Camera.size.x/2-zoomPos.x, Camera.size.y/2-zoomPos.y);
}
void MouseClass::updateAxes(ALLEGRO_EVENT event, bool fullscreen){
    didMouseMove = true;

    vec2d lastPos(pos);
    pos.set(event.mouse.x, event.mouse.y /*+ !fullscreen * 22*/);

    scrollPos = event.mouse.z;
}
void MouseClass::updateZoomForCamera(Camera2D * Camera){
    if(lastScrollPos == scrollPos){
        return;
    }
    if(lastScrollPos < scrollPos){
        Camera->zoom += Camera->zoomIncrease;
    }   
    else{
        Camera->zoom -= Camera->zoomIncrease;
    }
    if(Camera->zoom > Camera->maxZoom){
        Camera->zoom = Camera->maxZoom;
    }
    if(Camera->zoom < Camera->minZoom){
        Camera->zoom = Camera->minZoom;
    }
    lastScrollPos = scrollPos;
}
void MouseClass::updateButtonsPressed(ALLEGRO_EVENT event){
    bool updatePressedPos = true;
    if((event.mouse.button & 1) && (event.mouse.button & 2) && (event.mouse.button & 3)){
        pressed[2] = true;
        firstPressed[2] = true;
    }
    else if((event.mouse.button & 1) && (event.mouse.button & 3) && (event.mouse.button & 4)){
        pressed[3] = true;
        firstPressed[3] = true;
    }
    else if(event.mouse.button & 1){
        pressed[0] = true;
        firstPressed[0] = true;
    }
    else if(event.mouse.button & 2){
        pressed[1] = true;
        firstPressed[1] = true;
    }
    else if(event.mouse.button & 4){
        pressed[4] = true;
        firstPressed[4] = true;
    }
    else{
        updatePressedPos = false;
    }
    if(updatePressedPos){
        pressedPos.set(pos);
        zoomPressedPos.set(zoomPos);
    }
}
void MouseClass::updateButtonsReleased(ALLEGRO_EVENT event){
    if((event.mouse.button & 1) && (event.mouse.button & 2) && (event.mouse.button & 3)){
        released[2] = true;
        firstPressed[2] = false;
        pressed[2] = false;
    }
    else if((event.mouse.button & 1) && (event.mouse.button & 3) && (event.mouse.button & 4)){
        released[3] = true;
        firstPressed[3] = false;
        pressed[3] = false;
    }
    else if(event.mouse.button & 1){
        released[0] = true;
        firstPressed[0] = false;
        pressed[0] = false;
    }
    else if(event.mouse.button & 2){
        released[1] = true;
        firstPressed[1] = false;
        pressed[1] = false;
    }
    else if(event.mouse.button & 4){
        released[4] = true;
        firstPressed[4] = false;
        pressed[4] = false;
    }
}
bool MouseClass::inRectangle(vec2d rPos, vec2d rSize, bool isCameraOrAttachedToCamera){
    if(isCameraOrAttachedToCamera){
        if(pos.x >= rPos.x && pos.x <= rPos.x + rSize.x && pos.y >= rPos.y && pos.y <= rPos.y + rSize.y){
            return true;
        }
    }
    else{
        if(zoomPos.x >= rPos.x && zoomPos.x <= rPos.x + rSize.x && zoomPos.y >= rPos.y && zoomPos.y <= rPos.y + rSize.y){
            return true;
        }
    }

    return false;
}
bool MouseClass::isFirstPressed(){
    for(unsigned int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        if(firstPressed[i])
            return true;
    }
    return false;
}
bool MouseClass::isPressed(){
    for(unsigned int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        if(pressed[i])
            return true;
    }
    return false;
}
bool MouseClass::isReleased(){
    for(unsigned int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        if(released[i])
            return true;
    }
    return false;
}
bool MouseClass::doesButtonExist(short button){
    if(button < 0 || button >= MOUSE_BUTTONS_NUM_MAX){
        std::cout << "Error [Mouse]: Mouse button with number " << button << " does not exist.\n";
        return false;
    }
    return true;
}
bool MouseClass::isFirstPressed(short button){
    if(!doesButtonExist(button)){
        return false;
    }
    return firstPressed[button];
}
bool MouseClass::isPressed(short button){
    if(!doesButtonExist(button)){
        return false;
    }
    return pressed[button];
}
bool MouseClass::isReleased(short button){
    if(!doesButtonExist(button)){
        return false;
    }
    return released[button];
}
bool MouseClass::firstPressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(firstPressed[button]){
        return inRectangle(rPos, rSize, isCameraOrAttachedToCamera);
    }
    return false;
}
bool MouseClass::pressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(pressed[button]){
        return inRectangle(rPos, rSize, isCameraOrAttachedToCamera);
    }
    return false;
}
bool MouseClass::firstPositionInRectangle(vec2d rPos, vec2d rSize, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(pressed[button]){
        if(isCameraOrAttachedToCamera){
            if(pressedPos.x >= rPos.x && pressedPos.x <= rPos.x + rSize.x && pressedPos.y >= rPos.y && pressedPos.y <= rPos.y + rSize.y){
                return true;
            }
        }
        else{
            if(zoomPressedPos.x >= rPos.x && zoomPressedPos.x <= rPos.x + rSize.x && zoomPressedPos.y >= rPos.y && zoomPressedPos.y <= rPos.y + rSize.y){
                return true;
            }
        }
    }
    return false;
}
bool MouseClass::releasedInRectangle(vec2d rPos, vec2d rSize, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(released[button]){
        if(isCameraOrAttachedToCamera){
            if(pressedPos.x >= rPos.x && pressedPos.x <= rPos.x + rSize.x && pressedPos.y >= rPos.y && pressedPos.y <= rPos.y + rSize.y){
                if(pos.x >= rPos.x && pos.x <= rPos.x + rSize.x && pos.y >= rPos.y && pos.y <= rPos.y + rSize.y){
                    return true;
                }
            }
        }
        else{
            if(zoomPressedPos.x >= rPos.x && zoomPressedPos.x <= rPos.x + rSize.x && zoomPressedPos.y >= rPos.y && zoomPressedPos.y <= rPos.y + rSize.y){
                if(zoomPos.x >= rPos.x && zoomPos.x <= rPos.x + rSize.x && zoomPos.y >= rPos.y && zoomPos.y <= rPos.y + rSize.y){
                    return true;
                }
            }
        }
    }
    return false;
}
bool MouseClass::inRadius(vec2d rPos, double radius, bool isCameraOrAttachedToCamera){
    if(isCameraOrAttachedToCamera){
        if(countDistance2(pos.x, pos.y, rPos.x, rPos.y) <= radius){
            return true;
        }
    }
    else{
        if(countDistance2(zoomPos.x, zoomPos.y, rPos.x, rPos.y) <= radius){
            return true;
        }
    }

    return false;
}
bool MouseClass::firstPressedInRadius(vec2d rPos, double radius, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(firstPressed[button]){
        return inRadius(rPos, radius, isCameraOrAttachedToCamera);
    }
    return false;
}
bool MouseClass::pressedInRadius(vec2d rPos, double radius, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(pressed[button]){
        return inRadius(rPos, radius, isCameraOrAttachedToCamera);
    }
    return false;
}
bool MouseClass::releasedInRadius(vec2d rPos, double radius, short button, bool isCameraOrAttachedToCamera){
    if(!doesButtonExist(button)){
        return false;
    }
    if(released[button]){
        if(isCameraOrAttachedToCamera){
            if(countDistance2(pressedPos.x, pressedPos.y, rPos.x, rPos.y) <= radius){
                if(countDistance2(pos.x, pos.y, rPos.x, rPos.y) <= radius){
                    return true;
                }
            }
        }
        else{
            if(countDistance2(zoomPressedPos.x, zoomPressedPos.y, rPos.x, rPos.y) <= radius){
                if(countDistance2(zoomPos.x, zoomPos.y, rPos.x, rPos.y) <= radius){
                    return true;
                }
            }
        }
    }
    return false;
}
