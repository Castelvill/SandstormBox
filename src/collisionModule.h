#ifndef COLLISIONMODULE_H_INCLUDED
#define COLLISIONMODULE_H_INCLUDED
#include "usefull.h"

#include <allegro5/allegro_primitives.h>

bool areObjectsOverlaping(vec2d pos1, vec2d size1, vec2d pos2, vec2d size2);

struct DetectedCollision{
    string solidID;
    string collisionID;
    short collisionType; //0 - invalid collision, 1 - collision on X axis, 2 - collision on Y axis, 3 - collision with corners, 4 - overlaping "collision"
    vec2d momentum;
};

class CollisionModule: public PrimaryModule{
private:
    bool isSolid;
    bool canPenetrateSolids;
    vector <string> ignoredObjectsList;
    vector <string> ignoredGroupsOfObjectsList;
    vector <string> ignoredHitboxesList;
    vector <string> ignoredGroupsOfHitboxesList;
public:
    vector <DetectedCollision> Detected;
    CollisionModule(string newID, vec2d size);
    CollisionModule(unsigned newID, vec2d size);
    CollisionModule(string newID);
    CollisionModule(unsigned newID);
    bool isCloseEnough(vec2d solidPos, string solidName, vec2d movingPos, vec2d movingMomentum, CollisionModule * movingColl);
    void detectOverlaping(string solidID, string collisionID, vec2d solidPos, vec2d solidSize, vec2d movPos, vec2d momentum);
    void detectCollision(string solidID, string collisionID, vec2d solidPos, vec2d solidSize, vec2d movPos, vec2d momentum);
    void clearModule();
    vec2d countMinimalDistanceBetween(vec2d sPos, vec2d sSize, vec2d mPos, vec2d mSize, double precision);
    void changeParameters(bool newIsSolid, bool newCanPenetrateSolids, vec2d newPos, vec2d newSize);
    void switchSolid();
    void switchSolidPenetration();
    //ignoring_type = ["objects", "groups_of_objects", "hitboxes", "groups_of_hitboxes"]
    void addToIgnoreList(string ignoring_type, string entity);
    //ignoring_type = ["objects", "groups_of_objects", "hitboxes", "groups_of_hitboxes"]
    void removeFromIgnoreList(string ignoring_type, string entity);
    //ignoring_type = ["objects", "groups_of_objects", "hitboxes", "groups_of_hitboxes"]
    bool ignores(string ignoring_type, string entity);
    void removeImaginaryCollisions();

    bool getIsSolid();
    bool getCanPenetrateSolids();

    void setIsSolid(bool);
    void setCanPenetrateSolids(bool);
};


#endif // COLLISIONMODULE_H_INCLUDED
