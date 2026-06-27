#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>

using namespace std;

// Con KDTree
/*
    Antes de asignar los puntos, se construye un kdtree con los centroides
    Luego, para cada punto hace la busqueda nearestNeighbor
    para encontrar el centroide mas cercano

*/



class Point{
public:
    double x, y;
    int cluster = -1;
    Point(double x, double y): x(x), y(y) {}
    double dist(Point &A){
        return sqrt((A.x-x)*(A.x-x)+(A.y-y)*(A.y-y));
    }

};

struct Centroid{
    Point point;
    int index;
    Centroid(Point p, int i): point(p), index(i) {};
};

struct Node{
    Centroid c;
    int index;
    int axis;
    Node* nodes[2];
    Node(Centroid c, int idx, int ax)
        : c(c), index(idx), axis(ax)
    {
        nodes[0] = nullptr;
        nodes[1] = nullptr;
    }
};

class KDTree{
    Node* root = nullptr;

    Node* buildRec(vector<Centroid>& centroids, int lo, int hi, int level);
    void searchNN(Node* node, Point& q, int& bestIdx, double& bestDist);

    void deleteTree(Node* n) {
        if (!n) return;
        deleteTree(n->nodes[0]);
        deleteTree(n->nodes[1]);
        delete n;
    }

public:
    void build(vector<Point>& centroids);
    int nearestNeighbor(const Point& p);
    ~KDTree() { deleteTree(root); }

};

void KDTree::build(vector<Point>& centroids){
    deleteTree(root);
    root = nullptr;
    vector<Centroid> data;
    for(int i = 0; i<centroids.size(); i++)
        data.emplace_back(centroids[i], i);
    root = buildRec(data, 0, data.size() -1, 0);
}

Node* KDTree::buildRec(vector<Centroid>& centroids, int ini, int fin, int level){
    if(ini > fin) return nullptr; // indice inferior y superior
    int axis = level % 2;
    int mid = (ini + fin) / 2;

    nth_element(
        centroids.begin() + ini,
        centroids.begin() + mid,
        centroids.begin() + fin + 1,
        [axis](const CentroidInfo& a,
               const CentroidInfo& b)
        {
            if (axis == 0)
                return a.point.x < b.point.x;
            else
                return a.point.y < b.point.y;
        });

    Node* node = new Node(centroids[mid], axis);

    node->nodes[0] = buildRec(centroids,
                              ini,
                              mid - 1,
                              level + 1);

    node->nodes[1] = buildRec(centroids,
                              mid + 1,
                              fin,
                              level + 1);

    return node;
}

class KMeans{
    vector<Point> centroids;
    vector<Point> points;
    int k;
    int maxIter;
    void inicializeCentroids();
    bool updateCentroids();
    bool assignClusters();
public:
    void printPuntos();
    void printCentroids();
    KMeans(int k_, vector<Point> p, int maxIt);
    void clustering();

};

KMeans::KMeans(int k_, vector<Point> p, int maxIt){
    k = k_;
    points = p;
    maxIter = maxIt;
}

void KMeans::printPuntos() {
    cout << "x,y,cluster\n";

    for (Point& p : points) {
        cout << p.x << "," << p.y << "," << p.cluster << "\n";
    }
}

void KMeans::inicializeCentroids(){
    mt19937 rng(42);
    vector<int> indices(points.size());
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), rng);

    for(int i = 0; i<k; i++){
        int idx = indices[i];
        centroids.push_back(Point(points[idx].x, points[idx].y));
    }
}
void KMeans::printCentroids() {
    for (int i = 0; i < centroids.size(); i++) {
        cout << "C" << i << ": ("
             << centroids[i].x << ", "
             << centroids[i].y << ")\n";
    }

    cout << endl;
}

bool KMeans::assignClusters(){
    bool changed = 0;
    for (auto& p : points) {
        double bestDist = numeric_limits<double>::max();
        int idx = -1;
        for (int i = 0; i < k; i++) {
            double dist = p.dist(centroids[i]);
            if (dist < bestDist) {
                bestDist = dist;
                idx = i;
            }
        }
        if(p.cluster != idx) changed = 1;
        p.cluster = idx;
    }
    return changed;
}

bool KMeans::updateCentroids() {
    bool changed = false;

    for (int i = 0; i < k; i++) {

        double sumX = 0;
        double sumY = 0;
        int n = 0;

        for (auto& p : points) {
            if (p.cluster == i) {
                sumX += p.x;
                sumY += p.y;
                n++;
            }
        }

        if (n > 0) {
            double newX = sumX / n;
            double newY = sumY / n;

            if (newX != centroids[i].x || newY != centroids[i].y)
                changed = true;

            centroids[i].x = newX;
            centroids[i].y = newY;
        }
    }
    return changed;
}

void KMeans::clustering(){
    inicializeCentroids();
    printCentroids();
    for(int i = 0; i < maxIter; i++){
        if(!assignClusters() && !updateCentroids()) break;
    }
}

int main()
{
    vector<Point> points = {
        Point(1,2),
        Point(2,1),
        Point(2,3),
        Point(3,2),

        Point(8,8),
        Point(9,8),
        Point(8,9),
        Point(9,9),

        Point(15,2),
        Point(16,1),
        Point(16,3),
        Point(17,2)
    };
    KMeans k(2, points, 50);
    k.printPuntos();
    cout << "Centroides inicializados: "<<endl;
    //k.inicializeCentroids();
    k.clustering();
    k.printPuntos();
    cout << "Centroides despues: "<<endl;
    k.printCentroids();
    return 0;
}
