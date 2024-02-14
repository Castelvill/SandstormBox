#ifndef ENGINECLASS_H_INCLUDED
#define ENGINECLASS_H_INCLUDED

#include "layerClass.h"

#if __linux__
    #include <sys/time.h>
    unsigned long long GetTickCount();
#endif

#define KEY_SEEN     1
#define KEY_RELEASED 2

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
    ALLEGRO_DISPLAY * display;
    ALLEGRO_BITMAP * cursorBitmap;
    ALLEGRO_BITMAP * iconBitmap;
    ALLEGRO_MOUSE_CURSOR * mouseCursor;
    ALLEGRO_TIMER * timer;
    ALLEGRO_EVENT_QUEUE * eventQueue;
    ALLEGRO_EVENT event;

    Fps fps;
    string windowTitle;
    vec2i displaySize;
    bool fullscreen;
    bool isPixelArt; //If true, zoomed bitmaps will not look blurry.

    bool closeProgram, reboot, redraw, displayResized;
    string EXE_PATH;

    vector <SingleFont> FontContainer;
    vector <SingleBitmap> BitmapContainer;
    vector <string> processIDs;
    string focusedProcessID; //Only the focused process can change the cursor.

    MouseClass Mouse;
    unsigned char key[ALLEGRO_KEY_MAX];
    vector <short> pressedKeys;
    vector <short> firstPressedKeys;
    vector <short> releasedKeys;

    vector <unsigned> processDisplayOrder;

    void resetState(string title);
    EngineClass();
    void prepare();
    void initAllegro();
    void clear();
    void exitAllegro();
    void updateEvents();
    void endEvents();

    void loadNewFont(string path, int size, string newID);

    bool isRunning() const;
    int getWindowW() const;
    int getWindowH() const;
    ALLEGRO_DISPLAY *getWindow();
    vec2i getDisplaySize() const;
    bool secondHasPassed() const;
};

#endif