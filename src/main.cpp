#include "processClass.h"

int main(int argc, char* argv[]){
    #if __WIN32__
        HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 15);
    #endif

    srand(time(NULL));

    EngineClass Engine;
    Engine.resetState(true);
    Engine.readCommandLine(argc, argv);
    Engine.initAllegro();
    vector <ProcessClass> Processes;
    vec2d oldMousePos[2];

    do{
        Engine.resetState(false);
        for(string file : Engine.inputFiles){
            Processes.push_back(ProcessClass());
            Processes.back().create(Engine.EXE_PATH, Engine.allowNotAscii, Engine.getDisplaySize(),
                file, "Init0", "KERNEL", "Init", Engine.processIDs);
        }
        for(string initFile : Engine.initFiles){
            Processes.push_back(ProcessClass());
            Processes.back().create(Engine.EXE_PATH, Engine.allowNotAscii, Engine.getDisplaySize(),
                initFile, "Init0", "KERNEL", "Init", Engine.processIDs);
        }
        if(Processes.size() == 0){
            cout << "No source files provided.\n"
                << "Basic \"Hello World\" program:\n\n"
                << "start helloWorld\n"
                << "\ttriggers on_init\n"
                << "\tprint \"Hello, World!\\n\"\n"
                << "\texit\n"
                << "end\n\n";
            break;
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

            if(Engine.display != nullptr && Engine.redraw){
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

    //cout << "Program terminated gracefully.\n";

    return 0;
}