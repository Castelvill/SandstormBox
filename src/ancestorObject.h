#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "eventModule.h"
#include <unordered_set>

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

struct BranchingStackStruct{
    vector<unsigned> ifElseJumpStack; //Indexes of operations that will store line numbers for jumping to else_ifs and elses from ifs and else_ifs.
    vector<vector<unsigned>> ifEndJumpStack; //Indexes of operations that store line numbers for jumping to end_if labels from ifs, else_ifs and elses.
    vector<char> usedElseStatements; //If an else statement was used in an if statement store 1, otherwise store 0. This vector is required for clearing pointers in if statements without elses.  
    
    vector<unsigned> whileStartStack; //Indexes of operations that will store line number for jumping from "end_while" and "continue" labels to the "while" instruction.
    vector<vector<unsigned>> whileEndStack; //Indexes of operations that will store line number for jumping from "break" and "while" instructions to the "end_while" label.
};

/**
The most important class, a container for all modules that make an object.
*/
class AncestorObject: public PrimaryModule{
public:
    string objectLookupID;
    vector <TextModule> TextContainer;
    vector <EditableTextModule> EditableTextContainer;
    vector <SuperTextModule> SuperTextContainer;
    vector <SuperEditableTextModule> SuperEditableTextContainer;
    vector <ImageModule> ImageContainer;
    vector <MovementModule> MovementContainer;
    vector <CollisionModule> CollisionContainer;
    vector <ParticleEffectModule> ParticlesContainer;
    vector <EventModule> EventContainer;
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
    vector <string> eventsContainerIDs;
    vector <string> EventContainerIDs;
    vector <string> variablesContainerIDs;
    vector <string> scrollbarContainerIDs;
    vector <string> primitivesContainerIDs;
    vector <string> vectorContainerIDs;

    vector <string> bindedScripts;
    
    bool canBeMovedWithMouse = false;
    bool canDrawSelectionBorder = false;

    AncestorObject();
    void deleteLater();
    void clone(const AncestorObject& Original, vector <string> & listOfUniqueIDs, string newLayerID, const bool & changeOldID);
    void clearVectorsOfIDs();
    void clear();
    void operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects,
        vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs, string workingDirectory
    );
    void refreshPositionsAndSizesOfObjectAndItsImages();
    void createVectorsOfIds();
    vec2d getPosOnCamera(Camera2D * SelectedCamera);
    void setID(string newID, vector<string> & listOfIDs);
    void primaryConstructor(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    void setIsScrollable(bool newValue);
    VariableModule getAttributeValue(const AttributeType & attribute, const string & detail);
    ReturnType translateTokensIntoEngineInstruction(
        const vector<WordStruct> & words, const string & scriptName, const unsigned & lineNumber,
        vector<vector<VariableLocationStruct>> & Scopes, unsigned & topAddress,
        bool & triggerBreakpoint, EventModule & NewEvent, vector<string> & allAvailableEventIDs,
        BranchingStackStruct & BranchingStack
    );
    /*Translate instructions into events and add them to the event container of the object.*/
    ReturnType assembleEvents(vector<string> & code, const string & scriptName,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
    );
    void clearAllEvents();
    void translateAllScripts(bool clearEvents, bool allowNotAscii, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress);
    void translateScriptsFromPaths(bool clearEvents, vector<string> scriptsPaths, bool allowNotAscii,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
    );
    void translateSubsetBindedScripts(bool clearEvents, vector<string> scripts, bool allowNotAscii,
        vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
    );
    void injectCode(bool clearEvents, vector<string> code, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress);
    void injectInstructions(bool clearEvents, vector<string> instructions, vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress);
    void propagateLayerID();
    void propagateObjectID();
};

vector <WordStruct> tokenizeCode(string input);


template<class Module>
bool removeModuleInstanceByID(vector <Module> & Container, string destroyID){
    auto foundInstance = std::find_if(begin(Container), end(Container), [destroyID](Module &Instance){
        return Instance.getID() == destroyID;
    });
    if(foundInstance != std::end(Container)){
        Container.erase(foundInstance);
        return true;
    }
    return false;
}

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