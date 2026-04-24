#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cfloat>

const int CAPACITY = 20;

struct Point {
    double x, y, z;
    Point() : x(-1), y(-1), z(-1) {}
    Point(double a, double b, double c) : x(a), y(b), z(c) {}
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
    bool find_closest(const Point& A, double radius, Point& xp, double& hN, Point& btN);
    void print(Octree* node, int level = 0, int idxch = -1);
    void printRoot();
    void exportOBJ(const std::string& fileName);
    void collectLeaves(Octree* node, std::vector<std::pair<Point,double>>& leaves, double h_root);
};

bool leerCSV(Octree& tree, const std::string& dir);
bool calcularBoundsCSV(const std::string& dir, Point& bt, double& h);
bool leerXYZ(Octree& tree, const std::string& dir);
bool calcularBoundsXYZ(const std::string& dir, Point& bt, double& h);

#endif
