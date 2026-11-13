// huffman.h - Huffman coding to compress campus announcements (Exp 3.2)
#pragma once
#include "ds.h"

struct HNode {
    char ch;
    int freq;
    HNode *left, *right;
    HNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};
struct HCompare {
    bool operator()(HNode *a, HNode *b) const { return a->freq > b->freq; }
};

class Huffman {
    HNode *root = nullptr;
    map<char, string> codes;

    void makeCodes(HNode *n, const string &path) {
        if (!n) return;
        if (!n->left && !n->right) {
            codes[n->ch] = path.empty() ? "0" : path;     // single-character text
            return;
        }
        makeCodes(n->left, path + "0");
        makeCodes(n->right, path + "1");
    }
    void destroy(HNode *n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    ~Huffman() { destroy(root); }

    void build(const string &text) {
        destroy(root);
        root = nullptr;
        codes.clear();
        map<char, int> freq;
        for (char c : text) freq[c]++;
        priority_queue<HNode *, vector<HNode *>, HCompare> pq;
        for (auto &kv : freq) pq.push(new HNode(kv.first, kv.second));
        while (pq.size() > 1) {
            HNode *a = pq.top(); pq.pop();
            HNode *b = pq.top(); pq.pop();
            HNode *parent = new HNode('\0', a->freq + b->freq);
            parent->left = a;
            parent->right = b;
            pq.push(parent);
        }
        root = pq.empty() ? nullptr : pq.top();
        makeCodes(root, "");
    }
    const map<char, string> &getCodes() const { return codes; }

    string encode(const string &text) const {
        string bits;
        for (char c : text) bits += codes.at(c);
        return bits;
    }
    string decode(const string &bits) const {
        string out;
        if (!root) return out;
        if (!root->left && !root->right) return string(bits.size(), root->ch);
        HNode *cur = root;
        for (char b : bits) {
            cur = (b == '0') ? cur->left : cur->right;
            if (!cur->left && !cur->right) { out += cur->ch; cur = root; }
        }
        return out;
    }
};
