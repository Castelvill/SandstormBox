#include "mouse.h"

vector <short> getPressedKeys(unsigned char key[]){
    vector <short> pressedKeys;
    for(short i = 1; i < ALLEGRO_KEY_MAX; i++){
        if(key[i]){
            pressedKeys.push_back(i);
        }
    }
    return pressedKeys;
}

vector <short> getReleasedKeys(unsigned char key[], vector <short> pressedKeys){
    vector <short> releasedKeys;
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(!key[pressedKeys[i]]){
            releasedKeys.push_back(pressedKeys[i]);
        }
    }
    return releasedKeys;
}


MouseClass::MouseClass(){
    scrollPos = 0;
    lastScrollPos = 0;
}
void MouseClass::reset(){
    for(int i = 0; i < 5; i++){
        pressed[i] = false;
        released[i] = false;
    }
}
void MouseClass::resetReleased(){
    for(int i = 0; i < 5; i++){
        released[i] = false;
    }
}
void MouseClass::resetButtonDown(){
    for(int i = 0; i < 5; i++){
        buttonDown[i] = false;
    }
}
void MouseClass::setUp(){
    reset();
}
void MouseClass::getPressed(bool tempArr[]){
    for(int i = 0; i < 5; i++){
        tempArr[i] = pressed[i];
    }
}
void MouseClass::getReleased(bool tempArr[]){
    for(int i = 0; i < 5; i++){
        tempArr[i] = released[i];
    }
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
void MouseClass::updateAxes(ALLEGRO_EVENT event){
    didMouseMove = true;

    vec2d lastPos(pos);
    pos.set(event.mouse.x, event.mouse.y);

    scrollPos = event.mouse.z;
}
void MouseClass::updateZoomForCamera(Camera2D * Camera){
    if(lastScrollPos == scrollPos){
        return;
    }
    if(lastScrollPos < scrollPos)
        Camera->zoom += Camera->zoomIncrease;
    else
        Camera->zoom -= Camera->zoomIncrease;
    if(Camera->zoom > Camera->maxZoom)
        Camera->zoom = Camera->maxZoom;
    if(Camera->zoom < Camera->minZoom)
        Camera->zoom = Camera->minZoom;
    lastScrollPos = scrollPos;
}
void MouseClass::updateButtonsPressed(ALLEGRO_EVENT event){
    reset();
    bool updatePressedPos = true;
    if((event.mouse.button & 1) && (event.mouse.button & 2) && (event.mouse.button & 3)){
        pressed[2] = true;
        buttonDown[2] = true;
    }
    else if((event.mouse.button & 1) && (event.mouse.button & 3) && (event.mouse.button & 4)){
        pressed[3] = true;
        buttonDown[3] = true;
    }
    else if(event.mouse.button & 1){
        pressed[0] = true;
        buttonDown[0] = true;
    }
    else if(event.mouse.button & 2){
        pressed[1] = true;
        buttonDown[1] = true;
    }
    else if(event.mouse.button & 4){
        pressed[4] = true;
        buttonDown[4] = true;
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
    reset();
    if((event.mouse.button & 1) && (event.mouse.button & 2) && (event.mouse.button & 3)){
        released[2] = true;
    }
    else if((event.mouse.button & 1) && (event.mouse.button & 3) && (event.mouse.button & 4)){
        released[3] = true;
    }
    else if(event.mouse.button & 1){
        released[0] = true;
    }
    else if(event.mouse.button & 2){
        released[1] = true;
    }
    else if(event.mouse.button & 4){
        released[4] = true;
    }
}
bool MouseClass::inRectangle(vec2d rPos, vec2d rSize, bool isObjectAttachedToCamera){
    if(isObjectAttachedToCamera){
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
bool MouseClass::isPressed(short button){
    return pressed[button];
}
bool MouseClass::buttonDownInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera){
    if(buttonDown[button]){
        return inRectangle(rPos, rSize, isObjectAttachedToCamera);
    }
    return false;
}
bool MouseClass::pressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera){
    if(pressed[button]){
        return inRectangle(rPos, rSize, isObjectAttachedToCamera);
    }
    return false;
}
bool MouseClass::firstPositionInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera){
    if(pressed[button]){
        if(isObjectAttachedToCamera){
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
bool MouseClass::releasedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera){
    if(released[button]){
        if(isObjectAttachedToCamera){
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
bool MouseClass::inRadius(vec2d rPos, double radius, bool isObjectAttachedToCamera){
    if(isObjectAttachedToCamera){
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
bool MouseClass::buttonDownInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera){
    if(buttonDown[button]){
        return inRadius(rPos, radius, isObjectAttachedToCamera);
    }
    return false;
}
bool MouseClass::pressedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera){
    if(pressed[button]){
        return inRadius(rPos, radius, isObjectAttachedToCamera);
    }
    return false;
}
bool MouseClass::releasedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera){
    if(released[button]){
        if(isObjectAttachedToCamera){
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
bool MouseClass::isMouseButtonDown(){
    for(unsigned int i = 0; i < 5; i++){
        if(buttonDown[i])
            return true;
    }
    return false;
}
bool MouseClass::wasMousePressed(){
    for(unsigned int i = 0; i < 5; i++){
        if(pressed[i])
            return true;
    }
    return false;
}
bool MouseClass::wasMouseReleased(){
    for(unsigned int i = 0; i < 5; i++){
        if(released[i])
            return true;
    }
    return false;
}
