#include "eventModule.h"

ConditionClass::ConditionClass(unsigned int newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass(string newID) : Literal(newID, nullptr, "", ""){}
ConditionClass::ConditionClass() : Literal(){}

OperaClass::OperaClass(){
    instruction = EngineInstr::null;
}

void EveModule::clone(const EveModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
}

void EveModule::setUpNewInstance(){
    conditionalStatus = 'n';
    elseChildID = "";
    areDependentOperationsDone = false;
    elseChildFinished = false;
    loop = false;
    willBeDeleted = false;
}
EveModule::EveModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
EveModule::EveModule(unsigned int eventModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(eventModuleID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
EveModule::EveModule(string eventModuleID, vector<string> *listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(eventModuleID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
EveModule::~EveModule(){

}
void EveModule::clear(){
    ConditionalChain.clear();
    PostOperations.clear();
    Children.clear();
}
void EveModule::resetStatus(){
	for(ChildStruct & Child : Children){
		Child.finished = false;
	}
}
bool EveModule::checkIfAllChildrenFinished(){
    for(ChildStruct Child : Children){
        if(!Child.finished){
            return false;
        }
    }
    return true;
}

void EveModule::controlText(TextModule * Text, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, const vector<SingleFont> & FontContainer){
    if(attribute == "set_id" && Values.size() > 0){
        Text->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Text->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Text->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        Text->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_scale" && Values.size() >= 2){
        Text->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        Text->setColors(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "set_color_r" && Values.size() >= 1){
        Text->color.r = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_g" && Values.size() >= 1){
        Text->color.g = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_b" && Values.size() >= 1){
        Text->color.b = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_color_a" && Values.size() >= 1){
        Text->color.a = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_random_color"){
        Text->setRandomColors();
    }
    else if(attribute == "increment_random_color"){
        Text->incrementRandomColor();
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        Text->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_random_change_speed" && Values.size() >= 1){
        Text->randomChangeSpeed = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_min_color" && Values.size() >= 1){
        Text->minColorValue = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_color" && Values.size() >= 1){
        Text->maxColorValue = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "rotate" && Values.size() >= 1){
        Text->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_font" && Values.size() > 0){
        Text->setFontID(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_new_content" && Values.size() > 0){
        Text->addNewContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_new_and_update" && Values.size() > 0){
        Text->addNewContentAndResize(Values[0].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "update_size"){
        Text->fitSizeToText(FontContainer);
    }
    else if(attribute == "select_content" && Values.size() > 0){
        Text->chooseContent(Values[0].getIntUnsafe());
    }
    else if(attribute == "delete" && Values.size() > 0){
        Text->deleteContent(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_text" && Values.size() > 1){
        Text->addTextToContent(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "set_text" && Values.size() > 1){
        Text->modifyContent(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "set_resize_text" && Values.size() > 1){
        Text->modifyContentAndResize(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "set_wrapping" && Values.size() > 0){
        Text->setWrapping(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_horizontal_align" && Values.size() > 0){
        Text->setHorizontalAlign(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_vertical_align" && Values.size() > 0){
        Text->setVerticalAlign(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_cursor_pos" && Values.size() > 0){
        Text->setCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_second_cursor_pos" && Values.size() > 0){
        Text->setSecondCursorPos(Values[0].getIntUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Text->control(attribute, temp, Values.size());
    }
}
void EveModule::controlEditableText(EditableTextModule *EditableText, string attribute, const vector<VariableModule> &Values, vector<string> &IDs, const vector<SingleFont> &FontContainer){
    if(attribute == "set_editable" && Values.size() > 0){
        EditableText->setCanBeEdited(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_space_use" && Values.size() > 0){
        EditableText->setCanUseSpace(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_enter_use" && Values.size() > 0){
        EditableText->setCanUseEnter(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_can_enter_accept" && Values.size() > 0){
        EditableText->setCanEnterAcceptChanges(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_numerical" && Values.size() > 0){
        EditableText->setIsNumerical(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_floating_point" && Values.size() > 0){
        EditableText->setHasFloatingPoint(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_can_update_variable" && Values.size() > 0){
        EditableText->setUpdateConnectedVariable(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_auto_cleaning" && Values.size() > 0){
        EditableText->setCanClearContentAfterSuccess(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_use_tabs" && Values.size() > 0){
        EditableText->setUseTabs(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_min_content_length" && Values.size() > 0){
        EditableText->setMinContentSize(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_max_content_length" && Values.size() > 0){
        EditableText->setMaxContentSize(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_input_delay" && Values.size() > 0){
        EditableText->setInputDelay(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_repetition_delay" && Values.size() > 0){
        EditableText->setRepetitionDelay(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "connect_object" && Values.size() > 0){
        EditableText->connectedObject = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_group" && Values.size() > 0){
        EditableText->connectedGroup = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_module" && Values.size() > 0){
        EditableText->connectedModule = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_module_id" && Values.size() > 0){
        EditableText->connectedModuleID = Values[0].getStringUnsafe();
    }
    else if(attribute == "connect_variable" && Values.size() > 0){
        EditableText->connectedVariable = Values[0].getStringUnsafe();
    }
    else if(attribute == "set_protected_area" && Values.size() > 0){
        EditableText->setProtectedArea(Values[0].getIntUnsafe());
    }
    else if(attribute == "cut_unprotected_area"){
        EditableText->cutCurrentContent(EditableText->getProtectedArea());
    }
    else if(attribute == "stop_editing"){
        EditableText->setEditingIsActive(false);
    }
    else if(attribute == "start_editing"){
        EditableText->setEditingIsActive(true);
    }
    else if(attribute == "set_ignore_vertical_arrows" && Values.size() > 0){
        EditableText->ignoreVerticalArrows = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_auto_resize_axis_y" && Values.size() > 0){
        EditableText->setAutoResizeAxisY(Values[0].getBoolUnsafe());
    }
    else{
        controlText(EditableText, attribute, Values, IDs, FontContainer);
    }
}
void EveModule::controlSuperText(SuperTextModule * SuperText, string attribute, const vector<VariableModule> & Values, vector <string> & IDs, vector<SingleFont> & FontContainer){
    if(attribute == "set_id" && Values.size() > 0){
        SuperText->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        SuperText->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        SuperText->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        SuperText->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_scale" && Values.size() >= 2){
        SuperText->addScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_cursor_pos" && Values.size() > 0){
        SuperText->setCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_second_cursor_pos" && Values.size() > 0){
        SuperText->setSecondCursorPos(Values[0].getIntUnsafe());
    }
    else if(attribute == "update"){
        cout << "update\n";
        SuperText->update();
    }
    else if(attribute == "crop_size_to_text"){
        SuperText->cropSizeToText();
    }
    else if(attribute == "set_content" && Values.size() >= 1){
        SuperText->setContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_content" && Values.size() >= 1){
        SuperText->addContent(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_new_text_line" && Values.size() >= 1){
        SuperText->addNewTextLine(Values[0].getStringUnsafe());
    }
    else if(attribute == "set_text_line" && Values.size() >= 2){
        SuperText->setTextLine(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "add_to_text_line" && Values.size() >= 2){
        SuperText->addToTextLine(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "add_format" && Values.size() >= 13){
        SuperText->addFormat(al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
            al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()),
            Values[8].getStringUnsafe(), FontContainer, Values[9].getIntUnsafe(), Values[10].getIntUnsafe(), Values[11].getIntUnsafe(),
            Values[12].getIntUnsafe()
        );
    }
    else if(attribute == "add_format" && Values.size() >= 6){
        SuperText->addFormat(al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
            al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
            Values[4].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[5].getIntUnsafe()
        );
    }
    else if(attribute == "add_format" && Values.size() >= 2){
        SuperText->addFormat(al_map_rgba_f(0.0, 0.0, 0.0, 1.0), al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
            Values[0].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[1].getIntUnsafe()
        );
    }
    else if(attribute == "modify_format" && Values.size() >= 14){
        SuperText->modifyFormat(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(),
            Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()), al_map_rgba_f(Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(),
            Values[7].getDoubleUnsafe(), Values[8].getDoubleUnsafe()), Values[9].getStringUnsafe(), FontContainer, Values[10].getIntUnsafe(),
            Values[11].getIntUnsafe(), Values[12].getBoolUnsafe(), Values[13].getIntUnsafe()
        );
    }
    else if(attribute == "modify_last_format" && Values.size() >= 13){
        SuperText->modifyFormat(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(),
            Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()), al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(),
            Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()), Values[8].getStringUnsafe(), FontContainer, Values[9].getIntUnsafe(),
            Values[10].getIntUnsafe(), Values[11].getBoolUnsafe(), Values[12].getIntUnsafe()
        );
    }
    else if(attribute == "modify_last_format" && Values.size() >= 10){
        SuperText->modifyFormat(SuperText->Formatting.size() - 1,
            al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()),
            al_map_rgba_f(Values[4].getDoubleUnsafe(), Values[5].getDoubleUnsafe(), Values[6].getDoubleUnsafe(), Values[7].getDoubleUnsafe()),
            Values[8].getStringUnsafe(), FontContainer, 0.0, 0.0, false, Values[9].getIntUnsafe()
        );
    }
    else if(attribute == "delete_format" && Values.size() >= 1){
        SuperText->deleteFormat(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 5){
        SuperText->setColor(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(),
            Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_last_color" && Values.size() >= 4){
        SuperText->setColor(SuperText->Formatting.size() - 1, al_map_rgba_f(Values[0].getDoubleUnsafe(),
            Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_accent_color" && Values.size() >= 5){
        SuperText->setAccentColor(Values[0].getIntUnsafe(), al_map_rgba_f(Values[1].getDoubleUnsafe(),
            Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe(), Values[4].getDoubleUnsafe()
        ));
    }
    else if(attribute == "set_font" && Values.size() >= 2){
        SuperText->setFont(Values[0].getIntUnsafe(), Values[1].getStringUnsafe(), FontContainer);
    }
    else if(attribute == "set_offset" && Values.size() >= 3){
        SuperText->setOffset(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "add_offset" && Values.size() >= 3){
        SuperText->addOffset(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "set_offset_x" && Values.size() >= 2){
        SuperText->setOffsetX(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "add_offset_x" && Values.size() >= 2){
        SuperText->addOffsetX(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_offset_y" && Values.size() >= 2){
        SuperText->setOffsetY(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "add_offset_y" && Values.size() >= 2){
        SuperText->addOffsetY(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_selected" && Values.size() >= 2){
        SuperText->setSelected(Values[0].getIntUnsafe(), Values[1].getBoolUnsafe());
    }
    else if(attribute == "set_limit" && Values.size() >= 2){
        SuperText->setLimit(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_wrapping" && Values.size() >= 1){
        if(Values[0].getStringUnsafe().size() > 0){
            SuperText->setWrapping(Values[0].getStringUnsafe()[0]);
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": In event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
        }
    }
    else if(attribute == "set_horizontal_align" && Values.size() >= 1){
        if(Values[0].getStringUnsafe().size() > 0){
            SuperText->setHorizontalAlign(Values[0].getStringUnsafe()[0]);
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": In event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
        }
    }
    else if(attribute == "set_vertical_align" && Values.size() >= 1){
        if(Values[0].getStringUnsafe().size() > 0){
            SuperText->setVerticalAlign(Values[0].getStringUnsafe()[0]);
        }
        else{
            std::cout << "Error: In " << __FUNCTION__ << ": In event '" << ID << "': In function '" << attribute << "': Char variable cannot be blank.\n";
        }
    }
    else if(attribute == "set_padding_between_lines" && Values.size() >= 1){
        SuperText->setPaddingBetweenLines(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        SuperText->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "add_rotation" && Values.size() >= 1){
        SuperText->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_tab_length" && Values.size() >= 1){
        SuperText->setTabLength(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_draw_selection_first" && Values.size() >= 1){
        SuperText->drawSelectionFirst = Values[0].getBoolUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        SuperText->control(attribute, temp, Values.size());
    }
}
void EveModule::controlSuperEditableText(SuperEditableTextModule * SuperEditableText, string attribute,
    const vector<VariableModule> & Values, vector <string> & IDs, vector<SingleFont> & FontContainer
){
    if(attribute == "set_can_be_edited" && Values.size() > 0){
        SuperEditableText->canBeEdited = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_use_space" && Values.size() > 0){
        SuperEditableText->canUseSpace = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_use_enter" && Values.size() > 0){
        SuperEditableText->canUseEnter = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_can_use_tabs" && Values.size() > 0){
        SuperEditableText->canUseTabs = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_is_numerical" && Values.size() > 0){
        SuperEditableText->isNumerical = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_has_floating_point" && Values.size() > 0){
        SuperEditableText->hasFloatingPoint = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_ignore_vertical_arrows" && Values.size() > 0){
        SuperEditableText->ignoreVerticalArrows = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_ignore_content_restriction" && Values.size() > 0){
        SuperEditableText->ignoreContentRestriction = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_is_storing_history" && Values.size() > 0){
        SuperEditableText->isStoringHistory = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_is_editing_active" && Values.size() > 0){
        SuperEditableText->canBeEdited = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_min_content_length" && Values.size() > 0){
        SuperEditableText->minContentLength = Values[0].getIntUnsafe();
    }
    else if(attribute == "set_max_content_length" && Values.size() > 0){
        SuperEditableText->maxContentLength = Values[0].getIntUnsafe();
    }
    else if(attribute == "set_input_delay" && Values.size() > 0){
        SuperEditableText->inputDelay = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_repetition_delay" && Values.size() > 0){
        SuperEditableText->repetitionDelay = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_protected_area" && Values.size() > 0){
        SuperEditableText->protectedArea = Values[0].getIntUnsafe();
    }
    else if(attribute == "cut_unprotected_area"){
        SuperEditableText->cutContent(SuperEditableText->protectedArea);
    }
    else{
        controlSuperText(SuperEditableText, attribute, Values, IDs, FontContainer);
    }
}
void EveModule::controlImage(ImageModule *Image, string attribute, const vector<VariableModule> &Values, vector<string> &IDs, vector<SingleBitmap> &BitmapContainer, string workingDirectory){
    if(attribute == "set_id" && Values.size() > 0){
        Image->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Image->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Image->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_scale" && Values.size() >= 2){
        Image->setScale(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "resize" && Values.size() >= 2){
        Image->resize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation" && Values.size() >= 1){
        Image->setRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "rotate" && Values.size() >= 1){
        Image->addRotation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_mirror" && Values.size() >= 2){
        Image->setMirror(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        float arr[] = {float(Values[0].getDoubleUnsafe()), float(Values[1].getDoubleUnsafe()), float(Values[2].getDoubleUnsafe()), float(Values[3].getDoubleUnsafe())};
        Image->setImageColor(arr);
    }
    else if(attribute == "set_color_r" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'r');
    }
    else if(attribute == "set_color_g" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'g');
    }
    else if(attribute == "set_color_b" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'b');
    }
    else if(attribute == "set_color_a" && Values.size() >= 1){
        Image->setImageColor(Values[0].getDoubleUnsafe(), 'a');
    }
    else if(attribute == "set_light" && Values.size() >= 4){
        Image->setLightColor(vec3d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe()), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "connect_bitmap" && Values.size() > 0){
        if(Values.size() == 1){
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", workingDirectory);
        }
        else{
            Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), Values[1].getStringUnsafe(), workingDirectory);
        }   
    }
    else if(attribute == "connect_bitmap_via_path" && Values.size() > 0){
        Image->connectBitmap(BitmapContainer, Values[0].getStringUnsafe(), "", workingDirectory);  
    }
    else if(attribute == "connect_bitmap_via_alias" && Values.size() > 0){
        Image->connectBitmap(BitmapContainer, "", Values[0].getStringUnsafe(), workingDirectory);  
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Image->control(attribute, temp, Values.size());
    }
}
void EveModule::controlMovement(MovementModule * Movement, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "move" && Values.size() >= 2){
        Movement->addMomentum(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "move_up"){
        Movement->setNextMove(true, false, false, false, false, false, false);
    }
    else if(attribute == "move_right"){
        Movement->setNextMove(false, true, false, false, false, false, false);
    }
    else if(attribute == "move_down"){
        Movement->setNextMove(false, false, true, false, false, false, false);
    }
    else if(attribute == "move_left"){
        Movement->setNextMove(false, false, false, true, false, false, false);
    }
    else if(attribute == "move_jump"){
        Movement->setNextMove(false, false, false, false, true, false, false);
    }
    else if(attribute == "move_fall"){
        Movement->setNextMove(false, false, false, false, false, true, false);
    }
    else if(attribute == "move_run"){
        Movement->setNextMove(false, false, false, false, false, false, true);
    }
    else if(attribute == "set_id" && Values.size() > 0){
        Movement->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_movement_type" && Values.size() > 0){
        Movement->setMovementType(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_input_type" && Values.size() > 0){
        Movement->setInputType(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_allowed_jumps" && Values.size() > 0){
        Movement->setAllowedJumps(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_jump_cooldown" && Values.size() > 0){
        Movement->setJumpCooldownDuration(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_move_cooldown" && Values.size() > 0){
        Movement->setMoveCooldownDuration(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_diagonal_movement" && Values.size() > 0){
        Movement->setCanMoveDiagonally(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_reset_momentum_when_jumping" && Values.size() > 0){
        Movement->setResetMomentumWhenJumping(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_mass" && Values.size() > 0){
        Movement->setBodyMass(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_walking_speed" && Values.size() > 0){
        Movement->setWalkingSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_running_speed" && Values.size() > 0){
        Movement->setRunningSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_jump_speed" && Values.size() > 0){
        Movement->setJumpSpeed(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_gravity" && Values.size() > 0){
        Movement->setGravitation(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_friction" && Values.size() > 0){
        Movement->setBaseFriction(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_max_momentum_x" && Values.size() > 0){
        Movement->setMaxMomentumX(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_momentum_y" && Values.size() > 1){
        Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
        Movement->setMaxMomentumY(Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_min_momentum_y" && Values.size() > 0){
        Movement->setMinMomentumY(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_max_momentum_y" && Values.size() > 0){
        Movement->setMaxMomentumY(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "set_mouse_movement_button" && Values.size() > 0){
        Movement->setMouseButton(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_move_on_mouse_release" && Values.size() > 0){
        Movement->setMoveOnMouseRelease(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_direction_reset_after_collision" && Values.size() > 0){
        Movement->setResetDirectionAfterCollision(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_moving_keys" && Values.size() > 5){
        Movement->bindKeys(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe(), Values[4].getIntUnsafe(), Values[5].getIntUnsafe());
    }
    else if(attribute == "set_key_up" && Values.size() > 0){
        Movement->setUpKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_right" && Values.size() > 0){
        Movement->setRightKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_down" && Values.size() > 0){
        Movement->setDownKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_left" && Values.size() > 0){
        Movement->setLeftKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_jump" && Values.size() > 0){
        Movement->setJumpKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_key_running" && Values.size() > 0){
        Movement->setRunningKey(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_random_actions" && Values.size() > 0){
        Movement->areRandomActionsEnabled = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_action_time" && Values.size() > 1){
        Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
        Movement->maxTimeOfAction = Values[1].getDoubleUnsafe();
    }
    else if(attribute == "set_min_action_time" && Values.size() > 0){
        Movement->minTimeOfAction = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_action_time" && Values.size() > 0){
        Movement->maxTimeOfAction = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_move_distance" && Values.size() > 1){
        Movement->minMoveDistance = Values[0].getDoubleUnsafe();
        Movement->maxMoveDistance = Values[1].getDoubleUnsafe();
    }
    else if(attribute == "set_min_move_distance" && Values.size() > 0){
        Movement->minMoveDistance = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_max_move_distance" && Values.size() > 0){
        Movement->maxMoveDistance = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_breaks_allowed" && Values.size() > 0){
        Movement->areBreaksAllowed = Values[0].getBoolUnsafe();
    }
    else if(attribute == "set_break_chance" && Values.size() > 0){
        Movement->chanceForBreak = Values[0].getDoubleUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Movement->control(attribute, temp, Values.size());
    }
}
void EveModule::controlCollision(CollisionModule * Collision, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Collision->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Collision->setPos(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Collision->setSize(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_solid" && Values.size() > 0){
        Collision->setIsSolid(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_solid"){
        Collision->switchSolid();
    }
    else if(attribute == "set_penetration" && Values.size() > 0){
        Collision->setCanPenetrateSolids(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_penetration"){
        Collision->switchSolidPenetration();
    }
    else if(attribute == "ignore" && Values.size() > 1){
        Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "stop_ignoring" && Values.size() > 1){
        Collision->addToIgnoreList(Values[0].getStringUnsafe(), Values[1].getStringUnsafe());
    }
    else if(attribute == "clear_ignoring" && Values.size() > 0){
        Collision->clearIgnoreList(Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_object" && Values.size() > 0){
        Collision->addToIgnoreList("objects", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_group_of_objects" && Values.size() > 0){
        Collision->addToIgnoreList("groups_of_objects", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_hitbox" && Values.size() > 0){
        Collision->addToIgnoreList("hitboxes", Values[0].getStringUnsafe());
    }
    else if(attribute == "ignore_group_of_hitboxes" && Values.size() > 0){
        Collision->addToIgnoreList("groups_of_hitboxes", Values[0].getStringUnsafe());
    }
    else if(attribute == "set_is_circle" && Values.size() > 0){
        Collision->setIsCircle(Values[0].getBoolUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Collision->control(attribute, temp, Values.size());
    }
}
void EveModule::controlParticles(ParticleEffectModule * Particles, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Particles->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_environment" && Values.size() >= 2){
        Particles->setEnvironment(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_environment_speed" && Values.size() >= 2){
        Particles->setEnvironmentSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_speed" && Values.size() > 1){
        Particles->setSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_basic_speed" && Values.size() > 1){
        Particles->setBasicSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_acceleration" && Values.size() > 1){
        Particles->setAcceleration(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_mass" && Values.size() > 1){
        Particles->setParticleMass(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_directional_degree" && Values.size() > 1){
        Particles->setDirectionDegree(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_rotation_speed" && Values.size() > 1){
        Particles->setRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_time_to_negate_rotation" && Values.size() > 1){
        Particles->setTimeToNegateRotation(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_are_particles_moving" && Values.size() > 0){
        Particles->setAreParticlesMoving(Values[0].getBoolUnsafe());
    }
    else if(attribute == "allow_movement"){
        Particles->switchAreParticlesMoving();
    }
    else if(attribute == "set_is_env_sync" && Values.size() > 0){
        Particles->setIsEnvironmentSynchronized(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_radius" && Values.size() > 1){
        Particles->setParticleRadius(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_time_to_death" && Values.size() > 1){
        Particles->setTimeToDeath(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_shape_rotation_speed" && Values.size() > 1){
        Particles->setShapeRotationSpeed(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_color_intensity" && Values.size() > 1){
        Particles->setColorIntensity(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "add_color_interval" && Values.size() > 3){
        Particles->addColorInterval(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe(), Values[3].getIntUnsafe());
    }
    else if(attribute == "add_color_interval_in_hex" && Values.size() > 0){
        Particles->addColorIntervalInHex(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_color_interval" && Values.size() > 0){
        Particles->removeColorInterval(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_color" && Values.size() > 2){
        Particles->addColor(Values[0].getIntUnsafe(), Values[1].getIntUnsafe(), Values[2].getIntUnsafe());
    }
    else if(attribute == "add_color_in_hex" && Values.size() > 0){
        Particles->addColorInHex(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_color" && Values.size() > 0){
        Particles->removeColor(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_image" && Values.size() > 0){
        Particles->addImage(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_image" && Values.size() > 0){
        Particles->removeImage(Values[0].getStringUnsafe());
    }
    else if(attribute == "remove_image_by_index" && Values.size() > 0){
        Particles->removeImage(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_shape" && Values.size() > 0){
        Particles->setParticlesShape(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_use_images" && Values.size() > 0){
        Particles->setUseImageAsParticles(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_source"){
        Particles->switchUseImageAsParticles();
    }
    else if(attribute == "set_use_random_colors" && Values.size() > 0){
        Particles->setUseRandomColors(Values[0].getBoolUnsafe());
    }
    else if(attribute == "toggle_random_colors"){
        Particles->switchUseRandomColors();
    }
    else if(attribute == "set_is_static" && Values.size() > 0){
        Particles->setIsModuleStatic(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_drawing_details" && Values.size() > 0){
        Particles->setIsDrawingWithDetails(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_block_spawning" && Values.size() > 0){
        Particles->setBlockParticlesSpawn(Values[0].getBoolUnsafe());
    }
    else if(attribute == "set_spawning" && Values.size() > 0){
        Particles->setCanParticlesSpawn(Values[0].getBoolUnsafe());
    }
    else if(attribute == "allow_spawning"){
        Particles->allowSpawning();
    }
    else if(attribute == "set_spawn_rate" && Values.size() > 1){
        Particles->setParticlesPerSpawn(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
    }
    else if(attribute == "set_spawn_cap" && Values.size() > 0){
        Particles->setMaxParticlesCount(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_max_time_to_spawn" && Values.size() > 0){
        Particles->setMaxTimeToSpawn(Values[0].getDoubleUnsafe());
    }
    else if(attribute == "bind_key" && Values.size() > 0){
        Particles->setSpawnKeyBind(Values[0].getStringUnsafe()[0]);
    }
    else if(attribute == "bind_key_by_int" && Values.size() > 0){
        Particles->setSpawnKeyBindShort(Values[0].getIntUnsafe());
    }
    else if(attribute == "spawn_on_key_release" && Values.size() > 0){
        Particles->setSpawnOnKeyRelease(Values[0].getBoolUnsafe());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Particles->control(attribute, temp, Values.size());
    }
}
void EveModule::controlVariables(VariableModule * Variable, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Variable->setID(Values[0].getStringUnsafe(), &IDs);
    }
    else if(attribute == "toggle_bool"){
        Variable->toggleBool();
    }
    else if(attribute == "set_bool_random"){
        if(rand() % 2 == 0)
        Variable->toggleBool();
    }
    else if(attribute == "set_bool_probability" && Values.size() >= 3){
        bool settingBool;
        if(Values[0].getDoubleUnsafe() == 0){
            settingBool = false;
        }
        else if(Values[0].getDoubleUnsafe() == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % Values[2].getIntUnsafe() <= Values[1].getIntUnsafe()){
            Variable->setBool(settingBool);
        }
    }
    else if(attribute == "set_bool_probability" && Values.size() >= 2){
        bool settingBool;
        if(Values[0].getDoubleUnsafe() == 0){
            settingBool = false;
        }
        else if(Values[0].getDoubleUnsafe() == 1){
            settingBool = true;
        }
        else{
            return;
        }

        if(rand() % 100 <= Values[1].getDoubleUnsafe()){
            Variable->setBool(settingBool);
        }
    }
    else if(attribute == "set_bool" && Values.size() >= 1){
        if(Values[0].getDoubleUnsafe() == 0){
            Variable->setBool(false);
        }
        if(Values[0].getDoubleUnsafe() == 1){
            Variable->setBool(true);
        }
    }
    else if(attribute == "set_int" && Values.size() >= 1){
        Variable->setInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "set_int_interval" && Values.size() >= 2){
        Variable->setInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "set_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "set_double" && Values.size() >= 1){
        Variable->setDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "set_double_interval" && Values.size() >= 2){
        Variable->setDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "set_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->setDouble(Values[dice].getDoubleUnsafe());
    }
    else if(attribute == "set_string" && Values.size() >= 1){
        Variable->setString(Values[0].getString());
    }
    else if(attribute == "set_string_random" && Values.size() >= 1){
        cout << "hello ";
        unsigned int dice = rand() % Values.size();
        cout << Variable->setString(Values[dice].getString()) << " ";
        cout << Variable->getString() << "\n";
    }
    else if(attribute == "add_int" && Values.size() >= 1){
        Variable->addInt(Values[0].getIntUnsafe());
    }
    else if(attribute == "add_int_interval" && Values.size() >= 2){
        Variable->addInt(randomInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe()));
    }
    else if(attribute == "add_int_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addInt(Values[dice].getIntUnsafe());
    }
    else if(attribute == "add_double" && Values.size() >= 1){
        Variable->addDouble(Values[0].getDoubleUnsafe());;
    }
    else if(attribute == "add_double_interval" && Values.size() >= 2){
        Variable->addDouble(randomDouble(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));;
    }
    else if(attribute == "add_double_random" && Values.size() >= 1){
        unsigned int dice = rand() % Values.size();
        Variable->addDouble(Values[dice].getDoubleUnsafe());
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": function " << attribute << "<" << Values.size() << "> does not exist.\n";
    }
}
void EveModule::controlScrollbar(ScrollbarModule * Scrollbar, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Scrollbar->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Scrollbar->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_thumb_position" && Values.size() >= 2){
        Scrollbar->setThumbPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_thumb_size" && Values.size() >= 2){
        Scrollbar->setThumbSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_scrolling_area" && Values.size() >= 2){
        Scrollbar->setScrollingArea(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_real_scrolling_area" && Values.size() >= 2){
        Scrollbar->setRealScrollingArea(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "add_real_scrolling_area" && Values.size() >= 2){
        Scrollbar->addRealScrollingArea(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe());
    }
    else if(attribute == "set_thumb_image_id" && Values.size() >= 1){
        Scrollbar->setThumbImageID(Values[0].getString());
    }
    else if(attribute == "set_track_image_id" && Values.size() >= 1){
        Scrollbar->setTrackImageID(Values[0].getString());
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Scrollbar->control(attribute, temp, Values.size());
    }
}
void EveModule::controlPrimitives(PrimitivesModule * Primitives, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "update_with_size"){
        Primitives->updateWithSize();
    }
    else if(attribute == "set_id" && Values.size() > 0){
        Primitives->setID(Values[0].getStringUnsafe(), IDs);
    }
    else if(attribute == "set_position" && Values.size() >= 2){
        Primitives->setPos(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_size" && Values.size() >= 2){
        Primitives->setSize(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_type" && Values.size() >= 1){
        Primitives->type = getPrimitiveType(Values[0].getStringUnsafe());
    }
    else if(attribute == "add_point" && Values.size() >= 2){
        Primitives->points.push_back(vec2d(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe()));
    }
    else if(attribute == "set_color" && Values.size() >= 4){
        Primitives->color = al_map_rgba_f(Values[0].getDoubleUnsafe(), Values[1].getDoubleUnsafe(), Values[2].getDoubleUnsafe(), Values[3].getDoubleUnsafe());
    }
    else if(attribute == "set_thickness" && Values.size() >= 1){
        Primitives->thickness = Values[0].getDoubleUnsafe();
    }
    else if(attribute == "set_radius" && Values.size() >= 1){
        Primitives->radius = Values[0].getDoubleUnsafe();
    }
    else{
        bool temp = false;
        if(Values.size() > 0){
            temp = Values[0].getBoolUnsafe();
        }
        Primitives->control(attribute, temp, Values.size());
    }
}
void EveModule::controlVector(VectorModule * Vector, string attribute, const vector<VariableModule> & Values, vector <string> & IDs){
    if(attribute == "set_id" && Values.size() > 0){
        Vector->setID(Values[0].getStringUnsafe(), &IDs);
    }
    else if(attribute == "push_back" && Values.size() >= 1){
        if(Values[0].getType() == 'b'){
            Vector->pushBool(Values[0].getBoolUnsafe());
        }
        else if(Values[0].getType() == 'i'){
            Vector->pushInt(Values[0].getIntUnsafe());
        }
        else if(Values[0].getType() == 'd'){
            Vector->pushDouble(Values[0].getDoubleUnsafe());
        }
        else if(Values[0].getType() == 's'){
            Vector->pushString(Values[0].getStringUnsafe());
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Value of '" << Values[0].getType()
                << "' type cannot be pushed back into the vector '" << Vector->getID()
                << "' of '" << Vector->getType() << "' type.\n";
        }
    }
    else if(attribute == "pop_back"){
        Vector->popBack();
    }
    else if(attribute == "remove" && Values.size() >= 1){
        Vector->removeIndex(Values[0].getIntUnsafe());
    }
    else if(attribute == "set" && Values.size() >= 2){
        if(Values[1].getType() == 'b'){
            Vector->setBool(Values[0].getIntUnsafe(), Values[1].getBoolUnsafe());
        }
        else if(Values[1].getType() == 'i'){
            Vector->setInt(Values[0].getIntUnsafe(), Values[1].getIntUnsafe());
        }
        else if(Values[1].getType() == 'd'){
            Vector->setDouble(Values[0].getIntUnsafe(), Values[1].getDoubleUnsafe());
        }
        else if(Values[1].getType() == 's'){
            Vector->setString(Values[0].getIntUnsafe(), Values[1].getStringUnsafe());
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Value of '" << Values[0].getType()
                << "' type cannot be assign to the vector '" << Vector->getID()
                << "' of '" << Vector->getType() << "' type.\n";
        }
    }
    else{
        cout << "Error: In " << __FUNCTION__ << ": Function " << attribute << "<" << Values.size() << "> does not exist.\n";
    }
}

void EveModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    if(attribute == "else_child_id"){
        BasePointers.push_back(BasePointersStruct());
        BasePointers.back().setPointer(&elseChildID);
    }
    else{
        getPrimaryContext(attribute, BasePointers);
    }
}


void ValueLocation::print(string dynamicID){
    if(dynamicID != ""){
        cout << dynamicID;
    }
    if(source != "" && source != "_"){
        if(dynamicID != ""){
            cout << ".";
        }
        cout << source;
    }
    if(cameraID != "" && cameraID != "_"){
        cout << "." << cameraID;
    }
    if(layerID != "" && layerID != "_"){
        cout << "." << layerID;
    }
    if(objectID != "" && objectID != "_"){
        cout << "." << objectID;
    }
    if(moduleType != "" && moduleType != "_"){
        cout << "." << moduleType;
    }
    if(moduleID != "" && moduleID != "_"){
        cout << "." << moduleID;
    }
    if(attribute != "" && attribute != "_"){
        cout << "." << attribute;
    }
    if(spareID != "" && spareID != "_"){
        cout << "." << spareID;
    }
    cout << " ";
}
