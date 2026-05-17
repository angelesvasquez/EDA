#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

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

struct MBR {
    double xMin, yMin;
    double xMax, yMax;

public:
    MBR() : xMin(-1.0), yMin(-1.0), xMax(-1.0), yMax(-1.0) {};
    MBR(double xMin_, double xMax_, double yMin_, double yMax_) :
        xMin(xMin_), yMin(yMin_), xMax(xMax_), yMax(yMax_) {
    };

    double getArea() {
        return (xMax - xMin) * (yMax - yMin);
    }
    MBR combine(MBR& B) {
        MBR J = *this;
        if (B.xMin < J.xMin) J.xMin = B.xMin;
        if (B.xMax > J.xMax) J.xMax = B.xMax;
        if (B.yMin < J.yMin) J.yMin = B.yMin;
        if (B.yMax > J.yMax) J.yMax = B.yMax;
        return J;
    }
    double enlargement(MBR B) {
        double oldArea = getArea();
        MBR A = combine(B);
        double newArea = A.getArea() - oldArea;
        return newArea;
    }

    // Calcular su propia area
    // Ver si choca con otro rectangulo Overlap (search)
    // Calcular cuanto creceria el rect A si incluyera el rect B (insert)
};


struct Entry {
    MBR mbr;
    Node* childPointer;
    int tupleId;
    Entry(MBR mbr_ = MBR(), Node* chP = nullptr, int tpId = 0) {
        mbr = mbr_;
        childPointer = chP;
        tupleId = tpId;
    }
};

// Nodo hoja o interno
struct Node {
    MBR mbrNode;
    vector<Entry> entries;
    bool isLeaf;
    Node* parent;
    Node(vector<Entry> ent, MBR mbr_ = MBR(), bool isLeaf_ = true, Node* parnt = nullptr) {
        entries = ent;
        isLeaf = isLeaf_;
        mbrNode = mbr_;
        parent = parnt;
    }
};

enum Axis { X_AXIS, Y_AXIS };

struct CompareAxis {
    Axis axis;
    bool operator()(const Entry& A, const Entry& B) const {
        if (axis == X_AXIS) {
            return A.mbr.xMin + A.mbr.xMax < B.mbr.xMin + B.mbr.xMax;
        }
        else {
            return A.mbr.yMin + A.mbr.yMax < B.mbr.yMin + B.mbr.yMax;
        }
    }
};

class Rtree {
    int M, m;
    Node* root;

    int pickNext(MBR r1, MBR r2, vector<Entry>& entries) {
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

    pair<int, int> linearPickSeeds(vector<Entry> entries) {
        pair<int, int> seeds;
        double MaxX = std::numeric_limits<double>::lowest(),
            MaxY = std::numeric_limits<double>::lowest(),
            MinY = std::numeric_limits<double>::max(),
            MinX = std::numeric_limits<double>::max();

        double lowestHighX = std::numeric_limits<double>::max(); // x max mas bajo
        double highestLowX = std::numeric_limits<double>::lowest(); // x min mas alto
        double lowestHighY = std::numeric_limits<double>::max(); // y max mas bajo
        double highestLowY = std::numeric_limits<double>::lowest(); // y min mas alto

        int idLHX = 0, idHLX = 0, idLHY = 0, idHLY = 0;

        for (int i = 0; i < entries.size(); i++) {
            if (entries[i].mbr.xMax < lowestHighX) {
                lowestHighX = entries[i].mbr.xMax;
                idLHX = i;
            }
            if (entries[i].mbr.xMin > highestLowX) {
                highestLowX = entries[i].mbr.xMin;
                idHLX = i;
            }
            if (entries[i].mbr.yMax < lowestHighY) {
                lowestHighY = entries[i].mbr.yMax;
                idLHY = i;
            }
            if (entries[i].mbr.yMin > highestLowY) {
                highestLowY = entries[i].mbr.yMin;
                idHLY = i;
            }
            MaxX = max(MaxX, entries[i].mbr.xMax);
            MinX = min(MinX, entries[i].mbr.xMin);
            MaxY = max(MaxY, entries[i].mbr.yMax);
            MinY = min(MinY, entries[i].mbr.yMin);
        }

        double widthX = MaxX - MinX;
        double widthY = MaxY - MinY;

        double gapX = (widthX == 0) ? 0 : (highestLowX - lowestHighX) / widthX;
        double gapY = (widthY == 0) ? 0 : (highestLowY - lowestHighY) / widthY;

        if (gapX > gapY) return { idHLX, idLHX };
        else return { idHLY, idLHY };
    }

    Node* linearSplit(Node* L) {
        vector<Entry> entries = L->entries;
        pair<int, int> seeds = linearPickSeeds(entries);
        pair<vector<Entry>, MBR> g1 = { {entries[seeds.first]}, entries[seeds.first].mbr };
        pair<vector<Entry>, MBR> g2 = { {entries[seeds.second]}, entries[seeds.second].mbr };

        if (seeds.first > seeds.second) swap(seeds.first, seeds.second);
        entries.erase(entries.begin() + seeds.second);
        entries.erase(entries.begin() + seeds.first);

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
        Node* n2 = new Node(g2.first, g2.second, L->isLeaf);
        return n2;
    }

    MBR calcularMBREntries(vector<Entry> entries) {
        if (entries.empty()) return MBR();
        MBR mbr = entries[0].mbr;
        for (int i = 1; i < entries.size(); i++) mbr = mbr.combine(entries[i].mbr);
        return mbr;
    }

    Node* exhaustiveSplit(Node* L) {
        double bestArea = std::numeric_limits<double>::infinity();
        int n = L->entries.size();
        vector<Entry> bestG1, bestG2;
        for (int i = 0; i < (1 << n); i++) {
            vector<Entry> g1, g2;
            for (int j = 0; j < n; j++) {
                if (i >> j & 1) g1.push_back(L->entries[j]);
                else g2.push_back(L->entries[j]);
            }
            if (g1.size() < m || g2.size() < m) continue;
            MBR mbr1 = calcularMBREntries(g1);
            MBR mbr2 = calcularMBREntries(g2);

            double area = mbr1.getArea() + mbr2.getArea();
            if (area < bestArea) {
                bestG1 = g1; bestG2 = g2; bestArea = area;
            }
        }

        L->entries = bestG1;
        L->mbrNode = calcularMBREntries(bestG1);
        Node* n2 = new Node(bestG2, calcularMBREntries(bestG2), L->isLeaf);
        return n2;
    }

    pair<int, int> pickSeeds(vector<Entry>& entries) {
        double d = -std::numeric_limits<double>::infinity();
        pair<int, int> seeds;
        for (int i = 0; i < entries.size() - 1; i++) {
            MBR I;
            for (int j = i + 1; j < entries.size(); j++) {
                I = entries[i].mbr;
                MBR J = entries[j].mbr;
                I = I.combine(J);
                double dActual = I.getArea() - entries[i].mbr.getArea() - J.getArea();
                if (dActual > d) {
                    d = dActual;
                    seeds = { i, j };
                }
            }
        }
        return seeds;
    }
    Node* quadraticSplit(Node* L) {
        vector<Entry> entries = L->entries;
        pair<int, int> seeds = pickSeeds(entries);
        pair<vector<Entry>, MBR> g1 = { {entries[seeds.first]}, entries[seeds.first].mbr };
        pair<vector<Entry>, MBR> g2 = { {entries[seeds.second]}, entries[seeds.second].mbr };
        /*for (auto it = entries.begin(); it != entries.end(); ) {
            if ((*it).tupleId == seeds.first.tupleId || (*it).tupleId == seeds.second.tupleId) {
                it = entries.erase(it);
            }
            else ++it;
        }*/
        //vector<bool> visited;
        if (seeds.first > seeds.second) swap(seeds.first, seeds.second);
        entries.erase(entries.begin() + seeds.second);
        entries.erase(entries.begin() + seeds.first);
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
        Node* n2 = new Node(g2.first, g2.second, L->isLeaf);
        return n2;
    }

    int chooseAxis(vector<Entry> entries) {
        pair<int, int> seeds = pickSeeds(entries);
        MBR mbr1 = entries[seeds.first].mbr;
        MBR mbr2 = entries[seeds.second].mbr;
        double c1x = (mbr1.xMax + mbr1.xMin) / 2;
        double c2x = (mbr2.xMax + mbr2.xMin) / 2;
        double distx = abs(c1x - c2x);        
        
        double c1y = (mbr1.yMax + mbr1.yMin) / 2;
        double c2y = (mbr2.yMax + mbr2.yMin) / 2;
        double disty = abs(c1y - c2y);
        
        // range x = x global max - x global min
        double xMax = std::numeric_limits<double>::lowest();
        double xMin = std::numeric_limits<double>::max();
        double yMax = std::numeric_limits<double>::lowest();
        double yMin = std::numeric_limits<double>::max();
        for (auto& e : entries) {
            xMax = max(e.mbr.xMax, xMax);
            xMin = min(e.mbr.xMin, xMin);
            yMax = max(e.mbr.yMax, yMax);
            yMin = min(e.mbr.yMin, yMin);
         }
        double rangeX = xMax - xMin;
        double rangeY = yMax - yMin;
        distx = (rangeX == 0) ? 0 : distx / rangeX;
        disty = (rangeY == 0) ? 0 : disty / rangeY;
        
        if (distx > disty) return 0;
        else return 1;
    }

    Node* distributeGreene(Node* L, vector<Entry> entries, int axis) {
        //ordenar las entradas
        if (axis == X_AXIS) sort(entries.begin(), entries.end(), CompareAxis{ X_AXIS });
        else sort(entries.begin(), entries.end(), CompareAxis{ Y_AXIS });
        
        vector<Entry> g1, g2;
        int n = entries.size();
        for (int i = 0; i < n / 2; i++) g1.push_back(entries[i]);
        if (n % 2 == 1) {
            Entry extra = entries[n/2];
            for (int i = n / 2 + 1; i < n; i++) g2.push_back(entries[i]);
            MBR mbr1 = calcularMBREntries(g1);
            MBR mbr2 = calcularMBREntries(g2);
            double exp1 = mbr1.enlargement(extra.mbr);
            double exp2 = mbr2.enlargement(extra.mbr);
            if (exp1 < exp2) g1.push_back(extra);
            else g2.push_back(extra);
        }
        else {
            for (int i = n/2; i < n; i++) g2.push_back(entries[i]);
        }
        L->entries = g1;
        L->mbrNode = calcularMBREntries(g1);
        Node* LL = new Node(g2, calcularMBREntries(g2), L->isLeaf);
        return LL;
    }

    Node* greeneSplit(Node* L) {
        int axis = chooseAxis(L->entries);
        return distributeGreene(L, L->entries, axis);
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
            if (n == root) {
                if (nn != nullptr) {
                    Entry e1(n->mbrNode, n), e2(nn->mbrNode, nn);
                    vector<Entry> entries;
                    entries.push_back(e1);
                    entries.push_back(e2);
                    MBR newMbr = n->mbrNode.combine(nn->mbrNode);
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
            if (ep) ep->mbr = n->mbrNode;

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

                    pp = quadraticSplit(p);
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
        cout << (node->isLeaf ? "Leaf" : "Internal")<<endl;

        // // imprimir MBR del nodo
        // cout << " | Node MBR: ";
        // cout << "(" << node->mbrNode.xMin << "," << node->mbrNode.xMax << ") - ";
        // cout << "(" << node->mbrNode.yMin << "," << node->mbrNode.yMax << ")"<<endl;
        // //cout << " | addr: " << node << " | parent: " << node->parent << "\n" << endl;

        for (auto& e : node->entries) {

            for (int i = 0; i < level; i++) cout << "  ";
            cout << "  |__ Entry MBR: ";
            cout << "(" << e.mbr.xMin << "," << e.mbr.xMax << ") - ";
            cout << "(" << e.mbr.yMin << "," << e.mbr.yMax << ")";

            if (node->isLeaf) {
                cout << " | id: " << e.tupleId;
            }
            cout << endl;
            /*else {
                cout << " | child: " << e.childPointer << endl;
            }*/

            // recursión
            if (!node->isLeaf && e.childPointer != nullptr) {
                printTree(e.childPointer, level + 1);
            }
        }
    }

    bool overlap(MBR& A, MBR& B) {
        return !(A.xMax < B.xMin || A.xMin > B.xMax ||
            A.yMax < B.yMin || A.yMin > B.yMax);
    }
    Node* findLeaf(Node* T, Entry e) {
        if (!T->isLeaf) {
            // Buscar en subarboles
            for (auto& et : T->entries) {
                // ver el overlap
                if (overlap(e.mbr, et.mbr)) {
                    Node* n = findLeaf(et.childPointer, e);
                    if (n != nullptr) return n;
                }
            }
        }
        else {
            // Buscar el registro en el nodo hoja
            for (int i = 0; i < T->entries.size(); i++) {
                if (T->entries[i].tupleId == e.tupleId) {
                    return T;
                }
            }
            return nullptr;
        }
        return nullptr;
    }
    void extractLeaves(Node* n, vector<Entry>& leaves) {
        if (n->isLeaf) {
            for (auto& e : n->entries) {
                leaves.push_back(e);
            }
        }
        else {
            for (auto& e : n->entries) {
                extractLeaves(e.childPointer, leaves);
            }
        }
        delete n;
    }
    void condenseTree(Node* L) {
        // propagar los cambios por el nodo eliminado
        Node* n = L;
        vector<Node*> deleteNodes;
        while (true) {
            if (n == root) break;

            else {
                Node* p = n->parent;
                Entry* ep = nullptr;
                for (int i = 0; i < p->entries.size(); i++) {
                    if (n == p->entries[i].childPointer) ep = &p->entries[i];
                }
                if (n->entries.size() < m) {
                    deleteNodes.push_back(n);
                    int idx = 0;
                    for (int i = 0; i < p->entries.size(); i++) {
                        if (n == p->entries[i].childPointer) p->entries.erase(p->entries.begin() + i);
                    }

                }
                else {
                    // reajustar mbr
                    MBR newR = n->entries[0].mbr;
                    for (int i = 1; i < n->entries.size(); i++) {
                        newR = newR.combine(n->entries[i].mbr);
                    }
                    if (ep) ep->mbr = newR;
                    n->mbrNode = newR;
                }
                n = p;
            }
        }
        // reinsertar entradas de q
        for (auto n : deleteNodes) {
            vector<Entry> leaves;
            extractLeaves(n, leaves);
            for (auto& e : leaves) {
                insert(e);
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
            root = new Node(entries, e.mbr);
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
            Node* LL = quadraticSplit(l);
            adjustTree(l, LL);
        }
    }
    void remove(Entry e) {
        Node* L = findLeaf(root, e);
        if (!L) return;
        for (int i = 0; i < L->entries.size(); i++) {
            if (L->entries[i].tupleId == e.tupleId) L->entries.erase(L->entries.begin() + i);
        }
        // L puntero al nodo donde se elimino la entrada e
        condenseTree(L);
        if (root->entries.size() == 1 && !root->isLeaf) {
            Node* oldRoot = root;
            root = root->entries[0].childPointer;
            root->parent = nullptr;
            delete oldRoot;
        }
        else if (root->entries.empty()) {
            delete root;
            root = nullptr;
        }
    }
    void print() {
        if (root) printTree(root);
        else cout << "Arbol vacio" << endl;
    }

};

int main() {

    Rtree t(3);

    vector<Entry> entradas = {
        Entry(MBR(1, 1, 9, 9), nullptr, 0),
        Entry(MBR(2, 2, 10, 10), nullptr, 1),
        Entry(MBR(4, 4, 8, 8), nullptr, 2),
        Entry(MBR(6, 6, 7, 7), nullptr, 3),
        Entry(MBR(9, 9, 10, 10), nullptr, 4),
        Entry(MBR(7, 7, 5, 5), nullptr, 5),
        Entry(MBR(5, 5, 6, 6), nullptr, 6),
        Entry(MBR(4, 4, 3, 3), nullptr, 7),
        Entry(MBR(3, 3, 2, 2), nullptr, 8)
    };

    for (auto& e : entradas) {
        t.insert(e);
    }

    t.print();

   
    return 0;
}