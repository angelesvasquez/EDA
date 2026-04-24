#include <iostream>
#include <vector>

/*
    m (impar): orden cant de hijos q puede tener cada nodo
    m-1 keys: cant de datos(keys) por nodo
    
*/

struct Node{
    std::vector<int> keys;
    std::vector<Node*> children;
    //Node(int k){ keys.push_back(k); }
    Node(){
        
    }
};

class Btree {
private:
    int m;
    Node* root;
    
public:
    Btree(int m_){
        root = nullptr;
        m = m_;
    }
    void insert(int k){
        if(!root) root = new Node(k);
        Node* p = root; int i = 0;
        int n = p->keys.size();
        while(true){
            while(i<n){ // recorre las llaves
                if(k < p->keys[i]) break;
                if(k > p->keys[n-1]) { i++; break;}
                // seguir avanzando
                i++; 
            }
            if(!p->children[0]){
                if(i<=n){
                    // si no tiene hijos insertar a la izq
                    int j = p->keys.size();
                    p->keys.push_back(p->keys[p->keys.size()-1]);
                    while(j>=i){
                        p->keys[j+1] = p->keys[j];
                        j--;
                    }
                    p->keys[i]=k;
                    return;
                }
                else p->keys.push_back(k);
            }
            else{ // mientras no sea hoja
                p = p->children[i];
                i = 0;
            }
        }
    }
    
    void remove(){
    
    }
    bool find(){
    
    }
    void printTree(Node* node, int level=0){
        if (node == nullptr) return;

        for (int i = 0; i < level; i++) {
            std::cout << "   ";
        }

        std::cout << "[ ";
        for (int k : node->keys) {
            std::cout << k << " ";
        }
        std::cout << "]\n";

        if (!node->children[0]) {
            for (Node* child : node->children) {
                printTree(child, level + 1);
            }
        }
    }
    void print(){
        printTree(root);
    }
};

int main() {
    Btree t(5);
    t.insert(1);
    t.insert(2);
    t.insert(8);
    t.insert(12);
    t.print();
}