#include "usefull.h"

int cstoi(string text, string & error){
    error = "";
    if(text == "true"){
        return 1;
    }
    if(text == "false"){
        return 0;
    }
    try{
        return stoi(text);
    }
    catch(std::invalid_argument const& ex){
        error = "Error: In cstoi: In stoi: string \"" + text + "\" triggers 'std::invalid_argument'.";
        return 0;
    }
}
double cstod(string text, string & error){
    error = "";
    try{
        return stod(text);
    }
    catch(std::invalid_argument const& ex){
        error = "Error: In cstod: In stod: string \"" + text + "\" triggers 'std::invalid_argument'.";
        return 0;
    }
}
float cstof(string text, string & error){
    error = "";
    try{
        return stof(text);
    }
    catch(std::invalid_argument const& ex){
        error = "Error: In cstod: In stod: string \"" + text + "\" triggers 'std::invalid_argument'.";
        return 0;
    }
}
string shortToStr(short integer){
    char buff[7];
    sprintf(buff, "%d", integer);
    return buff;
}
string intToStr(int integer){
    char buff[11];
    sprintf(buff, "%d", integer);
    return buff;
}
string uIntToStr(unsigned int integer){
    char buff[11];
    sprintf(buff, "%u", integer);
    return buff;
}
string doubleToStr(double doubleVar){
    if(doubleVar >= 1e16){
        return "[too big]";
    }
    char buff[16];
    sprintf(buff, "%f", doubleVar);
    return buff;
}
double countDistance(double x1, double y1, double x2, double y2){
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}
int randomInt(int minValue, int maxValue){
    return minValue + (rand() % (maxValue - minValue + 1));
    //return minValue + ((double)rand() / (double)RAND_MAX) * (maxValue - minValue);
}
float randomFloat(float minValue, float maxValue){
    return minValue + ((rand() % 10001) / 10000.0) * (maxValue - minValue);
    //return minValue + ((double)rand() / (double)RAND_MAX) * (maxValue - minValue);
}
double randomDouble(double minValue, double maxValue){
    return minValue + ((rand() % 10001) / 10000.0) * (maxValue - minValue);
    //return minValue + ((double)rand() / (double)RAND_MAX) * (maxValue - minValue);
}
short translateToAllegroKey(char key){
    if(short(key) >= 97 && short(key) <= 122)
        return short(key-'a'+1);
    if(short(key) >= 48 && short(key) <= 57)
        return short(key)-21;
    if(short(key) == 32)
        return short(key)+43;
    if(short(key) == 24)
        return 84;
    if(short(key) == 26)
        return 83;
    if(short(key) == 25)
        return 85;
    if(short(key) == 27)
        return 82;
    return 128;
}

bool isStringInGroup(string text, int count, ...){
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++){
        if(text == va_arg(args, const char*)){
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
        char temp = va_arg(args, int);
        if(ID == temp){
            return true;
        }
    }
    va_end(args);
    return false;
}
bool stringToBool(string text){
    if(text == "true" || text == "1"){
        return true;
    }
    if(text == "false" || text == "0"){
        return false;
    }
    cerr << "Error: In " << __FUNCTION__ << ": Provided string of value '"
        << text << "' cannot be transform into a bool value.\n";
    return false;
}

void addUniqueToStringVector(vector <string> & stringVec, string newString){
    auto findString = std::find(begin(stringVec), end(stringVec), newString);
    if(findString == std::end(stringVec)){
        stringVec.push_back(newString);
    }
}
void removeFromStringVector(vector <string> & stringVec, string selectedString){
    auto foundString = std::find(begin(stringVec), end(stringVec), selectedString);
    if(foundString != std::end(stringVec)){
        stringVec.erase(foundString);
    }
}
bool isStringInVector(const vector <string> & stringVec, string findString){
    auto foundGroup = std::find(begin(stringVec), end(stringVec), findString);
    return foundGroup != std::end(stringVec);
}

string findNewUniqueID(vector <string> uniqueIDs, string newID){
    string error;
    if(newID == ""){
        newID = "0";
    }
    while(isStringInVector(uniqueIDs, newID)){
        if(isdigit(newID.back())){
            size_t last_index = newID.find_last_not_of("0123456789");
            newID = newID.substr(0, last_index + 1) + intToStr(cstoi(newID.substr(last_index + 1), error) + 1);
            if(error.size() > 0){
                cerr << "Error: In " << __FUNCTION__ << ":\n" << error << "\n";
            }
        }
        else{
            newID += "0";
        }
    }
    return newID;
}

bool hasEnding(string const &fullString, string const &ending){
    if(fullString.size() >= ending.size()){
        return (fullString.compare(fullString.size() - ending.size(), ending.size(), ending) == 0);
    }
    else{
        return false;
    }
}


void printInColor(string text, short color){
    #if __WIN32__
        if(IsDebuggerPresent()){
            cout << text;
            return;
        }
        HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO Info;
        GetConsoleScreenBufferInfo(hConsole, &Info);
        SetConsoleTextAttribute(hConsole, color);
    #endif
    cout << text;
    #if __WIN32__
        SetConsoleTextAttribute(hConsole, Info.wAttributes);
    #endif
}

string removeNotAscii(string originalText){
    string newText;
    for(unsigned letter = 0; letter < originalText.size(); letter++){
        if(int(originalText[letter]) < 0){
            newText += '#';
            letter += 2;
            continue;
        }
        newText += originalText[letter];
    }
    return newText;
}