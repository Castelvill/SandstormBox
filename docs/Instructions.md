## break
   
**Description**

    Interrupts the execution of the current scope.

## return

**Description**   

    Interrupts the execution of all events from the object.

## reboot

**Description**

    Restarts the engine. All boot triggered events will be executed in the next engine iteration.

## power_off

**Description**
   
    Exits the program.

## *aggregation* *source* [contextIDs] [cameraID] [layerID] [objectID] [moduleType] [moduleID] [attribute] [expression] [newContextID]

**Description**

    Returns a context that fulfils the boolean expression and optional parameters.

**Parameters:**

- aggregation (string): “all” - returns whole context, “first” - returns the first context, “last” - returns the last context, “random” - returns one random context;
- source (string): “camera”, “layer”, “context”, “_” - if context ids are not provided, choose one of the highest types of abstractions. Each camera contains its attributes and a bitmap (screen/window), while each layer contains its attributes and objects;
- [optional] contextIDs (vector*string*) - context ids will be used when source is equal to “context” or “_”. Each context can have one of these types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”. Type of the context affects other parameters;
- [optional] expression (vector*ConditionClass*) - the list of locations of values and the list of operators that create a relationship between those values, which in turn results in a single boolean value. This whole expression is calculated for each entity separately and if it returns truth, entity is aggregated into the new context. But if expression is empty, this boolean is equal to true by default;
- [optional] cameraID (string) - if source is equal to “camera” or provided context is of a camera type, only the camera with this id can be aggregated;
- [optional] layerID (string) - if source is equal to “layer” or provided context is of a layer type, only the layer with this id can be aggregated;
- [optional] objectID (string) - if source is equal to “layer” or provided context is of a layer or object type, only the object with this id can be aggregated;
- [optional] moduleType (string) - if source is equal to “layer”, or provided context contains layers, objects or modules; only the module of this type can be aggregated;
- [optional] moduleID (string) - if source is equal to “layer”, or provided context contains layers, objects or modules; only the module with this id can be aggregated;
- [optional] attribute (string) - if provided, aggregate this attribute from selected entities;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## index *source* [indexes] [contextIDs] [attribute] [newContextID]

**Description**

    takes a list of indexes and returns a context with entities found by indexes.

**Parameters:**

- source (string): “camera”, “layer”, “context”, “_”  - if source is equal to “camera” or “layer”, contexts from contextIDs are treated as indexes.
- indexes [optional] (vector*int/unsigned*) - the list of indexes used to find entities in vectors;
- contextIDs [optional] (vector*string*): “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value” - if source is not provided, the first id serves as the container for index search. The rest of ids, no matter the source, are treated as indexes - they have to be of “pointer” or “value” type.
- attribute [optional] (string) - if provided, aggregate this attribute from selected entities;
- newContextID [optional] (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## sum *contextID* *contextID* [newContextID]

**Description**

    takes a pair of contexts and returns the sum of these sets.

**Parameters**

- contextID (string) - id of the selected context. This instruction accepts all types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## intersection *contextID* *contextID* [newContextID]

**Description**

    takes a pair of contexts and returns the intersection of these sets.

**Parameters**

- contextID (string) - id of the selected context. This instruction accepts all types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## difference *contextID* *contextID* [newContextID]

**Description**

    takes a pair of contexts and returns the difference of these sets.

**Parameters**

- contextID (string) - id of the selected context. This instruction accepts all types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## value *expression* [newContextID]

**Description**

    returns a context made out of given literals or values found in different sources.

**Parameters**

- expression (vector*ConditionClass*) - the list of locations of values and literals. Each value has one of these sources: “context”, “camera”, “layer”, “object”, “variable”, “literal”, “on_boot”, “second_passed”, “key_pressed”, “key_pressing”, “key_released”, “any_key_pressed”, “any_key_pressing”, “any_key_released”, “mouse_moved”, “mouse_pressed”, “mouse_pressing”, “mouse_released”, “window_w”, “window_h”. If you choose “literal”, you can provide literals from these types: bool, int, double, string.
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## *literal* *literals* [newContextID]

**Description**

    returns a context filled with provided literals.

**Parameters**

- literal *string*: “bool”, “int”, “double”, “string”;
- literals (vector*VariableModule*): bool, int, double, string;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## random_int literal *literal* *literal* [newContextID]

**Description**

    returns a random integer from the range provided in the pair of literals.

**Parameters**

- literal (VariableModule): bool, int, double, string;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## random_int context *contextID* *contextID* [newContextID]

**Description**

    returns a list of random integers from the ranges provided in the pair of contexts.

**Parameters**

- contextID (string) - id of the choosen context. Context types allowed by this instruction are: “value” and “pointer”;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## find_by_id *source* [contextIDs] [cameraID] [layerID] [objectID] [moduleType] [moduleID] [attribute] [newContextID]

**Description**

    returns a context filled with entities which ids are equal to ones provided in parameters. If no ids are provided, aggregates all possible entities.

**Parameters**

- source (string): “camera”, “layer”, “context”, “_” - if a context is not provided, choose one of the highest types of abstractions. Each camera contains its attributes and a bitmap (screen/window), while each layer contains its attributes and objects;
- [optional] contextIDs (vector*string*) - context ids will be used when source is equal to “context” or “_”. Each context can have one of these types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”. Type of the context affects other parameters;
- [optional] cameraID (string) - if source is equal to “camera” or provided context is of a camera type, only the camera with this id can be aggregated;
- [optional] layerID (string) - if source is equal to “layer” or provided context is of a layer type, only the layer with this id can be aggregated;
- [optional] objectID (string) - if source is equal to “layer” or provided context is of a layer or object type, only the object with this id can be aggregated;
- [optional] moduleType (string) - if source is equal to “layer”, or provided context contains layers, objects or modules; only the module of this type can be aggregated;
- [optional] moduleID (string) - if source is equal to “layer”, or provided context contains layers, objects or modules; only the module with this id can be aggregated;
- [optional] attribute (string) - if provided, aggregate this this attribute from selected entities;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## let *contextID* *newContextID*

**Description**

    creates a variable with provided id from the selected or last existing context or overwrites the context of the existing variable with the same id. New variables will exist only in the current scope.

**Parameters**

- contextID (string) - id of the context. Context can have one of these types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”;
- newContextID (string) - id of the new variable or the id of existing variable intended for a context overwrite.

## clone *contextID* *contextID* [changeOldID]

**Description**

    clones the values and structure of the right context to the left context. 

**Parameters**

- contextID (string) - id of the context. Context can have one of these types: “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar", “pointer”, “value”;
- [optional] changeOldID (bool) - if true, the left context will inherit the id of the right context (with the last number incrementation). True by default.

## *operator* *contextID* *contextID* [newContextID]

**Description**

    returns a context with a result of an arithmetic operation on the pair of contexts (with numeric values).

**Parameters**

- operator (string): "+", "-", "*", "/";
- contextID (string) - id of the context. This instruction accepts these two context types: “pointer” and “value”;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## *operator* *contextID*

**Description**

    increments or decrements the numeric value found in the context by 1.

**Parameters**

- operator (string): "++" - increment by 1, "--" - decrement by 1;
- contextID (string) - id of the context. This instruction accepts these two context types: “pointer” and “value”.

## *operator* *contextID* *contextID*

**Description**

    moves a numeric value from the right context to the left context based on the choosen operator.

**Parameters**

- operator (string): "=", "+=", "-=", "*=", "/=" - these work exactly like in c++;
- contextID (string) - id of the context. This instruction accepts these two context types: “pointer” and “value”.

## in *contextID* *contextID* [newContextID]

**Description**

    returns true if any entity from the left context occurs in the right context. In case of numeric values, instruction compares their values. For other entities like layers, instruction compares their ids, but only if both entities are of the same type.

**Parameters**

- contextID (string) - id of the context. This instruction accepts these two context types: “pointer” and “value”;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## new *type* location/l [layerID] [objectID] [size1] [size2] [newIDs] [newContextID]

**Description**

    creates new entities and returns a context with their pointers.

**Parameters**

- type/source (string): “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar" - the type of the new entities;
- [semi-optional] layerID (string) - the id of the layer with the role of a container for new objects. Required only if the new entities are objects;
- [semi-optional] objectID (string) - the id of the object with the role of a container for new modules. Required only if the new entities are modules;
- [optional] size1 (int) - this is the number of new entities that will be created. If not provided, operation will try to get this number from size2 *parameter*;
- [optional] size2 (string) - id of the context of the type “pointer” or “value”. This is the number of new entities that will be created;
- [optional] newIDs (string) - the id of the context with the type “pointer” or “value”. This context provides the list of strings that will become the new identificators for new objects. You can provide any number of ids and if the number is too low, the last id will be repeated. Although, to ensure the uniqueness in the given container, indexing numbers will be automatically added to ends of repeated ids and incremented by one for every next repetition. The same automatic action will be taken if no new ids are provided;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## new *type* context/c [destination] [size1] [size2] [newIDs] [newContextID]

**Description**

    creates new entities and returns a context with their pointers.

**Parameters**

- type/source (string): “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar" - the type of the new entities;
- [semi-optional] destination (string) - id of the context with a container for new entities;
- [optional] size1 (int) - this is the number of new entities that will be created. If not provided, operation will try to get this number from size2 *parameter*;
- [optional] size2 (string) - id of the context of the type “pointer” or “value”. This is the number of new entities that will be created;
- [optional] newIDs (string) - the id of the context with the type “pointer” or “value”. This context provides the list of strings that will become the new identificators for new objects. You can provide any number of ids and if the number is too low, the last id will be repeated. Although, to ensure the uniqueness in the given container, indexing numbers will be automatically added to ends of repeated ids and incremented by one for every next repetition. The same automatic action will be taken if no new ids are provided;
- [optional] newContextID (string) - giving an id to a new context creates a variable in the current scope or overwrites the context of an existing variable with the same id.

## delete *contextID*

**Description**

    deletes all entities provided in the context.

**Parameters**

- contextID (string): “camera”, “layer”, “object”, “text”, "editable_text", "image", "movement", "collision", "particles", "event", "variable", "scrollbar" - id of the context with entities selected for deletion.

## bind *contextID* *source* *scriptPaths*

**Description**

    adds or removes paths to scripts from objects.

**Parameters**

- contextID (string) - id of the context with objects;
- source (string):
	“context” or “c” - next parameter will require an id of the context with paths to scripts. Instruction will bind those paths to provided objects;
	“literal” or “l” - next parameter will require a list of string values - script paths. Instruction will bind those paths to provided objects;
	“remove_context”, “rcontext” or “rc” - next parameter will require an id of the context with paths to scripts. Instruction will remove those paths from provided objects;
	“remove_literal”, “rliteral” or “rl” - next parameter will require an id of the context with paths to scripts. Instruction will remove those paths from provided objects;
	“reset” - instruction will remove all binded paths from provided objects.
- scriptPaths (string / vector*string*) - paths to scripts, their purpose depends from “source” parameter.

## build *contextID* [reset]

**Description**

    instruction translates all scripts binded to objects provided in the context and creates events based on those scripts.

**Parameters**

- contextID (string) - id of the context with objects intended for event building;
- [optional] reset (bool) - if true, before creating new events, instruction removes all events from provided objects.

## env *name* *value* [value]

**Description**

    changes the value of environmental variable.

**Parameters**

- name (string): window_title, window_size, fullscreen, pixel_art, draw_text_borders, draw_hitboxes, ignore_distant, draw_only_visible, bitmap_layers_number, print_logical_evaluations, print_instructions, reservation_multiplier - the name of the variable selected for modification;
- value (VariableModule): bool, int, double, string - the new value of the selected variable.

## fun *contextID* *attribute* [[type] [value] …]

**Description**

    executes a function for all objects in the context. You can add as many type-value pairs as needed.

**Parameters**

- contextID (string) - id of the context with objects;
- attribute (string) - name of the function intended for execution;
- type (string): context, c, bool, int, double, string - type of the next parameter;
- value (string / vector*VariableModule*) - id of the context with values or a list of values.
