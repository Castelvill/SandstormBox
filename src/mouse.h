#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include "camera.h"

#define BUTTONS_NUM_MAX 5

class MouseClass{
private:
    vec2d pos;
    vec2d zoomPos;
    vec2d pressedPos;
    vec2d zoomPressedPos;
    int scrollPos, lastScrollPos;
    bool pressed[BUTTONS_NUM_MAX];
    bool released[BUTTONS_NUM_MAX];
    bool buttonDown[BUTTONS_NUM_MAX];
public:
    bool didMouseMove;

    MouseClass();
    void reset();
    void resetReleased();
    void resetButtonDown();
    void setUp();
    void getPressed(bool tempArr[]);
    void getReleased(bool tempArr[]);
    vec2d getPos();
    vec2d getZoomPos();
    vec2d getPressedPos();
    vec2d getZoomPressedPos();
    void updateZoomPos(Camera2D Camera);
    void updateAxes(ALLEGRO_EVENT event);
    void updateZoomForCamera(Camera2D * Camera);
    void updateButtonsPressed(ALLEGRO_EVENT event);
    void updateButtonsReleased(ALLEGRO_EVENT event);
    bool isPressed(short button);
    bool inRectangle(vec2d rPos, vec2d rSize, bool isObjectAttachedToCamera);
    bool buttonDownInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool pressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool firstPositionInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool releasedInRectangle(vec2d rPos, vec2d rSize, short button, bool isObjectAttachedToCamera);
    bool inRadius(vec2d rPos, double radius, bool isObjectAttachedToCamera);
    bool buttonDownInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera);
    bool pressedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera);
    bool releasedInRadius(vec2d rPos, double radius, short button, bool isObjectAttachedToCamera);
    bool isMouseButtonDown();
    bool wasMousePressed();
    bool wasMouseReleased();
};


#endif // MOUSE_H_INCLUDED
