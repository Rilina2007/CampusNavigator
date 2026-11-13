// graph.h - Weighted undirected graph + the graph algorithms of Unit 3
#pragma once
#include "ds.h"

struct Edge {
    int to, w;
};
struct RoadEdge {
    int u, v, w;
};

class Graph {
public:
    vector<string> names;
    vector<vector<Edge>> adj;
    vector<RoadEdge> roads;
    HashTable index;                      // name -> id
    static constexpr int INF = 1000000000;

    int addLocation(const string &name) {
        if (index.get(name) != -1) return -1;           // already exists
        names.push_back(name);
        adj.push_back({});
        index.put(name, (int)names.size() - 1);
        return (int)names.size() - 1;
    }
    bool addRoad(int u, int v, int w) {
        if (u == v || w <= 0) return false;
        for (auto &e : adj[u]) if (e.to == v) return false;   // road already exists
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
        roads.push_back({u, v, w});
        return true;
    }
    int size() const { return (int)names.size(); }

    // ---- Dijkstra's algorithm (shortest path, Exp 3.1) ----------------------------
    vector<int> dijkstra(int src, vector<int> &parent) const {
        vector<int> dist(size(), INF);
        parent.assign(size(), -1);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        dist[src] = 0;
        pq.push({0, src});
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;                     // stale entry
            for (auto &e : adj[u]) {
                if (d + e.w < dist[e.to]) {                // relaxation
                    dist[e.to] = d + e.w;
                    parent[e.to] = u;
                    pq.push({dist[e.to], e.to});
                }
            }
        }
        return dist;
    }
    vector<int> buildPath(int dst, const vector<int> &parent) const {
        vector<int> path;
        for (int v = dst; v != -1; v = parent[v]) path.push_back(v);
        reverse(path.begin(), path.end());
        return path;
    }

    // ---- BFS (fewest stops) and DFS (Chapter 3.1) ---------------------------------
    vector<int> bfs(int src) const {
        vector<int> order;
        vector<bool> seen(size(), false);
        queue<int> q;
        q.push(src);
        seen[src] = true;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);
            for (auto &e : adj[u])
                if (!seen[e.to]) { seen[e.to] = true; q.push(e.to); }
        }
        return order;
    }
    void dfsHelper(int u, vector<bool> &seen, vector<int> &order) const {
        seen[u] = true;
        order.push_back(u);
        for (auto &e : adj[u])
            if (!seen[e.to]) dfsHelper(e.to, seen, order);
    }
    vector<int> dfs(int src) const {
        vector<int> order;
        vector<bool> seen(size(), false);
        dfsHelper(src, seen, order);
        return order;
    }

    // ---- Kruskal's MST (Chapter 3.2 / Exp 3.3) -------------------------------------
    // returns the chosen roads; 'total' receives their combined length
    vector<RoadEdge> kruskal(long long &total) const {
        vector<RoadEdge> sorted = roads;
        sort(sorted.begin(), sorted.end(), [](const RoadEdge &a, const RoadEdge &b) { return a.w < b.w; });
        DSU dsu(size());
        vector<RoadEdge> mst;
        total = 0;
        for (auto &r : sorted) {
            if (dsu.unite(r.u, r.v)) {
                mst.push_back(r);
                total += r.w;
            }
        }
        return mst;
    }

    // ---- Bridges: roads whose closure disconnects the campus (Exp 3.4) -------------
    void bridgeDfs(int u, int parent, int &timer, vector<int> &tin, vector<int> &low,
                   vector<RoadEdge> &out) const {
        tin[u] = low[u] = ++timer;
        for (auto &e : adj[u]) {
            if (e.to == parent) continue;
            if (tin[e.to]) {
                low[u] = min(low[u], tin[e.to]);           // back-edge
            } else {
                bridgeDfs(e.to, u, timer, tin, low, out);
                low[u] = min(low[u], low[e.to]);
                if (low[e.to] > tin[u]) out.push_back({u, e.to, e.w});
            }
        }
    }
    vector<RoadEdge> bridges() const {
        vector<int> tin(size(), 0), low(size(), 0);
        vector<RoadEdge> out;
        int timer = 0;
        for (int i = 0; i < size(); i++)
            if (!tin[i]) bridgeDfs(i, -1, timer, tin, low, out);
        return out;
    }
};
