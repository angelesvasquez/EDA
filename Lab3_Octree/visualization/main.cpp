#include "Octree.h"

int main() {
    Point bt;
    double h;

    std::string archivo = "../visualization/data/1M_Hogwarts.xyz";

    if (!calcularBoundsXYZ(archivo, bt, h)) return 1;

    Octree ot(bt, h);

    if (!leerXYZ(ot, archivo)) return 1;

    std::string filename = "../visualization/obj/1M/octree_1M_Hog_" + std::to_string(CAPACITY) + ".obj";
    ot.exportOBJ(filename);

    return 0;
}
