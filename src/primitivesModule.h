#ifndef PRIMITIVESMODULE_H_INCLUDED
#define PRIMITIVESMODULE_H_INCLUDED

#include "camera.h"

enum PrimitiveShapeType : char{
    prim_null, prim_line, prim_triangle, prim_filled_triangle, prim_rectangle, prim_filled_rectangle,
    prim_rounded_rectangle, prim_filled_rounded_rectangle, prim_circle, prim_filled_circle,
    prim_ellipse, prim_filled_ellipse
};

PrimitiveShapeType getPrimitiveType(string type);
string transPrimitiveTypeToString(PrimitiveShapeType type);

class PrimitivesModule : public PrimaryModule{
    private:

    public:
    PrimitiveShapeType type;
    vector<vec2d> points;
    ALLEGRO_COLOR color;
    float thickness;
    float radius;

    void setUpNewInstance();
    PrimitivesModule();
    PrimitivesModule(string newAlias, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~PrimitivesModule();
    void clear();
    void clone(const PrimitivesModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, bool changeOldID);
    void draw(vec2d base, Camera2D Camera, bool outSourcing) const;
    void updateWithSize();
    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
};

#endif // PRIMITIVESMODULE_H_INCLUDED