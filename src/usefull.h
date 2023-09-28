#ifndef USEFULL_H_INCLUDED
#define USEFULL_H_INCLUDED

#define ALLEGRO_UNSTABLE
#include <algorithm>
#include <iterator>
#include "vectors.h"

using std::string;

const double FPS = 60.0;
const vector<string> reservedIDs = {"KERNEL", "TERMINAL"};

string shortToStr(short integer);
string intToStr(int integer);
string uIntToStr(unsigned int integer);
string doubleToStr(double doubleVar);
double countDistance(double x1, double y1, double x2, double y2);
double countDistance2(double & x1, double & y1, double & x2, double & y2);
int randomInt(int minValue, int maxValue);
double randomDouble(double minValue, double maxValue);
short translateToAllegroKey(char key);
bool isStringInGroup(string ID, int count, ...);
bool isCharInGroup(char ID, int count, ...);

void addUniqueToStringVector(vector <string> & stringVec, string newString);
void removeFromStringVector(vector <string> & stringVec, std::string selectedString);
bool isStringInVector(const vector <string> & stringVec, std::string findString);

string findNewUniqueID(vector <string> IDs, string newID);


template<typename Iterator, typename Pred, typename OperationClass>
void for_each_if(Iterator begin, Iterator end, Pred p, OperationClass op) {
    for(; begin != end; begin++) {
        if (p(*begin)) {
            op(*begin);
        }
    }
}

bool hasEnding(string const &fullString, string const &ending);

//Function is windows-only.
//Numbers of colors are in "windows colors.PNG" file.
void printInColor(string text, short color);


#endif // USEFULL_H_INCLUDED
