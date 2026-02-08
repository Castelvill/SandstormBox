#include "primaryModule.h"

EngineInstr strToInstr(const string & instruction, bool printError){
    if(instruction == "@"){
        return EngineInstr::annotation_i;
    }
    if(instruction == "start"){
        return EngineInstr::start;
    }
    if(instruction == "end"){
        return EngineInstr::end_i;
    }
    if(instruction == "run"){
        return EngineInstr::run;
    }
    if(instruction == "if"){
        return EngineInstr::if_i;
    }
    if(instruction == "else_if"){
        return EngineInstr::else_if;
    }
    if(instruction == "else"){
        return EngineInstr::else_i;
    }
    if(instruction == "end_if"){
        return EngineInstr::end_if;
    }
    if(instruction == "while"){
        return EngineInstr::while_i;
    }
    if(instruction == "end_while"){
        return EngineInstr::end_while;
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
    if(instruction == "exit"){
        return EngineInstr::exit_i;
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
    if(instruction == "dump_memory"){
        return EngineInstr::dump_memory;
    }
    if(instruction == "dump_local_memory"){
        return EngineInstr::dump_local_memory;
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
    if(instruction == "index_vec"){
        return EngineInstr::index_vec;
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
    if(instruction == "assign"){
        return EngineInstr::assign;
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
    if(instruction == "next"){
        return EngineInstr::next;
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
    if(instruction == "access"){
        return EngineInstr::access_i;
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
    if(instruction == "bool_vec"){
        return EngineInstr::bool_vec_i;
    }
    if(instruction == "int_vec"){
        return EngineInstr::int_vec_i;
    }
    if(instruction == "double_vec"){
        return EngineInstr::double_vec_i;
    }
    if(instruction == "string_vec"){
        return EngineInstr::string_vec_i;
    }
    if(instruction == "random_int"){
        return EngineInstr::rand_int;
    }
    if(instruction == "random_double"){
        return EngineInstr::rand_double;
    }
    if(instruction == "find_by_id"){
        return EngineInstr::find_by_id;
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
    if(instruction == "rbind"){
        return EngineInstr::rbind_i;
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
    if(instruction == "mv"){
        return EngineInstr::mv_i;
    }
    if(instruction == "print_v"){
        return EngineInstr::print_v_i;
    }
    if(instruction == "print_d"){
        return EngineInstr::print_d_i;
    }
    if(instruction == "print"){
        return EngineInstr::print_i;
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
        return EngineInstr::size_i;
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
    if(instruction == "count"){
        return EngineInstr::count;
    }
    if(instruction == "create_display"){
        return EngineInstr::create_display;
    }
    if(instruction == "console_input"){
        return EngineInstr::console_input;
    }
    if(instruction == "start_timer"){
        return EngineInstr::start_timer;
    }
    if(instruction == "stop_timer"){
        return EngineInstr::stop_timer;
    }
    if(instruction == "compiler_breakpoint"){
        return EngineInstr::breakpoint;
    }
    if(instruction == "breakpoint"){
        return EngineInstr::breakpoint;
    }
    if(instruction == "assert"){
        return EngineInstr::assert;
    }
    if(instruction == "type"){
        return EngineInstr::type;
    }
    if(instruction == "load"){
        return EngineInstr::load_i;
    }
    if(instruction == "import"){
        return EngineInstr::import;
    }
    if(instruction == "resume_assembling"){
        return EngineInstr::resume_assembling;
    }
    if(instruction == "stop_assembling"){
        return EngineInstr::stop_assembling;
    }
    if(instruction == "start_printing_tokens"){
        return EngineInstr::start_printing_tokens;
    }
    if(instruction == "stop_printing_tokens"){
        return EngineInstr::stop_printing_tokens;
    }
    if(instruction == "Val"){
        return EngineInstr::Val;
    }
    if(instruction == "ValVec"){
        return EngineInstr::ValVec;
    }
    if(instruction == "Pointer"){
        return EngineInstr::Pointer;
    }
    if(instruction == "PointerVec"){
        return EngineInstr::PointerVec;
    }
    if(instruction == "Camera"){
        return EngineInstr::Camera;
    }
    if(instruction == "CameraVec"){
        return EngineInstr::CameraVec;
    }
    if(instruction == "Layer"){
        return EngineInstr::Layer;
    }
    if(instruction == "LayerVec"){
        return EngineInstr::LayerVec;
    }
    if(instruction == "Object"){
        return EngineInstr::Object;
    }
    if(instruction == "ObjectVec"){
        return EngineInstr::ObjectVec;
    }
    if(instruction == "Var"){
        return EngineInstr::Var;
    }
    if(instruction == "VarVec"){
        return EngineInstr::VarVec;
    }
    if(instruction == "Vec"){
        return EngineInstr::Vec;
    }
    if(instruction == "VecVec"){
        return EngineInstr::VecVec;
    }
    if(instruction == "SText"){
        return EngineInstr::SText;
    }
    if(instruction == "STextVec"){
        return EngineInstr::STextVec;
    }
    if(instruction == "SEditText"){
        return EngineInstr::SEditText;
    }
    if(instruction == "SEditTextVec"){
        return EngineInstr::SEditTextVec;
    }
    if(instruction == "Image"){
        return EngineInstr::Image;
    }
    if(instruction == "ImageVec"){
        return EngineInstr::ImageVec;
    }
    if(instruction == "Movement"){
        return EngineInstr::Movement;
    }
    if(instruction == "MovementVec"){
        return EngineInstr::MovementVec;
    }
    if(instruction == "Collision"){
        return EngineInstr::Collision;
    }
    if(instruction == "CollisionVec"){
        return EngineInstr::CollisionVec;
    }
    if(instruction == "Particles"){
        return EngineInstr::Particles;
    }
    if(instruction == "Event"){
        return EngineInstr::Event;
    }
    if(instruction == "EventVec"){
        return EngineInstr::EventVec;
    }
    if(instruction == "Scrollbar"){
        return EngineInstr::Scrollbar;
    }
    if(instruction == "ScrollbarVec"){
        return EngineInstr::ScrollbarVec;
    }
    if(instruction == "Primitive"){
        return EngineInstr::Primitive;
    }
    if(instruction == "PrimitiveVec"){
        return EngineInstr::PrimitiveVec;
    }
    if(instruction == "any"){
        return EngineInstr::any;
    }
    printError && cerr << "Error: In " << __FUNCTION__ << ": instruction '" << instruction << "' does not exist.\n"; 
    return EngineInstr::null;
}
string instrToStr(const EngineInstr & instruction){
    switch(instruction){
        case null:
            return "null";
        case annotation_i:
            return "@";
        case start:
            return "start";
        case end_i:
            return "end";
        case run:
            return "run";
        case if_i:
            return "if";
        case else_if:
            return "else_if";
        case else_i:
            return "else";
        case end_if:
            return "end_if";
        case while_i:
            return "while";
        case end_while:
            return "end_while";
        case continue_i:
            return "continue";
        case break_i:
            return "break";
        case return_i:
            return "return";
        case reboot:
            return "reboot";
        case exit_i:
            return "exit";
        case delete_this_event:
            return "delete_this_event";
        case reset_keyboard:
            return "reset_keyboard";
        case dump_context_stack:
            return "dump_context_stack";
        case dump_memory:
            return "dump_memory";
        case dump_local_memory:
            return "dump_local_memory";
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
        case index_vec:
            return "index_vec";
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
        case assign:
            return "assign";
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
        case next:
            return "next";
        case dec:
            return "--";
        case del:
            return "delete";
        case demolish:
            return "demolish";
        case access_i:
            return "access";
        case bool_i:
            return "bool";
        case int_i:
            return "int";
        case double_i:
            return "double";
        case string_i:
            return "string";
        case bool_vec_i:
            return "bool_vec";
        case int_vec_i:
            return "int_vec";
        case double_vec_i:
            return "double_vec";
        case string_vec_i:
            return "string_vec";
        case rand_int:
            return "random_int";
        case rand_double:
            return "random_double";
        case find_by_id:
            return "find_by_id";
        case clone_i:
            return "clone";
        case new_i:
            return "new";
        case bind_i:
            return "bind";
        case rbind_i:
            return "rbind";
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
        case mv_i:
            return "mv";
        case print_v_i:
            return "print_v";
        case print_d_i:
            return "print_d";
        case print_i:
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
        case size_i:
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
        case count:
            return "count";
        case create_display:
            return "create_display";
        case console_input:
            return "console_input";
        case start_timer:
            return "start_timer";
        case stop_timer:
            return "stop_timer";
        case compiler_breakpoint:
            return "compiler_breakpoint";
        case breakpoint:
            return "breakpoint";
        case assert:
            return "assert";
        case type:
            return "type";
        case load_i:
            return "load";
        case import:
            return "import";
        case resume_assembling:
            return "resume_assembling";
        case stop_assembling:
            return "stop_assembling";
        case start_printing_tokens:
            return "start_printing_tokens";
        case stop_printing_tokens:
            return "stop_printing_tokens";
        case start_expr:
            return "(";
        case Val:
            return "Val";
        case ValVec:
            return "ValVec";
        case Pointer:
            return "Pointer";
        case PointerVec:
            return "PointerVec";
        case Camera:
            return "Camera";
        case CameraVec:
            return "CameraVec";
        case Layer:
            return "Layer";
        case LayerVec:
            return "LayerVec";
        case Object:
            return "Object";
        case ObjectVec:
            return "ObjectVec";
        case Var:
            return "Var";
        case VarVec:
            return "VarVec";
        case Vec:
            return "Vec";
        case VecVec:
            return "VecVec";
        case SText:
            return "SText";
        case STextVec:
            return "STextVec";
        case SEditText:
            return "SEditText";
        case SEditTextVec:
            return "SEditTextVec";
        case Image:
            return "Image";
        case ImageVec:
            return "ImageVec";
        case Movement:
            return "Movement";
        case MovementVec:
            return "MovementVec";
        case Collision:
            return "Collision";
        case CollisionVec:
            return "CollisionVec";
        case Particles:
            return "Particles";
        case Event:
            return "Event";
        case EventVec:
            return "EventVec";
        case Scrollbar:
            return "Scrollbar";
        case ScrollbarVec:
            return "ScrollbarVec";
        case Primitive:
            return "Primitive";
        case PrimitiveVec:
            return "PrimitiveVec";
        case any:
            return "any";
        default:
            cerr << "Error: In " << __FUNCTION__ << ": EngineInstr with code: '" << instruction 
                << "' is undefined.\n"; 
        return "undefined";
    }
}
string instructionError(const InstrDescription & Description, const string & functionName, 
    const string & messageType
){
    if(Description.scriptName == ""){
        if(Description.layerID == ""){
            return messageType + ": In " + functionName + ":\n"
                + NEW_LINE_PADDING;
        }
        return messageType + ": In " + Description.layerID + "::" + Description.objectID + "::" 
            + Description.eventID + ": In the '" + instrToStr(Description.instruction)
            + "' instruction: In " + functionName + ":\n" + NEW_LINE_PADDING;
    }
    return messageType + ": In " + Description.scriptName + ":" + uIntToStr(Description.lineNumber) 
        + ":\n" + NEW_LINE_PADDING + "In " + Description.layerID + "::" + Description.objectID
        + "::" + Description.eventID + ": In the '" + instrToStr(Description.instruction) 
        + "' instruction: In " + functionName + ":\n" + NEW_LINE_PADDING;
}
string instructionWarning(const InstrDescription & Description, const string & functionName){
    return instructionError(Description, functionName, "Warning");
}

void PrimaryModule::primaryConstructor(size_t & topObjectUniqueIndex){
    uniqueIndex = topObjectUniqueIndex++;
    pos.set(0.0, 0.0);
    size.set(100.0, 100.0);
    scale.set(0.0, 0.0);
}
void PrimaryModule::primaryConstructor(PrimaryData & initData){
    primaryConstructor(*initData.topIndex);

    objectUniqueIndex = initData.objectUniqueIndex;
    layerUniqueIndex = initData.layerUniqueIndex;

    if(initData.listOfIDs != nullptr){
        setAllIDs(initData, true);
    }
    else{
        ID = initData.newID;
    }
}
void PrimaryModule::clone(const PrimaryModule & Original, PrimaryData & initData, bool changeOldID){
    objectUniqueIndex = initData.objectUniqueIndex;
    layerUniqueIndex = initData.layerUniqueIndex;
    
    initData.newID = Original.getID();
    setAllIDs(initData, changeOldID);

    groups = Original.groups;
    pos = Original.pos;
    size = Original.size;
    scale = Original.scale;

    isActive = Original.isActive;
    deleted = Original.deleted;
    isScaledFromCenter = Original.isScaledFromCenter;
    isScrollable = Original.isScrollable;
    canBeSelected = Original.canBeSelected;
}

void PrimaryModule::setUniqueIndex(size_t value){
    uniqueIndex = value;
}
size_t PrimaryModule::getUniqueIndex() const{
    return uniqueIndex;
}
void PrimaryModule::setObjectUniqueIndex(size_t value){
    objectUniqueIndex = value;
}
size_t PrimaryModule::getObjectUniqueIndex() const{
    return objectUniqueIndex;
}
void PrimaryModule::setLayerUniqueIndex(size_t value){
    layerUniqueIndex = value;
}
size_t PrimaryModule::getLayerUniqueIndex() const{
    return layerUniqueIndex;
}

void PrimaryModule::setID(const string & newID, vector<string> & listOfIDs){
    if(isStringInVector(reservedIDs, ID)){
        cerr << "Error: In " << __FUNCTION__ << ": reserved ID \'" << ID
            << "\' cannot be changed.\n";
        return;
    }
    removeFromStringVector(listOfIDs, ID);
    ID = findNewUniqueID(listOfIDs, newID);
    listOfIDs.push_back(ID);
}

void PrimaryModule::setLayerID(const string & newLayerID){
    layerID = newLayerID;
}

void PrimaryModule::setObjectID(const string & newOwnerID){
    objectID = newOwnerID;
}

void PrimaryModule::setAllIDs(PrimaryData & initData, bool changeOldID){
    if(changeOldID){
        setID(initData.newID, *initData.listOfIDs);
    }
    setLayerID(initData.newLayerID);
    setObjectID(initData.newObjectID);
}

void PrimaryModule::addGroup(const string & newGroup){
    addUniqueToStringVector(groups, newGroup);
}
void PrimaryModule::removeGroup(const string & selectedGroup){
    removeFromStringVector(groups, selectedGroup);
}
void PrimaryModule::clearGroups(){
    groups.clear();
}
bool PrimaryModule::isInAGroup(const string & findGroup) const{
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
void PrimaryModule::control(AttributeType attribute, bool value, unsigned paramCount){
    switch(attribute){
        case activate_a:
            activate();
            return;
        case deactivate_a:
            deactivate();
            return;
        case toggle:
            toggleIsActive();
            return;
        case scale_from_center:
            isScaledFromCenter = true;
            return;
        case do_not_scale_from_center:
            isScaledFromCenter = false;
            return;
        case set_scale_from_center:
            isScaledFromCenter = value;
            return;
        case disable_scrolling:
            isScrollable = false;
            return;
        case enable_scrolling:
            isScrollable = true;
            return;
        case set_is_scrollable:
            isScrollable = value;
            return;
        case allow_selection:
            canBeSelected = true;
            return;
        case forbid_selection:
            canBeSelected = false;
            return;
        case set_can_be_selected:
            canBeSelected = value;
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Function "
                << attributeToStr(attribute) << "<" << paramCount << "> does not exist.\n";
            return;
    }
}
void PrimaryModule::setIsScrollable(bool newIsScrollable){
    isScrollable = newIsScrollable;
}
void PrimaryModule::setCanBeSelected(bool newValue){
    canBeSelected = newValue;
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
vec2d PrimaryModule::getPos() const{
    return pos;
}
vec2d &PrimaryModule::getPosAddr(){
    return pos;
}
vec2d PrimaryModule::getSize() const{
    return size;
}
vec2d& PrimaryModule::getSizeAddr(){
    return size;
}
vec2d PrimaryModule::getScale() const{
    return scale;
}
vec2d PrimaryModule::getScaledSize() const{
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
bool PrimaryModule::getIsScaledFromCenter() const{
    return isScaledFromCenter;
}
bool PrimaryModule::getIsScrollable() const{
    return isScrollable;
}
bool PrimaryModule::getCanBeSelected(){
    return canBeSelected;
}
void PrimaryModule::deleteLater(){
    deleted = true;
    deactivate();
}
void PrimaryModule::getPrimaryContext(AttributeType attribute, vector<BasePointersStruct> &BasePointers){
    BasePointers.emplace_back(BasePointersStruct());
    switch(attribute){
        case id:
            if(isStringInVector(reservedIDs, ID)){
                cerr << "Error: In " << __FUNCTION__ << ": Access to the reserved ID \'" << ID << "\' address was denied.\n";
                BasePointers.pop_back();
                return;
            }
            BasePointers.back().setPointer(&ID);
            BasePointers.back().readOnly = true;
            return;
        case group:
            BasePointers.pop_back();
            for(string & group : groups){
                BasePointers.emplace_back(BasePointersStruct());
                BasePointers.back().setPointer(&group);
            }
            return;
        case pos_x:
            BasePointers.back().setPointer(&pos.x);
            return;
        case pos_y:
            BasePointers.back().setPointer(&pos.y);
            return;
        case size_x:
            BasePointers.back().setPointer(&size.x);
            return;
        case size_y:
            BasePointers.back().setPointer(&size.y);
            return;
        case scale_x:
            BasePointers.back().setPointer(&scale.x);
            return;
        case scale_y:
            BasePointers.back().setPointer(&scale.y);
            return;
        case is_active:
            BasePointers.back().setPointer(&isActive);
            return;
        case is_scaled_from_center:
            BasePointers.back().setPointer(&isScaledFromCenter);
            return;
        case is_scrollable:
            BasePointers.back().setPointer(&isScrollable);
            return;
        case can_be_selected:
            BasePointers.back().setPointer(&canBeSelected);
            return;
        default:
            BasePointers.pop_back();
            cerr << "Error: In " << __FUNCTION__ << ": Attribute '"
                << attributeToStr(attribute) << "' is not valid.\n";
            return;
    }
}


BaseType strToBaseType(const string & type){
    if(type == "null"){
        return null_bt;
    }
    else if(type == "bool"){
        return bool_bt;
    }
    else if(type == "char"){
        return char_bt;
    }
    else if(type == "short"){
        return short_bt;
    }
    else if(type == "unsigned_short"){
        return u_short_bt;
    }
    else if(type == "int"){
        return int_bt;
    }
    else if(type == "unsigned_int"){
        return u_int_bt;
    }
    else if(type == "float"){
        return float_bt;
    }
    else if(type == "double"){
        return double_bt;
    }
    else if(type == "string"){
        return string_bt;
    }
    cerr << "Error: In " << __FUNCTION__ << ": BaseType with the name \'"
        << type << "\' is not valid.\n";
    return null_bt;
}
string baseTypeToStr(const BaseType & type){
    switch (type) {
        case null_bt:
            return "null";
        case bool_bt:
            return "bool";
        case char_bt:
            return "char";
        case short_bt:
            return "short";
        case u_short_bt:
            return "unsigned_short";
        case int_bt:
            return "int";
        case u_int_bt:
            return "unsigned_int";
        case float_bt:
            return "float";
        case double_bt:
            return "double";
        case string_bt:
            return "string";
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << "is not valid.\n";
            return "null";
    }
}

BasePointersStruct::BasePointersStruct(){}
BasePointersStruct::BasePointersStruct(const BasePointersStruct &Original){
    type = Original.type;
    readOnly = Original.readOnly;
    switch (type) {
        case bool_bt:
            pBool = Original.pBool;
            return;
        case char_bt:
            pChar = Original.pChar;
            return;
        case short_bt:
            pShort = Original.pShort;
            return;
        case u_short_bt:
            pUShort = Original.pUShort;
            return;
        case int_bt:
            pInt = Original.pInt;
            return;
        case u_int_bt:
            pUInt = Original.pUInt;
            return;
        case float_bt:
            pFloat = Original.pFloat;
            return;
        case double_bt:
            pDouble = Original.pDouble;
            return;
        case string_bt:
            pString = Original.pString;
            return;
        case null_bt:
            // cerr << "Error: In " << __PRETTY_FUNCTION__
            //     << ": Cannot move value to a pointer of the 'null' type.\n";
            return;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            return;
    }
}
BasePointersStruct &BasePointersStruct::operator=(const BasePointersStruct &Original){
    type = Original.type;
    readOnly = Original.readOnly;
    switch (type) {
        case bool_bt:
            pBool = Original.pBool;
            break;
        case char_bt:
            pChar = Original.pChar;
            break;
        case short_bt:
            pShort = Original.pShort;
            break;
        case u_short_bt:
            pUShort = Original.pUShort;
            break;
        case int_bt:
            pInt = Original.pInt;
            break;
        case u_int_bt:
            pUInt = Original.pUInt;
            break;
        case float_bt:
            pFloat = Original.pFloat;
            break;
        case double_bt:
            pDouble = Original.pDouble;
            break;
        case string_bt:
            pString = Original.pString;
            break;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move value to a pointer of the 'null' type.\n";
            break;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            break;
    }
    return *this;
}
void BasePointersStruct::clear(){
    type = null_bt;
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
void BasePointersStruct::executeMoveTypeInstruction(LeftType * LeftOperand,
    const RightType * RightOperand, const EngineInstr & instruction
){
    if(instruction == EngineInstr::inc){
        (*LeftOperand)++;
        return;
    }
    else if(instruction == EngineInstr::dec){
        (*LeftOperand)--;
        return;
    }
    if(LeftOperand == nullptr){
        cerr << "Error: In " << __FUNCTION__ << ": Left operand of \'" << baseTypeToStr(type)
            << "\' type does not exist.\n";
        return;
    }
    if(RightOperand == nullptr){
        cerr << "Error: In " << __FUNCTION__ << ": Right operand of \'" << baseTypeToStr(type)
            << "\' type does not exist.\n";
        return;
    }
    switch (instruction) {
        case EngineInstr::move:
        case EngineInstr::clone_i:
            *LeftOperand = *RightOperand;
            return;
        case EngineInstr::add_move:
            *LeftOperand += *RightOperand;
            return;
        case EngineInstr::sub_move:
            *LeftOperand -= *RightOperand;
            return;
        case EngineInstr::mul_move:
            *LeftOperand *= *RightOperand;
            return;
        case EngineInstr::div_move:
            if(*RightOperand != 0){
                *LeftOperand /= *RightOperand;
            }
            else{
                cerr << "Error: In " << __FUNCTION__ << ": Cannot divide by zero.\n";
            }
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": \'" << instrToStr(instruction)
                << "\' is not a valid instruction.\n";
            return;
    }
}


template<typename LeftType>
void BaseVariableStruct::moveValue(LeftType * LeftOperand) const{
    switch (type) {
        case bool_bt:
            *LeftOperand = vBool;
            return;
        case char_bt:
            *LeftOperand = vChar;
            return;
        case short_bt:
            *LeftOperand = vShort;
            return;
        case u_short_bt:
            *LeftOperand = vUShort;
            return;
        case int_bt:
            *LeftOperand = vInt;
            return;
        case u_int_bt:
            *LeftOperand = vUInt;
            return;
        case float_bt:
            *LeftOperand = vFloat;
            return;
        case double_bt:
            *LeftOperand = vDouble;
            return;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move value to a variable of the 'null' type.\n";
            return;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            return;
    }
}
BaseVariableStruct::BaseVariableStruct(){}
BaseVariableStruct::BaseVariableStruct(const BaseVariableStruct &Original){
    type = Original.type;
    switch (type) {
        case bool_bt:
            vBool = Original.vBool;
            return;
        case char_bt:
            vChar = Original.vChar;
            return;
        case short_bt:
            vShort = Original.vShort;
            return;
        case u_short_bt:
            vUShort = Original.vUShort;
            return;
        case int_bt:
            vInt = Original.vInt;
            return;
        case u_int_bt:
            vUInt = Original.vUInt;
            return;
        case float_bt:
            vFloat = Original.vFloat;
            return;
        case double_bt:
            vDouble = Original.vDouble;
            return;
        case string_bt:
            vString = Original.vString;
            return;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move value to a variable of the 'null' type.\n";
            return;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return;
    }
}
BaseVariableStruct &BaseVariableStruct::operator=(const BaseVariableStruct &Original){
    type = Original.type;
    switch (type) {
        case bool_bt:
            vBool = Original.vBool;
            break;
        case char_bt:
            vChar = Original.vChar;
            break;
        case short_bt:
            vShort = Original.vShort;
            break;
        case u_short_bt:
            vUShort = Original.vUShort;
            break;
        case int_bt:
            vInt = Original.vInt;
            break;
        case u_int_bt:
            vUInt = Original.vUInt;
            break;
        case float_bt:
            vFloat = Original.vFloat;
            break;
        case double_bt:
            vDouble = Original.vDouble;
            break;
        case string_bt:
            vString = Original.vString;
            break;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move value to a variable of the 'null' type.\n";
            break;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            break;
    }
    return *this;
}
string BaseVariableStruct::getString() const{
    switch (type) {
        case bool_bt:
            if(vBool){
                return "true";
            }
            return "false";
        case char_bt:
            return string(1, vChar);
        case short_bt:
            return shortToStr(vShort);
        case u_short_bt:
            return intToStr(vUShort);
        case int_bt:
            return intToStr(vInt);
        case u_int_bt:
            return uIntToStr(vUInt);
        case float_bt:
            return doubleToStr(vFloat);
        case double_bt:
            return doubleToStr(vDouble);
        case string_bt:
            return vString;
        case null_bt:
            return "";
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is undefined.\n";
            return "undefined";
    }
    return "";
}

void BasePointersStruct::tryToSetValue(const BaseVariableStruct & RightOperand){
    if(readOnly){
        cerr << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    switch (type) {
        case bool_bt:
            RightOperand.moveValue(pBool);
            return;
        case char_bt:
            RightOperand.moveValue(pChar);
            return;
        case short_bt:
            RightOperand.moveValue(pShort);
            return;
        case u_short_bt:
            RightOperand.moveValue(pUShort);
            return;
        case int_bt:
            RightOperand.moveValue(pInt);
            return;
        case u_int_bt:
            RightOperand.moveValue(pUInt);
            return;
        case float_bt:
            RightOperand.moveValue(pFloat);
            return;
        case double_bt:
            RightOperand.moveValue(pDouble);
            return;
        case string_bt:
            if(RightOperand.type == string_bt){
                *pString = RightOperand.getString();
            }
            else{
                cerr << "Error: In " << __FUNCTION__
                    << ": Cannot assign a non string value to a string variable.\n";
            }
            return;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move value to a pointer of the 'null' type.\n";
            return;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return;
    }
}
template<typename RightType>
void BasePointersStruct::moveFromTemp(const RightType * RightOperand,
    const EngineInstr & instruction
){
    if(readOnly){
        cerr << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    switch (type) {
        case bool_bt:{
            short temp = *pBool;
            executeMoveTypeInstruction(&temp, RightOperand, instruction);
            *pBool = temp > 0;}
            return;
        case char_bt:
            executeMoveTypeInstruction(pChar, RightOperand, instruction);
            return;
        case short_bt:
            executeMoveTypeInstruction(pShort, RightOperand, instruction);
            return;
        case u_short_bt:
            executeMoveTypeInstruction(pUShort, RightOperand, instruction);
            return;
        case int_bt:
            executeMoveTypeInstruction(pInt, RightOperand, instruction);
            return;
        case u_int_bt:
            executeMoveTypeInstruction(pUInt, RightOperand, instruction);
            return;
        case float_bt:
            executeMoveTypeInstruction(pFloat, RightOperand, instruction);
            return;
        case double_bt:
            executeMoveTypeInstruction(pDouble, RightOperand, instruction);
            return;
        case string_bt:
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move numeric value to a pointer of the '" << baseTypeToStr(type)
                << "' type.\n";
            return;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return;
    }
}
void BasePointersStruct::move(const BasePointersStruct &RightOperand,
    const EngineInstr & instruction
){
    if(readOnly){
        cerr << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveFromTemp((short*)nullptr, instruction);
        return;
    }
    switch (RightOperand.type) {
        case bool_bt:{
            short temp = *RightOperand.pBool;
            moveFromTemp(&temp, instruction);}
            return;
        case char_bt:
            moveFromTemp(RightOperand.pChar, instruction);
            return;
        case short_bt:
            moveFromTemp(RightOperand.pShort, instruction);
            return;
        case u_short_bt:
            moveFromTemp(RightOperand.pUShort, instruction);
            return;
        case int_bt:
            moveFromTemp(RightOperand.pInt, instruction);
            return;
        case u_int_bt:
            moveFromTemp(RightOperand.pUInt, instruction);
            return;
        case float_bt:
            moveFromTemp(RightOperand.pFloat, instruction);
            return;
        case double_bt:
            moveFromTemp(RightOperand.pDouble, instruction);
            return;
        case string_bt:
            if(RightOperand.type == string_bt){
                if(instruction == EngineInstr::move){
                    *pString = RightOperand.getString();
                }
                else if(instruction == EngineInstr::add_move){
                    *pString += RightOperand.getString();
                }
                else{
                    cerr << "Error: In " << __FUNCTION__ << ": Cannot execute \'"
                        << instrToStr(instruction) << "\' instruction on a string type values.\n";
                }
            }
            else{
                cerr << "Error: In " << __FUNCTION__
                    << ": Cannot execute any instructions if only the left operand is "
                    << "of a string type.\n";
            }
            return;
        default:
            break;
    }
    if(RightOperand.type == null_bt){
        cerr << "Error: In " << __PRETTY_FUNCTION__
            << ": Cannot move value of the 'null' type to a pointer of the '" << baseTypeToStr(type)
            << "' type.\n";
    }
    else{
        cerr << "Error: In " << __PRETTY_FUNCTION__
            << ": BaseType with the code: " << RightOperand.type << " is not valid.\n";
    }
}
void BasePointersStruct::move(const BaseVariableStruct & RightOperand, 
    const EngineInstr & instruction
){
    if(readOnly){
        cerr << "Error: In " << __FUNCTION__ << ": This pointer is read-only.\n";
        return;
    }
    if(instruction == EngineInstr::inc || instruction == EngineInstr::dec){
        moveFromTemp((short*)nullptr, instruction);
        return;
    }
    switch (RightOperand.type) {
        case bool_bt:{
            short temp = RightOperand.vBool;
            moveFromTemp(&temp, instruction);}
            return;
        case char_bt:
            moveFromTemp(&RightOperand.vChar, instruction);
            return;
        case short_bt:
            moveFromTemp(&RightOperand.vShort, instruction);
            return;
        case u_short_bt:
            moveFromTemp(&RightOperand.vUShort, instruction);
            return;
        case int_bt:
            moveFromTemp(&RightOperand.vInt, instruction);
            return;
        case u_int_bt:
            moveFromTemp(&RightOperand.vUInt, instruction);
            return;
        case float_bt:
            moveFromTemp(&RightOperand.vFloat, instruction);
            return;
        case double_bt:
            moveFromTemp(&RightOperand.vDouble, instruction);
            return;
        case string_bt:
            if(type == string_bt){
                if(instruction == EngineInstr::move){
                    *pString = RightOperand.getString();
                }
                else if(instruction == EngineInstr::add_move){
                    *pString += RightOperand.getString();
                }
                else{
                    cerr << "Error: In " << __FUNCTION__ << ": Cannot execute \'"
                        << instrToStr(instruction) << "\' instruction on a string type values.\n";
                }
            }
            else{
                cerr << "Error: In " << __FUNCTION__ << ": Cannot execute any instructions if only"
                    << " the left operand is of a string type.\n";
            }
            return;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Cannot move value of the 'null' type to a pointer of the '"
                << baseTypeToStr(type) << "' type.\n";
            return;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": BaseType with the code: " << RightOperand.type << " is not valid.\n";
            return;
    }
}
template<typename LeftType, typename RightType>
LeftType BasePointersStruct::tryArithmetics(LeftType * LeftOperand, const RightType * RightOperand,
    const EngineInstr & instruction
){
    if(LeftOperand == nullptr){
        cerr << "Error: In " << __FUNCTION__ << ": Left operand of \'" << type
            << "\' type does not exist.\n";
        return 0;
    }
    if(RightOperand == nullptr){
        cerr << "Error: In " << __FUNCTION__ << ": Right operand of \'" << type
            << "\' type does not exist.\n";
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
            cerr << "Error: In " << __FUNCTION__ << ": Cannot divide by zero.\n";
        }
    }
    else{
        cerr << "Error: In " << __FUNCTION__ << ": \'" << instrToStr(instruction)
            << "\' is not a valid instruction.\n";
    }
    return 0;
}
template<typename LeftType>
LeftType BasePointersStruct::callTryArithmeticsForEveryType(LeftType * LeftOperand,
    const BasePointersStruct & RightOperand, const EngineInstr & instruction
){
    switch (RightOperand.type) {
        case bool_bt:{
            short right = *RightOperand.pBool;
            return tryArithmetics(LeftOperand, &right, instruction) > 0;}
        case char_bt:
            return tryArithmetics(LeftOperand, RightOperand.pChar, instruction);
        case short_bt:
            return tryArithmetics(LeftOperand, RightOperand.pShort, instruction);
        case u_short_bt:
            return tryArithmetics(LeftOperand, RightOperand.pUShort, instruction);
        case int_bt:
            return tryArithmetics(LeftOperand, RightOperand.pInt, instruction);
        case u_int_bt:
            return tryArithmetics(LeftOperand, RightOperand.pUInt, instruction);
        case float_bt:
            return tryArithmetics(LeftOperand, RightOperand.pFloat, instruction);
        case double_bt:
            return tryArithmetics(LeftOperand, RightOperand.pDouble, instruction);
        case string_bt:
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Right operand is of the invalid type: '"
                << baseTypeToStr(RightOperand.type) << "'.\n";
            return 0;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return 0;
    }
    return 0;
}
BaseVariableStruct BasePointersStruct::executeArithmetics(const BasePointersStruct &RightOperand,
    const EngineInstr & instruction
){
    BaseVariableStruct result;
    result.type = type;

    switch (type) {
        case bool_bt:{
            short left = *pBool;
            result.vBool = callTryArithmeticsForEveryType(&left, RightOperand, instruction);}
            break;
        case char_bt:
            result.vChar = callTryArithmeticsForEveryType(pChar, RightOperand, instruction);
            break;
        case short_bt:
            result.vShort = callTryArithmeticsForEveryType(pShort, RightOperand, instruction);
            break;
        case u_short_bt:
            result.vUShort = callTryArithmeticsForEveryType(pUShort, RightOperand, instruction);
            break;
        case int_bt:
            result.vInt = callTryArithmeticsForEveryType(pInt, RightOperand, instruction);
            break;
        case u_int_bt:
            result.vUInt = callTryArithmeticsForEveryType(pUInt, RightOperand, instruction);
            break;
        case float_bt:
            result.vFloat = callTryArithmeticsForEveryType(pFloat, RightOperand, instruction);
            break;
        case double_bt:
            result.vDouble = callTryArithmeticsForEveryType(pDouble, RightOperand, instruction);
            break;
        case string_bt:
            if(RightOperand.type == string_bt && instruction == EngineInstr::add){
                result.vString = *pString + *RightOperand.pString;
            }
            else{
                result.type = null_bt;
                cerr << "Error: In " << __FUNCTION__
                    << ": Cannot execute arithmetic operations with string type values."
                    << "The only exception is addition of two strings.\n";
            }
            break;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": About the left operand: 'null' is not a valid type.\n"; 
            result.type = null_bt;
            break;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            break;
    }
    return result;
}
template<typename LeftType>
LeftType BasePointersStruct::callTryArithmetics(LeftType * LeftOperand,
    const BaseVariableStruct & RightOperand, const EngineInstr & instruction
){
    switch (RightOperand.type) {
        case bool_bt:{
            short right = RightOperand.vBool;
            return tryArithmetics(LeftOperand, &right, instruction) > 0;}
        case char_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vChar, instruction);
        case short_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vShort, instruction);
        case u_short_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vUShort, instruction);
        case int_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vInt, instruction);
        case u_int_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vUInt, instruction);
        case float_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vFloat, instruction);
        case double_bt:
            return tryArithmetics(LeftOperand, &RightOperand.vDouble, instruction);
        case string_bt:
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Right operand is of the invalid type: '"
                << baseTypeToStr(RightOperand.type) << "'.\n";
            return 0;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return 0;
    }
    return 0;
}
BaseVariableStruct BasePointersStruct::executeArithmetics(const BaseVariableStruct & RightOperand,
    const EngineInstr & instruction
){
    BaseVariableStruct result;
    result.type = type;
    
    switch (type) {
        case bool_bt:{
            short left = *pBool;
            result.vBool = callTryArithmetics(&left, RightOperand, instruction) > 0;}
            break;
        case char_bt:
            result.vChar = callTryArithmetics(pChar, RightOperand, instruction);
            break;
        case short_bt:
            result.vShort = callTryArithmetics(pShort, RightOperand, instruction);
            break;
        case u_short_bt:
            result.vUShort = callTryArithmetics(pUShort, RightOperand, instruction);
            break;
        case int_bt:
            result.vInt = callTryArithmetics(pInt, RightOperand, instruction);
            break;
        case u_int_bt:
            result.vUInt = callTryArithmetics(pUInt, RightOperand, instruction);
            break;
        case float_bt:
            result.vFloat = callTryArithmetics(pFloat, RightOperand, instruction);
            break;
        case double_bt:
            result.vDouble = callTryArithmetics(pDouble, RightOperand, instruction);
            break;
        case string_bt:
            if(RightOperand.type == string_bt && instruction == EngineInstr::add){
                result.vString = *pString + RightOperand.vString;
            }
            else{
                result.type = null_bt;
                cerr << "Error: In " << __FUNCTION__ << ": Cannot execute arithmetic operations"
                    << " with string type values. The only exception is addition on two strings.\n";
            }
            break;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": About the left operand: 'null' is not a valid type.\n"; 
            result.type = null_bt;
            break;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            result.type = null_bt;
            break;
    }

    return result;
}
void BasePointersStruct::setTypeFromVariableType(char variableType){
    switch(variableType){
    case 'n':
        type = null_bt;
        return;
    case 'b':
        type = bool_bt;
        return;
    case 'i':
        type = int_bt;
        return;
    case 'd':
        type = double_bt;
        return;
    case 's':
        type = string_bt;
        return;
    default:
        return;
    }
}
void BasePointersStruct::setPointer(bool *pointer){
    pBool = pointer;
    type = bool_bt;
}
void BasePointersStruct::setPointer(char * pointer){
    pChar = pointer;
    type = char_bt;
}
void BasePointersStruct::setPointer(short * pointer){
    pShort = pointer;
    type = short_bt;
}
void BasePointersStruct::setPointer(unsigned short * pointer){
    pUShort = pointer;
    type = u_short_bt;
}
void BasePointersStruct::setPointer(int * pointer){
    pInt = pointer;
    type = int_bt;
}
void BasePointersStruct::setPointer(unsigned int * pointer){
    pUInt = pointer;
    type = u_int_bt;
}
void BasePointersStruct::setPointer(float * pointer){
    pFloat = pointer;
    type = float_bt;
}
void BasePointersStruct::setPointer(double * pointer){
    pDouble = pointer;
    type = double_bt;
}
void BasePointersStruct::setPointer(string * pointer){
    pString = pointer;
    type = string_bt;
}
ReturnType BasePointersStruct::setPointer(const BasePointersStruct & Pointers,
    const InstrDescription & CurrentInstr
){
    if(readOnly){
        cout << instructionError(CurrentInstr, __FUNCTION__)
            << "Pointer is read-only and cannot be reassigned.\n";
        return READ_ONLY;
    }
    if(type != null_bt && type != Pointers.type){
        cout << instructionError(CurrentInstr, __FUNCTION__)
            << "Cannot assign a reference of the '" << baseTypeToStr(Pointers.type)
            << "' type to a pointer of the '" << baseTypeToStr(type) << "' type.\n";
        return INVALID_TYPE;
    }
    *this = Pointers;
    return OK;
}
void BasePointersStruct::negate(){
    switch (type) {
        case bool_bt:
            *pBool = !*pBool;
            return;
        case char_bt:
            *pChar = !*pChar;
            return;
        case short_bt:
            *pShort = !*pShort;
            return;
        case u_short_bt:
            *pUShort = !*pUShort;
            return;
        case int_bt:
            *pInt = !*pInt;
            return;
        case u_int_bt:
            *pUInt = !*pUInt;
            return;
        case float_bt:
            *pFloat = !*pFloat;
            return;
        case double_bt:
            *pDouble = !*pDouble;
            return;
        default:
            return;
    }
}

bool BasePointersStruct::areEqual(BasePointersStruct *OtherVariable){
    if(type != OtherVariable->type){
        cerr << "Error: In " << __FUNCTION__ << ":Pointers of built-in types have different types."
            << "\n";
        return false;
    }
    switch (type) {
        case bool_bt:
            return *pBool == *OtherVariable->pBool;
        case char_bt:
            return *pChar == *OtherVariable->pChar;
        case short_bt:
            return *pShort == *OtherVariable->pShort;
        case u_short_bt:
            return *pUShort == *OtherVariable->pUShort;
        case int_bt:
            return *pInt == *OtherVariable->pInt;
        case u_int_bt:
            return *pUInt == *OtherVariable->pUInt;
        case float_bt:
            return *pFloat == *OtherVariable->pFloat;
        case double_bt:
            return *pDouble == *OtherVariable->pDouble;
        case string_bt:
            return *pString == *OtherVariable->pString;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Operand is of the invalid type: '"
                << baseTypeToStr(type) << "'.\n";
            return false;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return false;
    }
    return false;
}
bool BasePointersStruct::areEqual(BaseVariableStruct *OtherVariable){
    if(type != OtherVariable->type){
        cerr << "Error: In " << __FUNCTION__ << ": Pointers of built-in types have different types."
            << "\n";
        return false;
    }
    switch (type) {
        case bool_bt:
            return *pBool == OtherVariable->vBool;
        case char_bt:
            return *pChar == OtherVariable->vChar;
        case short_bt:
            return *pShort == OtherVariable->vShort;
        case u_short_bt:
            return *pUShort == OtherVariable->vUShort;
        case int_bt:
            return *pInt == OtherVariable->vInt;
        case u_int_bt:
            return *pUInt == OtherVariable->vUInt;
        case float_bt:
            return *pFloat == OtherVariable->vFloat;
        case double_bt:
            return *pDouble == OtherVariable->vDouble;
        case string_bt:
            return *pString == OtherVariable->vString;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Operand is of the invalid type: '"
                << baseTypeToStr(type) << "'.\n";
            return false;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            type = null_bt;
            return false;
    }
    return false;
}

bool BasePointersStruct::getBool() const{
    switch (type) {
        case bool_bt:
            return *pBool;
        case char_bt:
            return *pChar;
        case short_bt:
            return *pShort;
        case u_short_bt:
            return *pUShort;
        case int_bt:
            return *pInt;
        case u_int_bt:
            return *pUInt;
        case float_bt:
            return *pFloat;
        case double_bt:
            return *pDouble;
        case string_bt:
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": Operand is of the invalid type: '"
                << baseTypeToStr(type) << "'.\n";
            return false;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            return false;
    }
}
bool BasePointersStruct::isInteger() const{
    switch (type) {
        case int_bt:
        case u_int_bt:
        case short_bt:
        case u_short_bt:
            return true;
        default:
            return false;
    }
    return false;
}
bool BasePointersStruct::isNumeric() const{
    switch (type) {
        case char_bt:
        case string_bt:
            return false;
        default:
            return true;
    }
    return false;
}
double BasePointersStruct::getDouble() const{
    switch (type) {
        case bool_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pBool;
        case char_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pChar;
        case short_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pShort;
        case u_short_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pUShort;
        case int_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pInt;
        case u_int_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pUInt;
        case float_bt:
            return *pFloat;
        case double_bt:
            return *pDouble;
        case string_bt:
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Operand is of the invalid type: '"
                << baseTypeToStr(type) << "'.\n";
            return 0.0;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            return 0.0;
    }
    return 0.0;
}
int BasePointersStruct::getInt() const{
    switch (type) {
        case bool_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": Conversion from bool to int.\n";
            return *pBool;
        case char_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": No floating point.\n";
            return *pChar;
        case short_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": Conversion from short to int.\n";
            return *pShort;
        case u_short_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": Conversion from unsigned short to int.\n";
            return *pUShort;
        case int_bt:
            return *pInt;
        case u_int_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": Conversion from unsigned int to int.\n";
            return *pUInt;
        case float_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": Floating point ignored.\n";
            return *pFloat;
        case double_bt:
            //cout << "Warning: In " << __FUNCTION__ << ": Floating point ignored.\n";
            return *pDouble;
        case string_bt:
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Operand is of the invalid type: '"
                << baseTypeToStr(type) << "'.\n";
            return 0;
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            return 0;
    }
    return 0;
}
string BasePointersStruct::getString() const{
    switch (type) {
        case bool_bt:
            if(*pBool){
                return "true";
            }
            return "false";
        case char_bt:
            return string(1, *pChar);
        case short_bt:
            return shortToStr(*pShort);
        case u_short_bt:
            return intToStr(*pUShort);
        case int_bt:
            return intToStr(*pInt);
        case u_int_bt:
            return uIntToStr(*pUInt);
        case float_bt:
            return doubleToStr(*pFloat);
        case double_bt:
            return doubleToStr(*pDouble);
        case string_bt:
            return *pString;
        case null_bt:
            cerr << "Error: In " << __PRETTY_FUNCTION__
                << ": Operand is of the invalid type: '"
                << baseTypeToStr(type) << "'.\n";
            return "invalid";
        default:
            cerr << "Error: In " << __PRETTY_FUNCTION__ << ": BaseType with code: "
                << type << " is not valid.\n";
            return "undefined";
    }
}

string attributeToStr(const AttributeType &attribute){
    switch (attribute){
        case null_a: return "null";
        case set_id: return "set_id";
        case set_pos: return "set_pos";
        case set_size: return "set_size";
        case set_scale: return "set_scale";
        case add_scale: return "add_scale";
        case set_color: return "set_color";
        case set_color_r: return "set_color_r";
        case set_color_g: return "set_color_g";
        case set_color_b: return "set_color_b";
        case set_color_a: return "set_color";
        case set_random_color: return "set_random_color";
        case increment_random_color: return "increment_random_color";
        case set_rotation: return "set_rotation";
        case set_random_change_speed: return "set_random_change_speed";
        case set_min_color: return "set_min_color";
        case set_max_color: return "set_max_color";
        case rotate: return "rotate";
        case set_font: return "set_font";
        case add_new_content: return "add_new_content";
        case add_new_and_update: return "add_new_and_update";
        case update_size: return "update_size";
        case select_content: return "select_content";
        case delete_a: return "delete";
        case add_text: return "add_text";
        case set_text: return "set_text";
        case set_resize_text: return "set_resize_text";
        case set_wrapping: return "set_wrapping";
        case set_horizontal_align: return "set_horizontal_align";
        case set_vertical_align: return "set_vertical_align";
        case set_cursor_pos: return "set_cursor_pos";
        case set_second_cursor_pos: return "set_second_cursor_pos";
        case set_editable: return "set_editable";
        case set_space_use: return "set_space_use";
        case set_enter_use: return "set_enter_use";
        case set_can_enter_accept: return "set_can_enter_accept";
        case set_numerical: return "set_numerical";
        case set_floating_point: return "set_floating_point";
        case set_can_update_variable: return "set_can_update_variable";
        case set_auto_cleaning: return "set_auto_cleaning";
        case set_use_tabs: return "set_use_tabs";
        case set_min_content_length: return "set_min_content_length";
        case set_max_content_length: return "set_max_content_length";
        case set_input_delay: return "set_input_delay";
        case set_repetition_delay: return "set_repetition_delay";
        case connect_object: return "connect_object";
        case connect_group: return "connect_group";
        case connect_module: return "connect_module";
        case connect_module_id: return "connect_module_id";
        case connect_variable: return "connect_variable";
        case set_protected_area: return "set_protected_area";
        case cut_unprotected_area: return "cut_unprotected_area";
        case stop_editing: return "stop_editing";
        case start_editing: return "start_editing";
        case set_ignore_vertical_arrows: return "set_ignore_vertical_arrows";
        case set_can_auto_resize_axis_y: return "set_can_auto_resize_axis_y";
        case update: return "update";
        case crop_size_to_text: return "crop_size_to_text";
        case crop_width_to_text: return "crop_width_to_text";
        case crop_height_to_text: return "crop_height_to_text";
        case set_content: return "set_content";
        case add_content: return "add_content";
        case add_new_text_line: return "add_new_text_line";
        case set_text_line: return "set_text_line";
        case add_to_text_line: return "add_to_text_line";
        case save_to_file: return "save_to_file";
        case load_from_file: return "load_from_file";
        case add_format: return "add_format";
        case modify_format: return "modify_format";
        case modify_last_format: return "modify_last_format";
        case inject_format: return "inject_format";
        case delete_format: return "delete_format";
        case set_last_color: return "set_last_color";
        case set_accent_color: return "set_accent_color";
        case set_last_accent_color: return "set_last_accent_color";
        case set_background_color: return "set_background_color";
        case set_last_background_color: return "set_last_background_color";
        case set_background_color_drawing: return "set_background_color";
        case set_last_background_color_drawing: return "set_last_background_color";
        case set_offset: return "set_offset";
        case add_offset: return "add_offset";
        case set_offset_x: return "set_offset_x";
        case add_offset_x: return "add_offset_x";
        case set_offset_y: return "set_offset_y";
        case add_offset_y: return "add_offset_y";
        case set_selected: return "set_selected";
        case set_limit: return "set_limit";
        case set_padding_between_lines: return "set_padding_between_lines";
        case add_rotation: return "add_rotation";
        case set_tab_length: return "set_tab_length";
        case set_draw_selection_first: return "set_draw_selection_first";
        case set_can_be_edited: return "set_can_be_edited";
        case set_can_use_space: return "set_can_use_space";
        case set_can_use_enter: return "set_can_use_enter";
        case set_can_use_tabs: return "set_can_use_tabs";
        case set_is_numerical: return "set_is_numerical";
        case set_has_floating_point: return "set_has_floating_point";
        case set_ignore_content_restriction: return "set_ignore_content_restriction";
        case set_is_storing_history: return "set_is_storing_history";
        case set_can_copy_format: return "set_can_copy_format";
        case deactivate_a: return "deactivate";
        case resize: return "resize";
        case set_mirror: return "set_mirror";
        case set_light: return "set_light";
        case connect_bitmap: return "connect_bitmap";
        case connect_bitmap_via_path: return "connect_bitmap_via_path";
        case connect_bitmap_via_alias: return "connect_bitmap_via_alias";
        case rot_pos_x: return "rot_pos_x";
        case rot_pos_y: return "rot_pos_y";
        case start_x: return "start_x";
        case start_y: return "start_y";
        case frame_size_x: return "frame_size_x";
        case frame_size_y: return "frame_size_y";
        case rotate_angle: return "rotate_angle";
        case mirror_x: return "mirror_x";
        case mirror_y: return "mirror_y";
        case image_color_r: return "image_color_r";
        case image_color_g: return "image_color_g";
        case image_color_b: return "image_color_b";
        case image_color_a: return "image_color_a";
        case light_level: return "light_level";
        case light_color_r: return "light_color_r";
        case light_color_g: return "light_color_g";
        case light_color_b: return "light_color_b";
        case move_a: return "move";
        case move_up: return "move_up";
        case move_right: return "move_right";
        case move_down: return "move_down";
        case move_left: return "move_left";
        case move_jump: return "move_jump";
        case move_fall: return "move_fall";
        case move_run: return "move_run";
        case set_movement_type: return "set_movement_type";
        case set_input_type: return "set_input_type";
        case set_allowed_jumps: return "set_allowed_jumps";
        case set_jump_cooldown: return "set_jump_cooldown";
        case set_move_cooldown: return "set_move_cooldown";
        case set_diagonal_movement: return "set_diagonal_movement";
        case set_reset_momentum_when_jumping: return "set_reset_momentum_when_jumping";
        case set_mass: return "set_mass";
        case set_walking_speed: return "set_walking_speed";
        case set_running_speed: return "set_running_speed";
        case set_jump_speed: return "set_jump_speed";
        case set_gravity: return "set_gravity";
        case set_friction: return "set_friction";
        case set_max_momentum_x: return "set_max_momentum_x";
        case set_momentum_y: return "set_momentum_y";
        case set_min_momentum_y: return "set_min_momentum_y";
        case set_max_momentum_y: return "set_max_momentum_y";
        case set_mouse_movement_button: return "set_mouse_movement_button";
        case set_move_on_mouse_release: return "set_move_on_mouse_release";
        case set_direction_reset_after_collision: return "set_direction_reset_after_collision";
        case set_moving_keys: return "set_moving_keys";
        case set_key_up: return "set_key_up";
        case set_key_right: return "set_key_right";
        case set_key_down: return "set_key_down";
        case set_key_left: return "set_key_left";
        case set_key_jump: return "set_key_jump";
        case set_key_running: return "set_key_running";
        case set_random_actions: return "set_random_actions";
        case set_action_time: return "set_action_time";
        case set_min_action_time: return "set_min_action_time";
        case set_max_action_time: return "set_max_action_time";
        case set_move_distance: return "set_move_distance";
        case set_min_move_distance: return "set_min_move_distance";
        case set_max_move_distance: return "set_max_move_distance";
        case set_breaks_allowed: return "set_breaks_allowed";
        case set_break_chance: return "set_break_chance";
        case set_solid: return "set_solid";
        case toggle_solid: return "toggle_solid";
        case set_penetration: return "set_penetration";
        case toggle_penetration: return "toggle_penetration";
        case ignore: return "ignore";
        case stop_ignoring: return "stop_ignoring";
        case clear_ignoring: return "clear_ignoring";
        case ignore_object: return "ignore_object";
        case ignore_group_of_objects: return "ignore_group_of_objects";
        case ignore_hitbox: return "ignore_hitbox";
        case ignore_group_of_hitboxes: return "ignore_group_of_hitboxes";
        case set_is_circle: return "set_is_circle";
        case set_environment: return "set_environment";
        case set_environment_speed: return "set_environment_speed";
        case set_speed: return "set_speed";
        case set_basic_speed: return "set_basic_speed";
        case set_acceleration: return "set_acceleration";
        case set_directional_degree: return "set_directional_degree";
        case set_rotation_speed: return "set_rotation_speed";
        case set_time_to_negate_rotation: return "set_time_to_negate_rotation";
        case set_are_particles_moving: return "set_are_particles_moving";
        case allow_movement: return "allow_movement";
        case set_is_env_sync: return "set_is_env_sync";
        case set_radius: return "set_radius";
        case set_line_join_type: return "set_line_join_type";
        case set_time_to_death: return "set_time_to_death";
        case set_shape_rotation_speed: return "set_shape_rotation_speed";
        case set_color_intensity: return "set_color_intensity";
        case add_color_interval: return "add_color_interval";
        case add_color_interval_in_hex: return "add_color_interval_in_hex";
        case remove_color_interval: return "remove_color_interval";
        case add_color: return "add_color";
        case add_color_in_hex: return "add_color_in_hex";
        case remove_color: return "remove_color";
        case add_image: return "add_image";
        case remove_image: return "remove_image";
        case remove_image_by_index: return "remove_image_by_index";
        case set_shape: return "set_shape";
        case set_use_images: return "set_use_images";
        case toggle_source: return "toggle_source";
        case set_use_random_colors: return "set_use_random_colors";
        case toggle_random_colors: return "toggle_random_colors";
        case set_is_static: return "set_is_static";
        case set_drawing_details: return "set_drawing_details";
        case set_block_spawning: return "set_block_spawning";
        case set_spawning: return "set_spawning";
        case allow_spawning: return "allow_spawning";
        case set_spawn_rate: return "set_spawn_rate";
        case set_spawn_cap: return "set_spawn_cap";
        case set_max_time_to_spawn: return "set_max_time_to_spawn";
        case bind_key: return "bind_key";
        case bind_key_by_int: return "bind_key_by_int";
        case spawn_on_key_release: return "spawn_on_key_release";
        case toggle_bool: return "toggle_bool";
        case set_bool_random: return "set_bool_random";
        case set_bool_probability: return "set_bool_probability";
        case set_bool: return "set_bool";
        case set_int: return "set_int";
        case set_int_interval: return "set_int_interval";
        case set_int_random: return "set_int_random";
        case set_double: return "set_double";
        case set_double_interval: return "set_double_interval";
        case set_double_random: return "set_double_random";
        case set_string: return "set_string";
        case set_string_random: return "set_string_random";
        case add_int: return "add_int";
        case add_int_interval: return "add_int_interval";
        case add_int_random: return "add_int_random";
        case add_double: return "add_double";
        case add_double_interval: return "add_double_interval";
        case add_double_random: return "add_double_random";
        case set_thumb_position: return "set_thumb_position";
        case set_thumb_size: return "set_thumb_size";
        case set_scrolling_area: return "set_scrolling_area";
        case set_real_scrolling_area: return "set_real_scrolling_area";
        case add_real_scrolling_area: return "add_real_scrolling_area";
        case set_thumb_image_id: return "set_thumb_image_id";
        case set_track_image_id: return "set_track_image_id";
        case set_can_be_drawn: return "set_can_be_drawn";
        case set_mouse_wheel_speed: return "set_mouse_wheel_speed";
        case scroll_to_the_beginning: return "scroll_to_the_beginning";
        case scroll_to_the_end: return "scroll_to_the_end";
        case scroll_by_distance: return "scroll_by_distance";
        case add_camera: return "add_camera";
        case remove_camera: return "remove_camera";
        case clear_cameras: return "clear_cameras";
        case update_with_size: return "update_with_size";
        case set_type: return "set_type";
        case add_point: return "add_point";
        case remove_point: return "remove_point";
        case set_point: return "set_point";
        case move_point: return "move_point";
        case set_thickness: return "set_thickness";
        case create: return "create";
        case push_back_a: return "push_back";
        case pop_back_a: return "pop_back";
        case clear_a: return "clear";
        case remove_a: return "remove";
        case set_a: return "set";
        case else_child_id: return "else_child_id";
        case group: return "group";
        case layer_id: return "layer_id";
        case number_of_cameras_a: return "number_of_cameras";
        case number_of_layers_a: return "number_of_layers";
        case number_of_objects_a: return "number_of_objects";
        case camera_a: return "camera";
        case is_active: return "is_active";
        case id: return "id";
        case pos_x: return "pos_x";
        case pos_y: return "pos_y";
        case relative_pos_x: return "relative_pos_x";
        case relative_pos_y: return "relative_pos_y";
        case vision_shift_x: return "vision_shift_x";
        case vision_shift_y: return "vision_shift_y";
        case size_x: return "size_x";
        case size_y: return "size_y";
        case zoom: return "zoom";
        case min_zoom: return "min_zoom";
        case max_zoom: return "max_zoom";
        case speed: return "speed";
        case zoom_in_key: return "zoom_in_key";
        case zoom_out_key: return "zoom_out_key";
        case zoom_reset_key: return "zoom_reset_key";
        case up_key: return "up_key";
        case right_key: return "right_key";
        case down_key: return "down_key";
        case left_key: return "left_key";
        case pinned_camera_id: return "pinned_camera_id";
        case followed_layer_id: return "followed_layer_id";
        case followed_object_id: return "followed_object_id";
        case followed_image_id: return "followed_image_id";
        case is_pinned_to_camera: return "is_pinned_to_camera";
        case is_following_object: return "is_following_object";
        case is_using_keyboard_to_move: return "is_using_keyboard_to_move";
        case is_using_keyboard_to_zoom: return "is_using_keyboard_to_zoom";
        case can_be_modified_by_mouse: return "can_be_modified_by_mouse";
        case can_move_objects: return "can_move_objects";
        case can_edit_text: return "can_edit_text";
        case grabbed: return "grabbed";
        case is_focused: return "is_focused";
        case draw_one_frame: return "draw_one_frame";
        case clear_bitmap: return "clear_bitmap";
        case set_active: return "set_active";
        case toggle: return "toggle";
        case set_relative_position: return "set_relative_position";
        case set_min_size: return "set_min_size";
        case set_zoom: return "set_zoom";
        case set_zoom_increase: return "set_zoom_increase";
        case set_min_zoom: return "set_min_zoom";
        case set_max_zoom: return "set_max_zoom";
        case bind_keys: return "bind_keys";
        case set_key_zoom_in: return "set_key_zoom_in";
        case set_key_zoom_out: return "set_key_zoom_out";
        case set_key_zoom_reset: return "set_key_zoom_reset";
        case pin_to_camera: return "pin_to_camera";
        case follow_layer: return "follow_layer";
        case follow_object: return "follow_object";
        case follow_image: return "follow_image";
        case set_is_pinned_to_camera: return "set_is_pinned_to_camera";
        case set_is_forcefully_pinned: return "set_is_forcefully_pinned";
        case activate_pin: return "activate_pin";
        case deactivate_pin: return "deactivate_pin";
        case toggle_pin: return "toggle_pin";
        case set_is_following_object: return "set_is_following_object";
        case set_can_move_with_keyboard: return "set_can_move_with_keyboard";
        case set_can_zoom_with_keyboard: return "set_can_zoom_with_keyboard";
        case set_is_vision_affected_by_mouse: return "set_is_vision_affected_by_mouse";
        case set_can_zoom_with_mouse: return "set_can_zoom_with_mouse";
        case set_can_be_modified_by_mouse: return "set_can_be_modified_by_mouse";
        case set_can_move_objects: return "set_can_move_objects";
        case set_can_edit_text: return "set_can_edit_text";
        case add_visible_layer: return "add_visible_layer";
        case add_accessible_layer: return "add_accessible_layer";
        case remove_visible_layer: return "remove_visible_layer";
        case remove_accessible_layer: return "remove_accessible_layer";
        case clear_visible_layers: return "clear_visible_layers";
        case clear_accessible_layers: return "clear_accessible_layers";
        case set_tint: return "set_tint";
        case set_drawing_borders: return "set_drawing_borders";
        case allow_drawing_object_borders: return "allow_drawing_object_borders";
        case forbid_drawing_object_borders: return "forbid_drawing_object_borders";
        case set_can_draw_object_borders: return "set_can_draw_object_borders";
        case minimize: return "minimize";
        case bring_forward: return "bring_forward";
        case set_can_be_grabbed: return "set_can_be_grabbed";
        case set_grabbing_area_position: return "set_grabbing_area_position";
        case set_grabbing_area_size: return "set_grabbing_area_size";
        case set_can_draw_on_camera: return "set_can_draw_on_camera";
        case set_can_clear_bitmap: return "set_can_clear_bitmap";
        case set_keep_inside_screen: return "set_keep_inside_screen";
        case set_can_mouse_resize: return "set_can_mouse_resize";
        case focus: return "focus";
        case unfocus: return "unfocus";
        case layer_a: return "layer";
        case object_a: return "object";
        case super_text_a: return "super_text";
        case super_editable_text_a: return "super_editable_text";
        case image_a: return "image";
        case movement_a: return "movement";
        case collision_a: return "collision";
        case particles_a: return "particles";
        case event_a: return "event";
        case variable_a: return "variable";
        case scrollbar_a: return "scrollbar";
        case primitives_a: return "primitives";
        case vector_a: return "vector";
        case context_a: return "context";
        case memory_address_a: return "memory_address";
        case literal_a: return "literal";
        case set_is_active: return "set_is_active";
        case add_group: return "add_group";
        case remove_group: return "remove_group";
        case move_in_drawing_order: return "move_in_drawing_order";
        case clear_groups: return "clear_groups";
        case disable_scrolling: return "disable_scrolling";
        case enable_scrolling: return "enable_scrolling";
        case set_is_scrollable: return "set_is_scrollable";
        case enable_selection_border: return "enable_selection_border";
        case disable_selection_border: return "disable_selection_border";
        case set_can_draw_selection_border: return "set_can_draw_selection_border";
        case set_can_be_moved_with_mouse: return "set_can_be_moved_with_mouse";
        case update_scrollbars: return "update_scrollbars";
        case allowed_jumps: return "allowed_jumps";
        case jumps_count: return "jumps_count";
        case jump_cooldown: return "jump_cooldown";
        case jump_cooldown_duration: return "jump_cooldown_duration";
        case can_jump: return "can_jump";
        case body_mass: return "body_mass";
        case walking_speed: return "walking_speed";
        case running_speed: return "running_speed";
        case jump_speed: return "jump_speed";
        case gravitation: return "gravitation";
        case base_friction: return "base_friction";
        case momentum_x: return "momentum_x";
        case momentum_y: return "momentum_y";
        case is_moving_up: return "is_moving_up";
        case is_moving_right: return "is_moving_right";
        case is_moving_down: return "is_moving_down";
        case is_moving_left: return "is_moving_left";
        case is_moving: return "is_moving";
        case is_still: return "is_still";
        case movement_type: return "movement_type";
        case input_type: return "input_type";
        case move_cooldown: return "move_cooldown";
        case move_cooldown_duration: return "move_cooldown_duration";
        case can_move_diagonally: return "can_move_diagonally";
        case reset_momentum_when_jumping: return "reset_momentum_when_jumping";
        case is_move_planned: return "is_move_planned";
        case max_momentum_x: return "max_momentum_x";
        case min_momentum_y: return "min_momentum_y";
        case max_momentum_y: return "max_momentum_y";
        case destination_x: return "destination_x";
        case destination_y: return "destination_y";
        case direction_of_move_x: return "direction_of_move_x";
        case direction_of_move_y: return "direction_of_move_y";
        case mouse_button: return "mouse_button";
        case move_on_mouse_release: return "move_on_mouse_release";
        case reset_direction_after_collision: return "reset_direction_after_collision";
        case is_jumping: return "is_jumping";
        case is_falling: return "is_falling";
        case is_running: return "is_running";
        case jump_key: return "jump_key";
        case running_key: return "running_key";
        case are_random_actions_enabled: return "are_random_actions_enabled";
        case time_when_action_persists: return "time_when_action_persists";
        case min_time_of_action: return "min_time_of_action";
        case max_time_of_action: return "max_time_of_action";
        case min_move_distance: return "min_move_distance";
        case max_move_distance: return "max_move_distance";
        case saved_action: return "saved_action";
        case saved_direction_x: return "saved_direction_x";
        case saved_direction_y: return "saved_direction_y";
        case chance_for_break: return "chance_for_break";
        case current_direction_x: return "current_direction_x";
        case current_direction_y: return "current_direction_y";
        case current_pos: return "current_pos";
        case loop_movement: return "loop_movement";
        case has_solid_hitbox: return "has_solid_hitbox";
        case is_fully_solid: return "is_fully_solid";
        case detected: return "detected";
        case with_object: return "with_object";
        case with_hitbox: return "with_hitbox";
        case in_group: return "in_group";
        case hitbox_is_solid: return "hitbox_is_solid";
        case hitbox_can_penetrate: return "hitbox_can_penetrate";
        case hitbox_ignores_object: return "hitbox_ignores_object";
        case hitbox_ignores_object_group: return "hitbox_ignores_object_group";
        case hitbox_ignores_hitbox: return "hitbox_ignores_hitbox";
        case hitbox_ignores_hitbox_group: return "hitbox_ignores_hitbox_group";
        case hitbox_detected: return "hitbox_detected";
        case hitbox_detected_hitbox: return "hitbox_detected_hitbox";
        case scale_x: return "scale_x";
        case scale_y: return "scale_y";
        case can_draw_selection_border: return "can_draw_selection_border";
        case visible: return "visible";
        case visible_specific: return "visible_specific";
        case pressed: return "pressed";
        case pressing: return "pressing";
        case released: return "released";
        case content: return "content";
        case current_text_id: return "current_text_id";
        case font_id: return "font_id";
        case color_r: return "color_r";
        case color_g: return "color_g";
        case color_b: return "color_b";
        case color_a: return "color";
        case wrapped: return "wrapped";
        case horizontal_align: return "horizontal_align";
        case vertical_align: return "vertical_align";
        case rotation: return "rotation";
        case text_color_r: return "text_color_r";
        case text_color_g: return "text_color_g";
        case text_color_b: return "text_color_b";
        case text_color_a: return "text_color";
        case can_be_edited: return "can_be_edited";
        case editing: return "editing";
        case protected_area: return "protected_area";
        case original_content: return "original_content";
        case can_use_space: return "can_use_space";
        case can_use_enter: return "can_use_enter";
        case enter_ends_editing: return "enter_ends_editing";
        case can_use_tabs: return "can_use_tabs";
        case is_numerical: return "is_numerical";
        case has_floating_point: return "has_floating_point";
        case ignore_vertical_arrows: return "ignore_vertical_arrows";
        case ignore_content_restriction: return "ignore_content_restriction";
        case is_storing_history: return "is_storing_history";
        case min_content_length: return "min_content_length";
        case max_content_length: return "max_content_length";
        case input_delay: return "input_delay";
        case repetition_delay: return "repetition_delay";
        case is_editing_active: return "is_editing_active";
        case cursor_pos: return "cursor_pos";
        case second_cursor_pos: return "second_cursor_pos";
        case cursor_pos_x: return "cursor_pos_x";
        case cursor_pos_y: return "cursor_pos_y";
        case min_cursor_pos: return "min_cursor_pos";
        case max_cursor_pos: return "max_cursor_pos";
        case wrapped_lines: return "wrapped_lines";
        case current_line: return "current_line";
        case scroll_shift_x: return "scroll_shift_x";
        case scroll_shift_y: return "scroll_shift_y";
        case thumb_pos_x: return "thumb_pos_x";
        case thumb_pos_y: return "thumb_pos_y";
        case thumb_size_x: return "thumb_size_x";
        case thumb_size_y: return "thumb_size_y";
        case scrolling_area_x: return "scrolling_area_x";
        case scrolling_area_y: return "scrolling_area_y";
        case real_scrolling_area_x: return "real_scrolling_area_x";
        case real_scrolling_area_y: return "real_scrolling_area_y";
        case drag_starting_pos_x: return "drag_starting_pos_x";
        case drag_starting_pos_y: return "drag_starting_pos_y";
        case can_be_drawn: return "can_be_drawn";
        case mouse_pressed_a: return "mouse_pressed";
        case mouse_wheel_speed: return "mouse_wheel_speed";
        case thumb_image_id: return "thumb_image_id";
        case track_image_id: return "track_image_id";
        case value: return "value";
        case size_a: return "size";
        case back_a: return "back";
        case index_a: return "index";
        case release_on: return "release_on";
        case is_selected: return "is_selected";
        case activate_a: return "activate";
        case scale_from_center: return "scale_from_center";
        case do_not_scale_from_center: return "do_not_scale_from_center";
        case set_scale_from_center: return "set_scale_from_center";
        case allow_selection: return "allow_selection";
        case forbid_selection: return "forbid_selection";
        case set_can_be_selected: return "set_can_be_selected";
        case is_scaled_from_center: return "is_scaled_from_center";
        case is_scrollable: return "is_scrollable";
        case can_be_selected: return "can_be_selected";
        case thickness: return "thickness";
        case environment_x: return "environment_x";
        case environment_y: return "environment_y";
        case environment_speed_x: return "environment_speed_x";
        case environment_speed_y: return "environment_speed_y";
        case min_speed: return "min_speed";
        case max_speed: return "max_speed";
        case min_basic_speed: return "min_basic_speed";
        case max_basic_speed: return "max_basic_speed";
        case min_acceleration: return "min_acceleration";
        case max_acceleration: return "max_acceleration";
        case min_particle_mass: return "min_particle_mass";
        case max_particle_mass: return "max_particle_mass";
        case min_direction_degree: return "min_direction_degree";
        case max_direction_degree: return "max_direction_degree";
        case min_rotation_speed: return "min_rotation_speed";
        case max_rotation_speed: return "max_rotation_speed";
        case min_time_to_negate_rotation: return "min_time_to_negate_rotation";
        case max_time_to_negate_rotation: return "max_time_to_negate_rotation";
        case are_particles_moving: return "are_particles_moving";
        case is_environment_synchronized: return "is_environment_synchronized";
        case min_particle_radius: return "min_particle_radius";
        case max_particle_radius: return "max_particle_radius";
        case min_time_to_death: return "min_time_to_death";
        case max_time_to_death: return "max_time_to_death";
        case min_shape_rotation_speed: return "min_shape_rotation_speed";
        case max_shape_rotation_speed: return "max_shape_rotation_speed";
        case min_color_intensity: return "min_color_intensity";
        case max_color_intensity: return "max_color_intensity";
        case particles_shape: return "particles_shape";
        case use_image_as_particles: return "use_image_as_particles";
        case use_random_colors: return "use_random_colors";
        case is_module_static: return "is_module_static";
        case is_drawing_with_details: return "is_drawing_with_details";
        case block_particles_spawn: return "block_particles_spawn";
        case can_particles_spawn: return "can_particles_spawn";
        case min_particles_per_spawn: return "min_particles_per_spawn";
        case max_particles_per_spawn: return "max_particles_per_spawn";
        case max_particles_count: return "max_particles_count";
        case time_to_spawn: return "time_to_spawn";
        case max_time_to_spawn: return "max_time_to_spawn";
        case spawn_key_bind: return "spawn_key_bind";
        default:
            cerr << "Error: In " << __FUNCTION__
                << ": AttributeType with code: '" << attribute << "' is undefined.\n"; 
            return "undefined";
    }
    return "null";
}
AttributeType strToAttribute(const string &attribute, string & error){
    //TODO: Use a hash map to map strings to enum values.
    if(attribute == "null") return null_a;
    else if(attribute == "set_id") return set_id;
    else if(attribute == "set_pos") return set_pos;
    else if(attribute == "set_size") return set_size;
    else if(attribute == "set_scale") return set_scale;
    else if(attribute == "add_scale") return add_scale;
    else if(attribute == "set_color") return set_color;
    else if(attribute == "set_color_r") return set_color_r;
    else if(attribute == "set_color_g") return set_color_g;
    else if(attribute == "set_color_b") return set_color_b;
    else if(attribute == "set_color") return set_color;
    else if(attribute == "set_random_color") return set_random_color;
    else if(attribute == "increment_random_color") return increment_random_color;
    else if(attribute == "set_rotation") return set_rotation;
    else if(attribute == "set_random_change_speed") return set_random_change_speed;
    else if(attribute == "set_min_color") return set_min_color;
    else if(attribute == "set_max_color") return set_max_color;
    else if(attribute == "rotate") return rotate;
    else if(attribute == "set_font") return set_font;
    else if(attribute == "add_new_content") return add_new_content;
    else if(attribute == "add_new_and_update") return add_new_and_update;
    else if(attribute == "update_size") return update_size;
    else if(attribute == "select_content") return select_content;
    else if(attribute == "delete") return delete_a;
    else if(attribute == "add_text") return add_text;
    else if(attribute == "set_text") return set_text;
    else if(attribute == "set_resize_text") return set_resize_text;
    else if(attribute == "set_wrapping") return set_wrapping;
    else if(attribute == "set_horizontal_align") return set_horizontal_align;
    else if(attribute == "set_vertical_align") return set_vertical_align;
    else if(attribute == "set_cursor_pos") return set_cursor_pos;
    else if(attribute == "set_second_cursor_pos") return set_second_cursor_pos;
    else if(attribute == "set_editable") return set_editable;
    else if(attribute == "set_space_use") return set_space_use;
    else if(attribute == "set_enter_use") return set_enter_use;
    else if(attribute == "set_can_enter_accept") return set_can_enter_accept;
    else if(attribute == "set_numerical") return set_numerical;
    else if(attribute == "set_floating_point") return set_floating_point;
    else if(attribute == "set_can_update_variable") return set_can_update_variable;
    else if(attribute == "set_auto_cleaning") return set_auto_cleaning;
    else if(attribute == "set_use_tabs") return set_use_tabs;
    else if(attribute == "set_min_content_length") return set_min_content_length;
    else if(attribute == "set_max_content_length") return set_max_content_length;
    else if(attribute == "set_input_delay") return set_input_delay;
    else if(attribute == "set_repetition_delay") return set_repetition_delay;
    else if(attribute == "connect_object") return connect_object;
    else if(attribute == "connect_group") return connect_group;
    else if(attribute == "connect_module") return connect_module;
    else if(attribute == "connect_module_id") return connect_module_id;
    else if(attribute == "connect_variable") return connect_variable;
    else if(attribute == "set_protected_area") return set_protected_area;
    else if(attribute == "cut_unprotected_area") return cut_unprotected_area;
    else if(attribute == "stop_editing") return stop_editing;
    else if(attribute == "start_editing") return start_editing;
    else if(attribute == "set_ignore_vertical_arrows") return set_ignore_vertical_arrows;
    else if(attribute == "set_can_auto_resize_axis_y") return set_can_auto_resize_axis_y;
    else if(attribute == "update") return update;
    else if(attribute == "crop_size_to_text") return crop_size_to_text;
    else if(attribute == "crop_width_to_text") return crop_width_to_text;
    else if(attribute == "crop_height_to_text") return crop_height_to_text;
    else if(attribute == "set_content") return set_content;
    else if(attribute == "add_content") return add_content;
    else if(attribute == "add_new_text_line") return add_new_text_line;
    else if(attribute == "set_text_line") return set_text_line;
    else if(attribute == "add_to_text_line") return add_to_text_line;
    else if(attribute == "save_to_file") return save_to_file;
    else if(attribute == "load_from_file") return load_from_file;
    else if(attribute == "add_format") return add_format;
    else if(attribute == "modify_format") return modify_format;
    else if(attribute == "modify_last_format") return modify_last_format;
    else if(attribute == "inject_format") return inject_format;
    else if(attribute == "delete_format") return delete_format;
    else if(attribute == "set_last_color") return set_last_color;
    else if(attribute == "set_accent_color") return set_accent_color;
    else if(attribute == "set_last_accent_color") return set_last_accent_color;
    else if(attribute == "set_background_color") return set_background_color;
    else if(attribute == "set_last_background_color") return set_last_background_color;
    else if(attribute == "set_background_color_drawing") return set_background_color_drawing;
    else if(attribute == "set_last_background_color_drawing") return set_last_background_color_drawing;
    else if(attribute == "set_offset") return set_offset;
    else if(attribute == "add_offset") return add_offset;
    else if(attribute == "set_offset_x") return set_offset_x;
    else if(attribute == "add_offset_x") return add_offset_x;
    else if(attribute == "set_offset_y") return set_offset_y;
    else if(attribute == "add_offset_y") return add_offset_y;
    else if(attribute == "set_selected") return set_selected;
    else if(attribute == "set_limit") return set_limit;
    else if(attribute == "set_padding_between_lines") return set_padding_between_lines;
    else if(attribute == "add_rotation") return add_rotation;
    else if(attribute == "set_tab_length") return set_tab_length;
    else if(attribute == "set_draw_selection_first") return set_draw_selection_first;
    else if(attribute == "set_can_be_edited") return set_can_be_edited;
    else if(attribute == "set_can_use_space") return set_can_use_space;
    else if(attribute == "set_can_use_enter") return set_can_use_enter;
    else if(attribute == "set_can_use_tabs") return set_can_use_tabs;
    else if(attribute == "set_is_numerical") return set_is_numerical;
    else if(attribute == "set_has_floating_point") return set_has_floating_point;
    else if(attribute == "set_ignore_content_restriction") return set_ignore_content_restriction;
    else if(attribute == "set_is_storing_history") return set_is_storing_history;
    else if(attribute == "set_can_copy_format") return set_can_copy_format;
    else if(attribute == "resize") return resize;
    else if(attribute == "set_mirror") return set_mirror;
    else if(attribute == "set_light") return set_light;
    else if(attribute == "connect_bitmap") return connect_bitmap;
    else if(attribute == "connect_bitmap_via_path") return connect_bitmap_via_path;
    else if(attribute == "connect_bitmap_via_alias") return connect_bitmap_via_alias;
    else if(attribute == "rot_pos_x") return rot_pos_x;
    else if(attribute == "rot_pos_y") return rot_pos_y;
    else if(attribute == "start_x") return start_x;
    else if(attribute == "start_y") return start_y;
    else if(attribute == "frame_size_x") return frame_size_x;
    else if(attribute == "frame_size_y") return frame_size_y;
    else if(attribute == "rotate_angle") return rotate_angle;
    else if(attribute == "mirror_x") return mirror_x;
    else if(attribute == "mirror_y") return mirror_y;
    else if(attribute == "image_color_r") return image_color_r;
    else if(attribute == "image_color_g") return image_color_g;
    else if(attribute == "image_color_b") return image_color_b;
    else if(attribute == "image_color_a") return image_color_a;
    else if(attribute == "light_level") return light_level;
    else if(attribute == "light_color_r") return light_color_r;
    else if(attribute == "light_color_g") return light_color_g;
    else if(attribute == "light_color_b") return light_color_b;
    else if(attribute == "move") return move_a;
    else if(attribute == "move_up") return move_up;
    else if(attribute == "move_right") return move_right;
    else if(attribute == "move_down") return move_down;
    else if(attribute == "move_left") return move_left;
    else if(attribute == "move_jump") return move_jump;
    else if(attribute == "move_fall") return move_fall;
    else if(attribute == "move_run") return move_run;
    else if(attribute == "set_movement_type") return set_movement_type;
    else if(attribute == "set_input_type") return set_input_type;
    else if(attribute == "set_allowed_jumps") return set_allowed_jumps;
    else if(attribute == "set_jump_cooldown") return set_jump_cooldown;
    else if(attribute == "set_move_cooldown") return set_move_cooldown;
    else if(attribute == "set_diagonal_movement") return set_diagonal_movement;
    else if(attribute == "set_reset_momentum_when_jumping") return set_reset_momentum_when_jumping;
    else if(attribute == "set_mass") return set_mass;
    else if(attribute == "set_walking_speed") return set_walking_speed;
    else if(attribute == "set_running_speed") return set_running_speed;
    else if(attribute == "set_jump_speed") return set_jump_speed;
    else if(attribute == "set_gravity") return set_gravity;
    else if(attribute == "set_friction") return set_friction;
    else if(attribute == "set_max_momentum_x") return set_max_momentum_x;
    else if(attribute == "set_momentum_y") return set_momentum_y;
    else if(attribute == "set_min_momentum_y") return set_min_momentum_y;
    else if(attribute == "set_max_momentum_y") return set_max_momentum_y;
    else if(attribute == "set_mouse_movement_button") return set_mouse_movement_button;
    else if(attribute == "set_move_on_mouse_release") return set_move_on_mouse_release;
    else if(attribute == "set_direction_reset_after_collision") return set_direction_reset_after_collision;
    else if(attribute == "set_moving_keys") return set_moving_keys;
    else if(attribute == "set_key_up") return set_key_up;
    else if(attribute == "set_key_right") return set_key_right;
    else if(attribute == "set_key_down") return set_key_down;
    else if(attribute == "set_key_left") return set_key_left;
    else if(attribute == "set_key_jump") return set_key_jump;
    else if(attribute == "set_key_running") return set_key_running;
    else if(attribute == "set_random_actions") return set_random_actions;
    else if(attribute == "set_action_time") return set_action_time;
    else if(attribute == "set_min_action_time") return set_min_action_time;
    else if(attribute == "set_max_action_time") return set_max_action_time;
    else if(attribute == "set_move_distance") return set_move_distance;
    else if(attribute == "set_min_move_distance") return set_min_move_distance;
    else if(attribute == "set_max_move_distance") return set_max_move_distance;
    else if(attribute == "set_breaks_allowed") return set_breaks_allowed;
    else if(attribute == "set_break_chance") return set_break_chance;
    else if(attribute == "set_solid") return set_solid;
    else if(attribute == "toggle_solid") return toggle_solid;
    else if(attribute == "set_penetration") return set_penetration;
    else if(attribute == "toggle_penetration") return toggle_penetration;
    else if(attribute == "ignore") return ignore;
    else if(attribute == "stop_ignoring") return stop_ignoring;
    else if(attribute == "clear_ignoring") return clear_ignoring;
    else if(attribute == "ignore_object") return ignore_object;
    else if(attribute == "ignore_group_of_objects") return ignore_group_of_objects;
    else if(attribute == "ignore_hitbox") return ignore_hitbox;
    else if(attribute == "ignore_group_of_hitboxes") return ignore_group_of_hitboxes;
    else if(attribute == "set_is_circle") return set_is_circle;
    else if(attribute == "set_environment") return set_environment;
    else if(attribute == "set_environment_speed") return set_environment_speed;
    else if(attribute == "set_speed") return set_speed;
    else if(attribute == "set_basic_speed") return set_basic_speed;
    else if(attribute == "set_acceleration") return set_acceleration;
    else if(attribute == "set_directional_degree") return set_directional_degree;
    else if(attribute == "set_rotation_speed") return set_rotation_speed;
    else if(attribute == "set_time_to_negate_rotation") return set_time_to_negate_rotation;
    else if(attribute == "set_are_particles_moving") return set_are_particles_moving;
    else if(attribute == "allow_movement") return allow_movement;
    else if(attribute == "set_is_env_sync") return set_is_env_sync;
    else if(attribute == "set_radius") return set_radius;
    else if(attribute == "set_line_join_type") return set_line_join_type;
    else if(attribute == "set_time_to_death") return set_time_to_death;
    else if(attribute == "set_shape_rotation_speed") return set_shape_rotation_speed;
    else if(attribute == "set_color_intensity") return set_color_intensity;
    else if(attribute == "add_color_interval") return add_color_interval;
    else if(attribute == "add_color_interval_in_hex") return add_color_interval_in_hex;
    else if(attribute == "remove_color_interval") return remove_color_interval;
    else if(attribute == "add_color") return add_color;
    else if(attribute == "add_color_in_hex") return add_color_in_hex;
    else if(attribute == "remove_color") return remove_color;
    else if(attribute == "add_image") return add_image;
    else if(attribute == "remove_image") return remove_image;
    else if(attribute == "remove_image_by_index") return remove_image_by_index;
    else if(attribute == "set_shape") return set_shape;
    else if(attribute == "set_use_images") return set_use_images;
    else if(attribute == "toggle_source") return toggle_source;
    else if(attribute == "set_use_random_colors") return set_use_random_colors;
    else if(attribute == "toggle_random_colors") return toggle_random_colors;
    else if(attribute == "set_is_static") return set_is_static;
    else if(attribute == "set_drawing_details") return set_drawing_details;
    else if(attribute == "set_block_spawning") return set_block_spawning;
    else if(attribute == "set_spawning") return set_spawning;
    else if(attribute == "allow_spawning") return allow_spawning;
    else if(attribute == "set_spawn_rate") return set_spawn_rate;
    else if(attribute == "set_spawn_cap") return set_spawn_cap;
    else if(attribute == "set_max_time_to_spawn") return set_max_time_to_spawn;
    else if(attribute == "bind_key") return bind_key;
    else if(attribute == "bind_key_by_int") return bind_key_by_int;
    else if(attribute == "spawn_on_key_release") return spawn_on_key_release;
    else if(attribute == "toggle_bool") return toggle_bool;
    else if(attribute == "set_bool_random") return set_bool_random;
    else if(attribute == "set_bool_probability") return set_bool_probability;
    else if(attribute == "set_bool") return set_bool;
    else if(attribute == "set_int") return set_int;
    else if(attribute == "set_int_interval") return set_int_interval;
    else if(attribute == "set_int_random") return set_int_random;
    else if(attribute == "set_double") return set_double;
    else if(attribute == "set_double_interval") return set_double_interval;
    else if(attribute == "set_double_random") return set_double_random;
    else if(attribute == "set_string") return set_string;
    else if(attribute == "set_string_random") return set_string_random;
    else if(attribute == "add_int") return add_int;
    else if(attribute == "add_int_interval") return add_int_interval;
    else if(attribute == "add_int_random") return add_int_random;
    else if(attribute == "add_double") return add_double;
    else if(attribute == "add_double_interval") return add_double_interval;
    else if(attribute == "add_double_random") return add_double_random;
    else if(attribute == "set_thumb_position") return set_thumb_position;
    else if(attribute == "set_thumb_size") return set_thumb_size;
    else if(attribute == "set_scrolling_area") return set_scrolling_area;
    else if(attribute == "set_real_scrolling_area") return set_real_scrolling_area;
    else if(attribute == "add_real_scrolling_area") return add_real_scrolling_area;
    else if(attribute == "set_thumb_image_id") return set_thumb_image_id;
    else if(attribute == "set_track_image_id") return set_track_image_id;
    else if(attribute == "set_can_be_drawn") return set_can_be_drawn;
    else if(attribute == "set_mouse_wheel_speed") return set_mouse_wheel_speed;
    else if(attribute == "scroll_to_the_beginning") return scroll_to_the_beginning;
    else if(attribute == "scroll_to_the_end") return scroll_to_the_end;
    else if(attribute == "scroll_by_distance") return scroll_by_distance;
    else if(attribute == "add_camera") return add_camera;
    else if(attribute == "remove_camera") return remove_camera;
    else if(attribute == "clear_cameras") return clear_cameras;
    else if(attribute == "update_with_size") return update_with_size;
    else if(attribute == "set_type") return set_type;
    else if(attribute == "add_point") return add_point;
    else if(attribute == "remove_point") return remove_point;
    else if(attribute == "set_point") return set_point;
    else if(attribute == "move_point") return move_point;
    else if(attribute == "set_thickness") return set_thickness;
    else if(attribute == "create") return create;
    else if(attribute == "push_back") return push_back_a;
    else if(attribute == "pop_back") return pop_back_a;
    else if(attribute == "clear") return clear_a;
    else if(attribute == "remove") return remove_a;
    else if(attribute == "set") return set_a;
    else if(attribute == "else_child_id") return else_child_id;
    else if(attribute == "group") return group;
    else if(attribute == "layer_id") return layer_id;
    else if(attribute == "number_of_cameras") return number_of_cameras_a;
    else if(attribute == "number_of_layers") return number_of_layers_a;
    else if(attribute == "number_of_objects") return number_of_objects_a;
    else if(attribute == "bool") return bool_a;
    else if(attribute == "int") return int_a;
    else if(attribute == "double") return double_a;
    else if(attribute == "string") return string_a;
    else if(attribute == "camera") return camera_a;
    else if(attribute == "is_active") return is_active;
    else if(attribute == "id") return id;
    else if(attribute == "pos_x") return pos_x;
    else if(attribute == "pos_y") return pos_y;
    else if(attribute == "relative_pos_x") return relative_pos_x;
    else if(attribute == "relative_pos_y") return relative_pos_y;
    else if(attribute == "vision_shift_x") return vision_shift_x;
    else if(attribute == "vision_shift_y") return vision_shift_y;
    else if(attribute == "size_x") return size_x;
    else if(attribute == "size_y") return size_y;
    else if(attribute == "zoom") return zoom;
    else if(attribute == "min_zoom") return min_zoom;
    else if(attribute == "max_zoom") return max_zoom;
    else if(attribute == "speed") return speed;
    else if(attribute == "zoom_in_key") return zoom_in_key;
    else if(attribute == "zoom_out_key") return zoom_out_key;
    else if(attribute == "zoom_reset_key") return zoom_reset_key;
    else if(attribute == "up_key") return up_key;
    else if(attribute == "right_key") return right_key;
    else if(attribute == "down_key") return down_key;
    else if(attribute == "left_key") return left_key;
    else if(attribute == "pinned_camera_id") return pinned_camera_id;
    else if(attribute == "followed_layer_id") return followed_layer_id;
    else if(attribute == "followed_object_id") return followed_object_id;
    else if(attribute == "followed_image_id") return followed_image_id;
    else if(attribute == "is_pinned_to_camera") return is_pinned_to_camera;
    else if(attribute == "is_following_object") return is_following_object;
    else if(attribute == "is_using_keyboard_to_move") return is_using_keyboard_to_move;
    else if(attribute == "is_using_keyboard_to_zoom") return is_using_keyboard_to_zoom;
    else if(attribute == "can_be_modified_by_mouse") return can_be_modified_by_mouse;
    else if(attribute == "can_move_objects") return can_move_objects;
    else if(attribute == "can_edit_text") return can_edit_text;
    else if(attribute == "grabbed") return grabbed;
    else if(attribute == "is_focused") return is_focused;
    else if(attribute == "draw_one_frame") return draw_one_frame;
    else if(attribute == "clear_bitmap") return clear_bitmap;
    else if(attribute == "set_active") return set_active;
    else if(attribute == "activate") return activate_a;
    else if(attribute == "deactivate") return deactivate_a;
    else if(attribute == "toggle") return toggle;
    else if(attribute == "set_relative_position") return set_relative_position;
    else if(attribute == "set_min_size") return set_min_size;
    else if(attribute == "set_zoom") return set_zoom;
    else if(attribute == "set_zoom_increase") return set_zoom_increase;
    else if(attribute == "set_min_zoom") return set_min_zoom;
    else if(attribute == "set_max_zoom") return set_max_zoom;
    else if(attribute == "bind_keys") return bind_keys;
    else if(attribute == "set_key_zoom_in") return set_key_zoom_in;
    else if(attribute == "set_key_zoom_out") return set_key_zoom_out;
    else if(attribute == "set_key_zoom_reset") return set_key_zoom_reset;
    else if(attribute == "pin_to_camera") return pin_to_camera;
    else if(attribute == "follow_layer") return follow_layer;
    else if(attribute == "follow_object") return follow_object;
    else if(attribute == "follow_image") return follow_image;
    else if(attribute == "set_is_pinned_to_camera") return set_is_pinned_to_camera;
    else if(attribute == "set_is_forcefully_pinned") return set_is_forcefully_pinned;
    else if(attribute == "activate_pin") return activate_pin;
    else if(attribute == "deactivate_pin") return deactivate_pin;
    else if(attribute == "toggle_pin") return toggle_pin;
    else if(attribute == "set_is_following_object") return set_is_following_object;
    else if(attribute == "set_can_move_with_keyboard") return set_can_move_with_keyboard;
    else if(attribute == "set_can_zoom_with_keyboard") return set_can_zoom_with_keyboard;
    else if(attribute == "set_is_vision_affected_by_mouse") return set_is_vision_affected_by_mouse;
    else if(attribute == "set_can_zoom_with_mouse") return set_can_zoom_with_mouse;
    else if(attribute == "set_can_be_modified_by_mouse") return set_can_be_modified_by_mouse;
    else if(attribute == "set_can_move_objects") return set_can_move_objects;
    else if(attribute == "set_can_edit_text") return set_can_edit_text;
    else if(attribute == "add_visible_layer") return add_visible_layer;
    else if(attribute == "add_accessible_layer") return add_accessible_layer;
    else if(attribute == "remove_visible_layer") return remove_visible_layer;
    else if(attribute == "remove_accessible_layer") return remove_accessible_layer;
    else if(attribute == "clear_visible_layers") return clear_visible_layers;
    else if(attribute == "clear_accessible_layers") return clear_accessible_layers;
    else if(attribute == "set_tint") return set_tint;
    else if(attribute == "set_drawing_borders") return set_drawing_borders;
    else if(attribute == "allow_drawing_object_borders") return allow_drawing_object_borders;
    else if(attribute == "forbid_drawing_object_borders") return forbid_drawing_object_borders;
    else if(attribute == "set_can_draw_object_borders") return set_can_draw_object_borders;
    else if(attribute == "minimize") return minimize;
    else if(attribute == "bring_forward") return bring_forward;
    else if(attribute == "set_can_be_grabbed") return set_can_be_grabbed;
    else if(attribute == "set_grabbing_area_position") return set_grabbing_area_position;
    else if(attribute == "set_grabbing_area_size") return set_grabbing_area_size;
    else if(attribute == "set_can_draw_on_camera") return set_can_draw_on_camera;
    else if(attribute == "set_can_clear_bitmap") return set_can_clear_bitmap;
    else if(attribute == "set_keep_inside_screen") return set_keep_inside_screen;
    else if(attribute == "set_can_mouse_resize") return set_can_mouse_resize;
    else if(attribute == "focus") return focus;
    else if(attribute == "unfocus") return unfocus;
    else if(attribute == "layer") return layer_a;
    else if(attribute == "object") return object_a;
    else if(attribute == "super_text") return super_text_a;
    else if(attribute == "super_editable_text") return super_editable_text_a;
    else if(attribute == "image") return image_a;
    else if(attribute == "movement") return movement_a;
    else if(attribute == "collision") return collision_a;
    else if(attribute == "particles") return particles_a;
    else if(attribute == "event") return event_a;
    else if(attribute == "variable") return variable_a;
    else if(attribute == "scrollbar") return scrollbar_a;
    else if(attribute == "primitives") return primitives_a;
    else if(attribute == "vector") return vector_a;
    else if(attribute == "context") return context_a;
    else if(attribute == "memory_address") return memory_address_a;
    else if(attribute == "literal") return literal_a;
    else if(attribute == "set_is_active") return set_is_active;
    else if(attribute == "add_group") return add_group;
    else if(attribute == "remove_group") return remove_group;
    else if(attribute == "move_in_drawing_order") return move_in_drawing_order;
    else if(attribute == "clear_groups") return clear_groups;
    else if(attribute == "disable_scrolling") return disable_scrolling;
    else if(attribute == "enable_scrolling") return enable_scrolling;
    else if(attribute == "set_is_scrollable") return set_is_scrollable;
    else if(attribute == "enable_selection_border") return enable_selection_border;
    else if(attribute == "disable_selection_border") return disable_selection_border;
    else if(attribute == "set_can_draw_selection_border") return set_can_draw_selection_border;
    else if(attribute == "set_can_be_moved_with_mouse") return set_can_be_moved_with_mouse;
    else if(attribute == "update_scrollbars") return update_scrollbars;
    else if(attribute == "allowed_jumps") return allowed_jumps;
    else if(attribute == "jumps_count") return jumps_count;
    else if(attribute == "jump_cooldown") return jump_cooldown;
    else if(attribute == "jump_cooldown_duration") return jump_cooldown_duration;
    else if(attribute == "can_jump") return can_jump;
    else if(attribute == "body_mass") return body_mass;
    else if(attribute == "walking_speed") return walking_speed;
    else if(attribute == "running_speed") return running_speed;
    else if(attribute == "jump_speed") return jump_speed;
    else if(attribute == "gravitation") return gravitation;
    else if(attribute == "base_friction") return base_friction;
    else if(attribute == "momentum_x") return momentum_x;
    else if(attribute == "momentum_y") return momentum_y;
    else if(attribute == "is_moving_up") return is_moving_up;
    else if(attribute == "is_moving_right") return is_moving_right;
    else if(attribute == "is_moving_down") return is_moving_down;
    else if(attribute == "is_moving_left") return is_moving_left;
    else if(attribute == "is_moving") return is_moving;
    else if(attribute == "is_still") return is_still;
    else if(attribute == "movement_type") return movement_type;
    else if(attribute == "input_type") return input_type;
    else if(attribute == "move_cooldown") return move_cooldown;
    else if(attribute == "move_cooldown_duration") return move_cooldown_duration;
    else if(attribute == "can_move_diagonally") return can_move_diagonally;
    else if(attribute == "reset_momentum_when_jumping") return reset_momentum_when_jumping;
    else if(attribute == "is_move_planned") return is_move_planned;
    else if(attribute == "max_momentum_x") return max_momentum_x;
    else if(attribute == "min_momentum_y") return min_momentum_y;
    else if(attribute == "max_momentum_y") return max_momentum_y;
    else if(attribute == "destination_x") return destination_x;
    else if(attribute == "destination_y") return destination_y;
    else if(attribute == "direction_of_move_x") return direction_of_move_x;
    else if(attribute == "direction_of_move_y") return direction_of_move_y;
    else if(attribute == "mouse_button") return mouse_button;
    else if(attribute == "move_on_mouse_release") return move_on_mouse_release;
    else if(attribute == "reset_direction_after_collision") return reset_direction_after_collision;
    else if(attribute == "is_jumping") return is_jumping;
    else if(attribute == "is_falling") return is_falling;
    else if(attribute == "is_running") return is_running;
    else if(attribute == "jump_key") return jump_key;
    else if(attribute == "running_key") return running_key;
    else if(attribute == "are_random_actions_enabled") return are_random_actions_enabled;
    else if(attribute == "time_when_action_persists") return time_when_action_persists;
    else if(attribute == "min_time_of_action") return min_time_of_action;
    else if(attribute == "max_time_of_action") return max_time_of_action;
    else if(attribute == "min_move_distance") return min_move_distance;
    else if(attribute == "max_move_distance") return max_move_distance;
    else if(attribute == "saved_action") return saved_action;
    else if(attribute == "saved_direction_x") return saved_direction_x;
    else if(attribute == "saved_direction_y") return saved_direction_y;
    else if(attribute == "chance_for_break") return chance_for_break;
    else if(attribute == "current_direction_x") return current_direction_x;
    else if(attribute == "current_direction_y") return current_direction_y;
    else if(attribute == "current_pos") return current_pos;
    else if(attribute == "loop_movement") return loop_movement;
    else if(attribute == "has_solid_hitbox") return has_solid_hitbox;
    else if(attribute == "is_fully_solid") return is_fully_solid;
    else if(attribute == "detected") return detected;
    else if(attribute == "with_object") return with_object;
    else if(attribute == "with_hitbox") return with_hitbox;
    else if(attribute == "in_group") return in_group;
    else if(attribute == "hitbox_is_solid") return hitbox_is_solid;
    else if(attribute == "hitbox_can_penetrate") return hitbox_can_penetrate;
    else if(attribute == "hitbox_ignores_object") return hitbox_ignores_object;
    else if(attribute == "hitbox_ignores_object_group") return hitbox_ignores_object_group;
    else if(attribute == "hitbox_ignores_hitbox") return hitbox_ignores_hitbox;
    else if(attribute == "hitbox_ignores_hitbox_group") return hitbox_ignores_hitbox_group;
    else if(attribute == "hitbox_detected") return hitbox_detected;
    else if(attribute == "hitbox_detected_hitbox") return hitbox_detected_hitbox;
    else if(attribute == "is_solid") return is_solid;
    else if(attribute == "can_penetrate_solids") return can_penetrate_solids;
    else if(attribute == "scale_x") return scale_x;
    else if(attribute == "scale_y") return scale_y;
    else if(attribute == "can_draw_selection_border") return can_draw_selection_border;
    else if(attribute == "visible") return visible;
    else if(attribute == "visible_specific") return visible_specific;
    else if(attribute == "pressed") return pressed;
    else if(attribute == "pressing") return pressing;
    else if(attribute == "released") return released;
    else if(attribute == "content") return content;
    else if(attribute == "current_text_id") return current_text_id;
    else if(attribute == "font_id") return font_id;
    else if(attribute == "color_r") return color_r;
    else if(attribute == "color_g") return color_g;
    else if(attribute == "color_b") return color_b;
    else if(attribute == "color") return color_a;
    else if(attribute == "wrapped") return wrapped;
    else if(attribute == "horizontal_align") return horizontal_align;
    else if(attribute == "vertical_align") return vertical_align;
    else if(attribute == "rotation") return rotation;
    else if(attribute == "text_color_r") return text_color_r;
    else if(attribute == "text_color_g") return text_color_g;
    else if(attribute == "text_color_b") return text_color_b;
    else if(attribute == "text_color") return text_color_a;
    else if(attribute == "can_be_edited") return can_be_edited;
    else if(attribute == "editing") return editing;
    else if(attribute == "protected_area") return protected_area;
    else if(attribute == "original_content") return original_content;
    else if(attribute == "can_use_space") return can_use_space;
    else if(attribute == "can_use_enter") return can_use_enter;
    else if(attribute == "enter_ends_editing") return enter_ends_editing;
    else if(attribute == "can_use_tabs") return can_use_tabs;
    else if(attribute == "is_numerical") return is_numerical;
    else if(attribute == "has_floating_point") return has_floating_point;
    else if(attribute == "ignore_vertical_arrows") return ignore_vertical_arrows;
    else if(attribute == "ignore_content_restriction") return ignore_content_restriction;
    else if(attribute == "is_storing_history") return is_storing_history;
    else if(attribute == "min_content_length") return min_content_length;
    else if(attribute == "max_content_length") return max_content_length;
    else if(attribute == "input_delay") return input_delay;
    else if(attribute == "repetition_delay") return repetition_delay;
    else if(attribute == "is_editing_active") return is_editing_active;
    else if(attribute == "cursor_pos") return cursor_pos;
    else if(attribute == "second_cursor_pos") return second_cursor_pos;
    else if(attribute == "cursor_pos_x") return cursor_pos_x;
    else if(attribute == "cursor_pos_y") return cursor_pos_y;
    else if(attribute == "min_cursor_pos") return min_cursor_pos;
    else if(attribute == "max_cursor_pos") return max_cursor_pos;
    else if(attribute == "wrapped_lines") return wrapped_lines;
    else if(attribute == "current_line") return current_line;
    else if(attribute == "scroll_shift_x") return scroll_shift_x;
    else if(attribute == "scroll_shift_y") return scroll_shift_y;
    else if(attribute == "thumb_pos_x") return thumb_pos_x;
    else if(attribute == "thumb_pos_y") return thumb_pos_y;
    else if(attribute == "thumb_size_x") return thumb_size_x;
    else if(attribute == "thumb_size_y") return thumb_size_y;
    else if(attribute == "scrolling_area_x") return scrolling_area_x;
    else if(attribute == "scrolling_area_y") return scrolling_area_y;
    else if(attribute == "real_scrolling_area_x") return real_scrolling_area_x;
    else if(attribute == "real_scrolling_area_y") return real_scrolling_area_y;
    else if(attribute == "drag_starting_pos_x") return drag_starting_pos_x;
    else if(attribute == "drag_starting_pos_y") return drag_starting_pos_y;
    else if(attribute == "can_be_drawn") return can_be_drawn;
    else if(attribute == "mouse_pressed") return mouse_pressed_a;
    else if(attribute == "mouse_wheel_speed") return mouse_wheel_speed;
    else if(attribute == "thumb_image_id") return thumb_image_id;
    else if(attribute == "track_image_id") return track_image_id;
    else if(attribute == "value") return value;
    else if(attribute == "size") return size_a;
    else if(attribute == "back") return back_a;
    else if(attribute == "index") return index_a;
    else if(attribute == "type") return type_a;
    else if(attribute == "last_bool") return last_bool;
    else if(attribute == "last_int") return last_int;
    else if(attribute == "last_double") return last_double;
    else if(attribute == "last_string") return last_string;
    else if(attribute == "first_bool") return first_bool;
    else if(attribute == "first_int") return first_int;
    else if(attribute == "first_double") return first_double;
    else if(attribute == "first_string") return first_string;
    else if(attribute == "random_bool") return random_bool;
    else if(attribute == "random_int") return random_int;
    else if(attribute == "random_double") return random_double;
    else if(attribute == "random_string") return random_string;
    else if(attribute == "release_on") return release_on;
    else if(attribute == "is_selected") return is_selected;
    else if(attribute == "scale_from_center") return scale_from_center;
    else if(attribute == "do_not_scale_from_center") return do_not_scale_from_center;
    else if(attribute == "set_scale_from_center") return set_scale_from_center;
    else if(attribute == "allow_selection") return allow_selection;
    else if(attribute == "forbid_selection") return forbid_selection;
    else if(attribute == "set_can_be_selected") return set_can_be_selected;
    else if(attribute == "is_scaled_from_center") return is_scaled_from_center;
    else if(attribute == "is_scrollable") return is_scrollable;
    else if(attribute == "can_be_selected") return can_be_selected;
    else if(attribute == "thickness") return thickness;
    else if(attribute == "environment_x") return environment_x;
    else if(attribute == "environment_y") return environment_y;
    else if(attribute == "environment_speed_x") return environment_speed_x;
    else if(attribute == "environment_speed_y") return environment_speed_y;
    else if(attribute == "min_speed") return min_speed;
    else if(attribute == "max_speed") return max_speed;
    else if(attribute == "min_basic_speed") return min_basic_speed;
    else if(attribute == "max_basic_speed") return max_basic_speed;
    else if(attribute == "min_acceleration") return min_acceleration;
    else if(attribute == "max_acceleration") return max_acceleration;
    else if(attribute == "min_particle_mass") return min_particle_mass;
    else if(attribute == "max_particle_mass") return max_particle_mass;
    else if(attribute == "min_direction_degree") return min_direction_degree;
    else if(attribute == "max_direction_degree") return max_direction_degree;
    else if(attribute == "min_rotation_speed") return min_rotation_speed;
    else if(attribute == "max_rotation_speed") return max_rotation_speed;
    else if(attribute == "min_time_to_negate_rotation") return min_time_to_negate_rotation;
    else if(attribute == "max_time_to_negate_rotation") return max_time_to_negate_rotation;
    else if(attribute == "are_particles_moving") return are_particles_moving;
    else if(attribute == "is_environment_synchronized") return is_environment_synchronized;
    else if(attribute == "min_particle_radius") return min_particle_radius;
    else if(attribute == "max_particle_radius") return max_particle_radius;
    else if(attribute == "min_time_to_death") return min_time_to_death;
    else if(attribute == "max_time_to_death") return max_time_to_death;
    else if(attribute == "min_shape_rotation_speed") return min_shape_rotation_speed;
    else if(attribute == "max_shape_rotation_speed") return max_shape_rotation_speed;
    else if(attribute == "min_color_intensity") return min_color_intensity;
    else if(attribute == "max_color_intensity") return max_color_intensity;
    else if(attribute == "particles_shape") return particles_shape;
    else if(attribute == "use_image_as_particles") return use_image_as_particles;
    else if(attribute == "use_random_colors") return use_random_colors;
    else if(attribute == "is_module_static") return is_module_static;
    else if(attribute == "is_drawing_with_details") return is_drawing_with_details;
    else if(attribute == "block_particles_spawn") return block_particles_spawn;
    else if(attribute == "can_particles_spawn") return can_particles_spawn;
    else if(attribute == "min_particles_per_spawn") return min_particles_per_spawn;
    else if(attribute == "max_particles_per_spawn") return max_particles_per_spawn;
    else if(attribute == "max_particles_count") return max_particles_count;
    else if(attribute == "time_to_spawn") return time_to_spawn;
    else if(attribute == "max_time_to_spawn") return max_time_to_spawn;
    else if(attribute == "spawn_key_bind") return spawn_key_bind;
    else if(attribute == "window_title") return window_title_a;
    else if(attribute == "display_size") return display_size_a;
    else if(attribute == "fullscreen") return fullscreen_a;
    else if(attribute == "pixel_art") return pixel_art_a;
    else if(attribute == "can_afk_timeout") return can_afk_timeout_a;
    else if(attribute == "afk_timeout_time") return afk_timeout_time_a;
    else if(attribute == "can_exit_on_idle") return can_exit_on_idle_a;
    error = "Invalid attribute: '"  + attribute + "'.\n";
    return null_a;
}
AttributeType strToAttribute(const string &attribute){
    string dummyString;
    return strToAttribute(attribute, dummyString);
}
