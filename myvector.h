#ifndef MYVECTOR_H
#define MYVECTOR_H

class Vector;
class Vector;
class FixedVec;
 

extern const Vector whiteV, blackV, gray_col, purple_col, yellow_col, green_col, orange_col, white_col, red_col;

bool isZero(double a);
int randInt(int lft, int rgt);
double randDouble(double lft, double rgt);
void solveQuadratic(double a, double b, double c, double* x1, double* x2, int* nRoots);

bool inIntRect(Vector p, Vector tl, Vector br);
bool clipLine(FixedVec line, FixedVec rect, FixedVec* ans);
bool rectIntersection(FixedVec r1, FixedVec r2, FixedVec* ans);

class Vector
{
public:
    Vector();
    Vector(double x, double y);
    Vector(double x, double y, double z);

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

double angle(Vector a, Vector b);
Vector proj(Vector a, Vector n);
Vector ortog(Vector a, Vector n);
double dist(Vector p, Vector a, Vector n);
Vector limitVector(Vector v, double lower, double upper);

double arg(Vector v);

Vector fixedToFree(FixedVec v);
FixedVec freeToFixed(Vector v, Vector start);
FixedVec rotateV(FixedVec v, double angle);

void print(Vector v);
void print(FixedVec v);

#endif // MYVECTOR_H