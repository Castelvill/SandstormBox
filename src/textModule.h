#ifndef TEXTMODULE_H_INCLUDED
#define TEXTMODULE_H_INCLUDED

#include "camera.h"
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "movementModule.h"
#include "particles.h"
#include "variableModule.h"

class SingleFont{
public:
    string ID;
    ALLEGRO_FONT * font;
    short sizeF;
};

SingleFont * findFontByID(std::vector <SingleFont> FontContainer, string fontID);
double getFontHeight(std::vector <SingleFont> FontContainer, string fontID);

class TextModule: public PrimaryModule{
//It connects with a font via font ID and size.
public:
    vector <string> content;
    unsigned int currentTextID; //indexes of content vector
    string fontID;
    unsigned short textColor[3];
    short wrapped; //0-single line, 1-wrapped, 2-smart wrap
    short horizontalAlign; //0-left-aligned, 1-center-aligned, 2-right-aligned
    short verticalAlign; //0-up, 1-center, 2-down
    double rotateAngle;
    double visibility;

    short usedBitmapLayer;  //Text fields with numbers higher or equal to zero are drawn in ascending order. If the value is -1, text will be drawn on top of everything else.

    void setUpNewInstance();
    TextModule();
    TextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    TextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void clone(const TextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void fitSizeToText(std::vector <SingleFont> FontContainer);
    void addNewContent(string newContent);
    void addNewContentAndResize(string newContent, std::vector <SingleFont> FontContainer);
    void chooseContent(unsigned int textID);
    void modifyContent(unsigned int textID, string modifiedContent);
    void modifyContentAndResize(unsigned int textID, string modifiedContent, vector <SingleFont> FontContainer);
    void deleteContent(unsigned int textID);
    void clear();
    void setColors(unsigned short red, unsigned short green, unsigned short blue);
    void setRandomColors();
    void setRotation(double newAngle);
    void setUsedBitmapLayer(int newLayer);
    void addRotation(double newAngle);
    void setVisibility(double newVisibility);
    void setFontID(string);
    void changeParameters(string newID, vector<string> & listOfIDs, vec4d posSize, vec3d fontColor, string newFontID, vec2d newScale,
                          double newRotateAngle, short newWrapped, int newHorizontalAlign, int newVerticalAlign);
    void drawText(vec2d base, ALLEGRO_FONT * font, bool drawBorders, Camera2D Camera, unsigned int cursorPos, bool editingIsActive);
    void drawTextByLetters(ALLEGRO_FONT * font);
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    string getFontID();
    string getContent(unsigned int textID) const;
    unsigned short getColor(char whichColor);
    unsigned int getCurrentTextID() const;
    string getCurrentContent() const;
    VariableModule getAttributeValue(const string &attribute, const string &detail) const;
};


class EditableTextModule: public TextModule{
private:
    bool canBeEdited;
    bool editingIsActive;
    bool canUseSpace;
    bool isNumerical;
    bool hasFloatingPoint;
    bool updateConnectedVariable;
    bool canClearContentAfterSuccess; //If true, content is cleaned only when all operations succeeded.
    bool useArrowsAsChar;
    unsigned int cursorPos;
    unsigned int minContentSize;
    unsigned int maxContentSize;

public:
    string connectedObject;
    string connectedGroup;
    string affectedModule;
    string connectedModuleID;
    string affectedVariable;

    void setUpNewInstance();
    EditableTextModule();
    EditableTextModule(unsigned EditableTextModuleID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    EditableTextModule(string EditableTextModuleID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void clone(const EditableTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void setCanBeEdited(bool newCanBeEdited);
    void setEditingIsActive(bool newEditingIsActive);
    void setCanUseSpace(bool newCanUseSpace);
    void setIsNumerical(bool newIsNumerical);
    void setHasFloatingPoint(bool newHasFloatingPoint);
    void setUpdateConnectedVariable(bool newUpdateConnectedVariable);
    void setUseArrowsAsChar(bool newValue);
    void setCursorPos(unsigned int newCursorPos);
    void setMinContentSize(unsigned int newMinContentSize);
    void setMaxContentSize(unsigned int newMaxContentSize);
    void setConnectedObjectID(string newValue);
    void setConnectedObject(string objectID, string moduleType, string moduleID, string variableName);
    void setConnectedGroup(string groupName, string moduleType, string moduleID, string variableName);
    void setCanClearContentAfterSuccess(bool);
    bool getCanBeEdited() const;
    bool getEditingIsActive() const;
    bool getCanUseSpace();
    bool getIsNumerical();
    bool getHasFloatingPoint();
    bool getUpdateConnectedVariable();
    bool getUseArrowsAsChar();
    unsigned int getCursorPos();
    unsigned int getMinContentSize();
    unsigned int getMaxContentSize();
    string getConnectedObjectID();
    bool canConvertContentToNumber();
    void clearContentAfterSuccess(bool success);
    void editText(vector <short> releasedKeys, vector <short> pressedKeys);
    //Function checks if the new id is unique and if it is, function changes previous id and updates listOfIds.
    bool tryUpdatingID(vector <string> & listOfIDs, string & currentID, string newID);
    bool controlAncestor(PrimaryModule & Primary, vector <string> & listOfAncestorIDs);
    bool controlText(TextModule & Text, vector <string> & listOfIDs);
    bool controlImage(ImageModule & Image, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfIDs);
    bool controlMovement(MovementModule & Movement, vector <string> & listOfIDs);
    bool controlCollision(CollisionModule & Collision, vector <string> & listOfIDs);
    bool controlParticles(ParticleEffectModule & Particles, vector <string> & listOfIDs);
    bool controlVariable(VariableModule & Variable, vector <string> & listOfIDs);
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    VariableModule getAttributeValue(const string &attribute, const string &detail) const;
};

void printNotNumericalWarning();
void printCommandDoesNotExistWarning();

#endif // TEXTMODULE_H_INCLUDED
