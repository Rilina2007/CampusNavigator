// ===========================================================================
//  CAMPUS NAVIGATOR  -  ADSA Mini Project (C++)
//  Uses: Linked List, BST, Hash Table, Graph (BFS/DFS), Dijkstra, Kruskal (DSU),
//        Bridges (DFS low-link), Huffman coding and 0/1 Knapsack (DP)
// ===========================================================================
#include "graph.h"
#include "huffman.h"

Graph campus;
BST directory;
HistoryList history(5);

struct Attraction {
    string name;
    int minutes;      // time needed to visit
    int rating;       // how useful/fun it is (1-10)
};
vector<Attraction> attractions = {
    {"Library", 30, 8}, {"Canteen", 45, 9}, {"Sports Complex", 60, 7},
    {"Auditorium", 40, 6}, {"Administrative Block", 20, 3}, {"Medical Centre", 25, 4},
    {"Academic Block A", 35, 7}};

// ----------------------------- input helpers -------------------------------
string readLine(const string &prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return a == string::npos ? "" : s.substr(a, b - a + 1);
}
int readInt(const string &prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "  Please enter a valid number.\n";
        if (cin.eof()) return 0;
    }
}
// accepts either the location number or its name (name lookup uses the hash table)
int askLocation(const string &prompt) {
    while (true) {
        string s = readLine(prompt);
        if (cin.eof()) return -1;
        bool digits = !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
        int id = digits ? stoi(s) - 1 : campus.index.get(s);
        if (id >= 0 && id < campus.size()) return id;
        cout << "  Location not found. Type its number or exact name.\n";
    }
}
void printPath(const vector<int> &path) {
    for (size_t i = 0; i < path.size(); i++)
        cout << campus.names[path[i]] << (i + 1 < path.size() ? " -> " : "\n");
}

// ----------------------------- sample data ---------------------------------
void loadCampus() {
    for (string n : {"Main Gate", "Administrative Block", "Library", "Academic Block A", "Academic Block B",
                     "Canteen", "Hostel", "Sports Complex", "Auditorium", "Medical Centre"}) {
        campus.addLocation(n);
        directory.insert(n);
    }
    auto L = [&](const string &n) { return campus.index.get(n); };
    campus.addRoad(L("Main Gate"), L("Administrative Block"), 150);
    campus.addRoad(L("Main Gate"), L("Library"), 300);
    campus.addRoad(L("Administrative Block"), L("Library"), 120);
    campus.addRoad(L("Administrative Block"), L("Academic Block A"), 200);
    campus.addRoad(L("Library"), L("Academic Block A"), 100);
    campus.addRoad(L("Library"), L("Canteen"), 250);
    campus.addRoad(L("Academic Block A"), L("Academic Block B"), 80);
    campus.addRoad(L("Academic Block B"), L("Canteen"), 140);
    campus.addRoad(L("Academic Block B"), L("Auditorium"), 220);
    campus.addRoad(L("Canteen"), L("Hostel"), 180);
    campus.addRoad(L("Canteen"), L("Sports Complex"), 350);
    campus.addRoad(L("Hostel"), L("Sports Complex"), 200);
    campus.addRoad(L("Hostel"), L("Medical Centre"), 160);
    campus.addRoad(L("Auditorium"), L("Canteen"), 260);
}

// ----------------------------- menu actions --------------------------------
void showMap() {
    cout << "\nCAMPUS MAP (adjacency list, distances in metres)\n";
    for (int i = 0; i < campus.size(); i++) {
        cout << "  " << setw(2) << i + 1 << ". " << left << setw(22) << campus.names[i] << right << "-> ";
        for (size_t j = 0; j < campus.adj[i].size(); j++) {
            auto &e = campus.adj[i][j];
            cout << campus.names[e.to] << " (" << e.w << "m)" << (j + 1 < campus.adj[i].size() ? ", " : "");
        }
        cout << "\n";
    }
}

void shortestRoute() {
    int a = askLocation("From (number or name): ");
    int b = askLocation("To   (number or name): ");
    if (a < 0 || b < 0) return;
    vector<int> parent;
    vector<int> dist = campus.dijkstra(a, parent);
    if (dist[b] == Graph::INF) { cout << "No route exists between these places.\n"; return; }
    vector<int> path = campus.buildPath(b, parent);
    cout << "\nShortest route (" << dist[b] << " m, ~" << (dist[b] + 79) / 80 << " min walking):\n  ";
    printPath(path);
    history.add(campus.names[a] + " to " + campus.names[b] + " = " + to_string(dist[b]) + " m");
    // BFS gives the route with the FEWEST stops, for comparison
    cout << "(Dijkstra minimises distance; BFS would minimise the number of stops.)\n";
}

void distancesFrom() {
    int a = askLocation("Source location: ");
    if (a < 0) return;
    vector<int> parent;
    vector<int> dist = campus.dijkstra(a, parent);
    cout << "\nDistances from " << campus.names[a] << ":\n";
    for (int i = 0; i < campus.size(); i++) {
        cout << "  " << left << setw(22) << campus.names[i] << right;
        if (dist[i] == Graph::INF) cout << "unreachable (-1)\n";
        else cout << dist[i] << " m\n";
    }
}

void traversal() {
    int a = askLocation("Start location: ");
    if (a < 0) return;
    cout << "\nBFS (nearest places first):\n  ";
    vector<int> order = campus.bfs(a);
    printPath(order);
    cout << "DFS (go as deep as possible first):\n  ";
    order = campus.dfs(a);
    printPath(order);
}

void minimumNetwork() {
    long long total;
    vector<RoadEdge> mst = campus.kruskal(total);
    cout << "\nCheapest set of roads/cables that still connects every building (Kruskal + DSU):\n";
    for (auto &r : mst)
        cout << "  " << campus.names[r.u] << " <-> " << campus.names[r.v] << "  (" << r.w << " m)\n";
    long long all = 0;
    for (auto &r : campus.roads) all += r.w;
    cout << "Total length needed: " << total << " m (all roads together: " << all << " m, saving "
         << all - total << " m)\n";
    if ((int)mst.size() != campus.size() - 1) cout << "Warning: the campus is not fully connected!\n";
}

void criticalRoads() {
    vector<RoadEdge> br = campus.bridges();
    cout << "\nCritical roads (closing any of them cuts part of the campus off):\n";
    if (br.empty()) cout << "  None - every building has an alternate route.\n";
    for (auto &r : br)
        cout << "  " << campus.names[r.u] << " <-> " << campus.names[r.v] << " (" << r.w << " m)\n";
}

void listDirectory() {
    cout << "\nLocation directory (BST in-order = alphabetical, tree height "
         << directory.height() << "):\n";
    for (auto &s : directory.sorted()) cout << "  - " << s << "\n";
    cout << "Hash table used for name search:\n";
    campus.index.stats();
}

void addLocation() {
    string name = readLine("New location name: ");
    if (name.empty()) { cout << "Name cannot be empty.\n"; return; }
    int id = campus.addLocation(name);
    if (id < 0) { cout << "That location already exists.\n"; return; }
    directory.insert(name);
    cout << "Added as location #" << id + 1 << ". Connect it with a road (option 11) so it is reachable.\n";
}

void addRoad() {
    int a = askLocation("Road from: ");
    int b = askLocation("Road to  : ");
    if (a < 0 || b < 0) return;
    int w = readInt("Length in metres: ");
    cout << (campus.addRoad(a, b, w) ? "Road added.\n" : "Could not add road (same place, duplicate, or invalid length).\n");
}

void compressAnnouncement() {
    string text = readLine("Type an announcement to compress: ");
    if (text.empty()) { cout << "Nothing to compress.\n"; return; }
    Huffman h;
    h.build(text);
    cout << "\nHuffman codes:\n";
    for (auto &kv : h.getCodes())
        cout << "  '" << (kv.first == ' ' ? string("space") : string(1, kv.first)) << "' -> " << kv.second << "\n";
    string bits = h.encode(text);
    size_t original = text.size() * 8;
    cout << "Original size  : " << original << " bits\n";
    cout << "Compressed size: " << bits.size() << " bits\n";
    cout << "Saved          : " << fixed << setprecision(1) << 100.0 * (original - bits.size()) / original << "%\n";
    cout << "Decoded back   : \"" << h.decode(bits) << "\"" << (h.decode(bits) == text ? "  [verified]" : "  [MISMATCH]") << "\n";
}

void tourPlanner() {
    int budget = readInt("How many minutes do you have for the visit tour? ");
    if (budget <= 0) { cout << "Time must be positive.\n"; return; }
    int n = attractions.size();
    vector<vector<int>> dp(n + 1, vector<int>(budget + 1, 0));
    for (int i = 1; i <= n; i++)
        for (int t = 0; t <= budget; t++) {
            dp[i][t] = dp[i - 1][t];
            if (attractions[i - 1].minutes <= t)
                dp[i][t] = max(dp[i][t], dp[i - 1][t - attractions[i - 1].minutes] + attractions[i - 1].rating);
        }
    cout << "\nBest tour for " << budget << " minutes (0/1 Knapsack DP) - total rating " << dp[n][budget] << ":\n";
    int t = budget, used = 0;
    for (int i = n; i >= 1; i--)
        if (dp[i][t] != dp[i - 1][t]) {
            cout << "  - " << attractions[i - 1].name << " (" << attractions[i - 1].minutes << " min, rating "
                 << attractions[i - 1].rating << ")\n";
            t -= attractions[i - 1].minutes;
            used += attractions[i - 1].minutes;
        }
    if (used == 0) cout << "  (not enough time to visit anything)\n";
    else cout << "Time used: " << used << " min\n";
}

void menu() {
    cout << "\n=========== CAMPUS NAVIGATOR ===========\n"
         << " 1. Show campus map\n"
         << " 2. Shortest route between two places (Dijkstra)\n"
         << " 3. Distances from one place to all others\n"
         << " 4. Explore from a place (BFS & DFS)\n"
         << " 5. Cheapest road/cable network (Kruskal MST)\n"
         << " 6. Critical roads (bridges)\n"
         << " 7. Location directory (BST + Hash Table)\n"
         << " 8. Recent route searches (Linked List)\n"
         << " 9. Compress an announcement (Huffman)\n"
         << "10. Plan a visitor tour (Knapsack DP)\n"
         << "11. Add a road\n"
         << "12. Add a location\n"
         << " 0. Exit\n";
}

int main() {
    loadCampus();
    cout << "Welcome to the Campus Navigator!\n";
    while (true) {
        menu();
        int c = readInt("Choose: ");
        if (cin.eof()) break;
        switch (c) {
            case 1: showMap(); break;
            case 2: shortestRoute(); break;
            case 3: distancesFrom(); break;
            case 4: traversal(); break;
            case 5: minimumNetwork(); break;
            case 6: criticalRoads(); break;
            case 7: listDirectory(); break;
            case 8: cout << "\nLast routes searched:\n"; history.print(); break;
            case 9: compressAnnouncement(); break;
            case 10: tourPlanner(); break;
            case 11: addRoad(); break;
            case 12: addLocation(); break;
            case 0: cout << "Goodbye!\n"; return 0;
            default: cout << "Invalid option.\n";
        }
    }
    return 0;
}
