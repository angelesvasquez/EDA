/*
    OCTREE
    Cada nodo interno tiene exactamente 8 hijos
    Un nodo representa un cubo en el espacio 3d,
    SI es hoja almacena puntos o
    SI esta subdividido tiene 8 hijos 
    si children[0] == nullptr es hoja
*/
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

const int CAPACITY = 1;


struct Point{
    float x, y, z;
    Point() : x(-1), y(-1), z(-1) {}
    Point(float a, float b, float c) : x(a), y(b), z(c) {}
};

double distanciaEuclidiana(Point p, Point q) {
    double suma = 0;
    suma += pow(p.x - q.x, 2);
    suma += pow(p.y - q.y, 2);
    suma += pow(p.z - q.z, 2);
    return sqrt(suma);
}

class Octree {
private:
    Octree *children[8];
    std::vector<Point> points;
    Point bottomLeft; // esquina inf izq
    double h; // altura del cubo (lado)

    bool contains(const Point& p) {
        return(p.x <= bottomLeft.x + h && p.x >= bottomLeft.x &&
               p.y <= bottomLeft.y + h && p.y >= bottomLeft.y &&
               p.z <= bottomLeft.z + h && p.z >= bottomLeft.z);
    }

    void subdivide() {

    }
public:
    Octree(Point bt, double h);
    bool exist(const Point&);
    void insert(const Point&);
    bool find_closest(const Point& A, int radius, Point& xp, double& hN);
    void print(Octree* node, int level = 0);
    void printRoot();
    void getLeafNodes(Octree* node, std::vector<std::pair<Point, double>>& leaves);
    void exportOBJ(const std::string& filename);
    void collectLeaves(Octree* node, std::vector<std::pair<Point, double>>& leaves);
};

Octree::Octree(Point bt, double h_) {
    bottomLeft = bt;
    h = h_;
    for (int i = 0; i < 8; i++) {
        children[i] = nullptr;
    }
}

/* 
    exist
    verificar si un punto esta en el octree
    si el nodo es hoja verificar en la lista de puntos, si no 
    buscar el hijo recursivamente con busqueda dirigida no revisar todo el arbol
    importante saber en q nodo esta para imprimir el h de ese nodo
*/ 

bool Octree::exist(const Point& p_obj) {
    if (!children[0]) {
        for (auto p: points) {
            if (p_obj.x == p.x && p_obj.y == p.y && p_obj.z == p.z) return 1;
        }
        return 0;
    }
    else {
        Point mid(bottomLeft.x + h / 2, bottomLeft.y + h / 2, bottomLeft.z + h / 2);
        int n = 0;
        if (p_obj.x >= mid.x) n += std::pow(2, 0);
        if (p_obj.y >= mid.y) n += std::pow(2, 1);
        if (p_obj.z >= mid.z) n += std::pow(2, 2);
        return children[n]->exist(p_obj);
    }
}

/*
    insert
    1 verificar si el punto esta dentro del cubo '
    2 si el nodo es hoja
    2.1 aun hay espacio se guarda el punto
    2.2 se llena se tiene q subdividir el nodo redistribuyendo
    los puntos existentes, e insertamos el nuevo punto
    3 si el nodo NO es hoja
    determinar en que hijo cae el punto
    llamar recursivamente a insert en ese hijo

    Subdivision
    crear 8 hijos si el cubo padre tiene h, los hijos tendran h/2

*/

void Octree::insert(const Point& p) {
    if (!contains(p)) return;
    // nodo hoja
    if (!children[0]) {
        if (points.size() < CAPACITY) {
            points.push_back(p);
            return;
        }
        else {
            //subdividir el nodo
            for (int i = 0; i < 8; i++) {
                Point bt = bottomLeft;
                if (i & 1) bt.x += h / 2;
                if (i & 2) bt.y += h / 2;
                if (i & 4) bt.z += h / 2;
                children[i] = new Octree(bt, h / 2);
            }
            points.push_back(p);
            Point mid(bottomLeft.x + h / 2, bottomLeft.y + h / 2, bottomLeft.z + h / 2);
            for (auto po : points) {
                int n = 0;
                if (po.x>=mid.x) n += std::pow(2,0);
                if (po.y>=mid.y) n += std::pow(2,1);
                if (po.z>=mid.z) n += std::pow(2,2);
                children[n]->points.push_back(po);
            }
            points.clear();
        }
    }
    else {
        Point mid(bottomLeft.x + h / 2, bottomLeft.y + h / 2, bottomLeft.z + h / 2);
        int n = 0;
        if (p.x >= mid.x) n += std::pow(2, 0);
        if (p.y >= mid.y) n += std::pow(2, 1);
        if (p.z >= mid.z) n += std::pow(2, 2);
        children[n]->insert(p);
    }
}

// busca el punto mas cercano a uno dado, dentro de un radio
// “Sea X el punto más cercano a A con radio 25 y N=100”
// recorrer el arbol solo considerando ese radio y 
// comparar las distancias quedandose con el minimo
/*
    verificar si el nodo actual puede servir
    si esa distancia es > a la mejor distancia descartado
    si no seguir explorando

*/
bool Octree::find_closest(const Point& A, int radius, Point& xp, double& hN) {
    if (!children[0]) {
        bool found = 0;
        double bestDistance = radius;
        for (auto& x : points) {
            double d = distanciaEuclidiana(A, x);
            if (d < bestDistance) {
                bestDistance = d;
                xp = x; found = 1; hN = h;
            }
        }
        return found;
    }
    else {
        double bestDistance = radius;
        Point bestPoint(0, 0, 0);
        double dist = 0;
        Point bt = bottomLeft;
        if (A.x < bt.x) dist += (bt.x - A.x) * (bt.x - A.x);
        else if (A.x > bt.x + h) dist += ((bt.x + h) - A.x) * ((bt.x + h) - A.x);

        if (A.y < bt.y) dist += (bt.y - A.y) * (bt.y - A.y);
        else if (A.y > bt.y + h) dist += ((bt.y + h) - A.y) * ((bt.y + h) - A.y);

        if (A.z < bt.z) dist += (bt.z - A.z) * (bt.z - A.z);
        else if (A.z > bt.z + h) dist += ((bt.z + h) - A.z) * ((bt.z + h) - A.z);
        
        if (dist > radius * radius) return 0;
        bool found = 0;
        for (int i = 0; i < 8; i++) {
            Point c; double hC;
            if (children[i]->find_closest(A, radius, c, hC)) {
                double d = distanciaEuclidiana(A, c);
                if (!found || d < distanciaEuclidiana(A, xp)) {
                    xp = c; found = 1; hN = hC;
                }
            }
        }
        return found;
    }
}

void Octree::print(Octree *node, int level) {
    for (int i = 0; i < level; i++) std::cout << " ";
    if (!node->children[0]) {
        std::cout << "\tHoja con " << node->points.size()<< " puntos" << std::endl;
        for (auto& p : node->points) {
            for (int i = 0; i < level; i++) std::cout << " ";
            std::cout << "\t(" << p.x << "," << p.y << "," << p.z << ")"<<std::endl;
        }
    }
    else {
        std::cout << "Nodo interno" << std::endl;
        for (int i = 0; i < 8; i++) {
            print(node->children[i], level + 1);
        }
    }
}

void Octree::printRoot(){
    std::cout<<"Raiz: btLeft = ("<<bottomLeft.x<<","
             <<bottomLeft.y<<","<<bottomLeft.z<<")"
             <<" h = "<<h<<std::endl;
}

void Octree::getLeafNodes(Octree* node, std::vector<std::pair<Point, double>>& leaves) {
    if (!node->children[0]) {
        if (!node->points.empty())  // solo hojas con puntos
            leaves.push_back({ node->bottomLeft, node->h });
    }
    else {
        for (int i = 0; i < 8; i++)
            getLeafNodes(node->children[i], leaves);
    }
}

void Octree::collectLeaves(Octree* node, std::vector<std::pair<Point, double>>& leaves) {
    if (!node->children[0]) {
        if (!node->points.empty())
            leaves.push_back({ node->bottomLeft, node->h });
    }
    else {
        for (int i = 0; i < 8; i++)
            collectLeaves(node->children[i], leaves);
    }
}

void Octree::exportOBJ(const std::string& filename) {
    std::vector<std::pair<Point, double>> leaves;
    collectLeaves(this, leaves);

    std::ofstream file(filename);
    int vOffset = 1; // OBJ indexa vértices desde 1

    for (auto& leaf : leaves) {
        float x = leaf.first.x;
        float y = leaf.first.y;
        float z = leaf.first.z;
        float s = leaf.second; // lado del cubo

        // 8 vértices del cubo
        file << "v " << x << " " << y << " " << z << "\n";
        file << "v " << x + s << " " << y << " " << z << "\n";
        file << "v " << x + s << " " << y + s << " " << z << "\n";
        file << "v " << x << " " << y + s << " " << z << "\n";
        file << "v " << x << " " << y << " " << z + s << "\n";
        file << "v " << x + s << " " << y << " " << z + s << "\n";
        file << "v " << x + s << " " << y + s << " " << z + s << "\n";
        file << "v " << x << " " << y + s << " " << z + s << "\n";

        // 6 caras (cada cara = 2 triángulos)
        int v = vOffset;
        file << "f " << v << " " << v + 1 << " " << v + 2 << " " << v + 3 << "\n"; // frente
        file << "f " << v + 4 << " " << v + 5 << " " << v + 6 << " " << v + 7 << "\n"; // atrás
        file << "f " << v << " " << v + 1 << " " << v + 5 << " " << v + 4 << "\n"; // abajo
        file << "f " << v + 2 << " " << v + 3 << " " << v + 7 << " " << v + 6 << "\n"; // arriba
        file << "f " << v << " " << v + 3 << " " << v + 7 << " " << v + 4 << "\n"; // izq
        file << "f " << v + 1 << " " << v + 2 << " " << v + 6 << " " << v + 5 << "\n"; // der

        vOffset += 8;
    }
    std::cout << "OBJ exportado: " << filename << " (" << leaves.size() << " cubos)\n";
}

bool getRange(const std::string& filename, float& minX, float& minY, float& minZ,
    float& maxX, float& maxY, float& maxZ) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir '" << filename << "'\n";
        return false;
    }
    minX = minY = minZ = FLT_MAX;
    maxX = maxY = maxZ = -FLT_MAX;
    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        float x, y, z;
        if (!(ss >> x >> y >> z)) continue;
        minX = std::min(minX, x); maxX = std::max(maxX, x);
        minY = std::min(minY, y); maxY = std::max(maxY, y);
        minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);
        count++;
    }
    if (count == 0) {
        std::cerr << "Error: archivo vacio\n";
        return false;
    }
    std::cout << "Puntos leidos: " << count << "\n";
    std::cout << "X: [" << minX << ", " << maxX << "]\n";
    std::cout << "Y: [" << minY << ", " << maxY << "]\n";
    std::cout << "Z: [" << minZ << ", " << maxZ << "]\n";
    return true;
}

bool loadXYZ(const std::string& filename, Octree& ot) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir '" << filename << "'\n";
        return false;
    }
    std::string line;
    int count = 0, skipped = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        float x, y, z;
        if (!(ss >> x >> y >> z)) {
            skipped++;
            continue;
        }
        ot.insert(Point(x, y, z));
        count++;
    }
    std::cout << "Insertados: " << count << " puntos";
    if (skipped > 0) std::cout << " (" << skipped << " lineas ignoradas)";
    std::cout << "\n";
    return true;
}

int main() {
    const std::string archivo = "D://Lenovo//Descargas//aguila.xyz";

    float minX, minY, minZ, maxX, maxY, maxZ;
    if (!getRange(archivo, minX, minY, minZ, maxX, maxY, maxZ)) return 1;

    float rangoX = maxX - minX;
    float rangoY = maxY - minY;
    float rangoZ = maxZ - minZ;

    float cx = (minX + maxX) / 2.0f;
    float cy = (minY + maxY) / 2.0f;
    float cz = (minZ + maxZ) / 2.0f;

    float h = std::max({ rangoX, rangoY, rangoZ }) + 0.001f;


    float btX = cx - h / 2.0f;
    float btY = cy - h / 2.0f;
    float btZ = cz - h / 2.0f;
    std::cout << "centro=(" << cx << "," << cy << "," << cz << ")\n";
    std::cout << "bottomLeft=(" << btX << "," << btY << "," << btZ << ") h=" << h << "\n";

    Point bt(btX, btY, btZ);
    Octree ot(bt, h);

    if (!loadXYZ(archivo, ot)) return 1;

    ot.exportOBJ("octree.obj");

    return 0;
}
