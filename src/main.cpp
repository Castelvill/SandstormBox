#include "engineLoop.h"
#include <filesystem>

void loadFontsToContainer(vector <SingleFont> & FontContainer, string EXE_PATH){
    std::ifstream fontListFile;
    fontListFile.open(EXE_PATH+"fonts/font_list.txt");

    short fontSizes[] = {12, 24, 48, 72};
    unsigned short i;
    string fileName, filePath;
    while(fontListFile >> fileName){
        if(fileName.substr(fileName.size()-4, fileName.size()) != ".ttf"){
            std::cout << "Wrong file extension! Aborting fonts loading.\n";
            fontListFile.close();
            return;
        }
        filePath = EXE_PATH+"fonts/" + fileName;
        std::cout << filePath << "\n";
        for(i = 0; i < (sizeof(fontSizes)/sizeof(*fontSizes)); i++){
            FontContainer.push_back(SingleFont());
            FontContainer.back().font = al_load_ttf_font(filePath.c_str(), fontSizes[i], 2);
            FontContainer.back().ID = fileName.substr(0, fileName.size()-4) + intToStr(fontSizes[i]);
            FontContainer.back().sizeF = fontSizes[i];
        }
    }
    fontListFile.close();
    std::cout << FontContainer.size() << " fonts loaded\n";
}
void freeFontsFromContainer(vector <SingleFont> & FontContainer){
    for(unsigned int i = 0; i < FontContainer.size(); i++){
        al_destroy_font(FontContainer[i].font);
    }
    FontContainer.clear();
    if(FontContainer.size() == 0)
        std::cout << "\nAll fonts destroyed.\n";
}

void loadBitmapsToContainer(vector <SingleBitmap> & BitmapContainer, string EXE_PATH){
    //BitmapContainer.reserve(11);
    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("background", "/images/stars.jpg", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("background1", "images/Ilulu.jpg", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("catSmall", "images/catSmall.jpg", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("cat260", "images/cat260.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("lel", "images/lel.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("rocks1_300", "images/rocks1_300.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("rocks2_300", "images/rocks2_300.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("rocks2_200", "images/rocks2_200.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("puss", "images/catgirl.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("chad", "images/chad_doge.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("sword", "images/sword.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("gin300", "images/gin300.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("minion_1", "images/minion1.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("dragon_h3", "images/h3_dragon.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("amongus_1", "images/amongus1.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("amongus_2", "images/amongus2.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("cobblestone", "images/cobblestone.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("steve", "images/steve.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("insanity1", "images/insanity1.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("vaporeon", "images/vaporeon.png", EXE_PATH);


    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("gui_background", "gui/background.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_general", "gui/icon_general.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_image", "gui/icon_image.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_text", "gui/icon_text.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_event", "gui/icon_event.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_movement", "gui/icon_movement.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_collision", "gui/icon_collision.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_particles", "gui/icon_particles.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_variables", "gui/icon_variables.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_editable_text", "gui/icon_editable_text.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("text_field_1", "gui/text_field_1.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("text_field_2", "gui/text_field_2.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("arrow_1", "gui/arrow_h3.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("plus_1", "gui/plus_h3.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("minus_1", "gui/minus_h3.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("thumb_1", "gui/thumb_h3.png", EXE_PATH);

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("thumb_track_1", "gui/thumb_track_h3.png", EXE_PATH);
}
void freeBitmapsFromContainer(vector <SingleBitmap> & BitmapContainer){
    for(unsigned int i = 0; i < BitmapContainer.size(); i++){
        if(BitmapContainer[i].bitmap){
            al_destroy_bitmap(BitmapContainer[i].bitmap);
            //std::cout << "Bitmap '" << BitmapContainer[i].ID << "' destroyed. (by GeneralBitmapContainer)\n";
        }
    }
    BitmapContainer.clear();
    if(BitmapContainer.size() == 0){
        //std::cout << "\nAll bitmaps in general image container destroyed.\n\n";
    }
}

vector<string> getAllFilesNamesWithinFolder(string folder){
    vector<string> names;
    for (const auto & entry : std::filesystem::directory_iterator(folder))
        names.push_back(entry.path().string());
    return names;
}

void createObjects1(vector <AncestorObject> & Objects, string layerID, vector <string> & listOfUniqueIDs, vector <SingleFont> & FontContainer,
    vector <SingleBitmap> & BitmapContainer, EngineLoop & Engine)
{
    Objects.push_back(AncestorObject("par", listOfUniqueIDs, layerID));
    Objects.back().setPos(500.0, 500.0);
    Objects.back().setIsAttachedToCamera(false);
    Objects.back().ParticlesContainer.push_back(ParticleEffectModule("par", &Objects.back().particlesContainerIDs, layerID, Objects.back().getID()));
    Objects.back().ParticlesContainer[0].changeSpawningParameters(false, true, true, 100, 100, 100, 0.05);
    Objects.back().ParticlesContainer[0].changeMotionParameters(3.5, 5.7, -10.0, 10.0, vec2d(0.0, 0.0), vec2d(0.0, 0.0), 0.00, 0.00,
        0.4, 0.7, 0.0, 360.0, -0.0, 0.0, 0.0, 0.0);
    Objects.back().ParticlesContainer[0].changeGeneralParameters(vec2d(0.0, 0.0), vec2d(0.0, 0.0), 50.0, 70.0, 3.0, 10.0,
        0.8, 1.0, -2.0, 2.0, 0, true, false, false);
    Objects.back().ParticlesContainer[0].setSpawnKeyBindShort(ALLEGRO_KEY_P);
    Objects.back().ParticlesContainer[0].addImage("insane");
    Objects.back().ParticlesContainer[0].setUseImageAsParticles(true);
    //Objects.back().ParticlesContainer[0].addColor(0, 255, 0);
    Objects.back().ParticlesContainer[0].setMinBasicSpeed(0.1);
    Objects.back().ParticlesContainer[0].setMaxBasicSpeed(10.0);
    Objects.back().ParticlesContainer[0].setMinSpeed(0.1);
    Objects.back().ParticlesContainer[0].setMaxSpeed(10.0);
    Objects.back().ParticlesContainer[0].setIsAttachedToCamera(false);
    Objects.back().MovementContainer.push_back(MovementModule("", &Objects.back().movementContainerIDs, layerID, Objects.back().getID()));
    Objects.back().VariablesContainer.push_back(VariableModule("par_test", &Objects.back().variablesContainerIDs, layerID, Objects.back().getID()));
    Objects.back().VariablesContainer.back().setInt(69);

    //changeMoveParameters(short newMovementType, short newInputType, double newBodyMass, double newWalkingSpeed, double newRunningSpeed,
    //                                      double newBaseFriction, double newMaxMomentumX, double newMoveCooldown, bool newCanMoveDiagonally)

    Objects.back().MovementContainer[0].changeMoveParameters(1, 2, 0.2, 250.0, 1.0, 1.0, 10.0, 0.0, true);
    //Objects.back().MovementContainer[0].changeJumpParameters(newJumpSpeed, newAllowedJumps, newJumpCooldownDuration, newGravitation, newMinMomentumY, newMaxMomentumY, newResetMomentumWhenJumping)
    Objects.back().MovementContainer[0].changeJumpParameters(18.0, 1, 0.5, 3.0, 20.0, 20.0, true);
    Objects.back().MovementContainer[0].bindKeys(ALLEGRO_KEY_W, ALLEGRO_KEY_D, ALLEGRO_KEY_S, ALLEGRO_KEY_A, ALLEGRO_KEY_W, ALLEGRO_KEY_LSHIFT);
    Objects.back().ImageContainer.push_back(ImageModule("steve", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "steve");
    Objects.back().ImageContainer[0].changeParameters("lel", Objects.back().imageContainerIDs, vec6d(0.0, 0.0, 100.0, 100.0, 0.0, 0.0), 0.0, vec2d(1.0, 1.0), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setUsedBitmapLayer(1);
    Objects.back().ImageContainer[0].setIsAttachedToCamera(false);
    //Objects.back().ImageContainer[0].setIsActive();
    //Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    //Objects.back().ImageContainer[1].connectBitmap(BitmapContainer, "cat260");
    //Objects.back().ImageContainer[1].changeParameters("cat", vec6d(0.0, 0.0, 260.0, 260.0, 0.0, 0.0), 0.0, vec2d(0.5, 0.5), false, false, vec4d(0.0, 0.0, 0.0, 0.0));
    Objects.back().CollisionContainer.push_back(CollisionModule(0, Objects.back().getSize(), &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
    Objects.back().CollisionContainer.back().changeParameters(true, false, vec2d(0.0, 0.0), vec2d(100.0, 100.0));
    Objects.back().CollisionContainer.back().addToIgnoreList("groups_of_hitboxes", "pupa");
    Objects.back().CollisionContainer.push_back(CollisionModule(0, &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
    Objects.back().CollisionContainer.back().setPos(Objects.back().getSize().x, 0);
    Objects.back().CollisionContainer.back().setSize(Objects.back().getSize());

    Objects.back().ImageContainer.push_back(ImageModule("insane", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
    Objects.back().ImageContainer.back().connectBitmap(BitmapContainer, "insanity1");
    Objects.back().ImageContainer.back().translatePos(vec2d(0.0, 0.0));
    Objects.back().ImageContainer.back().setScale(0.1, 0.1);
    Objects.back().ImageContainer.back().setUsedBitmapLayer(2);
    Objects.back().ImageContainer.back().deactivate();


    for(auto &Object : Objects){
        Object.refreshCoordinates();
    }
}

void createObjects0(vector <AncestorObject> & Objects, string layerID, vector <string> & objectsIDs, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer, int SCREEN_W, int SCREEN_H){
    Objects.push_back(AncestorObject("", objectsIDs, layerID));
    Objects.back().setCanBeSelected(false );
    Objects.back().TextContainer.push_back(TextModule("", &Objects.back().textContainerIDs, layerID, Objects.back().getID()));
    Objects.back().TextContainer[0].addNewContent("FPS: ");
    Objects.back().TextContainer[0].changeParameters("king", Objects.back().textContainerIDs, vec4d(10.0, 10.0, 200.0, 200.0), vec3d(255.0, 0.0, 0.0), "Minecraft24", vec2d(1.0, 1.0), 0.0, 2, 0, 0);
    Objects.back().TextContainer[0].setIsAttachedToCamera(true);
    Objects.back().TextContainer[0].setUsedBitmapLayer(-1);


    Objects.push_back(AncestorObject("background", objectsIDs, layerID));
    Objects.back().setPos(vec2d(0, 0));
    Objects.back().setCanBeSelected(false);
    Objects.back().ImageContainer.push_back(ImageModule("", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "background");
    Objects.back().ImageContainer[0].changeParameters("background", Objects.back().imageContainerIDs, vec6d(0.0, 0.0, 1280.0, 853.0, 0.0, 0.0), 0.0, vec2d(1.0, 1.0), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
    Objects.back().ImageContainer[0].resize(vec2d(SCREEN_W, SCREEN_H));
    Objects.back().deactivate();
}

void createCameras(vector <Camera2D> & Cameras, vector <string> & camerasIDs, int SCREEN_W, int SCREEN_H){
    Cameras.push_back(Camera2D("Cam0", camerasIDs, true, vec2d(0.0, 0.0), vec2d(SCREEN_W, SCREEN_H), vec2d(0.0, 0.0)));
    Cameras.back().setZoom(1.0, 0.05, 0.01, 10.0);
    Cameras.back().setSpeed(5.0);
    Cameras.back().setFollowedObjectID("par");
    Cameras.back().setFollowedLayerID("L1");
    Cameras.back().setFollowedImageID("");
    Cameras.back().setIsFollowingObject(false);
    Cameras.back().setKeyBinds(ALLEGRO_KEY_PAD_1, ALLEGRO_KEY_PAD_2, ALLEGRO_KEY_PAD_3, ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J);
    Cameras.back().addVisibleLayer("Editor");
    Cameras.back().addVisibleLayer("L1");
    Cameras.back().addVisibleLayer("L2");
    Cameras.back().addAccessibleLayer("L1");
    Cameras.back().addAccessibleLayer("L2");

    /*Cameras.push_back(Camera2D("Cam1", camerasIDs, true, vec2d(SCREEN_W/2.0, 0.0), vec2d(300.0, 300.0), vec2d(0.0, 0.0)));
    Cameras.back().setZoom(1.0, 0.05, 0.01, 10.0);
    Cameras.back().setSpeed(5.0);
    Cameras.back().setFollowedObjectID("par");
    Cameras.back().setFollowedLayerID("L2");
    Cameras.back().setFollowedImageID("");
    Cameras.back().setIsFollowingObject(false);
    Cameras.back().setKeyBinds(ALLEGRO_KEY_PAD_1, ALLEGRO_KEY_PAD_2, ALLEGRO_KEY_PAD_3, ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J);
    Cameras.back().addVisibleLayer("Editor");
    Cameras.back().addVisibleLayer("L2");
    Cameras.back().addAccessibleLayer("L2");
    Cameras.back().pinToCamera("Cam0");
    
    Cameras.push_back(Camera2D("Cam2", camerasIDs, true, vec2d(300.0, 0.0), vec2d(300.0, 300.0), vec2d(0.0, 0.0)));
    Cameras.back().setZoom(1.0, 0.05, 0.01, 10.0);
    Cameras.back().setSpeed(5.0);
    Cameras.back().setFollowedObjectID("par");
    Cameras.back().setFollowedLayerID("L1");
    Cameras.back().setFollowedImageID("");
    Cameras.back().setIsFollowingObject(false);
    Cameras.back().setKeyBinds(ALLEGRO_KEY_PAD_1, ALLEGRO_KEY_PAD_2, ALLEGRO_KEY_PAD_3, ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J);
    Cameras.back().addVisibleLayer("Editor");
    Cameras.back().addVisibleLayer("L1");
    Cameras.back().pinToCamera("Cam1");*/
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

    unsigned numberOfObjects = 0;

    Threads.push_back(EngineLoop("EGM"));
    Threads.back().initAllegro();
    loadFontsToContainer(FontContainer, Threads.back().EXE_PATH);

    loadBitmapsToContainer(BitmapContainer, Threads.back().EXE_PATH);

    createCameras(Cameras, Threads.back().camerasIDs, Threads.back().getWindowW(), Threads.back().getWindowH());
    Threads.back().updateAllForestOfCameras(Cameras);


    if(Threads.back().isCamerasUniquenessViolated(Cameras)){
        goto kernelGotLost;
    }

    Layers.push_back(LayerClass("KERNEL", Threads.back().layersIDs, true, vec2d(0.0, 0.0), Threads.back().getScreenSize()));
    Layers.back().Objects.push_back(AncestorObject("KERNEL", Layers.back().objectsIDs, Layers.back().getID()));
    Layers.back().Objects.back().bindedScripts.push_back(Threads.back().EXE_PATH + "scripts/KERNEL.txt");
    Layers.back().Objects.back().translateAllScripts(true);


    if(Layers[0].getID() != "KERNEL"){
        std::cout << "Error: In main: Kernel got lost. Never came back.";
        return -1;
    }

    //Layers[0].Objects[0].deactivate();
    //createObjects0(Layers[0].Objects, Layers[0].getID(), Layers[0].objectsIDs, FontContainer, BitmapContainer);

    Layers.push_back(LayerClass("L1", Threads.back().layersIDs, true, vec2d(0.0, 0.0), Threads.back().getScreenSize()));
    createObjects1(Layers[1].Objects, Layers[1].getID(), Layers[1].objectsIDs, FontContainer, BitmapContainer, Threads.back());
    Layers.back().addGroup("kek");
    
    if(Threads.back().isLayersUniquenessViolated(Layers)){
        goto uniquenessViolated;
    }

    for(LayerClass & layer : Layers){
        for(AncestorObject & obj : layer.Objects){
            obj.createVectorsOfIds();
        }
        if(layer.isObjectsUniquenessViolated())
            goto uniquenessViolated;
        numberOfObjects += layer.Objects.size();
    }
    std::cout << "\n\nNumber of objects: " << numberOfObjects << "\n\n";

    Threads.back().updateBaseOfTriggerableObjects(Layers);

    Threads.back().startTimer();
    
    //Main loop
    do{
        Threads.back().windowLoop(Layers, Cameras, FontContainer, fps, BitmapContainer);
    }while(Threads.back().isRunning());

    uniquenessViolated:

    kernelGotLost:

    vector <string> fileNames;
    //TODO: change "D:/Programming/Easy Game Maker" to EXE_PATH
    #if _WIN32
        fileNames = getAllFilesNamesWithinFolder("D:/Programming/Easy Game Maker");
    #elif __linux__
        fileNames = getAllFilesNamesWithinFolder(".");
    #endif
    //for(auto name : fileNames)
    //    std::cout << name << std::endl;

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
