#include "engineClass.h"

#if __linux__
    unsigned long long GetTickCount()
    {
        struct timeval tv;
        if (gettimeofday(&tv, NULL) != 0) {
            // Handle error
        }
        return (unsigned long long) tv.tv_sec * 1000 + (unsigned long long) tv.tv_usec / 1000;
    }
#endif

inline unsigned int Interval::value() const{
    return GetTickCount()-initial_;
}

void Fps::update(){
    // increase the counter by one
    m_fpscount++;

    // one second elapsed? (= 1000 milliseconds)
    if (m_fpsinterval.value() > 1000)
    {
        // save the current counter value to m_fps
        m_fps = m_fpscount;

        // reset the counter and the interval
        m_fpscount = -1;
        m_fpsinterval = Interval();
    }
}
unsigned int Fps::get() const{
    return m_fps;
}

void freeFontsFromContainer(vector <SingleFont> & FontContainer){
    for(unsigned int i = 0; i < FontContainer.size(); i++){
        al_destroy_font(FontContainer[i].font);
    }
    FontContainer.clear();
}
void freeBitmapsFromContainer(vector <SingleBitmap> & BitmapContainer){
    for(unsigned int i = 0; i < BitmapContainer.size(); i++){
        if(BitmapContainer[i].bitmap){
            al_destroy_bitmap(BitmapContainer[i].bitmap);
        }
    }
    BitmapContainer.clear();
}

vector <short> getPressedKeys(unsigned char key[]){
    vector <short> pressedKeys;
    for(short i = 1; i < ALLEGRO_KEY_MAX; i++){
        if(key[i]){
            pressedKeys.push_back(i);
        }
    }
    return pressedKeys;
}

vector <short> getReleasedKeys(unsigned char key[], vector <short> pressedKeys){
    vector <short> releasedKeys;
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(!key[pressedKeys[i]]){
            releasedKeys.push_back(pressedKeys[i]);
        }
    }
    return releasedKeys;
}

EngineClass::EngineClass(string title){
    isPixelArt = false;
    cursorBitmap = NULL;
    windowTitle = title;
    displaySize.set(1280, 720);
    fullscreen = false;
    closeProgram = false;
    redraw = false;
    displayResized = false;
    memset(key, 0, sizeof(key));
    initAllegro();
}
void getDesktopResolution(int adapter, int *w, int *h){
    ALLEGRO_MONITOR_INFO info;
    al_get_monitor_info(adapter, &info);

    *w = info.x2 - info.x1;
    *h = info.y2 - info.y1;
}
void EngineClass::initAllegro(){
    al_init();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    if(fullscreen){
        al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_RESIZABLE | ALLEGRO_NOFRAME);
    }
    else{
        al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    }
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST); //antialias stuff
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST); //antialias stuff
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

    int SCREEN_W;
    int SCREEN_H;
    getDesktopResolution(0, &SCREEN_W, &SCREEN_H);

    display = al_create_display(SCREEN_W, SCREEN_H);
    al_set_window_title(display, windowTitle.c_str());
    al_set_window_position(display, 0, 0);
    al_resize_display(display, displaySize.x, displaySize.y);

    EXE_PATH = "";
    ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    char buffer[1000] = { 0 };
    sprintf(buffer, "%s", al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
    EXE_PATH = string(buffer);


    //cursorBitmap = al_load_bitmap("images/cursor.png");
    if(cursorBitmap){
        mouseCursor = al_create_mouse_cursor(cursorBitmap, 0, 0);
        al_set_mouse_cursor(display, mouseCursor);
    }
    else{
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }

    eventQueue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    al_register_event_source(eventQueue, al_get_mouse_event_source());

    //Use it only on images that are not pixel arts
    if(!isPixelArt){
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    }
    else{
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
    }

    iconBitmap = al_load_bitmap((EXE_PATH + "icon.png").c_str());
    if(iconBitmap){
        al_set_display_icon(display, iconBitmap);
    }

    //loadFontsToContainer(FontContainer, EXE_PATH);

    //Find out how much buttons the user has.
    Mouse.setUp();

    al_start_timer(timer);
}
void EngineClass::exitAllegro(){
    processIDs.clear();
    
    releasedKeys.clear();
    firstPressedKeys.clear();
    pressedKeys.clear();
    
    al_destroy_display(display);
    if(cursorBitmap){
        al_destroy_bitmap(cursorBitmap);
        al_destroy_mouse_cursor(mouseCursor);
    }
    if(iconBitmap){
        al_destroy_bitmap(iconBitmap);
    }
    al_destroy_timer(timer);
    al_destroy_event_queue(eventQueue);

    freeFontsFromContainer(FontContainer);
    freeBitmapsFromContainer(BitmapContainer);
}
void EngineClass::updateEvents(){
    switch(event.type){
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(display);
            displaySize.set(al_get_display_width(display), al_get_display_height(display));
            displayResized = true;
            break;
        case ALLEGRO_EVENT_TIMER:
            releasedKeys.clear();
            releasedKeys = getReleasedKeys(key, pressedKeys);
            pressedKeys.clear();
            pressedKeys = getPressedKeys(key);
            if(key[ALLEGRO_KEY_LCTRL] && key[ALLEGRO_KEY_ESCAPE]){
                closeProgram = true;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_AXES:
            Mouse.updateAxes(event, fullscreen);
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            Mouse.updateButtonsPressed(event);
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            Mouse.updateButtonsReleased(event);
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
            firstPressedKeys = getPressedKeys(key);
            break;
        case ALLEGRO_EVENT_KEY_UP:
            key[event.keyboard.keycode] &= KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            closeProgram = true;
            break;
    }
}
void EngineClass::endEvents(){
    switch(event.type){
        case ALLEGRO_EVENT_TIMER:
            displayResized = false;
            firstPressedKeys.clear();
            Mouse.didMouseMove = false;
            Mouse.resetFirstPressed();
            for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;
            Mouse.resetReleased();
            redraw = true;
            break;
    }
}

void EngineClass::loadNewFont(string path, int size, string newID){
    if(path.size() < 5){
        cout << "Error: In " << __FUNCTION__ << ": Path to the font file is too short.\n";
        return;
    }
    string fileExtension = path.substr(path.size()-4, path.size());
    if(fileExtension != ".ttf"){
        cout << "Error: In " << __FUNCTION__ << ": '" << fileExtension << "' is not a valid font extension.\n";
        return;
    }
    path = EXE_PATH + path;
    FontContainer.push_back(SingleFont());
    FontContainer.back().font = al_load_ttf_font(path.c_str(), size, 0);
    if(!FontContainer.back().font || FontContainer.back().font == nullptr){
        FontContainer.pop_back();
        cout << "Error: In " << __FUNCTION__ << ": Failed to load a font from '" << path << "'.\n";
        return;
    }
    FontContainer.back().ID = newID;
    FontContainer.back().sizeF = size;
}

bool EngineClass::isRunning() const{
    return !closeProgram;
}
int EngineClass::getWindowW() const{
    return displaySize.x;
}
int EngineClass::getWindowH() const{
    return displaySize.y;
}
ALLEGRO_DISPLAY *EngineClass::getWindow(){
    return display;
}
vec2i EngineClass::getDisplaySize() const{
    return displaySize;
}
bool EngineClass::secondHasPassed() const{
    return al_get_timer_count(timer) % (int)FPS == 0;
}