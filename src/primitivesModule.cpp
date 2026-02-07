#include "primitivesModule.h"

void PrimitivesModule::setUpNewInstance(){
    type = prim_null;
    color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
    thickness = 1.0;
    radius = 1.0;
}
PrimitivesModule::PrimitivesModule(size_t & topModuleUniqueIndex){
    primaryConstructor(topModuleUniqueIndex);
    setUpNewInstance();
}
PrimitivesModule::PrimitivesModule(PrimaryData & initData){
    primaryConstructor(initData);
    setUpNewInstance();
}
PrimitivesModule::~PrimitivesModule(){

}
void PrimitivesModule::clear(){
    points.clear();
}
void PrimitivesModule::clone(const PrimitivesModule &Original, PrimaryData & initData,
    bool changeOldID
){
    size_t oldIndex = getUniqueIndex();
    string oldID = ID;
    *this = Original;
    setUniqueIndex(oldIndex);
    ID = oldID;

    setObjectUniqueIndex(initData.objectUniqueIndex);
    setLayerUniqueIndex(initData.layerUniqueIndex);
    
    initData.newID = Original.getID(); 
    setAllIDs(initData, changeOldID);
}
void PrimitivesModule::draw(vec2d base, Camera2D Camera, bool outSourcing) const{
    base.translate(pos);
    switch(type){
        case prim_line:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Line primitive requires 2 points.\n";
                return;
            }
            al_draw_line(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y, color, thickness);
            return;
        case prim_triangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Triangle primitive requires 3 points.\n";
                return;
            }
            al_draw_triangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y,
                base.x + points[2].x, base.y + points[2].y, color, thickness
            );
            return;
        case prim_filled_triangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Triangle primitive requires two points.\n";
                return;
            }
            al_draw_filled_triangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y,
                base.x + points[2].x, base.y + points[2].y, color
            );
            return;
        case prim_rectangle:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Rectangle primitive requires 2 points.\n";
                return;
            }
            al_draw_rectangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y, color, thickness);
            return;
        case prim_filled_rectangle:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Rectangle primitive requires 2 points.\n";
                return;
            }
            al_draw_filled_rectangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y, color);
            return;
        case prim_rounded_rectangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Rounded rectangle primitive requires 3 points.\n";
                return;
            }
            al_draw_rounded_rectangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y,
                points[2].x, points[2].y, color, thickness
            );
            return;
        case prim_filled_rounded_rectangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Rounded rectangle primitive requires 3 points.\n";
                return;
            }
            al_draw_filled_rounded_rectangle(base.x + points[0].x, base.y + points[0].y,
                base.x + points[1].x, base.y + points[1].y, points[2].x, points[2].y, color
            );
            return;
        case prim_circle:
            if(points.size() < 1){
                cerr << "Error: In " << __FUNCTION__ << ": Circle primitive requires 1 point.\n";
                return;
            }
            al_draw_circle(base.x + points[0].x, base.y + points[0].y, radius, color, thickness);
            return;
        case prim_filled_circle:
            if(points.size() < 1){
                cerr << "Error: In " << __FUNCTION__ << ": Circle primitive requires 1 points.\n";
                return;
            }
            al_draw_filled_circle(base.x + points[0].x, base.y + points[0].y, radius, color);
            return;
        case prim_ellipse:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Ellipse primitive requires 2 points.\n";
                return;
            }
            al_draw_ellipse(base.x + points[0].x, base.y + points[0].y, points[1].x, points[1].y, color, thickness);
            return;
        case prim_filled_ellipse:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Ellipse primitive requires 2 points.\n";
                return;
            }
            al_draw_filled_ellipse(base.x + points[0].x, base.y + points[0].y, points[1].x, points[1].y, color);
            return;
        case prim_polygon:{
            if(points.empty()){
                cerr << "Error: In " << __FUNCTION__
                    << ": Polygon primitive requires at least 1 point.\n";
                return;
            }
            float vertices[points.size()*2];
            for(size_t v = 0; v < points.size(); ++v){
                vertices[v*2] = base.x + points[v].x;
                vertices[v*2+1] = base.y + points[v].y;
            }
            al_draw_polygon(vertices, points.size(), lineJoinType, color, thickness,
                radius
            );}
            return;
        case prim_filled_polygon:{
            if(points.empty()){
                cerr << "Error: In " << __FUNCTION__
                    << ": Polygon primitive requires at least 1 point.\n";
                return;
            }
            float vertices[points.size()*2];
            for(size_t v = 0; v < points.size(); ++v){
                vertices[v*2] = base.x + points[points.size() - (v + 1)].x;
                vertices[v*2+1] = base.y + points[points.size() - (v + 1)].y;
            }
            al_draw_filled_polygon(vertices, points.size(), color);}
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Cannot draw primitive of '"
                << transPrimitiveTypeToString(type) << "' type.\n";
            return;
    }
}
void PrimitivesModule::updateWithSize(){
    if(points.size() == 0){
        return;
    }
    switch(type){
        case prim_line:
        case prim_rectangle:
        case prim_filled_rectangle:
            if(points.size() == 1){
                points.emplace_back(vec2d());
            }
            points[1].x = points[0].x + size.x;
            points[1].y = points[0].y + size.y;
            return;
        case prim_rounded_rectangle:
        case prim_filled_rounded_rectangle:
            if(points.size() == 1){
                points.emplace_back(vec2d());
            }
            points[1].x = points[0].x + size.x;
            points[1].y = points[0].y + size.y;
            if(points.size() == 2){
                points.emplace_back(vec2d());
            }
            return;
        case prim_circle:
        case prim_filled_circle:
            radius = std::min(size.x / 2, size.y / 2);
            return;
        case prim_ellipse:
        case prim_filled_ellipse:
            if(points.size() == 1){
                points.emplace_back(vec2d());
            }
            points[1].x = size.x / 2;
            points[1].y = size.y / 2;
            return;
        case prim_triangle:
        case prim_filled_triangle:
            cerr << "Error: In " << __FUNCTION__
                << ": Cannot update the size of triangle primitive.\n";
            return;
        case prim_polygon:
        case prim_filled_polygon:
            cerr << "Error: In " << __FUNCTION__
                << ": Cannot update the size of polygon primitive.\n";
            return;
        default:
            return;
    }
}
void PrimitivesModule::initPrimitiveByType(){
    points.clear();
    switch(type){
        case prim_line:
            points = {vec2d(), vec2d()};
            break;
        case prim_triangle:
            points = {vec2d(), vec2d(), vec2d()};
            break;
        case prim_filled_triangle:
            points = {vec2d(), vec2d(), vec2d()};
            break;
        case prim_rectangle:
            points = {vec2d(), vec2d()};
            break;
        case prim_filled_rectangle:
            points = {vec2d(), vec2d()};
            break;
        case prim_rounded_rectangle:
            points = {vec2d(), vec2d(), vec2d()};
            break;
        case prim_filled_rounded_rectangle:
            points = {vec2d(), vec2d(), vec2d()};
            break;
        case prim_circle:
            points = {vec2d()};
            break;
        case prim_filled_circle:
            points = {vec2d()};
            break;
        case prim_ellipse:
            points = {vec2d(), vec2d()};
            break;
        case prim_filled_ellipse:
            points = {vec2d(), vec2d()};
        case prim_polygon:
        case prim_filled_polygon:
            return;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Primitive of '"
                << transPrimitiveTypeToString(type) << "' type does not exist.\n";
            break;
    }
}
void PrimitivesModule::getContext(AttributeType attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.emplace_back(BasePointersStruct());
    if(attribute == AttributeType::thickness){
        BasePointers.back().setPointer(&thickness);
    }
    else{
        BasePointers.pop_back();
        getPrimaryContext(attribute, BasePointers);
    }
}
void PrimitivesModule::setLineJoinType(int type){
    switch(type){
        case 0:
            lineJoinType = ALLEGRO_LINE_JOIN::ALLEGRO_LINE_JOIN_NONE;
            return;
        case 1:
            lineJoinType = ALLEGRO_LINE_JOIN::ALLEGRO_LINE_JOIN_BEVEL;
            return;
        case 2:
            lineJoinType = ALLEGRO_LINE_JOIN::ALLEGRO_LINE_JOIN_ROUND;
            return;
        case 3:
            lineJoinType = ALLEGRO_LINE_JOIN::ALLEGRO_LINE_JOIN_MITER;
            return;
        default:
            cerr << "Error: Line join type with number " << type << " is not valid. "
                << "Choose type from the following:\n"
                << " - 0 - no line joining,\n"
                << " - 1 - bevel line joining,\n"
                << " - 2 - round line joining,\n"
                << " - 3 - miter line joining.\n";
            lineJoinType = ALLEGRO_LINE_JOIN::ALLEGRO_LINE_JOIN_NONE;
            return;
    }
}
PrimitiveShapeType getPrimitiveType(const string & type){
    if(type == "line"){
        return prim_line;
    }
    else if(type == "triangle"){
        return prim_triangle;
    }
    else if(type == "filled_triangle"){
        return prim_filled_triangle;
    }
    else if(type == "rectangle"){
        return prim_rectangle;
    }
    else if(type == "filled_rectangle"){
        return prim_filled_rectangle;
    }
    else if(type == "rounded_rectangle"){
        return prim_rounded_rectangle;
    }
    else if(type == "filled_rounded_rectangle"){
        return prim_filled_rounded_rectangle;
    }
    else if(type == "circle"){
        return prim_circle;
    }
    else if(type == "filled_circle"){
        return prim_filled_circle;
    }
    else if(type == "ellipse"){
        return prim_ellipse;
    }
    else if(type == "filled_ellipse"){
        return prim_filled_ellipse;
    }
    else if(type == "polygon"){
        return prim_polygon;
    }
    else if(type == "filled_polygon"){
        return prim_filled_polygon;
    }
    else{
        return prim_null;
    }
}

string transPrimitiveTypeToString(PrimitiveShapeType type){
    switch(type){
        case prim_null:
            return "null";
        case prim_line:
            return "line";
        case prim_triangle:
            return "triangle";
        case prim_filled_triangle:
            return "filled_triangle";
        case prim_rectangle:
            return "rectangle";
        case prim_filled_rectangle:
            return "filled_rectangle";
        case prim_rounded_rectangle:
            return "rounded_rectangle";
        case prim_filled_rounded_rectangle:
            return "filled_rounded_rectangle";
        case prim_circle:
            return "circle";
        case prim_filled_circle:
            return "filled_circle";
        case prim_ellipse:
            return "ellipse";
        case prim_filled_ellipse:
            return "filled_ellipse";
        default:
            cerr << "Error: In " << __FUNCTION__
                << ": PrimitiveType with code: '" << type << "' is undefined.\n"; 
            return "undefined";
    }
    return "null";
}

void draw_vertical_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR top, ALLEGRO_COLOR bottom) {
    ALLEGRO_VERTEX v[] = {
        {.x = x, .y = y, .z = 0, .color = top},
        {.x = x + w, .y = y, .z = 0, .color = top},
        {.x = x, .y = y + h, .z = 0, .color = bottom},
        {.x = x + w, .y = y + h, .z = 0, .color = bottom}};
    al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
}
void draw_horizontal_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR left, ALLEGRO_COLOR right) {
    ALLEGRO_VERTEX v[] = {
        {.x = x, .y = y, .z = 0, .color = left},
        {.x = x + w, .y = y, .z = 0, .color = right},
        {.x = x, .y = y + h, .z = 0, .color = left},
        {.x = x + w, .y = y + h, .z = 0, .color = right}};
    al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
}