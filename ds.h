// ds.h - Basic data structures built from scratch (Unit 2 + helpers for Unit 3)
#pragma once
#include <bits/stdc++.h>
using namespace std;

inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ---------------------------------------------------------------------------
// 1. Singly Linked List  - keeps the "recently searched routes" history
//    (new item inserted at the FRONT, oldest item removed from the TAIL)
// ---------------------------------------------------------------------------
class HistoryList {
    struct Node {
        string data;
        Node *next;
        Node(const string &d) : data(d), next(nullptr) {}
    };
    Node *head = nullptr;
    int size = 0, capacity;

public:
    explicit HistoryList(int cap = 5) : capacity(cap) {}
    ~HistoryList() {
        while (head) {
            Node *t = head;
            head = head->next;
            delete t;
        }
    }
    void add(const string &s) {
        Node *n = new Node(s);
        n->next = head;
        head = n;
        size++;
        if (size > capacity) {            // drop the oldest entry (tail)
            Node *cur = head;
            while (cur->next->next) cur = cur->next;
            delete cur->next;
            cur->next = nullptr;
            size--;
        }
    }
    void print() const {
        if (!head) { cout << "  (no routes searched yet)\n"; return; }
        int i = 1;
        for (Node *c = head; c; c = c->next) cout << "  " << i++ << ". " << c->data << "\n";
    }
};

// ---------------------------------------------------------------------------
// 2. Binary Search Tree - keeps all location names in alphabetical order
// ---------------------------------------------------------------------------
class BST {
    struct Node {
        string key;
        Node *left = nullptr, *right = nullptr;
        Node(const string &k) : key(k) {}
    };
    Node *root = nullptr;

    Node *insert(Node *n, const string &k) {
        if (!n) return new Node(k);
        if (toLower(k) < toLower(n->key)) n->left = insert(n->left, k);
        else if (toLower(k) > toLower(n->key)) n->right = insert(n->right, k);
        return n;                          // duplicates are ignored
    }
    void inorder(Node *n, vector<string> &out) const {
        if (!n) return;
        inorder(n->left, out);
        out.push_back(n->key);
        inorder(n->right, out);
    }
    int height(Node *n) const { return n ? 1 + max(height(n->left), height(n->right)) : 0; }
    void destroy(Node *n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    ~BST() { destroy(root); }
    void insert(const string &k) { root = insert(root, k); }
    vector<string> sorted() const {
        vector<string> out;
        inorder(root, out);
        return out;
    }
    int height() const { return height(root); }
};

// ---------------------------------------------------------------------------
// 3. Hash Table with CHAINING (collision avoidance) - name -> location index
//    Rehashes automatically when the load factor goes above 0.75
// ---------------------------------------------------------------------------
class HashTable {
    vector<list<pair<string, int>>> table;
    int count = 0;

    size_t hashOf(const string &key) const {
        size_t h = 0;
        for (char c : key) h = (h * 31 + (unsigned char)c) % table.size();   // polynomial rolling hash
        return h;
    }
    void rehash() {
        auto old = table;
        table.assign(old.size() * 2, {});
        count = 0;
        for (auto &chain : old)
            for (auto &kv : chain) put(kv.first, kv.second);
    }

public:
    HashTable(int buckets = 8) : table(buckets) {}
    void put(const string &rawKey, int value) {
        string key = toLower(rawKey);
        auto &chain = table[hashOf(key)];
        for (auto &kv : chain)
            if (kv.first == key) { kv.second = value; return; }
        chain.push_back({key, value});
        count++;
        if ((double)count / table.size() > 0.75) rehash();
    }
    int get(const string &rawKey) const {          // returns -1 when not found
        string key = toLower(rawKey);
        for (auto &kv : table[hashOf(key)])
            if (kv.first == key) return kv.second;
        return -1;
    }
    void stats() const {
        size_t longest = 0;
        for (auto &c : table) longest = max(longest, c.size());
        cout << "  Buckets: " << table.size() << " | Items: " << count
             << " | Load factor: " << fixed << setprecision(2) << (double)count / table.size()
             << " | Longest chain: " << longest << "\n";
    }
};

// ---------------------------------------------------------------------------
// 4. Disjoint Set Union (Union-Find) - used by Kruskal's algorithm
// ---------------------------------------------------------------------------
class DSU {
    vector<int> parent, rnk;

public:
    explicit DSU(int n) : parent(n), rnk(n, 0) { iota(parent.begin(), parent.end(), 0); }
    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }   // path compression
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rnk[a] < rnk[b]) swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};
