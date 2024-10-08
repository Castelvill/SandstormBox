#include "primitivesModule.h"

void PrimitivesModule::setUpNewInstance(){
    type = prim_null;
    color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
    thickness = 1.0;
    radius = 1.0;
}
PrimitivesModule::PrimitivesModule(){
    setUpNewInstance();
}
PrimitivesModule::PrimitivesModule(string newAlias, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newAlias, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
}
PrimitivesModule::~PrimitivesModule(){

}
void PrimitivesModule::clear(){
    points.clear();
}
void PrimitivesModule::clone(const PrimitivesModule &Original, vector<string> &listOfIDs, string newLayerID, string newObjectID, bool changeOldID){
    string oldID = ID;
    *this = Original;
    ID = oldID;
    setAllIDs(Original.getID(), listOfIDs, newLayerID, newObjectID, changeOldID);
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
            break;
        case prim_triangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Triangle primitive requires 3 points.\n";
                return;
            }
            al_draw_triangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y,
                base.x + points[2].x, base.y + points[2].y, color, thickness
            );
            break;
        case prim_filled_triangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Triangle primitive requires two points.\n";
                return;
            }
            al_draw_filled_triangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y,
                base.x + points[2].x, base.y + points[2].y, color
            );
            break;
        case prim_rectangle:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Rectangle primitive requires 2 points.\n";
                return;
            }
            al_draw_rectangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y, color, thickness);
            break;
        case prim_filled_rectangle:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Rectangle primitive requires 2 points.\n";
                return;
            }
            al_draw_filled_rectangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y, color);
            break;
        case prim_rounded_rectangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Rounded rectangle primitive requires 3 points.\n";
                return;
            }
            al_draw_rounded_rectangle(base.x + points[0].x, base.y + points[0].y, base.x + points[1].x, base.y + points[1].y,
                points[2].x, points[2].y, color, thickness
            );
            break;
        case prim_filled_rounded_rectangle:
            if(points.size() < 3){
                cerr << "Error: In " << __FUNCTION__ << ": Rounded rectangle primitive requires 3 points.\n";
                return;
            }
            al_draw_filled_rounded_rectangle(base.x + points[0].x, base.y + points[0].y,
                base.x + points[1].x, base.y + points[1].y, points[2].x, points[2].y, color
            );
            break;
        case prim_circle:
            if(points.size() < 1){
                cerr << "Error: In " << __FUNCTION__ << ": Circle primitive requires 1 point.\n";
                return;
            }
            al_draw_circle(base.x + points[0].x, base.y + points[0].y, radius, color, thickness);
            break;
        case prim_filled_circle:
            if(points.size() < 1){
                cerr << "Error: In " << __FUNCTION__ << ": Circle primitive requires 1 points.\n";
                return;
            }
            al_draw_filled_circle(base.x + points[0].x, base.y + points[0].y, radius, color);
            break;
        case prim_ellipse:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Ellipse primitive requires 2 points.\n";
                return;
            }
            al_draw_ellipse(base.x + points[0].x, base.y + points[0].y, points[1].x, points[1].y, color, thickness);
            break;
        case prim_filled_ellipse:
            if(points.size() < 2){
                cerr << "Error: In " << __FUNCTION__ << ": Ellipse primitive requires 2 points.\n";
                return;
            }
            al_draw_filled_ellipse(base.x + points[0].x, base.y + points[0].y, points[1].x, points[1].y, color);
            break;
        default:
            cerr << "Error: In " << __FUNCTION__ << ": Cannot draw primitive.\n";
            break;
    }
}
void PrimitivesModule::updateWithSize(){
    if(points.size() == 0){
        return;
    }
    if(type == prim_line || type == prim_rectangle || type == prim_filled_rectangle){
        if(points.size() == 1){
            points.push_back(vec2d());
        }
        points[1].x = points[0].x + size.x;
        points[1].y = points[0].y + size.y;
    }
    else if(type == prim_rounded_rectangle || type == prim_filled_rounded_rectangle){
        if(points.size() == 1){
            points.push_back(vec2d());
        }
        points[1].x = points[0].x + size.x;
        points[1].y = points[0].y + size.y;
        if(points.size() == 2){
            points.push_back(vec2d());
        }
    }
    else if(type == prim_circle || type == prim_filled_circle){
        radius = std::min(size.x / 2, size.y / 2);
    }
    else if(type == prim_ellipse || type == prim_filled_ellipse){
        if(points.size() == 1){
            points.push_back(vec2d());
        }
        points[1].x = size.x / 2;
        points[1].y = size.y / 2;
    }
    else if(type == prim_triangle || type == prim_filled_triangle){
        cerr << "Error: In " << __FUNCTION__ << ": Cannot calculate points of triangle primitive.\n";
    }
}
void PrimitivesModule::getContext(string attribute, vector <BasePointersStruct> & BasePointers){
    BasePointers.push_back(BasePointersStruct());
    if(attribute == "thickness"){
        BasePointers.back().setPointer(&thickness);
    }
    else{
        BasePointers.pop_back();
        getPrimaryContext(attribute, BasePointers);
    }
}
PrimitiveType getPrimitiveType(string type){
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
    else{
        return prim_null;
    }
}

string translatePrimitiveType(PrimitiveType type){
    switch(type){
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
            return "null";
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