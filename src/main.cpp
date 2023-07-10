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
     preprocessingStringtoInteger(tablesBeforePreprocessing, storeStringtoInt, tablesAfterPreprocessing);
     cout << "Preprocessing Time --> " << timer.elapsed() << endl;
}

void preProcessQuery(const unordered_map<long long int, string> &decodeQueryTables,
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

     unordered_map<string, long long int> storeStringtoIntData;
     unordered_map<string, long long int> storeStringtoIntQuery;
     unordered_map<long long int, list<pair<long long int, long long int>>> tablesAfterPreprocessing;
     unordered_map<long long int, list<pair<long long int, long long int>>> queryAfterPreprocessing;
     cout << "Total Tables Size of Data: " << data.size() << endl;
     preProcess(data, storeStringtoIntData, dataTables, tablesAfterPreprocessing);

     preProcess(query, storeStringtoIntQuery, queryTables, queryAfterPreprocessing);
     long long int totalVertices = storeStringtoIntQuery.size();
     // // TODO: totalVertices is not guaranteed to be correct. If all not unique.
     cout << endl
          << "Total Vertices in query: " << totalVertices << endl
          << endl;

     unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> storeSobit;

     createSobit(tablesAfterPreprocessing, storeSobit, dataTables, queryTables, true);
     cout << "Query Mapping: " << endl;
     for (auto x : storeStringtoIntQuery)
     {
          cout << x.first << " " << x.second << endl;
     }

     cout << endl
          << "Data Mapping: " << endl;
     for (auto x : storeStringtoIntData)
     {
          cout << x.first << " " << x.second << endl;
     }
     cout << endl;
     cout << "Sobit data: " << endl;
     for (auto &sobit : storeSobit)
     {
          cout << "Key: " << sobit.first << " Value: "
               << "Subject: " << sobit.second.first << " Object: " << sobit.second.second << endl;
     }
     cout << endl;
     // for (auto item : storeStringtoInt)
     // {
     //      cout << "Item: " << item.first << " index: " << item.second << endl;
     // }
     //////////////////////////////////////////////

     // for (int i = 0; i < queryAfterPreprocessing.size(); i++)
     // {
     //      for (auto &entry : queryAfterPreprocessing[i])
     //      {
     //           cout << "entry.first " << entry.first << " entry.second " << entry.second << endl;
     //      }
     // }

     // cout << "unique elements " << storeStringtoInt.size() << endl
     //      << endl;

     preProcessQuery(decodeQueryTables, queryAfterPreprocessing, totalVertices, storeStringtoIntQuery.size(), storeSobit);
}
