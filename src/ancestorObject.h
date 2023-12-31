#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "eventModule.h"
#include <fstream>

//Struct of vectors
struct ModulesPointers{
    vector <TextModule*> Texts;
    vector <EditableTextModule*> EditableTexts;
    vector <ImageModule*> Images;
    vector <MovementModule*> Movements;
    vector <CollisionModule*> Collisions;
    vector <ParticleEffectModule*> Particles;
    vector <EveModule*> Events;
    vector <VariableModule*> Variables;
    vector <ScrollbarModule*> Scrollbars;
    vector <PrimitivesModule*> Primitives;
    bool hasInstanceOfAnyModule() const;
    unsigned size() const;
};

/**
The most important class, a container for all modules that make an object.
*/
class AncestorObject: public PrimaryModule{
public:
    vector <TextModule> TextContainer;
    vector <EditableTextModule> EditableTextContainer;
    vector <ImageModule> ImageContainer;
    vector <MovementModule> MovementContainer;
    vector <CollisionModule> CollisionContainer;
    vector <ParticleEffectModule> ParticlesContainer;
    vector <EveModule> EveContainer;
    vector <VariableModule> VariablesContainer;
    vector <ScrollbarModule> ScrollbarContainer;
    vector <PrimitivesModule> PrimitivesContainer;
    vector <string> textContainerIDs;
    vector <string> editableTextContainerIDs;
    vector <string> imageContainerIDs;
    vector <string> movementContainerIDs;
    vector <string> collisionContainerIDs;
    vector <string> particlesContainerIDs;
    vector <string> eventsContainerIDs;
    vector <string> eveContainerIDs;
    vector <string> variablesContainerIDs;
    vector <string> scrollbarContainerIDs;
    vector <string> primitivesContainerIDs;
    vector <string> bindedScripts;

    AncestorObject();
    AncestorObject(string newID, vector<string> &listOfIDs, string newLayerID);
    AncestorObject(unsigned newID, vector<string> &listOfIDs, string newLayerID);
    void deleteLater();
    void clone(const AncestorObject& Original, vector <string> & listOfUniqueIDs, string newLayerID, const bool & changeOldID);
    void clearVectorsOfIDs();
    void clearContainers();
    void createButton(string bID, vec2d bPos, vec2d bSize, vec2d bImageSize, vec2d bImageScale, string bImageID, vector <SingleBitmap> & BitmapContainer, bool bIsScaledFromCenter);
    void operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs, string EXE_PATH);
    void refreshCoordinates();
    void createVectorsOfIds();
    vec2d getPosOnCamera(Camera2D * SelectedCamera);
    string addModuleInstance(string module, string newID);
    string destroyModuleInstance(string module, string destroyID);
    VariableModule getAttributeValue(const string & attribute, const string & detail);
    /*Translate instructions into events and add them to the event container of the object.*/
    void eventAssembler(vector<string> code, string scriptName);
    void clearAllEvents();
    void translateAllScripts(bool clearEvents);
    void translateScriptsFromPaths(vector<string> scriptsPaths);
    void translateSubsetBindedScripts(vector<string> scripts);
    void injectCode(vector<string> code);
    void injectInstructions(vector<string> instructions);
};

void deactivateAllVectorsInEditorWindow(AncestorObject * EditorWindow);
void activateBasedOnId(AncestorObject * EditorWindow, string activateID);
void activateBasedOnFirstChar(AncestorObject * EditorWindow, char activateID);

void deactivateWrapped(int categoryIndex, AncestorObject * EditorWindow);
void manageWrapper(int categoryIndex, int wrapperIndex, AncestorObject * EditorWindow, double containerHeight);


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

string SuccessInstanceAdded(string module, string ID);
string ErrorNoInstance(string module, string ID);
string SuccessInstanceDestroyed(string module, string ID);
template<class Module>
string tryRemovingModuleInstance(string module, vector <Module> & Container, vector <string> & IDs, string destroyID){
    if(!isStringInVector(IDs, destroyID)){
        return ErrorNoInstance(module, destroyID);
    }
    if(!removeModuleInstanceByID(Container, destroyID)){
        return ErrorNoInstance(module, destroyID);
    }
    removeFromStringVector(IDs, destroyID);
    return SuccessInstanceDestroyed(module, destroyID);
}

template<class Module>
struct isStringInGroupModule {
    bool operator()(Module& Object) {
        return isCharInGroup(Object.getID()[0], 10, "l" /*It's L*/, "1", "2", "3", "4", "5", "6", "7", "8", "9");
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