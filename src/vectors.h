#ifndef VECTORS_H_INCLUDED
#define VECTORS_H_INCLUDED
#include <vector>
#include <windows.h>
#include <string>


constexpr auto M_PI = 3.14159265358979323846;

using std::vector;

template <typename type>
class vecX{
public:
    vector <type> val;
    vecX(){}
    void makeVecX(int count, va_list args){
        for (int i = 0; i < count; i++){
            val.push_back(va_arg(args, type));
        }
    }
    void setVecX(int count, ...){
        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; i++){
            val.push_back(va_arg(args, type));
        }
        va_end(args);
    }
};

class vecXd: public vecX<double>{
public:
    vecXd();
    vecXd(int count, ...);
};

class vecXs{
public:
    vector <std::string> val;
    vecXs();
    vecXs(int count, ...);
};

class vec3d: public vecX<double>{
public:
    vec3d(double, double, double);
};

class vec4d: public vecX<double>{
public:
    vec4d(double, double, double, double);
};

class vec5d: public vecX<double>{
public:
    vec5d(double, double, double, double, double);
};

class vec6d: public vecX<double>{
public:
    vec6d(double, double, double, double, double, double);
};

class vec7d: public vecX<double>{
public:
    vec7d(double, double, double, double, double, double, double);
};

class vec2d{
public:
    double x, y, length;
    vec2d();
    vec2d(double x1, double y1);
    vec2d(double x1, double y1, double x2, double y2);
    vec2d neg();
    void set(vec2d vec);
    void set(double x1, double y1);
    void set(double x1, double y1, double x2, double y2);
    bool isEqual(double x1, double y1);
    bool isEqual(vec2d vec);
    void setLength(double newLength);
    void translate(vec2d vec);
    void translate(double x, double y);
    void operator+=(vec2d vec);
    void operator-=(vec2d vec);
    void rotate(double degrees);
    double getAngle();
    double getAngle(vec2d from);
    double countLength();
    void normalize();
    vec2d getNormalized();
    void multiply(vec2d vec);
    void multiply(double var);
    void multiply(double var1, double var2);
    void operator*=(vec2d vec);
    void operator*=(double var);
    void divide(vec2d vec);
    void divide(double var);
    void operator/=(vec2d vec);
    void operator/=(double var);
    void divide(double var1, double var2);
};

vec2d min(vec2d vec1, vec2d vec2);
vec2d max(vec2d vec1, vec2d vec2);
vec2d operator+(vec2d vec1, vec2d vec2);
vec2d operator+(vec2d vec, double var);
vec2d operator-(vec2d vec1, vec2d vec2);
vec2d operator-(vec2d vec, double var);
vec2d operator-(double var, vec2d vec);
vec2d operator-(vec2d vec);
vec2d operator*(vec2d vec1, vec2d vec2);
vec2d operator*(vec2d vec, double var);
vec2d operator/(vec2d vec1, vec2d vec2);
vec2d operator/(vec2d vec, double var);
bool operator==(vec2d vec1, vec2d vec2);

class vec2i{
public:
    int x, y, length;
    vec2i();
    vec2i(int x1, int y1);
    vec2i(int x1, int y1, int x2, int y2);
    void set(int x1, int y1, int x2, int y2);
    void set(int x1, int y1);
    void set(vec2i vec);
    bool isEqual(int x1, int y1);
    void setLength(int newLength);
    void translate(vec2i vec);
    void translate(int x, int y);
    void rotate(double degrees);
    int countLength();
    void normalize();
};




#endif // VECTORS_H_INCLUDED
