#ifndef VARIABLEMODULE_H_INCLUDED
#define VARIABLEMODULE_H_INCLUDED

#include "movementModule.h"

class VariableModule{
    bool deleted;
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
    
    void deleteLater();
    bool getIsDeleted() const;
    void clear();
    void clone(const VariableModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void setAllIDs(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    string getID() const;
    string getLayerID() const;
    string getObjectID() const;
    string &getIDAddr();
    string getAnyValue();
    char getType() const;
    string getFullTypeName();
    bool getDefaultBool();
    int getDefaultInt();
    double getDefaultDouble();
    string getDefaultString();
    bool getBool() const;
    bool getBoolUnsafe() const;
    bool getBoolUnsafe(const bool & defaultValue) const;
    int getInt() const;
    int getIntUnsafe() const;
    double getDouble() const;
    double getDoubleUnsafe() const;
    string getString() const;
    string getStringUnsafe() const;
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
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    template <typename condValueType>
    bool isConditionMet(condValueType condVal, string operatorType, char valType);
    bool isConditionMet(string condVal, string operatorType, char valType);
    bool isConditionMet(string operatorType, VariableModule * OtherVariable);
    bool isConditionMet(string operatorType, const BasePointersStruct & OtherVariable);
    double floatingOperation(string operatorType, VariableModule * OtherVariable);
    double floatingOperation(string operatorType, BasePointersStruct * OtherVariable);
    int intOperation(string operatorType, VariableModule * OtherVariable);
    int intOperation(string operatorType, BasePointersStruct * RightOperand);
    string stringOperation(string operatorType, VariableModule * OtherVariable);
    string stringOperation(string operatorType, BasePointersStruct * RightOperand);
    template<typename LeftType, typename RightType>
    void executeMoveTypeInstruction(LeftType * LeftOperand, RightType * RightOperand, string instruction);
    template<typename RightType>
    void moveFromTemp(RightType * RightOperand, string instruction);
    void move(VariableModule * RightOperand, string instruction);
    void move(const BasePointersStruct *RightOperand, string instruction);
    BaseVariableStruct getVariableStruct() const;
    VariableModule & operator=(const VariableModule& original);
    template <typename T>
    void tryToSetFromPointer(const T & value, char newType);
    void setValueFromPointer(const BasePointersStruct & BasePointer);
    void set(const BaseVariableStruct & BaseVariable);
    static VariableModule newBool(bool);
    static VariableModule newBool(bool val, string newID);
    static VariableModule newInt(int);
    static VariableModule newDouble(double);
    static VariableModule newString(string);
};


#endif // VARIABLEMODULE_H_INCLUDED
