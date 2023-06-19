#ifndef USEFULL_H_INCLUDED
#define USEFULL_H_INCLUDED

#define ALLEGRO_UNSTABLE

#include "primaryModule.h"
#include <allegro5/allegro.h>
#include <algorithm>


using std::string;

const double FPS = 60.0;

string intToStr4(int integer);
string intToStr8(int integer);
string doubleToStr4(double doubleVar);
string doubleToStr8(double doubleVar);
double countDistance(double x1, double y1, double x2, double y2);
double countDistance2(double & x1, double & y1, double & x2, double & y2);
int randomInt(int minValue, int maxValue);
double randomDouble(double minValue, double maxValue);
short translateToAllegroKey(char key);
bool isStringInGroup(string ID, int count, ...);
bool isCharInGroup(char ID, int count, ...);

void addNewStringToVector(vector <string> & stringVec, string newString);
void removeStringFromVector(vector <string> & stringVec, std::string selectedString);
bool isInAStringVector(vector <string> & stringVec, std::string findString);


template<typename Iterator, typename Pred, typename OperationClass>
void for_each_if(Iterator begin, Iterator end, Pred p, OperationClass op) {
    for(; begin != end; begin++) {
        if (p(*begin)) {
            op(*begin);
        }
    }
}


template<class SearchModule>
SearchModule * findByIDAndReturnPointer(vector <SearchModule> & Container, string ID) {
    for(SearchModule & Instance : Container) {
        if (Instance.getID() == ID) {
            return &Instance;
        }
    }
    return nullptr;
}


#endif // USEFULL_H_INCLUDED
