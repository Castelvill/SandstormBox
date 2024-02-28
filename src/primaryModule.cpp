#include "primaryModule.h"

EngineInstr transInstr(string instruction){
    if(instruction == "start"){
        return EngineInstr::start;
    }
    if(instruction == "end"){
        return EngineInstr::end_i;
    }
    if(instruction == "post"){
        return EngineInstr::post;
    }
    if(instruction == "triggers"){
        return EngineInstr::triggers;
    }
    if(instruction == "run"){
        return EngineInstr::run;
    }
    if(instruction == "if"){
        return EngineInstr::if_i;
    }
    if(instruction == "else"){
        return EngineInstr::else_i;
    }
    if(instruction == "continue"){
        return EngineInstr::continue_i;
    }
    if(instruction == "break"){
        return EngineInstr::break_i;
    }
    if(instruction == "return"){
        return EngineInstr::return_i;
    }
    if(instruction == "reboot"){
        return EngineInstr::reboot;
    }
    if(instruction == "power_off"){
        return EngineInstr::power_off;
    }
    if(instruction == "delete_this_event"){
        return EngineInstr::delete_this_event;
    }
    if(instruction == "reset_keyboard"){
        return EngineInstr::reset_keyboard;
    }
    if(instruction == "dump_context_stack"){
        return EngineInstr::dump_context_stack;
    }
    if(instruction == "first"){
        return EngineInstr::first;
    }
    if(instruction == "last"){
        return EngineInstr::last;
    }
    if(instruction == "all"){
        return EngineInstr::all;
    }
    if(instruction == "random"){
        return EngineInstr::random_i;
    }
    if(instruction == "index"){
        return EngineInstr::index_i;
    }
    if(instruction == "sum"){
        return EngineInstr::sum;
    }
    if(instruction == "intersection"){
        return EngineInstr::intersection;
    }
    if(instruction == "difference"){
        return EngineInstr::diff;
    }
    if(instruction == "+"){
        return EngineInstr::add;
    }
    if(instruction == "-"){
        return EngineInstr::sub;
    }
    if(instruction == "*"){
        return EngineInstr::mul;
    }
    if(instruction == "/"){
        return EngineInstr::div_i;
    }
    if(instruction == "%"){
        return EngineInstr::mod;
    }
    if(instruction == "**"){
        return EngineInstr::pow_i;
    }
    if(instruction == "="){
        return EngineInstr::move;
    }
    if(instruction == "+="){
        return EngineInstr::add_move;
    }
    if(instruction == "-="){
        return EngineInstr::sub_move;
    }
    if(instruction == "*="){
        return EngineInstr::mul_move;
    }
    if(instruction == "/="){
        return EngineInstr::div_move;
    }
    if(instruction == "=="){
        return EngineInstr::equal;
    }
    if(instruction == "!="){
        return EngineInstr::not_equal;
    }
    if(instruction == ">"){
        return EngineInstr::more;
    }
    if(instruction == "<"){
        return EngineInstr::less;
    }
    if(instruction == ">="){
        return EngineInstr::more_equal;
    }
    if(instruction == "<="){
        return EngineInstr::less_equal;
    }
    if(instruction == "!"){
        return EngineInstr::not_i;
    }
    if(instruction == "&&"){
        return EngineInstr::and_i;
    }
    if(instruction == "||"){
        return EngineInstr::or_i;
    }
    if(instruction == "igT"){
        return EngineInstr::igT;
    }
    if(instruction == "igF"){
        return EngineInstr::igF;
    }
    if(instruction == "in"){
        return EngineInstr::in;
    }
    if(instruction == "find_by_id_2"){
        return EngineInstr::find_by_id_2;
    }
    if(instruction == "++"){
        return EngineInstr::inc;
    }
    if(instruction == "--"){
        return EngineInstr::dec;
    }
    if(instruction == "delete"){
        return EngineInstr::del;
    }
    if(instruction == "demolish"){
        return EngineInstr::demolish;
    }
    if(instruction == "value"){
        return EngineInstr::value;
    }
    if(instruction == "bool"){
        return EngineInstr::bool_i;
    }
    if(instruction == "int"){
        return EngineInstr::int_i;
    }
    if(instruction == "double"){
        return EngineInstr::double_i;
    }
    if(instruction == "string"){
        return EngineInstr::string_i;
    }
    if(instruction == "random_int"){
        return EngineInstr::rand_int;
    }
    if(instruction == "find_by_id"){
        return EngineInstr::find_by_id;
    }
    if(instruction == "let"){
        return EngineInstr::let;
    }
    if(instruction == "clone"){
        return EngineInstr::clone_i;
    }
    if(instruction == "new"){
        return EngineInstr::new_i;
    }
    if(instruction == "bind"){
        return EngineInstr::bind_i;
    }
    if(instruction == "build"){
        return EngineInstr::build;
    }
    if(instruction == "load_build"){
        return EngineInstr::load_build;
    }
    if(instruction == "build_subset"){
        return EngineInstr::build_subset;
    }
    if(instruction == "inject_code"){
        return EngineInstr::inject_code;
    }
    if(instruction == "inject_instr"){
        return EngineInstr::inject_instr;
    }
    if(instruction == "fun"){
        return EngineInstr::fun;
    }
    if(instruction == "env"){
        return EngineInstr::env;
    }
    if(instruction == "edit_proc"){
        return EngineInstr::edit_proc;
    }
    if(instruction == "load_bitmap"){
        return EngineInstr::load_bitmap;
    }
    if(instruction == "mkdir"){
        return EngineInstr::mkdir_i;
    }
    if(instruction == "rm"){
        return EngineInstr::rm;
    }
    if(instruction == "rmll"){
        return EngineInstr::rmll;
    }
    if(instruction == "rename"){
        return EngineInstr::rename_i;
    }
    if(instruction == "print"){
        return EngineInstr::print;
    }
    if(instruction == "load_text"){
        return EngineInstr::load_text;
    }
    if(instruction == "save_text"){
        return EngineInstr::save_text;
    }
    if(instruction == "ls"){
        return EngineInstr::ls;
    }
    if(instruction == "lse"){
        return EngineInstr::lse;
    }
    if(instruction == "new_proc"){
        return EngineInstr::new_proc;
    }
    if(instruction == "var"){
        return EngineInstr::var;
    }
    if(instruction == "vec"){
        return EngineInstr::vec;
    }
    if(instruction == "tokenize"){
        return EngineInstr::tokenize;
    }
    if(instruction == "tree"){
        return EngineInstr::tree;
    }
    if(instruction == "len"){
        return EngineInstr::len;
    }
    if(instruction == "size"){
        return EngineInstr::size;
    }
    if(instruction == "substr"){
        return EngineInstr::substr;
    }
    if(instruction == "load_font"){
        return EngineInstr::load_font;
    }
    if(instruction == "restart_drag"){
        return EngineInstr::restart_drag;
    }
    if(instruction == "cd"){
        return EngineInstr::cd;
    }
    if(instruction == "pwd"){
        return EngineInstr::pwd;
    }
    if(instruction == "similar"){
        return EngineInstr::similar;
    }
    cout << "Error: In " << __FUNCTION__ << ": instruction '" << instruction << "' does not exist.\n"; 
    return EngineInstr::null;
}
string transInstrToStr(EngineInstr instruction){
    switch(instruction){
        case null:
            return "null";
        case start:
            return "start";
        case end_i:
            return "end";
        case post:
            return "post";
        case triggers:
            return "triggers";
        case run:
            return "run";
        case if_i:
            return "if";
        case else_i:
            return "else";
        case continue_i:
            return "continue";
        case break_i:
            return "break";
        case return_i:
            return "return";
        case reboot:
            return "reboot";
        case power_off:
            return "power_off";
        case delete_this_event:
            return "delete_this_event";
        case reset_keyboard:
            return "reset_keyboard";
        case dump_context_stack:
            return "dump_context_stack";
        case first:
            return "first";
        case last:
            return "last";
        case all:
            return "all";
        case random_i:
            return "random";
        case index_i:
            return "index";
        case sum:
            return "sum";
        case intersection:
            return "intersection";
        case diff:
            return "difference";
        case add:
            return "+";
        case sub:
            return "-";
        case mul:
            return "*";
        case div_i:
            return "/";
        case mod:
            return "%";
        case pow_i:
            return "**";
        case move:
            return "=";
        case add_move:
            return "+=";
        case sub_move:
            return "-=";
        case mul_move:
            return "*=";
        case div_move:
            return "/=";
        case equal:
            return "==";
        case not_equal:
            return "!=";
        case more:
            return ">";
        case less:
            return "<";
        case more_equal:
            return ">=";
        case less_equal:
            return "<=";
        case not_i:
            return "!";
        case and_i:
            return "&&";
        case or_i:
            return "||";
        case igT:
            return "igT";
        case igF:
            return "igF";
        case in:
            return "in";
        case find_by_id_2:
            return "find_by_id_2";
        case inc:
            return "++";
        case dec:
            return "--";
        case del:
            return "delete";
        case demolish:
            return "demolish";
        case value:
            return "value";
        case bool_i:
            return "bool";
        case int_i:
            return "int";
        case double_i:
            return "double";
        case string_i:
            return "string";
        case rand_int:
            return "random_int";
        case find_by_id:
            return "find_by_id";
        case let:
            return "let";
        case clone_i:
            return "clone";
        case new_i:
            return "new";
        case bind_i:
            return "bind";
        case build:
            return "build";
        case load_build:
            return "load_build";
        case build_subset:
            return "build_subset";
        case inject_code:
            return "inject_code";
        case inject_instr:
            return "inject_instr";
        case fun:
            return "fun";
        case env:
            return "env";
        case edit_proc:
            return "edit_proc";
        case load_bitmap:
            return "load_bitmap";
        case mkdir_i:
            return "mkdir";
        case rm:
            return "rm";
        case rmll:
            return "rmll";
        case rename_i:
            return "rename";
        case print:
            return "print";
        case load_text:
            return "load_text";
        case save_text:
            return "save_text";
        case ls:
            return "ls";
        case lse:
            return "lse";
        case new_proc:
            return "new_proc";
        case var:
            return "var";
        case vec:
            return "vec";
        case tokenize:
            return "tokenize";
        case tree:
            return "tree";
        case len:
            return "len";
        case size:
            return "size";
        case substr:
            return "substr";
        case load_font:
            return "load_font";
        case restart_drag:
            return "restart_drag";
        case cd:
            return "cd";
        case pwd:
            return "pwd";
        case similar:
            return "similar";
    }
    cout << "Error: In " << __FUNCTION__ << ": instruction '" << instruction << "' does not exist.\n"; 
    return "null";
}

void PrimaryModule::primaryConstructor(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    if(listOfIDs != nullptr){
        setAllIDs(newID, *listOfIDs, newLayerID, newObjectID, true);
    }
    else{
        ID = newID;
    }
    
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
void PrimaryModule::primaryConstructor(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
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
        cout << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID << "\' cannot be changed.\n";
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

void PrimaryModule::setAllIDs(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
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
void PrimaryModule::addScale(double x, double y){
    scale.translate(x, y);
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
void PrimaryModule::control(string attribute, bool value, unsigned paramCount){
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
        cout << "Error: In " << __FUNCTION__ << ": function " << attribute << "<" << paramCount << "> does not exist.\n";
    }
}
void PrimaryModule::setIsAttachedToCamera(bool newIsAttachedToCamera){
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
bool PrimaryModule::getIsDeleted() const{
    return deleted;
}
bool PrimaryModule::getIsScaledFromCenter(){
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
            cout << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << ID << "\' address was denied.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": No valid attribute provided.\n";
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
void BasePointersStruct::executeMoveTypeInstruction(LeftType * LeftOperand, const RightType * RightOperand, EngineInstr instruction){
    if(instruction == EngineInstr::inc){
        (*LeftOperand)++;
        return;
    }
    else if(instruction == EngineInstr::dec){
        (*LeftOperand)--;
        return;
    }
    if(LeftOperand == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Left operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(RightOperand == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Right operand of \'" << type << "\' type does not exist.\n";
        return;
    }
    if(instruction == EngineInstr::move || instruction == EngineInstr::clone_i){
        *LeftOperand = *RightOperand;
    }
    else if(instruction == EngineInstr::add_move){
        *LeftOperand += *RightOperand;
    }
    else if(instruction == EngineInstr::sub_move){
        *LeftOperand -= *RightOperand;
    }
    else if(instruction == EngineInstr::mul_move){
        *LeftOperand *= *RightOperand;
    }
    else if(instruction == EngineInstr::div_move){
        if(*RightOperand != 0){
            *LeftOperand /= *RightOperand;
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": You cannot divide by zero.\n";
        }
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": \'" << transInstrToStr(instruction) << "\' is not a valid instruction.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
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
    
    cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
    return "[invalid type]";
}
void BasePointersStruct::tryToSetValue(const BaseVariableStruct & RightOperand){
    if(readOnly){
        cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string"){
            *pString = RightOperand.getString();
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": You cannot assign string type value to a non-string type variable.\n";
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
void BasePointersStruct::moveFromTemp(const RightType * RightOperand, EngineInstr instruction){
    if(readOnly){
        cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type.\n"; 
    }
}
void BasePointersStruct::move(const BasePointersStruct &RightOperand, EngineInstr instruction){
    if(readOnly){
        cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string"){
            if(instruction == EngineInstr::move){
                *pString = RightOperand.getString();
            }
            else if(instruction == EngineInstr::add_move){
                *pString += RightOperand.getString();
            }
            else{
                cout << "Error: In " << __FUNCTION__ << ": You cannot execute \'" << transInstrToStr(instruction) << "\' instruction on string type values.\n";
            }
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": You cannot execute any instructions if only the right operand is of a string type.\n";
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
    else if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveFromTemp((short*)nullptr, instruction);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": \'" << RightOperand.type << "\' is not a valid type.\n"; 
    }
}
void BasePointersStruct::move(const BaseVariableStruct & RightOperand, EngineInstr instruction){
    if(readOnly){
        cout << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string"){
            if(instruction == EngineInstr::move){
                *pString = RightOperand.getString();
            }
            else if(instruction == EngineInstr::add_move){
                *pString += RightOperand.getString();
            }
            else{
                cout << "Error: In " << __FUNCTION__ << ": You cannot execute \'" << transInstrToStr(instruction) << "\' instruction on string type values.\n";
            }
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": You cannot execute any instructions if only the right operand is of a string type.\n";
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
    else if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveFromTemp((short*)nullptr, instruction);
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": \'" << RightOperand.type << "\' is not a valid type.\n"; 
    }
}
template<typename LeftType, typename RightType>
LeftType BasePointersStruct::tryArithmetics(LeftType * LeftOperand, const RightType * RightOperand, EngineInstr instruction){
    if(LeftOperand == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Left operand of \'" << type << "\' type does not exist.\n";
        return 0;
    }
    if(RightOperand == nullptr){
        cout << "Error: In " << __FUNCTION__ << ": Right operand of \'" << type << "\' type does not exist.\n";
        return 0;
    }
    if(instruction == EngineInstr::add){
        return (*LeftOperand) + (*RightOperand);
    }
    else if(instruction == EngineInstr::sub){
        return (*LeftOperand) - (*RightOperand);
    }
    else if(instruction == EngineInstr::mul){
        return (*LeftOperand) * (*RightOperand);
    }
    else if(instruction == EngineInstr::div_i){
        if(*RightOperand != 0){
            return (*LeftOperand) / (*RightOperand);
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": You cannot divide by zero.\n";
        }
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": \'" << transInstrToStr(instruction) << "\' is not a valid instruction.\n";
    }
    return 0;
}
template<typename LeftType>
LeftType BasePointersStruct::callTryArithmeticsForEveryType(LeftType * LeftOperand, const BasePointersStruct & RightOperand, EngineInstr instruction){
    if(RightOperand.type == "bool" /*&& instruction != "ret_mul"*/){
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
    cout << "Error: In " << __FUNCTION__ << ": About the right operand: \'" << RightOperand.type << "\' is not a valid type.\n"; 
    return 0;
}
BaseVariableStruct BasePointersStruct::executeArithmetics(const BasePointersStruct &RightOperand, EngineInstr instruction){
    BaseVariableStruct result;
    result.type = type;

    if(type == "string" || RightOperand.type == "string"){
        if(type == "string" && RightOperand.type == "string" && instruction == EngineInstr::add){
            result.vString = *pString + *RightOperand.pString;
        }
        else{
            type = "";
            cout << "Error: In " << __FUNCTION__ << ": You cannot execute arithmetic operations with string type values. The only exception is addition on two strings.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": About the left operand: \'" << type << "\' is not a valid type.\n"; 
    }
    return result;
}
template<typename LeftType>
LeftType BasePointersStruct::callTryArithmetics(LeftType * LeftOperand, const BaseVariableStruct & RightOperand, EngineInstr instruction){
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
BaseVariableStruct BasePointersStruct::executeArithmetics(const BaseVariableStruct & RightOperand, EngineInstr instruction){
    BaseVariableStruct result;
    result.type = type;
    
    if(type == "string" || RightOperand.type == "string"){
        if(type == "string" && RightOperand.type == "string" && instruction == EngineInstr::add){
            result.vString = *pString + RightOperand.vString;
        }
        else{
            type = "";
            cout << "Error: In " << __FUNCTION__ << ": You cannot execute arithmetic operations with string type values. The only exception is addition on two strings.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": left operand's \'" << type << "\' type does not exist.\n";
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
        cout << "Error: In " << __FUNCTION__ << ":Pointers of built-in types have different types.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type.\n"; 
    }
    return false;
}
bool BasePointersStruct::areEqual(BaseVariableStruct *OtherVariable){
    if(type != OtherVariable->type){
        cout << "Error: In " << __FUNCTION__ << ": Pointers of built-in types have different types.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type.\n"; 
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
        cout << "Error: In " << __FUNCTION__ << ": You can't access bool variable.\n";
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
        cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pBool;
    }
    else if(type == "char"){
        cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pChar;
    }
    else if(type == "short"){
        cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pShort;
    }
    else if(type == "unsigned_short"){
        cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pUShort;
    }
    else if(type == "int"){
        cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pInt;
    }
    else if(type == "unsigned_int"){
        cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
        return *pUInt;
    }
    else if(type == "float"){
        return *pFloat;
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": You can't access numeric variable.\n";
        return 0.0;
    }
}
int BasePointersStruct::getInt() const{
    if(type == "int"){
        return *pInt;
    }
    else if(type == "unsigned_int"){
        //cout << "Warning: In " << __FUNCTION__ << ": Conversion from unsigned int to int.\n";
        return *pUInt;
    }
    if(type == "bool"){
        cout << "Warning: In " << __FUNCTION__ << ": Conversion from bool to int.\n";
        return *pBool;
    }
    else if(type == "char"){
        cout << "Warning: In " << __FUNCTION__ << ": Conversion from char to int.\n";
        return *pChar;
    }
    else if(type == "short"){
        cout << "Warning: In " << __FUNCTION__ << ": Conversion from short to int.\n";
        return *pShort;
    }
    else if(type == "unsigned_short"){
        cout << "Warning: In " << __FUNCTION__ << ": Conversion from unsigned short to int.\n";
        return *pUShort;
    }
    else if(type == "float"){
        //cout << "Warning: In " << __FUNCTION__ << ": Floating point ignored.\n";
        return *pFloat;
    }
    else if(type == "double"){
       // cout << "Warning: In " << __FUNCTION__ << ": Floating point ignored.\n";
        return *pDouble;
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": You can't access numeric variable.\n";
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
    
    cout << "Error: In " << __FUNCTION__ << ": \'" << type << "\' is not a valid type for this operation.\n";
    return "[invalid type]";
}