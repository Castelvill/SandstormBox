#ifndef PROCESSCLASS_H_INCLUDED
#define PROCESSCLASS_H_INCLUDED

#include "engineClass.h"

#if __linux__
    #include <sys/time.h>
    unsigned long long GetTickCount();
#endif

enum CAMERA_MOVE: unsigned char{NONE, CAMERA_FULL, CAMERA_N, CAMERA_NE, CAMERA_E, CAMERA_SE, CAMERA_S, CAMERA_SW, CAMERA_W, CAMERA_NW};

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

void prepareEditorWindow(vector <AncestorObject> & Objects, string layerID, vector<string> &listOfIDs, vector <SingleFont> FontContainer, vector <SingleBitmap> & BitmapContainer);
void prepareEditorWindowGeneral(AncestorObject *, vector <SingleFont>, vector <SingleBitmap> &, EditorWindowArrangement);
void removeListsInEditorWindow(AncestorObject * EditorWindow);
char getActiveEditorWindowCategory(AncestorObject * EditorWindow);


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
    vector <AncestorIndex> BootTriggered; //Triggered only in the first iteration or in the first iteration after a reboot.
    vector <AncestorIndex> InitTriggered; //Triggered only when the object is created.
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

struct ContextClass{
    string ID;
    string type;
    bool readOnly;
    vector <VariableModule> Variables; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> BasePointers;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    ContextClass();
    void clear();
    void setID(vector<ContextClass> &EventContext, string newID, const bool & printOutInstructions);
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
    void addModule(PrimitivesModule * Module);

    void setFirstModule(TextModule * Module);
    void setFirstModule(EditableTextModule * Module);
    void setFirstModule(ImageModule * Module);
    void setFirstModule(MovementModule * Module);
    void setFirstModule(CollisionModule * Module);
    void setFirstModule(ParticleEffectModule * Module);
    void setFirstModule(EveModule * Module);
    void setFirstModule(VariableModule * Module);
    void setFirstModule(ScrollbarModule * Module);
    void setFirstModule(PrimitivesModule * Module);

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
    void findIndexesForCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext, Camera2D *& SelectedCamera);
    void findIndexesForLayers(vector<LayerClass> &Layers, vector<ContextClass> & EventContext);
    void findIndexesForObjects(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject, AncestorObject *& EditorObject);
    template <class Module>
    ModuleIndex getIndex(Module *& Instance, vector<LayerClass> & Layers);
    ModuleIndex getIndex(vector<EveModule>::iterator & Instance, vector<LayerClass> & Layers);
    template <class Module>
    void findIndexesInModule(vector<Module*> Instances, vector<LayerClass> & Layers);
    void findIndexesForModules(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack);
    void updatePointersToCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext, Camera2D *& SelectedCamera);
    void updatePointersToLayers(vector<LayerClass> &Layers, vector<ContextClass> & EventContext);
    void updatePointersToObjects(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject, AncestorObject *& EditorObject);
    void updatePointersToModules(vector<LayerClass> &Layers, vector<ContextClass> &EventContext, vector<EveModule>::iterator & StartingEvent, vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack);
};

ContextClass * getContextByID(vector<ContextClass> & AllContexts, string contextID, bool warning);
void extractPointersFromModules(ModulesPointers & ContextModules, AncestorObject * Object, string moduleType);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity> &Vector);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity*> &Vector);
vector <string> changeCodeIntoWords(string input);

class ProcessClass{
private:
    string ID;
    bool isActive;
    bool canInteractWithUser; 
    bool isRendering;
    ALLEGRO_BITMAP * WindowBuffer;
    vec2d windowPos, windowSize, minWindowSize;
    float windowTint[4];

    string EXE_PATH;
    
    vector <LayerClass> Layers;
    vector <Camera2D> Cameras;
    vector <string> layersIDs;
    vector <string> camerasIDs;

    string selectedObjectLayerID;
    string selectedObjectID;
    Camera2D * SelectedCamera;
    LayerClass * SelectedLayer;
    AncestorObject * SelectedObject;
    AncestorObject * EditorObject;
    vec2d dragStartingPos, dragStartingPos2;
    vec2d dragCameraStaringPos, dragLimit;
    vector <unsigned int> foregroundOfObjects;
    EventsLookupTable BaseOfTriggerableObjects;
    bool firstIteration, rebooted;
    bool wasDeleteExecuted, wasNewExecuted, wasBuildExecuted;

    bool wasMousePressedInSelectedObject;
    CAMERA_MOVE activeCameraMoveType;
    
    bool drawCameraBorders;
    bool drawTextFieldBorders;
    bool drawHitboxes; 
    bool ignoreDistantObjects;
    bool drawOnlyVisibleObjects; //If true, engine will not attempt to draw objects outside the camera view. (Allegro 5 is using similar mechanism.)
    int totalNumberOfBitmapLayers;
    bool printOutLogicalEvaluations;
    bool printOutInstructions;
    bool printOutStackAutomatically;
    float reservationMultiplier;

    long timeToInterruptMovement;
    long timeToInterruptParticles;
    
public:
    vector <unsigned> camerasOrder;

    void loadInitProcess(string EXE_PATH_FROM_ENGINE, vec2i screenSize, string initFilePath);
    ProcessClass(string EXE_PATH_FROM_ENGINE, vec2i screenSize, string initFilePath);
    ~ProcessClass();
    void clear();
    void resizeWindow(vec2d newSize);
    void resizeWindow(double x, double y);
    bool isLayersUniquenessViolated();
    bool isCamerasUniquenessViolated();
    void executeIteration(EngineClass & Engine, vector<ProcessClass> & Processes);
    void renderOnDisplay(EngineClass & Engine);
    void aggregateCameras(OperaClass & Operation, ContextClass & NewContext, vector <Camera2D*> AggregatedCameras,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    void aggregateLayers(OperaClass & Operation, ContextClass & NewVariable, vector <LayerClass*> AggregatedLayers,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    void aggregateObjects(OperaClass & Operation, ContextClass & NewVariable, vector <AncestorObject*> AggregatedObjects,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    //Returns true if the context is of a module type.
    bool chooseRandomModule(ContextClass & NewContext);
    template<class ModuleClass>
    void aggregateModuleContextFromVectors(vector<ModuleClass*> AggregatedModules, const string & type, OperaClass & Operation, ContextClass & NewContext,
        AncestorObject * Object, const EngineClass & Engine, vector<ContextClass> &EventContext
    );
    template<class ModuleClass>
    void findContextInModule(string module, string attribute, ContextClass & NewContext, ModuleClass * Module);
    template<class ModuleClass>
    void getContextFromModuleVectorById(string module, string moduleID, string attribute, ContextClass & NewContext, vector <ModuleClass*> AggregatedModules);
    void aggregateModules(OperaClass & Operation, ContextClass & NewVariable, ContextClass * OldContext, vector<ContextClass> &EventContext, const EngineClass & Engine);
    void aggregatePointers(string instruction, ContextClass & NewContext, vector <BasePointersStruct> & AggregatedPointers);
    void aggregateVariables(string instruction, ContextClass & NewContext, vector <VariableModule> & AggregatedVariables);
    void findContextInCamera(string attribute, ContextClass & NewContext, Camera2D * Camera);
    void findContextInLayer(ValueLocation Location, ContextClass & NewContext, LayerClass * Layer);
    template <class Module>
    void findContextInModuleVector(const ValueLocation & Location, ContextClass & NewContext, vector<Module> & Source);
    void findContextInObject(ValueLocation Location, ContextClass & NewContext, AncestorObject * Object);
    bool findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject);
    void aggregateCamerasAndLayersById(ValueLocation & Location, ContextClass & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer);
    void aggregateModulesById(string moduleType, string moduleID, string attribute, ContextClass & NewContext, ModulesPointers & AggregatedModules);
    void findLowerContextById(ValueLocation  & Location, ContextClass & NewContext, ContextClass * OldContext);
    //Method return true if a pair of contexts of the same type is found.
    bool getPairOfContexts(ContextClass *& LeftOperand, ContextClass *& RightOperand, vector<ContextClass> & AllContexts, vector <string> contextIDs);
    bool getOneContext(ContextClass *& SelectedContext, vector<ContextClass> & AllContexts, vector<string> contextIDs);
    bool getAllSelectedContexts(vector<ContextClass*> & SelectedContexts, vector<ContextClass> & AllContexts, const vector<string> & contextIDs);
    template<class Entity>
    void executeOperationsOnSets(string instruction, vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand);
    template<class Entity>
    void executeOperationsOnSets(string instruction, vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand);
    void aggregateTwoSets(OperaClass & Operation, vector<ContextClass> & EventContext);
    void addNewContext(vector<ContextClass> & EventContext, const ContextClass & NewContext, string type, string newID);
    void aggregateEntities(OperaClass & Operation, vector<ContextClass> & EventContext, const EngineClass & Engine);
    void aggregateValues(vector<ContextClass> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer,
        AncestorObject *Owner, const EngineClass & Engine);
    void aggregateOnlyById(vector<ContextClass> &EventContext, OperaClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner);
    void nameVariable(vector<ContextClass> & EventContext, OperaClass & Operation);
    template<class Entity>
    void cloneRightToLeft(vector <Entity*> & LeftOperand, vector <Entity*> & RightOperand, vector<LayerClass> & Layers, const bool & changeOldID);
    void moveValues(OperaClass & Operation, vector<ContextClass> &EventContext);
    void cloneEntities(vector<string> dynamicIDs, bool changeOldID, vector<ContextClass> &EventContext, vector<LayerClass> &Layers);
    void executeArithmetics(OperaClass & Operation, vector<ContextClass> &EventContext);
    void generateRandomVariable(vector<ContextClass> &EventContext, const OperaClass & Operation);
    void createLiteral(vector<ContextClass> &EventContext, const OperaClass & Operation);
    void checkIfVectorContainsVector(OperaClass & Operation, vector<ContextClass> &EventContext);
    bool prepareVectorSizeAndIDsForNew(vector<ContextClass> & EventContext, const vector<string> & dynamicIDs, const vector<VariableModule> & Literals, unsigned & newVectorSize, vector <string> & newIDs);
    bool prepareDestinationForNew(OperaClass & Operation, vector<ContextClass> & EventContext, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, string & layerID, string & objectID, vector<LayerClass> &Layers);
    void createNewEntities(OperaClass & Operation, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EveModule>::iterator & StartingEvent,
        vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
    );
    void markEntitiesForDeletion(OperaClass & Operation, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects
    );
    void getIndexes(const vector<VariableModule> & Literals, const vector<string> & dynamicIDs, vector<unsigned> & indexes, vector<ContextClass> & EventContext);
    void getReferenceByIndex(OperaClass & Operation, vector<ContextClass> & EventContext);
    void bindFilesToObjects(OperaClass & Operation, vector<ContextClass> & EventContext);
    void buildEventsInObjects(OperaClass & Operation, vector<ContextClass> & EventContext);
    void executeFunctionForCameras(OperaClass & Operation, vector <VariableModule> & Variables, vector<Camera2D*> CamerasFromContext);
    void executeFunctionForLayers(OperaClass & Operation, vector <VariableModule> & Variables, vector<LayerClass*> & Layers);
    void executeFunctionForObjects(OperaClass & Operation, vector <VariableModule> & Variables, vector<AncestorObject*> & Objects);
    void executeFunction(OperaClass Operation, vector<ContextClass> & EventContext, vector<EveModule>::iterator & Event, EngineClass & Engine);
    void changeEngineVariables(OperaClass & Operation, EngineClass & Engine);
    void changeProcessVariables(OperaClass & Operation);
    void loadBitmap(OperaClass & Operation, vector<SingleBitmap> & BitmapContainer);
    void createDirectory(OperaClass & Operation);
    void removeFileOrDirectory(OperaClass & Operation);
    void removeRecursivelyFileOrDirectory(OperaClass & Operation);
    void renameFileOrDirectory(OperaClass & Operation);
    void executePrint(OperaClass & Operation, vector<ContextClass> & EventContext);
    void saveStringAsFile(OperaClass & Operation, vector<ContextClass> & EventContext);
    void loadFileAsString(OperaClass & Operation, vector<ContextClass> & EventContext);
    OperaClass executeInstructions(vector<OperaClass> Operations, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector<ContextClass> & EventContext, vector<AncestorObject*> & TriggeredObjects,
        vector<ProcessClass> & Processes, vector<EveModule>::iterator & StartingEvent,
        vector<EveModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, EngineClass & Engine
    );
    VariableModule findNextValueInMovementModule(ConditionClass & Condition, AncestorObject * CurrentObject);
    VariableModule getValueFromObjectInCamera(AncestorObject * CurrentObject,
        const string & attribute, const string & cameraID);
    VariableModule getValueFromMouseClickingObject(const MouseClass & Mouse, AncestorObject * CurrentObject,
        const string & attribute, const short & button);
    VariableModule getValueFromObjectInCollision(ConditionClass &Condition, AncestorObject * CurrentObject, LayerClass * CurrentLayer);
    VariableModule findNextValueAmongObjects(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer,
        const MouseClass & Mouse);
    VariableModule findNextValue(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    char evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, AncestorObject * Owner, LayerClass * OwnerLayer,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    vector<EveModule>::iterator FindUnfinishedEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event);
    vector<EveModule>::iterator FindElseEvent(AncestorObject * Triggered, vector<EveModule>::iterator & Event);
    bool deleteEntities();
    void resetChildren(vector<EveModule>::iterator & Event, AncestorObject * Triggered);
    void triggerEve(EngineClass & Engine, vector<ProcessClass> & Processes);
    void updateTreeOfCamerasFromSelectedRoot(Camera2D * Selected);
    void updateWholeForestOfCameras();
    void adjustPositionOfAllCameras();
    void keepPositionInsideScreen(vec2d & pos, vec2d & size, vec2i displaySize);
    void updateCamerasPositions(const EngineClass & Engine);
    void bringCameraForward(unsigned index, Camera2D * ChosenCamera);
    void selectCamera(bool fromAltTab, const MouseClass & Mouse, const vector<short> & pressedKeys, const vector<short> & releasedKeys);
    bool isKeyFirstPressed(short key, vector <short> firstPressedKeys);
    bool isKeyPressed(short key, vector <short> pressedKeys);
    bool isKeyReleased(short key, vector <short> releasedKeys);
    void detectStartPosOfDraggingObjects(const MouseClass & Mouse);
    void changeCursor(ALLEGRO_DISPLAY *display, const MouseClass & Mouse);
    void detectStartPosOfDraggingCamera(ALLEGRO_DISPLAY *display, const MouseClass & Mouse);
    void drawEverything(EngineClass & Engine);
    void drawModules(AncestorObject & Object, unsigned int iteration, Camera2D & Cameras, vector <SingleFont> & FontContainer,
        int currentlyDrawnLayer, int & numberOfDrawnObjects, vector <unsigned int> & foregroundOfObjects,
        bool isTimeForForeground, vec2i displaySize);
    void detectBackgroundCollisions(LayerClass & Layer, AncestorObject & Object, vec2d momentum);
    bool shouldCheckOverlapingOnly(CollisionModule & Collision, AncestorObject & SolidObject, CollisionModule & SolidCollision);
    void detectRealCollisions(LayerClass & Layer, AncestorObject & Object, MovementModule & Movement);
    void adjustAndStopMomentum(AncestorObject & Object, MovementModule & Movement);
    void updateCameraPosition(Camera2D & Cameras, AncestorObject * FollowedByCamera);
    void moveObjects(const vector<short> & pressedKeys, const MouseClass & Mouse);
    void moveParticles(const vector<short> & pressedKeys, const vector<short> & releasedKeys);
    void moveSelectedObject(const MouseClass & Mouse);
    void delayEditableTextFields();
    void updateEditableTextFields(EngineClass & Engine);
    void selectObject(const MouseClass & Mouse, vector <SingleBitmap> & BitmapContainer, vector <SingleFont> & FontContainer);
    void unselectObject();
    void updateEditorWindowOnSelection(vector <EditableTextModule> & EditableTextContainer);
    void updateEditorWindowOnAxisChange(vector <EditableTextModule> & EditableTextContainer);
    void drawSelectionBorder(Camera2D Camera);
    void startScrollbarDragging(const MouseClass & Mouse);
    void dragScrollbars(const MouseClass & Mouse);
    void updateBaseOfTriggerableObjects();
    void detectTriggeredEvents(const EngineClass & Engine, vector <AncestorObject*> & TriggeredObjects);

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






#endif // PROCESSCLASS_H_INCLUDED
