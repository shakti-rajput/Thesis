#include "bitset.h"
#include "config.h"
#include <bitset>
#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

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
    for (auto x : table)
    {
      Sobit s;
      s.setData(x.first, x.second, storeSobit[x.first].first,
                storeSobit[x.first].second, storeSobit[x.second].first,
                storeSobit[x.second].second);
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

void createSobit(
    unordered_map<long long int, list<pair<long long int, long long int>>> tables,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit,
    unordered_map<string, long long int> tablesName,
    unordered_map<string, long long int> queryTables,
    bool buildSobitDataFlag)
{
  vector<list<Sobit>> sobitTables;
  if (buildSobitDataFlag)
  {
    for (auto x : tables)
    {
      buildSobitData(x.second, x.first, storeSobit);
    }
  }

  // for (auto x : tables)
  // {
  //   list<Sobit> sobitTable;
  //   maptoSobits(x, sobitTable, storeSobit, buildSobitDataFlag);
  //   sobitTables.push_back(sobitTable);
  // }
}