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