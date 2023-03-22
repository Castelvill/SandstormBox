#include <iostream>
#include <math.h>

using namespace std;

class vec2f{
public:
    double x, y, len;
    void set(double x1, double y1, double x2, double y2);
    void set(double x1, double y1);
    void translate(vec2f vec);
    void translate(double tx, double ty);
    void rotate(double);
    double length();
    void normalize();
    vec2f(double x1, double y1);
    vec2f();
};
vec2f::vec2f(){
    x = 0;
    y = 0;
    len = 0.0;
}
vec2f::vec2f(double x1, double y1){
    x = x1;
    y = y1;
    len = 0.0;
}
void vec2f::set(double x1, double y1){
    x = x1;
    y = y1;
    len = 0.0;
}
void vec2f::translate(vec2f vec){
    x += vec.x;
    y += vec.y;
}
void vec2f::translate(double tx, double ty){
    x += tx;
    y += ty;
}
void vec2f::set(double x1, double y1, double x2, double y2){
    x = x2 - x1;
    y = y2 - y1;
    len = length();
}
double vec2f::length(){
    return sqrt(pow(x, 2) + pow(y, 2));
}
void vec2f::rotate(double degrees) {
	double theta = degrees / 180.0 * M_PI;
	double c = cos(theta);
	double s = sin(theta);
	double tx = x * c - y * s;
	double ty = x * s + y * c;
	x = tx;
	y = ty;
	if(x < 0.0001 && x > 0.0)
        x = 0.0;
    if(y < 0.0001 && y > 0.0)
        y = 0.0;
}
void vec2f::normalize() {
	if (length() == 0) return;
	x *= (1.0 / length());
	y *= (1.0 / length());
}

void hello(vec2f heh){
    cout << "Booba " << heh.x << " " << heh.y << "\n";
}

int main()
{
    vec2f wektorek1;
    wektorek1.set(0, 0, 5, 0);

    vec2f wektorek2;
    wektorek2.set(0, 0, 1.25, -2.0);


    cout << wektorek1.length() << ", " << wektorek1.x << ", " << wektorek1.y << endl;
    //wektorek1.translate(0.5, -0.5);
    //cout << wektorek1.len << ", " << wektorek1.x << ", " << wektorek1.y << endl;
    //wektorek1.translate(wektorek2);
    //cout << wektorek1.len << ", " << wektorek1.x << ", " << wektorek1.y << endl;
    //wektorek1.normalize();
    //cout << wektorek1.len << ", " << wektorek1.x << ", " << wektorek1.y << endl;
    wektorek1.normalize();
    for(double i=0.0; i<360; i+=10.0){
        wektorek1.rotate(i);

        cout << wektorek1.length() << ", " << wektorek1.x << ", " << wektorek1.y << endl;
    }

    //wektorek1.normalize();
    //cout << wektorek1.len<< ", " << wektorek1.x << ", " << wektorek1.y << endl;
    return 0;
}
