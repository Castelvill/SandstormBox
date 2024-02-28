#ifndef IMAGEMODULE_H_INCLUDED
#define IMAGEMODULE_H_INCLUDED

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include "mouse.h"



void draw_bitmap(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float red, float green, float blue, float alpha,
float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags);

void draw_bitmap(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float red, float green, float blue, float alpha,
float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, bool mirrorX, bool mirrorY);

class SingleBitmap{
public:
    string filePath, ID;
    ALLEGRO_BITMAP * bitmap;
    SingleBitmap();
    void loadBitmap(string newID, string newFilePath, string workingDirectory);
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
    string imageFilePath, imageID;
    string lightFilePath, lightID;
    double rotateAngle;
    bool mirrorX, mirrorY;
    float imageColor[4];
    float lightLevel;
    float lightColor[3];
public:
    float getLightLevel() const;

    void setUpNewInstance();
    ImageModule();
    ImageModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ImageModule(unsigned int newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~ImageModule();
    void clone(const ImageModule& Image, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);

    void loadImage(string newFilePath, string newImageID, string workingDirectory);
    void connectBitmap(vector <SingleBitmap> & BitmapContainer, string newFilePath, string newImageID, string workingDirectory);
    void loadLight(string newFilePath, string newLightID, string workingDirectory);
    void connectLightBitmap(vector <SingleBitmap> & BitmapContainer, string newFilePath, string newLightID, string workingDirectory);
    void checkImage(ALLEGRO_DISPLAY * window, string workingDirectory);
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
    void setImageColor(vec4d);
    void setImageColor(float newImageColor, char whichColor);
    void setLightColor(vec3d newLightColor, float newLightLevel);
    void setLightColor(float newLightColor, char whichLight);
    void modifyFrames(vec2d newFrameSize);
    void chooseFrames(vec2i newCurrentFrame);
    void clear();
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
};


#endif // IMAGEMODULE_H_INCLUDED
