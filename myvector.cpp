#include "myvector.h"

#include <cmath>
#include <cstdio>

extern const Vector whiteV = {255, 255, 255}, blackV = {0, 0, 0};


bool inIntRect(IntVec p, IntVec tl, IntVec br)
{
    return (tl.x <= p.x && p.x <= br.x && tl.y <= p.y && p.y <= br.y);
}

bool rectIntersection(Rect r1, Rect r2, Rect* ans)
{
    Rect answer;
    answer.tl.x = std::max(r1.tl.x, r2.tl.x);
    answer.tl.y = std::max(r1.tl.y, r2.tl.y);
    answer.br.x = std::min(r1.br.x, r2.br.x);
    answer.br.y = std::min(r1.br.y, r2.br.y);

    if (answer.tl.x >= answer.br.x || answer .tl.y >= answer.br.y) return 0;
    *ans = answer;
    return 1;
}

bool clipIntLine(IntVec p1, IntVec p2, IntVec tl, IntVec br, IntVec* ans1, IntVec* ans2)
{
    if (inIntRect(p1, tl, br) && inIntRect(p2, tl, br))
    {
        *ans1 = p1;
        *ans2 = p2;
        return 1;
    }
    
    if (p1.x < tl.x && p1.y < tl.y || p1.x < tl.x && p1.y > br.y ||
        p1.x > br.x && p1.y < tl.y || p1.x > br.x && p1.y > br.y ||
        p2.x < tl.x && p2.y < tl.y || p2.x < tl.x && p2.y > br.y ||
        p2.x > br.x && p2.y < tl.y || p2.x > br.x && p2.y > br.y)
            return 0;
    
    if (p1.x < tl.x) {
        if (p2.x < tl.x) return 0;
        p1.x = tl.x;
    }
    if (p1.y < tl.y) {
        if (p2.y < tl.y) return 0;
        p1.y = tl.y;
    }
    if (p1.x > br.x) {
        if (p2.x > br.x) return 0;
        p1.x = br.x;
    }
    if (p1.y > br.y) {
        if (p2.y > br.y) return 0;
        p1.y = br.y;
    }

    if (p2.x < tl.x) p2.x = tl.x;
    if (p2.y < tl.y) p2.y = tl.y;
    if (p2.x > br.x) p2.x = br.x;
    if (p2.y > br.y) p2.y = br.y;
    
    
    *ans1 = p1;
    *ans2 = p2;
    return 1;
}




Rect::Rect() {};

Rect::Rect(IntVec tl, IntVec br) : tl(tl), br(br)
{
    //
}



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
Vector::Vector(IntVec v) : x(v.x), y(v.y), z(v.z) {};

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

double arg(Vector v)
{
    return std::atan2(v.y, v.x);
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
IntVec::IntVec(Vector v) : x(v.x), y(v.y), z(v.z) {};


IntVec operator+(IntVec v1, IntVec v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
IntVec operator-(IntVec v1, IntVec v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
IntVec operator*(IntVec v1, IntVec v2) { return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
 
IntVec& operator+=(IntVec& v1, IntVec v2) { v1 = v1 + v2; return v1; }
IntVec& operator-=(IntVec& v1, IntVec v2) { v1 = v1 - v2; return v1; }

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



Vector fixedToFree(FixedVec v) { return v.p2 - v.p1; }
FixedVec freeToFixed(Vector v, Vector start) { return {start, start + v}; }

FixedVec rotateV(FixedVec v, double angle)
{
    Vector pAdjust = v.p2 - v.p1;

    double sinA = std::sin(angle), cosA = std::cos(angle);
    Vector newAdjust = {pAdjust.x * cosA - pAdjust.y * sinA, pAdjust.x * sinA + pAdjust.y * cosA, pAdjust.z};
    return {v.p1, v.p1 + newAdjust};
}
