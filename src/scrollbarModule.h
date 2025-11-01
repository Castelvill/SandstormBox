#ifndef SCROLLBARMODULE_H_INCLUDED
#define SCROLLBARMODULE_H_INCLUDED

#include "imageModule.h"

class ScrollbarModule : public PrimaryModule{
    vec2d thumbPos;
    vec2d thumbSize;
    vec2d scrollingArea;
    vec2d realScrollingArea;
    vec2d dragStartingPos;
    string thumbImageID;
    string trackImageID;
    Camera2D * FocusedCamera;
public:
    mutable ImageModule * TrackImage;
    mutable ImageModule * ThumbImage;
    vector<string> cameraIDs; 
    vec2d scrollShift;
    bool canBeDrawn;
    bool mousePressed;
    double mouseWheelSpeed;
    void clear();
    ScrollbarModule(size_t & topModuleUniqueIndex);
    ScrollbarModule(PrimaryData & initData);
    ~ScrollbarModule();
    void clone(const ScrollbarModule &Original, PrimaryData & initData, bool changeOldID);

    void draw(vec2d basePos, const vector <ImageModule> & ImageContainer, Camera2D Camera) const;
    bool startDragging(vec2d basePos, const MouseClass & Mouse, Camera2D * Camera);
    void stopDragging();
    void correctThumbPosition();
    void scrollToTheBeginning();
    void scrollToTheEnd();
    void scrollByDistance(vec2d distance);
    bool dragThumb(vec2d basePos, const MouseClass & Mouse);
    vec2d countScrollShift();
    vec2d dragThumbWithMouseWheel(const MouseClass & Mouse);
    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
    VariableModule getValue(const AttributeType &attribute, const InstrDescription & CurrentInstr) const;

    void setThumbPos(vec2d newValue);
    void setThumbSize(vec2d newValue);
    void setScrollingArea(vec2d newValue);
    void setRealScrollingArea(vec2d newValue);
    void addRealScrollingArea(vec2d newValue);
    void addRealScrollingArea(double x, double y);
    void setDragStaringPos(vec2d newValue);
    void setMousePressed(bool newValue);
    void setThumbImageID(const string & newValue);
    void setTrackImageID(const string & newValue);

    vec2d getThumbPos();
    vec2d getThumbSize();
    vec2d getScrollingArea();
    vec2d getRealScrollingArea();
    vec2d getDragStartingPos();
    bool getMousePressed();
    string getThumbImageID();
    string getTrackImageID();
    void nullifyFocusedCameraPointer();
};


#endif // SCROLLBARMODULE_H_INCLUDED
