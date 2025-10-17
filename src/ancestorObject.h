#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "compiler.h"

DataType vectorizeEntityDataType(const InstrDescription & CurrentInstr, const DataType & oldType);

//Struct of vectors
struct ModulesPointers{
    vector <TextModule*> Texts;
    vector <EditableTextModule*> EditableTexts;
    vector <SuperTextModule*> SuperTexts;
    vector <SuperEditableTextModule*> SuperEditableTexts;
    vector <ImageModule*> Images;
    vector <MovementModule*> Movements;
    vector <CollisionModule*> Collisions;
    vector <ParticleEffectModule*> Particles;
    vector <EventModule*> Events;
    vector <VariableModule*> Variables;
    vector <ScrollbarModule*> Scrollbars;
    vector <PrimitivesModule*> Primitives;
    vector <VectorModule*> Vectors;
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
    size_t uniqueIndex = 0;
    vector <TextModule> TextContainer;
    vector <EditableTextModule> EditableTextContainer;
    vector <SuperTextModule> SuperTextContainer;
    vector <SuperEditableTextModule> SuperEditableTextContainer;
    vector <ImageModule> ImageContainer;
    vector <MovementModule> MovementContainer;
    vector <CollisionModule> CollisionContainer;
    vector <ParticleEffectModule> ParticlesContainer;
    vector <EventModule> EventContainer;
    //Other name: Parameters
    vector <VariableModule> VariablesContainer;
    vector <ScrollbarModule> ScrollbarContainer;
    vector <PrimitivesModule> PrimitivesContainer;
    vector <VectorModule> VectorContainer;
    vector <string> textContainerIDs;
    vector <string> editableTextContainerIDs;
    vector <string> superTextContainerIDs;
    vector <string> superEditableTextContainerIDs;
    vector <string> imageContainerIDs;
    vector <string> movementContainerIDs;
    vector <string> collisionContainerIDs;
    vector <string> particlesContainerIDs;
    vector <string> eventContainerIDs;
    vector <string> variablesContainerIDs;
    vector <string> scrollbarContainerIDs;
    vector <string> primitivesContainerIDs;
    vector <string> vectorContainerIDs;

    vector <string> bindedScripts;
    
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
    void deleteLater();
    void clone(const AncestorObject& Original, vector <string> & listOfUniqueIDs, string newLayerID,
        const bool & changeOldID, size_t & topUniqueIndex);
    void clearVectorsOfIDs();
    void clear();
    void operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects,
        vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs, string workingDirectory
    );
    void refreshPositionsAndSizesOfObjectAndItsImages();
    void createVectorsOfIds();
    vec2d getPosOnCamera(Camera2D * SelectedCamera);
    void setID(string newID, vector<string> & listOfIDs);
    void primaryConstructor(string newID, vector<string> *listOfIDs, string newLayerID, 
        string newObjectID, size_t & topUniqueIndex
    );
    void setIsScrollable(bool newValue);
    VariableModule getAttributeValue(const AttributeType & attribute, const string & detail);
    void clearAllEvents();
    void translateAllScripts(const string & exePath, bool clearEvents, bool allowNotAscii, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress);
    void translateScriptsFromPaths(const string & exePath, bool clearEvents, vector<string> scriptsPaths,
        bool allowNotAscii, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
    );
    void translateSubsetBindedScripts(const string & exePath, bool clearEvents, vector<string> scripts, bool allowNotAscii,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
    );
    void injectCode(bool clearEvents, vector<string> code, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress);
    void injectInstructions(bool clearEvents, vector<string> instructions, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress);
    void propagateLayerID();
    void propagateObjectID();
};

std::pair<vector<WordStruct>, bool> tokenizeCode(const string & input);


template<class Module>
struct isStringInGroupModule {
    bool operator()(Module& Object) {
        return isCharInGroup(Object.getID()[0], 10, 'l' /*It's L*/, '1', '2', '3', '4', '5', '6', '7', '8', '9');
    }
};
template<class Module>
struct isIDEqualString {
    isIDEqualString(string ID) : activateID(ID) {}
    string activateID;
    bool operator()(Module& Object) { return Object.getID() == activateID;}
};
template<class Module>
struct isIDEqualChar {
    isIDEqualChar(char ID) : activateID(ID) {}
    char activateID;
    bool operator()(Module& Object) { return Object.getID()[0] == activateID;}
};
template<class Module>
struct deactivateModule {
    void operator()(Module& Object) { Object.deactivate();}
};
template<class Module>
struct activateModule {
    void operator()(Module& Object) { Object.activate();}
};

#endif // ALLOBJECTS_H_INCLUDED