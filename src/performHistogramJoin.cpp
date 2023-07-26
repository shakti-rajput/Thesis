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
         unordered_map<long long int, string> &decodeStringToData,
         unordered_map<long long int, unordered_map<long long int, list<Sobit>>> &hashStore)
{
    visited.insert(node);
    pathVisited.insert(node);

    for (auto table : g.VCTree.at(node))
    {
        for (auto v : table.second)
        {
            if (g.VCTree.find(v.second.first) != g.VCTree.end() && pathVisited.find(v.second.first) == pathVisited.end())
            {
                int tableIndex1 = table.first;
                int u1 = node;
                int v1 = v.first;
                // sobitTables[tableIndex1];
                bool currTableCommonVertex = g.minVertexAdj.at(u1).at(tableIndex1).at(v1);

                int tableIndex2 = v.second.second;
                int u2 = v.first;
                int v2 = v.second.first;
                // sobitTables[v.second.second];
                bool nextTableCommonVertex = g.minVertexAdj.at(v2).at(tableIndex2).at(u2);

                // hashStore[tableIndex2];

                reduceFrwd(sobitTables[tableIndex1],
                           currTableCommonVertex,
                           sobitTables[tableIndex2],
                           nextTableCommonVertex,
                           hashStore[tableIndex2]);

                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[tableIndex2], hashStore[tableIndex2], nextTableCommonVertex);

                reduceCurrentTable(sobitTables[tableIndex2],
                                   nextTableCommonVertex,
                                   hashStore[tableIndex2]);

                reduceBckrwd(sobitTables[tableIndex1],
                             currTableCommonVertex,
                             nextTableCommonVertex,
                             hashStore[tableIndex2]);
            }
        }
    }
    pathVisited.erase(node);
}

void semiJoinOpConVertices(unordered_map<long long int, list<Sobit>> &sobitTables,
                           const Graph &g,
                           unordered_map<long long int, string> &decodeStringToData,
                           unordered_map<long long int, unordered_map<long long int, list<Sobit>>> &hashStore)
{
    unordered_set<long long> visited;
    unordered_set<long long> pathVisited;

    for (auto node : g.nodesTopoOrder)
    {
        if (g.VCTree.find(node) != g.VCTree.end() && visited.find(node) == visited.end())
        {
            dfs(node, g, visited, pathVisited, sobitTables, decodeStringToData, hashStore);
        }
    }
}

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

bool checkConvertex(const Graph& g, long long int u, long long int table, long long int v)
{
    if (g.VCTree.find(u) != g.VCTree.end() &&
        g.VCTree.at(u).find(table) != g.VCTree.at(u).end() &&
        g.VCTree.at(u).at(table).find(v) != g.VCTree.at(u).at(table).end())
    {
        return true;
    }

    return false;
}

void createVertexCoverConVertices(const Graph& g,
                                  unordered_set<pair<long long int, long long int>, PairHash>& vertexCoverSet,
                                  unordered_map<long long int, vector<pair<long long int, bool>>>& vertexCoverConVertices)
{
    for (const auto& u : g.minVertexAdj)
    {
        for (const auto& table : u.second)
        {
            for (const auto& v : table.second)
            {
                if (checkConvertex(g, u.first, table.first, v.first))
                {
                    vertexCoverSet.insert(make_pair(u.first, table.first));
                    vertexCoverConVertices[u.first].push_back(make_pair(table.first, !v.second));
                }
            }
        }
    }
}

void semiJoinOpNonConVertices(unordered_map<long long int, list<Sobit>>& sobitTables,
                              const Graph& g, unordered_map<long long int, string> decodeStringToData)
{
    unordered_map<long long int, unordered_map<bool, unordered_map<long long int, list<Sobit>>>> temphashStore;
    unordered_map<long long int, vector<pair<long long int, bool>>> vertexCoverConVertices;
    unordered_set<pair<long long int, long long int>, PairHash> vertexCoverSet;
    
    createVertexCoverConVertices(g, vertexCoverSet, vertexCoverConVertices);

    for (const auto& x : vertexCoverConVertices)
    {
        for (const auto& y : x.second)
        {
            makeHash(sobitTables[y.first], y.second, temphashStore[y.first][y.second]);
        }
    }

    for (const auto& u : g.minVertexAdj)
    {
        for (const auto& table : u.second)
        {
            for (const auto& v : table.second)
            {
                const auto& vertexCoverKey = make_pair(u.first, table.first);
                if (vertexCoverSet.find(vertexCoverKey) == vertexCoverSet.end())
                {
                    for (const auto& conTable : vertexCoverConVertices[u.first])
                    {
                        const auto& currTableCommonVertex = !v.second;

                        reduceBckrwd(sobitTables[table.first],
                                     currTableCommonVertex,
                                     conTable.second,
                                     temphashStore[conTable.first][conTable.second]);
                    }
                }
            }
        }
    }
}
