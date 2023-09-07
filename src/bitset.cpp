#include <bitset>
#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "bitset.h"
#include "graph.h"

using namespace std;

Sobit::Sobit(){};

Sobit::Sobit(long long int subject, long long int object, bitset<MAX_SIZE> ss,
             bitset<MAX_SIZE> so, bitset<MAX_SIZE> os, bitset<MAX_SIZE> oo)
{

  this->subject = subject;
  this->object = object;
  this->ss = ss;
  this->so = so;
  this->os = os;
  this->oo = oo;
}

void Sobit::setData(long long int subject, long long int object,
                    bitset<MAX_SIZE> ss, bitset<MAX_SIZE> so,
                    bitset<MAX_SIZE> os, bitset<MAX_SIZE> oo)
{

  this->subject = subject;
  this->object = object;
  this->ss = ss;
  this->so = so;
  this->os = os;
  this->oo = oo;
}

vector<bitset<MAX_SIZE>> Sobit::getNeighbourd() const
{

  vector<bitset<MAX_SIZE>> res;
  res.push_back(ss);
  res.push_back(so);
  res.push_back(os);
  res.push_back(oo);
  return res;
}

pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> Sobit::getSubjectSobit() const
{
  pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> res;
  res.first = ss;
  res.second = so;
  return res;
}

void Sobit::setSubjectSobit(pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo)
{
  ss = sobitInfo.first;
  so = sobitInfo.second;
}

pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> Sobit::getObjectSobit() const
{
  pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> res;
  res.first = os;
  res.second = oo;
  return res;
}

void Sobit::setObjectSobit(pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo)
{
  ss = sobitInfo.first;
  so = sobitInfo.second;
}

long long int Sobit::getSubject() const { return subject; }

long long int Sobit::getObject() const { return object; }

void maptoSobits(
    list<pair<long long int, long long int>> table, list<Sobit> &sobitTable,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>>
        &storeSobit,
    bool buildSobitDataFlag)
{
  if (buildSobitDataFlag)
  {
    for (auto entry : table)
    {
      Sobit s;
      s.setData(entry.first, entry.second, storeSobit[entry.first].first,
                storeSobit[entry.first].second, storeSobit[entry.second].first,
                storeSobit[entry.second].second);
      sobitTable.push_back(s);
    }
  }
  else
  {
    for (auto x : table)
    {
      Sobit s;
      s.setData(x.first, x.second);
      sobitTable.push_back(s);
    }
  }
}

void buildSobitData(
    list<pair<long long int, long long int>> table, long long int index,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit)
{
  long long int setBit = pow(2, index);
  long long int count = 0;
  for (auto x : table)
  {
    storeSobit[x.first].first |= setBit;
    storeSobit[x.second].second |= setBit;
  }
}

void createSobit(
    unordered_map<long long int, list<pair<long long int, long long int>>> tables,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit,
    unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
    unordered_map<string, long long int> tablesName,
    unordered_map<string, long long int> queryTables,
    unordered_map<long long int, string> decodeQueryTables,
    Graph &g,
    bool buildSobitDataFlag)
{

  if (buildSobitDataFlag)
  {
    for (auto x : tables)
    {
      buildSobitData(x.second, x.first, storeSobit);
    }
  }

  unordered_map<long long int, list<Sobit>> tempSobitTables;
  for (auto table : queryTables)
  {
    long long int dataTableIndex;
    dataTableIndex = tablesName[table.first];
    list<Sobit> sobitTable;
    maptoSobits(tables[dataTableIndex], sobitTable, storeSobit, buildSobitDataFlag);
    tempSobitTables[table.second] = sobitTable;
  }

  for (auto u : g.minVertexAdj)
  {
    for (auto table : u.second)
    {
      for (auto v : table.second)
      {
        sobitTables[u.first][table.first][v.first] = tempSobitTables[table.first];
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
                           const unordered_map<long long int, string> &decodeStringToQuery,
                           int &count)
{
  bool del = false;

  for (const auto &tableC : conditions.at(u))
  {
    const long long int tableCIndex = findQueryCorrespondingIndexToData(storeStringtoData, decodeStringToQuery, tableC.first);
    for (const auto &entryC : tableC.second)
    {
      if (entryC.first != v || tableC.first != table)
      {
        if (count < 1)
        {
          cout << "tableCIndex: " << tableCIndex << endl;
          // cout << "Sobit Subject: " << sobitInfo.first << endl;
          // cout << "Sobit Object: " << sobitInfo.second << endl;
        }
        const bool entryCondition = entryC.second;
        const bool sobitCondition = entryCondition ? !sobitInfo.first.test(tableCIndex) : !sobitInfo.second.test(tableCIndex);

        if (sobitCondition)
        {
          return true; // Early termination if condition is satisfied
        }
      }
    }
  }
  return false;
}

void findJoiningConditions(const Graph &g, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> &conditions)
{
  for (const auto &node : g.minVertexAdj)
  {
    for (const auto &table : node.second)
    {
      for (const auto &entry : table.second)
      {
        conditions[node.first][table.first].emplace(entry.first, entry.second);
      }
    }
  }
}

void initializeUsingSobit(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                          const Graph &g,
                          const unordered_map<string, long long int> &storeStringtoData,
                          const unordered_map<long long int, string> &decodeStringToQuery)
{
  cout << "*********" << endl;
  for (const auto &[nodeId, nodeTables] : g.minVertexAdj)
  {
    for (const auto &[tableId, tableEntries] : nodeTables)
    {
      for (const auto &[entry, isCommonSubject] : tableEntries)
      {
        auto &sobitTable = sobitTables[nodeId][tableId][entry];
        pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo;
        cout << "Here : " << nodeId << " " << tableId << " " << entry << " " << isCommonSubject << endl;
        if (isCommonSubject)
        {
          int count = 0;
          for (auto it = sobitTable.begin(); it != sobitTable.end();)
          {
            sobitInfo = (*it).getSubjectSobit();
            bool del = checkCondition(g.minVertexAdj, nodeId, tableId, entry, sobitInfo, storeStringtoData, decodeStringToQuery, count);
            count++;
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
          int count = 0;
          for (auto it = sobitTable.begin(); it != sobitTable.end();)
          {
            sobitInfo = (*it).getObjectSobit();
            bool del = checkCondition(g.minVertexAdj, nodeId, tableId, entry, sobitInfo, storeStringtoData, decodeStringToQuery, count);
            count++;
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