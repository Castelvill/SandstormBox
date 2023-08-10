#ifndef PRIMARYMODULE_H_INCLUDED
#define PRIMARYMODULE_H_INCLUDED
#include <iostream>
#include <string>
#include <cstdio>
#include <math.h>
#include "vectors.h"
#include "usefull.h"

using std::string;

class BasePointersStruct{
public:
    string type;
    bool * pBool;
    char * pChar;
    short * pShort;
    unsigned short * pUShort;
    int * pInt;
    unsigned int * pUInt;
    float * pFloat;
    double * pDouble;
    string * pString;
    BasePointersStruct();
    void clear();
    template<typename T>
    void tryToSet(T * LeftOperand, const T * RightOperand);
    void clone(const BasePointersStruct & RightOperand);
    void setPointer(bool*);
    void setPointer(char*);
    void setPointer(short*);
    void setPointer(unsigned short*);
    void setPointer(int*);
    void setPointer(unsigned int*);
    void setPointer(float*);
    void setPointer(double*);
    void setPointer(string*);
    bool areEqual(BasePointersStruct *OtherVariable);
};

class PrimaryModule{
    //List of inheriting classes: ImageModule, TextModule, CollisionModule, EventModule, ParticleEffectModule, MovementModule
    protected:

    string ID;
    string layerID; //This ID is needed in events' trigger detection.
    string objectID;
    vector <string> groups;
    vec2d pos, scrollShift, size, scale;

    bool isActive; //deactivated object doesn't interact with the program, you can only activate it.
    bool isScaledFromCenter;
    bool isAttachedToCamera;
    bool canBeSelected;
    bool isScrollable;

    public:

    void primaryConstructor(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void primaryConstructor(unsigned int newID, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void clone(const PrimaryModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void setID(string newID, vector<string> & listOfIDs);
    void setLayerID(string newLayerID);
    void setObjectID(string newOwnerID);
    void setAllIDs(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID);

    void addGroup(string newGroup);
    void removeGroup(string selectedGroup);
    void clearGroups();
    bool isInAGroup(string findGroup);
    vector <string> getGroups();

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
    string getID() const;
    string &getLayerIDAddr();
    string getLayerID() const;
    string & getIDAddr();
    string getObjectID();
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
    void bindPrimaryToVariable(string attribute, BasePointersStruct & BasePointer);
};

#endif // PRIMARYMODULE_H_INCLUDED
