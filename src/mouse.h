#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include "primitivesModule.h"

#define MOUSE_BUTTONS_NUM_MAX 5

class MouseClass{
private:
    vec2d pos;
    vec2d pressedPos;
    int scrollPos, lastScrollPos;
    bool pressed[MOUSE_BUTTONS_NUM_MAX];
    bool released[MOUSE_BUTTONS_NUM_MAX];
    bool firstPressed[MOUSE_BUTTONS_NUM_MAX];
public:
    bool didMouseMove;

    MouseClass();
    ~MouseClass();
    void reset();
    void resetReleased();
    void resetFirstPressed();
    void setUp();
    void getPressed(bool pressedMouse[5]) const;
    void getReleased(bool releasedMouse[5]) const;
    vec2d getPos() const;
    vec2d getPressedPos() const;
    vec2d getZoomedPos(const Camera2D * Camera) const;
    vec2d getZoomedPressedPos(const Camera2D * Camera) const;
    void updateAxes(ALLEGRO_EVENT event, bool fullscreen);
    void updateZoomForCamera(Camera2D * Camera);
    void updateButtonsPressed(ALLEGRO_EVENT event);
    void updateButtonsReleased(ALLEGRO_EVENT event);
    bool isFirstPressed() const;
    bool isPressed() const;
    bool isReleased() const;
    bool doesButtonExist(short button) const;
    bool isFirstPressed(short button) const;
    bool isPressed(short button) const;
    bool isReleased(short button) const;
    bool inRectangle(vec2d rPos, vec2d rSize, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool firstPressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool pressedInRectangle(vec2d rPos, vec2d rSize, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool firstPositionInRectangle(vec2d rPos, vec2d rSize, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool releasedInRectangle(vec2d rPos, vec2d rSize, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool inRadius(vec2d rPos, double radius, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool firstPressedInRadius(vec2d rPos, double radius, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool pressedInRadius(vec2d rPos, double radius, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
    bool releasedInRadius(vec2d rPos, double radius, short button, bool isAttachedToCamera, const Camera2D * Camera) const;
};


#endif // MOUSE_H_INCLUDED
