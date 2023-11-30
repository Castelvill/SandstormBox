#include "processClass.h"

vector<string> getAllFilesNamesWithinFolder(string folder){
    vector<string> names;
    for (const auto & entry : std::filesystem::directory_iterator(folder))
        names.push_back(entry.path().string());
    return names;
}

int main(){
    #if __WIN32__
        HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 15);
    #endif

    srand(time(NULL));

    EngineClass Engine("EGM");
    vector <ProcessClass> Processes;
    Processes.push_back(ProcessClass(Engine.EXE_PATH, Engine.getDisplaySize(), "init/init.txt", "Init", Engine.processIDs));
    Processes.push_back(ProcessClass(Engine.EXE_PATH, Engine.getDisplaySize(), "init/init1.txt", "Init1", Engine.processIDs));
    vec2d oldMousePos[2];
    //Main loop
    do{
        Engine.redraw = false;
        do{
            al_wait_for_event(Engine.eventQueue , &Engine.event);
            Engine.updateEvents();
            oldMousePos[0].set(Engine.Mouse.getPos());
            oldMousePos[1].set(Engine.Mouse.getPressedPos());
            for(ProcessClass & Process : Processes){
                Process.executeIteration(Engine, Processes);
                if(!Engine.isRunning()){
                    break;
                }
                Engine.Mouse.setPos(oldMousePos[0]);
                Engine.Mouse.setPressedPos(oldMousePos[1]);
            }
            Engine.endEvents();     
        }while(!al_is_event_queue_empty(Engine.eventQueue));

        if(!Engine.isRunning()){
            break;
        }

        if(Engine.redraw){
            for(ProcessClass & Process : Processes){
                Process.renderOnDisplay(Engine);
            }
            al_flip_display();
            al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
            Engine.redraw = false;
            Engine.fps.update();
        }

    }while(Engine.isRunning());

    vector <string> fileNames;
    //TODO: change "D:/Programming/Easy Game Maker" to EXE_PATH
    #if _WIN32
        fileNames = getAllFilesNamesWithinFolder("D:/Programming/Easy Game Maker");
    #elif __linux__
        fileNames = getAllFilesNamesWithinFolder(".");
    #endif
    //for(auto name : fileNames)
    //    cout << name << std::endl;

    Processes.clear();

    Engine.exitAllegro();

    return 0;
}