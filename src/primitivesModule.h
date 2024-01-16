#ifndef PRIMITIVESMODULE_H_INCLUDED
#define PRIMITIVESMODULE_H_INCLUDED

#include "camera.h"

enum PrimitiveType : char{
    prim_null, prim_line, prim_triangle, prim_filled_triangle, prim_rectangle, prim_filled_rectangle,
    prim_rounded_rectangle, prim_filled_rounded_rectangle, prim_circle, prim_filled_circle,
    prim_ellipse, prim_filled_ellipse
};

PrimitiveType getPrimitiveType(string type);
string translatePrimitiveType(PrimitiveType type);

class PrimitivesModule : public PrimaryModule{
    private:

    public:
    PrimitiveType type;
    vector<vec2d> points;
    ALLEGRO_COLOR color;
    float thickness;
    float radius;
    int usedBitmapLayer;

    void setUpNewInstance();
    PrimitivesModule();
    PrimitivesModule(string newAlias, vector<string> * listOfIDs, string newLayerID, string newObjectID);
    ~PrimitivesModule();
    void clear();
    void clone(const PrimitivesModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, bool changeOldID);
    void draw(vec2d base, Camera2D Camera, bool outSourcing);
    void updateWithSize();
    void getContext(string attribute, vector <BasePointersStruct> & BasePointers);
    int getUsedBitmapLayer() const;
};

#endif // PRIMITIVESMODULE_H_INCLUDED