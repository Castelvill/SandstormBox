#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "eventModule.h"
#include "scrollbarModule.h"

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
    bool hasInstanceOfAnyModule();
};

/**
The most important class, a container for all modules that make an object.
*/
class AncestorObject: public PrimaryModule{
public:
    string layerID; //This ID is needed in events' trigger detection in the engine class. Warning: DO NOT CHANGE IT IN CLONE METHOD.
    vector <TextModule> TextContainer;
    vector <EditableTextModule> EditableTextContainer;
    vector <ImageModule> ImageContainer;
    vector <MovementModule> MovementContainer;
    vector <CollisionModule> CollisionContainer;
    vector <ParticleEffectModule> ParticlesContainer;
    vector <EventModule> EventsContainer;
    vector <EveModule> EveContainer;
    vector <VariableModule> VariablesContainer;
    vector <ScrollbarModule> ScrollbarContainer;
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

    AncestorObject();
    AncestorObject(int ancestorID, string newLayerID);
    void clone(const AncestorObject& Original, bool isClonedFromDifferentLayer, vector <string> & listOfUniqueIDs);
    void clearVectorsOfIDs();
    void clearContainers();
    void createButton(string bID, vec2d bPos, vec2d bSize, vec2d bImageSize, vec2d bImageScale, string bImageID, vector <SingleBitmap> & BitmapContainer, bool bIsScaledFromCenter);
    void operateEvent(int sourceID, int event, int operationID, vector <AncestorObject> & Objects);
    void operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs);
    void refreshCoordinates();
    void createVectorsOfIds();
    vec2d getPosOnCamera(Camera2D * SelectedCamera);
    string addModuleInstance(string module, string newID);
    string destroyModuleInstance(string module, string destroyID);
    void bindModuleToContext(string moduleType, string moduleID, string attribute, ModulesPointers & AggregatedModules);
    void bindVariableToContext(string moduleType, string moduleID, string attribute, BasePointersStruct & UniversalVariable);
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
    if(!inStringVector(IDs, destroyID)){
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
