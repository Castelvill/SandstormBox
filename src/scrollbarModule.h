#ifndef SCROLLBARMODULE_H_INCLUDED
#define SCROLLBARMODULE_H_INCLUDED

#include "imageModule.h"
#include "mouse.h"

class ScrollbarModule : public PrimaryModule{
    vec2d thumbPos;
    vec2d thumbSize;
    vec2d scrollingArea;
    vec2d realScrollingArea;
    vec2d dragStartingPos;
    bool mousePressed;
    string thumbImageID;
    string trackImageID;
    int usedBitmapLayer;
    Camera2D * FocusedCamera;
public:
    void initScrollbar();
    ScrollbarModule();
    ScrollbarModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ScrollbarModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void clone(const ScrollbarModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID);

    void draw(vec2d basePos, vector <ImageModule> & ImageContainer, Camera2D Camera);
    bool startDragging(vec2d basePos, MouseClass Mouse, Camera2D * Camera);
    bool dragThumb(vec2d basePos, MouseClass Mouse);
    vec2d countScrollShift();
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);

    void setThumbPos(vec2d newValue);
    void setThumbSize(vec2d newValue);
    void setScrollingArea(vec2d newValue);
    void setRealScrollingArea(vec2d newValue);
    void addRealScrollingArea(vec2d newValue);
    void setDragStaringPos(vec2d newValue);
    void setMousePressed(bool newValue);
    void setThumbImageID(string newValue);
    void setTrackImageID(string newValue);
    void setUsedBitmapLayer(int newValue);

    vec2d getThumbPos();
    vec2d getThumbSize();
    vec2d getScrollingArea();
    vec2d getRealScrollingArea();
    vec2d getDragStartingPos();
    bool getMousePressed();
    string getThumbImageID();
    string getTrackImageID();
    int getUsedBitmapLayer();
};


#endif // SCROLLBARMODULE_H_INCLUDED
