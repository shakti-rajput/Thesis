
#pragma once

#include "config.h"
#include <bitset>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

class Sobit
{
private:
    long long int subject;
    long long int object;

    bitset<MAX_SIZE> ss;
    bitset<MAX_SIZE> so;
    bitset<MAX_SIZE> os;
    bitset<MAX_SIZE> oo;

public:
    Sobit();

    Sobit(long long int subject, long long int object, bitset<MAX_SIZE> ss = 0,
          bitset<MAX_SIZE> so = 0, bitset<MAX_SIZE> os = 0,
          bitset<MAX_SIZE> oo = 0);

    void setData(long long int subject, long long int object,
                 bitset<MAX_SIZE> ss = 0, bitset<MAX_SIZE> so = 0,
                 bitset<MAX_SIZE> os = 0, bitset<MAX_SIZE> oo = 0);

    long long int getSubject() const;
    long long int getObject() const;
    vector<bitset<MAX_SIZE>> getNeighbourd() const;
    pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> getSubjectSobit() const;
    void setSubjectSobit(pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo);
    pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> getObjectSobit() const;
    void setObjectSobit(pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>> sobitInfo);
};

void buildSobitData(
    list<pair<long long int, long long int>> table, long long int index,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>>
        &storeSobit);

void maptoSobits(
    list<pair<long long int, long long int>> table, list<Sobit> &sobitTable,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>>
        &storeSobit,
    bool buildSobitDataFlag = false);

void createSobit(
    unordered_map<long long int, list<pair<long long int, long long int>>> tables,
    unordered_map<long long int, pair<bitset<MAX_SIZE>, bitset<MAX_SIZE>>> &storeSobit,
    unordered_map<long long int, list<Sobit>> &sobitTables,
    unordered_map<string, long long int> tablesName,
    unordered_map<string, long long int> queryTables,
    bool buildSobitDataFlag = false);