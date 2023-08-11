#ifndef VARIABLEMODULE_H_INCLUDED
#define VARIABLEMODULE_H_INCLUDED

#include "usefull.h"

class VariableModule{
    string ID;
    string layerID; //This ID is needed in events' trigger detection.
    string objectID;
    char type;
    bool vBool, defaultBool;
    int vInt, defaultInt;
	double vDouble, defaultDouble;
	string vString, defaultString;
public:
    VariableModule(unsigned newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VariableModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VariableModule();
    void clone(const VariableModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID);
    void setAllIDs(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    string getID() const;
    string getLayerID();
    string getObjectID();
    string &getIDAddr();
    string getAnyValue();
    char getType();
    string getFullTypeName();
    bool getDefaultBool();
    int getDefaultInt();
    double getDefaultDouble();
    string getDefaultString();
    bool getBool();
    int getInt();
    double getDouble();
    string getString();
    void setID(string, vector<string> * listOfIDs);
    void setLayerID(string);
    void setObjectID(string);
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
    bool setString(char);
    bool addDefaultInt(int);
    bool addDefaultDouble(double);
    bool addInt(int);
    bool addDouble(double);
    void resetValue();
    void negate();
    void getContext(string attribute, BasePointersStruct & BasePointer);
    template <typename condValueType>
    bool isConditionMet(condValueType condVal, string operatorType, char valType);
    bool isConditionMet(string condVal, string operatorType, char valType);
    bool isConditionMet(string operatorType, VariableModule * OtherVariable);
    int intOperation(string operatorType, VariableModule * OtherVariable);
    double floatingOperation(string operatorType, VariableModule * OtherVariable);
    VariableModule & operator=(const VariableModule& original);
    void setValueFromPointer(const BasePointersStruct & BasePointer);
    void set(const BaseVariableStruct & BaseVariable);
};


#endif // VARIABLEMODULE_H_INCLUDED
