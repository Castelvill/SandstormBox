#ifndef ENGINELOOP_H_INCLUDED
#define ENGINELOOP_H_INCLUDED

#include "layerClass.h"



#if __linux__
    #include <sys/time.h>
    unsigned long long GetTickCount();
#endif

#define KEY_SEEN     1
#define KEY_RELEASED 2

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

struct AncestorIndex{
    unsigned layerIndex;
    unsigned objectIndex;
    AncestorObject * object(vector <LayerClass> & Layers);
};

struct ModuleIndex : AncestorIndex{
    unsigned moduleIndex;
    ModuleIndex(unsigned layer, unsigned object, unsigned module);
    template <class Module>
    Module * module(vector <LayerClass> & Layers);
    vector<EveModule>::iterator module(vector<LayerClass> &Layers);
};

//This struct consists of pointers to every object that has at least one event triggerable by the right source  
struct EventsLookupTable{
    vector <AncestorIndex> BootTriggered; //Triggeered only in the first iteration or in the first iteration after a reboot.
    vector <AncestorIndex> IterationTriggered; //If a trigger is negated or has else statements, in most cases interpreter puts its event into IterationTriggered events. 
    vector <AncestorIndex> TimeTriggered;
    vector <AncestorIndex> KeyPressedTriggered;
    vector <AncestorIndex> KeyPressingTriggered;
    vector <AncestorIndex> KeyReleasedTriggered;
    vector <AncestorIndex> MouseMovedTriggered;
    vector <AncestorIndex> MouseNotMovedTriggered;
    vector <AncestorIndex> MousePressedTriggered;
    vector <AncestorIndex> MousePressingTriggered;
    vector <AncestorIndex> MouseReleasedTriggered;
    vector <AncestorIndex> ObjectsTriggered;
    vector <AncestorIndex> VariablesTriggered;
    vector <AncestorIndex> CollisionTriggered;
    vector <AncestorIndex> EditableTextTriggered;
    vector <AncestorIndex> MovementTriggered;
    vector <AncestorIndex> StillnessTriggered;
    vector <AncestorIndex> ResizeTriggered;
    void clear();
};

struct PointerContainer{
    string ID;
    string type;
    bool readOnly;
    vector <VariableModule> Variables; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> BasePointers;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    PointerContainer();
    void clear();
    void setID(vector<PointerContainer> &EventContext, string newID, const bool & printOutInstructions);
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

    unsigned size() const;
};

struct MemoryStackStruct{
    vector<EveModule>::iterator Event;
    size_t contextSize;
};

struct PointerRecalculator{
    vector<vector<unsigned>> CameraIndexes;
    vector<vector<unsigned>> LayerIndexes;
    vector<vector<AncestorIndex>> ObjectIndexes;
    vector<vector<ModuleIndex>> ModuleIndexes;
    vector<AncestorIndex> TriggeredObjectIndexes;
    vector<ModuleIndex> PastEvents;

    AncestorIndex OtherObjectIndexes[3];

    unsigned selectedLayerIndex = 0;
    unsigned selectedCameraIndex = 0;
    ModuleIndex startingEventIndex = {0, 0, 0};
    ModuleIndex eventIndex = {0, 0, 0};

    void clear();
    void findIndexesForCameras(vector<Camera2D> &Cameras, vector<PointerContainer> & EventContext, Camera2D *& SelectedCamera);
    void findIndexesForLayers(vector<LayerClass> &Layers, vector<PointerContainer> & EventContext);
    void findIndexesForObjects(vector<LayerClass> &Layers, vector<PointerContainer> & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject, AncestorObject *& EditorObject);
    template <class Module>
    ModuleIndex getIndex(Module *& Instance, vector<LayerClass> & Layers);
    ModuleIndex getIndex(vector<EveModule>::iterator & Instance, vector<LayerClass> & Layers);
    template <class Module>
    void findIndexesInModule(vector<Module*> Instances, vector<LayerClass> & Layers);
    void findIndexesForModules(vector<LayerClass> &Layers, vector<PointerContainer> & EventContext, vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack);
    void updatePointersToCameras(vector<Camera2D> &Cameras, vector<PointerContainer> & EventContext, Camera2D *& SelectedCamera);
    void updatePointersToLayers(vector<LayerClass> &Layers, vector<PointerContainer> & EventContext);
    void updatePointersToObjects(vector<LayerClass> &Layers, vector<PointerContainer> & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject, AncestorObject *& EditorObject);
    void updatePointersToModules(vector<LayerClass> &Layers, vector<PointerContainer> &EventContext, vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack);
};

PointerContainer * getContextByID(vector<PointerContainer> & AllContexts, string contextID, bool warning);
void extractPointersFromModules(ModulesPointers & ContextModules, AncestorObject * Object, string moduleType);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity> &Vector);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity*> &Vector);
vector <string> changeCodeIntoWords(string input);

class EngineLoop{
private:
    MouseClass Mouse;
    ALLEGRO_DISPLAY * window;
    ALLEGRO_BITMAP * cursorBitmap;
    ALLEGRO_BITMAP * iconBitmap;
    ALLEGRO_MOUSE_CURSOR * mouseCursor;
    ALLEGRO_MOUSE_STATE mouseState;
    ALLEGRO_TIMER * timer;
    ALLEGRO_EVENT_QUEUE * eventQueue;
    ALLEGRO_EVENT event;
    unsigned char key[ALLEGRO_KEY_MAX];
    bool wasMousePressedInSelectedObject;
    vector <short> pressedKeys;
    vector <short> firstPressedKeys;
    vector <short> releasedKeys;
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
    bool firstIteration, closeProgram, displayResized;

    string windowTitle;
    int windowW;
    int windowH;
    bool fullscreen;
    bool isPixelArt; //If true, zoomed bitmaps will not look blurry.
    
    bool drawCameraBorders;
    bool drawTextFieldBorders;
    bool drawHitboxes; 
    bool ignoreDistantObjects;
    bool drawOnlyVisibleObjects; //If true, engine will not attempt to draw objects outside the camera view. (Allegro 5 is using similar mechanism.)
    int totalNumberOfBitmapLayers;
    bool printOutLogicalEvaluations;
    bool printOutInstructions;
    float reservationMultiplier;

    long timeToInterruptMovement;
    long timeToInterruptParticles;
    
public:
    string EXE_PATH;
    vector <string> layersIDs;
    vector <string> camerasIDs;

    EngineLoop(string title);
    void initAllegro();
    void exitAllegro();
    bool isRunning() const;
    ALLEGRO_DISPLAY * getWindow();
    int getWindowW() const;
    int getWindowH() const;
    vec2i getScreenSize() const;
    bool isLayersUniquenessViolated(vector <LayerClass> Layers);
    bool isCamerasUniquenessViolated(vector <Camera2D> Cameras);
    void windowLoop(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <SingleFont> & FontContainer, Fps & fps, vector <SingleBitmap> & BitmapContainer);
    void aggregateCameras(OperaClass & Operation, PointerContainer & NewContext, vector <Camera2D*> AggregatedCameras, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void aggregateLayers(OperaClass & Operation, PointerContainer & NewVariable, vector <LayerClass*> AggregatedLayers, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void aggregateObjects(OperaClass & Operation, PointerContainer & NewVariable, vector <AncestorObject*> AggregatedObjects, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    //Returns true if the context is of a module type.
    bool chooseRandomModule(PointerContainer & NewContext);
    template<class ModuleClass>
    void aggregateModuleContextFromVectors(vector<ModuleClass*> AggregatedModules, const string & type, OperaClass & Operation, PointerContainer & NewContext,
        AncestorObject * Object, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext
    );
    template<class ModuleClass>
    void findContextInModule(string module, string attribute, PointerContainer & NewContext, ModuleClass * Module);
    template<class ModuleClass>
    void getContextFromModuleVectorById(string module, string moduleID, string attribute, PointerContainer & NewContext, vector <ModuleClass*> AggregatedModules);
    void aggregateModules(OperaClass & Operation, PointerContainer & NewVariable, PointerContainer * OldContext, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    void aggregatePointers(string instruction, PointerContainer & NewContext, vector <BasePointersStruct> & AggregatedPointers);
    void aggregateVariables(string instruction, PointerContainer & NewContext, vector <VariableModule> & AggregatedVariables);
    void findContextInCamera(string attribute, PointerContainer & NewContext, Camera2D * Camera);
    void findContextInLayer(ValueLocation Location, PointerContainer & NewContext, LayerClass * Layer);
    template <class Module>
    void findContextInModuleVector(const ValueLocation & Location, PointerContainer & NewContext, vector<Module> & Source);
    void findContextInObject(ValueLocation Location, PointerContainer & NewContext, AncestorObject * Object);
    bool findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, vector <LayerClass> & Layers);
    void aggregateCamerasAndLayersById(ValueLocation & Location, PointerContainer & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void aggregateModulesById(string moduleType, string moduleID, string attribute, PointerContainer & NewContext, ModulesPointers & AggregatedModules);
    void findLowerContextById(ValueLocation  & Location, PointerContainer & NewContext, PointerContainer * OldContext);
    //Method return true if a pair of contexts of the same type is found.
    bool getPairOfContexts(PointerContainer *& LeftOperand, PointerContainer *& RightOperand, vector<PointerContainer> & AllContexts, vector <string> contextIDs);
    bool getOneContext(PointerContainer *& SelectedContext, vector<PointerContainer> & AllContexts, vector<string> contextIDs);
    bool getAllSelectedContexts(vector<PointerContainer*> & SelectedContexts, vector<PointerContainer> & AllContexts, const vector<string> & contextIDs);
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
    void cloneRightToLeft(vector <Entity*> & LeftOperand, vector <Entity*> & RightOperand, vector<LayerClass> & Layers, const bool & changeOldID);
    void moveValues(OperaClass & Operation, vector<PointerContainer> &EventContext);
    void cloneEntities(vector<string> dynamicIDs, bool changeOldID, vector<PointerContainer> &EventContext, vector<LayerClass> &Layers);
    void executeArithmetics(OperaClass & Operation, vector<PointerContainer> &EventContext);
    void generateRandomVariable(vector<PointerContainer> &EventContext, const OperaClass & Operation);
    void createLiteral(vector<PointerContainer> &EventContext, const OperaClass & Operation);
    void checkIfVectorContainsVector(OperaClass & Operation, vector<PointerContainer> &EventContext);
    bool prepareVectorSizeAndIDsForNew(vector<PointerContainer> & EventContext, const vector<string> & dynamicIDs, const vector<VariableModule> & Literals, unsigned & newVectorSize, vector <string> & newIDs);
    bool prepareDestinationForNew(OperaClass & Operation, vector<PointerContainer> & EventContext, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, string & layerID, string & objectID, vector<LayerClass> &Layers);
    void createNewEntities(OperaClass & Operation, vector<PointerContainer> & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector<LayerClass> &Layers, vector<Camera2D> &Cameras, vector <AncestorObject*> & TriggeredObjects,
        vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, bool & wasNewExecuted
    );
    void markEntitiesForDeletion(OperaClass & Operation, vector<PointerContainer> & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, bool & wasDeleteExecuted
    );
    void getIndexes(const vector<VariableModule> & Literals, const vector<string> & dynamicIDs, vector<unsigned> & indexes, vector<PointerContainer> & EventContext);
    void getReferenceByIndex(OperaClass & Operation, vector<PointerContainer> & EventContext, vector<LayerClass> &Layers, vector<Camera2D> &Cameras);
    void bindFilesToObjects(OperaClass & Operation, vector<PointerContainer> & EventContext);
    void buildEventsInObjects(OperaClass & Operation, vector<PointerContainer> & EventContext, bool & wasBuildExecuted);
    void executeFunction(OperaClass & Operation, vector<PointerContainer> & EventContext, vector<EveModule>::iterator & Event, vector<LayerClass> &Layers,
        vector<Camera2D> &Cameras, vector<SingleBitmap> & BitmapContainer, const vector<SingleFont> & FontContainer
    );
    void changeEngineVariables(OperaClass & Operation);
    void loadBitmap(OperaClass & Operation, vector<SingleBitmap> & BitmapContainer);
    void createDirectory(OperaClass & Operation);
    void removeFileOrDirectory(OperaClass & Operation);
    void removeRecursivelyFileOrDirectory(OperaClass & Operation);
    void renameFileOrDirectory(OperaClass & Operation);
    OperaClass executeOperations(vector<OperaClass> Operations, LayerClass *& OwnerLayer, AncestorObject *& Owner,
        vector <PointerContainer> & EventContext, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector <AncestorObject*> & TriggeredObjects,
        vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, bool & wasDeleteExecuted,
        bool & wasNewExecuted, bool & wasBuildExecuted, vector<SingleBitmap> & BitmapContainer, const vector<SingleFont> & FontContainer
    );
    VariableModule findNextValueInMovementModule(ConditionClass & Condition, AncestorObject * CurrentObject);
    VariableModule getValueFromObjectInCamera(AncestorObject * CurrentObject, vector <Camera2D> & Cameras, const string & attribute, const string & cameraID);
    VariableModule getValueFromMouseClickingObject(AncestorObject * CurrentObject, const string & attribute, const short & button);
    VariableModule getValueFromObjectInCollision(ConditionClass &Condition, AncestorObject * CurrentObject, LayerClass * CurrentLayer);
    VariableModule findNextValueAmongObjects(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    VariableModule findNextValue(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    char evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, AncestorObject * Owner, LayerClass * OwnerLayer, vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<PointerContainer> &EventContext);
    vector<EveModule>::iterator FindUnfinishedEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event);
    vector<EveModule>::iterator FindElseEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event);
    bool deleteEntities(vector <LayerClass> & Layers, vector <Camera2D> & Cameras);
    void resetChildren(vector<EveModule>::iterator & Event, AncestorObject * Triggered);
    void triggerEve(vector <LayerClass> & Layers, vector <Camera2D> & Cameras, vector<SingleBitmap> & BitmapContainer, const vector<SingleFont> & FontContainer);
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
    void updateBaseOfTriggerableObjects(vector <LayerClass> & Layers);
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
