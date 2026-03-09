#include "vectorModule.h"

VectorModule::VectorModule(size_t & topModuleUniqueIndex){
    uniqueIndex = topModuleUniqueIndex++;
}
void VectorModule::setAllIndexes(size_t index, size_t objectIndex, size_t layerIndex){
    uniqueIndex = index;
    objectUniqueIndex = objectIndex;
    layerUniqueIndex = layerIndex;
}
VectorModule::VectorModule(const VectorModule &Original){
    setAllIndexes(Original.uniqueIndex, Original.objectUniqueIndex, Original.layerUniqueIndex);

    deleted = Original.deleted;
    ID = Original.ID;
    layerID = Original.layerID;
    objectID = Original.objectID;
    type = Original.type;
    if(deleted){
        return;
    }
    switch(type){
        case 'b':
            vBool = Original.vBool;
            return;
        case 'i':
            vInt = Original.vInt;
            return;
        case 'd':
            vDouble = Original.vDouble;
            return;
        case 's':
            vString = Original.vString;
            return;
    }
}
VectorModule &VectorModule::operator=(const VectorModule &Original){
    setAllIndexes(Original.uniqueIndex, Original.objectUniqueIndex, Original.layerUniqueIndex);
    
    deleted = Original.deleted;
    ID = Original.ID;
    layerID = Original.layerID;
    objectID = Original.objectID;
    type = Original.type;
    if(deleted){
        return *this;
    }
    switch(type){
        case 'b':
            vBool = Original.vBool;
            break;
        case 'i':
            vInt = Original.vInt;
            break;
        case 'd':
            vDouble = Original.vDouble;
            break;
        case 's':
            vString = Original.vString;
            break;
    }
    return *this;
}
VectorModule::VectorModule(PrimaryData & initData){
    setAllIndexes(*initData.topIndex++, initData.objectUniqueIndex, initData.layerUniqueIndex);
    setAllIDs(initData, true);
}
VectorModule::VectorModule(PrimaryData & initData, const string & newType){
    setAllIndexes(*initData.topIndex++, initData.objectUniqueIndex, initData.layerUniqueIndex);
    setAllIDs(initData, true);
    if(newType == "null"){
        type = 'n';
    }
    else if(newType == "bool"){
        type = 'b';
    }
    else if(newType == "int"){
        type = 'i';
    }
    else if(newType == "double"){
        type = 'd';
    }
    else if(newType == "string"){
        type = 's';
    }
    else{
        cerr << "Error: In " << __FUNCTION__ << ": Type '" << newType << "' is not valid.\n";
    }
}
VectorModule::VectorModule(PrimaryData & initData, char newType){
    setAllIndexes(*initData.topIndex++, initData.objectUniqueIndex, initData.layerUniqueIndex);
    setAllIDs(initData, true);
    if(newType == 'n' || newType == 'b' || newType == 'i' || newType == 'd' || newType == 's'){
        type = newType;
    }
    else{
        cerr << "Error: In " << __FUNCTION__ << ": Type '" << newType << "' is not valid.\n";
    }
}
void VectorModule::clear(){
    switch(type){
        case 'b':
            vBool.clear();
            break;
        case 'i':
            vInt.clear();
            break;
        case 'd':
            vDouble.clear();
            break;
        case 's':
            vString.clear();
            break;
    }
    type = 'n';
}
void VectorModule::setAllIDs(PrimaryData & initData, bool changeOldID){
    if(changeOldID){
        setID(initData.newID, initData.listOfIDs);
    }
    setLayerID(initData.newLayerID);
    setObjectID(initData.newObjectID);
}
void VectorModule::clone(const VectorModule &Original, PrimaryData & initData, bool changeOldID){
    setAllIndexes(uniqueIndex, initData.objectUniqueIndex, initData.layerUniqueIndex);

    initData.newID = Original.getID();
    setAllIDs(initData, changeOldID);

    deleted = Original.deleted;
    
    type = Original.type;
    switch(type){
        case 'b':
            vBool = Original.vBool;
            break;
        case 'i':
            vInt = Original.vInt;
            break;
        case 'd':
            vDouble = Original.vDouble;
            break;
        case 's':
            vString = Original.vString;
            break;
    }
}
void VectorModule::deleteLater(){
    deleted = true;
}
void VectorModule::setUniqueIndex(size_t value){
    uniqueIndex = value;
}
size_t VectorModule::getUniqueIndex() const{
    return uniqueIndex;
}
void VectorModule::setObjectUniqueIndex(size_t value){
    objectUniqueIndex = value;
}
size_t VectorModule::getObjectUniqueIndex() const{
    return objectUniqueIndex;
}
void VectorModule::setLayerUniqueIndex(size_t value){
    layerUniqueIndex = value;
}
size_t VectorModule::getLayerUniqueIndex() const{
    return layerUniqueIndex;
}
void VectorModule::setID(string newID, vector<string> *listOfIDs){
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID
            << "\' cannot be changed.\n";
        return;
    }
    if(listOfIDs != nullptr){
        removeFromStringVector(*listOfIDs, ID);
        ID = findNewUniqueID(*listOfIDs, newID);
        listOfIDs->push_back(ID);
    }
    else{
        ID = newID;
    }
}
void VectorModule::setLayerID(const string & newID){
    layerID = newID;
}
void VectorModule::setObjectID(const string & newID){
    objectID = newID;
}
bool VectorModule::setType(char newType){
    if(type != 'n' && type != newType){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": You can't change the type of already "
            << "initialized vector without clearing it first.\n";
        return false;
    }
    if(!isCharInGroup(newType, 4, 'b', 'i', 'd', 's')){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Variable type '" << newType
            << "' does not exist.\n";
        return false;
    }
    type = newType;
    return true;
}
bool VectorModule::pushBool(bool newValue){
    if(type == 'b' || type == 'n'){
        vBool.emplace_back(stupidBool(newValue));
        type = 'b';
        return true;
    }
    else if(type == 'i'){
        vInt.push_back(newValue);
        return true;
    }
    else if(type == 'd'){
        vDouble.push_back(newValue);
        return true;
    }
    else if(type == 's'){
        vString.emplace_back(intToStr(newValue));
        return true;
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__
        << ": Cannot push back a value of 'bool' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushBoolUnsafe(bool newValue){
    vBool.emplace_back(stupidBool(newValue));
}
bool VectorModule::pushInt(int newValue){
    if(type == 'b'){
        vBool.emplace_back(stupidBool(newValue));
        return true;
    }
    else if(type == 'i' || type == 'n'){
        vInt.push_back(newValue);
        type = 'i';
        return true;
    }
    else if(type == 'd'){
        vDouble.push_back(newValue);
        return true;
    }
    else if(type == 's'){
        vString.emplace_back(intToStr(newValue));
        return true;
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__
        << ": Cannot push back a value of 'int' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushIntUnsafe(int newValue){
    vInt.push_back(newValue);
}
bool VectorModule::pushDouble(double newValue){
    if(type == 'b'){
        vBool.emplace_back(stupidBool(newValue));
        return true;
    }
    else if(type == 'i'){
        vInt.push_back(newValue);
        return true;
    }
    else if(type == 'd' || type == 'n'){
        vDouble.push_back(newValue);
        type = 'd';
        return true;
    }
    else if(type == 's'){
        vString.emplace_back(doubleToStr(newValue));
        return true;
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__
        << ": Cannot push back a value of 'double' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushDoubleUnsafe(double newValue){
    vDouble.push_back(newValue);
}
bool VectorModule::pushString(string newValue){
    if(type == 's' || type == 'n'){
        vString.push_back(newValue);
        type = 's';
        return true;
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__
        << ": Cannot push back a value of 'string' type to a '" << type << "' vector.\n";
    return false;
}
void VectorModule::pushStringUnsafe(string newValue){
    vString.push_back(newValue);
}
bool VectorModule::popBack(){
    if(type == 'b' && vBool.size() > 0){
        vBool.pop_back();
        return true;
    }
    if(type == 'i' && vInt.size() > 0){
        vInt.pop_back();
        return true;
    }
    if(type == 'd' && vDouble.size() > 0){
        vDouble.pop_back();
        return true;
    }
    if(type == 's' && vString.size() > 0){
        vString.pop_back();
        return true;
    }
    cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Cannot pop back an empty vector.\n";
    return false;
}
bool VectorModule::removeIndex(size_t index){
    if(type == 'b' && index < vBool.size()){
        vBool.erase(vBool.begin()+index);
        return false;
    }
    if(type == 'i' && index < vInt.size()){
        vInt.erase(vInt.begin()+index);
        return false;
    }
    if(type == 'd' && index < vDouble.size()){
        vDouble.erase(vDouble.begin()+index);
        return false;
    }
    if(type == 's' && index < vString.size()){
        vString.erase(vString.begin()+index);
        return false;
    }
    return true;
}
bool VectorModule::setBool(size_t index, bool newValue){
    if(type == 'b' && index < vBool.size()){
        vBool[index].value = newValue;
    }
    else if(type == 'i' && index < vInt.size()){
        vInt[index] = newValue;
    }
    else if(type == 'd' && index < vDouble.size()){
        vDouble[index] = newValue;
    }
    else if(type == 's' && index < vString.size()){
        vString[index] = newValue;
    }
    else{
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Index (" << index
            << ") is out of scope of the vector " << ID << " of size " << vBool.size() << ".\n";
        return false;
    }
    return true;
}
bool VectorModule::setInt(size_t index, int newValue){
    if(type == 'b' && index < vBool.size()){
        vBool[index].value = newValue;
    }
    else if(type == 'i' && index < vInt.size()){
        vInt[index] = newValue;
    }
    else if(type == 'd' && index < vDouble.size()){
        vDouble[index] = newValue;
    }
    else if(type == 's' && index < vString.size()){
        vString[index] = newValue;
    }
    return true;
}
bool VectorModule::setDouble(size_t index, double newValue){
    if(type == 'b' && index < vBool.size()){
        vBool[index].value = newValue;
    }
    else if(type == 'i' && index < vInt.size()){
        vInt[index] = newValue;
    }
    else if(type == 'd' && index < vDouble.size()){
        vDouble[index] = newValue;
    }
    else if(type == 's' && index < vString.size()){
        vString[index] = newValue;
    }
    return true;
}
bool VectorModule::setString(size_t index, const string & newValue){
    if(index >= vString.size()){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Index (" << index
            << ") is out of scope of the vector " << ID << " of size " << vBool.size() << ".\n";
        return false;
    }
    if(type != 's'){
        cerr << "Error: In " << __PRETTY_FUNCTION__
            << ": Cannot assign a string value to a vector of '" << type << "' type.\n";
        return false;
    }
    vString[index] = newValue;
    return true;
}
void VectorModule::reserve(unsigned newSize){
    if(type == 'b'){
        vBool.reserve(newSize);
    }
    else if(type == 'i'){
        vInt.reserve(newSize);
    }
    else if(type == 'd'){
        vDouble.reserve(newSize);
    }
    else if(type == 's'){
        vString.reserve(newSize);
    }
    else{
        cout << "Warning: In " << __PRETTY_FUNCTION__
            << ": Cannot reserve memory for a vector of '" << type << "' type.\n";
    }
}
template<typename LeftType, typename RightType>
void executeMoveTypeInstruction2(LeftType * LeftOperand, RightType * RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    if(LeftOperand == nullptr){
        cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
            << "Left operand is null.\n";
        return;
    }
    if(instruction == inc){
        (*LeftOperand)++;
        return;
    } 
    if(instruction == dec){
        (*LeftOperand)--;
        return;
    }
    if(RightOperand == nullptr){
        cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
            << "Right operand is null.\n";
        return;
    }
    switch(instruction){
        case move:
        case clone_i:
            *LeftOperand = *RightOperand;
            return;
        case add_move:
            *LeftOperand += *RightOperand;
            return;
        case sub_move:
            *LeftOperand -= *RightOperand;
            return;
        case mul_move:
            *LeftOperand *= *RightOperand;
            return;
        case div_move:
            if(*RightOperand != 0) [[likely]]{
                *LeftOperand /= *RightOperand;
            }
            else{
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Cannot divide by zero.\n";
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "Instruction \'" << instrToStr(instruction) << "\' is not valid.\n";
            return;
    }
}
template<typename RightType>
void VectorModule::moveValueToEachInstance(RightType * RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    switch(type){
        case 'b':
            for(stupidBool & valBool : vBool){
                short temp = valBool.value;
                executeMoveTypeInstruction2(&temp, RightOperand, instruction, CurrentInstrInfo);
                valBool.value = temp > 0;
            }
            return;
        case 'i':
            for(int & valInt : vInt){
                executeMoveTypeInstruction2(&valInt, RightOperand, instruction, CurrentInstrInfo);
            }
            return;
        case 'd':
            for(double & valDouble : vDouble){
                executeMoveTypeInstruction2(&valDouble, RightOperand, instruction,
                    CurrentInstrInfo
                );
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the left operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}
void VectorModule::move(VariableModule* RightOperand, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveValueToEachInstance((int*)nullptr, instruction, CurrentInstrInfo);
        return;
    }
    switch(RightOperand->type){
        case 'b':{
            short temp = RightOperand->vBool;
            moveValueToEachInstance(&temp, instruction, CurrentInstrInfo);
            return;}
        case 'i':
            moveValueToEachInstance(&RightOperand->vInt, instruction, CurrentInstrInfo);
            return;
        case 'd':
            moveValueToEachInstance(&RightOperand->vDouble, instruction, CurrentInstrInfo);
            return;
        case 's':
            if(RightOperand->type == 's'){
                if(instruction == EngineInstr::move){
                    for(string & stringInstance : vString){
                        stringInstance = RightOperand->vString;
                    }
                }
                else if(instruction == EngineInstr::add_move){
                    for(string & stringInstance : vString){
                        stringInstance += RightOperand->vString;
                    }
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute \'" << instrToStr(instruction)
                        << "\' instruction on string type values.\n";
                }
            }
            else{
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Cannot execute any instructions if only the right operand is of a string "
                    << "type.\n";
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}
//Return true if both vectors are of the same size or one of them has one instance.
//Set incrementation flags based on the size of both vectors.
bool checkForVectorSize(const InstrDescription & CurrentInstr, size_t leftSize,
    size_t rightSize, bool & incLeftIdx, bool & incRightIdx, unsigned & maxIndex,
    const string & functionName
){
    if(leftSize == rightSize){
        incLeftIdx = true;
        incRightIdx = true;
        maxIndex = leftSize;
    }
    else if(leftSize == 1){
        incRightIdx = true;
        maxIndex = rightSize;
    }
    else if(rightSize == 1){
        incLeftIdx = true;
        maxIndex = leftSize;
    }
    else{
        cerr << printErrorMessage(CurrentInstr, functionName)
            << "Vector sizes " << leftSize << " (left) and " << rightSize
            << " (right) are incorrect.\n";
        return false;
    }
    return true;
}
template <typename LeftType>
void moveAllValuesToEachInstance(vector<LeftType> * LeftOperand,
    vector<VariableModule> * RightOperand, bool incLeftIdx, bool incRightIdx, unsigned maxIndex,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    int tempValueForBool = 0;
    for(unsigned leftIndex = 0, rightIndex = 0; leftIndex < maxIndex && rightIndex < maxIndex;
        leftIndex+=incLeftIdx, rightIndex+=incRightIdx
    ){
        switch((*RightOperand)[rightIndex].type){
            case 'b':
                tempValueForBool = (*RightOperand)[rightIndex].vBool;
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex], &tempValueForBool,
                    instruction, CurrentInstrInfo
                );
                continue;
            case 'i':
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    &(*RightOperand)[rightIndex].vInt, instruction, CurrentInstrInfo
                );
                continue;
            case 'd':
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    &(*RightOperand)[rightIndex].vDouble, instruction, CurrentInstrInfo
                );
                continue;
            default:
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Instance " << rightIndex << " of the right operand has an invalid type: '"
                    << (*RightOperand)[rightIndex].type << "'.\n";
                continue;
        }
    }
}
template <typename LeftType>
void moveAllValuesToEachInstance(vector<LeftType> * LeftOperand,
    vector<VariableModule*> * RightOperand, bool incLeftIdx, bool incRightIdx, unsigned maxIndex,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    int tempValueForBool = 0;
    for(unsigned leftIndex = 0, rightIndex = 0; leftIndex < maxIndex && rightIndex < maxIndex;
        leftIndex+=incLeftIdx, rightIndex+=incRightIdx
    ){
        switch((*RightOperand)[rightIndex]->type){
            case 'b':
                tempValueForBool = (*RightOperand)[rightIndex]->vBool;
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex], &tempValueForBool,
                    instruction, CurrentInstrInfo
                );
                continue;
            case 'i':
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    &(*RightOperand)[rightIndex]->vInt, instruction, CurrentInstrInfo
                );
                continue;
            case 'd':
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    &(*RightOperand)[rightIndex]->vDouble, instruction, CurrentInstrInfo
                );
                continue;
            default:
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Instance " << rightIndex << " of the right operand has an invalid type: '"
                    << (*RightOperand)[rightIndex]->type << "'.\n";
                continue;
        }
    }
}
template <typename LeftType>
void moveAllValuesToEachInstance(vector<LeftType> * LeftOperand,
    vector<BasePointersStruct> * RightOperand, bool incLeftIdx, bool incRightIdx,
    unsigned maxIndex, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    short tempValueForBool = 0;
    for(unsigned leftIndex = 0, rightIndex = 0; leftIndex < maxIndex && rightIndex < maxIndex;
        leftIndex+=incLeftIdx, rightIndex+=incRightIdx
    ){
        switch((*RightOperand)[rightIndex].type){
            case bool_bt:
                tempValueForBool = *(*RightOperand)[rightIndex].pBool;
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex], &tempValueForBool,
                    instruction, CurrentInstrInfo
                );
                continue;
            case char_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pChar, instruction, CurrentInstrInfo
                );
                continue;
            case short_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pShort, instruction, CurrentInstrInfo
                );
                continue;
            case u_short_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pUShort, instruction, CurrentInstrInfo
                );
                continue;
            case int_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pInt, instruction, CurrentInstrInfo
                );
                continue;
            case u_int_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pUInt, instruction, CurrentInstrInfo
                );
                continue;
            case float_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pFloat, instruction, CurrentInstrInfo
                );
                continue;
            case double_bt:
                executeMoveTypeInstruction2(&(*LeftOperand)[leftIndex],
                    (*RightOperand)[rightIndex].pDouble, instruction, CurrentInstrInfo
                );
                continue;
            default:
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Instance " << rightIndex << " of the right operand has an invalid type: '"
                    << baseTypeToStr((*RightOperand)[rightIndex].type) << "'.\n";
                continue;
        }
    }
}
void moveString(vector<string> & vString, vector<VariableModule> * RightOperand, bool incLeftIdx,
    bool incRightIdx, unsigned maxIndex, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    switch(instruction){
        case EngineInstr::move:
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                if((*RightOperand)[rightIndex].type == 's'){
                    vString[leftIndex] = (*RightOperand)[rightIndex].vString;
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute any instructions if only the left operand is of a string"
                        << " type.\n";
                }
            }
        case EngineInstr::add_move:
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                if((*RightOperand)[rightIndex].type == 's'){
                    vString[leftIndex] += (*RightOperand)[rightIndex].vString;
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute any instructions if only the left operand is of a string"
                        << " type.\n";
                }
            }
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "Cannot execute \'" << instrToStr(instruction)
                << "\' instruction on string type values.\n";
    }
}
void moveString(vector<string> & vString, vector<VariableModule*> * RightOperand, bool incLeftIdx,
    bool incRightIdx, unsigned maxIndex, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    switch(instruction){
        case EngineInstr::move:
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                if((*RightOperand)[rightIndex]->type == 's'){
                    vString[leftIndex] = (*RightOperand)[rightIndex]->vString;
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute any instructions if only the left operand is of a string"
                        << " type.\n";
                }
            }
        case EngineInstr::add_move:
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                if((*RightOperand)[rightIndex]->type == 's'){
                    vString[leftIndex] += (*RightOperand)[rightIndex]->vString;
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute any instructions if only the left operand is of a string"
                        << " type.\n";
                }
            }
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "Cannot execute \'" << instrToStr(instruction)
                << "\' instruction on string type values.\n";
    }
}
void moveString(vector<string> & vString, vector<BasePointersStruct> * RightOperand,
    bool incLeftIdx, bool incRightIdx, unsigned maxIndex,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    switch(instruction){
        case EngineInstr::move:
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                if((*RightOperand)[rightIndex].type == 's'){
                    vString[leftIndex] = *(*RightOperand)[rightIndex].pString;
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute any instructions if only the left operand is of a string"
                        << " type.\n";
                }
            }
        case EngineInstr::add_move:
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                if((*RightOperand)[rightIndex].type == 's'){
                    vString[leftIndex] += *(*RightOperand)[rightIndex].pString;
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute any instructions if only the left operand is of a string"
                        << " type.\n";
                }
            }
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "Cannot execute \'" << instrToStr(instruction)
                << "\' instruction on string type values.\n";
    }
}
// requires (std::same_as<RightOperandType, VariableModule>
//     || std::same_as<RightOperandType, BasePointersStruct>
//     || std::same_as<RightOperandType, VariableModule*>)
template<class RightOperandType>
void VectorModule::move(vector<RightOperandType> * RightOperand, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveValueToEachInstance((int*)nullptr, instruction, CurrentInstrInfo);
        return;
    }
    bool incLeftIdx = false, incRightIdx = false;
    unsigned maxIndex = 0;
    switch(type){
        case 'b':{
            if(!checkForVectorSize(CurrentInstrInfo, vBool.size(),
                RightOperand->size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            vector<int> tempVector;
            tempVector.reserve(vBool.size());
            for(const stupidBool & valBool : vBool){
                tempVector.push_back(valBool.value);
            }
            moveAllValuesToEachInstance(&tempVector, RightOperand, incLeftIdx, incRightIdx,
                maxIndex, instruction, CurrentInstrInfo
            );
            for(unsigned boolIndex = 0; boolIndex < vBool.size(); ++boolIndex){
                vBool[boolIndex].value = tempVector[boolIndex] > 0;
            }
            return;}
        case 'i':
            if(!checkForVectorSize(CurrentInstrInfo, vInt.size(),
                RightOperand->size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            moveAllValuesToEachInstance(&vInt, RightOperand, incLeftIdx, incRightIdx, maxIndex,
                instruction, CurrentInstrInfo
            );
            return;
        case 'd':
            if(!checkForVectorSize(CurrentInstrInfo, vDouble.size(),
                RightOperand->size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            moveAllValuesToEachInstance(&vDouble, RightOperand, incLeftIdx, incRightIdx, maxIndex,
                instruction, CurrentInstrInfo
            );
            return;
        case 's':
            if(!checkForVectorSize(CurrentInstrInfo, vString.size(),
                RightOperand->size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            moveString(vString, RightOperand, incLeftIdx, incRightIdx, maxIndex, instruction,
                CurrentInstrInfo
            );
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}
template void VectorModule::move<VariableModule>(vector<VariableModule> * RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
template void VectorModule::move<VariableModule*>(vector<VariableModule*> * RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
template void VectorModule::move<BasePointersStruct>(vector<BasePointersStruct> * RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
void VectorModule::move(BasePointersStruct* RightOperand, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveValueToEachInstance((int*)nullptr, instruction, CurrentInstrInfo);
        return;
    }
    switch(RightOperand->type){
        case bool_bt:{
            char temp = *RightOperand->pBool;
            moveValueToEachInstance(&temp, instruction, CurrentInstrInfo);}
            return;
        case char_bt:
            moveValueToEachInstance(RightOperand->pChar, instruction, CurrentInstrInfo);
            return;
        case short_bt:
            moveValueToEachInstance(RightOperand->pShort, instruction, CurrentInstrInfo);
            return;
        case u_short_bt:
            moveValueToEachInstance(RightOperand->pUShort, instruction, CurrentInstrInfo);
            return;
        case int_bt:
            moveValueToEachInstance(RightOperand->pInt, instruction, CurrentInstrInfo);
            return;
        case u_int_bt:
            moveValueToEachInstance(RightOperand->pUInt, instruction, CurrentInstrInfo);
            return;
        case float_bt:
            moveValueToEachInstance(RightOperand->pFloat, instruction, CurrentInstrInfo);
            return;
        case double_bt:
            moveValueToEachInstance(RightOperand->pDouble, instruction, CurrentInstrInfo);
            return;
        case string_bt:
            if(RightOperand->type == 's'){
                if(instruction == EngineInstr::move){
                    for(string & stringInstance : vString){
                        stringInstance = *RightOperand->pString;
                    }
                }
                else if(instruction == EngineInstr::add_move){
                    for(string & stringInstance : vString){
                        stringInstance += *RightOperand->pString;
                    }
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute \'" << instrToStr(instruction)
                        << "\' instruction on string type values.\n";
                }
            }
            else{
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Cannot execute any instructions if only the right operand is of a string "
                    << "type.\n";
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}

template<typename LeftType>
void checkTypeAndMove(vector<LeftType> & LeftOperand, VectorModule* RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    bool incLeftIdx = false, incRightIdx = false;
    unsigned maxIndex = 0;
    switch(RightOperand->getType()){
        case 'b':{
            if(!checkForVectorSize(CurrentInstrInfo, LeftOperand.size(),
                RightOperand->vBool.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            short boolSubstitute = 0;
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                boolSubstitute = RightOperand->vBool[rightIndex].value;
                executeMoveTypeInstruction2(&LeftOperand[leftIndex], &boolSubstitute, instruction,
                    CurrentInstrInfo
                );
            }
            return;}
        case 'i':
            if(!checkForVectorSize(CurrentInstrInfo, LeftOperand.size(),
                RightOperand->vInt.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                executeMoveTypeInstruction2(&LeftOperand[leftIndex],
                    &RightOperand->vInt[rightIndex], instruction, CurrentInstrInfo
                );
            }
            return;
        case 'd':
            if(!checkForVectorSize(CurrentInstrInfo, LeftOperand.size(),
                RightOperand->vDouble.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
            ){
                return;
            }
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                executeMoveTypeInstruction2(&LeftOperand[leftIndex], 
                    &RightOperand->vDouble[rightIndex], instruction, CurrentInstrInfo
                );
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << RightOperand->getType()
                << "\' type is not valid.\n";
            return;
    }
}
void moveString(vector<string> & leftOperand, VectorModule* RightOperand,
    const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo
){
    if(RightOperand->getType() == 's'){
        bool incLeftIdx = false, incRightIdx = false;
        unsigned maxIndex = 0;
        if(!checkForVectorSize(CurrentInstrInfo, leftOperand.size(),
            RightOperand->vString.size(), incLeftIdx, incRightIdx, maxIndex, __FUNCTION__)
        ){
            return;
        }
        if(instruction == EngineInstr::move){
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                leftOperand[leftIndex] = RightOperand->vString[rightIndex];
            }
        }
        else if(instruction == EngineInstr::add_move){
            for(unsigned leftIndex = 0, rightIndex = 0;
                leftIndex < maxIndex && rightIndex < maxIndex;
                leftIndex+=incLeftIdx, rightIndex+=incRightIdx
            ){
                leftOperand[leftIndex] += RightOperand->vString[rightIndex];
            }
        }
        else{
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "Cannot execute \'" << instrToStr(instruction)
                << "\' instruction on string type values.\n";
        }
    }
    else{
        cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
            << "Cannot execute any instructions if only the right operand is of a string type.\n";
    }
}
void VectorModule::move(VectorModule* RightOperand, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveValueToEachInstance((int*)nullptr, instruction, CurrentInstrInfo);
        return;
    }
    switch(type){
        case 'b':{
            vector<short> boolSubstitute;
            for(const stupidBool & valBool : vBool){
                boolSubstitute.push_back(valBool.value);
            }
            checkTypeAndMove(boolSubstitute, RightOperand, instruction, CurrentInstrInfo);
            for(unsigned boolIndex = 0; boolIndex < boolSubstitute.size(); ++boolIndex){
                vBool[boolIndex].value = boolSubstitute[boolIndex] > 0;
            }
            return;}
        case 'i':
            checkTypeAndMove(vInt, RightOperand, instruction, CurrentInstrInfo);
            return;
        case 'd':
            checkTypeAndMove(vDouble, RightOperand, instruction, CurrentInstrInfo);
            return;
        case 's':
            moveString(vString, RightOperand, instruction, CurrentInstrInfo);
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}
void VectorModule::moveFrom(VariableModule * LeftOperand, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        LeftOperand->moveFromTemp((int*)nullptr, instruction, CurrentInstrInfo);
        return;
    }
    switch(type){
        case 'b':{
            int temp;
            for(const stupidBool & valBool : vBool){
                temp = valBool.value;
                LeftOperand->moveFromTemp(&temp, instruction, CurrentInstrInfo);
            }
            return;}
        case 'i':
            for(int valInt : vInt){
                LeftOperand->moveFromTemp(&valInt, instruction, CurrentInstrInfo);
            }
            return;
        case 'd':
            for(double valDouble : vDouble){
                LeftOperand->moveFromTemp(&valDouble, instruction, CurrentInstrInfo);
            }
            return;
        case 's':
            if(LeftOperand->type == 's'){
                if(instruction == EngineInstr::move){
                    LeftOperand->vString = "";
                    for(const string & text : vString){
                        LeftOperand->vString += text;
                    }
                }
                else if(instruction == EngineInstr::add_move){
                    for(const string & text : vString){
                        LeftOperand->vString += text;
                    }
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute \'" << instrToStr(instruction)
                        << "\' instruction on string type values.\n";
                }
            }
            else{
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Cannot execute any instructions if only the right operand is of a string "
                    << "type.\n";
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}
void VectorModule::moveFrom(BasePointersStruct * LeftPointer, const EngineInstr & instruction,
    const InstrDescription & CurrentInstrInfo
){
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        LeftPointer->moveFromTemp((int*)nullptr, instruction);
        return;
    }
    switch(type){
        case 'b':{
            int temp;
            for(const stupidBool & valBool : vBool){
                temp = valBool.value;
                LeftPointer->moveFromTemp(&temp, instruction);
            }
            return;}
        case 'i':
            for(const int & valInt : vInt){
                LeftPointer->moveFromTemp(&valInt, instruction);
            }
            return;
        case 'd':
            for(const double & valDouble : vDouble){
                LeftPointer->moveFromTemp(&valDouble, instruction);
            }
            return;
        case 's':
            if(LeftPointer->type == 's'){
                if(instruction == EngineInstr::move){
                    *LeftPointer->pString = "";
                    for(const string & text : vString){
                        *LeftPointer->pString += text;
                    }
                }
                else if(instruction == EngineInstr::add_move){
                    for(const string & text : vString){
                        *LeftPointer->pString += text;
                    }
                }
                else{
                    cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                        << "Cannot execute \'" << instrToStr(instruction)
                        << "\' instruction on string type values.\n";
                }
            }
            else{
                cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                    << "Cannot execute any instructions if only the right operand is of a string "
                    << "type.\n";
            }
            return;
        default:
            cerr << printErrorMessage(CurrentInstrInfo, __FUNCTION__)
                << "About the right operand: \'" << type << "\' type is not valid.\n";
            return;
    }
}
ReturnType VectorModule::setVariableWithFirstValue(VariableModule &Variable){
    switch(type){
        case 'b':
            if(vBool.size() == 0){
                return ReturnType::EMPTY;
            }
            Variable.setBool(vBool[0].value);
            return ReturnType::OK;
        case 'i':
            if(vInt.size() == 0){
                return ReturnType::EMPTY;
            }
            Variable.setInt(vInt[0]);
            return ReturnType::OK;
        case 'd':
            if(vDouble.size() == 0){
                return ReturnType::EMPTY;
            }
            Variable.setDouble(vDouble[0]);
            return ReturnType::OK;
        case 's':
            if(vString.size() == 0){
                return ReturnType::EMPTY;
            }
            Variable.setString(vString[0]);
            return ReturnType::OK;
        case 'n':
            return ReturnType::NULL_VAL;
        default:
            return ReturnType::OK;
    }
}

bool VectorModule::getIsDeleted() const{
    return deleted;
}
string VectorModule::getID() const{
    return ID;
}
string & VectorModule::getIDAddr(){
    return ID;
}
string VectorModule::getLayerID() const{
    return layerID;
}
string VectorModule::getObjectID() const{
    return objectID;
}
char VectorModule::getType() const{
    return type;
}
string VectorModule::getFullTypeName() const{
    switch(type){
        case 'b':
            return "bool";
        case 'i':
            return "int";
        case 'd':
            return "double";
        case 's':
            return "string";
        default:
            return "null";
    }
}
VariableModule VectorModule::getValue(AttributeType attribute, size_t index) const{
    if(attribute == back_a){
        switch(type) {
            case 'b':
                return VariableModule::newBool(getLastBool());
            case 'i':
                return VariableModule::newInt(getLastInt());
            case 'd':
                return VariableModule::newDouble(getLastDouble());
            case 's':
                return VariableModule::newString(getLastString());
            default:
                cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Type '" << type
                    << "' is not valid for last value extraction.\n";
                break;
        }
    }
    else if(attribute == value){
        switch(type){
            case 'b':
                return VariableModule::newBool(getBool(index));
            case 'i':
                return VariableModule::newInt(getInt(index));
            case 'd':
                return VariableModule::newDouble(getDouble(index));
            case 's':
                return VariableModule::newString(getString(index));
            default:
                cout << "Warning: In " << __PRETTY_FUNCTION__
                    << ": Type '" << type << "' is not valid for value extraction.\n";
                break;
        }
    }
    else if(attribute == size_a){
        return VariableModule::newInt(getSize());
    }
    else{
        cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Attribute '"
            << attributeToStr(attribute) << "' does not exist in the context of Vector module.\n";
    }
    return VariableModule::newBool(false, "null");
}
vector<VariableModule> VectorModule::getValues() const {
    vector<VariableModule> Vector;
    switch(type){
        case 'b':
            for(stupidBool value : vBool){
                Vector.emplace_back(VariableModule::newBool(value.value));
            }
            return Vector;
        case 'i':
            for(int value : vInt){
                Vector.emplace_back(VariableModule::newInt(value));
            }
            return Vector;
        case 'd':
            for(double value : vDouble){
                Vector.emplace_back(VariableModule::newDouble(value));
            }
            return Vector;
        case 's':
            for(string value : vString){
                Vector.emplace_back(VariableModule::newString(value));
            }
            return Vector;
        case 'n':
            return Vector;
        default:
            cout << "Warning: In " << __PRETTY_FUNCTION__ << ": Type '" << type
                << "' is not valid.\n";
            return Vector;
    }
}
ReturnType VectorModule::getValuesIntoContext(vector<VariableModule> & Values, bool negate) const {
    if(negate){
        return getNegatedValuesIntoContext(Values);
    }
    switch(type){
        case 'b':
            for(const stupidBool & value : vBool){
                Values.emplace_back(VariableModule(value.value));
            }
            return ReturnType::OK;
        case 'i':
            for(const int & value : vInt){
                Values.emplace_back(VariableModule(value));
            }
            return ReturnType::OK;
        case 'd':
            for(const double & value : vDouble){
                Values.emplace_back(VariableModule(value));
            }
            return ReturnType::OK;
        case 's':
            for(const string & value : vString){
                Values.emplace_back(VariableModule(value));
            }
            return ReturnType::OK;
        case 'n':
            return ReturnType::NULL_VAL;
        default:
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": Type '" << type
                << "' is not valid.\n";
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::INVALID_TYPE;
}
ReturnType VectorModule::getNegatedValuesIntoContext(vector<VariableModule> & Values) const {
    switch(type){
        case 'b':
            for(const stupidBool & value : vBool){
                Values.emplace_back(VariableModule(value.value).negate());
            }
            return ReturnType::OK;
        case 'i':
            for(const int & value : vInt){
                Values.emplace_back(VariableModule(value).negate());
            }
            return ReturnType::OK;
        case 'd':
            for(const double & value : vDouble){
                Values.emplace_back(VariableModule(value).negate());
            }
            return ReturnType::OK;
        case 's':
            for(const string & value : vString){
                Values.emplace_back(VariableModule(value).negate());
            }
            return ReturnType::OK;
        case 'n':
            return ReturnType::NULL_VAL;
        default:
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": Type '" << type
                << "' is not valid.\n";
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::INVALID_TYPE;
}
ReturnType VectorModule::getDoubles(vector<double> & Values, bool negate) const {
    if(negate){
        return getNegatedDoubles(Values);
    }
    switch(type){
        case 'b':
            for(stupidBool value : vBool){
                Values.push_back(value.value);
            }
            return ReturnType::OK;
        case 'i':
            for(int value : vInt){
                Values.push_back(value);
            }
            return ReturnType::OK;
        case 'd':
            for(double value : vDouble){
                Values.push_back(value);
            }
            return ReturnType::OK;
        case 'n':
            return ReturnType::NULL_VAL;
        case 's':
        default:
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": Type '" << type
                << "' is not valid.\n";
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::INVALID_TYPE;
}
ReturnType VectorModule::getNegatedDoubles(vector<double> & Values) const {
    switch(type){
        case 'b':
            for(stupidBool value : vBool){
                Values.push_back(!value.value);
            }
            return ReturnType::OK;
        case 'i':
            for(int value : vInt){
                Values.push_back(-value);
            }
            return ReturnType::OK;
        case 'd':
            for(double value : vDouble){
                Values.push_back(-value);
            }
            return ReturnType::OK;
        case 'n':
            return ReturnType::NULL_VAL;
        case 's':
        default:
            cout << "Error: In " << __PRETTY_FUNCTION__ << ": Type '" << type
                << "' is not valid.\n";
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::INVALID_TYPE;
}
string VectorModule::getAnyStringValue(size_t index) const{
    if(type == 'b' && index < vBool.size()){
        if(vBool[index].value){
            return "true";
        }
        return "false";
    }
    if(type == 'i' && index < vInt.size()){
        return intToStr(vInt[index]);
    }
    if(type == 'd' && index < vDouble.size()){
        return doubleToStr(vDouble[index]);
    }
    if(type == 's' && index < vString.size()){
        return vString[index];
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return "";
}
string VectorModule::getAnyStringValueUnsafe(long index) const{
    if(type == 'b'){
        if(getBoolUnsafe(index)){
            return "true";
        }
        return "false";
    }
    if(type == 'i'){
        return intToStr(getIntUnsafe(index));
    }
    if(type == 'd'){
        return doubleToStr(getDoubleUnsafe(index));
    }
    if(type == 's'){
        return getStringUnsafe(index);
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return "";
}
vector<string> VectorModule::getAllValuesAsStringVector() const{
    vector<string> newStringVector;
    if(type == 'b'){
        for(size_t index = 0; index < vBool.size(); index++){
            if(vBool[index].value)
                newStringVector.emplace_back("true");
            else
                newStringVector.emplace_back("false");
        }
    }
    else if(type == 'i'){
        for(size_t index = 0; index < vInt.size(); index++)
            newStringVector.emplace_back(intToStr(vInt[index]));
    }
    else if(type == 'd'){
        for(size_t index = 0; index < vDouble.size(); index++)
            newStringVector.emplace_back(doubleToStr(vDouble[index]));
    }
    else if(type == 's')
        return vString;
    else
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return newStringVector;
}
bool VectorModule::getBool(size_t index) const{
    if(index >= vBool.size()){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vBool.size() << ".\n";
        return false;
    }
    return vBool[index].value;
}
bool VectorModule::getBoolUnsafe(long index) const{
    if(vBool.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    
    if(index >= 0){
        index %= vBool.size();
    }
    else{
        index = vBool.size() - ((-index) % vBool.size());
    }

    return vBool[index].value;
}
vector<stupidBool> VectorModule::getAllBools() const{
    return vBool;
}
vector<VariableModule> VectorModule::getAllBoolsAsVariables(){
    vector<VariableModule> Values;
    for(stupidBool stupidValue : vBool){
        Values.emplace_back(VariableModule::newBool(stupidValue.value));
    }
    return Values;
}
int VectorModule::getInt(size_t index) const{
    if(index >= vInt.size()){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vInt.size() << ".\n";
        return 0;
    }
    return vInt[index];
}
int VectorModule::getIntUnsafe(long index) const{
    if(vInt.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0;
    }
    
    if(index >= 0){
        index %= vInt.size();
    }
    else{
        index = vInt.size() - ((-index) % vInt.size());
    }
    
    return vInt[index];
}
vector<int> VectorModule::getAllInts() const{
    return vInt;
}
vector<VariableModule> VectorModule::getAllIntsAsVariables(){
    vector<VariableModule> Values;
    for(int value : vInt){
        Values.emplace_back(VariableModule::newInt(value));
    }
    return Values;
}
double VectorModule::getDouble(size_t index) const{
    if(index >= vDouble.size()){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vDouble.size() << ".\n";
        return 0.0;
    }
    return vDouble[index];
}
double VectorModule::getDoubleUnsafe(long index) const{
    if(vDouble.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0.0;
    }
    
    if(index >= 0){
        index %= vDouble.size();
    }
    else{
        index = vDouble.size() - ((-index) % vDouble.size());
    }
    
    return vDouble[index];
}
vector<double> VectorModule::getAllDoubles() const{
    return vDouble;
}
vector<VariableModule> VectorModule::getAllDoublesAsVariables(){
    vector<VariableModule> Values;
    for(double value : vDouble){
        Values.emplace_back(VariableModule::newDouble(value));
    }
    return Values;
}
string VectorModule::getString(size_t index) const{
    if(index >= vString.size()){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Index " << index
            << " is out of scope of the vector '" << ID << "' of size " << vString.size() << ".\n";
        return "";
    }
    return vString[index];
}
string VectorModule::getStringUnsafe(long index) const{
    if(vString.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    
    if(index >= 0){
        index %= vString.size();
    }
    else{
        index = vString.size() - ((-index) % vString.size());
    }
    
    return vString[index];
}
string VectorModule::getLastValue() const{
    if(type == 'b' && vBool.size() > 0){
        if(vBool.back().value){
            return "true";
        }
        return "false";
    }
    if(type == 'i' && vInt.size() > 0){
        return intToStr(vInt.back());
    }
    if(type == 'd' && vDouble.size() > 0){
        return doubleToStr(vDouble.back());
    }
    if(type == 's' && vString.size() > 0){
        return vString.back();
    }
    cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
    return "";
}
vector<string> VectorModule::getAllStrings() const{
    return vString;
}
void VectorModule::getAllStrings(vector<string> & values) const{
    values.insert(values.end(), vString.begin(), vString.end());
}
vector<VariableModule> VectorModule::getAllStringsAsVariables(){
    vector<VariableModule> Values;
    for(string value : vString){
        Values.emplace_back(VariableModule::newString(value));
    }
    return Values;
}
bool VectorModule::getLastBool() const
{
    if(vBool.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vBool.back().value;
}
int VectorModule::getLastInt() const{
    if(vInt.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vInt.back();
}
double VectorModule::getLastDouble() const{
    if(vDouble.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vDouble.back();
}
string VectorModule::getLastString() const{
    if(vString.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    return vString.back();
}
bool VectorModule::getFirstBool() const{
    if(vBool.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return false;
    }
    return vBool[0].value;
}
int VectorModule::getFirstInt() const{
    if(vInt.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0;
    }
    return vInt[0];
}
double VectorModule::getFirstDouble() const{
    if(vDouble.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return 0.0;
    }
    return vDouble[0];
}
string VectorModule::getFirstString() const{
    if(vString.size() == 0){
        cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Vector '" << ID << " is empty.\n";
        return "";
    }
    return vString[0];
}
size_t VectorModule::getSize() const{
    switch (type) {
        case 'b':
            return vBool.size();
        case 'i':
            return vInt.size();
        case 'd':
            return vDouble.size();
        case 's':
            return vString.size();
        default:
            return 0;
    }
    return 0;
}

void VectorModule::getContext(AttributeType attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.emplace_back(BasePointersStruct());
    if(attribute == id){
        if(isStringInVector(reservedIDs, ID)){
            cerr << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << ID
                << "\' address was denied.\n";
            BasePointers.pop_back();
            return;
        }
        BasePointers.back().setPointer(&ID);
        BasePointers.back().readOnly = true;
    }
    else if(attribute == type_a){
        BasePointers.back().setPointer(&type);
    }
    else if(getSize() > 0){
        switch(attribute){
            case last_bool:
                BasePointers.back().setPointer(&vBool.back().value);
                break;
            case last_int:
                BasePointers.back().setPointer(&vInt.back());
                break;
            case last_double:
                BasePointers.back().setPointer(&vDouble.back());
                break;
            case last_string:
                BasePointers.back().setPointer(&vString.back());
                break;
            case first_bool:
                BasePointers.back().setPointer(&vBool[0].value);
                break;
            case first_int:
                BasePointers.back().setPointer(&vInt[0]);
                break;
            case first_double:
                BasePointers.back().setPointer(&vDouble[0]);
                break;
            case first_string:
                BasePointers.back().setPointer(&vString[0]);
                break;
            case random_bool:
                BasePointers.back().setPointer(&vBool[rand() % vBool.size()].value);
                break;
            case random_int:
                BasePointers.back().setPointer(&vInt[rand() % vInt.size()]);
                break;
            case random_double:
                BasePointers.back().setPointer(&vDouble[rand() % vDouble.size()]);
                break;
            case random_string:
                BasePointers.back().setPointer(&vString[rand() % vString.size()]);
                break;
            default:
                BasePointers.pop_back();
                cerr << "Error: In " << __FUNCTION__ << ": Attribute '"
                    << attributeToStr(attribute) << "' is not valid.\n";
                break;
        }
    }
    else{
        BasePointers.pop_back();
        if(getSize() == 0){
            cerr << "Error: In " << __FUNCTION__ << ": Vector '" << ID << "' is empty.\n";
        }
        else{
            cerr << "Error: In " << __FUNCTION__ << ": Attribute '" << attribute
                << "' is not valid.\n";
        }
    }
}

BasePointersStruct VectorModule::getBasePointersStruct(const unsigned & index){
    BasePointersStruct NewPointer;

    if(index >= getSize()){
        
        return NewPointer;
    }

    switch (type) {
        case 'b':
            NewPointer.type = bool_bt;
            NewPointer.pBool = &vBool[index].value;
            break;
        case 'i':
            NewPointer.type = int_bt;
            NewPointer.pInt = &vInt[index];
            break;
        case 'd':
            NewPointer.type = double_bt;
            NewPointer.pDouble = &vDouble[index];
            break;
        case 's':
            NewPointer.type = string_bt;
            NewPointer.pString = &vString[index];
            break;
        default:
            NewPointer.type = null_bt;
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Type \'" << type
                << "\' is not a valid.\n";
            break;
    }

    return NewPointer;
}

bool VectorModule::contains(const VariableModule &Variable){
    if(type != Variable.getType()){
        return false;
    }
    if(type == 'b'){
        if(std::find(vBool.begin(), vBool.end(), Variable.getBool()) != vBool.end()){
            return true;
        }
    }
    else if(type == 'i'){
        if(std::find(vInt.begin(), vInt.end(), Variable.getInt()) != vInt.end()){
            return true;
        }
    }
    else if(type == 'd'){
        if(std::find(vDouble.begin(), vDouble.end(), Variable.getDouble()) != vDouble.end()){
            return true;
        }
    }
    else if(type == 's'){
        if(std::find(vString.begin(), vString.end(), Variable.getString()) != vString.end()){
            return true;
        }
    }
    return false;
}
bool VectorModule::contains(const BasePointersStruct &Pointer){
    if(type == 'b'){
        if(std::find(vBool.begin(), vBool.end(), Pointer.getBool()) != vBool.end()){
            return true;
        }
    }
    else if(type == 'i'){
        if(std::find(vInt.begin(), vInt.end(), Pointer.getInt()) != vInt.end()){
            return true;
        }
    }
    else if(type == 'd'){
        if(std::find(vDouble.begin(), vDouble.end(), Pointer.getDouble()) != vDouble.end()){
            return true;
        }
    }
    else if(type == 's'){
        if(std::find(vString.begin(), vString.end(), Pointer.getString()) != vString.end()){
            return true;
        }
    }
    return false;
}

bool operator==(stupidBool sb, const bool b){
    return sb.value == b;
}
