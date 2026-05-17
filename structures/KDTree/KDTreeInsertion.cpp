//KDTreeInsertion
/*
    KDTree por orden de insercion de los puntos
    Se alternan los ejes de forma normal
*/

#include <iostream>
#include <vector>
using namespace std;

using Coord = vector<double>;
const int K = 3; // Dimension

struct Node {
    Coord coords;
    int axis;
    Node* nodes[2];
    int level;
    Node(Coord c, int a, int l = 0) : coords(c), axis(a), level(l) { nodes[0] = nullptr; nodes[1] = nullptr; }
};

class KDTree {
    int k = 2;
    Node* root;
    void clear(Node* node);
    void preOrden(Node* n);
    bool findp(Coord& p, Node**& n);
    Node** findRemp(Node** p);
public:
    KDTree(int k_) : k(k_), root(nullptr) {}
    ~KDTree();
    void insert(Coord& p);
    bool remove(Coord x);
    bool find(Coord& x);
    void print();
};

Node** KDTree::findRemp(Node** p) {
    Node** t = &((*p)->nodes[0]);
    while ((*t)->nodes[1] != 0) {
        t = &(*t)->nodes[1];
    }
    return t;
}

bool KDTree::findp(Coord& x, Node**& p) {
    for(p = &root; *p && (*p)->coords != x; p = &((*p)->nodes[x[(*p)->axis] > (*p)->coords[(*p)->axis]]));
    return *p != 0;
}

bool KDTree::remove(Coord x) {
    Node** p;
    if(!findp(x, p)) return 0;

    if ((*p)->nodes[0] && (*p)->nodes[1]) {
        //caso 2
        Node** t = findRemp(p);
        (*p)->coords = (*t)->coords;
        p = t;
    }
    //Caso 0 y 1
    Node* temp = *p;
    *p = (*p)->nodes[(*p)->nodes[1] != 0];
    delete temp;
    return 1;
}

bool KDTree::find(Coord& x) {
    Node** p;
    if(findp(x, p)) return true;
    else return false;
}

void KDTree::preOrden(Node* n) {
    if (!n) return;
    cout << "( ";
    for (auto& c : n->coords) {
        cout << c << " ";
    }
    cout << ")" << endl;
    preOrden(n->nodes[0]);
    preOrden(n->nodes[1]);
}

void KDTree::print() {
    preOrden(root);
}

void KDTree::clear(Node* node) {
    if (!node) return;
    clear(node->nodes[0]);
    clear(node->nodes[1]);

    delete node;
}

KDTree::~KDTree() {
    clear(root);
}

void KDTree::insert(Coord& x) {
    if (!root) {
        Node* n = new Node(x, 0, 0);
        root = n;
        return;
    }
    else {
        Node* p = root;
        while (true) {
            if (x[p->axis] < p->coords[p->axis]) {
                if (p->nodes[0]) p = p->nodes[0];
                //insertar
                else {
                    int newLevel = p->level + 1;
                    Node* n = new Node(x, newLevel % k, newLevel);
                    p->nodes[0] = n;
                    break;
                }
            }
            else {
                if (p->nodes[1]) p = p->nodes[1];
                // insertar
                else {
                    int newLevel = p->level + 1;
                    Node* n = new Node(x, newLevel % k, newLevel);
                    p->nodes[1] = n;
                    break;
                }
            }

        }
    }
}

int main() {
    //vector<Coord> puntos = { {7,8}, {12,3}, {14,1}, {4,12}, {9,1}, {2,7}, {10,19} };
    vector<Coord> puntos = {
        { 2, 3, 1 },
        { 5, 4, 7 },
        { 9, 6, 2 },
        { 4, 7, 9 },
        { 8, 1, 5 },
        { 7, 2, 6 },
        { 6, 8, 3 },
        { 1, 5, 4 },
        { 3, 9, 8 },
        { 10, 10, 10 }
    };
    KDTree t(K);
    for (auto& p : puntos) {
        t.insert(p);
    }
    t.print();
    Coord c = { 3,9,8 };
    if (t.find(c)) cout << "Encontrado" << endl;
    else cout << "No se encontro" << endl;
    t.remove({ 4,7,9 });
    t.print();
}