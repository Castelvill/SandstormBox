#include "gameWindow.h"

GameWindow::GameWindow(std::string title){
    windowTitle = title;
}
void GameWindow::initAllegro(){
    al_init();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    window = al_create_display(640, 480);
    al_set_window_title(window, windowTitle.c_str());
    //cursor_bitmap = al_load_bitmap("images/cursor.png");
    //mouse_cursor = al_create_mouse_cursor(cursor_bitmap, 0, 0);
    //al_set_mouse_cursor(window, mouse_cursor);

}
void GameWindow::exitAllegro(){
    al_destroy_display(window);
    //al_destroy_bitmap(cursor_bitmap);
    //al_destroy_mouse_cursor(mouse_cursor);
}
void GameWindow::windowLoop(){

}
