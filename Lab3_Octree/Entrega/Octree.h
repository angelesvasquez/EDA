#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

const int CAPACITY = 4;

struct Point {
    int x, y, z;
    Point() : x(-1), y(-1), z(-1) {}
    Point(int a, int b, int c) : x(a), y(b), z(c) {}
};

double distanciaEuclidiana(Point p, Point q);

class Octree {
private:
    Octree* children[8];
    std::vector<Point> points;
    Point bottomLeft; // esquina inf izq
    double h; 

    bool contains(const Point& p);
    int getChildIndex(Point p);

public:
    Octree(Point bt, double h);
    ~Octree(); 

    bool exist(const Point& p_obj);
    void insert(const Point& p);
    bool find_closest(const Point& A, double radius, Point& xp, double& hN);
    void print(Octree* node, int level = 0, int idxch = -1);
    void printRoot();
};

#endif