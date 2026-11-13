# Campus Navigator (C++)
ADSA mini project - a console app that models a university campus as a weighted graph and uses
the data structures and algorithms from the syllabus to solve real campus problems.

## How to run
Needs a C++17 compiler (g++ / MinGW / Code::Blocks).
```
g++ -std=c++17 -o campus main.cpp
./campus          (Windows: campus.exe)
```

## Menu features and the syllabus topic behind each
| Menu option | What it does | Syllabus topic |
|---|---|---|
| 1 | Shows the campus map as an adjacency list | Graph representation (Unit 3, Ch 3.1) |
| 2, 3 | Shortest walking route / distances to all places; unreachable = -1 | Dijkstra's algorithm, priority queue (Ch 3.2, **Exp 3.1**) |
| 4 | Explore from a place in BFS and DFS order | Graph traversal (Ch 3.1) |
| 5 | Cheapest set of roads/cables connecting all buildings | Kruskal's MST + Union-Find (Ch 3.2, **Exp 3.3**) |
| 6 | Finds roads whose closure cuts off part of the campus | Bridges with DFS low-link (**Exp 3.4**) |
| 7 | Alphabetical location directory; shows hash-table statistics | Binary Search Tree (Ch 2.2), Hashing with chaining (Ch 2.3) |
| 8 | Last 5 route searches | Singly linked list (Ch 2.1) |
| 9 | Compresses an announcement and shows the saving | Huffman coding (**Exp 3.2**) |
| 10 | Picks the best visitor tour within a time limit | 0/1 Knapsack, Dynamic Programming (Ch 3.3) |
| 11, 12 | Add a road / a new location while the program runs | Dynamic graph updates, hash table rehashing |

## Files
```
ds.h        Linked list, BST, hash table (chaining + rehash), DSU  - all written from scratch
graph.h     Graph class: Dijkstra, BFS, DFS, Kruskal, bridges
huffman.h   Huffman tree, encode, decode
main.cpp    Campus data, menu and the knapsack tour planner
```

## Complexity summary
| Operation | Time |
|---|---|
| Dijkstra (binary heap) | O((V + E) log V) |
| BFS / DFS / Bridges | O(V + E) |
| Kruskal | O(E log E) |
| Huffman build | O(n log n) |
| Knapsack tour | O(items x minutes) |
| BST insert (average) | O(log n) |
| Hash table lookup (average) | O(1) |
| Linked-list history insert | O(1) at front |

## Sample run
```
Choose: 2
From (number or name): Main Gate
To   (number or name): Hostel
Shortest route (700 m, ~9 min walking):
  Main Gate -> Administrative Block -> Library -> Canteen -> Hostel

Choose: 6
Critical roads:
  Hostel <-> Medical Centre (160 m)
```
Places can be typed by number (1-10) or by name (not case sensitive).

## Ideas to extend
Read the map from a file, add a visual map with SFML/Python, use A* search, add a Prim's algorithm comparison.
