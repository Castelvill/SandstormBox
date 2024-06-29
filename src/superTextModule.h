#ifndef SUPERTEXTMODULE_H_INCLUDED
#define SUPERTEXTMODULE_H_INCLUDED

#include "textModule.h"
#include <fstream>

class FormatClass{
public:
    ALLEGRO_COLOR color;
    ALLEGRO_COLOR accentColor;
    SingleFont * Font; //Not a dynamic pointer.
    vec2f offset;
    bool selected;
    size_t limit, drawingLimit;
    bool isTheSame(const FormatClass & Compare);
};

class SuperTextModule: public PrimaryModule{
public:
    string content;
    vector<string> textLines; //Tabs are converted here into spaces.
    vector<float> lineWidths;
    vector<int> lineHeights;
    vector<size_t> lineLengths;
    vector<unsigned> lineStarts; //Needed for the situation when there are tabs on the beginning of the line.
    vector<bool> floatingNewLine;
    vec2f realTextSize;

    vector<FormatClass> Formatting;
    char wrapped; //n - no wrapping, c - cropping, l - wrapping by letters, w - wrapping by words
    char horizontalAlign; //l - left-aligned, c - center-aligned, r - right-aligned
    char verticalAlign; //i - up, c - center, d - down
    float paddingBetweenLines;
    float rotation;
    unsigned short tabLength;
    unsigned cursorPos, secondCursorPos;
    unsigned localCursorPos, lineWithCursorIdx;
    unsigned localSecondCursorPos, lineWithSecondCursorIdx;
    float lineWidthToCursor, lineWidthToSecondCursor;
    bool updated;

    bool drawSelectionFirst; //If true, whole text content is rendered twice, first - only the selection, second - only the text.  

    void setUpNewInstance();
    SuperTextModule();
    SuperTextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    SuperTextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~SuperTextModule();
    void clone(const SuperTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clear();

    void update();
    void cropSizeToText();
    void cropWidthToText();
    void cropHeightToText();

    VariableModule getAttributeValue(const string &attribute, const string &detail, EventDescription EventIds) const;
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    
    void drawFormattedString(string text, vec2d finalPos, size_t lineIdx,
        vector<FormatClass>::iterator Format, bool drawSelection
    );
    void drawAllLines(vec2d finalPos, bool drawSelection, vec2i displaySize);
    void draw(vec2d base, bool drawBorders, Camera2D Camera, unsigned cursorPos,
        unsigned secondCursorPos, bool editingIsActive, vec2i displaySize
    );

    void fitFormattingToContent();
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
    void injectFormat(unsigned fragmentStart, unsigned fragmentEnd, ALLEGRO_COLOR newColor,
        ALLEGRO_COLOR newAccentColor, string fontID, vector <SingleFont> & FontContainer,
        float offsetX, float offsetY, bool isSelected
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
    void setCursorPos(int newPos);
    void setSecondCursorPos(int newPos);
    void divideFormattingByCursor();
    void cutContent(size_t newSize);
};

class SuperEditableTextModule : public SuperTextModule{
public:
    vector<string> previousContent, futureContent; //Checkpoints for content history - undo and redo.
    vector<FormatClass> previousFormatting, futureFormatting;

    bool canBeEdited;
    bool canUseSpace;
    bool canUseEnter;
    bool enterEndsEditing;
    bool canUseTabs;
    bool isNumerical;
    bool hasFloatingPoint;
    bool ignoreVerticalArrows; //Terminal history requires disabled arrows.
    bool ignoreContentRestriction;
    bool isStoringHistory; //For undo and redo.
    bool canCopyFormat;
    unsigned minContentLength, maxContentLength;
    float inputDelay, repetitionDelay;
    unsigned protectedArea; //User cannot edit the text placed before the protection cursor.

    bool isEditingActive;
    vector<short> blockedKeys;
    short lastInputedKey;
    float currentInputDelay;

    void setUpNewInstance();
    SuperEditableTextModule();
    SuperEditableTextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    SuperEditableTextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~SuperEditableTextModule();
    void clone(const SuperEditableTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clear();

    VariableModule getAttributeValue(const string &attribute, const string &detail, EventDescription EventIds) const;
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);

    bool prepareEditing(const vector <short> & releasedKeys, vector <short> & pressedKeys, bool & shift, bool & control);
    void executeOneBackspaceOrCtrlX(char pKey, string text, ALLEGRO_DISPLAY * window, unsigned & leftCursorOnFormatIdx,
        unsigned & rightCursorOnFormatIdx, bool ENABLE_al_set_clipboard_text, string & internalClipboard,
        vector<FormatClass> & CopiedFormatting, string EXE_PATH
    );
    void executeOneDeletion(string text, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    bool deleteFromText(char pKey, string text, bool & control, ALLEGRO_DISPLAY * window,
        unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx,
        bool ENABLE_al_set_clipboard_text, string & internalClipboard,
        vector<FormatClass> & CopiedFormatting, string EXE_PATH
    );
    void getNumbers(char pKey, char & character, bool shift);
    void addFloatingPoint(char pKey, char & character, string text);
    bool addMinus(char pKey, char & character, string text);
    void getLetters(char pKey, char & character, bool shift);
    void prepareToMoveCursorUp(bool shift, float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength,
        unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx
    );
    bool isCursorOnTheFirstLine(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void takeCareOfLastLineIfEmpty(float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit);
    void moveCursorToTheStartOfTheCurrentTextLine(unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit);
    void moveCursorToTheStartOfThePreviousTextLine(bool shift, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit);
    void moveCursorToTheSavedWidth(float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx,
        unsigned & limit, float selectedWidth, unsigned & currentTabLength);
    void updateFormattingForUpMoveWithoutShift(unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void updateFormattingForUpMoveWithShift(unsigned & letterIdx, unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveCursorUp(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void prepareToMoveCursorDown(bool shift, float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength,
        unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx
    );
    bool isCursorOnTheLastLine(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveToTheEndOfTheLine(float & currentWidth, unsigned & letterIdx, unsigned & currentLineLength, unsigned & formatIdx, unsigned & limit);
    void updateFormattingForDownMoveWithoutShift(unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void updateFormattingForDownMoveWithShift(unsigned & letterIdx, unsigned & formatIdx, unsigned & limit, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveCursorDown(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveCursorToLeftByOne(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveCursorToLeft(bool shift, bool control, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveCursorToRightByOne(bool shift, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void moveCursorToRight(bool shift, bool control, unsigned & leftCursorOnFormatIdx, unsigned & rightCursorOnFormatIdx);
    void edit(vector <short> releasedKeys, vector <short> pressedKeys, ALLEGRO_DISPLAY * window,
        bool ENABLE_al_set_clipboard_text, string & internalClipboard, vector<FormatClass> & CopiedFormatting,
        string EXE_PATH, bool allowNotAscii
    );
    void setCursorPos(int newPos);
    void setSecondCursorPos(int newPos);
    float getWidthOfLetterInTheText(unsigned currentCursorPos);
    void setCursorsWithMouse(vec2d finalPos, const MouseClass & Mouse, const Camera2D *Camera);
};

#endif