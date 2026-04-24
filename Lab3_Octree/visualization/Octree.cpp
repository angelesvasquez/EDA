#include "Octree.h"

bool leerCSV(Octree& tree, const std::string& dir) {
    std::ifstream archivo(dir);
    std::string linea;

    if (!archivo.is_open()) {
        std::cout << "Error al abrir el archivo" << std::endl;
        return false;
    }

    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string x_str, y_str, z_str;

        if (std::getline(ss, x_str, ',') &&
            std::getline(ss, y_str, ',') &&
            std::getline(ss, z_str, ',')) {

            int x = std::stod(x_str);
            int y = std::stod(y_str);
            int z = std::stod(z_str);

            tree.insert(Point(x, y, z));
        }
    }

    archivo.close();
    return true;
}

bool calcularBoundsCSV(const std::string& dir, Point& bt, double& h) {
    std::ifstream archivo(dir);
    
    if (!archivo.is_open()) {
        std::cout << "Error al abrir el archivo" << dir <<std::endl;
        return false;
    }
    
    double minX = DBL_MAX, minY = DBL_MAX, minZ = DBL_MAX;
    double maxX = -DBL_MAX, maxY = -DBL_MAX, maxZ = -DBL_MAX;
    
    std::string linea;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        std::string xs, ys, zs;

        if (std::getline(ss, xs, ',') && std::getline(ss, ys, ',') && std::getline(ss, zs, ',')) {
            double x = std::stod(xs);
            double y = std::stod(ys);
            double z = std::stod(zs);

            minX = std::min(minX, x); maxX = std::max(maxX, x);
            minY = std::min(minY, y); maxY = std::max(maxY, y);
            minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);
        }
    }

    archivo.close();

    double margin = 1e-6;
    bt = Point(minX - margin, minY - margin, minZ - margin);
    double rx = (maxX - minX) + 2 * margin;
    double ry = (maxY - minY) + 2 * margin;
    double rz = (maxZ - minZ) + 2 * margin;
    h = std::max({rx, ry, rz});
    return true;
}

double distanciaEuclidiana(Point p, Point q) {
    return sqrt((p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y) + (p.z - q.z)*(p.z - q.z));
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
    return (p.x < bottomLeft.x + h && p.x >= bottomLeft.x &&
            p.y < bottomLeft.y + h && p.y >= bottomLeft.y &&
            p.z < bottomLeft.z + h && p.z >= bottomLeft.z);
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

bool Octree::find_closest(const Point& A, double radius, Point& xp, double& hN, Point& btN) {
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
        if (found) { hN = h; btN = bottomLeft;} 
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
            Point xpl; double hC; Point btC;
            if (children[i]->find_closest(A, bestDistance, xpl, hC, btC)) {
                double d = distanciaEuclidiana(A, xpl);
                if (!found || d < distanciaEuclidiana(A, xp)) {
                    xp = xpl; found = 1; hN = hC;
                    bestDistance = d;
                    btN = btC;
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

void Octree::collectLeaves(Octree* node, std::vector<std::pair<Point,double>>& leaves, double h_root) {
    if (!node) return;

    if (!node->children[0]) {
        if (!node->points.empty()) {
            //if (node->h > h_root * 0.01) return; // filtrado
            leaves.push_back({node->bottomLeft, node->h});
        }
    } else {
        for (int i = 0; i < 8; i++) {
            collectLeaves(node->children[i], leaves, h_root);
        }
    }
}

void Octree::exportOBJ(const std::string& filename) {
    std::vector<std::pair<Point,double>> leaves;
    collectLeaves(this, leaves, this->h);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error al crear archivo\n";
        return;
    }

    int vOffset = 1;

    for (auto& leaf : leaves) {
        double x = leaf.first.x;
        double y = leaf.first.y;
        double z = leaf.first.z;
        double s = leaf.second;

        //vertices
        file << "v " << x << " " << y << " " << z << "\n";
        file << "v " << x+s << " " << y << " " << z << "\n";
        file << "v " << x+s << " " << y+s << " " << z << "\n";
        file << "v " << x << " " << y+s << " " << z << "\n";
        file << "v " << x << " " << y << " " << z+s << "\n";
        file << "v " << x+s << " " << y << " " << z+s << "\n";
        file << "v " << x+s << " " << y+s << " " << z+s << "\n";
        file << "v " << x << " " << y+s << " " << z+s << "\n";

        int v = vOffset;

        //caras
        file << "f " << v << " " << v+1 << " " << v+2 << " " << v+3 << "\n";
        file << "f " << v+4 << " " << v+5 << " " << v+6 << " " << v+7 << "\n";
        file << "f " << v << " " << v+1 << " " << v+5 << " " << v+4 << "\n";
        file << "f " << v+2 << " " << v+3 << " " << v+7 << " " << v+6 << "\n";
        file << "f " << v << " " << v+3 << " " << v+7 << " " << v+4 << "\n";
        file << "f " << v+1 << " " << v+2 << " " << v+6 << " " << v+5 << "\n";

        vOffset += 8;
    }

    file.close();
    std::cout << "OBJ generado: " << filename << "\n";
    std::cout << "Cubos: " << leaves.size() << "\n";
}

bool leerXYZ(Octree& tree, const std::string& dir) {
    std::ifstream archivo(dir);
    if (!archivo.is_open()) {
        std::cout << "Error al abrir el archivo: " << dir << std::endl;
        return false;
    }

    std::string linea;
    int count = 0;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::istringstream ss(linea);
        double x, y, z;

        if (ss >> x >> y >> z) {
            tree.insert(Point(x, y, z));
            count++;
        }
    }

    std::cout << "Puntos insertados: " << count << std::endl;
    archivo.close();
    return true;
}

bool calcularBoundsXYZ(const std::string& dir, Point& bt, double& h) {
    std::ifstream archivo(dir);
    if (!archivo.is_open()) {
        std::cout << "Error al abrir el archivo: " << dir << std::endl;
        return false;
    }

    double minX = DBL_MAX, minY = DBL_MAX, minZ = DBL_MAX;
    double maxX = -DBL_MAX, maxY = -DBL_MAX, maxZ = -DBL_MAX;

    std::string linea;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::istringstream ss(linea);
        double x, y, z;

        if (ss >> x >> y >> z) {
            minX = std::min(minX, x); maxX = std::max(maxX, x);
            minY = std::min(minY, y); maxY = std::max(maxY, y);
            minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);
        }
    }

    archivo.close();

    double margin = 1e-6;
    bt = Point(minX - margin, minY - margin, minZ - margin);

    double rx = (maxX - minX) + 2 * margin;
    double ry = (maxY - minY) + 2 * margin;
    double rz = (maxZ - minZ) + 2 * margin;

    h = std::max({rx, ry, rz});

    return true;
}