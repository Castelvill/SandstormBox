#include "camera.h"

void Camera2D::setUpInstance(string newID, vector <string> & camerasIDs, bool newIsActive, vec2d newPos, vec2d newSize, vec2d newVisionShift, int newSamples){
    setID(newID, camerasIDs);
    isActive = newIsActive;
    isMinimized = false;
    deleted = false;
    relativePos = newPos;
    pos = newPos;
    visionShift = newVisionShift;
    size = newSize;
    minSize.set(50.0, 50.0);
    zoom = 1.0;
    zoomIncrease = 0.01;
    minZoom = 0.01;
    maxZoom = 2.0;
    speed = 1.0;
    zoomInKey = ALLEGRO_KEY_1;
    zoomOutKey = ALLEGRO_KEY_2;
    zoomResetKey = ALLEGRO_KEY_3;
    upKey = ALLEGRO_KEY_UP;
    rightKey = ALLEGRO_KEY_RIGHT;
    downKey = ALLEGRO_KEY_DOWN;
    leftKey = ALLEGRO_KEY_LEFT;
    pinnedCameraID = "";
    followedLayerID = "";
    followedObjectID = "";
    followedImageID = "";
    isPinnedToCamera = false;
    isForcefullyPinned = false;
    isFollowingObject = false;
    isUsingKeyboardToMove = true;
    isUsingKeyboardToZoom = true;
    isVisionAffectedByMouse = true;
    canZoomWithMouse = true;
    canBeModifiedByMouse = false;
    allowsDrawingBorders = false;
    canDrawOnCamera = true;
    drawOneFrame = false;
    canClearBitmap = true;
    clearBitmap = false;
    keepInsideScreen = false;
    grabbed = false;
    canBeGrabbed = true;
    canMouseResizeNow = true;
    isFocused = false;
    canMoveObjects = true;
    canEditText = true;

    grabbingAreaPos.set(0.0, 0.0);
    grabbingAreaSize.set(50.0, 50.0);

    std::fill_n(tint, 4, 1);

    samples = newSamples;
}
Camera2D::Camera2D(string newID, vector <string> & camerasIDs, bool newIsActive, vec2d newPos, vec2d newSize, vec2d newVisionShift, int newSamples){
    setUpInstance(newID, camerasIDs, newIsActive, newPos, newSize, newVisionShift, newSamples);
    al_set_new_bitmap_samples(samples);
    bitmapBuffer = al_create_bitmap(size.x, size.y);
}
Camera2D::Camera2D(string newID, vector <string> & camerasIDs, int newSamples){
    setUpInstance(newID, camerasIDs, false, vec2d(0.0, 0.0), vec2d(50.0, 50.0), vec2d(0.0, 0.0), newSamples);
    al_set_new_bitmap_samples(samples);
    bitmapBuffer = al_create_bitmap(size.x, size.y);
}
Camera2D::~Camera2D(){

}
void Camera2D::clone(const Camera2D &Original, vector<string> &camerasIDs, bool changeOldID){
    if(isStringInVector(reservedIDs, Original.ID)){
        cout << "Error: In " << __FUNCTION__ << ": Camera with a reserved ID \'" << Original.ID << "\' cannot be cloned.\n";
        return;
    }
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": Camera with a reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    clear();
    string oldID = ID;
    *this = Original;
    ID = oldID;
    if(changeOldID){
        setID(Original.getID(), camerasIDs);
    }
    al_set_new_bitmap_samples(samples);
    bitmapBuffer = al_create_bitmap(size.x, size.y);
}
void Camera2D::clear(){
    al_destroy_bitmap(bitmapBuffer);
    bitmapBuffer = nullptr;
    clearVisibleLayers();
    clearAccessibleLayers();
}
void Camera2D::setID(string newID, vector <string> & camerasIDs){
    if(isStringInVector(reservedIDs, ID)){
        cout << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(camerasIDs, ID);
    ID = findNewUniqueID(camerasIDs, newID);
    camerasIDs.push_back(ID);
}
string Camera2D::getID() const{
    return ID;
}
VariableModule Camera2D::getValue(string attribute) const{
    VariableModule NewValue;
    if(attribute == "id"){
        NewValue.setString(getID());
        return NewValue;
    }
    else if(attribute == "pos_x"){
        NewValue.setDouble(pos.x);
        return NewValue;
    }
    else if(attribute == "pos_y"){
        NewValue.setDouble(pos.y);
        return NewValue;
    }
    else if(attribute == "size_x"){
        NewValue.setDouble(size.x);
        return NewValue;
    }
    else if(attribute == "size_y"){
        NewValue.setDouble(size.y);
        return NewValue;
    }
    else if(attribute == "zoom"){
        NewValue.setDouble(zoom);
        return NewValue;
    }
    else if(attribute == "grabbed"){
        NewValue.setBool(grabbed);
        return NewValue;
    }
    else if(attribute == "is_focused"){
        NewValue.setBool(isFocused);
        return NewValue;
    }
    cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    return VariableModule();
}
void Camera2D::setIsActive(bool newValue){
    isActive = newValue;
}
void Camera2D::activate(){
    isActive = true;
}
void Camera2D::minimize(){
    isMinimized = true;
}
void Camera2D::bringBack(){
    isMinimized = false;
}
void Camera2D::deleteLater(){
    isActive = false;
    deleted = true;
}
void Camera2D::deactivate(){
    isActive = false;
}
void Camera2D::toggleIsActive(){
    isActive = !isActive;
}
bool Camera2D::getIsActive() const{
    return isActive;
}
bool Camera2D::getIsMinimized() const{
    return isMinimized;
}
bool Camera2D::getIsDeleted() const{
    return deleted;
}
bool Camera2D::getCanMoveObjects() const{
    return canMoveObjects;
}
void Camera2D::setPos(vec2d newPos){
    pos.set(newPos);
}
void Camera2D::setPos(double x, double y){
    pos.x = x;
    pos.y = y;
}
void Camera2D::setRelativePos(vec2d newPos){
    relativePos.set(newPos);
}
void Camera2D::setRelativePos(double x, double y){
    relativePos.x = x;
    relativePos.y = y;
}
void Camera2D::setVisionShift(vec2d newVisionShift){
    visionShift.set(newVisionShift);
}
void Camera2D::setSize(vec2d newSize){
    size.set(newSize);
    if(size.x < minSize.x){
        size.x = minSize.x;
    }
    if(size.y < minSize.y){
        size.y = minSize.y;
    }
    al_destroy_bitmap(bitmapBuffer);
    al_set_new_bitmap_samples(samples);
    bitmapBuffer = al_create_bitmap(size.x, size.y);
}
void Camera2D::setSize(double x, double y){
    setSize(vec2d(x, y));
}
void Camera2D::setMinSize(vec2d newSize){
    minSize = newSize;
    if(size.x < 5){
        size.x = 5;
    }
    if(size.y < 5){
        size.y = 5;
    }
}
void Camera2D::setMinSize(double x, double y){
    minSize.set(x, y);
    if(size.x < 5){
        size.x = 5;
    }
    if(size.y < 5){
        size.y = 5;
    }
}
void Camera2D::setZoom(double newZoom, double newZoomIncrease, double newMinZoom, double newMaxZoom){
    zoom = newZoom;
    zoomIncrease = newZoomIncrease;
    minZoom = newMinZoom;
    maxZoom = newMaxZoom;
}
void Camera2D::setSpeed(double newSpeed){
    speed = newSpeed;
}
void Camera2D::setKeyBinds(short newZoomInKey, short newZoomOutKey, short newZoomResetKey, short newUpKey, short newRightKey, short newDownKey, short newLeftKey){
    zoomInKey = newZoomInKey;
    zoomOutKey = newZoomOutKey;
    zoomResetKey = newZoomResetKey;
    upKey = newUpKey;
    rightKey = newRightKey;
    downKey = newDownKey;
    leftKey = newLeftKey;
}
void Camera2D::setFollowedLayerID(string ID){
    followedLayerID = ID;
    cout << "Camera is following: " << ID << "\n";
}
void Camera2D::setFollowedObjectID(string ID){
    followedObjectID = ID;
    cout << "Camera is following: " << ID << "\n";
}
void Camera2D::setFollowedImageID(string ID){
    followedImageID = ID;
}
void Camera2D::setIsFollowingObject(bool newIsFollowingObject){
    isFollowingObject = newIsFollowingObject;
}
void Camera2D::setIsUsingKeyboardToMove(bool isUsingKeyboard){
    isUsingKeyboardToMove = isUsingKeyboard;
}
void Camera2D::setIsUsingKeyboardToZoom(bool isUsingKeyboard){
    isUsingKeyboardToZoom = isUsingKeyboard;
}
void Camera2D::setCanInteractWithMouse(bool newValue){
    canBeModifiedByMouse = newValue;
}
void Camera2D::setTint(float r, float g, float b, float a){
    tint[0] = r;
    tint[1] = g;
    tint[2] = b;
    tint[3] = a;
}
void Camera2D::update(const vector<short> & pressedKeys){
    if(!isUsingKeyboardToMove && !isUsingKeyboardToZoom){
        return;
    }
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(isUsingKeyboardToMove){
            if(pressedKeys[i] == upKey)
                visionShift.y += speed;
            if(pressedKeys[i] == rightKey)
                visionShift.x -= speed;
            if(pressedKeys[i] == downKey)
                visionShift.y -= speed;
            if(pressedKeys[i] == leftKey)
                visionShift.x += speed;
        }
        if(isUsingKeyboardToZoom){
            if(pressedKeys[i] == zoomInKey)
                zoom += zoomIncrease;
            if(pressedKeys[i] == zoomOutKey)
                zoom -= zoomIncrease;
            if(pressedKeys[i] == zoomResetKey)
                zoom = 1.0;
        }
    }
    if(zoom > maxZoom)
        zoom = maxZoom;
    if(zoom < minZoom)
        zoom = minZoom;
}
bool Camera2D::isObjectVisible(vec2d objPos, vec2d objSize) const{
    objPos.translate(visionShift);
    vec2d zeroPoint((size.x/2)*(1-1/zoom), (size.y/2)*(1-1/zoom));
    vec2d maxPoint((size.x/2)*(1+1/zoom), (size.y/2)*(1+1/zoom));

    bool canOverlap = false;

    if((objPos.x >= zeroPoint.x && objPos.x <= maxPoint.x) || (objPos.x + objSize.x >= zeroPoint.x && objPos.x + objSize.x <= maxPoint.x))
        canOverlap = true;
    if((zeroPoint.x >= objPos.x && zeroPoint.x <= objPos.x + objSize.x) || (maxPoint.x >= objPos.x && maxPoint.x <= objPos.x + objSize.x))
        canOverlap = true;

    if(!canOverlap)
        return false;

    if((objPos.y >= zeroPoint.y && objPos.y <= maxPoint.y) || (objPos.y + objSize.y >= zeroPoint.y && objPos.y + objSize.y <= maxPoint.y))
        return true;
    if((zeroPoint.y >= objPos.y && zeroPoint.y <= objPos.y + objSize.y) || (maxPoint.y >= objPos.y && maxPoint.y <= objPos.y + objSize.y))
        return true;

    return false;
}
bool Camera2D::isOnScreenWithRadius(vec2d objPos, vec2d objSize){
    double radius1 = countDistance(0.0, 0.0, objSize.x/2, objSize.y/2);
    double radius2 = countDistance(0.0, 0.0, (size.x/zoom)/2, (size.y/zoom)/2);
    double radius3 = countDistance(size.x/2, size.y/2, visionShift.x+objPos.x, visionShift.y+objPos.y);
    if(radius3 <= radius1 + radius2){
        return true;
    }
    return false;
}
vec2d Camera2D::translateWithZoom(vec2d objBasePos, vec2d objPosShift){
    objPosShift.set(size / 2 - (size / 2 - objBasePos - objPosShift) * (zoom));
    objPosShift.translate(visionShift * zoom);
    return objPosShift;
}
vec2d Camera2D::translateWithZoom(vec2d objPos){
    return translateWithZoom(vec2d(0.0, 0.0), objPos);
}
void Camera2D::addVisibleLayer(string newLayer){
    addUniqueToStringVector(visibleLayersIDs, newLayer);
}
void Camera2D::addAccessibleLayer(string newLayer){
    addUniqueToStringVector(accessibleLayersIDs, newLayer);
}
void Camera2D::removeVisibleLayer(string selectedLayer){
    removeFromStringVector(visibleLayersIDs, selectedLayer);
}
void Camera2D::removeAccessibleLayer(string selectedLayer){
    removeFromStringVector(accessibleLayersIDs, selectedLayer);
}
void Camera2D::clearVisibleLayers(){
    visibleLayersIDs.clear();
}
void Camera2D::clearAccessibleLayers(){
    accessibleLayersIDs.clear();
}
bool Camera2D::isLayerVisible(string findLayer){
    return isStringInVector(visibleLayersIDs, findLayer);
}
bool Camera2D::isLayerAccessible(string findLayer){
    return isStringInVector(accessibleLayersIDs, findLayer);
}
void Camera2D::pinToCamera(string cameraID){
    pinnedCameraID = cameraID;
    isPinnedToCamera = true;
}
void Camera2D::setIsPinned(bool isPinned){
    isPinnedToCamera = isPinned;
}
void Camera2D::setIsForcefullyPinned(bool newValue){
    isForcefullyPinned = newValue;
}
void Camera2D::activatePin(){
    isPinnedToCamera = true;
}
void Camera2D::deactivatePin(){
    isPinnedToCamera = false;
}
void Camera2D::togglePin(){
    isPinnedToCamera = !isPinnedToCamera;
}
void Camera2D::setCanMoveObjects(bool newValue){
    canMoveObjects = newValue;
}
void Camera2D::setCanEditText(bool newValue){
    canEditText = newValue;
}
void Camera2D::setGrabbingAreaPos(vec2d newValue){
    grabbingAreaPos = newValue;
}
void Camera2D::setGrabbingAreaSize(vec2d newValue){
    grabbingAreaSize = newValue;
}
void Camera2D::setGrabbingAreaPos(double x, double y){
    grabbingAreaPos.set(x, y);
}
void Camera2D::setGrabbingAreaSize(double x, double y){
    grabbingAreaSize.set(x, y);
}
void Camera2D::setAntialiasingSamples(int newSamples){
    samples = newSamples;
    al_destroy_bitmap(bitmapBuffer);
    al_set_new_bitmap_samples(samples);
    bitmapBuffer = al_create_bitmap(size.x, size.y);
}