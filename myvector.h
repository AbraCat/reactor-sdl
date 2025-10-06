#ifndef MYVECTOR_H
#define MYVECTOR_H

class Vector;
class IntVec;
class Rect;


extern const Vector whiteV, blackV;


class Vector
{
public:
    Vector();
    Vector(double x, double y);
    Vector(double x, double y, double z);
    Vector(IntVec v);

    double x, y, z;
};

class FixedVec
{
public:
    Vector p1, p2;
};

Vector operator+(Vector v1, Vector v2);
Vector operator-(Vector v1, Vector v2);
Vector operator*(Vector v1, Vector v2);

Vector& operator+=(Vector& v1, Vector v2);
Vector& operator-=(Vector& v1, Vector v2);
Vector& operator*=(Vector& v, double a);
Vector& operator/=(Vector& v, double a);

Vector operator*(Vector v, double a);
Vector operator/(Vector v, double a);

double operator*(Vector v);
Vector operator!(Vector v);
double operator^(Vector a, Vector b);

Vector proj(Vector a, Vector n);
Vector ortog(Vector a, Vector n);
double dist(Vector p, Vector a, Vector n);
Vector limitVector(Vector v, double lower, double upper);

double arg(Vector v);




class IntVec
{
public:
    IntVec();
    IntVec(int x, int y);
    IntVec(int x, int y, int z);
    IntVec(Vector v);

    int x, y, z;
};

IntVec operator+(IntVec v1, IntVec v2);
IntVec operator-(IntVec v1, IntVec v2);
IntVec operator*(IntVec v1, IntVec v2);

IntVec& operator+=(IntVec& v1, IntVec v2);
IntVec& operator-=(IntVec& v1, IntVec v2);

int operator^(IntVec a, IntVec b);

IntVec proj(IntVec a, IntVec n);
IntVec ortog(IntVec a, IntVec n);
int dist(IntVec p, IntVec a, IntVec n);
IntVec limitVector(IntVec v, int lower, int upper);




class Rect
{
public:
    Rect(IntVec tl, IntVec br);
    
    IntVec tl, br;
};




Vector fixedToFree(FixedVec v);
FixedVec freeToFixed(Vector v, Vector start);
FixedVec rotateV(FixedVec v, double angle);

void print(Vector v);

#endif // MYVECTOR_H