#ifndef COLLISIONMODULE_H_INCLUDED
#define COLLISIONMODULE_H_INCLUDED

#include "primaryModule.h"

bool areObjectsOverlaping(vec2d pos1, vec2d size1, vec2d pos2, vec2d size2);

struct DetectedCollision{
    string solidID;
    string collisionID;
    //0 - invalid collision, 1 - collision on X axis, 2 - collision on Y axis,
    //3 - collision with corners, 4 - overlaping "collision"
    short collisionType; 
    vec2d momentum;
};

class CollisionModule: public PrimaryModule{
private:
    bool isSolid;
    bool canPenetrateSolids;
    bool isCircle;
    char mouseCollision; //n-no, p-pressing, f-first pressed, r-released
    vector<string> ignoredObjectsList;
    vector<string> ignoredGroupsOfObjectsList;
    vector<string> ignoredHitboxesList;
    vector<string> ignoredGroupsOfHitboxesList;
public:
    vector <DetectedCollision> Detected;
    void setUpNewInstance(PrimaryData & initData, const vec2d & size);
    CollisionModule(size_t & topModuleUniqueIndex);
    CollisionModule(PrimaryData & initData, vec2d size);
    CollisionModule(PrimaryData & initData);
    ~CollisionModule();
    void clone(const CollisionModule & Original, PrimaryData & initData, bool changeOldID);
    bool isCloseEnough(vec2d solidPos, string solidName, vec2d movingPos, vec2d movingMomentum,
        CollisionModule * movingColl);
    void detectOverlaping(const string & solidID, const string & collisionID, vec2d solidPos,
        vec2d solidSize, vec2d movPos, vec2d momentum);
    void detectCollision(const string & solidID, const string & collisionID, vec2d solidPos,
        vec2d solidSize, vec2d movPos, vec2d momentum);
    void clear();
    vec2d countMinimalDistanceBetween(vec2d sPos, vec2d sSize, vec2d mPos, vec2d mSize,
        double precision);
    void switchSolid();
    void switchSolidPenetration();
    //ignoring_type = ["objects", "groups_of_objects", "hitboxes", "groups_of_hitboxes"]
    void addToIgnoreList(string ignoring_type, string entity);
    //ignoring_type = ["objects", "groups_of_objects", "hitboxes", "groups_of_hitboxes"]
    void removeFromIgnoreList(string ignoring_type, string entity);
    void clearIgnoreList(string ignoring_type);
    //ignoring_type = ["objects", "groups_of_objects", "hitboxes", "groups_of_hitboxes"]
    bool ignores(string ignoring_type, string entity) const;
    void removeImaginaryCollisions();
    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);

    bool getIsSolid() const;
    bool getCanPenetrateSolids() const;
    char getMouseCollision() const;
    bool getIsCircle() const;

    void setIsSolid(bool);
    void setCanPenetrateSolids(bool);
    void setMouseCollision(char);
    void setIsCircle(bool);
};


#endif // COLLISIONMODULE_H_INCLUDED
