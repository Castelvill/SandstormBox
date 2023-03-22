#include "particles.h"

SingleParticle::SingleParticle(){
    pos.set(0.0, 0.0);
    direction.set(0.0, 0.0);
    environment.set(0.0, 0.0);
    environmentSpeed.set(0.0, 0.0);
    speed = 0.0;
    acceleration = 0.0;
    minSpeed = 0.0;
    maxSpeed = 0.0;
    shapeRotation = 0.0;
    shapeRotationSpeed = 0.0;
    rotationSpeed = 0.0;
    timeToNegateRotation = 0.0;
    maxTimeToNegateRotation = 0.0;

    radius = 0.0;
    mass = 0.0;
    timeToDeath = 0.0;
    colorIntensity = 0.0; //for circle 2.5, for triangle it's 1.5
    colorFading = 0.0;
    usedImageName = "";
    particleColor[0] = 0.0;
    particleColor[1] = 0.0;
    particleColor[2] = 0.0;
    shape = 0;

    isDrawnWithDetails = false;
    isAttachedToCamera = false;
}
void SingleParticle::moveParticle(){
    speed += acceleration;
    if(speed > maxSpeed)
        speed = maxSpeed;
    if(speed < minSpeed)
        speed = minSpeed;

    environment.translate(environmentSpeed);

    timeToNegateRotation -= 1/FPS;
    if(timeToNegateRotation <= 0.0 && maxTimeToNegateRotation > 0.0){
        rotationSpeed *= -1;
        shapeRotationSpeed *= -1;
        timeToNegateRotation = maxTimeToNegateRotation;
    }

    direction.rotate(rotationSpeed);

    vec2d momentum(direction.x * speed * mass, direction.y * speed * mass);
    momentum.translate(environment);
    pos.translate(momentum);

    shapeRotation += shapeRotationSpeed;
}
void SingleParticle::drawOneParticle(Camera2D Camera){
    if(colorIntensity > 1.0)
        colorIntensity = 1.0;
    if(colorIntensity < 0.0)
        colorIntensity = 0.0;
    double red = particleColor[0] * colorIntensity;
    double green = particleColor[1] * colorIntensity;
    double blue = particleColor[2] * colorIntensity;
    double alpha = 255.0 * colorIntensity;

    vec2d finPos(pos);
    double finRadius = radius;

    if(!isAttachedToCamera){
        finPos.set(Camera.translateWithZoom(finPos));
        finRadius *= Camera.zoom;
    }


    if(shape == 0){
        if(isDrawnWithDetails){
            for(int n = finRadius; n > 0; n-=3)
                al_draw_filled_circle(finPos.x, finPos.y, n, al_map_rgba(red, green, blue, alpha));
        }
        else
            al_draw_filled_circle(finPos.x, finPos.y, finRadius, al_map_rgba(red, green, blue, alpha));
    }
    if(shape == 1){
        if(isDrawnWithDetails){
            for(int n = finRadius; n > 0; n-=3)
                al_draw_circle(finPos.x, finPos.y, finRadius*0.5, al_map_rgba(red, green, blue, alpha), n*0.5);
        }
        else
            al_draw_circle(finPos.x, finPos.y, finRadius, al_map_rgba(red, green, blue, alpha), finRadius/3);
    }
    if(shape == 2){
        //vec2d rectVec1, rectVec2;
        //rectVec2.set(-finRadius/sqrt(2.0), -finRadius/sqrt(2.0));
        //rectVec2.rotate(shapeRotation);
        //rectVec1.set(rectVec2);
        //rectVec2.rotate(90.0);
        //for(int n = finRadius; n > 0; n-=3)
        //    al_draw_filled_rectangle(finPos.x+rectVec1.x, finPos.y+rectVec1.y, finPos.x+rectVec2.x, finPos.y+rectVec2.y, al_map_rgba(red, green, blue, alpha));
        vec2d triVec1, triVec2, triVec3, triVec4;
        triVec4.set(-1.0, -1.0);
        triVec4.rotate(shapeRotation);
        triVec1.set(triVec4);
        triVec4.rotate(90.0);
        triVec2.set(triVec4);
        triVec4.rotate(90.0);
        triVec3.set(triVec4);
        triVec4.rotate(90.0);
        if(isDrawnWithDetails){
            for(int n = finRadius; n > 0; n-=3){
                al_draw_filled_triangle(finPos.x+triVec1.x*n, finPos.y+triVec1.y*n, finPos.x+triVec2.x*n, finPos.y+triVec2.y*n, finPos.x+triVec3.x*n, finPos.y+triVec3.y*n, al_map_rgba(red, green, blue, alpha));
                al_draw_filled_triangle(finPos.x+triVec3.x*n, finPos.y+triVec3.y*n, finPos.x+triVec4.x*n, finPos.y+triVec4.y*n, finPos.x+triVec1.x*n, finPos.y+triVec1.y*n, al_map_rgba(red, green, blue, alpha));
            }
        }
        else{
            al_draw_filled_triangle(finPos.x+triVec1.x*finRadius, finPos.y+triVec1.y*finRadius, finPos.x+triVec2.x*finRadius, finPos.y+triVec2.y*finRadius, finPos.x+triVec3.x*finRadius, finPos.y+triVec3.y*finRadius, al_map_rgba(red, green, blue, alpha));
            al_draw_filled_triangle(finPos.x+triVec3.x*finRadius, finPos.y+triVec3.y*finRadius, finPos.x+triVec4.x*finRadius, finPos.y+triVec4.y*finRadius, finPos.x+triVec1.x*finRadius, finPos.y+triVec1.y*finRadius, al_map_rgba(red, green, blue, alpha));
        }

    }
    if(shape == 3){
        vec2d triVec1, triVec2, triVec3;
        triVec3.set(0.0, 1.0);
        triVec3.rotate(shapeRotation);
        triVec1.set(triVec3);
        triVec3.rotate(120.0);
        triVec2.set(triVec3);
        triVec3.rotate(120.0);
        if(isDrawnWithDetails){
            for(int n = finRadius; n > 0; n-=3)
                al_draw_filled_triangle(finPos.x-triVec1.x*n, finPos.y-triVec1.y*n, finPos.x-triVec2.x*n, finPos.y-triVec2.y*n, finPos.x-triVec3.x*n, finPos.y-triVec3.y*n, al_map_rgba(red, green, blue, alpha));
        }
        else{
            al_draw_filled_triangle(finPos.x-triVec1.x*finRadius, finPos.y-triVec1.y*finRadius, finPos.x-triVec2.x*finRadius, finPos.y-triVec2.y*finRadius, finPos.x-triVec3.x*finRadius, finPos.y-triVec3.y*finRadius, al_map_rgba(red, green, blue, alpha));
        }
    }
}
void SingleParticle::drawParticleUsingImage(ImageModule image, Camera2D Camera){
    image.drawImage(pos, Camera, true);
}
bool SingleParticle::isOnScreen(vec2i screen){
    if((pos.x+radius >= 0 && pos.x+radius <= screen.x) || (pos.x-radius >= 0 && pos.x-radius <= screen.x)){
        if((pos.y+radius >= 0 && pos.y+radius <= screen.y) || (pos.y-radius >= 0 && pos.y-radius <= screen.y)){
            return true;
        }
    }
    return false;
}
vec2d SingleParticle::getPos(){
    return pos;
}
double SingleParticle::getRadius(){
    return radius;
}


ParticleEffectModule::ParticleEffectModule(int moduleID){
    primaryConstructor(moduleID);
    environment.set(0.0, 0.0);
    environmentSpeed.set(0.0, 0.0);
    minSpeed = 0.0;
    maxSpeed = 0.0;
    minBasicSpeed = 0.0;
    maxBasicSpeed = 0.0;
    minAcceleration = 0.0;
    maxAcceleration = 0.0;
    minParticleMass = 0.0;
    maxParticleMass = 0.0;
    minDirectionDegree = 0.0;
    maxDirectionDegree = 0.0;
    minRotationSpeed = 0.0;
    maxRotationSpeed = 0.0;
    minTimeToNegateRotation = 0.0;
    maxTimeToNegateRotation = 0.0;
    areParticlesMoving = true;
    isEnvironmentSynchronized = false;
    minParticleRadius = 0.0;
    maxParticleRadius = 0.0;
    minTimeToDeath = 0.0;
    maxTimeToDeath = 0.0;
    minShapeRotationSpeed = 0.0;
    maxShapeRotationSpeed = 0.0;
    minColorIntensity = 0.0;
    maxColorIntensity = 0.0;
    particlesShape = 0;
    useImageAsParticles = false;
    useRandomColors = false;
    isModuleStatic = false;
    isDrawingWithDetails = false;
    blockParticlesSpawn = true;
    canParticlesSpawn = false;
    minParticlesPerSpawn = 0;
    maxParticlesPerSpawn = 0;
    maxParticlesCount = 0;
    timeToSpawn = 0.0;
    maxTimeToSpawn = 0.0;
    spawnKeyBind = ALLEGRO_KEY_P;
    spawnOnKeyRelease = false;
    usedBitmapLayer = 0;
}
void ParticleEffectModule::clearModule(){
    particleEffect.clear();
    colorIntervals.clear();
    basicColorsList.clear();
    usedImagesList.clear();
}
void ParticleEffectModule::spawnParticles(vec2d objPos, vector <short> pressedKeys){
    for(unsigned int i = 0; i < pressedKeys.size(); i++){
        if(pressedKeys[i] == spawnKeyBind){
            canParticlesSpawn = true;
            break;
        }
    }
    if(!canParticlesSpawn || particleEffect.size() == maxParticlesCount || maxParticlesPerSpawn <= 0 || minParticlesPerSpawn < 0)
        return;
    timeToSpawn -= 1/FPS;
    if(timeToSpawn > 0.0)
        return;
    timeToSpawn = maxTimeToSpawn;
    if(blockParticlesSpawn)
        canParticlesSpawn = false;

    int particlesPerSpawn = minParticlesPerSpawn + rand()%(maxParticlesPerSpawn-minParticlesPerSpawn+1);
    if(particleEffect.size() + particlesPerSpawn > maxParticlesCount)
        particlesPerSpawn = maxParticlesCount - particleEffect.size();
    if(particlesPerSpawn <= 0)
        return;

    vec2d temp;
    double randomRotation, randomNumber;
    unsigned int initialCount = particleEffect.size();

    particleEffect.reserve(initialCount + particlesPerSpawn);
    for(unsigned int i=initialCount; i < initialCount + particlesPerSpawn; i++){
        particleEffect.push_back(SingleParticle());
        if(isModuleStatic)
            particleEffect.back().pos.set(randomDouble(pos.x, pos.x+size.x), randomDouble(pos.y, pos.y+size.y));
        else
            particleEffect.back().pos.set(randomDouble(objPos.x+pos.x, objPos.x+pos.x+size.x), randomDouble(objPos.y+pos.y, objPos.y+pos.y+size.y));
        randomRotation = randomDouble(minDirectionDegree, maxDirectionDegree);
        particleEffect.back().direction.set(0.0, -1.0);
        particleEffect.back().direction.rotate(randomRotation);
        particleEffect.back().environment.set(environment);
        particleEffect.back().environmentSpeed.set(environmentSpeed);
        particleEffect.back().speed = randomDouble(minBasicSpeed, maxBasicSpeed);
        particleEffect.back().acceleration = randomDouble(minAcceleration, maxAcceleration);
        particleEffect.back().minSpeed = minSpeed;
        particleEffect.back().maxSpeed = maxSpeed;
        particleEffect.back().shapeRotation = 0.0;
        particleEffect.back().shapeRotationSpeed = randomDouble(minShapeRotationSpeed, maxShapeRotationSpeed);
        particleEffect.back().rotationSpeed = randomDouble(minRotationSpeed, maxRotationSpeed);
        particleEffect.back().maxTimeToNegateRotation = randomDouble(minTimeToNegateRotation, maxTimeToNegateRotation);
        particleEffect.back().radius = randomDouble(minParticleRadius, maxParticleRadius);
        particleEffect.back().mass = randomDouble(minParticleMass, maxParticleMass);
        particleEffect.back().timeToDeath = randomDouble(minTimeToDeath, maxTimeToDeath);
        particleEffect.back().colorIntensity = randomDouble(minColorIntensity, maxColorIntensity);
        particleEffect.back().colorFading = particleEffect.back().colorIntensity*((1/FPS)/particleEffect.back().timeToDeath);
        particleEffect.back().isDrawnWithDetails = isDrawingWithDetails;
        particleEffect.back().isAttachedToCamera = isAttachedToCamera;
        if(usedImagesList.size() > 0){
            randomNumber = rand() % usedImagesList.size();
            particleEffect.back().usedImageName = usedImagesList[randomNumber];
        }
        if(!useRandomColors){
            if(basicColorsList.size() > 0){
                randomNumber = rand() % basicColorsList.size();
                particleEffect.back().particleColor[0] = basicColorsList[randomNumber].color[0];
                particleEffect.back().particleColor[1] = basicColorsList[randomNumber].color[1];
                particleEffect.back().particleColor[2] = basicColorsList[randomNumber].color[2];
            }
            else{
                particleEffect.back().particleColor[0] = 255;
                particleEffect.back().particleColor[1] = 255;
                particleEffect.back().particleColor[2] = 255;
            }
        }
        else{
            particleEffect.back().particleColor[0] = rand() % 255;
            particleEffect.back().particleColor[1] = rand() % 255;
            particleEffect.back().particleColor[2] = rand() % 255;
        }

        particleEffect.back().shape = particlesShape;
    }
}
void ParticleEffectModule::killParticles(){
    for(unsigned int i=0; i < particleEffect.size(); i++){
        particleEffect[i].timeToDeath -= 1/FPS;
        
        if(particleEffect[i].timeToDeath > 0.0)
            continue;

        particleEffect.erase(particleEffect.begin()+i);
        
        if(particleEffect.size() == 0){
            std::vector<SingleParticle>().swap(particleEffect);
            particleEffect.shrink_to_fit();
            break;
        }
        i--;
    }
}
void ParticleEffectModule::drawParticles(vector <ImageModule> & ImageContainer, vec2i screen, Camera2D Camera){
    float arr[4];
    vec2d newPos;
    for(unsigned int i=0; i < particleEffect.size(); i++){
        if(particleEffect[i].timeToDeath <= 0)
            continue;
        if(!useImageAsParticles){
            //if(Camera.isOnScreenWithRadius(particleEffect[i].getPos(), particleEffect[i].getRadius()))
                particleEffect[i].drawOneParticle(Camera);
        }
        else{
            for(unsigned int j=0; j < ImageContainer.size(); j++){
                newPos.set(particleEffect[i].getPos());
                newPos.translate(ImageContainer[j].getPos(false));
                if(particleEffect[i].usedImageName == ImageContainer[j].getID()){
                    ImageContainer[j].activate();
                    ImageContainer[j].setRotation(particleEffect[i].shapeRotation);
                    ImageContainer[j].resize(vec2d(particleEffect[i].radius*2, particleEffect[i].radius*2));
                    arr[0] = (particleEffect[i].particleColor[0] * particleEffect[i].colorIntensity)/255;
                    arr[1] = (particleEffect[i].particleColor[1] * particleEffect[i].colorIntensity)/255;
                    arr[2] = (particleEffect[i].particleColor[2] * particleEffect[i].colorIntensity)/255;
                    arr[3] = particleEffect[i].colorIntensity;
                    for(int c = 0; c < 3; c++){
                        if(arr[c] < 0.0)
                            arr[c] = 0.0;
                        if(arr[c] > 1.0)
                            arr[c] = 1.0;
                    }
                    ImageContainer[j].setImageColor(arr);
                    particleEffect[i].drawParticleUsingImage(ImageContainer[j], Camera);
                    ImageContainer[j].deactivate();
                }
            }
        }
    }
}
void ParticleEffectModule::updateParticles(){
    for(unsigned int i=0; i < particleEffect.size(); i++){
        particleEffect[i].colorIntensity -= particleEffect[i].colorFading;
        if(isEnvironmentSynchronized){
            particleEffect[i].environment = environment;
            particleEffect[i].environmentSpeed = environmentSpeed;
        }
    }
    killParticles();
    moveParticles();
    for(unsigned int j=0; j < colorIntervals.size(); j++){
        for(unsigned int i=0; i < particleEffect.size(); i++){
            if(particleEffect[i].speed >= colorIntervals[j].minSpeed){
                particleEffect[i].particleColor[0] = colorIntervals[j].color[0];
                particleEffect[i].particleColor[1] = colorIntervals[j].color[1];
                particleEffect[i].particleColor[2] = colorIntervals[j].color[2];
            }
        }
    }
}
void ParticleEffectModule::moveParticles(){
    for(unsigned int i=0; i < particleEffect.size(); i++){
        if(areParticlesMoving)
            particleEffect[i].moveParticle();
    }
}
void ParticleEffectModule::changeSpawningParameters(bool newCanParticlesSpawn, bool newBlockParticlesSpawn, bool newSpawnOnKeyRelease, int newMinParticlesPerSpawn,
        int newMaxParticlesPerSpawn, int newMaxParticlesCount, double newMaxTimeToSpawn){
    canParticlesSpawn = newCanParticlesSpawn;
    blockParticlesSpawn = newBlockParticlesSpawn;
    spawnOnKeyRelease = newSpawnOnKeyRelease;
    minParticlesPerSpawn = newMinParticlesPerSpawn;
    maxParticlesPerSpawn = newMaxParticlesPerSpawn;
    maxParticlesCount = newMaxParticlesCount;
    maxTimeToSpawn = newMaxTimeToSpawn;
    timeToSpawn = maxTimeToSpawn;
}
void ParticleEffectModule::changeMotionParameters(double newMinBasicSpeed, double newMaxBasicSpeed, double newMinSpeed, double newMaxSpeed, vec2d newEnvironment, vec2d newEnvironmentSpeed, double newMinAcceleration, double newMaxAcceleration,
        double newMinParticleMass, double newMaxParticleMass, double newMinDirectionDegree,
        double newMaxDirectionDegree, double newMinRotationSpeed, double newMaxRotationSpeed, double newMinTimeToNegateRotation, double newMaxTimeToNegateRotation){
    minBasicSpeed = newMinBasicSpeed;
    maxBasicSpeed = newMaxBasicSpeed;
    minSpeed = newMinSpeed;
    maxSpeed = newMaxSpeed;
    environment.set(newEnvironment);
    environmentSpeed.set(newEnvironmentSpeed);
    minAcceleration = newMinAcceleration;
    maxAcceleration = newMaxAcceleration;
    minParticleMass = newMinParticleMass;
    maxParticleMass = newMaxParticleMass;
    minDirectionDegree = newMinDirectionDegree;
    maxDirectionDegree = newMaxDirectionDegree;
    minRotationSpeed = newMinRotationSpeed;
    maxRotationSpeed = newMaxRotationSpeed;
    minTimeToNegateRotation = newMinTimeToNegateRotation;
    maxTimeToNegateRotation = newMaxTimeToNegateRotation;
}
void ParticleEffectModule::setEnvironment(vec2d newEnvironment){
    environment.set(newEnvironment);
}
void ParticleEffectModule::setEnvironmentSpeed(vec2d newEnvironmentSpeed){
    environmentSpeed.set(newEnvironmentSpeed);
}
void ParticleEffectModule::setUsedBitmapLayer(int newLayer){
    usedBitmapLayer = newLayer;
}
void ParticleEffectModule::switchUseImageAsParticles(){
    if(useImageAsParticles)
        useImageAsParticles = false;
    else
        useImageAsParticles = true;
}
void ParticleEffectModule::switchUseRandomColors(){
    if(useRandomColors)
        useRandomColors = false;
    else
        useRandomColors = true;
}
void ParticleEffectModule::switchAreParticlesMoving(){
    if(areParticlesMoving)
        areParticlesMoving = false;
    else
        areParticlesMoving = true;
}
void ParticleEffectModule::changeGeneralParameters(vec2d newPos, vec2d newSize, double newMinParticleRadius, double newMaxParticleRadius, double newMinTimeToDeath, double newMaxTimeToDeath, double newMinColorIntensity,
        double newMaxColorIntensity, double newMinShapeRotationSpeed, double newMaxShapeRotationSpeed, short newParticlesShape, bool newUseImageAsParticles, bool newIsDrawingWithDetails, bool newIsStatic){
    setPos(newPos);
    setSize(newSize);

    minParticleRadius = newMinParticleRadius;
    maxParticleRadius = newMaxParticleRadius;
    minTimeToDeath = newMinTimeToDeath;
    maxTimeToDeath = newMaxTimeToDeath;
    minColorIntensity = newMinColorIntensity;
    maxColorIntensity = newMaxColorIntensity;
    minShapeRotationSpeed = newMinShapeRotationSpeed;
    maxShapeRotationSpeed = newMaxShapeRotationSpeed;
    particlesShape = newParticlesShape;
    useImageAsParticles = newUseImageAsParticles;
    isDrawingWithDetails = newIsDrawingWithDetails;
    isModuleStatic = newIsStatic;
}
void ParticleEffectModule::addColor(unsigned short red, unsigned short green, unsigned short blue){
    basicColorsList.push_back(colorList());
    basicColorsList.back().color[0] = red;
    basicColorsList.back().color[1] = green;
    basicColorsList.back().color[2] = blue;
}
void ParticleEffectModule::addColorInterval(unsigned short red, unsigned short green, unsigned short blue, double newMinSpeed){
    colorIntervals.push_back(colorAndSpeed());
    colorIntervals.back().color[0] = red;
    colorIntervals.back().color[1] = green;
    colorIntervals.back().color[2] = blue;
    colorIntervals.back().minSpeed = newMinSpeed;
}
void ParticleEffectModule::addImage(string newImageID){
    usedImagesList.push_back(newImageID);
}
void ParticleEffectModule::removeImage(string imageID){
    usedImagesList.erase(std::remove(usedImagesList.begin(),
                                     usedImagesList.end(), imageID),
                                     usedImagesList.end());
}
void ParticleEffectModule::removeImage(unsigned int id){
    if(id >= usedImagesList.size())
        return;
    usedImagesList.erase(usedImagesList.begin() + id);
}
void ParticleEffectModule::activeSpawn(){
    canParticlesSpawn = true;
}


void ParticleEffectModule::addColorInHex(string hexColor){
    if(hexColor.size() != 6)
        return;
    basicColorsList.push_back(colorList());
    std::sscanf(hexColor.c_str(), "%2hx%2hx%2hx", &basicColorsList.back().color[0],
                &basicColorsList.back().color[1], &basicColorsList.back().color[2]);
}
void ParticleEffectModule::removeColor(unsigned int id){
    if(id >= basicColorsList.size())
        return;
    basicColorsList.erase(basicColorsList.begin() + id);
}
void ParticleEffectModule::addColorIntervalInHex(string hexColorAndSpeed){
    if(hexColorAndSpeed.size() < 9)
        return;

    colorIntervals.push_back(colorAndSpeed());

    string hexColor = hexColorAndSpeed.substr(0, 6);
    string strSpeed = hexColorAndSpeed.substr(6);

    std::sscanf(hexColor.c_str(), "%2hx%2hx%2hx", &colorIntervals.back().color[0],
                &colorIntervals.back().color[1], &colorIntervals.back().color[2]);
    colorIntervals.back().minSpeed = std::stod(strSpeed);
}
void ParticleEffectModule::removeColorInterval(unsigned int id){
    if(id >= colorIntervals.size())
        return;
    colorIntervals.erase(colorIntervals.begin() + id);
}


double ParticleEffectModule::getEnvironmentX(){
    return environment.x;
}
double ParticleEffectModule::getEnvironmentY(){
    return environment.y;
}
double ParticleEffectModule::getEnvironmentSpeedX(){
    return environmentSpeed.x;
}
double ParticleEffectModule::getEnvironmentSpeedY(){
    return environmentSpeed.y;
}
double ParticleEffectModule::getMinSpeed(){
    return minSpeed;
}
double ParticleEffectModule::getMaxSpeed(){
    return maxSpeed;
}
double ParticleEffectModule::getMinBasicSpeed(){
    return minBasicSpeed;
}
double ParticleEffectModule::getMaxBasicSpeed(){
    return maxBasicSpeed;
}
double ParticleEffectModule::getMinAcceleration(){
    return minAcceleration;
}
double ParticleEffectModule::getMaxAcceleration(){
    return maxAcceleration;
}
double ParticleEffectModule::getMinParticleMass(){
    return minParticleMass;
}
double ParticleEffectModule::getMaxParticleMass(){
    return maxParticleMass;
}
double ParticleEffectModule::getMinDirectionDegree(){
    return minDirectionDegree;
}
double ParticleEffectModule::getMaxDirectionDegree(){
    return maxDirectionDegree;
}
double ParticleEffectModule::getMinRotationSpeed(){
    return minRotationSpeed;
}
double ParticleEffectModule::getMaxRotationSpeed(){
    return maxRotationSpeed;
}
double ParticleEffectModule::getMinTimeToNegateRotation(){
    return minTimeToNegateRotation;
}
double ParticleEffectModule::getMaxTimeToNegateRotation(){
    return maxTimeToNegateRotation;
}
bool ParticleEffectModule::getAreParticlesMoving(){
    return areParticlesMoving;
}
bool ParticleEffectModule::getIsEnvironmentSynchronized(){
    return isEnvironmentSynchronized;
}
double ParticleEffectModule::getMinParticleRadius(){
    return minParticleRadius;
}
double ParticleEffectModule::getMaxParticleRadius(){
    return maxParticleRadius;
}
double ParticleEffectModule::getMinTimeToDeath(){
    return minTimeToDeath;
}
double ParticleEffectModule::getMaxTimeToDeath(){
    return maxTimeToDeath;
}
double ParticleEffectModule::getMinShapeRotationSpeed(){
    return minShapeRotationSpeed;
}
double ParticleEffectModule::getMaxShapeRotationSpeed(){
    return maxShapeRotationSpeed;
}
double ParticleEffectModule::getMinColorIntensity(){
    return minColorIntensity;
}
double ParticleEffectModule::getMaxColorIntensity(){
    return maxColorIntensity;
}
short ParticleEffectModule::getParticlesShape(){
    return particlesShape;
}
bool ParticleEffectModule::getUseImageAsParticles(){
    return useImageAsParticles;
}
bool ParticleEffectModule::getUseRandomColors(){
    return useRandomColors;
}
bool ParticleEffectModule::getIsModuleStatic(){
    return isModuleStatic;
}
bool ParticleEffectModule::getIsDrawingWithDetails(){
    return isDrawingWithDetails;
}
bool ParticleEffectModule::getBlockParticlesSpawn(){
    return blockParticlesSpawn;
}
bool ParticleEffectModule::getCanParticlesSpawn(){
    return canParticlesSpawn;
}
unsigned int ParticleEffectModule::getMinParticlesPerSpawn(){
    return minParticlesPerSpawn;
}
unsigned int ParticleEffectModule::getMaxParticlesPerSpawn(){
    return maxParticlesPerSpawn;
}
unsigned int ParticleEffectModule::getMaxParticlesCount(){
    return maxParticlesCount;
}
double ParticleEffectModule::getMaxTimeToSpawn(){
    return maxTimeToSpawn;
}
short ParticleEffectModule::getSpawnKeyBind(){
    return spawnKeyBind;
}
bool ParticleEffectModule::getSpawnOnKeyRelease(){
    return spawnOnKeyRelease;
}

void ParticleEffectModule::setEnvironmentX(double newValue){
    environment.x = newValue;
}
void ParticleEffectModule::setEnvironmentY(double newValue){
    environment.y = newValue;
}
void ParticleEffectModule::setEnvironmentSpeedX(double newValue){
    environmentSpeed.x = newValue;
}
void ParticleEffectModule::setEnvironmentSpeedY(double newValue){
    environmentSpeed.y = newValue;
}
void ParticleEffectModule::setMinSpeed(double newValue){
    minSpeed = newValue;
}
void ParticleEffectModule::setMaxSpeed(double newValue){
    maxSpeed = newValue;
}
void ParticleEffectModule::setMinBasicSpeed(double newValue){
    minBasicSpeed = newValue;
}
void ParticleEffectModule::setMaxBasicSpeed(double newValue){
    maxBasicSpeed = newValue;
}
void ParticleEffectModule::setMinAcceleration(double newValue){
    minAcceleration = newValue;
}
void ParticleEffectModule::setMaxAcceleration(double newValue){
    maxAcceleration = newValue;
}
void ParticleEffectModule::setMinParticleMass(double newValue){
    minParticleMass = newValue;
}
void ParticleEffectModule::setMaxParticleMass(double newValue){
    maxParticleMass = newValue;
}
void ParticleEffectModule::setMinDirectionDegree(double newValue){
    minDirectionDegree = newValue;
}
void ParticleEffectModule::setMaxDirectionDegree(double newValue){
    maxDirectionDegree = newValue;
}
void ParticleEffectModule::setMinRotationSpeed(double newValue){
    minRotationSpeed = newValue;
}
void ParticleEffectModule::setMaxRotationSpeed(double newValue){
    maxRotationSpeed = newValue;
}
void ParticleEffectModule::setMinTimeToNegateRotation(double newValue){
    minTimeToNegateRotation = newValue;
}
void ParticleEffectModule::setMaxTimeToNegateRotation(double newValue){
    maxTimeToNegateRotation = newValue;
}
void ParticleEffectModule::setAreParticlesMoving(bool newValue){
    areParticlesMoving = newValue;
}
void ParticleEffectModule::setIsEnvironmentSynchronized(bool newValue){
    isEnvironmentSynchronized = newValue;
}
void ParticleEffectModule::setMinParticleRadius(double newValue){
    minParticleRadius = newValue;
}
void ParticleEffectModule::setMaxParticleRadius(double newValue){
    maxParticleRadius = newValue;
}
void ParticleEffectModule::setMinTimeToDeath(double newValue){
    minTimeToDeath = newValue;
}
void ParticleEffectModule::setMaxTimeToDeath(double newValue){
    maxTimeToDeath = newValue;
}
void ParticleEffectModule::setMinShapeRotationSpeed(double newValue){
    minShapeRotationSpeed = newValue;
}
void ParticleEffectModule::setMaxShapeRotationSpeed(double newValue){
    maxShapeRotationSpeed = newValue;
}
void ParticleEffectModule::setMinColorIntensity(double newValue){
    minColorIntensity = newValue;
}
void ParticleEffectModule::setMaxColorIntensity(double newValue){
    maxColorIntensity = newValue;
}
void ParticleEffectModule::setParticlesShape(short newValue){
    particlesShape = newValue;
}
void ParticleEffectModule::setUseImageAsParticles(bool newValue){
    useImageAsParticles = newValue;
}
void ParticleEffectModule::setUseRandomColors(bool newValue){
    useRandomColors = newValue;
}
void ParticleEffectModule::setIsModuleStatic(bool newValue){
    isModuleStatic = newValue;
}
void ParticleEffectModule::setIsDrawingWithDetails(bool newValue){
    isDrawingWithDetails = newValue;
}
void ParticleEffectModule::setBlockParticlesSpawn(bool newValue){
    blockParticlesSpawn = newValue;
}
void ParticleEffectModule::setMinParticlesPerSpawn(unsigned int newValue){
    minParticlesPerSpawn = newValue;
}
void ParticleEffectModule::setMaxParticlesPerSpawn(unsigned int newValue){
    maxParticlesPerSpawn = newValue;
}
void ParticleEffectModule::setMaxParticlesCount(unsigned int newValue){
    maxParticlesCount = newValue;
}
void ParticleEffectModule::setMaxTimeToSpawn(double newValue){
    maxTimeToSpawn = newValue;
}
void ParticleEffectModule::setSpawnKeyBindShort(short newValue){
    spawnKeyBind = newValue;
}
void ParticleEffectModule::setSpawnKeyBind(char newValue){
    spawnKeyBind = translateToAllegroKey(newValue);
}
void ParticleEffectModule::setSpawnOnKeyRelease(bool newValue){
    spawnOnKeyRelease = newValue;
}
