# Sandstorm-Box / Easy-Game-Maker
2D Game Engine written in c++ and Allegro5 graphical library.

# Implemented functionality:

- Sand Scripting Language - you can use any instruction from the /docs/instructions.md file. To learn about this language read /docs/language.md file. All the following programs/utils were written in the Sand language.
- Terminal - you can use many commands like ls (list files in the directory), edit (text editor), sand (execute Sand scripts), cat (print out the contents of the file on the terminal).
- Text editor - you can edit any text files, save them, select multiple letters, words and lines in the text (with or w/o the mouse). Text is wrapped automatically.
- Windows - you can create, move, resize, fullscreen and close them.



# Config file

EXECUTE script_path - executes the script in a new process - on boot and restart.

BUFFER_SIZE width height - sets the size of Allegro5 window buffer.

SAMPLES X - sets the number of samples used in antialiasing. Maximal recommended number of samples is 8. If sampling is not supported on your machine, you must set this value to 0.

ENABLE_al_set_clipboard_text - al_set_clipboard_text can cause undefined behavior on some linux configurations so it's disabled by default. When enabled, allows to copy text from the engine to the clipboard of the host OS.

ENABLE_NOT_ASCII - allows using characters other than ASCII.
