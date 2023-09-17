#ifndef PARTICLES_H_INCLUDED
#define PARTICLES_H_INCLUDED

#include "imageModule.h"

class SingleParticle{
public:
    vec2d pos, direction, environment, environmentSpeed;
    double speed, acceleration, minSpeed, maxSpeed;
    double shapeRotation, shapeRotationSpeed, rotationSpeed, timeToNegateRotation, maxTimeToNegateRotation; //If the max time to negate is 0, then don't negate rotation.
    double radius, mass, timeToDeath, colorIntensity, colorFading;
    bool isDrawnWithDetails;
    bool isAttachedToCamera;
    string usedImageName;
    unsigned short particleColor[3];
    short shape; //0-filled_circle, 1-ring, 2-square, 3-triangle
    SingleParticle();
    void moveParticle();
    void drawOneParticle(Camera2D Camera);
    bool isOnScreen(vec2i screen);
    vec2d getPos();
    double getRadius();
};

class colorAndSpeed{
public:
    unsigned short color[3];
    double minSpeed;
};

class colorList{
public:
    unsigned short color[3];
};

class ParticleEffectModule: public PrimaryModule{
private:
    //You can spawn particles from one point in all directions, or spawn them from any point inside a rectangle and they still can move in all directions from there.

    //motion
    vec2d environment; //Directional momentum of the wind. All particles move in this direction. Environment modifies speed vector.
    vec2d environmentSpeed; //Acceleration of the wind.
    double minSpeed, maxSpeed;
    double minBasicSpeed, maxBasicSpeed;
    double minAcceleration, maxAcceleration;
    double minParticleMass, maxParticleMass;
    double minDirectionDegree, maxDirectionDegree; //from 0 to 360, directions particles can move in
    double minRotationSpeed, maxRotationSpeed;
    double minTimeToNegateRotation, maxTimeToNegateRotation;
    bool areParticlesMoving;
    bool isEnvironmentSynchronized; //if true, update 'environment' of living particles
    //general
    double minParticleRadius, maxParticleRadius;
    double minTimeToDeath, maxTimeToDeath;
    double minShapeRotationSpeed, maxShapeRotationSpeed;
    double minColorIntensity, maxColorIntensity;
    vector <SingleParticle> particleEffect;
    vector <colorAndSpeed> colorIntervals; //It's used during particles spawning and to change color of existing particles.
    vector <colorList> basicColorsList;
    vector <string> usedImagesList;
    short particlesShape; //0-filled_circle, 1-ring, 2-square, 3-triangle
    bool useImageAsParticles;
    bool useRandomColors;
    bool isModuleStatic;
    bool isDrawingWithDetails; //Is drawing primitives with details
    //spawning
    bool blockParticlesSpawn; //Only an event can unlock spawn
    bool canParticlesSpawn;
    unsigned int minParticlesPerSpawn, maxParticlesPerSpawn;
    unsigned int maxParticlesCount;
    double timeToSpawn;
    double maxTimeToSpawn;
    short spawnKeyBind;
    bool spawnOnKeyRelease;
public:
    int usedBitmapLayer;

    ParticleEffectModule();
    ParticleEffectModule(string newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ParticleEffectModule(unsigned newID, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    void clone(const ParticleEffectModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void clear();
    void spawnParticles(vec2d objPos, vector <short> pressedKeys); //Particles spawn in a point or a rectangle.
    void killParticles();
    void drawParticles(vector <ImageModule> ImageContainer, vec2i screen, Camera2D Camera);
    void updateParticles(); //Changing the color accordingly to speed.
    void moveParticles();
    void changeSpawningParameters(bool newCanParticlesSpawn, bool newBlockParticlesSpawn, bool newSpawnOnKeyRelease, int newMinParticlesPerSpawn,
                                  int newMaxParticlesPerSpawn, int newMaxParticlesCount, double newMaxTimeToSpawn);
    void changeMotionParameters(double newMinBasicSpeed, double newMaxBasicSpeed, double newMinSpeed, double newMaxSpeed, vec2d newEnvironment, vec2d newEnvironmentSpeed, double newMinAcceleration, double newMaxAcceleration,
                                double newMinParticleMass, double newMaxParticleMass, double newMinDirectionDegree, double newMaxDirectionDegree,
                                double newMinRotationSpeed, double newMaxRotationSpeed, double newMinTimeToNegateRotation, double newMaxTimeToNegateRotation);
    void changeGeneralParameters(vec2d newPos, vec2d newSize, double minParticleRadius, double newMaxParticleRadius, double newMinTimeToDeath, double newMaxTimeToDeath, double newMinColorIntensity,
                                 double newMaxColorIntensity, double newMinShapeRotationSpeed, double newMaxShapeRotationSpeed, short newParticlesShape, bool newUseImageAsParticles, bool newIsDrawingWithDetails, bool newIsStatic);
    void switchUseImageAsParticles();
    void switchUseRandomColors();
    void switchAreParticlesMoving();
    void addColor(unsigned short red, unsigned short green, unsigned short blue);
    void addColorInHex(string hexColor);
    void removeColor(unsigned int id);
    void addColorInterval(unsigned short red, unsigned short green, unsigned short blue, double newMinSpeed);
    void addColorIntervalInHex(string hexColorAndSpeed);
    void removeColorInterval(unsigned int id);
    void addImage(string newImageID);
    void removeImage(string imageID);
    void removeImage(unsigned int id);
    void activeSpawn();
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);

    double getEnvironmentX();
    double getEnvironmentY();
    double getEnvironmentSpeedX();
    double getEnvironmentSpeedY();
    double getMinSpeed();
    double getMaxSpeed();
    double getMinBasicSpeed();
    double getMaxBasicSpeed();
    double getMinAcceleration();
    double getMaxAcceleration();
    double getMinParticleMass();
    double getMaxParticleMass();
    double getMinDirectionDegree();
    double getMaxDirectionDegree();
    double getMinRotationSpeed();
    double getMaxRotationSpeed();
    double getMinTimeToNegateRotation();
    double getMaxTimeToNegateRotation();
    bool getAreParticlesMoving();
    bool getIsEnvironmentSynchronized();
    double getMinParticleRadius();
    double getMaxParticleRadius();
    double getMinTimeToDeath();
    double getMaxTimeToDeath();
    double getMinShapeRotationSpeed();
    double getMaxShapeRotationSpeed();
    double getMinColorIntensity();
    double getMaxColorIntensity();
    short getParticlesShape();
    bool getUseImageAsParticles();
    bool getUseRandomColors();
    bool getIsModuleStatic();
    bool getIsDrawingWithDetails();
    bool getBlockParticlesSpawn();
    bool getCanParticlesSpawn();
    unsigned int getMinParticlesPerSpawn();
    unsigned int getMaxParticlesPerSpawn();
    unsigned int getMaxParticlesCount();
    double getMaxTimeToSpawn();
    short getSpawnKeyBind();
    bool getSpawnOnKeyRelease();

    void setShape(short newShape);
    void setUsedBitmapLayer(int newLayer);
    void setEnvironment(vec2d newEnvironment);
    void setEnvironmentX(double);
    void setEnvironmentY(double);
    void setEnvironmentSpeed(vec2d newEnvironmentSpeed);
    void setEnvironmentSpeedX(double);
    void setEnvironmentSpeedY(double);
    void setMinSpeed(double);
    void setMaxSpeed(double);
    void setMinBasicSpeed(double);
    void setMaxBasicSpeed(double);
    void setMinAcceleration(double);
    void setMaxAcceleration(double);
    void setMinParticleMass(double);
    void setMaxParticleMass(double);
    void setMinDirectionDegree(double);
    void setMaxDirectionDegree(double);
    void setMinRotationSpeed(double);
    void setMaxRotationSpeed(double);
    void setMinTimeToNegateRotation(double);
    void setMaxTimeToNegateRotation(double);
    void setAreParticlesMoving(bool);
    void setIsEnvironmentSynchronized(bool);
    void setMinParticleRadius(double);
    void setMaxParticleRadius(double);
    void setMinTimeToDeath(double);
    void setMaxTimeToDeath(double);
    void setMinShapeRotationSpeed(double);
    void setMaxShapeRotationSpeed(double);
    void setMinColorIntensity(double);
    void setMaxColorIntensity(double);
    void setParticlesShape(short);
    void setUseImageAsParticles(bool);
    void setUseRandomColors(bool);
    void setIsModuleStatic(bool);
    void setIsDrawingWithDetails(bool);
    void setBlockParticlesSpawn(bool);
    void setMinParticlesPerSpawn(unsigned int);
    void setMaxParticlesPerSpawn(unsigned int);
    void setMaxParticlesCount(unsigned int);
    void setMaxTimeToSpawn(double);
    void setSpawnKeyBind(char);
    void setSpawnKeyBindShort(short);
    void setSpawnOnKeyRelease(bool);
};


#endif // PARTICLES_H_INCLUDED
