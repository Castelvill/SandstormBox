#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "compiler.h"

DataType vectorizeEntityDataType(const InstrDescription & CurrentInstr, const DataType & oldType);

//Struct of vectors
struct ModulesPointers{
    vector<SuperTextModule*> SuperTexts;
    vector<SuperEditableTextModule*> SuperEditableTexts;
    vector<ImageModule*> Images;
    vector<MovementModule*> Movements;
    vector<CollisionModule*> Collisions;
    vector<ParticleEffectModule*> Particles;
    vector<EventModule*> Events;
    vector<VariableModule*> Variables;
    vector<ScrollbarModule*> Scrollbars;
    vector<PrimitivesModule*> Primitives;
    vector<VectorModule*> Vectors;
    bool hasInstanceOfAnyModule() const;
    unsigned size() const;
    bool empty() const;
    ModulesPointers(){};
};

/**
The most important class, a container for all modules that make an object.
*/
class AncestorObject: public PrimaryModule{
public:
    vector<SuperTextModule> SuperTextContainer;
    vector<SuperEditableTextModule> SuperEditableTextContainer;
    vector<ImageModule> ImageContainer;
    vector<MovementModule> MovementContainer;
    vector<CollisionModule> CollisionContainer;
    vector<ParticleEffectModule> ParticlesContainer;
    vector<EventModule> EventContainer;
    //Other name: Parameters
    vector<VariableModule> VariablesContainer;
    vector<ScrollbarModule> ScrollbarContainer;
    vector<PrimitivesModule> PrimitivesContainer;
    vector<VectorModule> VectorContainer;
    vector<string> superTextContainerIDs;
    vector<string> superEditableTextContainerIDs;
    vector<string> imageContainerIDs;
    vector<string> movementContainerIDs;
    vector<string> collisionContainerIDs;
    vector<string> particlesContainerIDs;
    vector<string> eventContainerIDs;
    vector<string> variablesContainerIDs;
    vector<string> scrollbarContainerIDs;
    vector<string> primitivesContainerIDs;
    vector<string> vectorContainerIDs;

    vector<string> bindedScripts;
    
    bool canBeMovedWithMouse = false;
    bool canDrawSelectionBorder = false;
    //Engine invalidates object's memory in these instances:
    // - object is created
    // - object/layer is cloned
    // - container with layers is expanded
    // - container with objects is expanded in the same layer
    // - container with parameters and/or vectorParameters is expanded in this object
    bool hasInvalidatedMemory = true;
    //If true clean all non-member pointers that could be deleted between event execution.
    bool executeGarbageCollector = false;

    AncestorObject();
    AncestorObject(size_t & topObjectUniqueIndex);
    void deleteLater();
    void clone(const AncestorObject &Original, vector<string> &listOfUniqueIDs, 
        size_t layerUniqueIndex, const string & newLayerID, bool changeOldID,
        size_t & topModuleUniqueIndex);
    void clearVectorsOfIDs();
    void clear();
    void refreshPositionsAndSizesOfObjectAndItsImages();
    void createVectorsOfIds();
    vec2d getPosOnCamera(Camera2D * SelectedCamera);
    void setID(const string & newID, vector<string> & listOfIDs);
    void primaryConstructor(PrimaryData & initData);
    void setIsScrollable(bool newValue);
    VariableModule getAttributeValue(const AttributeType & attribute, const string & detail);
    void clearAllEvents();
    void translateAllScripts(const string & exePath, bool clearEvents, bool allowNotAscii,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress,
        size_t & topModuleUniqueIndex);
    void translateScriptsFromPaths(const string & exePath, bool clearEvents,
        vector<string> scriptsPaths, bool allowNotAscii,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress,
        size_t & topModuleUniqueIndex);
    void translateSubsetBindedScripts(const string & exePath, bool clearEvents,
        vector<string> scripts, bool allowNotAscii, vector<VariableLocationStruct> & GlobalScope,
        unsigned & topMemoryAddress, size_t & topModuleUniqueIndex);
    void injectCode(bool clearEvents, vector<string> code,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress,
        size_t & topModuleUniqueIndex);
    void injectInstructions(bool clearEvents, const vector<string> & instructions,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress,
        size_t & topModuleUniqueIndex);
    void propagateLayer();
    void propagateObjectID();
    string getObjectID();
    std::pair<ReturnType, string> findIndexesOfEventChildren(bool postDelete = false);
};

#endif // ALLOBJECTS_H_INCLUDED