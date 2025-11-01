#include "parser.h"
#include <unordered_set>
#include <unordered_map>

vector<string> removeComments(const vector<string> & lines);
vector<string> divideStringIntoLines(const string & input);
vector<string> divideStringVectorIntoLines(const vector<string> & input);
vector<string> mergeStringSections(const vector<string> & input);
void removeStringDuplicatesFromVector(vector<string> & stringVec);
/*Translate instructions into events and add them to the event container of the object.*/
ReturnType assembleEvents(vector<EventModule> & eventContainer, vector<string> & eventContainerIds,
    size_t layerIndex, const string & layerId, size_t objectIndex, string & objectId,
    vector<string> & code, const string & scriptName, vector<VariableLocationStruct> & GlobalScope,
    unsigned & topMemoryAddress, size_t & topModuleUniqueIndex
);
std::pair<ReturnType, bool> compile(const string & exePath, const vector<string> & bindedScripts,
    bool allowNotAscii, vector<EventModule> &eventContainer, vector<string> &eventContainerIds,
    vector<VariableLocationStruct> & globalScope, unsigned & topMemoryAddress,
    size_t layerIndex, const string & layerId, size_t objectIndex, string & objectId,
    size_t & topModuleUniqueIndex
);

struct CodeGenerator{
    std::unordered_map<string, string> constants;

    vector<vector<WordStruct>> preprocessTokens(const vector<WordStruct> & inputTokens);
};

class Compiler{
public:
    
};