#ifndef PRIMARYMODULE_H_INCLUDED
#define PRIMARYMODULE_H_INCLUDED
#include <iostream>
#include <string>
#include <cstdio>
#include <math.h>
#include <windows.h>
#include "vectors.h"
#include "usefull.h"

using std::string;

class PrimaryModule{
    //List of inheriting classes: ImageModule, TextModule, CollisionModule, EventModule, ParticleEffectModule, MovementModule
    protected:

    string ID;

    vector <string> groups;

    vec2d pos, scrollShift, size, scale;

    bool isActive; //deactivated object doesn't interact with the program, you can only activate it.
    bool isScaledFromCenter;
    bool isAttachedToCamera;
    bool canBeSelected;
    bool isScrollable;

    public:

    void primaryConstructor(unsigned int moduleID);
    void primaryConstructor(string moduleID);
    void setID(string newID);

    void addGroup(string newGroup);
    void removeGroup(string selectedGroup);
    void clearGroups();
    bool isInAGroup(string findGroup);

    void setPos(vec2d newPos);
    void setScrollShift(vec2d newValue);
    void setPos(double x, double y);
    void translatePos(vec2d newPos);
    void setSize(vec2d newSize);
    void setSize(double x, double y);
    void setScale(vec2d newScale);
    void setScale(double x, double y);
    void addScale(vec2d newScale);
    void resize(vec2d newSize); //calculate a new scale vector according to new dimensions
    void resize(double newSizeX, double newSizeY);
    void resizeX(double newSizeX); //calculate scale on X axis according to new length
    void resizeY(double newSizeY); //calculate scale on Y axis according to new height
    void setIsActive(bool newValue);
    void activate();
    void deactivate();
    void toggleIsActive();
    void setIsScaledFromCenter(bool newIsScaledFromCenter);
    void setIsAttachedToCamera(bool newIsAttachedToCamera);
    void setCanBeSelected(bool newValue);
    void setIsScrollable(bool newValue);
    string getID();
    string & getIDAddr();
    vec2d getPos(bool useScrollshift);
    vec2d & getPosAddr();
    vec2d getSize();
    vec2d & getSizeAddr();
    vec2d getScale();
    vec2d getScaledSize();
    bool getIsActive();
    bool getIsScaledFromCenter();
    bool getIsAttachedToCamera();
    bool getCanBeSelected();
    bool getIsScrollable();
};

#endif // PRIMARYMODULE_H_INCLUDED
