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

     // cout << "Here1" << endl;
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
     // std::cout << "*******************************************************" << endl;
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

     cout << endl
          << "-- Completed In " << totalTimer.elapsed() << " seconds-- " << endl;
}
