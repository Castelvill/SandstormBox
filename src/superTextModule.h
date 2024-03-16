#ifndef SUPERTEXTMODULE_H_INCLUDED
#define SUPERTEXTMODULE_H_INCLUDED

#include "textModule.h"

class Format{
public:
    ALLEGRO_COLOR color;
    SingleFont font;
    vec2f offset;
    bool selected;
};

class SuperTextModule: public PrimaryModule{
public:
    string content;
    vector<string> textLines;

    vector<float> offsetInLine; //Adjusting letters when more than one font is used.
    vector<float> lineLengths;
    unsigned char tabLength;

    vector<Format> Formatting;
    char wrapped; //n - no wrapping, w - wrapping, s - smart wrapping
    char horizontalAlign; //l - left-aligned, c - center-aligned, r - right-aligned
    char verticalAlign; //i - up, c - center, d - down
    float paddingBetweenLines;
    float rotation;

    void setUpNewInstance();
    SuperTextModule();
    SuperTextModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    SuperTextModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~SuperTextModule();
    void clone(const SuperTextModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clear();

    VariableModule getAttributeValue(const string &attribute, const string &detail) const;
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    void draw(vec2d base, vector <SingleFont> & FontContainer, bool drawBorders, Camera2D Camera, unsigned cursorPos, unsigned secondCursorPos, bool editingIsActive);
};


#endif