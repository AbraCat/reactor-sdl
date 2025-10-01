#include "myvector.h"

#include <cmath>
#include <cstdio>

Vector::Vector() { this->x = this->y = this->z = 0; };
Vector::Vector(double x, double y)
{
    this->x = x;
    this->y = y;
    this->z = 0;
}
Vector::Vector(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


Vector operator+(Vector v1, Vector v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
Vector operator-(Vector v1, Vector v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
Vector operator*(Vector v1, Vector v2) { return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
 
 
Vector& operator+=(Vector& v1, Vector v2) { v1 = v1 + v2; return v1; }
Vector& operator-=(Vector& v1, Vector v2) { v1 = v1 - v2; return v1; }
Vector& operator*=(Vector& v, double a) { v = v * a; return v; }
Vector& operator/=(Vector& v, double a) { v = v / a; return v; }
 
 
Vector operator*(Vector v, double a) { return {v.x * a, v.y * a, v.z * a}; }
Vector operator/(Vector v, double a) { return {v.x / a, v.y / a, v.z / a}; }
 
double operator*(Vector v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
Vector operator!(Vector v) { return v / (*v); }
double operator^(Vector a, Vector b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
void print(Vector v) { printf("vector %.3lf %.3lf %.3lf\n", v.x, v.y, v.z); }
 
 
Vector proj(Vector a, Vector n)
{
    double t = (a ^ n) / std::pow(*n, 2);
    return n * t;
}   
Vector ortog(Vector a, Vector n)
{
    // (a - tn, n) = 0
    // (a, n) - t|n|^2 = 0
    // t = (a, n) / |n|^2
    double t = (a ^ n) / std::pow(*n, 2);
    return a - n * t;
}
double dist(Vector p, Vector a, Vector n)
{
    // distance from point p to line {a + nt | t in R}
    Vector AP = p - a;
    // AP.print();
    Vector projAP_n = proj(AP, n);
    // projAP_n.print();
    return *(AP - projAP_n);
}

Vector limitVector(Vector v, double lower, double upper)
{
    if (v.x < lower) v.x = lower;
    if (v.y < lower) v.y = lower;
    if (v.z < lower) v.z = lower;
    
    if (v.x > upper) v.x = upper;
    if (v.y > upper) v.y = upper;
    if (v.z > upper) v.z = upper;
    
    return v;
}


IntVec::IntVec() { this->x = this->y = this->z = 0; };
IntVec::IntVec(int x, int y)
{
    this->x = x;
    this->y = y;
    this->z = 0;
}
IntVec::IntVec(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


IntVec operator+(IntVec v1, IntVec v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
IntVec operator-(IntVec v1, IntVec v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
IntVec operator*(IntVec v1, IntVec v2) { return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }

IntVec operator*(IntVec v, int a) { return {v.x * a, v.y * a, v.z * a}; }
IntVec operator/(IntVec v, int a) { return {v.x / a, v.y / a, v.z / a}; }
 
IntVec& operator+=(IntVec& v1, IntVec v2) { v1 = v1 + v2; return v1; }
IntVec& operator-=(IntVec& v1, IntVec v2) { v1 = v1 - v2; return v1; }

IntVec& operator*=(IntVec& v, int a) { v = v * a; return v; }
IntVec& operator/=(IntVec& v, int a) { v = v / a; return v; }

int operator^(IntVec a, IntVec b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
void print(IntVec v) { printf("vector %d %d %d\n", v.x, v.y, v.z); }

IntVec limitVector(IntVec v, int lower, int upper)
{
    if (v.x < lower) v.x = lower;
    if (v.y < lower) v.y = lower;
    if (v.z < lower) v.z = lower;
    
    if (v.x > upper) v.x = upper;
    if (v.y > upper) v.y = upper;
    if (v.z > upper) v.z = upper;
    
    return v;
}



Vector fixedToFree(FixedVector v) { return v.p2 - v.p1; }
FixedVector freeToFixed(Vector v, Vector start) { return {start, start + v}; }

FixedVector rotateV(FixedVector v, double angle)
{
    Vector pAdjust = v.p2 - v.p1;

    double sinA = std::sin(angle), cosA = std::cos(angle);
    Vector newAdjust = {pAdjust.x * cosA - pAdjust.y * sinA, pAdjust.x * sinA + pAdjust.y * cosA, pAdjust.z};
    return {v.p1, v.p1 + newAdjust};
}
