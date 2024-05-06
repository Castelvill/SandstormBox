#include "mouse.h"

MouseClass::MouseClass(){
    pos.set(0.0, 0.0);
    pressedPos.set(0.0, 0.0);
    didMouseMove = false;
    scrollPos = 0;
    lastScrollPos = 0;
    reset();
}
MouseClass::~MouseClass(){
    reset();
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
void MouseClass::getPressed(bool pressedMouse[5]) const{
    memcpy(pressedMouse, pressed, 5 * sizeof(bool));
}
void MouseClass::getReleased(bool releasedMouse[5]) const{
    memcpy(releasedMouse, released, 5 * sizeof(bool));
}
void MouseClass::translateAllPos(vec2d translation){
    pos.translate(translation);
    pressedPos.translate(translation);
}
void MouseClass::setPos(vec2d newPos){
    pos = newPos;
}
void MouseClass::setPressedPos(vec2d newPos){
    pressedPos = newPos;
}
vec2d MouseClass::getPos() const
{
    return pos;
}
vec2d MouseClass::getPressedPos() const{
    return pressedPos;
}
vec2d MouseClass::getZoomedPos(const Camera2D * Camera) const{
    if(Camera == nullptr){
        return pos;
    }
    vec2d zoomedPos;
    zoomedPos.set(Camera->size.x/2-pos.x, Camera->size.y/2-pos.y);
    zoomedPos.divide(Camera->zoom);
    zoomedPos.set(Camera->size.x/2-zoomedPos.x, Camera->size.y/2-zoomedPos.y);
    return zoomedPos;
}
vec2d MouseClass::getZoomedPressedPos(const Camera2D * Camera) const{
    if(Camera == nullptr){
        return pos;
    }
    vec2d zoomedPressedPos;
    zoomedPressedPos.set(Camera->size.x/2-pressedPos.x, Camera->size.y/2-pressedPos.y);
    zoomedPressedPos.divide(Camera->zoom);
    zoomedPressedPos.set(Camera->size.x/2-zoomedPressedPos.x, Camera->size.y/2-zoomedPressedPos.y);
    return zoomedPressedPos;
}
void MouseClass::updateAxes(ALLEGRO_EVENT event, bool fullscreen){
    didMouseMove = true;

    pos.set(event.mouse.x, event.mouse.y);

    lastScrollPos = scrollPos;
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
    //lastScrollPos = scrollPos;
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
bool MouseClass::inRectangle(vec2d rPos, vec2d rSize, bool isPartOfInterface, const Camera2D * Camera) const{
    if(isPartOfInterface){
        if(pos.x >= rPos.x && pos.x <= rPos.x + rSize.x && pos.y >= rPos.y && pos.y <= rPos.y + rSize.y){
            return true;
        }
    }
    else{
        vec2d zoomedPos(getZoomedPos(Camera));
        if(zoomedPos.x >= rPos.x && zoomedPos.x <= rPos.x + rSize.x && zoomedPos.y >= rPos.y && zoomedPos.y <= rPos.y + rSize.y){
            return true;
        }
    }

    return false;
}
bool MouseClass::isFirstPressed() const{
    for(unsigned int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        if(firstPressed[i])
            return true;
    }
    return false;
}
bool MouseClass::isPressed() const{
    for(unsigned int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        if(pressed[i])
            return true;
    }
    return false;
}
bool MouseClass::isReleased() const{
    for(unsigned int i = 0; i < MOUSE_BUTTONS_NUM_MAX; i++){
        if(released[i])
            return true;
    }
    return false;
}
bool MouseClass::doesButtonExist(short button)  const{
    if(button < 0 || button >= MOUSE_BUTTONS_NUM_MAX){
        cout << "Error [Mouse]: Mouse button with number " << button << " does not exist.\n";
        return false;
    }
    return true;
}
bool MouseClass::isFirstPressed(short button) const{
    if(!doesButtonExist(button)){
        return false;
    }
    return firstPressed[button];
}
bool MouseClass::isPressed(short button) const{
    if(!doesButtonExist(button)){
        return false;
    }
    return pressed[button];
}
bool MouseClass::isReleased(short button) const{
    if(!doesButtonExist(button)){
        return false;
    }
    return released[button];
}
bool MouseClass::firstPressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(firstPressed[button]){
        return inRectangle(rPos, rSize, isPartOfInterface, Camera);
    }
    return false;
}
bool MouseClass::pressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(pressed[button]){
        return inRectangle(rPos, rSize, isPartOfInterface, Camera);
    }
    return false;
}
bool MouseClass::firstPositionInRectangle(vec2d rPos, vec2d rSize, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(pressed[button]){
        if(isPartOfInterface){
            if(pressedPos.x >= rPos.x && pressedPos.x <= rPos.x + rSize.x && pressedPos.y >= rPos.y && pressedPos.y <= rPos.y + rSize.y){
                return true;
            }
        }
        else{
            vec2d zoomedPressedPos(getZoomedPressedPos(Camera));
            if(zoomedPressedPos.x >= rPos.x && zoomedPressedPos.x <= rPos.x + rSize.x
                && zoomedPressedPos.y >= rPos.y && zoomedPressedPos.y <= rPos.y + rSize.y)
            {
                return true;
            }
        }
    }
    return false;
}
bool MouseClass::releasedInRectangle(vec2d rPos, vec2d rSize, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(released[button]){
        if(isPartOfInterface){
            if(pressedPos.x >= rPos.x && pressedPos.x <= rPos.x + rSize.x && pressedPos.y >= rPos.y && pressedPos.y <= rPos.y + rSize.y){
                if(pos.x >= rPos.x && pos.x <= rPos.x + rSize.x && pos.y >= rPos.y && pos.y <= rPos.y + rSize.y){
                    return true;
                }
            }
        }
        else{
            vec2d zoomedPressedPos(getZoomedPressedPos(Camera));
            if(zoomedPressedPos.x >= rPos.x && zoomedPressedPos.x <= rPos.x + rSize.x
                && zoomedPressedPos.y >= rPos.y && zoomedPressedPos.y <= rPos.y + rSize.y)
            {
                vec2d zoomedPos(getZoomedPos(Camera));
                if(zoomedPos.x >= rPos.x && zoomedPos.x <= rPos.x + rSize.x && zoomedPos.y >= rPos.y && zoomedPos.y <= rPos.y + rSize.y){
                    return true;
                }
            }
        }
    }
    return false;
}
bool MouseClass::inRadius(vec2d rPos, double radius, bool isPartOfInterface, const Camera2D * Camera) const{
    if(isPartOfInterface){
        if(countDistance(pos.x, pos.y, rPos.x, rPos.y) <= radius){
            return true;
        }
    }
    else{
        vec2d zoomedPos(getZoomedPos(Camera));
        if(countDistance(zoomedPos.x, zoomedPos.y, rPos.x, rPos.y) <= radius){
            return true;
        }
    }

    return false;
}
bool MouseClass::firstPressedInRadius(vec2d rPos, double radius, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(firstPressed[button]){
        return inRadius(rPos, radius, isPartOfInterface, Camera);
    }
    return false;
}
bool MouseClass::pressedInRadius(vec2d rPos, double radius, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(pressed[button]){
        return inRadius(rPos, radius, isPartOfInterface, Camera);
    }
    return false;
}
bool MouseClass::releasedInRadius(vec2d rPos, double radius, short button, bool isPartOfInterface, const Camera2D * Camera) const{
    if(!doesButtonExist(button)){
        return false;
    }
    if(released[button]){
        if(isPartOfInterface){
            if(countDistance(pressedPos.x, pressedPos.y, rPos.x, rPos.y) <= radius){
                if(countDistance(pos.x, pos.y, rPos.x, rPos.y) <= radius){
                    return true;
                }
            }
        }
        else{
            vec2d zoomedPressedPos(getZoomedPressedPos(Camera));
            if(countDistance(zoomedPressedPos.x, zoomedPressedPos.y, rPos.x, rPos.y) <= radius){
                vec2d zoomedPos(getZoomedPos(Camera));
                if(countDistance(zoomedPos.x, zoomedPos.y, rPos.x, rPos.y) <= radius){
                    return true;
                }
            }
        }
    }
    return false;
}
