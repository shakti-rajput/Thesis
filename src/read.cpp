
#include <fstream>
#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "read.h"
#include "helper.h"

using namespace std;

string FileReader::readFile(const string fileName)
{
    unordered_map<string, list<pair<string, string>>> tabelsChecking = {};

    ifstream file(fileName);
    if (!file.is_open())
    {
        cout << "Error opening file: " << fileName << std::endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

void FileReader::processFile(const string fileName, int option,
                             unordered_map<string, list<pair<string, string>>>& tabelsChecking)
{

    string fileContent = readFile(fileName);
    if (!fileContent.empty())
    {

        vector<string> lines = string_burst(fileContent, "\n");

        // Process the line as needed
        for (const auto &line : lines)
        {
            if (option == 1)
            {

                format10M(line, tabelsChecking);
                // Specific actions for option 1
            }
            else if (option == 2)
            {
                // Specific actions for option 2
            }
        }
    }
}
