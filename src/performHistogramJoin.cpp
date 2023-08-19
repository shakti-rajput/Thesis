#include "config.h"
#include "timer.h"
#include "helper.h"

#include "performHistogramJoin.h"
#include <unordered_set>

using namespace std;

void makeHash(list<Sobit> &tables,
              bool index,
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
    cout << tables.size() << endl;
    cout << "Hash Table size: " << hashTable.size() << endl;
    cout << "Hash Table making time --> " << timer.elapsed() << endl;
}

void reduceFrwd(list<Sobit> &currTable,
                bool currTableCommonVertex,
                list<Sobit> &rightTable,
                bool rightTableCommonVertex,
                unordered_map<long long int, list<Sobit>> &finalNextTableHash)
{
    unordered_map<long long int, list<Sobit>> hashTable;
    makeHash(rightTable, rightTableCommonVertex, hashTable);
    int tempKey;

    if (currTableCommonVertex)
    {
        for (auto x : currTable)
        {
            tempKey = x.getObject();
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
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (finalRightTableHash.find(x->getObject()) == finalRightTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
                x++;
        }
    }
    else
    {
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (finalRightTableHash.find(x->getSubject()) == finalRightTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
                x++;
        }
    }
}

void reduceBckrwd(list<Sobit> &currTable,
                  bool currTableCommonVertex,
                  unordered_map<long long int, list<Sobit>> &finalRightTableHash)
{
    if (currTableCommonVertex)
    {
        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (finalRightTableHash.find(x->getObject()) == finalRightTableHash.end())
            {
                x = currTable.erase(x);
            }
            else
                x++;
        }
    }
    else
    {
        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (finalRightTableHash.find(x->getSubject()) == finalRightTableHash.end())
            {
                x = currTable.erase(x);
            }
            else
                x++;
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
    if (currTableCommonVertex)
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                tempKey = it2->getObject();
                if (hashTable.find(tempKey) != hashTable.end())
                {
                    // count++;
                    finalNextTableHash[tempKey] = hashTable[tempKey];
                    hashTable.erase(tempKey);
                }
                if (finalNextTableHash.find(tempKey) == finalNextTableHash.end())
                {
                    // count++;
                    it2 = it->second.erase(it2);
                }
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }
    }
    else
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                tempKey = it2->getObject();
                if (hashTable.find(tempKey) != hashTable.end())
                {
                    finalNextTableHash[tempKey] = hashTable[tempKey];
                    hashTable.erase(tempKey);
                }
                if (finalNextTableHash.find(tempKey) == finalNextTableHash.end())
                {
                    it2 = it->second.erase(it2);
                }
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
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
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                int tempKey = it2->getObject();

                if (nextTableHash.find(tempKey) == nextTableHash.end())
                    it2 = it->second.erase(it2);
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }

        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (currTableHash.find(x->getSubject()) != currTableHash.end() &&
                nextTableHash.find(x->getObject()) != nextTableHash.end())
            {
                x++;
            }
            else
                x = currTable.erase(x);
        }
    }
    else
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                int tempKey = it2->getSubject();

                if (nextTableHash.find(tempKey) == nextTableHash.end())
                    it2 = it->second.erase(it2);
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }
        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (currTableHash.find(x->getObject()) != currTableHash.end() &&
                nextTableHash.find(x->getSubject()) != nextTableHash.end())
            {
                x++;
            }
            else
                x = currTable.erase(x);
        }
    }
    if (nextTableCommonVertex)
    {
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (nextTableHash.find(x->getObject()) == nextTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
                x++;
        }
    }
    else
    {
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (nextTableHash.find(x->getSubject()) == nextTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
                x++;
        }
    }
}

void dfsHash(long long int node,
             const Graph &g,
             unordered_set<long long int> &visited,
             unordered_set<long long int> &pathVisited,
             unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
             unordered_map<long long int,
                           unordered_map<long long int,
                                         unordered_map<long long int,
                                                       unordered_map<long long int, list<Sobit>>>>> &hashStore,
             list<Sobit> &currTable,
             unordered_map<long long int, list<Sobit>> &currTableHash,
             bool currTableCommonVertex)
{
    visited.insert(node);
    pathVisited.insert(node);
    cout << "Nodee: " << node << endl;
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

                currTable;
                currTableHash;         // 1st table
                currTableCommonVertex; //

                long long int u1 = node;
                long long int v1 = v.first;
                long long int tableIndex1 = table.first;
                g.minVertexAdj.at(u1).at(tableIndex1).at(v1); // check direction of curr table
                sobitTables[tableIndex1];                     // 2nd Table
                bool firstTableCommonVertex = g.minVertexAdj.at(u1).at(tableIndex1).at(v1);

                int count;
                count = 0;

                reduceFrwdHash(currTableHash, // hash Table of table 1 is created
                               currTableCommonVertex,
                               sobitTables[u1][tableIndex1][v1],
                               !firstTableCommonVertex,
                               hashStore[u1][tableIndex1][v1]);

                long long int u2 = v.first;
                long long int v2 = v.second.first;
                long long int tableIndex2 = v.second.second;

                sobitTables[tableIndex2];                     // 3rd table
                g.minVertexAdj.at(v2).at(tableIndex2).at(u2); // check direction of second table
                bool secondTableCommonVertex = g.minVertexAdj.at(v2).at(tableIndex2).at(u2);

                reduceFrwdHash(hashStore[u1][tableIndex1][v1], // hash Table of table 2 is created
                               firstTableCommonVertex,
                               sobitTables[v2][tableIndex2][u2],
                               secondTableCommonVertex,
                               hashStore[v2][tableIndex2][u2]);

                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[v2][tableIndex2][u2], hashStore[v2][tableIndex2][u2], secondTableCommonVertex);

                reduceBckrwdHash(sobitTables[u1][tableIndex1][v1], // Table of Table 2 is reduced and hash of table 1 is reduced
                                 hashStore[u1][tableIndex1][v1],
                                 firstTableCommonVertex,
                                 sobitTables[v2][tableIndex2][u2],
                                 hashStore[v2][tableIndex2][u2],
                                 secondTableCommonVertex);

                reduceBckrwdHash(currTable, // Table of Table 1 is reduced and hash of curr Table is reduced
                                 currTableHash,
                                 currTableCommonVertex,
                                 sobitTables[u1][tableIndex1][v1],
                                 hashStore[u1][tableIndex1][v1], // firstTableHash,
                                 !firstTableCommonVertex);
            }
        }
    }
    pathVisited.erase(node);
}

void dfs(long long int node,
         const Graph &g,
         unordered_set<long long int> &visited,
         unordered_set<long long int> &pathVisited,
         unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
         unordered_map<long long int, string> &decodeStringToData,
         unordered_map<long long int,
                       unordered_map<long long int,
                                     unordered_map<long long int,
                                                   unordered_map<long long int, list<Sobit>>>>> &hashStore)
{
    visited.insert(node);
    pathVisited.insert(node);
    cout << "Nodee: " << node << endl;
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

                cout << "tableIndex1: " << tableIndex1 << " tableIndex2: " << tableIndex2 << endl;

                cout << u1 << " " << tableIndex1 << " " << v1 << " " << currTableCommonVertex << endl;
                cout << v2 << " " << tableIndex2 << " " << u2 << " " << nextTableCommonVertex << endl;
                reduceFrwd(sobitTables[u1][tableIndex1][v1],
                           currTableCommonVertex,
                           sobitTables[v2][tableIndex2][u2],
                           nextTableCommonVertex,
                           hashStore[v2][tableIndex2][u2]);

                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[v2][tableIndex2][u2], hashStore[v2][tableIndex2][u2], !nextTableCommonVertex);

                reduceCurrentTable(sobitTables[v2][tableIndex2][u2],
                                   nextTableCommonVertex,
                                   hashStore[v2][tableIndex2][u2]);

                reduceBckrwd(sobitTables[u1][tableIndex1][v1],
                             currTableCommonVertex,
                             hashStore[v2][tableIndex2][u2]);
            }
        }
    }
    pathVisited.erase(node);
}

void semiJoinOpConVertices(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                           const Graph &g,
                           unordered_map<long long int, string> &decodeStringToData,
                           unordered_map<long long int,
                                         unordered_map<long long int,
                                                       unordered_map<long long int,
                                                                     unordered_map<long long int, list<Sobit>>>>> &hashStore)
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

struct PairHash
{
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

bool checkConvertex(const Graph &g, long long int u, long long int table, long long int v)
{
    if (g.VCTree.find(u) != g.VCTree.end() &&
        g.VCTree.at(u).find(table) != g.VCTree.at(u).end() &&
        g.VCTree.at(u).at(table).find(v) != g.VCTree.at(u).at(table).end())
    {
        return true;
    }

    return false;
}

void createVertexCoverConVertices(const Graph &g,
                                  unordered_set<pair<long long int, long long int>, PairHash> &vertexCoverSet,
                                  unordered_map<long long int, vector<pair<long long int, bool>>> &vertexCoverConVertices)
{
    for (const auto &u : g.minVertexAdj)
    {
        for (const auto &table : u.second)
        {
            for (const auto &v : table.second)
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

void fetchTable(const unordered_map<long long int,
                                    unordered_map<long long int,
                                                  unordered_map<long long int,
                                                                unordered_map<bool,
                                                                              unordered_map<long long int, list<Sobit>>>>>> &
                    tempHashStore,
                long long int u,
                unordered_map<long long int, list<Sobit>> &hashTable)
{
    if (tempHashStore.find(u) != tempHashStore.end())
    {
        for (auto table : tempHashStore.at(u))
        {
            for (auto &v : table.second)
            {
                for (auto direction : v.second)
                {
                    hashTable = direction.second;
                    cout << "Yha Table Size: " << hashTable.size() << endl;
                    return;
                }
            }
        }
    }
}

void semiJoinOpNonConVertices(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                              const Graph &g, unordered_map<long long int, string> decodeStringToData)
{

    unordered_map<long long int,
                  unordered_map<long long int,
                                unordered_map<long long int,
                                              bool>>>
        nonConVertex;

    unordered_map<long long int,
                  unordered_map<long long int,
                                unordered_map<long long int,
                                              unordered_map<bool,
                                                            unordered_map<long long int, list<Sobit>>>>>>
        tempHashStore;

    unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> conTableEdges;

    for (auto u : g.minVertexAdj)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                if (!checkConvertex(g, u.first, table.first, v.first))
                {
                    nonConVertex[u.first][table.first][v.first] = v.second;
                    cout << "nonConVertex: " << u.first << " " << table.first << " " << v.first << " " << v.second << endl;
                }
            }
        }
    }
    for (auto u : g.VCTree)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                cout << u.first << " " << table.first << " " << v.first << endl;
            }
        }
    }

    if (!g.VCTree.empty())
    {
        for (auto u : g.VCTree)
        {
            if (nonConVertex.find(u.first) != nonConVertex.end())
            {
                for (auto table : u.second)
                {
                    for (auto v : table.second)
                    {
                        bool direction = !g.minVertexAdj.at(u.first).at(table.first).at(v.first);
                        list<Sobit> tables = sobitTables[u.first][table.first][v.first];
                        makeHash(tables, direction, tempHashStore[u.first][table.first][v.first][direction]);
                        cout << "Hash: " << u.first << " " << table.first << " " << v.first << " " << direction << endl;
                        break;
                    }
                }
            }
        }
        for (auto u : nonConVertex)
        {
            unordered_map<long long int, list<Sobit>> hashTable;
            fetchTable(tempHashStore, u.first, hashTable);
            cout << "hashTable Size: " << hashTable.size();
            for (auto table : u.second)
            {
                for (auto v : table.second)
                {
                    const auto &currTableCommonVertex = !v.second;
                    reduceBckrwd(sobitTables[u.first][table.first][v.first],
                                 currTableCommonVertex,
                                 hashTable);
                }
            }
        }
    }
}
