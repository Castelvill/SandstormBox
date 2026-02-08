#include "parser.h"

bool prepareNewInstruction(const vector<WordStruct> &words, EventModule &NewEvent, OperationClass *&Operation,
    unsigned minLength, unsigned lineNumber, const string &scriptName
){
    if(words.size() < minLength){
        if(minLength == 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least 1 parameter.\n";
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least " << minLength-1 << " parameters.\n";
        }
        return false;
    }

    NewEvent.Operations.emplace_back(OperationClass());
    NewEvent.Operations.back().instruction = strToInstr(words[0].value);
    NewEvent.Operations.back().scriptName = scriptName;
    NewEvent.Operations.back().lineNumber = lineNumber;
    Operation = &NewEvent.Operations.back();
    
    return true;
}


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
bool createEvent(const string & scriptName, const unsigned & lineNumber, const size_t layerIndex,
    const string & layerID, const size_t objectIndex, const string & objectID,
    vector <EventModule> & EventContainer, vector<string> & EventContainerIDs,
    EventModule & NewEvent, const vector<WordStruct> & words, ScopeType & Scopes,
    unsigned & topAddress, bool override, size_t & topModuleUniqueIndex
){
    string eventID = "";
    
    if(words.size() < 2){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value
            << "\' requires 1 parameter.\n";
        return true;
    }
    if(words[1].type != TokenType::identifier_tk && words[1].type != TokenType::empty_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": In the '" << words[0].value
            << "' instruction: The first parameter (id) is not a context.\n";
        return true;
    }

    eventID = words[1].value;
    
    if(eventID[0] != '_' && isStringInVector(EventContainerIDs, eventID)){
        if(override){
            removeFromStringVector(EventContainerIDs, eventID);
            removeModuleInstanceByID(EventContainer, eventID);
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Event with id \'" << eventID 
                << "\' already exists.\n";
            return true;
        }
    }
    PrimaryData initData = {
        .topIndex = &topModuleUniqueIndex,
        .objectUniqueIndex = objectIndex,
        .layerUniqueIndex = layerIndex,
        .newID = eventID,
        .listOfIDs = &EventContainerIDs,
        .newLayerID = layerID,
        .newObjectID = objectID
    };
    NewEvent = EventModule(initData);
    
    Scopes.emplace_back(vector<VariableLocationStruct>()); //Create a new scope

    unsigned cursor = 2;
    
    if(NewEvent.getPassedVariables(words, cursor, lineNumber, scriptName, Scopes, topAddress)){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return true;
    }
    return false;
}

inline bool isLogicInstr(EngineInstr instruction){
    switch(instruction){
        case EngineInstr::not_i:
        case EngineInstr::equal:
        case EngineInstr::not_equal:
        case EngineInstr::less:
        case EngineInstr::less_equal:
        case EngineInstr::more:
        case EngineInstr::more_equal:
        case EngineInstr::or_i:
        case EngineInstr::and_i:
            return true;
        default:
            return false;
    }
}
bool nextCond(const vector<WordStruct> & words, unsigned & cursor, VariableModule & Variable,
    const char & type, const string & scriptName, unsigned lineNumber
){
    string error = "";
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
        switch(type){   
            case 'b':
                Variable.setBool(cstoi(words[cursor].value, error));
                if(error.size() > 0){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
                }
                break;
            case 'i':
                Variable.setInt(cstoi(words[cursor].value, error));
                if(error.size() > 0){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
                }
                break;
            case 'd':
                Variable.setDouble(cstod(words[cursor].value, error));
                if(error.size() > 0){
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
                }
                break;
            case 's':
                if(words[cursor].type == TokenType::string_tk){
                    Variable.setString(words[cursor].value);
                }
                else{
                    cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                        << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter nr." << cursor << " is not a string.\n";
                }
                break;
            default:
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter nr."
                    << cursor << " has invalid type: '" << type << "'.\n";
                break;
        }
    }
    if(words[cursor].type != TokenType::close_brackets_tk){
        cursor++;
    }
    return false;
}
inline bool getLocalAddressWithError(unsigned & localAddress, const string & variableId,
    ScopeType &Scopes, vector<VariableInfo> &NewLocalVariables,
    unsigned &topAddress, const string &scriptName, const unsigned &lineNumber
){
    ReturnType result = ReturnType::OK;
    std::tie(localAddress, result) = getLocalAddress(
        variableId, any_dt, Scopes, NewLocalVariables, topAddress, false
    );
    switch(result){
        case ReturnType::OUT_OF_SCOPE:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
                << "Index (" << localAddress
                << ") of a variable '" << variableId
                << "' is out of scope (" << NewLocalVariables.size() << ").\n";
            localAddress = 0;
            return true;
        case ReturnType::UNDEFINED:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
                << "Variable '" << variableId << "' is undefined.\n";
            localAddress = 0;
            return true;
        default:
            return false;
    }
}
inline void setOptionalAddressInCond(const size_t & index, const vector<WordStruct> & words, unsigned & localAddress,
    const string & scriptName, unsigned lineNumber, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable
){
    if(index >= words.size() || words[index].type == TokenType::empty_tk){
        return;
    }
    if(words[index].type == TokenType::identifier_tk){
        getLocalAddressWithError(localAddress, words[index].value,
            Scopes, NewLocalVariables, topAddress, scriptName, lineNumber
        );
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter '" << words[index].value << "' is not a context.\n";
    }
}
inline string getStringByIndex(const vector<string> & strVec, const size_t & index) noexcept {
    if(index >= strVec.size()){
        return "";
    }
    return strVec[index];
}
DataType instrToDataType(EngineInstr instruction){
    switch(instruction){
        case Val:
            return DataType::value_inst;
        case ValVec:
            return DataType::value_vec;
        case Pointer:
            return DataType::pointer_inst;
        case PointerVec:
            return DataType::pointer_vec;
        case Camera:
            return DataType::camera_inst;
        case CameraVec:
            return DataType::camera_vec;
        case Layer:
            return DataType::layer_inst;
        case LayerVec:
            return DataType::layer_vec;
        case Object:
            return DataType::object_inst;
        case ObjectVec:
            return DataType::object_vec;
        case Var:
            return DataType::variable_mod;
        case VarVec:
            return DataType::variable_mod_vec;
        case Vec:
            return DataType::vector_mod;
        case VecVec:
            return DataType::vector_mod_vec;
        case SText:
            return DataType::super_text_mod;
        case STextVec:
            return DataType::super_text_mod_vec;
        case SEditText:
            return DataType::super_editable_text_mod;
        case SEditTextVec:
            return DataType::super_editable_text_mod_vec;
        case Image:
            return DataType::image_mod;
        case ImageVec:
            return DataType::image_mod_vec;
        case Movement:
            return DataType::movement_mod;
        case MovementVec:
            return DataType::movement_mod_vec;
        case Collision:
            return DataType::collision_mod;
        case CollisionVec:
            return DataType::collision_mod_vec;
        case Particles:
            return DataType::particles_mod;
        case Event:
            return DataType::event_mod;
        case EventVec:
            return DataType::event_mod_vec;
        case Scrollbar:
            return DataType::scrollbar_mod;
        case ScrollbarVec:
            return DataType::scrollbar_mod_vec;
        case Primitive:
            return DataType::primitives_mod;
        case PrimitiveVec:
            return DataType::primitives_mod_vec;
        case any:
            return DataType::any_dt;
        default:
            return DataType::null_dt;
    }
}
void setOptionalAttributeValueInCond(const size_t & index, const char & type,
    const vector<WordStruct> & words, VariableModule & Variable, const string & scriptName,
    const unsigned & lineNumber
){
    if(index >= words.size() || words[index].type == TokenType::empty_tk){
        return;
    }
    string error = "";
    switch(type){
        case 'b':
            Variable.setBool(cstoi(words[index].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
            }
            return;
        case 'i':
            Variable.setInt(cstoi(words[index].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
            }
            return;
        case 'd':
            Variable.setDouble(cstod(words[index].value, error));
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": " << error << "\n";
            }
            return;
        case 's':
            if(words[index].type == TokenType::string_tk){
                Variable.setString(words[index].value);
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parameter '" << words[index].value << "' is not a string.\n";
            }
            return;
        default:
            return;
    }
}
inline bool prepareVectorSource(const vector<string> & attributes, const vector<WordStruct> & attributeArgs,
    const unsigned & lineNumber, const string & scriptName, ConditionClass & Expression,
    ScopeType & Scopes, vector<VariableInfo> & NewLocalVariables, unsigned & topAddress,
    const bool & canCreateNewVariable
){
    Expression.Location.moduleID = getStringByIndex(attributes, 1);
    getLocalAddressWithError(Expression.localAddresses[0], Expression.Location.moduleID,
        Scopes, NewLocalVariables, topAddress, scriptName, lineNumber
    );
    Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 2));
    switch(Expression.Location.attribute){
        case index_a:
            setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            Expression.Location.attribute = value;
            break;
        case context_a:
            if(!attributeArgs.empty()){ //Set variableId
                Expression.Literal.setString(attributeArgs[0].value);
            }
            setOptionalAddressInCond(0, attributeArgs, Expression.localAddresses[1],
                scriptName, lineNumber, Scopes, NewLocalVariables, topAddress, canCreateNewVariable
            );
            Expression.Location.attribute = value;
            break;
        case size_a:
            Expression.Location.attribute = size_a;
            break;
        case back_a:
            Expression.Location.attribute = back_a;
            break;
        case null_a:
            break;
        default:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Invalid attribute '"
                << attributeToStr(Expression.Location.attribute) << "'.\n";
            return true;
    }
    return false;
}
inline bool setLocalAddressForExpression(ConditionClass &Expression, const vector<string> &attributes,
    ScopeType &Scopes, vector<VariableInfo> &NewLocalVariables,
    unsigned &topAddress, const string &scriptName, const unsigned &lineNumber
){
    if(getLocalAddressWithError(Expression.localAddresses[0], getStringByIndex(attributes, 0),
        Scopes, NewLocalVariables, topAddress, scriptName, lineNumber
    )){ return true; }

    Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 1));
    Expression.Location.spareID = getStringByIndex(attributes, 2);

    return false;
}
inline void prepareObjectSource(const vector<string> & attributes, const vector<WordStruct> & attributeArgs,
    const unsigned & lineNumber, const string & scriptName, ConditionClass & Expression
){
    Expression.Location.layerID = getStringByIndex(attributes, 1);
    Expression.Location.objectID = getStringByIndex(attributes, 2);
    Expression.Location.moduleType = strToSource(getStringByIndex(attributes, 3));
    Expression.Location.moduleID = getStringByIndex(attributes, 4);
    Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 5));

    switch(Expression.Location.moduleType){
        case ancestor:
            setOptionalAttributeValueInCond(0, 's', attributeArgs, Expression.Literal, scriptName, lineNumber);
            return;
        case mouse:
            setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            return;
        case collision:
            setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            if(attributeArgs.size() > 1){
                Expression.Location.spareID = attributeArgs[1].value;
            }
            return;
        default:
            return;
    }
    return;
}
//Return true on failure.
bool setComplexDataAccessors(const vector<string> & attributes, const vector<WordStruct> & attributeArgs, unsigned lineNumber,
    const string & scriptName, ConditionClass & Expression, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, const bool & canCreateNewVariable
){
    string valueSource = attributes[0];
    Expression.Location.source = strToSource(valueSource);
    
    switch(Expression.Location.source){
        case key_pressed:
        case key_pressing:
        case key_released:
        case mouse_pressed:
        case mouse_pressing:
        case mouse_released:
        case screen_w:
        case screen_h:
            if(attributeArgs.size() > 0 && attributeArgs[0].type == TokenType::identifier_tk){
                Expression.Literal.setString(attributeArgs[0].value); //Set variableId
                setOptionalAddressInCond(0, attributeArgs, Expression.localAddresses[0],
                    scriptName, lineNumber, Scopes, NewLocalVariables, topAddress, canCreateNewVariable
                );
            }
            else{
                setOptionalAttributeValueInCond(0, 'i', attributeArgs, Expression.Literal, scriptName, lineNumber);
            }
            return false;
        case exists:
        case is_directory:
            if(attributeArgs.size() > 0 && attributeArgs[0].type == TokenType::identifier_tk){
                Expression.Literal.setString(attributeArgs[0].value);
                setOptionalAddressInCond(0, attributeArgs, Expression.localAddresses[0],
                    scriptName, lineNumber, Scopes, NewLocalVariables, topAddress, canCreateNewVariable
                );
            }
            else{
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ":\n"
                    << NEW_LINE_PADDING << "Function " << valueSource << " requires one argument.\n";
            }
            return false;
        case booting:
        case second_passed:
        case fps:
        case any_key_pressed:
        case any_key_pressing:
        case any_key_released:
        case mouse_x:
        case mouse_y:
        case mouse_moved:
        case display_w:
        case display_h:
        case fullscreen:
        case display_resized:
        case used_os:
        case number_of_processes:
        case number_of_cameras:
        case number_of_layers:
        case number_of_objects:
            return false;
        case vector_s:
            return prepareVectorSource(attributes, attributeArgs, lineNumber, scriptName, Expression,
                Scopes, NewLocalVariables, topAddress, canCreateNewVariable
            );
        case camera:
            Expression.Location.cameraID = getStringByIndex(attributes, 1);
            Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 2));
            return false;
        case layer:
            Expression.Location.layerID = getStringByIndex(attributes, 1);
            Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 2));
            if(Expression.Location.attribute == in_group){
                setOptionalAttributeValueInCond(0, 's', attributeArgs, Expression.Literal, scriptName, lineNumber);
            }
        case object:
            prepareObjectSource(attributes, attributeArgs, lineNumber, scriptName, Expression);
            return false;
        case variable:
            Expression.Location.moduleID = getStringByIndex(attributes, 1);
            return false;
        case process:{
            string error;
            Expression.Location.attribute = strToAttribute(getStringByIndex(attributes, 1), error);
            if(error.size() > 0){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": "
                    << error;
                return true;
            }
            return false;}
        default:
            Expression.Location.source = ValueSource::context;
            return setLocalAddressForExpression(Expression, attributes, Scopes, NewLocalVariables, topAddress, scriptName, lineNumber);
    }
    return false;
}
ReturnType parseUserDefinedIdentifiers(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    unsigned lineNumber, const string & scriptName, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable, WordStruct &firstWord
){
    if(words[cursor].type != TokenType::close_brackets_tk){
        cursor++;
    }

    vector<string> valueLocationAttributes;
    string currentAttribute = "";
    for(const char & letter : firstWord.value){
        if(letter != '.'){
            currentAttribute += letter;
        }
        else{
            valueLocationAttributes.push_back(currentAttribute);
            currentAttribute = "";
        }
    }
    valueLocationAttributes.push_back(currentAttribute);

    vector<WordStruct> locationAttributesArgs; //Value in optional parenthesis
    if(words[cursor].type == TokenType::start_expr_tk){
        if(cursor == words.size() - 1){
            return ReturnType::CONTINUE;
        }
        ++cursor;
        while(cursor < words.size() - 1 && words[cursor].type != TokenType::end_expr_tk){
            if(words[cursor].type == TokenType::close_brackets_tk){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parenthesis were not closed.\n";
                return ReturnType::ERROR;
            }
            locationAttributesArgs.push_back(words[cursor]);
            ++cursor;
        }
        if(words[cursor].type != TokenType::end_expr_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Parenthesis were not closed.\n";
            return ReturnType::ERROR;
        }
        if(cursor == words.size() - 1){
            return ReturnType::CONTINUE;
        }
        ++cursor;
    }

    if(setComplexDataAccessors(valueLocationAttributes, locationAttributesArgs,
        lineNumber, scriptName, Expression.back(), Scopes, NewLocalVariables, topAddress,
        canCreateNewVariable
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType parseExpression(const vector<WordStruct> & words, unsigned & cursor, vector<ConditionClass> & Expression,
    unsigned lineNumber, const string & scriptName, ScopeType & Scopes,
    vector<VariableInfo> & NewLocalVariables, unsigned & topAddress, bool canCreateNewVariable, WordStruct &firstWord
){
    if(words.size() < cursor + 1){ return ReturnType::CONTINUE; };

    if(words[cursor].type != TokenType::empty_tk && words[cursor].type != TokenType::close_brackets_tk){
        firstWord = words[cursor];
    }

    Expression.push_back(ConditionClass(""));

    string error;
    Expression.back().Location.source = strToSource(firstWord.value, error);

    switch(firstWord.type){
        case TokenType::bool_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 'b', scriptName, lineNumber)){
                return ReturnType::CONTINUE;
            };
            return ReturnType::OK;
        case TokenType::int_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 'i', scriptName, lineNumber)){ return ReturnType::CONTINUE; };
            return ReturnType::OK;
        case TokenType::double_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 'd', scriptName, lineNumber)){ return ReturnType::CONTINUE; };
            return ReturnType::OK;
        case TokenType::string_tk:
            Expression.back().Location.source = ValueSource::literal;
            if(nextCond(words, cursor, Expression.back().Literal, 's', scriptName, lineNumber)){ return ReturnType::CONTINUE; };
            return ReturnType::OK;
        case TokenType::identifier_tk:
            return parseUserDefinedIdentifiers(words, cursor, Expression, lineNumber, scriptName, Scopes,
                NewLocalVariables, topAddress, canCreateNewVariable, firstWord
            );    
        default:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Word of the '"
                << tokenToStr(firstWord.type) << "' type is not valid.\n";
            return ReturnType::ERROR;
    }
}

constexpr int getPrecedence(EngineInstr op){
    switch(op){
        case EngineInstr::not_i:
            return 4;
        case EngineInstr::equal:
        case EngineInstr::not_equal:
        case EngineInstr::less:
        case EngineInstr::less_equal:
        case EngineInstr::more:
        case EngineInstr::more_equal:
            return 3;
        case EngineInstr::and_i:
            return 2;
        case EngineInstr::or_i:
            return 1;
        default:
            return 0;
    }
}

ReturnType createConditionalExpression(const vector<WordStruct> & words, unsigned & cursor,
    vector<ConditionClass> & Expression, vector<VariableModule> & resultStack, unsigned lineNumber,
    const string & scriptName, ScopeType & Scopes, vector<VariableInfo> & NewLocalVariables,
    unsigned & topAddress, bool canCreateNewVariable
){
    if(cursor >= words.size()){
        return ReturnType::OK;
    }
    if(words[cursor].type == TokenType::empty_tk){
        cursor++;
        return ReturnType::OK;
    }

    if(words[cursor].type != TokenType::start_expr_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Every expression must begin with parentheses.\n";
        return ReturnType::ERROR;
    }
    
    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression is too short.\n";
        return ReturnType::ERROR;
    }
    WordStruct firstWord;
    vector<EngineInstr> operatorsStack;
    bool isParenthesisOpen = false;
    
    while(isParenthesisOpen || words[cursor].type != TokenType::end_expr_tk){
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression is too short.\n";
            return ReturnType::ERROR;
        }
        if(words[cursor].type == TokenType::start_expr_tk){
            isParenthesisOpen = true;
            operatorsStack.push_back(EngineInstr::start_expr);
            cursor++;
        }
        else if(words[cursor].type == TokenType::end_expr_tk){
            isParenthesisOpen = false;
            while(!operatorsStack.empty() && operatorsStack.back() != EngineInstr::start_expr){
                Expression.back().operators.push_back(operatorsStack.back());
                operatorsStack.pop_back();
            }
            if(operatorsStack.empty()){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Missing a ')'.\n";
                return ReturnType::ERROR;
            }
            operatorsStack.pop_back();
            cursor++;
        }
        else if(words[cursor].type == TokenType::keyword_tk
            && isLogicInstr(words[cursor].instruction)
        ){
            if(Expression.empty() && !operatorsStack.empty()
                && getPrecedence(operatorsStack.back()) > getPrecedence(words[cursor].instruction)
            ){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": Operator '" << words[cursor].value
                    << "' cannot be added to an empty expression.\n";
                return ReturnType::ERROR;
            }
            
            while(!operatorsStack.empty()
                && getPrecedence(operatorsStack.back()) > getPrecedence(words[cursor].instruction)
            ){
                Expression.back().operators.push_back(operatorsStack.back());
                operatorsStack.pop_back();
            }

            operatorsStack.push_back(words[cursor].instruction);

            cursor++;
        }
        else{
            ReturnType status = parseExpression(words, cursor, Expression, lineNumber, scriptName,
                Scopes, NewLocalVariables, topAddress, canCreateNewVariable, firstWord
            );
            if(status == ReturnType::CONTINUE){
                continue;
            }
            if(status == ReturnType::ERROR){
                return ReturnType::ERROR;
            }
        }
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
            return ReturnType::ERROR;
        }
    }
    while(!operatorsStack.empty()){
        Expression.back().operators.push_back(operatorsStack.back());
        operatorsStack.pop_back();
    }

    cursor++;
    resultStack.reserve(Expression.size());
    for(unsigned i = 0; i < Expression.size(); ++i){
        resultStack.push_back(VariableModule());
    }
    return ReturnType::OK;
}

ReturnType createNonConditionalExpression(const vector<WordStruct> & words, unsigned & cursor,
    vector<ConditionClass> & Expression, vector<VariableModule> & resultStack, unsigned lineNumber,
    const string & scriptName, ScopeType & Scopes, vector<VariableInfo> & NewLocalVariables,
    unsigned & topAddress, bool canCreateNewVariable
){
    if(cursor >= words.size()){
        return ReturnType::OK;
    }
    if(words[cursor].type == TokenType::empty_tk){
        cursor++;
        return ReturnType::OK;
    }

    if(words[cursor].type != TokenType::open_brackets_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Parameter " << cursor << " must begin with square brackets.\n";
        return ReturnType::ERROR;
    }

    cursor++;
    if(cursor >= words.size()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
        return ReturnType::ERROR;
    }
    WordStruct firstWord;
    
    while(words[cursor].type != TokenType::close_brackets_tk){
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
            return ReturnType::ERROR;
        }
        if(words[cursor].type == TokenType::keyword_tk && isLogicInstr(words[cursor].instruction)){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": Cannot use operators inside not conditional expression."
                << "Correct syntax: [[source_0] [source_1] ...]\n";
            return ReturnType::ERROR;
        }
        else{
            ReturnType status = parseExpression(words, cursor, Expression, lineNumber, scriptName,
                Scopes, NewLocalVariables, topAddress, canCreateNewVariable, firstWord
            );
            if(status == ReturnType::CONTINUE){
                continue;
            }
            if(status == ReturnType::ERROR){
                return ReturnType::ERROR;
            }
        }
        if(cursor >= words.size()){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Command is too short.\n";
            return ReturnType::ERROR;
        }
    }
    cursor++;
    resultStack.reserve(Expression.size());
    for(unsigned i = 0; i < Expression.size(); ++i){
        resultStack.push_back(VariableModule());
    }
    return ReturnType::OK;
}

bool optional(const vector<WordStruct> & words, unsigned & cursor, string & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
        variable = words[cursor].value;
    }
    cursor++;
    return false;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, AttributeType & attribute){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
        attribute = strToAttribute(words[cursor].value);
    }
    cursor++;
    return false;
}
bool optional(const vector<WordStruct> & words, unsigned & cursor, ValueSource & variable){
    if(words.size() < cursor + 1){
        return true;
    }
    if(words[cursor].type != TokenType::empty_tk){
        string error;
        variable = strToSource(words[cursor].value, error);
        if(error.size() > 0){
            cerr << "Error: In " << __FUNCTION__ << ": " << error << "\n";
            return true;
        }
    }
    cursor++;
    return false;
}
bool optionalOutput(const string & scriptName, const unsigned & lineNumber, string & error, const vector<WordStruct> & words,
    unsigned & cursor, const DataType & outputType, ScopeType & Scopes, vector<VariableInfo> & NewLocalVariables,
    unsigned & topAddress, OutputParameterStruct & OutputParameter, bool makeOutputGlobal = false,
    bool forceNewDeclaration = false
){
    error = "";
    if(cursor + 1 > words.size()){
        return true;
    }
    const WordStruct & instructionOutput = words[cursor];
    
    if(instructionOutput.type != TokenType::empty_tk && instructionOutput.type != TokenType::identifier_tk){
        error = "Parameter 'output' (" + intToStr(cursor);
        error += ") must be a variable name";
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return true;
    }

    if(instructionOutput.type == TokenType::identifier_tk){
        OutputParameter.variableID = instructionOutput.value;
        OutputParameter.type = outputType;

        ReturnType result = ReturnType::OK;
        std::tie(OutputParameter.localAddress, result) = getLocalAddress(
            OutputParameter.variableID, outputType, Scopes, NewLocalVariables,
            topAddress, true, makeOutputGlobal, false, forceNewDeclaration
        );
        if(result == ReturnType::OUT_OF_SCOPE){
            error = + "Index (" + std::to_string(OutputParameter.localAddress)
                + ") of the local variable is out of scope ("
                + std::to_string(NewLocalVariables.size()) + ").\n";
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            OutputParameter.localAddress = 0;
            return true;
        }
        else if(result == ReturnType::UNDEFINED){
            error = + "Variable '" + OutputParameter.variableID + "' is undefined.\n";
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the '" << words[0].value << "' instruction: " << error << "\n";
            return true;
        }
    }

    cursor++;
    return false;
}

ReturnType InstrParser::parseCompilerBreakpoint(bool & triggerBreakpoint){
    triggerBreakpoint = true;
    cerr << "Warning: The 'compiler_breakpoint' instruction can be used only in the debugger.\n";
    return ReturnType::OK;
}
ReturnType InstrParser::parseAnnotations(){
    if(words.size() < 1){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Invalid annotation syntax. Correct syntax is: @*annotation_name* [args...]\n";
        return ReturnType::ERROR;
    }
    if(words[1].value == "trigger"){
        return parseTriggersAnnotation();
    }
    else if(words[1].value == "override"){
        return parseOverrideAnnotation();
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Unknown annotation: '" << words[1].value << "'.\n";
        return ReturnType::ERROR;
    }
    return ReturnType::OK;
}
ReturnType InstrParser::parseOverrideAnnotation(){
    annotations.override = true;
    return ReturnType::OK;
}
ReturnType InstrParser::parseTriggersAnnotation(){
    cursor = 2;
    while(cursor < words.size()){
        if(words[cursor].type != TokenType::identifier_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the annotation '" << words[1].value << "': Parameter " << cursor << " is not a context.\n";
            return ReturnType::ERROR;
        }
        if(words[cursor].type != TokenType::empty_tk){
            TriggerType NewTrigger = strToTrigger(words[cursor].value);
            if(NewTrigger == null_t){
                cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                    << NEW_LINE_PADDING << "In " << __FUNCTION__
                    << ": In the '" << words[0].value << "' instruction: Parameter '"
                    << words[cursor].value << "' is not a valid trigger.\n";
                return ReturnType::ERROR;
            }
            annotations.triggersForNextEvent.push_back(NewTrigger);
        }
        cursor++;
    }
    return ReturnType::OK;
}
ReturnType InstrParser::parseStartAndOverride(vector<string> & allAvailableEventIDs,
    const size_t layerIndex, const string & layerId, const size_t objectIndex,
    const string & objectId, vector<EventModule> &eventContainer, vector<string> &eventContainerIds,
    size_t & topModuleUniqueIndex
){
    if(createEvent(scriptName, lineNumber, layerIndex, layerId, objectIndex, objectId,
        eventContainer, eventContainerIds, NewEvent, words, Scopes, topAddress,
        annotations.override, topModuleUniqueIndex
    ))
        return ReturnType::ERROR;
    allAvailableEventIDs.clear();
    allAvailableEventIDs.push_back(NewEvent.getID());

    //handle annotations
    annotations.override = false;
    if(!annotations.triggersForNextEvent.empty()){
        NewEvent.primaryTriggerTypes.insert(
            NewEvent.primaryTriggerTypes.begin(),
            annotations.triggersForNextEvent.begin(),
            annotations.triggersForNextEvent.end()
        );
        annotations.triggersForNextEvent.clear();
        NewEvent.isFunction = false;
    }
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseEnd(vector<EventModule> &eventContainer,
    BranchingStackStruct & BranchingStack
){
    if(!BranchingStack.openScopes.empty()){
        if(BranchingStack.openScopes.back() == EngineInstr::if_i){
            words[0].instruction = EngineInstr::end_if;
            words[0].value = instrToStr(EngineInstr::end_if);
            return parseEndIf(BranchingStack);
        }
        if(BranchingStack.openScopes.back() == EngineInstr::while_i){
            words[0].instruction = EngineInstr::end_while;
            words[0].value = instrToStr(EngineInstr::end_while);
            return parseEndWhile(BranchingStack);
        }
        return ReturnType::ERROR;
    }
    if(NewEvent.isInline){
        NewEvent.Parameters.clear();
    }
    Scopes.pop_back(); //Remove the scope of the last event
    eventContainer.push_back(NewEvent);
    NewEvent = EventModule();
    return ReturnType::OK;
}
ReturnType InstrParser::parseEmpty(){
    OperationClass * operation = nullptr;
    if(!prepareNewInstruction(words, NewEvent, operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    return ReturnType::OK;
}
ReturnType InstrParser::parseIf(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    Scopes.emplace_back(vector<VariableLocationStruct>());
    if(ReturnType::OK != createConditionalExpression(words, cursor, Operation->ConditionalChain,
        Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables, topAddress,
        false
    )){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return ReturnType::ERROR;
    }
    BranchingStack.ifElseJumpStack.push_back(NewEvent.Operations.size()-1);
    BranchingStack.usedElseStatements.push_back(0);
    BranchingStack.ifEndJumpStack.push_back(vector<unsigned>());
    BranchingStack.ifEndJumpStack.back().push_back(NewEvent.Operations.size()-1);
    BranchingStack.openScopes.emplace_back(EngineInstr::if_i);
    return ReturnType::OK;
}
ReturnType InstrParser::parseElseIf(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    Scopes.pop_back();
    Scopes.emplace_back(vector<VariableLocationStruct>());

    if(ReturnType::OK != createConditionalExpression(words, cursor, Operation->ConditionalChain,
        Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables, topAddress,
        false
    )){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return ReturnType::ERROR;
    }
    
    if(BranchingStack.ifElseJumpStack.empty()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }

    NewEvent.Operations[BranchingStack.ifElseJumpStack.back()].specialValue = NewEvent.Operations.size()-1;

    BranchingStack.ifElseJumpStack.pop_back();
    BranchingStack.ifElseJumpStack.push_back(NewEvent.Operations.size()-1);
    if(BranchingStack.ifEndJumpStack.empty()){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }
    BranchingStack.ifEndJumpStack.back().push_back(NewEvent.Operations.size()-1);

    return ReturnType::OK;
}
ReturnType InstrParser::parseElse(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    Scopes.pop_back();
    Scopes.emplace_back(vector<VariableLocationStruct>());

    if(BranchingStack.ifElseJumpStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }

    NewEvent.Operations[BranchingStack.ifElseJumpStack.back()].specialValue = NewEvent.Operations.size()-1;
    BranchingStack.ifElseJumpStack.pop_back();

    if(BranchingStack.usedElseStatements.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }

    BranchingStack.usedElseStatements.back() = 1;

    if(BranchingStack.ifEndJumpStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }

    BranchingStack.ifEndJumpStack.back().push_back(NewEvent.Operations.size()-1);

    return ReturnType::OK;
}
ReturnType InstrParser::parseEndIf(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    Scopes.pop_back();
    if(BranchingStack.ifEndJumpStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }
    if(BranchingStack.usedElseStatements.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside the if statement scope.\n";
        return ReturnType::ERROR;
    }
    if(BranchingStack.usedElseStatements.back() == 0){
        if(BranchingStack.ifElseJumpStack.size() == 0){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
                << words[0].value << "' outside the if statement scope.\n";
            return ReturnType::ERROR;
        }
        NewEvent.Operations[BranchingStack.ifElseJumpStack.back()].specialValue = NewEvent.Operations.size()-1;
        BranchingStack.ifElseJumpStack.pop_back();
    }
    BranchingStack.usedElseStatements.pop_back();
    if(BranchingStack.ifEndJumpStack.size() > 0){
        for(unsigned lineNumberIdx : BranchingStack.ifEndJumpStack.back()){
            NewEvent.Operations[lineNumberIdx].jumpToLine = NewEvent.Operations.size()-1;
        }
        BranchingStack.ifEndJumpStack.back().clear();
        BranchingStack.ifEndJumpStack.pop_back();
    }
    BranchingStack.openScopes.pop_back();

    return ReturnType::OK;
}
ReturnType InstrParser::parseWhile(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    Scopes.emplace_back(vector<VariableLocationStruct>());
    if(ReturnType::OK != createConditionalExpression(words, cursor, Operation->ConditionalChain,
        Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables, topAddress,
        false
    )){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return ReturnType::ERROR;
    }

    BranchingStack.whileStartStack.push_back(NewEvent.Operations.size()-1);
    BranchingStack.whileEndStack.push_back(vector<unsigned>());
    BranchingStack.whileEndStack.back().push_back(NewEvent.Operations.size()-1);
    BranchingStack.openScopes.emplace_back(EngineInstr::while_i);
    return ReturnType::OK;
}
ReturnType InstrParser::parseEndWhile(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    Scopes.pop_back();

    if(BranchingStack.whileStartStack.size() == 0 || BranchingStack.whileEndStack.size() == 0 || BranchingStack.whileEndStack.back().size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' before any loop.\n";
        return ReturnType::ERROR;
    }

    NewEvent.Operations.back().jumpToLine = BranchingStack.whileStartStack.back();
    BranchingStack.whileStartStack.pop_back();
    for(unsigned & index : BranchingStack.whileEndStack.back()){
        NewEvent.Operations[index].jumpToLine = NewEvent.Operations.size()-1;
    }
    BranchingStack.whileEndStack.back().clear();
    BranchingStack.whileEndStack.pop_back();
    BranchingStack.openScopes.pop_back();

    return ReturnType::OK;
}
ReturnType InstrParser::parseContinue(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(BranchingStack.whileStartStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside a loop.\n";
        return ReturnType::ERROR;
    }

    NewEvent.Operations.back().jumpToLine = BranchingStack.whileStartStack.back();

    return ReturnType::OK;
}
ReturnType InstrParser::parseBreak(BranchingStackStruct & BranchingStack){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(BranchingStack.whileEndStack.size() == 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Cannot use '"
            << words[0].value << "' outside a loop.\n";
        return ReturnType::ERROR;
    }
    BranchingStack.whileEndStack.back().push_back(NewEvent.Operations.size() - 1);
    
    return ReturnType::OK;
}
DataType vectorizeEntityDataType(const InstrDescription & CurrentInstr, const DataType & oldType){
    switch(oldType){
        case camera_inst:
            return camera_vec;
        case layer_inst:
            return layer_vec;
        case object_inst:
            return object_vec;
        case super_text_mod:
            return super_text_mod_vec;
        case super_editable_text_mod:
            return super_editable_text_mod_vec;
        case image_mod:
            return image_mod_vec;
        case movement_mod:
            return movement_mod_vec;
        case collision_mod:
            return collision_mod_vec;
        case particles_mod:
            return particles_mod_vec;
        case event_mod:
            return event_mod_vec;
        case variable_mod:
            return variable_mod_vec;
        case scrollbar_mod:
            return scrollbar_mod_vec;
        case primitives_mod:
            return primitives_mod_vec;
        case vector_mod:
            return vector_mod_vec;
        case pointer_inst:
            return pointer_vec;
        case any_dt:
            return any_dt;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__) << "Entity type \'"
                << dataTypeToStr(oldType) << "\' is not valid for this operation.\n";
            return null_dt;
    }
}
inline DataType attributeToInstDataType(const InstrDescription & CurrentInstr, const AttributeType &attribute){
    switch(attribute){
        case camera_a:
            return camera_inst;
        case layer_a:
            return layer_inst;
        case object_a:
            return object_inst;
        case super_text_a:
            return super_text_mod;
        case super_editable_text_a:
            return super_editable_text_mod;
        case image_a:
            return image_mod;
        case movement_a:
            return movement_mod;
        case collision_a:
            return collision_mod;
        case particles_a:
            return particles_mod;
        case event_a:
            return event_mod;
        case variable_a:
            return variable_mod;
        case scrollbar_a:
            return scrollbar_mod;
        case primitives_a:
            return primitives_mod;
        case vector_a:
            return vector_mod;
        default:
            return null_dt;
    }
}
ReturnType InstrParser::parseFirstLastAllRandom(vector <string> & allAvailableEventIDs){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName)){
        return ReturnType::ERROR;
    }
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";

    if(words[1].value == "Layers"){
        Operation->Location.source = ValueSource::layer;
    }
    else if(words[1].value == "Cameras"){
        Operation->Location.source = ValueSource::camera;
    }
    else if(words[1].value != ""){
        Operation->Location.source = ValueSource::context;
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'c', "context", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return ReturnType::ERROR;
    }

    DataType outputType = null_dt;
    
    cursor = 2;
    if(Operation->Location.source == ValueSource::camera){
        if(optional(words, cursor, Operation->Location.cameraID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(ReturnType::OK != createConditionalExpression(words, cursor, Operation->ConditionalChain,
            Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables,
            topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
        outputType = camera_inst;
        if(Operation->Location.attribute != null_a && Operation->Location.attribute != camera_a){
            outputType = pointer_inst;
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor,
            outputType, Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.size() == 0){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(Operation->Location.source == ValueSource::layer){
        if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(ReturnType::OK != createConditionalExpression(words, cursor, Operation->ConditionalChain,
            Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables,
            topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
        if(Operation->Location.attribute == null_a && Operation->Location.objectID.empty()
            && Operation->Location.moduleType == null_s && Operation->Location.moduleID.empty()
        ){
            outputType = layer_inst;
        }
        else if(Operation->Location.attribute != object_a && Operation->Location.objectID.empty()
            && Operation->Location.moduleType == null_s && Operation->Location.moduleID.empty()
        ){
            outputType = pointer_inst;
        }
        else{
            if(Operation->Location.moduleType != null_s){
                switch(Operation->Location.moduleType){
                    case super_text:
                        outputType = super_text_mod;
                        break;
                    case super_editable_text:
                        outputType = super_editable_text_mod;
                        break;
                    case image:
                        outputType = image_mod;
                        break;
                    case movement:
                        outputType = image_mod;
                        break;
                    case collision:
                        outputType = collision_mod;
                        break;
                    case particles:
                        outputType = particles_mod;
                        break;
                    case event:
                        outputType = event_mod;
                        break;
                    case variable:
                        outputType = variable_mod;
                        break;
                    case scrollbar:
                        outputType = scrollbar_mod;
                        break;
                    case primitives:
                        outputType = primitives_mod;
                        break;
                    case vector_s:
                        outputType = vector_mod;
                        break;
                    default:
                        break;
                }
            }
            else if(Operation->Location.attribute == object_a || Operation->Location.attribute == null_a){
                outputType = object_inst;
            }
            else{
                outputType = pointer_inst;
            }
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor,
            outputType, Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.size() == 0){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(Operation->Location.source == ValueSource::context){
        if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(ReturnType::OK != createConditionalExpression(words, cursor, Operation->ConditionalChain,
            Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables,
            topAddress, false
        )){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
            return ReturnType::ERROR;
        }
        outputType = attributeToInstDataType(InstrDescription(), Operation->Location.attribute);
        if(outputType == null_dt){
            outputType = any_dt;
        }
        if(words[0].value == "all"){
            outputType = vectorizeEntityDataType(InstrDescription(), outputType);
        }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor,
            outputType, Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.size() == 0){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": Source '" << words[1].value << "' does not exist.\n";
        return ReturnType::ERROR;
    }
    return ReturnType::OK;
}
ReturnType InstrParser::parseIndex(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value
            << "' instruction: The first two parameters are not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";

    if(words[1].value == "Processes"){
        Operation->Location.source = ValueSource::process;
    }
    else if(words[1].value == "Layers"){
        Operation->Location.source = ValueSource::layer;
    }
    else if(words[1].value == "Cameras"){
        Operation->Location.source = ValueSource::camera;
    }
    else if(words[1].value != ""){
        Operation->Location.source = ValueSource::context;
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "source", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return ReturnType::ERROR;
    }
    
    cursor = 2;
    
    if(Operation->addVectorOrVariableToParameters(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'i', "indexes", false, false
    )){ return ReturnType::ERROR; }

    if(optional(words, cursor, Operation->Location.attribute))
        return ReturnType::OK;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseIndexVec(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first two parameters are not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";

    if(words[1].value != ""){
        Operation->Location.source = ValueSource::context;
        if(Operation->addParameter(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'c', "vector", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": First parameter cannot be an empty context.\n";
        return ReturnType::ERROR;
    }
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'i', "index", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 3;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseAddSubMulDivModPowRand(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(words[0].instruction == add){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'a', "left", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'a', "right", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            1, 'n', "left", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'n', "right", false, false, false
        )){ return ReturnType::ERROR; }
    }
    cursor = 3;
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }
    return ReturnType::OK;
}
ReturnType InstrParser::parseAssert(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'a', "left", false, false, true
    )){ return ReturnType::ERROR; }
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'a', "right", false, false, true
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoad(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'a', "address", false, true, true
    )){ return ReturnType::ERROR; }
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseMove(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "left", false, false, false
    )){ return ReturnType::ERROR; }

    if(words[0].instruction == move || words[0].instruction == add_move){
        if(Operation->addParameter(scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'a', "right", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'n', "right", false, false, false
        )){ return ReturnType::ERROR; }
    }

    cursor = 3;
    
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseFindById2(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "source", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "id", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 3;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSumIntersecDiffIn(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "left", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'c', "right", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 3;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseIncDecDelDemolishRbindType(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string parameterName = "context";
    if(words[0].instruction == demolish || words[0].instruction == rbind_i){
        parameterName = "objects";
    }

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', parameterName, false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;
    
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty())
            return ReturnType::OK;
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseNext(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'i', "number", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseAccess(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.size() > 0)
            return ReturnType::ERROR;
    }

    if(ReturnType::OK != createNonConditionalExpression(words, cursor, Operation->ConditionalChain,
        Operation->resultStack, lineNumber, scriptName, Scopes, NewEvent.LocalVariables, topAddress,
        false
    )){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Expression creation failed.\n";
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseBoolIntDoubleStringAndTheirVectors(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    DataType newVariableType = value_inst;
    
    switch(Operation->instruction){
        case bool_vec_i:
        case int_vec_i:
        case double_vec_i:
        case string_vec_i:
            newVariableType = value_vec;
            break;
        default:
            break;
    }

    bool makeOutputGlobal = false;
    string error = "";

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, newVariableType,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, makeOutputGlobal, true
    )){
        if(error.size() > 0)
            return ReturnType::ERROR;
    }
    
    switch(Operation->instruction){
        case bool_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'b', "value", false, false, true
            )){ return ReturnType::ERROR; }
            break;
        case bool_vec_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'b', "values", false, false, false
            )){ return ReturnType::ERROR; }
            break;
        case int_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'i', "value", false, false, true
            )){ return ReturnType::ERROR; }
            break;
        case int_vec_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'i', "values", false, false, false
            )){ return ReturnType::ERROR; }
            break;
        case double_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'd', "value", false, false, true
            )){ return ReturnType::ERROR; }
            break;
        case double_vec_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 'd', "values", false, false, false
            )){ return ReturnType::ERROR; }
            break;
        case string_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 's', "value", false, false, true
            )){ return ReturnType::ERROR; }
            break;
        case string_vec_i:
            if(Operation->addLiteralOrVectorOrVariableToParameters(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                cursor, 's', "values", false, false, false
            )){ return ReturnType::ERROR; }
            break;
        default:
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Literal type is required.\n";
            return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseFindById(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";
    Operation->Location.source = strToSource(words[1].value, error);

    if(error.size() > 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return ReturnType::ERROR;
    }

    cursor = 2;

    if(words[1].value == "Cameras"){
        if(words.size() < 4){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": \'find_by_id " << words[1].value << "\' requires at least 2 additional parameters.\n";
            return ReturnType::ERROR;
        }

        if(optional(words, cursor, Operation->Location.cameraID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[1].value == "Layers"){
        if(words.size() < 7){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": \'find_by_id " << words[1].value << "\' requires at least 5 additional parameters.\n";
            return ReturnType::ERROR;
        }

        if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }
    else if(words[1].value == "context" || words[1].value == "c" || words[1].type == TokenType::empty_tk){
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'c', "scripts", false, false
        )){ return ReturnType::ERROR; }
        
        if(words.size() < cursor + 5){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": \'find_by_id " << words[1].value << " [context_list]\' requires at least 5 additional parameters.\n";
            return ReturnType::ERROR;
        }

        if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleType)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.moduleID)){ return ReturnType::OK; }
        if(optional(words, cursor, Operation->Location.attribute)){ return ReturnType::OK; }
        if(optionalOutput(scriptName, lineNumber, error, words, cursor, any_dt,
            Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
        )){
            if(error.empty()){ return ReturnType::OK; }
            return ReturnType::ERROR;
        }
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseClone(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "left", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'c', "right", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 'b', "changeOldID", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseNew(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not of a context type.\n";
        return ReturnType::ERROR;
    }

    string error = "";
    Operation->Location.source = strToSource(words[1].value, error);
    
    if(error.size() > 0){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: " << error << "\n";
        return ReturnType::ERROR;
    }
    
    //If the destination is provided as a variable, skip one parameter in the instruction.
    if(words[2].type == TokenType::identifier_tk){
        Operation->addLiteralParameter(VariableModule::newString("variable"));
    }
    else{
        Operation->addLiteralParameter(VariableModule::newString("location"));
    }
    
    cursor = 2;

    if(words[cursor].type == TokenType::identifier_tk){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'c', "destination", false, false, false
        )){ return ReturnType::ERROR; }
        cursor++;
    }
    else{
        if(Operation->Location.source == ValueSource::object){
            if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
        }
        else if(Operation->Location.source != ValueSource::camera && Operation->Location.source != ValueSource::layer){
            if(optional(words, cursor, Operation->Location.layerID)){ return ReturnType::OK; }
            if(optional(words, cursor, Operation->Location.objectID)){ return ReturnType::OK; }
        }
    }
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'i', "quantity", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    cursor++;

    if(Operation->addLiteralOrVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 's', "new_ids", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    DataType typeForNewEntity = sourceToEntityType(InstrDescription(), Operation->Location.source);
    
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, typeForNewEntity,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }
    
    return ReturnType::OK;
}
ReturnType InstrParser::parseBind(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
    
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "objects", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;
    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 's', "scripts", false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseBuild(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "objects", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'b', "reset", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 'b', "do_not_preserve", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }
    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadBuildInject(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'c', "objects", false, false
    )){ return ReturnType::ERROR; }

    string parameterName = "paths";
    if(words[0].instruction == inject_code){
        parameterName = "code";
    }
    else if(words[0].instruction == inject_instr){
        parameterName = "instructions";
    }

    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 's', parameterName, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'b', "reset", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor + 1, 'b', "do_not_preserve", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseFunction(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "objects", false, false, false
    )){ return ReturnType::ERROR; }

    Operation->Location.attribute = strToAttribute(words[2].value);

    cursor = 3;

    while(words.size() > cursor){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'a', "value", false, false, false
        )){ return ReturnType::ERROR; }
        cursor++;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseEnv(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
    
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    string error = "";

    if(words[1].value == "window_title"){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "title", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "display_size"){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "width", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'i', "height", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "value", false, false, false
        )){ return ReturnType::ERROR; }
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseEditProc(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    for(cursor = 2; cursor < words.size(); cursor++){
        if(words[cursor].type != TokenType::identifier_tk){
            continue;
        }
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: Parameter " << cursor << " must be a literal.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    if(isStringInGroup(words[1].value, 2, "clear_layers", "clear_cameras")){
        return ReturnType::OK;
    }

    string error = "";
    
    if(words[1].value == "id"){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 's', "new_id", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "reservation_multiplier"){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'd', "multiplier", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "window_pos" || words[1].value == "window_size" || words[1].value == "min_window_size"){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "x", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'i', "y", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "window_tint"){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'd', "red", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            3, 'd', "green", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            4, 'd', "blue", false, false, false
        )){ return ReturnType::ERROR; }
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            5, 'd', "alpha", false, false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            2, 'i', "value", false, false, false
        )){ return ReturnType::ERROR; }
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadBitmap(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "name", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 'b', "light", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        4, 'b', "ignore_warnings", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseMkdirRmRmll(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR; 

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseMv(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "new_path", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parsePrint(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    switch(words[0].instruction){
        case print_v_i:
            if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
                Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
            )){
                if(error.empty()){ return ReturnType::OK; }
                return ReturnType::ERROR;
            }
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                2, 's', "delimeter", false, false, false
            )){ return ReturnType::ERROR; }
            cursor = 3;
            break;
        case print_d_i:
            if(Operation->addParameter(
                scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
                1, 's', "delimeter", false, false, false
            )){ return ReturnType::ERROR; }
            cursor = 2;
            break;
        default:
            Operation->addEmptyParameter();
            break;
    }

    while(cursor < words.size()){
        if(Operation->addParameter(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'a', "value", false, false, false
        )){ return ReturnType::ERROR; }
        cursor++;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadText(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSaveText(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "text", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 's', "delimeter", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLs(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    cursor = 2;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 'b', "recursive", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        4, 'i', "max_depth", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLse(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'b', "detail", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseNewProc(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "name", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "layer", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 's', "object", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        4, 's', "script", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseVar(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, variable_mod,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, true
    )){
        if(error.size() > 0){ return ReturnType::ERROR; }
    }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'a', "value", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseVec(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->addLiteralParameter(VariableModule::newString(words[1].value));

    cursor = 2;
    string error = "";

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, vector_mod,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output, true
    )){
        if(error.size() > 0){ return ReturnType::ERROR; }
    }

    if(words.size() < 4){
        return ReturnType::OK;
    }

    if(words[1].value == "bool"){
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'b', "scripts", false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "int"){
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'i', "scripts", false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "double"){
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 'd', "scripts", false, false
        )){ return ReturnType::ERROR; }
    }
    else if(words[1].value == "string"){
        if(Operation->addVectorOrVariableToParameters(
            scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
            cursor, 's', "scripts", false, false
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": In the instruction '" << words[0].value
            << "': The type \'" << words[1].value << "\' does not exist.\n";
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseTokenize(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "delimeter", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "text", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 3;

    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'c', "outputs", false, true
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseTreePwdConsoleInput(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLen(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "text", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSize(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'c', "text", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSubstr(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "text", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'i', "begin", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 'i', "length", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 4;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseLoadFont(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 4, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'i', "size", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 's', "name", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        4, 'b', "ignore_warnings", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseCd(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "path", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseSimilar(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "pattern", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;

    if(Operation->addVectorOrVariableToParameters(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 's', "vector", false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        cursor, 'b', "longest_common_part", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    cursor++;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseCount(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 3, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "pattern", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 's', "text", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 3;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseCreateDisplay(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 5, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 'i', "display_width", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        2, 'i', "display_height", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        3, 'i', "backbuffer_width", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        4, 'i', "backbuffer_height", false, false, false
    )){ return ReturnType::ERROR; }

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        5, 'b', "auto_scale_backbuffer", true, false, false
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseStartTimer(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;

    string error = "";
        
    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "name", false, false, false
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseStopTimer(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 1, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    string error = "";

    if(Operation->addParameter(
        scriptName, lineNumber, error, words, Scopes, NewEvent.LocalVariables, topAddress,
        1, 's', "name", false, false, false
    )){ return ReturnType::ERROR; }

    cursor = 2;

    if(optionalOutput(scriptName, lineNumber, error, words, cursor, value_inst,
        Scopes, NewEvent.LocalVariables, topAddress, Operation->Output
    )){
        if(error.empty()){ return ReturnType::OK; }
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
bool prepareNewVariableDeclaration(const vector<WordStruct> & words, unsigned minLength, unsigned lineNumber, const string & scriptName){
    if(words.size() < minLength){
        if(minLength == 2){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least 1 parameter.\n";
        }
        else{
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' requires at least " << minLength-1 << " parameters.\n";
        }
        return false;
    }
    return true;
}
ReturnType InstrParser::parseVarDefinition(){
    if(!prepareNewVariableDeclaration(words, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    auto[localAddress, result] = getLocalAddress(
        words[1].value, instrToDataType(words[0].instruction), Scopes, NewEvent.LocalVariables,
        topAddress, true, false, false, true
    );

    if(result != ReturnType::OK){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Variable declaration failed.\n";
        return ReturnType::ERROR;
    }

    return ReturnType::OK;
}
ReturnType InstrParser::parseRun(){
    if(!prepareNewInstruction(words, NewEvent, Operation, 2, lineNumber, scriptName))
        return ReturnType::ERROR;
        
    if(words[1].type != TokenType::identifier_tk){
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__
            << ": In the '" << words[0].value << "' instruction: The first parameter is not a context.\n";
        return ReturnType::ERROR;
    }

    Operation->specialValue = NewEvent.Children.size();

    NewEvent.Children.emplace_back(ChildStruct(0, words[1].value, vector<PassingVariableInfo>(), 0,
        scriptName, lineNumber
    ));

    cursor = 2;

    if(NewEvent.getPassingVariables(NewEvent.Children.back().Arguments,
        words, cursor, lineNumber, scriptName, Scopes, topAddress
    )){ return ReturnType::ERROR; }

    return ReturnType::OK;
}
ReturnType InstrParser::parseAutoRun(){
    if(words.size() >= 3 && words[1].type == TokenType::start_expr_tk && words.back().type == TokenType::end_expr_tk){
        const vector<WordStruct> runToken = {WordStruct(TokenType::keyword_tk, "run", false)};
        if(!prepareNewInstruction(runToken, NewEvent, Operation, 1, lineNumber, scriptName))
            return ReturnType::ERROR;
        
        if(words[0].type != TokenType::identifier_tk){
            cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
                << NEW_LINE_PADDING << "In " << __FUNCTION__
                << ": In the 'auto_run' instruction: The first parameter is not a context.\n";
            return ReturnType::ERROR;
        }
        Operation->specialValue = NewEvent.Children.size();
        NewEvent.Children.emplace_back(ChildStruct(0, words[0].value, vector<PassingVariableInfo>(),
            0, scriptName, lineNumber
        ));
        cursor = 1;

        if(NewEvent.getPassingVariables(NewEvent.Children.back().Arguments,
            words, cursor, lineNumber, scriptName, Scopes, topAddress
        )){ return ReturnType::ERROR; }
    }
    else{
        cerr << "Error: In " << scriptName << ":" << lineNumber << ":\n"
            << NEW_LINE_PADDING << "In " << __FUNCTION__ << ": Instruction \'" << words[0].value << "\' does not exist.\n";
    }
    return ReturnType::OK;
}