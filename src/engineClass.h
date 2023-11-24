#ifndef ENGINECLASS_H_INCLUDED
#define ENGINECLASS_H_INCLUDED

#include "layerClass.h"

class Interval{
private:
    unsigned int initial_;
public:
    // Ctor
    inline Interval() : initial_(GetTickCount()){};
    // Dtor
    virtual ~Interval(){};
    inline unsigned int value() const;
};

class Fps{
protected:
    unsigned int m_fps;
    unsigned int m_fpscount;
    Interval m_fpsinterval;
public:
    Fps() : m_fps(0), m_fpscount(0){};
    void update();
    unsigned int get() const;
};

class EngineClass{
public:
    ALLEGRO_DISPLAY * window;
    ALLEGRO_BITMAP * cursorBitmap;
    ALLEGRO_BITMAP * iconBitmap;
    ALLEGRO_MOUSE_CURSOR * mouseCursor;
    ALLEGRO_TIMER * timer;
    ALLEGRO_EVENT_QUEUE * eventQueue;
    ALLEGRO_EVENT event;

    Fps fps;
    string windowTitle;
    int windowW;
    int windowH;
    bool fullscreen;
    bool isPixelArt; //If true, zoomed bitmaps will not look blurry.

    bool closeProgram, redraw, displayResized;
    string EXE_PATH;

    vector <SingleFont> FontContainer;
    vector <SingleBitmap> BitmapContainer;

    MouseClass Mouse;
    unsigned char key[ALLEGRO_KEY_MAX];
    vector <short> pressedKeys;
    vector <short> firstPressedKeys;
    vector <short> releasedKeys;

    EngineClass(string title);
    void initAllegro();
    void exitAllegro();
    void updateEvents();
    void clearEvents();

    bool isRunning() const;
    int getWindowW() const;
    int getWindowH() const;
    ALLEGRO_DISPLAY *getWindow();
    vec2i getScreenSize() const;
    bool secondHasPassed() const;
};

#endif