#ifndef ENGINELOOP_H_INCLUDED
#define ENGINELOOP_H_INCLUDED

#include "layerClass.h"
#include <memory>

using std::unique_ptr;

#if __linux__
    #include <sys/time.h>
    unsigned long long GetTickCount();
#endif

#define KEY_SEEN     1
#define KEY_RELEASED 2

const int SCREEN_W = 1680;//1920/1.0;
const int SCREEN_H = 900;//1025/1.0;
const float MAX_ZOOM = 1.0;
const int BUFFER_W = SCREEN_W * MAX_ZOOM;
const int BUFFER_H = SCREEN_H * MAX_ZOOM;


void loadFontsToContainer(vector <SingleFont> & FontContainer);
void freeFontsFromContainer(vector <SingleFont> & FontContainer);

vector<string> getAllFilesNamesWithinFolder(string folder);

struct EditorWindowArrangement{
    double windowWidth;
    vec2d buttonSize;
    vec2d margin;
    double buttonMargin;
    string labelFontID;
    string attributeFontID;
    double labelHeight;
    double attributeStart;
    double labelMargin;
    double attributeMargin;
    vec2d attributePadding;
    vec2d attributeSize;
    vec2d editablePadding;

    double containerHeight;
    vec2d wraperSize;
    double wraperMargin;

    double labelMoveAhead;
    double attributeMoveBack;
    double indentation;
    vec2d scrollbarMargin;

    vec2d thumbSize;

    EditorWindowArrangement();
};

void createObjects0(vector <AncestorObject> & Objects, vector <SingleFont> FontContainer, ALLEGRO_DISPLAY * window);
void createObjects(vector <AncestorObject> & Objects, vector <SingleFont> FontContainer, ALLEGRO_DISPLAY * window);
void prepareEditorWindow(vector <AncestorObject> & Objects, string layerID, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer);
void prepareEditorWindowGeneral(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
void removeListsInEditorWindow(AncestorObject * EditorWindow);
char getActiveEditorWindowCategory(AncestorObject * EditorWindow);
void activeEditorWindowCategory(AncestorObject * EditorWindow, char lastActiveID);

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
    // Constructor
    Fps() : m_fps(0), m_fpscount(0){};

    // Update
    void update();

    // Get fps
    unsigned int get() const;
};



//This struct consists of pointers to every object that has at least one event triggerable by the right source  
struct EventsLookupTable{
    vector <AncestorObject*> TimeTriggered;
    vector <AncestorObject*> CameraTriggered;
    vector <AncestorObject*> KeyboardTriggered;
    vector <AncestorObject*> MouseTriggered;
    vector <AncestorObject*> ObjectsTriggered;
    vector <AncestorObject*> VariablesTriggered;
    vector <AncestorObject*> CollisionTriggered;
    vector <AncestorObject*> EditableTextTriggered;
    vector <AncestorObject*> MovementTriggered;
    void clearLookupTable();
};

class EngineLoop{
private:
    ALLEGRO_BITMAP * mainBuffer;
    ALLEGRO_BITMAP * cursorBitmap;
    ALLEGRO_MOUSE_CURSOR * mouseCursor;
    ALLEGRO_MOUSE_STATE mouseState;
    ALLEGRO_TIMER * timer;
    ALLEGRO_EVENT_QUEUE * eventQueue;
    ALLEGRO_EVENT event;
    unsigned char key[ALLEGRO_KEY_MAX];
    string windowTitle;
    bool drawTextFieldBorders;
    bool isPixelArt; //If true, zoomed bitmaps will not look blurry.
    bool ignoreDistantObjects;
    bool drawOnlyVisibleObjects; //If true, engine will not attempt to draw objects outside the camera view. (Allegro 5 is using similar mechanism.)
    bool wasMousePressedInSelectedObject;
    vector <short> pressedKeys;
    vector <short> releasedKeys;
    long timeToInterruptMovement;
    long timeToInterruptParticles;
    int totalNumberOfBitmapLayers;
    string selectedObjectLayerID;
    string selectedObjectID;
    Camera2D * SelectedCamera;
    LayerClass * SelectedLayer;
    AncestorObject * SelectedObject;
    AncestorObject * EditorObject;
    vec2d dragStartingPos;
    vec2d dragCameraStaringPos;
    vector <unsigned int> foregroundOfObjects;
    EventsLookupTable BaseOfTriggerableObjects;

public:
    MouseClass Mouse;
    ALLEGRO_DISPLAY * window;
    bool bootGame, closeGame, closeEditor, isGameActive;
    EngineLoop(string title);
    void initAllegro();
    void exitAllegro();
    void windowLoop(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer, Fps & fps, vector <SingleBitmap> & BitmapContainer);
    void triggerEve(vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void updateTreeOfCamerasFromSelectedRoot(vector <Camera2D> & Cameras, Camera2D * Selected);
    void updateAllForestOfCameras(vector <Camera2D> & Cameras);
    void updateCamerasPositions(vector <Camera2D> & Cameras);
    void focusOnCamera(vector <Camera2D> & Cameras);
    bool isKeyPressed(short key);
    bool isKeyReleased(short key);
    void detectStartPosOfDraggingObjects();
    void drawObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer);
    void drawModules(AncestorObject & Object, unsigned int iteration, Camera2D Cameras, vector <SingleFont> & FontContainer, int currentlyDrawnLayer, int & numberOfDrawnObjects,
                     vector <unsigned int> & foregroundOfObjects, bool isTimeForForeground);
    void updateOtherEvents(vector <AncestorObject> & Objects);
    void detectBackgroundCollisions(LayerClass & Layer, AncestorObject & Object, MovementModule & Movement);
    void detectRealCollisions(LayerClass & Layer, AncestorObject & Object, MovementModule & Movement);
    void adjustAndStopMomentum(AncestorObject & Object, MovementModule & Movement);
    void updateCameraPosition(Camera2D & Cameras, AncestorObject * FollowedByCamera);
    void moveObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void moveParticles(vector <LayerClass> & Layers);
    void moveSelectedObject(vector <EditableTextModule> & EditableTextContainer);
    void triggerEvents(vector <LayerClass> & Layers, short eventsType);
    void updateEditableTextFields(vector <LayerClass> & Layers, vector <SingleBitmap> & BitmapContainer);
    void startTimer();
    void selectObject(vector <LayerClass> & Layers, vector <SingleBitmap> & BitmapContainer, vector <SingleFont> & FontContainer);
    void unselectObject();
    void updateEditorWindowOnSelection(vector <EditableTextModule> & EditableTextContainer);
    void updateEditorWindowOnAxisChange(vector <EditableTextModule> & EditableTextContainer);
    void drawSelectionBorder(Camera2D Camera);
    void startScrollbarDragging(vector <LayerClass> & Layers);
    void dragScrollbars(vector <LayerClass> & Layers);
    void updateBaseOfTriggerableObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void detectTriggeredEvents(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <unique_ptr<AncestorObject>> & TriggeredObjects);
    bool secondHasPassed();

    void prepareEditorWindowObjectsList(int categoryIndex, AncestorObject * EditorWindow, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer, EditorWindowArrangement Arr);
    void prepareEditorWindowImage(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowText(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowMovement(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowCollisions(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowEvents(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowParticles(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowVariables(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
    void prepareEditorWindowEditable(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
};





#endif // ENGINELOOP_H_INCLUDED
