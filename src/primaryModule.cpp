#include "primaryModule.h"


void PrimaryModule::primaryConstructor(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID){
    setAllIDs(newID, listOfIDs, newLayerID, newObjectID, true);
    
    pos.set(0.0, 0.0);
    scrollShift.set(0.0, 0.0);
    size.set(100.0, 100.0);
    scale.set(0.0, 0.0);

    isActive = true;
    deleted = false;
    isScaledFromCenter = false;
    isAttachedToCamera = false;
    canBeSelected = true;
    isScrollable = false;
}
void PrimaryModule::primaryConstructor(unsigned newID, vector<string> & listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(intToStr(newID), listOfIDs, newLayerID, newObjectID);
}
void PrimaryModule::clone(const PrimaryModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}

void PrimaryModule::setID(string newID, vector<string> & listOfIDs){
    if(isStringInVector(reservedIDs, ID)){
        std::cout << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(listOfIDs, ID);
    ID = findNewUniqueID(listOfIDs, newID);
    listOfIDs.push_back(ID);
}

void PrimaryModule::setLayerID(string newLayerID){
    layerID = newLayerID;
}

void PrimaryModule::setObjectID(string newOwnerID){
    objectID = newOwnerID;
}

void PrimaryModule::setAllIDs(string newID, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    if(changeOldID){
        setID(newID, listOfIDs);
    }
    setLayerID(newLayerID);
    setObjectID(newObjectID);
}

void PrimaryModule::addGroup(string newGroup){
    addUniqueToStringVector(groups, newGroup);
}
void PrimaryModule::removeGroup(string selectedGroup){
    removeFromStringVector(groups, selectedGroup);
}
void PrimaryModule::clearGroups(){
    groups.clear();
}
bool PrimaryModule::isInAGroup(string findGroup) const{
    return isStringInVector(groups, findGroup);
}
vector <string> PrimaryModule::getGroups() const{
    return groups;
}
vector <string> & PrimaryModule::getGroupsAddr(){
    return groups;
}

void PrimaryModule::setPos(vec2d newPos){
    pos.set(newPos);
}
void PrimaryModule::setPos(double x, double y){
    pos.set(x, y);
}
void PrimaryModule::translatePos(vec2d newPos){
    pos.translate(newPos);
}
void PrimaryModule::setScrollShift(vec2d newValue){
    scrollShift = newValue;
}
void PrimaryModule::setSize(vec2d newSize){
    size.set(newSize);
}
void PrimaryModule::setSize(double x, double y){
    size.set(x, y);
}
void PrimaryModule::setScale(vec2d newScale){
    scale.set(newScale);
}
void PrimaryModule::setScale(double x, double y){
    scale.set(x, y);
}
void PrimaryModule::addScale(vec2d newScale){
    scale.translate(newScale);
}
void PrimaryModule::resize(vec2d newSize){
    scale.set(newSize.x/size.x, newSize.y/size.y);
}
void PrimaryModule::resize(double newSizeX, double newSizeY){
    scale.set(newSizeX/size.x, newSizeY/size.y);
}
void PrimaryModule::resizeX(double newSizeX){
    scale.x = newSizeX/size.x;
}
void PrimaryModule::resizeY(double newSizeY){
    scale.y = newSizeY/size.y;
}
void PrimaryModule::setIsActive(bool newValue){
    isActive = newValue;
}
void PrimaryModule::activate(){
    setIsActive(true);
}
void PrimaryModule::deactivate(){
    setIsActive(false);
}
void PrimaryModule::toggleIsActive(){
    setIsActive(!getIsActive());
}
void PrimaryModule::setIsScaledFromCenter(bool newIsScaledFromCenter){
    isScaledFromCenter = newIsScaledFromCenter;
}
void PrimaryModule::control(const string &attribute, const bool &value){
    if(attribute == "activate"){
        activate();
    }
    else if(attribute == "deactivate"){
        deactivate();
    }
    else if(attribute == "toggle"){
        toggleIsActive();
    }
    else if(attribute == "scale_from_center"){
        isScaledFromCenter = true;
    }
    else if(attribute == "do_not_scale_from_center"){
        isScaledFromCenter = false;
    }
    else if(attribute == "set_scale_from_center"){
        isScaledFromCenter = value;
    }
    else if(attribute == "attach_to_camera"){
        isAttachedToCamera = true;
    }
    else if(attribute == "detach_from_camera"){
        isAttachedToCamera = false;
    }
    else if(attribute == "set_camera_attachment"){
        isAttachedToCamera = value;
    }
    else if(attribute == "allow_selection"){
        canBeSelected = true;
    }
    else if(attribute == "forbid_selection"){
        canBeSelected = false;
    }
    else if(attribute == "set_can_be_selected"){
        canBeSelected = value;
    }
    else if(attribute == "allow_scrolling"){
        isScrollable = true;
    }
    else if(attribute == "forbid_scrolling"){
        isScrollable = false;
    }
    else if(attribute == "set_is_scrollable"){
        isScrollable = value;
    }
    else{
        std::cout << "Error: In: " << __FUNCTION__ << ": attribute \'" << attribute << "\' does not exist.\n";
    }
}
void PrimaryModule::setIsAttachedToCamera(bool newIsAttachedToCamera)
{
    isAttachedToCamera = newIsAttachedToCamera;
}
void PrimaryModule::setCanBeSelected(bool newValue){
    canBeSelected = newValue;
}
void PrimaryModule::setIsScrollable(bool newValue){
    isScrollable = newValue;
}
string PrimaryModule::getID() const{
    return ID;
}
string PrimaryModule::getLayerID() const{
    return layerID;
}
string &PrimaryModule::getLayerIDAddr(){
    return layerID;
}
string &PrimaryModule::getIDAddr(){
    return ID;
}
string PrimaryModule::getObjectID() const{
    return objectID;
}
vec2d PrimaryModule::getPos(bool useScrollshift){
    if(!useScrollshift){
        return pos;
    }
    return pos-scrollShift;
}
vec2d& PrimaryModule::getPosAddr(){
    return pos;
}
vec2d PrimaryModule::getSize(){
    return size;
}
vec2d& PrimaryModule::getSizeAddr(){
    return size;
}
vec2d PrimaryModule::getScale(){
    return scale;
}
vec2d PrimaryModule::getScaledSize(){
    vec2d scaledSize(size);
    scaledSize.multiply(scale);
    return scaledSize;
}
bool PrimaryModule::getIsActive() const{
    return isActive;
}
bool PrimaryModule::getIsDeleted() const
{
    return deleted;
}
bool PrimaryModule::getIsScaledFromCenter()
{
    return isScaledFromCenter;
}
bool PrimaryModule::getIsAttachedToCamera(){
    return isAttachedToCamera;
}
bool PrimaryModule::getCanBeSelected(){
    return canBeSelected;
}
bool PrimaryModule::getIsScrollable(){
    return isScrollable;
}
void PrimaryModule::deleteLater(){
    deleted = true;
    deactivate();
}
void PrimaryModule::getPrimaryContext(string attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "id"){
        if(isStringInVector(reservedIDs, ID)){
            std::cout << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << ID << "\' address was denied.\n";
            BasePointers.pop_back();
            return;
        }
        BasePointers.back().setPointer(&ID);
        BasePointers.back().readOnly = true;
    }
    else if(attribute == "group"){
        BasePointers.pop_back();
        for(string & group : groups){
            BasePointers.push_back(BasePointersStruct());
            BasePointers.back().setPointer(&group);
        }
    }
    else if(attribute == "pos_x"){
        BasePointers.back().setPointer(&pos.x);
    }
    else if(attribute == "pos_y"){
        BasePointers.back().setPointer(&pos.y);
    }
    else if(attribute == "size_x"){
        BasePointers.back().setPointer(&size.x);
    }
    else if(attribute == "size_y"){
        BasePointers.back().setPointer(&size.y);
    }
    else if(attribute == "scale_x"){
        BasePointers.back().setPointer(&scale.x);
    }
    else if(attribute == "scale_y"){
        BasePointers.back().setPointer(&scale.y);
    }
    else if(attribute == "is_active"){
        BasePointers.back().setPointer(&isActive);
    }
    else if(attribute == "is_scaled_from_center"){
        BasePointers.back().setPointer(&isScaledFromCenter);
    }
    else if(attribute == "is_attached_to_camera"){
        BasePointers.back().setPointer(&isAttachedToCamera);
    }
    else if(attribute == "can_be_selected"){
        BasePointers.back().setPointer(&canBeSelected);
    }
    else if(attribute == "is_scrollable"){
        BasePointers.back().setPointer(&isScrollable);
    }
    else{
        BasePointers.pop_back();
        std::cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
    }
}
BasePointersStruct::BasePointersStruct(){
    type = "";
    readOnly = false;
}
void BasePointersStruct::clear(){
    type = "";
    readOnly = false;
    pBool = nullptr;
    pShort = nullptr;
    pUShort = nullptr;
    pInt = nullptr;
    pUInt = nullptr;
    pDouble = nullptr;
    pString = nullptr;
}
template<typename LeftType, typename RightType>
void BasePointersStruct::executeMoveTypeInstruction(LeftType * LeftOperand, const RightType * RightOperand, string instruction){
    if(instruction == "++"){
        (*LeftOperand)++;
        return;
    }
    else if(instruction == "--"){
        (*LeftOperand)--;
        return;
    }
    if(LeftOperand == nullptr){
        std::cout << "Error: In " << __FUNCTION__ << ": Left operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(RightOperand == nullptr){
        std::cout << "Error: In " << __FUNCTION__ << ": Right operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(instruction == "=" || instruction == "clone"){
        *LeftOperand = *RightOperand;
    }
    else if(instruction == "+="){
        *LeftOperand += *RightOperand;
    }
    else if(instruction == "-="){
        *LeftOperand -= *RightOperand;
    }
    else if(instruction == "*="){
        *LeftOperand *= *RightOperand;
    }
    else if(instruction == "/="){
        if(*RightOperand != 0){
            *LeftOperand /= *RightOperand;
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot divide by zero.\n";
        }
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << instruction << "\' is not a valid instruction.\n";
    }
}
template<typename LeftType>
void BaseVariableStruct::moveValue(LeftType * LeftOperand) const{
    if(type == "bool"){
        *LeftOperand = vBool;
    }
    else if(type == "char"){
        *LeftOperand = vChar;
    }
    else if(type == "short"){
        *LeftOperand = vShort;
    }
    else if(type == "unsigned_short"){
        *LeftOperand = vUShort;
    }
    else if(type == "int"){
        *LeftOperand = vInt;
    }
    else if(type == "unsigned_int"){
        *LeftOperand = vUInt;
    }
    else if(type == "float"){
        *LeftOperand = vFloat;
    }
    else if(type == "double"){
        *LeftOperand = vDouble;
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
    }
}
string BaseVariableStruct::getString() const{
    if(type == "string"){
        return vString;
    }
    else if(type == "bool"){
        if(vBool){
            return "true";
        }
        return "false";
    }
    else if(type == "char"){
        return string(1, vChar);
    }
    else if(type == "short"){
        return shortToStr(vShort);
    }
    else if(type == "unsigned_short"){
        return intToStr(vUShort);
    }
    else if(type == "int"){
        return intToStr(vInt);
    }
    else if(type == "unsigned_int"){
        return uIntToStr(vUInt);
    }
    else if(type == "float"){
        return doubleToStr(vFloat);
    }
    else if(type == "double"){
        return doubleToStr(vDouble);
    }
    
    std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
    return "[invalid type]";
}
void BasePointersStruct::tryToSetValue(const BaseVariableStruct & RightOperand){
    if(readOnly){
        std::cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string"){
            *pString = RightOperand.getString();
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot assign string type value to a non-string type variable.\n";
        }
    }
    else if(type == "bool"){
        RightOperand.moveValue(pBool);
    }
    else if(type == "char"){
        RightOperand.moveValue(pChar);
    }
    else if(type == "short"){
        RightOperand.moveValue(pShort);
    }
    else if(type == "unsigned_short"){
        RightOperand.moveValue(pUShort);
    }
    else if(type == "int"){
        RightOperand.moveValue(pInt);
    }
    else if(type == "unsigned_int"){
        RightOperand.moveValue(pUInt);
    }
    else if(type == "float"){
        RightOperand.moveValue(pFloat);
    }
    else if(type == "double"){
        RightOperand.moveValue(pDouble);
    }
}

template<typename RightType>
void BasePointersStruct::moveFromTemp(const RightType * RightOperand, string instruction){
    if(readOnly){
        std::cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "bool"){
        short temp = *pBool;
        executeMoveTypeInstruction(&temp, RightOperand, instruction);
        *pBool = temp > 0;
    }
    else if(type == "char"){
        executeMoveTypeInstruction(pChar, RightOperand, instruction);
    }
    else if(type == "short"){
        executeMoveTypeInstruction(pShort, RightOperand, instruction);
    }
    else if(type == "unsigned_short"){
        executeMoveTypeInstruction(pUShort, RightOperand, instruction);
    }
    else if(type == "int"){
        executeMoveTypeInstruction(pInt, RightOperand, instruction);
    }
    else if(type == "unsigned_int"){
        executeMoveTypeInstruction(pUInt, RightOperand, instruction);
    }
    else if(type == "float"){
        executeMoveTypeInstruction(pFloat, RightOperand, instruction);
    }
    else if(type == "double"){
        executeMoveTypeInstruction(pDouble, RightOperand, instruction);
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type.\n"; 
    }
}
void BasePointersStruct::move(const BasePointersStruct &RightOperand, string instruction){
    if(readOnly){
        std::cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string"){
            if(instruction == "="){
                *pString = RightOperand.getString();
            }
            else if(instruction == "+="){
                *pString += RightOperand.getString();
            }
            else{
                std::cout << "Error: In " << __FUNCTION__ << ": You cannot execute \'" << instruction << "\' instruction on string type values.\n";
            }
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot execute any instructions if only the right operand is of a string type.\n";
        }
    }
    else if(RightOperand.type == "bool"){
        short temp = *RightOperand.pBool;
        moveFromTemp(&temp, instruction);
    }
    else if(RightOperand.type == "char"){
        moveFromTemp(RightOperand.pChar, instruction);
    }
    else if(RightOperand.type == "short"){
        moveFromTemp(RightOperand.pShort, instruction);
    }
    else if(RightOperand.type == "unsigned_short"){
        moveFromTemp(RightOperand.pUShort, instruction);
    }
    else if(RightOperand.type == "int"){
        moveFromTemp(RightOperand.pInt, instruction);
    }
    else if(RightOperand.type == "unsigned_int"){
        moveFromTemp(RightOperand.pUInt, instruction);
    }
    else if(RightOperand.type == "float"){
        moveFromTemp(RightOperand.pFloat, instruction);
    }
    else if(RightOperand.type == "double"){
        moveFromTemp(RightOperand.pDouble, instruction);
    }
    else if(instruction == "++" || instruction == "--"){
        moveFromTemp((short*)nullptr, instruction);
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << RightOperand.type << "\' is not a valid type.\n"; 
    }
}
void BasePointersStruct::move(const BaseVariableStruct & RightOperand, string instruction){
    if(readOnly){
        std::cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string"){
            if(instruction == "="){
                *pString = RightOperand.getString();
            }
            else if(instruction == "+="){
                *pString += RightOperand.getString();
            }
            else{
                std::cout << "Error: In " << __FUNCTION__ << ": You cannot execute \'" << instruction << "\' instruction on string type values.\n";
            }
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot execute any instructions if only the right operand is of a string type.\n";
        }
    }
    else if(RightOperand.type == "bool"){
        short temp = RightOperand.vBool;
        moveFromTemp(&temp, instruction);
    }
    else if(RightOperand.type == "char"){
        moveFromTemp(&RightOperand.vChar, instruction);
    }
    else if(RightOperand.type == "short"){
        moveFromTemp(&RightOperand.vShort, instruction);
    }
    else if(RightOperand.type == "unsigned_short"){
        moveFromTemp(&RightOperand.vUShort, instruction);
    }
    else if(RightOperand.type == "int"){
        moveFromTemp(&RightOperand.vInt, instruction);
    }
    else if(RightOperand.type == "unsigned_int"){
        moveFromTemp(&RightOperand.vUInt, instruction);
    }
    else if(RightOperand.type == "float"){
        moveFromTemp(&RightOperand.vFloat, instruction);
    }
    else if(RightOperand.type == "double"){
        moveFromTemp(&RightOperand.vDouble, instruction);
    }
    else if(instruction == "++" || instruction == "--"){
        moveFromTemp((short*)nullptr, instruction);
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << RightOperand.type << "\' is not a valid type.\n"; 
    }
}
template<typename LeftType, typename RightType>
LeftType BasePointersStruct::tryArithmetics(LeftType * LeftOperand, const RightType * RightOperand, string instruction){
    if(LeftOperand == nullptr){
        std::cout << "Error: In " << __FUNCTION__ << ": Left operand of \'" << type << "\' type does not exist.\n";
        return 0;
    }
    if(RightOperand == nullptr){
        std::cout << "Error: In " << __FUNCTION__ << ": Right operand of \'" << type << "\' type does not exist.\n";
        return 0;
    }
    if(instruction == "+"){
        return (*LeftOperand) + (*RightOperand);
    }
    else if(instruction == "+"){
        return (*LeftOperand) - (*RightOperand);
    }
    else if(instruction == "*"){
        return (*LeftOperand) * (*RightOperand);
    }
    else if(instruction == "/"){
        if(*RightOperand != 0){
            return (*LeftOperand) / (*RightOperand);
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot divide by zero.\n";
        }
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << instruction << "\' is not a valid instruction.\n";
    }
    return 0;
}
template<typename LeftType>
LeftType BasePointersStruct::callTryArithmeticsForEveryType(LeftType * LeftOperand, const BasePointersStruct & RightOperand, string instruction){
    if(RightOperand.type == "bool" && instruction != "ret_mul"){
        short right = *RightOperand.pBool;
        return tryArithmetics(LeftOperand, &right, instruction) > 0;
    }
    else if(RightOperand.type == "char"){
        return tryArithmetics(LeftOperand, RightOperand.pChar, instruction);
    }
    else if(RightOperand.type == "short"){
        return tryArithmetics(LeftOperand, RightOperand.pShort, instruction);
    }
    else if(RightOperand.type == "unsigned_short"){
        return tryArithmetics(LeftOperand, RightOperand.pUShort, instruction);
    }
    else if(RightOperand.type == "int"){
        return tryArithmetics(LeftOperand, RightOperand.pInt, instruction);
    }
    else if(RightOperand.type == "unsigned_int"){
        return tryArithmetics(LeftOperand, RightOperand.pUInt, instruction);
    }
    else if(RightOperand.type == "float"){
        return tryArithmetics(LeftOperand, RightOperand.pFloat, instruction);
    }
    else if(RightOperand.type == "double"){
        return tryArithmetics(LeftOperand, RightOperand.pDouble, instruction);
    }
    std::cout << "Error: In " << __FUNCTION__ << ": About the right operand: \'" << RightOperand.type << "\' is not a valid type.\n"; 
    return 0;
}
BaseVariableStruct BasePointersStruct::executeArithmetics(const BasePointersStruct &RightOperand, string instruction){
    BaseVariableStruct result;
    result.type = type;

    if(type == "string" || RightOperand.type == "string"){
        if(type == "string" && RightOperand.type == "string" && instruction == "+"){
            result.vString = *pString + *RightOperand.pString;
        }
        else{
            type = "";
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot execute arithmetic operations with string type values. The only exception is addition on two strings.\n";
        }
    }
    else if(type == "bool"){
        short left = *pBool;
        result.vBool = callTryArithmeticsForEveryType(&left, RightOperand, instruction);
    }
    else if(type == "char"){
        result.vChar = callTryArithmeticsForEveryType(pChar, RightOperand, instruction);
    }
    else if(type == "short"){
        result.vShort = callTryArithmeticsForEveryType(pShort, RightOperand, instruction);
    }
    else if(type == "unsigned_short"){
        result.vUShort = callTryArithmeticsForEveryType(pUShort, RightOperand, instruction);
    }
    else if(type == "int"){
        result.vInt = callTryArithmeticsForEveryType(pInt, RightOperand, instruction);
    }
    else if(type == "unsigned_int"){
        result.vUInt = callTryArithmeticsForEveryType(pUInt, RightOperand, instruction);
    }
    else if(type == "float"){
        result.vFloat = callTryArithmeticsForEveryType(pFloat, RightOperand, instruction);
    }
    else if(type == "double"){
        result.vDouble = callTryArithmeticsForEveryType(pDouble, RightOperand, instruction);
    }
    else{
        type = "";
        std::cout << "Error: In " << __FUNCTION__ << ": About the left operand: \'" << type << "\' is not a valid type.\n"; 
    }
    return result;
}
template<typename LeftType>
LeftType BasePointersStruct::callTryArithmetics(LeftType * LeftOperand, const BaseVariableStruct & RightOperand, string instruction){
    if(RightOperand.type == "bool"){
        short right = RightOperand.vBool;
        return tryArithmetics(LeftOperand, &right, instruction) > 0;
    }
    else if(RightOperand.type == "char"){
        return tryArithmetics(LeftOperand, &RightOperand.vChar, instruction);
    }
    else if(RightOperand.type == "short"){
        return tryArithmetics(LeftOperand, &RightOperand.vShort, instruction);
    }
    else if(RightOperand.type == "unsigned_short"){
        return tryArithmetics(LeftOperand, &RightOperand.vUShort, instruction);
    }
    else if(RightOperand.type == "int"){
        return tryArithmetics(LeftOperand, &RightOperand.vInt, instruction);
    }
    else if(RightOperand.type == "unsigned_int"){
        return tryArithmetics(LeftOperand, &RightOperand.vUInt, instruction);
    }
    else if(RightOperand.type == "float"){
        return tryArithmetics(LeftOperand, &RightOperand.vFloat, instruction);
    }
    else if(RightOperand.type == "double"){
        return tryArithmetics(LeftOperand, &RightOperand.vDouble, instruction);
    }
    return 0;
}
BaseVariableStruct BasePointersStruct::executeArithmetics(const BaseVariableStruct & RightOperand, string instruction){
    BaseVariableStruct result;
    result.type = type;
    
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string" && RightOperand.type == "string" && instruction == "+"){
            result.vString = *pString + RightOperand.vString;
        }
        else{
            type = "";
            std::cout << "Error: In " << __FUNCTION__ << ": You cannot execute arithmetic operations with string type values. The only exception is addition on two strings.\n";
        }
    }
    else if(type == "bool"){
        short left = *pBool;
        result.vBool = callTryArithmetics(&left, RightOperand, instruction) > 0;
    }
    else if(type == "char"){
        result.vChar = callTryArithmetics(pChar, RightOperand, instruction);
    }
    else if(type == "short"){
        result.vShort = callTryArithmetics(pShort, RightOperand, instruction);
    }
    else if(type == "unsigned_short"){
        result.vUShort = callTryArithmetics(pUShort, RightOperand, instruction);
    }
    else if(type == "int"){
        result.vInt = callTryArithmetics(pInt, RightOperand, instruction);
    }
    else if(type == "unsigned_int"){
        result.vUInt = callTryArithmetics(pUInt, RightOperand, instruction);
    }
    else if(type == "float"){
        result.vFloat = callTryArithmetics(pFloat, RightOperand, instruction);
    }
    else if(type == "double"){
        result.vDouble = callTryArithmetics(pDouble, RightOperand, instruction);
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": left operand's \'" << type << "\' type does not exist.\n";
        result.type = "";
    }

    return result;
}
void BasePointersStruct::setPointer(bool *pointer){
    pBool = pointer;
    type = "bool";
}
void BasePointersStruct::setPointer(char * pointer){
    pChar = pointer;
    type = "char";
}
void BasePointersStruct::setPointer(short * pointer){
    pShort = pointer;
    type = "short";
}
void BasePointersStruct::setPointer(unsigned short * pointer){
    pUShort = pointer;
    type = "unsigned_short";
}
void BasePointersStruct::setPointer(int * pointer){
    pInt = pointer;
    type = "int";
}
void BasePointersStruct::setPointer(unsigned int * pointer){
    pUInt = pointer;
    type = "unsigned_int";
}
void BasePointersStruct::setPointer(float * pointer){
    pFloat = pointer;
    type = "float";
}
void BasePointersStruct::setPointer(double * pointer){
    pDouble = pointer;
    type = "double";
}
void BasePointersStruct::setPointer(string * pointer){
    pString = pointer;
    type = "string";
}

bool BasePointersStruct::areEqual(BasePointersStruct *OtherVariable){
    if(type != OtherVariable->type){
        std::cout << "Error: In " << __FUNCTION__ << ":Pointers of built-in types have different types.\n";
        return false;
    }
    if(type == "bool"){
        return *pBool == *OtherVariable->pBool;
    }
    else if(type == "char"){
        return *pChar == *OtherVariable->pChar;
    }
    else if(type == "short"){
        return *pShort == *OtherVariable->pShort;
    }
    else if(type == "unsigned_short"){
        return *pUShort == *OtherVariable->pUShort;
    }
    else if(type == "int"){
        return *pInt == *OtherVariable->pInt;
    }
    else if(type == "unsigned_int"){
        return *pUInt == *OtherVariable->pUInt;
    }
    else if(type == "float"){
        return *pFloat == *OtherVariable->pFloat;
    }
    else if(type == "double"){
        return *pDouble == *OtherVariable->pDouble;
    }
    else if(type == "string"){
        return *pString == *OtherVariable->pString;
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type.\n"; 
    }
    return false;
}
bool BasePointersStruct::areEqual(BaseVariableStruct *OtherVariable){
    if(type != OtherVariable->type){
        std::cout << "Error: In " << __FUNCTION__ << ": Pointers of built-in types have different types.\n";
        return false;
    }
    if(type == "bool"){
        return *pBool == OtherVariable->vBool;
    }
    else if(type == "char"){
        return *pChar == OtherVariable->vChar;
    }
    else if(type == "short"){
        return *pShort == OtherVariable->vShort;
    }
    else if(type == "unsigned_short"){
        return *pUShort == OtherVariable->vUShort;
    }
    else if(type == "int"){
        return *pInt == OtherVariable->vInt;
    }
    else if(type == "unsigned_int"){
        return *pUInt == OtherVariable->vUInt;
    }
    else if(type == "float"){
        return *pFloat == OtherVariable->vFloat;
    }
    else if(type == "double"){
        return *pDouble == OtherVariable->vDouble;
    }
    else if(type == "string"){
        return *pString == OtherVariable->vString;
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type.\n"; 
    }
    return false;
}

bool BasePointersStruct::getBool() const{
    if(type == "int"){
        return *pInt;
    }
    else if(type == "unsigned_int"){
        return *pUInt;
    }
    if(type == "bool"){
        return *pBool;
    }
    else if(type == "char"){
        return *pChar;
    }
    else if(type == "short"){
        return *pShort;
    }
    else if(type == "unsigned_short"){
        return *pUShort;
    }
    else if(type == "float"){
        return *pFloat;
    }
    else if(type == "double"){
        return *pDouble;
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": You can't access bool variable.\n";
        return 0;
    }
}
bool BasePointersStruct::isInteger() const{
    if(isStringInGroup(type, 4, "int", "unsigned_int", "short", "unsigned_short")){
        return true;
    }
    return false;
}
bool BasePointersStruct::isNumeric() const{
    return type != "char" && type != "string";
}
double BasePointersStruct::getDouble() const{
    if(type == "double"){
        return *pDouble;
    }
    if(type == "bool"){
        std::cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pBool;
    }
    else if(type == "char"){
        std::cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pChar;
    }
    else if(type == "short"){
        std::cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pShort;
    }
    else if(type == "unsigned_short"){
        std::cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pUShort;
    }
    else if(type == "int"){
        std::cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pInt;
    }
    else if(type == "unsigned_int"){
        std::cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pUInt;
    }
    else if(type == "float"){
        return *pFloat;
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": You can't access numeric variable.\n";
        return 0.0;
    }
}
int BasePointersStruct::getInt() const{
    if(type == "int"){
        return *pInt;
    }
    else if(type == "unsigned_int"){
        //std::cout << "Warning: In " << __FUNCTION__ << ": Conversion from unsigned int to int.\n";
        return *pUInt;
    }
    if(type == "bool"){
        std::cout << "Warning: In " << __FUNCTION__ << ": Conversion from bool to int.\n";
        return *pBool;
    }
    else if(type == "char"){
        std::cout << "Warning: In " << __FUNCTION__ << ": Conversion from char to int.\n";
        return *pChar;
    }
    else if(type == "short"){
        std::cout << "Warning: In " << __FUNCTION__ << ": Conversion from short to int.\n";
        return *pShort;
    }
    else if(type == "unsigned_short"){
        std::cout << "Warning: In " << __FUNCTION__ << ": Conversion from unsigned short to int.\n";
        return *pUShort;
    }
    else if(type == "float"){
        //std::cout << "Warning: In " << __FUNCTION__ << ": Floating point ignored.\n";
        return *pFloat;
    }
    else if(type == "double"){
       // std::cout << "Warning: In " << __FUNCTION__ << ": Floating point ignored.\n";
        return *pDouble;
    }
    else{
        std::cout << "Error: In " << __FUNCTION__ << ": You can't access numeric variable.\n";
        return 0;
    }
}
string BasePointersStruct::getString() const{
    if(type == "string"){
        return *pString;
    }
    else if(type == "bool"){
        if(*pBool){
            return "true";
        }
        return "false";
    }
    else if(type == "char"){
        return string(1, *pChar);
    }
    else if(type == "short"){
        return shortToStr(*pShort);
    }
    else if(type == "unsigned_short"){
        return intToStr(*pUShort);
    }
    else if(type == "int"){
        return intToStr(*pInt);
    }
    else if(type == "unsigned_int"){
        return uIntToStr(*pUInt);
    }
    else if(type == "float"){
        return doubleToStr(*pFloat);
    }
    else if(type == "double"){
        return doubleToStr(*pDouble);
    }
    
    std::cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
    return "[invalid type]";
}