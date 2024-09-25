#ifndef VECTORMODULE_H_INCLUDED
#define VECTORMODULE_H_INCLUDED
#include "variableModule.h"

//Vector<bool> optimalisation does not allow getting a reference to its bool instances thus a wrapper is needed.
struct stupidBool{
    bool value;
};

bool operator== (stupidBool, const bool);

class VectorModule{
    bool deleted;
    string ID;
    string layerID;
    string objectID;
    char type;
    vector <stupidBool> vBool;
    vector <int> vInt;
	vector <double> vDouble;
	vector <string> vString;
public:
    VectorModule();
    VectorModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID);
    VectorModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, string newType);
    VectorModule(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, char newType);
    void clear();
    void setAllIDs(string newID, vector<string> *listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void clone(const VectorModule & Original, vector<string> & listOfIDs, string newLayerID, string newObjectID, const bool & changeOldID);
    void deleteLater();
    void setID(string newID, vector<string> * listOfIDs);
    void setLayerID(string);
    void setObjectID(string);
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
    bool setString(size_t index, string newValue);
    void move(std::vector<VariableModule *> Variables, const InstrDescription & CurrentInstr);

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

    VariableModule getValue(string attribute, size_t index) const;
    vector<VariableModule> getValues() const;
    string getAnyStringValue(size_t index) const;
    string getAnyStringValueUnsafe(long index) const;
    vector <string> getAllValuesAsStringVector() const;
    bool getBool(size_t index) const;
    bool getBoolUnsafe(long index) const;
    int getInt(size_t index) const;
    int getIntUnsafe(long index) const;
    double getDouble(size_t index) const;
    double getDoubleUnsafe(long index) const;
    string getString(size_t index) const;
    string getStringUnsafe(long index) const;
    string getLastValue() const;
    vector <string> getAllStrings() const;
    bool getLastBool() const;
    int getLastInt() const;
    double getLastDouble() const;
    string getLastString() const;
    bool getFirstBool() const;
    int getFirstInt() const;
    double getFirstDouble() const;
    string getFirstString() const;
    size_t getSize() const;
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);

    bool contains(const VariableModule & Variable);
    bool contains(const BasePointersStruct & Pointer);
};

#endif // VECTORMODULE_H_INCLUDED