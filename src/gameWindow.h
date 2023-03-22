#ifndef GAMEWINDOW_H_INCLUDED
#define GAMEWINDOW_H_INCLUDED
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <string>
#include <vector>

class GameWindow{
private:
    ALLEGRO_DISPLAY *window;
    ALLEGRO_KEYBOARD_STATE keyboard;
    //ALLEGRO_BITMAP *cursor_bitmap;
    //ALLEGRO_MOUSE_CURSOR * mouse_cursor;
    //ALLEGRO_MOUSE_STATE state;
    std::string windowTitle;
public:
    GameWindow(std::string title);
    void initAllegro();
    void exitAllegro();
    void windowLoop();

};


#endif // GAMEWINDOW_H_INCLUDED
