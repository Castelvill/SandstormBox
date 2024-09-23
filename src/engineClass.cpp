#define ALLEGRO_UNSTABLE
#include "engineClass.h"

vector<string> tokenizeString(string input, char delimeter){
    vector<string> tokens = {""};
    bool isInsideStringSector = false;
    for(char letter : input){
        if(letter == '\"'){
            tokens.back() += letter;
            isInsideStringSector = !isInsideStringSector;
        }
        else if(!isInsideStringSector && letter == delimeter){
            tokens.push_back("");
        }
        else{
            tokens.back() += letter;
        }
    }
    return tokens;
}

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
        if(BitmapContainer[i].lightBitmap != nullptr){
            al_destroy_bitmap(BitmapContainer[i].lightBitmap);
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

void EngineClass::readCommandLine(int argc, char *argv[]){
    for(int argument = 1; argument < argc; ++argument){
        if(strcmp(argv[argument], "--interpreter") == 0 || strcmp(argv[argument], "-i") == 0){
            inputFiles.push_back("?interpreter");
            continue;
        }
        if(strcmp(argv[argument], "--ignore-config") == 0 || strcmp(argv[argument], "-c") == 0){
            loadConfig = false;
            continue;
        }
        if(strcmp(argv[argument], "--samples") == 0 || strcmp(argv[argument], "-s") == 0){
            if(argument + 1 >= argc){
                cout << "Error: In " << __FUNCTION__ << ": Parameter '" << argv[argument]
                    << "' requires 1 more arguments of the integer type\n.";
                return;
            }
            argument++;
            string error;
            samples = cstoi(argv[argument], error);
            if(error.size() > 0){
                cout << "Error: In " << __FUNCTION__ << ": " << error << "\n";
                return;
            }
            continue;
        }
        if(strcmp(argv[argument], "--pixel-art") == 0 || strcmp(argv[argument], "-p") == 0){
            isPixelArt = true;
            continue;
        }
        if(strcmp(argv[argument], "--fullscreen") == 0 || strcmp(argv[argument], "-f") == 0){
            fullscreen = true;
            continue;
        }
        if(strcmp(argv[argument], "--not-ascii") == 0 || strcmp(argv[argument], "-n") == 0){
            allowNotAscii = true;
            continue;
        }
        inputFiles.push_back(argv[argument]);
    }
}

void EngineClass::resetState(bool resetScreen){
    cursorBitmap = NULL;
    if(resetScreen){
        displaySize.set(640, 480);
        fullscreen = false;
    }
    closeProgram = false;
    if(reboot){
        cout << "Rebooted\n";
        cout.flush();
    }
    reboot = false;
    redraw = false;
    displayResized = false;
}
EngineClass::EngineClass(){
    memset(key, 0, sizeof(key));
    reboot = false;
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

    //getDesktopResolution(0, &SCREEN_W, &SCREEN_H);

    displaySize.set(640, 480);

    EXE_PATH = "";
    ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    char buffer[1000] = { 0 };
    sprintf(buffer, "%s", al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
    EXE_PATH = string(buffer);

    eventQueue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));
    al_register_event_source(eventQueue, al_get_mouse_event_source());

    //Use it only on images that are not pixel arts
    if(!isPixelArt){
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    }
    else{
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
    }

    //Find out how much buttons the user has.
    Mouse.setUp();

    al_start_timer(timer);

    if(loadConfig){
        std::ifstream File(EXE_PATH+".config");
        if(File){
            string buffer;
            vector<string> words;
            while(getline(File, buffer)){
                if(buffer == ""){
                    continue;
                }
                words = tokenizeString(buffer, ' ');
                if(words.size() == 0 || words[0][0] == '#' || words[0] == ""){
                    continue;
                }
                if(words[0] == "SAMPLES"){
                    if(words.size() > 1){
                        samples = atoi(words[1].c_str());
                    }
                    else{
                        cout << "Error: In " << __FUNCTION__ << ": SAMPLES command requires one numeric argument (0-4 recommended).\n";
                    }
                }
                else if(words[0] == "ENABLE_al_set_clipboard_text"){
                    ENABLE_al_set_clipboard_text = true;
                }
                else if(words[0] == "ENABLE_NOT_ASCII"){
                    allowNotAscii = true;
                }
                else if(words[0] == "EXECUTE"){
                    if(words.size() > 1){
                        initFiles.push_back(words[1]);
                    }
                    else{
                        cout << "Error: In " << __FUNCTION__ << ": EXECUTE command requires one numeric argument (0-4 recommended).\n";
                    }
                    File >> buffer;
                    
                }
                else{
                    cout << "Error: In " << __FUNCTION__ << ": Configuration option '" << words[0] << "' is not valid.\n";
                }
            }
        }
        File.close();
    }
}
void EngineClass::createDisplay(){
    if(display != nullptr){
        cout << "Warning: Display already exists. Nothing to be done.\n";
        return;
    }

    if(samples > 8){
        samples = 8;
    }
    if(backbufferSize.x > 3840){
        backbufferSize.x = 3840;
    }
    if(backbufferSize.x < 10){
        backbufferSize.x = 10;
    }
    if(backbufferSize.y > 2160){
        backbufferSize.y = 2160;
    }
    if(backbufferSize.y < 10){
        backbufferSize.y = 10;
    }
    al_set_new_bitmap_samples(samples);
    backbuffer = al_create_bitmap(backbufferSize.x, backbufferSize.y);
    al_set_target_bitmap(backbuffer);
    al_set_new_bitmap_samples(0);

    if(fullscreen){
        al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_RESIZABLE | ALLEGRO_NOFRAME);
    }
    else{
        al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    }
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST); //antialias stuff
    al_set_new_display_option(ALLEGRO_SAMPLES, samples, ALLEGRO_SUGGEST); //antialias stuff
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

    //getDesktopResolution(0, &SCREEN_W, &SCREEN_H);

    display = al_create_display(displaySize.x, displaySize.y);
    al_set_window_title(display, windowTitle.c_str());
    al_set_window_position(display, 0, 0);

    al_register_event_source(eventQueue, al_get_display_event_source(display));

    if(cursorBitmap){
        mouseCursor = al_create_mouse_cursor(cursorBitmap, 0, 0);
        al_set_mouse_cursor(display, mouseCursor);
    }
    else{
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }
    iconBitmap = al_load_bitmap((EXE_PATH + "images/icon.png").c_str());
    if(iconBitmap){
        al_set_display_icon(display, iconBitmap);
    }
}
void EngineClass::clear(){
    processIDs.clear();
    releasedKeys.clear();
    firstPressedKeys.clear();
    pressedKeys.clear();
    CopiedFormatting.clear();
}
void EngineClass::exitAllegro(){
    freeFontsFromContainer(FontContainer);
    freeBitmapsFromContainer(BitmapContainer);
    if(backbuffer != nullptr){
        al_destroy_bitmap(backbuffer);
    }
    if(display != nullptr){
        al_destroy_display(display);
        cout << "Display destroyed.\n";
    }
    al_destroy_timer(timer);
    al_destroy_event_queue(eventQueue);
    if(cursorBitmap){
        al_destroy_bitmap(cursorBitmap);
        al_destroy_mouse_cursor(mouseCursor);
    }
    if(iconBitmap != nullptr){
        al_destroy_bitmap(iconBitmap);
    }
    //al_uninstall_keyboard();
    //al_uninstall_system();
}
void EngineClass::updateEvents(){
    switch(event.type){
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            if(display == nullptr){
                break;
            }
            al_acknowledge_resize(display);
            displaySize.set(al_get_display_width(display), al_get_display_height(display));
            displayResized = true;
            break;
        case ALLEGRO_EVENT_TIMER:
            if(display == nullptr && canTerminateWithTimeout){
                terminationTimer--;
                if(terminationTimer <= 0){
                    closeProgram = true;
                    cout << "Program has been terminated due to a timeout.\n";
                }
            }
            else{
                terminationTimer = TERMINATION_TIME;
            }
            releasedKeys.clear();
            releasedKeys = getReleasedKeys(key, pressedKeys);
            pressedKeys.clear();
            pressedKeys = getPressedKeys(key);
            if(key[ALLEGRO_KEY_LCTRL] && key[ALLEGRO_KEY_LSHIFT] && key[ALLEGRO_KEY_R]){
                reboot = true;
                key[ALLEGRO_KEY_LCTRL] = false;
                key[ALLEGRO_KEY_LSHIFT] = false;
                key[ALLEGRO_KEY_R] = false;
                cout << "Rebooting...";
            }
            else if(key[ALLEGRO_KEY_LCTRL] && key[ALLEGRO_KEY_ESCAPE]){
                //closeProgram = true;
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
    if(size < 0 || size > 1000){
        cout << "Error: In " << __FUNCTION__ << ": Font size equal to " << size << " is not a valid size.\n";
    }
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
    //al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    FontContainer.back().font = al_load_ttf_font(path.c_str(), size, 0);
    //al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    
    if(!FontContainer.back().font){
        FontContainer.pop_back();
        cout << "Error: In " << __FUNCTION__ << ": Failed to load a font from '" << path << "'.\n";
        return;
    }
    FontContainer.back().ID = newID;
    FontContainer.back().size = size;
    FontContainer.back().height = al_get_font_line_height(FontContainer.back().font);
}

bool EngineClass::isRunning() const{
    return !closeProgram;
}
int EngineClass::getDisplayW() const{
    return displaySize.x;
}
int EngineClass::getDisplayH() const{
    return displaySize.y;
}
ALLEGRO_DISPLAY *EngineClass::getDisplay(){
    return display;
}
vec2i EngineClass::getDisplaySize() const{
    return displaySize;
}
bool EngineClass::secondHasPassed() const{
    return al_get_timer_count(timer) % (int)FPS == 0;
}