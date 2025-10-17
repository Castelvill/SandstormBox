#ifndef PRIMARYMODULE_H_INCLUDED
#define PRIMARYMODULE_H_INCLUDED
#include <cstdio>
#include "usefull.h"

enum ReturnType : char{
    OK, NULL_VAL, EMPTY, INVALID_TYPE, READ_ONLY, ERROR,
    ERROR_INF, OUT_OF_SCOPE, UNDEFINED, CORRUPTED, CONTINUE
};

enum EngineInstr : unsigned char{
    null, start, end_i, annotation_i, run, if_i, else_if, else_i, end_if,while_i, end_while, continue_i,
    break_i, return_i, reboot, exit_i, delete_this_event, reset_keyboard, dump_context_stack,
    dump_memory, dump_local_memory, first, last, all, random_i, index_i, index_vec, sum, intersection,
    diff, add, sub, mul, div_i, mod, pow_i, assign, move, add_move, sub_move, mul_move, div_move,
    equal, not_equal, more, less, more_equal, less_equal, not_i, and_i, or_i, igT, igF, in,
    find_by_id_2, inc, next, dec, del, demolish, access_i, bool_i, int_i, double_i, string_i,
    bool_vec_i, int_vec_i, double_vec_i, string_vec_i, rand_int, find_by_id, clone_i, new_i, bind_i,
    rbind_i, build, load_build, build_subset, inject_code, inject_instr, fun, env, edit_proc,
    load_bitmap, mkdir_i, rm, rmll, mv_i, print_v_i, print_d_i, print_i, load_text, save_text, ls, lse,
    new_proc, var, vec, tokenize, tree, len, size_i, substr, load_font, restart_drag, cd, pwd, similar,
    count, create_display, console_input, start_timer, stop_timer, compiler_breakpoint, breakpoint, assert, type, load_i,
    import, resume_assembling, stop_assembling, start_printing_tokens, stop_printing_tokens,
    //instance declarations
    Val, ValVec, Pointer, PointerVec, Camera, CameraVec, Layer, LayerVec,
    Object, ObjectVec, Var, VarVec, Vec, VecVec, Text, TextVec, EditText, EditTextVec, SText, STextVec,
    SEditText, SEditTextVec, Image, ImageVec, Movement, MovementVec, Collision, CollisionVec, Particles,
    Event, EventVec, Scrollbar, ScrollbarVec, Primitive, PrimitiveVec, any 
};
EngineInstr strToInstr(const string & instruction, bool printError = true);
string instrToStr(const EngineInstr & instruction);

enum AttributeType: short{
    //text
    null_a, set_id, set_pos, set_size, set_scale, add_scale, set_color,
    set_color_r, set_color_g, set_color_b, set_color_a, set_random_color,
    increment_random_color, set_rotation, set_random_change_speed,
    set_min_color, set_max_color, rotate, set_font, add_new_content,
    add_new_and_update, update_size, select_content, delete_a, add_text,
    set_text, set_resize_text, set_wrapping, set_horizontal_align,
    set_vertical_align, set_cursor_pos, set_second_cursor_pos,
    //editable_text
    set_editable, set_space_use, set_enter_use, set_can_enter_accept,
    set_numerical, set_floating_point, set_can_update_variable,
    set_auto_cleaning, set_use_tabs, set_min_content_length,
    set_max_content_length, set_input_delay, set_repetition_delay,
    connect_object, connect_group, connect_module, connect_module_id,
    connect_variable, set_protected_area, cut_unprotected_area,
    stop_editing, start_editing, set_ignore_vertical_arrows,
    set_can_auto_resize_axis_y,
    //super_text
    update, crop_size_to_text, crop_width_to_text,
    crop_height_to_text, set_content, add_content, add_new_text_line,
    set_text_line, add_to_text_line, save_to_file, load_from_file,
    add_format, modify_format, modify_last_format, inject_format,
    delete_format, set_last_color, set_accent_color, set_last_accent_color,
    set_background_color, set_last_background_color,
    set_background_color_drawing, set_last_background_color_drawing,
    set_offset, add_offset, set_offset_x, add_offset_x, set_offset_y,
    add_offset_y, set_selected, set_limit, set_padding_between_lines,
    add_rotation, set_tab_length, set_draw_selection_first,
    //super_editable_text
    set_can_be_edited, set_can_use_space, set_can_use_enter,
    set_can_use_tabs, set_is_numerical, set_has_floating_point,
    set_ignore_content_restriction, set_is_storing_history,
    set_can_copy_format, deactivate_a,
    //image
    resize, set_mirror, set_light, connect_bitmap, connect_bitmap_via_path,
    connect_bitmap_via_alias,
    //image_pointer
    rot_pos_x, rot_pos_y, start_x, start_y, frame_size_x, frame_size_y,
    rotate_angle, mirror_x, mirror_y, image_color_r, image_color_g,
    image_color_b, image_color_a, light_level, light_color_r, light_color_g,
    light_color_b,
    //movement
    move_a, move_up, move_right, move_down, move_left, move_jump, move_fall,
    move_run, set_movement_type, set_input_type, set_allowed_jumps,
    set_jump_cooldown, set_move_cooldown, set_diagonal_movement,
    set_reset_momentum_when_jumping, set_mass, set_walking_speed,
    set_running_speed, set_jump_speed, set_gravity, set_friction,
    set_max_momentum_x, set_momentum_y, set_min_momentum_y,
    set_max_momentum_y, set_mouse_movement_button, set_move_on_mouse_release,
    set_direction_reset_after_collision, set_moving_keys, set_key_up,
    set_key_right, set_key_down, set_key_left, set_key_jump, set_key_running,
    set_random_actions, set_action_time, set_min_action_time,
    set_max_action_time, set_move_distance, set_min_move_distance,
    set_max_move_distance, set_breaks_allowed, set_break_chance,
    //movement
    set_solid,toggle_solid, set_penetration, toggle_penetration, ignore,
    stop_ignoring, clear_ignoring, ignore_object, ignore_group_of_objects,
    ignore_hitbox, ignore_group_of_hitboxes, set_is_circle, 
    //particles
    set_environment, set_environment_speed, set_speed, set_basic_speed,
    set_acceleration, set_directional_degree, set_rotation_speed,
    set_time_to_negate_rotation, set_are_particles_moving, allow_movement,
    set_is_env_sync, set_radius, set_time_to_death, set_shape_rotation_speed,
    set_color_intensity, add_color_interval, add_color_interval_in_hex,
    remove_color_interval, add_color, add_color_in_hex, remove_color,
    add_image, remove_image, remove_image_by_index, set_shape, set_use_images,
    toggle_source, set_use_random_colors, toggle_random_colors, set_is_static,
    set_drawing_details, set_block_spawning, set_spawning, allow_spawning,
    set_spawn_rate, set_spawn_cap, set_max_time_to_spawn, bind_key,
    bind_key_by_int, spawn_on_key_release,
    //variables
    toggle_bool, set_bool_random, set_bool_probability, set_bool, set_int,
    set_int_interval, set_int_random, set_double, set_double_interval,
    set_double_random, set_string, set_string_random, add_int,
    add_int_interval, add_int_random, add_double, add_double_interval,
    add_double_random,
    //scrollbar
    set_thumb_position, set_thumb_size, set_scrolling_area,
    set_real_scrolling_area, add_real_scrolling_area, set_thumb_image_id,
    set_track_image_id, set_can_be_drawn, set_mouse_wheel_speed,
    scroll_to_the_beginning, scroll_to_the_end, scroll_by_distance,
    add_camera, remove_camera, clear_cameras,
    //primitives
    update_with_size, set_type, add_point, set_thickness,
    //vector
    push_back_a, pop_back_a, clear_a, remove_a, set_a,
    //misc
    else_child_id, group, layer_id, number_of_cameras_a, number_of_layers_a,
    number_of_objects_a, bool_a, int_a, double_a, string_a,
    //camera
    camera_a, is_active, id, pos_x, pos_y, relative_pos_x, relative_pos_y,
    vision_shift_x, vision_shift_y, size_x, size_y, zoom, min_zoom,
    max_zoom, speed, zoom_in_key, zoom_out_key, zoom_reset_key, up_key,
    right_key, down_key, left_key, pinned_camera_id, followed_layer_id,
    followed_object_id, followed_image_id, is_pinned_to_camera,
    is_following_object, is_using_keyboard_to_move, is_using_keyboard_to_zoom,
    can_be_modified_by_mouse, can_move_objects, can_edit_text, grabbed,
    is_focused,
    //camera_functions
    draw_one_frame, clear_bitmap, set_active, toggle,
    set_relative_position, set_min_size, set_zoom, set_zoom_increase,
    set_min_zoom, set_max_zoom, bind_keys, set_key_zoom_in, set_key_zoom_out,
    set_key_zoom_reset, pin_to_camera, follow_layer, follow_object,
    follow_image, set_is_pinned_to_camera, set_is_forcefully_pinned,
    activate_pin, deactivate_pin, toggle_pin, set_is_following_object,
    set_can_move_with_keyboard, set_can_zoom_with_keyboard, 
    set_is_vision_affected_by_mouse, set_can_zoom_with_mouse,
    set_can_be_modified_by_mouse, set_can_move_objects, set_can_edit_text,
    add_visible_layer, add_accessible_layer, remove_visible_layer,
    remove_accessible_layer, clear_visible_layers, clear_accessible_layers,
    set_tint, set_drawing_borders, allow_drawing_object_borders,
    forbid_drawing_object_borders, set_can_draw_object_borders, minimize,
    bring_forward, set_can_be_grabbed, set_grabbing_area_position, 
    set_grabbing_area_size, set_can_draw_on_camera, set_can_clear_bitmap,
    set_keep_inside_screen, set_can_mouse_resize, focus, unfocus,
    //entities
    layer_a, object_a, text_a, editable_text_a, super_text_a, super_editable_text_a, image_a,
    movement_a, collision_a, particles_a, event_a, variable_a, scrollbar_a,
    primitives_a, vector_a, context_a, memory_address_a, literal_a,
    //layer
    set_is_active, add_group, remove_group, move_in_drawing_order,
    //object
    clear_groups, disable_scrolling, enable_scrolling, set_is_scrollable,
    enable_selection_border, disable_selection_border, set_can_draw_selection_border,
    set_can_be_moved_with_mouse, update_scrollbars,
    //movement_value
    allowed_jumps, jumps_count, jump_cooldown, jump_cooldown_duration,
    can_jump, body_mass, walking_speed, running_speed, jump_speed, gravitation,
    base_friction, momentum_x, momentum_y, is_moving_up, is_moving_right,
    is_moving_down, is_moving_left, is_moving, is_still,
    //movement_pointer
    movement_type, input_type, move_cooldown, move_cooldown_duration,
    can_move_diagonally, reset_momentum_when_jumping, is_move_planned,
    max_momentum_x, min_momentum_y, max_momentum_y,
    destination_x, destination_y, direction_of_move_x, direction_of_move_y,
    mouse_button, move_on_mouse_release, reset_direction_after_collision,
    is_jumping, is_falling, is_running, jump_key, running_key,
    are_random_actions_enabled, time_when_action_persists, min_time_of_action,
    max_time_of_action, min_move_distance, max_move_distance, saved_action,
    saved_direction_x, saved_direction_y, chance_for_break, current_direction_x,
    current_direction_y, current_pos, loop_movement,
    //collision_value
    has_solid_hitbox, is_fully_solid, detected, with_object, with_hitbox, in_group,
    hitbox_is_solid, hitbox_can_penetrate, hitbox_ignores_object,
    hitbox_ignores_object_group, hitbox_ignores_hitbox, hitbox_ignores_hitbox_group,
    hitbox_detected, hitbox_detected_hitbox,
    //collision_pointer
    is_solid, can_penetrate_solids,
    //object_value
    scale_x, scale_y, can_draw_selection_border, visible, visible_specific, pressed,
    pressing, released,
    //text_value
    content, current_text_id, font_id, color_r, color_g, color_b, color_a, wrapped,
    horizontal_align, vertical_align, rotation,
    //text_pointer
    text_color_r, text_color_g, text_color_b, text_color_a,
    //editable_text_value
    can_be_edited, editing, protected_area,
    //super_text_pointer
    original_content, can_use_space, can_use_enter, enter_ends_editing, can_use_tabs,
    is_numerical, has_floating_point, ignore_vertical_arrows,
    ignore_content_restriction, is_storing_history, min_content_length,
    max_content_length, input_delay, repetition_delay, is_editing_active, cursor_pos,
    second_cursor_pos,
    //super_text_value
    cursor_pos_x, cursor_pos_y, min_cursor_pos, max_cursor_pos, wrapped_lines,
    current_line,
    //scrollbar_value
    scroll_shift_x, scroll_shift_y, thumb_pos_x, thumb_pos_y, thumb_size_x,
    thumb_size_y, scrolling_area_x, scrolling_area_y, real_scrolling_area_x,
    real_scrolling_area_y, drag_starting_pos_x, drag_starting_pos_y, can_be_drawn,
    mouse_pressed_a, mouse_wheel_speed,
    //scrollbar_pointer
    thumb_image_id, track_image_id,
    //vector_value
    value, size_a, back_a, index_a,
    //vector_pointer
    type_a, last_bool, last_int, last_double, last_string, first_bool, first_int,
    first_double, first_string, random_bool, random_int, random_double, random_string,
    //collision_value
    release_on,
    //camera_value
    is_selected,
    //primitive
    activate_a, scale_from_center, do_not_scale_from_center,
    set_scale_from_center, allow_selection, forbid_selection, set_can_be_selected,
    is_scaled_from_center, is_scrollable, can_be_selected,
    //primitive_pointer
    thickness, 
    //particle_pointer
    environment_x, environment_y, environment_speed_x, environment_speed_y, min_speed,
    max_speed, min_basic_speed, max_basic_speed, min_acceleration, max_acceleration,
    min_particle_mass, max_particle_mass, min_direction_degree, max_direction_degree,
    min_rotation_speed, max_rotation_speed, min_time_to_negate_rotation,
    max_time_to_negate_rotation, are_particles_moving, is_environment_synchronized,
    min_particle_radius, max_particle_radius, min_time_to_death, max_time_to_death,
    min_shape_rotation_speed, max_shape_rotation_speed, min_color_intensity,
    max_color_intensity, particles_shape, use_image_as_particles, use_random_colors,
    is_module_static, is_drawing_with_details, block_particles_spawn,
    can_particles_spawn, min_particles_per_spawn, max_particles_per_spawn,
    max_particles_count, time_to_spawn, max_time_to_spawn, spawn_key_bind,
    //engine
    window_title_a, display_size_a, fullscreen_a, pixel_art_a, can_afk_timeout_a,
    afk_timeout_time_a, can_exit_on_idle_a
};

AttributeType strToAttribute(const string & attribute, string & error);
AttributeType strToAttribute(const string & attribute);
string attributeToStr(const AttributeType & attribute);

enum BaseType: char {
    null_bt, bool_bt, char_bt, short_bt, u_short_bt, int_bt, u_int_bt,
    float_bt, double_bt, string_bt
};
BaseType strToBaseType(const string & type);
string baseTypeToStr(const BaseType & type);

class BaseVariableStruct{
public:
    BaseType type = null_bt;
    bool vBool = false;
    char vChar;
    short vShort = 0;
    unsigned short vUShort = 0;
    int vInt = 0;
    unsigned int vUInt = 0;
    float vFloat = 0.0;
    double vDouble = 0.0;
    string vString;
    BaseVariableStruct();
    BaseVariableStruct(const BaseVariableStruct & Original);
    BaseVariableStruct & operator=(const BaseVariableStruct& Original);
    template<typename LeftType>
    void moveValue(LeftType * LeftOperand) const;
    string getString() const;
};

struct InstrDescription{
    string layerID;
    string objectID;
    string eventID;
    EngineInstr instruction;
    string scriptName;
    unsigned lineNumber = 0;
};

class BasePointersStruct{
public:
    BaseType type = null_bt;
    bool readOnly = false;
    bool * pBool = nullptr;
    char * pChar = nullptr;
    short * pShort = nullptr;
    unsigned short * pUShort = nullptr;
    int * pInt = nullptr;
    unsigned int * pUInt = nullptr;
    float * pFloat = nullptr;
    double * pDouble = nullptr;
    string * pString = nullptr;
    BasePointersStruct();
    BasePointersStruct(const BasePointersStruct & Original);
    BasePointersStruct & operator=(const BasePointersStruct& Original);
    void clear();
    template<typename LeftType, typename RightType>
    void executeMoveTypeInstruction(LeftType * LeftOperand, const RightType * RightOperand, const EngineInstr & instruction);
    void tryToSetValue(const BaseVariableStruct & RightOperand);
    template<typename RightType>
    void moveFromTemp(const RightType * RightOperand, const EngineInstr & instruction);
    void move(const BasePointersStruct & RightOperand, const EngineInstr & instruction);
    void move(const BaseVariableStruct & RightOperand, const EngineInstr & instruction);
    template<typename LeftType, typename RightType>
    LeftType tryArithmetics(LeftType * LeftOperand, const RightType * RightOperand, const EngineInstr & instruction);
    template<typename LeftType>
    LeftType callTryArithmeticsForEveryType(LeftType * LeftOperand, const BasePointersStruct & RightOperand, const EngineInstr & instruction);
    BaseVariableStruct executeArithmetics(const BasePointersStruct &RightOperand, const EngineInstr & instruction);
    template<typename LeftType>
    LeftType callTryArithmetics(LeftType * LeftOperand, const BaseVariableStruct & RightOperand, const EngineInstr & instruction);
    BaseVariableStruct executeArithmetics(const BaseVariableStruct & RightOperand, const EngineInstr & instruction);
    void setTypeFromVariableType(char variableType);
    void setPointer(bool*);
    void setPointer(char*);
    void setPointer(short*);
    void setPointer(unsigned short*);
    void setPointer(int*);
    void setPointer(unsigned int*);
    void setPointer(float*);
    void setPointer(double*);
    void setPointer(string*);
    ReturnType setPointer(const BasePointersStruct & Pointers, const InstrDescription & CurrentInstr);
    bool areEqual(BasePointersStruct *OtherVariable);
    bool areEqual(BaseVariableStruct *OtherVariable);

    bool getBool() const;
    double getDouble() const;
    int getInt() const;
    string getString() const;
    bool isInteger() const;
    bool isNumeric() const;
};

constexpr const char* NEW_LINE_PADDING = "\t";
inline void printLogMessage(const string & messageType, const string & fileName, const size_t lineNumber, const string & functionName, const string & message){
    cerr << messageType << ":";
    if(messageType.size() < 8){
        cerr << NEW_LINE_PADDING;
    }
    cerr << "In " << fileName << ":" << uIntToStr(lineNumber) << ":" << functionName << ":\n"
        << NEW_LINE_PADDING << message;
}
string instructionError(const InstrDescription & Description, const string & functionName, const string & messageType = "Error");
string instructionWarning(const InstrDescription & Description, const string & functionName);

class PrimaryModule{
    //List of inheriting classes: ImageModule, TextModule, CollisionModule, EventModule, ParticleEffectModule, MovementModule
    protected:

    string ID;
    string layerID; //This ID is needed in events' trigger detection.
    string objectID;
    vector <string> groups;
    vec2d pos, size, scale;

    bool isActive; //Deactivated entity doesn't interact with the program, you can only activate it.
    bool deleted;
    bool isScaledFromCenter;
    bool isScrollable; //If true, the vision shift and zoom from the cameras will affect the entity. Setting this variable for the object propagates its value onto all modules. New modules copy this value from their object.
    bool canBeSelected;

    public:
    void primaryConstructor(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void primaryConstructor(unsigned int newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void clone(const PrimaryModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void setID(string newID, vector<string> & listOfIDs);
    void setLayerID(string newLayerID);
    void setObjectID(string newOwnerID);
    void setAllIDs(string newID, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);

    void addGroup(string newGroup);
    void removeGroup(string selectedGroup);
    void clearGroups();
    bool isInAGroup(string findGroup) const;
    vector <string> getGroups() const;
    vector <string> & getGroupsAddr();

    void control(AttributeType attribute, bool value, unsigned paramCount);

    void setPos(vec2d newPos);
    void setPos(double x, double y);
    void translatePos(vec2d newPos);
    void setSize(vec2d newSize);
    void setSize(double x, double y);
    void setScale(vec2d newScale);
    void setScale(double x, double y);
    void addScale(vec2d newScale);
    void addScale(double x, double y);
    void resize(vec2d newSize); //calculate a new scale vector according to new dimensions
    void resize(double newSizeX, double newSizeY);
    void resizeX(double newSizeX); //calculate scale on X axis according to new length
    void resizeY(double newSizeY); //calculate scale on Y axis according to new height
    void setIsActive(bool newValue);
    void activate();
    void deactivate();
    void toggleIsActive();
    void deleteLater();
    void setIsScaledFromCenter(bool newIsScaledFromCenter);
    void setIsScrollable(bool newIsScrollable);
    void setCanBeSelected(bool newValue);
    string getID() const;
    string &getLayerIDAddr();
    string getLayerID() const;
    string & getIDAddr();
    string getObjectID() const;
    vec2d getPos() const;
    vec2d & getPosAddr();
    vec2d getSize() const;
    vec2d & getSizeAddr();
    vec2d getScale() const;
    vec2d getScaledSize() const;
    bool getIsActive() const;
    bool getIsDeleted() const;
    bool getIsScaledFromCenter() const;
    bool getIsScrollable() const;
    bool getCanBeSelected();
    void getPrimaryContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
};

template<class SearchModule>
SearchModule * findByIDAndReturnPointer(vector <SearchModule> & Container, string ID) {
    for(SearchModule & Instance : Container) {
        if (Instance.getID() == ID) {
            return &Instance;
        }
    }
    return nullptr;
}

#endif // PRIMARYMODULE_H_INCLUDED
