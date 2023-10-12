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
    /*Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("king_arthur");
    Objects.back().setPos(vec2d(0.0, 0.0));
    Objects.back().EditableTextContainer.push_back(EditableTextModule(Objects.back().EditableTextContainer.size()));
    Objects.back().EditableTextContainer.back().addNewContent("0.0");
    Objects.back().EditableTextContainer.back().changeParameters("king", vec4d(0, 0.0, 850.0, 60.0), vec3d(255.0, 255.0, 255.0), "Minecraft48", vec2d(1.0, 1.0), 0.0, 2, 0, 1);
    Objects.back().EditableTextContainer.back().setUsedBitmapLayer(2);
    Objects.back().EditableTextContainer.back().setIsScaledFromCenter(true);
    Objects.back().EditableTextContainer.back().setCanBeEdited(true);
    Objects.back().EditableTextContainer.back().setIsNumerical(true);
    Objects.back().EditableTextContainer.back().setConnectedObject("Doge", "variable", "var_1", "string");*/

    /*Objects.back().EditableTextContainer.push_back(EditableTextModule(Objects.back().EditableTextContainer.size()));
    Objects.back().EditableTextContainer[1].addNewContent("");
    Objects.back().EditableTextContainer[1].changeParameters("king", vec4d(moveEditables, 160.0, 850.0, 60.0), vec3d(255.0, 255.0, 255.0), "Minecraft48", vec2d(1.0, 1.0), 0.0, 2, 0, 1);
    Objects.back().EditableTextContainer[1].setUsedBitmapLayer(2);
    Objects.back().EditableTextContainer[1].setIsScaledFromCenter(true);
    Objects.back().EditableTextContainer[1].setCanBeEdited(true);
    Objects.back().EditableTextContainer[1].setIsNumerical(true);
    Objects.back().EditableTextContainer[1].setCanClearContentAfterSuccess(false);
    Objects.back().EditableTextContainer[1].setConnectedObject("par", "particles", "par", "remove_color_interval");
    Objects.back().EditableTextContainer.push_back(EditableTextModule(Objects.back().EditableTextContainer.size()));
    Objects.back().EditableTextContainer[2].addNewContent("");
    Objects.back().EditableTextContainer[2].changeParameters("king", vec4d(moveEditables, 240.0, 850.0, 60.0), vec3d(255.0, 255.0, 255.0), "Minecraft48", vec2d(1.0, 1.0), 0.0, 2, 0, 1);
    Objects.back().EditableTextContainer[2].setUsedBitmapLayer(2);
    Objects.back().EditableTextContainer[2].setIsScaledFromCenter(true);
    Objects.back().EditableTextContainer[2].setCanBeEdited(true);
    Objects.back().EditableTextContainer[2].setIsNumerical(true);
    Objects.back().EditableTextContainer[2].setConnectedObject("par", "particles", "par", "max_particles_count");
    Objects.back().EditableTextContainer.push_back(EditableTextModule(Objects.back().EditableTextContainer.size()));
    Objects.back().EditableTextContainer[3].addNewContent("");
    Objects.back().EditableTextContainer[3].changeParameters("king", vec4d(moveEditables, 320.0, 850.0, 60.0), vec3d(255.0, 255.0, 255.0), "Minecraft48", vec2d(1.0, 1.0), 0.0, 2, 0, 1);
    Objects.back().EditableTextContainer[3].setUsedBitmapLayer(2);
    Objects.back().EditableTextContainer[3].setIsScaledFromCenter(true);
    Objects.back().EditableTextContainer[3].setCanBeEdited(true);
    Objects.back().EditableTextContainer[3].setIsNumerical(true);
    Objects.back().EditableTextContainer[3].setConnectedObject("par", "particles", "par", "max_basic_speed");
    Objects.back().EditableTextContainer.push_back(EditableTextModule(Objects.back().EditableTextContainer.size()));
    Objects.back().EditableTextContainer[4].addNewContent("");
    Objects.back().EditableTextContainer[4].changeParameters("king", vec4d(moveEditables, 400.0, 850.0, 60.0), vec3d(255.0, 255.0, 255.0), "Minecraft48", vec2d(1.0, 1.0), 0.0, 2, 0, 1);
    Objects.back().EditableTextContainer[4].setUsedBitmapLayer(2);
    Objects.back().EditableTextContainer[4].setIsScaledFromCenter(true);
    Objects.back().EditableTextContainer[4].setCanBeEdited(true);
    Objects.back().EditableTextContainer[4].setIsNumerical(true);
    Objects.back().EditableTextContainer[4].setConnectedObject("par", "particles", "par", "environment_synchronized");*/

    /*Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Smok");
    Objects.back().setPos(vec2d(100, 100));
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer.back().connectBitmap(BitmapContainer, "dragon_h3");
    Objects.back().ImageContainer.back().setID("dragon");
    Objects.back().ImageContainer.back().setIsScaledFromCenter(false);
    Objects.back().ImageContainer.back().setUsedBitmapLayer(1);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label1");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Smoczek <3", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 0.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label2");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Dragon", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 50.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label3");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Le Dragun", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 100.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label4");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Drachen", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 150.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);*/

    Objects.push_back(AncestorObject("Amongus", listOfUniqueIDs, layerID));
    Objects.back().setPos(vec2d(100, 400));
    Objects.back().setIsAttachedToCamera(false);
    Objects.back().ImageContainer.push_back(ImageModule("amongus", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
    Objects.back().ImageContainer.back().connectBitmap(BitmapContainer, "amongus_2");
    Objects.back().ImageContainer.back().setIsScaledFromCenter(false);
    Objects.back().ImageContainer.back().setUsedBitmapLayer(1);
    Objects.back().ImageContainer.back().setScale(0.5, 0.5);
    Objects.back().ImageContainer.back().setIsAttachedToCamera(false);
    Objects.back().ImageContainer.push_back(ImageModule("insanity", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
    Objects.back().ImageContainer.back().connectBitmap(BitmapContainer, "insanity1");
    Objects.back().ImageContainer.back().translatePos(vec2d(100.0, 300.0));
    Objects.back().ImageContainer.back().setScale(0.1, 0.1);
    Objects.back().ImageContainer.back().setUsedBitmapLayer(2);
    Objects.back().VariablesContainer.push_back(VariableModule("timer", &Objects.back().variablesContainerIDs, layerID, Objects.back().getID()));
    Objects.back().VariablesContainer.back().setDouble(0.0);
    Objects.back().VariablesContainer.push_back(VariableModule("bbb", &Objects.back().variablesContainerIDs, layerID, Objects.back().getID()));
    Objects.back().VariablesContainer.push_back(VariableModule("rot_spd", &Objects.back().variablesContainerIDs, layerID, Objects.back().getID()));
    Objects.back().VariablesContainer.back().setInt(10);
    Objects.back().VariablesContainer.push_back(VariableModule("xd", &Objects.back().variablesContainerIDs, layerID, Objects.back().getID()));
    Objects.back().VariablesContainer.back().setInt(0);
    Objects.back().CollisionContainer.push_back(CollisionModule(0, vec2d(50, 50), &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
    Objects.back().CollisionContainer.push_back(CollisionModule(1, vec2d(50, 50), &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
    Objects.back().CollisionContainer.back().setPos(50, 0);
    Objects.back().TextContainer.push_back(TextModule("label1", &Objects.back().textContainerIDs, layerID, Objects.back().getID()));
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("0", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 0.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);


    /*Objects.back().EveContainer.push_back(EveModule("rotate-init", &Objects.back().eveContainerIDs, layerID, Objects.back().getID()));
    Objects.back().EveContainer.back().primaryTriggerTypes.push_back("each_iteration");
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "all";
    Objects.back().EveContainer.back().DependentOperations.back().Location.source = "layer";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass());
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.source = "layer";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "in_group";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Literal.setString("kek");
    Objects.back().EveContainer.back().DependentOperations.back().newContextID = "L1";
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "all";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("L1");
    Objects.back().EveContainer.back().DependentOperations.back().Location.source = "object";
    Objects.back().EveContainer.back().DependentOperations.back().newContextID = "Objects";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass());
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.source = "object";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.moduleType = "ancestor";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "id";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass());
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.source = "literal";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Literal.setString("22a5");
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().operators.push_back("==");


    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "all";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("Objects");
    Objects.back().EveContainer.back().DependentOperations.back().Location.source = "image";
    Objects.back().EveContainer.back().DependentOperations.back().newContextID = "image";
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "find_by_id";
    Objects.back().EveContainer.back().DependentOperations.back().newContextID = "speed";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass());
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.source = "object";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.layerID = layerID;
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.objectID = "Amongus";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.moduleType = "variable";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.moduleID = "rot_spd";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "int";

    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "random_int";
    Objects.back().EveContainer.back().DependentOperations.back().newContextID = "random";
    Objects.back().EveContainer.back().DependentOperations.back().Literals.push_back(VariableModule::newInt(-50));
    Objects.back().EveContainer.back().DependentOperations.back().Literals.push_back(VariableModule::newInt(50));

    Objects.back().EveContainer.back().Children.push_back({false, "rotate-first-if"});
    //Objects.back().EveContainer.back().Children.push_back({false, "rotate-second-if"});

    Objects.back().EveContainer.push_back(EveModule("rotate-first-if", &Objects.back().eveContainerIDs, layerID, Objects.back().getID()));
    //Objects.back().EveContainer.back().primaryTriggerTypes.push_back("key_pressing");
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("a"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "key_pressing";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(ALLEGRO_KEY_Q);
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("image");
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "find_by_id";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass("all"));
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "rotate_angle";
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "+=";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("");
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("speed");
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("image");
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "find_by_id";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass("all"));
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "pos_x";
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "+=";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("");
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("random");
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("image");
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "find_by_id";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass("all"));
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "pos_y";
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "+=";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("");
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("random");
    Objects.back().EveContainer.back().elseChildID = "rotate-second-if";

    Objects.back().EveContainer.push_back(EveModule("rotate-second-if", &Objects.back().eveContainerIDs, layerID, Objects.back().getID()));
    //Objects.back().EveContainer.back().primaryTriggerTypes.push_back("key_pressing");
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("a"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "key_pressing";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(ALLEGRO_KEY_E);
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("image");
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "find_by_id";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.push_back(ConditionClass("all"));
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.source = "object";
    Objects.back().EveContainer.back().DependentOperations.back().ConditionalChain.back().Location.attribute = "rotate_angle";
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "-=";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("");
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("speed");
    Objects.back().EveContainer.back().elseChildID = "speed-1";

    Objects.back().EveContainer.push_back(EveModule("speed-1", &Objects.back().eveContainerIDs, layerID, Objects.back().getID()));
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("pressed"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "key_pressed";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(ALLEGRO_KEY_A);
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("speed"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "context";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setString("speed");
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("0"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(0);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back(">");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "--";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("speed");
    Objects.back().EveContainer.back().elseChildID = "speed-2";

    Objects.back().EveContainer.push_back(EveModule("speed-2", &Objects.back().eveContainerIDs, layerID, Objects.back().getID()));
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass(""));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "key_pressed";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(ALLEGRO_KEY_S);
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "literal";
    Objects.back().EveContainer.back().DependentOperations.back().Literals.push_back(VariableModule::newInt(10));
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "=";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("speed");
    Objects.back().EveContainer.back().elseChildID = "speed-3";

    Objects.back().EveContainer.push_back(EveModule("speed-3", &Objects.back().eveContainerIDs, layerID, Objects.back().getID()));
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass(""));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "key_pressed";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(ALLEGRO_KEY_D);
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("speed"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "context";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setString("speed");
    Objects.back().EveContainer.back().ConditionalChain.push_back(ConditionClass("20"));
    Objects.back().EveContainer.back().ConditionalChain.back().Location.source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(20);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("<");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().DependentOperations.push_back(OperaClass());
    Objects.back().EveContainer.back().DependentOperations.back().instruction = "++";
    Objects.back().EveContainer.back().DependentOperations.back().dynamicIDs.push_back("speed");
    Objects.back().EveContainer.back().elseChildID = "speed-3";*/
    
    
/*
    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Doge");
    Objects.back().setPos(vec2d(500, 100));
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "chad");
    Objects.back().ImageContainer[0].changeParameters("dog", vec6d(0.0, 0.0, 392.0, 449.0, 0.0, 0.0), 0.0, vec2d(1.5, 1.5), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setRotPos(vec2d(0.0,0.0));
    Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
    Objects.back().ImageContainer[0].setUsedBitmapLayer(1);
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[1].setID("sword");
    Objects.back().ImageContainer[1].connectBitmap(BitmapContainer, "sword");
    Objects.back().ImageContainer[1].setPos(150.0, 400.0);
    Objects.back().ImageContainer[1].setScale(2.0, 2.0);
    Objects.back().ImageContainer[1].setSize(160.0, 160.0);
    Objects.back().ImageContainer[1].setUsedBitmapLayer(2);*/
    //Objects.back().ImageContainer[1].setRotation(45.0);
    /*Objects.back().MovementContainer.push_back(MovementModule(Objects.back().MovementContainer.size()));
    Objects.back().MovementContainer[0].setID("ruch");
    //Objects.back().MovementContainer[0].changeMoveParameters(1, 0.5, 2.0, 3.5, 1.1, 9.0, 0.0);
    Objects.back().MovementContainer[0].changeMoveParameters(1, 2, 0.5, 5.0, 5.0, 5.0, 5.0, 0.0, true);
    Objects.back().MovementContainer[0].bindKeys(ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J, ALLEGRO_KEY_I, ALLEGRO_KEY_LSHIFT);
    Objects.back().MovementContainer[0].changeJumpParameters(30.0, 2, 0.5, 3.0, 30.0, 20.0, true);
    Objects.back().MovementContainer[0].changeVectorMoveParameters(0, false, true);
    Objects.back().MovementContainer[0].changeRandomActionsSettings(true, 0.5, 2.5, 100.0, 200.0, true, 0.3);
    Objects.back().CollisionContainer.push_back(CollisionModule(Objects.back().CollisionContainer.size(), Objects.back().getSize()));
    vector<string> temp;
    temp.push_back(string());
    temp[0] = "Obiekt";
    Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(100.0, 100.0), true);
*/

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
/*

    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setIsAttachedToCamera(true);
    Objects.back().setID("Butty1");
    Objects.back().setCanBeSelected(true);
    //Objects.back().createButton("Butty1", vec2d(0.0, 0.0), vec2d(300.0, 300.0), vec2d(300.0, 300.0), vec2d(1.0, 1.0), "gin300", BitmapContainer, true);
    Objects.back().ImageContainer.push_back(ImageModule(0));
    Objects.back().ImageContainer[0].setIsAttachedToCamera(true);
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "gin300");
    Objects.back().ImageContainer[0].setIsScaledFromCenter(true);
    Objects.back().ImageContainer[0].setID("0");
    Objects.back().EventsContainer.push_back(EventModule(Objects.back().EventsContainer.size()));
    Objects.back().EventsContainer[0].setUpButton(vec2d(.0, .0), vec2d(300.0, 300.0));
    Objects.back().EventsContainer[0].addOperation("0", "0", "obiekt", "movement", "ruch", "move_right", vecXd());
    Objects.back().EventsContainer[0].addOperation(0, 0, "Obiekt", "image", "Feet", "rotate", vecXd(1, -1.0));
    Objects.back().EventsContainer[0].addOperation(3, 0, "Butty1", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.2));
    Objects.back().EventsContainer[0].addOperation(2, 0, "Butty1", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.2));
    Objects.back().EventsContainer[0].addOperation(4, 0, "Butty1", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty1", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty1", "image", "0", "scale", vecXd(2, 0.85, 0.85));
    Objects.back().EventsContainer[0].addOperation(2, 0, "Butty1", "image", "0", "scale", vecXd(2, 1.0, 1.0));
    Objects.back().EventsContainer[0].addOperation(1, 0, "Butty1", "image", "0", "scale", vecXd(2, 1.0, 1.0));



    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().createButton("Butty2", vec2d(1300.0, 100.0), vec2d(300.0, 300.0), vec2d(600.0, 600.0), vec2d(0.5, 0.5), "gin300", BitmapContainer, false);
    Objects.back().EventsContainer.push_back(EventModule(Objects.back().EventsContainer.size()));
    Objects.back().EventsContainer[0].setUpButton(vec2d(0.0, 0.0), vec2d(300.0, 300.0), true);
    Objects.back().EventsContainer[0].addOperation(0, 0, "par", "particles", "par", "spawn", vecXd());
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty2", "image", "0", "rotate", vecXd(1, 5.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty2", "image", "1", "rotate", vecXd(1, 5.0));
    Objects.back().EventsContainer[0].addOperation(3, 0, "Butty2", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.2));
    Objects.back().EventsContainer[0].addOperation(4, 0, "Butty2", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty2", "image", "0", "light", vecXd(4, 1.0, 1.0, 1.0, 0.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty2", "image", "0", "scale", vecXd(2, 0.45, 0.45));
    Objects.back().EventsContainer[0].addOperation(1, 0, "Butty2", "image", "0", "scale", vecXd(2, 0.5, 0.5));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty1", "image", "0", "rotate", vecXd(1, -3.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty1", "image", "1", "rotate", vecXd(1, -3.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty", "image", "0", "rotate", vecXd(1, 4.0));
    Objects.back().EventsContainer[0].addOperation(0, 0, "Butty", "image", "1", "rotate", vecXd(1, 4.0));*/

    /*unsigned int vam = 10*1, bam = 10*1;
    for(unsigned int i = 0; i < vam; i++){
        for(unsigned int j = 0; j < bam; j++){
            if(i>0 && j>0 && i<vam-1 && j<bam-1)
                continue;
            Objects.push_back(AncestorObject("22a", listOfUniqueIDs, layerID));
            Objects.back().addGroup("cobblestone");
            Objects.back().addGroup("wall");
            Objects.back().addGroup("hello");
            Objects.back().setPos(40+i*200, 50+j*200);
            Objects.back().CollisionContainer.push_back(CollisionModule("", Objects.back().getSize(), &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
            Objects.back().CollisionContainer.back().addGroup("pupa");
            Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(200.0, 200.0));
            Objects.back().ImageContainer.push_back(ImageModule("", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
            Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "cobblestone");
            Objects.back().ImageContainer[0].changeParameters("rocks", Objects.back().imageContainerIDs, vec6d(0.0, 0.0, 400.0, 400.0, 0.0, 0.0), 0.0, vec2d(0.5, 0.5), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
        }
    }*/

    vec2d scaleFDS(1.0, 1.0);

    unsigned int vam1 = 2, bam1 = 2;
    for(unsigned int i = 0; i < vam1; i++){
        for(unsigned int j = 0; j < bam1; j++){
            if(i>0 && j>0 && i<vam1-1 && j<bam1-1)
                continue;
            if(rand()%2 == 0){
                double temp = scaleFDS.x;
                scaleFDS.x = scaleFDS.y;
                scaleFDS.y = temp;
            }
            Objects.push_back(AncestorObject("", listOfUniqueIDs, layerID));
            //Objects.back().setID("A"+intToStr8(i*1000+j));
            Objects.back().setPos(40+i*100+rand()%50, 50+j*100+rand()%50);
            Objects.back().CollisionContainer.push_back(CollisionModule("1", Objects.back().getSize(), &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
            Objects.back().CollisionContainer.back().changeParameters(true, false, vec2d(0.0, 0.0), vec2d(100.0, 100.0)*scaleFDS);
            Objects.back().ImageContainer.push_back(ImageModule("", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
            Objects.back().ImageContainer.back().connectBitmap(BitmapContainer, "puss");
            Objects.back().ImageContainer.back().changeParameters("puss", Objects.back().imageContainerIDs, vec6d(0.0, 0.0, 100.0, 100.0, 0.0, 0.0), 0.0, vec2d(1.0, 1.0)*scaleFDS, false, false, vec4d(1.0, 1.0, 1.0, 1.0));
        }
    }
/*
    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Ruchliwy");
    Objects.back().setPos(vec2d(200, 200));
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "chad");
    Objects.back().ImageContainer[0].changeParameters("smol_chad", vec6d(0.0, 0.0, 392.0, 449.0, 0.0, 0.0), 0.0, vec2d(0.3, 0.3), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setIsScaledFromCenter(true);
    Objects.back().MovementContainer.push_back(0);
    Objects.back().MovementContainer[0].setID("chad_ruch");
    Objects.back().MovementContainer[0].changeMoveParameters(1, 2, 0.5, 5.0, 10.0, 5.0, 10.0, 0.0, true);
    Objects.back().CollisionContainer.push_back(CollisionModule(0, Objects.back().getSize()));
    Objects.back().CollisionContainer[0].setID("kolizja");
    Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0, 100*0.3), vec2d(117.6-1.0, 134.7-1.0-100.0*0.3), false);
    //Objects.back().CollisionContainer[0].addToIgnoreCollisionList("1");

    //void MovementModule::changeMoveParameters(short newMovementType, short newInputType, double newBodyMass, double newWalkingSpeed, double newRunningSpeed,
      //                                    double newBaseFriction, double newMaxMomentumX, double newMoveCooldown, bool newCanMoveDiagonally)
*/
    for(unsigned int i = 0; i < 0; i++){
        Objects.push_back(AncestorObject("Obiekt"+intToStr(i), listOfUniqueIDs, layerID));
        Objects.back().ImageContainer.push_back(ImageModule("", &Objects.back().imageContainerIDs, layerID, Objects.back().getID()));
        Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "cat260");
        Objects.back().ImageContainer[0].changeParameters("Feet", Objects.back().imageContainerIDs, vec6d(0.0, 0.0, 260.0, 260.0, 0.0, 0.0), 0.0, vec2d(0.5, 0.5), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
        Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
        Objects.back().setPos(800.0, 300.0);
        //Objects.back().pos.set(3000.0, 3000.0);
        Objects.back().MovementContainer.push_back(MovementModule("ruch", &Objects.back().movementContainerIDs, layerID, Objects.back().getID()));
        //Objects.back().MovementContainer[0].changeMoveParameters(1, 0.5, 2.0, 3.5, 1.1, 9.0, 0.0);
        Objects.back().MovementContainer[0].changeMoveParameters(0, 1, 0.5, 5.0, 5.0, 5.0, 5.0, 0.0, true);
        Objects.back().MovementContainer[0].changeJumpParameters(30.0, 2, 0.5, 3.0, 30.0, 20.0, true);
        Objects.back().MovementContainer[0].changeRandomActionsSettings(true, 0.5, 2.0, 40.0, 70.0, false, 0.1);
        Objects.back().CollisionContainer.push_back(CollisionModule("kolizja", Objects.back().getSize(), &Objects.back().collisionContainerIDs, layerID, Objects.back().getID()));
        Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(130.0, 130.0));
        //Objects.back().CollisionContainer[0].addToIgnoreCollisionList("Obiekt");
    }

    for(auto &Object : Objects){
        Object.refreshCoordinates();
    }
}

void createObjects(vector <AncestorObject> & Objects, string layerID, vector <string> & listOfUniqueIDs, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer){
    /*Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Smok");
    Objects.back().setPos(vec2d(100, 100));
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer.back().connectBitmap(BitmapContainer, "dragon_h3");
    Objects.back().ImageContainer.back().setID("dragon");
    Objects.back().ImageContainer.back().setIsScaledFromCenter(false);
    Objects.back().ImageContainer.back().setUsedBitmapLayer(1);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label1");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Smoczek <3", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 0.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label2");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Dragon", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 50.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label3");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Le Dragun", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 100.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer.back().setID("label4");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("Drachen", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 150.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);

    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Amongus");
    Objects.back().setPos(vec2d(100, 400));
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "amongus_1");
    Objects.back().ImageContainer[0].setID("amongus");
    Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
    Objects.back().ImageContainer[0].setUsedBitmapLayer(1);
    Objects.back().ImageContainer[0].setScale(0.5, 0.5);
    Objects.back().VariablesContainer.push_back(0);
    Objects.back().VariablesContainer.back().setID("aaa");
    Objects.back().VariablesContainer.push_back(0);
    Objects.back().VariablesContainer.back().setID("bbb");
    Objects.back().CollisionContainer.push_back(0);

    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Doge");
    Objects.back().setPos(vec2d(500, 100));
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "chad");
    Objects.back().ImageContainer[0].changeParameters("dog", vec6d(0.0, 0.0, 392.0, 449.0, 0.0, 0.0), 0.0, vec2d(1.5, 1.5), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setRotPos(vec2d(0.0,0.0));
    Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
    Objects.back().ImageContainer[0].setUsedBitmapLayer(1);
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[1].setID("sword");
    Objects.back().ImageContainer[1].connectBitmap(BitmapContainer, "sword");
    Objects.back().ImageContainer[1].setPos(150.0, 400.0);
    Objects.back().ImageContainer[1].setScale(2.0, 2.0);
    Objects.back().ImageContainer[1].setSize(160.0, 160.0);
    Objects.back().ImageContainer[1].setUsedBitmapLayer(2);

    for(auto &Object : Objects){
        Object.refreshCoordinates();
    }*/
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
