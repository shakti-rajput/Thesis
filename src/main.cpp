// ConsoleApplication1.cpp : This file contains the 'main' function. Program
// execution begins and ends there.
//

#include "bitset.h"
#include "performHistogramJoin.h"
#include "config.h"
#include "helper.h"
#include "timer.h"
#include "read.h"
#include <bitset>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

void preProcess(const unordered_map<string, list<pair<string, string>>> &data,
                unordered_map<string, long long int> &storeStringtoInt,
                unordered_map<long long int, string> &decodeStringToData,
                const unordered_map<string, long long int> &tableNames,
                unordered_map<long long int, list<pair<long long int, long long int>>> &tablesAfterPreprocessing)
{
     unordered_map<long long int, list<pair<string, string>>> tablesBeforePreprocessing;

     for (auto &tableName : tableNames) // Taking tables that we have to deal with
     {
          tablesBeforePreprocessing[tableName.second] = data.at(tableName.first);
     }
     Timer timer;
     timer.start();
     preprocessingStringtoInteger(tablesBeforePreprocessing, storeStringtoInt, decodeStringToData, tablesAfterPreprocessing);
     cout << "Preprocessing Time --> " << timer.elapsed() << endl;
}

Graph preProcessQuery(const unordered_map<long long int, string> &decodeQueryTables,
                      const unordered_map<long long int, list<pair<long long int, long long int>>> &queryAfterPreprocessing,
                      long long int totalVertices,
                      long long int newItemCounter)
{

     Timer timer;

     Graph g;

     /*Build the Graph and write to File*/
     cout << "******* Start building the Graph ********" << endl;

     timer.start();
     g = buildGraph(totalVertices, queryAfterPreprocessing);
     cout << g.printGraph() << endl;
     cout << "--------- Graph Built in ---------> " << timer.elapsed() << endl
          << endl;

     cout << "________ Writing to file Before Transforming To Acyclic ________" << endl;
     timer.start();
     writeGraphtoFile("BeforeTransformingToAcyclic.txt", g);
     cout << "Writing to file Before Transforming To Acyclic Completed in --> " << timer.elapsed() << endl
          << endl
          << endl;

     /*Transform to Acyclic and write to File*/
     cout << "***** Transforming To Acyclic*****" << endl;
     timer.start();
     g.transformToAcyclic(newItemCounter);
     cout << "-------- Transforming to Acyclic Completed in -------> " << timer.elapsed() << endl;

     cout << "______Writing to file After Transforming To Acyclic_______" << endl;
     timer.start();
     writeGraphtoFile("AfterTransformingToAcyclic.txt", g);
     std::cout << "Writing to file After Transforming To Acyclic Completed in --> " << timer.elapsed() << endl
               << endl;

     /*Find Minimum Vertex Cover and write to File*/
     cout << "******* Finding Minimum Vertex Cover *****" << endl;
     timer.start();
     g.get_minVertex_cover();

     g.generatingVCTree();
     cout << "Finding Minimum Vertex Cover Completed in --> " << timer.elapsed() << endl
          << endl;

     cout << "Query Transformed Successfully" << endl;
     return g;
}

void findJoiningConditions(const Graph &g, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &conditions)
{
     for (const auto &node : g.minVertexAdj)
     {
          for (const auto &table : node.second)
          {
               for (const auto &entry : table.second)
               {
                    conditions[node.first][table.first].emplace(entry.first, !entry.second);
               }
          }
     }
}

inline long long int findQueryCorrespondingIndexToData(const unordered_map<string, long long int> &storeStringtoData, const unordered_map<long long int, string> &decodeStringToQuery, const long long int &index)
{
     return storeStringtoData.at(decodeStringToQuery.at(index));
}

inline bool checkCondition(const unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &conditions,
                           const long long int &u, const long long int &table, const long long int &v,
                           const pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> &sobitInfo,
                           const unordered_map<string, long long int> &storeStringtoData,
                           const unordered_map<long long int, string> &decodeStringToQuery)
{
     bool del = false;

     for (const auto &tableC : conditions.at(u))
     {
          const long long int tableCIndex = findQueryCorrespondingIndexToData(storeStringtoData, decodeStringToQuery, tableC.first);
          for (const auto &entryC : tableC.second)
          {
               if (entryC.first != v || tableC.first != table)
               {
                    const bool entryCondition = entryC.second;
                    const bool sobitCondition = entryCondition ? !sobitInfo.second.test(tableCIndex) : !sobitInfo.first.test(tableCIndex);

                    if (sobitCondition)
                    {
                         return true; // Early termination if condition is satisfied
                    }
               }
          }
     }
     return false;
}

void initializeUsingSobit(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                          const Graph &g,
                          const unordered_map<string, long long int> &storeStringtoData,
                          const unordered_map<long long int, string> &decodeStringToQuery)
{
     unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> conditions;
     findJoiningConditions(g, conditions);

     cout << "Conditions: " << endl;
     for (auto u : conditions)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << u.first << " " << table.first << " " << v.first << " " << v.second << endl;
               }
          }
     }
     cout << "*********" << endl;
     for (const auto &[nodeId, nodeTables] : g.minVertexAdj)
     {
          for (const auto &[tableId, tableEntries] : nodeTables)
          {
               for (const auto &[entry, isCommonSubject] : tableEntries)
               {
                    auto &sobitTable = sobitTables[nodeId][tableId][entry];
                    pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo;
                    cout << nodeId << " " << tableId << " " << entry << " " << isCommonSubject << endl;
                    if (isCommonSubject)
                    {
                         for (auto it = sobitTable.begin(); it != sobitTable.end();)
                         {
                              sobitInfo = (*it).getSubjectSobit();
                              bool del = checkCondition(conditions, nodeId, tableId, entry, sobitInfo, storeStringtoData, decodeStringToQuery);

                              if (del)
                              {
                                   it = sobitTable.erase(it);
                              }
                              else
                              {
                                   ++it;
                              }
                         }
                    }
                    else
                    {
                         for (auto it = sobitTable.begin(); it != sobitTable.end();)
                         {
                              sobitInfo = (*it).getObjectSobit();
                              bool del = checkCondition(conditions, nodeId, tableId, entry, sobitInfo, storeStringtoData, decodeStringToQuery);

                              if (del)
                              {
                                   it = sobitTable.erase(it);
                              }
                              else
                              {
                                   ++it;
                              }
                         }
                    }
               }
               for (auto u : g.minVertexAdj)
               {
                    for (auto table : u.second)
                    {
                         for (auto v : table.second)
                         {
                              cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
                         }
                    }
               }
          }
     }
}

void detectTablesofVertex(long long int u,
                          long long int table,
                          long long int v,
                          Graph g,
                          long long int oldV,
                          unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                          unordered_map<long long int, string> decodeStringToData)
{
     list<Sobit> tableStoreO, tableStoreN;
     unordered_map<long long int, list<Sobit>> hashTableN, hashTableO;
     bool indexN, indexO;
     bool flag = false;
     cout << u << " " << table << " " << v << endl;
     if (g.minVertexAdj.find(u) != g.minVertexAdj.end())
     {
          cout << "Here: ";
          indexN = g.minVertexAdj[u][table][v];
          flag = true;
     }
     else if (g.minVertexAdj.find(v) != g.minVertexAdj.end())
     {
          cout << "OR_HEREEE: ";
          indexN = !g.minVertexAdj[v][table][u];
     }
     if (g.minVertexAdj.find(oldV) != g.minVertexAdj.end())
     {
          cout << "OR_HEREEE::::::::::: " << endl;
          for (auto table1 : g.minVertexAdj[oldV])
          {
               for (auto v1 : table1.second)
               {

                    indexO = !g.minVertexAdj[oldV][table1.first][v1.first];
                    tableStoreO = sobitTables[oldV][table1.first][v1.first];
                    makeHash(tableStoreO, indexO, hashTableO);
                    if (flag)
                    {
                         reduceBckrwd(sobitTables[u][table][v], indexN, hashTableO);
                         makeHash(sobitTables[u][table][v], indexN, hashTableN);
                    }
                    else
                    {
                         reduceBckrwd(sobitTables[v][table][u], indexN, hashTableO);
                         makeHash(sobitTables[v][table][u], indexN, hashTableN);
                    }

                    reduceBckrwd(sobitTables[oldV][table1.first][v1.first], indexO, hashTableN);
               }
          }
     }
     else
     {
          cout << "HEREEEEEEE: " << endl;
          for (auto u1 : g.minVertexAdj)
          {
               for (auto table1 : u1.second)
               {
                    for (auto v1 : table1.second)
                    {
                         if (v1.first == oldV)
                         {
                              indexO = g.minVertexAdj[u1.first][table1.first][v1.first];
                              tableStoreO = sobitTables[oldV][table1.first][v1.first];
                              makeHash(tableStoreO, indexO, hashTableO);
                              if (flag)
                              {
                                   reduceBckrwd(sobitTables[u][table][v], indexN, hashTableO);
                                   makeHash(sobitTables[u][table][v], indexN, hashTableN);
                              }
                              else
                              {
                                   reduceBckrwd(sobitTables[v][table][u], indexN, hashTableO);
                                   makeHash(sobitTables[v][table][u], indexN, hashTableN);
                              }
                              reduceBckrwd(sobitTables[oldV][table1.first][v1.first], indexO, hashTableN);
                         }
                    }
               }
          }
     }
}

bool reduceLoopdfs(const Sobit &parentSobitEntry,
                   vector<long long int>::iterator iter,
                   const Graph &g,
                   Edge parentEdge,
                   const long long int &nodeO,
                   const long long int &nodeN,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                   const unordered_map<long long int, string> &decodeQueryTables,
                   const unordered_map<string, long long int> &dataTables,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>>> &hashStore,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>>> &updatedHashStore,
                   const Sobit &initialEntry,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>>>> &dp,
                   const unordered_map<long long int, string> &decodeStringToData,
                   const unordered_map<string, long long int> &storeStringtoData)
{
     long long int u = *iter;
     long long int prevObj = parentSobitEntry.getObject();
     if (u == nodeN)
     {
          return parentSobitEntry.getObject() == initialEntry.getSubject();
     }

     // if (dp.find(parentEdge.u) != dp.end() &&
     //     dp[parentEdge.u].find(parentEdge.table) != dp[parentEdge.u].end() &&
     //     dp[parentEdge.u][parentEdge.table].find(parentEdge.v) != dp[parentEdge.u][parentEdge.table].end() &&
     //     dp[parentEdge.u][parentEdge.table][parentEdge.v].find(prevObj) != dp[parentEdge.u][parentEdge.table][parentEdge.v].end() &&
     //     dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].find(initialEntry.getSubject()) != dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].end())
     // {
     //      return dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj][initialEntry.getSubject()];
     // }

     long long int nextU = *next(iter);

     for (const auto &table : g.adj.at(u))
     {
          for (const auto &v : table.second)
          {
               if (v.first == nextU)
               {

                    Edge currentEdge;

                    if (parentEdge.table != -1)
                    {
                         currentEdge.u = u;
                         currentEdge.table = table.first;
                         currentEdge.v = v.first;

                         if (g.minVertexAdj.find(u) == g.minVertexAdj.end())
                         {
                              swap(currentEdge.u, currentEdge.v);
                         }

                         if (hashStore[currentEdge.u][currentEdge.table].find(currentEdge.v) != hashStore[currentEdge.u][currentEdge.table].end()) // If the Hash was already calculated
                         {
                              if (hashStore[currentEdge.u][currentEdge.table][currentEdge.v].find(prevObj) != hashStore[currentEdge.u][currentEdge.table][currentEdge.v].end())
                              {
                                   bool check = false;
                                   for (const auto &entry : hashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj])
                                   {
                                        bool flag = reduceLoopdfs(entry, iter + 1, g, currentEdge, nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, initialEntry, dp, decodeStringToData, storeStringtoData);
                                        if (flag)
                                        {
                                             check = true;
                                             updatedHashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].push_back(entry);
                                             // dp[initialEntry.getSubject()][parentEdge.u][parentEdge.table][parentEdge.v][prevObj] = true;
                                        }
                                   }

                                   // dp[initialEntry.getSubject()][parentEdge.u][parentEdge.table][parentEdge.v][prevObj] = false;
                                   return check;
                              }
                              else
                              {
                                   // dp[initialEntry.getSubject()][parentEdge.u][parentEdge.table][parentEdge.v][prevObj] = false;
                                   return false;
                              }
                         }
                         else
                         {
                              makeHash(sobitTables[currentEdge.u][currentEdge.table][currentEdge.v], 0, hashStore[currentEdge.u][currentEdge.table][currentEdge.v]);

                              if (hashStore[currentEdge.u][currentEdge.table][currentEdge.v].find(prevObj) != hashStore[currentEdge.u][currentEdge.table][currentEdge.v].end())
                              {
                                   bool check = false;
                                   for (const auto &entry : hashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj])
                                   {
                                        bool flag = reduceLoopdfs(entry, iter + 1, g, currentEdge, nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, initialEntry, dp, decodeStringToData, storeStringtoData);
                                        if (flag)
                                        {
                                             updatedHashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].push_back(entry);
                                             // dp[initialEntry.getSubject()][parentEdge.u][parentEdge.table][parentEdge.v][prevObj] = true;
                                             check = true;
                                        }
                                   }
                                   // dp[initialEntry.getSubject()][parentEdge.u][parentEdge.table][parentEdge.v][prevObj] = false;
                                   return check;
                              }

                              // dp[initialEntry.getSubject()][parentEdge.u][parentEdge.table][parentEdge.v][prevObj] = false;
                              return false;
                         }
                    }
                    else
                    {
                         parentEdge.u = u;
                         parentEdge.table = table.first;
                         parentEdge.v = v.first;

                         if (g.minVertexAdj.find(parentEdge.u) == g.minVertexAdj.end()) // if Parent.u is not found in minvertex edge
                         {
                              swap(parentEdge.u, parentEdge.v);
                         }
                         int countKept = 0;
                         int countDeleted = 0;
                         for (auto entry = sobitTables[parentEdge.u][parentEdge.table][parentEdge.v].begin(); entry != sobitTables[parentEdge.u][parentEdge.table][parentEdge.v].end();)
                         {
                              bool flag = reduceLoopdfs(*entry, iter + 1, g, parentEdge, nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, *entry, dp, decodeStringToData, storeStringtoData);

                              if (!flag) // flag == false
                              {
                                   countDeleted++;
                                   entry = sobitTables[parentEdge.u][parentEdge.table][parentEdge.v].erase(entry);
                              }
                              else
                              {
                                   countKept++;
                                   // cout << "Kuch rakhaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa: " << countKept << endl;
                                   // cout << decodeStringToData.at((*entry).getSubject()) << " " << decodeStringToData.at((*entry).getObject()) << endl;
                                   entry++;
                              }
                         }
                         cout << "Kitne bache bhai: " << countKept << endl;
                    }
               }
          }
     }
}

void reduceLoopTables(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                      Graph &g,
                      long long int nodeO,
                      long long int nodeN,
                      unordered_map<long long int, string> &decodeQueryTables,
                      unordered_map<string, long long int> &dataTables,
                      unordered_map<long long int,
                                    unordered_map<long long int,
                                                  unordered_map<long long int,
                                                                unordered_map<long long int, list<Sobit>>>>>
                          &hashStore,
                      unordered_map<long long int,
                                    unordered_map<long long int,
                                                  unordered_map<long long int,
                                                                unordered_map<long long int, list<Sobit>>>>>
                          &updatedHashStore,
                      unordered_map<long long int, string> decodeStringToData,
                      unordered_map<string, long long int> storeStringtoData)
{
     vector<long long int> loopPath = g.loopTablePaths[nodeN];
     vector<long long int>::iterator iter = loopPath.begin();
     unordered_map<long long int,
                   unordered_map<long long int,
                                 unordered_map<long long int,
                                               unordered_map<long long int,
                                                             unordered_map<long long int, bool>>>>>
         dp;

     reduceLoopdfs(Sobit(), iter, g, Edge(), nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, Sobit(), dp, decodeStringToData, storeStringtoData);

     for (auto u : updatedHashStore)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    reduceCurrentTable(sobitTables[u.first][table.first][v.first],
                                       0,
                                       updatedHashStore[u.first][table.first][v.first]);
               }
          }
     }
}

int main()
{
     ios_base::sync_with_stdio(false);
     cin.tie(NULL);
     cout.tie(NULL);

     // Tables Name and pair entries of the table will be stored. Ex - tabels["follows"] as 1 entry.
     unordered_map<string, list<pair<string, string>>> data;
     unordered_map<string, list<pair<string, string>>> query;
     // Tables Name and pair entries of the table. Ex - tabels["follows"] as 1 entry.

     FileReader fr;
     cout << "Reading data: " << endl;
     fr.processFile(DATA, 1, data);
     fr.processFile(QUERY, 1, query);
     cout << "Read data: " << endl
          << endl;

     // Total tables and their names
     unordered_map<string, long long int> dataTables;
     unordered_map<long long int, string> decodeDataTables;
     long long int index = 0;
     // Tables are stored seperately now.
     for (auto &table : data)
     {
          // cout << "Table: " << table.first << " index: " << index << endl;
          dataTables[table.first] = index;
          decodeDataTables[index] = table.first;
          index++;
     }

     unordered_map<string, long long int> queryTables;
     unordered_map<long long int, string> decodeQueryTables;

     index = 0;
     for (auto &table : query)
     {
          queryTables[table.first] = index;
          decodeQueryTables[index] = table.first;
          index++;
     }
     cout << "Tables of Query-->" << endl;
     cout << endl;
     for (auto x : queryTables)
     {
          cout << x.first << " " << x.second << endl;
     }
     cout << endl;

     unordered_map<string, long long int> storeStringtoData;
     unordered_map<long long int, string> decodeStringToData;
     unordered_map<string, long long int> storeStringtoQuery;
     unordered_map<long long int, string> decodeStringToQuery;
     unordered_map<long long int, list<pair<long long int, long long int>>> tablesAfterPreprocessing;
     unordered_map<long long int, list<pair<long long int, long long int>>> queryAfterPreprocessing;
     cout << "Total Tables Size of Data: " << data.size() << endl;
     preProcess(data, storeStringtoData, decodeStringToData, dataTables, tablesAfterPreprocessing);

     preProcess(query, storeStringtoQuery, decodeStringToQuery, queryTables, queryAfterPreprocessing);
     long long int totalVertices = storeStringtoQuery.size();
     cout << endl
          << "Total Vertices in query: " << totalVertices << endl
          << endl;

     cout << endl;
     cout << "Query Mapping Encoding: " << endl;
     for (auto x : storeStringtoQuery)
     {
          cout << x.first << " " << x.second << endl;
     }
     Timer timer;
     Timer totalTimer;
     timer.start();
     Graph g = preProcessQuery(decodeQueryTables, queryAfterPreprocessing, totalVertices, storeStringtoQuery.size());
     cout << "-- preProcessQuery Completed In " << timer.elapsed() << " seconds-- " << endl;

     unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> storeSobit;
     unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> sobitTables;
     createSobit(tablesAfterPreprocessing, storeSobit, sobitTables, dataTables, queryTables, decodeQueryTables, g, true);

     cout << endl;
     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }
     cout << "---------- Join Condition Starts -------------" << endl;
     timer.start();
     initializeUsingSobit(sobitTables, g, dataTables, decodeQueryTables);
     cout << endl
          << "-- initializeUsingSobit Completed In " << timer.elapsed() << " seconds-- " << endl
          << endl;
     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }
     unordered_map<long long int,
                   unordered_map<long long int,
                                 unordered_map<long long int,
                                               unordered_map<long long int, list<Sobit>>>>>
         hashStore;
     timer.start();
     semiJoinOpConVertices(sobitTables, g, decodeStringToData, hashStore);
     cout << endl
          << "-- semiJoinOpConVertices Completed In " << timer.elapsed() << " seconds-- " << endl
          << endl;

     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }
     timer.start();
     semiJoinOpNonConVertices(sobitTables, g, decodeStringToData);
     cout << endl
          << "-- semiJoinOpNonConVertices Completed In " << timer.elapsed() << " seconds-- " << endl
          << endl;

     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }

     unordered_map<long long int,
                   unordered_map<long long int,
                                 unordered_map<long long int,
                                               unordered_map<long long int, list<Sobit>>>>>
         hashStore3;
     unordered_map<long long int,
                   unordered_map<long long int,
                                 unordered_map<long long int,
                                               unordered_map<long long int, list<Sobit>>>>>
         updatedHashStore;
     for (auto u : g.loopEdges)
     {
          cout << u.first << " --> ";
          for (auto table : u.second)
          {
               cout << table.first << "[ ";
               for (auto v : table.second)
               {
                    cout << v.first << " " << v.second << endl;
                    long long int nodeN = v.first;
                    long long int nodeO = v.second;

                    reduceLoopTables(sobitTables, g, nodeO, nodeN, decodeQueryTables, dataTables, hashStore3, updatedHashStore, decodeStringToData, storeStringtoData);
               }
               cout << "]" << endl;
          }
          cout << endl;
     }
     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }

     cout << "Yhaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << endl;
     for (auto u : g.loopEdges)
     {
          cout << u.first << " --> ";
          for (auto table : u.second)
          {
               cout << table.first << "[ ";
               for (auto v : table.second)
               {
                    cout << v.first << " " << v.second << endl;
                    detectTablesofVertex(u.first, table.first, v.first, g, v.second, sobitTables, decodeStringToData);
               }
               cout << "]" << endl;
          }
          cout << endl;
     }

     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }

     // unordered_map<long long int, list<Sobit>> hashTable2;
     // makeHash(sobitTables[1][0][3], 0, hashTable2);

     // cout << "Hash table Size: " << hashTable2.size() << endl;
     // reduceBckrwd(sobitTables[1][0][0], 1, hashTable2);
     // cout << "YhaaaaaaaaaaYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYZZZZZZZZZZZZZZZZZZZZ" << endl;
     // for (auto u : g.minVertexAdj)
     // {
     //      for (auto table : u.second)
     //      {
     //           for (auto v : table.second)
     //           {
     //                cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
     //           }
     //      }
     // }
     // unordered_map<long long int, list<Sobit>> hashTable3;
     // makeHash(sobitTables[1][0][0], 0, hashTable3);
     // reduceBckrwd(sobitTables[2][0][0], 1, hashTable3);

     // for (auto u : g.minVertexAdj)
     // {
     //      for (auto table : u.second)
     //      {
     //           for (auto v : table.second)
     //           {
     //                cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
     //           }
     //      }
     // }

     // list<Sobit> l1, l2, l3;
     // l1 = sobitTables[1][0][3];
     // l2 = sobitTables[1][0][0];
     // l3 = sobitTables[2][0][0];

     // unordered_map<long long int, list<Sobit>> h1_s;
     // unordered_map<long long int, list<Sobit>> h1_o;
     // unordered_map<long long int, list<Sobit>> h2_s;
     // unordered_map<long long int, list<Sobit>> h2_o;
     // unordered_map<long long int, list<Sobit>> h3_s;
     // unordered_map<long long int, list<Sobit>> h3_o;

     // makeHash(l1, 0, h1_s);
     // makeHash(l1, 1, h1_o);

     // makeHash(l2, 0, h2_s);
     // makeHash(l2, 1, h2_o);

     // makeHash(l3, 0, h3_s);
     // makeHash(l3, 1, h3_o);

     // int count = 0;
     // for (auto entry : l1)
     // {
     //      bool flag = 0;
     //      long long int sub = entry.getSubject();
     //      long long int obj = entry.getObject();
     //      if (h2_s.find(obj) != h2_s.end())
     //      {
     //           for (auto h2Item : h2_s[obj])
     //           {
     //                if (h3_s.find(h2Item.getObject()) != h3_s.end())
     //                {
     //                     for (auto h3Item : h3_s[h2Item.getObject()])
     //                     {
     //                          if (h3Item.getObject() == sub)
     //                          {
     //                               flag = 1;
     //                          }
     //                     }
     //                }
     //           }
     //      }
     //      if (flag)
     //           count++;
     // }

     // cout << "Correct Items count: " << count << endl;
     // int count = 0;

     // for (auto u5 : hashTable2)
     // {
     //      for (auto x : u5.second)
     //      {
     //           count++;
     //      }
     // }

     // cout << "Idhar: " << count << endl;
     // for (auto u : g.minVertexAdj)
     // {
     //      for (auto table : u.second)
     //      {
     //           for (auto v : table.second)
     //           {
     //                cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
     //           }
     //      }
     // }
     timer.start();
     // unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> storeSobit1;
     // unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> sobitTables1;
     // createSobit(tablesAfterPreprocessing, storeSobit1, sobitTables1, dataTables, queryTables, decodeQueryTables, g, true);

     // initializeUsingSobit(sobitTables, g, dataTables, decodeQueryTables);
     cout << endl
          << "-- initializeUsingSobit Completed In " << timer.elapsed() << " seconds-- " << endl
          << endl;
     unordered_map<long long int,
                   unordered_map<long long int,
                                 unordered_map<long long int,
                                               unordered_map<long long int, list<Sobit>>>>>
         hashStore1;
     semiJoinOpConVertices(sobitTables, g, decodeStringToData, hashStore1);
     cout << endl
          << "-- semiJoinOpConVertices Completed In " << timer.elapsed() << " seconds-- " << endl
          << endl;

     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }
     timer.start();
     semiJoinOpNonConVertices(sobitTables, g, decodeStringToData);
     cout << endl
          << "-- semiJoinOpNonConVertices Completed In " << timer.elapsed() << " seconds-- " << endl
          << endl;

     for (auto u : g.minVertexAdj)
     {
          for (auto table : u.second)
          {
               for (auto v : table.second)
               {
                    writeSobitTabletoFile("follows_" + to_string(u.first) + "_" + to_string(v.first) + ".txt", sobitTables[u.first][table.first][v.first], decodeStringToData);
                    cout << "U, Table, V --> " << u.first << " " << table.first << " " << v.first << " " << sobitTables[u.first][table.first][v.first].size() << endl;
               }
          }
     }

     cout
         << endl
         << "-- Completed In " << totalTimer.elapsed() << " seconds-- " << endl;
}
