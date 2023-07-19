#include "vectors.h"
#include <math.h>
#include <iostream>

vecXd::vecXd(){}
vecXd::vecXd(int count, ...){
    va_list args;
    va_start(args, count);
    makeVecX(count, args);
    va_end(args);
}

vecXs::vecXs(){}
vecXs::vecXs(int count, ...){
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++){
        val.push_back(va_arg(args, const char *));
    }
    va_end(args);
}


vec3d::vec3d(double a, double b, double c){
    setNewVecX(3, a, b, c);
}
vec4d::vec4d(double a, double b, double c, double d){
    setNewVecX(4, a, b, c, d);
}
vec5d::vec5d(double a, double b, double c, double d, double e){
    setNewVecX(5, a, b, c, d, e);
}
vec6d::vec6d(double a, double b, double c, double d, double e, double f){
    setNewVecX(6, a, b, c, d, e, f);
}
vec7d::vec7d(double a, double b, double c, double d, double e, double f, double g){
    setNewVecX(7, a, b, c, d, e, f, g);
}


vec2d::vec2d(){
    x = 0.0;
    y = 0.0;
    length = 0.0;
}
vec2d::vec2d(double x1, double y1){
    x = x1;
    y = y1;
    length = 0.0;
}
vec2d::vec2d(double x1, double y1, double x2, double y2){
    x = x2 - x1;
    y = y2 - y1;
    length = countLength();
}
vec2d vec2d::neg(){
    return vec2d(-x, -y);
}
void vec2d::set(vec2d vec){
    x = vec.x;
    y = vec.y;
    length = 0.0;
}
void vec2d::set(double x1, double y1){
    x = x1;
    y = y1;
    length = 0.0;
}
void vec2d::set(double x1, double y1, double x2, double y2){
    x = x2 - x1;
    y = y2 - y1;
    length = countLength();
}
bool vec2d::isEqual(double x1, double y1){
    if(x == x1 && y == y1)
        return true;
    return false;
}
bool vec2d::isEqual(vec2d vec){
    if(x == vec.x && y == vec.y)
        return true;
    return false;
}
double vec2d::countLength(){
    return sqrt(pow(x, 2) + pow(y, 2));
}
void vec2d::translate(vec2d vec){
    x += vec.x;
    y += vec.y;
}
void vec2d::translate(double tx, double ty){
    x += tx;
    y += ty;
}
void vec2d::operator+=(vec2d vec){
    x += vec.x;
    y += vec.y;
}
void vec2d::operator-=(vec2d vec){
    x -= vec.x;
    y -= vec.y;
}
void vec2d::setLength(double newLength){
    length = newLength;
}
void vec2d::rotate(double degrees) {
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
double vec2d::getAngle(){
    vec2d from(0.0, -1.0);
    double dot = x*from.x + y*from.y; //dot product between [x1, y1] and [x2, y2]
    double det = x*from.y - y*from.x; //determinant
    double angle = atan2(det, dot);
    return angle;
}
double vec2d::getAngle(vec2d from){
    double dot = x*from.x + y*from.y; //dot product between [x1, y1] and [x2, y2]
    double det = x*from.y - y*from.x; //determinant
    double angle = atan2(det, dot);
    return angle;
}
void vec2d::normalize() {
    double len = countLength();
	if (len == 0) return;
	x /= len;
	y /= len;
}
vec2d vec2d::getNormalized(){
    double len = countLength();
	if (len == 0) return vec2d(0.0, 0.0);
	return vec2d(x/len, y/len);
}
void vec2d::multiply(vec2d vec){
    x *= vec.x;
    y *= vec.y;
}
void vec2d::multiply(double var){
    x *= var;
    y *= var;
}
void vec2d::operator*=(vec2d vec){
    x *= vec.x;
    y *= vec.y;
}
void vec2d::operator*=(double var){
    x *= var;
    y *= var;
}
void vec2d::multiply(double var1, double var2){
    x *= var1;
    y *= var2;
}
void vec2d::divide(vec2d vec){
    x /= vec.x;
    y /= vec.y;
}
void vec2d::divide(double var){
    x /= var;
    y /= var;
}
void vec2d::operator/=(vec2d vec){
    x /= vec.x;
    y /= vec.y;
}
void vec2d::operator/=(double var){
    x /= var;
    y /= var;
}
void vec2d::divide(double var1, double var2){
    x /= var1;
    y /= var2;
}
vec2d min(vec2d vec1, vec2d vec2){
    return vec2d(std::min(vec1.x, vec2.x), std::min(vec1.y, vec2.y));
}
vec2d max(vec2d vec1, vec2d vec2){
    return vec2d(std::max(vec1.x, vec2.x), std::max(vec1.y, vec2.y));
}
vec2d operator+(vec2d vec1, vec2d vec2){
    return vec2d(vec1.x+vec2.x, vec1.y+vec2.y);
}
vec2d operator+(vec2d vec, double var){
    return vec2d(vec.x+var, vec.y+var);
}
vec2d operator-(vec2d vec1, vec2d vec2){
    return vec2d(vec1.x-vec2.x, vec1.y-vec2.y);
}
vec2d operator-(vec2d vec, double var){
    return vec2d(vec.x-var, vec.y-var);
}
vec2d operator-(double var, vec2d vec){
    return vec2d(var-vec.x, var-vec.y);
}
vec2d operator-(vec2d vec){
    return vec2d(-vec.x, -vec.y);
}
vec2d operator*(vec2d vec1, vec2d vec2){
    return vec2d(vec1.x*vec2.x, vec1.y*vec2.y);
}
vec2d operator*(vec2d vec, double var){
    return vec2d(vec.x*var, vec.y*var);
}
vec2d operator/(vec2d vec1, vec2d vec2){
    return vec2d(vec1.x/vec2.x, vec1.y/vec2.y);
}
vec2d operator/(vec2d vec, double var){
    return vec2d(vec.x/var, vec.y/var);
}
bool operator==(vec2d vec1, vec2d vec2){
    return vec1.isEqual(vec2);
}

vec2i::vec2i(){
    x = 0.0;
    y = 0.0;
    length = 0.0;
}
vec2i::vec2i(int x1, int y1){
    x = x1;
    y = y1;
    length = 0.0;
}
vec2i::vec2i(int x1, int y1, int x2, int y2){
    x = x2 - x1;
    y = y2 - y1;
    length = countLength();
}
void vec2i::set(vec2i vec){
    x = vec.x;
    y = vec.y;
    length = 0.0;
}
void vec2i::set(int x1, int y1){
    x = x1;
    y = y1;
    length = 0.0;
}
void vec2i::set(int x1, int y1, int x2, int y2){
    x = x2 - x1;
    y = y2 - y1;
    length = countLength();
}
bool vec2i::isEqual(int x1, int y1){
    if(x == x1 && y == y1)
        return true;
    return false;
}
int vec2i::countLength(){
    return sqrt(pow(x, 2) + pow(y, 2));
}
void vec2i::translate(vec2i vec){
    x += vec.x;
    y += vec.y;
}
void vec2i::translate(int tx, int ty){
    x += tx;
    y += ty;
}
void vec2i::setLength(int newLength){
    length = newLength;
}
void vec2i::rotate(double degrees) {
	if(degrees == 0.0)
        return;
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
void vec2i::normalize() {
	if (countLength() == 0) return;
	x *= (1.0 / countLength());
	y *= (1.0 / countLength());
}
