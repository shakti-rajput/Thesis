#include "helper.h"
#include "graph.h"
#include "timer.h"
#include "config.h"
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

vector<string> string_burst(const string &str, string delimiter)
{
  vector<string> words;
  size_t n = str.length();
  size_t delimiterLength = delimiter.length();
  size_t i = 0;

  while (i < n)
  {
    size_t j = i;
    while (str.compare(i, delimiterLength, delimiter) != 0 && i < n)
      i++;
    string temp = str.substr(j, i - j);
    words.push_back(temp);
    i += delimiterLength;
  }
  return words;
}

void format10M(string line,
               unordered_map<string, list<pair<string, string>>> &tabels)
{
  vector<string> results = string_burst(line, "	");

  pair<string, string> entry;
  entry.first = results[0];
  entry.second = results[2];

  tabels[results[1]].push_back(entry);
}

long long int joinAttributePosition(list<list<string>> leftTable,
                                    string attribute)
{
  list<list<string>>::iterator ptr1 = leftTable.begin();
  list<string>::iterator innerPtr = (*ptr1).begin();
  if (*innerPtr == attribute)
    return 0;
  advance(innerPtr, 1);
  if (*innerPtr == attribute)
    return 1;
  else
    return -1;
}

void preprocessingStringtoInteger(
    vector<list<pair<string, string>>> &tablesBeforePreprocessing,
    unordered_map<string, long long int> &store,
    vector<list<pair<long long int, long long int>>> &tablesAfterPreprocessing)
{

  // Tables are stored seperately now.

  long long int count = store.size();
  for (auto &table : tablesBeforePreprocessing)
  {
    list<pair<long long int, long long int>> tempTable;
    for (auto x : table)
    {
      if (store.find(x.first) == store.end())
      {
        store[x.first] = count;
        count++;
      }
      if (store.find(x.second) == store.end())
      {
        store[x.second] = count;
        count++;
      }
      tempTable.push_back(pair(store[x.first], store[x.second]));
    }
    tablesAfterPreprocessing.push_back(tempTable);
  }
}

void createAndWriteToFile(const string &filename, const string &content)
{
  std::ofstream outputFile(outputPath + filename, ios::out);
  // filename = filename.find(".txt") == true ? filename : filename + ".txt";
  if (outputFile.is_open())
  {
    outputFile << content;
    outputFile.close();
    cout << "Content written successfully " + filename << endl;
  }
  else
  {
    cerr << "Failed to create or open the file." << endl;
  }
}

string convertTableToString(const list<pair<long long int, long long int>> &table)
{
  string result;
  for (auto x : table)
  {
    result += to_string(x.first) + " " + to_string(x.second) + "\n";
  }
  return result;
}

void writeTabletoFile(const string filename, const list<pair<long long int, long long int>> &table)
{
  string res;
  res = convertTableToString(table);
  createAndWriteToFile(filename, res);
}

void writeTablesToFile(vector<string> arr, vector<list<pair<long long int, long long int>>> tables)
{
  int pointer = 0;
  for (auto &table : tables)
  {
    writeTabletoFile(arr[pointer] + ".txt", table);
    pointer++;
  }
}

void writeGraphtoFile(const string filename, Graph g)
{
  string res;
  res = g.printGraph();
  createAndWriteToFile(filename, res);
}

void writePathToFile(const string filename, const string &path)
{
  createAndWriteToFile(filename, path);
}

void writeNodesOrder(const string filename, const string &nodesOrder)
{
  Timer timer;
  timer.start();
  createAndWriteToFile(filename, nodesOrder);
  cout << "Printing Time for  nodesOrder --> " << timer.elapsed() << endl;
}
