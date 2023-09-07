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

     if (!g.loopEdges.empty())
     {

          timer.start();

          reduceLoopTables(g, sobitTables, decodeQueryTables, dataTables, decodeStringToData, storeStringtoData);

          cout << endl
               << "-- Loop part Completed In " << timer.elapsed() << " seconds-- " << endl
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
     }
     timer.start();
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

     cout
         << endl
         << "-- Completed In " << totalTimer.elapsed() << " seconds-- " << endl;
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
