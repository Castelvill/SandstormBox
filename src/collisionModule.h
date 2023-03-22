#ifndef COLLISIONMODULE_H_INCLUDED
#define COLLISIONMODULE_H_INCLUDED
#include "usefull.h"

#include <allegro5/allegro_primitives.h>

bool areObjectsOverlaping(vec2d pos1, vec2d size1, vec2d pos2, vec2d size2);

struct DetectedCollision{
    string solidID;
    string collisionID;
    short collisionType;
    vec2d momentum;
};

class CollisionModule: public PrimaryModule{
private:
    bool isSolid;
    bool canPenetrateSolids;
    vector <string> ignoreCollisionList; //Collisions are ignored if object
public:
    vector <DetectedCollision> Detected;
    CollisionModule(int collisionModuleID);
    CollisionModule(int collisionModuleID, vec2d size);
    bool isCloseEnough(vec2d solidPos, string solidName, vec2d movingPos, vec2d movingMomentum, CollisionModule * movingColl);
    void detectOverlaping(string solidID, string collisionID, vec2d solidPos, vec2d solidSize, vec2d movPos, vec2d momentum);
    void detectCollision(string solidID, string collisionID, vec2d solidPos, vec2d solidSize, vec2d movPos, vec2d momentum);
    void clearModule();
    vec2d countMinimalDistanceBetween(vec2d sPos, vec2d sSize, vec2d mPos, vec2d mSize, double precision);
    void changeParameters(bool newIsSolid, bool newCanPenetrateSolids, vec2d newPos, vec2d newSize);
    void switchSolid();
    void switchSolidPenetration();
    void addToIgnoreCollisionList(string objectName);
    void removeFromIgnoreCollisionList(string objectName);
    void removeImaginaryCollisions();

    bool getIsSolid();
    bool getCanPenetrateSolids();
    vector <string> getIgnoreCollisionList();

    void setIsSolid(bool);
    void setCanPenetrateSolids(bool);
};


#endif // COLLISIONMODULE_H_INCLUDED
