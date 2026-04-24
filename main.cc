#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;

struct Point { double x, y; };
struct Obstacle { double x, y, width, depth; };
struct CeilingRegion { double x, height; };
struct Box { double x, y, w, d; };

struct BayType {
    int id;
    double width, depth, height, gap, price;
    // Según el PDF, queremos el área máxima al menor precio [cite: 11]
    double getEff() const { return (width * depth) / price; }
};

struct PlacedBay { int id; double x, y, rot; };

struct WarehouseProblem {
    vector<Point> warehousePolygon;
    vector<Obstacle> obstacles;
    vector<CeilingRegion> ceiling;
    vector<BayType> bayTypes;
};

// --- PARSERS (Se mantienen igual para no romper tu flujo) ---
vector<Point> pWH(const string& f) {
    vector<Point> p; ifstream file(f); string l;
    while (getline(file, l)) {
        stringstream ss(l); string x, y;
        if (getline(ss, x, ',') && getline(ss, y, ',')) p.push_back({stod(x), stod(y)});
    }
    return p;
}
vector<Obstacle> pObs(const string& f) {
    vector<Obstacle> obs; ifstream file(f); string l;
    while (getline(file, l)) {
        stringstream ss(l); string x, y, w, d;
        if (getline(ss, x, ',') && getline(ss, y, ',') && getline(ss, w, ',') && getline(ss, d, ','))
            obs.push_back({stod(x), stod(y), stod(w), stod(d)});
    }
    return obs;
}
vector<CeilingRegion> pC(const string& f) {
    vector<CeilingRegion> c; ifstream file(f); string l;
    while (getline(file, l)) {
        stringstream ss(l); string x, h;
        if (getline(ss, x, ',') && getline(ss, h, ',')) c.push_back({stod(x), stod(h)});
    }
    return c;
}
vector<BayType> pBT(const string& f) {
    vector<BayType> b; ifstream file(f); string l;
    while (getline(file, l)) {
        stringstream ss(l); string id, w, d, h, g, nl, pr;
        if (getline(ss, id, ',') && getline(ss, w, ',') && getline(ss, d, ',') && 
            getline(ss, h, ',') && getline(ss, g, ',') && getline(ss, nl, ',') && getline(ss, pr, ','))
            b.push_back({stoi(id), stod(w), stod(d), stod(h), stod(g), stod(pr)});
    }
    return b;
}

// --- FÍSICA ---
bool ptIn(double x, double y, const vector<Point>& poly) {
    bool in = false;
    for (size_t i = 0, j = poly.size() - 1; i < poly.size(); j = i++) {
        if (((poly[i].y > y) != (poly[j].y > y)) &&
            (x < (poly[j].x - poly[i].x) * (y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)) in = !in;
    }
    return in;
}

bool over(const Box& a, const Box& b) {
    double eps = 0.5; // Margen de seguridad para evitar solapamientos visuales
    return !(a.x >= b.x + b.w - eps || b.x >= a.x + a.w - eps || 
             a.y >= b.y + b.d - eps || b.y >= a.y + a.d - eps);
}

bool boxIn(const Box& b, const vector<Point>& poly) {
    double eps = 0.5;
    return ptIn(b.x + eps, b.y + eps, poly) && 
           ptIn(b.x + b.w - eps, b.y + eps, poly) &&
           ptIn(b.x + b.w - eps, b.y + b.d - eps, poly) && 
           ptIn(b.x + eps, b.y + b.d - eps, poly) &&
           ptIn(b.x + b.w/2, b.y + b.d/2, poly);
}

pair<Box, Box> getB(double x, double y, double w, double d, double g, double r) {
    int rot = (int)r % 360;
    if (rot == 0)   return {{x, y, w, d}, {x, y + d, w, g}};
    if (rot == 90)  return {{x, y, d, w}, {x + d, y, g, w}};
    if (rot == 180) return {{x, y, w, d}, {x, y - g, w, g}};
    return {{x, y, d, w}, {x - g, y, g, w}}; // 270
}

bool valid(const PlacedBay& nb, const BayType& bt, const WarehouseProblem& prob, const vector<PlacedBay>& cur) {
    auto [ph, ga] = getB(nb.x, nb.y, bt.width, bt.depth, bt.gap, nb.rot);
    if (!boxIn(ph, prob.warehousePolygon) || !boxIn(ga, prob.warehousePolygon)) return false;

    for (const auto& o : prob.obstacles) {
        Box ob = {o.x, o.y, o.width, o.depth};
        if (over(ph, ob) || over(ga, ob)) return false;
    }

    for (const auto& eb : cur) {
        auto it = find_if(prob.bayTypes.begin(), prob.bayTypes.end(), [&](const BayType& t){return t.id == eb.id;});
        auto [eP, eG] = getB(eb.x, eb.y, it->width, it->depth, it->gap, eb.rot);
        // Los Gaps sí pueden solaparse entre ellos (comparten pasillo), pero no con el hierro
        if (over(ph, eP) || over(ph, eG) || over(ga, eP)) return false;
    }
    return true;
}

// --- SOLVER OPTIMIZADO ---
int main() {
    WarehouseProblem prob;
    prob.warehousePolygon = pWH("warehouse.csv");
    prob.obstacles = pObs("obstacles.csv");
    prob.bayTypes = pBT("types_of_bays.csv");

    if (prob.bayTypes.empty()) return 1;

    vector<BayType> types = prob.bayTypes;
    // Ordenamos por eficiencia para maximizar el retorno de inversión [cite: 138]
    sort(types.begin(), types.end(), [](const BayType& a, const BayType& b){ return a.getEff() > b.getEff(); });

    double mxX = 0, mxY = 0;
    for (auto& p : prob.warehousePolygon) { mxX = max(mxX, p.x); mxY = max(mxY, p.y); }

    vector<PlacedBay> bestBays;
    double step = 10.0; // PASO FINO para eliminar los "huecazos"

    for (double y = 0; y < mxY; y += step) {
        for (double x = 0; x < mxX; ) { // Controlamos el avance de x manualmente
            bool placed = false;
            for (auto& t : types) {
                // Probamos 0 y 180 primero para compactar filas
                for (double r : {0.0, 180.0, 90.0, 270.0}) {
                    PlacedBay b = {t.id, x, y, r};
                    if (valid(b, t, prob, bestBays)) {
                        bestBays.push_back(b);
                        double w = (int(r)%180==0) ? t.width : t.depth;
                        x += w; // Avanza justo lo que mide el bay
                        placed = true;
                        break;
                    }
                }
                if (placed) break;
            }
            if (!placed) x += step; // Si no cabe nada, avanza un poquito
        }
    }

    for (auto& b : bestBays) cout << b.id << ", " << b.x << ", " << b.y << ", " << b.rot << endl;
    return 0;
}