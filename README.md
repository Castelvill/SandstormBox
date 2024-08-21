# SandstormBox
2D Game Engine written in c++ and Allegro5 graphical library.

This engine uses Sand - a custom scripting language made out of engine instructions. You can find all the implemented instructions in the /docs/instructions.md file. To learn about this language read the /docs/language.md file. All programs and utilities that work inside the engine are written in the Sand language.

## Implemented functionality

Below are the most important working parts of the engine: 
- JIT Compilation - all the software inside the engine is currently compiled from the scripts. You can also compile scripts inside the terminal using "sand" compilator.
- Terminal - you can use many commands like "ls" (list files in the directory), "edit" (text editor), "sand" (execute Sand scripts) or "cat" (print out the contents of the file on the terminal).
- Text editor - is built into the terminal and can be opened with "edit" command. With this editor you can edit any text files and save them. You can also select multiple letters, words and lines in the text with the keyboard or mouse. Text inside the editor is wrapped automatically.
- Mover - a program that lets you move around the objects inside the chosen layer. 
- Scrollbars - programs that use scrollbars let you scroll through their contents with the mouse - both with the wheel and by dragging the scrollbar.
- Windows - you can create, move, resize, fullscreen and close them. Command "nw" creates a blank window and its first parameter is used as a title of this window.

Use 'help' command inside the terminal or press the TAB key to list all the available programs.

## Config file

EXECUTE script_path - executes the script in a new process - on boot and restart.

BUFFER_SIZE width height - sets the size of Allegro5 window buffer.

SAMPLES X - sets the number of samples used in antialiasing. Maximal recommended number of samples is 8. If sampling is not supported on your machine, you must set this value to 0.

ENABLE_al_set_clipboard_text - al_set_clipboard_text can cause undefined behavior on some linux configurations so it's disabled by default. When enabled, allows to copy text from the engine to the clipboard of the host OS.

ENABLE_NOT_ASCII - allows using characters other than ASCII.
