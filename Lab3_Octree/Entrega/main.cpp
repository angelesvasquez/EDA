#include "Octree.h"

int main() {
    Point bt(0, 0, 0);
    Octree ot(bt, 8);

    ot.insert(Point(1, 1, 1));
    ot.insert(Point(7, 7, 7));
    ot.insert(Point(1, 7, 1));
    ot.insert(Point(0, 0, 0));
    ot.insert(Point(0, 0, 1));
    ot.insert(Point(0, 1, 0));
    ot.insert(Point(1, 0, 0));

    ot.print(&ot);
    
    std::cout << "\n--------------------------------------------" << std::endl;

    Point p(1, 1, 1);
    if (ot.exist(p)) {
        std::cout << "El punto (1,1,1) existe en el arbol." << std::endl;
    }

    Point obj(1, 1, 2);
    Point res;
    double hN;
    double radio = 3;

    std::cout << "Buscando cerca de (1,1,2) con radio " << radio << "..." << std::endl;
    if (ot.find_closest(obj, radio, res, hN)) {
        std::cout << "Punto mas cercano encontrado: (" << res.x << ","
            << res.y << "," << res.z << ")" << std::endl;
        std::cout << "Encontrado en un nodo de altura h = " << hN << std::endl;
    }
    else {
        std::cout << "No se encontraron puntos en el radio de busqueda." << std::endl;
    }

    if (!ot.find_closest(Point(7, 0, 7), 1, res, hN)) {
        std::cout << "No hay puntos cerca de (7,0,7) en un radio de 1." << std::endl;
    }

    return 0;
}