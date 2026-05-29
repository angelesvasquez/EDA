#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

struct Point {
    double x, y;
    int cluster;
    Point(double x, double y) 
        : x(x), y(y), cluster(-1)
    {}
    Point() : x(0.0), y(0.0), cluster(-1) {}

    double distance(Point& p) {
        return sqrt((p.x-x)*(p.x-x) + (p.y-y)*(p.y-y));
    }
};

class KMeans {
    vector<Point> centroids;
    vector<Point> points;
    int k; // nro de centroides
    int maxIter;

    void inicializeCentroids();
    void updateCentroids();
public:
    KMeans(int k_, vector<Point> p, int maxiter);
    void clustering();
    void exportarPuntos();
    void exportarCentroides();

};
KMeans::KMeans(int k_, vector<Point> p, int maxiter) {
    k = k_;
    points = p;
    maxIter = maxiter;
}

void KMeans::inicializeCentroids() {
    /*for (int i = 0; i < k; i++) {
        double x = (double)rand() / RAND_MAX * 100;
        double y = (double)rand() / RAND_MAX * 50;
        centroids.push_back(Point(x, y));
    }*/
    for (int i = 0; i < k; i++) {
        int idx = rand() % points.size();
        centroids.push_back(Point(points[idx].x, points[idx].y));
    }
}

bool leerCSV(vector<Point>& points) {
    std::ifstream archivo("D://1 UCSP material//7//EDA//labs//EDA//Lab5_KMeans//points2d.csv");

    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 0;
    }

    std::string linea;

    if (!std::getline(archivo, linea)) {
        std::cerr << "El archivo está vacío." << std::endl;
        return 0;
    }

    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string valorX, valorY;

        if (std::getline(ss, valorX, ',') && std::getline(ss, valorY)) {
            try {
                Point p;
                p.x = std::stod(valorX);
                p.y = std::stod(valorY);
                points.push_back(p);
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Error al convertir datos numéricos en línea: " << linea << std::endl;
            }
        }
    }

    archivo.close();

    return 1;
}

void KMeans::updateCentroids() {
    for (int i = 0; i < k; i++) {
        double sumX = 0, sumY = 0;
        int n = 0;
        for (auto& p : points) {
            if (p.cluster == i) {
                sumX += p.x;
                sumY += p.y;
                n++;
            }
        }
        centroids[i].x = sumX / n;
        centroids[i].y = sumY / n;
    }
}

void KMeans::clustering() {
    inicializeCentroids();
    int iter = 0;
    bool changed = 1;
    while (iter < maxIter && changed) {
        if(iter == 0){
            exportarPuntos();
            exportarCentroides();
        }
            
        cout << "CENTROIDES" << endl;
        for (int i = 0; i < k; i++) {
            cout << "(" << centroids[i].x << ", " << centroids[i].y << ")"<<" ";        
        }
        // cout << "\nPUNTOS" << endl;
        // for (auto& p: points) {
        //     cout << "(" << p.x << ", " << p.y << ")" << " cluster: "<<p.cluster << endl;
        // }
        cout << endl;
        changed = 0;
        for (auto& p : points) {
            double bestDist = numeric_limits<double>::max();
            int idx = -1;
            for (int i = 0; i < k; i++) {
                double dist = p.distance(centroids[i]);
                if (dist < bestDist) {
                    bestDist = dist;
                    idx = i;
                }
            }
            if(p.cluster != idx) changed = 1;
            p.cluster = idx;
        }
        updateCentroids();
        iter++;
    }
}

void KMeans::exportarPuntos() {
    ofstream file("points.csv");

    file << "x,y,cluster\n";

    for (Point& p : points) {

        file << p.x << ","
            << p.y << ","
            << p.cluster << "\n";
    }
}

void KMeans::exportarCentroides() {
    ofstream file2("centroids.csv");

    file2 << "x,y\n";

    for (Point& c : centroids) {

        file2 << c.x << ","
            << c.y << "\n";
    }
}

int main() {
    srand(29);
    vector<Point> points;
    leerCSV(points);
    KMeans k(18, points, 50);
    k.clustering();
}