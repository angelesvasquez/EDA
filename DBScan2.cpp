#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

// Parámetros importantes:
// 1. eps         : Distancia máxima entre dos puntos para que se consideren vecinos.
// 2. min_samples : Cantidad mínima de puntos necesarios para formar una región densa.
// 3. core_point  : Punto con suficientes vecinos alrededor.
// Pasos:
//   Escoge punto
//   Busca vecinos
//   Si hay suficiente densidad: crea cluster
//   Expande el cluster
//   Si no hay suficientes vecinos: marca ruido

struct Point {
    std::vector<double> coords;
};

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool load_csv(const std::string& filename, std::vector<Point>& points) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo '" << filename << "'\n";
        return false;
    }
    std::string line;
    int expectedDim = -1;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;

        std::istringstream ss(trimmed);
        std::string token;
        std::vector<double> coords;
        bool malformada = false;

        while (std::getline(ss, token, ',')) {
            std::string v = trim(token);
            if (v.empty()) continue;
            try { coords.push_back(std::stod(v)); }
            catch (...) { malformada = true; break; }
        }

        if (malformada || coords.empty()) continue;

        if (expectedDim == -1) {
            expectedDim = (int)coords.size();
        } else if ((int)coords.size() != expectedDim) {
            std::cerr << "Aviso: linea " << lineNumber << " tiene " << coords.size()
                       << " dimensiones, se esperaban " << expectedDim
                       << ". Se ignora esta fila.\n";
            continue;
        }

        points.push_back({ coords });
    }
    return !points.empty();
}

bool save_csv(const std::string& filename,
    const std::vector<Point>& points,
    const std::vector<int>& labels) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo crear el archivo '" << filename << "'\n";
        return false;
    }

    if (!points.empty()) {
        for (size_t d = 0; d < points[0].coords.size(); d++) file << "x" << d << ",";
    }
    file << "cluster\n";

    for (int i = 0; i < (int)points.size(); i++) {
        for (double c : points[i].coords) file << c << ",";
        file << labels[i] << "\n";
    }
    return true;
}

static const inline double distance(const Point& a, const Point& b) {
    double sum = 0;
    for (size_t i = 0; i < a.coords.size(); i++) {
        double d = b.coords[i] - a.coords[i];
        sum += d*d;
    }
    return sqrt(sum);
}

const inline int region_query(const std::vector<Point>& input, int p,
    std::vector<int>& output, double eps) {
    for (int i = 0; i < (int)input.size(); i++) {
        if (distance(input[i], input[p]) < eps) {
            output.push_back(i);
        }
    }
    return output.size();
}

bool expand_cluster(const std::vector<Point>& input, int p,
    std::vector<int>& output, int cluster,
    double eps, int min) {
    std::vector<int> seeds;
    if (region_query(input, p, seeds, eps) < min) {
        output[p] = -1;
        return false;
    }
    for (int i = 0; i < (int)seeds.size(); i++) {
        output[seeds[i]] = cluster;
    }
    seeds.erase(std::remove(seeds.begin(), seeds.end(), p), seeds.end());

    while ((int)seeds.size() > 0) {
        int cp = seeds.front();
        std::vector<int> result;
        if (region_query(input, cp, result, eps) >= min) {
            for (int i = 0; i < (int)result.size(); i++) {
                int rp = result[i];
                if (output[rp] < 1) {
                    if (!output[rp]) seeds.push_back(rp);
                    output[rp] = cluster;
                }
            }
        }
        seeds.erase(std::remove(seeds.begin(), seeds.end(), cp), seeds.end());
    }
    return true;
}

int dbscan(const std::vector<Point>& input, std::vector<int>& labels,
    double eps, int min) {
    int size = input.size();
    int cluster = 1;
    std::vector<int> state(size, 0);
    for (int i = 0; i < size; i++) {
        if (!state[i]) {
            if (expand_cluster(input, i, state, cluster, eps, min)) {
                cluster++;
            }
        }
    }
    labels = state;
    return cluster - 1;
}

int main(int argc, char* argv[]) {

    // Parámetros con valores por defecto
    std::string input_file = "puntos_dbscan_circulos.csv";
    std::string output_file = "results2.csv";
    double eps = 0.2;
    int    min = 3;

    if (argc > 1) input_file = argv[1];
    if (argc > 2) output_file = argv[2];
    if (argc > 3) eps = std::stod(argv[3]);
    if (argc > 4) min = std::stoi(argv[4]);

    std::cout << "Leyendo puntos desde: " << input_file << "\n";
    std::vector<Point> points;
    if (!load_csv(input_file, points)) {
        return 1;
    }
    std::cout << "Puntos cargados: " << points.size()
               << " (dimension " << points[0].coords.size() << ")\n";
    std::cout << "eps=" << eps << "  min_samples=" << min << "\n\n";

    std::vector<int> labels;
    int num = dbscan(points, labels, eps, min);

    std::cout << "Clusters encontrados: " << num << "\n";
    for (int i = 0; i < (int)points.size(); i++) {
        std::cout << "Point(";
        for (size_t d = 0; d < points[i].coords.size(); d++) {
            std::cout << points[i].coords[d];
            if (d + 1 < points[i].coords.size()) std::cout << ", ";
        }
        std::cout << "): cluster=" << labels[i]
            << (labels[i] == -1 ? "  [RUIDO]" : "") << "\n";
    }

    if (save_csv(output_file, points, labels)) {
        std::cout << "\nResultados guardados en: " << output_file << "\n";
    }

    return 0;
}
