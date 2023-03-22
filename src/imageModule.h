#ifndef IMAGEMODULE_H_INCLUDED
#define IMAGEMODULE_H_INCLUDED


#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "camera.h"



void draw_bitmap(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float red, float green, float blue, float alpha,
float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags);

void draw_bitmap(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float red, float green, float blue, float alpha,
float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, bool mirrorX, bool mirrorY);

class SingleBitmap{
public:
    string ID;
    ALLEGRO_BITMAP * bitmap;
    SingleBitmap(unsigned int bitmapID);
    void loadBitmap(string newID, string filePath);
};

class ImageModule: public PrimaryModule {
private:
    vec2d rotPos;
    vec2d start; //Start drawing from these pixels
    vec2d frameSize; //Dimensions of a one animation frame
    vec2i currentFrame;
    ALLEGRO_BITMAP * image;
    ALLEGRO_BITMAP * lightBitmap;
    bool isBitmapFromContainer[2];
    double rotateAngle;
    bool mirrorX, mirrorY;
    float imageColor[4];
    float lightLevel;
    float lightColor[3];
public:
    int usedBitmapLayer; //Images with numbers higher or equal to zero are drawn in ascending order, but if the value is -1, image will be drawn on top of everything else.

    void setUpImageInstance();
    ImageModule(int imageModuleID);
    ImageModule(string imageModuleID);

    void loadImage(string newID, string filePath);
    void connectBitmap(vector <SingleBitmap> & BitmapContainer, string bitmapName);
    void loadLight(string filePath);
    void connectLightBitmap(vector <SingleBitmap> & BitmapContainer, string bitmapName);
    void checkImage(ALLEGRO_DISPLAY * window);
    void drawImage(vec2d base, Camera2D Camera, bool outSourcing);
    void drawFrame(vec2d base);
    void setRotPos(vec2d newRotPos);
    void setCenterX(double x); //center == rotPos
    void setCenterY(double y); //center == rotPos

    void setStartX(double x);
    void setStartY(double y);
    void setFrameSizeX(double x);
    void setFrameSizeY(double y);
    void setCurrentFrameX(int x);
    void setCurrentFrameY(int y);

    void setRotation(double newRotateAngle);
    void addRotation(double newRotateAngle);
    void setMirror(bool newMirrorX, bool newMirrorY);
    void setMirrorX(bool newMirrorX);
    void setMirrorY(bool newMirrorY);
    void setImageColor(float newImageColor[4]);
    void setImageColor(float newImageColor, char whichColor);
    void setLightColor(vec3d newLightColor, float newLightLevel);
    void setLightColor(float newLightColor, char whichLight);
    void setUsedBitmapLayer(int newLayer);
    float getLightLevel();
    void changeParameters(string newName, vec6d dimPos, double newRotateAngle, vec2d newScale, bool newMirrorX, bool newMirrorY, vec4d newImageColors);
    void modifyFrames(vec2d newFrameSize);
    void chooseFrames(vec2i newCurrentFrame);
    void destroyBitmap();
};


#endif // IMAGEMODULE_H_INCLUDED
