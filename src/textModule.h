#ifndef TEXTMODULE_H_INCLUDED
#define TEXTMODULE_H_INCLUDED

#include <allegro5/allegro_ttf.h>
#include "particles.h"

class SingleFont{
public:
    string ID;
    ALLEGRO_FONT * font;
    short sizeF;
};

SingleFont * findFontByID(vector <SingleFont> & FontContainer, string fontID);
double getFontHeight(vector <SingleFont> FontContainer, string fontID);

class TextModule: public PrimaryModule{
//It connects with a font via font ID and size.
public:
    vector <string> content;
    unsigned currentTextIdx; //indexes of content vector
    string fontID;
    ALLEGRO_COLOR color;
    short wrapped; //0-single line, 1-wrapped, 2-smart wrap
    short horizontalAlign; //0-left-aligned, 1-center-aligned, 2-right-aligned
    short verticalAlign; //0-up, 1-center, 2-down
    double rotation;
    float randomChangeSpeed;
    float minColorValue, maxColorValue;
    unsigned tabLength;
    unsigned cursorPos, secondCursorPos;
    bool ignoreSize;

    short usedBitmapLayer;  //Text fields with numbers higher or equal to zero are drawn in ascending order. If the value is -1, text will be drawn on top of everything else.

    void setUpNewInstance();
    TextModule();
    TextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    TextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~TextModule();
    void clone(const TextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void adjustCursorPos();
    void fitSizeToText(vector <SingleFont> FontContainer);
    void addNewContent(string newContent);
    void addNewContentAndResize(string newContent, vector <SingleFont> FontContainer);
    void chooseContent(unsigned int textID);
    bool checkSize(unsigned int textID) const;
    void addTextToContent(unsigned int textNumber, string newText);
    void modifyContent(unsigned int textID, string modifiedContent);
    void modifyContentAndResize(unsigned int textID, string modifiedContent, vector <SingleFont> FontContainer);
    void deleteContent(unsigned int textID);
    void clear();
    void setColors(float r, float g, float b, float a);
    void setRandomColors();
    void incrementRandomColor();
    void setRotation(double newAngle);
    void setUsedBitmapLayer(int newLayer);
    void addRotation(double newAngle);
    void setAlpha(float newValue);
    void setFontID(string);
    void setWrapping(short newValue);
    void setHorizontalAlign(short newValue);
    void setVerticalAlign(short newValue);
    void setCursorPos(unsigned int newCursorPos);
    void setSecondCursorPos(unsigned int newCursorPos);
    void setCursorPos(vec2d finalPos, vec2d finalSize, const vector<SingleFont> & FontContainer, const MouseClass & Mouse, const Camera2D & Camera);
    void drawText(vec2d base, ALLEGRO_FONT * font, bool drawBorders, Camera2D Camera, unsigned int cursorPos, unsigned secondCursorPos, bool editingIsActive);
    void drawTextByLetters(ALLEGRO_FONT * font);
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    string getFontID();
    string getContent(unsigned int textID) const;
    float getColor(char whichColor);
    unsigned int getCurrentTextIdx() const;
    string getCurrentContent() const;
    VariableModule getAttributeValue(const string &attribute, const string &detail) const;
    unsigned getCurrentTabLength(const unsigned & tabCounter);
};


class EditableTextModule: public TextModule{
private:
    bool canBeEdited;
    bool canUseSpace;
    bool canUseEnter;
    bool isNumerical;
    bool hasFloatingPoint;
    bool updateConnectedVariable;
    bool canClearContentAfterSuccess; //If true, content is cleaned only when all operations succeeded.
    bool enterAcceptsChanges;
    bool useTabs;
    bool ignoreContentSize;
    unsigned minContentSize;
    unsigned maxContentSize;
    float inputDelay, repetitionDelay;

    bool editingIsActive;
    vector<short> blockedKeys;
    short lastInputedKey;
    unsigned protectedArea; //User cannot edit the text placed before the protection cursor.
public:
    string connectedObject;
    string connectedGroup;
    string connectedModule;
    string connectedModuleID;
    string connectedVariable;
    float currentInputDelay;

    void setUpNewInstance();
    EditableTextModule();
    EditableTextModule(unsigned EditableTextModuleID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    EditableTextModule(string EditableTextModuleID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void clone(const EditableTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void setCanBeEdited(bool newCanBeEdited);
    void setEditingIsActive(bool newEditingIsActive);
    void setCanUseSpace(bool newCanUseSpace);
    void setCanUseEnter(bool newCanUseEnter);
    void setIsNumerical(bool newIsNumerical);
    void setHasFloatingPoint(bool newHasFloatingPoint);
    void setUpdateConnectedVariable(bool newUpdateConnectedVariable);
    void setUseTabs(bool newValue);
    void setMinContentSize(unsigned int newMinContentSize);
    void setMaxContentSize(unsigned int newMaxContentSize);
    void setInputDelay(float);
    void setRepetitionDelay(float);
    void setConnectedObjectID(string newValue);
    void setConnectedObject(string objectID, string moduleType, string moduleID, string variableName);
    void setConnectedGroup(string groupName, string moduleType, string moduleID, string variableName);
    void setCanClearContentAfterSuccess(bool);
    void setCanEnterAcceptChanges(bool);
    void setProtectedArea(unsigned cursor);
    bool getCanBeEdited() const;
    bool getEditingIsActive() const;
    bool getCanUseSpace();
    bool getCanUseEnter();
    bool getIsNumerical();
    bool getHasFloatingPoint();
    bool getUpdateConnectedVariable();
    unsigned getCursorPos();
    unsigned getMinContentSize();
    unsigned getMaxContentSize();
    string getConnectedObjectID();
    bool canConvertContentToNumber();
    unsigned getProtectedArea() const;
    void clearContentAfterSuccess(bool success);
    void moveCursorUp(const string & text, bool shift);
    void moveCursorDown(const string & text, bool shift);
    bool prepareEditing(const vector <short> & releasedKeys, vector <short> & pressedKeys, bool & shift, bool & control);
    void getNumbers(char pKey, char & character, bool shift);
    void getLetters(char pKey, char & character, bool shift);
    bool deleteFromText(char pKey, char character, string text, bool & control, ALLEGRO_DISPLAY * window);
    void addFloatingPoint(char pKey, char & character, string text);
    bool addMinus(char pKey, char & character, string text);
    void editText(vector <short> releasedKeys, vector <short> pressedKeys, vector <SingleFont> & FontContainer, ALLEGRO_DISPLAY * window);
    //Function checks if the new id is unique and if it is, function changes previous id and updates listOfIds.
    bool tryUpdatingID(vector <string> & listOfIDs, string & currentID, string newID);
    bool controlAncestor(PrimaryModule & Primary, vector <string> & listOfAncestorIDs);
    bool controlText(TextModule & Text, vector <string> & listOfIDs);
    bool controlImage(ImageModule & Image, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfIDs, string EXE_PATH);
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
