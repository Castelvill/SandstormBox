#include "parser.h"
#include <unordered_set>
#include <unordered_map>

vector<string> removeComments(const vector<string> & lines);
string findAndUseSpecialCharacters(string input);
std::pair<vector<WordStruct>, bool> tokenizeCode(const string & input);
void removeStringDuplicatesFromVector(vector<string> & stringVec);
ReturnType addImportsToBindedScripts(const string & exePath, const vector<string> & bindedScripts, vector<string> & allScriptsToAssemble);
vector<string> readLines(const string& filename, bool allowNotAscii) ;
ReturnType parseTokensAndAssembleEvents(vector<EventModule> &eventContainer, vector<string> &eventContainerIds,
    const string & layerId, const string & objectId, const vector<WordStruct> & words, const string & scriptName,
    unsigned lineNumber, ScopeType & Scopes, unsigned & topAddress, bool & triggerBreakpoint, EventModule & NewEvent,
    vector<string> & allAvailableEventIDs, BranchingStackStruct & BranchingStack, Annotations& annotations
);
/*Translate instructions into events and add them to the event container of the object.*/
ReturnType assembleEvents(vector<EventModule> &eventContainer, vector<string> &eventContainerIds,
    const string & layerId, const string & objectId, vector<string> & code, const string & scriptName,
    vector<VariableLocationStruct> & GlobalScope, unsigned & topMemoryAddress
);
std::pair<ReturnType, bool> compile(const string & exePath, const vector<string> & bindedScripts, bool allowNotAscii,
    vector<EventModule> &eventContainer, vector<string> &eventContainerIds, vector<VariableLocationStruct> & globalScope,
    unsigned & topMemoryAddress, const string & layerId, string & objectId
);

struct CodeGenerator{
    std::unordered_map<string, string> constants;

    vector<vector<WordStruct>> preprocessTokens(const vector<WordStruct> & inputTokens);
};

class Compiler{
public:
    
};