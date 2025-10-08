#include "myvector.h"

#include <cmath>
#include <cstdio>

extern const Vector whiteV = {255, 255, 255}, blackV = {0, 0, 0};



bool isZero(double a)
{
    const double eps = 1e-3;
    return a > -eps && a < eps;
}

int randInt(int lft, int rgt) { return rand() % (rgt - lft + 1) + lft;}
double randDouble(double lft, double rgt) { return rand() / (RAND_MAX / (rgt - lft)) + lft; }

void solveQuadratic(double a, double b, double c, double* x1, double* x2, int* nRoots)
{
    double d = b * b - 4 * a * c;

    if (isZero(d))
    {
        if (nRoots != nullptr) *nRoots = 1;
        if (x1 != nullptr) *x1 = -b / (2 * a);
        if (x2 != nullptr) *x2 = 0;
        return;
    }

    if (d < 0)
    {
        *nRoots = 0;
        *x1 = *x2 = 0;
        return;
    }

    if (nRoots != nullptr) *nRoots = 2;
    if (x1 != nullptr) *x1 = (-b - std::sqrt(d)) / (2 * a);
    if (x2 != nullptr) *x2 = (-b + std::sqrt(d)) / (2 * a);
}



bool inIntRect(Vector p, Vector tl, Vector br)
{
    return (tl.x <= p.x && p.x <= br.x && tl.y <= p.y && p.y <= br.y);
}

bool rectIntersection(FixedVec r1, FixedVec r2, FixedVec* ans)
{
    FixedVec answer;
    answer.p1.x = std::max(r1.p1.x, r2.p1.x);
    answer.p1.y = std::max(r1.p1.y, r2.p1.y);
    answer.p2.x = std::min(r1.p2.x, r2.p2.x);
    answer.p2.y = std::min(r1.p2.y, r2.p2.y);

    if (answer.p1.x >= answer.p2.x || answer.p1.y >= answer.p2.y) return 0;
    *ans = answer;
    return 1;
}

bool clipLine(FixedVec line, FixedVec rect, FixedVec* ans)
{
    Vector p1 = line.p1, p2 = line.p2, tl = rect.p1, br = rect.p2;

    if (inIntRect(p1, tl, br) && inIntRect(p2, tl, br))
    {
        ans->p1 = p1;
        ans->p2 = p2;
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
    
    
    ans->p1 = p1;
    ans->p2 = p2;
    return 1;
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

void print(Vector v) { printf("Vector %.3lf %.3lf %.3lf\n", v.x, v.y, v.z); }
void print(FixedVec v) { printf("FixedVec (%.3lf %.3lf %.3lf) (%.3lf %.3lf %.3lf)\n", 
    v.p1.x, v.p1.y, v.p1.z, v.p2.x, v.p2.y, v.p2.z); }
 
 

double angle(Vector a, Vector b)
{
    return (!a) ^ (!b);
}

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



Vector fixedToFree(FixedVec v) { return v.p2 - v.p1; }
FixedVec freeToFixed(Vector v, Vector start) { return {start, start + v}; }

FixedVec rotateV(FixedVec v, double angle)
{
    Vector pAdjust = v.p2 - v.p1;

    double sinA = std::sin(angle), cosA = std::cos(angle);
    Vector newAdjust = {pAdjust.x * cosA - pAdjust.y * sinA, pAdjust.x * sinA + pAdjust.y * cosA, pAdjust.z};
    return {v.p1, v.p1 + newAdjust};
}
