#include "context.h"

void ContextClass::clear(){
    type = any_dt;
    clearState();
    type = null_dt;
    isPointingToMember = false;
}
void ContextClass::clearState(){
    switch(type){
        case value_inst:
            if(!Values.empty()){
                Values[0].clear();
            }
            else{
                Values.emplace_back(VariableModule());
            }
            return;
        case value_vec:
            Values.clear();
            return;
        case pointer_inst:
        case pointer_vec:
            BasePointers.clear();
            return;
        case camera_inst:
        case camera_vec:
            Cameras.clear();
            return;
        case layer_inst:
        case layer_vec:
            Layers.clear();
            return;
        case object_inst:
        case object_vec:
            Objects.clear();
            return;
        case variable_mod:
        case variable_mod_vec:
            Modules.Variables.clear();
            return;
        case vector_mod:
        case vector_mod_vec:
            Modules.Vectors.clear();
            return;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts.clear();
            return;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts.clear();
            return;
        case image_mod:
        case image_mod_vec:
            Modules.Images.clear();
            return;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements.clear();
            return;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions.clear();
            return;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles.clear();
            return;
        case event_mod:
        case event_mod_vec:
            Modules.Events.clear();
            return;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars.clear();
            return;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives.clear();;
            return;
        case any_dt:
            Values.clear();
            BasePointers.clear();
            Cameras.clear();
            Layers.clear();
            Objects.clear();
            Modules.Variables.clear();
            Modules.Vectors.clear();
            Modules.SuperTexts.clear();
            Modules.SuperEditableTexts.clear();
            Modules.Images.clear();
            Modules.Movements.clear();
            Modules.Collisions.clear();
            Modules.Particles.clear();
            Modules.Events.clear();
            Modules.Scrollbars.clear();
            Modules.Primitives.clear();
            return;
        case null_dt:
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Type '" << dataTypeToStr(type) << "' is not valid.\n";
            type = null_dt;
            return;
    }
}
void ContextClass::clearPointers(){
    switch(type){
        case camera_inst:
        case camera_vec:
            Cameras.clear();
            return;
        case layer_inst:
        case layer_vec:
            Layers.clear();
            return;
        case object_inst:
        case object_vec:
            Objects.clear();
            return;
        case variable_mod:
        case variable_mod_vec:
            Modules.Variables.clear();
            return;
        case vector_mod:
        case vector_mod_vec:
            Modules.Vectors.clear();
            return;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts.clear();
            return;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts.clear();
            return;
        case image_mod:
        case image_mod_vec:
            Modules.Images.clear();
            return;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements.clear();
            return;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions.clear();
            return;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles.clear();
            return;
        case event_mod:
        case event_mod_vec:
            Modules.Events.clear();
            return;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars.clear();
            return;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives.clear();;
            return;
        case any_dt:
            Cameras.clear();
            Layers.clear();
            Objects.clear();
            Modules.Variables.clear();
            Modules.Vectors.clear();
            Modules.SuperTexts.clear();
            Modules.SuperEditableTexts.clear();
            Modules.Images.clear();
            Modules.Movements.clear();
            Modules.Collisions.clear();
            Modules.Particles.clear();
            Modules.Events.clear();
            Modules.Scrollbars.clear();
            Modules.Primitives.clear();
            return;
        default:
            return;
    }
}
size_t ContextClass::getVectorSize() const{
    switch(type){
        case value_inst:
        case value_vec:
            return Values.size();
        case pointer_inst:
        case pointer_vec:
            return BasePointers.size();
        case variable_mod:
        case variable_mod_vec:
            return Modules.Variables.size();
        case camera_inst:
        case camera_vec:
            return Cameras.size();
        case layer_inst:
        case layer_vec:
            return Layers.size();
        case object_inst:
        case object_vec:
            return Objects.size();
        case super_text_mod:
        case super_text_mod_vec:
            return Modules.SuperTexts.size();
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            return Modules.SuperEditableTexts.size();
        case image_mod:
        case image_mod_vec:
            return Modules.Images.size();
        case movement_mod:
        case movement_mod_vec:
            return Modules.Movements.size();
        case collision_mod:
        case collision_mod_vec:
            return Modules.Collisions.size();
        case particles_mod:
        case particles_mod_vec:
            return Modules.Particles.size();
        case event_mod:
        case event_mod_vec:
            return Modules.Events.size();
        case scrollbar_mod:
        case scrollbar_mod_vec:
            return Modules.Scrollbars.size();
        case primitives_mod:
        case primitives_mod_vec:
            return Modules.Primitives.size();
        case vector_mod:
        case vector_mod_vec:{ //Should we also add Modules.Vectors.size()? 
            size_t finalSize = 0;
            for(const VectorModule * vec : Modules.Vectors){
                if(vec != nullptr)
                    finalSize += vec->getSize();
            }
            return finalSize;}
        case any_dt:
            return Values.size() + BasePointers.size() + Modules.Variables.size()
            + Cameras.size() + Layers.size() + Objects.size()
            + Modules.SuperTexts.size() + Modules.SuperEditableTexts.size() + Modules.Images.size()
            + Modules.Movements.size() + Modules.Collisions.size()
            + Modules.Particles.size() + Modules.Events.size()
            + Modules.Scrollbars.size() + Modules.Primitives.size() + Modules.Vectors.size();
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Type '" << dataTypeToStr(type) << "' is not valid.\n";
            return 0;
    }
    return 0;
}

template <typename T>
inline string getAllIndexes(const vector<T*> & entityVec){
    string allIndexes;
    for(const T * entity : entityVec){
        if(entity != nullptr){
            allIndexes += std::to_string(entity->getUniqueIndex());
        }
        allIndexes += ", ";
    }
    return allIndexes;
}
string ContextClass::getUniqueIndexes() const{
    switch(type){
        case variable_mod:
        case variable_mod_vec:
            return getAllIndexes(Modules.Variables);
        case layer_inst:
        case layer_vec:
            return getAllIndexes(Layers);
        case object_inst:
        case object_vec:
            return getAllIndexes(Objects);
        case super_text_mod:
        case super_text_mod_vec:
            return getAllIndexes(Modules.SuperTexts);
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            return getAllIndexes(Modules.SuperEditableTexts);
        case image_mod:
        case image_mod_vec:
            return getAllIndexes(Modules.Images);
        case movement_mod:
        case movement_mod_vec:
            return getAllIndexes(Modules.Movements);
        case collision_mod:
        case collision_mod_vec:
            return getAllIndexes(Modules.Collisions);
        case particles_mod:
        case particles_mod_vec:
            return getAllIndexes(Modules.Particles);
        case event_mod:
        case event_mod_vec:
            return getAllIndexes(Modules.Events);
        case scrollbar_mod:
        case scrollbar_mod_vec:
            return getAllIndexes(Modules.Scrollbars);
        case primitives_mod:
        case primitives_mod_vec:
            return getAllIndexes(Modules.Primitives);
        case vector_mod:
        case vector_mod_vec:
            return getAllIndexes(Modules.Vectors);
        default:
            return "";
    }
}

string shortenText(const string & text, const int textLimit){
    string newText = text;
    if(textLimit >= 0 && text.size() > unsigned(textLimit)){
        newText = text.substr(0, textLimit);
        if(newText[0] == '\"'){
            newText += "...\"";
        }
        newText += "(+" + intToStr(text.size()-textLimit);
        newText += ")";
    }
    string squeezedText = "";
    for(char letter : newText){
        if(letter == '\n'){
            squeezedText += "\\n";
        }
        else{
            squeezedText += letter;
        }
    }
    return squeezedText;
}
string ContextClass::getValue(const InstrDescription & CurrentInstr, int maxLengthOfValuesPrinting) const{
    string buffer = "";
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                buffer += shortenText(Values[0].getAnyValue(), maxLengthOfValuesPrinting);
            }
            else{
                buffer += "null";
            }
            break;
        case value_vec:
            buffer = "[";
            for(const VariableModule & Variable: Values){
                buffer += shortenText(Variable.getAnyValue(), maxLengthOfValuesPrinting);
                buffer += ", ";
            }
            buffer += "]<";
            buffer += uIntToStr(Values.size());
            buffer += ">";
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                buffer += shortenText(BasePointers[0].getString(), maxLengthOfValuesPrinting);
                if(BasePointers[0].readOnly){
                    buffer += "(R)";
                }
            }
            else{
                buffer += "null";
            }
            break;
        case pointer_vec:
            buffer = "[";
            for(const BasePointersStruct & Pointer: BasePointers){
                buffer += shortenText(Pointer.getString(), maxLengthOfValuesPrinting);
                if(Pointer.readOnly){
                    buffer += "(R)";
                }
                buffer += ", ";
            }
            buffer += "]<";
            buffer += uIntToStr(BasePointers.size());
            buffer += ">";
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                if(Modules.Variables[0] == nullptr){
                    buffer += "nullptr";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the variable has a nullptr value.\n";
                }
                else{
                    buffer += shortenText(Modules.Variables[0]->getAnyValue(),
                        maxLengthOfValuesPrinting
                    );
                }
            }
            else{
                buffer += "null";
            }
            break;
        case variable_mod_vec:
            buffer = "[";
            for(const VariableModule * Variable: Modules.Variables){
                if(Variable == nullptr){
                    buffer += "<nullptr>, ";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the variable has a nullptr value.\n";
                    continue;
                }
                buffer += shortenText(Variable->getAnyValue(), maxLengthOfValuesPrinting);
                buffer += ", ";
            }
            buffer += "]<";
            buffer += uIntToStr(Modules.Variables.size());
            buffer += ">";
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                if(Modules.Vectors[0] == nullptr){
                    buffer += "nullptr";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the vector has a nullptr value.\n";
                }
                else{
                    buffer += "[";
                    for(size_t i = 0; i < Modules.Vectors[0]->getSize(); i++){
                        buffer += shortenText(Modules.Vectors[0]->getAnyStringValueUnsafe(i),
                            maxLengthOfValuesPrinting) + ", ";
                    }
                    buffer += "]<";
                    buffer += uIntToStr(Modules.Vectors[0]->getSize());
                    buffer += ">";
                }
            }
            else{
                buffer += "null";
            }
            break;
        case vector_mod_vec:
            buffer = "[";
            for(const VectorModule * Vector: Modules.Vectors){
                if(Vector == nullptr){
                    buffer += "<nullptr>, ";
                    cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "In the context \'" << ID
                        << "\' the pointer to the vector has a nullptr value.\n";
                    continue;
                }
                buffer += "[";
                for(size_t i = 0; i < Vector->getSize(); i++){
                    buffer += shortenText(Vector->getAnyStringValueUnsafe(i),
                        maxLengthOfValuesPrinting) + ", ";
                }
                buffer += "]<";
                buffer += uIntToStr(Vector->getSize());
                buffer += ">, ";
            }
            buffer += "]<";
            buffer += uIntToStr(Modules.Variables.size());
            buffer += ">";
            break;
        case null_dt:
            buffer += "null";
            break;
        default:
            buffer += "<";
            buffer += uIntToStr(getVectorSize());
            buffer += ">";
            break;
    }
    
    if(readOnly){
        buffer += "(R)";
    }

    return buffer;
}
bool ContextClass::getUnsignedOrAbort(unsigned &number, const InstrDescription & CurrentInstr){
    int temp = 0;
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__) << "Context is empty.\n";
                return false;
            }
            if(Values.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "\': Context has more than 1 value - only the last value will be used.\n";
            }
            temp = Values.back().getInt();
            break;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(BasePointers.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the last value will be used.\n";
            }
            temp = BasePointers.back().getInt();
            break;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Modules.Variables.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the last value will be used.\n";
            }
            temp = Modules.Variables.back()->getInt();
            break;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context \'" << ID << "\' has invalid type: \'" << dataTypeToStr(type) << "\'.\n";
            return false;
    }
    if(temp < 0){
        number = 0;
        cout << instructionWarning(CurrentInstr, __FUNCTION__)
            << "Value is not unsigned - returning 0.\n";
    }
    number = temp;
    return true;
}
bool ContextClass::getIntOrAbort(int &number, const InstrDescription & CurrentInstr){
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return true;
            }
            if(Values.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            number = Values[0].getInt();
            return false;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return true;
            }
            if(BasePointers.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            number = BasePointers[0].getInt();
            return false;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return true;
            }
            if(Modules.Variables.size() != 1){
                cout << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            number = Modules.Variables[0]->getInt();
            return false;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context \'" << ID << "\' has invalid type: \'" << dataTypeToStr(type)
                << "\'Function expects an integer or a container of integers.\n";
            return true;
    }
    return false;
}
bool ContextClass::getStringOrAbort(string & text, const InstrDescription & CurrentInstr){
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Values.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            text = Values[0].getString();
            return true;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(BasePointers.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            text = BasePointers[0].getString();
            return true;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Modules.Variables.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            text = Modules.Variables[0]->getString();
            return true;
        case vector_mod:
        case vector_mod_vec:
            if(Modules.Vectors.size() == 0){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Context is empty.\n";
                return false;
            }
            if(Modules.Vectors.size() != 1){
                cerr << instructionWarning(CurrentInstr, __FUNCTION__)
                    << "Context has more than 1 value - only the first value will be used.\n";
            }
            {
                vector<string> textVector = Modules.Vectors[0]->getAllStrings(); 
                text = "";
                for(string textIter : textVector){
                    text += textIter + "\n";
                }
            }
            text.pop_back(); //Remove the last new line.
            return true;
        default:
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Context \'" << ID << "\' has invalid type: '" << dataTypeToStr(type)
                << "'Function expects a string or a container of strings.\n";
            return false;
    }
    return true;
}
bool ContextClass::getStringOrIgnore(string & text, const EngineInstr & instruction){
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0 || Values[0].getType() != 's'){
                return false;
            }
            text = Values[0].getStringUnsafe();
            return true;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0 || BasePointers[0].type != string_bt){
                return false;
            }
            text = BasePointers[0].getString();
            return true;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0 || Modules.Variables[0]->getType() != 's'){
                return false;
            }
            text = Modules.Variables[0]->getStringUnsafe();
            return true;
        default:
            return false;
    }
    return true;
}
bool ContextClass::getStringVectorOrIgnore(vector<string> & result, const EngineInstr & instruction){
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                result.emplace_back(Values[0].getAnyValue());
                return true;
            }
            return false;
        case value_vec:
            for(const VariableModule & Variable : Values){
                result.emplace_back(Variable.getAnyValue());
            }
            return true;
        case pointer_inst:
            if(BasePointers.size() > 0){
                result.emplace_back(BasePointers[0].getString());
                return true;
            }
            return false;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                result.emplace_back(Pointer.getString());
            }
            return true;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                result.emplace_back(Modules.Variables[0]->getAnyValue());
                return true;
            }
            return false;
        case variable_mod_vec:
            for(VariableModule * Variable : Modules.Variables){
                result.emplace_back(Variable->getAnyValue());
            }
            return true;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getAllStrings(result);
                return true;
            }
            return false;
        case vector_mod_vec:
            for(VectorModule * Vector : Modules.Vectors){
                Vector->getAllStrings(result);
            }
            return true;
        default:
            return false;
    }
    return true;
}
ContextClass::ContextClass(){}
ContextClass::ContextClass(const ContextClass &Original){
    ID = Original.ID;
    type = Original.type;
    readOnly = Original.readOnly;
    isPointingToMember = Original.isPointingToMember;
    containerIndex = Original.containerIndex;
    switch(Original.type){
        case value_inst:
            if(Original.Values.size() == 0){
                return;
            }
            if(Values.size() == 0){
                Values.emplace_back(VariableModule());
            }
            Values[0] = Original.Values[0];
            break;
        case value_vec:
            Values = Original.Values;
            break;
        case pointer_inst:
            if(Original.BasePointers.size() == 0){
                return;
            }
            if(BasePointers.size() == 0){
                BasePointers.emplace_back(BasePointersStruct());
            }
            BasePointers[0] = Original.BasePointers[0];
            break;
        case pointer_vec:
            BasePointers = Original.BasePointers;
            break;
        case camera_inst:
            if(Original.Cameras.size() == 0){
                return;
            }
            if(Cameras.size() == 0){
                Cameras.emplace_back(nullptr);
            }
            Cameras[0] = Original.Cameras[0];
            break;
        case camera_vec:
            Cameras = Original.Cameras;
            break;
        case layer_inst:
            if(Original.Layers.size() == 0){
                return;
            }
            if(Layers.size() == 0){
                Layers.emplace_back(nullptr);
            }
            Layers[0] = Original.Layers[0];
            break;
        case layer_vec:
            Layers = Original.Layers;
            break;
        case object_inst:
            if(Original.Objects.size() == 0){
                return;
            }
            if(Objects.size() == 0){
                Objects.emplace_back(nullptr);
            }
            Objects[0] = Original.Objects[0];
            break;
        case object_vec:
            Objects = Original.Objects;
            break;
        case variable_mod:
            if(Original.Modules.Variables.size() == 0){
                return;
            }
            if(Modules.Variables.size() == 0){
                Modules.Variables.emplace_back(nullptr);
            }
            Modules.Variables[0] = Original.Modules.Variables[0];
            break;
        case variable_mod_vec:
            Modules.Variables = Original.Modules.Variables;
            break;
        case vector_mod:
            if(Original.Modules.Vectors.size() == 0){
                return;
            }
            if(Modules.Vectors.size() == 0){
                Modules.Vectors.emplace_back(nullptr);
            }
            Modules.Vectors[0] = Original.Modules.Vectors[0];
            break;
        case vector_mod_vec:
            Modules.Vectors = Original.Modules.Vectors;
            break;
        case super_text_mod:
            if(Original.Modules.SuperTexts.size() == 0){
                return;
            }
            if(Modules.SuperTexts.size() == 0){
                Modules.SuperTexts.emplace_back(nullptr);
            }
            Modules.SuperTexts[0] = Original.Modules.SuperTexts[0];
            break;
        case super_text_mod_vec:
            Modules.SuperTexts = Original.Modules.SuperTexts;
            break;
        case super_editable_text_mod:
            if(Original.Modules.SuperEditableTexts.size() == 0){
                return;
            }
            if(Modules.SuperEditableTexts.size() == 0){
                Modules.SuperEditableTexts.emplace_back(nullptr);
            }
            Modules.SuperEditableTexts[0] = Original.Modules.SuperEditableTexts[0];
            break;
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts = Original.Modules.SuperEditableTexts;
            break;
        case image_mod:
            if(Original.Modules.Images.size() == 0){
                return;
            }
            if(Modules.Images.size() == 0){
                Modules.Images.emplace_back(nullptr);
            }
            Modules.Images[0] = Original.Modules.Images[0];
            break;
        case image_mod_vec:
            Modules.Images = Original.Modules.Images;
            break;
        case movement_mod:
            if(Original.Modules.Movements.size() == 0){
                return;
            }
            if(Modules.Movements.size() == 0){
                Modules.Movements.emplace_back(nullptr);
            }
            Modules.Movements[0] = Original.Modules.Movements[0];
            break;
        case movement_mod_vec:
            Modules.Movements = Original.Modules.Movements;
            break;
        case collision_mod:
            if(Original.Modules.Collisions.size() == 0){
                return;
            }
            if(Modules.Collisions.size() == 0){
                Modules.Collisions.emplace_back(nullptr);
            }
            Modules.Collisions[0] = Original.Modules.Collisions[0];
            break;
        case collision_mod_vec:
            Modules.Collisions = Original.Modules.Collisions;
            break;
        case particles_mod:
            if(Original.Modules.Particles.size() == 0){
                return;
            }
            if(Modules.Particles.size() == 0){
                Modules.Particles.emplace_back(nullptr);
            }
            Modules.Particles[0] = Original.Modules.Particles[0];
            break;
        case particles_mod_vec:
            Modules.Particles = Original.Modules.Particles;
            break;
        case event_mod:
            if(Original.Modules.Events.size() == 0){
                return;
            }
            if(Modules.Events.size() == 0){
                Modules.Events.emplace_back(nullptr);
            }
            Modules.Events[0] = Original.Modules.Events[0];
            break;
        case event_mod_vec:
            Modules.Events = Original.Modules.Events;
            break;
        case scrollbar_mod:
            if(Original.Modules.Scrollbars.size() == 0){
                return;
            }
            if(Modules.Scrollbars.size() == 0){
                Modules.Scrollbars.emplace_back(nullptr);
            }
            Modules.Scrollbars[0] = Original.Modules.Scrollbars[0];
            break;
        case scrollbar_mod_vec:
            Modules.Scrollbars = Original.Modules.Scrollbars;
            break;
        case primitives_mod:
            if(Original.Modules.Primitives.size() == 0){
                return;
            }
            if(Modules.Primitives.size() == 0){
                Modules.Primitives.emplace_back(nullptr);
            }
            Modules.Primitives[0] = Original.Modules.Primitives[0];
            break;
        case primitives_mod_vec:
            Modules.Primitives = Original.Modules.Primitives;
            break;
        case null_dt:
            break;
        case any_dt:
            break;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": DataType code '" << type << "' is undefined.\n";
            type = null_dt;
            break;
    }
}
ContextClass &ContextClass::operator=(const ContextClass &Original){
    ID = Original.ID;
    type = Original.type;
    readOnly = Original.readOnly;
    isPointingToMember = Original.isPointingToMember;
    containerIndex = Original.containerIndex;
    switch(Original.type){
        case value_inst:
        case value_vec:
            Values = Original.Values;
            break;
        case pointer_inst:
        case pointer_vec:
            BasePointers = Original.BasePointers;
            break;
        case camera_inst:
        case camera_vec:
            Cameras = Original.Cameras;
            break;
        case layer_inst:
        case layer_vec:
            Layers = Original.Layers;
            break;
        case object_inst:
        case object_vec:
            Objects = Original.Objects;
            break;
        case variable_mod:
        case variable_mod_vec:
            Modules.Variables = Original.Modules.Variables;
            break;
        case vector_mod:
        case vector_mod_vec:
            Modules.Vectors = Original.Modules.Vectors;
            break;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts = Original.Modules.SuperTexts;
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts = Original.Modules.SuperEditableTexts;
            break;
        case image_mod:
        case image_mod_vec:
            Modules.Images = Original.Modules.Images;
            break;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements = Original.Modules.Movements;
            break;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions = Original.Modules.Collisions;
            break;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles = Original.Modules.Particles;
            break;
        case event_mod:
        case event_mod_vec:
            Modules.Events = Original.Modules.Events;
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars = Original.Modules.Scrollbars;
            break;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives = Original.Modules.Primitives;
            break;
        case null_dt:
        case any_dt:
            break;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Type '" << dataTypeToStr(Original.type) << "' is not valid.\n";
            type = null_dt;
            break;
    }
    return *this;
}
template<typename T>
void ContextClass::setFirstBasePointer(T * pointer){
    if(pointer == nullptr){
        return;
    }
    if(BasePointers.size() == 0){
        BasePointers.emplace_back(BasePointersStruct());
        type = pointer_inst;
    }
    BasePointers.back().setPointer(pointer);
}
void ContextClass::updateType(size_t vecSize){
    if(vecSize > 1){
        type = vectorizeEntityDataType(InstrDescription(), type);
    }
}
void ContextClass::addModule(SuperTextModule *Module){
    if(Module != nullptr){
        Modules.SuperTexts.push_back(Module);
        updateType(Modules.SuperTexts.size());
    }
}
void ContextClass::addModule(SuperEditableTextModule *Module){
    if(Module != nullptr){
        Modules.SuperEditableTexts.push_back(Module);
        updateType(Modules.SuperEditableTexts.size());
    }
}
void ContextClass::addModule(ImageModule *Module){
    if(Module != nullptr){
        Modules.Images.push_back(Module);
        updateType(Modules.Images.size());
        
    }
}
void ContextClass::addModule(MovementModule * Module){
    if(Module != nullptr){
        Modules.Movements.push_back(Module);
        updateType(Modules.Movements.size());
    }
}
void ContextClass::addModule(CollisionModule * Module){
    if(Module != nullptr){
        Modules.Collisions.push_back(Module);
        updateType(Modules.Collisions.size());
    }
}
void ContextClass::addModule(ParticleEffectModule * Module){
    if(Module != nullptr){
        Modules.Particles.push_back(Module);
        updateType(Modules.Particles.size());
    }
}
void ContextClass::addModule(EventModule * Module){
    if(Module != nullptr){
        Modules.Events.push_back(Module);
        updateType(Modules.Events.size());
    }
}
void ContextClass::addModule(VariableModule * Module){
    if(Module != nullptr){
        Modules.Variables.push_back(Module);
        updateType(Modules.Variables.size());
    }
}
void ContextClass::addModule(ScrollbarModule * Module){
    if(Module != nullptr){
        Modules.Scrollbars.push_back(Module);
        updateType(Modules.Scrollbars.size());
    }
}
void ContextClass::addModule(PrimitivesModule * Module){
    if(Module != nullptr){
        Modules.Primitives.push_back(Module);
        updateType(Modules.Primitives.size());
    }
}
void ContextClass::addModule(VectorModule * Module){
    if(Module != nullptr){
        Modules.Vectors.push_back(Module);
        updateType(Modules.Vectors.size());
    }
}

bool translateIndexToTreeRoots(const vector<ParameterStruct> & Parameters, unsigned index,
    unsigned & realIndex
){
    unsigned rootIndex = 0;
    for(realIndex = 0; realIndex < Parameters.size(); ++realIndex){
        if(Parameters[realIndex].treeLevel != 0){
            continue;
        }
        if(rootIndex == index){
            return false;
        }
        ++rootIndex;
    }
    return true;
}

ContextClass * getVariableByAddress(const InstrDescription & CurrentInstr,
    MemoryMapType & MemoryMap, const DynamicVariableInfo & Variable,
    const string & variableName, const bool & printError
){
    if(Variable.dynamicAddress > MemoryMap.size()){
        if(printError){
            if(!variableName.empty()){
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Address " << Variable.dynamicAddress <<" is not allocated.\n";
            }
            else{
                cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Variable '" << variableName << "' does not exist at the address: "
                    << Variable.dynamicAddress <<".\n";
            }
        }
        return nullptr;
    }
    return &MemoryMap[Variable.dynamicAddress];
    
    // if(Variable.isReference){
    //     if(!ObjectMemory.ReferencesMap.contains(Variable.dynamicAddress)){
    //         printError && cerr << instructionError(CurrentInstr, __FUNCTION__) << "Reference '" << variableName << "' does not exist.\n";
    //         return nullptr;
    //     }
    //     if(!ObjectMemory.MemoryMap.contains(ObjectMemory.ReferencesMap[Variable.dynamicAddress].realAddress)){
    //         printError && cerr << instructionError(CurrentInstr, __FUNCTION__)
    //             << "Variable '" << ObjectMemory.ReferencesMap[Variable.dynamicAddress].name
    //             << "' from reference '" << variableName << "' does not exist.\n";
    //         return nullptr;
    //     }
    //     return &ObjectMemory.MemoryMap[ObjectMemory.ReferencesMap[Variable.dynamicAddress].realAddress];
    // }
    // if(!ObjectMemory.MemoryMap.contains(Variable.dynamicAddress)){
    //     printError && cerr << instructionError(CurrentInstr, __FUNCTION__) << "Variable '" << variableName << "' does not exist.\n";
    //     return nullptr;
    // }
    // return &ObjectMemory.MemoryMap[Variable.dynamicAddress];
}

bool ContextClass::copyFromTheParameter(
    std::vector<ContextClass> & MemoryMap, const vector<DynamicVariableInfo> & EventLocalVariables,
    const InstrDescription & CurrentInstr, const vector<ParameterStruct> & Parameters,
    bool & negateAfterCopy, unsigned index, bool printErrors
){
    clear();
    unsigned realIndex = 0;
    if(translateIndexToTreeRoots(Parameters, index, realIndex)){
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " does not exist.\n";
        return true;
    }
    const ParameterStruct & CurrentParameter = Parameters[realIndex];
    if(CurrentParameter.type == 'e'){
        return false;
    }
    if(CurrentParameter.type == 'c'){
        if(EventLocalVariables.empty()){
            cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Local variables container is empty. This may cause undefined behavior.\n";
            return true;
        }
        ContextClass * TempContext = getVariableByAddress(CurrentInstr, MemoryMap,
            EventLocalVariables[CurrentParameter.localAddress],
            CurrentParameter.variableID, printErrors
        );
        if(TempContext == nullptr){ 
            printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                << "Variable '" << CurrentParameter.variableID << "' from the parameter "
                << index + 2 << " does not exist.\n";
            return true;
        }
        *this = *TempContext;
        //Can't negate a value in a possible pointer since it's not temporary.
        //Also remember that lvalues/outputs can't be negated from obvious reasons.
        negateAfterCopy = CurrentParameter.negateVariable;
    }
    else if(CurrentParameter.type == 'l'){
        type = value_inst;
        Values.push_back(CurrentParameter.Literal);
        if(CurrentParameter.negateVariable)
            Values.back().negate();
    }
    else if(CurrentParameter.type == 'v'){
        type = value_vec;
        for(++realIndex; realIndex < Parameters.size(); ++realIndex){
            const ParameterStruct & parameterIt = Parameters[realIndex];
            if(parameterIt.treeLevel != 1){
                return false;
            }
            if(parameterIt.type == 'l'){
                Values.push_back(parameterIt.Literal);
                if(parameterIt.negateVariable)
                    Values.back().negate();
                continue;
            }
            if(parameterIt.type != 'c'){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "In the parameter " << index+2 << ": Value with index " << realIndex << " is of '" << parameterIt.type << "' type.\n";
                return true;
            }

            ContextClass * TempContext = getVariableByAddress(CurrentInstr, MemoryMap,
                EventLocalVariables[parameterIt.localAddress],
                parameterIt.variableID, printErrors
            );
            if(TempContext == nullptr){
                printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                    << "Variable '" << parameterIt.variableID
                    << "' from the parameter " << index+2 << " does not exist.\n";
                return true;
            }
            switch(TempContext->type){
                case value_inst:
                    if(TempContext->Values.size() > 0){
                        Values.push_back(TempContext->Values[0]);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case value_vec:
                    if(!parameterIt.negateVariable){
                        Values.insert(Values.end(), TempContext->Values.begin(),
                            TempContext->Values.end()
                        );
                    }
                    else{
                        for(const VariableModule & var : TempContext->Values){
                            Values.push_back(var);
                            Values.back().negate();
                        }
                    }
                    break;
                case pointer_inst:
                    if(!TempContext->BasePointers.empty()){
                        Values.emplace_back(VariableModule());
                        Values.back().move(&TempContext->BasePointers[0], CurrentInstr.instruction, CurrentInstr);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case pointer_vec:
                    for(const BasePointersStruct & Pointer : TempContext->BasePointers){
                        Values.emplace_back(VariableModule());
                        Values.back().move(&Pointer, CurrentInstr.instruction, CurrentInstr);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case variable_mod:
                    if(TempContext->Modules.Variables.size() > 0){
                        Values.emplace_back(VariableModule());
                        Values.back().copyValue(TempContext->Modules.Variables[0]);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case variable_mod_vec:
                    for(const VariableModule * Variable : TempContext->Modules.Variables){
                        Values.emplace_back(VariableModule());
                        Values.back().copyValue(Variable);
                        if(parameterIt.negateVariable)
                            Values.back().negate();
                    }
                    break;
                case vector_mod:
                    for(VectorModule * Vector: TempContext->Modules.Vectors){
                        Vector->getValuesIntoContext(Values, parameterIt.negateVariable);
                    }
                    break;
                default:
                    printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
                        << "Context with ID '" << parameterIt.variableID << "' found in the parameter "
                        << index << " is of the invalid type: '" << dataTypeToStr(TempContext->type)
                        << "'. Function expects only numeric and string values.\n";
                    return true;
            }
        }
    }
    else{
        printErrors && cerr << instructionError(CurrentInstr, __FUNCTION__)
            << "Parameter " << index+2 << " has invalid type: '" << CurrentParameter.type << "'.\n";
        return true;
    }
    return false;
}

void ContextClass::copyOnlyCurrentType(const ContextClass *Original){
    type = Original->type;
    switch(Original->type){
        case value_inst:
            if(Original->Values.empty()){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(Values.empty()){
                Values.push_back(Original->Values[0]);
            }
            else{
                Values[0] = Original->Values[0];
            }
            break;
        case value_vec:
            Values = Original->Values;
        case pointer_inst:
            if(Original->BasePointers.size() == 0){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(BasePointers.size() == 0){
                BasePointers.emplace_back(BasePointersStruct());
            }
            BasePointers[0] = Original->BasePointers[0];
            break;
        case pointer_vec:
            BasePointers = Original->BasePointers;
        case variable_mod:
            if(Original->Modules.Variables.size() == 0){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(Modules.Variables.size() == 0){
                Modules.Variables.emplace_back(nullptr);
            }
            Modules.Variables[0] = Original->Modules.Variables[0];
            break;
        case variable_mod_vec:
            Modules.Variables = Original->Modules.Variables;
        case vector_mod:
            if(Original->Modules.Vectors.size() == 0){
                cerr << "Error: In " << __FUNCTION__ << ": For the context '"
                    << Original->ID << "' of the type '" << dataTypeToStr(Original->type)
                    << "': Container is empty.\n";
                return;
            }
            if(Modules.Vectors.size() == 0){
                Modules.Vectors.emplace_back(nullptr);
            }
            Modules.Vectors[0] = Original->Modules.Vectors[0];
            break;
        case vector_mod_vec:
            Modules.Vectors = Original->Modules.Vectors;
        case camera_inst:
        case camera_vec:
            Cameras = Original->Cameras;
            break;
        case layer_inst:
        case layer_vec:
            Layers = Original->Layers;
            break;
        case object_inst:
        case object_vec:
            Objects = Original->Objects;
            break;
        case super_text_mod:
        case super_text_mod_vec:
            Modules.SuperTexts = Original->Modules.SuperTexts;
            break;
        case super_editable_text_mod:
        case super_editable_text_mod_vec:
            Modules.SuperEditableTexts = Original->Modules.SuperEditableTexts;
            break;
        case image_mod:
        case image_mod_vec:
            Modules.Images = Original->Modules.Images;
            break;
        case movement_mod:
        case movement_mod_vec:
            Modules.Movements = Original->Modules.Movements;
            break;
        case collision_mod:
        case collision_mod_vec:
            Modules.Collisions = Original->Modules.Collisions;
            break;
        case particles_mod:
        case particles_mod_vec:
            Modules.Particles = Original->Modules.Particles;
            break;
        case event_mod:
        case event_mod_vec:
            Modules.Events = Original->Modules.Events;
            break;
        case scrollbar_mod:
        case scrollbar_mod_vec:
            Modules.Scrollbars = Original->Modules.Scrollbars;
            break;
        case primitives_mod:
        case primitives_mod_vec:
            Modules.Primitives = Original->Modules.Primitives;
            break;
        default:
            type = null_dt;
    }
}

void ContextClass::leaveOneRandomBasePointer(){
    if(BasePointers.size() == 0){
        return;
    }
    BasePointersStruct randomPointer = BasePointers[rand() % BasePointers.size()];
    BasePointers.clear();
    BasePointers.push_back(randomPointer);
}
void ContextClass::printOutObjects(){
    if(Objects.size() > 1){
        cout << " [";
        for(AncestorObject * Object : Objects){
            cout << Object->getID() << ", ";
        }
        cout << "] ";
    }
    else if(Objects.size() == 1){
        cout << Objects.back()->getID() << " ";
    }
}
unsigned ContextClass::size() const{
    return Cameras.size() + Layers.size() + Objects.size() + Modules.size() + Values.size() + BasePointers.size();
}
bool ContextClass::empty() const{
    return Cameras.empty() && Layers.empty() && Objects.empty() && Modules.empty() && Values.empty() && BasePointers.empty();
}
ReturnType ContextClass::getAllValues(vector<VariableModule> & NewValues, bool negate) const {
    if(negate){
        return getAllNegatedValues(NewValues);
    }
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Values[0]);
            }
            break;
        case value_vec:
            NewValues.insert(NewValues.end(), Values.begin(), Values.end());
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                NewValues.emplace_back();
                NewValues.back().setValueFromPointer(BasePointers[0]);
            }
            break;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                NewValues.emplace_back(VariableModule());
                NewValues.back().setValueFromPointer(Pointer);
            }
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            for(const VariableModule * Variable : Modules.Variables){
                NewValues.push_back(*Variable);
            }
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(NewValues);
            }
            break;
        case vector_mod_vec:
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(NewValues);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getAllNegatedValues(vector<VariableModule> & NewValues) const {
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Values[0]);
                NewValues.back().negate();
            }
            break;
        case value_vec:
            for(const VariableModule & var : Values){
                NewValues.push_back(var);
                NewValues.back().negate();
            }
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                NewValues.emplace_back();
                NewValues.back().setValueFromPointer(BasePointers[0]);
                NewValues.back().negate();
            }
            break;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                NewValues.emplace_back(VariableModule());
                NewValues.back().setValueFromPointer(Pointer);
                NewValues.back().negate();
            }
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Modules.Variables[0]);
                NewValues.back().negate();
            }
            break;
        case variable_mod_vec:
            for(const VariableModule * Variable : Modules.Variables){
                NewValues.push_back(*Variable);
                NewValues.back().negate();
            }
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(NewValues, true);
            }
            break;
        case vector_mod_vec:
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(NewValues, true);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::moveAllValues(vector<VariableModule> & NewValues, bool negate, unsigned realIndex) const {
    if(negate){
        return getAllNegatedValues(NewValues);
    }
    switch(type){
        case value_inst:
            if(Values.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Values[0]);
            }
            break;
        case value_vec:
            NewValues.insert(NewValues.end(), Values.begin(), Values.end());
            break;
        case pointer_inst:
            if(BasePointers.size() > 0){
                NewValues.emplace_back();
                NewValues.back().setValueFromPointer(BasePointers[0]);
            }
            break;
        case pointer_vec:
            for(const BasePointersStruct & Pointer : BasePointers){
                NewValues.emplace_back(VariableModule());
                NewValues.back().setValueFromPointer(Pointer);
            }
            break;
        case variable_mod:
            if(Modules.Variables.size() > 0){
                NewValues.emplace_back();
                NewValues.back().copyValue(Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            for(const VariableModule * Variable : Modules.Variables){
                NewValues.push_back(*Variable);
            }
            break;
        case vector_mod:
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(NewValues);
            }
            break;
        case vector_mod_vec:
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(NewValues);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getDoubleValues(vector<double> & NewValues, bool negate) const {
    if(negate){
        return getNegatedDoubleValues(NewValues);
    }
    switch(type){
        case value_inst:
        case value_vec:
            for(const VariableModule & val : Values){
                NewValues.push_back(val.getDouble());
            }
            break;
        case pointer_inst:
        case pointer_vec:
            for(const BasePointersStruct & ptr : BasePointers){
                NewValues.push_back(ptr.getDouble());
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            for(const VariableModule * var : Modules.Variables){
                NewValues.push_back(var->getDouble());
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            for(const VectorModule * vec : Modules.Vectors){
                vec->getDoubles(NewValues);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getNegatedDoubleValues(vector<double> & NewValues) const {
    switch(type){
        case value_inst:
        case value_vec:
            for(const VariableModule & val : Values){
                NewValues.push_back(-val.getDouble());
            }
            break;
        case pointer_inst:
        case pointer_vec:
            for(const BasePointersStruct & ptr : BasePointers){
                NewValues.push_back(-ptr.getDouble());
            }
            break;
        case variable_mod:
        case variable_mod_vec:
            for(const VariableModule * var : Modules.Variables){
                NewValues.push_back(-var->getDouble());
            }
            break;
        case vector_mod:
        case vector_mod_vec:
            for(const VectorModule * vec : Modules.Vectors){
                vec->getDoubles(NewValues, true);
            }
            break;
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::getValue(VariableModule & NewValue){
    NewValue.clear();
    switch(type){
        case value_inst:
        case value_vec:
            if(Values.size() == 0){
                return ReturnType::EMPTY;
            }
            NewValue.copyValue(Values[0]);
            return ReturnType::OK;
        case pointer_inst:
        case pointer_vec:
            if(BasePointers.size() == 0){
                return ReturnType::EMPTY;
            }
            NewValue.setValueFromPointer(BasePointers[0]);
            break;
        case variable_mod:
        case variable_mod_vec:
            if(Modules.Variables.size() == 0){
                return ReturnType::EMPTY;
            }
            NewValue.copyValue(Modules.Variables[0]);
            return ReturnType::OK;
        case vector_mod:
        case vector_mod_vec:{
            if(Modules.Vectors.size() == 0){
                return ReturnType::EMPTY;
            }
            if(Modules.Vectors[0]->getSize() == 0){
                return ReturnType::EMPTY;
            }
            if(Modules.Vectors[0]->setVariableWithFirstValue(NewValue)){
                return ReturnType::EMPTY;
            }
            return ReturnType::OK;
        }
        default:
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}
ReturnType ContextClass::collapseToValue(){
    switch(type){
        case value_inst:
        case value_vec:
            return ReturnType::OK;
        case pointer_inst:
            type = DataType::value_inst;
            Values.clear();
            Values.emplace_back();
            if(!BasePointers.empty()){
                Values.back().setValueFromPointer(BasePointers[0]);
            }
            break;
        case pointer_vec:
            type = DataType::value_vec;
            Values.clear();
            for(const BasePointersStruct & Pointer : BasePointers){
                Values.emplace_back();
                Values.back().setValueFromPointer(Pointer);
            }
            break;
        case variable_mod:
            type = DataType::value_inst;
            Values.clear();
            Values.emplace_back();
            if(Modules.Variables.size() > 0){
                Values.back().copyValue(Modules.Variables[0]);
            }
            break;
        case variable_mod_vec:
            type = DataType::value_vec;
            Values.clear();
            for(const VariableModule * Variable : Modules.Variables){
                Values.push_back(*Variable);
            }
            break;
        case vector_mod:
            type = DataType::value_inst;
            Values.clear();
            if(Modules.Vectors.size() > 0){
                Modules.Vectors[0]->getValuesIntoContext(Values);
            }
            break;
        case vector_mod_vec:
            type = DataType::value_vec;
            Values.clear();
            for(const VectorModule * Vector : Modules.Vectors){
                Vector->getValuesIntoContext(Values, true);
            }
            break;
        default:
            type = DataType::value_inst;
            return ReturnType::INVALID_TYPE;
    }
    return ReturnType::OK;
}