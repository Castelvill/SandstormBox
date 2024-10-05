#ifndef PROCESSCLASS_H_INCLUDED
#define PROCESSCLASS_H_INCLUDED

#include "engineClass.h"
#include <unordered_set>

enum CAMERA_MOVE: unsigned char{NONE, CAMERA_FULL, CAMERA_N, CAMERA_NE, CAMERA_E, CAMERA_SE, CAMERA_S, CAMERA_SW, CAMERA_W, CAMERA_NW};

void freeFontsFromContainer(vector <SingleFont> & FontContainer);

vector<string> getAllFilesNamesWithinFolder(string directory, char mode);

bool checkForVectorSize(size_t leftSize, size_t rightSize, bool & sameSize, string functionName);

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
    Module * getModulePointer(vector <LayerClass> & Layers);
    vector<EventModule>::iterator module(vector<LayerClass> &Layers);
};

//This struct consists of pointers to every object that has at least one event triggerable by the right source  
struct EventsLookupTable{
    vector <AncestorIndex> BootTriggered; //Triggered only in the first iteration or in the first iteration after a reboot.
    vector <AncestorIndex> InitTriggered; //Triggered only when the object is created.
    vector <AncestorIndex> IterationTriggered; //If a trigger is negated or has else statement, in most cases interpreter puts its event into IterationTriggered events. 
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
    //value, pointer, variable, vector, camera, layer, object, text, editable_text, super_text, super_editable_text, image, movement, collision, particles, event, variable, scrollbar, primitives, vector
    string type;
    bool readOnly;
    vector <VariableModule> Values; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> BasePointers;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    ContextClass();
    void clear();
    void setID(const InstrDescription & CurrentInstr, vector<ContextClass> &EventContext, string newID,
        const bool & printOutInstructions, int maxLengthOfValuesPrinting
    );
    size_t getVectorSize() const;
    string getValue(const InstrDescription & CurrentInstr, int maxLengthOfValuesPrinting);
    bool getUnsignedOrAbort(unsigned & number, const InstrDescription & CurrentInstr);
    bool getIntOrAbort(int & number, const InstrDescription & CurrentInstr);
    bool getStringOrAbort(string & text, const InstrDescription & CurrentInstr);
    bool getStringOrIgnore(string & text, EngineInstr instruction);
    bool getStringVectorOrIgnore(vector<string> & result, EngineInstr instruction);
    template<typename T>
    void addBasePointer(T * pointer);
    template<typename T>
    void setFirstBasePointer(T * pointer);
    void addModule(TextModule * Module);
    void addModule(EditableTextModule * Module);
    void addModule(SuperTextModule * Module);
    void addModule(SuperEditableTextModule * Module);
    void addModule(ImageModule * Module);
    void addModule(MovementModule * Module);
    void addModule(CollisionModule * Module);
    void addModule(ParticleEffectModule * Module);
    void addModule(EventModule * Module);
    void addModule(VariableModule * Module);
    void addModule(ScrollbarModule * Module);
    void addModule(PrimitivesModule * Module);
    void addModule(VectorModule * Module);

    void setFirstModule(TextModule * Module);
    void setFirstModule(EditableTextModule * Module);
    void setFirstModule(SuperTextModule * Module);
    void setFirstModule(SuperEditableTextModule * Module);
    void setFirstModule(ImageModule * Module);
    void setFirstModule(MovementModule * Module);
    void setFirstModule(CollisionModule * Module);
    void setFirstModule(ParticleEffectModule * Module);
    void setFirstModule(EventModule * Module);
    void setFirstModule(VariableModule * Module);
    void setFirstModule(ScrollbarModule * Module);
    void setFirstModule(PrimitivesModule * Module);
    void setFirstModule(VectorModule * Module);

    bool copyFromTheParameter(vector<ContextClass> & EventContext, const InstrDescription & CurrentInstr,
        const vector<ParameterStruct> & Parameters, unsigned index, bool printErrors
    );

    void leaveOneRandomBasePointer();

    void printOutObjects();

    unsigned size() const;
};

struct MemoryStackStruct{
    vector<EventModule>::iterator Event;
    size_t contextSize;
};

struct PointerRecalculator{
    vector<vector<unsigned>> CameraIndexes;
    vector<vector<unsigned>> LayerIndexes;
    vector<vector<AncestorIndex>> ObjectIndexes;
    vector<vector<ModuleIndex>> ModuleIndexes;
    vector<AncestorIndex> TriggeredObjectIndexes;
    vector<ModuleIndex> PastEvents;

    unsigned eventOwnerLayerIndex;
    AncestorIndex EventOwnerIndex;
    AncestorIndex SelectedObjectIndex;

    unsigned selectedLayerIndex = 0;
    unsigned selectedCameraIndex = 0;
    ModuleIndex startingEventIndex = {0, 0, 0};
    ModuleIndex eventIndex = {0, 0, 0};
    bool didActiveEditableTextExist;
    ModuleIndex ActiveEditableTextIndex = {0, 0, 0};

    void clear();
    void findIndexesForCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext, Camera2D *& SelectedCamera);
    void findIndexesForLayers(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer);
    void findIndexesForObjects(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject);
    template <class Module>
    ModuleIndex getIndex(Module *& Instance, vector<LayerClass> & Layers, const InstrDescription & CurrentInstr);
    ModuleIndex getIndex(vector<EventModule>::iterator & Instance, vector<LayerClass> & Layers, const InstrDescription & CurrentInstr);
    template <class Module>
    void findIndexesInModule(vector<Module*> Instances, vector<LayerClass> & Layers, const InstrDescription & CurrentInstr);
    void findIndexesForModules(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, vector<EventModule>::iterator & StartingEvent,
        vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, SuperEditableTextModule *& ActiveEditableText, const InstrDescription & CurrentInstr);
    void updatePointersToCameras(vector<Camera2D> &Cameras, vector<ContextClass> & EventContext,
        Camera2D *& SelectedCamera, string processID, string & focusedProcessID, const InstrDescription & CurrentInstr);
    void updatePointersToLayers(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer, const InstrDescription & CurrentInstr);
    void updatePointersToObjects(vector<LayerClass> &Layers, vector<ContextClass> & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject, const InstrDescription & CurrentInstr);
    void updatePointersToModules(vector<LayerClass> &Layers, vector<ContextClass> &EventContext, vector<EventModule>::iterator & StartingEvent,
        vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, SuperEditableTextModule *& ActiveEditableText, const InstrDescription & CurrentInstr);
    LayerClass * getOwnerLayer(vector <LayerClass> & Layers);
};

ContextClass * getContextByID(const InstrDescription & CurrentInstr, vector<ContextClass> & AllContexts, string contextID, bool printError);
void extractPointersFromModules(ModulesPointers & ContextModules, AncestorObject * Object, string moduleType);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity> &Vector);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity*> &Vector);

struct Triggers{
    std::unordered_set<string> active;
    std::unordered_set<string> movingObjects;
    std::unordered_set<string> stillObjects;
    void clear();
};

class ProcessClass{
private:
    string ID;
    bool isActive;
    bool canUserInteract; 
    bool isRendering;
    vec2d windowPos, windowSize, minWindowSize;
    float windowTint[4];

    string EXE_PATH;
    string workingDirectory;
    
    vector <LayerClass> Layers;
    vector <Camera2D> Cameras;
    vector <string> layersIDs;
    vector <string> camerasIDs;

    string selectedObjectLayerID;
    string selectedObjectID;
    Camera2D * SelectedCamera;
    LayerClass * SelectedLayer;
    AncestorObject * SelectedObject;
    SuperEditableTextModule * ActiveEditableText;
    bool isDraggingScrollbar;
    vec2d dragStartingPos, dragStartingPos2;
    vec2d dragCameraStaringPos, dragLimit;
    EventsLookupTable BaseOfTriggerableObjects;
    bool firstIteration;
    bool wasDeleteExecuted, wasNewExecuted, wasAnyEventUpdated, wereGlobalVariablesCreated;

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
    int maxLengthOfValuesPrinting = 100; //Used in printing values of variables while debugging.
    bool printOutStackAutomatically;
    float reservationMultiplier;

    long timeToInterruptMovement;
    long timeToInterruptParticles;
    
public:
    vector <unsigned> camerasOrder;
    vector <unsigned> layersOrder;
    EventDescription EventIds;
    InstrDescription CurrentInstr;

    string getID() const;
    void setID(string newID, vector<string> & listOfIDs);
    void create(string EXE_PATH_FROM_ENGINE, bool allowNotAscii, vec2i screenSize, string initFilePath, string newID, string newLayerID, string newObjectID, vector<string> &listOfIDs);
    void clear();
    void resizeWindow(vec2d newSize);
    void resizeWindow(double x, double y);
    bool isLayersUniquenessViolated();
    bool isCamerasUniquenessViolated();
    void executeIteration(EngineClass & Engine, vector<ProcessClass> & Processes);
    void selectLettersInText(const MouseClass & Mouse);
    void checkMouseCollisions(EngineClass & Engine);
    void renderOnDisplay(EngineClass & Engine);
    void aggregateCameras(OperationClass & Operation, ContextClass & NewContext, vector <Camera2D*> AggregatedCameras,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    void aggregateLayers(OperationClass & Operation, ContextClass & NewVariable, vector <LayerClass*> AggregatedLayers,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    void aggregateObjects(OperationClass & Operation, ContextClass & NewVariable, vector <AncestorObject*> AggregatedObjects,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    //Returns true if the context is of a module type.
    bool chooseRandomModule(ContextClass & NewContext);
    template<class ModuleClass>
    void aggregateModuleContextFromVectors(vector<ModuleClass*> AggregatedModules, const string & type, OperationClass & Operation, ContextClass & NewContext,
        AncestorObject * Object, const EngineClass & Engine, vector<ContextClass> &EventContext
    );
    template<class ModuleClass>
    void findContextInModule(string module, string attribute, ContextClass & NewContext, ModuleClass * Module);
    template<class ModuleClass>
    void getContextFromModuleVectorById(string module, string moduleID, string attribute, ContextClass & NewContext, vector <ModuleClass*> AggregatedModules);
    void aggregateModules(OperationClass & Operation, ContextClass & NewVariable, ContextClass * OldContext,
        vector<ContextClass> &EventContext, const EngineClass & Engine);
    void aggregatePointers(ContextClass & NewContext, vector <BasePointersStruct> & AggregatedPointers);
    void aggregateVariables(ContextClass & NewContext, vector <VariableModule> & AggregatedVariables);
    void findContextInCamera(string attribute, ContextClass & NewContext, Camera2D * Camera);
    void findContextInLayer(ValueLocation Location, ContextClass & NewContext, LayerClass * Layer);
    template <class Module>
    void findContextInModuleVector(const ValueLocation & Location, ContextClass & NewContext, vector<Module> & Source);
    void findContextInObject(ValueLocation Location, ContextClass & NewContext, AncestorObject * Object);
    bool findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer,
        LayerClass *& CurrentLayer, AncestorObject *& CurrentObject);
    void aggregateCamerasAndLayersById(ValueLocation & Location, ContextClass & NewVariable, AncestorObject * Owner, LayerClass * OwnerLayer);
    void aggregateModulesById(string moduleType, string moduleID, string attribute,
        ContextClass & NewContext, ModulesPointers & AggregatedModules);
    void findLowerContextById(ValueLocation & Location, ContextClass & NewContext, ContextClass * OldContext);
    //Method return true if a pair of contexts of the same type is found.
    bool getPairOfContexts(ContextClass *& LeftOperand, ContextClass *& RightOperand,
        vector<ContextClass> & AllContexts, vector <string> contextIDs);
    bool getOneContext(ContextClass *& SelectedContext, vector<ContextClass> & AllContexts, vector<string> contextIDs);
    bool getAllSelectedContexts(vector<ContextClass*> & SelectedContexts, vector<ContextClass> & AllContexts,
        const vector<string> & contextIDs);
    template<class Entity>
    void executeOperationsOnSets(vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand);
    template<class Entity>
    void executeOperationsOnSets(vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand);
    void aggregateTwoSets(OperationClass & Operation, vector<ContextClass> & EventContext);
    void addNewContext(vector<ContextClass> & EventContext, const ContextClass & NewContext, string type, string newID);
    void aggregateEntities(OperationClass & Operation, vector<ContextClass> & EventContext, const EngineClass & Engine);
    void moveOrRename(vector<ContextClass> & EventContext, ContextClass NewContext, string newContextID);
    void aggregateValues(vector<ContextClass> &EventContext, OperationClass & Operation, LayerClass *OwnerLayer,
        AncestorObject *Owner, const EngineClass & Engine, vector<ProcessClass> * Processes);
    void aggregateOnlyById(vector<ContextClass> &EventContext, OperationClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner);
    void nameVariable(vector<ContextClass> & EventContext, OperationClass & Operation);
    template<class Entity>
    void cloneRightToLeft(vector <Entity*> & LeftOperand, vector <Entity*> & RightOperand, vector<LayerClass> & Layers, const bool & changeOldID);
    void moveValues(OperationClass & Operation, vector<ContextClass> &EventContext);
    void cloneEntities(OperationClass & Operation, vector<ContextClass> &EventContext, vector<LayerClass> &Layers);
    void executeArithmetics(OperationClass & Operation, vector<ContextClass> &EventContext);
    void generateRandomVariable(vector<ContextClass> &EventContext, const OperationClass & Operation);
    void createLiteral(vector<ContextClass> &EventContext, const OperationClass & Operation);
    void checkIfVectorContainsVector(OperationClass & Operation, vector<ContextClass> &EventContext);
    bool prepareVectorSizeAndIDsForNew(OperationClass & Operation, vector<ContextClass> & EventContext, unsigned & newVectorSize, vector <string> & newIDs);
    bool prepareDestinationForNew(OperationClass & Operation, vector<ContextClass> & EventContext, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, string & layerID, string & objectID, vector<LayerClass> &Layers);
    void createNewEntities(OperationClass & Operation, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & StartingEvent,
        vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, string & focusedProcessID
    );
    void markEntitiesForDeletion(OperationClass & Operation, vector<ContextClass> & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, string & focusedProcess
    );
    void getIndexes(vector<ContextClass> & EventContext, const vector<ParameterStruct> & Parameters, vector<unsigned> & indexes, bool skipContext);
    void getReferenceByIndex(OperationClass & Operation, vector<ContextClass> & EventContext);
    void bindFilesToObjects(OperationClass & Operation, vector<ContextClass> & EventContext);
    void removeBindedFilesFromObjects(OperationClass & Operation, vector<ContextClass> & EventContext);
    bool buildEventsInObjects(OperationClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
        vector<EventModule>::iterator & StartingEvent, vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, bool allowNotAscii
    );
    bool customBuildEventsInObjects(OperationClass & Operation, vector<ContextClass> & EventContext,
        AncestorObject * Owner, vector<EventModule>::iterator & StartingEvent, vector<EventModule>::iterator & Event,
        vector<MemoryStackStruct> & MemoryStack, char mode, bool allowNotAscii
    );
    void clearEventsInObjects(OperationClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner);
    void executeFunctionForCameras(OperationClass & Operation, vector <VariableModule> & Variables,
        vector<Camera2D*> CamerasFromContext, Camera2D *& SelectedCamera, string & focusedProcessID
    );
    void moveLayerInDrawingOrder(LayerClass * Layer, unsigned newIndex);
    void minimizeLayerInDrawingOrder(LayerClass * Layer);
    void bringForwardLayerInDrawingOrder(LayerClass * Layer);
    void executeFunctionForLayers(OperationClass & Operation, vector <VariableModule> & Variables, vector<LayerClass*> & Layers);
    inline bool getLayerOfTheObject(LayerClass *& ObjectLayer, AncestorObject * Objec, string functionName);
    inline bool findCurrentIndexInObjectsDrawingOrder(LayerClass * ObjectLayer, AncestorObject * Object, string functionName, unsigned & currentIndex);
    void moveObjectInDrawingOrder(AncestorObject * Object, unsigned newIndex);
    void minimizeObjectInDrawingOrder(AncestorObject * Object);
    void bringForwardObjectInDrawingOrder(AncestorObject * Object);
    void executeFunctionForObjects(OperationClass & Operation, vector <VariableModule> & Variables, vector<AncestorObject*> & Objects);
    void executeFunction(OperationClass Operation, vector<ContextClass> & EventContext, vector<EventModule>::iterator & Event, EngineClass & Engine);
    void changeEngineVariables(OperationClass & Operation, vector<ContextClass> & EventContext, EngineClass & Engine);
    void changeProcessVariables(OperationClass & Operation, vector<ContextClass> & EventContext, vector <string> & processIDs);
    void loadBitmap(OperationClass & Operation, vector<ContextClass> & EventContext, vector<SingleBitmap> & BitmapContainer);
    void createDirectory(OperationClass & Operation, vector<ContextClass> & EventContext);
    void removeFileOrDirectory(OperationClass & Operation, vector<ContextClass> & EventContext);
    void removeRecursivelyFileOrDirectory(OperationClass & Operation, vector<ContextClass> & EventContext);
    void renameFileOrDirectory(OperationClass & Operation, vector<ContextClass> & EventContext);
    void executePrint(OperationClass & Operation, vector<ContextClass> & EventContext);
    void saveStringAsFile(OperationClass & Operation, vector<ContextClass> & EventContext);
    void loadFileAsString(OperationClass & Operation, vector<ContextClass> & EventContext);
    void listOutEntities(OperationClass & Operation, vector<ContextClass> & EventContext, const vector<ProcessClass> & Processes, const EngineClass & Engine);
    void createNewProcess(OperationClass & Operation, vector<ProcessClass> & Processes, vector<ContextClass> &EventContext,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & StartingEvent,
        vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, EngineClass & Engine
    );
    void createNewOwnerVariable(OperationClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
        vector<EventModule>::iterator & StartingEvent, vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
    );
    void createNewOwnerVector(OperationClass & Operation, vector<ContextClass> & EventContext, AncestorObject * Owner,
        vector<EventModule>::iterator & StartingEvent, vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack
    );
    void tokenizeStringFromContext(OperationClass & Operation, vector<ContextClass> & EventContext);
    void printTree(OperationClass & Operation, vector<ContextClass> & EventContext, vector<ProcessClass> & Processes);
    void getStringSizeFromContext(OperationClass & Operation, vector<ContextClass> & EventContext);
    void getSizeOfContext(OperationClass & Operation, vector<ContextClass> & EventContext);
    void getSubStringFromContext(OperationClass & Operation, vector<ContextClass> & EventContext);
    void loadFontFromContext(OperationClass & Operation, vector<ContextClass> & EventContext, EngineClass & Engine);
    void findByIDInEventContext(OperationClass & Operation, vector<ContextClass> & EventContext);
    void listOutFiles(OperationClass & Operation, vector<ContextClass> & EventContext);
    void changeWorkingDirectory(OperationClass & Operation, vector<ContextClass> & EventContext);
    void printWorkingDirectory(OperationClass & Operation, vector<ContextClass> & EventContext);
    void findSimilarStrings(OperationClass & Operation, vector<ContextClass> & EventContext);
    void countPatternOccurrences(OperationClass &Operation, vector<ContextClass> &EventContext);
    void getConsoleInput(OperationClass & Operation, vector<ContextClass> & EventContext, int & terminationTimer, ALLEGRO_EVENT_QUEUE * eventQueue);
    void createDisplay(OperationClass & Operation, vector<ContextClass> & EventContext, EngineClass & Engine);
    OperationClass executeInstructions(vector<OperationClass> Operations, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector<ContextClass> & EventContext, vector<AncestorObject*> & TriggeredObjects,
        vector<ProcessClass> & Processes, vector<EventModule>::iterator & StartingEvent,
        vector<EventModule>::iterator & Event, vector<MemoryStackStruct> & MemoryStack, EngineClass & Engine
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
        const EngineClass & Engine, vector<ProcessClass> * Processes, vector<ContextClass> &EventContext);
    char evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, AncestorObject * Owner, LayerClass * OwnerLayer,
        const EngineClass & Engine, vector<ContextClass> &EventContext);
    vector<EventModule>::iterator FindUnfinishedEvent(AncestorObject * Triggered, vector<EventModule>::iterator & Event);
    vector<EventModule>::iterator FindElseEvent(AncestorObject * Triggered, vector<EventModule>::iterator & Event);
    bool deleteEntities();
    void resetChildren(vector<EventModule>::iterator & Event, AncestorObject * Triggered);
    void executeEvents(EngineClass & Engine, vector<ProcessClass> & Processes);
    void updateTreeOfCamerasFromSelectedRoot(Camera2D * Selected);
    void updateWholeForestOfCameras();
    void adjustPositionOfAllCameras();
    void keepPositionInsideScreen(vec2d & pos, vec2d & size, vec2i displaySize);
    void updateCamerasPositions(const EngineClass & Engine);
    void bringCameraForward(unsigned index, Camera2D * ChosenCamera);
    void selectCamera(bool fromAltTab, const MouseClass & Mouse, const vector<short> & pressedKeys,
        const vector<short> & releasedKeys, string & focusedProcessID
    );
    bool isKeyFirstPressed(short key, vector <short> firstPressedKeys);
    bool isKeyPressed(short key, vector <short> pressedKeys);
    bool isKeyReleased(short key, vector <short> releasedKeys);
    void detectStartPosOfDraggingObjects(const MouseClass & Mouse);
    void changeCursor(ALLEGRO_DISPLAY *display, const MouseClass & Mouse);
    void detectStartPosOfDraggingCamera(ALLEGRO_DISPLAY *display, const MouseClass & Mouse);
    void drawEverything(EngineClass & Engine);
    void drawModules(const AncestorObject & Object, Camera2D & Cameras,
        vector <SingleFont> & FontContainer, size_t & numberOfDrawnObjects, vec2i displaySize
    );
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
    void selectObject(const MouseClass & Mouse);
    void unselectObject();
    void drawSelectionBorder(Camera2D Camera);
    void startScrollbarDragging(const MouseClass & Mouse);
    void stopScrollbarDragging();
    void dragScrollbars(const MouseClass & Mouse);
    void updateBaseOfTriggerableObjects();
    void detectTriggeredEvents(const EngineClass & Engine, vector <AncestorObject*> & TriggeredObjects, Triggers & CurrentTriggers);
    size_t countLayers() const;
    size_t countCameras() const;
    
    bool checkDefaultCondition(VariableModule * Left, VariableModule * Right);
    bool checkDefaultCondition(BasePointersStruct * Left, BasePointersStruct * Right);
};

struct ProcessSkeleton{
    string scriptPath;
    string ID;
    string firstLayerID;
    string firstObjectID;
};

#endif // PROCESSCLASS_H_INCLUDED
