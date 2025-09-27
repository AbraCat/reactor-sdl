class IntVec
{
public:
    IntVec();
    IntVec(int x, int y, int z);

    int x, y, z;
};

class FixedVector
{
public:
    IntVec p1, p2;
};

IntVec operator+(IntVec v1, IntVec v2);
IntVec operator-(IntVec v1, IntVec v2);
IntVec operator*(IntVec v1, IntVec v2);

IntVec& operator+=(IntVec& v1, IntVec v2);
IntVec& operator-=(IntVec& v1, IntVec v2);
IntVec& operator*=(IntVec& v, int a);
IntVec& operator/=(IntVec& v, int a);

IntVec operator*(IntVec v, double a);
IntVec operator/(IntVec v, double a);

double operator*(IntVec v);
Vector operator!(IntVec v);
int operator^(IntVec a, IntVec b);

IntVec proj(IntVec a, IntVec n);
IntVec ortog(IntVec a, IntVec n);
int dist(IntVec p, IntVec a, IntVec n);
IntVec limitVector(IntVec v, int lower, int upper);





IntVec::IntVec() { this->x = this->y = this->z = 0; };
IntVec::IntVec(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


IntVec operator+(IntVec v1, IntVec v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
IntVec operator-(IntVec v1, IntVec v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
IntVec operator*(IntVec v1, IntVec v2) { return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; }
 
 
IntVec& operator+=(IntVec& v1, IntVec v2) { v1 = v1 + v2; return v1; }
IntVec& operator-=(IntVec& v1, IntVec v2) { v1 = v1 - v2; return v1; }
IntVec& operator*=(IntVec& v, int a) { v = v * a; return v; }
IntVec& operator/=(IntVec& v, int a) { v = v / a; return v; }
 
 
IntVec operator*(IntVec v, double a) { return {v.x * a, v.y * a, v.z * a}; }
IntVec operator/(IntVec v, double a) { return {v.x / a, v.y / a, v.z / a}; }
 
double operator*(IntVec v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
Vector operator!(IntVec v) { return v / (*v); }
int operator^(IntVec a, IntVec b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
void print(IntVec v) { printf("vector %.3lf %.3lf %.3lf\n", v.x, v.y, v.z); }
 
 
IntVec proj(IntVec a, IntVec n)
{
    int t = (a ^ n) / std::pow(*n, 2);
    return n * t;
}   
IntVec ortog(IntVec a, IntVec n)
{
    // (a - tn, n) = 0
    // (a, n) - t|n|^2 = 0
    // t = (a, n) / |n|^2
    int t = (a ^ n) / std::pow(*n, 2);
    return a - n * t;
}
int dist(IntVec p, IntVec a, IntVec n)
{
    // distance from point p to line {a + nt | t in R}
    IntVec AP = p - a;
    // AP.print();
    IntVec projAP_n = proj(AP, n);
    // projAP_n.print();
    return *(AP - projAP_n);
}