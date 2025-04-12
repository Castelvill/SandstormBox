#ifndef PROCESSCLASS_H_INCLUDED
#define PROCESSCLASS_H_INCLUDED

#include "engineClass.h"
#include <unordered_map>
#include <chrono>

enum CameraMoveType: unsigned char{NONE, CAMERA_FULL, CAMERA_N, CAMERA_NE, CAMERA_E, CAMERA_SE, CAMERA_S, CAMERA_SW, CAMERA_W, CAMERA_NW};

void freeFontsFromContainer(vector <SingleFont> & FontContainer);

vector<string> getAllFilesNamesWithinFolder(string directory, char mode);

vector <short> getPressedKeys(unsigned char key[]);
vector <short> getReleasedKeys(unsigned char key[], vector <short> pressedKeys);

struct AncestorIndex{
    unsigned layerIndex;
    unsigned objectIndex;
    AncestorObject * object(vector <LayerClass> & Layers) const;
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
    vector <AncestorIndex> InitTriggered; //Triggered only once right after the object is created.
    vector <AncestorIndex> IterationTriggered; //If a trigger is negated (huh?), in most cases interpreter puts its event into IterationTriggered events.
    vector <AncestorIndex> IdleTriggered; //Triggered when no events were triggered in the current iteration.
    vector <AncestorIndex> TimeTriggered;
    vector <AncestorIndex> KeyPressedTriggered;
    vector <AncestorIndex> KeyPressingTriggered;
    vector <AncestorIndex> KeyReleasedTriggered;
    vector <AncestorIndex> MouseMovedTriggered;
    vector <AncestorIndex> MouseStillnessTriggered;
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

struct ReferenceStruct{
    string id;
    string eventID;
    bool isNotDirect = false;
};

class ContextClass{
public:
    string ID;
    string eventID;
    //value, pointer, variable, vector, camera, layer, object, text, editable_text, super_text, super_editable_text, image, movement, collision, particles, event, variable, scrollbar, primitives, vector
    DataType type = null_dt;
    bool readOnly = false;
    vector <VariableModule> Values; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> BasePointers;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    ContextClass();
    ContextClass(const ContextClass& Original);
    ContextClass & operator=(const ContextClass& Original);
    void clear();
    void clearState();
    // void setID(const InstrDescription & CurrentInstr,
    //     std::unordered_map<string, ContextClass> & EventContext,
    //     std::unordered_map<string, ReferenceStruct> & References, string newID,
    //     const bool & printOutInstructions, int maxLengthOfValuesPrinting
    // );
    size_t getVectorSize() const;
    string getValue(const InstrDescription & CurrentInstr, int maxLengthOfValuesPrinting);
    ReturnType getAllValues(vector<VariableModule> & NewValues);
    ReturnType getValue(VariableModule & NewValue);
    bool getUnsignedOrAbort(unsigned & number, const InstrDescription & CurrentInstr);
    bool getIntOrAbort(int & number, const InstrDescription & CurrentInstr);
    bool getStringOrAbort(string & text, const InstrDescription & CurrentInstr);
    bool getStringOrIgnore(string & text, const EngineInstr & instruction);
    bool getStringVectorOrIgnore(vector<string> & result, const EngineInstr & instruction);
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

    bool copyFromTheParameter(std::unordered_map<string, ContextClass> & EventContext,
        std::unordered_map<string, ReferenceStruct> & References,
        const string & callingSource, const InstrDescription & CurrentInstr,
        const vector<ParameterStruct> & Parameters, unsigned index, bool printErrors
    );
    void copyOnlyCurrentType(const ContextClass *Original);

    void leaveOneRandomBasePointer();

    void printOutObjects();

    unsigned size() const;
};

template<class EntityType>
void copyFirstInstance(vector<EntityType> & NewContainer, vector<EntityType> & OriginalContainer,
    const DataType originalType, string originalID, string functionName
){
    if(OriginalContainer.size() == 0){
        cerr << "Error: In " << functionName << ": For the context '"
            << originalID << "' of the type '" << dataTypeToStr(originalType)
            << "': Container is empty.";
        return;
    }
    if(NewContainer.size() == 0){
        NewContainer.emplace_back(EntityType());
    }
    NewContainer[0] = OriginalContainer[0];
}

struct ContextMapStruct{
public:
	std::unordered_map<string, ContextClass> Contexts;
    std::unordered_map<string, ReferenceStruct> References;
    string callingSource;
    void clear();
};

struct EventStackStruct{
    vector<EventModule>::iterator Event;
    vector<string> passingVariables;
};

struct PointerRecalculator{
    std::unordered_map<string, vector<unsigned>> CameraIndexes;
    std::unordered_map<string, vector<unsigned>> LayerIndexes;
    std::unordered_map<string, vector<AncestorIndex>> ObjectIndexes;
    std::unordered_map<string, vector<ModuleIndex>> ModuleIndexes;
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
    void findIndexesForCameras(vector<Camera2D> &Cameras, ContextMapStruct & EventContext, Camera2D *& SelectedCamera);
    void findIndexesForLayers(vector<LayerClass> &Layers, ContextMapStruct & EventContext, LayerClass *& OwnerLayer);
    void findIndexesForObjects(vector<LayerClass> &Layers, ContextMapStruct & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject);
    template <class Module>
    ModuleIndex getIndex(Module *& Instance, vector<LayerClass> & Layers, const InstrDescription & CurrentInstr);
    ModuleIndex getIndex(vector<EventModule>::iterator & Instance, vector<LayerClass> & Layers, const InstrDescription & CurrentInstr);
    template <class Module>
    void findIndexesInModule(vector<Module*> Instances, vector<LayerClass> & Layers, const InstrDescription & CurrentInstr, string contextID);
    void findIndexesForModules(vector<LayerClass> &Layers, ContextMapStruct & EventContext, vector<EventModule>::iterator & it_StartingEvent,
        vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack, SuperEditableTextModule *& ActiveEditableText, const InstrDescription & CurrentInstr);
    void updatePointersToCameras(vector<Camera2D> &Cameras, ContextMapStruct & EventContext,
        Camera2D *& SelectedCamera, string processID, string & focusedProcessID, const InstrDescription & CurrentInstr);
    void updatePointersToLayers(vector<LayerClass> &Layers, ContextMapStruct & EventContext, LayerClass *& OwnerLayer, const InstrDescription & CurrentInstr);
    void updatePointersToObjects(vector<LayerClass> &Layers, ContextMapStruct & EventContext, AncestorObject *& Owner,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& SelectedLayer, AncestorObject *& SelectedObject, const InstrDescription & CurrentInstr);
    void updatePointersToModules(vector<LayerClass> &Layers, ContextMapStruct & EventContext, vector<EventModule>::iterator & it_StartingEvent,
        vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack, SuperEditableTextModule *& ActiveEditableText, const InstrDescription & CurrentInstr);
    LayerClass * getOwnerLayer(vector <LayerClass> & Layers);
};

template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity> &Vector);
template<class Entity>
Entity * lastNotDeletedInVector(vector<Entity*> &Vector);

struct Triggers{
    std::unordered_set<TriggerType> active;
    std::unordered_set<string> movingObjects;
    std::unordered_set<string> stillObjects;
    void clear();
};

enum EventControlFlow: char{
    flow_next_event, flow_run_function, flow_jump_back, flow_abort, flow_self_deletion
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
    CameraMoveType activeCameraMoveType;
    
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

    string lastContextID;

    VariableModule LeftOperandProc, RightOperandProc;
    ContextClass NewContext, HelpContext;
    EngineInstr interruptInstruction;
    
public:
    vector <unsigned> camerasOrder;
    vector <unsigned> layersOrder;
    InstrDescription CurrentInstr;
    std::unordered_map<string, ContextMapStruct> ContextLookupTable; //Keys are made of ids of the layer and object.

    std::unordered_map<string, std::chrono::steady_clock::time_point> userDefinedTimers;

    std::unordered_map<EngineInstr, int64_t> TimeSpentOnInstructions;

    int64_t INDEX_TESTS[5] = {0, 0, 0, 0, 0};

    int64_t IF_TESTS[5] = {0, 0, 0, 0, 0};

    void printProfiler();

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
        const EngineClass & Engine, ContextMapStruct & EventContext, bool onlyFirstRequired
    );
    void aggregateLayers(OperationClass & Operation, ContextClass & NewVariable, vector <LayerClass*> AggregatedLayers,
        const EngineClass & Engine, ContextMapStruct & EventContext, bool onlyFirstRequired
    );
    void aggregateObjects(OperationClass & Operation, ContextClass & NewVariable, vector <AncestorObject*> AggregatedObjects,
        const EngineClass & Engine, ContextMapStruct & EventContext, bool onlyFirstRequired
    );
    //Returns true if the context is of a module type.
    bool chooseRandomModuleInstance(ContextClass & NewContext);
    template<class ModuleClass>
    void aggregateModuleContextFromVectors(vector<ModuleClass*> AggregatedModules, const DataType & aggregatedType, OperationClass & Operation, ContextClass & NewContext,
        AncestorObject * Object, const EngineClass & Engine, ContextMapStruct & EventContext, bool onlyFirstRequired
    );
    template<class ModuleClass>
    void findContextInModule(DataType type, AttributeType attribute, ContextClass & NewContext, ModuleClass * Module);
    template<class ModuleClass>
    void getContextFromModuleVectorById(DataType module, string moduleID, AttributeType attribute,
        ContextClass & NewContext, vector <ModuleClass*> AggregatedModules, bool onlyFirstRequired
    );
    void aggregateModules(OperationClass & Operation, ContextClass & NewVariable, ContextClass * OldContext,
        ContextMapStruct & EventContext, const EngineClass & Engine);
    void aggregatePointers(ContextClass & NewContext, vector <BasePointersStruct> & AggregatedPointers, bool onlyFirstRequired);
    void aggregateVariables(ContextClass & NewContext, vector <VariableModule> & AggregatedVariables, bool onlyFirstRequired);
    void findContextInCamera(AttributeType attribute, ContextClass & NewContext, Camera2D * Camera);
    void findContextInLayer(ValueLocation Location, ContextClass & NewContext, LayerClass * Layer);
    template <class Module>
    void findContextInModuleVector(const ValueLocation & Location, ContextClass & NewContext, vector<Module> & Source);
    void findContextInObject(ValueLocation Location, ContextClass & NewContext, AncestorObject * Object);
    bool findLayerAndObject(ValueLocation & Location, AncestorObject * Owner, LayerClass * OwnerLayer,
        LayerClass *& CurrentLayer, AncestorObject *& CurrentObject);
    void aggregateCamerasAndLayersById(ValueLocation & Location, ContextClass & NewVariable,
        AncestorObject * Owner, LayerClass * OwnerLayer
    );
    void aggregateModulesById(DataType moduleType, string moduleID, AttributeType attribute,
        ContextClass & NewContext, ModulesPointers & AggregatedModules
    );
    void findLowerContextById(ValueLocation & Location, ContextClass & NewContext, ContextClass * OldContext);
    template<class Entity>
    void executeOperationsOnSets(vector<Entity*> & NewContext, vector<Entity*> & LeftOperand, vector<Entity*> & RightOperand);
    template<class Entity>
    void executeOperationsOnSets(vector<Entity> & NewContext, vector<Entity> & LeftOperand, vector<Entity> & RightOperand);
    void aggregateTwoSets(OperationClass & Operation, ContextMapStruct & EventContext);
    // void addNewContext(ContextMapStruct & EventContext, 
    //     const ContextClass & NewContext, string type, string newID, bool global
    // );
    void aggregateEntities(OperationClass & Operation, ContextMapStruct & EventContext, const EngineClass & Engine);
    void assignVariableFromPointer(ContextMapStruct & EventContext, ContextClass * Context, string variableID, bool isReference);
    void assignVariable(ContextMapStruct & EventContext, string variableID, bool isReference);
    void aggregateValues(ContextMapStruct & EventContext, OperationClass & Operation, LayerClass *OwnerLayer,
        AncestorObject *Owner, const EngineClass & Engine, vector<ProcessClass> * Processes);
    void aggregateOnlyById(ContextMapStruct & EventContext, OperationClass & Operation, LayerClass *OwnerLayer, AncestorObject *Owner);
    //void nameVariable(ContextMapStruct & EventContext, OperationClass & Operation);
    void moveValues(OperationClass & Operation, ContextMapStruct & EventContext);
    void incrementInteger(OperationClass & Operation, ContextMapStruct & EventContext);
    void cloneEntities(OperationClass & Operation, ContextMapStruct & EventContext, vector<LayerClass> &Layers);
    void executeArithmetics(OperationClass & Operation, ContextMapStruct & EventContext);
    void generateRandomVariable(ContextMapStruct & EventContext, const OperationClass & Operation);
    void createLiteral(ContextMapStruct & EventContext, const OperationClass & Operation);
    void checkIfVectorContainsVector(OperationClass & Operation, ContextMapStruct & EventContext);
    bool prepareVectorSizeAndIDsForNew(OperationClass & Operation, ContextMapStruct & EventContext, unsigned & newVectorSize, vector <string> & newIDs);
    bool prepareDestinationForNew(OperationClass & Operation, ContextMapStruct & EventContext, LayerClass *& CurrentLayer, AncestorObject *& CurrentObject, string & layerID, string & objectID, vector<LayerClass> &Layers);
    void assignEntities(ContextMapStruct & EventContext, ContextClass & NewValue,
        string variableID, bool isReference, const DataType & source
    );
    void createNewEntities(OperationClass & Operation, ContextMapStruct & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & it_StartingEvent,
        vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack, string & focusedProcessID
    );
    void markEntitiesForDeletion(OperationClass & Operation, ContextMapStruct & EventContext, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, string & focusedProcess
    );
    void getIndexes(ContextMapStruct & EventContext, const vector<ParameterStruct> & Parameters, vector<unsigned> & indexes, bool skipContext);
    void getReferenceFromLayer(const OperationClass & Operation, ContextClass & SourceContext, vector<unsigned> & indexes, ContextClass & NewContext);
    void getReferenceFromObject(const OperationClass & Operation, ContextClass & SourceContext, vector<unsigned> & indexes, ContextClass & NewContext);
    void getReferenceFromContextByIndex(OperationClass & Operation, ContextClass & SourceContext, vector<unsigned> & indexes, ContextClass & NewContext);
    void getReferenceByIndex(OperationClass & Operation, ContextMapStruct & EventContext);
    void getInstanceFromVector(OperationClass & Operation, ContextMapStruct & EventContext);
    void bindFilesToObjects(OperationClass & Operation, ContextMapStruct & EventContext);
    void removeBindedFilesFromObjects(OperationClass & Operation, ContextMapStruct & EventContext);
    bool buildEventsInObjects(OperationClass & Operation, ContextMapStruct & EventContext, AncestorObject * Owner,
        vector<EventModule>::iterator & it_StartingEvent, vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack, bool allowNotAscii
    );
    bool customBuildEventsInObjects(OperationClass & Operation, ContextMapStruct & EventContext,
        AncestorObject * Owner, vector<EventModule>::iterator & it_StartingEvent, vector<EventModule>::iterator & it_Event,
        vector<EventStackStruct> & MemoryStack, const EngineInstr & mode, bool allowNotAscii
    );
    void clearEventsInObjects(OperationClass & Operation, ContextMapStruct & EventContext, AncestorObject * Owner);
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
    void executeFunction(OperationClass Operation, ContextMapStruct & EventContext, vector<EventModule>::iterator & it_Event, EngineClass & Engine);
    void changeEngineVariables(OperationClass & Operation, ContextMapStruct & EventContext, EngineClass & Engine);
    void changeProcessVariables(OperationClass & Operation, ContextMapStruct & EventContext, vector <string> & processIDs);
    void loadBitmap(OperationClass & Operation, ContextMapStruct & EventContext, vector<SingleBitmap> & BitmapContainer);
    void createDirectory(OperationClass & Operation, ContextMapStruct & EventContext);
    void removeFileOrDirectory(OperationClass & Operation, ContextMapStruct & EventContext);
    void removeRecursivelyFileOrDirectory(OperationClass & Operation, ContextMapStruct & EventContext);
    void renameFileOrDirectory(OperationClass & Operation, ContextMapStruct & EventContext);
    void executePrint(OperationClass & Operation, ContextMapStruct & EventContext);
    void saveStringAsFile(OperationClass & Operation, ContextMapStruct & EventContext);
    void loadFileAsString(OperationClass & Operation, ContextMapStruct & EventContext);
    void listOutEntities(OperationClass & Operation, ContextMapStruct & EventContext, const vector<ProcessClass> & Processes, const EngineClass & Engine);
    void createNewProcess(OperationClass & Operation, vector<ProcessClass> & Processes, ContextMapStruct & EventContext,
        AncestorObject *& Owner, vector <AncestorObject*> & TriggeredObjects, vector<EventModule>::iterator & it_StartingEvent,
        vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack, EngineClass & Engine
    );
    void createNewOwnerVariable(OperationClass & Operation, ContextMapStruct & EventContext, AncestorObject * Owner,
        vector<EventModule>::iterator & it_StartingEvent, vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack
    );
    void createNewOwnerVector(OperationClass & Operation, ContextMapStruct & EventContext, AncestorObject * Owner,
        vector<EventModule>::iterator & it_StartingEvent, vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack
    );
    void tokenizeStringFromContext(OperationClass & Operation, ContextMapStruct & EventContext);
    void printTree(OperationClass & Operation, ContextMapStruct & EventContext, vector<ProcessClass> & Processes);
    void getStringSizeFromContext(OperationClass & Operation, ContextMapStruct & EventContext);
    void getSizeOfContext(OperationClass & Operation, ContextMapStruct & EventContext);
    void getSubStringFromContext(OperationClass & Operation, ContextMapStruct & EventContext);
    void loadFontFromContext(OperationClass & Operation, ContextMapStruct & EventContext, EngineClass & Engine);
    void findByIDInEventContext(OperationClass & Operation, ContextMapStruct & EventContext);
    void listOutFiles(OperationClass & Operation, ContextMapStruct & EventContext);
    void changeWorkingDirectory(OperationClass & Operation, ContextMapStruct & EventContext);
    void printWorkingDirectory(OperationClass & Operation, ContextMapStruct & EventContext);
    void findSimilarStrings(OperationClass & Operation, ContextMapStruct & EventContext);
    void countPatternOccurrences(OperationClass &Operation, ContextMapStruct & EventContext);
    void getConsoleInput(OperationClass & Operation, ContextMapStruct & EventContext,
        int & terminationTimer, const int & timeoutTerminationTime, ALLEGRO_EVENT_QUEUE * eventQueue
    );
    void createDisplay(OperationClass & Operation, ContextMapStruct & EventContext, EngineClass & Engine);
    void startTimer(OperationClass & Operation, ContextMapStruct & EventContext);
    void stopTimer(OperationClass & Operation, ContextMapStruct & EventContext);
    bool assertValues(OperationClass & Operation, ContextMapStruct & EventContext);
    void getContextType(OperationClass & Operation, ContextMapStruct & EventContext);
    EngineInstr executeInstructions(vector<OperationClass> & Operations, LayerClass *& OwnerLayer,
        AncestorObject *& Owner, ContextMapStruct & EventContext, vector<AncestorObject*> & TriggeredObjects,
        vector<ProcessClass> & Processes, vector<EventModule>::iterator & it_StartingEvent,
        vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & MemoryStack, EngineClass & Engine,
        unsigned & runChildEventWithIndex
    );
    VariableModule findNextValueInMovementModule(ConditionClass & Condition, AncestorObject * CurrentObject);
    VariableModule getValueFromObjectInCamera(AncestorObject * CurrentObject,
        const AttributeType & attribute, const string & cameraID
    );
    VariableModule getValueFromMouseClickingObject(const MouseClass & Mouse, AncestorObject * CurrentObject,
        const AttributeType & attribute, const short & button
    );
    VariableModule getValueFromObjectInCollision(ConditionClass &Condition, AncestorObject * CurrentObject, LayerClass * CurrentLayer);
    VariableModule findNextValueAmongObjects(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer,
        const MouseClass & Mouse
    );
    void getExistsOrIsDirectory(ContextMapStruct & EventContext, VariableModule & NewValue, const string & variableID, const ValueSource & source);
    void getScreenWidthOrHeigth(ContextMapStruct & EventContext, VariableModule & NewValue, const VariableModule & Literal, const ValueSource & source);
    bool getProcess(VariableModule & NewValue, vector<ProcessClass> * Processes, ProcessClass *& Process, const string & processID);
    VariableModule getValueFromVector(ConditionClass & Condition, ContextMapStruct & EventContext);
    void getValueFromContext(ConditionClass & Condition, ContextMapStruct & EventContext,
        AncestorObject * Owner, LayerClass * OwnerLayer, const MouseClass & Mouse, VariableModule & NewValue
    );
    void findNextValue(ConditionClass & Condition, AncestorObject * Owner, LayerClass * OwnerLayer,
        const EngineClass & Engine, vector<ProcessClass> * Processes, ContextMapStruct & EventContext, VariableModule & NewValue);
    char evaluateConditionalChain(vector<ConditionClass> & ConditionalChain, vector<VariableModule> & resultStack,
    AncestorObject * Owner, LayerClass * OwnerLayer, const EngineClass & Engine, ContextMapStruct & EventContext);
    vector<EventModule>::iterator findChildEventToRun(
        vector<EventModule> & EventContainer, vector<EventModule>::iterator & it_Event,
        vector<string> & passingVariables, string & callingScript, unsigned & lineNumber,
        const unsigned & runChildEventWithIndex
    );
    bool deleteEntities();
    void resetChildren(vector<EventModule>::iterator & it_Event, AncestorObject * Triggered);
    bool passVariablesToTheChild(const vector<string> & ParentEventVariables,
        const vector<StartingVariableStruct> & CurrentEventVariables, ContextMapStruct & VariablesLoookupTable
    );
    EventControlFlow executeSingleEvent(EngineClass & Engine, vector<ProcessClass> & Processes,
        vector<EventModule>::iterator & it_StartingEvent, vector<EventModule>::iterator & it_Event,
        vector<EventStackStruct> & EventStack, ContextMapStruct & VariablesLoookupTable,
        vector <AncestorObject*> & TriggeredObjects, LayerClass *& TriggeredLayer,
        AncestorObject *& Triggered
    );
    //Return true if engine should be rebooted or terminated.
    bool executeEventLoop(EngineClass & Engine, vector<ProcessClass> & Processes,
        const Triggers & CurrentTriggers, vector<EventModule>::iterator & it_StartingEvent,
        vector<EventModule>::iterator & it_Event, vector<EventStackStruct> & EventStack,
        ContextMapStruct & VariablesLoookupTable, vector <AncestorObject*> & TriggeredObjects,
        LayerClass *& TriggeredLayer, AncestorObject *& TriggeredObject
    );
    bool executeTriggeredEvents(EngineClass & Engine, vector<ProcessClass> & Processes,
        vector <AncestorObject*> & TriggeredObjects, Triggers & CurrentTriggers
    );
    void detectAndExecuteTriggeredEvents(EngineClass & Engine, vector<ProcessClass> & Processes);
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
    void detectTriggeredByIdleEvents(const EngineClass & Engine, vector <AncestorObject*> & TriggeredObjects, Triggers & CurrentTriggers);
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
