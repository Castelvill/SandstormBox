#ifndef VARIABLEMODULE_H_INCLUDED
#define VARIABLEMODULE_H_INCLUDED

#include "usefull.h"

class VariableModule{
    string ID;
    char type;
    bool vBool, defaultBool;
    int vInt, defaultInt;
	double vDouble, defaultDouble;
	string vString, defaultString;
public:
    VariableModule(unsigned int newID);
    string getID();
    string &getIDAddr();
    char getType();
    bool getDefaultBool();
    int getDefaultInt();
    double getDefaultDouble();
    string getDefaultString();
    bool getBool();
    int getInt();
    double getDouble();
    string getString();
    void setID(string);
    bool setType(char);
    bool tryToSetType(char newType);
    bool setDefaultBool(bool);
    bool toggleDefaultBool();
    bool setDefaultInt(int);
    bool setDefaultDouble(double);
    bool setDefaultString(string);
    bool setBool(bool);
    bool toggleBool();
    bool setInt(int);
    bool setDouble(double);
    bool setString(string);
    bool addDefaultInt(int);
    bool addDefaultDouble(double);
    bool addInt(int);
    bool addDouble(double);
    void resetValue();
    template <typename condValueType>
    bool isConditionMet(condValueType condVal, string operatorType, char valType);
    bool isConditionMet(string condVal, string operatorType, char valType);
    bool isConditionMet(VariableModule * OtherVariable, string operatorType);
    VariableModule & operator=(const VariableModule& original);
};


#endif // VARIABLEMODULE_H_INCLUDED
