#include <iostream>

using namespace std;

class Point{
public:
    double x, y;
};
class Line{
public:
    Point p1, p2;
    void create(double x1, double y1, double x2, double y2){
        p1.x = x1;
        p1.y = y1;
        p2.x = x2;
        p2.y = y2;
    }
};

bool isMiddlePointBetween(Point p1, Point p2, Point p3){
    if (p2.x <= max(p1.x, p3.x) && p2.x >= min(p1.x, p3.x) &&
        p2.y <= max(p1.y, p3.y) && p2.y >= min(p1.y, p3.y))
       return true;
    return false;
}

int pointsOrientation(Point p1, Point p2, Point p3){
    int val = (p2.y - p1.y) * (p3.x - p2.x) -
              (p2.x - p1.x) * (p3.y - p2.y);

    if (val == 0) return 0;  // colinear

    // clock or counterclock wise
    if(val > 0)
        return 1;
    else
        return 2;
}

bool doLinesIntersect(Line L1, Line L2){
    //Check if points are in clock sequence (1), counterclock sequence (2) or are on the same line (0)
    //It uses directional coordinates. If a1=a2, then lines are parallel.
    int orient1 = pointsOrientation(L1.p1, L1.p2, L2.p1);
    int orient2 = pointsOrientation(L1.p1, L1.p2, L2.p2);
    int orient3 = pointsOrientation(L1.p2, L2.p2, L1.p1);
    int orient4 = pointsOrientation(L1.p2, L2.p2, L1.p2);

    //General case - checking if lines are intersecting without points on other lines
    if (orient1 != orient2 && orient3 != orient4)
        return true;

    //Special Cases - checking if one point is a part of another line
    if (orient1 == 0 && isMiddlePointBetween(L1.p1, L2.p1, L1.p2)) return true;
    if (orient2 == 0 && isMiddlePointBetween(L1.p1, L2.p2, L1.p2)) return true;
    if (orient3 == 0 && isMiddlePointBetween(L2.p1, L1.p1, L2.p2)) return true;
    if (orient4 == 0 && isMiddlePointBetween(L2.p1, L1.p2, L2.p2)) return true;

    return false;
}

int main()
{
    Line L1, L2;

    L1.create(-6, 4, 4, 0);
    L2.create(-1, 8, -6, 0);
    doLinesIntersect(L1, L2)? cout << "Yes\n": cout << "No\n";

    L1.create(10, 0, 0, 10);
    L2.create(0, 0, 10, 10);
    doLinesIntersect(L1, L2)? cout << "Yes\n": cout << "No\n";

    L1.create(-5, -5, 0, 0);
    L2.create(1, 1, 10, 10);
    doLinesIntersect(L1, L2)? cout << "Yes\n": cout << "No\n";

    return 0;
}
