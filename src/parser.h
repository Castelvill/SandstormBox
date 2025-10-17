#include "eventModule.h"

using ScopeType = vector<vector<VariableLocationStruct>>;

struct Annotations{
    vector<TriggerType> triggersForNextEvent;
    bool override = false;
};

struct BranchingStackStruct{
    vector<unsigned> ifElseJumpStack; //Indexes of operations that will store line numbers for jumping to else_ifs and elses from ifs and else_ifs.
    vector<vector<unsigned>> ifEndJumpStack; //Indexes of operations that store line numbers for jumping to end_if labels from ifs, else_ifs and elses.
    vector<char> usedElseStatements; //If an else statement was used in an if statement store 1, otherwise store 0. This vector is required for clearing pointers in if statements without elses.  
    
    vector<unsigned> whileStartStack; //Indexes of operations that will store line number for jumping from "end_while" and "continue" labels to the "while" instruction.
    vector<vector<unsigned>> whileEndStack; //Indexes of operations that will store line number for jumping from "break" and "while" instructions to the "end_while" label.
};

struct InstrParser{
    const vector<WordStruct> & words;
    const string & scriptName;
    const unsigned & lineNumber;
    EventModule & NewEvent;
    ScopeType & Scopes;
    unsigned & topAddress;
    Annotations & annotations;

    OperationClass * Operation = nullptr;
    unsigned cursor = 1;

    ReturnType parseCompilerBreakpoint(bool & triggerBreakpoint);
    ReturnType parseAnnotations();
    ReturnType parseOverrideAnnotation();
    ReturnType parseTriggersAnnotation();
    ReturnType parseStartAndOverride(vector<string> & allAvailableEventIDs,
        const string & layerId, const string & objectId, vector<EventModule> &eventContainer, vector<string> &eventContainerIds
    );
    ReturnType parseEnd(vector<EventModule> &eventContainer);
    ReturnType parseEmpty();
    ReturnType parseIf(BranchingStackStruct & BranchingStack);
    ReturnType parseElseIf(BranchingStackStruct & BranchingStack);
    ReturnType parseElse(BranchingStackStruct & BranchingStack);
    ReturnType parseEndIf(BranchingStackStruct & BranchingStack);
    ReturnType parseWhile(BranchingStackStruct & BranchingStack);
    ReturnType parseEndWhile(BranchingStackStruct & BranchingStack);
    ReturnType parseContinue(BranchingStackStruct & BranchingStack);
    ReturnType parseBreak(BranchingStackStruct & BranchingStack);
    ReturnType parseFirstLastAllRandom(vector<string> & allAvailableEventIDs);
    ReturnType parseIndex();
    ReturnType parseIndexVec();
    ReturnType parseAddSubMulDivModPowRand();
    ReturnType parseAssert();
    ReturnType parseLoad();
    ReturnType parseMove();
    ReturnType parseFindById2();
    ReturnType parseSumIntersecDiffIn();
    ReturnType parseIncDecDelDemolishRbindType();
    ReturnType parseNext();
    ReturnType parseAccess();
    ReturnType parseBoolIntDoubleStringAndTheirVectors();
    ReturnType parseFindById();
    ReturnType parseClone();
    ReturnType parseNew();
    ReturnType parseBind();
    ReturnType parseBuild();
    ReturnType parseLoadBuildInject();
    ReturnType parseFunction();
    ReturnType parseEnv();
    ReturnType parseEditProc();
    ReturnType parseLoadBitmap();
    ReturnType parseMkdirRmRmll();
    ReturnType parseMv();
    ReturnType parsePrint();
    ReturnType parseLoadText();
    ReturnType parseSaveText();
    ReturnType parseLs();
    ReturnType parseLse();
    ReturnType parseNewProc();
    ReturnType parseVar();
    ReturnType parseVec();
    ReturnType parseTokenize();
    ReturnType parseTreePwdConsoleInput();
    ReturnType parseLen();
    ReturnType parseSize();
    ReturnType parseSubstr();
    ReturnType parseLoadFont();
    ReturnType parseCd();
    ReturnType parseSimilar();
    ReturnType parseCount();
    ReturnType parseCreateDisplay();
    ReturnType parseStartTimer();
    ReturnType parseStopTimer();
    ReturnType parseVarDefinition();
    ReturnType parseRun();
    ReturnType parseAutoRun();
};