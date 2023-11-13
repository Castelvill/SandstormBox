#include "engineLoop.h"

void loadFontsToContainer(vector <SingleFont> & FontContainer, string EXE_PATH){
    std::ifstream fontListFile;
    fontListFile.open(EXE_PATH+"fonts/font_list.txt");

    short fontSizes[] = {12, 24, 48, 72};
    unsigned short i;
    string fileName, filePath;
    while(fontListFile >> fileName){
        if(fileName.substr(fileName.size()-4, fileName.size()) != ".ttf"){
            cout << "Wrong file extension! Aborting fonts loading.\n";
            fontListFile.close();
            return;
        }
        filePath = EXE_PATH+"fonts/" + fileName;
        //cout << filePath << "\n";
        for(i = 0; i < (sizeof(fontSizes)/sizeof(*fontSizes)); i++){
            FontContainer.push_back(SingleFont());
            FontContainer.back().font = al_load_ttf_font(filePath.c_str(), fontSizes[i], 2);
            FontContainer.back().ID = fileName.substr(0, fileName.size()-4) + intToStr(fontSizes[i]);
            FontContainer.back().sizeF = fontSizes[i];
        }
    }
    fontListFile.close();
    //cout << FontContainer.size() << " fonts loaded\n";
}
void freeFontsFromContainer(vector <SingleFont> & FontContainer){
    for(unsigned int i = 0; i < FontContainer.size(); i++){
        al_destroy_font(FontContainer[i].font);
    }
    FontContainer.clear();
    if(FontContainer.size() == 0){
        //cout << "\nAll fonts destroyed.\n";
    }
}
void freeBitmapsFromContainer(vector <SingleBitmap> & BitmapContainer){
    for(unsigned int i = 0; i < BitmapContainer.size(); i++){
        if(BitmapContainer[i].bitmap){
            al_destroy_bitmap(BitmapContainer[i].bitmap);
            //cout << "Bitmap '" << BitmapContainer[i].ID << "' destroyed. (by GeneralBitmapContainer)\n";
        }
    }
    BitmapContainer.clear();
    if(BitmapContainer.size() == 0){
        //cout << "\nAll bitmaps in general image container destroyed.\n\n";
    }
}

vector<string> getAllFilesNamesWithinFolder(string folder){
    vector<string> names;
    for (const auto & entry : std::filesystem::directory_iterator(folder))
        names.push_back(entry.path().string());
    return names;
}

Fps fps;

int main(){
    #if __WIN32__
        HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 15);
    #endif

    srand(time(NULL));

    vector <EngineLoop> Threads;
    vector <SingleFont> FontContainer;
    vector <SingleBitmap> BitmapContainer;
    vector <LayerClass> Layers;
    vector <Camera2D> Cameras;

    Threads.push_back(EngineLoop("EGM"));
    Threads.back().initAllegro();
    loadFontsToContainer(FontContainer, Threads.back().EXE_PATH);

    Layers.push_back(LayerClass("KERNEL", Threads.back().layersIDs, true, vec2d(0.0, 0.0), Threads.back().getScreenSize()));
    Layers.back().Objects.push_back(AncestorObject("init", Layers.back().objectsIDs, Layers.back().getID()));
    Layers.back().Objects.back().bindedScripts.push_back(Threads.back().EXE_PATH + "init/init.txt");
    Layers.back().Objects.push_back(AncestorObject("KERNEL", Layers.back().objectsIDs, Layers.back().getID()));
    Layers.back().Objects.back().translateAllScripts(true);
    
    if(Threads.back().isLayersUniquenessViolated(Layers)){
        goto uniquenessViolated;
    }

    for(LayerClass & layer : Layers){
        for(AncestorObject & obj : layer.Objects){
            obj.createVectorsOfIds();
        }
        if(layer.isObjectsUniquenessViolated()){
            goto uniquenessViolated;
        }
    }

    Threads.back().updateBaseOfTriggerableObjects(Layers);

    Threads.back().startTimer();
    
    //Main loop
    do{
        Threads.back().windowLoop(Layers, Cameras, FontContainer, fps, BitmapContainer);
    }while(Threads.back().isRunning());

    uniquenessViolated:

    vector <string> fileNames;
    //TODO: change "D:/Programming/Easy Game Maker" to EXE_PATH
    #if _WIN32
        fileNames = getAllFilesNamesWithinFolder("D:/Programming/Easy Game Maker");
    #elif __linux__
        fileNames = getAllFilesNamesWithinFolder(".");
    #endif
    //for(auto name : fileNames)
    //    cout << name << std::endl;

    Threads.back().exitAllegro();

    freeFontsFromContainer(FontContainer);
    freeBitmapsFromContainer(BitmapContainer);
    for(LayerClass & Layer : Layers){
        for(AncestorObject & obj : Layer.Objects){
            obj.clearContainers();
        }
        Layer.Objects.clear();
        Layer.clear();
    }
    Layers.clear();

    for(Camera2D & Camera : Cameras){
        Camera.clear();
    }
    Cameras.clear();

    return 0;
}