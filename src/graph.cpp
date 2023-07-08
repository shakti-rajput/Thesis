#include "graph.h"
#include "helper.h"
#include "timer.h"
#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_set>

using namespace std;

void dfsLineGraph(const pair<long long int, long long int> &parentNode,
                  const pair<long long int, long long int> &currentNode,
                  unordered_map<pair<long long int, long long int>, bool, hash_pair> &visited,
                  stack<pair<pair<long long int, long long int>, pair<long long int, long long int>>> &st,
                  const unordered_map<pair<long long int, long long int>, vector<unordered_map<long long int, Sobit>>, hash_pair> &lineGraphAdj)
{
  visited[currentNode] = true;
  auto it = lineGraphAdj.find(currentNode);
  if (it != lineGraphAdj.end())
  {
    const vector<unordered_map<long long int, Sobit>> &neighbors = it->second;
    for (const auto &neighbor : neighbors)
    {
      for (const auto &entry : neighbor)
      {
        const pair<long long int, long long int> &childNode = make_pair(currentNode.second, entry.first);
        if (visited.find(childNode) == visited.end())
        {
          dfsLineGraph(currentNode, childNode, visited, st, lineGraphAdj);
        }
      }
    }
  }
  st.push(pair<pair<long long int, long long int>, pair<long long int, long long int>>(parentNode, currentNode));
}

void dfs(long long int node, unordered_set<long long int> &vis, stack<long long int> &st,
         unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj)
{
  vis.insert(node);
  if (adj.find(node) != adj.end())
    for (auto table : adj[node])
    {
      for (auto child : table)
      {
        long long int V = child.first;
        if (vis.find(V) == vis.end())
          dfs(V, vis, st, adj);
      }
    }
  st.push(node);
}

vector<long long int> topoSort(unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj)
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

void topoSortLineGraph(unordered_map<pair<long long int, long long int>, vector<unordered_map<long long int, Sobit>>, hash_pair> &lineGaphAdj,
                       vector<pair<pair<long long int, long long int>, pair<long long int, long long int>>> &result, bool addParent)
{
  unordered_map<pair<long long int, long long int>, bool, hash_pair> visited;
  stack<pair<pair<long long int, long long int>, pair<long long int, long long int>>> topologicalStack;

  if (addParent)
  {
    for (auto it = result.begin(); it != result.end(); it++)
    {
      if (visited.find(it->second) == visited.end())
      {
        dfsLineGraph(make_pair(-1, -1), it->second, visited, topologicalStack, lineGaphAdj);
      }
    }
  }
  else
  {
    for (auto it = lineGaphAdj.begin(); it != lineGaphAdj.end(); it++)
    {
      if (visited.find(it->first) == visited.end())
      {
        dfsLineGraph(make_pair(-1, -1), it->first, visited, topologicalStack, lineGaphAdj);
      }
    }
  }
  result.clear();
  result.reserve(topologicalStack.size());

  while (!topologicalStack.empty())
  {
    result.push_back(topologicalStack.top());
    topologicalStack.pop();
  }
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
    vector<list<pair<long long int, long long int>>> tables,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit,
    vector<list<Sobit>> &sobitTables)
{
  Graph g(vertices, tables.size());
  int count = 0;

  for (int tableIndex = 0; tableIndex < tables.size(); tableIndex++)
  {
    for (auto entries : tables[tableIndex])
    {
      cout << "Subject: " << entries.first << " Object: " << entries.second << " tableIndex " << tableIndex << endl;
      g.addEdge(entries.first, entries.second, tableIndex);
      count++;
    }
  }
  cout << "Total Edges : " << count << endl;
  return g;
}

// Function to add an edge to the graph
void Graph::addEdge(long long int u, long long int v, long long int tableindex)
{
  if (adj.find(u) == adj.end())
  {
    adj[u].resize(totalTables);
  }
  adj[u][tableindex][v].setData(u, v);
}

void detectandConvertLoopEdges(long long int node,
                               unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj,
                               unordered_set<long long int> &vis,
                               unordered_set<long long int> &pathVis,
                               unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, long long int>>> &loopEdges,
                               long long int &newItem, long long int &count, string &path)
{
  vis.insert(node);
  pathVis.insert(node);
  if (adj.find(node) != adj.end())
  {
    // cout << "Yha: " << node << endl;
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
          // cout << "Yha aye kya??" << endl;
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

void Graph::transformToAcyclic(long long int newItemCounter)
{
  unordered_set<long long int> vis;
  nodesTopoOrder = topoSort(adj);
  unordered_set<long long int> pathVis;
  long long int earlierItems = newItemCounter;
  // cout << "New Item : " << newItemCounter << endl;
  long long int count = 0;
  string path;
  cout << "Total Vertices : " << adj.size() << endl;
  for (auto vertex : nodesTopoOrder)
  {
    // cout << "Node : " << vertex << endl;
    if (vis.find(vertex) == vis.end())
    {
      // cout << "Vertex : " << vertex << endl;
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
      for (auto v : table)
      {
        result = result + to_string(v.first) + " ";
      }
      result = result + "]\n";
    }
    result = result + "    ]\n";
  }
  return result;
}

void Graph::convertToLineGraph()
{
  unordered_set<long long int> uniqueVertices;
  for (long long int u = 0; u < adj.size(); u++)
  {
    for (long long int table = 0; table < adj[u].size(); table++)
    {
      for (const auto &entry : adj[u][table])
      {
        long long int v = entry.first;

        if (uniqueVertices.count(v) == 0)
        {
          // vector<unordered_map<long long int, Sobit>> sample(adj[u].size());
          // lineGraphAdj[make_pair(u, v)] = adj[v].size() == 0 ? sample : adj[v];
          lineGraphAdj[make_pair(u, v)] = adj[v];
          uniqueVertices.insert(v);
        }
      }
    }
  }
}

// string Graph::printlineGraphAdj()
// {
//   string result;

//   for (const auto &entry : lineGraphAdj)
//   {
//     const auto &vertices = entry.first;
//     const auto &adjacencyList = entry.second;

//     // Convert vertices to string representation
//     string vertexString = "(" + to_string(vertices.first) + ", " + to_string(vertices.second) + ")";

//     // Convert adjacency list to string representation
//     string adjacencyString;
//     for (const auto &adj : adjacencyList)
//     {
//       string adjString = "[";
//       for (const auto &pair : adj)
//       {
//         adjString += to_string(pair.first) + ",";
//       }
//       // Remove the trailing comma if present
//       if (!adj.empty())
//         adjString.pop_back();
//       adjString += "]";

//       adjacencyString += adjString + ", ";
//     }
//     // Remove the trailing comma and space if present
//     if (!adjacencyString.empty())
//       adjacencyString = adjacencyString.substr(0, adjacencyString.length() - 2);

//     // Append the formatted entry to the result string
//     result += vertexString + " --> " + adjacencyString + "\n";
//   }

//   return result;
// }

string printnodesOrder(vector<pair<pair<long long int, long long int>, pair<long long int, long long int>>> nodesOrder)
{
  string str;
  for (auto nodes = nodesOrder.begin(); nodes != nodesOrder.end(); nodes++)
  {
    pair<long long int, long long int> parentNode = (*nodes).first;
    pair<long long int, long long int> currentNode = (*nodes).second;
    long long int u = currentNode.first;
    long long int v = currentNode.second;
    long long int parentU = parentNode.first;
    long long int parentV = parentNode.second;
    str = str + "(" + to_string(parentU) + " " + to_string(parentV) + ")-->(" + to_string(u) + " " + to_string(v) + ") ";
  }
  return str;
}

string getGroupInfoString(const unordered_map<long long int, vector<long long int>> &groupInfo)
{
  string result;
  for (const auto &entry : groupInfo)
  {
    result += "Key: " + to_string(entry.first) + ", Values: ";
    for (const auto &value : entry.second)
    {
      result += to_string(value) + " ";
    }
    result += "\n";
  }
  return result;
}

void makeNewNodeEntry(unordered_map<long long int, vector<long long int>> &vertexGrpList,
                      unordered_map<long long int, vector<long long int>> &groupInfo,
                      const long long int node, long long int &newGroup,
                      const long long int grpOfParent = -1, bool flag = false)
{
  if (flag)
  {
    if (groupInfo.find(grpOfParent) != groupInfo.end())
    {
      vertexGrpList[node].push_back(newGroup);

      groupInfo[newGroup] = groupInfo[grpOfParent];
      groupInfo[newGroup].push_back(node);

      vertexGrpList[grpOfParent].push_back(newGroup);
      newGroup++;
    }
  }
  else
  {
    vertexGrpList[node].push_back(newGroup);
    groupInfo[newGroup].push_back(node);
    newGroup++;
  }
}

// vector<long long int> minVertexCoverUtil(long long int totalVertices,
//                                          vector<pair<pair<long long int, long long int>, pair<long long int, long long int>>> &nodesOrder)
// {

//   Timer timer;
//   timer.start();
//   unordered_map<pair<long long int, long long int>, bool, hash_pair> vis[totalVertices];

//   /*
//   pair<u,v> -> [(v,z),(v,o)] --> lineGaphAdj
//   pair<u,v> -> [g1,g2] --> vertexGrpList
//   g1 -> (u,v,z)
//   g2 -> (u,v,o)
//   */

//   unordered_map<long long int, vector<long long int>> vertexGrpList;
//   unordered_map<long long int, vector<long long int>> groupInfo;
//   long long int groupcounter = 0;
//   bool flag = false;
//   string str;

//   for (auto nodes = nodesOrder.begin(); nodes != nodesOrder.end(); nodes++)
//   {

//     pair<long long int, long long int> parentNode = (*nodes).first;
//     pair<long long int, long long int> currentNode = (*nodes).second;
//     long long int u = currentNode.first;
//     long long int v = currentNode.second;
//     long long int parentU = parentNode.first;
//     long long int parentV = parentNode.second;

//     // cout << parentNode.first << " " << parentNode.second << " " << u << " " << v << endl;

//     if (parentV == u)
//     {
//       if (vertexGrpList.find(v) == vertexGrpList.end()) // not present
//       {
//         vector<long long int> orignalParentVertexGrpList = vertexGrpList[parentV];
//         long long int parentUSize = vertexGrpList[parentU].size();
//         long long int parentVSize = vertexGrpList[parentV].size();
//         for (long long int grp = 0; grp < parentUSize; grp++)
//         {
//           long long int grpOfParent = vertexGrpList[parentU][grp]; // vertexGrpList[parentU][grp] --> gives grp number of node/vertex

//           if (groupInfo.find(grpOfParent) != groupInfo.end())
//           {
//             // cout << v << groupcounter << grpOfParent << endl;
//             makeNewNodeEntry(vertexGrpList, groupInfo, v, groupcounter, grpOfParent, true);
//             groupInfo.erase(grpOfParent);
//           }
//           // cout << getGroupInfoString(groupInfo) << endl;
//           // cout << getGroupInfoString(vertexGrpList) << endl;
//         }
//         // cout << "AAp yha h --> " << endl;

//         // cout << getGroupInfoString(groupInfo) << endl;
//         // cout << getGroupInfoString(vertexGrpList) << endl;

//         // for (long long int grp = 0; grp < parentVSize; grp++)
//         // {
//         //   makeNewNodeEntry(vertexGrpList, groupInfo, v, groupcounter, vertexGrpList[parentV][grp], true);
//         //   cout << getGroupInfoString(groupInfo) << endl;
//         //   cout << getGroupInfoString(vertexGrpList) << endl;
//         // }
//         // vertexGrpList[parentV] = orignalParentVertexGrpList;
//       }
//     }
//     else
//     {
//       if (vertexGrpList.find(u) == vertexGrpList.end()) // not present
//       {
//         makeNewNodeEntry(vertexGrpList, groupInfo, u, groupcounter);
//       }
//       cout << groupcounter << endl;
//       if (vertexGrpList.find(v) == vertexGrpList.end()) // not present
//       {
//         makeNewNodeEntry(vertexGrpList, groupInfo, v, groupcounter);
//       }
//       // cout << getGroupInfoString(groupInfo) << endl;
//       // cout << getGroupInfoString(vertexGrpList) << endl;
//     }
//   }
//   cout << "Time processing Minimum Vertex Cover Except NodesOrder --> " << timer.elapsed() << endl
//        << endl;

//   cout << "______Writing Minimum Vertex Cover Details to file_______" << endl;
//   timer.start();
//   createAndWriteToFile("FinalGrpInfo.txt", getGroupInfoString(groupInfo));
//   long long int max = -1;
//   long long int key = -1;
//   for (auto g : groupInfo)
//   {
//     if (max < g.second.size())
//     {
//       max = g.second.size();
//       key = g.first;
//     }
//   }
//   createAndWriteToFile("FinalVertexGrpList.txt", getGroupInfoString(vertexGrpList));
//   cout << "Minimum Vertex Cover Details written in File completed in --> " << timer.elapsed() << endl;
//   if (key != -1)
//     return groupInfo[key];
//   else
//     return {};
// }

// void search(long long int node,
//             vector<long long int> &vertexCover,
//             unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj,
//             unordered_map<long long int, MonitoredStatus> &visited)
// {
//   for (auto &table : adj[node])
//   {
//     for (auto &neighbor : table)
//     {
//       if (visited.find(node) == visited.end())
//         search(neighbor.first, vertexCover, adj, visited);
//     }
//   }

//   bool isLeaf = true;
//   int countNotMONITORED = 0, countMONITORED = 0, totalNeighbours = 0;

//   for (auto &table : adj[node])
//   {
//     for (auto &neighbor : table)
//     {
//       isLeaf = false;
//       totalNeighbours++;
//       if (visited[neighbor.first] == NOTMONITORED)
//       {
//         countNotMONITORED++;
//       }
//       if (visited[neighbor.first] == MONITORED)
//       {
//         countMONITORED++;
//       }
//     }
//   }
//   // cout << "Node: " << node << " totalNeighbours: " << totalNeighbours << " countNotMONITORED: " << countNotMONITORED << " countMONITORED: " << countMONITORED << endl;

//   // for (auto node : visited)
//   // {
//   //   cout << "node: " << node.first << "Status: " << node.second << endl;
//   // }
//   if (countNotMONITORED > 0)
//   {
//     visited[node] = CAMERA;
//     vertexCover.push_back(node);
//     return;
//   }

//   if (countMONITORED == totalNeighbours && countMONITORED != 0)
//   {
//     visited[node] = NOTMONITORED;
//     return;
//   }

//   if (isLeaf)
//     visited[node] = NOTMONITORED;
//   else
//     visited[node] = MONITORED;
// }

// void Graph::getMinVertexCover()
// {
//   int count = 0;
//   unordered_map<long long int, MonitoredStatus> visited;
//   for (auto node : nodesTopoOrder)
//   {
//     if (visited.find(node) == visited.end())
//     {
//       cout << "First Node: " << node << endl;
//       search(node, minVertexCover, adj, visited);
//       if (visited[node] == NOTMONITORED)
//         minVertexCover.push_back(node);
//     }
//   }
//   cout << "minVertexCover Node: ";
//   for (auto node : minVertexCover)
//   {
//     cout << node << " ";
//   }
//   cout << endl;
// }
// string printVector(vector<long long int> vector)
// {
//   string ans = "";
//   for (auto node : vector)
//   {
//     ans = ans + to_string(node) + " ";
//   }
//   return ans;
// }
void dfs_minVertexCover(unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj,
                        unordered_map<long long int, pair<vector<long long int>, vector<long long int>>> &dp,
                        unordered_set<long long int> &visited,
                        int node)
{
  visited.insert(node);
  if (adj.find(node) != adj.end())
  {
    for (auto &table : adj[node])
    {
      for (auto &neighbor : table)
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
      for (auto &neighbor : table)
      {
        long long int child = neighbor.first;

        // cout << "Node: " << node << " Child: " << child << endl;
        // not including source in the vertex cover
        dp[node].first.insert(dp[node].first.end(), dp[child].second.begin(), dp[child].second.end());

        // cout << "dp[child].first: " << printVector(dp[child].first) << endl;   // first for not including source in the vertex cover
        // cout << "dp[child].second: " << printVector(dp[child].second) << endl; // second for not including source in the vertex cover
        // cout << "dp[node].first: " << printVector(dp[node].first) << endl;
        if (dp[child].second.size() <= dp[child].first.size())
          dp[node].second.insert(dp[node].second.end(), dp[child].second.begin(), dp[child].second.end());
        else
          dp[node].second.insert(dp[node].second.end(), dp[child].first.begin(), dp[child].first.end());
        // cout << "dp[node].second: " << printVector(dp[node].second) << endl
        //      << endl
        //      << endl;
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

// void Graph::getMinimumVertexCover()
// {
//   Timer timer;
//   timer.start();
//   vector<pair<pair<long long int, long long int>, pair<long long int, long long int>>> nodesOrder;

//   // nodesOrder.push_back(make_pair(make_pair(-1, -1), make_pair(0, 1)));
//   // nodesOrder.push_back(make_pair(make_pair(0, 1), make_pair(1, 2)));

//   // nodesOrder.push_back(make_pair(make_pair(1, 2), make_pair(2, 3)));

//   // nodesOrder.push_back(make_pair(make_pair(2, 3), make_pair(3, 4)));
//   // nodesOrder.push_back(make_pair(make_pair(3, 4), make_pair(4, 5)));

//   topoSortLineGraph(lineGraphAdj, nodesOrder, false);
//   topoSortLineGraph(lineGraphAdj, nodesOrder, true);
//   cout << "Preprocessing Time for  nodesOrder --> " << timer.elapsed() << endl;

//   cout << "______Writing Nodes Order to file_______" << endl;
//   timer.start();
//   writeNodesOrder("NodesOrder.txt", printnodesOrder(nodesOrder));
//   cout << "Writting Nodes Order to file Completed in --> " << timer.elapsed() << endl
//        << endl;

//   timer.start();
//   minVertexCover = minVertexCoverUtil(V, nodesOrder);
//   cout << "Processing Time for  whole program except Nodes Order--> " << timer.elapsed() << endl;
// }

// void lookUpMinVertex(long long int vertex,
//                      unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj,
//                      unordered_map<long long int, unordered_map<long long int, bool>> &lookUp,
//                      unordered_map<long long int, bool> minVertexCover)
// {
//   for (auto table : adj[vertex])
//   {
//     for (auto v : table)
//     {
//       if (minVertexCover.find(v.first) != minVertexCover.end())
//         lookUp[vertex][v.first] = false;
//     }
//   }
// }

// lookUpMinVertex(adj, lookup, minVertexCover);
void generateLookUp(unordered_map<long long int, vector<unordered_map<long long int, Sobit>>> &adj,
                    unordered_map<long long int, unordered_map<long long int, bool>> &lookUp,
                    unordered_map<long long int, bool> minVertexCover)
{
  for (auto node : adj)
  {
    int tableIndex = 0;
    for (auto &table : node.second)
    {
      for (auto v : table)
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

bool adamConnection(unordered_map<long long int, unordered_map<long long int, long long int>> &lookUp,
                    long long int vertex,
                    unordered_map<long long int, bool> &minVertexCoverCopy)
{
  int count = 0;
  if (lookUp[vertex].size() > 1)
  {
    for (auto &node : lookUp[vertex])
    {
      if (minVertexCoverCopy.find(node.first) != minVertexCoverCopy.end())
      {
        count++;
      }
    }
  }
  return count > 1 ? true : false;
}

// void createVcJoinTree()
// {
//   for (auto &node : nodesTopoOrder)
//   {
//     int tableIndex = 0;
//     for (auto &table : adj[node])
//     {
//       for (auto &vertex : table)
//       {
//         if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end())
//         {
//           // cout << "YHaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1" << endl;
//           VCJoinTreeAdj[vertex.first].resize(adj[node].size());
//           VCJoinTreeAdj[vertex.first][tableIndex][node] = false;
//           // cout << "yoo1" << endl;
//         }

//         if (minVertexCoverCopy.find(node) != minVertexCoverCopy.end())
//         {
//           // cout << "YHaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa2" << endl;
//           VCJoinTreeAdj[node].resize(adj[node].size());
//           VCJoinTreeAdj[node][tableIndex][vertex.first] = true;
//           // cout << "yoo2" << endl;
//         }
//       }
//       tableIndex++;
//     }
//   }
// }

void Graph::generatingVCTree()
{
  unordered_map<long long int, unordered_map<long long int, bool>> lookUp;
  unordered_map<long long int, bool> minVertexCoverCopy;

  for (auto &vertex : minVertexCover)
  {
    minVertexCoverCopy[vertex] = true;
  }

  for (auto &node : nodesTopoOrder)
  {
    int tableIndex = 0;
    for (auto &table : adj[node])
    {
      for (auto &vertex : table)
      {
        if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end())
        {
          VCJoinTreeAdj[vertex.first].resize(adj[node].size());
          VCJoinTreeAdj[vertex.first][tableIndex][node] = false;
        }

        if (minVertexCoverCopy.find(node) != minVertexCoverCopy.end())
        {
          VCJoinTreeAdj[node].resize(adj[node].size());
          VCJoinTreeAdj[node][tableIndex][vertex.first] = true;
        }
      }
      tableIndex++;
    }
  }

  generateLookUp(adj, lookUp, minVertexCoverCopy);
  cout << endl
       << endl
       << "LookUp: " << endl;
  for (auto &vertex : lookUp)
  {
    cout << "NODE::: " << vertex.first << endl;
    cout << "Entries::: [";
    for (auto entry : vertex.second)
    {
      cout << "(" << entry.first << " " << entry.second << ")";
    }
    cout << "]" << endl;
  }

  // cout << "YHaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl;

  // cout << "YHaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl;

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
    cout << "NODE " << VNP << endl;
    for (auto &table : VCJoinTreeAdj[VNP])
    {
      for (auto &vertex : table)
      {
        // if (vis.find(VNP) == vis.end())
        // {
        cout << "Vertex: " << vertex.first << endl;
        if (minVertexCoverCopy.find(vertex.first) != minVertexCoverCopy.end()) // Overlapping Connection
        {
          cout << "YHA1: " << endl;
          VCTree[VNP].resize(VCJoinTreeAdj[VNP].size());
          VCTree[VNP][tableIndex].push_back(make_pair(vertex.first, vertex.first)); // Doubt
          if (vis.find(vertex.first) == vis.end())
            q.push(vertex.first);
          cout << "YHA12: " << endl;
        }
        else if (lookUp[vertex.first].size() > 1) // Adam Connection
        {
          cout << "YHA2: " << endl;
          VCTree[VNP].resize(VCJoinTreeAdj[VNP].size());
          for (auto &v : lookUp[vertex.first])
          {
            if (v.first != VNP)
            {
              VCTree[VNP][tableIndex].push_back(make_pair(vertex.first, v.first));
              if (vis.find(v.first) == vis.end())
                q.push(v.first);
            }
          }
          cout << "YHA21: " << endl;
        }
        // vis[vertex.first] = true;
      }
      tableIndex++;
      // }
    }
  }
  cout << endl
       << endl
       << "VCJoinTreeAdj: " << endl;
  for (auto node : VCJoinTreeAdj)
  {
    cout << "Node111111111111:   " << node.first << endl;
    cout << "Entries: [";
    auto tables = node.second;
    for (auto table : tables)
    {
      cout << "[";
      for (auto entry : table)
      {
        cout << "(" << entry.first << " " << entry.second << ")";
      }
      cout << "]" << endl;
    }
    cout << "]" << endl;
  }

  cout << endl
       << endl
       << "VCTree: " << endl;
  for (auto node : VCTree)
  {
    cout << "Node: " << node.first << endl;
    cout << "Entries: [";
    auto tables = node.second;
    for (auto table : tables)
    {
      cout << "[";
      for (auto entry : table)
      {
        cout << "(" << entry.first << " " << entry.second << ")";
      }
      cout << "]" << endl;
    }
    cout << "]" << endl;
  }
}

void Graph::generatingVCJoinTree()
{
}