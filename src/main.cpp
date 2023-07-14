// ConsoleApplication1.cpp : This file contains the 'main' function. Program
// execution begins and ends there.
//

#include "bitset.h"
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
          // cout << "tableName " << tableName << endl;
          tablesBeforePreprocessing[tableName.second] = data.at(tableName.first);
     }
     // for (auto tableNameIter = tableNames.rbegin(); tableNameIter != tableNames.rend(); ++tableNameIter)
     // {
     //      const string &tableName = *tableNameIter;
     //      tablesBeforePreprocessing.push_back(data.at(tableName));
     // }
     // for (int i = 0; i < tablesBeforePreprocessing.size(); i++)
     // {
     //      for (auto &entry : tablesBeforePreprocessing[i])
     //      {
     //           cout << "entry.first " << entry.first << " entry.second " << entry.second << endl;
     //      }
     // }
     Timer timer;
     timer.start();
     preprocessingStringtoInteger(tablesBeforePreprocessing, storeStringtoInt, decodeStringToData, tablesAfterPreprocessing);
     cout << "Preprocessing Time --> " << timer.elapsed() << endl;
}

Graph preProcessQuery(const unordered_map<long long int, string> &decodeQueryTables,
                      const unordered_map<long long int, list<pair<long long int, long long int>>> &queryAfterPreprocessing,
                      long long int totalVertices,
                      long long int newItemCounter,
                      unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> storeSobit)
{

     Timer timer;
     cout << "________ Writing all tables content fetch from the files after Reading ________" << endl;
     timer.start();
     writeTablesToFile(decodeQueryTables, queryAfterPreprocessing);
     cout << "Writing all tables content fetch from the files after Reading Completed in --> " << timer.elapsed() << endl
          << endl;

     vector<list<Sobit>> sobitTables;

     Graph g;

     /*Build the Graph and write to File*/
     cout << "******* Start building the Graph ********" << endl;

     timer.start();
     g = buildGraph(totalVertices, queryAfterPreprocessing, storeSobit, sobitTables);
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

     /*Convert to Line Graph and write to File*/
     // cout << "****** Converting to Line Graph *****" << endl;
     // timer.start();
     // g.convertToLineGraph();
     // cout << "Converting to Line Graph Completed in --> " << timer.elapsed() << endl;

     // cout << "______Writing to file After COnverting To LineGraph_______" << endl;
     // timer.start();
     // createAndWriteToFile("AfterConvertingtoLineGraph.txt", g.printlineGraphAdj());
     // cout << "Writing to file After COnverting To LineGraph Completed in --> " << timer.elapsed() << endl
     //      << endl;
     ////////////////////////////////////////////////////////////////SELECT *

     /*Find Minimum Vertex Cover and write to File*/
     cout << "******* Finding Minimum Vertex Cover *****" << endl;
     timer.start();
     g.get_minVertex_cover();

     g.generatingVCTree();
     cout << "Finding Minimum Vertex Cover Completed in --> " << timer.elapsed() << endl
          << endl;

     cout << "Completed Successfully" << endl;
     return g;
}

// voidget_minVertex_coverTables,
//                  const vector<list<pair<long long int, long long int>>> &queryAfterPreprocessing,
//                  long long int totalVertices,
//                  long long int newItemCounter,
//                  unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> storeSobit)
// {
//      Timer timer;
//      cout << "________ Writing all tables content fetch from the files after Reading ________" << endl;
//      timer.start();
//      writeTablesToFile(queryTables, queryAfterPreprocessing);
//      cout << "Writing all tables content fetch from the files after Reading Completed in --> " << timer.elapsed() << endl
//           << endl;

//      vector<list<Sobit>> sobitTables;

//      Graph g;
//      /*Build the Graph and write to File*/
//      cout << "******* Start building the Graph ********" << queryTables.size() << endl;

//      timer.start();
//      g = buildGraph(totalVertices, queryAfterPreprocessing, storeSobit, sobitTables);
//      cout << "--------- Graph Built in ---------> " << timer.elapsed() << endl
//           << endl;

//      cout << "________ Writing to file Before Transforming To Acyclic ________" << endl;
//      timer.start();
//      writeGraphtoFile("BeforeTransformingToAcyclic.txt", g);
//      cout << "Writing to file Before Transforming To Acyclic Completed in --> " << timer.elapsed() << endl
//           << endl
//           << endl;

//      /*Transform to Acyclic and write to File*/
//      cout << "***** Transforming To Acyclic*****" << endl;
//      timer.start();
//      g.transformToAcyclic(newItemCounter);
//      cout << "-------- Transforming to Acyclic Completed in -------> " << timer.elapsed() << endl;

//      cout << "______Writing to file After Transforming To Acyclic_______" << endl;
//      timer.start();
//      writeGraphtoFile("AfterTransformingToAcyclic.txt", g);
//      std::cout << "Writing to file After Transforming To Acyclic Completed in --> " << timer.elapsed() << endl
//                << endl;

//      vector<int> tablesIndex;
//      tablesIndex.push_back(0);
//      tablesIndex.push_back(1);
//      tablesIndex.push_back(2);
//      tablesIndex.push_back(3);
//      vector<pair<int,int>> joins; // Object --> Subject
//      for (auto sobit : storeSobit)
//      {
//           bool flag = true;
//           for(auto join: joins)
//           {
//                long long int objectIndex = pow(2,join.first);
//                long long int subjectIndex = pow(2,join.second);
//                if(!(sobit.second.second.test(objectIndex) && sobit.second.first.test(subjectIndex)))
//                {
//                     flag = false;
//                }
//           }
//           if(flag)
//           {

//           }
//      }
// }

void findJoiningConditions(Graph &g,
                           unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &conditions)
{
     g.minVertexAdj;

     for (auto node : g.minVertexAdj)
     {
          for (auto table : node.second)
          {
               for (auto entry : table.second)
               {
                    conditions[node.first][table.first][entry.first] = !entry.second;
               }
          }
     }
}
long long int findQUeryCorrespondingIndexTOData(unordered_map<string, long long int> storeStringtoData,
                                                unordered_map<long long int, string> decodeStringToQuery,
                                                long long int index)
{
     return storeStringtoData[decodeStringToQuery[index]];
}

string getAllEntriesString(const unordered_map<long long int, list<Sobit>> &sobitTables)
{
     stringstream ss;

     for (const auto &entry : sobitTables)
     {
          ss << "Table Name: " << entry.first << endl
             << "Values: " << endl;

          const list<Sobit> &values = entry.second;
          for (const Sobit &value : values)
          {
               // Append each value to the string stream
               ss << value.getSubject() << " " << value.getObject() << endl;
          }

          ss << endl;
     }

     // Return the generated string
     return ss.str();
}

bool checkCondition(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &conditions,
                    long long int u,
                    long long int table,
                    long long int v,
                    pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo,
                    unordered_map<string, long long int> storeStringtoData,
                    unordered_map<long long int, string> decodeStringToQuery)
{
     long long int tableIndex = findQUeryCorrespondingIndexTOData(storeStringtoData,
                                                                  decodeStringToQuery,
                                                                  table);
     // tableIndex = storeStringtoData[decodeStringToQuery[table.first]];
     // std::cout << "Decoded Table Index: " << tableIndex << endl;
     bool del = false;
     for (auto tableC : conditions[u])
     {
          tableIndex = findQUeryCorrespondingIndexTOData(storeStringtoData, decodeStringToQuery, tableC.first);

          for (auto entryC : tableC.second)
          {
               if (v != entryC.first && table != tableC.first)
               {

                    if (entryC.second == 0)
                    {
                         if (!sobitInfo.first.test(tableIndex))
                         {
                              del = true;
                         }
                    }
                    else
                    {
                         if (!sobitInfo.second.test(tableIndex))
                         {
                              del = true;
                         }
                    }
               }
          }
     }
     return del;
}

void initializeUsingSobit(unordered_map<long long int, list<Sobit>> &sobitTables,
                          Graph &g,
                          unordered_map<string, long long int> storeStringtoData,
                          unordered_map<long long int, string> decodeStringToQuery)
{
     // std::cout << "*******************************************************" << endl;
     unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> conditions;
     findJoiningConditions(g, conditions);
     for (auto node : g.minVertexAdj)
     {
          for (auto table : node.second)
          {
               for (auto entry : table.second)
               {
                    // std::cout << "Table Name: " << table.first << endl;
                    pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo;

                    for (auto it = sobitTables[table.first].begin(); it != sobitTables[table.first].end();)
                    {
                         // std::cout << "Info: " << (*it).getSubject() << " " << (*it).getObject() << endl;
                         // std::cout << "Sobit Info: "
                         //           << (*it).getNeighbourd()[0] << " "
                         //           << (*it).getNeighbourd()[1] << " "
                         //           << (*it).getNeighbourd()[2] << " "
                         //           << (*it).getNeighbourd()[3] << " "
                         //           << endl;

                         if (entry.second) // 1
                         {
                              // std::cout << "Common Subject" << endl;
                              sobitInfo = (*it).getSubjectSobit();
                         }
                         else // 0
                         {
                              // std::cout << "Common Object" << endl;
                              sobitInfo = (*it).getObjectSobit();
                         }

                         bool del = false;
                         // cout << "Sobit Passed: " << sobitInfo.first << " " << sobitInfo.second << endl;
                         // Check Conditions
                         del = checkCondition(conditions, node.first, table.first,
                                              entry.first, sobitInfo,
                                              storeStringtoData, decodeStringToQuery);

                         if (del)
                         {
                              // cout << "----------Getting Deleted---------" << endl;
                              auto next = std::next(it);
                              sobitTables[table.first].erase(it);
                              it = next;
                         }
                         else
                         {
                              it++;
                         }
                    }
                    // cout << endl
                    //      << endl;
               }
          }
     }
}

void semiJoinOpConVertices(unordered_map<long long int, list<Sobit>> &sobitTables,
                           Graph &g,
                           unordered_map<string, long long int> storeStringtoData,
                           unordered_map<long long int, string> decodeStringToQuery)
{
     
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
     cout << "Tables of Data: " << endl;
     for (auto &table : data)
     {
          cout << "Table: " << table.first << " index: " << index << endl;
          dataTables[table.first] = index;
          decodeDataTables[index] = table.first;
          index++;
     }
     cout << endl
          << endl;

     for (auto x : dataTables)
     {
          cout << x.first << " " << x.second << endl;
     }

     unordered_map<string, long long int> queryTables;
     unordered_map<long long int, string> decodeQueryTables;

     cout << endl;
     // vector<string> queryTables; // = {"follows", "friendOf", "likes", "hasReview"};

     cout << "Tables of Query: " << endl;
     index = 0;
     for (auto &table : query)
     {
          queryTables[table.first] = index;
          cout << "Table: " << table.first << endl;
          decodeQueryTables[index] = table.first;
          index++;
     }
     cout << endl;
     cout << "Tables of Query-->" << endl;
     // for (auto x : queryTables)
     // {
     //      cout << x << " ";
     // }
     cout << endl;
     for (auto x : queryTables)
     {
          cout << x.first << " " << x.second << endl;
     }
     cout << endl;
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

     unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> storeSobit;
     unordered_map<long long int, list<Sobit>> sobitTables;
     createSobit(tablesAfterPreprocessing, storeSobit, sobitTables, dataTables, queryTables, true);

     cout << endl
          << "Data Mapping: " << endl;
     for (auto x : storeStringtoData)
     {
          cout << x.first << " " << x.second << endl;
     }
     cout << "Data Mapping Decoding: " << endl;
     for (auto x : decodeStringToData)
     {
          cout << x.first << " " << x.second << endl;
     }

     cout << endl;
     cout << "Query Mapping Encoding: " << endl;
     for (auto x : storeStringtoQuery)
     {
          cout << x.first << " " << x.second << endl;
     }
     cout << "Query Mapping Decoding: " << endl;
     for (auto x : decodeStringToQuery)
     {
          cout << x.first << " " << x.second << endl;
     }
     cout << endl
          << endl;

     cout << "Sobit data: " << endl;
     for (auto &sobit : storeSobit)
     {
          cout << "Key: " << sobit.first << " Value: "
               << "Subject: " << sobit.second.first << " Object: " << sobit.second.second << endl;
     }
     cout << endl;
     cout << "Printting Sobit Tables: " << endl;
     for (auto sobitTable : sobitTables)
     {
          cout << decodeQueryTables[sobitTable.first] << endl;
          for (auto entries : sobitTable.second)
          {
               cout << decodeStringToData[entries.getSubject()] << " " << decodeStringToData[entries.getObject()] << " ---> ";
               for (auto sobitInfo : entries.getNeighbourd())
               {
                    cout << sobitInfo << " ";
               }
               cout << endl;
          }
     }

     Graph g = preProcessQuery(decodeQueryTables, queryAfterPreprocessing, totalVertices, storeStringtoQuery.size(), storeSobit);
     createAndWriteToFile("DataTablesBeforeProcessing.txt", getAllEntriesString(sobitTables));
     initializeUsingSobit(sobitTables, g, dataTables, decodeQueryTables);
     createAndWriteToFile("DataTablesAfterProcessing.txt", getAllEntriesString(sobitTables));

     semiJoinOpConVertices(sobitTables, g, dataTables, decodeQueryTables);

     cout << "-- Completed -- " << endl;
}
