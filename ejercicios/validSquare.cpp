#include <iostream>
#include <vector>
#include <map>
using namespace std;

/*
    Lab (Out1, Out2, Out6)

    Dadas las coordenadas de cuatro puntos en el espacio 2D p1, p2, p3 y p4; implementa la funcion validSquare en C++ que devuelve
    verdadero si los cuatro puntos construyen un cuadrado.
    La coordenada de un punto se representa como un vector [x, y].
    Los puntos de entrada a la función no siguen ningun orden.
    Un cuadrado válido tiene cuatro lados iguales con longitud positiva y cuatro angulos iguales (angulos de 90 grados).

    bool validSquare(vector<int>& p1, vector<int>& p2, vector<int>& p3, vector<int>& p4){
    // Tu codigo va aqui

    Ejemplo 2:
    Input: p1 = [0,0], p2 = [1,1], p3 = [1,0], p4 = [0,12]
    Output: false

    Ejemplo 3:
    Input: p1 = [1,0], p2 =[-1,0], p3=[0,1], p4 = [0,-1]
    Output: true

    Indica la complejidad de tu solución.
*/


// Ejemplo 1:
// Input: p1 = [0,0], p2 = [1,1], p3 = [1,0], p4 =[0,1]
// Output: true

bool validSquare(vector<int>& p1, vector<int>& p2, vector<int>& p3, vector<int>& p4){
    // Tu codigo va aqui
    vector<vector<int>> p = {p1,p2,p3,p4};
    map<int,int> freq;
    for(int i = 0; i < 4; i++){
        for(int j = i + 1; j < 4; j++){
            int dist = (p[i][0] - p[j][0]) * (p[i][0] - p[j][0]) +
                       (p[i][1] - p[j][1]) * (p[i][1] - p[j][1]);
            freq[dist]++;
        }
    }
    if(freq.size() != 2) return false;
    for(auto& [dist,count] : freq){
        if(count != 2 && count != 4) return false;
        if(dist == 0) return false;   
    }
    return true;
}

int main() {
    vector<int> p1 = {-1,0}, p2 = {0,-1}, p3 = {1,0}, p4 = {0,1};
    cout << validSquare(p1, p2, p3, p4) << endl;
}