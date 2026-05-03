#include <iostream>
#include <vector>
#include <limits>

using namespace std;

/*
    Cada objeto es un MBR (rectangulo envolvente minimo)
    Rectangulo n-dim I = (I_1, I_2, ... , I_n)
    donde I_i = [a,b]
    Nodos hoja: (I, tuple-identifier)
    Nodos internos : (I, child-pointer)
*/

int id = 0;

struct Node;

struct Rectangle {
    double xMin, yMin;
    double xMax, yMax;

public:
    Rectangle() : xMin(-1.0), yMin(-1.0), xMax(-1.0), yMax(-1.0) {};
    Rectangle(double xMin_, double xMax_, double yMin_, double yMax_) :
        xMin(xMin_), yMin(yMin_), xMax(xMax_), yMax(yMax_) {
    };

    double getArea() {
        return (xMax - xMin) * (yMax - yMin);
    }
    Rectangle combine(Rectangle& B) {
        Rectangle J = *this;
        if (B.xMin < J.xMin) J.xMin = B.xMin;
        if (B.xMax > J.xMax) J.xMax = B.xMax;
        if (B.yMin < J.yMin) J.yMin = B.yMin;
        if (B.yMax > J.yMax) J.yMax = B.yMax;
        return J;
    }
    double enlargement(Rectangle B) {
        double oldArea = getArea();
        Rectangle A = combine(B);
        double newArea = A.getArea() - oldArea;
        return newArea;
    }

    // Calcular su propia area
    // Ver si choca con otro rectangulo Overlap (search)
    // Calcular cuanto creceria el rect A si incluyera el rect B (insert)
};


struct Entry {
    Rectangle mbr;
    Node* childPointer;
    int tupleId;
    Entry(Rectangle mbr_ = Rectangle(), Node* chP = nullptr, int tpId = 0) {
        mbr = mbr_;
        childPointer = chP;
        tupleId = tpId;
    }
};

// Nodo hoja o interno
struct Node {
    Rectangle mbrNode;
    vector<Entry> entries;
    bool isLeaf;
    Node* parent;
    Node(vector<Entry> ent, Rectangle mbr_ = Rectangle(), bool isLeaf_ = true, Node* parnt = nullptr) {
        entries = ent;
        isLeaf = isLeaf_;
        mbrNode = mbr_;
        parent = parnt;
    }
};

class Rtree {
    int M, m;
    Node* root;
    
    pair<int, int> pickSeeds(vector<Entry>& entries) {
        double d = -std::numeric_limits<double>::infinity();
        pair<int, int> seeds;
        for (int i = 0; i < entries.size() - 1; i++) {
            Rectangle I;
            for (int j = i+1; j <= entries.size() - 1; j++) {
                I = entries[i].mbr;
                Rectangle J = entries[j].mbr;
                I = I.combine(J);
                double dActual = I.getArea() - entries[i].mbr.getArea() - J.getArea();
                if (dActual > d) {
                    d = dActual;
                    seeds = {i, j};
                }
            }
        }
        return seeds;
    }
    int pickNext(Rectangle r1, Rectangle r2, vector<Entry>& entries) {
        double diff = -std::numeric_limits<double>::infinity();
        int idxEntry = 0;
        for (int i = 0; i < entries.size(); i++) {
            double d1 = r1.enlargement(entries[i].mbr);
            double d2 = r2.enlargement(entries[i].mbr);
            if (diff < abs(d1 - d2)) {
                idxEntry = i;
                diff = abs(d1 - d2);
            }

            /*
            Resolver los empates añadiendo la entrada al grupo con área más pequeña,
            luego al que tenga menos entradas, luego a cualquiera de los dos.
            */
        }
        return idxEntry;
    }
    Node* splitNode(Node* L) {
        vector<Entry> entries = L->entries;
        pair<int, int> seeds = pickSeeds(entries);
        pair<vector<Entry>, Rectangle> g1 = { {entries[seeds.first]}, entries[seeds.first].mbr };
        pair<vector<Entry>, Rectangle> g2 = { {entries[seeds.second]}, entries[seeds.second].mbr };
        /*for (auto it = entries.begin(); it != entries.end(); ) {
            if ((*it).tupleId == seeds.first.tupleId || (*it).tupleId == seeds.second.tupleId) {
                it = entries.erase(it);
            }
            else ++it;
        }*/
        //vector<bool> visited;
        if (seeds.first > seeds.second) swap(seeds.first, seeds.second);
        entries.erase(entries.begin() + seeds.first);
        entries.erase(entries.begin() + seeds.second-1);
        /*for (int i = 0; i < entries.size(); i++) {
            if (i != seeds.first && i != seeds.second) entries.push_back(entries[i]);
        }*/
        
        while (!entries.empty()) {
            /*
            Si un grupo tiene tan pocas entradas que todas las demás deben asignarse a él
            para que tenga el número mínimo m, asignarlas y detenerse.
            */

            if (entries.size() + g1.first.size() == m) {
                for (int i = 0; i < entries.size(); i++) {
                    g1.first.push_back(entries[i]);
                    g1.second = g1.second.combine(entries[i].mbr);
                }
                break;
            }
            if (entries.size() + g2.first.size() == m) {
                for (int i = 0; i < entries.size(); i++) {
                    g2.first.push_back(entries[i]);
                    g2.second = g2.second.combine(entries[i].mbr);
                }
                break;
            }

            int idxNextEntry = pickNext(g1.second, g2.second, entries);

            double d1 = g1.second.enlargement(entries[idxNextEntry].mbr);
            double d2 = g2.second.enlargement(entries[idxNextEntry].mbr);
        
            if (d1 < d2 || 
               (d1 == d2 && (g1.second.getArea() < g2.second.getArea()) || 
               (d1 == d2 && g1.second.getArea() == g2.second.getArea() && g1.first.size() <= g2.first.size()))) {
                g1.first.push_back(entries[idxNextEntry]);
                g1.second = g1.second.combine(entries[idxNextEntry].mbr);
            }
            else {
                g2.first.push_back(entries[idxNextEntry]);
                g2.second = g2.second.combine(entries[idxNextEntry].mbr);
            }
            entries.erase(entries.begin() + idxNextEntry);
        }
        L->entries = g1.first;
        L->mbrNode = g1.second;
        Node* n2 = new Node(g2.first, g2.second,L->isLeaf);
        return n2;
    }
    Node* chooseLeaf(Entry e) {
        Node* n = root;
        while (true) {
            if (n->isLeaf) return n;
            double bestExpand = std::numeric_limits<double>::infinity();
            double bestArea = std::numeric_limits<double>::infinity();
            Entry f;
            Node* fpointer = nullptr;
            for (auto entry : n->entries) {
                double expand = entry.mbr.enlargement(e.mbr);
                double area = entry.mbr.getArea();
                if (expand < bestExpand || 
                   (expand == bestExpand && area < bestArea)) {
                    bestExpand = expand;
                    bestArea = area;
                    //f = entry;
                    fpointer = entry.childPointer;
                }
            }
            n = fpointer;
        }
    }
    void adjustTree(Node* l, Node* ll = nullptr) {
        Node* n = l;
        Node* nn = nullptr;
        if (ll) { nn = ll; }

        while (true) {
            if (n == root){
                if (nn != nullptr) {
                    Entry e1(n->mbrNode, n), e2(nn->mbrNode, nn);
                    vector<Entry> entries;
                    entries.push_back(e1);
                    entries.push_back(e2);
                    Rectangle newMbr = n->mbrNode.combine(nn->mbrNode);
                    Node* newRoot = new Node(entries, newMbr, false);
                    root = newRoot;
                    n->parent = root;
                    nn->parent = root;
                    
                }
                break;
            }
            Node* p = n->parent; // revisar como conectar el parent
            Entry* ep = nullptr;

            for (auto& e : p->entries) {
                if (e.childPointer == n) { ep = &e; break; }
            }
            Node* pp = nullptr;
            // ajustar el mbr de la entrada para q cubra n
            if(ep) ep->mbr = n->mbrNode;
            
            p->mbrNode = p->entries[0].mbr;
            for (auto& e : p->entries) {
                p->mbrNode = p->mbrNode.combine(e.mbr);
            }
            if (nn) {
                Entry ee(nn->mbrNode, nn);
                if (p->entries.size() < M) {
                    p->entries.push_back(ee);
                    ee.childPointer->parent = p;
                    p->mbrNode = p->entries[0].mbr;
                    for (auto& e : p->entries) {
                        p->mbrNode = p->mbrNode.combine(e.mbr);
                    }
                }
                else {
                    p->entries.push_back(ee);

                    p->mbrNode = p->entries[0].mbr;
                    for (auto& e : p->entries) {
                        p->mbrNode = p->mbrNode.combine(e.mbr);
                    }

                    pp = splitNode(p);
                    //nn->parent = pp;
                    for (int i = 0; i < p->entries.size(); i++) p->entries[i].childPointer->parent = p;
                    for (int i = 0; i < pp->entries.size(); i++) pp->entries[i].childPointer->parent = pp;
                    
                }
            }
            n = p; nn = pp;
        }
    }
    void printTree(Node* node, int level = 0) {
        if (!node) return;
            // indentación
            for (int i = 0; i < level; i++) cout << "  ";

        cout << "[Level " << level << "] ";
        cout << (node->isLeaf ? "Leaf" : "Internal");

        // imprimir MBR del nodo
        cout << " | Node MBR: ";
        cout << "(" << node->mbrNode.xMin << "," << node->mbrNode.xMax << ") - ";
        cout << "(" << node->mbrNode.yMin << "," << node->mbrNode.yMax << ")";
        cout << " | addr: " << node << " | parent: " << node->parent << "\n"<<endl;

        for (auto& e : node->entries) {

            for (int i = 0; i < level; i++) cout << "  ";
            cout << "  |__ Entry MBR: ";
            cout << "(" << e.mbr.xMin << "," << e.mbr.xMax << ") - ";
            cout << "(" << e.mbr.yMin << "," << e.mbr.yMax << ")";

            if (node->isLeaf) {
                cout << " | id: " << e.tupleId << endl;
            }
            else {
                cout << " | child: " << e.childPointer<<endl;
            }

            // recursión
            if (!node->isLeaf && e.childPointer != nullptr) {
                printTree(e.childPointer, level + 1);
            }
        }
    }
public:
    Rtree(int M_) {
        M = M_;
        m = M_ / 2;
        root = nullptr;
    }

    void insert(Entry e) {
        if (root == nullptr) {
            vector<Entry> entries;
            entries.push_back(e);
            root = new Node(entries,e.mbr);
            return;
        }
        Node* l = chooseLeaf(e);
        if (l->entries.size() < M) {
            l->entries.push_back(e);
            l->mbrNode = l->mbrNode.combine(e.mbr);
            adjustTree(l);
        }
        else {
            // OverFlow
            l->entries.push_back(e);
            Node* LL = splitNode(l);
            adjustTree(l, LL);
        }
    }
    void print() {
        printTree(root);
    }

};

int main() {
    Rtree t(4);

    vector<Rectangle> rects = {
        Rectangle(0, 3, 0, 2),
        Rectangle(1, 4, 1, 5),
        Rectangle(2, 6, 3, 7),
        Rectangle(8, 12, 8, 11),
        Rectangle(9, 13, 9, 14),
        Rectangle(4, 9, 0, 2)
    };

    for (auto& r : rects) {
        t.insert(Entry(r, nullptr, id++));
    }

    t.print();

    return 0;
}