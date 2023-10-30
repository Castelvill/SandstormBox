#include "imageModule.h"
#include <allegro5/allegro_native_dialog.h>

void draw_bitmap(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float red, float green, float blue, float alpha,
    float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags
){
    if(bitmap == NULL || bitmap == nullptr){
        return;
    }
    double cx1 = cx + sw/2;
    double cy1 = cy + sh/2;
    double dx1 = dx+cx*xscale+sw/2+(xscale-1)*sw/2;
    double dy1 = dy+cy*xscale+sh/2+(yscale-1)*sh/2;
    al_draw_tinted_scaled_rotated_bitmap_region(bitmap, sx, sy, sw, sh, al_map_rgba_f(red, green, blue, alpha), cx1, cy1, dx1, dy1, xscale, yscale, angle, flags);
}
void draw_bitmap(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float red, float green, float blue, float alpha,
    float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, bool mirrorX, bool mirrorY
){
    if(bitmap == NULL || bitmap == nullptr){
        return;
    }
    double cx1 = cx + sw/2;
    double cy1 = cy + sh/2;
    double dx1 = dx+cx*xscale+sw/2+(xscale-1)*sw/2;
    double dy1 = dy+cy*xscale+sh/2+(yscale-1)*sh/2;

    int flags;

    if(!mirrorX && !mirrorY)
        flags = 0;
    else if(mirrorX && !mirrorY)
        flags = ALLEGRO_FLIP_VERTICAL;
    else if(!mirrorX && mirrorY)
        flags = ALLEGRO_FLIP_HORIZONTAL;
    else
        flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL;

    al_draw_tinted_scaled_rotated_bitmap_region(bitmap, sx, sy, sw, sh, al_map_rgba_f(red, green, blue, alpha), cx1, cy1, dx1, dy1, xscale, yscale, angle, flags);
}


SingleBitmap::SingleBitmap(){
    filePath = "";
    alias = "";
    bitmap = nullptr;
}
void SingleBitmap::loadBitmap(string newAlias, string newFilePath, string EXE_PATH){
    if(bitmap)
        al_destroy_bitmap(bitmap);
    filePath = newFilePath;
    alias = newAlias;
    bitmap = al_load_bitmap((EXE_PATH + filePath).c_str());
    if(!bitmap){
        cout << "Failed to load a bitmap: File \'" << filePath << "\' not found.\n";
        //al_show_native_message_box(window, "Error", "", "Can't load an image.", NULL, ALLEGRO_MESSAGEBOX_ERROR);

        bitmap = al_load_bitmap((EXE_PATH + "images/error.png").c_str());
        if(!bitmap){
            bitmap = al_load_bitmap((EXE_PATH + "error.png").c_str());
        }
        if(!bitmap){
            cout << "File 'error.png' not found in \'" << EXE_PATH << "\'.\n";
        }
        return;
    }
}

void ImageModule::setUpNewInstance(){
    rotPos.set(0.0, 0.0);
    start.set(0.0, 0.0);
    frameSize.set(0.0, 0.0);
    currentFrame.set(0.0, 0.0);
    rotateAngle = 0.0;
    scale.set(1.0, 1.0);
    mirrorX = false;
    mirrorY = false;
    for(int i = 0; i < 4; i++)
        imageColor[i] = 1.0;
    lightLevel = 0.0; //below 0.0 it's geting darker, above 0.0 it's getting brighter
    for(int i = 0; i < 3; i++)
        lightColor[i] = 1.0;
    isScaledFromCenter = false;
    image = nullptr;
    lightBitmap = nullptr;
    isBitmapFromContainer[0] = false;
    isBitmapFromContainer[1] = false;
    usedBitmapLayer = 0;
}
ImageModule::ImageModule(){
    setUpNewInstance();
}
ImageModule::ImageModule(string newAlias, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newAlias, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
ImageModule::ImageModule(unsigned int newAlias, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newAlias, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
void ImageModule::clone(const ImageModule& Image, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID){
    PrimaryModule::clone(Image, listOfIDs, newLayerID, newObjectID, changeOldID);
    rotPos = Image.rotPos;
    start = Image.start;
    frameSize = Image.frameSize;
    currentFrame = Image.currentFrame;
    isBitmapFromContainer[0] = Image.isBitmapFromContainer[0];
    isBitmapFromContainer[1] = Image.isBitmapFromContainer[1];
    rotateAngle = Image.rotateAngle;
    mirrorX = Image.mirrorX;
    mirrorY = Image.mirrorY;
    std::copy(Image.imageColor, Image.imageColor+4, imageColor);
    lightLevel = Image.lightLevel;
    std::copy(Image.lightColor, Image.lightColor+4, lightColor);
    usedBitmapLayer = Image.usedBitmapLayer;
    image = nullptr;
    lightBitmap = nullptr;
    if(Image.image){
        if(!Image.isBitmapFromContainer[0]){
            image = al_clone_bitmap(Image.image);
        }
        else{
            image = Image.image;
        }
    }
    if(Image.lightBitmap){
        if(!Image.isBitmapFromContainer[1]){
            lightBitmap = al_clone_bitmap(Image.lightBitmap);
        }
        else{
            lightBitmap = Image.lightBitmap;
        }
    }
    if(image || lightBitmap){
        //cout << "Copy has been made!\n";
    }
}
void ImageModule::clear(){
    if(image){
        if(!isBitmapFromContainer[0]){
            al_destroy_bitmap(image);
            cout << "Bitmap '"<< ID <<"' destroyed! (by ImageModule)\n";
        }
        image = nullptr;
    }
    if(lightBitmap){
        if(!isBitmapFromContainer[1]){
            al_destroy_bitmap(lightBitmap);
            cout << "Light bitmap '"<< ID <<"' destroyed! (by ImageModule)\n";
        }
        lightBitmap = nullptr;
    }
}
void ImageModule::loadImage(string newFilePath, string newAlias, string EXE_PATH){
    if(image && !isBitmapFromContainer[0])
        al_destroy_bitmap(image);
    else
        image = nullptr;
    if(lightBitmap && !isBitmapFromContainer[1])
        al_destroy_bitmap(lightBitmap);
    else
        lightBitmap = nullptr;

    isBitmapFromContainer[0] = false;
    isBitmapFromContainer[1] = false;
    imageAlias = newAlias;
    imageFilePath = newFilePath;
    image = al_load_bitmap((EXE_PATH + imageFilePath).c_str());
    if(!image){
        cout << "Error: In " << __FUNCTION__ << ": Loading image failed: File \'" << imageFilePath << "\' does not exist.\n";
        return;
    }
    size.set(al_get_bitmap_width(image), al_get_bitmap_height(image));
    lightBitmap = al_create_bitmap(al_get_bitmap_width(image), al_get_bitmap_height(image));
    al_set_target_bitmap(lightBitmap);
    al_clear_to_color(al_map_rgb(255, 255, 255));

}
void ImageModule::connectBitmap(vector <SingleBitmap> & BitmapContainer, string newFilePath, string newAlias, string EXE_PATH){
    //If the bitmap doesn't exist, return without changing the bitmap.
    unsigned int i = 0;
    bool bitmapExists = false;
    
    if(newFilePath != ""){
        for(; i < BitmapContainer.size(); i++){
            if(BitmapContainer[i].filePath == newFilePath){
                bitmapExists = true;
                break;
            }
        }
    }
    else if(newAlias != ""){
        for(; i < BitmapContainer.size(); i++){
            if(BitmapContainer[i].alias == newAlias){
                bitmapExists = true;
                break;
            }
        }
    }
    
    if(!bitmapExists){
        if(newAlias != ""){
            cout << "Error: In " << __FUNCTION__ << ": Bitmap \'" << newAlias << "\' not found.\n";
        }
        else if(newFilePath != ""){
            cout << "Error: In " << __FUNCTION__ << ": File \'" << newFilePath << "\' not loaded into memory.\n";
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Path to the file and the alias of the bitmap were not provided.\n";
        }
        return;
    }

    imageFilePath = newFilePath;
    imageAlias = newAlias;

    if(image && !isBitmapFromContainer[0]){
        al_destroy_bitmap(image);
    }
    else{
        image = nullptr;
    }
    if(lightBitmap && !isBitmapFromContainer[1]){
        al_destroy_bitmap(lightBitmap);
    } 
    else{
        lightBitmap = nullptr;
    }
        
    isBitmapFromContainer[0] = true;
    isBitmapFromContainer[1] = true;

    image = BitmapContainer[i].bitmap;

    if(!image){
        image = al_create_bitmap(100, 100);
        if(imageAlias != ""){
            cout << "Error: In " << __FUNCTION__ << ": Bitmap \'" << imageAlias << "\' not found.\n";
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": File \'" << imageFilePath << "\' not loaded into memory.\n";
        }
        return;
    }
    size.set(al_get_bitmap_width(image), al_get_bitmap_height(image));
    lightBitmap = al_create_bitmap(al_get_bitmap_width(image), al_get_bitmap_height(image));
    al_set_target_bitmap(lightBitmap);
    al_clear_to_color(al_map_rgb(255, 255, 255));
}
void ImageModule::loadLight(string newFilePath, string newAlias, string EXE_PATH){
    if(lightBitmap && !isBitmapFromContainer[1]){
        al_destroy_bitmap(lightBitmap);
    }
    else{
        lightBitmap = nullptr;
    }
        
    lightFilePath = newFilePath;
    lightAlias = newAlias;

    isBitmapFromContainer[1] = false;
    lightBitmap = al_load_bitmap((EXE_PATH + lightFilePath).c_str());
    if(!lightBitmap){
        cout << "Error: In " << __FUNCTION__ << ": Loading image failed: File \'" << imageFilePath << "\' does not exist.\n";
        return;
    }
}
void ImageModule::connectLightBitmap(vector <SingleBitmap> & BitmapContainer, string newFilePath, string newAlias, string EXE_PATH){
    //If the bitmap doesn't exist, return without changing the bitmap.
    unsigned int i = 0;
    bool bitmapExists = false;
    
    if(newFilePath != ""){
        for(; i < BitmapContainer.size(); i++){
            if(BitmapContainer[i].filePath == newFilePath){
                bitmapExists = true;
                break;
            }
        }
    }
    else if(newAlias != ""){
        for(; i < BitmapContainer.size(); i++){
            if(BitmapContainer[i].alias == newAlias){
                bitmapExists = true;
                break;
            }
        }
    }
    
    if(!bitmapExists){
        if(newAlias != ""){
            cout << "Error: In " << __FUNCTION__ << ": Bitmap \'" << newAlias << "\' not found.\n";
        }
        else if(newFilePath != ""){
            cout << "Error: In " << __FUNCTION__ << ": File \'" << newFilePath << "\' not loaded into memory.\n";
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": Path to the file and the alias of the bitmap were not provided.\n";
        }
        return;
    }

    lightFilePath = newFilePath;
    lightAlias = newAlias;


    if(lightBitmap && !isBitmapFromContainer[1]){
        al_destroy_bitmap(lightBitmap);
    } 
    else{
        lightBitmap = nullptr;
    }
        
    isBitmapFromContainer[0] = true;
    isBitmapFromContainer[1] = true;

    lightBitmap = BitmapContainer[i].bitmap;

    if(!lightBitmap){
        lightBitmap = al_create_bitmap(100, 100);
        if(lightAlias != ""){
            cout << "Error: In " << __FUNCTION__ << ": Bitmap \'" << lightAlias << "\' not found.\n";
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": File \'" << lightFilePath << "\' not loaded into memory.\n";
        }
        return;
    }
}
void ImageModule::checkImage(ALLEGRO_DISPLAY * window, string EXE_PATH){
    if(!image) {
        al_show_native_message_box(window, "Error", "", "Can't load an image.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        image = al_load_bitmap((EXE_PATH + "images/error.png").c_str());
        size.set(al_get_bitmap_width(image), al_get_bitmap_height(image));
        if(!image){
            al_show_native_message_box(window, "Error", "", "You're fucked.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        }
    }
}
void ImageModule::drawImage(vec2d base, Camera2D Camera, bool outSourcing){
    if((!outSourcing && !getIsActive()) || image == NULL || image == nullptr){
        return;
    }
    vec2d newScale(scale);

    if(!isAttachedToCamera){
        base.set(Camera.translateWithZoom(base, pos));
        newScale.multiply(Camera.zoom);
    }
    else{
        base.set(base+pos);
    }

    float red = imageColor[0];
    float green = imageColor[1];
    float blue = imageColor[2];
    float alpha = imageColor[3];

    double radians = (rotateAngle*M_PI)/180.0;
    if(!isScaledFromCenter){
        draw_bitmap(image, start.x, start.y, size.x, size.y, red, green, blue, alpha, rotPos.x, rotPos.y,
            base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);

        if(lightLevel > 0.0){
            //Blending allows to mix colors of different bitmaps
            //Starting the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

            draw_bitmap(lightBitmap, start.x, start.y, size.x, size.y, lightColor[0], lightColor[1], lightColor[2], lightLevel, rotPos.x, rotPos.y,
                base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);

            //Ending the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        }
    }
    else{
        if(!isAttachedToCamera){
            base.translate(-((scale-1)*size*Camera.zoom)/2);
        }
        else{
            base.translate(size*(1-scale)/2);
        }

        draw_bitmap(image, start.x, start.y, size.x, size.y, red, green, blue, alpha, rotPos.x, rotPos.y,
            base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);


        if(lightLevel > 0.0){
            //Blending allows to mix colors of different bitmaps
            //Starting the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

            draw_bitmap(lightBitmap, start.x, start.y, size.x, size.y, lightColor[0], lightColor[1], lightColor[2], lightLevel, rotPos.x, rotPos.y,
                base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);

            //Ending the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        }
    }
}
void ImageModule::drawFrame(vec2d base){
    if(!getIsActive() || image == NULL || image == nullptr){
        return;
    }
    double radians = (rotateAngle*M_PI)/180.0;
    vec2d start2, add;
    start2.set(currentFrame.x * frameSize.x, currentFrame.y * frameSize.y);
    add.set(size.x/2, size.y/2);

    float red = imageColor[0];
    float green = imageColor[1];
    float blue = imageColor[2];
    float alpha = imageColor[3];

    if(lightLevel < 0.0){
        red -= lightLevel;
        green -= lightLevel;
        blue -= lightLevel;
        if(red < 0.0)
            red = 0.0;
        if(green < 0.0)
            green = 0.0;
        if(blue < 0.0)
            blue = 0.0;
    }

    if(isScaledFromCenter){
        if(!mirrorX && !mirrorY)
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, 0);
        else if(mirrorX && !mirrorY)
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL);
        else if(!mirrorX && mirrorY)
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_HORIZONTAL);
        else
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL);

        if(lightLevel > 0.0){
            //Blending allows us to mix colors of different bitmaps
            //Starting the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

            if(!mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, 0);
            else if(mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL);
            else if(!mirrorX && mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_HORIZONTAL);
            else
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL);

            //Ending the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        }
    }
    else{
        vec2d add2;
        add2.set(((frameSize.x-1)*scale.x)/2, ((frameSize.y-1)*scale.y)/2);
        if(!mirrorX && !mirrorY)
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, 0);
        else if(mirrorX && !mirrorY)
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL);
        else if(!mirrorX && mirrorY)
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_HORIZONTAL);
        else
            al_draw_tinted_scaled_rotated_bitmap_region(image, start2.x, start2.y, size.x, size.y, al_map_rgba_f(red, green, blue, alpha), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL);

        if(lightLevel > 0.0){
            //Blending allows us to mix colors of different bitmaps
            //Starting the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

            if(!mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, 0);
            else if(mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL);
            else if(!mirrorX && mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_HORIZONTAL);
            else
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL);

            //Ending the blending process
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        }
    }
}
void ImageModule::setRotPos(vec2d newRotPos){
    rotPos.set(newRotPos);
}
void ImageModule::setCenterX(double x){
    rotPos.x = x;
}
void ImageModule::setCenterY(double y){
    rotPos.y = y;
}
void ImageModule::setStartX(double x){
    start.x = x;
}
void ImageModule::setStartY(double y){
    start.y = y;
}
void ImageModule::setFrameSizeX(double x){
    frameSize.x = x;
}
void ImageModule::setFrameSizeY(double y){
    frameSize.y = y;
}
void ImageModule::setCurrentFrameX(int x){
    currentFrame.x = x;
}
void ImageModule::setCurrentFrameY(int y){
    currentFrame.y = y;
}
void ImageModule::setRotation(double newRotateAngle){
    rotateAngle = newRotateAngle;
}
void ImageModule::addRotation(double newRotateAngle){
    rotateAngle += newRotateAngle;
}
void ImageModule::setMirror(bool newMirrorX, bool newMirrorY){
    mirrorX = newMirrorX;
    mirrorY = newMirrorY;
}
void ImageModule::setMirrorX(bool newMirrorX){
    mirrorX = newMirrorX;
}
void ImageModule::setMirrorY(bool newMirrorY){
    mirrorY = newMirrorY;
}
void ImageModule::setImageColor(float newImageColor[4]){
    for(unsigned i = 0; i < 4; i++){
        if(newImageColor[i] > 1){
            newImageColor[i] = 1.0;
        }
        else if(newImageColor[i] < 0){
            newImageColor[i] = 0.0;
        }
        else{
            imageColor[i] = newImageColor[i];
        }
    }
}
void ImageModule::setImageColor(vec4d newImageColor){
    for(unsigned i = 0; i < 4; i++){
        if(newImageColor.val[i] > 1){
            imageColor[i] = 1.0;
        }
        else if(newImageColor.val[i] < 0){
            imageColor[i] = 0.0;
        }
        else{
            imageColor[i] = newImageColor.val[i];
        }
    }
}
void ImageModule::setImageColor(float newImageColor, char whichColor){
    if(whichColor == 'r')
        imageColor[0] = newImageColor;
    if(whichColor == 'g')
        imageColor[1] = newImageColor;
    if(whichColor == 'b')
        imageColor[2] = newImageColor;
    if(whichColor == 'a')
        imageColor[3] = newImageColor;
}
void ImageModule::setLightColor(vec3d newLightColor, float newLightLevel){
    lightLevel = newLightLevel;
    for(unsigned int i = 0; i < 3; i++)
        lightColor[i] = newLightColor.val[i];
}
void ImageModule::setLightColor(float newLightColor, char whichLight){
    if(whichLight == 'r' || whichLight == 'x')
        lightColor[0] = newLightColor;
    if(whichLight == 'g' || whichLight == 'y')
        lightColor[1] = newLightColor;
    if(whichLight == 'b' || whichLight == 'z')
        lightColor[2] = newLightColor;
    if(whichLight == 'a' || whichLight == 'l')
        lightLevel = newLightColor;
}
void ImageModule::setUsedBitmapLayer(int newLayer){
    usedBitmapLayer = newLayer;
}
float ImageModule::getLightLevel() const{
    return lightLevel;
}
void ImageModule::changeParameters(string newAlias, vector<string> & listOfIDs, vec6d dimPos, double newRotateAngle, vec2d newScale, bool newMirrorX, bool newMirrorY, vec4d newImageColors){
    setID(newAlias, listOfIDs);
    setPos(dimPos.val[0], dimPos.val[1]);
    setSize(dimPos.val[2], dimPos.val[3]);
    setScale(newScale);
    start.set(dimPos.val[4], dimPos.val[5]);
    rotateAngle = newRotateAngle;
    mirrorX = newMirrorX;
    mirrorY = newMirrorY;
    for(int i = 0; i < 4; i++)
        imageColor[i] = newImageColors.val[i];
    if(image)
        al_convert_mask_to_alpha(image, al_map_rgb(255, 0, 255));
}
void ImageModule::modifyFrames(vec2d newFrameSize){
    frameSize.set(newFrameSize);
    if(!image)
        return;
    if(frameSize.x > al_get_bitmap_width(image))
        frameSize.x = al_get_bitmap_width(image);
    if(frameSize.y > al_get_bitmap_height(image))
        frameSize.y = al_get_bitmap_height(image);
}
void ImageModule::chooseFrames(vec2i newCurrentFrame){
    if(!image)
        return;
    if((newCurrentFrame.x + 1) * frameSize.x <= al_get_bitmap_width(image))
        currentFrame.x = newCurrentFrame.x;
    else
        currentFrame.x = 0;
    if((newCurrentFrame.y + 1) * frameSize.y <= al_get_bitmap_height(image))
        currentFrame.y = newCurrentFrame.y;
    else
        currentFrame.y = 0;
}
void ImageModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "rot_pos_x"){
        BasePointers.back().setPointer(&rotPos.x);
    }
    else if(attribute == "rot_pos_y"){
        BasePointers.back().setPointer(&rotPos.y);
    }
    else if(attribute == "start_y"){
        BasePointers.back().setPointer(&start.y);
    }
    else if(attribute == "start_y"){
        BasePointers.back().setPointer(&start.y);
    }
    else if(attribute == "frame_size_y"){
        BasePointers.back().setPointer(&frameSize.y);
    }
    else if(attribute == "frame_size_y"){
        BasePointers.back().setPointer(&frameSize.y);
    }
    else if(attribute == "rotate_angle"){
        BasePointers.back().setPointer(&rotateAngle);
    }
    else if(attribute == "mirror_x"){
        BasePointers.back().setPointer(&mirrorX);
    }
    else if(attribute == "mirror_y"){
        BasePointers.back().setPointer(&mirrorY);
    }
    else if(attribute == "image_color_r"){
        BasePointers.back().setPointer(&imageColor[0]);
    }
    else if(attribute == "image_color_g"){
        BasePointers.back().setPointer(&imageColor[1]);
    }
    else if(attribute == "image_color_b"){
        BasePointers.back().setPointer(&imageColor[2]);
    }
    else if(attribute == "image_color_a"){
        BasePointers.back().setPointer(&imageColor[3]);
    }
    else if(attribute == "light_level"){
        BasePointers.back().setPointer(&lightLevel);
    }
    else if(attribute == "light_color_r"){
        BasePointers.back().setPointer(&lightColor[0]);
    }
    else if(attribute == "light_color_g"){
        BasePointers.back().setPointer(&lightColor[1]);
    }
    else if(attribute == "light_color_b"){
        BasePointers.back().setPointer(&lightColor[2]);
    }
    else if(attribute == "used_bitmap_layer"){
        BasePointers.back().setPointer(&usedBitmapLayer);
    }
    else{
        BasePointers.pop_back();
        getPrimaryContext(attribute, BasePointers);
    }
}