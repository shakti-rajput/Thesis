#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class FileReader
{
public:
    string readFile(const string fileName);
    void processFile(const string fileName, int option,
                     unordered_map<string, list<pair<string, string>>> &tabelsChecking);
};