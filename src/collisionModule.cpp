#include "collisionModule.h"


bool areObjectsOverlaping(vec2d pos1, vec2d size1, vec2d pos2, vec2d size2){
    //Function has two "unnecessary" arguments, because each hitbox can be placed in different position in relation to object's real position.
    vec2d pos1s(pos1+size1);
    vec2d pos2s(pos2+size2);
    bool canOverlap = false;

    if((pos1.x >= pos2.x && pos1.x <= pos2s.x) || (pos1s.x >= pos2.x && pos1s.x <= pos2s.x))
        canOverlap = true;
    if((pos2.x >= pos1.x && pos2.x <= pos1s.x) || (pos2s.x >= pos1.x && pos2s.x <= pos1s.x))
        canOverlap = true;

    if(!canOverlap)
        return false;

    if((pos1.y >= pos2.y && pos1.y <= pos2s.y) || (pos1s.y >= pos2.y && pos1s.y <= pos2s.y))
        return true;
    if((pos2.y >= pos1.y && pos2.y <= pos1s.y) || (pos2s.y >= pos1.y && pos2s.y <= pos1s.y))
        return true;

    return false;
}


CollisionModule::CollisionModule(int collisionModuleID){
    primaryConstructor(collisionModuleID);
    setSize(0.0, 0.0);
    isSolid = false;
    canPenetrateSolids = false;
}
CollisionModule::CollisionModule(int collisionModuleID, vec2d size){
    primaryConstructor(collisionModuleID);
    setSize(size);
    isSolid = false;
    canPenetrateSolids = false;
}

void CollisionModule::detectOverlaping(string solidID, string collisionID, vec2d solidPos, vec2d solidSize, vec2d movPos, vec2d momentum){
    //Check if object is in other object
    if(areObjectsOverlaping(solidPos, solidSize, pos + movPos, size)){
        Detected.push_back({solidID, collisionID, 4, momentum});
    }
}

void CollisionModule::detectCollision(string solidID, string collisionID, vec2d solidPos, vec2d solidSize, vec2d movPos, vec2d momentum){
    vec2d mPos, mPos2;
    mPos = pos + movPos;

    //Check if object is in other object, if yes, then don't worry about it
    if(areObjectsOverlaping(solidPos, solidSize, mPos, size)){
        Detected.push_back({solidID, collisionID, 4, momentum});
        return;
    }

    //Find the smallest momentum needed to reach collision (without it, object with enough speed would ignore some collisions)
    vec2d minMomentum = countMinimalDistanceBetween(solidPos, solidSize, mPos, size, -1.0);

    mPos2 = mPos + momentum;
    if(momentum.x * minMomentum.x > 0 && fabs(momentum.x) > fabs(minMomentum.x)){
        mPos2.x = mPos.x + minMomentum.x;
    }
    if(momentum.y * minMomentum.y > 0 && fabs(momentum.y) > fabs(minMomentum.y)){
        mPos2.y = mPos.y + minMomentum.y;
    }

    //Check if x momentum will cause a collision
    if(areObjectsOverlaping(solidPos, solidSize, vec2d(mPos2.x, mPos.y), size)){
        momentum.x = countMinimalDistanceBetween(solidPos, solidSize, mPos, size, 0.1).x;
        momentum.y = 0;
        Detected.push_back({solidID, collisionID, 1, momentum});
        return;
    }

    //Check if y momentum will cause a collision.
    if(areObjectsOverlaping(solidPos, solidSize, vec2d(mPos.x, mPos2.y), size)){
        momentum.x = 0;
        momentum.y = countMinimalDistanceBetween(solidPos, solidSize, mPos, size, 0.1).y;
        Detected.push_back({solidID, collisionID, 2, momentum});
        return;
    }

    //Check if both momentums combined will cause a collision.
    if(areObjectsOverlaping(solidPos, solidSize, vec2d(mPos2.x, mPos2.y), size)){
        momentum.x = countMinimalDistanceBetween(solidPos, solidSize, mPos, size, 0.1).x;
        momentum.y = countMinimalDistanceBetween(solidPos, solidSize, mPos, size, 0.1).y;
        Detected.push_back({solidID, collisionID, 3, momentum});
    }
}
vec2d CollisionModule::countMinimalDistanceBetween(vec2d sPos, vec2d sSize, vec2d mPos, vec2d mSize, double precision){
    vec2d distance(0.0, 0.0), sPos2(sPos + sSize), mPos2(mPos + mSize);
    if(mPos2.x < sPos.x){
        distance.x = sPos.x-precision-mSize.x-mPos.x;
    }
    else if(mPos.x > sPos2.x){
        distance.x = sPos2.x+precision-mPos.x;
    }
    if(mPos2.y < sPos.y){
        distance.y = sPos.y-precision-mSize.y-mPos.y;
    }
    else if(mPos.y > sPos2.y){
        distance.y = sPos2.y+precision-mPos.y;
    }
    return distance;
}
bool CollisionModule::isCloseEnough(vec2d solidPos, string solidName, vec2d movingPos, vec2d movingMomentum, CollisionModule * movingColl){
    //This is optimalization function. It tells if object can be checked for collision with moving objects. Using this function can speed up games with complex collisions or too many distant objects.
    double x = solidPos.x+pos.x+size.x/2;
    double y = solidPos.y+pos.y+size.y/2;
    double x2 = movingPos.x+movingColl->pos.x+movingColl->size.x/2;
    double y2 = movingPos.y+movingColl->pos.y+movingColl->size.y/2;
    double distance = countDistance2(x, y, x2, y2); //distance from centers

    double radiusOfSolid = 2+sqrt(pow(size.x, 2) + pow(size.y, 2)) / 2.0;
    double radiusOfMoving = 2+sqrt(pow(movingColl->size.x + abs(movingMomentum.x), 2) + pow(movingColl->size.y + abs(movingMomentum.y), 2)) / 2.0;
    if(distance <= radiusOfSolid + radiusOfMoving)
        return true;
    return false;
}
void CollisionModule::changeParameters(bool newIsSolid, bool newCanPenetrateSolids, vec2d newPos, vec2d newSize){
    isSolid = newIsSolid;
    setPos(newPos);
    setSize(newSize);
    canPenetrateSolids = newCanPenetrateSolids;
}
void CollisionModule::clearModule(){
    ignoreCollisionList.clear();
    Detected.clear();
}
void CollisionModule::switchSolid(){
    isSolid = !isSolid;
}
void CollisionModule::switchSolidPenetration(){
    if(canPenetrateSolids)
        canPenetrateSolids = false;
    else
        canPenetrateSolids = true;
}
void CollisionModule::addToIgnoreCollisionList(string objectName){
    ignoreCollisionList.push_back(objectName);
}
void CollisionModule::removeFromIgnoreCollisionList(string objectName){
    //removeValueFromVector<std::string>(ignoreCollisionList, objectName);
    ignoreCollisionList.erase(std::remove(ignoreCollisionList.begin(),
                                          ignoreCollisionList.end(), objectName),
                                          ignoreCollisionList.end());
}
void CollisionModule::removeImaginaryCollisions(){
    if(Detected.size() <= 1){
        return;
    }

    //Remove collision walls of the same type placed further from the moving object's collision boxes
    for(unsigned int i = 0; i < Detected.size(); i++){
        if(Detected[i].collisionType == 0 || Detected[i].collisionType == 4 || Detected[i].collisionType == 3){
            continue;
        }
        for(unsigned int j = 0; j < Detected.size(); j++){
            if(i == j || Detected[j].collisionType == 0 || Detected[j].collisionType == 4 || Detected[i].collisionType == 3){
                continue;
            }
            if(Detected[i].collisionType == Detected[j].collisionType){
                if(Detected[i].momentum.countLength() > Detected[j].momentum.countLength()){
                    Detected[i].collisionType = 0;
                    break;
                }
                if(Detected[i].momentum.countLength() < Detected[j].momentum.countLength()){
                    Detected[j].collisionType = 0;
                }
            }
        }
    }

    //Remove invalid collision walls of the third type
    for(unsigned int i = 0; i < Detected.size(); i++){
        if(Detected[i].collisionType != 3){
            continue;
        }
        for(unsigned int j = 0; j < Detected.size(); j++){
            if(i == j || Detected[j].collisionType == 0 || Detected[j].collisionType == 4
                    || Detected[j].collisionType == 3){
                continue;
            }
            if(Detected[j].collisionType == 1 && fabs(Detected[i].momentum.x) >= fabs(Detected[j].momentum.x)){
                Detected[i].collisionType = 0;
                break;
            }
            if(Detected[j].collisionType == 2 && fabs(Detected[i].momentum.y) >= fabs(Detected[j].momentum.y)){
                Detected[i].collisionType = 0;
                break;
            }
        }
    }

    //Remove all collision walls of the first type if collision walls of the third type still exist
    for(unsigned int i = 0; i < Detected.size(); i++){
        if(Detected[i].collisionType != 3){
            continue;
        }
        for(unsigned int j = 0; j < Detected.size(); j++){
            if(Detected[j].collisionType == 1){
                Detected[j].collisionType = 0;
            }
        }
    }
}

bool CollisionModule::getIsSolid(){
    return isSolid;
}
bool CollisionModule::getCanPenetrateSolids(){
    return canPenetrateSolids;
}
vector <string> CollisionModule::getIgnoreCollisionList(){
    return ignoreCollisionList;
}

void CollisionModule::setIsSolid(bool newValue){
    isSolid = newValue;
}
void CollisionModule::setCanPenetrateSolids(bool newValue){
    canPenetrateSolids = newValue;
}

