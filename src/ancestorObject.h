#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "eventModule.h"
#include "scrollbarModule.h"

/**
The most important class, a container for all modules that make an object.
*/
class AncestorObject: public PrimaryModule{
public:
    string layerID; //This ID is needed in events' trigger detection in the engine class. 
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
    bool isAddedToTriggeredObjects;
    AncestorObject();
    AncestorObject(int ancestorID, string newLayerID);
    void clearVectorsOfIDs();
    void clearContainers();
    void createButton(string bID, vec2d bPos, vec2d bSize, vec2d bImageSize, vec2d bImageScale, string bImageID, vector <SingleBitmap> & BitmapContainer, bool bIsScaledFromCenter);
    void operateEvent(int sourceID, int event, int operationID, vector <AncestorObject> & Objects);
    void operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects, vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs);
    void refreshCoordinates();
    void createVectorsOfIds();
    void triggerEvents();
};

void deactivateAllVectorsInEditorWindow(AncestorObject * EditorWindow);
void activateBasedOnId(AncestorObject * EditorWindow, string activateID);
void activateBasedOnFirstChar(AncestorObject * EditorWindow, char activateID);

void deactivateWrapped(int categoryIndex, AncestorObject * EditorWindow);
void manageWrapper(int categoryIndex, int wrapperIndex, AncestorObject * EditorWindow, double containerHeight);

template<class Module>
struct isStringInGroupModule {
    bool operator()(Module& Object) { return isCharInGroup(Object.getID()[0], 10, "l" /*It's L*/, "1", "2", "3", "4", "5", "6", "7", "8", "9");}
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
