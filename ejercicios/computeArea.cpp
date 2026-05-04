#include <iostream>
using namespace std;

/*
    COMPUTE AREA
    [Outcomes 1,2,6] Considere dos rectángulos alineados con los ejes cartesianos en un plano bidimensional., Cada rectángulo
    se representa mediante las coordenadas de su vértice inferior izquierdo y su vértice superior derecho:

    Rectangulo 1: (ax1, ay1) y (ax2, ay2).
    Rectangulo 2: (bx1, by1) y (bx2, by2).

    Escribe la función computeArea en C++ que calcule el área total cubierta por ambos rectángulos. Indique tambien la
    complejidad de su propuesta. La función está definida de la siguiente manera (ver archivo adjunto):

    int computeArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
        // Tu codigo va aqui
    }

    Usa el archivo adjunto para escribir tu función (DISPONIBLE AQUÍ). Indica la complejidad de tu solución. El ejercicio tendrá una
    nota de 0 en caso el código entregado no compile.

    Entregables:
    . Archivo [ApellidosNombres].cpp con la implementación
    · Complejidad de la solución en la caja de texto
*/

// int computeArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
//     // Tu codigo va aqui
//     int xmin = min(ax1,bx1);
//     int xmax = max(ax2,bx2);
//     int ymin = min(ay1,by1);
//     int ymax = max(ay2,by2);  
//     cout<<xmin<<" , "<<xmax<<" , "<<ymin<<" , "<<ymax<<endl;
//     return (xmax-xmin) * (ymax-ymin);
// }

int getArea(int x1, int y1, int x2, int y2){
    return (x2-x1) * (y2-y1);
}

bool overlap(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2){
    return !(ax2 < bx1 || ax1 > bx2 || ay2 < by1 || ay1 > by2);
}

int computeArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
    // Tu codigo va aqui
    int areaTotal = getArea(ax1,ay1,ax2,ay2) + getArea(bx1,by1,bx2,by2);
    if(overlap(ax1,ay1,ax2,ay2,bx1,by1,bx2,by2)){
        int x1, x2, y1, y2;
        if(ax1 < bx1){
            x1 = bx1;
            x2 = ax2;
        } else {
            x1 = ax1;
            x2 = bx2;
        }
        if(ay1 < by1){
            y1 = by1;
            y2 = ay2;
        } else {
            y1 = ay1;
            y2 = by2;
        }
        areaTotal -= getArea(x1,y1,x2,y2);
    }
    return areaTotal;
}

int main() {
    cout<<"Area: "<<computeArea(1,1,3,3,5,1,8,4)<<endl;
    cout<<"Area: "<<computeArea(-3,3,3,6,1,1,8,5)<<endl;
}