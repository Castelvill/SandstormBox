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
    ID = "";
    bitmap = nullptr;
}
void SingleBitmap::loadBitmap(string newID, string newFilePath, string workingDirectory, bool createLightBitmap){
    if(bitmap){
        al_destroy_bitmap(bitmap);
        cout << "Bitmap '" << ID << "' destroyed! Prepare for the memory leaks :)\n";
    }
    filePath = newFilePath;
    ID = newID;
    bitmap = al_load_bitmap((workingDirectory + filePath).c_str());
    if(!bitmap){
        cout << "Failed to load a bitmap: File \'" << filePath << "\' not found.\n";
        //al_show_native_message_box(window, "Error", "", "Can't load an image.", NULL, ALLEGRO_MESSAGEBOX_ERROR);

        bitmap = al_load_bitmap((workingDirectory + "images/error.png").c_str());
        if(!bitmap){
            bitmap = al_load_bitmap((workingDirectory + "error.png").c_str());
        }
        if(!bitmap){
            cout << "File 'error.png' not found in \'" << workingDirectory << "\'.\n";
        }
        return;
    }
    lightBitmap = nullptr;
    if(bitmap && createLightBitmap){
        lightBitmap = al_create_bitmap(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap));
        al_set_target_bitmap(lightBitmap);
        al_clear_to_color(al_map_rgb(255, 255, 255));
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
}
ImageModule::ImageModule(){
    primaryConstructor("", nullptr, "", "");
    setUpNewInstance();
}
ImageModule::ImageModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
ImageModule::ImageModule(unsigned int newID, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newID, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
ImageModule::~ImageModule(){

}
void ImageModule::clone(const ImageModule &Image, vector<string> &listOfIDs, string newLayerID, string newObjectID, const bool &changeOldID){
    PrimaryModule::clone(Image, listOfIDs, newLayerID, newObjectID, changeOldID);
    rotPos = Image.rotPos;
    start = Image.start;
    frameSize = Image.frameSize;
    currentFrame = Image.currentFrame;
    rotateAngle = Image.rotateAngle;
    mirrorX = Image.mirrorX;
    mirrorY = Image.mirrorY;
    std::copy(Image.imageColor, Image.imageColor+4, imageColor);
    lightLevel = Image.lightLevel;
    std::copy(Image.lightColor, Image.lightColor+4, lightColor);
    image = Image.image;
    lightBitmap = Image.lightBitmap;
}
void ImageModule::clear(){
    image = nullptr;
    lightBitmap = nullptr;
}
void ImageModule::connectBitmap(vector <SingleBitmap> & BitmapContainer, string newFilePath, string newImageID, string workingDirectory){
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
    else if(newImageID != ""){
        for(; i < BitmapContainer.size(); i++){
            if(BitmapContainer[i].ID == newImageID){
                bitmapExists = true;
                break;
            }
        }
    }
    
    if(!bitmapExists){
        if(newImageID != ""){
            cout << "Error: In " << __FUNCTION__ << ": Bitmap \'" << newImageID << "\' not found.\n";
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
    imageID = newImageID;

    image = BitmapContainer[i].bitmap;
    lightBitmap = BitmapContainer[i].lightBitmap;

    if(!image){
        if(newImageID != ""){
            cout << "Error: In " << __FUNCTION__ << ": Bitmap \'" << newImageID << "\' not found.\n";
        }
        else{
            cout << "Error: In " << __FUNCTION__ << ": File \'" << imageFilePath << "\' not loaded into memory.\n";
        }
        return;
    }

    size.set(al_get_bitmap_width(image), al_get_bitmap_height(image));
}
void ImageModule::checkImage(ALLEGRO_DISPLAY * window, string workingDirectory){
    if(!image) {
        al_show_native_message_box(window, "Error", "", "Can't load an image.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        image = al_load_bitmap((workingDirectory + "images/error.png").c_str());
        size.set(al_get_bitmap_width(image), al_get_bitmap_height(image));
        if(!image){
            al_show_native_message_box(window, "Error", "", "Can't load an image.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        }
    }
}
void ImageModule::drawImage(vec2d base, Camera2D Camera, bool outSourcing){
    if((!outSourcing && !getIsActive()) || image == NULL || image == nullptr){
        return;
    }
    vec2d newScale(scale);

    if(isScrollable){
        base.set(Camera.translateWithZoom(base, pos));
        newScale.multiply(Camera.zoom);
    }
    else{
        base.set(base + pos);
    }

    float red = imageColor[0];
    float green = imageColor[1];
    float blue = imageColor[2];
    float alpha = imageColor[3];

    double radians = (rotateAngle * M_PI) / 180.0;
    if(!isScaledFromCenter){
        draw_bitmap(image, start.x, start.y, size.x, size.y, red, green, blue, alpha, rotPos.x, rotPos.y,
            base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);

        if(lightBitmap != nullptr && lightLevel > 0.0){
            draw_bitmap(lightBitmap, start.x, start.y, size.x, size.y, lightColor[0], lightColor[1], lightColor[2], lightLevel, rotPos.x, rotPos.y,
                base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);
        }
    }
    else{
        if(isScrollable){
            base.translate(-((scale-1)*size*Camera.zoom)/2);
        }
        else{
            base.translate(size*(1-scale)/2);
        }

        draw_bitmap(image, start.x, start.y, size.x, size.y, red, green, blue, alpha, rotPos.x, rotPos.y,
            base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);

        if(lightBitmap != nullptr && lightLevel > 0.0){
            draw_bitmap(lightBitmap, start.x, start.y, size.x, size.y, lightColor[0], lightColor[1], lightColor[2], lightLevel, rotPos.x, rotPos.y,
                base.x, base.y, newScale.x, newScale.y, radians, mirrorX, mirrorY);
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
            if(!mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, 0);
            else if(mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL);
            else if(!mirrorX && mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_HORIZONTAL);
            else
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add.x, base.y+pos.y+add.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL);
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

        if(lightBitmap != nullptr && lightLevel > 0.0){
            if(!mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, 0);
            else if(mirrorX && !mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL);
            else if(!mirrorX && mirrorY)
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_HORIZONTAL);
            else
                al_draw_tinted_scaled_rotated_bitmap_region(lightBitmap, start2.x, start2.y, size.x, size.y, al_map_rgba_f(lightColor[0], lightColor[1], lightColor[2], lightLevel), add.x, add.y, base.x+pos.x+add2.x, base.y+pos.y+add2.y, scale.x, scale.y, radians, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL);
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
    if(whichColor == 'r' || whichColor == 'x')
        imageColor[0] = newImageColor;
    else if(whichColor == 'g' || whichColor == 'y')
        imageColor[1] = newImageColor;
    else if(whichColor == 'b' || whichColor == 'z')
        imageColor[2] = newImageColor;
    else if(whichColor == 'a' || whichColor == 'l')
        imageColor[3] = newImageColor;
    else{
        cout << "Error: In " << __FUNCTION__ << ": Color channel '" << whichColor << "' does not exist.\n";
    }
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
    else{
        cout << "Error: In " << __FUNCTION__ << ": Light channel '" << whichLight << "' does not exist.\n";
    }
}
float ImageModule::getLightLevel() const{
    return lightLevel;
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
    else{
        BasePointers.pop_back();
        getPrimaryContext(attribute, BasePointers);
    }
}