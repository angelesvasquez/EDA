#include "Octree.h"

int main() {
    Point bt;
    double h;

    std::string archivo = "../visualization/data/10M_Hogw.xyz";

    if (!calcularBoundsXYZ(archivo, bt, h)) return 1;

    Octree ot(bt, h);

    if (!leerXYZ(ot, archivo)) return 1;

    std::string filename = "../visualization/obj/10M/octree_10M_Hog_" + std::to_string(CAPACITY) + ".obj";
    ot.exportOBJ(filename);

    return 0;
}
