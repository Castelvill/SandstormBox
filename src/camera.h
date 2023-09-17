#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "usefull.h"
#include <allegro5/allegro.h>

class Camera2D{
public:
    string ID;
    bool isActive;
    bool deleted;
    vec2d pos; //Actual position on the screen.
    vec2d relativePos; //Position on the screen if isPinnedCamera is false, otherwise position relative to pinned camera.
    vec2d visionShift;
    vec2d size; //Size of the program's window
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
    bool isFollowingObject; //Camera's position on screen won't change by following an object.
    bool isUsingKeyboardToMove;
    bool isUsingCursorPositionToMove;

    vector <string> visibleLayersIDs;
    vector <string> accessibleLayersIDs; //User can interact with those Layers through this Camera;
    ALLEGRO_BITMAP * bitmapBuffer;

    void setUpInstance(string newID, vector <string> & camerasIDs, bool newIsActive, vec2d newPos, vec2d newSize, vec2d newVisionShift);
    Camera2D(string newID, vector <string> & camerasIDs, bool newIsActive, vec2d newPos, vec2d newSize, vec2d newVisionShift);
    Camera2D(string newID, vector <string> & camerasIDs);
    void clone(const Camera2D& Original, vector <string> & camerasIDs);
    void clear();
    void setID(string newID, vector <string> & camerasIDs);
    string getID() const;
    void setIsActive(bool newValue);
    void activate();
    void deleteLater();
    void deactivate();
    void toggleIsActive();
    bool getIsActive();
    bool getIsDeleted() const;
    void setPos(vec2d newPos);
    void setRelativePos(vec2d newPos);
    void setVisionShift(vec2d newVisionShift);
    void setSize(vec2d newSize);
    void setZoom(double newZoom, double newZoomIncrease, double newMinZoom, double newMaxZoom);
    void setSpeed(double newSpeed);
    void setKeyBinds(short newZoomInKey, short newZoomOutKey, short newZoomResetKey, short newUpKey, short newRightKey, short newDownKey, short newLeftKey);
    void setFollowedLayerID(string);
    void setFollowedObjectID(string);
    void setFollowedImageID(string);
    void setIsFollowingObject(bool newIsFollowingObject);
    void setIsUsingKeyboardToMove(bool isUsingKeyboard);
    void setIsUsingCursorPositionToMove(bool isUsingCursor);
    void update(vector <short> pressedKeys);
    bool isObjectVisible(vec2d objPos, vec2d objSize);
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
    void activatePin();
    void deactivatePin();
    void togglePin();
};


#endif // CAMERA_H_INCLUDED
