#include "egmCompiler.h"

Text::Text(std::string newText){
    textContent = newText;
}
void Text::writeText(){
    std::cout << textContent << std::endl;
}

egmCompiler::egmCompiler(){
    temp = 0;
}
void egmCompiler::writeAllText(){
    for(auto singleText : Texts)
        singleText.writeText();
    std::cout << "\nNumber: " << temp << std::endl;
}
void egmCompiler::loadTestData(){
    std::ifstream file;
    file.open("app0.txt");
    if(!file)  // operator! is used here
    {
        std::cout << "File opening failed\n";
        return;
    }
    std::string input;
    std::string objType = "";
    std::string txt = "";
    bool readText = false;
    int number = 0;
    char inputChar;

    while(file.get(inputChar)){
        input += inputChar;
        if(objType == ""){
            if(inputChar == ' ' || inputChar == '\n'){
                input = "";
            }
            if(input.size() == 2){
                objType = input;
                input = "";
                number = 0;
            }
            continue;
        }
        if(objType == "00"){
            if(!readText && (inputChar == ':' || inputChar == ',')){
                input = "";
                continue;
            }
            if(!readText && inputChar == '"'){
                readText = true;
                input = "";
            }
            else if(!readText && inputChar == ';'){
                readText = false;
                objType = "";
                input = "";
            }
            else if(readText && inputChar == '"'){
                input = input.substr(0, input.size()-1);
                Texts.push_back(Text(input));
                readText = false;
                input = "";
            }
        }
        else if(objType == "01"){
            if(!readText && inputChar == ':'){
                readText = true;
                input = "";
                continue;
            }
            if(readText && (inputChar == ',' || inputChar == ' ' || inputChar == ';')){
                input = input.substr(0, input.size()-1);
                if(input != "")
                    number += atoi(input.c_str());
                input = "";
                if(inputChar == ';'){
                    readText = false;
                    objType = "";
                    temp += number;
                    number = 0;
                }
            }
        }
        else if(objType == "02"){
            if(!readText && inputChar == ':'){
                readText = true;
                input = "";
                continue;
            }
            if(readText && (inputChar == ',' || inputChar == ' ' || inputChar == ';')){
                input = input.substr(0, input.size()-1);
                if(input != "")
                    number += atoi(input.c_str());
                input = "";
                if(inputChar == ';'){
                    readText = false;
                    objType = "";
                    temp -= number;
                    number = 0;
                }
            }
        }

    }
    file.close();
}
