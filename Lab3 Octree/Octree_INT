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

const int CAPACITY = 4;

struct Point {
    int x, y, z;
    Point() : x(-1), y(-1), z(-1) {}
    Point(int a, int b, int c) : x(a), y(b), z(c) {}
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
    Octree* children[8];
    std::vector<Point> points;
    Point bottomLeft; // esquina inf izq
    double h; // altura del cubo (lado)

    bool contains(const Point& p) {
        return(p.x <= bottomLeft.x + h && p.x >= bottomLeft.x &&
            p.y <= bottomLeft.y + h && p.y >= bottomLeft.y &&
            p.z <= bottomLeft.z + h && p.z >= bottomLeft.z);
    }
    //obtiene el índice del hijo en base a un punto usando el centro del cubo
    int getChildIndex(Point p) {
        Point mid(bottomLeft.x + h / 2, bottomLeft.y + h / 2, bottomLeft.z + h / 2);
        int n = 0;
        if (p.x >= mid.x) n += 1;
        if (p.y >= mid.y) n += 2;
        if (p.z >= mid.z) n += 4;
        return n;
    }
public:
    Octree(Point bt, double h);
    bool exist(const Point&);
    void insert(const Point&);
    bool find_closest(const Point& A, int radius, Point& xp, double& hN);
    void print(Octree* node, int level = 0, int idxch = -1);
    void printRoot();
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
*/

bool Octree::exist(const Point& p_obj) {
    if (!contains(p_obj)) return 0;
    if (!children[0]) {
        for (auto& p : points) {
            if (p_obj.x == p.x && p_obj.y == p.y && p_obj.z == p.z) return 1;
        }
        return 0;
    }
    else {
        int idx = getChildIndex(p_obj);
        return children[idx]->exist(p_obj);
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
    // verifica si el punto esta dentro del cubo actual
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
                int idx = getChildIndex(po);
                children[idx]->insert(po);
            }
        }
    }
    else {
        int idx = getChildIndex(p);
        children[idx]->insert(p);
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
// hN: lado del cubo donde esta el punto más cercano
bool Octree::find_closest(const Point& A, int radius, Point& xp, double& hN) {
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
        if(found) hN = h;
        return found;
    }
    // tiene hijos
    else {
        double bestDistance = radius;
        Point bestPoint(0, 0, 0);
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

        if (dist > radius * radius) return 0;
        bool found = 0;
        for (int i = 0; i < 8; i++) {
            Point xpl; double hC;
            if (children[i]->find_closest(A, radius, xpl, hC)) {
                double d = distanciaEuclidiana(A, xpl);
                if (!found || d < distanciaEuclidiana(A, xp)) {
                    xp = xpl; found = 1; hN = hC;
                }
            }
        }
        return found;
    }
}

void Octree::print(Octree* node, int level, int idxch) {
    std::string indent(level * 4, ' ');
    if (!node->children[0]) {
        //if (node->points.empty()) return;
        std::cout << indent <<"Hijo [" << idxch << "] - (bt = (" << node->bottomLeft.x <<  "," << node->bottomLeft.y << "," << node->bottomLeft.z << "), puntos: " << node->points.size() << ")" << std::endl;
        for (auto& p : node->points) {
            std::cout << indent << "   (" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
        }
    }
    else {
        if (level == 0) {
            std::cout << "RAIZ (h: " << node->h << ")" << std::endl;
        }
        else std::cout << indent << "Hijo [" << idxch << "] -> INTERNO (h: " << node->h << ")" << std::endl;
        for (int i = 0; i < 8; i++) {
            print(node->children[i], level + 1, i);
        }
    }
}

void Octree::printRoot() {
    std::cout << "Raiz: bottomLeft = (" << bottomLeft.x << ","
        << bottomLeft.y << "," << bottomLeft.z << ")"
        << " h = " << h << std::endl;
}

int main()
{
   /* Point bt(0, 0, 0);
    Octree ot(bt, 4);
    ot.insert(Point(1, 1, 1));
    ot.insert(Point(1, 2, 1));
    ot.insert(Point(0, 0, 0));
    ot.insert(Point(2, 1, 1));
    ot.insert(Point(3, 3, 3));
    ot.insert(Point(2, 3, 3));
    ot.insert(Point(2, 3, 1));
    ot.print(&ot);
    if (ot.exist(Point(1, 1, 1))) std::cout << "SI";
    else std::cout << "NO";
    Point resultado;
    double hN;
    if (ot.find_closest(Point(2, 2, 2), 25, resultado, hN)) {
        std::cout << "X = (" << resultado.x << "," << resultado.y << "," << resultado.z << ")";
        std::cout << "altura del cubo: " << hN;
    }
    else std::cout << "NULL";*/
    Point bt(0, 0, 0);
    Octree ot(bt, 8);
    ot.insert(Point(1, 1, 1)); ot.insert(Point(6, 1, 1));
    ot.insert(Point(1, 6, 1)); ot.insert(Point(6, 6, 1));
    ot.insert(Point(1, 1, 6)); ot.insert(Point(6, 1, 6));
    ot.insert(Point(1, 6, 6)); ot.insert(Point(6, 6, 6));

    ot.insert(Point(0, 0, 0)); ot.insert(Point(0, 0, 1));
    ot.insert(Point(0, 1, 0)); ot.insert(Point(1, 0, 0));
    ot.insert(Point(1, 1, 1)); ot.insert(Point(1, 1, 0));
    ot.insert(Point(1, 0, 1)); ot.insert(Point(0, 1, 1));
    ot.insert(Point(2, 2, 2)); ot.insert(Point(2, 2, 3));
    ot.insert(Point(2, 3, 2)); ot.insert(Point(3, 2, 2));
    ot.insert(Point(3, 3, 3)); ot.insert(Point(3, 3, 2));
    ot.insert(Point(3, 2, 3)); ot.insert(Point(2, 3, 3));

    ot.insert(Point(4, 4, 4)); ot.insert(Point(4, 0, 0));
    ot.insert(Point(0, 4, 0)); ot.insert(Point(0, 0, 4));
    ot.insert(Point(4, 4, 0)); ot.insert(Point(4, 0, 4));
    ot.insert(Point(0, 4, 4)); ot.insert(Point(2, 4, 2));

    ot.insert(Point(7, 0, 0)); ot.insert(Point(0, 7, 0));
    ot.insert(Point(0, 0, 7)); ot.insert(Point(7, 7, 0));
    ot.insert(Point(7, 0, 7)); ot.insert(Point(0, 7, 7));
    ot.insert(Point(5, 5, 0)); ot.insert(Point(0, 5, 5));

    ot.print(&ot);
    ot.printRoot();
    if (ot.exist(Point(1, 1, 1))) std::cout << "SI";
    else std::cout << "NO";
    Point resultado;
    double hN;
    if (ot.find_closest(Point(1, 1, 2), 5, resultado, hN)) {
        std::cout << "X = (" << resultado.x << "," << resultado.y << "," << resultado.z << ")";
        std::cout << "altura del cubo: " << hN;
    }
    else std::cout << "NULL";
}
