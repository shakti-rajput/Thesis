#include "config.h"
#include "timer.h"
#include "helper.h"

#include "performHistogramJoin.h"
#include <unordered_set>

using namespace std;

void makeHash(list<Sobit> &tables,
              int index,
              unordered_map<long long int, list<Sobit>> &hashTable)
{
    Timer timer;
    timer.start();

    if (index)
    {
        for (auto x : tables)
        {
            hashTable[x.getObject()].push_back(x);
        }
    } // if index == 1 then custom key is second element
    else
    {
        for (auto x : tables)
        {
            hashTable[x.getSubject()].push_back(x);
        }
    }
    cout << "Hash Table making time --> " << timer.elapsed() << endl;
}

void reduceFrwd(list<Sobit> &currTable,
                bool currTableCommonVertex,
                list<Sobit> &rightTable,
                bool rightTableCommonVertex,
                unordered_map<long long int, list<Sobit>> &finalNextTableHash)
{
    unordered_map<long long int, list<Sobit>> hashTable;
    // cout << "rightTableCommonVertex: " << rightTableCommonVertex << endl;
    makeHash(rightTable, rightTableCommonVertex, hashTable);
    // cout << "Hash Table: " << endl;
    // for (auto x : hashTable)
    // {
    //     cout << x.first << " ";
    // }
    // cout << endl;
    int tempKey;

    if (currTableCommonVertex)
    {
        for (auto x : currTable)
        {
            tempKey = x.getObject();
            // cout << "tempKey: " << tempKey << endl;
            if (hashTable.find(tempKey) != hashTable.end())
            {
                finalNextTableHash[tempKey] = hashTable[tempKey];
                hashTable.erase(tempKey);
            }
        }
    }
    else
    {
        for (auto x : currTable)
        {
            tempKey = x.getSubject();
            if (hashTable.find(tempKey) != hashTable.end())
            {
                finalNextTableHash[tempKey] = hashTable[tempKey];
                hashTable.erase(tempKey);
            }
        }
    }
}

void reduceCurrentTable(list<Sobit> &nextTable,
                        bool nextTableCommonVertex,
                        unordered_map<long long int, list<Sobit>> &finalRightTableHash)
{
    if (nextTableCommonVertex)
    {
        // for (auto x : finalRightTableHash)
        // {
        //     cout << "Yhaaaaaaaaaa: " << x.first << endl;
        // }
        for (auto x = nextTable.begin(); x != nextTable.end(); x++)
        {
            // cout << "subject: " << x->getSubject() << " object: " << x->getObject() << endl;
            if (finalRightTableHash.find(x->getObject()) == finalRightTableHash.end())
            {
                x = nextTable.erase(x);
                x--;
            }
        }
    }
    else
    {
        for (auto x = nextTable.begin(); x != nextTable.end(); x++)
        {
            if (finalRightTableHash.find(x->getSubject()) == finalRightTableHash.end())
            {
                x = nextTable.erase(x);
                x--;
            }
        }
    }
}

void reduceBckrwd(list<Sobit> &currTable,
                  bool currTableCommonVertex,
                  bool nextTableCommonVertex,
                  unordered_map<long long int, list<Sobit>> &finalRightTableHash)
{
    if (currTableCommonVertex)
    {
        for (auto x = currTable.begin(); x != currTable.end(); x++)
        {
            if (finalRightTableHash.find(x->getObject()) == finalRightTableHash.end())
            {
                x = currTable.erase(x);
                x--;
            }
        }
    }
    else
    {
        for (auto x = currTable.begin(); x != currTable.end(); x++)
        {
            if (finalRightTableHash.find(x->getSubject()) == finalRightTableHash.end())
            {
                x = currTable.erase(x);
                x--;
            }
        }
    }
}

void reduceFrwdHash(unordered_map<long long int, list<Sobit>> &currTableHash,
                    bool currTableCommonVertex,
                    list<Sobit> &nextTable,
                    bool nextTableCommonVertex,
                    unordered_map<long long int, list<Sobit>> &finalNextTableHash)
{

    unordered_map<long long int, list<Sobit>> hashTable;
    makeHash(nextTable, nextTableCommonVertex, hashTable);

    int tempKey;
    int flag = 0;
    if (currTableCommonVertex)
    {
        for (unordered_map<long long int, list<Sobit>>::iterator it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (list<Sobit>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                tempKey = it2->getObject();
                if (hashTable.find(tempKey) != hashTable.end())
                {
                    finalNextTableHash[tempKey] = hashTable[tempKey];
                    hashTable.erase(tempKey);
                }
                else if (finalNextTableHash.find(tempKey) == finalNextTableHash.end())
                {
                    it2 = it->second.erase(it2);
                    it2--;
                }
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
                flag = 1;
            }
            if (flag != 1)
                it++;
            flag = 0;
        }
    }
    else
    {
        for (unordered_map<long long int, list<Sobit>>::iterator it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (list<Sobit>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                tempKey = it2->getSubject();
                if (hashTable.find(tempKey) != hashTable.end())
                {
                    finalNextTableHash[tempKey] = hashTable[tempKey];
                    hashTable.erase(tempKey);
                }
                else if (finalNextTableHash.find(tempKey) == finalNextTableHash.end())
                {
                    it2 = it->second.erase(it2);
                    it2--;
                }
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
                flag = 1;
            }
            if (flag != 1)
                it++;
            flag = 0;
        }
    }
}

void reduceBckrwdHash(list<Sobit> &currTable,
                      unordered_map<long long int, list<Sobit>> &currTableHash,
                      bool currTableCommonVertex,
                      list<Sobit> &nextTable,
                      unordered_map<long long int, list<Sobit>> &nextTableHash,
                      bool nextTableCommonVertex)
{
    int flag = 0;

    if (currTableCommonVertex)
    {
        for (unordered_map<long long int, list<Sobit>>::iterator it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (list<Sobit>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {

                if (nextTableHash.find(it2->getObject()) == nextTableHash.end())
                {
                    it2 = it->second.erase(it2);
                    it2--;
                }

                // if (finalNextTableHash.find(it2->getObject()) != finalNextTableHash.end())
                //      rightTable.push_back(*it2);
                // else
                // {
                //      it2 = it->second.erase(it2);
                //      it2--;
                // }
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
                flag = 1;
            }
            if (flag != 1)
                it++;
            flag = 0;
        }

        for (auto x = nextTable.begin(); x != nextTable.end(); x++)
        {
            if (nextTableHash.find(x->getObject()) == nextTableHash.end())
            {
                x = nextTable.erase(x);
                x--;
            }
        }
    }
    else
    {
        for (unordered_map<long long int, list<Sobit>>::iterator it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (list<Sobit>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                if (nextTableHash.find(it2->getSubject()) == nextTableHash.end())
                {
                    it2 = it->second.erase(it2);
                    it2--;
                }

                // if (finalNextTableHash.find(it2->getSubject()) != finalNextTableHash.end())
                //     rightTable.push_back(*it2);
                // else
                // {
                //     it2 = it->second.erase(it2);
                //     it2--;
                // }
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
                flag = 1;
            }
            if (flag != 1)
                it++;
            flag = 0;
        }

        for (auto x = nextTable.begin(); x != nextTable.end(); x++)
        {
            if (nextTableHash.find(x->getSubject()) == nextTableHash.end())
            {
                x = currTable.erase(x);
                x--;
            }
        }
    }
}

void dfsHash(long long int node,
             const Graph &g,
             unordered_set<long long int> &visited,
             unordered_set<long long int> &pathVisited,
             unordered_map<long long int, list<Sobit>> &sobitTables,
             unordered_map<long long int, unordered_map<long long int, list<Sobit>>> &hashStore,
             list<Sobit> &currTable,
             unordered_map<long long int, list<Sobit>> &currTableHash,
             bool currTableCommonVertex)
{
    visited.insert(node);
    pathVisited.insert(node);
    // g.minVertexAdj;

    // 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 -- 8 -- 9
    //      C         C         C         C

    //      6 -- 4 -- 0 -- 1 -- 7 -- 5 -- 2

    for (auto table : g.VCTree.at(node))
    {
        for (auto v : table.second)
        {
            if (g.VCTree.find(v.second.first) != g.VCTree.end() && pathVisited.find(v.second.first) == pathVisited.end())
            {

                // cout << "Tandavvvvvvvvvvvvvv" << endl;
                currTable;
                currTableHash;         // 1st table
                currTableCommonVertex; //

                long long int u1 = node;
                long long int v1 = v.first;
                long long int tableIndex1 = table.first;
                g.minVertexAdj.at(u1).at(tableIndex1).at(v1); // check direction of curr table
                sobitTables[tableIndex1];                     // 2nd Table
                bool firstTableCommonVertex = g.minVertexAdj.at(u1).at(tableIndex1).at(v1);

                // unordered_map<long long int, list<Sobit>> firstTableHash;

                reduceFrwdHash(currTableHash, // hash Table of table 1 is created
                               currTableCommonVertex,
                               sobitTables[tableIndex1],
                               firstTableCommonVertex,
                               hashStore[tableIndex1]);

                long long int u2 = v.first;
                long long int v2 = v.second.first;
                long long int tableIndex2 = v.second.second;

                sobitTables[tableIndex2];                     // 3rd table
                g.minVertexAdj.at(v2).at(tableIndex2).at(u2); // check direction of second table
                bool secondTableCommonVertex = g.minVertexAdj.at(v2).at(tableIndex2).at(u2);

                // unordered_map<long long int, list<Sobit>> secondTableHash;

                reduceFrwdHash(hashStore[tableIndex1], // hash Table of table 2 is created
                               firstTableCommonVertex,
                               sobitTables[tableIndex2],
                               secondTableCommonVertex,
                               hashStore[tableIndex2]);

                // if (g.VCTree.find(v.second.second) != g.VCTree.end() && pathVisited.find(v.second.second) == pathVisited.end())
                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[tableIndex2], hashStore[tableIndex2], secondTableCommonVertex);

                reduceBckrwdHash(sobitTables[tableIndex1], // Table of Table 2 is reduced and hash of table 1 is reduced
                                 hashStore[tableIndex1],
                                 firstTableCommonVertex,
                                 sobitTables[tableIndex2],
                                 hashStore[tableIndex2],
                                 secondTableCommonVertex);

                reduceBckrwdHash(currTable, // Table of Table 1 is reduced and hash of curr Table is reduced
                                 currTableHash,
                                 currTableCommonVertex,
                                 sobitTables[tableIndex1],
                                 hashStore[tableIndex1], // firstTableHash,
                                 firstTableCommonVertex);
            }
        }
    }
    pathVisited.erase(node);
}

void dfs(long long int node,
         const Graph &g,
         unordered_set<long long int> &visited,
         unordered_set<long long int> &pathVisited,
         unordered_map<long long int, list<Sobit>> &sobitTables,
         unordered_map<long long int, string> decodeStringToData,
         unordered_map<long long int, unordered_map<long long int, list<Sobit>>> &hashStore)
{
    visited.insert(node);
    pathVisited.insert(node);
    // g.minVertexAdj;
    // cout << "Tandav2 Node:  " << node << endl;
    for (auto table : g.VCTree.at(node))
    {
        // cout << "Tandav3" << endl;
        for (auto v : table.second)
        {
            // cout << "Tandav4" << endl;
            // cout << "Node: " << node << endl;
            // cout << "Table: " << table.first << endl;
            // cout << "v.first: " << v.first << endl;
            // cout << "v.second.first: " << v.second.first << endl;
            // cout << "v.second.second: " << v.second.second << endl;

            if (g.VCTree.find(v.second.first) != g.VCTree.end() && pathVisited.find(v.second.first) == pathVisited.end())
            {
                int tableIndex1 = table.first;
                int u1 = node;
                int v1 = v.first;
                // cout << "Inghe...1" << endl;
                g.minVertexAdj.at(u1).at(tableIndex1).at(v1);
                // cout << "Inghe...2" << endl; // check direction of first table
                sobitTables[tableIndex1]; // first table
                bool currTableCommonVertex = g.minVertexAdj.at(u1).at(tableIndex1).at(v1);

                int tableIndex2 = v.second.second;
                int u2 = v.first; // common is u2
                int v2 = v.second.first;
                sobitTables[v.second.second];
                // cout << "Inghe...3" << endl;                  // second table
                g.minVertexAdj.at(v2).at(tableIndex2).at(u2); // check direction of second table
                // cout << "Inghe...4" << endl;
                bool nextTableCommonVertex = g.minVertexAdj.at(v2).at(tableIndex2).at(u2);

                // 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 -- 8 -- 9
                //      C         C         C         C
                // 4 -- 3 -- 2 -- 0 -- 1
                //      6 -- 4 -- 0 -- 1 -- 7 -- 5 -- 2
                hashStore[tableIndex2];
                // unordered_map<long long int, list<Sobit>> nextTableHash;

                reduceFrwd(sobitTables[tableIndex1],
                           currTableCommonVertex,
                           sobitTables[tableIndex2],
                           nextTableCommonVertex,
                           hashStore[tableIndex2]);

                // createAndWriteToFile("AfterreduceFrwd.txt", getAllEntriesString(sobitTables, decodeStringToData));

                // if (g.VCTree.find(v.second.second) != g.VCTree.end() && pathVisited.find(v.second.second) == pathVisited.end())
                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[tableIndex2], hashStore[tableIndex2], nextTableCommonVertex);

                reduceCurrentTable(sobitTables[tableIndex2],
                                   nextTableCommonVertex,
                                   hashStore[tableIndex2]);
                // createAndWriteToFile("AfterreduceBckrwd1.txt", getAllEntriesString(sobitTables, decodeStringToData));
                reduceBckrwd(sobitTables[tableIndex1], // current table are getting reduced form hash of next table
                             currTableCommonVertex,
                             nextTableCommonVertex,
                             hashStore[tableIndex2]);

                // createAndWriteToFile("AfterreduceBckrwd2.txt", getAllEntriesString(sobitTables, decodeStringToData));
            }
        }
    }
    pathVisited.erase(node);
}

void semiJoinOpConVertices(unordered_map<long long int, list<Sobit>> &sobitTables,
                           const Graph &g, unordered_map<long long int, string> decodeStringToData,
                           unordered_map<long long int, unordered_map<long long int, list<Sobit>>> &hashStore)
{
    g.VCTree;
    unordered_set<long long> visited;
    unordered_set<long long> pathVisited;
    // cout << "Tandav" << endl;
    for (auto node : g.nodesTopoOrder)
    {
        if (g.VCTree.find(node) != g.VCTree.end() && visited.find(node) == visited.end())
        {
            dfs(node, g, visited, pathVisited, sobitTables, decodeStringToData, hashStore);
        }
    }
}

bool checkConvertex(const Graph &g, long long int u, long long int table, long long int v)
{
    // cout << "Yha aye kya?????" << endl;
    // cout << u << " " << table << " " << v << endl;
    if (g.VCTree.find(u) != g.VCTree.end() &&
        g.VCTree.at(u).find(table) != g.VCTree.at(u).end() &&
        g.VCTree.at(u).at(table).find(v) != g.VCTree.at(u).at(table).end())
        return true;
    // auto it1 = g.VCTree.find(u);
    // if (it1 != g.VCTree.end())
    // {
    //     cout << "Kha tak aye?" << endl;
    //     auto it2 = it1->second.find(table);
    //     if (it2 != it1->second.end())
    //     {
    //         cout << "Kha tak aye???" << endl;
    //         auto it3 = it2->second.find(v);
    //         if (it3 != it2->second.end())
    //         {
    //             return true;
    //         }
    //     }
    // }

    return false;
}
void createVertexCoverConVertices(const Graph &g,
                                  unordered_map<long long int, vector<pair<long long int, bool>>> &vertexCoverConVertices)
{
    for (auto u : g.minVertexAdj)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                if (checkConvertex(g, u.first, table.first, v.first))
                {
                    // cout << "?????????" << endl;
                    vertexCoverConVertices[u.first].push_back(make_pair(table.first, !v.second));
                }
            }
        }
    }
}

void semiJoinOpNonConVertices(unordered_map<long long int, list<Sobit>> &sobitTables,
                              const Graph &g, unordered_map<long long int, string> decodeStringToData)
{
    unordered_map<long long int, unordered_map<bool, unordered_map<long long int, list<Sobit>>>> temphashStore;
    unordered_map<long long int, vector<pair<long long int, bool>>> vertexCoverConVertices;
    // printminVertexAdj(g.minVertexAdj);
    // printVCTree(g.VCTree);
    createVertexCoverConVertices(g, vertexCoverConVertices);
    for (auto x : vertexCoverConVertices)
    {
        // cout << "Node: " << x.first << endl;

        for (auto y : x.second)
        {
            // cout << "Entry4: " << y.first << endl;
            // cout << "Entry5: " << y.second << endl;
            makeHash(sobitTables[y.first], y.second, temphashStore[y.first][y.second]);
            // cout << y.first << " " << y.second << endl;
        }
        cout << endl;
    }
    // cout << "End Game:" << endl;
    for (auto u : g.minVertexAdj)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                // cout << "Entry1: " << endl;
                // cout << u.first << " " << table.first << " " << v.first << endl;
                if (!checkConvertex(g, u.first, table.first, v.first))
                {
                    for (auto conTable : vertexCoverConVertices[u.first])
                    {
                        // cout << "Entry2: ";
                        // cout << table.first << endl;

                        sobitTables[table.first]; // table of non-convertices
                        bool currTableCommonVertex = !v.second;
                        // cout << "Entry3: " << currTableCommonVertex << endl;

                        sobitTables[conTable.first]; // table of con vertices
                                                     // hashStore[conTable.first];   // hash table

                        reduceBckrwd(sobitTables[table.first], // current table are getting reduced form hash of next table
                                     currTableCommonVertex,
                                     conTable.second,
                                     temphashStore[conTable.first][conTable.second]);
                    }
                }
            }
        }
    }
}

// 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 -- 8 -- 9
//      C         C         C         C
// 4 -- 3 -- 2 -- 0 -- 1
//      6 -- 4 -- 0 -- 1 -- 7 -- 5 -- 2