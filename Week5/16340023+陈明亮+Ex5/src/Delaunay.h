#ifndef DELAUNAY_H
#define DELAUNAY_H
#include <cstdlib>  
#include <iostream>  
#include <cstring>  
#include <string>  
#include <fstream>  
#include <math.h>  
#include <vector>

using namespace std;

class point {
public:
    point() {}

    point(double fx, double fy, double fz)
        :x(fx), y(fy), z(fz) {
    }

    // Subtract
    point operator - (const point& v) const {
        return point(x - v.x, y - v.y, z - v.z);
    }

    // Dot product
    double Dot(const point& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    // Cross product
    point Cross(const point& v) const {
        return point(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    bool SameSide(const point &A, const point &B, const point &C, const point &P) {
        point AB = B - A;
        point AC = C - A;
        point AP = P - A;

        point v1 = AB.Cross(AC);
        point v2 = AB.Cross(AP);

        return v1.Dot(v2) >= 0;
    }

    bool PointinTriangle1(const point &A, const point &B, const point &C, const point &P) {
        return SameSide(A, B, C, P) &&
            SameSide(B, C, A, P) &&
            SameSide(C, A, B, P);
    }

public:
    double x, y, z;
};

typedef std::vector<point> PointArray; 
 
typedef struct  
{  
    int left;  
    int right;  
    int count;
}Edge; 
typedef std::vector<Edge> EdgeArray;
  
typedef struct  
{  
    int v[3]; 
    Edge s[3];
    double xc;  
    double yc; 
    double r;
	double** m1; 
	double** m2; 
}triangle;
typedef std::vector<triangle> TriangleArray;  
  
typedef std::vector<int> intArray;
  
class Delaunay
{  
public:  
    Delaunay(point p1,point p2,point p3,point p4);
    ~Delaunay();
  
    bool AddPoint(double xx,double yy,double zz);
    void Delete_Frame();
    void Boundary_Recover(int fromPoint,int toPoint);

	PointArray m_Pts;
	EdgeArray m_Edges;
	TriangleArray m_Tris;

private:  
    void Cal_Centre(double &x_centre,double &y_centre,double &radius,int n1,int n2,int n3);
    void MakeTriangle(int n1,int n2,int n3);
    bool inCircle(double xx,double yy,triangle currentTris); 
    void DelTriangle(int n,EdgeArray &BoundEdges);
  
};

#endif