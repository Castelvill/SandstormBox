#ifndef SUPERTEXTMODULE_H_INCLUDED
#define SUPERTEXTMODULE_H_INCLUDED

#include "textModule.h"

class Letter{
public:
    char character;
    ALLEGRO_COLOR color;
    bool selected;
    bool inversed;
};

class SuperTextModule: public PrimaryModule{
public:
    string content;

    string fontID;
    char wrapped; //n - no wrapping, w - wrapping, s - smart wrapping
    char horizontalAlign; //l - left-aligned, c - center-aligned, r - right-aligned
    char verticalAlign; //i - up, c - center, d - down
    double rotation;
    unsigned char tabLength;

    vec2d cursorRealPos;
    unsigned cursorPos, secondCursorPos;

    vector<string> textLines;
    vector<int> lineLength;
};


#endif