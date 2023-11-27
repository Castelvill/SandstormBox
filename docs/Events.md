# Goo scripting language description:

1. Adding a script to an object does nothing. In order to use it, first you need to prompt the Goo Interpreter to translate your scripts into events with “build” instruction.
2.	All instruction in the first level of the script are translated into events that will be executed every iteration – so called default events. Each if statement in the script creates a conditional/triggerable event – instructions placed inside the scope of these statements are executed only if all conditions in the if statement are fulfilled. Nested if and else statements, while also being translated into different events, are connected with each other and will be executed in the order given in the script – using interrupts if needed.

    Using “event [name]\(\)” statement you can create dormant events with a name set to a given string. Dormant events won’t be executed in every loop nor in a reaction to normal triggers. They are an equivalent of functions from normal programming languages and you can give them parameters by putting them in the parenthesis. There’s a major difference between these events and functions – events cannot return values, you must use variables belonging to the owner of the script if you want to save results of the event. Dormant events can be called manually in the terminal or in a different event using run() method. You can improve readability of your script by combining conditional events with dormant events. Examples:
	    
        event(“sleepy”){ print(“Zzz…\n”) }; //A line from a script of the object with “bob” id.

        objects.bob.events.sleepy.run();

3. Most methods or accessors need a context based on one of these templates:

        [subject].[method/variable] – accessing methods and variables of the subjects: layers, cameras, mouse and env(environment);

        cameras.[camera_id].[method/variable];
        
        layers.[layer_id].[method/variable];
        
        layers.[layer_id].[object_id].[method/variable];
        
        layers.[layer_id].[object_id].[module].[method/variable];
        
        layers.[layer_id].[object_id].[module].[element_id].[method/variable];
	For example: 
        
        layers.first.patrick.var.apples = 10; //In the “first” layer find an object with an id “patrick” and set its variable named “apples” to 10.
    You can shorten “layers.[layer_id].[object_id]” using built-in variable objects like this:
        
        objects.[object_id].[method/variable];
        
        objects.[object_id].[module].[method/variable];
        
        objects.[object_id].[module].[element_id].[method/variable];
	When accessing the owner of the script, you can use built-in variable me like this:
		
        me.var.money += 50;
	Under the hood, accessing the context by id in this way uses aggregation command first:
        
        cameras.[camera_id]  first(cameras.id == [camera_id]);
        
        layers.[layer_id]  first(layers.id == [layer_id]);
        
        objects.[object_id]  first(objects.id == [object_id]).

4.	Values or contexts can be assigned to a variable using let statement:
        
        let number = 10;
        
        let string = “hello Goo”;
        
        let a = layers.background;
        
        let bob = objects.bob;
        
        let money = objects.bob.var.money;

5.	In order to simultaneously access the context of a group of objects use aggregation methods: first, last, all, random. Aggregation templates:
        
        [context].[aggregation]([…].[method/variable] [comparison] [value]);
        
        [context]. [aggregation]([…].[method/variable] [comparison]… […].[method/variable]);
	You can assign aggregated context to a variable using let statement.
	
    Examples of aggregation:
        
        objects.first(id == “bob”).var.money = 10; //This line does exactly the same thing as this line: “objects.bob.var.money = 10”;
        
        objects.all(group == “tree”).events.spawn_apples.run(); //For all objects in a group “tree” spawn apples.
        
        let first_bob = objects.first(group == “bob”);
        
        let last_bob = objects.last(group == “bob”);
        
        let all_bobs = objects.all(group == “bob”);
        
        let all_rich_kids = objects.all(var.money >= 1000).
6.	In order to repeat instructions or whole events use while loop. While loop repeats everything in its scope as long as its condition returns true. Examples:
        
        while(objects.x.var.bool==”true”){ objects.x.events.doSomething(); }
            let i = 0;
            while(i < 10){ x.var.int+=2; i++; }
                let x = all(group == “x”);
                    while(x.size > 0){ me.events.destroyRandomX(); }
    Operands in the conditions of the while loop must return single values – groups of objects cannot be used.

7.	Each if statement creates new conditional event. Nested statements, after being translated into separate events, are still connected to their parents. Examples of if statements:
    
        if(objects.bob.var.int > 10){ objects.bob.events.doSomething(); }
        let group_a = all(group == “group_a”);
        if(group_a.var.apples > 5){ group_a.events.giveOrange(); }
    Just like in while loops, operands in the conditions must return single values – groups of objects cannot be used without a special directive like atomic. Without directives every operation on groups is treated like a SIMD instruction (Single Instruction Multiple Data).

8.	After being triggered, conditional events execute all instructions placed inside them. These are sources of all triggers:
    - Terminal – “run()” command;
    - Events – “run()” command;
    - Time – based on main loop iterations. Passing of seconds and minutes can be detected by using built-in variables second_passed and minute_passed – they are set to true if the stated time has passed in the current iteration;
    - Camera – based on its state, position and collision_with method.
    - Keyboard – with variables: key_down, key_pressed and key_released;
    - Mouse – with variables: mouse_moving, mouse_down, mouse_pressed, mouse_released. Mouse can also select a group of objects based on their position and approximated size – selected group is saved in a built-in context variable mouse_selection;
    - Objects – current state;
    - Variables – current state;
    - Collision – collisions of objects from the same layer detected in the current main loop iteration;
    - Editable Text Fields – with content variable;
    - Movement – based on current state and is_moving variable.
9.	To create a new object you can use clone() method after choosing an existing object or function new() to create a blank object. Remember that you can use objects from another layers as templates. Examples:
        
        Objects.FirstStar.clone(“SecondStar”);
        
        Objects.SecondStar.var.brightness+=10;
        
        Objects.new(“BlankObject”);
        
        Objects.BlankObject.var.newVariable = 42;
	
    Action of creating new objects can return a handle to an object if let statement is used:
            
        let newBob = Layer.bobTemplates.bob123.clone(“newBob”);
        newBob.var.type = 10;
        newBob.event.prepareBasedOnType();
        let newPatrick = Objects.new(“newPatrick”);

10.	Accesing and creating new variables inside all objects is done by using .var accessor on a selected object and giving an id of a user defined variable. Example:
        
        Objects.MrVariable.var.newVariable = 12;
11.	In order to create and destroy elements of every other module in the object use methods new(string id) and destroy(string id), for example:
        
        Objects.walter.particles.new(“snow”);
        Objects.walter.particles.destroy(“rain”);
