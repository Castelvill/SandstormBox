#include "primitivesModule.h"

void PrimitivesModule::setUpNewInstance(){
    type = prim_null;
    color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
    thickness = 1.0;
    radius = 1.0;
    samples = 0;
}

PrimitivesModule::PrimitivesModule(){
    setUpNewInstance();
}
PrimitivesModule::PrimitivesModule(string newAlias, vector<string> * listOfIDs, string newLayerID, string newObjectID){
    primaryConstructor(newAlias, listOfIDs, newLayerID, newObjectID);
    setUpNewInstance();
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
void PrimitivesModule::draw(vec2d base, Camera2D Camera, bool outSourcing){
    switch(type){
        case prim_line:
            if(points.size() < 2){
                cout << "Error: In " << __FUNCTION__ << ": Line primitive requires 2 points.\n";
                return;
            }
            al_draw_line(base.x + points[0].x, points[0].y, points[1].x, points[1].y, color, thickness);
            break;
        case prim_triangle:
            if(points.size() < 3){
                cout << "Error: In " << __FUNCTION__ << ": Triangle primitive requires 3 points.\n";
                return;
            }
            al_draw_triangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, color, thickness);
            break;
        case prim_filled_triangle:
            if(points.size() < 3){
                cout << "Error: In " << __FUNCTION__ << ": Triangle primitive requires two points.\n";
                return;
            }
            al_draw_filled_triangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, color);
            break;
        case prim_rectangle:
            if(points.size() < 2){
                cout << "Error: In " << __FUNCTION__ << ": Rectangle primitive requires 2 points.\n";
                return;
            }
            al_draw_rectangle(points[0].x, points[0].y, points[1].x, points[1].y, color, thickness);
            break;
        case prim_filled_rectangle:
            if(points.size() < 2){
                cout << "Error: In " << __FUNCTION__ << ": Rectangle primitive requires 2 points.\n";
                return;
            }
            al_draw_filled_rectangle(points[0].x, points[0].y, points[1].x, points[1].y, color);
            break;
        case prim_rounded_rectangle:
            if(points.size() < 3){
                cout << "Error: In " << __FUNCTION__ << ": Rounded rectangle primitive requires 3 points.\n";
                return;
            }
            al_draw_rounded_rectangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, color, thickness);
            break;
        case prim_filled_rounded_rectangle:
            if(points.size() < 3){
                cout << "Error: In " << __FUNCTION__ << ": Rounded rectangle primitive requires 3 points.\n";
                return;
            }
            if(samples <= 0){
                al_draw_filled_rounded_rectangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, color);
            }
            else{
                ALLEGRO_COLOR sampledColor = color;
                sampledColor.a = color.a / samples;
                for(int i = 0; i <= samples; i++){
                    al_draw_filled_rounded_rectangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x + i, points[2].y + i, sampledColor);
                }
            }
            
            break;
        case prim_circle:
            if(points.size() < 1){
                cout << "Error: In " << __FUNCTION__ << ": Circle primitive requires 1 point.\n";
                return;
            }
            al_draw_circle(points[0].x, points[0].y, radius, color, thickness);
            break;
        case prim_filled_circle:
            if(points.size() < 1){
                cout << "Error: In " << __FUNCTION__ << ": Circle primitive requires 1 points.\n";
                return;
            }
            al_draw_filled_circle(points[0].x, points[0].y, radius, color);
            break;
        case prim_ellipse:
            if(points.size() < 2){
                cout << "Error: In " << __FUNCTION__ << ": Ellipse primitive requires 2 points.\n";
                return;
            }
            al_draw_ellipse(points[0].x, points[0].y, points[1].x, points[1].y, color, thickness);
            break;
        case prim_filled_ellipse:
            if(points.size() < 2){
                cout << "Error: In " << __FUNCTION__ << ": Ellipse primitive requires 2 points.\n";
                return;
            }
            al_draw_filled_ellipse(points[0].x, points[0].y, points[1].x, points[1].y, color);
            break;
        default:
            cout << "Error: In " << __FUNCTION__ << ": Cannot draw primitive.\n";
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
        cout << "Error: In " << __FUNCTION__ << ": Cannot calculate points of triangle primitive.\n";
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

int PrimitivesModule::getUsedBitmapLayer() const{
    return usedBitmapLayer;
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
