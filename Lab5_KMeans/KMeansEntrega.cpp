#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <numeric>

struct Point {
    double x, y;
    int cluster = -1;
};

using Dataset = std::vector<Point>;

double squaredDist(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}

struct KDNode {
    Point point;
    int index;
    int axis;
    KDNode* left = nullptr;
    KDNode* right = nullptr;
};

class KDTree {
public:
    KDNode* root = nullptr;

    void build(std::vector<Point>& centroids) {
        deleteTree(root);
        root = nullptr;
        std::vector<std::pair<Point, int>> pts;
        for (int i = 0; i < (int)centroids.size(); i++)
            pts.push_back({ centroids[i], i });
        root = buildRec(pts, 0, (int)pts.size() - 1, 0);
    }

    int nearestNeighbor(const Point& q) const {
        int bestIdx = -1;
        double bestDist = std::numeric_limits<double>::infinity();
        searchNN(root, q, bestIdx, bestDist);
        return bestIdx;
    }

    ~KDTree() { deleteTree(root); }

private:
    KDNode* buildRec(std::vector<std::pair<Point, int>>& pts, int lo, int hi, int depth) {
        if (lo > hi) return nullptr;
        int axis = depth % 2;
        int mid = (lo + hi) / 2;
        std::nth_element(pts.begin() + lo, pts.begin() + mid, pts.begin() + hi + 1,
            [axis](const auto& a, const auto& b) {
                return (axis == 0 ? a.first.x : a.first.y) < (axis == 0 ? b.first.x : b.first.y);
            });
        KDNode* node = new KDNode{ pts[mid].first, pts[mid].second, axis };
        node->left = buildRec(pts, lo, mid - 1, depth + 1);
        node->right = buildRec(pts, mid + 1, hi, depth + 1);
        return node;
    }

    void searchNN(KDNode* node, const Point& q, int& bestIdx, double& bestDist) const {
        if (!node) return;
        double d = squaredDist(q, node->point);
        if (d < bestDist) { bestDist = d; bestIdx = node->index; }
        double diff = (node->axis == 0) ? (q.x - node->point.x) : (q.y - node->point.y);
        KDNode* near = (diff <= 0) ? node->left : node->right;
        KDNode* far  = (diff <= 0) ? node->right : node->left;
        searchNN(near, q, bestIdx, bestDist);
        if (diff * diff < bestDist)
            searchNN(far, q, bestIdx, bestDist);
    }

    void deleteTree(KDNode* n) {
        if (!n) return;
        deleteTree(n->left);
        deleteTree(n->right);
        delete n;
    }
};

struct KMeans {
    enum class Mode { BruteForce, KDTree };

    int k;
    int maxIter;
    unsigned seed;
    Dataset data;
    std::vector<Point> centroids;
    int iterations;
    double timeMs;
    KDTree tree;

    KMeans(Dataset data_, int k_, int maxIter_ = 100, unsigned seed_ = 25)
        : k(k_), maxIter(maxIter_), seed(seed_), data(std::move(data_)), iterations(0), timeMs(0) {}

    void initCentroids() {
        std::mt19937 rng(seed);
        std::vector<int> indices(data.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), rng);
        centroids.resize(k);
        for (int i = 0; i < k; i++)
            centroids[i] = { data[indices[i]].x, data[indices[i]].y, -1 };
    }

    bool assignBruteForce() {
        bool changed = false;
        for (auto& p : data) {
            int best = 0;
            double bestDist = squaredDist(p, centroids[0]);
            for (int i = 1; i < k; i++) {
                double d = squaredDist(p, centroids[i]);
                if (d < bestDist) { bestDist = d; best = i; }
            }
            if (p.cluster != best) { p.cluster = best; changed = true; }
        }
        return changed;
    }

    bool assignKDTree() {
        tree.build(centroids);
        bool changed = false;
        for (auto& p : data) {
            int best = tree.nearestNeighbor(p);
            if (p.cluster != best) { p.cluster = best; changed = true; }
        }
        return changed;
    }

    bool recomputeCentroids() {
        std::vector<double> sumX(k, 0), sumY(k, 0);
        std::vector<int> count(k, 0);
        for (const auto& p : data) {
            if (p.cluster >= 0) {
                sumX[p.cluster] += p.x;
                sumY[p.cluster] += p.y;
                count[p.cluster]++;
            }
        }
        bool changed = false;
        for (int i = 0; i < k; i++) {
            if (count[i] > 0) {
                double nx = sumX[i] / count[i];
                double ny = sumY[i] / count[i];
                if (std::abs(nx - centroids[i].x) > 1e-9 || std::abs(ny - centroids[i].y) > 1e-9)
                    changed = true;
                centroids[i].x = nx;
                centroids[i].y = ny;
            }
        }
        return changed;
    }

    double totalInertia() {
        double inertia = 0.0;
        for (const auto& p : data)
            if (p.cluster >= 0)
                inertia += squaredDist(p, centroids[p.cluster]);
        return inertia;
    }

    void clustering(Mode mode) {
        initCentroids();
        auto t0 = std::chrono::high_resolution_clock::now();
        for (iterations = 0; iterations < maxIter; iterations++) {
            bool a = (mode == Mode::BruteForce) ? assignBruteForce() : assignKDTree();
            bool c = recomputeCentroids();
            if (!a && !c) break;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    void exportPoints(const std::string& filename) {
        std::ofstream f(filename);
        f << "x,y,cluster\n";
        for (const auto& p : data)
            f << p.x << "," << p.y << "," << p.cluster << "\n";
    }

    void exportCentroids(const std::string& filename) {
        std::ofstream f(filename);
        f << "cx,cy,cluster\n";
        for (int i = 0; i < (int)centroids.size(); i++)
            f << centroids[i].x << "," << centroids[i].y << "," << i << "\n";
    }
};

Dataset loadCSV(const std::string& filename, int maxPoints = -1) {
    Dataset data;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        double x, y; char comma;
        if (ss >> x >> comma >> y) {
            data.push_back({ x, y, -1 });
            if (maxPoints > 0 && (int)data.size() >= maxPoints) break;
        }
    }
    return data;
}

void experiment10Runs(const Dataset& data, int k = 18) {
    std::cout << "\n===== 10 ejecuciones con k=" << k << " =====\n";
    std::cout << "Run | BF_ms  | KD_ms  | BF_inercia      | KD_inercia      | Iters_BF | Iters_KD\n";
    std::cout << std::string(85, '-') << "\n";

    std::ofstream fBF("runs_brute_force.csv");
    std::ofstream fKD("runs_kdtree.csv");
    fBF << "run,time_ms,inertia,iterations\n";
    fKD << "run,time_ms,inertia,iterations\n";

    for (int run = 0; run < 10; run++) {
        unsigned seed = run * 137 + 42;

        KMeans kmBF(data, k, 100, seed);
        KMeans kmKD(data, k, 100, seed);
        kmBF.clustering(KMeans::Mode::BruteForce);
        kmKD.clustering(KMeans::Mode::KDTree);

        double bfIne = kmBF.totalInertia();
        double kdIne = kmKD.totalInertia();

        fBF << run + 1 << "," << kmBF.timeMs << "," << bfIne << "," << kmBF.iterations << "\n";
        fKD << run + 1 << "," << kmKD.timeMs << "," << kdIne << "," << kmKD.iterations << "\n";

        printf("  %2d | %6.2f | %6.2f | %15.2f | %15.2f | %8d | %8d\n",
            run + 1, kmBF.timeMs, kmKD.timeMs, bfIne, kdIne, kmBF.iterations, kmKD.iterations);

        if (run < 3) {
            kmBF.exportPoints("result_bf_run" + std::to_string(run + 1) + ".csv");
            kmBF.exportCentroids("centroids_bf_run" + std::to_string(run + 1) + ".csv");
            kmKD.exportPoints("result_kd_run" + std::to_string(run + 1) + ".csv");
            kmKD.exportCentroids("centroids_kd_run" + std::to_string(run + 1) + ".csv");
        }
    }
}

void experimentVaryN(const Dataset& fullData, int k, const std::vector<int>& nVals, const std::string& out) {
    std::ofstream f(out);
    f << "n,brute_ms,kdtree_ms\n";
    std::cout << "\n[k=" << k << "] variando n\n";
    for (int n : nVals) {
        Dataset sub(fullData.begin(), fullData.begin() + std::min(n, (int)fullData.size()));

        KMeans kmBF(sub, k);
        KMeans kmKD(sub, k);
        kmBF.clustering(KMeans::Mode::BruteForce);
        kmKD.clustering(KMeans::Mode::KDTree);

        f << n << "," << kmBF.timeMs << "," << kmKD.timeMs << "\n";
        printf("  n=%4d  BF=%.2f ms  KD=%.2f ms\n", n, kmBF.timeMs, kmKD.timeMs);
    }
}

void experimentVaryK(const Dataset& fullData, int n, const std::vector<int>& kVals, const std::string& out) {
    std::ofstream f(out);
    f << "k,brute_ms,kdtree_ms\n";
    Dataset sub(fullData.begin(), fullData.begin() + std::min(n, (int)fullData.size()));
    std::cout << "\n[n=" << n << "] variando k\n";
    for (int k : kVals) {
        KMeans kmBF(sub, k);
        KMeans kmKD(sub, k);
        kmBF.clustering(KMeans::Mode::BruteForce);
        kmKD.clustering(KMeans::Mode::KDTree);

        f << k << "," << kmBF.timeMs << "," << kmKD.timeMs << "\n";
        printf("  k=%3d  BF=%.2f ms  KD=%.2f ms\n", k, kmBF.timeMs, kmKD.timeMs);
    }
}

int main(int argc, char* argv[]) {
    std::string csvFile = "points2d.csv";
    if (argc > 1) csvFile = argv[1];

    Dataset fullData = loadCSV(csvFile);

    if (fullData.empty()) {
        std::cerr << "ERROR: No se pudo cargar '" << csvFile << "'\n";
        return 1;
    }
    std::cout << "Puntos cargados: " << fullData.size() << "\n";

    experiment10Runs(fullData, 18);

    std::vector<int> nVals = { 1000,1150,1300,1450,1600,1750,1900,2050,2200,2400 };
    for (int k : {5, 15, 25, 50, 75})
        experimentVaryN(fullData, k, nVals, "exp_varyN_k" + std::to_string(k) + ".csv");

    std::vector<int> kVals = { 5,15,25,50,75,100,125,150,200 };
    for (int n : {1000, 1450, 1900, 2400})
        experimentVaryK(fullData, n, kVals, "exp_varyK_n" + std::to_string(n) + ".csv");

    return 0;
}
