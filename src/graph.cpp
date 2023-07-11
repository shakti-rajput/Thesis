#include "graph.h"
#include "helper.h"
#include "timer.h"
#include <iostream>
#include <sstream>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_set>

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
    unordered_map<long long int, list<pair<long long int, long long int>>> tables,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit,
    vector<list<Sobit>> &sobitTables)
{
  Graph g(vertices, tables.size());
  int count = 0;

  for (auto table : tables)
  {
    for (auto entries : table.second)
    {
      cout << "Subject: " << entries.first << " Object: " << entries.second << " tableIndex " << table.first << endl;
      g.addEdge(entries.first, entries.second, table.first);
      count++;
    }
  }
  cout << "Total Edges : " << count << endl;
  return g;
}

// Function to add an edge to the graph
void Graph::addEdge(long long int u, long long int v, long long int tableindex)
{
  // if (adj.find(u) == adj.end())
  // {
  //   adj[u].resize(totalTables);
  // }
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

    for (long long int table = 0; table < adj[node].size(); table++)
    {
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
  // cout << "New Item : " << newItemCounter << endl;
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
  // cout << "NodesOrder: " << endl;
  for (auto node : nodesTopoOrder)
  {
    // 0 denotes not included in vertex cover
    dp[node].first = {};
    // 1 denotes included in vertex cover
    dp[node].second.push_back(node);
    // cout << "Node: " << node << endl;
  }

  unordered_set<long long int> visited;
  for (auto node : nodesTopoOrder)
  {
    // cout << node << " " << endl;
    if (visited.find(node) == visited.end())
    {

      dfs_minVertexCover(adj, dp, visited, node);
    }
  };
  // printing minimum size vertex cover
  if (dp[nodesTopoOrder[0]].second.size() <= dp[nodesTopoOrder[0]].first.size())
    minVertexCover.insert(minVertexCover.begin(), dp[nodesTopoOrder[0]].second.begin(), dp[nodesTopoOrder[0]].second.end());
  else
    minVertexCover.insert(minVertexCover.begin(), dp[nodesTopoOrder[0]].first.begin(), dp[nodesTopoOrder[0]].first.end());

  cout << "Vertex cover ----> " << endl;
  string vertexCover;
  for (auto node : minVertexCover)
  {
    vertexCover = vertexCover + to_string(node) + " ";
  }
  createAndWriteToFile("vertexCover.txt", vertexCover);
  cout << vertexCover << endl;
}

void printlookUp(unordered_map<long long int, unordered_map<long long int, bool>> lookUp)
{
  cout << endl
       << "LookUp: " << endl;

  string lookUpStr = "";
  for (auto &vertex : lookUp)
  {
    lookUpStr = lookUpStr + "NODE: " + to_string(vertex.first) + "\n" + "Entries: [";
    for (auto entry : vertex.second)
    {
      lookUpStr = lookUpStr + "(" + to_string(entry.first) + " " + to_string(entry.second) + ")";
    }
    lookUpStr = lookUpStr + "]" + "\n";
  }
  cout << lookUpStr;
  createAndWriteToFile("lookUp.txt", lookUpStr);
}

void createMinVertexAdj(unordered_map<long long int, bool> minVertexCoverCopy,
                        vector<long long int> nodesTopoOrder,
                        unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> adj,
                        unordered_map<long long int, vector<unordered_map<long long int, bool>>> &minVertexAdj)
{
  for (auto &node : nodesTopoOrder)
  {
    for (auto &table : adj[node])
    {
      for (auto &vertex : table.second)
      {
        if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end())
        {
          minVertexAdj[vertex.first].resize(adj[node].size());
          minVertexAdj[vertex.first][table.first][node] = false;
        }

        if (minVertexCoverCopy.find(node) != minVertexCoverCopy.end())
        {
          minVertexAdj[node].resize(adj[node].size());
          minVertexAdj[node][table.first][vertex.first] = true;
        }
      }
    }
  }
}

void printminVertexAdj(const unordered_map<long long int, vector<unordered_map<long long int, bool>>> &minVertexAdj)
{
  cout << endl
       << "VCJoinTreeAdj: " << endl;
  string minVertexAdjStr = "";
  for (auto node : minVertexAdj)
  {
    minVertexAdjStr = minVertexAdjStr + "Node:   " + to_string(node.first) + "\n" + "Entries: [";
    auto tables = node.second;
    for (auto table : tables)
    {
      minVertexAdjStr = minVertexAdjStr + "[";
      for (auto entry : table)
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

void printVCTree(const unordered_map<long long int, vector<vector<pair<long long int, long long int>>>> &VCTree)
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
      for (auto entry : table)
      {
        VCTreestr = VCTreestr + "(" + to_string(entry.first) + " " + to_string(entry.second) + ")";
      }
      VCTreestr = VCTreestr + "]" + "\n";
    }
    VCTreestr = VCTreestr + "]" + "\n";
  }
  cout << VCTreestr;
  createAndWriteToFile("VCTreestr.txt", VCTreestr);
}

void generateLookUp(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, Sobit>>> &adj,
                    unordered_map<long long int, unordered_map<long long int, bool>> &lookUp,
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
          lookUp[node.first][v.first] = true;
        }
        else if (minVertexCover.find(v.first) != minVertexCover.end())
        {
          lookUp[node.first][v.first] = true;
        }
        else if (minVertexCover.find(node.first) != minVertexCover.end())
        {
          lookUp[v.first][node.first] = false;
        }
        // edge is coming to v.first from MinVertexCover
        // lookUpMinVertex(v.first, adj, lookUp, minVertexCover); // edge going to MinVertexCover from v.first
      }
    }
  }
}

void Graph::generatingVCTree()
{
  unordered_map<long long int, unordered_map<long long int, bool>> lookUp;
  unordered_map<long long int, bool> minVertexCoverCopy;

  for (auto &vertex : minVertexCover)
  {
    minVertexCoverCopy[vertex] = true;
  }

  createMinVertexAdj(minVertexCoverCopy, nodesTopoOrder, adj, minVertexAdj);
  generateLookUp(adj, lookUp, minVertexCoverCopy);

  // Generate VCTree
  unordered_map<long long int, bool> vis;
  queue<long long int> q;
  q.push(minVertexCover[0]);
  vis[minVertexCover[0]] = true;
  while (!q.empty())
  {
    long long int VNP = q.front();
    q.pop();
    minVertexCoverCopy.erase(VNP);
    int tableIndex = 0;
    // cout << "NODE " << VNP << endl;
    for (auto &table : minVertexAdj[VNP])
    {
      for (auto &vertex : table)
      {
        // if (vis.find(VNP) == vis.end())
        // {
        // cout << "Vertex: " << vertex.first << endl;
        if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end()) // Overlapping Connection
        {
          cout << "Yha1" << endl;
          VCTree[VNP].resize(minVertexAdj[VNP].size());
          VCTree[VNP][tableIndex].push_back(make_pair(vertex.first, vertex.first)); // Doubt
          cout << VNP << endl;
          if (vis.find(vertex.first) == vis.end())
            q.push(vertex.first);
        }
        else if (lookUp[vertex.first].size() > 1) // Adam Connection
        {
          cout << "Yha2" << endl;
          cout << VNP << endl;
          VCTree[VNP].resize(minVertexAdj[VNP].size());
          for (auto &v : lookUp[vertex.first])
          {
            if (v.first != VNP)
            {
              VCTree[VNP][tableIndex].push_back(make_pair(vertex.first, v.first));
              if (vis.find(v.first) == vis.end())
                q.push(v.first);
            }
          }
        }
      }
      tableIndex++;
      // }
    }
  }

  printlookUp(lookUp);
  printminVertexAdj(minVertexAdj);
  printVCTree(VCTree);
}
