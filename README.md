# Sandstorm-Box / Easy-Game-Maker
2D Game Engine written in c++ and Allegro5 graphical library.


# Config file
EXECUTE script_path - executes the script in a new process - on boot and restart.

BUFFER_SIZE width height - sets the size of Allegro5 window buffer.

SAMPLES X - sets the number of samples used in antialiasing. Maximal recommended number of samples is 8. If sampling is not supported on your machine, you must set this value to 0.

ENABLE_al_set_clipboard_text - al_set_clipboard_text can cause undefined behavior on some linux configurations so it's disabled by default. When enabled, allows to copy text from the engine to the clipboard of the host OS.

ENABLE_NOT_ASCII - allows using characters other than ASCII.