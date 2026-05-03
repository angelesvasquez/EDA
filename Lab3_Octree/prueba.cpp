#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int& xpto(int sinal){
    int p = 4;
    if(!sinal) {
        p*=sinal;    
    }
    else{
        p++;        
    }
    return p;
}

void ypto(){
    int c[1000];
    int aux;
    for(aux = 0; aux < 1000; aux++){
        c[aux] = aux;    
    }
}

int main() {
    int a = 1;
    int& b = xpto(a);
    ypto();
    cout<< b;
    return 0;
}