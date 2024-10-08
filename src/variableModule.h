#ifndef VARIABLEMODULE_H_INCLUDED
#define VARIABLEMODULE_H_INCLUDED

#include "movementModule.h"

class UniversalVariable{
public:
    char type; //n - null, b - bool, i - int, d - double, s - string
    bool vBool;
    int vInt;
	double vDouble;
	string vString;
};

class VariableModule: public UniversalVariable{
    bool deleted;
    string ID;
    string layerID; //This ID is needed in events' trigger detection.
    string objectID;
public:
    VariableModule(unsigned newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VariableModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VariableModule();
    ~VariableModule();
    
    void deleteLater();
    bool getIsDeleted() const;
    void clear();
    void clone(const VariableModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void setAllIDs(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    string getID() const;
    string getLayerID() const;
    string getObjectID() const;
    string &getIDAddr();
    string getAnyValue() const;
    char getType() const;
    string getFullTypeName();
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
    bool setBool(bool);
    bool toggleBool();
    bool setInt(int);
    bool setDouble(double);
    bool setString(string);
    bool setString(char);
    bool addInt(int);
    bool addDouble(double);
    void negate();
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    template <typename condValueType>
    bool isConditionMet(condValueType condVal, EngineInstr operatorType, char valType);
    bool isConditionMet(string condVal, EngineInstr operatorType, char valType);
    bool isConditionMet(EngineInstr operatorType, VariableModule * OtherVariable);
    bool isConditionMet(EngineInstr operatorType, const BasePointersStruct & OtherVariable);
    double floatingOperation(EngineInstr operatorType, VariableModule * OtherVariable);
    double floatingOperation(EngineInstr operatorType, BasePointersStruct * OtherVariable);
    int intOperation(EngineInstr operatorType, VariableModule * OtherVariable);
    int intOperation(EngineInstr operatorType, BasePointersStruct * RightOperand);
    string stringOperation(EngineInstr operatorType, VariableModule * OtherVariable);
    string stringOperation(EngineInstr operatorType, BasePointersStruct * RightOperand);
    template<typename LeftType, typename RightType>
    void executeMoveTypeInstruction(LeftType * LeftOperand, RightType * RightOperand, EngineInstr instruction);
    template<typename RightType>
    void moveFromTemp(RightType * RightOperand, EngineInstr instruction);
    void move(VariableModule * RightOperand, EngineInstr instruction);
    void move(const BasePointersStruct *RightOperand, EngineInstr instruction);
    BaseVariableStruct getVariableStruct() const;
    VariableModule & operator=(const VariableModule& original);
    template <typename T>
    void tryToSetFromPointer(const T & value, char newType);
    void setValueFromPointer(const BasePointersStruct & BasePointer);
    void set(const BaseVariableStruct & BaseVariable);
    static VariableModule newBool(bool);
    static VariableModule newBool(bool val, string newID);
    static VariableModule newBool(bool val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    static VariableModule newInt(int);
    static VariableModule newInt(int val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    static VariableModule newDouble(double);
    static VariableModule newDouble(double val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    static VariableModule newString(string);
    static VariableModule newString(string val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    bool isNumeric() const;
};


#endif // VARIABLEMODULE_H_INCLUDED
