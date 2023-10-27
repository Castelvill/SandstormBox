#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <regex>

using std::string;
using std::vector;

struct expression{
    string subject; //id, me, mouse, count, cam
    string target; //id, me, group
    string targetID;
    string module; //gen, coll, event, img, mov, part, scroll, text, var
    string elementID;
    string attribute;
};

class ScopeVariable{
public:
    string ID;
    string pointer;
    double value;
    ScopeVariable(string newID){
        ID = newID;
    }
};

class Scope{
public:
    string type;
    vector <ScopeVariable> Variables;
};

bool isOneOfSubjects(string word){
    string subjects[] = {"layer", "obj", "camera", "mouse", "let", "me",
            "if", "else", "first", "last", "forall", "break", "return"};
    string * subjectsEnd = subjects+sizeof(subjects)/sizeof(subjects[0]);
    if(subjectsEnd != std::find(subjects, subjectsEnd, word)) {
        return true;
    }
    return false;
}

bool startsNewScope(string word){
    string subjects[] = {"if", "else", "first", "last", "forall"};
    string * subjectsEnd = subjects+sizeof(subjects)/sizeof(subjects[0]);
    if(subjectsEnd != std::find(subjects, subjectsEnd, word)) {
        return true;
    }
    return false;
}

vector <string> sytaxOutput(string input){
    std::regex word_regex("(\"*[\\w+\\.*]*\\w+\"*)|;|==|=|>|<|>=|<=|-=|\\+=|\\+|-|\\*|/|%", std::regex_constants::icase);
    auto words_begin =
        std::sregex_iterator(input.begin(), input.end(), word_regex);
    auto words_end = std::sregex_iterator();

    int amountOfWords = std::distance(words_begin, words_end);

    vector <string> output;
    string match_str;

    //cout << "Words (" << amountOfWords <<"):\n";
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        match_str = match.str();
        if (match_str.size() > 0){
            output.push_back(match_str);
            cout << output.back() << ", ";
        }
    }
    cout << "\n";
    return output;
}

int main()
{
    std::ifstream file;
    file.open("mario.cpp");
    if(!file)
    {
        cout << "File opening failed\n";
        return -1;
    }

    string input;
    vector <string> words;
    char inputChar;
    vector <Scope> localMem;
    localMem.push_back(Scope());
    cout << "Scope " << localMem.size()-1 << ":\n";

    while(file.get(inputChar)){
        cout << ".";
        input += inputChar;

        if(localMem.back().type != ""){
            words = sytaxOutput(input);

            if(localMem.back().type == "let"){
                if(words.size() < 4){
                    words.clear();
                    continue;
                }
                if(words[1] != "="){
                    cout << "Syntax error: no equals sign!\n";
                    return -1;
                }
                if(words[2] == ";"){
                    cout << "Syntax error: cannot create empty variable!\n";
                    return -1;
                }
                if(words.back() != ";"){
                    cout << "Syntax error: no semicolon!\n";
                    return -1;
                }

                //localMem.back().Variables.push_back(ScopeVariable(input));
                //localMem.back().type += "_name";

                input = "";
                localMem.back().type = "";
                words.clear();
                cout << "Variable added!\n";

            }
            continue;
        }
        else if(inputChar == ' ' || inputChar == '\n'){
            input.pop_back();
            continue;
        }

        if(isOneOfSubjects(input)){
            if(startsNewScope(input)){
                localMem.push_back(Scope());
                cout << "Scope " << localMem.size()-1 << ":\n";
            }
            localMem.back().type = input;
            cout << "Type: " << localMem.back().type << "\n";
            input = "";
            continue;
        }

    }

    //cout << input;

    file.close();
    return 0;
}



