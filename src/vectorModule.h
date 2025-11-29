#ifndef VECTORMODULE_H_INCLUDED
#define VECTORMODULE_H_INCLUDED
#include "variableModule.h"

//Vector<bool> optimalisation does not allow getting a reference to its bool instances thus a wrapper is needed.
struct stupidBool{
    bool value = false;
};

bool operator== (stupidBool, const bool);

bool checkForVectorSize(const InstrDescription & CurrentInstr, size_t leftSize,
    size_t rightSize, bool & incLeftIdx, bool & incRightIdx, unsigned & maxIndex, const string & functionName
);

class VectorModule{
    size_t uniqueIndex = 0;
    size_t objectUniqueIndex = 0;
    size_t layerUniqueIndex = 0;

    bool deleted = false;
    string ID;
    string layerID;
    string objectID;
    char type = 'n';
public:
    vector <stupidBool> vBool;
    vector <int> vInt;
	vector <double> vDouble;
	vector <string> vString;
    VectorModule(size_t & topModuleUniqueIndex);
    void setAllIndexes(size_t index, size_t layerIndex, size_t objectIndex);
    VectorModule(const VectorModule & Original);
    VectorModule & operator=(const VectorModule& Original);
    VectorModule(PrimaryData & initData);
    VectorModule(PrimaryData & initData, const string & newType);
    VectorModule(PrimaryData & initData, char newType);
    void clear();
    void setAllIDs(PrimaryData & initData, bool  changeOldID);
    void clone(const VectorModule &Original, PrimaryData & initData, bool changeOldID);
    void deleteLater();
    void setUniqueIndex(size_t value);
    size_t getUniqueIndex() const;
    void setObjectUniqueIndex(size_t value);
    size_t getObjectUniqueIndex() const;
    void setLayerUniqueIndex(size_t value);
    size_t getLayerUniqueIndex() const;
    void setID(string newID, vector<string> * listOfIDs);
    void setLayerID(const string & newID);
    void setObjectID(const string & newID);
    bool setType(char newType);
    bool pushBool(bool newValue);
    void pushBoolUnsafe(bool newValue);
    bool pushInt(int newValue);
    void pushIntUnsafe(int newValue);
    bool pushDouble(double newValue);
    void pushDoubleUnsafe(double newValue);
    bool pushString(string newValue);
    void pushStringUnsafe(string newValue);
    bool setBool(size_t index, bool newValue);
    bool setInt(size_t index, int newValue);
    bool setDouble(size_t index, double newValue);
    bool setString(size_t index, const string & newValue);
    template<typename RightType>
    void moveValueToEachInstance(RightType * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    void move(VariableModule* Variables, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    // requires (std::same_as<RightOperandType, VariableModule> || std::same_as<RightOperandType, BasePointersStruct>
    //     || std::same_as<RightOperandType, VariableModule*>)
    template<class RightOperandType>
    void move(vector<RightOperandType> * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    void move(BasePointersStruct* RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    void move(VectorModule* RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    //Move its content to LeftOperand.
    void moveFrom(VariableModule * LeftOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    void moveFrom(BasePointersStruct * LeftOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
    ReturnType setVariableWithFirstValue(VariableModule & Variable);

    bool popBack();
    bool removeIndex(size_t index);
    void reserve(unsigned newSize);

    bool getIsDeleted() const;
    string getID() const;
    string & getIDAddr();
    string getLayerID() const;
    string getObjectID() const;
    char getType() const;
    string getFullTypeName() const;

    VariableModule getValue(AttributeType attribute, size_t index) const;
    vector<VariableModule> getValues() const;
    ReturnType getValuesIntoContext(vector<VariableModule> & Values, bool negate = false) const;
    ReturnType getNegatedValuesIntoContext(vector<VariableModule> & Values) const;
    string getAnyStringValue(size_t index) const;
    string getAnyStringValueUnsafe(long index) const;
    vector <string> getAllValuesAsStringVector() const;
    bool getBool(size_t index) const;
    bool getBoolUnsafe(long index) const;
    vector <stupidBool> getAllBools() const;
    vector<VariableModule> getAllBoolsAsVariables();
    int getInt(size_t index) const;
    int getIntUnsafe(long index) const;
    vector <int> getAllInts() const;
    vector<VariableModule> getAllIntsAsVariables();
    double getDouble(size_t index) const;
    double getDoubleUnsafe(long index) const;
    vector <double> getAllDoubles() const;
    vector<VariableModule> getAllDoublesAsVariables();
    string getString(size_t index) const;
    string getStringUnsafe(long index) const;
    string getLastValue() const;
    vector <string> getAllStrings() const;
    void getAllStrings(vector<string> & values) const;
    vector<VariableModule> getAllStringsAsVariables();
    bool getLastBool() const;
    int getLastInt() const;
    double getLastDouble() const;
    string getLastString() const;
    bool getFirstBool() const;
    int getFirstInt() const;
    double getFirstDouble() const;
    string getFirstString() const;
    size_t getSize() const;
    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
    BasePointersStruct getBasePointersStruct(const unsigned & index);

    bool contains(const VariableModule & Variable);
    bool contains(const BasePointersStruct & Pointer);
};

extern template void VectorModule::move<VariableModule>(vector<VariableModule> * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
extern template void VectorModule::move<VariableModule*>(vector<VariableModule*> * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);
extern template void VectorModule::move<BasePointersStruct>(vector<BasePointersStruct> * RightOperand, const EngineInstr & instruction, const InstrDescription & CurrentInstrInfo);

#endif // VECTORMODULE_H_INCLUDED