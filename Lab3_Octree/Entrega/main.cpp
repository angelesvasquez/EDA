#include "Octree.h"

int main() {

    //leerCSV(ot, "C://Users//alumno-b305//Downloads//Octree//points2.csv");

    Point bt;
    double h;

    std::string archivo = "D://Lenovo//Descargas//points2.csv";

    if (!calcularBoundsCSV(archivo, bt, h)) return 1;

    Octree ot(bt, h);
    leerCSV(ot, archivo);

    //ot.print(&ot);

    std::cout << "bt = (" << bt.x << ", " << bt.y << ", " << bt.z << ")" << std::endl;
    std::cout << "h  = " << h << std::endl;
    ot.printRoot();
    /*ot.insert(Point(1, 1, 1));
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
    */

    //1.1) Sea X el punto más cercano a (18,28,175) con un radio de 10 y con un N=1, cual es el valor de X (si no existe escriba NULL). 
    std::cout << "Buscando cerca de (18,28,175) con radio " << 10 << "..." << std::endl;
    Point res; double hN;
    if (ot.find_closest(Point(18,28,175), 10, res, hN)) {
        std::cout << "Punto mas cercano encontrado: (" << res.x << ","
            << res.y << "," << res.z << ")" << std::endl;
        std::cout << "Encontrado en un nodo de altura h = " << hN << std::endl;
    }
    else {
        std::cout << "No se encontraron puntos en el radio de busqueda." << std::endl;
    }
    return 0;
}
