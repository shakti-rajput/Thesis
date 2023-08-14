#include <iostream>
#include <sstream>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_set>
#include "graph.h"
#include "helper.h"

using namespace std;

void dfs(long long int node, unordered_set<long long int> &vis, stack<long long int> &st,
         unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj)
{
  vis.insert(node);
  if (adj.find(node) != adj.end())
    for (auto table : adj[node])
    {
      for (auto child : table.second)
      {
        long long int V = child.first;
        if (vis.find(V) == vis.end())
          dfs(V, vis, st, adj);
      }
    }
  st.push(node);
}

vector<long long int> topoSort(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj)
{
  unordered_set<long long int> visited;
  vector<long long int> result;
  stack<long long int> topologicalStack;

  for (auto node : adj)
  {
    if (visited.find(node.first) == visited.end())
    {
      dfs(node.first, visited, topologicalStack, adj);
    }
  }

  result.clear();
  result.reserve(topologicalStack.size());
  while (!topologicalStack.empty())
  {
    result.push_back(topologicalStack.top());
    topologicalStack.pop();
  }
  return result;
}

Graph::Graph() {}
Graph::Graph(long long int V, long long int totalTables)
{
  this->V = V;
  this->totalTables = totalTables;
}

void Graph::viewadj() { cout << "adj.size(): " << adj.size() << endl; }

Graph buildGraph(
    long long int vertices,
    unordered_map<long long int, list<pair<long long int, long long int>>> tables)
{
  Graph g(vertices, tables.size());
  int count = 0;

  for (const auto &table : tables)
  {
    for (const auto &entry : table.second)
    {
      g.addEdge(entry.first, entry.second, table.first);
      count++;
    }
  }

  cout << "Total Edges: " << count << endl;
  return g;
}

// Function to add an edge to the graph
void Graph::addEdge(long long int u, long long int v, long long int tableindex)
{
  adj[u][tableindex][v].setData(u, v);
}

void detectandConvertLoopEdges(long long int node,
                               unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj,
                               unordered_set<long long int> &vis,
                               unordered_set<long long int> &pathVis,
                               unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, long long int>>> &loopEdges,
                               long long int &newItem, long long int &count, string &path)
{
  vis.insert(node);
  pathVis.insert(node);
  if (adj.find(node) != adj.end())
  {
    count++;

    for (auto tables: adj[node])
    {
      long long int table = tables.first;
      for (auto it = adj[node][table].begin(); it != adj[node][table].end();)
      {
        bool nextPointer = true;

        if (vis.find((*it).first) == vis.end())
        {
          path = path + to_string(node) + " --> ";
          detectandConvertLoopEdges((*it).first, adj, vis, pathVis, loopEdges, newItem, count, path);
        }
        else if (pathVis.find((*it).first) != pathVis.end())
        {
          auto next = std::next(it);
          nextPointer = false;
          loopEdges[node][table][newItem] = (*it).first;
          if (adj[node][table].count((*it).first) > 0)
            adj[node][table].erase((*it).first);
          adj[node][table][newItem].setData(node, newItem);
          newItem++;
          it = next;
        }

        if (nextPointer)
          it++;
      }
    }
  }
  pathVis.erase(node);
}

string printNodesOrder(const vector<long long int> &nodesTopoOrder)
{
  stringstream ss;
  ss << "Nodes Topo Order: ";
  for (const auto &node : nodesTopoOrder)
  {
    ss << node << " ";
  }
  return ss.str();
}

void Graph::transformToAcyclic(long long int newItemCounter)
{
  unordered_set<long long int> vis;
  nodesTopoOrder = topoSort(adj);

  createAndWriteToFile("NodesOrder.txt", printNodesOrder(nodesTopoOrder));

  unordered_set<long long int> pathVis;
  long long int earlierItems = newItemCounter;
  long long int count = 0;
  string path;
  cout << "Total Vertices : " << adj.size() << endl;
  for (auto vertex : nodesTopoOrder)
  {
    if (vis.find(vertex) == vis.end())
    {
      detectandConvertLoopEdges(vertex, adj, vis, pathVis, loopEdges, newItemCounter, count, path);
    }
  }

  writePathToFile("graphPath.txt", path);
  cout << "New Item : " << newItemCounter << endl;
  cout << "adj.size : " << adj.size() << endl;
  cout << "Loop Edges : " << newItemCounter - earlierItems << endl;
  cout << "Function called : " << count << endl;
  cout << "Total Vertices : " << adj.size() << endl
       << endl;
}

// Function to print the graph
string Graph::printGraph()
{
  string result;
  for (auto node : adj)
  {
    long long int u = node.first;
    result = result + to_string(u) + "  : [\n";
    for (auto table : adj[u])
    {
      result = result + "       [";
      for (auto v : table.second)
      {
        result = result + to_string(v.first) + " ";
      }
      result = result + "]\n";
    }
    result = result + "    ]\n";
  }
  return result;
}

void dfs_minVertexCover(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj,
                        unordered_map<long long int, pair<vector<long long int>, vector<long long int>>> &dp,
                        unordered_set<long long int> &visited,
                        int node)
{
  visited.insert(node);
  if (adj.find(node) != adj.end())
  {
    for (auto &table : adj[node])
    {
      for (auto &neighbor : table.second)
      {
        long long int child = neighbor.first;
        if (visited.find(child) == visited.end())
        {
          dfs_minVertexCover(adj, dp, visited, child);
        }
      }
    }

    for (auto &table : adj[node])
    {
      for (auto &neighbor : table.second)
      {
        long long int child = neighbor.first;

        // not including source in the vertex cover
        dp[node].first.insert(dp[node].first.end(), dp[child].second.begin(), dp[child].second.end());

        if (dp[child].second.size() <= dp[child].first.size())
          dp[node].second.insert(dp[node].second.end(), dp[child].second.begin(), dp[child].second.end());
        else
          dp[node].second.insert(dp[node].second.end(), dp[child].first.begin(), dp[child].first.end());
      }
    }
  }
}

void Graph::get_minVertex_cover()
{
  unordered_map<long long int, pair<vector<long long int>, vector<long long int>>> dp;
  for (auto node : nodesTopoOrder)
  {
    // 0 denotes not included in vertex cover
    dp[node].first = {};
    // 1 denotes included in vertex cover
    dp[node].second.push_back(node);
  }

  unordered_set<long long int> visited;
  for (auto node : nodesTopoOrder)
  {
    if (visited.find(node) == visited.end())
    {

      dfs_minVertexCover(adj, dp, visited, node);
    }
  };
  if (dp[nodesTopoOrder[0]].second.size() <= dp[nodesTopoOrder[0]].first.size())
    minVertexCover.insert(minVertexCover.begin(), dp[nodesTopoOrder[0]].second.begin(), dp[nodesTopoOrder[0]].second.end());
  else
    minVertexCover.insert(minVertexCover.begin(), dp[nodesTopoOrder[0]].first.begin(), dp[nodesTopoOrder[0]].first.end());

  cout << "Vertex cover ----> ";
  string vertexCover;
  for (auto node : minVertexCover)
  {
    vertexCover = vertexCover + to_string(node) + " ";
  }
  cout << vertexCover << endl;
  createAndWriteToFile("vertexCover.txt", vertexCover);
}

void printlookUp(unordered_map<long long int, pair<long long int, unordered_map<long long int, unordered_map<long long int, bool>>>> lookUp)
{
  cout << endl
       << "LookUp: " << endl;

  string lookUpStr = "";
  for (auto &vertex : lookUp)
  {
    lookUpStr = lookUpStr + "NODE: " + to_string(vertex.first) + "\n" + "Entries: [";
    auto tables = vertex.second.second;
    for (auto table : tables)
    {
      for (auto entry : table.second)
      {
        lookUpStr = lookUpStr + "(" + to_string(entry.first) + " " + to_string(entry.second) + ")";
      }
    }
    lookUpStr = lookUpStr + "]" + "\n";
  }
  cout << lookUpStr;
  createAndWriteToFile("lookUp.txt", lookUpStr);
}

void createMinVertexAdj(const unordered_map<long long int, bool> &minVertexCoverCopy,
                        const vector<long long int> &nodesTopoOrder,
                        unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj,
                        unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &minVertexAdj)
{
  for (auto &node : nodesTopoOrder)
  {
    if (adj.find(node) != adj.end())
    {
      for (auto &table : adj.at(node))
      {
        for (auto &vertex : table.second)
        {
          if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end())
          {
            minVertexAdj[vertex.first][table.first][node] = false;
          }

          if (minVertexCoverCopy.find(node) != minVertexCoverCopy.end())
          {
            minVertexAdj[node][table.first][vertex.first] = true;
          }
        }
      }
    }
  }
}

void printminVertexAdj(const unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &minVertexAdj)
{
  cout << endl
       << "minVertexAdj: " << endl;
  string minVertexAdjStr = "";
  for (auto node : minVertexAdj)
  {
    minVertexAdjStr = minVertexAdjStr + "Node:   " + to_string(node.first) + "\n" + "Entries: [";
    auto tables = node.second;
    for (auto table : tables)
    {
      minVertexAdjStr = minVertexAdjStr + "[";
      for (auto entry : table.second)
      {
        minVertexAdjStr = minVertexAdjStr + "(" + to_string(entry.first) + " " + to_string(entry.second) + ")";
      }
      minVertexAdjStr = minVertexAdjStr + "]" + "\n";
    }
    minVertexAdjStr = minVertexAdjStr + "]" + "\n";
  }
  cout << minVertexAdjStr;
  createAndWriteToFile("minVertexAdjStr.txt", minVertexAdjStr);
}

void printVCTree(const unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, pair<long long int, long long int>>>> &VCTree)
{
  cout << endl
       << "VCTree: " << endl;
  string VCTreestr = "";
  for (auto node : VCTree)
  {
    VCTreestr = VCTreestr + "Node: " + to_string(node.first) + "\n";
    VCTreestr = VCTreestr + "Entries: [";
    auto tables = node.second;
    for (auto table : tables)
    {
      VCTreestr = VCTreestr + "[";
      for (auto entry : table.second)
      {
        VCTreestr = VCTreestr + "(" + to_string(entry.first) + " " + to_string(entry.second.first) + ")";
      }
      VCTreestr = VCTreestr + "]" + "\n";
    }
    VCTreestr = VCTreestr + "]" + "\n";
  }
  cout << VCTreestr;
  createAndWriteToFile("VCTreestr.txt", VCTreestr);
}

void generateLookUp(
    unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj,
    unordered_map<long long int, pair<long long int, unordered_map<long long int, unordered_map<long long int, bool>>>> &lookUp,
    unordered_map<long long int, bool> &minVertexCover)
{
  for (auto node : adj)
  {
    for (auto &table : node.second)
    {
      for (auto v : table.second)
      {
        if (minVertexCover.find(node.first) != minVertexCover.end() && minVertexCover.find(v.first) != minVertexCover.end())
        {
          lookUp[node.first].first++;
          lookUp[node.first].second[table.first][v.first] = true;
        }
        else if (minVertexCover.find(v.first) != minVertexCover.end())
        {
          lookUp[node.first].first++;
          lookUp[node.first].second[table.first][v.first] = true;
        }
        else if (minVertexCover.find(node.first) != minVertexCover.end())
        {
          lookUp[v.first].first++;
          lookUp[v.first].second[table.first][node.first] = false;
        }
      }
    }
  }
}

void Graph::generatingVCTree()
{
  unordered_map<long long int, pair<long long int, unordered_map<long long int, unordered_map<long long int, bool>>>> lookUp;
  unordered_map<long long int, bool> minVertexCoverCopy;

  for (auto &vertex : minVertexCover)
  {
    minVertexCoverCopy[vertex] = true;
  }

  createMinVertexAdj(minVertexCoverCopy, nodesTopoOrder, adj, minVertexAdj);
  generateLookUp(adj, lookUp, minVertexCoverCopy);

  unordered_map<long long int, bool> vis;
  queue<long long int> q;
  q.push(minVertexCover[0]);
  vis[minVertexCover[0]] = true;

  while (!q.empty())
  {
    long long int VNP = q.front();
    q.pop();
    minVertexCoverCopy.erase(VNP);
    for (auto &table : minVertexAdj[VNP])
    {
      for (auto &vertex : table.second)
      {
        if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end()) // Overlapping Connection
        {
          VCTree[VNP][table.first][vertex.first] = make_pair(vertex.first, table.first); // Doubt
                                                                                         // cout << VNP << endl;
                                                                                         // if (vis.find(vertex.first) == vis.end())
          q.push(vertex.first);
        }
        else if (lookUp[vertex.first].first > 1) // Adam Connection
        {
          for (auto lookUptable : lookUp[vertex.first].second)
          {
            for (auto v : lookUptable.second)
            {
              if (v.first != VNP)
              {
                VCTree[VNP][table.first][vertex.first] = make_pair(v.first, lookUptable.first);
                if (minVertexCoverCopy.find(v.first) != minVertexCoverCopy.end()) // Overlapping Connection
                  q.push(v.first);
              }
            }
          }
        }
      }
    }
  }

  printlookUp(lookUp);
  printminVertexAdj(minVertexAdj);
  printVCTree(VCTree);
}
