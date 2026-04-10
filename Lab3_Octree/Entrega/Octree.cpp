#include "Octree.h"

double distanciaEuclidiana(Point p, Point q) {
    double suma = pow(p.x - q.x, 2) + pow(p.y - q.y, 2) + pow(p.z - q.z, 2);
    return sqrt(suma);
}

Octree::Octree(Point bt, double h_) {
    bottomLeft = bt;
    h = h_;
    for (int i = 0; i < 8; i++) {
        children[i] = nullptr;
    }
}

Octree::~Octree() {
    for (int i = 0; i < 8; i++) {
        if (children[i]) delete children[i];
    }
}

bool Octree::contains(const Point& p) {
    return (p.x <= bottomLeft.x + h && p.x >= bottomLeft.x &&
            p.y <= bottomLeft.y + h && p.y >= bottomLeft.y &&
            p.z <= bottomLeft.z + h && p.z >= bottomLeft.z);
}

//obtiene el índice del hijo en base a un punto usando el centro del cubo
int Octree::getChildIndex(Point p) {
    Point mid(bottomLeft.x + h / 2, bottomLeft.y + h / 2, bottomLeft.z + h / 2);
    int n = 0;
    if (p.x >= mid.x) n += 1;
    if (p.y >= mid.y) n += 2;
    if (p.z >= mid.z) n += 4;
    return n;
}

bool Octree::exist(const Point& p_obj) {
    if (!contains(p_obj)) return 0;
    if (!children[0]) {
        for (auto& p : points) {
            if (p_obj.x == p.x && p_obj.y == p.y && p_obj.z == p.z) return 1;
        }
        return 0;
    }
    return children[getChildIndex(p_obj)]->exist(p_obj);
}

void Octree::insert(const Point& p) {
    // verifica si el punto esta dentro del cubo actual
    if (!contains(p)) return;
    
    // nodo hoja
    if (!children[0]) {
        if (points.size() < CAPACITY) {
            points.push_back(p);
        } else {
            //subdividir el nodo
            for (int i = 0; i < 8; i++) {
                Point bt = bottomLeft;
                // cada bit de i (0-7) se usa para calcular el bt de cada hijo
                if (i & 1) bt.x += h / 2;
                if (i & 2) bt.y += h / 2;
                if (i & 4) bt.z += h / 2;
                children[i] = new Octree(bt, h / 2);
            }
            points.push_back(p);
            std::vector<Point> cpoints = points;
            points.clear();

            // redistribuir los puntos en los hijos
            for (auto& po : cpoints) {
                children[getChildIndex(po)]->insert(po);
            }
        }
    } else {
        children[getChildIndex(p)]->insert(p);
    }
}

bool Octree::find_closest(const Point& A, double radius, Point& xp, double& hN) {
    // nodo hoja
    if (!children[0]) {
        bool found = 0;
        double bestDistance = radius; //ignora puntos fuera del radio
        // revisa todos los puntos del cubo
        for (auto& x : points) {
            double d = distanciaEuclidiana(A, x);
            if (d <= bestDistance) {
                bestDistance = d;
                xp = x; found = 1;
            }
        }
        if (found) hN = h;
        return found;
    }
    // tiene hijos
    else {
        double bestDistance = radius;
        double dist = 0; // distancia minima del punto A al cubo actual
        Point bt = bottomLeft;
        // caso 1: A esta a la izq del cubo
        if (A.x < bt.x) dist += (bt.x - A.x) * (bt.x - A.x); // dist al borde izq
        // caso 2: A esta a la der del cubo
        else if (A.x > bt.x + h) dist += ((bt.x + h) - A.x) * ((bt.x + h) - A.x);

        if (A.y < bt.y) dist += (bt.y - A.y) * (bt.y - A.y);
        else if (A.y > bt.y + h) dist += ((bt.y + h) - A.y) * ((bt.y + h) - A.y);

        if (A.z < bt.z) dist += (bt.z - A.z) * (bt.z - A.z);
        else if (A.z > bt.z + h) dist += ((bt.z + h) - A.z) * ((bt.z + h) - A.z);

        if (dist > bestDistance * bestDistance) return 0;
        bool found = 0;
        for (int i = 0; i < 8; i++) {
            Point xpl; double hC;
            if (children[i]->find_closest(A, bestDistance, xpl, hC)) {
                double d = distanciaEuclidiana(A, xpl);
                if (!found || d < distanciaEuclidiana(A, xp)) {
                    xp = xpl; found = 1; hN = hC;
                    bestDistance = d;
                }
            }
        }
        return found;
    }
}

void Octree::print(Octree* node, int level, int idxch) {
    std::string indent(level * 4, ' ');
    if (!node->children[0]) {
        if (node->points.empty()) return;
        std::cout << indent << "Hijo [" << idxch << "] - bt = (" << node->bottomLeft.x << "," << node->bottomLeft.y << "," << node->bottomLeft.z << ")" << std::endl;
        for (auto& p : node->points) std::cout << indent << "   (" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
    } else {
        if (level == 0) std::cout << "RAIZ (h: " << node->h << ")" << std::endl;
        else std::cout << indent << "Hijo [" << idxch << "] -> INTERNO (h: " << node->h << ")" << std::endl;
        for (int i = 0; i < 8; i++) print(node->children[i], level + 1, i);
    }
}

void Octree::printRoot() {
    std::cout << "Raiz: bt=(" << bottomLeft.x << "," << bottomLeft.y << "," << bottomLeft.z << ") h=" << h << std::endl;
}