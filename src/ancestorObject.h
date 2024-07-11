#ifndef ALLOBJECTS_H_INCLUDED
#define ALLOBJECTS_H_INCLUDED
#include "eventModule.h"

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
    vector <EveModule*> Events;
    vector <VariableModule*> Variables;
    vector <ScrollbarModule*> Scrollbars;
    vector <PrimitivesModule*> Primitives;
    vector <VectorModule*> Vectors;
    bool hasInstanceOfAnyModule() const;
    unsigned size() const;
};

struct WordStruct{
    char type; //o - operation, s - string, d - double, i - int, e - empty, c - context
    string value;
};

/**
The most important class, a container for all modules that make an object.
*/
class AncestorObject: public PrimaryModule{
public:
    vector <TextModule> TextContainer;
    vector <EditableTextModule> EditableTextContainer;
    vector <SuperTextModule> SuperTextContainer;
    vector <SuperEditableTextModule> SuperEditableTextContainer;
    vector <ImageModule> ImageContainer;
    vector <MovementModule> MovementContainer;
    vector <CollisionModule> CollisionContainer;
    vector <ParticleEffectModule> ParticlesContainer;
    vector <EveModule> EveContainer;
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
    vector <string> eveContainerIDs;
    vector <string> variablesContainerIDs;
    vector <string> scrollbarContainerIDs;
    vector <string> primitivesContainerIDs;
    vector <string> vectorContainerIDs;

    vector <string> bindedScripts;
    
    bool canBeMovedWithMouse;
    bool canDrawSelectionBorder;

    AncestorObject();
    void deleteLater();
    void clone(const AncestorObject& Original, vector <string> & listOfUniqueIDs, string newLayerID, const bool & changeOldID);
    void clearVectorsOfIDs();
    void clear();
    void operateTextFieldUpdate(EditableTextModule & EditableText, vector <AncestorObject> & Objects,
        vector <SingleBitmap> & BitmapContainer, vector <string> & listOfAncestorIDs, string workingDirectory
    );
    void refreshCoordinates();
    void createVectorsOfIds();
    vec2d getPosOnCamera(Camera2D * SelectedCamera);
    void setIsScrollable(bool newValue);
    VariableModule getAttributeValue(const string & attribute, const string & detail);
    /*Translate instructions into events and add them to the event container of the object.*/
    void eventAssembler(vector<string> code, string scriptName);
    void clearAllEvents();
    void translateAllScripts(bool clearEvents, bool allowNotAscii);
    void translateScriptsFromPaths(vector<string> scriptsPaths, bool allowNotAscii);
    void translateSubsetBindedScripts(vector<string> scripts, bool allowNotAscii);
    void injectCode(vector<string> code);
    void injectInstructions(vector<string> instructions);
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