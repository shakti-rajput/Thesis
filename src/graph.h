#pragma once
#include <iostream>
#include <set>
#include <vector>
#include "bitset.h"
#include "enum.h"
#include "timer.h"

using namespace std;

struct hash_pair
{
  template <class T1, class T2>
  size_t operator()(const pair<T1, T2> &p) const
  {
    auto hash1 = hash<T1>{}(p.first);
    auto hash2 = hash<T2>{}(p.second);

    if (hash1 != hash2)
    {
      return hash1 ^ hash2;
    }

    // If hash1 == hash2, their XOR is zero.
    return hash1;
  }
};

class Graph
{

public:
  long long int V;
  long long int totalTables;                                                                           // number of vertices
  unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> adj; // adjacency list

  vector<long long int> nodesTopoOrder;

  unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, long long int>>> loopEdges;

  unordered_map<pair<long long int, long long int>, vector<unordered_map<long long int, Sobit>>, hash_pair> lineGraphAdj;

  vector<long long int> minVertexCover;

  unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> minVertexAdj;

  unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, pair<long long int, long long int>>>> VCTree;

public:
  Graph();
  Graph(long long int V, long long int totalTables);
  void addEdge(long long int u, long long int v, long long int tableindex);
  void viewadj();
  void transformToAcyclic(long long int newItemCounter);
  string printGraph();
  void get_minVertex_cover();
  void generatingVCTree();
  bool checkDirection(long long int u, long long int table, long long int v);
};

Graph buildGraph(
    long long int vertices,
    unordered_map<long long int, list<pair<long long int, long long int>>> tables);

void printminVertexAdj(const unordered_map<long long int,
                                           unordered_map<long long int,
                                                         unordered_map<long long int, bool>>> &minVertexAdj);
void printVCTree(const unordered_map<long long int,
                                     unordered_map<long long int,
                                                   unordered_map<long long int,
                                                                 pair<long long int, long long int>>>> &VCTree);