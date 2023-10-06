#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include "camera.h"

#define MOUSE_BUTTONS_NUM_MAX 5

class MouseClass{
private:
    vec2d pos;
    vec2d zoomPos;
    vec2d pressedPos;
    vec2d zoomPressedPos;
    int scrollPos, lastScrollPos;
    bool pressed[MOUSE_BUTTONS_NUM_MAX];
    bool released[MOUSE_BUTTONS_NUM_MAX];
    bool firstPressed[MOUSE_BUTTONS_NUM_MAX];
public:
    bool didMouseMove;

    MouseClass();
    void reset();
    void resetReleased();
    void resetFirstPressed();
    void setUp();
    bool * getPressed();
    bool * getReleased();
    vec2d getPos();
    vec2d getZoomPos();
    vec2d getPressedPos();
    vec2d getZoomPressedPos();
    void updateZoomPos(Camera2D Camera);
    void updateAxes(ALLEGRO_EVENT event, bool fullscreen);
    void updateZoomForCamera(Camera2D * Camera);
    void updateButtonsPressed(ALLEGRO_EVENT event);
    void updateButtonsReleased(ALLEGRO_EVENT event);
    bool isFirstPressed();
    bool isPressed();
    bool isReleased();
    bool doesButtonExist(short button);
    bool isFirstPressed(short button);
    bool isPressed(short button);
    bool isReleased(short button);
    bool inRectangle(vec2d rPos, vec2d rSize, bool isObjectAttachedToCamera);
    bool firstPressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool pressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool firstPositionInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool releasedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool inRadius(vec2d rPos, double radius, bool isObjectAttachedToCamera);
    bool firstPressedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera);
    bool pressedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera);
    bool releasedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera);
};


#endif // MOUSE_H_INCLUDED
