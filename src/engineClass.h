#ifndef ENGINECLASS_H_INCLUDED
#define ENGINECLASS_H_INCLUDED

#include "layerClass.h"

vector<string> tokenizeString(string input, char delimeter);
void getDesktopResolution(int adapter, int *w, int *h);

#if __linux__
    #include <sys/time.h>
    unsigned long long GetTickCount();
#endif

#define KEY_SEEN     1
#define KEY_RELEASED 2

inline int TERMINATION_TIME = 500;

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
    ALLEGRO_DISPLAY * display = nullptr;
    ALLEGRO_BITMAP * cursorBitmap;
    ALLEGRO_BITMAP * iconBitmap = nullptr;
    ALLEGRO_BITMAP * backbuffer = nullptr;
    ALLEGRO_MOUSE_CURSOR * mouseCursor;
    ALLEGRO_TIMER * timer;
    ALLEGRO_EVENT_QUEUE * eventQueue;
    ALLEGRO_EVENT event;

    Fps fps;
    string windowTitle = "SandstormBox";
    vec2i displaySize, backbufferSize;

    bool canTerminateWithTimeout = true;
    bool loadConfig = true;
    unsigned short samples = 0;
    bool fullscreen = false;
    bool isPixelArt = false; //If true, zoomed bitmaps will not look blurry.
    bool ENABLE_al_set_clipboard_text = false; //al_set_clipboard_text can cause undefined behavior on GNU/Linux (it depends on window manager used).
    bool allowNotAscii = false;
    bool autoScaleBackbuffer = false;

    vector<string> inputFiles;

    string internalClipboard;
    vector<FormatClass> CopiedFormatting;

    bool closeProgram, reboot, redraw, displayResized;
    int terminationTimer = TERMINATION_TIME;
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

    vector <string> initFiles;

    void readCommandLine(int argc, char* argv[]);
    void resetState(bool resetScreen);
    EngineClass();
    void initAllegro();
    void createDisplay();
    void clear();
    void exitAllegro();
    void updateEvents();
    void endEvents();

    void loadNewFont(string path, int size, string newID);

    bool isRunning() const;
    int getDisplayW() const;
    int getDisplayH() const;
    ALLEGRO_DISPLAY *getDisplay();
    vec2i getDisplaySize() const;
    bool secondHasPassed() const;
};

#endif