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

void makeHash(list<Sobit> &tables,
              bool index,
              unordered_map<long long int, list<Sobit>> &hashTable);

void reduceBckrwd(list<Sobit> &currTable,
                  bool currTableCommonVertex,
                  unordered_map<long long int, list<Sobit>> &finalRightTableHash);

void reduceFrwd(list<Sobit> &currTable,
                bool currTableCommonVertex,
                list<Sobit> &rightTable,
                bool rightTableCommonVertex,
                unordered_map<long long int, list<Sobit>> &finalNextTableHash);
void reduceFrwdHash(unordered_map<long long int, list<Sobit>> &currTableHash,
                    bool currTableCommonVertex,
                    list<Sobit> &nextTable,
                    bool nextTableCommonVertex,
                    unordered_map<long long int, list<Sobit>> &finalNextTableHash);

void reduceCurrentTable(list<Sobit> &nextTable,
                        bool nextTableCommonVertex,
                        unordered_map<long long int, list<Sobit>> &finalRightTableHash);

void buildSobitTableFromHashMap(list<Sobit> &nextTable,
                                bool nextTableCommonVertex,
                                unordered_map<long long int, list<Sobit>> &finalRightTableHash);
void reduceLoopTables(Graph &g,
                      unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                      unordered_map<long long int, string> &decodeQueryTables,
                      unordered_map<string, long long int> &dataTables,
                      unordered_map<long long int, string> &decodeStringToData,
                      unordered_map<string, long long int> &storeStringtoData);