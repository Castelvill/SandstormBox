#ifndef SUPERTEXTMODULE_H_INCLUDED
#define SUPERTEXTMODULE_H_INCLUDED

#include "textModule.h"

class FormatClass{
public:
    ALLEGRO_COLOR color;
    ALLEGRO_COLOR accentColor;
    SingleFont * Font;
    vec2f offset;
    bool selected;
    size_t limit;
};

class SuperTextModule: public PrimaryModule{
public:
    string content;
    vector<string> textLines; //Tabs are converted here into spaces.
    vector<float> lineWidths;
    vector<size_t> lineLengths;
    vec2f realTextSize;

    vector<FormatClass> Formatting;
    char wrapped; //n - no wrapping, c - cropping, l - wrapping by letters, w - wrapping by words
    char horizontalAlign; //l - left-aligned, c - center-aligned, r - right-aligned
    char verticalAlign; //i - up, c - center, d - down
    float paddingBetweenLines;
    float rotation;
    unsigned short tabLength;

    void setUpNewInstance();
    SuperTextModule();
    SuperTextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    SuperTextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~SuperTextModule();
    void clone(const SuperTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clear();

    void update();
    void cropSizeToText();

    VariableModule getAttributeValue(const string &attribute, const string &detail) const;
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    void drawFormattedString(string text, vec2d finalPos, size_t lineIdx, vector<FormatClass>::iterator Format);
    void draw(vec2d base, bool drawBorders, Camera2D Camera, unsigned cursorPos,
        unsigned secondCursorPos, bool editingIsActive
    );

    void setContent(string newContent);
    void addContent(string newContent);
    void addNewTextLine(string newLine);
    void setTextLine(size_t index, string newLine);
    void addToTextLine(size_t index, string newLine);
    void addFormat(ALLEGRO_COLOR newColor, ALLEGRO_COLOR newAccentColor, string fontID,
        vector <SingleFont> & FontContainer, float offsetX, float offsetY, bool isSelected, size_t newLimit
    );
    void modifyFormat(size_t index, ALLEGRO_COLOR newColor, ALLEGRO_COLOR newAccentColor, string fontID,
        vector <SingleFont> & FontContainer, float offsetX, float offsetY, bool isSelected, size_t newLimit
    );
    void deleteFormat(size_t index);
    void setColor(size_t index, ALLEGRO_COLOR newColor);
    void setAccentColor(size_t index, ALLEGRO_COLOR newAccentColor);
    void setFont(size_t index, string newFontID, vector <SingleFont> & FontContainer);
    void setOffset(size_t index, float offsetX, float offsetY);
    void addOffset(size_t index, float offsetX, float offsetY);
    void setOffsetX(size_t index, float newOffset);
    void addOffsetX(size_t index, float newOffset);
    void setOffsetY(size_t index, float newOffset);
    void addOffsetY(size_t index, float newOffset);
    void setSelected(size_t index, bool isSelected);
    void setLimit(size_t index, size_t newLimit);

    void setWrapping(char newWrapping);
    void setHorizontalAlign(char newHorizontalAlign);
    void setVerticalAlign(char newVerticalAlign);
    void setPaddingBetweenLines(float newPadding);
    void setRotation(float newRotation);
    void addRotation(float newRotation);
    void setTabLength(unsigned short newTabLength);
};

class SuperEditableTextModule : public SuperTextModule{
public:
    vector<string> previousContent, futureContent; //Checkpoints for content history - undo and redo.

    bool canBeEdited;
    bool canUseSpace;
    bool canUseEnter;
    bool canUseTabs;
    bool isNumerical;
    bool hasFloatingPoint;
    bool ignoreVerticalArrows; //Terminal history requires disabled arrows.
    bool ignoreContentRestriction;
    bool isStoringHistory; //For undo and redo.
    unsigned minContentLength, maxContentLength;
    float inputDelay, repetitionDelay;
    unsigned protectedArea; //User cannot edit the text placed before the protection cursor.

    bool isEditingActive;
    vector<short> blockedKeys;
    short lastInputedKey;
    float currentInputDelay;
    unsigned cursorPos, secondCursorPos;

    void setUpNewInstance();
    SuperEditableTextModule();
    SuperEditableTextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    SuperEditableTextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~SuperEditableTextModule();
    void clone(const SuperEditableTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clear();

    VariableModule getAttributeValue(const string &attribute, const string &detail) const;
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
};

#endif