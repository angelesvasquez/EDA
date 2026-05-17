#include <iostream>
#include <vector>
using namespace std;

/*
    Se te proporciona un vector llamado rectangles donde rectangles[i] = [li, hi] indica que el rectángulo i-ésimo tiene una longitud de li y una altura de
    hi. La esquina inferior izquierda de cada rectángulo se encuentra en las coordenadas (0, 0) y la esquina superior derecha en (li, hi).

    También se te da un vector llamado points, donde points[j] = [xj, yj] representa un punto con coordenadas (xj, yj).

    Escribe la función countRectangles en C++ que calcule cuántos rectángulos contienen cada punto dado. La función debe devolver un vector c,
    donde c[j] es el número de rectángulos que contienen al punto j. Se dice que un rectángulo contiene un punto si el punto (xj, yj) cumple con las
    siguientes condiciones: 0 <= xj <= li y 0 <= yj <= hi. Ten en cuenta que los puntos que se encuentran exactamente en los bordes de un rectángulo
    también se consideran contenidos por dicho rectángulo.

    La función está definida de la siguiente manera (ver archivo adjunto):

    vector<int> countRectangles(vector<vector<int>>& rectangles, vector<vector<int>>& points) {
    // Tu codigo va aqui
    }

    Restricciones de los datos de prueba:
    · 1 <= li, xj <= 10^9: Las longitudes y las coordenadas en el eje x pueden ser muy grandes.
    · 1 <= hi, yj <= 100: Las alturas y las coordenadas en el eje y están limitadas a 100.

    La función countRectangles debe tener una complejidad menor a O(p*r) donde p es la cantidad puntos en el vector points y r es la cantidad
    de elementos en el vector rectangles.

    Ejemplos:

    Input: rectangles = [[1,2],[2,3],[2,5]], points = [[2,1],[1,4]]
    Output: [2,1]
    Explicación: El punto [2, 1] está contenido en los dos primeros rectángulos, por lo que el resultado para este punto es 2. El punto [1, 4] solo está
    contenido en el tercer rectángulo, por lo que el resultado para este punto es 1.

    Input: rectangles = [[1,1],[2,2],[3,3]], points = [[1,3],[1,1]]
    Output: [1,3]
    Explicación: El punto [1, 3] solo está contenido en el tercer rectángulo. El punto [1, 1] está contenido en todos los rectángulos.

*/

vector<int> countRectangles(vector<vector<int>>& rectangles, vector<vector<int>>& points) {
    vector<int> c(points.size());
    for(int i = 0; i<points.size();i++){
        for(int j = 0; j<rectangles.size();j++){
            if(0<= points[i][0] && points[i][0] <= rectangles[j][0] &&
               0<= points[i][1] && points[i][1] <= rectangles[j][1]){
                c[i]++;
            }
        }
    }
    return c;
}

int main() {
    vector<vector<int>> r = {{1,1}, {2,2}, {3,3}};
    vector<vector<int>> p = {{1,3}, {1,1}};
    vector<int> c = countRectangles(r,p);
    for(auto& count : c){
        cout<<count<<" ";
    }
}