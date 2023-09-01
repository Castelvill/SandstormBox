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
const bool printOutLogicalEvaluations = false;
const bool printInstructions = true;


void loadFontsToContainer(vector <SingleFont> & FontContainer);
void freeFontsFromContainer(vector <SingleFont> & FontContainer);

vector<string> getAllFilesNamesWithinFolder(string folder);

template <class T>
void getPointersFromVector(vector <T*> & Left, vector<T> & Right);

bool checkForVectorSize(size_t leftSize, size_t rightSize, bool & sameSize, string functionName);

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

void createObjects0(vector <AncestorObject> & Objects, string layerID, vector <string> & listOfUniqueIDs, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer, ALLEGRO_DISPLAY * window);
void createObjects(vector <AncestorObject> & Objects, string layerID, vector <string> & listOfUniqueIDs, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer, ALLEGRO_DISPLAY * window);
void prepareEditorWindow(vector <AncestorObject> & Objects, string layerID, vector<string> &listOfIDs, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer);
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
    vector <VariableModule> Variables; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> BasePointers;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    PointerContainer();
    void clear();
    void setID(vector<PointerContainer> &EventContext, string newID);
    string getValue();
    template<typename T>
    void addBasePointer(T * pointer);
    template<typename T>
    void setFirstBasePointer(T * pointer);
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

    void leaveOneRandomBasePointer();
};

PointerContainer * getContextByID(vector<PointerContainer> & AllContexts, string contextID, bool warning);
void extractPointersFromModules(ModulesPointers & ContextModules, AncestorObject * Object, string moduleType);

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
    bool isLayersUniquenessViolated(vector <LayerClass> Layers);
    bool isCamerasUniquenessViolated(vector <Camera2D> Cameras);
    void windowLoop(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer, Fps & fps, vector <SingleBitmap> & BitmapContainer);
    void aggregateCameras(OperaClass & Operation, PointerContainer & NewContext, vector <Camera2D*> AggregatedCameras, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void aggregateLayers(OperaClass & Operation, PointerContainer & NewVariable, vector <LayerClass*> AggregatedLayers, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void aggregateObjects(OperaClass & Operation, PointerContainer & NewVariable, vector <LayerClass*> AggregatedLayers, vector <AncestorObject*> AggregatedObjects, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void chooseRandomModule(string source, PointerContainer & NewContext);
    template<class ModuleClass>
    void aggregateModuleContextFromVectors(
        vector<ModuleClass> & ModuleContainer, vector<ModuleClass*> AggregatedModules, OperaClass & Operation,
        PointerContainer & NewContext, AncestorObject * Object, vector <LayerClass> & Layers, vector <Camera2D> & Cameras,
        vector<PointerContainer> &EventContext
    );
    template<class ModuleClass>
    void getContextFromModule(string module, string attribute, PointerContainer & NewContext, ModuleClass * Module);
    template<class ModuleClass>
    void getContextFromModuleVector(string module, string moduleID, string attribute, PointerContainer & NewContext, vector <ModuleClass> * ModuleVector, vector <ModuleClass*> AggregatedModules);
    template<class ModuleClass>
    void findLastContextInModules(string module, string attribute, PointerContainer & NewContext, ModuleClass * Module);
    void aggregateModules(OperaClass & Operation, PointerContainer & NewVariable, vector <AncestorObject*> AggregatedObjects, ModulesPointers AggregatedModules, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void aggregateAttributes(string instruction, PointerContainer & NewContext, vector <BasePointersStruct> AggregatedAttributes);
    void findLastContextInCamera(string attribute, PointerContainer & NewContext, Camera2D * Camera);
    void findContextInCamera(string attribute, PointerContainer & NewContext, Camera2D * Camera);
    void findLastContextInLayer(string attribute, PointerContainer & NewContext, LayerClass * Layer);
    void findContextInLayer(ValueLocation Location, PointerContainer & NewContext, LayerClass * Layer);
    void findContextInLayer(string attribute, PointerContainer & NewContext, LayerClass * Layer);
    void findLastContextInTheAncestor(string attribute, PointerContainer & NewContext, AncestorObject * Object);
    void findContextInTheAncestor(string attribute, PointerContainer & NewContext, AncestorObject * Object);
    bool findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, vector <LayerClass> & Layers);
    void findContextInOneObject(ValueLocation & Location, PointerContainer & NewContext, AncestorObject * CurrentObject);
    void findContextInObjects(ValueLocation & Location, PointerContainer & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers);
    void findContextInEnv(ValueLocation & Location, PointerContainer & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void findContextInOneModule(string moduleType, string moduleID, string attribute, PointerContainer & NewContext, ModulesPointers & AggregatedModules);
    void findLowerContextInObjects(ValueLocation & Location, PointerContainer & NewContext, PointerContainer * OldContext);
    void findLowerContext(ValueLocation  & Location, PointerContainer & NewContext, PointerContainer * OldContext);
    //Method return true if a pair of contexts of the same type is found.
    bool getPairOfContexts(PointerContainer *& LeftOperand, PointerContainer *& RightOperand, vector<PointerContainer> & AllContexts, vector <string> contextIDs);
    bool getOneContext(PointerContainer *& LeftOperand, vector<PointerContainer> & AllContexts, vector<string> contextIDs);
    template<class Entity>
    void executeOperationsOnSets(string instruction, vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand);
    template<class Entity>
    void executeOperationsOnSets(string instruction, vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand);
    void aggregateTwoSets(OperaClass & Operation, vector<PointerContainer> & EventContext);
    void addNewContext(vector<PointerContainer> & EventContext, const PointerContainer & NewContext, string type, string newID);
    void aggregateEntities(OperaClass & Operation, vector<PointerContainer> & EventContext, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void aggregateValues(vector<PointerContainer> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void aggregateOnlyById(vector<PointerContainer> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void nameVariable(vector<PointerContainer> & EventContext, OperaClass & Operation);
    template<class Entity>
    void cloneRightToLeft(vector <Entity*> & LeftOperand, vector <Entity*> & RightOperand, vector<LayerClass> & Layers);
    void moveValues(OperaClass & Operation, vector<PointerContainer> &EventContext);
    void cloneEntities(vector<string> dynamicIDs, vector<PointerContainer> &EventContext, vector<LayerClass> &Layers);
    void executeArithmetics(OperaClass & Operation, vector<PointerContainer> &EventContext);
    void generateRandomVariable(vector<PointerContainer> &EventContext, const OperaClass & Operation);
    void createLiteral(vector<PointerContainer> &EventContext, const OperaClass & Operation);
    void checkIfVectorContainsVector(OperaClass & Operation, vector<PointerContainer> &EventContext);
    void createNewEntities(OperaClass & Operation, vector<PointerContainer> & EventContext, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    OperaClass executeOperations(vector<OperaClass> Operations, LayerClass * OwnerLayer, AncestorObject * Owner, vector <PointerContainer> & EventContext, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    VariableModule findNextValueInMovementModule(ConditionClass & Condition, AncestorObject * CurrentObject);
    VariableModule getValueFromObjectInCamera(AncestorObject * CurrentObject, vector <Camera2D> & Cameras, const string & attribute, const string & cameraID);
    VariableModule getValueFromMouseClickingObject(AncestorObject * CurrentObject, const string & attribute, const short & button);
    VariableModule getValueFromObjectInCollision(AncestorObject * CurrentObject, LayerClass * CurrentLayer, const ValueLocation & Location);
    VariableModule findNextValueAmongObjects(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    VariableModule findNextValue(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    char evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
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
    void moveSelectedObject();
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

    bool checkDefaultCondition(VariableModule * Left, VariableModule * Right);
    bool checkDefaultCondition(BasePointersStruct * Left, BasePointersStruct * Right);
};






#endif // ENGINELOOP_H_INCLUDED
