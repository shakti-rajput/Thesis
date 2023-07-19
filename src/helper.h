#pragma once
#include "graph.h"
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

vector<string> string_burst(const string &str, string delimeter);

void format10M(string line,
               unordered_map<string, list<pair<string, string>>> &tabels);

long long int joinAttributePosition(list<list<long long int>> leftTable,
                                    long long int attribute);

void preprocessingStringtoInteger(
    unordered_map<long long int, list<pair<string, string>>> &tablesBeforePreprocessing,
    unordered_map<string, long long int> &store,
    unordered_map<long long int, string> &decodeStringToData,
    unordered_map<long long int, list<pair<long long int, long long int>>> &tablesAfterPreprocessing);

void createAndWriteToFile(const string &filename, const string &content);

void writeTabletoFile(const string filename, const list<pair<long long int, long long int>> &table);

void writeTablesToFile(const unordered_map<long long int, string> &decodeQueryTables, const unordered_map<long long int, list<pair<long long int, long long int>>> &tables);

void writeGraphtoFile(const string filename, Graph g);

void writePathToFile(const string filename, const string &path);

void writeNodesOrder(const string filename, const string &nodesOrder);

string getAllEntriesString(const unordered_map<long long int, list<Sobit>> &sobitTables,
                           unordered_map<long long int, string> &decodeStringToData);