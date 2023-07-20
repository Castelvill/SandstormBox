#include "usefull.h"

string intToStr4(int integer){
    char buff[4];
    sprintf(buff, "%d", integer);
    return buff;
}
string intToStr8(int integer){
    char buff[8];
    sprintf(buff, "%d", integer);
    return buff;
}
string doubleToStr4(double doubleVar){
    char buff[4];
    sprintf(buff, "%f", doubleVar);
    return buff;
}
string doubleToStr8(double doubleVar){
    char buff[8];
    sprintf(buff, "%f", doubleVar);
    return buff;
}
double countDistance(double x1, double y1, double x2, double y2){
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}
double countDistance2(double & x1, double & y1, double & x2, double & y2){
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}
int randomInt(int minValue, int maxValue){
    return minValue + (rand() % (maxValue - minValue + 1));
    //return minValue + ((double)rand() / (double)RAND_MAX) * (maxValue - minValue);
}
double randomDouble(double minValue, double maxValue){
    return minValue + ((rand() % 10001) / 10000.0) * (maxValue - minValue);
    //return minValue + ((double)rand() / (double)RAND_MAX) * (maxValue - minValue);
}
short translateToAllegroKey(char key){
    std::cout << short(key-'a'+1) << "\n";
    std::cout << short(key) << "\n\n";
    if(short(key) >= 97 && short(key) <= 122)
        return short(key-'a'+1);
    if(short(key) >= 48 && short(key) <= 57)
        return short(key)-21;
    if(short(key) == 32)
        return short(key)+43;
    if(short(key) == 24)
        return ALLEGRO_KEY_UP;
    if(short(key) == 26)
        return ALLEGRO_KEY_RIGHT;
    if(short(key) == 25)
        return ALLEGRO_KEY_DOWN;
    if(short(key) == 27)
        return ALLEGRO_KEY_LEFT;
    return ALLEGRO_KEY_UNKNOWN;
}

bool isStringInGroup(string ID, int count, ...){
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++){
        if(ID == va_arg(args, const char*)){
            return true;
        }
    }
    va_end(args);
    return false;
}
bool isCharInGroup(char ID, int count, ...){
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++){
        char temp = va_arg(args, const char*)[0];
        if(ID == temp){
            return true;
        }
    }
    va_end(args);
    return false;
}

void addUniqueToStringVector(vector <string> & stringVec, string newString){
    auto findString = std::find(begin(stringVec), end(stringVec), newString);
    if(findString == std::end(stringVec)){
        stringVec.push_back(newString);
    }
}
void removeFromStringVector(vector <string> & stringVec, std::string selectedString){
    auto foundString = std::find(begin(stringVec), end(stringVec), selectedString);
    if(foundString != std::end(stringVec)){
        stringVec.erase(foundString);
    }
}
bool inStringVector(vector <string> & stringVec, std::string findString){
    auto foundGroup = std::find(begin(stringVec), end(stringVec), findString);
    return foundGroup != std::end(stringVec);
}



