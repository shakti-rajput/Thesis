#include <unordered_map>
#include "bitset.h"
#include "graph.h"

using namespace std;

void semiJoinOpConVertices(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                           const Graph &g,
                           unordered_map<long long int, string> &decodeStringToData,
                           unordered_map<long long int,
                                         unordered_map<long long int,
                                                       unordered_map<long long int,
                                                                     unordered_map<long long int, list<Sobit>>>>> &hashStore);

void semiJoinOpNonConVertices(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                              const Graph &g, unordered_map<long long int, string> decodeStringToData);