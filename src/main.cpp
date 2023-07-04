#include "egmCompiler.h"
#include "engineLoop.h"
#include "gameWindow.h"
#include <filesystem>


void loadFontsToContainer(vector <SingleFont> & FontContainer){
    std::ifstream fontListFile;
    fontListFile.open("fonts/font_list.txt");

    short fontSizes[] = {12, 24, 48, 72};
    unsigned short i;
    std::string fileName, filePath;
    while(fontListFile >> fileName){
        if(fileName.substr(fileName.size()-4, fileName.size()) != ".ttf"){
            std::cout << "Wrong file extension! Aborting fonts loading.\n";
            fontListFile.close();
            return;
        }
        filePath = "fonts/" + fileName;
        std::cout << filePath << "\n";
        for(i = 0; i < (sizeof(fontSizes)/sizeof(*fontSizes)); i++){
            FontContainer.push_back(SingleFont());
            FontContainer.back().font = al_load_ttf_font(filePath.c_str(), fontSizes[i], 2);
            FontContainer.back().ID = fileName.substr(0, fileName.size()-4) + intToStr4(fontSizes[i]);
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

void loadBitmapsToContainer(vector <SingleBitmap> & BitmapContainer){
    BitmapContainer.reserve(11);
    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("background", "images/stars.jpg");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("background1", "images/Ilulu.jpg");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("catSmall", "images/catSmall.jpg");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("cat260", "images/cat260.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("lel", "images/lel.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("rocks1_300", "images/rocks1_300.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("rocks2_300", "images/rocks2_300.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("rocks2_200", "images/rocks2_200.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("puss", "images/catgirl.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("chad", "images/chad_doge.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("sword", "images/sword.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("gin300", "images/gin300.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("minion_1", "images/minion1.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("dragon_h3", "images/h3_dragon.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("amongus_1", "images/amongus1.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("cobblestone", "images/cobblestone.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("steve", "images/steve.png");



    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("gui_background", "gui/background.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_general", "gui/icon_general.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_image", "gui/icon_image.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_text", "gui/icon_text.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_event", "gui/icon_event.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_movement", "gui/icon_movement.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_collision", "gui/icon_collision.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_particles", "gui/icon_particles.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_variables", "gui/icon_variables.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("icon_editable_text", "gui/icon_editable_text.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("text_field_1", "gui/text_field_1.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("text_field_2", "gui/text_field_2.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("arrow_1", "gui/arrow_h3.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("plus_1", "gui/plus_h3.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("minus_1", "gui/minus_h3.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("thumb_1", "gui/thumb_h3.png");

    BitmapContainer.push_back(SingleBitmap(BitmapContainer.size()));
    BitmapContainer.back().loadBitmap("thumb_track_1", "gui/thumb_track_h3.png");
}
void freeBitmapsFromContainer(vector <SingleBitmap> & BitmapContainer){
    for(unsigned int i = 0; i < BitmapContainer.size(); i++){
        if(BitmapContainer[i].bitmap){
            al_destroy_bitmap(BitmapContainer[i].bitmap);
            std::cout << "Bitmap '" << BitmapContainer[i].ID << "' destroyed. (by GeneralBitmapContainer)\n";
        }
    }
    BitmapContainer.clear();
    if(BitmapContainer.size() == 0)
        std::cout << "\nAll bitmaps in general image container destroyed.\n\n";
}

vector<string> getAllFilesNamesWithinFolder(string folder){
    vector<string> names;
    for (const auto & entry : std::filesystem::directory_iterator(folder))
        names.push_back(entry.path().string());
    return names;
}

void createObjects1(vector <AncestorObject> & Objects, string layerID, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer, ALLEGRO_DISPLAY * window){
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

    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Amongus");
    Objects.back().setPos(vec2d(100, 400));
    Objects.back().setIsAttachedToCamera(false);
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "amongus_1");
    Objects.back().ImageContainer[0].setID("amongus");
    Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
    Objects.back().ImageContainer[0].setUsedBitmapLayer(1);
    Objects.back().ImageContainer[0].setScale(0.5, 0.5);
    Objects.back().ImageContainer[0].setIsAttachedToCamera(false);
    Objects.back().VariablesContainer.push_back(0);
    Objects.back().VariablesContainer.back().setID("timer");
    Objects.back().VariablesContainer.back().setDouble(0.0);
    Objects.back().VariablesContainer.push_back(0);
    Objects.back().VariablesContainer.back().setID("bbb");
    Objects.back().CollisionContainer.push_back(0);
    Objects.back().TextContainer.push_back(0);
    Objects.back().TextContainer.back().setID("label1");
    Objects.back().TextContainer.back().setFontID("Minecraft24");
    Objects.back().TextContainer.back().addNewContentAndResize("0", FontContainer);
    Objects.back().TextContainer.back().setIsAttachedToCamera(false);
    Objects.back().TextContainer.back().setUsedBitmapLayer(2);
    Objects.back().TextContainer.back().setPos(0.0, 0.0);
    Objects.back().TextContainer.back().setColors(255, 0, 0);
    Objects.back().EveContainer.push_back(0);
    Objects.back().EveContainer.back().primaryTriggerTypes.push_back("keyboard_released");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("a"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(3);
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("b"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(3);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("*");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("c"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(2);
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("d"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(2);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("+");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("**");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("d"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(6562);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("!=");
    
    /*  !(a > b && (c || d == e))&&(f || !g && h != i)
    Objects.back().EveContainer.push_back(0);
    Objects.back().EveContainer.back().primaryTriggerTypes.push_back("keyboard_released");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("a"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(99);
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("b"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(965);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back(">");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("c"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(false);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igT");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("d"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(0);
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("e"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(0);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("==");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("||");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("!");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("f"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(false);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igT");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("g"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(true);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("!");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("h"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(420);
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("i"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setInt(69);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("!=");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("||");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");*/

    /* a && b || c && d && e && f
    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("Boby");
    Objects.back().EveContainer.push_back(0);
    Objects.back().EveContainer.back().primaryTriggerTypes.push_back("keyboard_released");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("a"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(false);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("b"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(false);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igT");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("c"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(true);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("d"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(true);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("e"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(true);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("igF");
    Objects.back().EveContainer.back().ConditionalChain.push_back(TriggerClass("f"));
    Objects.back().EveContainer.back().ConditionalChain.back().source = "literal";
    Objects.back().EveContainer.back().ConditionalChain.back().Literal.setBool(true);
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("&&");
    Objects.back().EveContainer.back().ConditionalChain.back().operators.push_back("||");*/
    
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

    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("par");
    Objects.back().setPos(500.0, 500.0);
    Objects.back().setIsAttachedToCamera(false);
    Objects.back().ParticlesContainer.push_back(ParticleEffectModule(Objects.back().ParticlesContainer.size()));
    Objects.back().ParticlesContainer[0].setID("par");
    Objects.back().ParticlesContainer[0].changeSpawningParameters(false, true, true, 5000, 5000, 5000, 0.05);
    Objects.back().ParticlesContainer[0].changeMotionParameters(3.5, 5.7, -10.0, 10.0, vec2d(0.0, 0.0), vec2d(0.0, 0.0), 0.00, 0.00,
        0.4, 0.7, 0.0, 360.0, -0.0, 0.0, 0.0, 0.0);
    Objects.back().ParticlesContainer[0].changeGeneralParameters(vec2d(0.0, 0.0), vec2d(0.0, 0.0), 10.0, 20.0, 3.0, 10.0,
        0.8, 1.0, -2.0, 2.0, 0, true, false, false);
    Objects.back().ParticlesContainer[0].setSpawnKeyBindShort(ALLEGRO_KEY_P);
    Objects.back().ParticlesContainer[0].addImage("face");
    Objects.back().ParticlesContainer[0].setUseImageAsParticles(false);
    Objects.back().ParticlesContainer[0].addColor(0, 255, 0);
    Objects.back().ParticlesContainer[0].setMinBasicSpeed(0.1);
    Objects.back().ParticlesContainer[0].setMaxBasicSpeed(10.0);
    Objects.back().ParticlesContainer[0].setMinSpeed(0.1);
    Objects.back().ParticlesContainer[0].setMaxSpeed(10.0);
    Objects.back().ParticlesContainer[0].setIsAttachedToCamera(false);
    Objects.back().MovementContainer.push_back(MovementModule(Objects.back().MovementContainer.size()));

    //changeMoveParameters(short newMovementType, short newInputType, double newBodyMass, double newWalkingSpeed, double newRunningSpeed,
    //                                      double newBaseFriction, double newMaxMomentumX, double newMoveCooldown, bool newCanMoveDiagonally)

    Objects.back().MovementContainer[0].changeMoveParameters(1, 2, 0.2, 250.0, 1.0, 1.0, 10.0, 0.0, true);
    //Objects.back().MovementContainer[0].changeJumpParameters(newJumpSpeed, newAllowedJumps, newJumpCooldownDuration, newGravitation, newMinMomentumY, newMaxMomentumY, newResetMomentumWhenJumping)
    Objects.back().MovementContainer[0].changeJumpParameters(18.0, 1, 0.5, 3.0, 20.0, 20.0, true);
    Objects.back().MovementContainer[0].bindKeys(ALLEGRO_KEY_W, ALLEGRO_KEY_D, ALLEGRO_KEY_S, ALLEGRO_KEY_A, ALLEGRO_KEY_W, ALLEGRO_KEY_LSHIFT);
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].setID("steve");
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "steve");
    Objects.back().ImageContainer[0].changeParameters("lel", vec6d(0.0, 0.0, 100.0, 100.0, 0.0, 0.0), 0.0, vec2d(1.0, 1.0), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setUsedBitmapLayer(1);
    Objects.back().ImageContainer[0].setIsAttachedToCamera(false);
    //Objects.back().ImageContainer[0].setIsActive();
    //Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    //Objects.back().ImageContainer[1].connectBitmap(BitmapContainer, "cat260");
    //Objects.back().ImageContainer[1].changeParameters("cat", vec6d(0.0, 0.0, 260.0, 260.0, 0.0, 0.0), 0.0, vec2d(0.5, 0.5), false, false, vec4d(0.0, 0.0, 0.0, 0.0));
    Objects.back().CollisionContainer.push_back(CollisionModule(Objects.back().CollisionContainer.size(), Objects.back().getSize()));
    Objects.back().CollisionContainer[0].setID("1");
    Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(100.0, 100.0));
    Objects.back().CollisionContainer[0].addToIgnoreCollisionList("pupa");

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

    unsigned int vam = 10*1, bam = 10*1;
    for(unsigned int i = 0; i < vam; i++){
        for(unsigned int j = 0; j < bam; j++){
            if(i>0 && j>0 && i<vam-1 && j<bam-1)
                continue;
            Objects.push_back(AncestorObject(Objects.size(), layerID));
            Objects.back().setPos(40+i*200, 50+j*200);
            Objects.back().CollisionContainer.push_back(CollisionModule(Objects.back().CollisionContainer.size(), Objects.back().getSize()));
            Objects.back().CollisionContainer.back().addGroup("pupa");
            Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(200.0, 200.0));
            Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
            Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "cobblestone");
            Objects.back().ImageContainer[0].changeParameters("rocks", vec6d(0.0, 0.0, 400.0, 400.0, 0.0, 0.0), 0.0, vec2d(0.5, 0.5), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
        }
    }

    vec2d scaleFDS(1.0, 1.0);

    unsigned int vam1 = 0, bam1 = 0;
    for(unsigned int i = 0; i < vam1; i++){
        for(unsigned int j = 0; j < bam1; j++){
            if(i>0 && j>0 && i<vam1-1 && j<bam1-1)
                continue;
            if(rand()%2 == 0){
                double temp = scaleFDS.x;
                scaleFDS.x = scaleFDS.y;
                scaleFDS.y = temp;
            }
            Objects.push_back(AncestorObject(Objects.size(), layerID));
            //Objects.back().setID("A"+intToStr8(i*1000+j));
            Objects.back().setPos(40+i*100+rand()%50, 50+j*100+rand()%50);
            Objects.back().CollisionContainer.push_back(CollisionModule(Objects.back().CollisionContainer.size(), Objects.back().getSize()));
            Objects.back().CollisionContainer[0].setID("1");
            Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(100.0, 100.0)*scaleFDS);
            Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
            Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "puss");
            Objects.back().ImageContainer[0].changeParameters("puss", vec6d(0.0, 0.0, 100.0, 100.0, 0.0, 0.0), 0.0, vec2d(1.0, 1.0)*scaleFDS, false, false, vec4d(1.0, 1.0, 1.0, 1.0));
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
        Objects.push_back(AncestorObject(Objects.size(), layerID));
        Objects.back().setID("Obiekt"+intToStr8(i));
        Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
        Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "cat260");
        Objects.back().ImageContainer[0].changeParameters("Feet", vec6d(0.0, 0.0, 260.0, 260.0, 0.0, 0.0), 0.0, vec2d(0.5, 0.5), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
        Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
        Objects.back().setPos(800.0, 300.0);
        //Objects.back().pos.set(3000.0, 3000.0);
        Objects.back().MovementContainer.push_back(MovementModule(Objects.back().MovementContainer.size()));
        Objects.back().MovementContainer[0].setID("ruch");
        //Objects.back().MovementContainer[0].changeMoveParameters(1, 0.5, 2.0, 3.5, 1.1, 9.0, 0.0);
        Objects.back().MovementContainer[0].changeMoveParameters(0, 1, 0.5, 5.0, 5.0, 5.0, 5.0, 0.0, true);
        Objects.back().MovementContainer[0].changeJumpParameters(30.0, 2, 0.5, 3.0, 30.0, 20.0, true);
        Objects.back().MovementContainer[0].changeRandomActionsSettings(true, 0.5, 2.0, 40.0, 70.0, false, 0.1);
        Objects.back().CollisionContainer.push_back(CollisionModule(Objects.back().CollisionContainer.size(), Objects.back().getSize()));
        Objects.back().CollisionContainer[0].setID("kolizja");
        Objects.back().CollisionContainer[0].changeParameters(true, false, vec2d(0.0, 0.0), vec2d(130.0, 130.0));
        //Objects.back().CollisionContainer[0].addToIgnoreCollisionList("Obiekt");
    }

    for(auto &Object : Objects){
        Object.refreshCoordinates();
    }
}

void createObjects(vector <AncestorObject> & Objects, string layerID, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer, ALLEGRO_DISPLAY * window){
    Objects.push_back(AncestorObject(Objects.size(), layerID));
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
    }
}


void createObjects0(vector <AncestorObject> & Objects, string layerID, vector <SingleFont> & FontContainer, vector <SingleBitmap> & BitmapContainer, ALLEGRO_DISPLAY * window){
    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setCanBeSelected(false);
    Objects.back().TextContainer.push_back(TextModule(0));
    Objects.back().TextContainer[0].addNewContent("FPS: ");
    Objects.back().TextContainer[0].changeParameters("king", vec4d(10.0, 10.0, 200.0, 200.0), vec3d(255.0, 0.0, 0.0), "Minecraft24", vec2d(1.0, 1.0), 0.0, 2, 0, 0);
    Objects.back().TextContainer[0].setIsAttachedToCamera(true);
    Objects.back().TextContainer[0].setUsedBitmapLayer(-1);


    Objects.push_back(AncestorObject(Objects.size(), layerID));
    Objects.back().setID("background");
    Objects.back().setPos(vec2d(0, 0));
    Objects.back().setCanBeSelected(false);
    Objects.back().ImageContainer.push_back(ImageModule(Objects.back().ImageContainer.size()));
    Objects.back().ImageContainer[0].connectBitmap(BitmapContainer, "background");
    Objects.back().ImageContainer[0].changeParameters("background", vec6d(0.0, 0.0, 1280.0, 853.0, 0.0, 0.0), 0.0, vec2d(1.0, 1.0), false, false, vec4d(1.0, 1.0, 1.0, 1.0));
    Objects.back().ImageContainer[0].setIsScaledFromCenter(false);
    Objects.back().ImageContainer[0].resize(vec2d(SCREEN_W, SCREEN_H));
    Objects.back().deactivate();
}

void createCameras(vector <Camera2D> & Cameras){
    Cameras.push_back(Camera2D("Cam0", true, vec2d(0.0, 0.0), vec2d(SCREEN_W/3.0, SCREEN_H), vec2d(0.0, 0.0)));
    Cameras.back().setZoom(1.0, 0.05, 0.01, 10.0);
    Cameras.back().setSpeed(5.0);
    Cameras.back().setFollowedObjectID("par");
    Cameras.back().setFollowedLayerID("L1");
    Cameras.back().setFollowedImageID("");
    Cameras.back().setIsFollowingObject(false);
    Cameras.back().setKeyBinds(ALLEGRO_KEY_PAD_1, ALLEGRO_KEY_PAD_2, ALLEGRO_KEY_PAD_3, ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J);
    Cameras.back().addVisibleLayer("Editor");
    Cameras.back().addVisibleLayer("L1");

    Cameras.push_back(Camera2D("Cam1", true, vec2d(560.0, 0.0), vec2d(300.0, 300.0), vec2d(0.0, 0.0)));
    Cameras.back().setZoom(1.0, 0.05, 0.01, 10.0);
    Cameras.back().setSpeed(5.0);
    Cameras.back().setFollowedObjectID("par");
    Cameras.back().setFollowedLayerID("L1");
    Cameras.back().setFollowedImageID("");
    Cameras.back().setIsFollowingObject(false);
    Cameras.back().setKeyBinds(ALLEGRO_KEY_PAD_1, ALLEGRO_KEY_PAD_2, ALLEGRO_KEY_PAD_3, ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J);
    Cameras.back().addVisibleLayer("Editor");
    Cameras.back().addVisibleLayer("L1");
    Cameras.back().pinToCamera("Cam0");
    
    Cameras.push_back(Camera2D("Cam2", true, vec2d(300.0, 0.0), vec2d(300.0, 300.0), vec2d(0.0, 0.0)));
    Cameras.back().setZoom(1.0, 0.05, 0.01, 10.0);
    Cameras.back().setSpeed(5.0);
    Cameras.back().setFollowedObjectID("par");
    Cameras.back().setFollowedLayerID("L1");
    Cameras.back().setFollowedImageID("");
    Cameras.back().setIsFollowingObject(false);
    Cameras.back().setKeyBinds(ALLEGRO_KEY_PAD_1, ALLEGRO_KEY_PAD_2, ALLEGRO_KEY_PAD_3, ALLEGRO_KEY_I, ALLEGRO_KEY_L, ALLEGRO_KEY_K, ALLEGRO_KEY_J);
    Cameras.back().addVisibleLayer("Editor");
    Cameras.back().addVisibleLayer("L1");
    Cameras.back().pinToCamera("Cam1");
}

Fps fps;

int main(){
    srand(time(NULL));
    egmCompiler Compiler;
    Compiler.loadTestData();
    Compiler.writeAllText();

    EngineLoop EngineLoop1("EGM");
    vector <SingleFont> FontContainer;
    vector <SingleBitmap> BitmapContainer;
    vector <LayerClass> Layers;
    vector <Camera2D> Cameras;


    EngineLoop1.initAllegro();
    loadFontsToContainer(FontContainer);
    std::cout << FontContainer.size() << "\n";

    loadBitmapsToContainer(BitmapContainer);

    createCameras(Cameras);
    EngineLoop1.updateAllForestOfCameras(Cameras);


    Layers.push_back(LayerClass("Editor", true, vec2d(0.0, 0.0), vec2d(SCREEN_W, SCREEN_H)));
    prepareEditorWindow(Layers[0].Objects, Layers[0].getID(), FontContainer, BitmapContainer);
    Layers[0].Objects[0].deactivate();
    createObjects0(Layers[0].Objects, Layers[0].getID(), FontContainer, BitmapContainer, EngineLoop1.window);


    Layers.push_back(LayerClass("L1", true, vec2d(0.0, 0.0), vec2d(SCREEN_W, SCREEN_H)));
    createObjects1(Layers[1].Objects, Layers[1].getID(), FontContainer, BitmapContainer, EngineLoop1.window);
    
    unsigned numberOfObjects = 0;

    for(LayerClass & layer : Layers){
        for(AncestorObject & obj : layer.Objects){
            obj.createVectorsOfIds();
        }
        if(layer.createListOfUniqueIDs())
            goto uniquenessViolated;
        numberOfObjects += layer.Objects.size();
    }
    std::cout << "Number of objects: " << numberOfObjects << "\n";

    EngineLoop1.updateBaseOfTriggerableObjects(Layers, Cameras);

    EngineLoop1.startTimer();
    
    //Main loop
    do{
        EngineLoop1.windowLoop(Layers, Cameras, FontContainer, fps, BitmapContainer);
    }while(!EngineLoop1.closeEditor);

    uniquenessViolated:

    vector <string> fileNames;
    #if _WIN32
        fileNames = getAllFilesNamesWithinFolder("D:/k\271cik programowanka/Easy Game Maker");
    #elif __linux__
        fileNames = getAllFilesNamesWithinFolder(".");
    #endif
    for(auto name : fileNames)
        std::cout << name << std::endl;

    EngineLoop1.exitAllegro();

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
