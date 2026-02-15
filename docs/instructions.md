# Annotations

## @override

**Description**

    Redefine the scope of an event with the same name.

## @trigger

**Syntax**

@trigger *trigger*

**Description**

    Automatically execute this event every time a given trigger is triggered.

**Parameters:**

- trigger - a name of the event trigger. Available trigger types: on_boot, on_init, each_iteration, on_idle, each_second, on_key_press, on_key_pressing, on_key_release, on_mouse_move, when_mouse_still, on_mouse_press, on_mouse_pressing, on_mouse_release, by_objects, by_variables, by_collision, by_editables, by_movement, by_stillness, on_display_resize.

# Instructions

## start

**Syntax**

start *name* ( [[parameter]...] )

**Description**

    Start the instruction scope for the new event. Each event scope must be closed with an "end" instruction.

**Parameters:**

- name - a name for a new event;
- [parameter] (variable) - input variable for a new event.

## end

**Description**

    Close the instruction scope for the current event.

## compiler_breakpoint

**Description**

    Set a debugger breakpoint right before the next instruction. It will be triggered during the compilation of a script. If the engine is executed outside the debugger, this instruction will trigger a crash.

## breakpoint

**Description**

    Set a debugger breakpoint right before the next instruction. It will be triggered at runtime. If the engine is executed outside the debugger, this instruction will trigger a crash.

## create_display

**Syntax**

create_display *display_width* *display_height* *backbuffer_width* *backbuffer_height* [auto_scale_backbuffer]

**Description**

    Create a display.

**Parameters:**

- display_width (variable / int);
- display_height (variable / int);
- backbuffer_width (variable / int);
- backbuffer_height (variable / int);
- [auto_scale_backbuffer] (variable / bool).

## if

**Syntax**

if ( *expression* )

**Description**

    Start a new scope that will be executed if the provided expression is evaluated to true. Each if statement scope must be closed with an "end", "else_if" or "else" instruction.

**Parameters**

- expression - a list of values, variables and logical operators. Available built-in variables: fullscreen, second_passed, fps, is_directory, used_os, screen_w, screen_h, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_x, mouse_y, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, val, display_resized, display_w, display_h, number_of_processes, number_of_cameras, number_of_layers, number_of_objects, booting, exists, var, vec, Process, context, Mouse, Camera, Layer, Object, ObjectBase, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive.

## else_if

**Syntax**

else_if ( *expression* )

**Description**

    Start a new scope that will be executed only if the previous if or else_if conditions evaluate to false state and the current expression is evaluated to true. Else_if scope can be started only after an if or else_if statement. Each else_if statement scope must be closed with an "end", "else_if" or "else" instruction.

**Parameters**

- expression - a list of values, variables and logical operators arranged in Reverse Polish Notation. Available built-in variables: fullscreen, second_passed, fps, is_directory, used_os, screen_w, screen_h, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_x, mouse_y, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, val, display_resized, display_w, display_h, number_of_processes, number_of_cameras, number_of_layers, number_of_objects, booting, exists, var, vec, Process, context, Mouse, Camera, Layer, Object, ObjectBase, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive.

## else

**Syntax**

else

**Description**

    Start a new scope that will be executed only if the previous if and else_if conditions were evaluated to false. Each else statement scope must be closed with an "end" instruction.

## while

**Syntax**

while ( *expression* )

**Description**

    Start a new looping scope that will be executed as long as the provided expression is evaluated to true. Each while loop scope must be closed with an "end" instruction.

**Parameters**

- expression - a list of values, variables and logical operators arranged in Reverse Polish Notation. Available built-in variables: fullscreen, second_passed, fps, is_directory, used_os, screen_w, screen_h, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_x, mouse_y, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, val, display_resized, display_w, display_h, number_of_processes, number_of_cameras, number_of_layers, number_of_objects, booting, exists, var, vec, Process, context, Mouse, Camera, Layer, Object, ObjectBase, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive.

## run

**Syntax**

run *event* ( [[argument]...] )

**Description**

    Stop the execution of the current event and call a new one. After executing the called event return to the calling event and resume its execution.

**Parameters:**

- event - a name of an event to be executed between other instructions;
- [argument] (variable) - a value that will be passed to the called event.

## exit

**Description**
   
    Exit the engine.

## reboot

**Description**

    Restart the process. All boot-triggered events will be executed in the next engine iteration.

## return

**Description**   

    Exit the current event and return to the calling event.

## break
   
**Description**

    Exit the current while loop and resume the execution one line after the next "end" instruction.

## continue
   
**Description**

    Stop the execution of the while loop scope and move to the beginning of the current loop.

## delete_this_event
   
**Description**

    Interrupt the execution of the current scope and delete the current event.

## dump_memory
   
**Description**

    Print all variables of the current object to the standard output. 

## dump_local_memory
   
**Description**

    Print all local (current scope) variables to the standard output. 

## all / first / last / random

**Syntax**

*aggregation_type* *entities* [camera_id] [layer_id] [object_id] [module_type] [module_id] [attribute] [expression] [output]

**Description**

    Return entities that fulfils the boolean expression and optional parameters.

**Parameters:**

- aggregation_type (instruction): “all” - return every entity that fullfils all conditions, “first” - return only the first entity that fullfils all conditions, “last” - return only the last entity that fullfils all conditions, “random” - return one random entity that fullfils all conditions;
- entities (variable) - a variable of one of these types: val, vec, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive. Type of the entity affects other parameters;
- [camera_id] (string) - if the provided variable is of a Camera type, only the camera with this id can be aggregated;
- [layer_id] (string) - if the provided variable is of a Layer type, only the layer with this id can be aggregated;
- [object_id] (string) - if the provided variable is of a Layer or object type, only the object with this id can be aggregated;
- [module_type] (string) - if the provided variable contains layers, objects or modules; only the module of this type can be aggregated;
- [module_id] (string) - if the provided variable contains layers, objects or modules; only the module with this id can be aggregated;
- [attribute] (string) - if provided, aggregate this attribute from the provided entities;
- [expression] - a list of values, variables and logical operators arranged in Reverse Polish Notation. This whole expression is calculated for each entity separately and if it returns truth, entity is aggregated into the output variable. But if expression is empty, this boolean is equal to true by default. Available built-in variables: fullscreen, second_passed, fps, is_directory, used_os, screen_w, screen_h, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_x, mouse_y, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, val, display_resized, display_w, display_h, number_of_processes, number_of_cameras, number_of_layers, number_of_objects, booting, exists, var, vec, Process, context, Mouse, Camera, Layer, Object, ObjectBase, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- [output] (variable) for all entities that fullfill all conditions.

## index

**Syntax**

index *entities* [indexes] [attribute] [output]

**Description**

    Take a list of indexes and return the entities or values located at those indexes.

**Parameters:**

- entities (variable) - a variable that stores entities; 
- [indexes] (variable / variable vector / int vector) - a list of indexes used to find entities or values;
- [attribute] (variable) - if provided, aggregate this attribute from all selected entities;
- [output] (variable) for entities and values.

## index_vec

**Syntax**

index_vec *vector* *index* [output]

**Description**

    Return an element of the vector based on the index. (It's much faster than "index" instruction, but not well-tested.)

**Parameters:**

- vector (variable) - a variable that stores a vector; 
- index (variable / int vector) - an element to return;
- [output] (variable).

## sum_of

**Syntax**

sum_of *left* *right* [output]

**Description**

    Take a pair of variables and return the sum of contained entities.

**Parameters**

- left (variable) - a variable of one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- right (variable) - a variable of one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- [output] (variable).

## intersection

**Syntax**

intersection *left* *right* [output]

**Description**

    Take a pair of variables and return the intersection of contained entities.

**Parameters**

- left (variable) - a variable of one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- right (variable) - a variable of one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- [output] (variable).

## difference

**Syntax**

difference *left* *right* [output]

**Description**

    Take a pair of variables and return the difference of contained entities.

**Parameters**

- left (variable) - a variable of one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- right (variable) - a variable of one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- [output] (variable).

## access

**Syntax**

access *output* [[*source*]...]

**Description**

    Access the values from the provided sources and put them inside a variable.

**Parameters**

- output (variable);
- [source] - a value or variable. Available built-in variables: ffullscreen, second_passed, fps, is_directory, used_os, screen_w, screen_h, key_pressed, key_pressing, key_released, any_key_pressed, any_key_pressing, any_key_released, mouse_x, mouse_y, mouse_moved, mouse_pressed, mouse_pressing, mouse_released, val, display_resized, display_w, display_h, number_of_processes, number_of_cameras, number_of_layers, number_of_objects, booting, exists, var, vec, Process, context, Mouse, Camera, Layer, Object, ObjectBase, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive.

## bool / int / double / string

**Syntax**

*literal_type* *output* *values*

**Description**

    Declare a variable with provided values. If the input values are variables, cast them to the correct type when possible. Casting is done during runtime.

**Parameters**

- literal_type (instruction): bool, int, double, string;
- output (variable);
- values - a variable, vector of variables, literal or vector of literals.

## bool[] / int[] / double[] / string[]

**Syntax**

*literal_type* [] *output* *values*

**Description**

    Declare a dynamic list/vector/array with provided values. If the input values are variables, cast them to the correct type when possible. Casting is done at runtime.

**Parameters**

- literal_type (instruction): bool, int, double, string;
- output (variable);
- values - a variable, vector of variables, literal or vector of literals.

## var

**Syntax**

var *output* *value*

**Description**

    Create a new variable for the current event's owner.

**Parameters**

- output (variable);
- value (variable / any literal).

## vec

**Syntax**

vec *type* *output* [values]

**Description**

    Create a vector of variables for the current event's owner.

**Parameters**

- type (variable): bool, int, double, string;
- output (variable);
- [values] (variable / variable vector / any literal vector).

## Camera / Layer / Object / TextField / EditableText / Image / Movement / Collision / Particles / Event / Scrollbar / Primitive

**Syntax**

*type* *name*

**Description**

    Create an empty pointer of the specified type. You can later assign an instance of this type to a variable with the provided name.

**Parameters**

- type (instruction): Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- name (variable).

## random_int

**Syntax**

random_int *min* *max* [output]

**Description**

    Return a random integer from the range provided in the pair of variables or literals.

**Parameters**

- min (variable / int) - minimal value of the output;
- max (variable / int) - maximal value of the output;
- [output] (variable).

## find_by_id

**Syntax**

find_by_id *source* [context_list] [camera_id] [layer_id] [object_id] [module_type] [module_id] [attribute] [output]

**Description**

    Return a context filled with entities which ids are equal to ones provided in parameters. If no ids are provided, aggregate all possible entities.

**Parameters**

- source (variable): Camera, Layer, context, “_” - if a context is not provided, choose one of the highest types of abstractions. Each camera contains its attributes and a bitmap (screen/window), while each layer contains its attributes and objects;
- [context_list] (variable vector) - context ids will be used when source is equal to “context” or “_”. Each context can have one of these types: val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive. Type of the context affects other parameters;
- [camera_id] (string) - if source is Camera or provided context is of a camera type, only the camera with this id can be aggregated;
- [layer_id] (string) - if source is Layer or provided context is of a layer type, only the layer with this id can be aggregated;
- [object_id] (string) - if source is Layer or provided context is of a layer or object type, only the object with this id can be aggregated;
- [module_type] (string) - if source is Layer, or provided context contains layers, objects or modules; only the module of this type can be aggregated;
- [module_id] (string) - if source is Layer, or provided context contains layers, objects or modules; only the module with this id can be aggregated;
- [attribute] (variable) - if provided, aggregate this this attribute from selected entities;
- [output] (variable).

## find_by_id layer

**Syntax**

find_by_id layer *layer_id* *object_id* *module_type* *module_id* *attribute* [output]

**Description**

    Return a variable filled with entities which ids are equal to ones provided in the parameters. If no ids are provided, aggregate all possible entities.

**Parameters**

- [layer_id] (string) - only the layer with this id can be aggregated;
- [object_id] (string) - only the object with this id can be aggregated;
- [module_type] (string) - only the module of this type can be aggregated;
- [module_id] (string) - only the module with this id can be aggregated;
- [attribute] (string) - aggregate this attribute from selected entities;
- [output] (variable).

## find_by_id_2

**Syntax**

find_by_id_2 *source* *id* [output]

**Description**

    Return the entity from the "source" variable with an id equal to the second parameter. Only an entity of the same type as the "source" variable can be accessed.

**Parameters**

- source (variable) - vector of entities,
- id (variable / string) - instruction will return the entity with this id,
- [output] (variable).

## clone

**Syntax**

clone *left* *right* *changeOldID*

**Description**

    Clone the values and structure of the right context to the left context. 

**Parameters**

- left (variable): val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- right (variable): val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive;
- changeOldID (bool) - if true, the left variable will inherit the id of the right variable (with the last number incrementation).

## +, -, *, /

**Syntax**

*operator* *left* *right* [output]

**Description**

    Return a context with a result of an arithmetic operation on the pair of contexts (with numeric values).

**Parameters**

- operator (instruction): "+", "-", "*", "/";
- left (variable / any literal) - this instruction accepts these two variable types: ptr and val;
- right (variable / any literal) - this instruction accepts these two variable types: ptr and val;
- [output] (variable).

## ++, --

**Syntax**

*operator* *context*

**Description**

    Increment or decrements the numeric value found in the context by 1.

**Parameters**

- operator (variable): "++" - increment by 1, "--" - decrement by 1;
- context (variable) - this instruction accepts these two types: ptr and val.

## =, +=, -=, *=, /=

**Syntax**

*operator* *left* *right*

**Description**

    Move a numeric value from the right context to the left context based on the choosen operator.

**Parameters**

- operator (variable): "=", "+=", "-=", "*=", "/=" - these work exactly like in c++;
- left (variable) - variable. This instruction accepts these two context types: ptr and val;
- right (variable / any literal) - variable. This instruction accepts these two context types: ptr and val.

## in

**Syntax**

in *left* *right* [output]

**Description**

    Return true if any entity from the left context occurs in the right context. In case of numeric values, compare their values. For other entities like layers, compare their ids, but only if both entities are of the same type.

**Parameters**

- left (variable) - variable. This instruction accepts these two context types: ptr and val;
- right (variable) - variable. This instruction accepts these two context types: ptr and val;
- [output] (variable).

## new

**Syntax**

new *type* [layer_id] [object_id] [quantity] [new_ids] [output]

**Description**

    Create new entities and return a context with their pointers.

**Parameters**

- type/source (variable): val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive - the type of the new entities;
- layer_id/[layer_id] (string) - the id of the layer with the role of a container for new objects. Required only if the new entities are objects;
- object_id/[object_id] (string) - the id of the object with the role of a container for new modules. Required only if the new entities are modules;
- [quantity] (variable / int) - the number of new entities that will be created. A literal or an variable of the ptr or val type.
- [new_ids] (variable / vector variable / string / string vector) - the list of strings that will become the new identificators for new objects. You can provide any number of ids and if the number is too low, the last id will be repeated. Although, to ensure the uniqueness in the given container, indexing numbers will be automatically added to ends of repeated ids and incremented by one for every next repetition. The same automatic action will be taken if no new ids are provided;
- [output] (variable).

## new

**Syntax**

new *type* [destination] [quantity] [new_ids] [output]

**Description**

    Create new entities and return a context with their pointers.

**Parameters**

- type/source (variable): val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive - the type of the new entities;
- destination/[destination] (variable) - variable with a container for new entities;
- [quantity] (variable / int) - the number of new entities that will be created. A literal or an variable of the ptr or val type.
- [new_ids] (variable) - the variable with the type ptr or val. This context provides the list of strings that will become the new identificators for new objects. You can provide any number of ids and if the number is too low, the last id will be repeated. Although, to ensure the uniqueness in the given container, indexing numbers will be automatically added to ends of repeated ids and incremented by one for every next repetition. The same automatic action will be taken if no new ids are provided;
- [output] (variable).

## delete

**Syntax**

delete *context*

**Description**

    Delete all entities provided in the context.

**Parameters**

- context (variable): val, vec, var, ptr, Camera, Layer, Object, TextField, EditableText, Image, Movement, Collision, Particles, Event, Scrollbar, Primitive - variable with entities selected for deletion.

## bind

**Syntax**

bind *objects* *scripts*

**Description**

    Add paths to scripts from objects.

**Parameters**

- objects (variable) - objects that will be binded with provided scripts;
- scripts (variable / string vector) - paths to the scripts.

## rbind

**Syntax**

rbind *objects*

**Description**

    Remove paths to scripts from objects.

**Parameters**

- objects (variable).

## build

**Syntax**

build *objects* [reset] [do_not_preserve]

**Description**

    Translate all scripts binded to objects provided in the context and create events based on those scripts.

**Parameters**

- objects (variable) - variable with objects intended for event building;
- [reset] (variable / bool) - false by default. If true, before creating new events this instruction removes all events from the provided objects;
- [do_not_preserve] (variable / bool) - if true, instruction can remove events of its owner.

## build_subset

**Syntax**

build_subset *objects* *paths* [reset] [do_not_preserve]

**Description**

    Translate the subset of scripts binded to objects provided in the context and create events based on selected scripts.

**Parameters**

- objects (variable) - variable with objects intended for event building,
- paths (variable / variable vector / string vector) - vector of paths to the scripts;
- [reset] (variable / bool) - false by default. If true, before creating new events, instruction removes all events from provided objects;
- [do_not_preserve] (variable / bool) - if true, instruction can remove events of its owner.

## load_build

**Syntax**

load_build *objects* *paths* [reset] [do_not_preserve]

**Description**

    Load scripts from provided paths and translate them into events for selected objects.

**Parameters**

- objects (variable) - variable with objects intended for event building;
- paths (variable / variable vector / string vector) - vector of paths to the scripts;
- [reset] (variable / bool) - if true, before creating new events, instruction removes all events from provided objects;
- [do_not_preserve] (variable / bool) - if true, instruction can remove events of its owner.

## inject_code

**Syntax**

inject_code *objects* *code* [reset] [do_not_preserve]

**Description**

    Translate provided code into events for all selected objects. Provided code must have correct syntax.

**Parameters**

- objects (variable) - variable with objects intended for event building,
- code (variable / variable vector / string vector) - vector of code lines;
- [reset] (variable / bool) - if true, before creating new events, instruction removes all events from provided objects;
- [do_not_preserve] (variable / bool) - if true, instruction can remove events of its owner.

## inject_instr

**Syntax**

inject_instr *objects* *instructions* [reset] [do_not_preserve]

**Description**

    Translate provided instructions into an event and insert it into all selected objects. Created events will be executed as soon as possible and will be deleted right afterwards.

**Parameters**

- objects (variable) - variable with objects intended for event building,
- instructions (variable / variable vector / string vector) - vector with instructions;
- [reset] (variable / bool) - if true, before creating new events, instruction removes all events from provided objects;
- [do_not_preserve] (variable / bool) - if true, instruction can remove events of its owner.

## demolish

**Syntax**

demolish *objects*

**Description**

    Destroy all events from provided objects. Objects cannot include the owner of the currently executed event.

**Parameters**

- objects (variable) - variable with objects intended for event building.

## env

**Syntax**

env *attribute* *value* [value]

**Description**

    Change the value of an environmental variable.

**Parameters**

- attribute (variable): window_title, display_size, fullscreen, pixel_art, draw_text_borders, draw_hitboxes, ignore_distant, draw_only_visible, bitmap_layers_number, print_logical_evaluations, print_instructions, reservation_multiplier, can_afk_timeout, afk_timeout_time, can_exit_on_idle - the name of the variable selected for modification;
- value (variable / any literal) - a new value for the selected attribute.

## edit_proc

**Syntax**

edit_proc *attribute* [value] [value] [value] [value]

**Description**

    Modify an attribute of the current process.

**Parameters**

- attribute (variable): is_active, can_interact_with_user, is_rendering, draw_camera_borders, draw_text_borders, draw_hitboxes, ignore_distant, draw_only_visible, bitmap_layers_number, print_logical_evaluations, print_instructions, auto_print_stack, reservation_multiplier, window_pos, window_size, min_window_size, window_tint - the name of the variable selected for modification,
- [value] (variable / any literal) - a new value for the selected attribute.

## new_proc

**Syntax**

new_proc *name* [layer] [object] [script]

**Description**

    Create a new process.

**Parameters**

- name (variable / string) - a name for the new process,
- [layer] (variable / string) - a name for the first layer of the new process,
- [object] (variable / string) - a name for the first object of the new process,
- [script] (variable / string) - a file path to the initialization script for the new process.

## fun

**Syntax**

fun *objects* *attribute* [[value]...]

**Description**

    Execute a function for all objects in the context. You can add as many type-value pairs as needed.

**Parameters**

- objects (variable) - variable with objects;
- attribute (variable) - name of the function intended for execution;
- value (variable / any literal) - variable name or a list of values.

## load_bitmap

**Syntax**

load_bitmap *path* *name* [light] [ignore_warnings]

**Description**

    Load a bitmap from a file path to the engine's RAM.

**Parameters**

- path (variable / string) - path to an image file;
- name (variable / string) - name for the new bitmap;
- [light] (variable / bool) - if true, create a light bitmap for the loaded bitmap. False by default;
- [ignore_warnings] (variable / bool) - if true, warnings about loading the same image will not be printed.

## load_font

**Syntax**

load_font *path* *size* *name* [ignore_warnings]

**Description**

    Load a font from a file path to the engine's memory.

**Parameters**

- path (variable / string) - a path to a font file;
- size (variable / int) - a size of the new font;
- name (variable / string) - a name for the new font;
- [ignore_warnings] (variable / bool) - if true, warnings about loading the same font will not be printed.

## mkdir

**Syntax**

mkdir *path*

**Description**

    Create a new directory in the filesystem of the host system. A new directory can be created only under the directory of the engine's directory. 

**Parameters**

- path (variable / string) - path to the directory.

## rm

**Syntax**

rm *path*

**Description**

    Remove a file or an empty directory in the selected path. 

**Parameters**

- path (variable / string) - path to the file or directory.

## rmll

**Syntax**

rmll *path*

**Description**

    Remove a file or directory in the selected path using recurrency. 

**Parameters**

- path (variable / string) - path to the file or directory.

## mv

**Syntax**

mv *path* *new_path*

**Description**

    Move a file or directory to a new path. 

**Parameters**

- path (variable / string) - path to the file or directory,
- new_path (variable / string) - a new path to the file or directory.

## print

**Syntax**

print [[value]...]

**Description**

    Print the list of values to the standard output.

**Parameters**

- [value] (variable / any literal) - variable name or a literal that will be printed.

## print_d

**Syntax**

print_d [delimeter] [[value]...]

**Description**

    Print the list of values separated by a delimeter to the standard output.

**Parameters**

- [delimeter] (variable / string) - text printed after each printed value. If it's the only passed argument, delimeter will be printed once;
- [value] (variable / any literal) - variable name or a literal that will be printed.

## print_v

**Syntax**

print [output] [delimeter] [[value]...] 

**Description**

    Print the list of values to the standard output or put them inside a string variable.

**Parameters**

- [output] (variable) - if equal to "_", print values to the stdout, otherwise print values to a variable;
- [delimeter] (variable / string) - text printed after each printed value. If it's the only passed argument, delimeter will be printed once;
- [value] (variable / any literal) - variable or a literal that will be printed.

## load_text

**Syntax**

load_text *path* [output]

**Description**

    Load a text from a file and move it into a string variable.

**Parameters**

- path (variable / string) - variable or a string literal with the path to a file;
- [output] (variable) - output for the loaded text.

## save_text

**Syntax**

save_text *path* *text* [delimeter]

**Description**

    Save a string value to a text file.

**Parameters**

- path (variable / string) - a path to a text file;
- text (variable / string / vector string) - a text that will be saved to a file;
- [delimeter] (variable / string) - it will be added to the end of each string from the text parameter.

## ls

**Syntax**

ls [path] [output] [recursive] [max_depth]

**Description**

    Print out the list of files in the directory.

**Parameters**

- [path] (variable / string) - a path to a directory;
- [output] (variable);
- [recursive] (variable / bool) - if true, list the files recursively;
- [max_depth] (variable / int) - max depth of recursive file search.

## lse

**Syntax**

lse *source* [detail]

**Description**

    Print the list of entities from the source to standard output.

**Parameters**

- source (variable): processes, layers, cameras, bitmaps, fonts;
- [detail] (variable / bool) - if true, prints items in the table with details.

## tokenize

**Syntax**

tokenize *delimeter* *text* [[output]...]

**Description**

    Create a new variable for the current event's owner.

**Parameters**

- delimeter (variable / char) - character that will divide the provided string to seperate tokens;
- text (variable / text) - string to be tokenize;
- [output] (variable).

## tree

**Syntax**

tree [output]

**Description**

    Print a tree of entities to the standard output or put it inside the string variable.

**Parameters**

- [output] (variable).

## len

**Syntax**

len *text* [output]

**Description**

    Return the length of the provided string.

**Parameters**

- text (variable / string) - text to be measured;
- [output] (variable).

## sizeof

**Syntax**

sizeof *vector* [output]

**Description**

    Return the size of the vector from the provided variable.

**Parameters**

- vector (variable);
- [output] (variable).

## substr

**Syntax**

substr *text* *begin* *length* [output]

**Description**

    Return the sub-string from the provided text.   

**Parameters**

- text (variable / string);
- begin (variable / int) - the index at which the substring begins in the text;
- length (variable / int) - the length of the sub-string;
- [output] (variable).

## restart_drag

**Description**

    Recalculate the starting position of the selected camera dragging.

## cd

**Syntax**

cd [path]

**Description**

    Change the current working directory.

**Parameters**

- [path] (variable / string) - path to the directory that will be added to the end of the working directory.

## pwd

**Syntax**

pwd [output]

**Description**

    Print working directory.

**Parameters**

- [output] (variable).

## similar

**Syntax**

similar *pattern* *vector* [longest_common_part] [output]

**Description**

    Return a vector of strings that start with the provided pattern or a string that is the longest common part of all similar strings.

**Parameters**

- pattern (variable / string) - pattern that will be compared to every string in the vector;
- vector (variable / vector variable / vector string) - vector of strings to be compared;
- [longest_common_part] (variable / bool) - if true, the output will return the longest common part within the similar strings;
- [output] (variable) - if the "longest_common_part" parameter is false or empty, return strings that start with the same pattern.

## count

**Syntax**

count *pattern* *text* [output]

**Description**

    Count occurrences of the pattern inside the text.

**Parameters**

- pattern (variable / string);
- text (variable / string);
- [output] (variable).


## console_input

**Syntax**

console_input [output]

**Description**

    Halt the engine execution and wait for the user input from the standard input stream (console).

**Parameters**

- [output] (variable) - output for the user input.


## start_timer

**Syntax**

start_timer *name*

**Description**

    Start the timer with the provided name.

**Parameters**

- name (variable / string) - name for a new timer.


## stop_timer

**Syntax**

stop_timer *name* [output]

**Description**

    Stop the timer with the provided name.

**Parameters**

- name (variable / string) - name for the new timer,
- [output] (variable).


## assert

**Syntax**

assert *left* *right*

**Description**

    If the left value is not equal to the right value return an error and exit the engine.

**Parameters**

- left (variable / any literal) - this instruction accepts these two variable types: ptr and val;
- right (variable / any literal) - this instruction accepts these two variable types: ptr and val;
- [output] (variable).

## type

**Syntax**

type *variable* *output*

**Description**

    Set output to the type of the input variable as a string value.

**Parameters**

- variable (variable);
- output (variable).

## load

**Syntax**

load *output* *address*

**Description**

    Assign the value from the specified address to the output variable.

**Parameters**

- output (variable);
- address (variable / any literal) - address of the input variable.