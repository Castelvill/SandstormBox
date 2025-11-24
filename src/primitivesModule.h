#ifndef PRIMITIVESMODULE_H_INCLUDED
#define PRIMITIVESMODULE_H_INCLUDED

#include "camera.h"

enum PrimitiveShapeType : char{
    prim_null, prim_line, prim_triangle, prim_filled_triangle, prim_rectangle, prim_filled_rectangle,
    prim_rounded_rectangle, prim_filled_rounded_rectangle, prim_circle, prim_filled_circle,
    prim_ellipse, prim_filled_ellipse, prim_polygon, prim_filled_polygon
};

PrimitiveShapeType getPrimitiveType(const string & type);
string transPrimitiveTypeToString(PrimitiveShapeType type);

class PrimitivesModule : public PrimaryModule{
    private:

    public:
    PrimitiveShapeType type;
    vector<vec2d> points;
    ALLEGRO_COLOR color;
    float thickness;
    float radius; //radius for some primitives, miter_limit for polygons
    ALLEGRO_LINE_JOIN lineJoinType = ALLEGRO_LINE_JOIN::ALLEGRO_LINE_JOIN_NONE;

    void setUpNewInstance();
    PrimitivesModule(size_t & topModuleUniqueIndex);
    PrimitivesModule(PrimaryData & initData);
    ~PrimitivesModule();
    void clear();
    void clone(const PrimitivesModule &Original, PrimaryData & initData, bool changeOldID);
    void draw(vec2d base, Camera2D Camera, bool outSourcing) const;
    void updateWithSize();
    void initPrimitiveByType();
    void getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers);
    void setLineJoinType(int type);
};

#endif // PRIMITIVESMODULE_H_INCLUDED