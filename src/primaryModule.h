#ifndef PRIMARYMODULE_H_INCLUDED
#define PRIMARYMODULE_H_INCLUDED
#include <cstdio>
#include "usefull.h"

using std::string;

class BaseVariableStruct{
public:
    string type;
    bool vBool;
    char vChar;
    short vShort;
    unsigned short vUShort;
    int vInt;
    unsigned int vUInt;
    float vFloat;
    double vDouble;
    string vString;

    template<typename LeftType>
    void moveValue(LeftType * LeftOperand) const;
    string getString() const;
};

class BasePointersStruct{
public:
    string type;
    bool readOnly;
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
    template<typename LeftType, typename RightType>
    void executeMoveTypeInstruction(LeftType * LeftOperand, const RightType * RightOperand, string instruction);
    void tryToSetValue(const BaseVariableStruct & RightOperand);
    template<typename RightType>
    void moveFromTemp(const RightType * RightOperand, string instruction);
    void move(const BasePointersStruct & RightOperand, string instruction);
    void move(const BaseVariableStruct & RightOperand, string instruction);
    template<typename LeftType, typename RightType>
    LeftType tryArithmetics(LeftType * LeftOperand, const RightType * RightOperand, string instruction);
    template<typename LeftType>
    LeftType callTryArithmeticsForEveryType(LeftType * LeftOperand, const BasePointersStruct & RightOperand, string instruction);
    BaseVariableStruct executeArithmetics(const BasePointersStruct &RightOperand, string instruction);
    template<typename LeftType>
    LeftType callTryArithmetics(LeftType * LeftOperand, const BaseVariableStruct & RightOperand, string instruction);
    BaseVariableStruct executeArithmetics(const BaseVariableStruct & RightOperand, string instruction);
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
    bool areEqual(BaseVariableStruct *OtherVariable);

    bool getBool() const;
    double getDouble() const;
    int getInt() const;
    string getString() const;
    bool isInteger() const;
    bool isNumeric() const;
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
    bool deleted;
    bool isScaledFromCenter;
    bool isAttachedToCamera;
    bool canBeSelected;
    bool isScrollable;

    public:

    void primaryConstructor(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void primaryConstructor(unsigned int newID, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void clone(const PrimaryModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void setID(string newID, vector<string> & listOfIDs);
    void setLayerID(string newLayerID);
    void setObjectID(string newOwnerID);
    void setAllIDs(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);

    void addGroup(string newGroup);
    void removeGroup(string selectedGroup);
    void clearGroups();
    bool isInAGroup(string findGroup) const;
    vector <string> getGroups() const;
    vector <string> & getGroupsAddr();

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
    void deleteLater();
    void setIsScaledFromCenter(bool newIsScaledFromCenter);
    void setIsAttachedToCamera(bool newIsAttachedToCamera);
    void setCanBeSelected(bool newValue);
    void setIsScrollable(bool newValue);
    string getID() const;
    string &getLayerIDAddr();
    string getLayerID() const;
    string & getIDAddr();
    string getObjectID() const;
    vec2d getPos(bool useScrollshift);
    vec2d & getPosAddr();
    vec2d getSize();
    vec2d & getSizeAddr();
    vec2d getScale();
    vec2d getScaledSize();
    bool getIsActive() const;
    bool getIsDeleted() const;
    bool getIsScaledFromCenter();
    bool getIsAttachedToCamera();
    bool getCanBeSelected();
    bool getIsScrollable();
    void getPrimaryContext(string attribute, vector <BasePointersStruct> & BasePointers);
};

template<class SearchModule>
SearchModule * findByIDAndReturnPointer(vector <SearchModule> & Container, string ID) {
    for(SearchModule & Instance : Container) {
        if (Instance.getID() == ID) {
            return &Instance;
        }
    }
    return nullptr;
}

#endif // PRIMARYMODULE_H_INCLUDED
