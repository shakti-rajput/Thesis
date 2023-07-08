// #include <iostream>
// #include <vector>

// using namespace std;

// // Data structure to represent a graph
// class Graph {
//     int V; // number of vertices
//     vector<int> *adj; // adjacency list
// public:
//     Graph(int V);
//     void addEdge(int u, int v);
//     void printGraph();
//     vector<int> getMinimumVertexCover();
//     void getMinimumVertexCoverUtil(int v, vector<int>& dp, vector<int>& parent, vector<bool>& visited);
// };

// // Constructor
// Graph::Graph(int V) {
//     this->V = V;
//     adj = new vector<int>[V];
// }

// // Function to add an edge to the graph
// void Graph::addEdge(int u, int v) {
//     adj[u].push_back(v);
// }

// // Function to print the graph
// void Graph::printGraph() {
//     for (int i = 0; i < V; i++) {
//         cout << i << ": ";
//         for (auto v : adj[i]) {
//             cout << v << " ";
//         }
//         cout << endl;
//     }
// }

// // Function to calculate the minimum vertex cover of the graph
// vector<int> Graph::getMinimumVertexCover() {
//     vector<int> result;
//     vector<int> dp(V, -1); // dp[i] = minimum vertex cover of subtree rooted at i
//     vector<int> parent(V, -1); // parent[i] = parent of i in the tree
//     vector<bool> visited(V, false); // mark visited vertices
//     for (int i = 0; i < V; i++) {
//         if (!visited[i]) {
//             visited[i] = true;
//             getMinimumVertexCoverUtil(i, dp, parent, visited);
//         }
//     }
//     for (int i = 0; i < V; i++) {
//         if (dp[i] == 1) { // vertex i is in the minimum vertex cover
//             result.push_back(i);
//         }
//     }
//     return result;
// }

// // Recursive function to calculate the minimum vertex cover of the subtree rooted at v
// void Graph::getMinimumVertexCoverUtil(int v, vector<int>& dp, vector<int>& parent, vector<bool>& visited) {
//     visited[v] = true;
//     dp[v] = 0; // vertex v is excluded from the minimum vertex cover
//     for (auto u : adj[v]) {
//         if (!visited[u]) {
//             visited[u] = true;
//             parent[u] = v;
//             getMinimumVertexCoverUtil(u, dp, parent, visited);
//             dp[v] += min(dp[u], 1); // choose u or its children in the minimum vertex cover
//         }
//     }
// }

// // Example usage
// int main() {
//     // Create a sample directed acyclic graph
//     Graph g(7);
//     g.addEdge(0, 1);
//     g.addEdge(0, 2);
//     g.addEdge(1, 3);
//     g.addEdge(1, 4);
//     g.addEdge(2, 5);
//     g.addEdge(2, 6);

//     // Print the original graph
//     cout << "Original graph:" << endl;
//     g.printGraph();

//     // Find the minimum vertex cover of the graph
//     vector<int> minimumVertexCover = g.getMinimumVertexCover();

//     // Print the minimum vertex cover
//     cout << "Minimum vertex cover:";
//     for (auto v : minimumVertexCover) {
//         cout << " " << v;
//     }
//     cout << endl;

//     return 0;
// }