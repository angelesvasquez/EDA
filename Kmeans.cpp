#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>

using namespace std;

class Point{
public:
    double x, y;
    int cluster = -1;
    Point(double x, double y): x(x), y(y) {}
    double dist(Point &A){
        return sqrt((A.x-x)*(A.x-x)+(A.y-y)*(A.y-y));
    }

};

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
