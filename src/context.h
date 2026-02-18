#include "engineClass.h"

bool translateIndexToTreeRoots(const vector<ParameterStruct> & Parameters, unsigned index,
    unsigned & realIndex
);

string shortenText(const string & text, const int textLimit);

class ContextClass{
public:
    string ID;
    //string definitionEventId;
    //value, pointer, variable, vector, camera, layer, object, text, editable_text, super_text, super_editable_text, image, movement, collision, particles, event, variable, scrollbar, primitives, vector
    DataType type = null_dt;
    bool readOnly = false;
    bool isPointingToMember = false; //If true, engine will automatically keep this pointer valid. Otherwise, accessing not-reassign pointer inside a new scope is an undefined behavior.
    size_t containerIndex = 0;
    vector <VariableModule> Values; //Variables exist during the lifespan of Events' chain execution.
    vector <BasePointersStruct> BasePointers;
    ModulesPointers Modules;
    vector <AncestorObject*> Objects;
    vector <LayerClass*> Layers;
    vector <Camera2D*> Cameras;
    ContextClass();
    ContextClass(const ContextClass& Original);
    ContextClass & operator=(const ContextClass& Original);
    void clear();
    void clearState();
    void clearPointers();
    size_t getVectorSize() const;
    string getUniqueIndexes() const;
    string getValue(const InstrDescription & CurrentInstr, int maxLengthOfValuesPrinting) const;
    ReturnType getAllValues(vector<VariableModule> & NewValues, bool negate = false) const;
    ReturnType getAllNegatedValues(vector<VariableModule> & NewValues) const;
    ReturnType moveAllValues(vector<VariableModule> & NewValues, bool negate, unsigned realIndex) const;
    ReturnType getDoubleValues(vector<double> & NewValues, bool negate) const;
    ReturnType getNegatedDoubleValues(vector<double> & NewValues) const;
    ReturnType getValue(VariableModule & NewValue);
    bool getUnsignedOrAbort(unsigned & number, const InstrDescription & CurrentInstr);
    bool getIntOrAbort(int & number, const InstrDescription & CurrentInstr);
    bool getStringOrAbort(string & text, const InstrDescription & CurrentInstr);
    bool getStringOrIgnore(string & text, const EngineInstr & instruction);
    bool getStringVectorOrIgnore(vector<string> & result, const EngineInstr & instruction);
    template<typename T>
    void addBasePointer(T * pointer);
    template<typename T>
    void setFirstBasePointer(T * pointer);
    void updateType(size_t vecSize);
    void addModule(SuperTextModule * Module);
    void addModule(SuperEditableTextModule * Module);
    void addModule(ImageModule * Module);
    void addModule(MovementModule * Module);
    void addModule(CollisionModule * Module);
    void addModule(ParticleEffectModule * Module);
    void addModule(EventModule * Module);
    void addModule(VariableModule * Module);
    void addModule(ScrollbarModule * Module);
    void addModule(PrimitivesModule * Module);
    void addModule(VectorModule * Module);

    bool copyFromTheParameter(
        std::vector<ContextClass> & MemoryMap, const vector<DynamicVariableInfo> & EventLocalVariables,
        const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
        bool & negateAfterCopy, unsigned index, bool printErrors
    );
    void copyOnlyCurrentType(const ContextClass *Original);
    void leaveOneRandomBasePointer();
    void printOutObjects();
    ReturnType collapseToValue();

    unsigned size() const;
    bool empty() const;
};

using MemoryMapType = std::vector<ContextClass>;

ContextClass * getVariableByAddress(const InstrDescription & CurrentInstr,
    MemoryMapType & MemoryMap, const DynamicVariableInfo & Variable,
    const string & variableName, const bool & printError
);

template <typename T>
void ContextClass::addBasePointer(T *pointer){
    if(pointer == nullptr){
        return;
    }
    BasePointers.emplace_back(BasePointersStruct());
    BasePointers.back().setPointer(pointer);
    type = pointer_vec;
}