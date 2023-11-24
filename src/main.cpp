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
    vector <ProcessClass> Processes = {ProcessClass(Engine.EXE_PATH, Engine.getScreenSize())};
    
    //Main loop
    do{
        do{
            al_wait_for_event(Engine.eventQueue , &Engine.event);
            Engine.updateEvents();
            for(ProcessClass & Process : Processes){
                Process.windowLoop(Engine);
            }            
        }while(!al_is_event_queue_empty(Engine.eventQueue));
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