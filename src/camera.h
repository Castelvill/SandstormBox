#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "variableModule.h"
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>

class Camera2D{
public:
    string ID;
    bool isActive;
    bool isMinimized;
    bool deleted;
    vec2d pos; //Actual position on the screen.
    vec2d relativePos; //Position on the screen if isPinnedCamera is false, otherwise position relative to pinned camera.
    vec2d visionShift;
    vec2d size, minSize; //Size of the program's window
    double zoom;
    double zoomIncrease;
    double minZoom;
    double maxZoom;
    double speed;
    short zoomInKey, zoomOutKey, zoomResetKey, upKey, rightKey, downKey, leftKey;
    string pinnedCameraID;
    string followedLayerID;
    string followedObjectID;
    string followedImageID;
    bool isPinnedToCamera; //Camera's position on screen will change relatively to pinnedCameraID. 
    bool isForcefullyPinned; //If true, camera will be always on top of the chosen camera.
    bool isFollowingObject; //Camera's position on screen won't change by following an object.
    bool isUsingKeyboardToMove;
    bool isUsingKeyboardToZoom;
    bool canMoveWithMouse;
    bool canZoomWithMouse;
    bool canInteractWithMouse;
    bool allowsDrawingBorders;
    bool canDrawOnCamera;
    bool canClearBitmap;
    bool drawOneFrame;
    bool clearBitmap;
    bool keepInsideScreen;
    bool grabbed;
    bool canBeGrabbed;
    bool canMouseResizeNow;
    bool isFocused;

    vec2d grabbingAreaPos, grabbingAreaSize;

    float tint[4];
    int samples;

    vector <string> visibleLayersIDs;
    vector <string> accessibleLayersIDs; //User can interact with those Layers through this Camera;
    ALLEGRO_BITMAP * bitmapBuffer;

    void setUpInstance(string newID, vector <string> & camerasIDs, bool newIsActive, vec2d newPos, vec2d newSize, vec2d newVisionShift, int newSamples);
    Camera2D(string newID, vector <string> & camerasIDs, bool newIsActive, vec2d newPos, vec2d newSize, vec2d newVisionShift, int newSamples);
    Camera2D(string newID, vector <string> & camerasIDs, int newSamples);
    ~Camera2D();
    void clone(const Camera2D& Original, vector <string> & camerasIDs, bool changeOldID);
    void clear();
    void setID(string newID, vector <string> & camerasIDs);
    string getID() const;
    VariableModule getValue(string attribute) const;
    void setIsActive(bool newValue);
    void activate();
    void minimize();
    void bringBack();
    void deleteLater();
    void deactivate();
    void toggleIsActive();
    bool getIsActive() const;
    bool getIsMinimized() const;
    bool getIsDeleted() const;
    void setPos(vec2d newPos);
    void setPos(double x, double y);
    void setRelativePos(vec2d newPos);
    void setRelativePos(double x, double y);
    void setVisionShift(vec2d newVisionShift);
    void setSize(vec2d newSize);
    void setSize(double x, double y);
    void setMinSize(vec2d newSize);
    void setMinSize(double x, double y);
    void setZoom(double newZoom, double newZoomIncrease, double newMinZoom, double newMaxZoom);
    void setSpeed(double newSpeed);
    void setKeyBinds(short newZoomInKey, short newZoomOutKey, short newZoomResetKey, short newUpKey, short newRightKey, short newDownKey, short newLeftKey);
    void setFollowedLayerID(string);
    void setFollowedObjectID(string);
    void setFollowedImageID(string);
    void setIsFollowingObject(bool newIsFollowingObject);
    void setIsUsingKeyboardToMove(bool isUsingKeyboard);
    void setIsUsingKeyboardToZoom(bool isUsingKeyboard);
    void setCanInteractWithMouse(bool newValue);
    void setTint(float r, float g, float b, float a);
    void update(const vector<short> & pressedKeys);
    bool isObjectVisible(vec2d objPos, vec2d objSize) const;
    bool isOnScreenWithRadius(vec2d objPos, vec2d objSize);
    vec2d translateWithZoom(vec2d objBasePos, vec2d objPosShift);
    vec2d translateWithZoom(vec2d objPos);
    void addVisibleLayer(string newLayer);
    void addAccessibleLayer(std::string newLayer);
    void removeVisibleLayer(string selectedLayer);
    void removeAccessibleLayer(std::string selectedLayer);
    void clearVisibleLayers();
    void clearAccessibleLayers();
    bool isLayerVisible(string findLayer);
    bool isLayerAccessible(string findLayer);
    void pinToCamera(string cameraID);
    void setIsPinned(bool isPinned);
    void setIsForcefullyPinned(bool newValue);
    void activatePin();
    void deactivatePin();
    void togglePin();
    void setGrabbingAreaPos(vec2d newValue);
    void setGrabbingAreaSize(vec2d newValue);
    void setGrabbingAreaPos(double x, double y);
    void setGrabbingAreaSize(double x, double y);
    void setAntialiasingSamples(int newSamples);
};


#endif // CAMERA_H_INCLUDED
