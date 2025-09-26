#include "myvector.h"

#include <cmath>
#include <cstdio>

template class Vector<int>;
template class Vector<double>;

Vec fixedToFree(FixedVector v) { return v.p2 - v.p1; }
FixedVector freeToFixed(Vec v, Vec start) { return {start, start + v}; }

template<class T>
Vector<T>::Vector() { this->x = this->y = this->z = 0; };

template<class T>
Vector<T>::Vector(T x, T y, T z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

template<class T>
Vector<T> operator+(Vector<T> v1, Vector<T> v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
template<class T>
Vector<T> operator-(Vector<T> v1, Vector<T> v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
template<class T>
Vector<T> operator*(Vector<T> v1, Vector<T> v2) { return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }

template<class T>
Vector<T>& operator+=(Vector<T>& v1, Vector<T> v2) { v1 = v1 + v2; return v1; }
template<class T>
Vector<T>& operator-=(Vector<T>& v1, Vector<T> v2) { v1 = v1 - v2; return v1; }
template<class T>
Vector<T>& operator*=(Vector<T>& v, double a) { v = v * a; return v; }
template<class T>
Vector<T>& operator/=(Vector<T>& v, double a) { v = v / a; return v; }

template<class T>
Vector<T> operator*(Vector<T> v, double a) { return {v.x * a, v.y * a, v.z * a}; }
template<class T>
Vector<T> operator/(Vector<T> v, double a) { return {v.x / a, v.y / a, v.z / a}; }

template<class T>
double operator*(Vector<T> v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
template<class T>
Vector<T> operator!(Vector<T> v) { return v / (*v); }
template<class T>
double operator^(Vector<T> a, Vector<T> b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

void print(Vec v) { printf("vector %.3lf %.3lf %.3lf\n", v.x, v.y, v.z); }

template<class T>
Vector<T> proj(Vector<T> a, Vector<T> n)
{
    double t = (a ^ n) / std::pow(*n, 2);
    return n * t;
}

template<class T>
Vector<T> ortog(Vector<T> a, Vector<T> n)
{
    // (a - tn, n) = 0
    // (a, n) - t|n|^2 = 0
    // t = (a, n) / |n|^2
    double t = (a ^ n) / std::pow(*n, 2);
    return a - n * t;
}

template<class T>
double dist(Vector<T> p, Vector<T> a, Vector<T> n)
{
    // distance from point p to line {a + nt | t in R}
    Vector<T> AP = p - a;
    // AP.print();
    Vector<T> projAP_n = proj(AP, n);
    // projAP_n.print();
    return *(AP - projAP_n);
}

template<class T>
Vector<T> limitVector(Vector<T> v, T lower, T upper)
{
    if (v.x < lower) v.x = lower;
    if (v.y < lower) v.y = lower;
    if (v.z < lower) v.z = lower;

    if (v.x > upper) v.x = upper;
    if (v.y > upper) v.y = upper;
    if (v.z > upper) v.z = upper;

    return v;
}

FixedVector rotateV(FixedVector v, double angle)
{
    Vec pAdjust = v.p2 - v.p1;

    double sinA = std::sin(angle), cosA = std::cos(angle);
    Vec newAdjust = {pAdjust.x * cosA - pAdjust.y * sinA, pAdjust.x * sinA + pAdjust.y * cosA, pAdjust.z};
    return {v.p1, v.p1 + newAdjust};
}
