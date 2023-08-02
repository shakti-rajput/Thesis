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
  // cout << index << endl;
  long long int setBit = pow(2, index);
  long long int count = 0;
  for (auto x : table)
  {
    // cout << "index: " << index << " x.first: " << x.first << " x.second: " << x.second << endl;
    // cout << storeSobit[x.first].first << endl;
    storeSobit[x.first].first |= setBit;
    storeSobit[x.second].second |= setBit;
    // count++;
    // if (count == 5)
    //   break;
  }
}

// void createSobitTable(long long int tableIndex,
//                       unordered_map<long long int, string> decodeQueryTables,
//                       unordered_map<string, long long int> tablesName,
//                       unordered_map<long long int, list<pair<long long int, long long int>>> tables,
//                       unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit,
//                       bool buildSobitDataFlag)
// {

//   long long int dataTableIndex;
//   dataTableIndex = tablesName[decodeQueryTables[tableIndex]];
//   list<Sobit> sobitTable;
//   maptoSobits(tables[dataTableIndex], sobitTable, storeSobit, buildSobitDataFlag);
//   // sobitTables[table.second] = sobitTable;
// }

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

  for (auto u : g.adj)
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