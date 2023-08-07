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

const int SCREEN_W = 1680*0.7;//1920/1.0;
const int SCREEN_H = 900*0.7;//1025/1.0;
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


vector <short> getPressedKeys(unsigned char key[]);
vector <short> getReleasedKeys(unsigned char key[], vector <short> pressedKeys);

//This struct consists of pointers to every object that has at least one event triggerable by the right source  
struct EventsLookupTable{
    vector <AncestorObject*> IterationTriggered; //If a trigger is negated or has else statements, in most cases interpreter puts its event into IterationTriggered events. 
    vector <AncestorObject*> TimeTriggered;
    vector <AncestorObject*> KeyPressedTriggered;
    vector <AncestorObject*> KeyPressingTriggered;
    vector <AncestorObject*> KeyReleasedTriggered;
    vector <AncestorObject*> MouseMovedTriggered;
    vector <AncestorObject*> MouseNotMovedTriggered;
    vector <AncestorObject*> MousePressedTriggered;
    vector <AncestorObject*> MousePressingTriggered;
    vector <AncestorObject*> MouseReleasedTriggered;
    vector <AncestorObject*> ObjectsTriggered;
    vector <AncestorObject*> VariablesTriggered;
    vector <AncestorObject*> CollisionTriggered;
    vector <AncestorObject*> EditableTextTriggered;
    vector <AncestorObject*> MovementTriggered;
    vector <AncestorObject*> StillnessTriggered;
    void clear();
};

struct PointerContainer{
    string ID;
    string type;
    vector <VariableModule> NormalVariables; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> UniversalVariables;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    PointerContainer();
    void clear();
    void addUniversalVariable(bool*);
    void addUniversalVariable(short*);
    void addUniversalVariable(unsigned short*);
    void addUniversalVariable(int*);
    void addUniversalVariable(unsigned int*);
    void addUniversalVariable(double*);
    void addUniversalVariable(string*);

    void setFirstUniversalVariable(bool*);
    void setFirstUniversalVariable(short*);
    void setFirstUniversalVariable(unsigned short*);
    void setFirstUniversalVariable(int*);
    void setFirstUniversalVariable(unsigned int*);
    void setFirstUniversalVariable(double*);
    void setFirstUniversalVariable(string*);

    void addModule(TextModule * Module);
    void addModule(EditableTextModule * Module);
    void addModule(ImageModule * Module);
    void addModule(MovementModule * Module);
    void addModule(CollisionModule * Module);
    void addModule(ParticleEffectModule * Module);
    void addModule(EveModule * Module);
    void addModule(VariableModule * Module);
    void addModule(ScrollbarModule * Module);

    void setFirstModule(TextModule * Module);
    void setFirstModule(EditableTextModule * Module);
    void setFirstModule(ImageModule * Module);
    void setFirstModule(MovementModule * Module);
    void setFirstModule(CollisionModule * Module);
    void setFirstModule(ParticleEffectModule * Module);
    void setFirstModule(EveModule * Module);
    void setFirstModule(VariableModule * Module);
    void setFirstModule(ScrollbarModule * Module);

    void leaveOneRandomUniversalVariable();
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
    bool drawHitboxes;
    bool isPixelArt; //If true, zoomed bitmaps will not look blurry.
    bool ignoreDistantObjects;
    bool drawOnlyVisibleObjects; //If true, engine will not attempt to draw objects outside the camera view. (Allegro 5 is using similar mechanism.)
    bool wasMousePressedInSelectedObject;
    vector <short> pressedKeys;
    vector <short> firstPressedKeys;
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
    vector <string> layersIDs;
    vector <string> camerasIDs;
    EngineLoop(string title);
    void initAllegro();
    void exitAllegro();
    bool createListOfUniqueIDsOfLayers(vector <LayerClass> & Layers);
    bool createListOfUniqueIDsOfCameras(vector <Camera2D> & Cameras);
    void windowLoop(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer, Fps & fps, vector <SingleBitmap> & BitmapContainer);
    void aggregateCameras(OperaClass & Operation, PointerContainer & NewContext, vector <Camera2D*> AggregatedCameras, vector <Camera2D> & Cameras);
    void aggregateLayers(OperaClass & Operation, PointerContainer & NewVariable, vector <LayerClass*> AggregatedLayers, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void aggregateObjects(OperaClass & Operation, PointerContainer & NewVariable, vector <LayerClass*> AggregatedLayers, vector <AncestorObject*> AggregatedObjects, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void chooseRandomModule(OperaClass & Operation, PointerContainer & NewContext);
    void aggregateModules(OperaClass & Operation, PointerContainer & NewVariable, vector <AncestorObject*> AggregatedObjects, ModulesPointers AggregatedModules, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void aggregateAttributes(string instruction, PointerContainer & NewContext, vector <BasePointersStruct> AggregatedAttributes);
    void findLastContextInCamera(string attribute, PointerContainer & NewContext, Camera2D * Camera);
    void findContextInCamera(string attribute, PointerContainer & NewContext, Camera2D * Camera);
    void findLastContextInLayer(string attribute, PointerContainer & NewContext, LayerClass * Layer);
    void findContextInLayer(string attribute, PointerContainer & NewContext, LayerClass * Layer);
    void findLastContextInTheAncestor(string attribute, PointerContainer & NewContext, AncestorObject * Object);
    void findContextInTheAncestor(string attribute, PointerContainer & NewContext, AncestorObject * Object);
    bool findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer, LayerClass * CurrentLayer, AncestorObject * CurrentObject, vector <LayerClass> & Layers);
    void findContextInOneObject(ValueLocation & Location, PointerContainer & NewContext, AncestorObject * CurrentObject);
    void findContextInObjects(ValueLocation & Location, PointerContainer & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers);
    void findContextInEnv(TriggerClass & Location, PointerContainer & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void findContextInOneModule(string moduleType, string moduleID, string attribute, PointerContainer & NewContext, ModulesPointers & AggregatedModules);
    void findLowerContextInObjects(ValueLocation & Location, PointerContainer & NewContext, PointerContainer * OldContext);
    void findLowerContext(TriggerClass & Location, PointerContainer & NewContext, PointerContainer * OldContext);
    PointerContainer * getContextByID(vector<PointerContainer> & AllContexts, string contextID);
    //Method return true if a pair of contexts of the same type is found.
    bool getPairOfContexts(PointerContainer * LeftOperand, PointerContainer * RightOperand, vector<PointerContainer> & AllContexts, vector <string> contextIDs);
    void aggregateTwoSets(OperaClass & Operation, PointerContainer & NewContext, vector<PointerContainer> & EventContext);
    void aggregateEntities(OperaClass & Operation, PointerContainer & NewContext, vector<PointerContainer> & EventContext, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void aggregateValues(PointerContainer & NewContext, vector<PointerContainer> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void aggregateOnlyById(PointerContainer & NewContext, vector<PointerContainer> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void nameVariable(vector<PointerContainer> & EventContext, OperaClass & Operation);
    OperaClass executeOperations(vector<OperaClass> Operations, LayerClass * OwnerLayer, AncestorObject * Owner, vector <PointerContainer> & EventContext, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    VariableModule findNextValueInMovementModule(TriggerClass & Condition, AncestorObject * CurrentObject);
    VariableModule findNextValueAmongObjects(TriggerClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    VariableModule findNextValue(TriggerClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    char evaluateConditionalChain(vector<TriggerClass> & ConditionalChain, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    vector<EveModule>::iterator FindUnfinishedEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event);
    vector<EveModule>::iterator FindElseEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event);
    void triggerEve(vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void updateTreeOfCamerasFromSelectedRoot(vector <Camera2D> & Cameras, Camera2D * Selected);
    void updateAllForestOfCameras(vector <Camera2D> & Cameras);
    void updateCamerasPositions(vector <Camera2D> & Cameras);
    void focusOnCamera(vector <Camera2D> & Cameras);
    bool isKeyFirstPressed(short key);
    bool isKeyPressed(short key);
    bool isKeyReleased(short key);
    void detectStartPosOfDraggingObjects();
    void drawObjects(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer);
    void drawModules(AncestorObject & Object, unsigned int iteration, Camera2D & Cameras, vector <SingleFont> & FontContainer, int currentlyDrawnLayer, int & numberOfDrawnObjects,
                     vector <unsigned int> & foregroundOfObjects, bool isTimeForForeground);
    void detectBackgroundCollisions(LayerClass & Layer, AncestorObject & Object, vec2d momentum);
    bool shouldCheckOverlapingOnly(CollisionModule & Collision, AncestorObject & SolidObject, CollisionModule & SolidCollision);
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
    void detectTriggeredEvents(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <AncestorObject*> & TriggeredObjects);
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

    template<class ModuleClass>
    void aggregateModuleContextFromVectors(
        vector<ModuleClass> & ModuleContainer, vector<ModuleClass*> AggregatedModules, OperaClass & Operation,
        PointerContainer & NewContext, AncestorObject * Object, vector <LayerClass> & Layers, vector <Camera2D> & Cameras
    );

    template<class ModuleClass>
    void getContextFromModule(string module, string attribute, PointerContainer & NewContext, ModuleClass * Module);

    template<class ModuleClass>
    void getContextFromModuleVector(string module, string moduleID, string attribute, PointerContainer & NewContext, vector <ModuleClass> * ModuleVector, vector <ModuleClass*> AggregatedModules);

    template<class ModuleClass>
    void findLastContextInModules(string module, string attribute, PointerContainer & NewContext, ModuleClass * Module);

    template<class Entity>
    void executeOperationsOnSets(string instruction, vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand);
};






#endif // ENGINELOOP_H_INCLUDED
