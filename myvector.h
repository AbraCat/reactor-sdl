#ifndef MYVECTOR_H
#define MYVECTOR_H

template<class T>
class Vector
{
public:
    Vector();
    Vector(T x, T y, T z);

    T x, y, z;
};

using Vec = Vector<double>;
using IntVec = Vector<int>;

class FixedVector
{
public:
    Vec p1, p2;
};

template<class T>
Vector<T> operator+(Vector<T> v1, Vector<T> v2);
template<class T>
Vector<T> operator-(Vector<T> v1, Vector<T> v2);
template<class T>
Vector<T> operator*(Vector<T> v1, Vector<T> v2);

template<class T>
Vector<T>& operator+=(Vector<T>& v1, Vector<T> v2);
template<class T>
Vector<T>& operator-=(Vector<T>& v1, Vector<T> v2);
template<class T>
Vector<T>& operator*=(Vector<T>& v, double a);
template<class T>
Vector<T>& operator/=(Vector<T>& v, double a);

template<class T>
Vector<T> operator*(Vector<T> v, double a);
template<class T>
Vector<T> operator/(Vector<T> v, double a);

template<class T>
double operator*(Vector<T> v);
template<class T>
Vector<T> operator!(Vector<T> v);
template<class T>
double operator^(Vector<T> a, Vector<T> b);

template<class T>
Vector<T> limitVector(Vector<T> v, T lower, T upper);

template<class T>
Vector<T> proj(Vector<T> a, Vector<T> n);
template<class T>
Vector<T> ortog(Vector<T> a, Vector<T> n);
template<class T>
double dist(Vector<T> p, Vector<T> a, Vector<T> n);

Vec fixedToFree(FixedVector v);
FixedVector freeToFixed(Vec v, Vec start);
FixedVector rotateV(FixedVector v, double angle);

void print(Vec v);

#endif // MYVECTOR_H