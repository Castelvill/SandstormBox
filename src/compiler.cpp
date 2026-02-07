#include "compiler.h"
#include <regex>
#include <queue>
#include "keymap.h"

bool canStringBeDouble(string text){
    if(text[0] == '.'){
        return false;
    }
    bool hasPoint = false;
    for(char a : text){
        if(int(a) < 48 || int(a) > 57){
            if(a == '.' && !hasPoint){
                hasPoint = true;
                continue;
            }
            return false;
        }
    }
    return hasPoint;
}

//Translate special letter pairs into special characters.
string findAndUseSpecialCharacters(const string & input){
    string output = "";
    for(size_t i = 0; i < input.size(); i++){
        if(input[i] == '\\' && i + 1 < input.size()){
            if(input[i + 1] == 'n'){
                output += '\n';
                i++;
                continue;
            }
            else if(input[i + 1] == 't'){
                output += '\t';
                i++;
                continue;
            }
        }
        output += input[i];
    }
    return output;
}

//For every '\n' or "\n" create a new vector element 
vector<string> divideStringIntoLines(const string & input){
    vector<string> output = {""};
    for(size_t i = 0; i < input.size(); i++){
        if(input[i] == '\n'){
            output.emplace_back();
            continue;
        }
        if(input[i] == '\\' && i + 1 < input.size()){
            if(input[i + 1] == 'n'){
                output.emplace_back();
                i++;
                continue;
            }
            if(input[i + 1] == '\"'){
                output.back() += '\"';
                i++;
                continue;
            }
        }
        output.back() += input[i];
    }
    return output;
}

//For every '\n' or "\n" create a new vector element 
vector<string> divideStringVectorIntoLines(const vector<string> & input){
    vector<string> outputWithoutNewLines;
    for(const string & codeSnippet : input){
        vector<string> lines = divideStringIntoLines(codeSnippet);
        outputWithoutNewLines.insert(outputWithoutNewLines.end(),
            std::make_move_iterator(lines.begin()),
            std::make_move_iterator(lines.end())
        );
    }
    return outputWithoutNewLines;
}

vector <string> mergeStrings(vector <string> code){
    vector <string> merged;
    string buffor = "";
    for(unsigned i = 0; i < code.size(); i++){
        buffor = code[i];
        if(code[i][0] == '\"'){
            buffor = "";
            i++;
            for(; i < code.size(); i++){
                if(code[i][0] == '\"'){
                    break;
                }
                if(i > 0 && buffor.size() > 0 && code[i-1][0] != '\\' &&
                    code[i-1][0] != '/' && code[i][0] != '\\' && code[i][0] != '/'
                    && code[i-1].back() != '\n' && code[i-1].back() != '\t'
                ){
                    buffor += " ";
                }
                buffor += code[i];
            }
        }
        merged.push_back(buffor);
    }
    return merged;
}

inline string mapEngineKeysToAllegroKeys(const string & key){
    auto it = engineKeysToAllegroKeys.find(key);
    if(it != engineKeysToAllegroKeys.end()){
        return it->second;;
    } 
    return "";
}

std::pair<vector<WordStruct>, bool> tokenizeCode(const string & input){
    std::regex word_regex("([\\w+\\.*]*\\w+)|;|:|\\,|\\.|==|=|>=|<=|>|<|-=|\\+=|\\*=|/=|\\*\\*|\\+\\+|\\-\\-|\\+|-|\\*|/|%|\\[|\\]|\\(|\\\\\\\"|\\)|\"|!=|!|\\|\\||&&|\n|\t|&|@|#", std::regex_constants::icase);
    auto words_begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
    auto words_end = std::sregex_iterator();

    bool triggerPreprocessor = false;

    vector <string> output;
    string match_str;

    for(std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        match_str = match.str();
        if (match_str.size() > 0){
            output.push_back(match_str);
        }
    }

    vector<string> stringSectors;
    bool isInsideStringSector = false;

    for(size_t i = 0; i < input.size(); i++){
        if(input[i] == '\"' && (i == 0 || input[i - 1] != '\\')){
            if(!isInsideStringSector){
                stringSectors.emplace_back("");
            }
            isInsideStringSector = !isInsideStringSector;
            continue;
        }
        if(isInsideStringSector){
            stringSectors.back() += input[i];
        }
    }

    vector <WordStruct> mergedOutput;
    isInsideStringSector = false;
    unsigned sector = 0;
    string error;
    bool keywordAquired = false;

    //If a word is not a part of a string, add it as a single word, otherwise add the whole string
    //as a single word and ignore its parts. Don't add " to the words.
    for(unsigned i = 0; i < output.size(); i++){
        if(output[i][0] == '\"' && !isInsideStringSector){
            isInsideStringSector = true;
            mergedOutput.emplace_back(WordStruct(TokenType::string_tk, stringSectors[sector], false));
            sector++;
            continue;
        }
        if(isInsideStringSector){
            if(output[i][0] == '\"'){
                isInsideStringSector = false;
            }
            continue;
        }
        if(output[i] == "\t"){
            continue;
        }
        if(!keywordAquired){
            mergedOutput.emplace_back(WordStruct(TokenType::keyword_tk, output[i], false, strToInstr(output[i], false)));
            if(mergedOutput.back().instruction == null){
                mergedOutput.back().type = TokenType::identifier_tk;
            }
            keywordAquired = true;
            continue;
        }
        if(isStringInGroup(output[i], 9, "!", "==", "!=", "<", "<=", ">", ">=", "||", "&&")){
            mergedOutput.emplace_back(WordStruct(TokenType::keyword_tk, output[i], false, strToInstr(output[i], false)));
            continue;
        }
        if(output[i] == "_"){
            mergedOutput.emplace_back(WordStruct(TokenType::empty_tk, output[i], false));
            continue;
        }
        if(output[i] == "("){
            triggerPreprocessor = false;
            mergedOutput.emplace_back(WordStruct(TokenType::start_expr_tk, output[i], false,
                EngineInstr::start_expr
            ));
            continue;
        }
        if(output[i] == ")"){
            mergedOutput.emplace_back(WordStruct(TokenType::end_expr_tk, output[i], false));
            continue;
        }
        if(output[i] == "{"){
            mergedOutput.emplace_back(WordStruct(TokenType::start_scope_tk, output[i], false));
            continue;
        }
        if(output[i] == "}"){
            mergedOutput.emplace_back(WordStruct(TokenType::end_scope_tk, output[i], false));
            continue;
        }
        if(output[i] == "["){
            mergedOutput.emplace_back(WordStruct(TokenType::open_brackets_tk, output[i], false));
            continue;
        }
        if(output[i] == "]"){
            mergedOutput.emplace_back(WordStruct(TokenType::close_brackets_tk, output[i], false));
            continue;
        }
        if(output[i] == "true"){
            mergedOutput.emplace_back(WordStruct(TokenType::bool_tk, "1", false));
            continue;
        }
        if(output[i] == "false"){
            mergedOutput.emplace_back(WordStruct(TokenType::bool_tk, "0", false));
            continue;
        }
        if(output[i].starts_with("KEY_")){
            const string & allegroMapping = mapEngineKeysToAllegroKeys(output[i]);
            if(!allegroMapping.empty()){
                mergedOutput.emplace_back(WordStruct(TokenType::int_tk, allegroMapping, false));
                continue;
            }
        }
        if(canStringBeDouble(output[i])){
            cstod(output[i], error);
            if(mergedOutput.size() > 0 && mergedOutput.back().type == TokenType::identifier_tk && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct(TokenType::double_tk, "-" + output[i], false));
            }
            else{
                mergedOutput.emplace_back(WordStruct(TokenType::double_tk, output[i], false));
            }
            continue;
        }
        cstoi(output[i], error);
        if(error == ""){
            if(mergedOutput.size() > 0 && mergedOutput.back().type == TokenType::identifier_tk && mergedOutput.back().value == "-"){
                mergedOutput.pop_back();
                mergedOutput.emplace_back(WordStruct(TokenType::int_tk, "-" + output[i], false));
            }
            else{
                mergedOutput.emplace_back(WordStruct(TokenType::int_tk, output[i], false));
            }
            
            continue;
        }
        if(mergedOutput.size() > 0 && mergedOutput.back().type == TokenType::identifier_tk && mergedOutput.back().value == "-"){
            mergedOutput.pop_back();
            mergedOutput.emplace_back(WordStruct(TokenType::identifier_tk, output[i], true));
        }
        else{
            mergedOutput.emplace_back(WordStruct(TokenType::identifier_tk, output[i], false));
        }
    }

    return {mergedOutput, triggerPreprocessor};
}

void removeStringDuplicatesFromVector(vector<string> & stringVec){
    std::unordered_set<string> encounteredStrings;
    for(auto iter = stringVec.begin(); iter != stringVec.end();){
        if(encounteredStrings.find(*iter) != encounteredStrings.end()){
            iter = stringVec.erase(iter);
        }
        else{
            encounteredStrings.insert(*iter);
            ++iter;
        }
    }
}

vector<string> removeComments(const vector<string> & lines){
    vector<string> newLines;
    unsigned cursor;
    bool commentSection = false;
    unsigned lineNumber = 1;
    for(const string & line : lines){
        newLines.emplace_back("");
        for(cursor = 0; cursor < line.size(); cursor++){
            if(line[cursor] == '/' && cursor < line.size() + 2){
                if(line[cursor + 1] == '/'){
                    break;
                }
                else if(line[cursor + 1] == '*'){
                    commentSection = true;
                    cursor++;
                    continue;
                }
            }
            if(line[cursor] == '*' && cursor < line.size() + 2 && line[cursor + 1] == '/'){
                if(!commentSection){
                    cout << "Error: Syntax error in line " << lineNumber << ".\n";
                    return newLines;
                }
                commentSection = false;
                cursor++;
                continue;
            }
            if(commentSection){
               continue;
            }
            newLines.back() += line[cursor];
        }
        lineNumber++;
    }
    
    return newLines;
}

inline vector<string> builtInScriptInterpreter = {
    "@trigger each_iteration",
    "start interpreter",
    "pwd path",
    "print path \">> \"",
    "console_input input",
    "inject_instr me input",
    "end"
};
inline vector<string> builtInScriptHello = {
    "@trigger on_init",
    "start interpreter",
    "print \"Hello, World!\n\"",
    "exit",
    "end"
};

vector<string> readLines(const string& filename, bool allowNotAscii) {
	if(filename.size() == 0){
        cerr << "Error: In " << __FUNCTION__ << ": The file name cannot be empty.\n";
        return vector<string>();
    }
    if(filename[0] == '?'){
        if(filename == "?interpreter"){
            return builtInScriptInterpreter;
        }
        if(filename == "?hello"){
            return builtInScriptHello;
        }
        cerr << "Error: In " << __FUNCTION__ << ": There is no built-in script with the name: \""
            << filename << "\".\n";
        return vector<string>();
    }
    vector<string> lines;

    std::ifstream File(filename);

	if(!File){
		std::cerr << "Cannot open file: " << filename << "\n";
        return lines;
	}
    if(allowNotAscii){
        for(string line; std::getline(File, line);){
            lines.push_back(line);
        }
    }
    else{
        for(string line; std::getline(File, line);){
            lines.emplace_back(removeNotAscii(line));
        }
    }
    
    File.close();

    lines = removeComments(lines);

	return lines;
}

ReturnType gatherImportsFromScript(const string & exePath, const string & scriptName, vector<string> & allImports){
    allImports.clear();
    vector<string> scriptLines = readLines(scriptName, false);
    if(scriptLines.size() == 0){
        return ReturnType::EMPTY;
    }
    vector<WordStruct> words;
    for(const string & line : scriptLines){
        bool triggerPreprocessor = false;
        std::tie(words, triggerPreprocessor) = tokenizeCode(line); //TODO: Use less expensive way to extract imports from lines
        if(words.size() == 0){
            continue;
        }
        if(words[0].value == "import"){
            if(words.size() == 1){
                cerr << "Error: Import requires at least one parameter.\n";
                continue;
            }
            if(words[1].value.size() == 0){
                cerr << "Error: First parameter of the import statement cannot be empty.\n";
                continue;
            }
            if(words[1].value == scriptName){
                allImports.clear();
                return ReturnType::ERROR_INF;
            }
            allImports.push_back(exePath + words[1].value);
            continue;
        }
    }
    return ReturnType::OK;
}

//Return a vector of scripts and their imports in left-to-right, depth-first order.
ReturnType addImportsToBindedScripts(const string & exePath, const vector<string> & bindedScripts, vector<string> & allScriptsToAssemble){
    if(bindedScripts.size() == 0){
        return OK;
    }
    
    vector<string> parentScripts;
    vector<std::queue<string>> scriptsToAnalyze;
    vector<string> subsequentImports;
    string currentScript;
    ReturnType importResult;

    scriptsToAnalyze.emplace_back(std::queue<string>());
    for(const string & script : bindedScripts){
        scriptsToAnalyze.back().push(script);
    }

    while(!scriptsToAnalyze.empty()){
        currentScript = scriptsToAnalyze.back().front();
        scriptsToAnalyze.back().pop();
        importResult = gatherImportsFromScript(exePath, currentScript, subsequentImports);
        if(importResult == ERROR_INF){
            return ReturnType::ERROR_INF;
        }
        if(subsequentImports.empty()){
            if(importResult != EMPTY){
                allScriptsToAssemble.push_back(currentScript);
            }
            if(scriptsToAnalyze.back().empty()){ //All children have been added to the final vector.
                scriptsToAnalyze.pop_back();
                while(!parentScripts.empty()){
                    allScriptsToAssemble.push_back(parentScripts.back());
                    parentScripts.pop_back();
                }
            }
        }
        else{
            if(scriptsToAnalyze.back().empty()){
                scriptsToAnalyze.pop_back();
            }
            //Check if imported scripts are not importing any parent scripts - if they are, return error to avoid a cycle.
            for(const string & parent : parentScripts){
                for(const string & importedScript : subsequentImports){
                    if(parent == importedScript){
                        return ReturnType::ERROR_INF;
                    }
                }
            }
            scriptsToAnalyze.emplace_back(std::queue<string>());
            for(const string & script : subsequentImports){
                scriptsToAnalyze.back().push(script);
            }
            parentScripts.push_back(currentScript);
        }
    }

    removeStringDuplicatesFromVector(allScriptsToAssemble);

    return ReturnType::OK;
}

ReturnType parseTokensAndAssembleEvents(vector<EventModule> &eventContainer,
    vector<string> &eventContainerIds, const size_t layerIndex, const string & layerId,
    const size_t objectIndex, const string & objectId, vector<WordStruct> & words,
    const string & scriptName, unsigned lineNumber, ScopeType & Scopes, unsigned & topAddress,
    bool & triggerBreakpoint, EventModule & NewEvent, vector<string> & allAvailableEventIDs,
    BranchingStackStruct & BranchingStack, Annotations & annotations, size_t & topModuleUniqueIndex
){
    switch(words[0].instruction){
        case EngineInstr::import:
        case EngineInstr::annotation_i:
        case EngineInstr::start:
            break;
        default:
            if(Scopes.size() < 2){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ":\n"
                    << NEW_LINE_PADDING << "Instruction \'" << words[0].value
                    << "\' cannot be used outside of an event scope.\n";
                return ReturnType::ERROR;
            }
            break;
    }

    InstrParser instrParser(words, scriptName, lineNumber, NewEvent, Scopes, topAddress,
        annotations
    );

    switch(words[0].instruction){
        case import:
            return ReturnType::OK;
        case compiler_breakpoint:
            return instrParser.parseCompilerBreakpoint(triggerBreakpoint);
        case annotation_i:
            return instrParser.parseAnnotations();
        case start:
            return instrParser.parseStartAndOverride(allAvailableEventIDs, layerIndex, layerId,
                objectIndex, objectId, eventContainer, eventContainerIds, topModuleUniqueIndex
            );
        case end_i:
            return instrParser.parseEnd(eventContainer, BranchingStack);
        case if_i:
            return instrParser.parseIf(BranchingStack);
        case else_if:
            return instrParser.parseElseIf(BranchingStack);
        case else_i:
            return instrParser.parseElse(BranchingStack);
        case end_if:
            return instrParser.parseEndIf(BranchingStack);
        case while_i:
            return instrParser.parseWhile(BranchingStack);
        case end_while:
            return instrParser.parseEndWhile(BranchingStack);
        case continue_i:
            return instrParser.parseContinue(BranchingStack);
        case break_i:
            return instrParser.parseBreak(BranchingStack);
        case return_i:
        case reboot:
        case exit_i:
        case delete_this_event:
        case reset_keyboard:
        case dump_context_stack:
        case dump_memory:
        case dump_local_memory:
        case restart_drag:
        case breakpoint:
            return instrParser.parseEmpty();
        case first:
        case last:
        case all:
        case random_i:
            return instrParser.parseFirstLastAllRandom(allAvailableEventIDs);
        case index_i:
            return instrParser.parseIndex();
        case index_vec:
            return instrParser.parseIndexVec();
        case add:
        case sub:
        case mul:
        case div_i:
        case mod:
        case pow_i:
        case rand_int:
        case rand_double:
            return instrParser.parseAddSubMulDivModPowRand();
        case assert:
            return instrParser.parseAssert();
        case load_i:
            return instrParser.parseLoad();
        case move:
        case add_move:
        case sub_move:
        case mul_move:
        case div_move:
            return instrParser.parseMove();
        case find_by_id_2:
            return instrParser.parseFindById2();
        case sum:
        case intersection:
        case diff:
        case in:
            return instrParser.parseSumIntersecDiffIn();
        case inc:
        case dec:
        case del:
        case demolish:
        case rbind_i:
        case type:
            return instrParser.parseIncDecDelDemolishRbindType();
        case next:
            return instrParser.parseNext();
        case access_i:
            return instrParser.parseAccess();
        case bool_i:
        case int_i:
        case double_i:
        case string_i:
        case bool_vec_i:
        case int_vec_i:
        case double_vec_i:
        case string_vec_i:
            return instrParser.parseBoolIntDoubleStringAndTheirVectors();
        case find_by_id:
            return instrParser.parseFindById();
        case clone_i:
            return instrParser.parseClone();
        case new_i:
            return instrParser.parseNew();
        case bind_i:
            return instrParser.parseBind();
        case build:
            return instrParser.parseBuild();
        case load_build:
        case build_subset:
        case inject_code:
        case inject_instr:
            return instrParser.parseLoadBuildInject();
        case fun:
            return instrParser.parseFunction();
        case env:
            return instrParser.parseEnv();
        case edit_proc:
            return instrParser.parseEditProc();
        case load_bitmap:
            return instrParser.parseLoadBitmap();
        case mkdir_i:
        case rm:
        case rmll:
            return instrParser.parseMkdirRmRmll();
        case mv_i:
            return instrParser.parseMv();
        case print_v_i:
        case print_d_i:
        case print_i:
            return instrParser.parsePrint();
        case load_text:
            return instrParser.parseLoadText();
        case save_text:
            return instrParser.parseSaveText();
        case ls:
            return instrParser.parseLs();
        case lse:
            return instrParser.parseLse();
        case new_proc:
            return instrParser.parseNewProc();
        case var:
            return instrParser.parseVar();
        case vec:
            return instrParser.parseVec();
        case tokenize:
            return instrParser.parseTokenize();
        case tree:
        case pwd:
        case console_input:
            return instrParser.parseTreePwdConsoleInput();
        case len:
            return instrParser.parseLen();
        case size_i:
            return instrParser.parseSize();
        case substr:
            return instrParser.parseSubstr();
        case load_font:
            return instrParser.parseLoadFont();
        case cd:
            return instrParser.parseCd();
        case similar:
            return instrParser.parseSimilar();
        case count:
            return instrParser.parseCount();
        case create_display:
            return instrParser.parseCreateDisplay();
        case start_timer:
            return instrParser.parseStartTimer();
        case stop_timer:
            return instrParser.parseStopTimer();
        case Val:
        case ValVec:
        case Pointer:
        case PointerVec:
        case Camera:
        case CameraVec:
        case Layer:
        case LayerVec:
        case Object:
        case ObjectVec:
        case Var:
        case VarVec:
        case Vec:
        case VecVec:
        case Text:
        case TextVec:
        case EditText:
        case EditTextVec:
        case SText:
        case STextVec:
        case SEditText:
        case SEditTextVec:
        case Image:
        case ImageVec:
        case Movement:
        case MovementVec:
        case Collision:
        case CollisionVec:
        case Particles:
        case Event:
        case EventVec:
        case Scrollbar:
        case ScrollbarVec:
        case Primitive:
        case PrimitiveVec:
        case any:
            return instrParser.parseVarDefinition();
        case run:
            return instrParser.parseRun();
        default:
            return instrParser.parseAutoRun();
    }
}

vector<string> mergeStringSections(const vector<string> & input){
    vector<string> mergedInput = {""};
    bool stringSection = false;
    size_t emptyLinesCount = 0;
    for(size_t line = 0; line < input.size(); line++){
        for(size_t ch = 0; ch < input[line].size(); ch++){
            if(input[line][ch] == '\"'){
                if(ch != 0 && input[line][ch-1] == '\\'){
                    continue;
                }
                stringSection = !stringSection;
            }
        }
        mergedInput.back() += input[line];
        if(!stringSection){
            mergedInput.emplace_back("");
            for(; emptyLinesCount > 0; emptyLinesCount--){
                mergedInput.emplace_back("");
            }
        }
        else{
            mergedInput.back() += '\n';
            emptyLinesCount++;
        }
    }
    return mergedInput;
}

vector<string> mergeStringSectionsAndUseSpecialSigns(const vector<string> & input){
    vector<string> mergedInput = {""};
    bool stringSection = false;
    size_t emptyLinesCount = 0;
    for(size_t line = 0; line < input.size(); line++){
        for(size_t ch = 0; ch < input[line].size(); ch++){
            if(input[line][ch] == '\"'){
                if(ch != 0 && input[line][ch-1] == '\\'){
                    continue;
                }
                stringSection = !stringSection;
            }
        }
        mergedInput.back() += input[line];
        if(!stringSection){
            mergedInput.emplace_back("");
            for(; emptyLinesCount > 0; emptyLinesCount--){
                mergedInput.emplace_back("");
            }
        }
        else{
            mergedInput.back() += '\n';
            emptyLinesCount++;
        }
    }
    return mergedInput;
}

ReturnType assembleEvents(vector<EventModule> & eventContainer, vector<string> & eventContainerIds,
    size_t layerIndex, const string & layerId, size_t objectIndex, const string & objectId,
    vector<string> & code, const string & scriptName, vector<VariableLocationStruct> & GlobalScope,
    unsigned & topMemoryAddress, size_t & topModuleUniqueIndex
){
    if(layerIndex == 0){
        cerr << "Error: In " << scriptName << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Unique index of layer is 0. Compilation aborted.\n";
        return ReturnType::ERROR;
    }
    if(objectIndex == 0){
        cerr << "Error: In " << scriptName << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Unique index of object is 0. Compilation aborted.\n";
        return ReturnType::ERROR;
    }
    if(layerId.empty()){
        cerr << "Error: In " << scriptName << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Layer id empty. Compilation aborted.\n";
        return ReturnType::ERROR;
    }
    if(objectId.empty()){
        cerr << "Error: In " << scriptName << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Object id empty. Compilation aborted.\n";
        return ReturnType::ERROR;
    }
    
    code = mergeStringSections(code);

    vector<WordStruct> words;
    EventModule NewEvent;
    unsigned lineNumber = 0;
    bool triggerBreakpoint = false;
    bool assembleEvents = true;
    bool printTokens = false;

    vector<string> allAvailableEventIDs;

    ScopeType Scopes;
    Scopes.push_back(GlobalScope);

    BranchingStackStruct BranchingStack;
    CodeGenerator codeGenerator;
    Annotations annotations;

    for(string line : code){
        lineNumber++;
        words.clear();
        line = findAndUseSpecialCharacters(line);

        bool triggerPreprocessor = false;
        
        std::tie(words, triggerPreprocessor) = tokenizeCode(line);

        if(words.empty()){
            continue;
        }

        if(triggerBreakpoint){
            raise(SIGINT);
            triggerBreakpoint = false;
        }

        //Compilation-time instructions
        bool resumeLoop = false;
        switch(words[0].instruction){
            case EngineInstr::resume_assembling:
                assembleEvents = true;
                break;
            case EngineInstr::stop_assembling:
                assembleEvents = false;
                break;
            case EngineInstr::start_printing_tokens:
                printTokens = true;
                break;
            case EngineInstr::stop_printing_tokens:
                printTokens = false;
                break;
            default:
                resumeLoop = true;
                break;
        }

        if(printTokens){
            for(const WordStruct & word : words){
                if(word.negateVariable){
                    cout << "neg ";
                }
                cout << "[" << tokenToStr(word.type) << ", " << instrToStr(word.instruction)
                    << ", \"" << word.value << "\"] ";
            }
            cout << "\n";
            cout.flush();
        }

        if(!assembleEvents || !resumeLoop){
            continue;
        }

        if(triggerPreprocessor){
            vector<vector<WordStruct>> preprocessedWords = codeGenerator.preprocessTokens(words);
            for(auto finalWords : preprocessedWords){
                ReturnType result = parseTokensAndAssembleEvents(
                    eventContainer, eventContainerIds, layerIndex, layerId, objectIndex, objectId,
                    finalWords, scriptName, lineNumber, Scopes, topMemoryAddress, triggerBreakpoint,
                    NewEvent, allAvailableEventIDs, BranchingStack, annotations,
                    topModuleUniqueIndex
                );
                if(result == ReturnType::ERROR){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__
                        << ": Compilation aborted due to previous errors.\n";
                    return ReturnType::ERROR;
                }
            }
        }
        else{
            ReturnType result = parseTokensAndAssembleEvents(
                eventContainer, eventContainerIds, layerIndex, layerId, objectIndex, objectId,
                words, scriptName, lineNumber, Scopes, topMemoryAddress, triggerBreakpoint,
                NewEvent, allAvailableEventIDs, BranchingStack, annotations, topModuleUniqueIndex
            );
            if(result == ReturnType::ERROR){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": Compilation aborted due to previous errors.\n";
                return ReturnType::ERROR;
            }
        }
    }
    
    if(words.size() > 0){
        if(words[0].value != "end"){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": Every event must end with 'end' instruction.\n";
        }
    }

    if(BranchingStack.ifElseJumpStack.size() > 0 || BranchingStack.ifEndJumpStack.size() > 0
        || BranchingStack.usedElseStatements.size() > 0
    ){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Every if statement must end with '"
            << instrToStr(end_if) << "' label.\n";
    }

    if(BranchingStack.whileStartStack.size() > 0 || BranchingStack.whileEndStack.size() > 0 ){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Every while loop must end with '"
            << instrToStr(end_while) << "' label.\n";
    }

    if(Scopes.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Global scope has been destroyed.\n";
        return ReturnType::ERROR;
    }
    if(Scopes[0].size() < GlobalScope.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Global variables has been deleted.\n";
        return ReturnType::ERROR;
    }

    //If new global variables were defined, add them to the global scope.
    if(Scopes[0].size() > GlobalScope.size()){
        GlobalScope = Scopes[0];
    }

    return ReturnType::OK;
}
inline void printEmptyFileWarning(string scriptName, string functionName){
    cout << "Warning: In " << functionName
        << ": Script '" << scriptName << "' is empty or cannot be opened.\n";
}

std::pair<ReturnType, bool> compile(const string & exePath, const vector<string> & bindedScripts,
    bool allowNotAscii, vector<EventModule> &eventContainer, vector<string> &eventContainerIds,
    vector<VariableLocationStruct> & globalScope, unsigned & topMemoryAddress,
    size_t layerIndex, const string & layerId, size_t objectIndex, const string & objectId,
    size_t & topModuleUniqueIndex
){
    vector<string> allScriptsToAssemble;
    ReturnType status = addImportsToBindedScripts(exePath, bindedScripts, allScriptsToAssemble);
    if(status == ReturnType::ERROR_INF){
        cerr << "Error: Assembler preprocessor detected a cycle in the imports.\n";
        return {ReturnType::ERROR_INF, false};
    }
    
    vector<string> code;
    bool anyAssembledEvents = false;
    
    for(const string & scriptName : allScriptsToAssemble){
        code = readLines(scriptName, allowNotAscii);
        if(!code.empty()){
            anyAssembledEvents = true;
            size_t preAssemblyEventCount = eventContainer.size();
            size_t preAssemblyModuleUniqueIndex = topModuleUniqueIndex;
            ReturnType assemblyStatus = assembleEvents(eventContainer, eventContainerIds,
                layerIndex, layerId, objectIndex, objectId, code, scriptName, globalScope,
                topMemoryAddress, topModuleUniqueIndex
            );
            if(assemblyStatus == ReturnType::ERROR){
                eventContainer.resize(preAssemblyEventCount);
                topModuleUniqueIndex = preAssemblyModuleUniqueIndex;
                return {ReturnType::ERROR, anyAssembledEvents};
            }
            code.clear();
        }
        else{
            printEmptyFileWarning(scriptName, __FUNCTION__);
        }
    }

    return {ReturnType::OK, anyAssembledEvents};
}