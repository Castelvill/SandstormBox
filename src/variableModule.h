#ifndef VARIABLEMODULE_H_INCLUDED
#define VARIABLEMODULE_H_INCLUDED

#include "movementModule.h"

class UniversalVariable{
public:
    char type = 'n'; //n - null, b - bool, i - int, d - double, s - string
    bool vBool = false;
    int vInt = 0;
	double vDouble = 0.0;
	string vString = "";
};

template<typename LeftType, typename RightType>
void executeMoveTypeInstruction(LeftType * LeftOperand, RightType * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);

class VariableModule: public UniversalVariable{
    bool deleted = false;
    string ID;
    string layerID; //This ID is needed in events' trigger detection.
    string objectID;
public:
    VariableModule(unsigned newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VariableModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VariableModule();
    VariableModule(bool value);
    VariableModule(int value);
    VariableModule(double value);
    VariableModule(string value);
    VariableModule(const VariableModule & Original);
    VariableModule & operator=(const VariableModule& Original);
    ReturnType copyValue(const VariableModule& Original);
    ReturnType copyValue(const VariableModule * Original);
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
    string getIdentity() const;
    void setID(string, vector<string> * listOfIDs);
    void setIdUnsafe(string);
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
    bool addString(string newValue);
    void addStringUnsafe(string newValue);
    ReturnType negate();
    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
    template <typename condValueType>
    bool isConditionMet(condValueType condVal, EngineInstr operatorType, char valType);
    std::pair<bool, ReturnType> isConditionMet(const string & condVal, EngineInstr operatorType, char valType);
    std::pair<bool, ReturnType> isConditionMet(EngineInstr operatorType, VariableModule * OtherVariable);
    std::pair<bool, ReturnType> isConditionMet(EngineInstr operatorType, const BasePointersStruct & OtherVariable);
    double floatingOperation(EngineInstr operatorType, VariableModule * OtherVariable);
    double floatingOperation(EngineInstr operatorType, BasePointersStruct * OtherVariable);
    int intOperation(EngineInstr operatorType, VariableModule * OtherVariable);
    int intOperation(EngineInstr operatorType, BasePointersStruct * RightOperand);
    string stringOperation(EngineInstr operatorType, VariableModule * OtherVariable);
    string stringOperation(EngineInstr operatorType, BasePointersStruct * RightOperand);
    template<typename RightType>
    void moveFromTemp(RightType * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    void move(VariableModule * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    void move(const BasePointersStruct *RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    BaseVariableStruct getBaseVariableStruct() const;
    BasePointersStruct getBasePointersStruct();
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
    static VariableModule newString(const string & val);
    static VariableModule newString(const string & val, string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    bool isNumeric() const;
};


#endif // VARIABLEMODULE_H_INCLUDED
