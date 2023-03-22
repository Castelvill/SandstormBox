#ifndef EGMCOMPILER_H_INCLUDED
#define EGMCOMPILER_H_INCLUDED
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>

/*
EgmCompiler compiles a game made in Editor to EgmCode
*/

class Text{
public:
    std::string textContent;
    void writeText();
    Text(std::string newText);
};
class egmCompiler{
private:
    std::vector<Text> Texts;
    int temp;

public:
    egmCompiler();
    void writeAllText();
    void loadTestData();
};

#endif // EGMCOMPILER_H_INCLUDED
