#include "processClass.h"

int main(){
    #if __WIN32__
        HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 15);
    #endif

    srand(time(NULL));

    EngineClass Engine;
    Engine.resetState("EGM");
    Engine.initAllegro();
    vector <ProcessClass> Processes;
    vec2d oldMousePos[2];

    do{
        Engine.resetState("EGM");
        for(string initFile : Engine.initFiles){
            Processes.push_back(ProcessClass());
            Processes.back().create(Engine.EXE_PATH, Engine.getDisplaySize(),
                initFile, "Init0", "KERNEL", "Init", Engine.processIDs);
        }
        do{
            Engine.redraw = false;
            do{
                al_wait_for_event(Engine.eventQueue , &Engine.event);
                Engine.updateEvents();
                oldMousePos[0].set(Engine.Mouse.getPos());
                oldMousePos[1].set(Engine.Mouse.getPressedPos());
                for(ProcessClass & Process : Processes){
                    Process.executeIteration(Engine, Processes);
                    if(!Engine.isRunning() || Engine.reboot){
                        break;
                    }
                    Engine.Mouse.setPos(oldMousePos[0]);
                    Engine.Mouse.setPressedPos(oldMousePos[1]);
                }
                Engine.endEvents();
                if(!Engine.isRunning() || Engine.reboot){
                    al_flush_event_queue(Engine.eventQueue);
                    break;
                }
            }while(!al_is_event_queue_empty(Engine.eventQueue));

            if(!Engine.isRunning() || Engine.reboot){
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
        for(ProcessClass & Process : Processes){
            Process.clear();
        }
        Processes.clear();
        Engine.clear();
    }while(Engine.reboot);
    
    Engine.exitAllegro();

    return 0;
}